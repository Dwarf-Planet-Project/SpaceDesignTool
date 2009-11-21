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

#ifndef _STA_SPACE_SCENARIO_WRITER_H_
#define _STA_SPACE_SCENARIO_WRITER_H_

#include "scenario.h"
#include <QXmlStreamWriter>


class SpaceScenarioWriter : public QXmlStreamWriter
{
public:
    SpaceScenarioWriter(QIODevice* device) :
        QXmlStreamWriter(device)
    {
    }
    
    void writeSpaceScenario();
    void writeParticipantsList();
    
    // Participants
    void writeParticipant();
    void writeSatellite();
    void writeGroundStation();
    void writeLaunchPad();
    void writeTarget();
    void writeLauncher();
    void writeReentryVehicle();

#if 0
    ScenarioTrajectoryPlan* readTrajectoryPlan();
    ScenarioTrajectory* readTrajectory();
    ScenarioAdditionalComponents* readAdditionalComponents();
    ScenarioProperties* readProperties();
    ScenarioPhysicalCharacteristics* readPhysicalCharacteristics();
    ScenarioTechnicalCharacteristics* readTechnicalCharacteristics();
    
    ScenarioLocation* readLocation();
    ScenarioEnvironment* readEnvironment();
    ScenarioSimulationParameters* readSimulationParameters();
    ScenarioTrajectoryPropagation* readTrajectoryPropagation();
    ScenarioInitialStatePosition* readInitialStatePosition();
    ScenarioAbstractInitialState* readInitialStateType();
    ScenarioStateVector* readStateVector();
    ScenarioKeplerianElements* readKeplerianElements();
    ScenarioTimeline* readTimeline();
#endif    
    void writeGroundElement(ScenarioGroundElement* groundElement);
    void writeSpaceVehicle(ScenarioSpaceVehicle* spaceVehicle);
    
    StaBody* writeBodyElement();
    
    bool writeDoubleElement(const QString& elementName, double* value);
    bool writeDoubleElementUnits(const QString& elementName, double* value);
};

#endif // _STA_SPACE_SCENARIO_WRITER_H_