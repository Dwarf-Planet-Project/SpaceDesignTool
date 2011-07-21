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
 ------ Copyright (C) 2011 STA Steering Board (space.trajectory.analysis AT gmail.com) ----
*/

//------------------ Author: Catarina Silva  -------------------------------------------------
// ------------------ E-mail: (catsilva20@gmail.com) ------------------------------------------

#ifndef ATTITUDEINTEGRATION_H
#define ATTITUDEINTEGRATION_H

#include <Eigen/Core>
#include <Eigen/Geometry>
using namespace Eigen;

typedef Eigen::Matrix< double, 3, 3 > 	MyMatrix3d;
typedef Eigen::Matrix< double, 3, 1 >   MyVector3d;
typedef Eigen::Matrix< double,  4, 1 >   MyVector4d;


void derivQUATERNIONS (VectorXd initQuaternions,
                       double time,
                       VectorXd parameters,
                       VectorXd& derivative);

Quaterniond propagateQUATERNIONS(Quaterniond& quaternions,
                                 double time,
                                 double timeStep,
                                 Vector3d bodyrates);

void derivEulerEquation (VectorXd bodyRates,
                         double time,
                         VectorXd inertiaANDmoments,
                         VectorXd& derivBodyRates);

void eulerEquation(VectorXd &initBodyRates,
                   MatrixXd &inertialMatrix,
                   VectorXd &finalBodyRates);

Vector3d propagateEulerEquation(Vector3d& initbodyRates,
                                double time,
                                double timeStep,
                                MatrixXd inertiaMatrix);


#endif // ATTITUDEINTEGRATION_H
