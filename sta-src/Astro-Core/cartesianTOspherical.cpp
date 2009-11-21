/*

 This program is free software; you can redistribute it and/or modify it under
 the terms of the GNU General Public License as published by the Free Software
 Foundation; either version 2 of the License, or (at your option) any later
 version.

 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE. See the GNU General Public License for more
 details.

 You should have received a copy of the GNU General Public License along with
 this program; if not, write to the Free Software Foundation, Inc., 59 Temple
 Place - Suite 330, Boston, MA 02111-1307, USA.
 Further information about the GNU General Public License can also be found on
 the world wide web at http://www.gnu.org.
 */


/*
 --- Copyright (C) 2009 STA Steering Board (space.trajectory.analysis AT gmail.com) ---
*/

/*
 ------------------ Author: Tiziana Sabatini  -------------------------------------------------
 ------------------ E-mail: (tiziana.sabatini@yahoo.it) ----------------------------
 */

#include "cartesianTOspherical.h"
#include "inertialTOfixed.h"
#include "math.h"
#include<Eigen/Core>
#include <QErrorMessage>
#include <QDebug>


void cartesianTOspherical(double x,double y,double z,double xd,double yd,double zd,
                                                double& tau,double& delta,double& r,double& V,double& gamma,double& chi)

{
    double pi = 3.141592653589;
    r = sqrt(x*x + y*y + z*z);
    delta = asin(z / r);
    tau = acos(x / r / cos(delta));
    V = sqrt(xd*xd + yd*yd + zd*zd);

    gamma = pi/2 - acos((x*xd + y*yd + z*zd)/(r*V));
    chi = atan(r*(x*yd - y*xd)/(y*(y*zd - z*yd) - x*(z*xd - x*zd)));
    if (chi < 0)
        chi += pi;

}


double altitude(const StaBody* body, sta::StateVector s, double time)
{
    double f, rs, tau, delta, r, V, gamma, chi, state[5];
    int planet;

    f = (body->equatorialRadius() - body->polarRadius()) / body->equatorialRadius();

    if (body->name()=="Earth") planet = 0;
    else if (body->name()=="Mars") planet = 3;
    else qFatal("Error in the central body : select Earth or Mars");

    inertialTOfixed(planet, time, s.position.x(), s.position.y(), s.position.z(), s.velocity.x() ,s.velocity.y() , s.velocity.z() , state[0], state[1], state[2], state[3], state[4], state[5]);
    cartesianTOspherical(state[0], state[1], state[2], state[3], state[4], state[5], tau, delta, r, V, gamma, chi);
    rs = body->equatorialRadius() * sqrt( pow((1-f),2) / (1- f*(2-f) * pow(cos(delta),2)) );

    return (r - rs);
}
