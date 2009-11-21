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
  Modified by Valentino Zuccarelli on June 23th 2009
  Modified by Tiziana Sabatini on July 2009 (ReadScenarioEnvironment, ReadGravityModel, ReadPartialEnvironment)
  Modified by Guillermo to include TLEs handling on October 2009
 */

#include "spacescenarioreader.h"
#include "scenarioreentrymode.h"
#include "scenariosimulationparameters.h"
#include "Astro-Core/date.h"

#include <iostream>
#include <QDebug>


SpaceScenario*
SpaceScenarioReader::readSpaceScenario()
{
    SpaceScenario* scenario = NULL;
    
    while (!atEnd())
    {
        readNext();
        
        if (isStartElement())
        {
            if (name() == "SpaceScenario")
            {
                QString scenarioName = attributes().value("Name").toString();
                scenario = readParticipantsList();
                if (scenario)
                    scenario->setName(scenarioName);
            }
            else
            {
		raiseError(QObject::tr("Not a Space Scenario version 2.0 file."));
            }
        }
    }
    
    return scenario;
}


SpaceScenario*
SpaceScenarioReader::readParticipantsList()
{
    SpaceScenario* scenario = NULL;
    
    nextElement();
    if (isStartElement() && name() == "Participants")
    {
        // Read items in the participants list
        scenario = new SpaceScenario();
        while (!atEnd())
        {
            readNext();
            if (isEndElement())
                break;
            
            if (isStartElement())
            {
                ScenarioParticipant* participant = readParticipant();
                if (participant != NULL)
                    scenario->addParticipant(participant);
            }
        }
    }
    else
    {
        raiseError(QObject::tr("Participants list missing from space scenario."));
    }
    
    return scenario;
}


ScenarioParticipant*
SpaceScenarioReader::readParticipant()
{
    ScenarioParticipant* participant = NULL;

    
    QString participantName = attributes().value("Name").toString();
    
    if (name() == "SpaceVehicle")
        participant = readSpaceVehicle();
    else if (name() == "GroundStation")
        participant = readGroundStation();
    //else if (name() == "Target")
    else if (name() == "Point")
        participant = readTarget();
    else if (name() == "LaunchPad")
        participant = readLaunchPad();
    else
        raiseError(QObject::tr("Unknown element '%1' in participants list.").arg(name().toString()));
    
    if (participant)
        participant->setName(participantName);
    
    return participant;
}


void
SpaceScenarioReader::readGroundElement(ScenarioGroundElement* groundElement)
{
    nextElement();
    if (isStartElement() && name() == "Location")
    {
        ScenarioLocation* location = readLocation();
        if (!hasError())
        {
            groundElement->setLocation(location);
            nextElement();
        }
    }
    else
    {
        raiseError("Location missing for ground element.");
    }    
}


ScenarioLocation*
SpaceScenarioReader::readLocation()
{
    GroundPosition groundPosition;
    const StaBody* centralBody = STA_SOLAR_SYSTEM->lookup("Earth");
    
    nextElement();
    if (isStartElement() && name() == "GroundPosition")
    {
        nextElement();
        if (isStartElement() && name() == "Latitude")
        {
            if (!readDouble(&groundPosition.latitude))
                return NULL;
        }
        else
        {
            raiseError(QObject::tr("Latitude missing from ground position."));
            return NULL;
        }
        
        nextElement();
        if (isStartElement() && name() == "Longitude")
        {
            if (!readDouble(&groundPosition.longitude))
                return NULL;
        }
        else
        {
            raiseError(QObject::tr("Longitude missing from ground position."));
            return NULL;
        }
        
        nextElement();
        if (isStartElement() && name() == "Altitude")
        {
            if (!readDouble(&groundPosition.altitude))
                return NULL;
        }
        else
        {
            raiseError(QObject::tr("Altitude missing from ground position."));
            return NULL;
        }
        
        nextElement();
        if (!isEndElement())
        {
            raiseError(QObject::tr("Unknown element in ground position."));
            return NULL;
        }
        
        nextElement();
        if (isStartElement() && name() == "CentralBody")
        {
            centralBody = readBody();
        }
        else
        {
            raiseError(QObject::tr("Central body missing in location."));
            return NULL;
        }
        
        nextElement();
    }
    else
    {
        raiseError(QObject::tr("Ground position missing for location."));
        return NULL;
    }
    
    if (!isEndElement())
    {
        raiseError(QObject::tr("Unknown element in location."));
        return NULL;
    }
    
    ScenarioLocation* location = new ScenarioLocation();
    location->setCentralBody(centralBody);
    location->setGroundPosition(groundPosition);
    
    return location;
}


ScenarioGroundStation*
SpaceScenarioReader::readGroundStation()
{
    ScenarioGroundStation* groundStation = new ScenarioGroundStation();
    readGroundElement(groundStation);
    
    return groundStation;
}


ScenarioTarget*
SpaceScenarioReader::readTarget()
{
    ScenarioTarget* target = new ScenarioTarget();
    readGroundElement(target);
    
    return target;
}


ScenarioLaunchPad*
SpaceScenarioReader::readLaunchPad()
{
    ScenarioLaunchPad* launchPad = new ScenarioLaunchPad();
    readGroundElement(launchPad);
    
    if (!hasError())
    {
        if (isStartElement() && name() == "LaunchPadClearingAltitude")
        {
            double clearingAltitude = 0.0;
            readDouble(&clearingAltitude);
            nextElement();
        }
        else
        {
            raiseError(QObject::tr("Clearing altitude missing from launch pad."));
        }
    }
    
    if (hasError())
    {
        delete launchPad;
        launchPad = NULL;
    }
    
    return launchPad;
}


ScenarioSpaceVehicle*
SpaceScenarioReader::readSpaceVehicle()
{
    ScenarioSpaceVehicle* spaceVehicle = new ScenarioSpaceVehicle();
    
    QString vehicleType = attributes().value("Type").toString();
    if (vehicleType.isEmpty())
    {
        raiseError(QObject::tr("Space vehicle type is missing."));
        delete spaceVehicle;
    }
    
    spaceVehicle->setType(vehicleType);
    
    nextElement();
    if (isStartElement() && name() == "Properties")
    {
        ScenarioProperties* properties = readProperties();
        if (hasError())
        {
            delete spaceVehicle;
            return NULL;
        }
        else
        {
            spaceVehicle->setProperties(properties);
            nextElement();
        }
    }
    else
    {
        raiseError("Properties missing for space vehicle.");
        delete spaceVehicle;
        return NULL;
    }

    if (isStartElement() && name() == "AdditionalComponents")
    {
        ScenarioAdditionalComponents* additionalComponents = readAdditionalComponents();
        spaceVehicle->setAdditionalComponents(additionalComponents);
        nextElement();
    }
    
    if (hasError())
    {
        delete spaceVehicle;
        return NULL;
    }

    if (isStartElement() && name() == "Appearance")
    {
        ScenarioAppearance* appearance = readAppearance();
        spaceVehicle->setAppearance(appearance);
        nextElement();
    }
    
    if (hasError())
    {
        delete spaceVehicle;
        return NULL;
    }
    
    ScenarioTrajectoryPlan* trajectoryPlan = readTrajectoryPlan();
    if (trajectoryPlan != NULL)
    {
        spaceVehicle->setTrajectoryPlan(trajectoryPlan);
        nextElement();
    }
    else
    {
        delete spaceVehicle;
        return NULL;
    }
    
    return spaceVehicle;
}


ScenarioTrajectoryPlan*
SpaceScenarioReader::readTrajectoryPlan()
{
    ScenarioTrajectoryPlan* trajectoryPlan = NULL;

    if (isStartElement() && name() == "TrajectoryPlan")
    {
        trajectoryPlan = new ScenarioTrajectoryPlan();
        
        while (!atEnd())
        {
            readNext();
            if (isEndElement())
                break;
        
            if (isStartElement())
            {
                ScenarioAbstractTrajectory* trajectory = readTrajectory();
                if (trajectory != NULL)
                {
                    trajectoryPlan->addTrajectory(trajectory);
                }
                else
                {
                    // Otherwise an error must have occurred
                }
            }
        }
    }
    else
    {
        raiseError(QObject::tr("TrajectoryPlan missing from SpaceVehicle."));
    }

    if (hasError())
    {
        delete trajectoryPlan;
        trajectoryPlan = NULL;
    }
    
    return trajectoryPlan;
}


ScenarioAbstractTrajectory*
SpaceScenarioReader::readTrajectory()
{
    ScenarioAbstractTrajectory* trajectory = NULL;
    
    if (name() == "Loitering")
    {
        trajectory = readLoiteringTrajectory();
    }
    else if (name() == "LoiteringTLE")
    {
        trajectory = readLoiteringTleTrajectory();
    }
    else if (name() == "External")
    {
	trajectory = readExternalTrajectory();
    }
    else if (name() == "Lagrangian")
    {
        trajectory = readLagrangianTrajectory();
    }
//    else if (name() == "Interplanetary")
//    {
//        trajectory = readInterplanetaryTrajectory();
//    }
    else if (name() == "Rendezvous")
    {
        trajectory = readRendezvousTrajectory();
    }
    else if (name() == "ReEntry")
    {
        trajectory = readReEntryTrajectory();
    }
    else if (name() == "Ascent")
    {
        trajectory = readAscentTrajectory();
    }
    else
    {
        raiseError(QObject::tr("Unknown trajectory type %1 in trajectory plan.").arg(name().toString()));
    }
    
    return trajectory;
}


