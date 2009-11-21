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

#ifndef _STA_SCENARIO_TRAJECTORYPLAN_H_
#define _STA_SCENARIO_TRAJECTORYPLAN_H_

#include "scenarioobject.h"
#include "Astro-Core/statevector.h"
#include <QList>

class ScenarioAbstractTrajectory;
class ScenarioGraphicsViews;
class VisualProperties;

class ScenarioTrajectoryPlan : public ScenarioObject
{
public:
    ScenarioTrajectoryPlan();
    ~ScenarioTrajectoryPlan();
    
    QString elementName() const { return "TrajectoryPlan"; }
    QString displayName() const { return QObject::tr("Trajectory Plan"); }
    
    virtual void writeContents(QXmlStreamWriter* out);
    virtual void createItemContents(QTreeWidgetItem* item);
    virtual QTreeWidgetItem* createItemSelf(QTreeWidgetItem* parent, QString itemName);
    
    const QList<ScenarioAbstractTrajectory*>& trajectories() const { return m_trajectoryList; }
    void addTrajectory(ScenarioAbstractTrajectory* trajectory);
    virtual bool removeChild(unsigned int index);
    void clear();
    
private:
    QList<ScenarioAbstractTrajectory*> m_trajectoryList;
};

#endif // _STA_SCENARIO_TRAJECTORY_PLAN_H_
