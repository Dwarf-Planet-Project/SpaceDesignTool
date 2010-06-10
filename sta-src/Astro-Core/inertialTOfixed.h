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

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Description: 
//		This function converts inertial planet-centered coordinates of position and
//		velocity into plnet-centered, planet-fixed coordinates,
//		using the Greenwich Hour Angle as the rotation angle of the planet
//
// Input:
//		body	identification of the planet/moon
//					Earth=0, Mercury=1, Venus=2, Mars=3, Jupiter=4, Saturn=5, Uranus=6, Nepture=7, Pluto=8, Sun=9
//		GreenwichHourAngle	Greenwich Hour Angle
//		x					coordinate X in ECI (km)
//		y					coordinate X in ECI (km)
//		x					coordinate X in ECI (km)
//		xd					speed X in ECI (km)
//		yd					speed X in ECI (km)
//		xd					speed X in ECI (km)
//
// Output:
//		x					coordinate X in ECEF (km)
//		y					coordinate X in ECEF (km)
//		x					coordinate X in ECEF (km)
//		xd					speed X in ECEF (km)
//		yd					speed X in ECEF (km)
//		xd					speed X in ECEF (km)
//	
// Example of call:
//      
//
// Date: October 2006
// Version: 1.0
// Change history:
//
// Created for STA by 
//      (c) G. Ortega 
//          (Email: astrodynamics.esa@gmail.com)
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#include "Astro-Core/statevector.h"
void inertialTOfixed(int , double , 
					double , double , double , double , double , double ,
					double& , double& , double& , double& , double& , double& );
										
sta::StateVector inertialTOfixed(int,sta::StateVector,double);