ScenarioLoiteringTrajectory*
SpaceScenarioReader::readLoiteringTrajectory()
{
    ScenarioLoiteringTrajectory* trajectory = new ScenarioLoiteringTrajectory();
    
    nextElement();
    if (!expectedElement("Environment"))
    {
        delete trajectory;
        return NULL;
    }
    
    ScenarioEnvironment* environment = readEnvironment();
    if (!environment)
    {
        delete trajectory;
        return NULL;
    }
    else
    {
        trajectory->setEnvironment(environment);
    }
    
    nextElement();
    if (!expectedElement("SimulationParameters"))
    {
        delete trajectory;
        return NULL;
    }
        
    ScenarioSimulationParameters* simulationParameters = readSimulationParameters(0);
    if (!simulationParameters)
    {
        delete trajectory;
        return NULL;
    }
    else
    {
        trajectory->setSimulationParameters(simulationParameters);
    }
    
    nextElement();
    if (!expectedElement("TrajectoryPropagation"))
    {
        delete trajectory;
        return NULL;
    }
    
    ScenarioTrajectoryPropagation* trajectoryPropagation = readTrajectoryPropagation();
    if (!trajectoryPropagation)
    {
        delete trajectory;
        return NULL;
    }
    else
    {
        trajectory->setTrajectoryPropagation(trajectoryPropagation);
    }
    
    nextElement();
    
    // Attitude propagation is optional (currently)
    if (isStartElement() && name() == "AttitudePropagation")
    {
        skipUnknownElement();
        if (hasError())
        {
            delete trajectory;
            return NULL;
        }
        nextElement();
    }
    
    // Impulsive maneuver is optional
    if (isStartElement() && name() == "ImpulsiveManeuver")
    {
        skipUnknownElement();
        if (hasError())
        {
            delete trajectory;
            return NULL;
        }
        nextElement();
    }
    
    if (hasError())
    {
        delete trajectory;
        return NULL;
    }

    return trajectory;
}


ScenarioTleTrajectory*
SpaceScenarioReader::readLoiteringTleTrajectory()
{
    ScenarioTleTrajectory* trajectory = new ScenarioTleTrajectory();

    // Read the timeline
    nextElement();
    if (!expectedElement("Timeline"))
    {
        delete trajectory;
        return NULL;
    }

    ScenarioExtendedTimeline* timeline = readExtendedTimeline();
    if (timeline)
    {
        trajectory->setTimeline(timeline);
    }
    else
    {
        delete trajectory;
        return NULL;
    }

    nextElement();

    QString tleLine0;
    QString tleLine1;
    QString tleLine2;

    readStringElement("Line0", &tleLine0);
    nextElement();
    readStringElement("Line1", &tleLine1);
    nextElement();
    readStringElement("Line2", &tleLine2);
    nextElement();

    trajectory->setTleLine1(tleLine1);
    trajectory->setTleLine2(tleLine2);

    return trajectory;
}



ScenarioExternalTrajectory*
SpaceScenarioReader::readExternalTrajectory()
{
    ScenarioExternalTrajectory* trajectory = new ScenarioExternalTrajectory();

    // Read the timeline
    nextElement();
    if (!expectedElement("Timeline"))
    {
	delete trajectory;
	return NULL;
    }

    ScenarioExtendedTimeline* timeline = readExtendedTimeline();
    if (timeline)
    {
	trajectory->setTimeline(timeline);
    }
    else
    {
	delete trajectory;
	return NULL;
    }

    nextElement();

    QString FisrtLineExternalTrajectory;
    QString LastLineExternalTrajectory;
    readStringElement("StateVector1", &FisrtLineExternalTrajectory);
    nextElement();
    readStringElement("StateVector2", &LastLineExternalTrajectory);

    trajectory->setFirstExternalPropagatedLine(FisrtLineExternalTrajectory);
    trajectory->setLastExternalPropagatedLine(LastLineExternalTrajectory);

    return trajectory;
}



ScenarioLagrangianTrajectory*
SpaceScenarioReader::readLagrangianTrajectory()
{
    ScenarioLagrangianTrajectory* trajectory = new ScenarioLagrangianTrajectory();
    nextElement();
    if (!expectedElement("Threebodyenvironment"))
    {
        delete trajectory;
        return NULL;
    }

    ScenarioThreebodyenvironment* threebodyenvironment = readThreebodyenvironment();
    if (!threebodyenvironment)
    {
        delete trajectory;
        return NULL;
    }
    else
    {
        trajectory->setThreebodyenvironment(threebodyenvironment);
    }


{nextElement();
    if (!expectedElement("SimulationParameters"))
    {
        delete trajectory;
        return NULL;
    }

  ScenarioSimulationParameters* simulationParameters = readSimulationParameters(1);
    if (!simulationParameters)
    {
        delete trajectory;
        return NULL;
    }
    else
    {
        trajectory->setSimulationParameters(simulationParameters);
        }
    }

{nextElement();
    ScenarioHalo* halo = readHalo();
    {
    if (hasError())
    {
        delete trajectory;
        return NULL;
    }

    {
        trajectory->setHalo(halo);
    }
    }
}

{nextElement();
    ScenarioThreebodyTransfer* threebodyTransfer = readThreebodyTransfer();
    {
    if (hasError())
    {
        delete trajectory;
        return NULL;
    }

    {
        trajectory->setThreebodyTransfer(threebodyTransfer);
    }
    }
}

    nextElement();

    if (hasError())
    {
        delete trajectory;
        return NULL;
    }

    return trajectory;
}

/*ScenarioInterplanetaryTrajectory*
SpaceScenarioReader::readInterplanetaryTrajectory()
{
    ScenarioInterplanetaryTrajectory* trajectory = new ScenarioInterplanetaryTrajectory();
    nextElement();

    if (!expectedElement("Mode"))
    {
        delete trajectory;
        return NULL;
    }

    QString mode;
    mode = readElementText();
    trajectory->setMode(mode);

    if (hasError())
    {
        delete trajectory;
        return NULL;
    }

    return trajectory;
}*/


ScenarioReEntryTrajectory*
SpaceScenarioReader::readReEntryTrajectory()
{
    //skipUnknownElement();
    ScenarioReEntryTrajectory* trajectory = new ScenarioReEntryTrajectory();

    nextElement();
    if (!expectedElement("Environment"))
    {
        delete trajectory;
        return NULL;
    }

    ScenarioEnvironment* environment = readEnvironment();
    if (!environment)
    {
        delete trajectory;
        return NULL;
    }
    else
    {
        trajectory->setEnvironment(environment);
    }
    
    nextElement();
    if (name()=="SimulationMode")
    {
        ScenarioSimulationMode* simulationMode = new ScenarioSimulationMode();
        simulationMode->setIsActiveTrue();
//        ScenarioWindowMode* windowMode = new ScenarioWindowMode();
//        windowMode->setIsActiveFalse();

        nextElement();
        if (!expectedElement("SimulationParameters"))
        {
            delete trajectory;
            return NULL;
        }
        else
        {
            ScenarioTargettingSimulationParameters* targettingParameters = readTargettingSimulationParameters();
            if (!targettingParameters)
            {
                delete trajectory;
                return NULL;
            }
            else
            {
                simulationMode->setSimulationParameters(targettingParameters);

                nextElement();

            }
        }
        nextElement();

//        if (isStartElement() && name() == "DispersionAnalysis")
//        {
//        //ScenarioDispersionAnalysis* dispersionAnalysis = new ScenarioDispersionAnalysis();
//        //dispersionAnalysis->setIsActiveFalse();
//        //simulationMode->setDispersionAnalysis(dispersionAnalysis);
//
//            ScenarioDispersionAnalysis* dispersionAnalysis = new ScenarioDispersionAnalysis();
//            dispersionAnalysis->setIsActiveTrue();
//            unsigned int numberofsimulations = 0;
//            nextElement();
//            readUnsignedIntElement("NumberOfSimulations", &numberofsimulations);
//            dispersionAnalysis->setnumSimulations(numberofsimulations);
//            nextElement();
//            if (!expectedElement("Deviations"))
//            {
//                delete trajectory;
//                return NULL;
//            }
//            else
//            {
//                ScenarioDeviations* deviations = readDeviations();
//                nextElement();
//                dispersionAnalysis->setDeviations(deviations);
//            }
//
//            simulationMode->setDispersionAnalysis(dispersionAnalysis);
//            nextElement();
//        }
//        else
//        {
//            ScenarioDispersionAnalysis* dispersionAnalysis = new ScenarioDispersionAnalysis();
//            dispersionAnalysis->setIsActiveFalse();
//            simulationMode->setDispersionAnalysis(dispersionAnalysis);
//        }
        trajectory->setSimulationMode(simulationMode);
//        trajectory->setWindowMode(windowMode);
    }

//   else if (name()=="WindowAnalysisMode")
//    {
//        ScenarioWindowMode* windowMode = new ScenarioWindowMode();
//        windowMode->setIsActiveTrue();
//        ScenarioSimulationMode* simulationMode = new ScenarioSimulationMode();
//        simulationMode->setIsActiveFalse();
//
//        nextElement();
//        if (!expectedElement("SimulationParameters"))
//        {
//            delete trajectory;
//            return NULL;
//        }
//        else
//        {
//            ScenarioTargettingSimulationParameters* targettingParameters = readTargettingSimulationParameters();
//            if (!targettingParameters)
//            {
//                delete trajectory;
//                return NULL;
//            }
//            else
//            {
//                windowMode->setSimulationParameters(targettingParameters);
//                nextElement();
//            }
//        }
//
//        unsigned int numberofsimulations = 0;
//        readUnsignedIntElement("NumberOfSimulations", &numberofsimulations);
//        windowMode->setnumSimulations(numberofsimulations);
//
//        nextElement();
//        if (!expectedElement("FixedVariables"))
//        {
//            delete trajectory;
//            return NULL;
//        }
//        else
//        {
//            ScenarioFixedVariables* fixedVariables = readFixedVariables();
//            if (!fixedVariables)
//            {
//                delete trajectory;
//                return NULL;
//            }
//            else
//            {
//                windowMode->setFixedVariables(fixedVariables);
//
//            }
//        }
//
//        nextElement();
//        if (!expectedElement("Window"))
//        {
//            delete trajectory;
//            return NULL;
//        }
//        else
//        {
//            ScenarioWindow* window = readWindow();
//            if (!window)
//            {
//                delete trajectory;
//                return NULL;
//            }
//            else
//            {
//                windowMode->setWindow(window);
//                nextElement();
//            }
//        }
//
//        trajectory->setWindowMode(windowMode);
//        trajectory->setSimulationMode(simulationMode);
//        nextElement();
//    }

    nextElement();
    if (!expectedElement("TrajectoryPropagation"))
    {
        delete trajectory;
        return NULL;
    }
    else {
    ScenarioTrajectoryPropagation* trajectoryPropagation = readTrajectoryPropagation();
    if (!trajectoryPropagation)
    {
        delete trajectory;
        return NULL;
    }
    else
    {
        trajectory->setTrajectoryPropagation(trajectoryPropagation);
    }
    }

    nextElement();
    
    // Attitude propagation is optional (currently)
    if (isStartElement() && name() == "AttitudePropagation")
    {
        skipUnknownElement();
        if (hasError())
        {
            delete trajectory;
            return NULL;
        }
        nextElement();
    }
    
    // Impulsive maneuver is optional
    if (isStartElement() && name() == "ImpulsiveManeuver")
    {
        skipUnknownElement();
        if (hasError())
        {
            delete trajectory;
            return NULL;
        }
        nextElement();
    }
    
    if (hasError())
    {
        delete trajectory;
        return NULL;
    }

    return trajectory;
}


