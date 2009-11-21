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
//------------------ Author:       Guillermo Ortega               -------------------
//------------------ Affiliation:  European Space Agency (ESA)    -------------------
//-----------------------------------------------------------------------------------


#include<float.h>
#include<math.h>
#include<stdio.h>


/////////////////////////////////////////////////////////////////////////////////////////
// Description: 
//      This function transforms spacecraft Keplerian 
//      orbital elements in cartesian orbital elements 
//
// Input: 
//	mu	standard gravitational parameter of the planet/moon
//	a  semi-major axis (km)
//      ec eccentricity (-)
//      i inclination (rad)
//      w0 argument of the perigee (rad)
//      o0  right ascention of the ascending node (rad)
//      m0 mean anomaly (rad)
//
// Output:
//	x  x-coordinate in ECI system (km)
//      y  y-coordinate in ECI system (km)
//      z  z-coordinate in ECI system (km)
//      xd vx-coordinate in ECI system (km/s)
//      yd vy-coordinate in ECI system (km/s)
//      zd vz-coordinate in ECI system (km/s)
//	
// Example of call:
//      orbitalTOcartesian(500,0,56,0,0,0,x,y,z,vx,vy,vz); 
//
// Date: October 2006
// Version: 1.0
// Change history:
//
//////////////////////////////////////////////////////////////////////////////////////////////

#include "orbitalTOcartesian.h"
#include <Eigen/Core>
#include <Eigen/Geometry>
#include <cmath>

using namespace sta;
using namespace Eigen;
using namespace std;


/**
 * Description:
 *      This function transforms spacecraft Keplerian
 *      orbital elements in cartesian orbital elements
 *
 *
 * @param	mu	standard gravitational parameter of the planet/moon
 * @param a  semi-major axis (km)
 * @param ec eccentricity (-)
 * @param i inclination (rad)
 * @param w0 argument of the perigee (rad)
 * @param o0  right ascention of the ascending node (rad)
 * @param m0 mean anomaly (rad)
 *
 * @return Cartesian state vector in the same coordinate system as the orbital elements
 *         Units are km for position and km/s for velocity.
 *
*/
sta::StateVector orbitalTOcartesian(double mu, double a, double ec, double i, double w0, double o0, double m0)
{
    // Declaring needed variables
    double n0, eca, diff, eps, ceca, seca, e1, xw, yw, edot, xdw, ydw, cw;
    double sw, so, co, si, ci, swci, cwci, px, py, pz, qx, qy, qz;
    // end of varibales declaration

    n0= sqrt(mu/(a*a*a));
    eca= m0+(ec/2.0);
    diff=10000.0;
    eps=.000001;
    while (diff>eps)
    {
            e1=eca-(eca-ec*sin(eca)-m0)/(1-ec*cos(eca));
            diff=fabs(e1-eca);  //Carefull: is FABS for a floating value!
            eca=e1;
    }; // from while
    ceca=cos(eca);
    seca=sin(eca);
    e1=a*sqrt(1-ec*ec);
    xw=a*(ceca-ec);		// printf("xw: %5e\n", xw);
    yw=e1*seca;
    edot=sqrt(mu/a)/(a*(1-ec*ceca));
    xdw=-a*edot*seca;
    ydw=e1*edot*ceca;
    cw = cos(w0); sw=sin(w0); co=cos(o0); so=sin(o0);
    ci=cos(i); si=sin(i); swci=sw*ci; cwci=cw*ci;
    px=cw*co-so*swci; py=cw*so+co*swci; pz=sw*si;	// printf("px: %5e\n", px);
    qx=-sw*co-so*cwci; qy=-sw*so+co*cwci; qz=cw*si;

    Vector3d position(xw*px+yw*qx, xw*py+yw*qy, xw*pz+yw*qz);
    Vector3d velocity(xdw*px+ydw*qx, xdw*py+ydw*qy, xdw*pz+ydw*qz);

    return StateVector(position, velocity);
}


/**
 * This function transforms spacecraft Keplerian
 * orbital elements to a cartesian state vector in the same reference frame.
 *
 * @param mu standard gravitational parameter of the planet/moon
 * @param elements Keplerian orbital elements, with angles in radians and SMA in km
 *
 * @return Cartesian state vector in the same coordinate system as the orbital elements
 *         Units are km for position and km/s for velocity.
 *
 */
sta::StateVector orbitalTOcartesian(double mu, const KeplerianElements& elements)

{
    double a = elements.SemimajorAxis;
    double m0 = elements.MeanAnomaly;

    double eca = m0 + (elements.Eccentricity / 2.0);

    double diff = 10000.0;
    double eps = 0.000001;
    double e1 = 0.0;

    // Solve Kepler's equation with the standard iteration
    while (diff > eps)
    {
        e1 = eca - (eca - elements.Eccentricity * sin(eca) - m0) / (1 - elements.Eccentricity * cos(eca));
        diff = std::abs(e1-eca);
        eca = e1;
    }

    double ceca = cos(eca);
    double seca = sin(eca);
    e1 = a * sqrt(1 - elements.Eccentricity * elements.Eccentricity);
    double xw = a * (ceca - elements.Eccentricity);
    double yw = e1 * seca;

    double edot = sqrt(mu / a) / (a * (1 - elements.Eccentricity * ceca));
    double xdw = -a * edot * seca;
    double ydw = e1 * edot * ceca;

    Quaterniond rotation = Quaterniond(AngleAxis<double>(elements.AscendingNode,       Vector3d::UnitZ())) *
                           Quaterniond(AngleAxis<double>(elements.Inclination,         Vector3d::UnitX())) *
                           Quaterniond(AngleAxis<double>(elements.ArgumentOfPeriapsis, Vector3d::UnitZ()));
    Vector3d position = rotation * Vector3d(xw, yw, 0.0);
    Vector3d velocity = rotation * Vector3d(xdw, ydw, 0.0);

    return StateVector(position, velocity);
}
