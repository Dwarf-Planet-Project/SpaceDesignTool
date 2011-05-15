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


/*
 ------------------ Author: Chris Laurel  -------------------------------------------------
 ------------------ E-mail: (claurel@gmail.com) ----------------------------

 Based on code by Chris Laurel from Celestia (http://www.shatters.net/celestia)
 */

#include <Eigen/Core>
//#include "Eigen/src/Core/Map.h"
#include <QtCore>
#include <QDataStream>
#include <QIODevice>
#include "date.h"
#include "jplephemeris.h"



using namespace Eigen;
using namespace std;

typedef Eigen::Matrix< double, 3, 3 > 	MyMatrix3d;
typedef Eigen::Matrix< double, 3, 1 > 	MyVector3d;

static const unsigned int DE200RecordSize    =  826;
static const unsigned int DE405RecordSize    = 1018;
static const unsigned int DE406RecordSize    =  728;

static const unsigned int NConstants         =  400;
static const unsigned int ConstantNameLength =  6;

static const unsigned int MaxChebyshevCoeffs = 32;

static const int LabelSize = 84;


sta::JPLEphemeris::JPLEphemeris()
{
}


sta::JPLEphemeris::~JPLEphemeris()
{
}


unsigned int
sta::JPLEphemeris::getDENumber() const
{
    return DENum;
}

double
sta::JPLEphemeris::getStartDate() const
{
    return startDate;
}

double
sta::JPLEphemeris::getEndDate() const
{
    return endDate;
}

sta::Ephemeris::TimeInterval
sta::JPLEphemeris::validTimeInterval(const StaBody* /* body */) const
{
    return TimeInterval(sta::JdToMjd(startDate), sta::JdToMjd(endDate));
}

// Internal method to map an STA body id to a JPL ephemeris item
sta::JPLEphemItem
sta::JPLEphemeris::mapStaId(StaBodyId id) const
{
    switch (id)
    {
    case STA_SOLAR_SYSTEM_BARYCENTER:
        return JPLEph_SSB;
    case STA_EARTH_BARYCENTER:
        return JPLEph_EarthMoonBary;
    case STA_MERCURY:
        return JPLEph_Mercury;
    case STA_VENUS:
        return JPLEph_Venus;
    case STA_EARTH:
        return JPLEph_Earth;
    case STA_MARS:
        return JPLEph_Mars;
    case STA_JUPITER:
        return JPLEph_Jupiter;
    case STA_SATURN:
        return JPLEph_Saturn;
    case STA_URANUS:
        return JPLEph_Uranus;
    case STA_NEPTUNE:
        return JPLEph_Neptune;
    case STA_PLUTO:
        return JPLEph_Pluto;
    case STA_SUN:
        return JPLEph_Sun;
    case STA_MOON:
        return JPLEph_Moon;
    default:
        return JPLEph_Invalid;
    }
}


const QList<StaBodyId>&
sta::JPLEphemeris::bodyList() const
{
    return m_bodies;
}


const StaBody*
sta::JPLEphemeris::parentBody(const StaBody* body) const
{
    JPLEphemItem id = mapStaId(body->id());
    if (id == JPLEph_Invalid)
    {
        return NULL;
    }
    else
    {
        if (id == JPLEph_SSB)
            return NULL;
        else if (id == JPLEph_Moon || id == JPLEph_Earth)
            return STA_SOLAR_SYSTEM->lookup(STA_EARTH_BARYCENTER);
        else
            return STA_SOLAR_SYSTEM->lookup(STA_SOLAR_SYSTEM_BARYCENTER);
    }
}


const sta::CoordinateSystemType
sta::JPLEphemeris::coordinateSystem(const StaBody* /* body */) const
{
    return sta::COORDSYS_EME_J2000;
}