ScenarioRendezvousTrajectory*
SpaceScenarioReader::readRendezvousTrajectory()
{
    ScenarioRendezvousTrajectory* trajectory = new ScenarioRendezvousTrajectory();
    
    nextElement();
    if (!expectedElement("Environment"))
    {
        delete trajectory;
        return NULL;
    }
    
    ScenarioEnvironment* environment = readEnvironment();
    if (!environment)
    {
        delete trajectory;
        return NULL;
    }
    else
    {
        trajectory->setEnvironment(environment);
    }
    
    nextElement();
    if (!expectedElement("SimulationParameters"))
    {
        delete trajectory;
        return NULL;
    }
        
    ScenarioTargettingSimulationParameters* simulationParameters = readTargettingSimulationParameters();
    if (!simulationParameters)
    {
        delete trajectory;
        return NULL;
    }
    else
    {
        trajectory->setSimulationParameters(simulationParameters);
    }
    
    nextElement();
    //if (!expectedElement("Target"))
    if (!expectedElement("Point"))
    {
        delete trajectory;
        return NULL;
    }
    else
    {
        skipUnknownElement();
    }

    nextElement();
    if (!expectedElement("ManoeuvrePlan"))
    {
        delete trajectory;
        return NULL;
    }
    else
    {
        skipUnknownElement();
    }
        
    if (hasError())
    {
        delete trajectory;
        return NULL;
    }

    nextElement();

    return trajectory;
}


ScenarioAscentTrajectory*
SpaceScenarioReader::readAscentTrajectory()
{
    skipUnknownElement();
    return new ScenarioAscentTrajectory();
}

ScenarioAppearance*
SpaceScenarioReader::readAppearance()
{
    ScenarioAppearance* appearance = new ScenarioAppearance();
    
    nextElement();
    if (isStartElement() && name() == "Model")
    {
        QString text = readElementText();
        appearance->setModel(text);
    }
    
    if (hasError())
    {
        delete appearance;
        return NULL;
    }
    
    nextElement();
    
    return appearance;
}


ScenarioAdditionalComponents*
SpaceScenarioReader::readAdditionalComponents()
{
    skipUnknownElement();
    return new ScenarioAdditionalComponents();
}


ScenarioProperties*
SpaceScenarioReader::readProperties()
{
    ScenarioProperties* properties = new ScenarioProperties(); 
    
    nextElement();
    if (!expectedElement("PhysicalProperties"))
    {
        delete properties;
        return NULL;
    }
    
    ScenarioPhysicalProperties* physicalProperties = readPhysicalProperties();
    if (!physicalProperties)
    {
        delete properties;
        return NULL;
    }
    else
    {
        properties->setPhysicalProperties(physicalProperties);
        nextElement();
    }


    if (!expectedElement("AerodynamicProperties"))
    {
        delete properties;
        return NULL;
    }

    ScenarioAerodynamicProperties* aerodynamicProperties = readAerodynamicProperties();
    if (!aerodynamicProperties)
    {
        delete properties;
        return NULL;
    }
    else
    {
        properties->setAerodynamicProperties(aerodynamicProperties);
        nextElement();
    }

    if (!expectedElement("PropulsionProperties"))
    {
        delete properties;
        return NULL;
    }

    ScenarioPropulsionProperties* propulsionProperties = readPropulsionProperties();
    if (!propulsionProperties)
    {
        delete properties;
        return NULL;
    }
    else
    {
        properties->setPropulsionProperties(propulsionProperties);
    }
    nextElement();

    if (!expectedElement("PayloadProperties"))
    {
        delete properties;
        return NULL;
    }

    ScenarioPayloadProperties* payloadProperties = readPayloadProperties();
    if (!payloadProperties)
    {
        delete properties;
        return NULL;
    }
    else
    {
        properties->setPayloadProperties(payloadProperties);
    }
    nextElement();

//    if (!expectedElement("Technical"))
//    {
//        delete properties;
//        return NULL;
//    }
//
//    ScenarioTechnicalCharacteristics* technical = readTechnicalCharacteristics();
//    if (!technical)
//    {
//        delete properties;
//        return NULL;
//    }
//    else
//    {
//        properties->setTechnical(technical);
//        nextElement();
//    }
//
//    if (isStartElement() && name() == "MainBody")
//    {
//        ScenarioMainBodyCharacteristics* mainBody = readMainBodyCharacteristics();
//        if (!mainBody)
//        {
//            delete properties;
//            return NULL;
//        }
//        else
//        {
//            properties->setMainBody(mainBody);
//            nextElement();
//        }
//    }
//
//    if (isStartElement() && name() == "Parachute")
//    {
//        ScenarioParachuteCharacteristics* parachute = readParachuteCharacteristics();
//        if (!parachute)
//        {
//            delete properties;
//            return NULL;
//        }
//        else
//        {
//            properties->setParachute(parachute);
//            nextElement();
//        }
//    }
//
//    if (isStartElement() && name() == "DesignOverview")
//    {
//        ScenarioDesignOverview* design = readDesignOverview();
//        if (!design)
//        {
//            delete properties;
//            return NULL;
//        }
//        else
//        {
//            properties->setDesignOverview(design);
//            nextElement();
//        }
//    }
    
    return properties;
}

ScenarioPhysicalProperties*
SpaceScenarioReader::readPhysicalProperties()
{
    ScenarioPhysicalProperties* physicalproperties = new ScenarioPhysicalProperties();
    nextElement();
    if (!expectedElement("PhysicalCharacteristics"))
    {
        delete physicalproperties;
        return NULL;
    }

    ScenarioPhysicalCharacteristics* physicalcharacteristics = readPhysicalCharacteristics();
    if (!physicalcharacteristics)
    {
        delete physicalproperties;
        return NULL;
    }
    else
    {
        physicalproperties->setPhysicalCharacteristics(physicalcharacteristics);
        nextElement();
    }
    if (isStartElement() && name() == "GeometricalCharacteristics")
    {
        ScenarioGeometricalCharacteristics* geometricalcharacteristics = readGeometricalCharacteristics();
        if (!geometricalcharacteristics)
        {
            delete physicalproperties;
            return NULL;
        }
        else
        {
            physicalproperties->setGeometricalCharacteristics(geometricalcharacteristics);
            nextElement();
        }
    }
    return physicalproperties;
}

ScenarioPhysicalCharacteristics*
SpaceScenarioReader::readPhysicalCharacteristics()
{
    double mass = 0.0;
    double volume = 0.0;
    double surfaceArea = 0.0;
    
    while (!atEnd())
    {
        readNext();
        if (isEndElement())
            break;

        // Mass, surface, and volume can appear in any order, and all values are optional
        if (isStartElement())
        {
            bool ok = true;
            
            if (name() == "mass")
                ok = readDouble(&mass);
            else if (name() == "volume")
                ok = readDouble(&volume);
            else if (name() == "surface")
                ok = readDouble(&surfaceArea);
            else
                skipUnknownElement();
            
            if (!ok)
                break;
        }
    }
    
    ScenarioPhysicalCharacteristics* physicalcharacteristics = NULL;
    if (!hasError())
    {
        physicalcharacteristics = new ScenarioPhysicalCharacteristics();
        physicalcharacteristics->setMass(mass);
        physicalcharacteristics->setVolume(volume);
        physicalcharacteristics->setSurfaceArea(surfaceArea);
    }
    
    return physicalcharacteristics;
}

