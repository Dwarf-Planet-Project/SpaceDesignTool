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
//  Converts TLE elements into Orbital Elements
//
//
//
//
//------------------ Author:       Guillermo Ortega               -------------------
//------------------ Affiliation:  European Space Agency (ESA)    -------------------
//-----------------------------------------------------------------------------------
// Created May 23rd 2009

#ifndef TLETOORBITALELEMENTS_H
#define TLETOORBITALELEMENTS_H
#include <QString>




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


Example of TLE

GOES 8
1 23051U 94022A   05110.02861301  .00000067  00000-0  00000+0 0  7468
2 23051   1.7976 101.4671 0002807  89.1462 271.4770  0.98903490 47696

*/



void TLEtoOrbitalElements (QString,                // Line 0, a twenty-four character name
                           QString,                // Lines 1 and 2 are the standard Two-Line Orbital Element Set Format
                           QString,                // Lines 1 and 2 are the standard Two-Line Orbital Element Set Format
                           double&,                // The semimajor axis of the satellite [m]
                           double&,                // The eccentricity [deg]
                           double&,                // The inclination [deg]
                           double&,                // The longitud of the ascencing node [deg]
                           double&,                // The argument of the perigee [deg]
                           double&,                // The mean anomaly [deg]
                           double&,                // The epoch of the TLE in Julian date
                           QString&,               // The International Designator
                           QString&                // The name of the object
                           );



#endif // TLETOORBITALELEMENTS_H
