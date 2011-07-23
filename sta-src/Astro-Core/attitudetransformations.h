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
// Patched by Guillermo to correct error, July 2011

#ifndef ATTITUDETRANSFORMATIONS_H
#define ATTITUDETRANSFORMATIONS_H

#include <Eigen/Core>
#include <Eigen/Geometry>


using namespace Eigen;


Quaterniond ToQuaternions(const Vector3d EulerAngles,
                                 int seq1,
                                 int seq2,
                                 int seq3);

Vector3d ToEulerAngles(Quaterniond quaternion,
                              int seq1,
                              int seq2,
                              int seq3);

Vector3d ToEulerAngleRates(const Vector3d angVel,
                           const Vector3d EulerAngles,
                           int seq1,
                           int seq2,
                           int seq3);

Vector3d ToAngularVelocity(const Vector3d EulerRates,
                           const Vector3d EulerAngles,
                           int seq1,
                           int seq2,
                           int seq3);

Vector3d ToAngularVelocity(Eigen::Quaterniond quaternion,
                           Eigen::Quaterniond initQuatRates);

Quaterniond ToQuaternionRates(Eigen::Quaterniond quaternion,
                              Vector3d bodyRates);


#endif // ATTITUDETRANSFORMATIONS_H
