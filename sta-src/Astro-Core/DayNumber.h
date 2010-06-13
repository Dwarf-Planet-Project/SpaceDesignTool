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
//  Returns the specified local time as a day number qith a secon fraction,
//  Day 1 is supossed asJan 1, 0000, hence Jan 1, 1970 is day number 719529.
//  The fractional second portion corresponds to the portion of the specified day.
//          * Years can be negative and/or fractional.
//          * Months below 1 are considered to be January.
//          * Days of the month start at 1.
//          * Days beyond the end of the month go into subsequent months.
//          * Days before the beginning of the month go to the previous month.
//          * Days can be fractional.
//
//       This routine does not handle Julian calendars with dates before Octave 15, 1582.
//       Leap seconds are ignored
//
//
//------------------ Author:       Guillermo Ortega               -------------------
//------------------ Affiliation:  European Space Agency (ESA)    -------------------
//-----------------------------------------------------------------------------------
// Created May 23rd 2009

#ifndef DAYNUMBER_H
#define DAYNUMBER_H



void DayNumber (
        int,               // Input: Year
        int,               // Input: Month
        int,               // Input: Day
        int,               // Input: Hours
        int,               // Input: Minutes
        double,            // Input: Seconds
        double&,           // Output: Number of days
        double&            // Output: franctions of seconds
        );




#endif // DAYNUMBER_H
