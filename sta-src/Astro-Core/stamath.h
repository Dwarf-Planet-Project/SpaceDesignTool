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
 ------------------ Author: Chris Laurel  -------------------------------------------------
 ------------------ E-mail: (claurel@gmail.com) ----------------------------
 */

#ifndef _ASTROCORE_STAMATH_H_
#define _ASTROCORE_STAMATH_H_
#include <QString>


namespace sta
{
    inline double Pi() { return 3.14159265358979323846; }
    inline double halfPi() { return Pi() * 0.5; }

    template <typename T> T degToRad(T deg)
    {
        return deg * (T) (Pi() / 180.0);
    }

    template <typename T> T radToDeg(T rad)
    {
        return rad * (T) (180.0 / Pi());
    }

    template <typename T> T arcsecToRad(T sec)
    {
        return degToRad(sec / 3600.0);
    }

    template <typename T> T radToArcsec(T rad)
    {
        return radToDeg(rad) * 3600.0;
    }
double ConvertUnits(QString OutputUnits, double Parameter, QString FromUnit);
}

//double ConvertUnits(QString OutputUnits, double Parameter);



/*
This function computes the modulo 2 pi of a double number
input x = argument (radians)
output y = x modulo 2 pi
*/
double modulo2Pi (double);





#endif // _ASTROCORE_STAMATH_H_
