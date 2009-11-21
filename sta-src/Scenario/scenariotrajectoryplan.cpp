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
 // Patched by Guillermo on June 16th 2009
 */

#include "scenariotrajectoryplan.h"
#include "scenarioabstracttrajectory.h"
#include "scenariosimulationparameters.h"
#include "scenarioinitialstate.h"
#include "scenarioenvironment.h"
#include "propagationfeedback.h"
#include <QTreeWidgetItem>


ScenarioTrajectoryPlan::ScenarioTrajectoryPlan()
{
}


ScenarioTrajectoryPlan::~ScenarioTrajectoryPlan()
{
    clear();
}


void ScenarioTrajectoryPlan::writeContents(QXmlStreamWriter* out)
{
    foreach (ScenarioAbstractTrajectory* trajectory, m_trajectoryList)
    {
        trajectory->writeElement(out);
    }
}


void ScenarioTrajectoryPlan::createItemContents(QTreeWidgetItem* item)
{
    foreach (ScenarioAbstractTrajectory* trajectory, m_trajectoryList)
    {
        trajectory->createItem(item);
    }
}


QTreeWidgetItem*
ScenarioTrajectoryPlan::createItemSelf(QTreeWidgetItem* parent, QString /* itemName */)
{
    QTreeWidgetItem* item = ScenarioObject::createItemSelf(parent);

    // The trajectory plan is a drop target
    item->setFlags(item->flags() | Qt::ItemIsDropEnabled);
    
    // Line patched by Guillermo to expand the item automatically when dropping a trajectory arc
    item->setExpanded(true);

    return item;
}



/*! Append a trajectory to the end of the trajectory plan.
 */
void 
ScenarioTrajectoryPlan::addTrajectory(ScenarioAbstractTrajectory* trajectory)
{
    Q_ASSERT(trajectory != NULL);
    Q_ASSERT(!trajectory->isAttached());
    attachChild(trajectory);
    m_trajectoryList.append(trajectory);
}


/*! Clear all trajectories in this trajectory plan.
 */
void
ScenarioTrajectoryPlan::clear()
{
    foreach (ScenarioAbstractTrajectory* traj, m_trajectoryList)
    {
        traj->release();
    }
}


/*! Remove the trajectory at the specified index.
 */
bool
ScenarioTrajectoryPlan::removeChild(unsigned int index)
{
    if ((int) index < m_trajectoryList.size())
    {
        ScenarioAbstractTrajectory* trajectory = m_trajectoryList.at((int) index);
        m_trajectoryList.removeAt((int) index);
        detachChild(trajectory);
        return true;
    }
    else
    {
        return false;
    }
}

