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

#ifndef _ASTROCORE_ROTATIONSTATE_H_
#define _ASTROCORE_ROTATIONSTATE_H_

#include <Eigen/Core>
#include <Eigen/Geometry>

namespace sta
{

class RotationState
{
public:
    virtual ~RotationState() {}

    /** Get the orientation of the object in the EME_2000 coordinate system
      * at the specified time.
      */
    virtual Eigen::Quaterniond orientation(double tjd) const = 0;

    /** Get the instantaneous angular velocity vector. The magnitude of the
      * vector is the rotation rate in radians per Julian day.
      */
    virtual Eigen::Vector3d angularVelocity(double tjd) const = 0;

    /** Return true if the rotation is approximately periodic.
      */
    virtual bool isPeriodic() const
    {
        return false;
    }

    /** Get the rotation period in Julian days.
      */
    virtual double rotationPeriod() const
    {
        return 0.0;
    }
};

}

#endif // _ASTROCORE_ROTATIONSTATE_H_
