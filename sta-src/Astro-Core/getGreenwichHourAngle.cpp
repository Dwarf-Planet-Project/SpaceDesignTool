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



//------------------ Author:       Guillermo Ortega               -------------------
//------------------ Affiliation:  European Space Agency (ESA)    -------------------
//-----------------------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Description: 
//  This subroutine computes the Greenwich hour angle in degrees for the
//  input time.  It uses the model referenced in The Astronomical Almanac
//  for 1984, Section S (Supplement) and documented in "Exact 
//  closed-form geolocation algorithm for Earth survey sensors", by 
//  F.S. Patt and W.W. Gregg, Int. Journal of Remote Sensing, 1993.
//  It includes the correction to mean sideral time for nutation
//  as well as precession.
//
// Calling Arguments
//
//  Name					Type 	I/O	Description
//  iyr						I*4	 I	Year (four digits)
//  day						R*8	 I	Day (time of day as fraction)
//
// Return argument
//  Greenwich Hour Angle	R*8	 O	Greenwich hour angle (degrees)
//
//
//	Subprograms referenced:
//
//	JD		Computes Julian day from calendar date
//	EPHPARMS	Computes mean solar longitude and anomaly and
//			 mean lunar lontitude and ascending node
//	NUTATE		Compute nutation corrections to lontitude and 
//		        obliquity
// 
//	Program written by:	Frederick S. Patt in  FORTRAN, October 92
//	Program adapted by G. Ortega in MATLAB, December 2001
//  Program adapted by G. Ortega in C++, October 2006
///////		Program adapted by G. Ortega for STA January 2007
//
//	
//////////////////////////////////////////////////////////////////////////////////////////

#include<math.h>
#include<float.h>
#include "calendarTOjulian.h"
#include "nutate.h"
#include "ephparms.h"
#include "Astro-Core/stamath.h"


double getGreenwichHourAngle (double jdate)

{		
	double dtr, atr, t, t2, t3, l, lp, lraan, dpsi, deps, eps0, obliq, gstm, gst;
	
	dtr = 3.1415927/180.00000; //Conversion to rad
	atr = dtr/3600;

// time arguments
	t = (jdate - 2451545) / 36525;
	t2 = t * t;
	t3 = t * t2;

// fundamental trig arguments
        l = modulo2Pi (dtr * (280.4665 + 36000.7698 * t));
        lp = modulo2Pi (dtr * (218.3165 + 481267.8813 * t));
        lraan = modulo2Pi (dtr * (125.04452 - 1934.136261 * t));

// nutations in longitude and obliquity
	dpsi = atr * (-17.2 * sin(lraan) - 1.32 * sin(2 * l)
       - 0.23 * sin(2 * lp) + 0.21 * sin(2 * lraan));

	deps = atr * (9.2 * cos(lraan) + 0.57 * cos(2 * l)
       + 0.1 * cos(2 * lp) - 0.09 * cos(2 * lraan));

// mean and apparent obliquity of the ecliptic
        eps0 = modulo2Pi (dtr * (23 + 26 / 60 + 21.448 / 3600)
       + atr * (-46.815 * t - 0.00059 * t2 + 0.001813 * t3)); 

	obliq = eps0 + deps;

// greenwich mean and apparent sidereal time
        gstm = modulo2Pi (dtr * (280.46061837 + 360.98564736629 * (jdate - 2451545)
       + 0.000387933 * t2 - t3 / 38710000));
    
        gst = modulo2Pi (gstm + dpsi * cos(obliq));

	return(gst);
	
	
}
