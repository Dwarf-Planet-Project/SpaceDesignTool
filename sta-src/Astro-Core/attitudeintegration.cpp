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

#include "attitudeintegration.h"
#include <Eigen/Core>
#include <Eigen/Geometry>
#include <Astro-Core/EODE/eode.h>
#include <Astro-Core/statevector.h>

using namespace Eigen;

typedef Eigen::Matrix< double, 3, 3 > 	MyMatrix3d;
typedef Eigen::Matrix< double, 3, 1 > 	MyVector3d;
typedef Eigen::Matrix< double, 4, 4 > 	MyMatrix4d;
typedef Eigen::Matrix< double, 4, 1 > 	MyVector4d;

//Integrates the Euler equation and the quaternion rates equation.
//Inputs: initial body rates, inertia matrix and moments (now = 0)
//The propagation will use the Runge Kutta 4 integrator
//Outputs: final euler angles or quaternions

// WRITE THE REFERENCE OF THE FORMULAS

/**
  * Derivative of the Euler equation
  *
  * @param bodyRates        initial body rates
  * @param inertiaMatrix    inertia matrix
  *
  * @return derivBodyRates  body rates derivatives
  */
void derivEulerEquation (VectorXd bodyRates, double time, VectorXd inertiaANDmoments, VectorXd& derivBodyRates)
{
    double p = bodyRates[0];
    double q = bodyRates[1];
    double r = bodyRates[2];

    static double bodyCoeffs[9]={
        0,  -r,   q,
        r,   0,  -p,
       -q,   p,   0
    };
    static const MyMatrix3d bodyMatrix(bodyCoeffs);

    //the input of derivEulerEquation needs to be a VectorXd (in this case, 12elements vector).
    //This vector will include the components of the INERTIA MATRIX (the first 9 elements), and
    //the components of the external MOMENTS applied to the vehicle (last 3 elements)
    double I11 = inertiaANDmoments[0];
    double I12 = inertiaANDmoments[1];
    double I13 = inertiaANDmoments[2];
    double I21 = inertiaANDmoments[3];
    double I22 = inertiaANDmoments[4];
    double I23 = inertiaANDmoments[5];
    double I31 = inertiaANDmoments[6];
    double I32 = inertiaANDmoments[7];
    double I33 = inertiaANDmoments[8];

    //External Moments
    double M1 = inertiaANDmoments[9];
    double M2 = inertiaANDmoments[10];
    double M3 = inertiaANDmoments[11];

    MyVector3d Moments(M1,M2,M3);

    static double inertiaCoeffs[9] = {
        I11, I12, I13,
        I21, I22, I23,
        I31, I32, I33

    };
    static const MyMatrix3d inertiaMatrix(inertiaCoeffs);

    //Euler's equation
    MyMatrix3d multipInertia= inertiaMatrix * inertiaMatrix.transpose();
    MyMatrix3d inertiaTimesBodyMatrix = bodyMatrix * multipInertia;

    derivBodyRates = Moments - inertiaTimesBodyMatrix * bodyRates;
}

/**
  * Propagate the Euler equation
  *
  * @param initbodyRates    initial body rates
  * @param time
  * @param timeStep
  * @param inertiaMatrix
  *
  * @return body rates      final body rates
  */
MyVector3d propagateEulerEquation(MyVector3d& initbodyRates,
                                  double time,
                                  double timeStep,
                                  MyMatrix3d inertiaMatrix)
{
    VectorXd bodyRates(3);
    bodyRates << initbodyRates[0], initbodyRates[1], initbodyRates[2];
    VectorXd parameters(9);
    parameters << inertiaMatrix[0], inertiaMatrix[1], inertiaMatrix[2],
                  inertiaMatrix[3], inertiaMatrix[4], inertiaMatrix[5],
                  inertiaMatrix[6], inertiaMatrix[7], inertiaMatrix[8];

    //what about the moments? Do they need to go to parameters?

    //For now, a Runge Kutta 4 will be used. Later, this can be changed to a better integrator (Runge Kutta 7,8 ex.)
    rk4(bodyRates, 3, time, timeStep, derivEulerEquation, parameters);

    //Returns the body rates, after integration
    return bodyRates;
}

/**
  * This function calculates the quaternions rates
  *
  * @param initQuaternions  initial quaternions
  * @param parameters       body rates
  * @param time
  *
  * @return derivative      quaternions derivatives
  */

//This function calculates the derivatives of the quaternions
void derivQUATERNIONS (VectorXd initQuaternions, double time, VectorXd parameters, VectorXd& derivative)
{
    double p = parameters[0];
    double q = parameters[1];
    double r = parameters[2];

    static double bodyCoeffs[16]= {
        0, -p, -q, -r,
        p, 0, r, -q,
        q, -r, 0, p,
        r, q, -p, 0
    };
    static const MyMatrix4d bodyRatesMatrix(bodyCoeffs);
    derivative = bodyRatesMatrix * initQuaternions;
}

/**
  * This function integrates the quaternions rates
  *
  * @param quaternions  initial quaternions
  * @param time         time
  * @param timeStep     time step of the integration
  * @param bodyrates
  *
  * @return quats       final quaternions
  */

MyVector4d propagateQUATERNIONS(MyVector4d& quaternions,
                                 double time,
                                 double timeStep,
                                 MyVector3d bodyrates)
{
    VectorXd parameters(3);
    parameters << bodyrates[0], bodyrates[1], bodyrates[2];

    VectorXd quats(4);
    quats << quaternions[0],quaternions[1],quaternions[2],quaternions[3];

    rk4 (quats, 4, time, timeStep, derivQUATERNIONS, parameters);

    return quats;
}


