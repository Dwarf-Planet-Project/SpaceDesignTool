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
 ------------------ Author:       Tiziana Sabatini               -------------------
 ------------------ E-mail:       tiziana.sabatini@yahoo.it      -------------------
 -----------------------------------------------------------------------------------
 */

#ifndef PROPAGATEENCKE_H
#define PROPAGATEENCKE_H

#include "perturbations.h"
#include "statevector.h"
#include "Eigen/Core"
#include "Scenario/propagationfeedback.h"

//USING_PART_OF_NAMESPACE_EIGEN

//class sta::StateVector;
class Perturbations;

/**
 *  Function to propagate the trajectory considering the perturbations influence; method of Encke.
 *  Returns the updated deviations.
 */
sta::StateVector propagateENCKE (double mu,
                                 const sta::StateVector& reference,
                                 double deltat,
                                 const QList<Perturbations*>& perturbations,
                                 double time,
                                 const sta::StateVector& state,
                                 const sta::StateVector& deviation,
                                 double q,
                                 const QString& integrator,
                                 PropagationFeedback& feedback);


/**
 *  Function to calculate the derivative of the state (for RK4); the dynamic equations are contained here.
 */
void derivENCKE (VectorXd, double, VectorXd, VectorXd&);

#endif // PROPAGATEENCKE_H
