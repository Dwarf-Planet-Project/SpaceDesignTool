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

#ifndef PROPAGATEGAUSS_H
#define PROPAGATEGAUSS_H


#include "statevector.h"
#include <Eigen/Core>
#include <QList>

USING_PART_OF_NAMESPACE_EIGEN

class Perturbations;


/**
 *  Function to calculate the derivative of the keplerian state (for RK4); the dynamic equations are contained here.
 */
void derivGAUSS(VectorXd, double, VectorXd, VectorXd&);

/**
 *  Function to calculate the derivative of the keplerian state (for RKF); it only converts input eigen::vectors to double[]
 *  in order to call the standard derivGAUSS function
 */
void derivGAUSS_RKF (double state[], double time, double parameters[], double derivative[]);

/**
 *  Function to propagate the trajectory considering the perturbations influence; method of Gauss.
 */
sta::StateVector propagateGAUSS(double mu,
                                const sta::StateVector& initialState,
                                double deltat,
                                QList<Perturbations*>,
                                double time,
                                const QString& integrator);

#endif // PROPAGATEGAUSS_H
