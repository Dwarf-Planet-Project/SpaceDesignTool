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
//------------------ Author:       Guillermo Ortega               -------------------
//------------------ Affiliation:  European Space Agency (ESA)    -------------------
//-----------------------------------------------------------------------------------
// Date: October 2006
// Version: 1.0
// Change history:
//

#include "orbitalTOcartesian.h"
#include <cmath>

using namespace std;


/**
 * This function propagates spacecraft Keplerian
 * orbital elements thought an interval of time
 * deltat and obtains the corresponding cartesian
 * orbital elements at that time. It uses the J2 perturbation for Earth.
 *
 * @param mu      the value of G times the mass of the central body
 * @param radius  radius of the central body (km)
 * @param J2      coefficient of the J2 gravitational harmonic
 * @param a       semi-major axis (km)
 * @param ec      eccentricity (-)
 * @param i       inclination (rad)
 * @param w0      argument of the perigee (rad)
 * @param o0      right ascention of the ascending node (rad)
 * @param m0      mean anomaly (rad)
 * @param deltat  propagation time length (s)
 *
 * @param ran1   RAAN at time=deltat (radians)
 * @param ap1    argument of perigee at time=deltat (radians)
 * @param ma1    Mean anomaly at time=deltat (radians)
 *
 * Example of call:
 * \code
 *      propagateJ2(0,500,0,56,0,0,0,500,x,y,z,xd,yd,zd,ran1,ap1,ma1);
 * \endcode
 */
sta::StateVector propagateJ2(double mu, double radius, double J2,
                             double a,double ec,double i,double w0,double o0,double m0,
                             double deltat,
                             double& ran1,double& ap1,double& ma1)
{
    // Doing the propagation
    double n0 = sqrt(mu/(a*a*a));
    double temp1 = (radius / a) * (radius / a);
    double temp2= (1-ec*ec);
    double mdot = n0*(1+1.5*J2*temp1*pow(temp2,-1.5)*(1-1.5*sin(i)*sin(i)));
    double odot = -1.5*J2*mdot*temp1*pow(temp2,-2.0)*cos(i);
    double wdot = 1.5*J2*mdot*temp1*pow(temp2,-2.0)*(2-2.5*sin(i)*sin(i));
    double m = m0 + mdot * deltat;
    double o = o0 + odot * deltat;
    double w = w0 + wdot * deltat;

    ran1 = o;
    ap1 = w;
    ma1 = m;

    // Calling the transformation routine
    return orbitalTOcartesian(mu, a, ec, i, w, o, m);
}

