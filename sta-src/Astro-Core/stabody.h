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

/*
 ------------------ Author: Chris Laurel  -------------------------------------------------
 ------------------ E-mail: (claurel@gmail.com) ----------------------------
 */

#ifndef _ASTROCORE_STABODY_H_
#define _ASTROCORE_STABODY_H_

#include <QHash>
#include <cmath>
#include <Eigen/Geometry>
#include "statevector.h"
#include "stacoordsys.h"
#include "RotationState.h"
#include "GravityModel.h"

// Move to astro core
typedef double JulianDate;

namespace sta
{
    class Ephemeris;
}

/*! Solar system body ID codes. Current values are NAIF IDs; eventual numbering
 *  scheme TBD.
 */
enum StaBodyId
{
    STA_SOLAR_SYSTEM_BARYCENTER = 0,
    STA_EARTH_BARYCENTER = 3,
    STA_MERCURY =  199,
    STA_VENUS   =  299,
    STA_EARTH   =  399,
    STA_MARS    =  499,
    STA_JUPITER =  599,
    STA_SATURN  =  699,
    STA_URANUS  =  799,
    STA_NEPTUNE =  899,
    STA_PLUTO   =  999,
    STA_SUN     =   10,
    STA_MOON	=  301,
    STA_PHOBOS  =  401,
    STA_DEIMOS	=  402,
    STA_EUROPA 	=  502,
    STA_TITAN	=  606,
    STA_ENCELADUS= 602,
    STA_TRITON	=  801,
    STA_INVALID_BODY = -999999999,
};


/*! STA body class; an instance of this class should be created for
 *  every body used in STA.
 */
class StaBody
{
public:
    StaBody(StaBodyId id, const QString& name, double mu, const Eigen::Vector3d& radii, double linearV, double distance, double orbitalPeriod, double inclination) :
        m_id(id),
        m_name(name),
        m_radii(radii),
        m_linearV(linearV),
        m_distance(distance),
        m_orbitalPeriod (orbitalPeriod),
        m_inclination (inclination),
        m_ephemeris(NULL),
        m_rotationState(NULL),
        m_gravityModel(mu)
    {
    }
    
    StaBodyId id() const
    {
        return m_id;
    }
    
    QString name() const
    {
        return m_name;
    }
    
    double mu() const
    {
        return m_gravityModel.mu();;
    }
    
     /*! The linear velocity is given in rad/days. It's the linear velocity of the body
     *  w.r.t. the Sun. For Moons the linear velocity has been given relative to the
     *  main body (Moon-->Earth, Europa-->Jupiter, et cetera).
     */

    double linearV() const
    {
        return m_linearV;
    }

    /*! Return elicentric average distances of planets. For moons the distance is taken from the main body.
     */
    double distance() const
    {
        return m_distance;
    }

    /*! Return the orbital period in days. In case of moons, the orbital period is referred to the main body
     */
    double orbitalPeriod() const
    {
        return m_orbitalPeriod;
    }

    /*! Return the inclination in degrees w.r.t. the ecliptic plane
     */
    double inclination() const
    {
        return m_inclination;
    }


    /*! Return the mean radius of the body. There are several definitions of
     *  of the mean radius; the one calculated by this method is the
     *  volumetric mean radius, which is the radius of a sphere with a
     *  volume equal to the planet ellipsoid. The volumetric mean radius
     *  is the one used in the IAU/IAG tables here:
     *     http://astrogeology.usgs.gov/Projects/WGCCRE/constants/iau2000_table5.html
     */
    double meanRadius() const
    {
        return std::pow(m_radii.x() * m_radii.y() * m_radii.z(), 1.0 / 3.0);
    }
    
    /*! Return the polar radius of the body. The polar radius is always the z
      * component of the radii vector.
      */
    double polarRadius() const
    {
        return m_radii.z();
    }

    /*! Return the mean equatorial radius. If the body has a triaxial shape (i.e.
      * the two equatorial radii are different), the mean equatorial radius is
      * computed as the square root of the products of the the two equatorial
      * radii.
      */
    double equatorialRadius() const
    {
        return std::sqrt(m_radii.x() * m_radii.y());
    }

    /*! Return the semiAxes of the body's reference ellipsoid.
     *  radii.x = equatorial radius (for satellites, the subplanetary eq. radius)
     *  radii.y = equatorial radius (for satellites, the along orbit eq. radius)
     *  radii.z = polar radius
     */
    Eigen::Vector3d radii() const
    {
      	return m_radii;
    }

    sta::StateVector stateVector(double mjd, const StaBody* center, sta::CoordinateSystemType coordSys) const;

    Eigen::Quaterniond orientation(double mjd, sta::CoordinateSystemType coordSys) const;

    const sta::Ephemeris* ephemeris() const
    {
        return m_ephemeris;
    }

    void setEphemeris(const sta::Ephemeris* ephemeris)
    {
        m_ephemeris = ephemeris;
    }

    const sta::RotationState* rotationState() const
    {
        return m_rotationState;
    }

    void setRotationState(const sta::RotationState* rotationState)
    {
        m_rotationState = rotationState;
    }

    const sta::GravityModel& gravityModel() const
    {
        return m_gravityModel;
    }

    // Non-const gravity model accessor
    sta::GravityModel& gravityModel()
    {
        return m_gravityModel;
    }

    /*! Return the base texture file name
     */
    const QString& baseTexture() const
    {
        return m_baseTexture;
    }

    void setBaseTexture(QString baseTexture)
    {
        m_baseTexture = baseTexture;
    }

    Eigen::Vector3d planetographicToCartesian(double latitude, double longitude, double altitude) const;

private:
    StaBodyId m_id;
    QString m_name;
    double m_mu;
    Eigen::Vector3d m_radii;
    double m_linearV;
    double m_distance;
    double m_orbitalPeriod;
    double m_inclination;
    const sta::Ephemeris* m_ephemeris;
    const sta::RotationState* m_rotationState;
    sta::GravityModel m_gravityModel;
    QString m_baseTexture;
};


/*! Singleton class with a list of all solar system bodies used in STA. This must
 *  be initialized at start time by calling the Create. Thereafter, the dictionary
 *  should be accessed via the STA_SOLAR_SYSTEM macro.
 */
class SolarSystemBodyDictionary
{
public:
    static void Create();
    static void UseEphemeris(const sta::Ephemeris* ephemeris);
    static SolarSystemBodyDictionary* instance()
    {
        return s_instance;
    }
    
    StaBody* lookup(const QString& name) const;
    StaBody* lookup(StaBodyId id) const;
    const QList<StaBody*> list() const;
    const QList<StaBody*>& majorBodies() const;

    /** Convenience method to get the Solar System Barycenter. */
    const StaBody* ssb() const
    {
        return m_ssb;
    }

    /** Convenience method to get the Sun. */
    const StaBody* sun() const
    {
        return m_sun;
    }

    /** Convenience method to get the Earth. */
    const StaBody* earth() const
    {
        return m_earth;
    }

private:
    SolarSystemBodyDictionary();
    
private:
    QHash<StaBodyId, StaBody*> m_bodies;
    QHash<QString, StaBody*> m_indexByName;
    QList<StaBody*> m_majorBodies;
    StaBody* m_sun;
    StaBody* m_earth;
    StaBody* m_ssb;
    static SolarSystemBodyDictionary* s_instance;
};

#define STA_SOLAR_SYSTEM SolarSystemBodyDictionary::instance()

#endif // _ASTROCORE_STABODY_H_
