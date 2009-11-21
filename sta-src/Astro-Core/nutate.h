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


/////////////////////////////////////////////////////////////////////////////////////////////
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
/////////////////////////////////////////////////////////////////////////////////////////////


void nutate(int , double , double , double , double , double& , double& );
