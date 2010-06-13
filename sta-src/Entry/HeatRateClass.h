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

/*
------------------ Author:       Marnix Volckaert               -------------------
------------------ Affiliation:  TUDelft                        -------------------
-----------------------------------------------------------------------------------
 Modified by Tiziana Sabatini on May-July 2009
*/


#ifndef HEATRATECLASS_H_INCLUDED
#define HEATRATECLASS_H_INCLUDED

#include <QString>

using namespace std;

class HeatRateClass
{
public:

    /**
     * Select the celestial body and load its thermal properties given the QString containing its name.
     */
    void selectBody (QString);
    /**
     * Return the convective heat rate for a generic step of hte integration.
     */
    double ConvectiveHeatRate(double, double, double);
    /**
     * Return the radiative heat rate for a generic step of hte integration.
     */
    double RadiativeHeatRate(double, double, double);

private:

    int aselector;
    double k, C, a, a1, a2, a3, b;
    double *velocity_array;
    double *function_array;

    int Ndatapoints;                    //Number of datapoints in the interpolation file
    int lowbound, highbound;

    /**
     * Return the interpolated function of the heat rate.
     */
    double qrdotInterpolation(double);

};

#endif // HEATRATECLASS_H_INCLUDED
