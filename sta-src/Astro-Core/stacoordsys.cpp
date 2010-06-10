/*

 This program is free software; you can redistribute it and/or modify it under
 the terms of the GNU General Public License as published by the Free Software
 Foundation; either version 2 of the License, or (at your option) any later
 version.

 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE. See the GNU General Public License for more
 details.

 You should have received a copy of the GNU General Public License along with
 this program; if not, write to the Free Software Foundation, Inc., 59 Temple
 Place - Suite 330, Boston, MA 02111-1307, USA.
 Further information about the GNU General Public License can also be found on
 the world wide web at http://www.gnu.org.
 */


/*
 --- Copyright (C) 2009 STA Steering Board (space.trajectory.analysis AT gmail.com) ---
*/
//
/*
 ------------------ Author: Chris Laurel  -------------------------------------------------
 ------------------ E-mail: (claurel@gmail.com) ----------------------------
 */

#include "stacoordsys.h"
#include "stamath.h"
#include "RotationState.h"
#include "stabody.h"

using namespace sta;
using namespace Eigen;


/*
Constants from SPICE's Frames Required Reading document
http://naif.jpl.nasa.gov/pub/naif/toolkit_docs/C/req/frames.html#Inertial%20Frames
B1950:
z     =  1153.04066200330"
theta = -1002.26108439117"
zeta  =  1152.84248596724"

The transformation is given by Rz(-z) * Ry(theta) * Rz(-zeta)

Jay Lieske, ``Expressions for the Precession  Quantities
       Based upon the IAU (1976) System of Astronomical
       Constants'' Astron. Astrophys. 58, 1-16 (1977)
*/

static const Quaterniond B1950_to_J2000 =
    Quaterniond(AngleAxis<double>(arcsecToRad(1153.04066200330), Vector3d::UnitZ())) *
    Quaterniond(AngleAxis<double>(arcsecToRad(-1002.26108439117), Vector3d::UnitY())) *
    Quaterniond(AngleAxis<double>(arcsecToRad(1152.84248596724), Vector3d::UnitZ()));
static const Matrix3d B1950_to_J2000_mat = B1950_to_J2000.toRotationMatrix();
static const Matrix3d J2000_to_B1950_mat = B1950_to_J2000.conjugate().toRotationMatrix();

/*
ECLIPTIC J2000:
The value for the obliquity of the ecliptic at J2000 is taken from
"Explanatory Supplement to the Astronomical Almanac", P. Kenneth Seidelmann, 1992.
p. 114, equation 3.222-1:
e0 = 23 26' 21".448 - 46".8150 T - 0".00059 T^2 + 0".001813 T^3
*/
static const Quaterniond Equator_to_Ecliptic =
    Quaterniond(AngleAxis<double>(degToRad(23.439291111), Vector3d::UnitX()));
static const Matrix3d Equator_to_Ecliptic_mat = Equator_to_Ecliptic.toRotationMatrix();
static const Matrix3d Ecliptic_to_Equator_mat = Equator_to_Ecliptic.conjugate().toRotationMatrix();


/*! Create a coordinate system with the specified name. The name
 *  should be one of the strings allowed in a space scenario file.
 */
CoordinateSystem::CoordinateSystem(QString name) :
    m_type(COORDSYS_INVALID)
{
    if (name == "INERTIAL J2000")
        m_type = COORDSYS_EME_J2000;
    else if (name == "INERTIAL B1950")
        m_type = COORDSYS_EME_B1950;
    else if (name == "ECLIPTIC")
        m_type = COORDSYS_ECLIPTIC_J2000;
    else if (name == "PLANETO FIXED")
        m_type = COORDSYS_BODYFIXED;
    else if (name == "ROTATING")
        m_type = COORDSYS_ROT;
    else if (name == "ROTATING NORMALIZED")
        m_type = COORDSYS_ROT_NORM;
}


/*! Return the coordinate system name. This will be a string that may be
 *  to specify the coordinate system in an STA space scenario file.
 */
QString
CoordinateSystem::name() const
{
    switch (m_type)
    {
    case COORDSYS_EME_J2000:
        return "INERTIAL J2000";
    case COORDSYS_EME_B1950:
        return "INERTIAL B1950";
    case COORDSYS_ECLIPTIC_J2000:
        return "ECLIPTIC";
    case COORDSYS_BODYFIXED:
        return "PLANETO FIXED";
    case COORDSYS_ROT:
        return "ROTATING";
    case COORDSYS_ROT_NORM:
        return "ROTATING NORMALIZED";
    case COORDSYS_INVALID:
    default:
        return "INVALID";
    }
}


/*! Convert a state vector from this coordinate system to
 *  the Earth Mean Equator J2000.0 system.
 *  (Only works for inertial systems.)
 */
sta::StateVector
CoordinateSystem::toEmeJ2000(const StateVector& v) const
{
    Q_ASSERT(isInertial());

    if (m_type == COORDSYS_EME_J2000)
        return v;

    Matrix3d rotation;
    switch (m_type)
    {
    case COORDSYS_EME_J2000:
        rotation = Matrix3d::Identity();
        break;
    case COORDSYS_EME_B1950:
        rotation = B1950_to_J2000_mat;
        break;
    case COORDSYS_ECLIPTIC_J2000:
        rotation = Ecliptic_to_Equator_mat;
        break;

    case COORDSYS_BODYFIXED:
    case COORDSYS_ROT:
    case COORDSYS_ROT_NORM:
    default:
        rotation = Matrix3d::Identity();
        break;
    }

    return StateVector(rotation * v.position, rotation * v.velocity);
}


