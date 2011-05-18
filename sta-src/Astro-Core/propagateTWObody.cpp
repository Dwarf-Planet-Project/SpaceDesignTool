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

///////////////////////////////////////////////////////////////////////////////////////
// Description: 
//      This function propagates spacecraft Keplerian 
//      orbital elements thought an interval of time
//      deltat and obtains the corresponding cartesian 
//      orbital elements at that time 
//
// Input: 
//		mu		standard gravitational parameter of the planet/moon 
//		a       semi-major axis (km)
//      ec      eccentricity (-)
//      i       inclination (rad)
//      w0      argument of the perigee (rad)
//      o0      right ascention of the ascending node (rad)
//      m0      mean anomaly (rad)
//      deltat  propagation time length (s)
//
// Output:
//	pos    xyz coordinates in ECI system (km)
//      vel    vxvyvz coordinates in ECI system (km/s)
//      ran1   RAAN at time=deltat (rad)
//      ap1    argument of perigee at time=deltat (rad)
//      ma1    Mean anomaly at time=deltat (rad)
//	
// Example of call:
//      propagateTWObody(398601.3,500,0,56,0,0,0,500,x,y,z,xd,yd,zd,ran1,ap1,ma1); 
//
// Date: October 2006
// Version: 1.0
// Change history:
//
////////////////////////////////////////////////////////////////////////////////////////

#include <math.h>
#include <float.h>
#include "orbitalTOcartesian.h"
#include <stdio.h>


sta::StateVector propagateTWObody(double mu,
                                  double a, double ec, double i, double w0, double o0, double m0,
                                  double deltat,
                                  double& ran1, double& ap1, double& ma1)
{
    // Declaring needed variables
    double n0;
    double mdot, odot, wdot, m, o, w;
    // end of varibales declaration

    n0= sqrt(mu/(a*a*a));
    mdot=n0;
    odot=0.0;  // No J2 or J4 considered
    wdot=0.0; // No J2 or J4 considered
    m=m0+mdot*deltat; o=o0+odot*deltat; w=w0+wdot*deltat;

    ran1 = o; //Already in rad
    ap1 = w; //Already in rad
    ma1 = m; //Already in rad

    return orbitalTOcartesian(mu, a, ec, i, w, o, m);
}


/**
 * Function: Computes Universal coefficients needed for the Kepler propagator
 * Source: Vallado, Fundamentals of Astrodynamics and Applications
 */
void computeCoefficients(double psi, double& c2, double& c3)
{
    if (psi > 1e-6)
    {
        c2 = (1 - cos(sqrt(psi))) / (psi);
        c3 = (sqrt(psi) - sin(sqrt(psi))) / (sqrt(pow(psi,3)));
    }
    else
    {
        if(psi < -1e-6)
        {
            c2 = (1 - cosh(sqrt(-psi))) / psi;
            c3 = (sinh(sqrt(-psi)) - sqrt(-psi)) / (sqrt(pow(-psi,3)));
        }
        else
        {
            c2 = 1.0/2.0;
            c3 = 1.0 / 6.0;
        }
    }
}

/**
 * Function: Propagates the trajectory, by solving Kepler's problem. (Two body propagator)
 * Source: Vallado, Fundamentals of Astrodynamics and Applications
 * @param mu The central body's gravity parameter.
 * @param initialState The state vector before propagation
 * @param dt The timestep [s]
 * @return The propagated state vector
 */
sta::StateVector propagateKEPLER(double mu, sta::StateVector initialState, double dt)
{
    double v0 = initialState.velocity.norm();
    double r0 = initialState.position.norm();
    double roDOTvo = initialState.position.dot(initialState.velocity);
    double sqrtMU = sqrt(mu);

    //double ksi =  ( (pow(v0,2)) / (2) ) - ( (mu) / (r0));
    double alpha = (-pow(v0,2) / mu) + (2 / r0);

    // Initial Guess
    double chi0;

    // Circle or Ellipse
    if (alpha > 0.000001)
    {
        chi0 = sqrtMU * dt * alpha;
    }
    // Parabola
    else if( fabs(alpha) < 0.000001)
    {
        Eigen::Vector3d angularMomentum = initialState.position.cross(initialState.velocity);
        double h = angularMomentum.norm();
        double p = pow(h,2) / mu;
        double cot_2s = 3 * sqrt(mu/pow(p,3)) * dt;
        double tan_2s = 1 / cot_2s;
        double s = atan(tan_2s) / 2;
        double tan_s = tan(s);
        double w = atan(pow(tan_s,(1/3)));
        double cot_2w = 1 / tan(2*w);

        chi0 = sqrt(p) * 2 *  cot_2w;
    }
    // Hyperbola
    else if (alpha < -0.000001)
    {
        double a = 1/alpha;
        int signDT = dt/fabs(dt);
        chi0 = signDT * sqrt(-a) * log( (-2*mu*alpha*dt) / (roDOTvo + signDT * sqrt(-mu*a) * (1-r0*alpha) ));
    }

    double psi;
    double chiN = chi0;
    double chiN1 = chi0;
    double c2, c3;
    double r;

    do
    {
        chiN = chiN1;

        psi = pow(chiN,2) * alpha;
        computeCoefficients(psi, c2, c3);

        r = (pow(chiN,2) * c2) + (( (roDOTvo)/ sqrtMU) * chiN * (1 - psi * c3)) + (r0 * (1 - psi * c2));

        chiN1 = chiN + ( (sqrtMU * dt) - (pow(chiN,3)*c3) - ((roDOTvo/sqrtMU)*pow(chiN,2)*c2) - (r0*chiN*(1-psi*c3)) ) / (r);
    }
    while( fabs(chiN - chiN1) > 1e-6);  // Decrease the value to increase the accuracy

    chiN = chiN1;

    double f = 1 - (pow(chiN,2))/(r0) * c2;
    double g = dt - (pow(chiN,3))/(sqrtMU) * c3;
    double f_dot = ((sqrtMU)/(r*r0)) * chiN * (psi*c3 -1);
    double g_dot = 1 - (pow(chiN,2)/r)*c2;

    sta::StateVector stateVector;

    stateVector.position = f*initialState.position + g*initialState.velocity;
    stateVector.velocity = f_dot*initialState.position + g_dot*initialState.velocity;

    return stateVector;
}