ScenarioGeometricalCharacteristics*
SpaceScenarioReader::readGeometricalCharacteristics()
{
    double radiusBase = 0.0;
    double radiusNose = 0.0;

    while (!atEnd())
    {
        readNext();
        if (isEndElement())
            break;

        if (isStartElement())
        {
            bool ok = true;

            if (name() == "radiusBase")
                ok = readDouble(&radiusBase);
            else if (name() == "radiusNose")
                ok = readDouble(&radiusNose);
            else
                skipUnknownElement();

            if (!ok)
                break;
        }
    }

    ScenarioGeometricalCharacteristics* geometricalcharacteristics = NULL;
    if (!hasError())
    {
        geometricalcharacteristics = new ScenarioGeometricalCharacteristics();
        geometricalcharacteristics->setRadiusBase(radiusBase);
        geometricalcharacteristics->setRadiusNose(radiusNose);
    }

    return geometricalcharacteristics;
}

ScenarioAerodynamicProperties*
SpaceScenarioReader::readAerodynamicProperties()
{
    QString CDCoefficients = "";
    double GLoadLimit = 0.0;
    double SPHeatRateLimit = 0.0;
    ScenarioParachuteProperties* parachuteProperties = new ScenarioParachuteProperties();

    while (!atEnd())
    {
        readNext();
        if (isEndElement())
            break;

        // Mass, surface, and volume can appear in any order, and all values are optional
        if (isStartElement())
        {
            bool ok = true;

            if (name() == "CDCoefficients")
                ok = readQString(&CDCoefficients);
            else if (name() == "GLoadLimit")
                ok = readDouble(&GLoadLimit);
            else if (name() == "SPHeatRateLimit")
                ok = readDouble(&SPHeatRateLimit);
            else if (name() == "ParachuteProperties")
                parachuteProperties = readParachuteProperties();
            else
                skipUnknownElement();

            if (!ok)
                break;
        }
    }
    ScenarioAerodynamicProperties* aerodynamicProperties = NULL;
    if (!hasError())
    {
        aerodynamicProperties = new ScenarioAerodynamicProperties();
        aerodynamicProperties->setCDCoefficients(CDCoefficients);
        aerodynamicProperties->setGLoadLimit(GLoadLimit);
        aerodynamicProperties->setSPHeatRateLimit(SPHeatRateLimit);
        aerodynamicProperties->setParachuteProperties(parachuteProperties);
    }

    return aerodynamicProperties;
}

ScenarioParachuteProperties*
SpaceScenarioReader::readParachuteProperties()
{
    double surfaceArea = 0.0;
    QString CDCoefficients;
    double deploymentMach = 0.0;

    nextElement();
    readDoubleElement("SurfaceArea", &surfaceArea);
    nextElement();
    readStringElement("CDCoefficients", &CDCoefficients);
    nextElement();
    readDoubleElement("DeploymentMach", &deploymentMach);
    nextElement();

    if (hasError())
    {
        return NULL;
    }
    else
    {
        ScenarioParachuteProperties * parachuteProperties = new ScenarioParachuteProperties();
        parachuteProperties ->setSurfaceArea(surfaceArea);
        parachuteProperties ->setCDCoefficients(CDCoefficients);
        parachuteProperties ->setDeploymentMach(deploymentMach);
        return parachuteProperties;
    }
}

ScenarioPropulsionProperties*
SpaceScenarioReader::readPropulsionProperties()
{
    unsigned int engineCount = 1;
    double isp = 0.0;
    double thrust = 0.0;
    double propellantMass = 0.0;
    double dV = 0.0;
    QString type;

    while (!atEnd())
    {
        readNext();
        if (isEndElement())
            break;

        // Propulsion properties can appear in any order
        // DragCoefficient, ReflectionCoefficient, Payload, and Propulsion can appear in any order,
        // and all values are optional.
        if (isStartElement())
        {
            bool ok = true;

            if (name() == "NumberOfEngines")
                ok = readUnsignedInt(&engineCount);
            else if (name() == "Isp")
                ok = readDouble(&isp);
            else if (name() == "ThrustPerEngine")
                ok = readDouble(&thrust);
            else if (name() == "PropellantMass")
                ok = readDouble(&propellantMass);
            else if (name() == "DV")
                ok = readDouble(&dV);
            else if (name() == "Type")
                type = readElementText();
            else
            {
                skipUnknownElement();
            }

            if (!ok)
                break;
        }
    }

    ScenarioPropulsionProperties* propulsionProperties = NULL;
    if (!hasError())
    {
        propulsionProperties = new ScenarioPropulsionProperties();
        propulsionProperties->setEngineCount(engineCount);
        propulsionProperties->setSpecificImpulse(isp);
        propulsionProperties->setThrustPerEngine(thrust);
        propulsionProperties->setPropellantMass(propellantMass);
        propulsionProperties->setDV(dV);
        propulsionProperties->setType(type);
    }

    return propulsionProperties;
}

ScenarioPayloadProperties*
SpaceScenarioReader::readPayloadProperties()
{
    double massPayload = 0.0;
    double powerPayload = 0.0;

    while (!atEnd())
    {
        readNext();
        if (isEndElement())
            break;

        // Payload properties can appear in any order
        // and all values are optional.
        if (isStartElement())
        {
            bool ok = true;

            if (name() == "PayloadMass")
                ok = readDouble(&massPayload);
            else if (name() == "PayloadPower")
                ok = readDouble(&powerPayload);
            else
            {
                skipUnknownElement();
            }

            if (!ok)
                break;
        }
    }

    ScenarioPayloadProperties* payloadProperties = NULL;
    if (!hasError())
    {
        payloadProperties = new ScenarioPayloadProperties();
        payloadProperties->setMassPayload(massPayload);
        payloadProperties->setPowerPayload(powerPayload);
    }

    return payloadProperties;
}

ScenarioDesignOverview*
SpaceScenarioReader::readDesignOverview()
{
    ScenarioDesignOverview* designOverview = new ScenarioDesignOverview();
    
    while (!atEnd())
    {
        readNext();
        if (isEndElement())
            break;
        
        if (isStartElement())
        {
            bool ok = true;
            
            if (name() == "Power" ||
                name() == "Thermal" ||
                name() == "Attitude" ||
                name() == "Communication" ||
                name() == "Propulsion" ||
                name() == "DataManagement" ||
                name() == "Structure")
            {
                QString subsystemName = name().toString();

                ScenarioSubsystem* subsystem = readSubsystem(subsystemName);
                if (!subsystem)
                {
                    ok = false;
                }
                else
                {
                    designOverview->setSubsystem(subsystemName, subsystem);
                    nextElement();
                }
            }
            else
            {
                raiseError("Unknown subsystem in design overview.\n");
                ok = false;
            }
                        
            if (!ok)
                break;
        }
    }
    
    if (hasError())
    {
        delete designOverview;
        return NULL;
    }
    else
    {
        return designOverview;
    }
}


ScenarioSubsystem*
SpaceScenarioReader::readSubsystem(QString name)
{
    QString budget;
    double value = 0.0;
    
    nextElement();
    readStringElement("budget", &budget);
    nextElement();
    readDoubleElement("value", &value);
    
    if (hasError())
    {
        return NULL;
    }
    else
    {
        ScenarioSubsystem* subsystem = new ScenarioSubsystem(name);
        subsystem->setBudget(budget);
        subsystem->setValue(value);
        
        return subsystem;
    }    
}


ScenarioPayload*
SpaceScenarioReader::readPayload()
{
    ScenarioPayload* payload = new ScenarioPayload();
    double mass = 0.0;

    nextElement();
    if (readDoubleElement("mass", &mass))
    {
        payload->setMass(mass);
        nextElement();
    }
    else
    {
        delete payload;
        return NULL;
    }

    return payload;
}


