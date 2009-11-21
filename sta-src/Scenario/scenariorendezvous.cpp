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

#include "scenariorendezvous.h"
#include "scenarioenvironment.h"
#include "scenariosimulationparameters.h"
#include "scenariomanoeuvreplan.h"
#include "scenariobody.h"
#include "scenarioinitialstate.h"
#include <QTreeWidgetItem>


ScenarioRendezvousTrajectory::ScenarioRendezvousTrajectory():
    m_environment(NULL),
    m_simulationParameters(NULL),
    m_target(NULL),
    m_manoeuvreplan(NULL)
{
    m_environment = new ScenarioEnvironment();
    attachChild(m_environment);
    m_simulationParameters = new ScenarioTargettingSimulationParameters();
    m_simulationParameters->setFinalState(NULL);
    attachChild(m_simulationParameters);
    m_manoeuvreplan = new ScenarioManoeuvrePlan();
    attachChild(m_manoeuvreplan);
	
}


void ScenarioRendezvousTrajectory::writeContents(QXmlStreamWriter* out)
{
    if (m_environment)
        m_environment->writeElement(out);
    if (m_simulationParameters)
    {
        if (m_simulationParameters->finalState())
            m_simulationParameters->setFinalState(NULL);
        m_simulationParameters->writeElement(out);
    }

    writeStringElement(out, "Target", m_target);
    if (m_manoeuvreplan)
        m_manoeuvreplan->writeElement(out);
}


void ScenarioRendezvousTrajectory::createItemContents(QTreeWidgetItem* item)
{
    if (m_environment)
        m_environment->createItem(item);

    if (m_simulationParameters)
    {
        if (m_simulationParameters->finalState())
            m_simulationParameters->setFinalState(NULL);
        m_simulationParameters->createItem(item);
    }

    QTreeWidgetItem* target = new QTreeWidgetItem(item);
    target->setText(0, "Target");
    target->setText(1, m_target);

    if (m_manoeuvreplan)
        m_manoeuvreplan->createItem(item);
}


sta::StateVector
ScenarioRendezvousTrajectory::computeInitialStateVector() const
{
    return m_simulationParameters->initialStatePosition()->computeStateVector(environment()->centralBody()->body());
}


sta::StateVector
ScenarioRendezvousTrajectory::propagate(PropagationFeedback& /* feedback */,
                                        const sta::StateVector& initialState,
                                        QList<double>& /* sampleTimes */,
                                        QList<sta::StateVector>& /* samples */)
{
    // TODO: Not yet implemented; just returns the initial state
    return initialState;
}


void ScenarioRendezvousTrajectory::setEnvironment(ScenarioEnvironment* environment)
{
    if (environment != m_environment)
    {
        detachChild(m_environment);
        m_environment = environment;
        attachChild(m_environment);
    }   
}


void ScenarioRendezvousTrajectory::setSimulationParameters(ScenarioTargettingSimulationParameters* simulationParameters)
{
    if (simulationParameters != m_simulationParameters)
    {
        detachChild(m_simulationParameters);
        m_simulationParameters = simulationParameters;
		if(m_simulationParameters->finalState())
			m_simulationParameters->setFinalState(NULL);
        attachChild(m_simulationParameters);
    }   
}


void ScenarioRendezvousTrajectory::setTarget(QString targetName)
{
	m_target = targetName;
}


void ScenarioRendezvousTrajectory::setManoeuvrePlan(ScenarioManoeuvrePlan* manoeuvrePlan)
{
    if (manoeuvrePlan != m_manoeuvreplan)
    {
        detachChild(m_manoeuvreplan);
        m_manoeuvreplan = manoeuvrePlan;
        attachChild(m_manoeuvreplan);
    }   
}
