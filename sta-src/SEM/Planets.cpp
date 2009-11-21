/*
 This program is free software; you can redistribute it and/or modify it under
 the terms of the GNU Lesser General Public License as published by the Free Software
 Foundation; either version 2 of the License, or (at your option) any later
 version.

 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more
 details.

 You should have received a copy of the GNU Lesser General Public License along with
 this program; if not, write to the Free Software Foundation, Inc., 59 Temple
 Place - Suite 330, Boston, MA 02111-1307, USA.
 Further information about the GNU Lesser General Public License can also be found on
 the world wide web at http://www.gnu.org.
 */

/*
 ------ Copyright (C) 2009 European Space Agency (space.trajectory.analysis AT gmail.com) ----
 ------------------ Author: Guido Ridolfi  -------------------------------------------------
 ------------------ E-mail: (guido.ridolfi@gmail.com) --------------------------------------
 ------------------ Author: Tiziana Sabatini  ----------------------------------------------
 ------------------ E-mail: (tiziana.sabatini@yahoo.it) ------------------------------------
 */

//
// Modified by Tiziana Sabatini, May 2009

#include "Planets.h"
#include <cmath>

using namespace std;


double Planets::getRadius(int central_body) //km
{
        if(central_body==1)
                         {double radius = 2439.7; return radius;}
        if(central_body==2)
                         {double radius = 6051.9; return radius;}
        if(central_body==3)
                         {double radius = 6378.135; return radius;}
        if(central_body==4)
                         {double radius = 3402.5; return radius;}
        if(central_body==5)
                         {double radius = 71492; return radius;}
        if(central_body==6)
                         {double radius = 60268; return radius;}
        if(central_body==7)
                         {double radius = 24764; return radius;}
        if(central_body==8)
                         {double radius = 25559; return radius;}
        if(central_body==9)
                         {double radius = 1195; return radius;}
}

double Planets::getGravConstant(int central_body) // m^3/s^2
{
        if(central_body==1)
                         { double mu = 22032e9;return mu;}
        if(central_body==2)
                         { double mu = 324859e9;return mu;}
        if(central_body==3)
                         { double mu = 398600e9;return mu;}
        if(central_body==4)
                         { double mu = 42828e9;return mu;}
        if(central_body==5)
                         { double mu = 126686534e9;return mu;}
        if(central_body==6)
                         { double mu = 37931187e9;return mu;}
        if(central_body==7)
                         { double mu = 5793947e9;return mu;}
        if(central_body==8)
                         { double mu = 6836529e9;return mu;}
        if(central_body==9)
                         { double mu = 10012e9;return mu;}
}

double Planets::getSolarFlux(int central_body) //W*m^2
{
        if(central_body==1)
                         { double SolarFlux = 0;return SolarFlux;}
        if(central_body==2)
                         { double SolarFlux = 0;return SolarFlux;}
        if(central_body==3)
                         { double SolarFlux = 1367;return SolarFlux;}
        if(central_body==4)
                         { double SolarFlux = 0;return SolarFlux;}
        if(central_body==5)
                         { double SolarFlux = 0;return SolarFlux;}
        if(central_body==6)
                         { double SolarFlux = 0;return SolarFlux;}
        if(central_body==7)
                         { double SolarFlux = 0;return SolarFlux;}
        if(central_body==8)
                         { double SolarFlux = 0;return SolarFlux;}
        if(central_body==9)
                         { double SolarFlux = 0;return SolarFlux;}
}

double Planets::getMaxIRradiation(int central_body) //W*m^2
{
        if(central_body==1)
                         { double MaxIRradiation = 0;return MaxIRradiation;}
        if(central_body==2)
                         { double MaxIRradiation = 0;return MaxIRradiation;}
        if(central_body==3)
                         { double MaxIRradiation = 258;return MaxIRradiation;}
        if(central_body==4)
                         { double MaxIRradiation = 0;return MaxIRradiation;}
        if(central_body==5)
                         { double MaxIRradiation = 0;return MaxIRradiation;}
        if(central_body==6)
                         { double MaxIRradiation = 0;return MaxIRradiation;}
        if(central_body==7)
                         { double MaxIRradiation = 0;return MaxIRradiation;}
        if(central_body==8)
                         { double MaxIRradiation = 0;return MaxIRradiation;}
        if(central_body==9)
                         { double MaxIRradiation = 0;return MaxIRradiation;}
}

double Planets::getMinIRradiation(int central_body) //W*m^2
{
        if(central_body==1)
                         { double MinIRradiation = 0;return MinIRradiation;}
        if(central_body==2)
                         { double MinIRradiation = 0;return MinIRradiation;}
        if(central_body==3)
                         { double MinIRradiation = 216;return MinIRradiation;}
        if(central_body==4)
                         { double MinIRradiation = 0;return MinIRradiation;}
        if(central_body==5)
                         { double MinIRradiation = 0;return MinIRradiation;}
        if(central_body==6)
                         { double MinIRradiation = 0;return MinIRradiation;}
        if(central_body==7)
                         { double MinIRradiation = 0;return MinIRradiation;}
        if(central_body==8)
                         { double MinIRradiation = 0;return MinIRradiation;}
        if(central_body==9)
                         { double MinIRradiation = 0;return MinIRradiation;}
}

double Planets::getMagneticMoment(int central_body) //Tesla*m^2
{
        if(central_body==1)
                         { double MagneticMoment = 0;return MagneticMoment;}
        if(central_body==2)
                         { double MagneticMoment = 0;return MagneticMoment;}
        if(central_body==3)
                         { double MagneticMoment = 7.960e15;return MagneticMoment;}
        if(central_body==4)
                         { double MagneticMoment = 0;return MagneticMoment;}
        if(central_body==5)
                         { double MagneticMoment = 0;return MagneticMoment;}
        if(central_body==6)
                         { double MagneticMoment = 0;return MagneticMoment;}
        if(central_body==7)
                         { double MagneticMoment = 0;return MagneticMoment;}
        if(central_body==8)
                         { double MagneticMoment = 0;return MagneticMoment;}
        if(central_body==9)
                         { double MagneticMoment = 0;return MagneticMoment;}
}







