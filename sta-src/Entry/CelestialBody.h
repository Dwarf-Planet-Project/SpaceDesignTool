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
