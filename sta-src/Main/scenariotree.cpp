/*
 This program is free software; you can redistribute it and/or modify it under
 the terms of the European Union Public Licence - EUPL v.1.1 as published by
 the European Commission.

 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE. See the European Union Public Licence - EUPL v.1.1
 for more details.

 You should have received a copy of the European Union Public Licence - EUPL v.1.1
 along with this program.

 Further information about the European Union Public Licence - EUPL v.1.1 can
 also be found on the world wide web at http://ec.europa.eu/idabc/eupl
 */

/*
 ------ Copyright (C) 2010 STA Steering Board (space.trajectory.analysis AT gmail.com) ----

 */
/*
 ------------------ Author: Chris Laurel  -------------------------------------------------
 ------------------ E-mail: (claurel@gmail.com) ----------------------------
 Modified by Valentino Zuccarelli on 14th June 2009
 Extensively modified by Guillermo to hold TLEs on October 2009
 Extensively modified by Guillermo to accomodate payloads April 2010
 Modified by Guillermo August 2010 to add maneuvers
 */


#include <cassert>
#include "scenariotree.h"
#include "scenarioelementbox.h"
#include "Scenario/scenario.h"
#include "Astro-Core/trajectorypropagation.h"
#include "initialstateeditor.h"
#include "initialstateThreebodyEditor.h"
#include "Locations/locationeditor.h"
//#include "RendezVous/rendezvous.h"
#include "Loitering/loitering.h"
#include "Loitering/loiteringTLE.h"
#include "External/external.h"
//#include "Lagrangian/lagrangian.h"
#include "Entry/reentry.h"
#include "physicalproperties.h"
#include "aerodynamicproperties.h"
#include "propulsionproperties.h"
#include "entrymass.h"
//#include "payloadproperties.h"
#include "Payloads/transmitterPayloadDialog.h"
#include "Payloads/receiverPayloadDialog.h"
#include "Payloads/opticalPayloadDialog.h"
#include "Payloads/radarPayloadDialog.h"
#include "Locations/environmentdialog.h"
#include "Maneuvers/deltaVDialog.h"

#include <QtGui>
#include <QHash>
#include <iostream>


int Lagrmode=-1;  // Guillermo says: take away this thing


// We keep a table of information about how the various scenario elements should be
// displayed in the tree view. Each element has the following display properties:
//   label - the text label for the element
//   icon - the name of an icon resource for the element
//   autoExpand - true if the element should be expanded by default
//
// The list of ScenarioTreeLabel structures is converted into a hash table of
// ElementDisplayInfo objects when the first ScenarioTree is created. The hash
// table provides fast lookup of display info properties by element name.
struct ScenarioTreeLabel
{
    const char* elementType;
    const char* label;
    const char* iconName;
    bool autoExpand;
};

struct ElementDisplayInfo
{
    QString label;
    QIcon icon;
    bool autoExpand;
};

static QHash<QString, ElementDisplayInfo> TreeDisplayInfo;

// Display properties for all scenario elements. If you add a new scenario element,
// you should update this table. If an element is missing from this table, the
// element name will be shown in the tree view, which may not be ideal (e.g.,
// "LoiteringType" instead of just "Loitering")
static ScenarioTreeLabel TreeLabels[] =
{
    { "SC", "Satellite", ":/icons/ParticipantSATELLITE.png", true },
    { "LV", "Launcher", ":/icons/ParticipantROCKET.png", true },
    { "GroundStation", "Ground Station", ":/icons/ParticipantSTATION.png", true },
    { "Point", "Point", ":/icons/ParticipantPOINT.png", true },
    { "REV", "Re-entry vehicle", ":/icons/ParticipantENTRYVEHICLE.png", true },
    { "Transmitter", "Transmitter", "", false },
    { "TransmitterPayload", "Transmitter", "", false },
    { "TransmitterPayloadType", "Transmitter", ":/icons/Payload.png", false },
    { "ReceiverPayload", "Receiver", "", false },
    { "ReceiverPayloadType", "Receiver", ":/icons/Payload.png", false },
    { "OpticalPayload", "Telescope", "", false },
    { "OpticalPayloadType", "Telescope", ":/icons/Payload.png", false },
    { "RadarPayload", "Radar", "", false },
    { "RadarPayloadType", "Radar", ":/icons/Payload.png", false },
    { "RadarProperties", "Radar properties", "", false },
    { "RadarType", "Radar type", "", false },
    { "Loitering", "Loitering", "", false },
    { "LoiteringType", "Loitering", ":/icons/mission-arcs-loitering.png", false },
    { "Rendezvous", "Rendezvous", "", false },
    { "FlyBy", "Fly By", "", false },
    { "LoiteringTLE", "TLE", "", false },
    { "LoiteringTLEType", "TLE", ":/icons/mission-arcs-loiteringTLE.png", false },
    { "EntryArc", "Entry", "", false },
    { "DeltaVType", "Delta V", ":/icons/engine.png", false },
    { "ExternalType", "External", "", false },
    { "ElementIdentifierType", "Identifier", "", false },
    { "SCProgram", "Program", "", false },
    { "SCMission", "Mission", "", true },
    { "TrajectoryPlan", "Trajectory plan", "", true },
    { "PayloadSet", "Payload set", "", true },
    { "SCSystemType", "Platform", "", false },
    { "SystemBudgets", "Budgets", "", false },
    { "MassOfSystem", "Mass", "", false },
    { "PowerOfSystem", "Power", "", false },
    { "Link", "Link", "", false },
    { "SCAerodynamics", "Aerodynamics", "", false },
    { "Propulsion", "Propulsion", "", false },
    { "Structure", "Structure", "", false },
    { "Sizing", "Sizing", "", false },
    { "MomentsOfInertia", "Inertia", "", false },
    { "SecondMomentsOfArea", "More inertia", "", false },
    { "NaturalFrequency", "Natural frequency", "", false },
    { "TCS", "Thermal control", "", false },
    { "Temperature", "Temperature", "", false },
    { "CoatingArea", "Coating area", "", false },
    { "ColdSurface", "Cold surface", "", false },
    { "ColdCoating", "Cold surface coating", "", false },
    { "HotSurface", "Hot surface", "", false },
    { "HotCoating", "Hot surface coating", "", false },
    { "EPS", "EPS", "", false },
    { "SolarArray", "Solar array", "", false },
    { "BatteryType", "Battery", "", false },
    { "TTC", "TTC", "", false },
    { "TTCAntenna", "TTC antenna", "", false },
    { "AOCS", "AOCS", "", false },
    { "OBDH", "OBDH", "", false },
    { "Optimization", "Optimization", "", false },
    { "OutputFiles", "Output files", "", false },
    { "REVProgramType", "Program", "", false },
    { "REVMissionType", "Mission", "", true },
    { "REVPayloadType", "Payload set", "", true },
    { "REVTrajectoryPlanType", "Trajectory plan", "", true },
    { "REVSystemType", "System", "", false },
    { "REVWeights", "Mass", "", false },
    { "InertialMatrix", "Inertia", "", false },
    { "OptVarString", "Optimization", "", false },
    { "OptVarInt", "Optimization", "", false },
    { "OptVarDouble", "Optimization", "", false },
    { "OptVarBool", "Optimization", "", false },
    { "REVComponentsType", "Components", "", false },
    { "REVComponentsMassType", "Mass", "", false },
    { "CoGLongPosition", "CoG position (L)", "", false },
    { "REVGeometryType", "Geometry", "", false },
    { "REVTPS", "Thermal protection", "", false },
    { "REVAeroThermodynamicsType", "Aerothermodynamics", "", false },
    { "AeroCoefFileType", "Aerodynamics coefficients", "", false },
    { "Parachutes", "Parachutes", "", false },
    { "REVStructureType", "Structure", "", false },
    { "REVSecondaryPropulsionType", "Propulsion (secondary)", "", false },
    { "REVRCSType", "Propulsion (primary)", "", false },
    { "REVCostsType", "Cost", "", false },
    { "REVSystemCostsType", "Cost (system)", "", false },
    { "REVSubsystemsDevelopCostsType", "Cost (subsystem)", "", false },
    { "REVSubsystemsProductionCostsType", "Cost of production (subsystem)", "", false },
    { "REVReliabilityType", "Reliability (system)", "", false },
    { "REVSubsystemsReliablityType", "Reliability (subsystem)", "", false },
    { "LocationType", "Location", "", false },
    { "GroundPositionType", "Position", "", false },
    { "GroundStationEnvironment", "Environment", "", false },
    { "Rain", "Rain", "", false },
    { "Modulation", "Modulation", "", false },
    { "SystemTemperature", "Temperature", "", false },
    { "TelescopeType", "Telescope", "", false },
    { "OpticalProperties", "Optical properties", "", false },
    { "SCEnvironmentType", "Environment", "", false },
    { "TimeLine", "Time line", "", false },
    { "InitialPositionType", "Initial position", "", false },
    { "KeplerianElementsType", "Keplerian elements", "", false },
    { "InitialAttitudeType", "Initial attitude", "", false },
    { "EulerBIType", "Euler angles", "", false },
    { "PropagationPositionType", "Propagation of position", "", false },
    { "PropagationAttitudeType", "Propagation of attitude", "", false },
    { "EntryArcType", "Re-entry arc", ":/icons/mission-arcs-reentry.png", false },
    { "EnvironmentType", "Environment", "", false },
    { "CentralBodyType", "Central body", "", false },
    { "REVConstraintsType", "Constraints", "", false },
    { "REVFinalStateType", "Final state", "", false },
    { "REVConstraintsViolationType", "Constrains violations", "", false },
    { "EntryCharacteristicsType", "Entry characteristics", "", false },
    { "GravityModel", "Gravity model", "", false },
    { "SphericalCoordinatesType", "Spherical coordinates", "", false },
    { "REVDispersionAnalysisType", "Dispersion analysis", "", false },
    { "SpaceScenario", "Scenario", "", true },
    { "PerturbationsType", "Perturbations", "", false },
};


