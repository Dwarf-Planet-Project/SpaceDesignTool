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

/*
 ------------------ Author: Chris Laurel  -------------------------------------------------
 ------------------ E-mail: (claurel@gmail.com) ----------------------------
 */

#ifndef _ASTROCORE_STAMATH_H_
#define _ASTROCORE_STAMATH_H_


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
}





/*
This function computes the modulo 2 pi of a double number
input x = argument (radians)
output y = x modulo 2 pi
*/
double modulo2Pi (double);





#endif // _ASTROCORE_STAMATH_H_
