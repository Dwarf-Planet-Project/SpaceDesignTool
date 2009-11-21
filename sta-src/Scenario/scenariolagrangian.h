
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
------ Copyright (C) 2009 European Space Agency (space.trajectory.analysis AT gmail.com) ----
 ------------------ Author: Valentino Zuccarelli  -------------------------------------------------
 ------------------ E-mail: (Valentino.Zuccarelli@gmail.com) ----------------------------
 */

#ifndef SCENARIOLAGRANGIAN_H
#define SCENARIOLAGRANGIAN_H

#include "scenarioabstracttrajectory.h"

class ScenarioThreebodyenvironment;
class ScenarioHalo;
class ScenarioSimulationParameters;
class ScenarioTrajectoryPropagation;
class ScenarioThreebodyTransfer;
class ScenarioHaloOrbit;

class ScenarioLagrangianTrajectory : public ScenarioAbstractTrajectory
{
public:

    ScenarioLagrangianTrajectory();
    ~ScenarioLagrangianTrajectory() {}

    QString elementName() const { return "Lagrangian"; }
    QString displayName() const { return QObject::tr("Lagrangian"); }
    virtual void writeElement(QXmlStreamWriter* out);
    virtual void writeContents(QXmlStreamWriter* out);
    virtual void createItemContents(QTreeWidgetItem* item);

    // ScenarioAbstractTrajectory methods
    virtual sta::StateVector computeInitialStateVector() const;
    virtual sta::StateVector propagate(PropagationFeedback& feedback,
                                       const sta::StateVector& initialState,
                                       QList<double>& sampleTimes,
                                       QList<sta::StateVector>& samples);

    ScenarioThreebodyenvironment* threebodyenvironment() const { return m_threebodyenvironment; }
    void setThreebodyenvironment(ScenarioThreebodyenvironment* threebodyenvironment);
    ScenarioSimulationParameters* simulationParameters() const { return m_simulationParameters; }
    void setSimulationParameters(ScenarioSimulationParameters* simulationParameters);
    ScenarioTrajectoryPropagation* trajectoryPropagation() const { return m_trajectoryPropagation; }
    void setTrajectoryPropagation(ScenarioTrajectoryPropagation* trajectoryPropagation);
    ScenarioHalo* halo() const { return m_halo; }
    void setHalo(ScenarioHalo* halo);
    ScenarioThreebodyTransfer* threebodyTransfer() const { return m_threebodyTransfer; }
    void setThreebodyTransfer(ScenarioThreebodyTransfer* threebodyTransfer);

    virtual const StaBody* firstBody() const;
    virtual const StaBody* secondBody() const;
    virtual const StaBody* centralBody() const;
    virtual const sta::CoordinateSystem coordinateSystem() const;

private:
    ScenarioThreebodyenvironment* m_threebodyenvironment;

    // Only one of the following should be non-NULL
    ScenarioSimulationParameters* m_simulationParameters;
    ScenarioTrajectoryPropagation* m_trajectoryPropagation;
    ScenarioHalo* m_halo;
    ScenarioThreebodyTransfer* m_threebodyTransfer;
};

#endif // SCENARIOLAGRANGIAN_H