/*! Create a new scenario view widget.
 */
ScenarioTree::ScenarioTree(QWidget *parent)
    : QTreeWidget(parent)
{
    QStringList labels;
    labels << tr("Element") << tr("Name");
    
    // Create the table of display info if it hasn't been created already
    if (TreeDisplayInfo.empty())
    {
        for (unsigned i = 0; i < sizeof(TreeLabels) / sizeof(TreeLabels[0]); ++i)
        {
            ElementDisplayInfo info;
            info.label = TreeLabels[i].label;
            info.autoExpand = TreeLabels[i].autoExpand;
            QString iconName = TreeLabels[i].iconName;
            if (!iconName.isEmpty())
            {
                info.icon = QIcon(iconName);
            }

            TreeDisplayInfo[TreeLabels[i].elementType] = info;
        }
    }

    header()->setResizeMode(QHeaderView::ResizeToContents);
    header()->setResizeMode(QHeaderView::Interactive);
    header()->setDefaultSectionSize(200);
    header()->setCascadingSectionResizes(true);

    setHeaderLabels(labels);
    
    m_folderIcon.addPixmap(style()->standardPixmap(QStyle::SP_DirClosedIcon),
                           QIcon::Normal, QIcon::Off);
    m_folderIcon.addPixmap(style()->standardPixmap(QStyle::SP_DirOpenIcon),
                           QIcon::Normal, QIcon::On);
    m_scenarioElementIcon.addPixmap(style()->standardPixmap(QStyle::SP_FileIcon));
    
    connect(this, SIGNAL(itemDoubleClicked(QTreeWidgetItem*, int)),
            this, SLOT(editItem(QTreeWidgetItem*, int)));
    connect(this, SIGNAL(itemChanged(QTreeWidgetItem*, int)),
            this, SLOT(editItemInline(QTreeWidgetItem*, int)));


    setExpandsOnDoubleClick(false);
    setAnimated(true);	// Tree spands now slowly (nicer for the user)
    
    // Set drag and drop behavior
    setDragEnabled(true);
    setAcceptDrops(true);
    setDragDropMode(QAbstractItemView::DragDrop);
    setDropIndicatorShown(true);
    setAutoFillBackground(true);

}


// Guillermo says: This method changes the XML schema labels by better readable labels in the scenario view
// It also adds the icons to the left of the widget and takes care of the expansion true/false of the initial creation
static void changeLabels(QTreeWidgetItem* item, ScenarioObject* scenarioObject)
{
    if (TreeDisplayInfo.contains(scenarioObject->elementName()))
    {
        // Use label and icon information from the TreeDisplayInfo table when available
        ElementDisplayInfo info = TreeDisplayInfo[scenarioObject->elementName()];
        item->setText(0, info.label);
        item->setIcon(0, info.icon);
        if (info.autoExpand)
        {
            item->setExpanded(true);
        }
    }
    else
    {
        // No record in the TreeDisplayInfo table for this element, so just show the
        // element name.
        item->setText(0, scenarioObject->elementName());

        // When debugging, set the color of elements missing from the table to red.
        // item->setTextColor(0, "red");
    }
}



void ScenarioTree::addScenarioItems(QTreeWidgetItem* item, ScenarioObject* scenarioObject)
{

    // Store a pointer to the this ScenarioObject in the widget item.
    // This will be used to map a tree widget item to a ScenarioObject.
    // IMPORTANT: The ScenarioObject must not be deleted before the tree
    // widget item that references it.
    QVariant scenarioObjectData = qVariantFromValue(reinterpret_cast<void*>(scenarioObject));
    item->setData(0, ScenarioObjectRole, scenarioObjectData);

    // Disable drag and drop by default. Only very limited drag and drop
    // is allowed within a space scenario because in general the elements
    // of the scenario have a defined order. Only rearranging of participants
    // and trajectories is permitted.
    item->setFlags(item->flags() & ~(Qt::ItemIsDragEnabled | Qt::ItemIsDropEnabled));

    // Allow dropping into scenarios, trajectory plans, and payload sets (and nothing else for now)
    if (dynamic_cast<SpaceScenario*>(scenarioObject) ||
        dynamic_cast<ScenarioTrajectoryPlan*>(scenarioObject) ||
        dynamic_cast<ScenarioPayloadSet*>(scenarioObject) ||  // Patched by Guillermo to allow drops into Payload plan
        dynamic_cast<ScenarioREVTrajectoryPlanType*>(scenarioObject))  //Modified by Dominic and Ricardo Noriega to allow dropping of EntryArc and Payloads over SC and GS
    {
        item->setFlags(item->flags() | Qt::ItemIsDropEnabled);
    }

    QList<QSharedPointer<ScenarioObject> > children = scenarioObject->children();
    foreach (QSharedPointer<ScenarioObject> child, children)
    {
        if (!child.isNull())
        {
            QTreeWidgetItem* childItem = new QTreeWidgetItem(item);
            addScenarioItems(childItem, child.data());
        }
    }

    // Guillermo: expand ALL items by default or not
    //item->setExpanded(true);

    // This part labels the tree with the proper identifiers
    if (dynamic_cast<ScenarioElementIdentifierType*>(scenarioObject))
    {
        item->setFlags(item->flags() | (Qt::ItemIsEditable));
        item->setText(1, dynamic_cast<ScenarioElementIdentifierType*>(scenarioObject)->Name());
    }
    else if (dynamic_cast<ScenarioParticipantType*>(scenarioObject))
    {
        item->setText(1, dynamic_cast<ScenarioParticipantType*>(scenarioObject)->Name());
    }

    changeLabels(item, scenarioObject);

}   // End of addScenarioItems ////////





