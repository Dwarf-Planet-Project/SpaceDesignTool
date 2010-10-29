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

using namespace sta;


SpiceEphemeris::SpiceEphemeris()
{
}


SpiceEphemeris::~SpiceEphemeris()
{
}


const QList<StaBodyId>&
SpiceEphemeris::bodyList() const
{

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

    return StateVector(Vector3d(state[0], state[1], state[2]), Vector3d(state[3], state[4], state[6]));
}


SpiceEphemeris*
SpiceEphemeris::load(QIODevice* device)
{
    return NULL;
}

/*
double getStartDate() const;
double getEndDate() const;
*/


static void FurnishKernel(const QString& dir, const QString& kernelName)
{
    QString pathName = dir + "/" + kernelName;
    furnsh_c(pathName.toAscii().data());
}


bool
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

    return failed_c() == SPICEFALSE;
}
