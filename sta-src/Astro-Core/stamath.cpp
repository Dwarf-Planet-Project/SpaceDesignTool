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



/*------------------ Author:       Guillermo Ortega               -------------------
------------------ Affiliation:  European Space Agency (ESA)    -------------------
----------------------------------------------------------------------------------
*/

#include<math.h>
#include<float.h>

#include "Astro-Core/stamath.h"

/*
This function computes the modulo 2 pi of a double number
input x = argument (radians)
output y = x modulo 2 pi
*/

double modulo2Pi (double x)
{

	double pi2, y;

        pi2 = 3.14159265358979323846 * 2;
	y = x - pi2 * floor(x/pi2);

	if (y < 0) y = y + pi2;
	
	return(y);

}
double sta::ConvertUnits(QString OutputUnits, double Parameter, QString FromUnit)
{

    double Output=Parameter;
    if(OutputUnits==FromUnit)
    {
        Output=Parameter;
    }
    if((OutputUnits=="deg")&&(FromUnit=="rad"))
    {
        Output=sta::radToDeg(Parameter);
    }
    if((OutputUnits=="rad")&&(FromUnit=="deg"))
    {
        Output=sta::degToRad(Parameter);
    }
    /*if(OutputUnits=="rad")
    {
        Output=Parameter;
    }
    if(OutputUnits=="km")
    {
        Output=Parameter;
    }*/
    if((OutputUnits=="m")&&(FromUnit=="km"))
    {
        Output=Parameter*1000;
    }
    if((OutputUnits=="AU")&&(FromUnit=="km"))
    {
        Output=Parameter*6.68458134*pow(10,-9);
    }
    if((OutputUnits=="nm")&&(FromUnit=="km"))
    {
        Output=Parameter*0.5399568;
    }
    /*if(OutputUnits=="km/s")
    {
        Output=Parameter;
    }*/
    if((OutputUnits=="m/s")&&(FromUnit=="km/s"))
    {
        Output=Parameter*1000;
    }
    return Output;
}
