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


//------------------ Author:       Guillermo Ortega               -------------------
//------------------ Affiliation:  European Space Agency (ESA)    -------------------
//-----------------------------------------------------------------------------------

////////////////////////////////////////////////////////////////////////////////////////////////
////////  This subroutine computes ephemeris parameters used by other Mission
////////  Operations routines:  the solar mean longitude and mean anomaly, and
////////  the lunar mean longitude and mean ascending node.  It uses the model
////////  referenced in The Astronomical Almana//////// for 1984, Section S 
////////  (Supplement) and documented and documented in "Exact closed-form 
////////  geolocation algorithm for Earth survey sensors", by F.S. Patt and 
////////  W.W. Gregg, Int. Journal of Remote Sensing, 1993.  These parameters 
////////  are used to compute the solar longitude and the nutation in 
////////  longitude and obliquity.
////////
////////  Calling Arguments
////////
////////  Name		Type 	I/O	Description
////////  t			R*8	 I	Time in days since Jan. 1, 2000 at 12 hours UT
////////  xls		R*8	 O	Mean solar longitude (degrees)
////////  gs		R*8	 O	Mean solar anomaly (degrees)
////////  xlm		R*8	 O	Mean lunar longitude (degrees)
////////  omega	R*8	 O	Ascending node of mean lunar orbit (degrees)
////////
////////
////////	Program written by:	Frederick S. Patt in  FORTRAN, October 92
////////	Program adapted by G. Ortega in SCILAB, March 2001
///////		Program adapted by G. Ortega to C++, October 2006 
///////		Program adapted by G. Ortega for STA January 2007
////////
///////////////////////////////////////////////////////////////////////////////////////////

#include "math.h"



void ephparms(int t, double& xls, double& gs, double& xlm, double& omega)
{

	double temp;
	
////////  Sun Mean Longitude 		
	temp = 280.46592 + 0.9856473516*t;
	xls = fmod(temp,360);
 
////////  Sun Mean Anomaly		
	temp = 357.52772 + 0.9856002831*t;
	gs = fmod(temp,360);

////////  Moon Mean Longitude		
	temp = 218.31643 + 13.17639648*t;
	xlm = fmod(temp,360);

////////  Ascending Node of Moon's Mean Orbit 
    temp = 125.04452 - 0.0529537648*t;
	omega = fmod(temp,360);
	
}	
	
	
	
	
	
