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
//  Converts Day Numbers into Gregorian Calendar dates
//
//
//
//
//------------------ Author:       Guillermo Ortega               -------------------
//------------------ Affiliation:  European Space Agency (ESA)    -------------------
//-----------------------------------------------------------------------------------
// Created May 23rd 2009

#include <math.h>

#include "DayNumberToCalendar.h"

void DayNumberToCalendar(double DayNumber,
                         double FranctionOfDay,
                         int& Year,
                         int& Month,
                         int& Day,
                         int& Hours,
                         int& Minutes,
                         double& Seconds)

{

    // Move day 0 from midnight -0001-12-31 to midnight 0000-3-1
    int Corrector = floor (DayNumber) - 60;

    //Calculate number of centuries; K1 = 0.25 is to avoid rounding problems.
    int Centuries = floor ((Corrector - 0.25) / 36524.25);

    //Days within century; K2 = 0.25 is to avoid rounding problems.
    int DaysInCenturies = Corrector - 0.25 + Centuries - floor (Centuries / 4.0);

    //Calculate the year (year starts on March 1).
    Year = (int) floor (DaysInCenturies / 365.25);

    //Correct day in year.
    int c = (int) floor (DaysInCenturies - floor (365.25 * Year)) + 1;

    //Calculate month in year and day in month
    Month = floor ((5.0 * Day + 456.0) / 153.0);
    Day = c - floor ((153.0 * Month - 457.0) / 5.0);

    //Move to Jan 1 as start of year.
    Year = Year + 1;
    Month = Month - 12;

    // Convert hour-minute-seconds.  Attempt to account for precision of DayNumber format.
    int Fraction = DayNumber - floor(DayNumber);
    int Temporal = fabs (2.2204e-16 * 86400 * DayNumber);
    if (Temporal == 0) Temporal = 1;

    double TempralSecond = pow (2.0, (floor (-log ((double) Temporal))));
    Seconds = floor (86400 * Fraction * TempralSecond + 0.5) / TempralSecond;

    Hours = (int) floor (Seconds / 3600);
    Seconds = Seconds - 3600 * Hours;
    Minutes = (int) floor (Seconds / 60);
    Seconds = Seconds - 60 * Minutes;


}