QStringList ScenarioTree::mimeTypes() const
{
    QStringList types;
    types << ScenarioElementBox::PARTICIPANT_MIME_TYPE
            << ScenarioElementBox::MISSION_ARC_MIME_TYPE
            << ScenarioElementBox::PAYLOAD_MIME_TYPE;
    return types;
}



//////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////// Dropping the objects in the scenario tree /////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////
bool ScenarioTree::dropMimeData(QTreeWidgetItem* parent,
                                int /* index */,
                                const QMimeData* data,
                                Qt::DropAction action)
{
    if (action == Qt::IgnoreAction)
    {
        return true;
    }
    
    if (data->formats().empty())
    {
        return false;
    }

    if (!parent)
    {
        return false;
    }
    
    QString dataFormat = data->formats().first();
    QByteArray encodedData = data->data(dataFormat);
    
    // The dropped data is XML text. Convert it to a DOM node...
    QDomDocument doc;
    QString errorMessage;
    if (!doc.setContent(encodedData, &errorMessage))
    {
        qDebug() << "Drop failed due to XML parsing error: " << errorMessage;
        return false;
    }

    QDomElement element = doc.firstChildElement();
    QString elementName = element.tagName();

    // Dropped item may be either a participant or a trajectory or a payload
    ScenarioParticipantType* participant = NULL;
    ScenarioAbstractTrajectoryType* trajectory = NULL;
    ScenarioAbstractPayloadType* payload = NULL; //Line added by Ricardo to create an item to be dropped of payload type

    if (elementName == "tns:GroundStation")
        participant = ScenarioGroundStation::create(element);
    else if (elementName == "tns:LaunchPad")
        participant = ScenarioLaunchPad::create(element);
    else if (elementName == "tns:Point")
        participant = ScenarioPoint::create(element);
    else if (elementName == "tns:LV")
        participant = ScenarioLV::create(element);
    else if (elementName == "tns:SC")
        participant = ScenarioSC::create(element);
    else if (elementName == "tns:REV")
        participant = ScenarioREV::create(element);

    if (elementName == "tns:TransmitterPayload")
        payload = ScenarioTransmitterPayloadType::create(element); //Ricardo edit this line to let the CommPayload element to be added to the scenario
    if (elementName == "tns:ReceiverPayload")
        payload = ScenarioReceiverPayloadType::create(element); //Guillermo
    if (elementName == "tns:OpticalPayload")
        payload = ScenarioOpticalPayloadType::create(element);  // Guillermo: optical
    if (elementName == "tns:RadarPayload")
        payload = ScenarioRadarPayloadType::create(element); // Guillermo radar

    if (elementName == "tns:Loitering")
        trajectory = ScenarioLoiteringType::create(element);
    else if (elementName == "tns:Lagrangian")
        trajectory = ScenarioLagrangianType::create(element);
    else if (elementName == "tns:FlyBy")
        trajectory = ScenarioFlyByType::create(element);
    else if (elementName == "tns:LoiteringTLE")
        trajectory = ScenarioLoiteringTLEType::create(element);
    else if (elementName == "tns:EntryArc")
        trajectory = ScenarioEntryArcType::create(element);

    if (elementName == "tns:DeltaV")
        trajectory = ScenarioDeltaVType::create(element);  // Guillermo says: creating the maneuver as a trajectory arc





    if (participant)
    {
        ScenarioObject* parentObject = objectForItem(parent);
        SpaceScenario* scenario = dynamic_cast<SpaceScenario*>(parentObject);
        if (scenario)
        {
            scenario->AbstractParticipant().append(QSharedPointer<ScenarioParticipantType>(participant));
            QTreeWidgetItem* participantItem = new QTreeWidgetItem(parent);
            addScenarioItems(participantItem, participant);
        }
        else
        {
            qDebug() << "Internal error in tree view: root item is null.";
        }

        return true;
    }
    else if (trajectory && elementName != "tns:EntryArc" && elementName != "tns:DeltaV")   // Loitering and loiteirng TLEs
    //else if (trajectory && elementName != "tns:EntryArc" && elementName != "tns:DeltaV" && elementName != "tns:LoiteringTLE")    // Loitering
    {
        ScenarioObject* parentObject = objectForItem(parent);
        ScenarioTrajectoryPlan* trajectoryPlan = dynamic_cast<ScenarioTrajectoryPlan*>(parentObject);
        if (trajectoryPlan)
        {
            int numberOfArcs = trajectoryPlan->children().size();
            if (numberOfArcs > 0)  // The trajectory plan is not empty
            {
                const QList<QSharedPointer<ScenarioAbstractTrajectoryType> >& trajectoryList = trajectoryPlan->AbstractTrajectory();
                QSharedPointer<ScenarioAbstractTrajectoryType> thePreviousTrajectory = trajectoryList.at(numberOfArcs - 1);

                if (dynamic_cast<ScenarioDeltaVType*>(thePreviousTrajectory.data()))
                // We do not allow to drop the arc except when the previous arc is a deltaV
                {                    
                    // Adding the current arc into the trajectory plan
                    trajectoryPlan->AbstractTrajectory().append(QSharedPointer<ScenarioAbstractTrajectoryType>(trajectory));
                    QTreeWidgetItem* trajectoryItem = new QTreeWidgetItem(parent);
                    addScenarioItems(trajectoryItem, trajectory);

                    ScenarioDeltaVType* thePreviousArc = dynamic_cast<ScenarioDeltaVType*>(thePreviousTrajectory.data());
                    QSharedPointer<ScenarioAbstractTrajectoryType> theCurrentManeuver = trajectoryList.at(numberOfArcs);

                    // Passing information from the previous arc into the current arc
                    if (theCurrentManeuver->elementName() == "LoiteringType")
                    {
                        ScenarioLoiteringType* theCurrentLoitering = dynamic_cast<ScenarioLoiteringType*>(theCurrentManeuver.data());

                        // concatenating the times for the mission arc
                        theCurrentLoitering->TimeLine()->setStartTime(thePreviousArc->TimeLine()->EndTime());
                        theCurrentLoitering->TimeLine()->setEndTime(thePreviousArc->TimeLine()->EndTime().addDays(1));
                        // TODO: to gray out the rest of the loitering GUI

                        /*
                        // concatenating the maneuver for the mission arc
                        ScenarioInitialPositionType* initialPosition = dynamic_cast<ScenarioInitialPositionType*>(theCurrentLoitering->InitialPosition().data());
                        ScenarioKeplerianElementsType* newKeplerianElements = new ScenarioKeplerianElementsType;
                        initialPosition->setAbstract6DOFPosition(QSharedPointer<ScenarioAbstract6DOFPositionType>(newKeplerianElements));
                        theCurrentLoitering->InitialPosition()->setAbstract6DOFPosition(QSharedPointer<ScenarioAbstract6DOFPositionType>(newKeplerianElements));
                        */
                    }                    
                      // for the time being TLEs are not treated
                    else if (theCurrentManeuver->elementName() == "LoiteringTLEType")
                    {
                        ScenarioLoiteringTLEType* theCurrentLoiteringTLE = dynamic_cast<ScenarioLoiteringTLEType*>(theCurrentManeuver.data());
                        theCurrentLoiteringTLE->TimeLine()->setStartTime(thePreviousArc->TimeLine()->EndTime()); // concatenating the times for the mission arcs
                        theCurrentLoiteringTLE->TimeLine()->setEndTime(thePreviousArc->TimeLine()->EndTime().addDays(1));
                    }
                }
            }
            else  // Then the trajectory plan is emptly and we just simply drop the arc
            {
                trajectoryPlan->AbstractTrajectory().append(QSharedPointer<ScenarioAbstractTrajectoryType>(trajectory));
                QTreeWidgetItem* trajectoryItem = new QTreeWidgetItem(parent);
                addScenarioItems(trajectoryItem, trajectory);
                return true;
            }
        }
        else
        {
            return false;
        }
    }
    else if (trajectory && elementName == "tns:DeltaV")   // delta Vs
    {
        ScenarioObject* parentObject = objectForItem(parent);
        ScenarioTrajectoryPlan* trajectoryPlan = dynamic_cast<ScenarioTrajectoryPlan*>(parentObject);
        if (trajectoryPlan)
        {
            int numberOfArcs = trajectoryPlan->children().size();
            //qDebug() << numberOfArcs << endl;
            if (numberOfArcs > 0)
            {
                const QList<QSharedPointer<ScenarioAbstractTrajectoryType> >& trajectoryList = trajectoryPlan->AbstractTrajectory();
                QSharedPointer<ScenarioAbstractTrajectoryType> thePreviousTrajectory = trajectoryList.at(numberOfArcs - 1);
                if (dynamic_cast<ScenarioLoiteringType*>(thePreviousTrajectory.data()))
                {
                    // Adding the delta V into the trajectory plan
                    trajectoryPlan->AbstractTrajectory().append(QSharedPointer<ScenarioAbstractTrajectoryType>(trajectory));
                    QTreeWidgetItem* trajectoryItem = new QTreeWidgetItem(parent);
                    addScenarioItems(trajectoryItem, trajectory);

                    // Passing information from the previous arc into the current deltaV
                    ScenarioLoiteringType* thePreviousLoiteringArc = dynamic_cast<ScenarioLoiteringType*>(thePreviousTrajectory.data());
                    QSharedPointer<ScenarioAbstractTrajectoryType> theCurrentManeuver = trajectoryList.at(numberOfArcs);
                    ScenarioDeltaVType* theCurrentDeltaV = dynamic_cast<ScenarioDeltaVType*>(theCurrentManeuver.data());
                    theCurrentDeltaV->TimeLine()->setStartTime(thePreviousLoiteringArc->TimeLine()->EndTime()); // concatenating the times for the mission arcs
                    theCurrentDeltaV->TimeLine()->setEndTime(thePreviousLoiteringArc->TimeLine()->EndTime().addSecs(1));

                    return true;
                }
            }
        }
        else
        {
            return false;
        }
    }
    else if (trajectory && elementName == "tns:EntryArc") // Added by Dominic to drop entry trajectory
    {
        ScenarioObject* parentObject = objectForItem(parent);
        ScenarioREVTrajectoryPlanType* trajectoryPlan = dynamic_cast<ScenarioREVTrajectoryPlanType*>(parentObject);
        if (trajectoryPlan)
        {
            trajectoryPlan->AbstractTrajectory().append(QSharedPointer<ScenarioAbstractTrajectoryType>(trajectory));
            QTreeWidgetItem* trajectoryItem = new QTreeWidgetItem(parent);
            // Adding the item to the scenario
            addScenarioItems(trajectoryItem, trajectory);
            return true;
        }
        else
        {
            return false;
        }
    }
    else if (payload)
    {
        ScenarioObject* parentObject = objectForItem(parent);
        ScenarioPayloadSet* scPayload = dynamic_cast<ScenarioPayloadSet*>(parentObject);
        if (scPayload) //if the draggable element is over the payload of the spacecraft, the payload could be attached
        {
            scPayload->AbstractPayload().append(QSharedPointer<ScenarioAbstractPayloadType>(payload));
            QTreeWidgetItem* payloadItem = new QTreeWidgetItem(parent);
            addScenarioItems(payloadItem, payload);
            return true;
        }
        else
        {
            return false;
        }
    }
    // Report drag and drop not handled (because item was unrecognized or dragged to the
    // wrong place in the tree view.)
    return false;
}





