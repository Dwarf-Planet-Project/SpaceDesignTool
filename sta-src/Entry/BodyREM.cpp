/*
 Copyright (C) 2007 European Space Agency (space.trajectory.analysis AT gmail.com) -

 This program is free software; you can redistribute it and/or modify it under
 the terms of the GNU Lesser General Public License as published by the Free Software
 Foundation; either version 2 of the License, or (at your option) any later
 version.

 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more
 details.

 You should have received a copy of the GNU Lesser General Public License along with
 this program; if not, write to the Free Software Foundation, Inc., 59 Temple
 Place - Suite 330, Boston, MA  02111-1307, USA.

 Further information about the GNU Lesser General Public License can also be found on
 the world wide web at http://www.gnu.org.
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
    double r, V, delta,local_altitude, rho, M, Cdp;

    r = sqrt(pow(stateVector.position.x(),2) + pow(stateVector.position.y(),2) + pow(stateVector.position.z(),2));
    V = sqrt(pow(stateVector.velocity.x(),2) + pow(stateVector.velocity.y(),2) + pow(stateVector.velocity.z(),2));
    delta = atan2(stateVector.position.z(), sqrt(pow(stateVector.position.x(),2) + pow(stateVector.position.y(),2)));
    local_altitude = altitude(r, delta);
    M = V / atmosphere.speedofsound(local_altitude);
    rho = atmosphere.density(local_altitude);// * (1 + uncertainties.density);
    Cdc = capsule.cdc(M);// * (1 + uncertainties.cdcapsule);                             //This is where the capsule's cd value is updated, since it will be used in the heat rate calculation
    //------
    if (state2[7] < 0.5) {
        Cdp = 0.0;
    }

    else {
        Cdp = capsule.cdp(M);// * (1 + uncertainties.cdparachute);
    }
    //------
    aero[0] = - 0.5 * rho * V * stateVector.velocity.x() * (Cdc*capsule.Sc + Cdp*capsule.Sp) / capsule.m;
    aero[1] = - 0.5 * rho * V * stateVector.velocity.y() * (Cdc*capsule.Sc + Cdp*capsule.Sp) / capsule.m;
    aero[2] = - 0.5 * rho * V * stateVector.velocity.z() * (Cdc*capsule.Sc + Cdp*capsule.Sp) / capsule.m;
    //------ Calculate information in state2 ------
    state2[0] = rho;
    state2[1] = M;
    state2[5] = local_altitude;
    state2[6] = V;

}