ScenarioEnvironment*
SpaceScenarioReader::readEnvironment()
{
    ScenarioEnvironment* environment = new ScenarioEnvironment();

    nextElement();
    if (isStartElement() && name() == "CentralBody")
    {
        ScenarioBody* centralBody = readBodyElement();
        if (centralBody != NULL)
        {
            environment->setCentralBody(centralBody);
            nextElement();
        }
    }
    else
    {
        raiseError(QObject::tr("Central body missing from environment."));
    }

    if (isStartElement() && name() == "GravityFieldPerturbation")
    {
        ScenarioGravityPerturbations* gravityField = new ScenarioGravityPerturbations();
        QString modelName;
        unsigned int zonalCount = 0;
        unsigned int tesseralCount = 0;

        nextElement();
        if (!expectedElement("ModelName"))
        {
            delete gravityField;
            return NULL;
        }
        modelName = readElementText();

        nextElement();
        readUnsignedIntElement("NumberOfZonals", &zonalCount);
        nextElement();
        readUnsignedIntElement("NumberOfTesserals", &tesseralCount);
        nextElement();

        gravityField->setModelName(modelName);
        gravityField->setZonalCount(zonalCount);
        gravityField->setTesseralCount(tesseralCount);
        gravityField->setCentralBody(environment->centralBody()->body());

        if (hasError())
        {
            delete gravityField;
            return NULL;
        }
        environment->addPerturbation(gravityField);
        nextElement();
    }

    if (isStartElement() && name() == "AtmosphericDragPerturbation")
    {
        ScenarioAtmosphericDragPerturbations* atmosphericDrag = new ScenarioAtmosphericDragPerturbations();

        QString modelName;
        nextElement();
        if (!expectedElement("ModelName"))
        {
            delete atmosphericDrag;
            return NULL;
        }
        modelName = readElementText();
        nextElement();
        atmosphericDrag->setAtmosphericModel(modelName);
        atmosphericDrag->setCentralBody(environment->centralBody()->body());

        environment->addPerturbation(atmosphericDrag);
        nextElement();
    }

    if (isStartElement() && name() == "SolarPressurePerturbation")
    {
        ScenarioSolarPressurePerturbations* solarPressure = new ScenarioSolarPressurePerturbations();
        solarPressure->setAlbedo(false);
        solarPressure->setIr(false);
        double reflectivity;
        nextElement();

        readDoubleElement("Reflectivity", &reflectivity);
        solarPressure->setReflectivity(reflectivity);
        solarPressure->setCentralBody(environment->centralBody()->body());
        nextElement();
        // Read the optional albedo and infra-red radiations
        while (!atEnd())
        {
           if (isEndElement()) break;

           if (isStartElement() && name() == "Albedo")
           {
               bool enabled;
               bool ok = readBoolean(&enabled);
               if (ok)
               {
                   solarPressure->setAlbedo(enabled);
                   nextElement();
               }
               else
               {
                   delete environment;
                   return NULL;
               }
           }

           else if (isStartElement() && name() == "Ir")
           {
               bool enabled;
               bool ok = readBoolean(&enabled);
               if (ok)
               {
                   solarPressure->setIr(enabled);
                   nextElement();
               }
               else
               {
                   delete environment;
                   return NULL;
               }
           }
           else raiseError(QObject::tr("Unrecognized element '%1' in environment.").arg(name().toString()));
       }
        environment->addPerturbation(solarPressure);
        nextElement();
    }

    if (isStartElement() && name() == "ExternalBodiesPerturbation")
    {
       ScenarioExternalBodyPerturbations* externalBody = new ScenarioExternalBodyPerturbations();
       externalBody->setCentralBody(environment->centralBody()->body());

       nextElement();
        // Read the optional list of perturbing bodies
       while (!atEnd())
       {
           if (isEndElement()) break;

           if (isStartElement() && name() == "PerturbingBody")
           {
               ScenarioBody* perturbingBody = readBodyElement();
               if (perturbingBody != NULL)
               {
                   externalBody->addPerturbingBody(perturbingBody);
                   nextElement();
               }
           }
           else raiseError(QObject::tr("Unrecognized element '%1' in environment.").arg(name().toString()));
       }
       environment->addPerturbation(externalBody);
       nextElement();
    }

    if (isStartElement() && name() == "DebrisPerturbation")
    {
        ScenarioDebrisPerturbations* spaceDebris = new ScenarioDebrisPerturbations();
        spaceDebris->setCentralBody(environment->centralBody()->body());

        environment->addPerturbation(spaceDebris);
        nextElement();
        nextElement();
    }

    if (hasError())
    {
        delete environment;
        environment = NULL;
    }

    return environment;
}


ScenarioThreebodyenvironment*
SpaceScenarioReader::readThreebodyenvironment()
{
    ScenarioThreebodyenvironment* threebodyenvironment = new ScenarioThreebodyenvironment();

    nextElement();
    if (isStartElement() && name() == "FirstBody")
    {
        ScenarioBody* firstBody = readBodyElement();
        if (firstBody != NULL)
        {
            threebodyenvironment->setFirstBody(firstBody);
            nextElement();
        }
    }
    else
    {
        raiseError(QObject::tr("First body missing from threebodyenvironment."));
    }

    if (isStartElement() && name() == "SecondBody")
    {
        ScenarioBody* secondBody = readBodyElement();
        if (secondBody != NULL)
        {
            threebodyenvironment->setSecondBody(secondBody);
            nextElement();
        }
    }
    else
    {
        raiseError(QObject::tr("Second body missing from threebodyenvironment."));
    }

    if (hasError())
    {
        delete threebodyenvironment;
        threebodyenvironment = NULL;
    }

    return threebodyenvironment;
}


ScenarioSimulationParameters*
SpaceScenarioReader::readSimulationParameters(int module)
{
    ScenarioSimulationParameters* simulationParameters = new ScenarioSimulationParameters(module);

    // Read the timeline
    nextElement();
    if (!expectedElement("Timeline"))
    {
        delete simulationParameters;
        return NULL;
    }
    ScenarioExtendedTimeline* timeline = readExtendedTimeline();
    if (timeline)
    {
        simulationParameters->setTimeline(timeline);
    }
    else
    {
        delete simulationParameters;
        return NULL;
    }

    // Read the initial position state
    nextElement();
    if (!expectedElement("InitialStatePosition"))
    {
        delete simulationParameters;
        return NULL;
    }
    ScenarioInitialStatePosition* initialStatePosition = readInitialStatePosition();
    if (initialStatePosition)
    {
        simulationParameters->setInitialStatePosition(initialStatePosition);
    }
    else
    {
        delete simulationParameters;
        return NULL;
    }
    nextElement();
    if (module==1)
    {nextElement();
        if (!expectedElement("Deltav"))
        {
            delete simulationParameters;
            return NULL;
        }
        ScenarioDeltav* deltav = readDeltav();
        if (deltav)
        {
            simulationParameters->setDeltav(deltav);
        }
        else
        {
            delete simulationParameters;
            return NULL;
        }
    }
    else
    {
    // Read the initial attitude state
    nextElement();
    if (!expectedElement("InitialStateAttitude"))
    {
        delete simulationParameters;
        return NULL;
    }
    ScenarioInitialStateAttitude* initialStateAttitude = readInitialStateAttitude();
    if (initialStateAttitude)
    {
        simulationParameters->setInitialStateAttitude(initialStateAttitude);
    }
    else
    {
        delete simulationParameters;
        return NULL;
    }
    }

    nextElement();

    return simulationParameters;
}


ScenarioTargettingSimulationParameters*
SpaceScenarioReader::readTargettingSimulationParameters()
{
    ScenarioTargettingSimulationParameters* simulationParameters = new ScenarioTargettingSimulationParameters();

    // Read the timeline
    nextElement();
    if (!expectedElement("Timeline"))
    {
        delete simulationParameters;
        return NULL;
    }
    ScenarioTimeline* timeline = readTimeline();
    if (timeline)
    {
        simulationParameters->setTimeline(timeline);
    }
    else
    {
        delete simulationParameters;
        return NULL;
    }

    // Read the initial position state
    nextElement();

    if (isStartElement() && name() == "InitialStatePosition")
    {
        ScenarioInitialStatePosition* initialStatePosition = readInitialStatePosition();
        if (initialStatePosition)
        {
           simulationParameters->setInitialStatePosition(initialStatePosition);
        }
        else
        {
            delete simulationParameters;
            return NULL;
        }
        nextElement();
    }

    if (isStartElement() && name() == "FinalState")
    {
        ScenarioFinalState* finalState = readFinalState();
        if (finalState)
        {
            simulationParameters->setFinalState(finalState);
            nextElement();
        }
        else
        {
            delete simulationParameters;
            return NULL;
        }
    }
    else
    {
        ScenarioFinalState* finalState = new ScenarioFinalState();
        finalState->setIsActiveFalse();
        simulationParameters->setFinalState(finalState);
    }

    return simulationParameters;
}



ScenarioHalo*
SpaceScenarioReader::readHalo()
{//nextElement();
    ScenarioHalo* halo = new ScenarioHalo();

    nextElement();
    if (!expectedElement("Lpoint"))
    {
        delete halo;
        return NULL;
    }

    QString lpoint;
    lpoint = readElementText();
    halo->setLpoint(lpoint);

   // Read the initial position state
    nextElement();
    if (!expectedElement("InitialStatePosition"))
    {
        delete halo;
        return NULL;
    }
    ScenarioInitialStatePosition* initialStatePosition = readInitialStatePosition();
    if (initialStatePosition)
    {
        halo->setInitialStatePosition(initialStatePosition);
        nextElement();
    }
    else
    {
        delete halo;
        return NULL;
    }

    // Read the Amplitudes
    nextElement();
#if 0
    if (!expectedElement("Amplitudes"))
    {
        delete halo;
        return NULL;
    }
#endif
    ScenarioAmplitudes* amplitudes = readAmplitudes();
    if (amplitudes)
    {
        halo->setAmplitudes(amplitudes);
    }
    else
    {
        delete halo;
        return NULL;
    }

    // Read Manifolds
    nextElement();
    if (!expectedElement("Manifolds"))
    {
        delete halo;
        return NULL;
    }
    ScenarioManifolds* manifolds = readManifolds();
    if (manifolds)
    {
        halo->setManifolds(manifolds);
    }
    else
    {
        delete halo;
        return NULL;
    }

    nextElement();

    return halo;
}


