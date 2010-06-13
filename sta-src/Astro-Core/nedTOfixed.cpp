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
 ------------------ Author: Claas Grohnfeldt, Steffen Peter  -------------------------------------------------
 ------------------ E-mail:                                  ----------------------------
 */

#include "nedTOfixed.h"

/**
  * function calculates the transformation of a vector from North-East-Down geodetic coordinates
  * to central body centered and fixed coordinates
  * don't care about north and south pole
  **/

void nedTOfixed(double elevation, double azimuth, Eigen::Vector3d input, Eigen::Vector3d& result)
{
    double cosel = cos(elevation);
    double sinel = sin(elevation);
    double cosaz = cos(azimuth);
    double sinaz = sin(azimuth);

    result = Eigen::Vector3d(-cosaz*sinel*input.x() - sinaz*input.y() - cosaz*cosel*input.z(),
                             -sinaz*sinel*input.x() + cosaz*input.y() - sinaz*cosel*input.z(),
                             cosel*input.x() - sinel*input.z());
}
