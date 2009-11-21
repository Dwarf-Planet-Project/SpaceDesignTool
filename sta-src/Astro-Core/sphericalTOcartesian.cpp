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

// Author G. Ortega

#include "sphericalTOcartesian.h"
#include<float.h>
#include<math.h>
#include<stdio.h>

void sphericalTOcartesian(double tau,double delta,double r,double V,double gamma,double chi,
                                                double& x,double& y,double& z,double& xd,double& yd,double& zd)

{
    x = r * cos(delta) * cos(tau);
    y = r * cos(delta) * sin(tau);
    z = r * sin(delta);
    xd = V * ( sin(gamma)*cos(delta)*cos(tau) - cos(gamma)*cos(chi)*sin(delta)*cos(tau) - cos(gamma)*sin(chi)*sin(tau) );
    yd = V * ( sin(gamma)*cos(delta)*sin(tau) - cos(gamma)*cos(chi)*sin(delta)*sin(tau) + cos(gamma)*sin(chi)*cos(tau) );
    zd = V * ( cos(gamma)*cos(chi)*cos(delta) + sin(gamma)*sin(delta) );
 //   x = cos(theta)*x + sin(theta)*y;
 //   y = -sin(theta)*x + cos(theta)*y;

    //------ Transform velocity to rotating frame by subtracting r x omega
    //double rxomega_x = -y * mu;
    //double rxomega_y =  x * mu;
    //xd -= rxomega_x;
    //yd -= rxomega_y;

}
