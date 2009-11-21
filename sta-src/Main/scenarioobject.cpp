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

#include "scenarioobject.h"
#include <QXmlStreamReader>
#include <QXmlStreamWriter>
#include <QTreeWidget>


/*** ScenarioObject ***/

ScenarioObject::~ScenarioObject()
{
}


void
ScenarioObject::attach(ScenarioObject* parent)
{
    Q_ASSERT(!isAttached());  // Detach must be called first
    Q_ASSERT(parent != this); // Avoid infinite recursion
    m_parent = parent;
}


void
ScenarioObject::detach()
{
    Q_ASSERT(isAttached());  // Flag multiple detaches
    m_parent = NULL;
}


void
ScenarioObject::attachChild(ScenarioObject* child)
{
    if (child != NULL)
    {
        Q_ASSERT(!child->isAttached());  // Detach must be called first
        Q_ASSERT(child != this);         // Avoid infinite recursion
        child->m_parent = this;
    }
}


void
ScenarioObject::detachChild(ScenarioObject* child)
{
    if (child != NULL)
    {
        Q_ASSERT(child->parent() == this);
        child->m_parent = NULL;
    }
}


QTreeWidgetItem* ScenarioObject::createItem(QTreeWidgetItem* parent)
{
    QTreeWidgetItem* item = createItemSelf(parent);
    createItemContents(item);
        
    return item;
}


QTreeWidgetItem* ScenarioObject::createItemSelf(QTreeWidgetItem* parent)
{
    QTreeWidgetItem* item = new QTreeWidgetItem(parent);
    item->setText(0, displayName());
    
    // Store a pointer to the this ScenarioObject in the widget item.
    // This will be used to map a tree widget item to a ScearioObject.
    // IMPORTANT: The ScenarioObject must not be deleted before the tree
    // widget item that references it.
    QVariant scenarioObjectData = qVariantFromValue(reinterpret_cast<void*>(this));
    item->setData(0, ScenarioObjectRole, scenarioObjectData);
    
    return item;
}

void ScenarioObject::createItemContents(QTreeWidgetItem* /* item */)
{
}


/*** SpaceScenario ***/

SpaceScenario::~SpaceScenario()
{
    foreach (ScenarioParticipant* participant, m_participantList)
    {
        delete participant;
    }
}


QTreeWidgetItem* SpaceScenario::createItemSelf(QTreeWidgetItem* parent)
{
    // Top-level item; don't create anything
    return parent;
}


void SpaceScenario::createItemContents(QTreeWidgetItem* item)
{
    foreach (ScenarioParticipant* participant, m_participantList)
    {
        participant->createItem(item);
    }
}


void 
SpaceScenario::addParticipant(ScenarioParticipant* participant)
{
    Q_ASSERT(participant != NULL);
    Q_ASSERT(!participant->isAttached());
    attachChild(participant);
    m_participantList.append(participant);
}


/*** ScenarioParticipant ***/

/*** ScenarioGroundElement ***/

ScenarioGroundElement::ScenarioGroundElement() :
    m_location(NULL)
{
}


ScenarioGroundElement::~ScenarioGroundElement()
{
}


void
ScenarioGroundElement::createItemContents(QTreeWidgetItem* item)
{
    if (m_location)
        m_location->createItem(item);
}


void
ScenarioGroundElement::setLocation(ScenarioLocation* location)
{
    if (location != m_location)
    {
        detachChild(m_location);
        m_location = location;
        attachChild(m_location);
    }    
}



/*** ScenarioLaunchPad ***/

void ScenarioLaunchPad::createItemContents(QTreeWidgetItem* item)
{
    ScenarioGroundElement::createItemContents(item);
    
    QTreeWidgetItem* clearingAltitudeItem = new QTreeWidgetItem(item);
    clearingAltitudeItem->setText(0, QObject::tr("Clearing Altitude"));
    clearingAltitudeItem->setText(1, QString::number(m_clearingAltitude));
}



/*** ScenarioSpaceVehicle ***/

ScenarioSpaceVehicle::ScenarioSpaceVehicle() :
    m_trajectoryPlan(NULL),
    m_additionalComponents(NULL)
{
}


ScenarioSpaceVehicle::~ScenarioSpaceVehicle()
{
}


void
ScenarioSpaceVehicle::setTrajectoryPlan(ScenarioTrajectoryPlan* trajectoryPlan)
{
    if (trajectoryPlan != m_trajectoryPlan)
    {
        detachChild(m_trajectoryPlan);
        m_trajectoryPlan = trajectoryPlan;
        attachChild(m_trajectoryPlan);
    }
}


