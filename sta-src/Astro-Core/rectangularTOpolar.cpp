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
//      This function obtains polar elements of a state vector from a cartesian form 
//
// Input:
//		x		coordinate X in ECI (km)
//		y		coordinate X in ECI (km)
//		x		coordinate X in ECI (km)
//
// Output:
//		radius		the distance (km)
//      longitude   longitudinal polar coordinate (radians) (0 <= lambda <= 2*pi)
//      latitiude   latitudinal polar coordinate (radians) (-pi/2 <= phi <= pi/2)
//	
// Example of call:
//      
//
// Date: October 2006
// Version: 1.0
// Change history:
//
/////////////////////////////////////////////////////////////////////////////////////////////

#include <math.h>
#include "reduce.h"
#include "rectangularTOpolar.h"

void rectangularTOpolar(double x,double y,double z, double& radius, double& longitude, double& latitude)
{

	double lon;
	radius = sqrt(x*x+y*y+z*z);
	latitude = atan(z/sqrt(x*x+y*y));
	lon = atan2(y,x);
	longitude = reduce(lon,-3.1415927,3.1415927);
	
}


// Alternate version of rectangularTOpolar that takes vector as input
void rectangularTOpolar(const Eigen::Vector3d& v, double& radius, double& longitude, double& latitude)
{
    rectangularTOpolar(v.x(), v.y(), v.z(), radius, longitude, latitude);
}

