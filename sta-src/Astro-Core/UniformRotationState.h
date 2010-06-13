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

#ifndef _ASTROCORE_UNIFORMROTATIONSTATE_H_
#define _ASTROCORE_UNIFORMROTATIONSTATE_H_

#include "RotationState.h"
#include <Eigen/Core>
#include <Eigen/Geometry>

namespace sta
{

class UniformRotationState : public RotationState
{
public:
    EIGEN_MAKE_ALIGNED_OPERATOR_NEW

    /** Create a new uniform rotation model.
      * @param poleRa the right ascendion of the north pole (in radians)
      * @param poleDec the declination of the north pole (in radians)
      * @param W0 the angle of the prime meridian at epoch J2000.0
      * @param rate the rotation rate in radians per Julian day
      */
    UniformRotationState(double poleRa, double poleDec, double W0, double rate);

    virtual Eigen::Quaterniond orientation(double tjd) const;
    virtual Eigen::Vector3d angularVelocity(double tjd) const;

    virtual bool isPeriodic() const
    {
        return true;
    }

    virtual double rotationPeriod() const
    {
        return 1.0 / m_rotationRate;
    }

private:
    double m_poleRa;
    double m_poleDec;
    double m_W0;
    double m_rotationRate;
    Eigen::Quaterniond m_poleRotation;
    Eigen::Vector3d m_pole;
};

}

#endif // _ASTROCORE_UNIFORMROTATIONSTATE_H_