void
ScenarioSpaceVehicle::setAdditionalComponents(ScenarioAdditionalComponents* additionalComponents)
{
    if (additionalComponents != m_additionalComponents)
    {
        detachChild(m_additionalComponents);
        m_additionalComponents = additionalComponents;
        attachChild(m_additionalComponents);
    }
}


void ScenarioSpaceVehicle::createItemContents(QTreeWidgetItem* item)
{
    if (m_trajectoryPlan)
        m_trajectoryPlan->createItem(item);
    if (m_additionalComponents)
        m_additionalComponents->createItem(item);
}


/*** ScenarioSatellite ***/

ScenarioSatellite::ScenarioSatellite() :
    m_properties(NULL)
{
}


ScenarioSatellite::~ScenarioSatellite()
{
}


void ScenarioSatellite::setProperties(ScenarioProperties* properties)
{
    if (properties != m_properties)
    {
        detachChild(m_properties);
        m_properties = properties;
        attachChild(m_properties);
    }
}


void ScenarioSatellite::createItemContents(QTreeWidgetItem* item)
{
    if (m_properties)
        m_properties->createItem(item);
    ScenarioSpaceVehicle::createItemContents(item);
}



/*** ScenarioTrajectoryPlan ***/

ScenarioTrajectoryPlan::~ScenarioTrajectoryPlan()
{
}


void ScenarioTrajectoryPlan::createItemContents(QTreeWidgetItem* item)
{
    foreach (ScenarioTrajectory* trajectory, m_trajectoryList)
    {
        trajectory->createItem(item);
    }
}


void 
ScenarioTrajectoryPlan::addTrajectory(ScenarioTrajectory* trajectory)
{
    Q_ASSERT(trajectory != NULL);
    Q_ASSERT(!trajectory->isAttached());
    attachChild(trajectory);
    m_trajectoryList.append(trajectory);
}


/*** ScenarioLocation ***/

ScenarioLocation::ScenarioLocation() :
    m_centralBody(STA_SOLAR_SYSTEM->lookup("Earth"))
{
}


ScenarioLocation::~ScenarioLocation()
{
}


void ScenarioLocation::createItemContents(QTreeWidgetItem* item)
{
    QTreeWidgetItem* centralBodyItem = new QTreeWidgetItem(item);
    centralBodyItem->setText(0, QObject::tr("Central Body"));
    centralBodyItem->setText(1, m_centralBody->name());
    
    QTreeWidgetItem* latitudeItem = new QTreeWidgetItem(item);
    latitudeItem->setText(0, QObject::tr("Latitude"));
    latitudeItem->setText(1, QString::number(m_groundPosition.latitude));

    QTreeWidgetItem* longitudeItem = new QTreeWidgetItem(item);
    longitudeItem->setText(0, QObject::tr("Longitude"));
    longitudeItem->setText(1, QString::number(m_groundPosition.longitude));

    QTreeWidgetItem* altitudeItem = new QTreeWidgetItem(item);
    altitudeItem->setText(0, QObject::tr("Altitude"));
    altitudeItem->setText(1, QString::number(m_groundPosition.altitude));
}



/*** ScenarioTrajectory ***/

ScenarioTrajectory::ScenarioTrajectory() :
    m_environment(NULL),
    m_simulationParameters(NULL),
    m_optimizationParameters(NULL),
    m_trajectoryPropagation(NULL)
{
}


void 
ScenarioTrajectory::createItemContents(QTreeWidgetItem* item)
{
    if (m_environment)
        m_environment->createItem(item);
    if (m_simulationParameters)
        m_simulationParameters->createItem(item);
    // TODO: if (m_optimizationParameters)
    //           m_optimizationParameters->createItem(item);
    if (m_trajectoryPropagation)
        m_trajectoryPropagation->createItem(item);
}


void
ScenarioTrajectory::setEnvironment(ScenarioEnvironment* environment)
{
    if (environment != m_environment)
    {
        detachChild(m_environment);
        m_environment = environment;
        attachChild(m_environment);
    }    
}


void
ScenarioTrajectory::setSimulationParameters(ScenarioSimulationParameters* simulationParameters)
{
    if (simulationParameters != m_simulationParameters)
    {
        detachChild(m_simulationParameters);
        m_simulationParameters = simulationParameters;
        attachChild(m_simulationParameters);
    }    
}


