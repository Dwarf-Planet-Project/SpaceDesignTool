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

#ifndef _STA_SCENARIO_TRAJECTORY_H_
#define _STA_SCENARIO_TRAJECTORY_H_

#include "scenarioobject.h"
#include "Astro-Core/statevector.h"


class ScenarioEnvironment;
class ScenarioSimulationParameters;
class ScenarioOptimizationParameters;
class ScenarioTrajectoryPropagation;
class PropagationFeedback;
class QTemporaryFile;

class ScenarioTrajectory : public ScenarioObject
{
public:
    ScenarioTrajectory();
    ~ScenarioTrajectory();
    
    QString elementName() const { return "Trajectory"; }
    QString displayName() const { return QObject::tr("Trajectory"); }
    virtual void writeElement(QXmlStreamWriter* out);
    virtual void writeContents(QXmlStreamWriter* out);
    virtual void createItemContents(QTreeWidgetItem* item);
    
    ScenarioEnvironment* environment() const { return m_environment; }
    void setEnvironment(ScenarioEnvironment* environment);
    ScenarioSimulationParameters* simulationParameters() const { return m_simulationParameters; }
    void setSimulationParameters(ScenarioSimulationParameters* simulationParameters);
    ScenarioTrajectoryPropagation* trajectoryPropagation() const { return m_trajectoryPropagation; }
    void setTrajectoryPropagation(ScenarioTrajectoryPropagation* trajectoryPropagation);
    
    void propagate(PropagationFeedback& propFeedback, sta::StateVector initialState);
                   
private:
    ScenarioEnvironment* m_environment;
    
    // Only one of the following should be non-NULL
    ScenarioSimulationParameters* m_simulationParameters;
    ScenarioOptimizationParameters* m_optimizationParameters;
    
    ScenarioTrajectoryPropagation* m_trajectoryPropagation;
    // TODO: ScenarioAttitudePropagation* m_attitudePropagation;
    // TODO: ScenarioManeuver* m_maneuver;
    
    QTemporaryFile* m_ephemerisFile;
    QTemporaryFile* m_groundTrackFile;
};

#endif // _STA_SCENARIO_TRAJECTORY_H_

