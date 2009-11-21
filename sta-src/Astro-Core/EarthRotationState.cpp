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
    return Vector3d::UnitZ() * (1.0 / MEAN_SIDEREAL_DAY_LENGTH);
}
