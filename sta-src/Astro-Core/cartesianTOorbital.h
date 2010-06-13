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
------------------ Author:       Guillermo Ortega               -------------------
------------------ Affiliation:  European Space Agency (ESA)    -------------------
-----------------------------------------------------------------------------------
 Patched by Guillermo on July  11th 2009 to comply with STA statevector defintions and treat special cases
 when eccentricity is bigger than zero or when the true anomly or the argument of the perigee are not well
 defined. This routine needs to be converted into equinoctial elements for v2.0

 */

#include "statevector.h"
using namespace sta;

/* This function converts a state vector given in cartesian coordinates to an state vector given in
   classical orbital elements. For a defintion about the orbital elements, please see "statevector.h"
   /
 Description:
      This function transforms spacecraft cartesian orbital elements in Keplerian
      orbital elements

 Input:
      mu standard gravitational parameter of the planet/moon
      x  x-coordinate in ECI system (km)
      y  y-coordinate in ECI system (km)
      z  z-coordinate in ECI system (km)
      xd vx-coordinate in ECI system (km/s)
      yd vy-coordinate in ECI system (km/s)
      zd vz-coordinate in ECI system (km/s)

 Output:
      a  semi-major axis (km)
      ec eccentricity (-)
      i inclination (rad)
      w0 argument of the perigee (rad)
      o0  right ascention of the ascending node (rad)
      m0 mean anomaly (rad)

 Example of call:


 Date: July 2009
 Version: 2.0
 */



/*
// This is the classical rotuine now deprecated that need to be removed in STA Archean 2.0
void cartesianTOorbital(double mu, double x, double y, double z, double xd, double yd,double zd,
                double& a,double& e,double& i,double& argperi,double& longnode,double& meananom);
*/


/* This function converts a state vector given in cartesian coordinates to an state vector given in
   classical orbital elements. For a defintion about the orbital elements, please see "statevector.h"  
 */
sta::KeplerianElements cartesianTOorbital(double mu,  sta::StateVector cartesianStateVector);
