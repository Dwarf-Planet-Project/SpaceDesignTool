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



//
 //------------------ Author: Chris Laurel  -------------------------------------------------
// ------------------ E-mail: (claurel@gmail.com) ----------------------------
 // Patched by Guillermo to add structures for OrbitalElements and TLEs, June 6th 2009
 // Patched by Guillemro to add class for PolarStateVector, June 7th 2009
//Added by Tiziana Sabatini TrueAnomaly and EccentricAnomaly


#ifndef _ASTROCORE_STATEVECTOR_H_
#define _ASTROCORE_STATEVECTOR_H_

#include "Eigen/Geometry"

namespace sta
{

  struct KeplerianElements  ////////////////////////////////// Keplerian elements structure ///////////////////////////////////
  {
 /*
    Orbital elements are the parameters required to uniquely identify a specific orbit.
    In celestial mechanics these elements are generally considered in classical two-body systems,
    where a Kepler orbit is used (derived from Newton's laws of motion and Newton's law of universal gravitation).
    There are many different ways to mathematically describe the same orbit, but certain schemes each consisting
    of a set of six parameters are commonly used in astronomy and orbital mechanics.

    The order of the structure is as follows:
    KeplerianElements.SemimajorAxis Semimajor axis (a) - similar to the radius of a circle,
    its length is the distance between the geometric center of the orbital ellipse
    with the periapsis (point of closest approach to the central body), passing through
    the focal point where the center of mass resides.

    KeplerianElements.Eccentricity (e) - shape of the ellipse, describing how flattened
    it is compared with a circle.

    KeplerianElements.Inclination (i) - vertical tilt of the ellipse with respect to the
    reference plane, measured at the ascending node (where the orbit passes upward through the
    reference plane).

    KeplerianElements.AscendingNode Longitude of the ascending node (RAAN) - horizontally orients the ascending
    node of the ellipse (where the orbit passes upward through the reference plane) with
    respect to the reference frame's vernal point.

    KeplerianElements.ArgumentOfPeriapsis Argument of periapsis (w) defines the orientation of the ellipse
    (in which direction it is flattened compared to a circle) in the orbital plane, as an angle
    measured from the ascending node to the semimajor axis

    KeplerianElements.MeanAnomaly Mean anomaly at epoch (M) defines the position of the orbiting body
    along the ellipse at a specific time (the "epoch").   */

    KeplerianElements() :
      SemimajorAxis(0.0),
      Eccentricity(0.0),
      Inclination(0.0),
      AscendingNode(0.0),
      ArgumentOfPeriapsis(0.0),
      MeanAnomaly(0.0),
      TrueAnomaly(0.0),
      EccentricAnomaly(0.0)
    {
    }

    double SemimajorAxis;
    double Eccentricity;
    double Inclination;
    double AscendingNode;
    double ArgumentOfPeriapsis;
    double MeanAnomaly;
    double TrueAnomaly;
    double EccentricAnomaly;
};
struct DelaunayElements
{
    double l;
    double g;
    double h;
    double L;
    double G;
    double H;
};
struct EquinoctialElements
{
    double SemimajorAxis;
    double esin;
    double ecos;
    double tancos;
    double tansin;
    double MeanLon;
};

 struct TwoLineElements  ////////////////////////////////// TLE structure ///////////////////////////////////
 {
  /* A Two-line element set (TLE) is a set of numbers that describe the orbit of an earth
     satellite. The TLE is in a format specified by NORAD and used by NORAD and NASA. The TLE
     can be used directly by the SGP4 model (or one of the SGP8, SDP4, SDP8 models).

     The order of the first row is as follows:
     TwoLineElements.Epoch = Epoch double
     TwoLineElements.Derivative1MeanMotion = Derivative 1 of the Mean Motion [rad/min] double
     TwoLineElements.Derivative2MeanMotion = Derivative 2 of the Mean Motion [rad/min] double
     TwoLineElements.BSTAR = BSTAR drag term double

     The order of the second row is as follows:
     TwoLineElements.Inclination = Inclination [rad] double
     TwoLineElements.AscendingNode = Right ascension of the ascending node [rad] double
     TwoLineElements.Eccentricity = Exccentricity [-] double
     TwoLineElements.ArgumentOfPeriapsis = Argument of perigee [rad] double
     TwoLineElements.MeanAomaly = Mean anomaly [rad] double
     TwoLineElements.MeanMotion = Mean motion [rad] double

      The third row contains the TLE ephemeris type
     TwoLineElements.EphemerisType = Type [-] int
       #define TLE_EPHEMERIS_TYPE_DEFAULT           0
       #define TLE_EPHEMERIS_TYPE_SGP               1
       #define TLE_EPHEMERIS_TYPE_SGP4              2
       #define TLE_EPHEMERIS_TYPE_SDP4              3
       #define TLE_EPHEMERIS_TYPE_SGP8              4
       #define TLE_EPHEMERIS_TYPE_SDP8              5
    */
        double Epoch, Derivative1MeanMotion, Derivative2MeanMotion, BSTAR;
        double Inclination, AscendingNode, Eccentricity, ArgumentOfPeriapsis, MeanAomaly, MeanMotion;
        int EphemerisType;
};



  struct GeographicVector  ////////////////// GeographicCoordinates structure ///////////////////////////////////
  {
 /*
Greographical coordinates
 */
        double Altitude;
        double Latitude;
        double Longitude;
        double AltitudeRate;
        double LatitudeRate;
        double LongitudeRate;
  };


  struct PolarVector  ////////////////// GeographicCoordinates structure ///////////////////////////////////
  {
 /*
Polar coordinates
 */
        double Range;
        double Azimuth;
        double Elevation;
        double RangeRate;
        double AzimuthRate;
        double ElevationRate;
  };


    
class StateVector
{
public:
    EIGEN_MAKE_ALIGNED_OPERATOR_NEW


    struct StateVectorCrossProduct
    {
        double CrossProductOfPosition;
        double CrossProductOfVelocity;
    };

    struct StateVectorModulo
    {
        double ModuloOfPosition;
        double ModuloOfVelocity;
    };

    StateVector() {}
    StateVector(const Eigen::Vector3d& _position, const Eigen::Vector3d& _velocity) :
       position(_position), velocity(_velocity) {}
    
    static StateVector zero()
    {
        return StateVector(Eigen::Vector3d::Zero(), Eigen::Vector3d::Zero());
    }

    StateVector operator+(const StateVector& other) const
    {
        return StateVector(position + other.position, velocity + other.velocity);
    }

    StateVector operator-(const StateVector& other) const
    {
        return StateVector(position - other.position, velocity - other.velocity);
    }

     // Delivers the cross product between two state vectors
    StateVectorCrossProduct operator*(const StateVector& other)
    {
        StateVectorCrossProduct _StateVectorCrossProduct;
        _StateVectorCrossProduct.CrossProductOfPosition = position.dot(other.position);
        _StateVectorCrossProduct.CrossProductOfVelocity = velocity.dot(other.velocity);
        return _StateVectorCrossProduct;
    }

     // Delivers the module of a state vectors
    StateVectorModulo modulo(const StateVector& other)
    {
        StateVectorModulo _StateVectorModulo;
        _StateVectorModulo.ModuloOfPosition = other.position.norm();
        _StateVectorModulo.ModuloOfVelocity = other.velocity.norm();
        return _StateVectorModulo;
    }

public:
    Eigen::Vector3d position;
    Eigen::Vector3d velocity;
};


}

#endif // _ASTROCORE_STATEVECTOR_H_