QMimeData* ScenarioTree::mimeData(const QList<QTreeWidgetItem*> items) const
{
    if (items.isEmpty())
    {
        return NULL;
    }

    QMimeData *mimeData = new QMimeData();
    QByteArray encodedData;

    QDataStream stream(&encodedData, QIODevice::WriteOnly);

    foreach (QTreeWidgetItem* item, items)
    {
        if (item)
        {
            QString text = "blah blah blah";
            stream << text;
        }
    }

    mimeData->setData(ScenarioElementBox::PARTICIPANT_MIME_TYPE, encodedData);
    return mimeData;

    //    return QTreeWidget::mimeData(items);

#if 0
    // Code to allow dragging scenario objects out of the view
    if (items.isEmpty())
        return NULL;

    ScenarioObject* scenarioObject = objectForItem(items.first());
    if (scenarioObject == NULL)
        return NULL;
    else
        return new ScenarioMimeData(scenarioObject);
#endif
}


/*! Update a scenario object to reflect changes made in the
 *  tree widget.
 */
void ScenarioTree::updateScenarioObject(QTreeWidgetItem* /* item */, int /* column */)
{
    QMessageBox::information(this, tr("item changed"), tr("item changed"));
}


void ScenarioTree::removeChildren(QTreeWidgetItem* parentItem)
{
    int childCount = parentItem->childCount();
    QDomElement element = m_domElementForItem[parentItem];
    Q_ASSERT(!element.isNull());

    for (int i = childCount - 1; i >= 0; i--)
    {
        QTreeWidgetItem* child = parentItem->takeChild(i);
        if (child->childCount() != 0)
        {
            removeChildren(child);
        }

        QDomElement childElement = m_domElementForItem[child];
        Q_ASSERT(!childElement.isNull());
        m_domElementForItem.remove(child);
        element.removeChild(childElement);

        delete child;
    }
}


