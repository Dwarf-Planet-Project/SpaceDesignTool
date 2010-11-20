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
------------------ Author:       Marnix Volckaert               -------------------
------------------ Affiliation:  TUDelft                        -------------------
-----------------------------------------------------------------------------------
 Modified by Tiziana Sabatini on May-July 2009
*/

#include <math.h>
#include "BodyREM.h"
#include <QDebug>
#include "Astro-Core/stamath.h"

using namespace std;

double BodyREM::altitude (double r, double delta)
{
    double f, rs;

    f = (equatorial_radius - polar_radius) / equatorial_radius;
    rs = equatorial_radius * sqrt( pow((1-f),2) / (1- f*(2-f) * pow(cos(delta),2)) );

    return (r - rs);
}


double BodyREM::altitude2r (double h, double delta)
{
    double f, rs;

    f = (equatorial_radius - polar_radius) / equatorial_radius;
    rs = equatorial_radius * sqrt( pow((1-f),2) / (1- f*(2-f) * pow(cos(delta),2)) );

    return (h + rs);
}


double BodyREM::gr (double r, double delta)
{
    return (mu/pow(r,2));
    //return ( (mu/pow(r,2))*(1.0 - (3.0/2.0)*J2*pow((equatorial_radius/r),2)*(3.0*pow(sin(delta),2) - 1.0) - 2.0*J3*pow((equatorial_radius/r),3)*sin(delta)*(5.0*pow(sin(delta),2) - 3.0) - (5.0/8.0)*J4*pow((equatorial_radius/r),4)*(35.0*pow(sin(delta),4) - 30.0*pow(sin(delta),2) + 3.0)) );
}


double BodyREM::gdelta (double r, double delta)
{
    return 0;
    //return ( -3.0*(mu/pow(r,2))*pow((equatorial_radius/r),2)*sin(delta)*cos(delta) * (J2 + (1.0/2.0*sin(delta))*J3*(equatorial_radius/r)*(5.0*pow(sin(delta),2) - 1.0) + (5.0/6.0)*J4*pow((equatorial_radius/r),2)*(7.0*pow(sin(delta),2) - 3.0)) );
}


void BodyREM::updateGravity (sta::StateVector stateVector, Vector3d& gravity)
{
    double r, p, delta;
    r = sqrt(pow(stateVector.position.x(),2) + pow(stateVector.position.y(),2) + pow(stateVector.position.z(),2));
    p = sqrt(pow(stateVector.position.x(),2) + pow(stateVector.position.y(),2));
    delta = atan2(stateVector.position.z(), sqrt(pow(stateVector.position.x(),2) + pow(stateVector.position.y(),2)));
    //------ Components in x, y, z
    gravity(0) = (1.0/r) * ( -stateVector.position.x()*gr(r,delta) - (stateVector.position.x()*stateVector.position.z()/p)*gdelta(r,delta) );
    gravity(1) = (1.0/r) * ( -stateVector.position.y()*gr(r,delta) - (stateVector.position.y()*stateVector.position.z()/p)*gdelta(r,delta) );
    gravity(2) = (1.0/r) * ( -stateVector.position.z()*gr(r,delta) + p*gdelta(r,delta) );
}


