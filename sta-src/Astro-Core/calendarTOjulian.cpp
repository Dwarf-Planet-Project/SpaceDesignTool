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
//       This function will compute the Julian date for a given calendar
//      date (year, month, day, hour, minute, second).
//
//------------------ Author:       Guillermo Ortega               -------------------
//------------------ Affiliation:  European Space Agency (ESA)    -------------------
//-----------------------------------------------------------------------------------
//
// Created March 11th 2009
// Modified June 1st, 2009 to include the TDB varient of the Julian day


#include <cmath>
#include"Astro-Core/statevector.h"
#include "Astro-Core/date.h"

double calendarTOjulian (int year,  int month, int day, int hour, int minute, int second)

{
	double frac, gyr, j_date;
	int iy0, im0;
	int ia, ib;
	int jd;

	/* decimal day fraction	*/
	frac = (hour/24.0)
		+ (minute/1440.0)
		+ (second/86400.0);
	/* convert date to format YYYY.MMDDdd	*/
	gyr = year
		+ (0.01 * month)
		+ (0.0001 * day)
		+ (0.0001 * frac) + 1.0e-9;
	/* conversion factors */
	if ( month <= 2 )
	{
		iy0 = year - 1L;
		im0 = month + 12;
	}
	else
	{
		iy0 = year;
		im0 = month;
	}
	ia = iy0 / 100L;
	ib = 2L - ia + (ia >> 2);
	/* calculate julian date	*/
	if ( year <= 0L )
		jd = (int) ((365.25 * (double) iy0) - 0.75)
			+ (int) (30.6001 * (im0 + 1L) )
			+ (int) day + 1720994L;
	else
		jd = (int) (365.25 * (double) iy0)
			+ (int) (30.6001 * (double) (im0 + 1L))
			+ (int) day + 1720994L;
	if ( gyr >= 1582.1015 )	/* on or after 15 October 1582	*/
		jd += ib;
	j_date = (double) jd + frac + 0.5;
	jd = (int) (j_date + 0.5);
	//weekday = (jd + 1L) % 7L;	
	
	return(j_date);
}


double calendarTOjulian (int year,  int month, int day, int hour, int minute, double second)
{

     double J_date = 367 * year - ceil(7.0 * (year + ceil ((month + 9.0) / 12.0)) / 4.0) +
                     ceil(275.0 * month / 9.0) + day + 1721013.5 +
                     (hour + (minute + second / 60) / 60) / 24;

     return(J_date);

}



double calendarTOjulianTDB (int year,  int month, int day, int hour, int minute, double second)

{

    double J_date = calendarTOjulian(year, month, day, hour, minute, second) + 0.00076604;

    return(J_date);
}

/*sta::mjdTOcalendar convertTOcalendar(double mjd)
{
    double jd=mjd+2400000.5;
    double T1900=(jd - 2415019.5)/365.25;
    double Year=1900;
}*/