void
ScenarioTrajectory::setTrajectoryPropagation(ScenarioTrajectoryPropagation* trajectoryPropagation)
{
    if (trajectoryPropagation != m_trajectoryPropagation)
    {
        detachChild(m_trajectoryPropagation);
        m_trajectoryPropagation = trajectoryPropagation;
        attachChild(m_trajectoryPropagation);
    }    
}



/*** ScenarioEnvironment ***/

ScenarioEnvironment::ScenarioEnvironment()
{
}


void
ScenarioEnvironment::createItemContents(QTreeWidgetItem* item)
{
    QTreeWidgetItem* centralBodyItem = new QTreeWidgetItem(item);
    centralBodyItem->setText(0, QObject::tr("Central Body"));
    centralBodyItem->setText(1, m_centralBody->name());    
}



/*** ScenarioSimulationParameters ***/

ScenarioSimulationParameters::ScenarioSimulationParameters() :
    m_timeline(NULL),
    m_initialStatePosition(NULL),
    m_initialStateAttitude(NULL)
{
}


ScenarioSimulationParameters::~ScenarioSimulationParameters()
{
}


void
ScenarioSimulationParameters::createItemContents(QTreeWidgetItem* item)
{
    if (m_timeline)
        m_timeline->createItem(item);
    if (m_initialStatePosition)
        m_initialStatePosition->createItem(item);
    if (m_initialStateAttitude)
        m_initialStateAttitude->createItem(item);
}


void
ScenarioSimulationParameters::setTimeline(ScenarioTimeline* timeline)
{
    if (timeline != m_timeline)
    {
        detachChild(m_timeline);
        m_timeline = timeline;
        attachChild(m_timeline);
    }    
}


void
ScenarioSimulationParameters::setInitialStatePosition(ScenarioInitialStatePosition* state)
{
    if (state != m_initialStatePosition)
    {
        detachChild(m_initialStatePosition);
        m_initialStatePosition = state;
        attachChild(m_initialStatePosition);
    }    
}



/*** ScenarioTimeline ***/

ScenarioTimeline::ScenarioTimeline() :
    m_startTime(0.0),
    m_endTime(0.0),
    m_timeStep(0.0)
{
}



/*** ScenarioTrajectoryPropagation ***/

ScenarioTrajectoryPropagation::ScenarioTrajectoryPropagation()
{
}


void
ScenarioTrajectoryPropagation::createItemContents(QTreeWidgetItem* item)
{
    QTreeWidgetItem* propagatorItem = new QTreeWidgetItem(item);
    propagatorItem->setText(0, QObject::tr("Propagator"));
    propagatorItem->setText(1, m_propagator);
    
    QTreeWidgetItem* integratorItem = new QTreeWidgetItem(item);
    integratorItem->setText(0, QObject::tr("Integrator"));
    integratorItem->setText(1, m_integrator);
    
    QTreeWidgetItem* timeStepItem = new QTreeWidgetItem(item);
    timeStepItem->setText(0, QObject::tr("Time Step"));
    timeStepItem->setText(1, QString::number(m_timeStep));    
}



/*** ScenarioInitialStatePosition ***/

ScenarioInitialStatePosition::ScenarioInitialStatePosition() :
    m_coordinateSystem(SCENARIO_EME_J2000),
    m_initialState(NULL)
{
}


void
ScenarioInitialStatePosition::createItemContents(QTreeWidgetItem* item)
{
    QTreeWidgetItem* coordSysItem = new QTreeWidgetItem(item);
    coordSysItem->setText(0, QObject::tr("Coordinate System"));
    coordSysItem->setText(1, CoordinateSystemToString(m_coordinateSystem));
    
    if (m_initialState)
        m_initialState->createItem(item);
}


void
ScenarioInitialStatePosition::setInitialState(ScenarioAbstractInitialState* initialState)
{
    if (initialState != m_initialState)
    {
        detachChild(m_initialState);
        m_initialState = initialState;
        attachChild(m_initialState);
    }    
}



/*** ScenarioKeplerianElements ***/

ScenarioKeplerianElements::ScenarioKeplerianElements() :
    m_semimajorAxis(0.0),
    m_eccentricity(0.0),
    m_inclination(0.0),
    m_raan(0.0),
    m_argumentOfPeriapsis(0.0),
    m_trueAnomaly(0.0)
{
}


static void createDoubleItem(QTreeWidgetItem* parent, const QString& name, double value)
{
    QTreeWidgetItem* item = new QTreeWidgetItem(parent);
    item->setText(0, name);
    item->setText(1, QString::number(value));
}


