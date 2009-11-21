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
 Modified by Valentino Zuccarelli on 11th June 2009
 Modified by Tiziana Sabatini on July 2009 (deleted readGravityModel, readPartialEnvironment)
 Modified by Guillermo oc October 2009 to accomodate TLEs
 Modified by Guillermo oc October 2009 to accomodate external trajectories
 */

#ifndef _STA_SPACE_SCENARIO_READER_H_
#define _STA_SPACE_SCENARIO_READER_H_

#include "scenario.h"
#include <QXmlStreamReader>


class SpaceScenarioReader : public QXmlStreamReader
{
public:
    SpaceScenarioReader(QIODevice* device) :
        QXmlStreamReader(device)
    {
    }
    
    SpaceScenarioReader(const QByteArray& data) :
        QXmlStreamReader(data)
    {
    }
    
    SpaceScenario* readSpaceScenario();
    SpaceScenario* readParticipantsList();
    
    // Participants
    ScenarioParticipant* readParticipant();
    ScenarioSpaceVehicle* readSpaceVehicle();
    ScenarioGroundStation* readGroundStation();
    ScenarioLaunchPad* readLaunchPad();
    ScenarioTarget* readTarget();
    
    ScenarioTrajectoryPlan* readTrajectoryPlan();
    ScenarioAbstractTrajectory* readTrajectory();
    ScenarioLoiteringTrajectory* readLoiteringTrajectory();
    ScenarioTleTrajectory* readLoiteringTleTrajectory();
    ScenarioExternalTrajectory* readExternalTrajectory();
    ScenarioLagrangianTrajectory* readLagrangianTrajectory();
    ScenarioRendezvousTrajectory* readRendezvousTrajectory();
    ScenarioReEntryTrajectory* readReEntryTrajectory();
    ScenarioAscentTrajectory* readAscentTrajectory();

    
    ScenarioAppearance* readAppearance();
    ScenarioAdditionalComponents* readAdditionalComponents();
    ScenarioProperties* readProperties();
    ScenarioPhysicalProperties* readPhysicalProperties();
    ScenarioPhysicalCharacteristics* readPhysicalCharacteristics();
    ScenarioGeometricalCharacteristics* readGeometricalCharacteristics();
    ScenarioAerodynamicProperties* readAerodynamicProperties();
    ScenarioPropulsionProperties* readPropulsionProperties();
    ScenarioPayloadProperties* readPayloadProperties();
    ScenarioParachuteProperties* readParachuteProperties();
    ScenarioDesignOverview* readDesignOverview();
    ScenarioSubsystem* readSubsystem(QString name);
    ScenarioPayload* readPayload();
    
    ScenarioLocation* readLocation();
    ScenarioEnvironment* readEnvironment();
    ScenarioThreebodyenvironment* readThreebodyenvironment();
    ScenarioHalo* readHalo();
    ScenarioDeltav* readDeltav();
    ScenarioThreebodyTransfer* readThreebodyTransfer();
    ScenarioSimulationParameters* readSimulationParameters(int module);
    ScenarioTargettingSimulationParameters* readTargettingSimulationParameters();
    ScenarioTrajectoryPropagation* readTrajectoryPropagation();
    ScenarioInitialStatePosition* readInitialStatePosition();
    ScenarioAmplitudes* readAmplitudes();
    ScenarioManifolds* readManifolds();
    ScenarioBranch* readBranch(int numbranch);
    ScenarioParkingOrbit* readParkingOrbit();
    ScenarioHaloOrbit* readHaloOrbit();
    ScenarioOptimizationParameters* readOptimizationParameters();
    ScenarioParameters* readParameters();
    ScenarioEndCondition* readEndCondition();
    ScenarioAbstractInitialState* readInitialStateType();
    ScenarioInitialStateAttitude* readInitialStateAttitude();
    ScenarioStateVector* readStateVector();
    ScenarioKeplerianElements* readKeplerianElements();
    ScenarioSphericalCoordinates* readSphericalCoordinates();
    ScenarioFinalState* readFinalState();
    ScenarioExtendedTimeline* readExtendedTimeline();
    ScenarioTimeline* readTimeline();
//    ScenarioDeviations* readDeviations();
//    ScenarioFixedVariables* readFixedVariables();
//    ScenarioWindow* readWindow();
//    ReentryTrajectoryParamType readReentryTrajectoryParamType();
    
    void readGroundElement(ScenarioGroundElement* groundElement);
    
    ScenarioBody* readBodyElement();
    
    void skipUnknownElement();
    void nextElement();
    bool readDouble(double* value);
    bool readBoolean(bool* value);
    bool readUnsignedInt(unsigned int* value);
    bool readQString(QString* value);
    bool readDoubleElement(const QString& elementName, double* value);
    bool readUnsignedIntElement(const QString& elementName, unsigned int* value);
    bool readStringElement(const QString& elementName, QString* value);
    StaBody* readBody();
    bool expectedElement(const QString& elementName);
};

#endif // _STA_SPACE_SCENARIO_READER_H_

