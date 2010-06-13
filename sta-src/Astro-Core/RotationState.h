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
