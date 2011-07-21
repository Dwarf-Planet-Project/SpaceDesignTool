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

#ifndef ATTITUDEVECTOR_H
#define ATTITUDEVECTOR_H

#include <Eigen/Core>
#include <Eigen/Geometry>

typedef Eigen::Matrix< double, 3, 1 > 	MyVector3d;
using namespace Eigen;

namespace staAttitude
{

    /*
     Quaternions are an alternative representation to the Euler angles. They define a rotation from
     one frame to another by a single rotation about an eigenvector, with a rotation angle *theta*.
     One of the main advantages of using quaternions is that there is no singularity in the equations,
     as in the case of Euler angles. Furthermore, the linear equation that needs to be integrated is l
     less computational expensive than the derived for the Euler angles.
     */

    /*
     The structure *QuaternionElements* is defined by the quaternions and the body rates. The reason why
     the body rates (p,q,r) are being used, instead of quaternion rates, is that for the integration of
     the attitude these rates are necessary in the Euler equation. The quaternion rates will be integrated
     for each time step.
     The initial quaternion rates are obtained from the body rates as follows:
     (q1Dot) = 1/2*[  0   r  -q    p] (q1)
     (q2Dot)       [ -r   0   p    q] (q2)
     (q3Dot)       [  q  -p   0    r] (q3)
     (q4Dot)       [ -p  -q  -r    0] (q4)
     */
    struct QuaternionElements
    {
        QuaternionElements():
                q1(1.0),
                q2(0.0),
                q3(0.0),
                q4(0.0),
                p(0.0),
                q(0.0),
                r(0.0)
        {
        }
        double q1, q2, q3, q4, p,q,r;
    };
    /*
     The Euler angles represent a rotation about a coordinate axis. The transformation from one frame to
     another is made through a series of rotations about the frame axis. Therefore a sequence os rotation
     is composed by three rotation, givin a total of twelve possible rotations for the transformation
     between frames. These rotations are characterised by the Euler angles, where the first rotation is made
     through an angle of rotation *phi*, the second through an angle *theta* and the third with an angle *psi*.
     This nomenclature is independent of the sequence order, meaning that *phi*, *theta* and *psi*
     will always name the first, second and third angle of rotation, respectively.
     */

    struct EulerElements
    {
        EulerElements():
                phi(0.0),
                theta(0.0),
                psi(0.0),
                p(0.0),
                q(0.0),
                r(0.0)
        {
        }
        double phi;
        double theta;
        double psi;
        double p;
        double q;
        double r;
    };



    class AttitudeVector
    {
    public:
        EIGEN_MAKE_ALIGNED_OPERATOR_NEW

                AttitudeVector() {}
        AttitudeVector(const Eigen::Quaterniond& _myQuaternion, const Eigen::Vector3d& _myBodyRates) :
                myQuaternion(_myQuaternion), myBodyRates(_myBodyRates) {}

    public:
        Eigen::Quaterniond myQuaternion;
        Eigen::Vector3d myBodyRates;
    };


}
#endif // ATTITUDEVECTOR_H