void ScenarioTree::updateTreeItems(QTreeWidgetItem* parentItem,
                                   ScenarioObject* scenarioObject)
{
    int childCount = parentItem->childCount();
    for (int i = childCount - 1; i >= 0; i--)
    {
        QTreeWidgetItem* child = parentItem->takeChild(i);
        delete child;
    }
}





//////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////// Editing the objects in the scenario ///////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////
void ScenarioTree::editScenarioObject(ScenarioObject* scenarioObject,
                                      QTreeWidgetItem* editItem)
{


    if (scenarioObject == NULL || editItem == NULL)
    {
        return;
    }

    if (dynamic_cast<ScenarioElementIdentifierType*>(scenarioObject) != NULL)
    {
        ScenarioElementIdentifierType* myIdentifier = dynamic_cast<ScenarioElementIdentifierType*>(scenarioObject);
        editItem->setFlags(editItem->flags() | (Qt::ItemIsEditable));
        myIdentifier->Name() = editItem->text(1);
        updateTreeItems(editItem, scenarioObject);
    }
    else if (dynamic_cast<ScenarioLoiteringType*>(scenarioObject) != NULL)
    {
        Lagrmode=-1;    // Guillermo: to remove this from here !!!!!!!!!!!!
        ScenarioLoiteringType* loitering = dynamic_cast<ScenarioLoiteringType*>(scenarioObject);
        LoiteringDialog editDialog(this);

        if (!editDialog.loadValues(loitering))
        {
            QMessageBox::information(this, tr("Bad Loitering element"), tr("Error in Loitering element"));
        }
        else
        {
            if (editDialog.exec() == QDialog::Accepted)
            {
                editDialog.saveValues(loitering);
                //qDebug() << "==> arc name :" << loitering->ElementIdentifier()->Name()  << endl;
                //updateTreeItems(editItem, scenarioObject);
            }
        }
    }
    else if (dynamic_cast<ScenarioLoiteringTLEType*>(scenarioObject) != NULL)
    {
        ScenarioLoiteringTLEType* loiteringTLE = dynamic_cast<ScenarioLoiteringTLEType*>(scenarioObject);
        LoiteringTLEDialog editDialog(this);

        if (!editDialog.loadValues(loiteringTLE))
        {
            QMessageBox::information(this, tr("Bad Loitering TLE element"), tr("Error in Loitering TLE element"));
        }
        else
        {
            if (editDialog.exec() == QDialog::Accepted)
            {
                editDialog.saveValues(loiteringTLE);
                // TODO Change the name of the participant using the TLE line 0
                //updateTreeItems(editItem, scenarioObject);
            }
        }
    }
    else if (dynamic_cast<ScenarioDeltaVType*>(scenarioObject) != NULL)
    {
        ScenarioDeltaVType* deltaV = dynamic_cast<ScenarioDeltaVType*>(scenarioObject);
        DeltaVDialog editDialog(this);

        if (!editDialog.loadValues(deltaV))
        {
            QMessageBox::information(this, tr("Bad delta V element"), tr("Error in delta V element"));
        }
        else
        {
            if (editDialog.exec() == QDialog::Accepted)
            {
                editDialog.saveValues(deltaV);
                //updateTreeItems(editItem, scenarioObject);
            }
        }
    }
    else if (dynamic_cast<ScenarioLocationType*>(scenarioObject) != NULL)
    {
        ScenarioLocationType* location = dynamic_cast<ScenarioLocationType*>(scenarioObject);
        LocationEditorDialog editDialog(this);
        editDialog.loadValues(location);
        if (editDialog.exec() == QDialog::Accepted)
        {
            editDialog.saveValues(location);
            //updateTreeItems(editItem, scenarioObject);
        }
    }
    else if (dynamic_cast<ScenarioGroundStationEnvironment*>(scenarioObject) != NULL)
    {
        ScenarioGroundStationEnvironment* environment = dynamic_cast<ScenarioGroundStationEnvironment*>(scenarioObject);
        environmentDialog editDialog(this);
        editDialog.loadValues(environment);
        if (editDialog.exec() == QDialog::Accepted)
        {
            editDialog.saveValues(environment);
            //updateTreeItems(editItem, scenarioObject);
        }
    }
    else if (dynamic_cast<ScenarioTransmitterPayloadType*>(scenarioObject) != NULL)   // Guillermo: patching Ricardo's code
    {
        ScenarioTransmitterPayloadType* payload = dynamic_cast<ScenarioTransmitterPayloadType*>(scenarioObject);
        transmitterPayloadDialog editDialog(this);
        if (!editDialog.loadValues(payload))
        {
            QMessageBox::information(this, tr("Bad Tx Payload element"), tr("Error in Tx Payload element"));
        }

        else
        {
            if (editDialog.exec() == QDialog::Accepted)
            {
                editDialog.saveValues(payload);
                //updateTreeItems(editItem, scenarioObject);
            }
        }

    }
    else if (dynamic_cast<ScenarioReceiverPayloadType*>(scenarioObject) != NULL)   // Guillermo: patching Ricardo's code
    {
        ScenarioReceiverPayloadType* payload = dynamic_cast<ScenarioReceiverPayloadType*>(scenarioObject);
        receiverPayloadDialog editDialog(this);
        if (!editDialog.loadValues(payload))
        {
            QMessageBox::information(this, tr("Bad Rx Payload element"), tr("Error in Rx Payload element"));
        }

        else
        {
            if (editDialog.exec() == QDialog::Accepted)
            {
                editDialog.saveValues(payload);
                //updateTreeItems(editItem, scenarioObject);
            }
        }

    }
    else if (dynamic_cast<ScenarioOpticalPayloadType*>(scenarioObject) != NULL)   // Guillermo: patching Ricardo's code
    {
        ScenarioOpticalPayloadType* payload = dynamic_cast<ScenarioOpticalPayloadType*>(scenarioObject);
        opticalPayloadDialog editDialog(this);
        if (!editDialog.loadValues(payload))
        {
            QMessageBox::information(this, tr("Bad Optical Payload element"), tr("Error in Optical Payload element"));
        }

        else
        {
            if (editDialog.exec() == QDialog::Accepted)
            {
                editDialog.saveValues(payload);
                //updateTreeItems(editItem, scenarioObject);
            }
        }

    }
    else if (dynamic_cast<ScenarioRadarPayloadType*>(scenarioObject) != NULL)   // Guillermo: patching Ricardo's code
    {
        ScenarioRadarPayloadType* payload = dynamic_cast<ScenarioRadarPayloadType*>(scenarioObject);
        radarPayloadDialog editDialog(this);
        if (!editDialog.loadValues(payload))
        {
            QMessageBox::information(this, tr("Bad Radar Payload element"), tr("Error in Radar Payload element"));
        }

        else
        {
            if (editDialog.exec() == QDialog::Accepted)
            {
                editDialog.saveValues(payload);
                //updateTreeItems(editItem, scenarioObject);
            }
        }

    }
    else if(dynamic_cast<ScenarioEntryArcType*>(scenarioObject) != NULL)//Added by Dominic to allow opening of re-entry GUI
    {
        ScenarioEntryArcType* entry = dynamic_cast<ScenarioEntryArcType*>(scenarioObject);
        ReEntryDialog editDialog(this);

        if (!editDialog.loadValues(entry))
        {
            QMessageBox::information(this, tr("Bad Entry element"), tr("Error in Entry element"));
        }

        else
        {
            if (editDialog.exec() == QDialog::Accepted)
            {
                editDialog.saveValues(entry);
                //updateTreeItems(editItem, scenarioObject);
            }
        }

    }


    else if (dynamic_cast<ScenarioREVGeometryType*>(scenarioObject) != NULL)//Added by Dominic to allow opening of Geometry GUI
    {
        ScenarioREVGeometryType* geometry = dynamic_cast<ScenarioREVGeometryType*>(scenarioObject);
        ScenarioREVAeroThermodynamicsType* aerothermo;
        QTreeWidgetItem* parentItem = editItem->parent();
        ScenarioObject* parentObject = objectForItem(parentItem);
        if (dynamic_cast<ScenarioREVSystemType*>(parentObject))
        {
            ScenarioREVSystemType* REVsystem = dynamic_cast<ScenarioREVSystemType*>(parentObject);
            aerothermo = REVsystem->AeroThermodynamics().data();
        }

        PhysicalPropertiesDialog editDialog(this);

        if (!editDialog.loadValues(geometry))
        {
            QMessageBox::information(this, tr("Bad Geometry element"), tr("Error in Geometry element"));
        }

        else
        {
            if (editDialog.exec() == QDialog::Accepted)
            {
                editDialog.saveValues(geometry,aerothermo);
                //updateTreeItems(editItem, scenarioObject);
            }
        }

    }

    else if (dynamic_cast<ScenarioREVAeroThermodynamicsType*>(scenarioObject) != NULL)  //Added by Dominic to allow opening of aerodynamic GUI
    {
        ScenarioREVAeroThermodynamicsType* aerothermo = dynamic_cast<ScenarioREVAeroThermodynamicsType*>(scenarioObject);
        ScenarioREVGeometryType * geometry;
        QTreeWidgetItem* parentItem=editItem->parent();//Next part added by Dominic to change REVgeometry from aerodynamics GUI.
        ScenarioObject * parentObject=objectForItem(parentItem);
        if(dynamic_cast<ScenarioREVSystemType*>(parentObject))
        {
            ScenarioREVSystemType* parentREVSystem = dynamic_cast<ScenarioREVSystemType*>(parentObject);
            geometry = parentREVSystem->Geometry().data();

        }
        AerodynamicPropertiesDialog editDialog(this);

        if (!editDialog.loadValues(aerothermo,geometry))
        {
            QMessageBox::information(this, tr("Bad Aerothermodynamics element"), tr("Error in Aerothermodynamics element"));
        }

        else
        {
            if (editDialog.exec() == QDialog::Accepted)
            {
                editDialog.saveValues(aerothermo,geometry);
                //updateTreeItems(editItem, scenarioObject);
            }
        }


    }

    else if (dynamic_cast<ScenarioREVWeights*>(scenarioObject) != NULL)//Added by Dominic to allow opening of REV mass GUI
    {
        ScenarioREVWeights* weights = dynamic_cast<ScenarioREVWeights*>(scenarioObject);
        EntryMassDialog editDialog(this);

        if (!editDialog.loadValues(weights))
        {
            QMessageBox::information(this, tr("Bad REV weights element"), tr("Error in REV weights element"));
        }

        else
        {
            if (editDialog.exec() == QDialog::Accepted)
            {
                editDialog.saveValues(weights);
                //updateTreeItems(editItem, scenarioObject);
            }
        }

    }

#if OLDSCENARIO
    if (dynamic_cast<ScenarioTrajectoryPropagation*>(scenarioObject) != NULL)
    {
        ScenarioTrajectoryPropagation* prop = dynamic_cast<ScenarioTrajectoryPropagation*>(scenarioObject);
        TrajectoryPropagationDialog editDialog(this);
        if (!editDialog.loadValues(prop))
        {
            QMessageBox::information(this, tr("Bad TrajectoryPropagation element"), tr("Error in TrajectoryPropagation element"));
        }
        else
        {
            if (editDialog.exec() == QDialog::Accepted)
            {
                editDialog.saveValues(prop);
                //updateTreeItems(editItem, scenarioObject);
            }
        }
    }

    else if (dynamic_cast<ScenarioInitialStatePosition*>(scenarioObject) != NULL)
    {
        ScenarioInitialStatePosition* initialStatePos = dynamic_cast<ScenarioInitialStatePosition*>(scenarioObject);
        if (Lagrmode==1 || Lagrmode==2 || Lagrmode==3 || Lagrmode==4 || Lagrmode==0)
        {

            InitialStateThreebodyEditorDialog editDialog(this);
            if (!editDialog.loadValues(initialStatePos))
            {
                QMessageBox::information(this, tr("Bad InitialStatePosition element"), tr("Error in InitialStatePosition element"));
            }
            else
            {
                if (editDialog.exec() == QDialog::Accepted)
                {
                    editDialog.saveValues(initialStatePos);
                    //updateTreeItems(editItem, scenarioObject);
                }
            }
        }
        else
        {
            InitialStateEditorDialog editDialog(this);
            if (!editDialog.loadValues(initialStatePos))
            {
                QMessageBox::information(this, tr("Bad InitialStatePosition element"), tr("Error in InitialStatePosition element"));
            }
            else
            {
                if (editDialog.exec() == QDialog::Accepted)
                {
                    editDialog.saveValues(initialStatePos);
                    //updateTreeItems(editItem, scenarioObject);
                }
            }
        }
    }
    else if (dynamic_cast<ScenarioLocation*>(scenarioObject) != NULL)
    {
        ScenarioLocation* location = dynamic_cast<ScenarioLocation*>(scenarioObject);
        LocationEditorDialog editDialog(this);
        editDialog.loadValues(location);
        if (editDialog.exec() == QDialog::Accepted)
        {
            editDialog.saveValues(location);
            //updateTreeItems(editItem, scenarioObject);
        }
    }
    else if (dynamic_cast<ScenarioRendezvousTrajectory*>(scenarioObject) != NULL)
    {
        ScenarioRendezvousTrajectory* rendezvous = dynamic_cast<ScenarioRendezvousTrajectory*>(scenarioObject);
        RendezvousDialog editDialog(this);
        if (!editDialog.loadValues(rendezvous))
        {
            QMessageBox::information(this, tr("Bad Rendezvous element"), tr("Error in Rendezvous element"));
        }
        else
        {
            if (editDialog.exec() == QDialog::Accepted)
            {
                editDialog.saveValues(rendezvous);
                //updateTreeItems(editItem, scenarioObject);
            }
        }
    }
    else if (dynamic_cast<ScenarioLagrangianTrajectory*>(scenarioObject) != NULL)
    {Lagrmode=0;
        ScenarioLagrangianTrajectory* lagrangian = dynamic_cast<ScenarioLagrangianTrajectory*>(scenarioObject);
        LagrangianDialog editDialog(this);
        if (!editDialog.loadValues(lagrangian))
        {
            QMessageBox::information(this, tr("Bad Lagrangian element"), tr("Error in Lagrangian element"));
        }
        else
        {
            if (editDialog.exec() == QDialog::Accepted)
            {
                editDialog.saveValues(lagrangian);
                //updateTreeItems(editItem, scenarioObject);
            }
        }
    }

    else if (dynamic_cast<ScenarioLoiteringTrajectory*>(scenarioObject) != NULL)
    {
        Lagrmode=-1;  // Guillermo says: take this away
        ScenarioLoiteringTrajectory* loitering = dynamic_cast<ScenarioLoiteringTrajectory*>(scenarioObject);
        LoiteringDialog editDialog(this);
        if (!editDialog.loadValues(loitering))
        {
            QMessageBox::information(this, tr("Bad Loitering element"), tr("Error in Loitering element"));
        }
        else
        {
            if (editDialog.exec() == QDialog::Accepted)
            {
                editDialog.saveValues(loitering);
                //updateTreeItems(editItem, scenarioObject);
            }
        }
    }

    else if (dynamic_cast<ScenarioTleTrajectory*>(scenarioObject) != NULL)
    {
        ScenarioTleTrajectory* loiteringTLE = dynamic_cast<ScenarioTleTrajectory*>(scenarioObject);
        LoiteringTLEDialog editDialog(this);

        if (!editDialog.loadValues(loiteringTLE))
        {
            QMessageBox::information(this, tr("Bad Loitering TLE element"), tr("Error in Loitering TLE element"));
        }
        else
        {
            if (editDialog.exec() == QDialog::Accepted)
            {
                editDialog.saveValues(loiteringTLE);
                // TODO Change the name of the participant using the TLE line 0
                //updateTreeItems(editItem, scenarioObject);
            }
        }
    }
    else if (dynamic_cast<ScenarioExternalTrajectory*>(scenarioObject) != NULL)
    {
        ScenarioExternalTrajectory* externalTrajectory = dynamic_cast<ScenarioExternalTrajectory*>(scenarioObject);
        ExternalDialog editDialog(this);

        if (!editDialog.loadValues(externalTrajectory))
        {
            QMessageBox::information(this, tr("Bad External element"), tr("Error in external trajectory"));
        }
        else
        {
            if (editDialog.exec() == QDialog::Accepted)
            {
                editDialog.saveValues(externalTrajectory);
                //updateTreeItems(editItem, scenarioObject);
            }
        }
    }

    else if (dynamic_cast<ScenarioPhysicalProperties*>(scenarioObject) != NULL)
    {
        ScenarioPhysicalProperties* physicalProperties = dynamic_cast<ScenarioPhysicalProperties*>(scenarioObject);
        PhysicalPropertiesDialog editDialog(this);
        if (!editDialog.loadValues(physicalProperties))
        {
            QMessageBox::information(this, tr("Bad Properties element"), tr("Error in Physical Properties element"));
        }
        else
        {
            if (editDialog.exec() == QDialog::Accepted)
            {
                editDialog.saveValues(physicalProperties);
                //updateTreeItems(editItem, scenarioObject);
            }
        }
    }

    else if (dynamic_cast<ScenarioAerodynamicProperties*>(scenarioObject) != NULL)
    {
        ScenarioAerodynamicProperties* aerodynamicProperties = dynamic_cast<ScenarioAerodynamicProperties*>(scenarioObject);
        AerodynamicPropertiesDialog editDialog(this);
        if (!editDialog.loadValues(aerodynamicProperties))
        {
            QMessageBox::information(this, tr("Bad Properties element"), tr("Error in Aerodynamic Properties element"));
        }
        else
        {
            if (editDialog.exec() == QDialog::Accepted)
            {
                editDialog.saveValues(aerodynamicProperties);
                //updateTreeItems(editItem, scenarioObject);
            }
        }
    }

    else if (dynamic_cast<ScenarioPropulsionProperties*>(scenarioObject) != NULL)
    {
        ScenarioPropulsionProperties* propulsionProperties = dynamic_cast<ScenarioPropulsionProperties*>(scenarioObject);
        PropulsionPropertiesDialog editDialog(this);
        if (!editDialog.loadValues(propulsionProperties))
        {
            QMessageBox::information(this, tr("Bad Properties element"), tr("Error in Propulsion Properties element"));
        }
        else
        {
            if (editDialog.exec() == QDialog::Accepted)
            {
                editDialog.saveValues(propulsionProperties);
                //updateTreeItems(editItem, scenarioObject);
            }
        }
    }

    /*
    else if (dynamic_cast<ScenarioPayloadProperties*>(scenarioObject) != NULL)
    {
        ScenarioPayloadProperties* payloadProperties = dynamic_cast<ScenarioPayloadProperties*>(scenarioObject);
        PayloadPropertiesDialog editDialog(this);
        if (!editDialog.loadValues(payloadProperties))
        {
            QMessageBox::information(this, tr("Bad Properties element"), tr("Error in Payload Properties element"));
        }
        else
        {
            if (editDialog.exec() == QDialog::Accepted)
            {
                editDialog.saveValues(payloadProperties);
                updateTreeItems(editItem, scenarioObject);
            }
        }
    }
    */

    else if (dynamic_cast<ScenarioReEntryTrajectory*>(scenarioObject) != NULL)
    {
        Lagrmode=0;
        ScenarioReEntryTrajectory* reentry = dynamic_cast<ScenarioReEntryTrajectory*>(scenarioObject);
        ReEntryDialog editDialog(this);

        if (!editDialog.loadValues(reentry))
        {
            QMessageBox::information(this, tr("Bad ReEntry element"), tr("Error in ReEntry element"));
        }
        else
        {
            if (editDialog.exec() == QDialog::Accepted)
            {
                editDialog.saveValues(reentry);
                //updateTreeItems(editItem, scenarioObject);
            }
        }
    }
    else if (dynamic_cast<ScenarioAppearance*>(scenarioObject) != NULL)
    {
        ScenarioAppearance* appearance = dynamic_cast<ScenarioAppearance*>(scenarioObject);
        QString modelFileName = QFileDialog::getOpenFileName(this,
                                                             tr("Select Model File"),
                                                             QString("./models/") + appearance->model(),
                                                             tr("Models (*.3ds *.cmod)"));
        if (!modelFileName.isEmpty())
        {
            // Strip away the path--we just want the filename
            if (modelFileName.contains('/'))
                modelFileName.remove(0, modelFileName.lastIndexOf('/') + 1);

            appearance->setModel(modelFileName);
            //updateTreeItems(editItem, scenarioObject);
        }
    }
    else
    {
        ScenarioObject* parentObject = NULL;
        editItem = editItem->parent();
        if (editItem != NULL)
        {
            QVariant data = editItem->data(0, ScenarioObjectRole);

            if (qVariantCanConvert<void*>(data))
            {
                void* pointer = qVariantValue<void*>(data);
                parentObject = reinterpret_cast<ScenarioObject*>(pointer);
            }

            editScenarioObject(parentObject, editItem);
        }
    }
#endif

}


