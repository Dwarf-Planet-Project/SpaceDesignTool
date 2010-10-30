/*
 This program is free software; you can redistribute it and/or modify it under
 the terms of the European Union Public Licence - EUPL v.1.1 as published by
 the European Commission.

 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE. See the European Union Public Licence - EUPL v.1.1
 for more details.

 You should have received a copy of the European Union Public Licence - EUPL v.1.1
 along with this program.

 Further information about the European Union Public Licence - EUPL v.1.1 can
 also be found on the world wide web at http://ec.europa.eu/idabc/eupl

*/

/*
 ------ Copyright (C) 2010 STA Steering Board (space.trajectory.analysis AT gmail.com) ----
*/

#include "date.h"
#include "SpiceEphemeris.h"
#include "SpiceUsr.h"
#include <QMap>

using namespace sta;


class CoverageWindow
{
public:
    CoverageWindow() : m_start(0.0), m_end(-1.0) {}
    CoverageWindow(double start, double end) : m_start(start), m_end(end) {};

    double start() const { return m_start; }
    double end() const { return m_end; }
    bool isEmpty() const { return m_end < m_start; }

private:
    double m_start;
    double m_end;
};


SpiceEphemeris::SpiceEphemeris()
{
}


SpiceEphemeris::~SpiceEphemeris()
{
}


const QList<StaBodyId>&
SpiceEphemeris::bodyList() const
{
    return m_bodies;
}


const StaBody* SpiceEphemeris::parentBody(const StaBody* body) const
{
    return STA_SOLAR_SYSTEM->sun();
}


const sta::CoordinateSystemType
SpiceEphemeris::coordinateSystem(const StaBody* /* body */) const
{
    return sta::COORDSYS_EME_J2000;
}


StateVector
SpiceEphemeris::stateVector(const StaBody* body,
                            double mjd,
                            const StaBody* center,
                            sta::CoordinateSystemType coordSys) const
{
    double state[6];
    double lightTime = 0.0;

    // The time argument for SPICE is seconds since J2000 TDB
    double et = sta::daysToSecs(mjd + (MJDBase - J2000));

    // Call SPICE to get the geometric state (i.e. not corrected for aberration or light
    // time) of the body relative to the specified center object. STA's ids for Solar
    // System bodies are identical to NAIF ids, so no translation is required.
    spkgeo_c(body->id(), et, "j2000", center->id(), state, &lightTime);

    return StateVector(Vector3d(state[0], state[1], state[2]), Vector3d(state[3], state[4], state[5]));
}


static QString DateString(double et)
{
    SpiceChar timeString[128];

    timout_c(et, "YYYY MON DD HR:MN:SC.### (TDB) ::TDB",
             sizeof(timeString), timeString);

    return QString(timeString);
}


static void FurnishKernel(const QString& dir, const QString& kernelName)
{
    QString pathName = dir + "/" + kernelName;
    furnsh_c(pathName.toAscii().data());
}


/** Scan the list of loaded kernels and build the list of objects
  * in the ephemeris.
  */
bool
SpiceEphemeris::buildBodyList()
{
    SpiceInt spkCount = 0;

    const SpiceInt FileSize = 256;
    const SpiceInt LineSize = 81;
    const SpiceInt MaxCoverage = 100000;
    const SpiceInt WindowSize = 2 * MaxCoverage;
    const SpiceInt TimeLength = 51;

    // Get the number of SPK files that are loaded
    ktotal_c("SPK", &spkCount);

    QMap<SpiceInt, CoverageWindow> objCoverage;

    // Iterate over the loaded kernel files
    for (SpiceInt spkIndex = 0; spkIndex < spkCount; ++spkIndex)
    {
        SpiceChar fileName[FileSize];
        SpiceChar fileType[LineSize];
        SpiceChar fileSource[FileSize];

        SpiceInt handle = 0;
        SpiceBoolean found = SPICEFALSE;

        // Get the kernel data
        kdata_c(spkIndex, "SPK", FileSize, LineSize, FileSize,
                fileName, fileType, fileSource, &handle, &found);

        const int MaxObjects = 1000;
        SPICEINT_CELL(bodyIds, MaxObjects);
        spkobj_c(fileName, &bodyIds);

        // Iterate over the bodies in the kernel file
        for (SpiceInt i = 0; i < card_c(&bodyIds); ++i)
        {
            // Get the coverage window for the current object
            SpiceInt objId  =  SPICE_CELL_ELEM_I(&bodyIds, i);
            SPICEDOUBLE_CELL(cover, WindowSize);

            scard_c(0, &cover);
            spkcov_c(fileName, objId, &cover);

            SpiceInt intervalCount = wncard_c(&cover);

            qDebug() << objId << ", " << intervalCount;

            if (intervalCount == 1)
            {
                // Get the endpoints of the jth interval.
                SpiceInt intervalIndex = 0;
                double intervalStart = 0.0;
                double intervalEnd = 0.0;
                wnfetd_c(&cover, intervalIndex, &intervalStart, &intervalEnd);

                objCoverage[objId] = CoverageWindow(intervalStart, intervalEnd);
            }
        }
    }

    foreach (SpiceInt objId, objCoverage.keys())
    {
        CoverageWindow w = objCoverage[objId];
        qDebug() << (int) objId << ": " << DateString(w.start()) << " -- " << DateString(w.end());
        m_bodies << StaBodyId(objId);
    }
}


SpiceEphemeris*
SpiceEphemeris::InitializeSpice(const QString& spiceKernelDirectory)
{
    qDebug() << "Initializing SPICE with kernel files in " << spiceKernelDirectory;

    // Configure the error handling behavior of SPICE so that it reports
    // errors
    char spiceErrorAction[] = "REPORT";
    erract_c("SET", 2, spiceErrorAction);

    // TODO: This shouldn't be hard coded

    // Leap seconds kernel
    FurnishKernel(spiceKernelDirectory, "naif0009.tls");

    // Planetary constants kernel (contains rotational elements for planets
    // and natural satellites.)
    FurnishKernel(spiceKernelDirectory, "pck00009.tpc");

    // Planetary ephemeris (1899 - 2053)
    FurnishKernel(spiceKernelDirectory, "de421.bsp");

    // Jupiter system ephemeris: covers Galilean satellites, Amalthea, and
    // Thebe from 1900 - 2100
    FurnishKernel(spiceKernelDirectory, "jup230l.bsp");

    if (failed_c() == SPICETRUE)
    {
        return NULL;
    }

    SpiceEphemeris* eph = new SpiceEphemeris();
    eph->buildBodyList();

    return eph;
}
