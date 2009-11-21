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

#ifndef _STA_SCENARIO_REENTRY_H_
#define _STA_SCENARIO_REENTRY_H_

#include "scenarioabstracttrajectory.h"

//#include "Entry/EntryTrajectory.h"

class ScenarioSimulationMode;
//class ScenarioWindowMode;
class ScenarioEnvironment;
class ScenarioTrajectoryPropagation;


class ScenarioReEntryTrajectory : public ScenarioAbstractTrajectory
{
public:
    ScenarioReEntryTrajectory();
    ~ScenarioReEntryTrajectory(){}
	
    QString elementName() const { return "ReEntry"; }
    QString displayName() const { return QObject::tr("Re-Entry"); }
    virtual void writeContents(QXmlStreamWriter* out);
    virtual void createItemContents(QTreeWidgetItem* item);
    virtual void writeElement(QXmlStreamWriter* out);
    // ScenarioAbstractTrajectory methods
    virtual sta::StateVector computeInitialStateVector() const;

    /** Propagate a re-entry trajectory, filling the lists of sampleTimes
     *  and samples with the results of each step. Errors during propagation
     *  are indicated by setting the error flag in the PropagationFeedback
     *  object.
     *
     *  The return value of propagate is the final state after propagation
     *  is complete (i.e. the last entry in the samples list.)
     */
    virtual sta::StateVector propagate(PropagationFeedback& feedback,
                                       const sta::StateVector& initialState,
                                       QList<double>& sampleTimes,
                                       QList<sta::StateVector>& samples);

    ScenarioEnvironment* environment() const { return m_environment; }
    void setEnvironment(ScenarioEnvironment* environment);
    ScenarioSimulationMode* simulationMode() const {return m_simulationMode;}
    void setSimulationMode(ScenarioSimulationMode* mode);
    //ScenarioWindowMode* windowMode() const {return m_windowMode;}
    //void setWindowMode(ScenarioWindowMode* mode);
    ScenarioTrajectoryPropagation* trajectoryPropagation() const { return m_trajectoryPropagation; }
    void setTrajectoryPropagation(ScenarioTrajectoryPropagation* trajectoryPropagation);

    virtual const StaBody* centralBody() const;
    virtual const sta::CoordinateSystem coordinateSystem() const;

private:
    ScenarioEnvironment* m_environment;
    // Only one of the following should be non-NULL
    ScenarioSimulationMode* m_simulationMode;
    //ScenarioWindowMode* m_windowMode;
	
    ScenarioTrajectoryPropagation* m_trajectoryPropagation;
};

#endif // _STA_SCENARIO_REENTRY_H_


