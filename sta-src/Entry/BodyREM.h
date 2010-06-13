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

#ifndef BODYREM_H_INCLUDED
#define BODYREM_H_INCLUDED

#include "CelestialBody.h"
#include "capsule.h"
#include <Eigen/Core>
#include "Astro-Core/Atmosphere/AtmosphereModel.h"
#include "Astro-Core/statevector.h"

USING_PART_OF_NAMESPACE_EIGEN

class BodyREM : public CelestialBody
{
public:
    /**
     * Function to compute the altitude based on radial distance and geocentric latitude
     */
    double altitude (double, double);
    /**
     *Function to compute the radial distance based on altitude and geocentric latitude
     */
    double altitude2r (double, double);
    /**
     * Function that update the gravity vector
     */
    void updateGravity (sta::StateVector, Vector3d&);
    /**
     * Function that updates the aerodynamic acceleration vector, Cd, and some elements of state2
     */
    void updateAero (sta::StateVector, VectorXd&, AtmosphereModel, capsule_class, Vector3d&, double&);

private:
    /**
      * Function to calculate the radial component of gravity
      */
    double gr (double, double);
    /**
      * Function to calculate the radial component of gravity
      */
    double gdelta (double, double);

};
#endif // BODYREM_H_INCLUDED
