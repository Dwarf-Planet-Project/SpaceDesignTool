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

#include "UniformRotationState.h"
#include "stamath.h"

using namespace sta;
using namespace Eigen;


UniformRotationState::UniformRotationState(double poleRa, double poleDec, double W0, double rate) :
    m_poleRa(poleRa),
    m_poleDec(poleDec),
    m_W0(W0),
    m_rotationRate(rate)
{
    m_poleRotation = Quaterniond(AngleAxis<double>(halfPi() + poleRa, Vector3d::UnitZ())) *
                     Quaterniond(AngleAxis<double>(halfPi() - poleDec, Vector3d::UnitX()));
    m_pole = m_poleRotation * Vector3d::UnitZ();
}

Quaterniond
UniformRotationState::orientation(double tjd) const
{
    return m_poleRotation * Quaterniond(AngleAxis<double>(m_rotationRate * tjd + m_W0, Vector3d::UnitZ()));
}

Vector3d
UniformRotationState::angularVelocity(double /* tjd */) const
{
    return m_pole * m_rotationRate;
}
