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
//  Converts TLE elements into Orbital Elements
//
//
//
//
//------------------ Author:       Guillermo Ortega               -------------------
//------------------ Affiliation:  European Space Agency (ESA)    -------------------
//-----------------------------------------------------------------------------------
// Created May 23rd 2009

#include <math.h>

#include <QString>
#include <QTextStream>
#include <QRegExp>
#include <QStringRef>
#include <QDateTime>

#include "constants.h"
#include "TLEtoOrbitalElements.h"
#include "DayNumber.h"
#include "calendarTOjulian.h"


/* TLE format description

AAAAAAAAAAAAAAAAAAAAAAAA
1 NNNNNU NNNNNAAA NNNNN.NNNNNNNN +.NNNNNNNN +NNNNN-N +NNNNN-N N NNNNN
2 NNNNN NNN.NNNN NNN.NNNN NNNNNNN NNN.NNNN NNN.NNNN NN.NNNNNNNNNNNNNN

Line 0 is a twenty-four character name
Lines 1 and 2 are the standard Two-Line Orbital Element Set Format

Line 1
Column	Description
01	Line Number of Element Data
03-07	Satellite Number
08	Classification (U=Unclassified)
10-11	International Designator (Last two digits of launch year)
12-14	International Designator (Launch number of the year)
15-17	International Designator (Piece of the launch)
19-20	Epoch Year (Last two digits of year)
21-32	Epoch (Day of the year and fractional portion of the day)
34-43	First Time Derivative of the Mean Motion
45-52	Second Time Derivative of Mean Motion (decimal point assumed)
54-61	BSTAR drag term (decimal point assumed)
63	Ephemeris type
65-68	Element number
69	Checksum (Modulo 10)
(Letters, blanks, periods, plus signs = 0; minus signs = 1)

Line 2
Column	Description
01	Line Number of Element Data
03-07	Satellite Number
09-16	Inclination [Degrees]
18-25	Right Ascension of the Ascending Node [Degrees]
27-33	Eccentricity (decimal point assumed)
35-42	Argument of Perigee [Degrees]
44-51	Mean Anomaly [Degrees]
53-63	Mean Motion [Revs per day]
64-68	Revolution number at epoch [Revs]
69	Checksum (Modulo 10)


Examples of TLEs

GOES 8
1 23051U 94022A   05110.02861301  .00000067  00000-0  00000+0 0  7468
2 23051   1.7976 101.4671 0002807  89.1462 271.4770  0.98903490 47696

XMM
1 25989U 99066A   05111.29166667  .00000000  00000-0  00000+0 0  9555
2 25989  48.3167 125.6178 6772592 131.6031 359.7424  0.50118105  1160

*/


//      a [m]   : semi-major axis
//      e []    : eccentricity
//      i [deg] : inclination
//      W [deg] : longitude of the ascending node
//      w [deg] : argument of perigee
//      M [deg] : mean anomaly at EPOCH


