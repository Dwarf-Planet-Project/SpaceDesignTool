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
 */

#ifndef _STA_SCENARIO_SPACEVEHICLE_H_
#define _STA_SCENARIO_SPACEVEHICLE_H_

#include "scenarioparticipant.h"
#include "Astro-Core/stabody.h"
#include "Astro-Core/stacoordsys.h"
#include "Astro-Core/statevector.h"

class ScenarioProperties;
class ScenarioAdditionalComponents;
class ScenarioAppearance;

class ScenarioTrajectoryPlan;

/*! Participant with a trajectory in space.
 */
class ScenarioSpaceVehicle : public ScenarioParticipant
{
public:
    ScenarioSpaceVehicle();
    virtual ~ScenarioSpaceVehicle();

    QString elementName() const { return "SpaceVehicle"; }
    QString displayName() const { return m_type.isEmpty() ? QObject::tr("Space Vehicle") : m_type; }

    virtual void writeElement(QXmlStreamWriter* out);
    virtual void writeContents(QXmlStreamWriter* out);
    virtual void createItemContents(QTreeWidgetItem* item);

    ScenarioProperties* properties() const { return m_properties; }
    void setProperties(ScenarioProperties* properties);
    
    ScenarioAdditionalComponents* additionalComponents() const { return m_additionalComponents; }
    void setAdditionalComponents(ScenarioAdditionalComponents* additionalComponents);
    
    ScenarioAppearance* appearance() const { return m_appearance; }
    void setAppearance(ScenarioAppearance* appearance);
    
    ScenarioTrajectoryPlan* trajectoryPlan() const { return m_trajectoryPlan; }
    void setTrajectoryPlan(ScenarioTrajectoryPlan* trajectoryPlan);

    QString type() const { return m_type; }
    void setType(QString type) { m_type = type; }
          
        
private:
    ScenarioProperties* m_properties;
    ScenarioAdditionalComponents* m_additionalComponents;
    ScenarioTrajectoryPlan* m_trajectoryPlan;
    ScenarioAppearance* m_appearance;
    
    QString m_type;
};


class ScenarioAdditionalComponents : public ScenarioObject
{
public:
    QString displayName() const { return QObject::tr("Additional Components"); \
    }
};

#endif // _STA_SCENARIO_SPACEVEHICLE_H_
