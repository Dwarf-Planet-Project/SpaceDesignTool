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
 Patched by Guillermo on June 2010 to include constants
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


	// Masses of the Solar System bodies

	       const double Sun_MASS = 1.98892e30;                     // kg
	       const double Mercury_MASS = 3.30e23;                     // kg
	       const double Venus_MASS = 4.87e24;                     // kg
	       const double Earth_MASS = 5.97e24;                     // kg
	       const double Mars_MASS = 6.42e23;                     // kg
	       const double Jupiter_MASS = 1.90e27;                     // kg
	       const double Saturn_MASS = 5.68e26;                     // kg
	       const double Uranus_MASS = 8.68e25;                     // kg
	       const double Neptune_MASS = 1.02e26;                     // kg
	       const double Pluto_MASS = 1.27e22;                     // kg
	       const double Moon_MASS = 7.35e22;                     // kg
	       const double Phobos_MASS = 1.08e16;                     // kg
	       const double Deimos_MASS = 1.80e15;                     // kg
	       const double Metis_MASS = 9.56e16;                     // kg
	       const double Adrastea_MASS = 1.91e16;                     // kg
	       const double Amalthea_MASS = 3.50e18;                     // kg
	       const double Thebe_MASS = 7.77e17;                     // kg
	       const double Io_MASS = 8.93e22;                     // kg
	       const double Europa_MASS = 4.80e22;                     // kg
	       const double Ganymede_MASS = 1.48e23;                     // kg
	       const double Callisto_MASS = 1.08e23;                     // kg
	       const double Leda_MASS = 5.68e15;                     // kg
	       const double Himalia_MASS = 9.56e18;                     // kg
	       const double Lysithea_MASS = 7.77e16;                     // kg
	       const double Elara_MASS = 7.77e17;                     // kg
	       const double Ananke_MASS = 3.82e16;                     // kg
	       const double Carme_MASS = 9.56e16;                     // kg
	       const double Pasiphae_MASS = 1.91e17;                     // kg
	       const double Sinope_MASS = 7.77e16;                     // kg

	// Radius of the Solar System bodies

	       const double Sun_RADIUS = 695000000;                     // m
	       const double Mercury_RADIUS = 2440000;                     // m
	       const double Venus_RADIUS = 6052000;                     // m
	       const double Earth_RADIUS = 6378000;                     // m
	       const double Mars_RADIUS = 3397000;                     // m
	       const double Jupiter_RADIUS = 71492000;                     // m
	       const double Saturn_RADIUS = 60268000;                     // m
	       const double Uranus_RADIUS = 25559000;                     // m
	       const double Neptune_RADIUS = 24766000;                     // m
	       const double Pluto_RADIUS = 1150000;                     // m
	       const double Moon_RADIUS = 1738000;                     // m
	       const double Phobos_RADIUS = 11000;                     // m
	       const double Deimos_RADIUS = 6000;                     // m
	       const double Metis_RADIUS = 20000;                     // m
	       const double Adrastea_RADIUS = 10000;                     // m
	       const double Amalthea_RADIUS = 94000;                     // m
	       const double Thebe_RADIUS = 50000;                     // m
	       const double Io_RADIUS = 1821000;                     // m
	       const double Europa_RADIUS = 1565000;                     // m
	       const double Ganymede_RADIUS = 2634000;                     // m
	       const double Callisto_RADIUS = 2403000;                     // m
	       const double Leda_RADIUS = 8000;                     // m
	       const double Himalia_RADIUS = 93000;                     // m
	       const double Lysithea_RADIUS = 18000;                     // m
	       const double Elara_RADIUS = 38000;                     // m
	       const double Ananke_RADIUS = 15000;                     // m
	       const double Carme_RADIUS = 20000;                     // m
	       const double Pasiphae_RADIUS = 25000;                     // m
	       const double Sinope_RADIUS = 18000;                     // m



	// Gravity parameters of the Solar System bodies (i.e. mu = GRAVITATIONAL_CONSTANT * Planet_MASS)

	       const double Sun_GRAVITY_PARAMETER = 1.328182e20;                    // m^3 s^-2
	       const double Mercury_GRAVITY_PARAMETER = 2.202512e13;                   // m^3 s^-2
	       const double Venus_GRAVITY_PARAMETER = 3.250374e14;                    // m^3 s^-2
	       const double Earth_GRAVITY_PARAMETER = 3.984545e14;                  // m^3 s^-2
	       const double Mars_GRAVITY_PARAMETER = 4.284888e13;                     // m^3 s^-2
	       const double Jupiter_GRAVITY_PARAMETER = 1.268113e17;                    // m^3 s^-2
	       const double Saturn_GRAVITY_PARAMETER = 3.790991e16;                    // m^3 s^-2
	       const double Uranus_GRAVITY_PARAMETER = 5.793275e15;                    // m^3 s^-2
	       const double Neptune_GRAVITY_PARAMETER = 6.807766e15;                    // m^3 s^-2
	       const double Pluto_GRAVITY_PARAMETER = 8.476336e11;                    // m^3 s^-2
	       const double Moon_GRAVITY_PARAMETER = 4.905596e12;                    // m^3 s^-2
	       const double Phobos_GRAVITY_PARAMETER = 7.208222e05;                    // m^3 s^-2
	       const double Deimos_GRAVITY_PARAMETER = 1.201370e05;                    // m^3 s^-2
	       const double Metis_GRAVITY_PARAMETER = 6.380612e06;                    // m^3 s^-2
	       const double Adrastea_GRAVITY_PARAMETER = 1.274787e06;                    // m^3 s^-2
	       const double Amalthea_GRAVITY_PARAMETER = 2.335998e08;                    // m^3 s^-2
	       const double Thebe_GRAVITY_PARAMETER = 5.185916e07;                    // m^3 s^-2
	       const double Io_GRAVITY_PARAMETER = 5.960132e12;                    // m^3 s^-2
	       const double Europa_GRAVITY_PARAMETER = 3.203654e12;                    // m^3 s^-2
	       const double Ganymede_GRAVITY_PARAMETER = 9.877934e12;                    // m^3 s^-2
	       const double Callisto_GRAVITY_PARAMETER = 7.208222e12;                    // m^3 s^-2
	       const double Leda_GRAVITY_PARAMETER = 3.790991e05;                    // m^3 s^-2
	       const double Himalia_GRAVITY_PARAMETER = 6.380612e08;                    // m^3 s^-2
	       const double Lysithea_GRAVITY_PARAMETER = 5.185916e06;                    // m^3 s^-2
	       const double Elara_GRAVITY_PARAMETER = 5.185916e07;                    // m^3 s^-2
	       const double Ananke_GRAVITY_PARAMETER = 2.549575e06;                    // m^3 s^-2
	       const double Carme_GRAVITY_PARAMETER = 6.380612e06;                    // m^3 s^-2
	       const double Pasiphae_GRAVITY_PARAMETER = 1.274787e07;                    // m^3 s^-2
	       const double Sinope_GRAVITY_PARAMETER = 5.185916e06;                    // m^3 s^-2



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
