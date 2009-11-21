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
 ------------------ Author:       Tiziana Sabatini               -------------------
 ------------------ E-Mail:       tiziana.sabatini@yahoo.it      -------------------
 -----------------------------------------------------------------------------------
 */

#ifndef PROPAGATECOWELL_H
#define PROPAGATECOWELL_H

#include "perturbations.h"
#include "statevector.h"
#include "Eigen/Core"
#include "Scenario/propagationfeedback.h"

//USING_PART_OF_NAMESPACE_EIGEN

//class sta::StateVector;
class Perturbations;

/**
 *  Function to propagate the trajectory considering the perturbations influence; method of Cowell.
 */
sta::StateVector propagateCOWELL(double mu,
                                 const sta::StateVector& initialState,
                                 double deltat,
                                 const QList<Perturbations*>& perturbations,
                                 double time,
                                 const QString& integrator,
                                 PropagationFeedback& feedback);

/**
 *  Function to calculate the derivative of the state (for RK4); the dynamic equations are contained here.
 */
void derivCOWELL (VectorXd, double, VectorXd, VectorXd&);

#endif // PROPAGATECOWELL_H

