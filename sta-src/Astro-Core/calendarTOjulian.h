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
//      The first routine computes Julian days in UTC while the second one
//      computes the TDB variant
//
//------------------ Author:       Guillermo Ortega               -------------------
//------------------ Affiliation:  European Space Agency (ESA)    -------------------
//-----------------------------------------------------------------------------------
//
// Created March 11th 2009
// Modified June 1st, 2009 to include the TDB varient of the Julian day
#include "Astro-Core/statevector.h"

double calendarTOjulian (int year,  int month, int day, int hour, int minute, int second);

//double calendarTOjulian (int year,  int month, int day, int hour, int minute, double second);

double calendarTOjulianTDB (int year,  int month, int day, int hour, int minute, double second);

//sta::mjdTOcalendar convertTOcalendar(double mjd);