/*! Convert a state vector to this coordinate system from
 *  the Earth Mean Equator J2000.0 system.
 *  (Only works for inertial systems.)
 */
sta::StateVector
CoordinateSystem::fromEmeJ2000(const StateVector& v) const
{
    Q_ASSERT(isInertial());

    if (m_type == COORDSYS_EME_J2000)
        return v;

    Matrix3d rotation;
    switch (m_type)
    {
    case COORDSYS_EME_J2000:
        rotation = Matrix3d::Identity();
        break;
    case COORDSYS_EME_B1950:
        rotation = J2000_to_B1950_mat;
        break;
    case COORDSYS_ECLIPTIC_J2000:
        rotation = Equator_to_Ecliptic_mat;
        break;

    case COORDSYS_BODYFIXED:
    case COORDSYS_ROT:
    case COORDSYS_ROT_NORM:
    default:
        rotation = Matrix3d::Identity();
        break;
    }

    return StateVector(rotation * v.position, rotation * v.velocity);
}


static StateVector bodyFixedToEmeJ2000(const StateVector& state,
                                       const StaBody* center,
                                       double mjd)
{
    const RotationState* rotation = center->rotationState();
    Matrix3d r = Matrix3d::Identity();
    Vector3d w = Vector3d::Zero();
    if (rotation)
    {
        r = rotation->orientation(mjd).toRotationMatrix();
        w = rotation->angularVelocity(mjd);
    }

    return StateVector(r * state.position, r * state.velocity + w.cross(state.position));
}


static StateVector emeJ2000ToBodyFixed(const StateVector& state,
                                       const StaBody* center,
                                       double mjd)
{
    const RotationState* rotation = center->rotationState();
    Matrix3d r = Matrix3d::Identity();
    Vector3d w = Vector3d::Zero();
    if (rotation)
    {
        r = rotation->orientation(mjd).conjugate().toRotationMatrix();
        w = -rotation->angularVelocity(mjd);
    }

    return StateVector(r * state.position, r * state.velocity + w.cross(state.position));
}


/*! Convert a state vector from this coordinate system to
 *  the Earth Mean Equator J2000.0 system.
 *  (Only works for inertial and body fixed systems.)
 */
sta::StateVector
CoordinateSystem::toEmeJ2000(const StateVector& v, const StaBody* center, double mjd) const
{
    Q_ASSERT(isInertial() || m_type == COORDSYS_BODYFIXED);
    if (m_type == COORDSYS_BODYFIXED)
        return bodyFixedToEmeJ2000(v, center, mjd);
    else
        return toEmeJ2000(v);
}


/*! Convert a state vector to this coordinate system from
 *  the Earth Mean Equator J2000.0 system.
 *  (Only works for inertial and body fixed systems.)
 */
sta::StateVector
CoordinateSystem::fromEmeJ2000(const StateVector& v, const StaBody* center, double mjd) const
{
    Q_ASSERT(isInertial() || m_type == COORDSYS_BODYFIXED);
    if (m_type == COORDSYS_BODYFIXED)
        return emeJ2000ToBodyFixed(v, center, mjd);
    else
        return fromEmeJ2000(v);
}


/*! General coordinate system conversion function: convert
 *  a state vector between any two coordinate system (though
 *  only inertial and body-fixed systems are supported right
 *  now.)
 */
sta::StateVector
CoordinateSystem::convert(const StateVector& state,
                          double mjd,
                          const StaBody* fromCenter,
                          const CoordinateSystem& fromSys,
                          const StaBody* toCenter,
                          const CoordinateSystem& toSys)
{
    // First, check for simple cases...

    // No conversion necessary.
    if (fromCenter == toCenter && fromSys == toSys)
    {
        return state;
    }

    // Simple case when coordinate systems are identical, but centers are
    // different.
    // Different centers may imply different body fixed coordinate systems,
    // so non-inertial systems are non handled here.
    if (fromSys == toSys && fromSys.isInertial())
    {
        return state + fromCenter->stateVector(mjd, toCenter, fromSys.type());
    }

    // Same center.
    if (fromCenter == toCenter)
    {
        return toSys.fromEmeJ2000(fromSys.toEmeJ2000(state, fromCenter, mjd), toCenter, mjd);
    }

    // Handle the general case:
    //    Convert the input state vector to an EME J2000 coordinate system centered at the Solar
    //    System Barycenter, then convert it to the target coordinate system.
    //    TODO: Reducing to SSB-centered coordinates is overkill in some situations
    StateVector emeState = fromSys.toEmeJ2000(state, fromCenter, mjd) +
        fromCenter->stateVector(mjd, STA_SOLAR_SYSTEM->ssb(), COORDSYS_EME_J2000);
    emeState = emeState - toCenter->stateVector(mjd, STA_SOLAR_SYSTEM->ssb(), COORDSYS_EME_J2000);
    return toSys.fromEmeJ2000(emeState, toCenter, mjd);
}

