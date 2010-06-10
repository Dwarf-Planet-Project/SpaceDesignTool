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