//void ScenarioTree::editItem(QTreeWidgetItem* item, int /* column */)
void ScenarioTree::editItem(QTreeWidgetItem* item, int column)
{

    //QTextStream out (stdout);

    if (item == NULL)
    {
        return;
    }

    ScenarioObject* scenarioObject = NULL;

    QVariant data = item->data(0, ScenarioObjectRole);
    if (qVariantCanConvert<void*>(data))
    {
        void* pointer = qVariantValue<void*>(data);
        scenarioObject = reinterpret_cast<ScenarioObject*>(pointer);
    }


#ifdef OLDSCENARIO
    // Some items in the tree view are just data fields; if the user has
    // double clicked on one, we'll invoke the editor on the item's
    // parent. This makes things a bit more convenient for the user.
    if (scenarioObject == NULL)
    {
        item = item->parent();

        if (item != NULL)
        {
            QVariant data = item->data(0, ScenarioObjectRole);
            if (qVariantCanConvert<void*>(data))
            {
                void* pointer = qVariantValue<void*>(data);
                scenarioObject = reinterpret_cast<ScenarioObject*>(pointer);
            }
        }
    }
#endif



    if (scenarioObject != NULL && item != NULL && column == 1)
    {
        editItemInline(item, 1);    // Guillermo says: we allow editing in line only on the second column
        //out << "Editing column 1 " << endl;
    }
    else if (scenarioObject != NULL && item != NULL && column == 0)
    {
        if (dynamic_cast<SpaceScenario*>(scenarioObject))
        {
            item->setFlags(item->flags() & (~Qt::ItemIsEditable));
            // Guillermo says: do not allow editing in line on 1st column and for the space scenario row
        }
        else if (dynamic_cast<ScenarioElementIdentifierType*>(scenarioObject))
        {
            item->setFlags(item->flags() & (~Qt::ItemIsEditable));
            // Guillermo says: do not allow editing in line on 1st column and for the element identifier
        }
        else
        {
            editScenarioObject(scenarioObject, item); // Guillermo says: allow editing the rest
        }
    }

}