ScenarioThreebodyTransfer*
SpaceScenarioReader::readThreebodyTransfer()
{
    ScenarioThreebodyTransfer* threebodyTransfer = new ScenarioThreebodyTransfer();

    nextElement();
    if (!expectedElement("StartingEpoch"))
    {
        delete threebodyTransfer;
        return NULL;
    }

    QString startimeString = readElementText();
    QDateTime startime = QDateTime::fromString(startimeString, Qt::ISODate);
    if (!startime.isValid())
    {
        raiseError(QObject::tr("Invalid Starting Epoch given"));
        return NULL;
    }
    startime.setTimeSpec(Qt::UTC);

    //qDebug()<<startimeString<<startime;
    threebodyTransfer->setStartTime(sta::JdToMjd(sta::CalendarToJd(startime)));

    nextElement();
    if (!expectedElement("ParkingOrbit"))
    {
        delete threebodyTransfer;
        return NULL;
    }
    ScenarioParkingOrbit* parkingOrbit = readParkingOrbit();
    if (parkingOrbit)
    {
        threebodyTransfer->setParkingOrbit(parkingOrbit);
        nextElement();
    }
    else
    {
        delete threebodyTransfer;
        return NULL;
    }

    nextElement();
    if (!expectedElement("HaloOrbit"))
    {
        delete threebodyTransfer;
        return NULL;
    }

    ScenarioHaloOrbit* haloOrbit = readHaloOrbit();
    if (haloOrbit)
    {
        threebodyTransfer->setHaloOrbit(haloOrbit);
        nextElement();
    }
    else
    {
        delete threebodyTransfer;
        return NULL;
    }

    nextElement();
    if (!expectedElement("OptimizationParameters"))
    {
        delete threebodyTransfer;
        return NULL;
    }

    ScenarioOptimizationParameters* optimizationParameters = readOptimizationParameters();
    if (optimizationParameters)
    {
        threebodyTransfer->setOptimizationParameters(optimizationParameters);
        nextElement();
    }
    else
    {
        delete threebodyTransfer;
        return NULL;
    }

    nextElement();
    return threebodyTransfer;
}


ScenarioParkingOrbit*
SpaceScenarioReader::readParkingOrbit()
{
    ScenarioParkingOrbit* parkingOrbit = new ScenarioParkingOrbit();
    nextElement();

    if (!expectedElement("InitialStatePosition"))
    {
        delete parkingOrbit;
        return NULL;
    }

    ScenarioInitialStatePosition* initialStatePosition = readInitialStatePosition();
    if (initialStatePosition)
    {
        parkingOrbit->setInitialStatePosition(initialStatePosition);
        nextElement();
    }
    else
    {
        delete parkingOrbit;
        return NULL;
    }

    return parkingOrbit;
}


ScenarioHaloOrbit*
SpaceScenarioReader::readHaloOrbit()
{
    ScenarioHaloOrbit* haloOrbit = new ScenarioHaloOrbit();
    nextElement();

    if (!expectedElement("InitialStatePosition"))
    {
        delete haloOrbit;
        return NULL;
    }

    ScenarioInitialStatePosition* initialStatePosition = readInitialStatePosition();
    if (initialStatePosition)
    {
        haloOrbit->setInitialStatePosition(initialStatePosition);
        nextElement();
    }
    else
    {;
        return NULL;
    }
    
    return haloOrbit;
}


ScenarioOptimizationParameters*
SpaceScenarioReader::readOptimizationParameters()
{
    ScenarioOptimizationParameters* optimizationParameters = new ScenarioOptimizationParameters();

    bool TOF, propellant;
    unsigned int maxTOF;

    nextElement();
    bool tof = readBoolean(&TOF);

    nextElement();
    readUnsignedIntElement("maximumToF", &maxTOF);

    nextElement();
    bool prop = readBoolean(&propellant);

    if (!hasError())
    {
        optimizationParameters->setTimeofFlight(TOF);
        optimizationParameters->setMaximumToF(maxTOF);
        optimizationParameters->setPropellant(propellant);
    }

    return optimizationParameters;
}


ScenarioManifolds*
SpaceScenarioReader::readManifolds()
{//bool branchnew1; bool branchnew2; bool branchnew3; bool branchnew4;
//    ScenarioManifolds* manifolds = new ScenarioManifolds(true,true, true, true);
    ScenarioManifolds* manifolds = new ScenarioManifolds();


    nextElement();
    if (!expectedElement("Branch"))
    {
        delete manifolds;
        return NULL;
    }

    ScenarioBranch* branch = readBranch(1);
    //if (branch->state()=="true")
    {   // branchnew1=true;
    //else
      //  branchnew1=false;

    if (branch)
    {nextElement();
        manifolds->setBranch(branch);
    }

    else
    {
        delete manifolds;
        return NULL;
    }
    }

    nextElement();
    if (!expectedElement("Branch"))
    {
        delete manifolds;
        return NULL;
    }

    ScenarioBranch* branch2 = readBranch(2);
    //if (branch2->state()=="true")
    //    branchnew2=true;
    //else
    //    branchnew2=false;
    {

    if (branch2)
    {nextElement();
        manifolds->setBranch2(branch2);
    }


    }

    nextElement();
    if (!expectedElement("Branch"))
    {
        delete manifolds;
        return NULL;
    }
    ScenarioBranch* branch3 = readBranch(3);
    //if (branch3->state()=="true")
    //    branchnew2=true;
    //else
    //    branchnew2=false;
    {

    if (branch3)
    {nextElement();
        manifolds->setBranch3(branch3);
    }

    else
    {
        delete manifolds;
        return NULL;
    }
    }


    nextElement();
    if (!expectedElement("Branch"))
    {
        delete manifolds;
        return NULL;
    }
    ScenarioBranch* branch4 = readBranch(4);
    //if (branch4->state()=="true")
    //    branchnew2=true;
    //else
    //    branchnew2=false;
    {

    if (branch4)
    {nextElement();
        manifolds->setBranch4(branch4);
    }

    else
    {
        delete manifolds;
        return NULL;
    }
    }
    //ScenarioManifolds* manifolds1 = new ScenarioManifolds(branchnew1,branchnew2, false, false);
    //if (branchnew2==true)
     //   manifolds1->setBranch(branch);


    nextElement();

    return manifolds;
}


ScenarioBranch*
SpaceScenarioReader::readBranch(int numbranch)
{nextElement();
    ScenarioBranch* branch = new ScenarioBranch(numbranch);


    if (!expectedElement("State"))
    {
        delete branch;
        return NULL;
    }
    QString state;
    state = readElementText();
    branch->setState(state);


    nextElement();
    if (!expectedElement("Parameters"))
    {
        delete branch;
        return NULL;
    }

    ScenarioParameters *parameters = readParameters();
    if (parameters)
    {
        branch->setParameters(parameters);
    }
    else
    {
        delete branch;
        return NULL;
    }


    nextElement();

    return branch;
}


ScenarioTrajectoryPropagation*
SpaceScenarioReader::readTrajectoryPropagation()
{
    QString integrator;
    QString propagator;
    double timeStep = 0.0;

    // IntegratorType
    // Timestep
    // PropagatorType
    nextElement();
    if (!expectedElement("IntegratorType"))
        return NULL;
    integrator = readElementText();

    nextElement();
    if (!readDoubleElement("Timestep", &timeStep))
        return NULL;

    nextElement();
    if (!expectedElement("PropagatorType"))
        return NULL;
    propagator = readElementText();

    nextElement();

    ScenarioTrajectoryPropagation* prop = new ScenarioTrajectoryPropagation();
    prop->setIntegrator(integrator);
    prop->setTimeStep(timeStep);
    prop->setPropagator(propagator);

    return prop;
}

ScenarioInitialStatePosition*
SpaceScenarioReader::readInitialStatePosition()
{
    nextElement();
    
    ScenarioInitialStatePosition* state = new ScenarioInitialStatePosition();
    if (!expectedElement("CoordinateSystem"))
    {
        delete state;
        return NULL;
    }
    else
    {
        QString coordSysName = readElementText();
        sta::CoordinateSystem coordSys(coordSysName);
        if (!coordSys.valid())
        {
            raiseError(QObject::tr("Unknown coordinate system %1.").arg(coordSysName));
            delete state;
            return NULL;
        }
        
        state->setCoordinateSystem(coordSys);
    }

    nextElement();
    
    ScenarioAbstractInitialState* stateSpec = readInitialStateType();
    if (!stateSpec)
    {
        delete state;
        return NULL;
    }
    else
    {
        nextElement();
        state->setInitialState(stateSpec);
    }

        if (isStartElement() && name() == "CentralsystemBody")
    {
        ScenarioBody* centralsystemBody = readBodyElement();
        if (centralsystemBody != NULL)
        {
            state->setCentralsystemBody(centralsystemBody);

        }
    }
    else
    {
        raiseError(QObject::tr("Centralsystem body missing from Initial state."));
    }

    return state;
}


ScenarioAbstractInitialState*
SpaceScenarioReader::readInitialStateType()
{
    ScenarioAbstractInitialState* initialState = NULL;
    
    if (name() == "StateVector")
        initialState = readStateVector();
    else if (name() == "KeplerianElements")
        initialState = readKeplerianElements();
    else if (name() == "SphericalCoordinates")
        initialState = readSphericalCoordinates();
    else
        raiseError(QObject::tr("Unknown initial state type %1.").arg(name().toString()));
    
    return initialState;
}


ScenarioInitialStateAttitude*
SpaceScenarioReader::readInitialStateAttitude()
{
    skipUnknownElement();
    if (hasError())
        return NULL;
    else
        return new ScenarioInitialStateAttitude();
}    