sta::StateVector
sta::JPLEphemeris::stateVector(const StaBody* body,
                               double mjd,
                               const StaBody* center,
                               sta::CoordinateSystemType coordSys) const
{
    JPLEphemItem id = mapStaId(body->id());
    if (id == JPLEph_Invalid || body == center)
    {
        return StateVector::zero();
    }

    StateVector state = getPlanetStateVector(id, MjdToJd(mjd));
    const StaBody* parent = parentBody(body);

    // If the parent object in the ephemeris is the same as the center, then no
    // extra work is necessary. Otherwise, we also need compute the position of
    // the parent with respect to the center.
    if (parent == center)
    {
        return state;
    }
    else
    {
        const StaBody* ssb = STA_SOLAR_SYSTEM->lookup(STA_SOLAR_SYSTEM_BARYCENTER);
        StateVector parentState = parent->stateVector(mjd, ssb, coordSys);
        StateVector centerState = center->stateVector(mjd, ssb, coordSys);

        return StateVector(parentState.position - centerState.position + state.position,
                           parentState.velocity - centerState.velocity + state.velocity);
    }
}

// Return the position of an object relative to the solar system barycenter
// or the Earth (in the case of the Moon) at a specified TDB Julian date tjd.
// If tjd is outside the span covered by the ephemeris it is clamped to a
// valid time.
sta::StateVector
sta::JPLEphemeris::getPlanetStateVector(JPLEphemItem planet, double tjd) const
{
    // Solar system barycenter is the origin
    if (planet == JPLEph_SSB)
    {
        return StateVector(MyVector3d(0.0, 0.0, 0.0), MyVector3d(0.0, 0.0, 0.0));
    }

    // The position of the Earth must be computed from the positions of the
    // Earth-Moon barycenter and Moon. This is handled automatically because
    // Earth's 'parent' in the ephemeris is the Earth-Moon barycenter.
    if (planet == JPLEph_Earth)
    {
        // Get the geocentric position of the Moon
        sta::StateVector moonState = getPlanetStateVector(JPLEph_Moon, tjd);

        double f = 1.0 / (earthMoonMassRatio + 1.0);
        return StateVector(-moonState.position * f, -moonState.velocity * f);
    }

    // Clamp time to [ startDate, endDate ]
    if (tjd < startDate)
    {
        tjd = startDate;
    }
    else if (tjd > endDate)
    {
        tjd = endDate;
    }

    // recordIndex is always >= 0:
    unsigned int recordIndex = (unsigned int) ((tjd - startDate) / daysPerInterval);
    // Make sure we don't go past the end of the array if t == endDate
    if (recordIndex >= records.size())
    {
        recordIndex = records.size() - 1;
    }
    const JPLEphRecord& record = records[recordIndex];

    Q_ASSERT(coeffInfo[planet].nGranules >= 1);
    Q_ASSERT(coeffInfo[planet].nGranules <= 32);
    Q_ASSERT(coeffInfo[planet].nCoeffs <= MaxChebyshevCoeffs);

    // u is the normalized time (in [-1, 1]) for interpolating
    // coeffs is a pointer to the Chebyshev coefficients
    double u = 0.0;
    const double* coeffs = NULL;
    unsigned int nCoeffs = coeffInfo[planet].nCoeffs;
    double velocityScale = 1.0;

    if (coeffInfo[planet].nGranules == 0xffffffff)
    {
        coeffs = record.coeffs + coeffInfo[planet].offset;
        u = 2.0 * (tjd - record.t0) / daysPerInterval - 1.0;
        velocityScale = 2.0 / daysPerInterval;
    }
    else
    {
        // This interval is subdivided into shorter subintervals (called granules)
        // Adjust the coefficient pointer accordingly.
        double daysPerGranule = daysPerInterval / coeffInfo[planet].nGranules;
        int granule = (int) ((tjd - record.t0) / daysPerGranule);
        double granuleStartDate = record.t0 + daysPerGranule * (double) granule;
        coeffs = record.coeffs + coeffInfo[planet].offset +
                 granule * coeffInfo[planet].nCoeffs * 3;
        u = 2.0 * (tjd - granuleStartDate) / daysPerGranule - 1.0;
        velocityScale = 2.0 / daysPerGranule;
    }

    // Calculate the Chebyshev polynomial terms and their derivatives
    //Matrix<double, MaxChebyshevCoeffs, 1> positionTerms;
    double positionTerms[MaxChebyshevCoeffs];
    positionTerms[0] = 1.0;
    positionTerms[1] = u;

    //Matrix<double, MaxChebyshevCoeffs, 1> velocityTerms;
    double velocityTerms[MaxChebyshevCoeffs];
    velocityTerms[0] = 0.0;
    velocityTerms[1] = 1.0;
    for (unsigned int i = 2; i < nCoeffs; i++)
    {
        positionTerms[i] = 2.0 * u * positionTerms[i - 1] - positionTerms[i - 2];
        velocityTerms[i] = 2.0 * u * velocityTerms[i - 1] - velocityTerms[i - 2] + 2.0 * positionTerms[i - 1];
    }

    // Note that Eigen uses column-major storage, so we need to transpose the coefficient
    // matrix.
    MyVector3d position = Map<MatrixXd>(coeffs, nCoeffs, 3).transpose() * Map<MatrixXd>(positionTerms, nCoeffs, 1);
    MyVector3d velocity = Map<MatrixXd>(coeffs, nCoeffs, 3).transpose() * Map<MatrixXd>(velocityTerms, nCoeffs, 1);

    return StateVector(position, velocity * (velocityScale / 86400.0));
}


