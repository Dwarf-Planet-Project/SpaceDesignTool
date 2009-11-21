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

#ifndef _STA_SCENARIO_RENDEZVOUS_H_
#define _STA_SCENARIO_RENDEZVOUS_H_

#include "scenarioabstracttrajectory.h"

class ScenarioManoeuvrePlan;
class ScenarioTargettingSimulationParameters;
class ScenarioEnvironment;


class ScenarioRendezvousTrajectory : public ScenarioAbstractTrajectory
{
public:
    ScenarioRendezvousTrajectory();
    ~ScenarioRendezvousTrajectory(){}
	
    QString elementName() const { return "Rendezvous"; }
    QString displayName() const { return QObject::tr("Rendezvous"); }
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
    ScenarioTargettingSimulationParameters* simulationParameters() const { return m_simulationParameters; }
    void setSimulationParameters(ScenarioTargettingSimulationParameters* simulationParameters);
    QString target() const{ return m_target;}
    void setTarget(QString targetName);
    ScenarioManoeuvrePlan* manoeuvrePlan() const {return m_manoeuvreplan;}
    void setManoeuvrePlan(ScenarioManoeuvrePlan* manoeuvrePlan);
    
private:
    ScenarioEnvironment* m_environment;
    ScenarioTargettingSimulationParameters* m_simulationParameters;
    QString m_target;
    ScenarioManoeuvrePlan* m_manoeuvreplan;	
};

#endif // _STA_SCENARIO_RENDEZVOUS_H_


