/* 
 This program is free software; you can redistribute it and/or modify it under
 the terms of the GNU Lesser General Public License as published by the Free Software
 Foundation; either version 2 of the License, or (at your option) any later
 version.
 
 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more
 details.
 
 You should have received a copy of the GNU Lesser General Public License along with
 this program; if not, write to the Free Software Foundation, Inc., 59 Temple
 Place - Suite 330, Boston, MA 02111-1307, USA.
 Further information about the GNU Lesser General Public License can also be found on
 the world wide web at http://www.gnu.org. 
 */

/*
 ------ Copyright (C) 2008 European Space Agency (space.trajectory.analysis AT gmail.com) ----
 ------------------ Author: Chris Laurel  -------------------------------------------------
 ------------------ E-mail: (claurel@gmail.com) ----------------------------
 //Modified by Valentino Zuccarelli on 11th June 2009
 */

#include "scenarioabstracttrajectory.h"
#include <Astro-Core/date.h>


ScenarioAbstractTrajectory::ScenarioAbstractTrajectory()
{
}


ScenarioAbstractTrajectory::~ScenarioAbstractTrajectory()
{
}


/*! Get the origin of the trajectory.
 */
const StaBody* 
ScenarioAbstractTrajectory::centralBody() const
{
    return STA_SOLAR_SYSTEM->lookup(STA_EARTH);
}

const StaBody*
ScenarioAbstractTrajectory::firstBody() const
{
    return STA_SOLAR_SYSTEM->lookup(STA_EARTH);
}

const StaBody*
ScenarioAbstractTrajectory::secondBody() const
{
    return STA_SOLAR_SYSTEM->lookup(STA_EARTH);
}


/*! Get the coordinate system in which this trajectory is defined.
 */
const sta::CoordinateSystem
ScenarioAbstractTrajectory::coordinateSystem() const
{
    return sta::CoordinateSystem(sta::COORDSYS_EME_J2000);
}
