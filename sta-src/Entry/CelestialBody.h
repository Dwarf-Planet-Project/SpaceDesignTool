/*
 Copyright (C) 2007 European Space Agency (space.trajectory.analysis AT gmail.com) -

 This program is free software; you can redistribute it and/or modify it under
 the terms of the GNU Lesser General Public License as published by the Free Software
 Foundation; either version 2 of the License, or (at your option) any later
 version.

 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more
 details.

 You should have received a copy of the GNU Lesser General Public License along with
 this program; if not, write to the Free Software Foundation, Inc., 59 Temple
 Place - Suite 330, Boston, MA  02111-1307, USA.

 Further information about the GNU Lesser General Public License can also be found on
 the world wide web at http://www.gnu.org.
*/
/*
------------------ Author:       Marnix Volckaert               -------------------
------------------ Affiliation:  TUDelft                        -------------------
-----------------------------------------------------------------------------------
 Modified by Tiziana Sabatini on May-July 2009
*/


#ifndef CELESTIALBODY_H_INCLUDED
#define CELESTIALBODY_H_INCLUDED

#include <QString>

using namespace std;

class CelestialBody {
    protected:
        double mu;                               //Gravitational parameter
        double omega;                            //Rotation rate
        double equatorial_radius;                //Equatorial radius
        double polar_radius;                     //Polar radius
        double J2, J3, J4;                       //Non spherical parameters
    public:
        double getMu() {return (mu);};
        double getOmega() {return (omega);};
        double getEquatorialRadius() {return (equatorial_radius);};
        double getPolarRadius() {return (polar_radius);};
        double getJ2() {return (J2);}; double getJ3() {return (J3);}; double getJ4() {return (J4);};
        //------
        void selectBody (QString);                //Fill the body object with the right data
};

#endif // CELESTIALBODY_H_INCLUDED
