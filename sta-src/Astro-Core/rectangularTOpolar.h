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

#include "Eigen/Geometry"

void rectangularTOpolar(double,double,double,double&,double&,double&);

// Alternate version of rectangularTOpolar that takes vector as input
void rectangularTOpolar(const Eigen::Vector3d& v, double& radius, double& longitude, double& latitude);

