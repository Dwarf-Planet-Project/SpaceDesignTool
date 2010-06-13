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

#include "propagateGAUSS.h"
#include <iostream>
#include <iomanip>
#include <fstream>
#include <math.h>
#include <QDebug>
#include "cartesianTOorbital.h"
#include "orbitalTOcartesian.h"
#include "eci2lhlv.h"
#include "EODE/eode.h"
#include "math.h"
#include "perturbations.h"


void derivGAUSS (VectorXd state, double time, VectorXd parameters, VectorXd& derivative)
{
    //COE classical orbital elements
    if (parameters(6) == 0)
    {
        double theta = parameters(3);

        double E = parameters(4);
        double mu = parameters(5);
        double p = state(0) * (1 - pow(state(1),2));
        double r = state(0) * (1 - pow(state(1),2)) / (1 + state(1)*cos(theta));
        double u = theta + state(3);
        double n = sqrt(mu / pow(state(0),3));

        //derivative of semimajor axis
        derivative(0) = 2 * pow(state(0),2.0) / (pow((mu * p),0.5)) * (parameters(0)*state(1)*sin(theta) + parameters(1)*p/r);

        //derivative of eccentricity
        derivative(1) = (pow(p/mu, 0.5)) * (parameters(0)*sin(theta) + parameters(1)*(cos(E) + cos(theta)));

        //derivative of inclination
        derivative(2) = parameters(2) * r / sqrt(mu*p) * cos(u);

        //derivative of argument of periapsis
        derivative(3) = - sqrt(p/mu) * (parameters(2) * r/p/tan(state(2)) * sin(u) +
                        1/state(1) * (parameters(0)*cos(theta) - parameters(1)*(1+r/p)*sin(theta)));

        //derivative of ascending node
        derivative(4) = parameters(2) * r / (sqrt(mu*p)*sin(state(2))) * sin(u) ;

        //derivative of mean anomaly
        derivative(5) = n - parameters(0) * (2*r/sqrt(mu*state(0)) - (1 - pow(state(1),2))/state(1) * sqrt(state(0)/mu) * cos(theta)) -
                        parameters(1) * (1-pow(state(1),2))/state(1) * sqrt(state(0)/mu) * (1 + r/p) * sin(theta);

    }

    //EOE equinoctial orbital elements
    else if (parameters(6) == 1)
    {
        //near-circular and/or near-equatorial orbits : equinoctial orbital elements
        double theta = parameters(3);
        double E = parameters(4);
        double mu = parameters(5);
        double e = parameters(8);
        double i = parameters(9);
        double Omega = parameters(10);
        double omega = parameters(11);

        double n = sqrt(mu / pow(state(0),3));
        double b = state(0) * sqrt(1 - pow(state(1),2.0) - pow(state(2),2.0));
        double h = n * state(0) * b;
        double L = Omega + omega + theta;
        double pr = 1 + state(1)*sin(L) * state(2)*cos(L);
        double r = state(0) * (1 - pow(e,2)) / (1 + e*cos(theta));

        //derivative of semimajor axis
        derivative(0) = 2 * pow(state(0),2.0) / h * ((state(2)*sin(L) - state(1)*cos(L)) * parameters(0) + parameters(1) * pr);

        //derivative of P1 = e * sin(Omega + omega)
        derivative(1) = r/h * (pr * cos(L) * parameters(0) + (state(1) + (1+pr) * sin(L)) * parameters(1) -
                        state(2) * (state(3) * cos(L) - state(4) * sin(L)) * parameters(2));

        //derivative of P2 = e * cos(Omega + omega)
        derivative(2) = r/h * (pr * sin(L) * parameters(0) + (state(2) + (1+pr) * cos(L)) * parameters(1) +
                        state(1) * (state(3) * cos(L) - state(4) * sin(L)) * parameters(2));

        //derivative of Q1 = tan(i/2) * sin(Omega)
        derivative(3) = 0.5*r/h * (1 + pow(state(3),2) + pow(state(4),2)) * sin(L) * parameters(2);

        //derivative of Q2 = tan(i/2) * cos(Omega)
        derivative(4) = 0.5*r/h * (1 + pow(state(3),2) + pow(state(4),2)) * cos(L) * parameters(2);

        //derivative of l = Omega + omega + M
        derivative(5) = n - r/h * (state(0)/(state(0) + b) * pr * (state(1) * sin(L) + state(2) * cos(L) + 2*b/state(0)) * parameters(0) +
                        state(0)/(state(0) + b) * (1+pr) * (state(1) * cos(L) + state(2) * sin(L)) * parameters(1) +
                        (state(3) * cos(L) - state(4) * sin(L)) * parameters(2));
    }
}