/* This slot is signalled when an inline edit is performed on an item
 * in the scenario tree.
 */
void ScenarioTree::editItemInline(QTreeWidgetItem* item, int column)
{

    QTextStream out (stdout);

    ScenarioObject* object = objectForItem(item);
    SpaceScenario* scenario = dynamic_cast<SpaceScenario*>(object);
    if (object == NULL)
        return;

    if (dynamic_cast<SpaceScenario*>(object) && column == 1)	// Guillermo says: the name of the scenario
    {
        // TODO: replace this special case with reflection/introspection methods for ScenarioObject
        item->setFlags(item->flags() | (Qt::ItemIsEditable));
        scenario->setName(item->text(1));
        //updateTreeItems(item, scenario);  // do not do that. The complete scenario will disapear
    }
    if (dynamic_cast<ScenarioParticipantType*>(object) && column == 1)  // Participant name
    {
        // TODO: replace this special case with reflection/introspection methods for ScenarioObject
        // Guillermo says: commented out for the time being forcing the user to use the ScenarioElementIdentifierType instead
        //item->setFlags(item->flags() | (Qt::ItemIsEditable));
        //dynamic_cast<ScenarioParticipantType*>(object)->setName(item->text(1));
    }
    else if (dynamic_cast<ScenarioElementIdentifierType*>(object) && column == 1)   // Guillermo says: the name of the arcs, payloads, etc.
    {
        item->setFlags(item->flags() | (Qt::ItemIsEditable));
        dynamic_cast<ScenarioElementIdentifierType*>(object)->setName(item->text(1));

        //Getting now to know the parent of this item
        QTreeWidgetItem* parentItem = item->parent();
        ScenarioObject* parentObject = objectForItem(parentItem);

        if (dynamic_cast<ScenarioParticipantType*>(parentObject))
        {
            ScenarioParticipantType* myPartipant = dynamic_cast<ScenarioParticipantType*>(parentObject);
            dynamic_cast<ScenarioParticipantType*>(myPartipant)->setName(item->text(1));
            parentItem->setText(1, item->text(1));
        }

        updateTreeItems(item, scenario);

    }

#if OLDSCENARIO
    ScenarioObject* object = objectForItem(item);
    if (object == NULL)
        return;

    // TODO: If we allow more objects to be editing in the scenario
    // tree view, we need to add an edit method.
    if (dynamic_cast<ScenarioParticipant*>(object) && column == 1)
    {
        // Change the name of a participant
        ScenarioParticipant* participant = dynamic_cast<ScenarioParticipant*>(object);
        participant->setName(item->text(1));
    }
    else if (dynamic_cast<SpaceScenario*>(object) && column == 1)
    {
        SpaceScenario* scenario = dynamic_cast<SpaceScenario*>(object);
        scenario->setName(item->text(1));
    }
#endif
}


