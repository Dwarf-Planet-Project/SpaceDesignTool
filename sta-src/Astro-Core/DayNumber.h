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
