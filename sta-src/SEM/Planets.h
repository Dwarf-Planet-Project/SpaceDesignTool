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

#ifndef PLANETS_H_
#define PLANETS_H_


class Planets
{
private:
                double Radius;
                double GravConstant;
                double SolarFlux;
                double MagneticMoment;
                double MaxIRradiation;
                double MinIRradiation;

public:
         double getRadius(int body);
         double getGravConstant(int body);
         double getSolarFlux(int body);
         double getMagneticMoment(int body);
         double getMaxIRradiation(int body);
         double getMinIRradiation(int body);


};

#endif // PLANETS_H