sta::JPLEphemeris*
sta::JPLEphemeris::load(QIODevice* device)
{
    QDataStream in(device);
    in.setByteOrder(QDataStream::BigEndian);

    JPLEphemeris* eph = NULL;

    // Skip past three header labels
    in.skipRawData(LabelSize * 3);
    if (in.status() != QDataStream::Ok)
        return NULL;

    // Skip past the constant names
    in.skipRawData(NConstants * ConstantNameLength);
    if (in.status() != QDataStream::Ok)
        return NULL;

    eph = new JPLEphemeris();
    if (eph == NULL)
        return NULL;

    // Read the start time, end time, and time interval
    in >> eph->startDate;
    in >> eph->endDate;
    in >> eph->daysPerInterval;
    if (in.status() != QDataStream::Ok)
    {
        delete eph;
        return NULL;
    }

    // Number of constants with valid values; not useful for us
    quint32 nConstants;
    in >> nConstants;

    in >> eph->au;     // kilometers per astronomical unit
    in >> eph->earthMoonMassRatio;

    // Read the coefficient information for each item in the ephemeris
    for (unsigned int i = 0; i < JPLEph_NItems; i++)
    {
        in >> eph->coeffInfo[i].offset;
        in >> eph->coeffInfo[i].nCoeffs;
        in >> eph->coeffInfo[i].nGranules;

        eph->coeffInfo[i].offset -= 3;
    }

    if (in.status() != QDataStream::Ok)
    {
        delete eph;
        return NULL;
    }

    in >> eph->DENum;

    switch (eph->DENum)
    {
    case 200:
        eph->recordSize = DE200RecordSize;
        break;
    case 405:
        eph->recordSize = DE405RecordSize;
        break;
    case 406:
        eph->recordSize = DE406RecordSize;
        break;
    default:
        delete eph;
        return NULL;
    }

    in >> eph->librationCoeffInfo.offset;
    in >> eph->librationCoeffInfo.nCoeffs;
    in >> eph->librationCoeffInfo.nGranules;
    if (in.status() != QDataStream::Ok)
    {
        delete eph;
        return NULL;
    }

    // Skip past the rest of the record
    in.skipRawData(eph->recordSize * 8 - 2856);
    // The next record contains constant values (which we don't need)
    in.skipRawData(eph->recordSize * 8);
    if (in.status() != QDataStream::Ok)
    {
        delete eph;
        return NULL;
    }

    unsigned int nRecords = (unsigned int) ((eph->endDate - eph->startDate) / eph->daysPerInterval);
    eph->records.resize(nRecords);

    for (unsigned int i = 0; i < nRecords; i++)
    {
        in >> eph->records[i].t0;
        in >> eph->records[i].t1;

    	// Allocate coefficient array for this record; the first two
    	// 'coefficients' are actually the start and end time (t0 and t1)
        eph->records[i].coeffs = new double[eph->recordSize - 2];
        for (unsigned int j = 0; j < eph->recordSize - 2; j++)
        {
            in >> eph->records[i].coeffs[j];
        }

    	// Make sure that we read this record successfully
        if (in.status() != QDataStream::Ok)
    	{
    	    delete eph;
    	    return NULL;
    	}
    }

    eph->m_bodies << STA_SUN << STA_MERCURY << STA_VENUS << STA_EARTH << STA_MARS
                  << STA_JUPITER << STA_SATURN << STA_URANUS << STA_NEPTUNE << STA_PLUTO
                  << STA_EARTH_BARYCENTER << STA_MOON;

    return eph;
}
