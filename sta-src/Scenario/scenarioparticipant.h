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

#ifndef _STA_SCENARIO_PARTICIPANT_H_
#define _STA_SCENARIO_PARTICIPANT_H_

#include "scenarioobject.h"
#include <QColor>

class PropagationFeedback;
class ScenarioTrajectoryPlan;


class VisualProperties
{
public:
    QColor trajectoryColor;
};


/*! Abtract base class of all scenario participants.
 */
class ScenarioParticipant : public ScenarioObject
{
public:
    ScenarioParticipant() {}
    virtual ~ScenarioParticipant() {};

    virtual void writeElement(QXmlStreamWriter* out);
    virtual void writeContents(QXmlStreamWriter* out) = 0;
    virtual QTreeWidgetItem* createItemSelf(QTreeWidgetItem* parent, QString itemName);

    void setName(const QString& name)
    {
        m_name = name;
    }
    
    QString name() const { return m_name; }
    
    const VisualProperties& visualProperties() const { return m_visualProperties; }
    VisualProperties& visualProperties() { return m_visualProperties; }
    
    /*! Return the trajectory of this participant. May return NULL for fixed
     *  objects. SpaceVehicle participants will always have a trajectory.
     */
    virtual ScenarioTrajectoryPlan* trajectoryPlan() const { return NULL; }
    
private:
    QString m_name;
    VisualProperties m_visualProperties;
};

#endif // _STA_SCENARIO_PARTICIPANT_H_
