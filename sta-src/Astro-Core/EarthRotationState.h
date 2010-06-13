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
