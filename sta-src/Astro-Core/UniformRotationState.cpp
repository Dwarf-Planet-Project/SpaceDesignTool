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
