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

// import most common Eigen types
//USING_PART_OF_NAMESPACE_EIGEN
typedef Eigen::Matrix< double, 3, 3 > 	MyMatrix3d;
typedef Eigen::Matrix< double, 3, 1 >   MyVector3d;
typedef Eigen::Matrix< double,  4, 1 >   MyVector4d;

class AttitudeIntegration
{
public:
    AttitudeIntegration();
    void derivQUATERNIONS (VectorXd initQuaternions, double time, VectorXd parameters, VectorXd& derivative);
    MyVector4d propagateQUATERNIONS(MyVector4d& quaternions,
                                     double time,
                                     double timeStep,
                                     MyVector3d bodyrates);
    void derivEulerEquation (VectorXd bodyRates, double time, MatrixXd inertiaMatrix, VectorXd& derivBodyRates);
    void eulerEquation(MyVector3d &initBodyRates, MyMatrix3d &inertialMatrix, MyVector3d &finalBodyRates);
    MyVector3d propagateEulerEquation(MyVector3d& initbodyRates,
                                      double time,
                                      double timeStep,
                                      MyMatrix3d inertiaMatrix);
};

#endif // ATTITUDEINTEGRATION_H
