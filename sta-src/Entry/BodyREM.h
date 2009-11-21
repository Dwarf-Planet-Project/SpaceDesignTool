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
