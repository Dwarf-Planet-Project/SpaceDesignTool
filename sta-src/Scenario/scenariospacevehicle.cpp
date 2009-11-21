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
  // Patched by Guillermo June 14th 2009

 */

#include "scenariospacevehicle.h"
#include "scenariotrajectoryplan.h"
#include "scenarioproperties.h"
#include "scenarioreentryproperties.h"
#include "scenarioabstracttrajectory.h"
#include "scenarioappearance.h"
#include "scenarioenvironment.h"
#include "scenarioloitering.h"
#include "scenarioreentry.h"
#include "scenariorendezvous.h"
#include "scenariosimulationparameters.h"
#include "scenarioinitialstate.h"
#include "scenariobody.h"
#include "Plotting/groundtrackplottool.h"
#include "Astro-Core/stabody.h"

#include <QIcon>
#include <QTreeWidgetItem>
#include <QtDebug>
#include <iostream>
using namespace std;

/*** ScenarioSpaceVehicle ***/

ScenarioSpaceVehicle::ScenarioSpaceVehicle() :
    m_properties(NULL),
    m_additionalComponents(NULL),
    m_trajectoryPlan(NULL),
    m_appearance(NULL)
{
    m_properties = new ScenarioProperties();
    attachChild(m_properties);
    m_trajectoryPlan = new ScenarioTrajectoryPlan();
    attachChild(m_trajectoryPlan);
    m_appearance = new ScenarioAppearance();
    attachChild(m_appearance);
}


ScenarioSpaceVehicle::~ScenarioSpaceVehicle()
{
    safeRelease(m_properties);
    safeRelease(m_additionalComponents);
    safeRelease(m_trajectoryPlan);
    safeRelease(m_appearance);
}


/*! Set new properties for this space vehicle. The properties may
 *  not be a NULL element.
 */
void 
ScenarioSpaceVehicle::setProperties(ScenarioProperties* properties)
{
    if (properties != m_properties)
    {
        detachChild(m_properties);
        m_properties = properties;
        attachChild(m_properties);
    }
}


void
ScenarioSpaceVehicle::setAdditionalComponents(ScenarioAdditionalComponents* additionalComponents)
{
    if (additionalComponents != m_additionalComponents)
    {
        detachChild(m_additionalComponents);
        m_additionalComponents = additionalComponents;
        attachChild(m_additionalComponents);
    }
}


/*! Set a new trajectory plan for this space vehicle. The trajectory plan cannot
 *  be NULL (though it is allowed to be empty.)
 */
void
ScenarioSpaceVehicle::setTrajectoryPlan(ScenarioTrajectoryPlan* trajectoryPlan)
{
    if (trajectoryPlan != m_trajectoryPlan)
    {
        detachChild(m_trajectoryPlan);
        m_trajectoryPlan = trajectoryPlan;
        attachChild(m_trajectoryPlan);
    }
}


/*! Set a new appearance for this space vehicle. The appearance may
 *  not be a NULL element.
 */
void
ScenarioSpaceVehicle::setAppearance(ScenarioAppearance* appearance)
{
    if (appearance != m_appearance)
    {
        detachChild(m_appearance);
        m_appearance = appearance;
        attachChild(m_appearance);
    }
}


void
ScenarioSpaceVehicle::writeElement(QXmlStreamWriter* out)
{
    out->writeStartElement(elementName());
    out->writeAttribute("Name", name());
    out->writeAttribute("Type", type());
    writeContents(out);
    out->writeEndElement();
}


void ScenarioSpaceVehicle::writeContents(QXmlStreamWriter* out)
{
    m_properties->writeElement(out);
    if (m_additionalComponents)
        m_additionalComponents->writeElement(out);
    m_appearance->writeElement(out);
    m_trajectoryPlan->writeElement(out);
}


void ScenarioSpaceVehicle::createItemContents(QTreeWidgetItem* item)
{
    QString iconName;
    QString typeName = type().toLower();
    if (typeName == "satellite")
        iconName = ":/icons/Satellite.png";
    else if (typeName == "spacecraft")
        iconName = ":/icons/Satellite.png";
    else if (typeName == "launcher")
        iconName = ":/icons/rocket.png";
    else if (typeName == "reentry vehicle")
        iconName = ":/icons/reentryVehicle.png";
    else
        iconName = ":/icons/Satellite.png";
    
    item->setData(0, Qt::DecorationRole, QIcon(iconName));
    m_properties->createItem(item);
    if (m_additionalComponents)
        m_additionalComponents->createItem(item);
    m_appearance->createItem(item);
    m_trajectoryPlan->createItem(item);

    // Expanding now the item that just being dropped into the Scenario Box
    item->setExpanded(true);


}
