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

#include "propagateENCKE.h"
#include "propagateTWObody.h"
#include "cartesianTOorbital.h"
#include "statevector.h"
#include "perturbations.h"
#include "EODE/eode.h"
#include <QDebug>

using namespace Eigen;

void derivENCKE (VectorXd state, double time, VectorXd parameters, VectorXd& derivative)
{
    double qf = parameters(6);
    double mu = parameters(7);
    Vector3d deviation;
        deviation << state(0), state(1), state(2);
    Vector3d reference;
        reference << parameters(0), parameters(1), parameters(2);

    derivative(0) = state(3);
    derivative(1) = state(4);
    derivative(2) = state(5);
    derivative(3) = mu / pow((reference.norm()), 3.0) * ((reference.x() + deviation.x()) * qf - deviation.x()) + parameters(3);
    derivative(4) = mu / pow((reference.norm()), 3.0) * ((reference.y() + deviation.y()) * qf - deviation.y()) + parameters(4);
    derivative(5) = mu / pow((reference.norm()), 3.0) * ((reference.z() + deviation.z()) * qf - deviation.z()) + parameters(5);
}



sta::StateVector
propagateENCKE(double mu,
               const sta::StateVector& reference,
               double deltat,
               const QList<Perturbations*>& listPerturbations,
               double time,
               const sta::StateVector& state,
               const sta::StateVector& deviation,
               double q,
               const QString& integrator,
               PropagationFeedback& propFeedback )
{
    
    //Calculating the perturbing accelerations
    Vector3d perturbingAcceleration(0,0,0);
    foreach (Perturbations* perturbation, listPerturbations)
    {
        perturbingAcceleration += perturbation->calculateAcceleration(state, time, deltat);
    }

    //Integrating
    double qf = 2*q / (1 + 2*q) * (1 + 1/(1 + 2*q + sqrt(1 + 2*q)));
    VectorXd parameters(8);
            parameters << reference.position, perturbingAcceleration, qf, mu;
    VectorXd dev(6);
            dev << deviation.position, deviation.velocity;

    rk4 (dev, 6, 0, deltat, derivENCKE, parameters);

    return sta::StateVector(dev.segment<3>(0), dev.segment<3>(3));
    //deviation.position << dev(0),dev(1),dev(2);
    //deviation.velocity << dev(3),dev(4),dev(5);
}