void derivGAUSS_RKF (double time, double state[], double parameters[], double derivative[])
{
    //Conversion of the double[] elements into Eigen vectors
    VectorXd state_v(6), parameters_v(8), derivative_v(6);
    state_v << state[0], state[1], state[2], state[3], state[4], state[5];
    parameters_v << parameters[0], parameters[1], parameters[2], parameters[3], parameters[4],
                    parameters[5], parameters[6], parameters[7];
    derivative_v << derivative[0], derivative[1], derivative[2], derivative[3], derivative[4], derivative[5];

    if (parameters[7] == 1)
    {
        parameters_v.resize(12);
        parameters_v << parameters[0], parameters[1], parameters[2], parameters[3], parameters[4], parameters[5],
                        parameters[6], parameters[7], parameters[8], parameters[9], parameters[10], parameters[11];
    }
    //The standard function for the calculation of the derivatives is called
    derivGAUSS (state_v, time, parameters_v, derivative_v);

    //Reconversion of the output of the function into double[] elements
    derivative[0] = derivative_v(0); derivative[1] = derivative_v(1); derivative[2] = derivative_v(2);
    derivative[3] = derivative_v(3); derivative[4] = derivative_v(4); derivative[5] = derivative_v(5);
}


sta::StateVector propagateGAUSS(double mu,
                                const sta::StateVector& initialstate,
                                double deltat,
                                QList<Perturbations*> listPerturbations,
                                double time,
                                const QString& integrator)
{
    //Converting to keplerian elements the cartesian state
    KeplerianElements keplerian = cartesianTOorbital(mu, initialstate);

    //Calculating the perturbing accelerations
    Vector3d perturbingAcceleration(0,0,0);

    foreach (Perturbations* perturbation, listPerturbations)
    {
        perturbingAcceleration += perturbation->calculateAcceleration(initialstate, time, deltat);
    }

    //Converting the perturbing accelerations into local orbit frame
    double fS, fN, fW;

    inertialTOlocal(perturbingAcceleration.x(), perturbingAcceleration.y(), perturbingAcceleration.z(),
                    keplerian.Inclination, keplerian.AscendingNode, keplerian.TrueAnomaly, keplerian.ArgumentOfPeriapsis, fS, fN, fW);

    //Integrating
    VectorXd parameters(8), keplerianvector(6);

    //COE classical orbital elements
    if (keplerian.Eccentricity > 0.00001 && keplerian.Inclination > 0.00001)
    {
        keplerianvector << keplerian.SemimajorAxis , keplerian.Eccentricity, keplerian.Inclination, keplerian.ArgumentOfPeriapsis, keplerian.AscendingNode, keplerian.MeanAnomaly;

        //Choosing the integrator
        if (integrator == "RK4")
        {
            parameters << fS, fN, fW, keplerian.TrueAnomaly, keplerian.EccentricAnomaly, mu, 0, 0;
            rk4 (keplerianvector, 6, 0, deltat, derivGAUSS, parameters);
        }

        else if (integrator == "RKF")
        {
            //RKF works with double[] and not Eigen::Vector: a boring redefinition of new double[] elements is necessary;
            //this could be improved by re-writing the RKF routine using Eigen.

            double param[8] = {fS, fN, fW, keplerian.TrueAnomaly, keplerian.EccentricAnomaly, mu, 0, 1};
            double deriv[6];
            double initial_conditions[6] = {keplerianvector(0), keplerianvector(1), keplerianvector(2),
                                        keplerianvector(3), keplerianvector(4), keplerianvector(5)};

            int flag = 1;
            double timefin = deltat, timein = 0, relerr = 1E-9;

            flag =  Runge_Kutta_Fehlberg(derivGAUSS_RKF, 6, initial_conditions, param, deriv, &timein, timefin, &relerr, 1E-9, flag );

            keplerianvector << initial_conditions[0], initial_conditions[1], initial_conditions[2],
                               initial_conditions[3], initial_conditions[4], initial_conditions[5];

        }
    }


    //EOE equinoctial orbital elements (Case of near circular and/or near equatorial orbits)
    else
    {
        parameters.resize(12);

        double P1 = keplerian.Eccentricity * sin(keplerian.AscendingNode + keplerian.ArgumentOfPeriapsis);
        double P2 = keplerian.Eccentricity * cos(keplerian.AscendingNode + keplerian.ArgumentOfPeriapsis);
        double Q1 = tan(0.5 * keplerian.Inclination) * sin(keplerian.AscendingNode);
        double Q2 = tan(0.5 * keplerian.Inclination) * cos(keplerian.AscendingNode);
        double l = keplerian.AscendingNode + keplerian.ArgumentOfPeriapsis + keplerian.MeanAnomaly;

        //Integrating
        keplerianvector << keplerian.SemimajorAxis, P1, P2, Q1, Q2, l;

        //Choosing the integrator
        if (integrator == "RK4")
        {
            parameters << fS, fN, fW, keplerian.TrueAnomaly, keplerian.EccentricAnomaly, mu, 1, 0,
                          keplerian.Eccentricity, keplerian.Inclination, keplerian.AscendingNode, keplerian.ArgumentOfPeriapsis;

            rk4 (keplerianvector, 6, 0, deltat, derivGAUSS, parameters);
        }
        else if (integrator == "RKF")
        {
            //RKF works with double[] and not Eigen::Vector: a boring redefinition of new double[] elements is necessary;
            //this could be improved by re-writing the RKF routine using Eigen.

            double param[12] = {fS, fN, fW, keplerian.TrueAnomaly, keplerian.EccentricAnomaly, mu, 1, 1,
                                keplerian.Eccentricity, keplerian.Inclination, keplerian.AscendingNode, keplerian.ArgumentOfPeriapsis};
            double deriv[6] = {0};
            double initial_conditions[6] = {keplerianvector(0), keplerianvector(1), keplerianvector(2),
                                        keplerianvector(3), keplerianvector(4), keplerianvector(5)};

            int flag = 1;
            double timefin = deltat, timein = 0, relerr = 1E-9;

            flag =  Runge_Kutta_Fehlberg(derivGAUSS_RKF, 6, initial_conditions, param, deriv, &timein, timefin, &relerr, 1E-9, flag );

            keplerianvector << initial_conditions[0], initial_conditions[1], initial_conditions[2],
                               initial_conditions[3], initial_conditions[4], initial_conditions[5];
        }

        //Converting to classical orbital elements
        double a = keplerianvector(0);
        double e = sqrt(pow(keplerianvector(1),2) + pow(keplerianvector(2),2));
        double i = 2 * atan(sqrt(pow(keplerianvector(3),2) + pow(keplerianvector(4),2)));
        double omega = atan2(keplerianvector(1),keplerianvector(2)) - atan2(keplerianvector(3),keplerianvector(4));

        if ((i == 0) || (e == 0))
            omega = keplerian.ArgumentOfPeriapsis; //check

        double Omega = atan2(keplerianvector(3), keplerianvector(4));
        double M = keplerianvector(5) - omega - Omega;

        keplerianvector << a, e, i, omega, Omega, M;

    }

    // Converting from keplerian elements to cartesian state
    return orbitalTOcartesian(mu, keplerianvector(0), keplerianvector(1), keplerianvector(2), keplerianvector(3), keplerianvector(4), keplerianvector(5));
}
