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
 ------------------ Author: Chris Laurel  -------------------------------------------------
 ------------------ E-mail: (claurel@gmail.com) ----------------------------
 */

#include "EarthRotationState.h"
#include "getGreenwichHourAngle.h"
#include "stamath.h"
#include "date.h"

using namespace sta;
using namespace Eigen;


// Mean sidereal day: 23h 56m 4.1s
const double EarthRotationState::MEAN_SIDEREAL_DAY_LENGTH = 23.934472 / 24.0;


EarthRotationState::EarthRotationState()
{
}


Quaterniond
EarthRotationState::orientation(double tjd) const
{
    // TODO: nutation and precession are not yet accounted for

    double greenwichAngle = getGreenwichHourAngle(sta::MjdToJd(tjd));

    return Quaterniond(AngleAxis<double>(greenwichAngle, Vector3d::UnitZ()));
}


Vector3d
EarthRotationState::angularVelocity(double /* tjd */) const
{
    // TODO: only an approximation
    //       No nutation or precession
    //       Also, no accurate time derivative of Greenwich hour angle
    //return Vector3d::UnitZ() * (1.0 / MEAN_SIDEREAL_DAY_LENGTH);
    return Vector3d::UnitZ() * 7.2921159e-5;

}