ScenarioStateVector*
SpaceScenarioReader::readStateVector()
{
    ScenarioStateVector* state = new ScenarioStateVector();

    nextElement();
    readDoubleElement("x", &state->m_stateVector.position.x());
    nextElement();
    readDoubleElement("y", &state->m_stateVector.position.y());
    nextElement();
    readDoubleElement("z", &state->m_stateVector.position.z());
    nextElement();
    readDoubleElement("vx", &state->m_stateVector.velocity.x());
    nextElement();
    readDoubleElement("vy", &state->m_stateVector.velocity.y());
    nextElement();
    readDoubleElement("vz", &state->m_stateVector.velocity.z());
    nextElement();

    if (hasError())
    {
        delete state;
        return NULL;
    }
    else
    {
        return state;
    }

}


ScenarioSphericalCoordinates*
SpaceScenarioReader::readSphericalCoordinates()
{
    ScenarioSphericalCoordinates* coordinates = new ScenarioSphericalCoordinates();
    
    nextElement();
    readDoubleElement("Altitude", &coordinates->m_altitude);
    nextElement();
    readDoubleElement("Longitude", &coordinates->m_longitude);
    nextElement();
    readDoubleElement("Latitude", &coordinates->m_latitude);
    nextElement();
    readDoubleElement("InertialVelocity", &coordinates->m_inertialV);
    nextElement();
    readDoubleElement("InertialFlightPathAngle", &coordinates->m_inertialFPA);
    nextElement();
    readDoubleElement("InertialHeading", &coordinates->m_inertialH);
    nextElement();
    
    if (hasError())
    {
        delete coordinates;
        return NULL;
    }
    else
    {
        return coordinates;
    }
}

ScenarioKeplerianElements*
SpaceScenarioReader::readKeplerianElements()
{
    ScenarioKeplerianElements* elements = new ScenarioKeplerianElements();

    nextElement();
    readDoubleElement("SemiMajorAxis", &elements->m_semimajorAxis);
    nextElement();
    readDoubleElement("Eccentricity", &elements->m_eccentricity);
    nextElement();
    readDoubleElement("Inclination", &elements->m_inclination);
    nextElement();
    readDoubleElement("RAAN", &elements->m_raan);
    nextElement();
    readDoubleElement("ArgumentOfPeriapsis", &elements->m_argumentOfPeriapsis);
    nextElement();
    readDoubleElement("TrueAnomaly", &elements->m_trueAnomaly);
    nextElement();

    if (hasError())
    {
        delete elements;
        return NULL;
    }
    else
    {
        return elements;
    }
}

ScenarioFinalState*
SpaceScenarioReader::readFinalState()
{
    ScenarioFinalState* finalState = new ScenarioFinalState();
    finalState->setIsActiveTrue();
    double targetlongitude = 0.0;
    double targetlatitude = 0.0;

    nextElement();
    readDoubleElement("TargetLongitude", &targetlongitude);
    nextElement();
    readDoubleElement("TargetLatitude", &targetlatitude);
    nextElement();

    finalState->setTargetLon(targetlongitude);
    finalState->setTargetLat(targetlatitude);

    return finalState;
}

ScenarioExtendedTimeline*
SpaceScenarioReader::readExtendedTimeline()
{
    nextElement();
    if (!expectedElement("StartTime"))
        return NULL;
    QString startTimeString = readElementText();
    QDateTime startTime = QDateTime::fromString(startTimeString, Qt::ISODate);
    if (!startTime.isValid())
    {
        raiseError(QObject::tr("Invalid StartTime given in Timeline"));
        return NULL;
    }
    startTime.setTimeSpec(Qt::UTC);
    
    // TODO: StepTime should be renamed to TimeStep
    double timeStep = 0.0;
    nextElement();
    readDoubleElement("StepTime", &timeStep);

    nextElement();
    if (!expectedElement("EndTime"))
        return NULL;
    QString endTimeString = readElementText();
    QDateTime endTime = QDateTime::fromString(endTimeString, Qt::ISODate);
    if (!endTime.isValid())
    {
        raiseError(QObject::tr("Invalid EndTime given in Timeline"));
        return NULL;
    }
    endTime.setTimeSpec(Qt::UTC);
        
    ScenarioExtendedTimeline* timeline = NULL;
    
    if (!hasError())
    {
        timeline = new ScenarioExtendedTimeline();
        timeline->setStartTime(sta::JdToMjd(sta::CalendarToJd(startTime)));
        timeline->setEndTime(sta::JdToMjd(sta::CalendarToJd(endTime)));
        timeline->setTimeStep(timeStep);
        nextElement();
    }
    
    return timeline;
}


ScenarioTimeline*
SpaceScenarioReader::readTimeline()
{
    nextElement();
    if (!expectedElement("StartTime"))
        return NULL;

    QString startTimeString = readElementText();
    QDateTime startTime = QDateTime::fromString(startTimeString, Qt::ISODate);
    if (!startTime.isValid() || hasError())
    {
        raiseError(QObject::tr("Invalid StartTime given in Timeline"));
        return NULL;
    }
    startTime.setTimeSpec(Qt::UTC);
    
    // TODO: StepTime should be renamed to TimeStep
    nextElement();
    double timeStep = 0.0;
    readDoubleElement("StepTime", &timeStep);

    ScenarioTimeline* timeline = NULL;
    
    if (!hasError())
    {
        timeline = new ScenarioTimeline();
        timeline->setStartTime(sta::JdToMjd(sta::CalendarToJd(startTime)));
        timeline->setTimeStep(timeStep);
        nextElement();
    }
    
    return timeline;
}


ScenarioDeltav*
SpaceScenarioReader::readDeltav()
{
    nextElement();

    double time;
    readDoubleElement("Time", &time);

    nextElement();
    double Vx;
    readDoubleElement("X-Deltav", &Vx);

    nextElement();
    double Vy;
    readDoubleElement("Y-Deltav", &Vy);

    nextElement();
    double Vz;
    readDoubleElement("Z-Deltav", &Vz);

    ScenarioDeltav* deltav = NULL;

    if (!hasError())
    {
        deltav = new ScenarioDeltav();
        deltav->setTime(time);
        deltav->setXvelocity(Vx);
        deltav->setYvelocity(Vy);
        deltav->setZvelocity(Vz);
        nextElement();
    }

    return deltav;

}


ScenarioAmplitudes*
SpaceScenarioReader::readAmplitudes()
{
    nextElement();
    double ax;
    readDoubleElement("xAmplitude", &ax);

    nextElement();
    double az;
    readDoubleElement("zAmplitude", &az);

    ScenarioAmplitudes* amplitudes = NULL;

    if (!hasError())
    {
        amplitudes = new ScenarioAmplitudes();
        amplitudes->setAx(ax);
        amplitudes->setAz(az);
        nextElement();
    }

    return amplitudes;
}




ScenarioParameters*
SpaceScenarioReader::readParameters()
{
    ScenarioParameters* parameters = new ScenarioParameters();

    nextElement();


    if (!expectedElement("Deviation"))
    {
        delete parameters;
        return NULL;
    }

    double deviation;
    readDoubleElement("Deviation", &deviation);

    if (deviation)
    {
                parameters->setDeviation(deviation);
    }


    nextElement();

    if (!expectedElement("EndCondition"))
    {
        delete parameters;
        return NULL;
    }
    ScenarioEndCondition* endCondition = readEndCondition();

    if (endCondition)
    {
        parameters->setEndCondition(endCondition);
    }
    else
    {
        delete parameters;
        return NULL;
    }

    return parameters;
}


ScenarioEndCondition*
SpaceScenarioReader::readEndCondition()
{
    ScenarioEndCondition* endCondition = new ScenarioEndCondition();

    nextElement();


    if (!expectedElement("intersection"))
    {
        delete endCondition;
        return NULL;
    }

    bool intersection;
    bool ok = readBoolean(&intersection);

    if (ok)
    {
        endCondition->setIntersection(intersection);
    }
    else
    {
        delete endCondition;
        return NULL;
    }

    nextElement();
    double integrationTime;
    readDoubleElement("integrationTime", &integrationTime);

    nextElement();
    unsigned int numPositions;
    readUnsignedIntElement("numPositions", &numPositions);


    if (!hasError())
    {
        endCondition->setIntegrationTime(integrationTime);
        endCondition->setNumPositions(numPositions);
        nextElement();
    }

    return endCondition;
}


/*! Read a double element into value. The return value is true if the
 *  conversion was successful; otherwise, the method returns false
 *  and raises an XML read error.
 */
bool
SpaceScenarioReader::readDouble(double* value)
{
    QString text = readElementText();
    bool convertOk = false;
    double v = text.toDouble(&convertOk);
    if (convertOk)
        *value = v;
    else
        raiseError(QObject::tr("Numeric value expected."));
    
    return convertOk;
}


/*! Read a boolean element into value. The element contents must be
 *  either false, true, 0, or 1. The return value is true if the
 *  conversion was successful. Otherwise, the method returns false
 *  and raises an XML read error.
 */
bool
SpaceScenarioReader::readBoolean(bool* value)
{
    QString text = readElementText().trimmed();

    if (text == "true" || text == "1")
    {
        *value = true;
        return true;
    }
    else if (text == "false" || text == "0")
    {
        *value = false;
        return true;
    }
    else
    {
        raiseError(QObject::tr("Boolean value expected."));
        return false;
    }
}