void
ScenarioKeplerianElements::createItemContents(QTreeWidgetItem* item)
{
    createDoubleItem(item, QObject::tr("Semimajor Axis"),   m_semimajorAxis);
    createDoubleItem(item, QObject::tr("Eccentricity"),     m_eccentricity);
    createDoubleItem(item, QObject::tr("Inclination"),      m_inclination);
    createDoubleItem(item, QObject::tr("RAAN"),             m_raan);
    createDoubleItem(item, QObject::tr("Arg of Periapsis"), m_argumentOfPeriapsis);
    createDoubleItem(item, QObject::tr("True Anomaly"),     m_trueAnomaly);
}



/*** SpaceScenarioReader ***/

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
                scenario = readParticipantsList();
                if (scenario)
                    scenario->setName(attributes().value("Name").toString());
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
    
    if (name() == "Satellite")
        participant = readSatellite();
    else if (name() == "GroundStation")
        participant = readGroundStation();
    else if (name() == "Target")
        participant = readTarget();
    else if (name() == "LaunchPad")
        participant = readLaunchPad();
    else if (name() == "ReEntryVehicle")
        participant = readReentryVehicle();
    else if (name() == "Launcher")
        participant = readLauncher();
    else
        raiseError(QObject::tr("Unknown element %1 in participants list.").arg(name().toString()));
    
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


void
SpaceScenarioReader::readSpaceVehicle(ScenarioSpaceVehicle* spaceVehicle)
{
    ScenarioTrajectoryPlan* trajectoryPlan = NULL;
    ScenarioAdditionalComponents* additionalComponents = NULL;
    
    nextElement();
    if (isStartElement() && name() == "TrajectoryPlan")
    {
        trajectoryPlan = readTrajectoryPlan();
        nextElement();
    }
    else
    {
        raiseError("TrajectoryPlan missing for space vehicle.");
    }
    
    if (isStartElement() && name() == "AdditionalComponents")
    {
        additionalComponents = readAdditionalComponents();
        nextElement();
    }
    
    spaceVehicle->setTrajectoryPlan(trajectoryPlan);
    spaceVehicle->setAdditionalComponents(additionalComponents);
}


ScenarioSatellite*
SpaceScenarioReader::readSatellite()
{
    ScenarioSatellite* satellite = new ScenarioSatellite();
    
    readSpaceVehicle(satellite);

    if (!hasError())
    {
        if (isStartElement() && name() == "Properties")
        {
            ScenarioProperties* properties = readProperties();
            satellite->setProperties(properties);
            nextElement();
        }
        else
        {
            raiseError("Properties missing for satellite.");
        }
        
        if (!hasError() && !isEndElement())
            raiseError(QObject::tr("Unknown element in satellite: %1.").arg(name().toString()));
    }
    
    if (hasError())
    {
        delete satellite;
        satellite = NULL;
    }
    
    return satellite;
}


ScenarioLauncher*
SpaceScenarioReader::readLauncher()
{
    skipUnknownElement();
    return new ScenarioLauncher();
}


ScenarioReentryVehicle*
SpaceScenarioReader::readReentryVehicle()
{
    skipUnknownElement();
    return new ScenarioReentryVehicle();
}


ScenarioTrajectoryPlan*
SpaceScenarioReader::readTrajectoryPlan()
{
    ScenarioTrajectoryPlan* trajectoryPlan = new ScenarioTrajectoryPlan();
        
    while (!atEnd())
    {
        readNext();
        if (isEndElement())
            break;
        
        if (isStartElement())
        {
            if (name() == "Trajectory")
            {
                ScenarioTrajectory* trajectory = readTrajectory();
                if (trajectory != NULL)
                {
                    trajectoryPlan->addTrajectory(trajectory);
                }
                else
                {
                    // Otherwise an error must have occurred
                }
            }
            else
            {
                raiseError(QObject::tr("Unknown element in trajectory plan: '%1'").arg(name().toString()));
            }
        }
    }

    if (hasError())
    {
        delete trajectoryPlan;
        trajectoryPlan = NULL;
    }
    
    return trajectoryPlan;
}


ScenarioTrajectory*
SpaceScenarioReader::readTrajectory()
{
    ScenarioTrajectory* trajectory = new ScenarioTrajectory();
    
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
        
    ScenarioSimulationParameters* simulationParameters = readSimulationParameters();
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
    if (expectedElement("AttitudePropagation"))
    {
        skipUnknownElement();
        nextElement();
    }
    
    if (expectedElement("ImpulsiveManeuver"))
    {
        skipUnknownElement();
        nextElement();
    }
    
    if (hasError())
    {
        delete trajectory;
        return NULL;
    }

    return trajectory;
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
    skipUnknownElement();
    return new ScenarioProperties();
}


