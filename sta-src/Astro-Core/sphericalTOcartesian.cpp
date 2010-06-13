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


// Author G. Ortega

#include "sphericalTOcartesian.h"
#include "Scenario/staschema.h"
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

sta::StateVector sphericalTOcartesian(double spherCoord[6])//Added by Dominic

{
    double delta = spherCoord[2];
    double tau = spherCoord[1];
    double r = spherCoord[0];
    double gamma = spherCoord[4];
    double chi = spherCoord[5];
    double V = spherCoord[3];
    sta::StateVector cartesian;
    cartesian.position.x() = r * cos(delta) * cos(tau);
    cartesian.position.y() = r * cos(delta) * sin(tau);
    cartesian.position.z() = r * sin(delta);
    cartesian.velocity.x() = V * ( sin(gamma)*cos(delta)*cos(tau) - cos(gamma)*cos(chi)*sin(delta)*cos(tau) - cos(gamma)*sin(chi)*sin(tau) );
    cartesian.velocity.y() = V * ( sin(gamma)*cos(delta)*sin(tau) - cos(gamma)*cos(chi)*sin(delta)*sin(tau) + cos(gamma)*sin(chi)*cos(tau) );
    cartesian.velocity.z() = V * ( cos(gamma)*cos(chi)*cos(delta) + sin(gamma)*sin(delta) );
 //   x = cos(theta)*x + sin(theta)*y;
 //   y = -sin(theta)*x + cos(theta)*y;

    //------ Transform velocity to rotating frame by subtracting r x omega
    //double rxomega_x = -y * mu;
    //double rxomega_y =  x * mu;
    //xd -= rxomega_x;
    //yd -= rxomega_y;
    return cartesian;
}