/*! Read an unsigned integer into value. The return value is true if the
 *  conversion was successful; otherwise, the method returns false
 *  and raises an XML read error.
 */
bool
SpaceScenarioReader::readUnsignedInt(unsigned int* value)
{
    QString text = readElementText();
    bool convertOk = false;
    double v = text.toUInt(&convertOk);
    if (convertOk)
        *value = v;
    else
        raiseError(QObject::tr("Non-negative integer expected."));
    
    return convertOk;
}

/*! Read an QString.
 */
bool
SpaceScenarioReader::readQString(QString* value)
{
    QString text = readElementText();
    *value = text;
    bool convertOk = true;

    return convertOk;
}


bool
SpaceScenarioReader::readDoubleElement(const QString& elementName,
                                       double* value)
{
    if (!hasError())
    {
        if (expectedElement(elementName))
        {
            if (readDouble(value))
                return true;
        }
    }
    
    return false;
}


bool
SpaceScenarioReader::readUnsignedIntElement(const QString& elementName,
                                           unsigned int* value)
{
    if (!hasError())
    {
        if (expectedElement(elementName))
        {
            if (readUnsignedInt(value))
                return true;
        }
    }
    
    return false;
}


bool
SpaceScenarioReader::readStringElement(const QString& elementName,
                                       QString* value)
{
    if (!hasError())
    {
        if (expectedElement(elementName))
        {
            *value = readElementText().trimmed();
            return true;
        }
    }
    
    return false;
}


/*! Read a CentralBody or PerturbingBody definition.
 */
ScenarioBody*
SpaceScenarioReader::readBodyElement()
{
    QString bodyName = attributes().value("Name").toString();

    if (bodyName.isEmpty())
    {
        raiseError(QObject::tr("Body name is missing."));
    }

    StaBody* stabody = STA_SOLAR_SYSTEM->lookup(bodyName);
    if (stabody == NULL)
    {
        raiseError(QObject::tr("Unrecognized body '%1'.").arg(bodyName));
        return NULL;
    }
    
    ScenarioBody* body = new ScenarioBody(stabody);
    nextElement();  

    return body;
}


/*! Parse the element text and return an STA body; return NULL if the
 *  body name doesn't match any known object.
 */
StaBody* 
SpaceScenarioReader::readBody()
{
    QString bodyName = readElementText();
    StaBody* body = STA_SOLAR_SYSTEM->lookup(bodyName);
    if (body == NULL)
    {
        raiseError(QObject::tr("Unrecognized body '%1'.").arg(bodyName));
        return NULL;
    }
    else
    {
        return body;
    }
}


//ScenarioDeviations*
//SpaceScenarioReader::readDeviations()
//{
//    double capsulebaseradius = 0.0;
//    double capsulemass = 0.0;
//    double capsuleCD = 10.0;
//    double parachuteCD = 0.0;
//    double atmospheredensity = 0.0;
//    double entrylong = 0.0;
//    double entrylat = 0.0;
//    double velocityentrylat = 0.0;
//    double flightpathangle = 0.0;
//    double inertialheading = 0.0;
//
//    ScenarioDeviations* deviations = new ScenarioDeviations();
//
//    nextElement();
//    readDoubleElement("CapsuleBaseRadius", &capsulebaseradius);
//    nextElement();
//    readDoubleElement("CapsuleMass", &capsulemass);
//    nextElement();
//    readDoubleElement("CapsuleCD", &capsuleCD);
//    nextElement();
//    readDoubleElement("ParachuteCD", &parachuteCD);
//    nextElement();
//    readDoubleElement("AtmosphereDensity", &atmospheredensity);
//    nextElement();
//    readDoubleElement("EntryLong", &entrylong);
//    nextElement();
//    readDoubleElement("EntryLat", &entrylat);
//    nextElement();
//    readDoubleElement("Velocity", &velocityentrylat);
//    nextElement();
//    readDoubleElement("FlightPathAngle", &flightpathangle);
//    nextElement();
//    readDoubleElement("InertialHeading", &inertialheading);
//    nextElement();
//
//    if (hasError())
//    {
//        return NULL;
//    }
//    else
//    {
//        deviations->setCapsuleBaseRadius(capsulebaseradius);
//        deviations->setCapsuleMass(capsulemass);
//        deviations->setCapsuleCD(capsuleCD);
//        deviations->setParachuteCD(parachuteCD);
//        deviations->setAtmosphereDensity(atmospheredensity);
//        deviations->setEntryLong(entrylong);
//        deviations->setEntryLat(entrylat);
//        deviations->setVelocity(velocityentrylat);
//        deviations->setFlightPathAngle(flightpathangle);
//        deviations->setInertialHeading(inertialheading);
//
//        return deviations;
//    }
//}
//
//ScenarioFixedVariables*
//SpaceScenarioReader::readFixedVariables()
//{
//    double ballisticCoeff = 0.0;
//    double entryAltitude = 0.0;
//    double inertialVelocity = 0.0;
//    double inertialFlightPathAngle = 0.0;
//    double inertialHeading = 0.0;
//
//    ScenarioFixedVariables* fixedVariables = new ScenarioFixedVariables();
//
//    nextElement();
//    readDoubleElement("BallisticCoeff", &ballisticCoeff);
//    nextElement();
//    readDoubleElement("EntryAltitude", &entryAltitude);
//    nextElement();
//    readDoubleElement("InertialVelocity", &inertialVelocity);
//    nextElement();
//    readDoubleElement("InertialFlightPathAngle", &inertialFlightPathAngle);
//    nextElement();
//    readDoubleElement("InertialHeading", &inertialHeading);
//    nextElement();
//
//    if (hasError())
//    {
//        return NULL;
//    }
//    else
//    {
//        fixedVariables->setBallisticCoeff(ballisticCoeff);
//        fixedVariables->setEntryAltitude(entryAltitude);
//        fixedVariables->setInertialVelocity(inertialVelocity);
//        fixedVariables->setInertialFlightPathAngle(inertialFlightPathAngle);
//        fixedVariables->setInertialHeading(inertialHeading);
//    }
//    return fixedVariables;
//}
//
//ScenarioWindow*
//SpaceScenarioReader::readWindow()
//{
//    ScenarioWindow* window = new ScenarioWindow();
//    double minX = 0.0;
//    double maxX = 0.0;
//    double minY = 0.0;
//    double maxY = 0.0;
//
//    nextElement();
//    ReentryTrajectoryParamType xType = readReentryTrajectoryParamType();
//    ScenarioAbstractReentryTrajectoryParam* xAxisVariable = new ScenarioAbstractReentryTrajectoryParam(xType);
//
//    nextElement();
//    readDoubleElement("min", &minX);
//    nextElement();
//    readDoubleElement("max", &maxX);
//
//    nextElement();
//    nextElement();
//    ReentryTrajectoryParamType yType = readReentryTrajectoryParamType();
//    ScenarioAbstractReentryTrajectoryParam* yAxisVariable = new ScenarioAbstractReentryTrajectoryParam(yType);
//    nextElement();
//    readDoubleElement("min", &minY);
//    nextElement();
//    readDoubleElement("max", &maxY);
//    nextElement();
//
//    xAxisVariable->setMin(minX);
//    xAxisVariable->setMax(maxX);
//    yAxisVariable->setMin(minY);
//    yAxisVariable->setMax(maxY);
//    window->setXAxisVariable(xAxisVariable);
//    window->setYAxisVariable(yAxisVariable);
//    return window;
//}
//
//ReentryTrajectoryParamType
//SpaceScenarioReader::readReentryTrajectoryParamType()
//{
//    ReentryTrajectoryParamType reentryTrajectoryParamType;
//    if (name() == "CapsuleBallisticCoeff")
//        reentryTrajectoryParamType = CAPSULE_NOSE_RADIUS;
//    else if (name() == "CapsuleNoseRadius")
//        reentryTrajectoryParamType = CAPSULE_BALLISTIC_COEFFICIENT;
//    else if (name() == "InertialEntryVelocity")
//        reentryTrajectoryParamType = INERTIAL_ENTRY_VELOCITY;
//    else if (name() == "InertialFlightPathAngle")
//        reentryTrajectoryParamType = INERTIAL_FLIGHT_PATH_ANGLE;
//    else if (name() == "InertialHeading")
//        reentryTrajectoryParamType =  INERTIAL_HEADING;
//
//    return reentryTrajectoryParamType;
//}

void
SpaceScenarioReader::nextElement()
{
    while (!atEnd())
    {
        readNext();
        if (isStartElement() || isEndElement())
            break;
    }
}

           
void
SpaceScenarioReader::skipUnknownElement()
{
    while (!atEnd())
    {
        readNext();
        
        if (isEndElement())
            break;
        if (isStartElement())
            skipUnknownElement();
    }
}


/*! Check that the current node type is a start element matching the
 *  specified kind. If it isn't raise an error and return false.
 */
bool
SpaceScenarioReader::expectedElement(const QString& elementName)
{
    if (!isStartElement())
    {
        // We read an end element; proably means that the expected element
        // was missing from a complex type.
        raiseError(QObject::tr("Missing %1 element in %2").arg(elementName).arg(name().toString()));
        return false;
    }
    else if (name() != elementName)
    {
        raiseError(QObject::tr("Expected %1 element but read %2.").arg(elementName).arg(name().toString()));
        return false;
    }
    else
    {
        return true;
    }
}