/** Return the scenario object associated with the specified tree item; may
 *  return NULL, since some tree items refer to scenario object properties
 *  and not scenario objects themselves.
 */
ScenarioObject* ScenarioTree::objectForItem(QTreeWidgetItem* item) const
{
    ScenarioObject* scenarioObject = NULL;
    QVariant data = item->data(0, ScenarioObjectRole);

    if (qVariantCanConvert<void*>(data))
    {
        void* pointer = qVariantValue<void*>(data);
        scenarioObject = reinterpret_cast<ScenarioObject*>(pointer);
    }

    return scenarioObject;
}


/** Return the currently selected scenario object, or NULL if nothing is currently
  * selected.
  */
ScenarioObject* ScenarioTree::selectedObject() const
{
    QTreeWidgetItem* item = selectedItem();
    if (item != NULL)
    {
        return objectForItem(item);
    }
    else
    {
        return NULL;
    }
}


/** Return the currently selected tree item, or NULL if nothing is currently
  * selected.
  */
QTreeWidgetItem* ScenarioTree::selectedItem() const
{
    QModelIndexList selected = selectionModel()->selectedRows();
    if (!selected.isEmpty())
    {
        return static_cast<QTreeWidgetItem*>(selected.first().internalPointer());
    }

    return NULL;
}


void ScenarioTree::removeItem(QTreeWidgetItem* item)
{
    removeItemWidget(item, 0);
}


/** Remove the selected item. This has no effect unless the selected
  * item represents a removeable object, e.g. a participant or
  * trajectory segment.
  */
void ScenarioTree::removeSelection()
{
    QModelIndexList selected = selectionModel()->selectedRows();
    QTreeWidgetItem* item = NULL;
    QModelIndex selectedIndex;
    if (!selected.isEmpty())
    {
        selectedIndex = selected.first();
        item = static_cast<QTreeWidgetItem*>(selected.first().internalPointer());
    }

    if (!item)
    {
        // Nothing selected, so nothing to do
        return;
    }

    ScenarioObject* obj = selectedObject();
    Q_ASSERT(obj != NULL);

    QTreeWidgetItem* parentItem = item->parent();
    ScenarioObject* parentObj = objectForItem(parentItem);

    int childIndex = parentItem->indexOfChild(item);

    if (childIndex >= 0)
    {
        // Only mission arcs and participants may be removed
        bool removed = false;
        if (dynamic_cast<SpaceScenario*>(parentObj))
        {
            dynamic_cast<SpaceScenario*>(parentObj)->AbstractParticipant().removeAt(childIndex);
            removed = true;
        }
        else if (dynamic_cast<ScenarioTrajectoryPlan*>(parentObj))
        {
            dynamic_cast<ScenarioTrajectoryPlan*>(parentObj)->AbstractTrajectory().removeAt(childIndex);
            removed = true;
        }

        if (removed)
        {
            model()->removeRow(selectedIndex.row(), selectedIndex.parent());
            clearSelection();
        }
    }
}
