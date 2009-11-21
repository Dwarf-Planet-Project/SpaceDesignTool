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

#ifndef _ASTROCORE_EARTHROTATIONSTATE_H_
#define _ASTROCORE_EARTHROTATIONSTATE_H_

#include "RotationState.h"
#include <Eigen/Core>
#include <Eigen/Geometry>

namespace sta
{

class EarthRotationState : public RotationState
{
public:
    /** Create a new rotation model for the Earth. This class should only
      * be instantiated in the STA solar system data module.
      */
    EarthRotationState();

    virtual Eigen::Quaterniond orientation(double tjd) const;
    virtual Eigen::Vector3d angularVelocity(double tjd) const;

    virtual bool isPeriodic() const
    {
        return true;
    }

    virtual double rotationPeriod() const
    {
        return MEAN_SIDEREAL_DAY_LENGTH;
    }

    static const double MEAN_SIDEREAL_DAY_LENGTH;
};

}

#endif // _ASTROCORE_EARTHROTATIONSTATE_H_
