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

#ifndef _STA_SCENARIO_LOITERING_H_
#define _STA_SCENARIO_LOITERING_H_

#include "scenarioabstracttrajectory.h"

class ScenarioEnvironment;
class ScenarioSimulationParameters;
class ScenarioOptimizationParameters;
class ScenarioTrajectoryPropagation;


class ScenarioLoiteringTrajectory : public ScenarioAbstractTrajectory
{
public:
    ScenarioLoiteringTrajectory();
    ~ScenarioLoiteringTrajectory() {}
    
    QString elementName() const { return "Loitering"; }
    QString displayName() const { return QObject::tr("Loitering"); }
    virtual void writeElement(QXmlStreamWriter* out);
    virtual void writeContents(QXmlStreamWriter* out);
    virtual void createItemContents(QTreeWidgetItem* item);

    // ScenarioAbstractTrajectory methods
    virtual sta::StateVector computeInitialStateVector() const;
    virtual sta::StateVector propagate(PropagationFeedback& feedback,
                                       const sta::StateVector& initialState,
                                       QList<double>& sampleTimes,
                                       QList<sta::StateVector>& samples);

    ScenarioEnvironment* environment() const { return m_environment; }
    void setEnvironment(ScenarioEnvironment* environment);
    ScenarioSimulationParameters* simulationParameters() const { return m_simulationParameters; }
    void setSimulationParameters(ScenarioSimulationParameters* simulationParameters);
    ScenarioTrajectoryPropagation* trajectoryPropagation() const { return m_trajectoryPropagation; }
    void setTrajectoryPropagation(ScenarioTrajectoryPropagation* trajectoryPropagation);
    
    virtual const StaBody* centralBody() const;
    virtual const sta::CoordinateSystem coordinateSystem() const;

private:
    ScenarioEnvironment* m_environment;
    
    // Only one of the following should be non-NULL
    ScenarioSimulationParameters* m_simulationParameters;
    ScenarioOptimizationParameters* m_optimizationParameters;
    
    ScenarioTrajectoryPropagation* m_trajectoryPropagation;
    // TODO: ScenarioAttitudePropagation* m_attitudePropagation;
    // TODO: ScenarioManeuver* m_maneuver;
};

#endif // _STA_SCENARIO_LOITERING_H_


