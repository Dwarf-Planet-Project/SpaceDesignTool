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

