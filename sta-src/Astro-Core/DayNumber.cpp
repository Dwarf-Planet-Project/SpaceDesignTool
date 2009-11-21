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
//  Returns the specified local time as a day number with the fraction of seconds
//  Day 1 is supossed to be Jan 1, 0000, hence Jan 1, 1970 is day number 719529.
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


#include <math.h>
#include <vector>       // stl vector header

#include <QTextStream>  // For tracing

#include "DayNumber.h"

// import most common Eigen types
//USING_PART_OF_NAMESPACE_EIGEN
using namespace std;    // saves us typing std:: before vector



void DayNumber (
        int Year,
        int Month,
        int Day,
        int Hours,
        int Minutes,
        double Seconds,
        double& NumberOfDays,
        double& FractionOfSeconds
        )

{


  // For trace purposes
  QTextStream out (stdout);


  // Allocating the days of each month until start of month assuming year starts March 1
  int StarOfMonths [] = {306, 337, 0, 31, 61, 92, 122, 153, 184, 214, 245, 275};

  // Seting the start of year to March by moving Jan. and Feb. to previous year.
  // and  correcting for months > 12 by moving to subsequent years.
  Year = Year +  floor ( ( Month - 14) / 12.0 );
  
  int MyModulo = ( Month - 1 ) - 12 * floor ( ( Month - 1) / 12.0);
  //out << "My modulo: " << MyModulo << endl;

  // Calulating the number of days since start of the given year
  Day = Day + StarOfMonths [ MyModulo + 1 ] + 60;
  //out << "Day intermediate: " << Day << endl;

  // Adding the number of days to start the requested year and correct for leap year
  // every 4 years except centuries not divisible by 400
  Day = Day + 365 * Year + floor (Year / 4.0) - floor (Year / 100.0) + floor (Year / 400.0);
  //out << "Day final: " << Day << endl;

  // Finally adding the fraction representing the current second of the day.
  NumberOfDays = Day + ((Hours + (Minutes + (Seconds / 60) ) / 60 ) / 24) ;
  //out << "Day superfinal: " << Day << endl;

  // Output fraction of seconds as well
  FractionOfSeconds = (NumberOfDays * 86400) + (Hours * 3600) + (Minutes * 60) + Seconds;
  //out << "Fractions: " << FractionOfSeconds << endl;

}