ScenarioEnvironment*
SpaceScenarioReader::readEnvironment()
{
    ScenarioEnvironment* environment = new ScenarioEnvironment();
    
    nextElement();
    if (isStartElement() && name() == "CentralBody")
    {
        StaBody* centralBody = readBodyElement();
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
    
    // Read the optional list of perturbing bodies
    while (!atEnd())
    {
        if (isEndElement())
            break;
        
        if (isStartElement() && name() == "PerturbingBody")
        {
            StaBody* perturbingBody = readBody();
            if (perturbingBody != NULL)
            {
                // environment->addPerturbingBody(perturbingBody);
                nextElement();
            }
        }
        else
        {
            raiseError(QObject::tr("Unrecognized element '%1' in environment.").arg(name().toString()));
        }
    }
    
    if (hasError())
    {
        delete environment;
        environment = NULL;
    }
    
    return environment;
}


ScenarioSimulationParameters*
SpaceScenarioReader::readSimulationParameters()
{
    ScenarioSimulationParameters* simulationParameters = new ScenarioSimulationParameters();
    
    // Read the timeline
    nextElement();
    if (!expectedElement("Timeline"))
    {
        delete simulationParameters;
        return NULL;
    }

    skipUnknownElement();
    if (hasError())
    {
        delete simulationParameters;
        return NULL;
    }
    simulationParameters->setTimeline(new ScenarioTimeline());
    
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
    
    // Read the initial attitude state
    nextElement();
    if (!expectedElement("InitialStateAttitude"))
    {
        delete simulationParameters;
        return NULL;
    }
    
    // TODO: Not yet implemented
    skipUnknownElement();
    if (hasError())
    {
        delete simulationParameters;
        return NULL;
    }
    
    nextElement();
    
    return simulationParameters;
}


ScenarioTrajectoryPropagation*
SpaceScenarioReader::readTrajectoryPropagation()
{
    skipUnknownElement();
    return new ScenarioTrajectoryPropagation();
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
        ScenarioCoordinateSystem coordSys = StringToCoordinateSystem(coordSysName);
        if (coordSys == SCENARIO_INVALID_COORDINATE_SYSTEM)
        {
            raiseError(QObject::tr("Unknown coordinate system %1.").arg(coordSysName));
            delete state;
            return NULL;
        }
        
        
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
    else
        raiseError(QObject::tr("Unknown initial state type %1.").arg(name().toString()));
    
    return initialState;
}


ScenarioStateVector*
SpaceScenarioReader::readStateVector()
{
    skipUnknownElement();
    return new ScenarioStateVector();
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


/*! Read a CentralBody or PerturbingBody definition. The current spec
 *  allows specifying an atmosphere model and gravity model for these
 *  objects, but those are ignored at the moment.
 */
StaBody*
SpaceScenarioReader::readBodyElement()
{
    QString bodyName = attributes().value("Name").toString();
    if (bodyName.isEmpty())
    {
        raiseError(QObject::tr("Body name is missing."));
    }
    
    StaBody* body = STA_SOLAR_SYSTEM->lookup(bodyName);
    if (body == NULL)
    {
        raiseError(QObject::tr("Unrecognized body '%1'.").arg(bodyName));
        return NULL;
    }
    else
    {
        // TODO: Need to read atmosphere and gravity model definitions
        skipUnknownElement();
        return body;
    }
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
            

QString CoordinateSystemToString(ScenarioCoordinateSystem coordSys)
{
    switch (coordSys)
    {
    case SCENARIO_BODYFIXED:
        return "PLANETO FIXED";
    case SCENARIO_EME_J2000:
        return "INERTIAL J2000";
    case SCENARIO_EME_B1950:
        return "INERTIAL B1950";
    case SCENARIO_ECLIPTIC_J2000:
        return "ECLIPTIC";
    default:
        return "INVALID";
    }
}

ScenarioCoordinateSystem StringToCoordinateSystem(QString coordSysName)
{
    QString lowerName = coordSysName.toLower();
    
    if (lowerName == "planeto fixed")
        return SCENARIO_BODYFIXED;
    else if (lowerName == "inertial j2000")
        return SCENARIO_EME_J2000;
    else if (lowerName == "inertial b1950")
        return SCENARIO_EME_B1950;
    else if (lowerName == "ecliptic")
        return SCENARIO_ECLIPTIC_J2000;
    else
        return SCENARIO_INVALID_COORDINATE_SYSTEM;
}


