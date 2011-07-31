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
 ------------------ Author:  Annalisa Riccardi   -----------------------------------------
 ------------------ E-mail: (nina1983@gmail.com) -----------------------------------------
 Modified by Tiziana Sabatini on July 2009
 Modified by J.Alonso on July 2011
 */
#include<Eigen/Core>
#include<Eigen/LU>
#include<QDebug>
#include "eci2lhlv.h"

USING_PART_OF_NAMESPACE_EIGEN;


void inertialTOlocal(double xInertial, double yInertial, double zInertial, double i, double Omega, double t, double w,double& xlocal, double& ylocal, double& zlocal)
{
        // phase angle
        double phi = w+t;

        Matrix3d conversion;

         conversion <<   cos(phi)*cos(Omega) - sin(Omega)*cos(i)*sin(phi),   cos(phi)*sin(Omega) + cos(Omega)*cos(i)*sin(phi), sin(i)*sin(phi),
                      - sin(phi)*cos(Omega) - sin(Omega)*cos(i)*cos(phi), - sin(phi)*sin(Omega) + cos(Omega)*cos(i)*cos(phi), sin(i)*cos(phi),
                        sin(i)*sin(Omega),  - sin(i)*cos(Omega),  cos(i);

        Vector3d local, inertial;
        inertial << xInertial, yInertial, zInertial;
        local = conversion * inertial;
        xlocal = local.x();
        ylocal = local.y();
        zlocal = local.z();

}

//Alonso add-ins for RendezVous

void localTOinertial(double xLocal, double yLocal, double zLocal, double i, double Omega, double t, double w,double& xInertial, double& yInertial, double& zInertial)
{
        // phase angle
        double phi = w+t;

        Matrix3d conversion;

         conversion <<   cos(phi)*cos(Omega) - sin(Omega)*cos(i)*sin(phi),   cos(phi)*sin(Omega) + cos(Omega)*cos(i)*sin(phi), sin(i)*sin(phi),
                      - sin(phi)*cos(Omega) - sin(Omega)*cos(i)*cos(phi), - sin(phi)*sin(Omega) + cos(Omega)*cos(i)*cos(phi), sin(i)*cos(phi),
                        sin(i)*sin(Omega),  - sin(i)*cos(Omega),  cos(i);

        Vector3d local, inertial;

        local << xLocal,yLocal,zLocal;

        inertial=conversion.inverse()*local;

        xInertial = inertial.x();
        yInertial = inertial.y();
        zInertial = inertial.z();


}


void lhlvTOlocal(double xLHLV,double yLHLV,double zLHLV, double& xLocal,double& yLocal,double& zLocal){

    xLocal=-zLHLV;
    yLocal=xLHLV;
    zLocal=-yLHLV;

}

void localTOlhlv(double xLocal,double yLocal,double zLocal, double& xLHLV,double& yLHLV,double& zLHLV){

    xLHLV=yLocal;
    yLHLV=-zLocal;
    zLHLV=-xLocal;

}

void inertialTOlhlv(double mu,sta::StateVector relativeReference,sta::StateVector transform,sta::StateVector* result){


    //Only for position

    sta::KeplerianElements e =cartesianTOorbital(mu,relativeReference);

    inertialTOlocal(transform.position(0),transform.position(1),transform.position(2),
                    e.Inclination, e.AscendingNode,
                    e.TrueAnomaly,e.ArgumentOfPeriapsis,
                    result->position(0),result->position(1),result->position(2));

    localTOlhlv(result->position(0),result->position(1),result->position(2),transform.position(0),transform.position(1),transform.position(2));

    result->position(0)=transform.position(0);
    result->position(1)=transform.position(1);
    result->position(2)=transform.position(2);


    inertialTOlocal(transform.velocity(0),transform.velocity(1),transform.velocity(2),
                    e.Inclination, e.AscendingNode,
                    e.TrueAnomaly,e.ArgumentOfPeriapsis,
                    result->velocity(0),result->velocity(1),result->velocity(2));

    localTOlhlv(result->velocity(0),result->velocity(1),result->velocity(2),transform.velocity(0),transform.velocity(1),transform.velocity(2));

    result->velocity(0)=transform.velocity(0);
    result->velocity(1)=transform.velocity(1);
    result->velocity(2)=transform.velocity(2);


}


void lhlvTOinertial(double mu,sta::StateVector relativeReference,sta::StateVector transform,sta::StateVector* result){

    sta::KeplerianElements e =cartesianTOorbital(mu,relativeReference);

    lhlvTOlocal(transform.position(0),transform.position(1),
                transform.position(2),result->position(0),result->position(1),result->position(2));

    localTOinertial(result->position(0),result->position(1),result->position(2),
                    e.Inclination, e.AscendingNode,
                    e.TrueAnomaly,e.ArgumentOfPeriapsis,
                    transform.position(0),transform.position(1),transform.position(2));

    result->position(0)=transform.position(0);
    result->position(1)=transform.position(1);
    result->position(2)=transform.position(2);

    lhlvTOlocal(transform.velocity(0),transform.velocity(1),
                transform.velocity(2),result->velocity(0),result->velocity(1),result->velocity(2));

    localTOinertial(result->velocity(0),result->velocity(1),result->velocity(2),
                    e.Inclination, e.AscendingNode,
                    e.TrueAnomaly,e.ArgumentOfPeriapsis,
                    transform.velocity(0), transform.velocity(1), transform.velocity(2));

    result->velocity(0)=transform.velocity(0);
    result->velocity(1)=transform.velocity(1);
    result->velocity(2)=transform.velocity(2);


}
