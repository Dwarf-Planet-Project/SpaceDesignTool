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


///////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////  This subroutine computes the nutation in longitude and the obliquity
////////  of the ecliptic corrected for nutation.  It uses the model referenced
////////  in The Astronomical Almana//////// for 1984, Section S (Supplement) and 
////////  documented in "Exact closed-form geolocation algorithm for Earth 
////////  survey sensors", by F.S. Patt and W.W. Gregg, Int. Journal of 
////////  Remote Sensing, 1993.  These parameters are used to compute the 
////////  apparent time correction to the Greenwich Hour Angle and for the 
////////  calculation of the geocentri//////// Sun vector.  The input ephemeris 
////////  parameters are computed using subroutine ephparms.  Terms are 
////////  included to 0.1 arcsecond.
////////
////////  Calling Arguments:
////////
////////  Name		Type 	I/O	Description
////////  t			R*8	 I	Time in days since Jan. 1, 2000 at 12 hours UT
////////  xls		R*8	 I	Mean solar longitude (degrees)
////////  gs		R*8	 I	Mean solar anomaly   (degrees)
////////  xlm		R*8	 I	Mean lunar longitude (degrees)
////////  Omega		R*8	 I	Ascending node of mean lunar orbit (degrees)
////////  dPsi		R*8	 O	Nutation in longitude (degrees)
////////  Eps		R*8	 O	Obliquity of the Ecliptic (degrees)
//////// 				 (includes nutation in obliquity)
////////
////////
////////	Program written by:	Frederick S. Patt in  FORTRAN, October 92
////////	Program adapted by G. Ortega in MATLAB, March 2001
////////	Program adapted by G. Ortega in C++, October 2006		
///////		Program adapted by G. Ortega for STA January 2007
////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#include "math.h"


void nutate(int t, double xls, double gs, double xlm, double omega, double& dpsi, double& eps)
{

	double re, rem, radeg, f, omf2, omegae, epsm, deps;

	re = 6378.137;  //////// Radius of Earth at equator (km)
	rem = 6371;  //////// Average of Earth radius (km)
	radeg = 3.1415927/180;  //////// Convertion factor rad to deg.
	f = 1/298.257;   //////// Earth flatering factor
	omf2 = (1-f)*(1-f);
    omegae = 7.29211585494e-5; //////// Earth rotation rate (rad/sec)


////////  Nutation in Longitude
	dpsi = - 17.1996*sin(omega*radeg) + 
      0.2062*sin(2.0*omega*radeg) - 
      1.3187*sin(2.0*xls*radeg) + 
      0.1426*sin(gs*radeg) - 
      0.2274*sin(2.0*xlm*radeg); 
	
////////  Mean Obliquity of the Ecliptic	
	epsm = 23.439291 - 3.560-7*t;

////////  Nutation in Obliquity 
	deps = 9.2025*cos(omega*radeg) + 0.5736*cos(2.0*xls*radeg);

////////  True Obliquity of the Ecliptic//////// 
	eps = epsm + deps/3600;

	dpsi = dpsi/3600;

}