void TLEtoOrbitalElements (QString Line_0,                      // Line 0, a twenty-four character name
                           QString Line_1,                      // Lines 1 and 2 are the standard Two-Line Orbital Element Set Format
                           QString Line_2,                      // Lines 1 and 2 are the standard Two-Line Orbital Element Set Format
                           double& SemiMajorAxis,               // The semimajor axis of the satellite [m]
                           double& Eccentricity,                // The eccentricity [deg]
                           double& Inclination,                 // The inclination [deg]
                           double& LongitudeOfAscendingNode,    // The longitud of the ascencing node [deg]
                           double& ArgumentoOfPerigee,          // The argument of the perigee [deg]
                           double& MeanAnomaly,                 // The mean anomaly [deg]
                           double& Epoch,                       // The epoch of the TLE in Julian date
                           QString& InternationalDesignator,    // The International Designator
                           QString& ObjectName                  // The name of the object
                           )
{
    
    // For the time being the Line_0 is not used

    double SecondsOfADay = 86400;
    int Year, NumberOfDays, TheSecondsOfTheDay;
    double Day, FractionOfDay;

    QTextStream out (stdout);

    //out << "Line 0: " << Line_0 << endl;
    //out << "Line 1: " << Line_1 << endl;
    //out << "Line 2: " << Line_2 << endl;

    // Defining the separator that will segment the lines of the TLEs
    QRegExp separator("\\s+");  // the + Means one or more spaces!

    //out << "1 : " << Line_2.section(seprator, 1, 1).toDouble() << endl;
    //out << "2 : " << Line_2.section(seprator, 2, 2).toDouble() << endl;
    //out << "3 : " << Line_2.section(seprator, 3, 3).toDouble() << endl;
    //out << "4 : " << Line_2.section(seprator, 4, 4).toDouble() << endl;
    //out << "5 : " << Line_2.section(seprator, 5, 5) << endl;
    //out << "6 : " << Line_2.section(seprator, 6, 6) << endl;
    //out << "7 : " << Line_2.section(seprator, 7, 7) << endl;

    ObjectName = Line_0.section(separator, 0, 0);

    QString MeanMotionString = Line_2.section(separator, 7, 7); // From 53 to 63 inclusive both
    double MeanMotion = MeanMotionString.toDouble();      //printf("MeanMotion=%.8f\n",MeanMotion);

    QString EccentricityString = "0." + Line_2.section(separator, 4, 4); // From 27 to 33 inclusive both
    Eccentricity = EccentricityString.toDouble();         //printf("Eccentricity=%.8f\n",Eccentricity);

    QString InclinationString = Line_2.section(separator, 2, 2); // From 09 to 16 inclusive both
    Inclination = InclinationString.toDouble();            //printf("Inclination=%.8f\n",Inclination);

    QString LongitudeOfAscendingNodeString = Line_2.section(separator, 3, 3); // From 18 to 25 inclusive both
    LongitudeOfAscendingNode = LongitudeOfAscendingNodeString.toDouble();     //printf("LongitudeOfAscendingNode=%.8f\n",LongitudeOfAscendingNode);

    QString ArgumentoOfPerigeeString = Line_2.section(separator, 5, 5); // From 35 to 42 inclusive both
    ArgumentoOfPerigee = ArgumentoOfPerigeeString.toDouble();  //printf("ArgumentoOfPerigee=%.8f\n",ArgumentoOfPerigee);

    QString MeanAnomalyString = Line_2.section(separator, 6, 6); // From 35 to 42 inclusive both
    MeanAnomaly = MeanAnomalyString.toDouble();               //printf("MeanAnomaly=%.8f\n",MeanAnomaly);

    SemiMajorAxis = pow ( Earth_GRAVITY_PARAMETER * pow ((SecondsOfADay / MeanMotion / 2.0 / 3.14159265358979323846 ), 2.0 ), 0.333333);
    //printf("SemiMajorAxis=%.8f\n",SemiMajorAxis);

    QString TLEEpoch = Line_1.section(separator, 3, 3);       // From 19 to 32 inclusive both
    // The string TLEEpoch constains now a composed Epoch as follows:
    // 19-20	Epoch Year (Last two digits of year)
    // 21-32	Epoch (Day of the year and fractional portion of the day)
    // Example 05111.29166667
    //out << "TLEEpoch: " << TLEEpoch << endl;

    //Computing now the Year
    // Segmenting the TLEEpoch into the first two digits
    QStringRef YearString = TLEEpoch.leftRef(2);
    // Making this small string into a double
    Year = int(YearString.toString().toDouble());
    //out << "Year : " << Year << endl;

    // Creating a proper Year with 4 digits
    if (Year > 56)  // First satellite launched in 1956
    {
        Year = 1900 + Year;
    }
    else
    {
        Year = 2000 + Year;
    };

    // Computing now the Day of the year and its fraction
    // Segmenting the TLEEpoch into the last twelve digits
    QStringRef DaysString = TLEEpoch.rightRef(12);
    // Making this small string into a double
    Day = DaysString.toString().toDouble();
    //out << "Day: " << Day << endl;

    // Notice that the double variable "Day" is form by the number of days since the begining
    // of the year "Year" (we call this integer variable "NumberOfDays") and a fraction of the time
    // in seconds of the current day called "FractionOfDay".
    // That is  Day = NumberOfDays + FractionOfDay
    NumberOfDays = floor (Day);                             //out << "NumberOfDays: " << NumberOfDays << endl;
    FractionOfDay = Day - NumberOfDays;                     //out << "FractionsOfSecond: " << FractionsOfSecond << endl;
    TheSecondsOfTheDay = int(FractionOfDay*SecondsOfADay);  //out << "TheSecondsOfTheDay: " << TheSecondsOfTheDay << endl;

    // Finally converting the TLE date into a Gregorian date
    QDateTime EpochDateTime(QDate(Year, 1, 1), QTime(0,0,0));
    EpochDateTime = EpochDateTime.addDays(NumberOfDays).addSecs(TheSecondsOfTheDay);

    // Converting from QDateTime into integers via QString
    QString Temporal = EpochDateTime.toString("yyyy MM dd hh mm ss.zzzz");
    //out << "Temporal: " << Temporal << endl;
    int TheYear           = int (Temporal.section(separator, 0, 0).toDouble());    //out << "YYYY: " << TheYear << endl;
    int TheMonth          = int (Temporal.section(separator, 1, 1).toDouble());    //out << "MM: " << TheMonth << endl;
    int TheDay            = int (Temporal.section(separator, 2, 2).toDouble());    //out << "dd: " << TheDay << endl;
    int TheHour           = int (Temporal.section(separator, 3, 3).toDouble());    //out << "hh: " << TheHour << endl;
    int TheMinute         = int (Temporal.section(separator, 4, 4).toDouble());    //out << "mm: " << TheMinute << endl;
    double TheSecond      = int (Temporal.section(separator, 5, 5).toDouble());    //out << "ss: " << TheSecond << endl;

    // Making the conversion to Julian days.
    Epoch = calendarTOjulian (TheYear, TheMonth, TheDay, TheHour, TheMinute, TheSecond);
    // Notice that currently convertion to JulianDate handles fractions of seconds in UTC
    //out << "JDay: " << Epoch << endl;

    // Geeting now out the the International Designator as QString
    // The International Designator, also known as COSPAR designation, and in the United States as NSSDC ID,
    // is an international naming convention for satellites. It consists of the launch year, a 3-digit
    // incrementing launch number of that year and up to a 3-letter code representing the sequential id of a
    // piece in a launch.
    InternationalDesignator =  Line_1.section(separator, 2, 2);  // From 19 to 32 inclusive both


}