//The input parameters.uncertainties have been deleted to not generate errors: insert it again
//when adding the dipersion functionality!!
void BodyREM::updateAero (sta::StateVector stateVector, VectorXd& state2, AtmosphereModel atmosphere, capsule_class capsule, Vector3d& aero, double& Cdc)
{
    double r, V, delta,local_altitude, tau, rho, M,gamma,chi, Cdp, Clc, Csc;

    r = sqrt(pow(stateVector.position.x(),2) + pow(stateVector.position.y(),2) + pow(stateVector.position.z(),2));
    V = sqrt(pow(stateVector.velocity.x(),2) + pow(stateVector.velocity.y(),2) + pow(stateVector.velocity.z(),2));
    delta = atan2(stateVector.position.z(), sqrt(pow(stateVector.position.x(),2) + pow(stateVector.position.y(),2)));
    tau = atan2(stateVector.position.y(),stateVector.position.x());
    Vector3d VelocityR = stateVector.velocity;
    Matrix3d Cvr;
    Cvr=Eigen::AngleAxisd(delta+sta::Pi()/2, Vector3d::UnitY())*Eigen::AngleAxisd(-tau, Vector3d::UnitZ());
    Matrix3d Crv;
    Crv=Eigen::AngleAxisd(tau, Vector3d::UnitZ())*Eigen::AngleAxisd(-delta-sta::Pi()/2, Vector3d::UnitY());
    Matrix3d Cvt;
    Vector3d VelocityV =Cvr*VelocityR;
    chi=atan2(VelocityV.y(),VelocityV.x());
    gamma=-atan2(VelocityV.z(),sqrt(pow(VelocityV.x(),2)+pow(VelocityV.y(),2)));
    Cvt=Eigen::AngleAxisd(chi, Vector3d::UnitZ())*Eigen::AngleAxisd(gamma, Vector3d::UnitY());
    local_altitude = altitude(r, delta);
    M = V / atmosphere.speedofsound(local_altitude);
    //qDebug()<<M<<"Mach";
    rho = atmosphere.density(local_altitude);// * (1 + uncertainties.density);
    Cdc = capsule.cdc(M);// * (1 + uncertainties.cdcapsule);                             //This is where the capsule's cd value is updated, since it will be used in the heat rate calculation
    Clc = capsule.clc(M);
    Csc = capsule.csc(M);

    //------
    if (state2[7] < 0.5) {
        Cdp = 0.0;
    }

    else {
        Cdp = capsule.cdp(M);// * (1 + uncertainties.cdparachute);
    }
    Vector3d FA=-0.5*rho*pow(V,2.0)*capsule.Sc*Vector3d(Cdc,Csc,Clc);
    Vector3d aR;
    aR=Crv*Cvt*FA/capsule.m;
    //qDebug()<<Crv(0,0)<<Crv(0,1)<<Crv(0,2)<<Crv(1,0)<<Crv(1,1)<<Crv(1,2)<<Crv(2,0)<<Crv(2,1)<<Crv(2,2)<<"Crv";
    //qDebug()<<Cvt(0,0)<<Cvt(0,1)<<Cvt(0,2)<<Cvt(1,0)<<Cvt(1,1)<<Cvt(1,2)<<Cvt(2,0)<<Cvt(2,1)<<Cvt(2,2)<<"Cvt";

    //Vector
    //------
    //qDebug()<<chi*180/sta::Pi()<<gamma*180/sta::Pi()<<tau*180/sta::Pi()<<delta*180/sta::Pi()<<VelocityV.x()<<VelocityV.y()<<VelocityV.z()<<VelocityR.x()<<VelocityR.y()<<VelocityR.z();
    aero[0] = aR.x();// - 0.5 * rho * V * stateVector.velocity.x() * (Cdc*capsule.Sc + Cdp*capsule.Sp) / capsule.m;
    aero[1] = aR.y();//- 0.5 * rho * V * stateVector.velocity.y() * (Cdc*capsule.Sc + Cdp*capsule.Sp) / capsule.m;
    aero[2] = aR.z();//- 0.5 * rho * V * stateVector.velocity.z() * (Cdc*capsule.Sc + Cdp*capsule.Sp) / capsule.m;
    //qDebug()<<aero[0]<<aero[1]<<aero[2];
    //qDebug()<<aR.x()<<aR.y()<<aR.z()<<" accels old new";
    //qDebug()<<Cdc<<Clc<<Csc;
    //------ Calculate information in state2 ------
    state2[0] = rho;
    state2[1] = M;
    state2[5] = local_altitude;
    state2[6] = V;

}

