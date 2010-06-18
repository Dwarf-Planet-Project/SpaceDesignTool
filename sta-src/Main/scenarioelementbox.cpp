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
  Modified by Tiziana Sabatini May 2009
  Modified by Guillermo June 3rd 2009
  Modified by Valentino Zuccarelli June 14th 2009
  Extensvely modified by Guillermo on October 2009 to include TLEs
  Modified by Guillermo April 2010 to include coverage
  Modified by Guillermo May 2010 to change the big integers of Ricardo into floats
  Modified by Guillermo to add labels to identify the scenario elements
 */

#include "scenarioelementbox.h"
#include "sheetDelegate.h"
#include "Scenario/scenario.h"
#include "Astro-Core/stamath.h"
#include "Astro-Core/calendarTOjulian.h"
#include "Astro-Core/date.h"
#include <Astro-Core/constants.h>
#include <QPushButton>
#include <QHeaderView>
#include <QMimeData>
#include <QTextStream>
#include <QtDebug>


static const QString SCENARIO_MIME_TYPE("application/sta.scenario.xml");

const QString ScenarioElementBox::PARTICIPANT_MIME_TYPE("application/sta.participant.xml");
const QString ScenarioElementBox::MISSION_ARC_MIME_TYPE("application/sta.missionarc.xml");
const QString ScenarioElementBox::MANEUVER_MIME_TYPE("application/sta.maneuver.xml");
const QString ScenarioElementBox::PAYLOAD_MIME_TYPE("application/sta.payload.xml"); //Line added by Ricardo Noriega

static const int ScenarioFragmentRole     = Qt::UserRole + 1;
static const int ScenarioFragmentTypeRole = Qt::UserRole + 2;

ScenarioElementWidget::ScenarioElementWidget(QWidget* parent) :
    QTreeWidget(parent)
{
    setHeaderHidden(true);
    setRootIsDecorated(false);
    //setIconSize(QSize(32, 32));
    setIconSize(QSize(25, 25)); // Smaller icons are a bit better
    //QFont font("Helvetica", 10); setFont(font);
    setItemDelegate(new SheetDelegate(this));
    setAlternatingRowColors(true);
    setDragDropMode(DragOnly);
    setAutoFillBackground(true);
    model()->setSupportedDragActions(Qt::CopyAction);
}


QStringList ScenarioElementWidget::mimeTypes() const
{
    QStringList types;
    types << SCENARIO_MIME_TYPE;
    return types;
}


QMimeData* ScenarioElementWidget::mimeData(const QList<QTreeWidgetItem*> items) const
{
    QMimeData *mimeData = new QMimeData();
    QByteArray encodedData;
    QString mimeType;

    foreach (QTreeWidgetItem* item, items)
    {
        if (item)
        {
            QString itemMimeType = item->data(0, ScenarioFragmentTypeRole).toString();
            if (!itemMimeType.isEmpty())
            {
                if (mimeType.isEmpty())
                {
                    mimeType = itemMimeType;
                }

                // Mime types of all dragged items must match mime type of
                // first item.
                if (mimeType == itemMimeType)
                {
                    encodedData += item->data(0, ScenarioFragmentRole).toByteArray();
                }
            }
        }
    }

    mimeData->setData(mimeType, encodedData);

    return mimeData;
}


static QString fragmentText(QDomElement e)
{
    QDomDocumentFragment fragment = e.toDocumentFragment();
    QString text;
    QTextStream stream(&text, QIODevice::WriteOnly);
    e.save(stream, 1);

    return text;
}



// Set the mime type and data for drag and drop
static void setDragAndDropInfo(QTreeWidgetItem* item,
			       const QString& mimeType,
			       const QByteArray& mimeData)
{
    item->setData(0, ScenarioFragmentTypeRole, mimeType);
    item->setData(0, ScenarioFragmentRole, mimeData);

}




////////////////////////  Creating PARTICIPANTS fragments /////////////////////////////////

// Create a scenario fragment representing a single space vehicle.
static QByteArray spaceVehicleFragment(const char* name, const char* vehicleType)
{
    ScenarioSC spacecraft;

    /*** fill in defaults ***/
    spacecraft.ElementIdentifier()->setName(name);

    QDomDocument doc;
    return fragmentText(CreateSCElement(&spacecraft, doc)).toUtf8();
}


//Created by Dominic to allow REV to be dragged and dropped

static QByteArray REVFragment(const char* name, const char* vehicleType)
{
    ScenarioREV entryVehicle;
    
    /*** fill in defaults ***/
    entryVehicle.ElementIdentifier()->setName(name);

    entryVehicle.REVSystem()->Geometry()->setNoseRadius(1);
    entryVehicle.REVSystem()->Weights()->setTotalDryMass(2000);

    entryVehicle.REVSystem()->AeroThermodynamics()->setReferenceArea(110);
    entryVehicle.REVSystem()->AeroThermodynamics()->setReferenceLength(13);


    entryVehicle.REVSystem()->AeroThermodynamics()->momentReferencePoint().append(0);
    entryVehicle.REVSystem()->AeroThermodynamics()->momentReferencePoint().append(0);
    entryVehicle.REVSystem()->AeroThermodynamics()->momentReferencePoint().append(0);

    entryVehicle.REVSystem()->AeroThermodynamics()->setCoefficientType(1);
    entryVehicle.REVSystem()->Geometry()->setGeometryFile("horus.wgs");
    //allocate memory for list of aerodynamic coefficient files
    ScenarioAeroCoefFileType * coef1 = new ScenarioAeroCoefFileType;
    coef1->setCoefName("CD");
    coef1->setFileLocation("horus_f_CD.stad");
    entryVehicle.REVSystem()->AeroThermodynamics()->AeroCoefFile().append(QSharedPointer<ScenarioAeroCoefFileType>(coef1));
    ScenarioAeroCoefFileType * coef2 = new ScenarioAeroCoefFileType;
    coef2->setCoefName("CS");
    coef2->setFileLocation("horus_f_CS.stad");
    entryVehicle.REVSystem()->AeroThermodynamics()->AeroCoefFile().append(QSharedPointer<ScenarioAeroCoefFileType>(coef2));
    ScenarioAeroCoefFileType * coef3 = new ScenarioAeroCoefFileType;
    coef3->setCoefName("CL");
    coef3->setFileLocation("horus_f_CL.stad");
    entryVehicle.REVSystem()->AeroThermodynamics()->AeroCoefFile().append(QSharedPointer<ScenarioAeroCoefFileType>(coef3));
    ScenarioAeroCoefFileType * coef4 = new ScenarioAeroCoefFileType;
    coef4->setCoefName("Cl");
    entryVehicle.REVSystem()->AeroThermodynamics()->AeroCoefFile().append(QSharedPointer<ScenarioAeroCoefFileType>(coef4));
    ScenarioAeroCoefFileType * coef5 = new ScenarioAeroCoefFileType;
    coef5->setCoefName("Cm");
    entryVehicle.REVSystem()->AeroThermodynamics()->AeroCoefFile().append(QSharedPointer<ScenarioAeroCoefFileType>(coef5));
    ScenarioAeroCoefFileType * coef6 = new ScenarioAeroCoefFileType;
    coef6->setCoefName("Cn");
    entryVehicle.REVSystem()->AeroThermodynamics()->AeroCoefFile().append(QSharedPointer<ScenarioAeroCoefFileType>(coef6));

     QDomDocument doc;
    return fragmentText(CreateREVElement(&entryVehicle, doc)).toUtf8();
}




static QByteArray groundStationFragment(const char* name,
					const char* centralBody,
					double latitude = 0.0,
					double longitude = 0.0,
					double altitude = 0.0)
{
    ScenarioGroundStation groundStation;
    /*** fill in defaults ***/
    groundStation.ElementIdentifier()->setName(name);
    groundStation.setName(name);               // Guillermo says: this has to disapear in the future
    groundStation.Location()->setCentralBody(centralBody);

    QSharedPointer<ScenarioGroundPositionType> groundPosition(new ScenarioGroundPositionType());
    groundPosition->setLatitude(latitude);
    groundPosition->setLongitude(longitude);
    groundPosition->setAltitude(altitude);
    groundStation.Location()->setAbstract3DOFPosition(groundPosition);
    groundStation.GroundStationEnvironment()->Rain()->setPercentageExceededLimit(0.01); //Line added by Ricardo to put this value as a default
    groundStation.GroundStationEnvironment()->setOxChoice("true");
    groundStation.GroundStationEnvironment()->setWaterVapourChoice("true");
    groundStation.GroundStationEnvironment()->Rain()->setRainChoice("true");

    QDomDocument doc;
    return fragmentText(CreateGroundStationElement(&groundStation, doc)).toUtf8();
}


static QByteArray launchPadFragment(const char* name,
				    const char* centralBody,
				    double latitude = 0.0,
				    double longitude = 0.0,
				    double altitude = 0.0)
{
    ScenarioLaunchPad launchPad;
    /*** fill in defaults ***/
    launchPad.ElementIdentifier()->setName(name);
    launchPad.setName(name);    // Guillermo says: this has to disapear in the future
    launchPad.Location()->setCentralBody(centralBody);

    QSharedPointer<ScenarioGroundPositionType> groundPosition(new ScenarioGroundPositionType());
    groundPosition->setLatitude(latitude);
    groundPosition->setLongitude(longitude);
    groundPosition->setAltitude(altitude);
    launchPad.Location()->setAbstract3DOFPosition(groundPosition);

    QDomDocument doc;
    return fragmentText(CreateLaunchPadElement(&launchPad, doc)).toUtf8();
}

static QByteArray pointFragment(const char* name,
				const char* centralBody,
				double latitude = 0.0,
				double longitude = 0.0,
				double altitude = 0.0)
{
    ScenarioPoint myPoint;
    /*** fill in defaults ***/
    myPoint.ElementIdentifier()->setName(name);
    myPoint.setName(name);  // Guillermo says: this has to disapear in the future
    myPoint.Location()->setCentralBody(centralBody);

    QSharedPointer<ScenarioGroundPositionType> groundPosition(new ScenarioGroundPositionType());
    groundPosition->setLatitude(latitude);
    groundPosition->setLongitude(longitude);
    groundPosition->setAltitude(altitude);
    myPoint.Location()->setAbstract3DOFPosition(groundPosition);

    QDomDocument doc;
    return fragmentText(CreatePointElement(&myPoint, doc)).toUtf8();
}



// Create a scenario fragment representing a ground element.
static QByteArray groundElementFragment(const char* name,
					const char* groundElementType,
					const char* centralBody,
					double latitude = 0.0,
					double longitude = 0.0,
					double altitude = 0.0)
{
    QByteArray encodedData;

    encodedData +=
    "<";
    encodedData += groundElementType;
    encodedData += " Name=\"";
    encodedData += name;
    encodedData += "\">"
    "<Location>"
    "<GroundPosition>"
    "<Latitude>" + QString::number(latitude) + "</Latitude>"
    "<Longitude>" + QString::number(longitude) + "</Longitude>"
    "<Altitude>" + QString::number(altitude) + "</Altitude>"
    "</GroundPosition>";
    encodedData += "<CentralBody>";
    encodedData += centralBody;
    encodedData += "</CentralBody>"
    "</Location>";

    // Clearing altitude is a required element for launch pads.
    if (QString(groundElementType) == "LaunchPad")
    {
	encodedData += "<LaunchPadClearingAltitude>10</LaunchPadClearingAltitude>";
    }

    encodedData += "</";
    encodedData += groundElementType;
    encodedData += ">";

    return encodedData;
}







////////////////////////  Creating MISSION ARCS fragments /////////////////////////////////


static QByteArray loiteringFragment(const char* name)
{
    ScenarioLoiteringType loitering;

    /*** fill in defaults ***/
    loitering.Environment()->CentralBody()->setName("EARTH");
    loitering.InitialPosition()->setCoordinateSystem("INERTIAL J2000");
    loitering.InitialAttitude()->setCoordinateSystem("EULER 123");

    // Patched by Guillermo April 23 2010 to get default values for ISS orbits
    QSharedPointer<ScenarioKeplerianElementsType> initPos(new ScenarioKeplerianElementsType());
    initPos->setSemiMajorAxis(6378.0+450.1365);
    initPos->setInclination(51.6651);
    initPos->setEccentricity(0.0006);
    initPos->setRAAN(12.4829);
    initPos->setArgumentOfPeriapsis(45.0278);
    initPos->setTrueAnomaly(87.3523);
    loitering.InitialPosition()->setAbstract6DOFPosition(initPos);
    QSharedPointer<ScenarioEulerBIType> initAtt(new ScenarioEulerBIType());
    initAtt->setPhi(0.00000);
    initAtt->setTheta(0.00000);
    initAtt->setPsi(0.00000);
    initAtt->setPhiDot(0.00000);
    initAtt->setThetaDot(0.00000);
    initAtt->setPsiDot(0.00000);
    loitering.InitialAttitude()->setAbstract6DOFAttitude(initAtt);

    loitering.TimeLine()->setStartTime(QDateTime(QDate(2012, 1, 1)));
    loitering.TimeLine()->setEndTime(QDateTime(QDate(2012, 1, 2)));
    loitering.TimeLine()->setStepTime(60.0);

    loitering.PropagationPosition()->setPropagator("TWO BODY");
    loitering.PropagationPosition()->setTimeStep(60.0);

    loitering.ElementIdentifier()->setName("loitering");

    QDomDocument doc;
    return fragmentText(CreateLoiteringElement(&loitering, doc)).toUtf8();
}



// Re-entry misison arc fragment
static QByteArray reentryFragment(const char* name) //Modified by Dominic to allow dragging of entry Arc
{
    ScenarioEntryArcType entry;

    // fill in defaults
    entry.Environment()->CentralBody()->setName("EARTH");
    entry.InitialPosition()->setCoordinateSystem("INERTIAL J2000");

    QSharedPointer<ScenarioSphericalCoordinatesType> initPos(new ScenarioSphericalCoordinatesType());
    initPos->setRadialDistance(6578.21);
    initPos->setFlightPathVelocity(6);
    initPos->setFlightPathAngle(sta::degToRad(-5.0));
    entry.InitialPosition()->setAbstract6DOFPosition(initPos);

    entry.PropagationPosition()->setTimeStep(5);
    entry.Constraints()->setMaxNormalLoad(10000.00);
    entry.Constraints()->setMaxHeatFlux(500.00);
    entry.Constraints()->setMaxAltitude(10000000.00);

    // Patched by Guillermo to include an initial time in the future
    entry.TimeLine()->setStartTime(QDateTime(QDate(2012, 1, 1)));
    //entry.TimeLine()->setStepTime(5.0);
    // Patched by Guillermo to include a full initial state vector
    initPos->setHeadingAngle(sta::degToRad(45.45));
    initPos->setLatitude(sta::degToRad(38.45));
    initPos->setLongitude(sta::degToRad(350.32));

    QDomDocument doc;
    return fragmentText(CreateEntryArcElement(&entry, doc)).toUtf8();
}



// Loitering for TLE elemennts fragment
static QByteArray loiteringTLEFragment(const char* name)
{
    ScenarioLoiteringTLEType loiteringTLE;

    /*** fill in defaults ***/
    loiteringTLE.TimeLine()->setEndTime(QDateTime(QDate(2009, 10, 11), QTime(12, 0, 0)));
    loiteringTLE.TimeLine()->setStartTime(QDateTime(QDate(2009, 10, 12), QTime(12, 0, 0)));
    loiteringTLE.TimeLine()->setStepTime(60.0);
    loiteringTLE.setTleLine0("ISS (ZARYA)");
    loiteringTLE.setTleLine1("1 25544U 98067A   09282.91732946  .00013034  00000-0  90850-4 0  7559");
    loiteringTLE.setTleLine2("2 25544  51.6398  77.7469 0007731 180.0753 317.5762 15.74717336624025");

    QDomDocument doc;
    return fragmentText(CreateLoiteringTLEElement(&loiteringTLE, doc)).toUtf8();
}



// External trajectories fragments
static QByteArray externalFragment(const char* name)
{
    QByteArray data;

    data += "<?xml version=\"1.0\" encoding=\"UTF-8\"?>";
    data += "<External Name=\"";
    data += name;
    data += "\">";
    data +=
    "<Timeline>"
    "  <StartTime>2010-01-01T00:00:00</StartTime>"
    "  <StepTime unit=\"s\">60.0</StepTime>"
    "  <EndTime>2010-01-01T01:01:00</EndTime>"
    "</Timeline>"
    "<StateVector1>2455563, 6778, 0, 0, 0, 4.76335586717, 6.00985940331</StateVector1>"
    "<StateVector2>2455563.0006944, 6762.38865439, 285.581895497, 360.314678963, -0.520178335644, 4.75238472603, 5.99601726814</StateVector2>"
    "</External>";

    return data;
}



////////////////////////  Creating PAYLOADS fragments /////////////////////////////////

// These lines added by Ricardo Noriega. Creates a Tx payload fragment representing a single payload and initialized the default values.
static QByteArray transmitterPayloadFragment(const char* name)
{
    ScenarioTransmitterPayloadType transmitterPayload;
    double ElInit=(90*DEG2RAD);
    transmitterPayload.Transmitter()->PointingDirection()->setElevation(ElInit);
    transmitterPayload.Transmitter()->PointingDirection()->setAzimuth(0);

    transmitterPayload.Transmitter()->EMproperties()->setEfficiency(55);
    transmitterPayload.Transmitter()->EMproperties()->setTiltAngle(0);
    transmitterPayload.Transmitter()->EMproperties()->setGainMax(30);
    transmitterPayload.Transmitter()->EMproperties()->setDiameter(0);
    transmitterPayload.Transmitter()->EMproperties()->setAngularBeamWidth(0);
    transmitterPayload.Transmitter()->EMproperties()->setPolarisation("Linear");
    transmitterPayload.Transmitter()->EMproperties()->setBandWidth(32000000.0);//in Hz

    transmitterPayload.Transmitter()->setTransmittingPower(1000.0);
    transmitterPayload.Budget()->setFrequencyBand(14500000000.0);//It is in hertz!!

    transmitterPayload.Transmitter()->setDepointingLossTx(0);
    transmitterPayload.Transmitter()->setFedderLossTx(0);

    transmitterPayload.Transmitter()->Modulation()->setDataRate(16);
    transmitterPayload.Transmitter()->Modulation()->setModulationType("BPSK");

    QDomDocument doc;
    return fragmentText(CreateTransmitterPayloadElement(&transmitterPayload, doc)).toUtf8();
}


// These lines added by Ricardo Noriega. Creates a Rx payload fragment representing a single payload and initialized the default values.
static QByteArray receiverPayloadFragment(const char* name)
{
    ScenarioReceiverPayloadType receiverPayload;

    double ElInit=(90*DEG2RAD);
    receiverPayload.Receiver()->PointingDirection()->setElevation(ElInit);
    receiverPayload.Receiver()->PointingDirection()->setAzimuth(0);
    receiverPayload.Receiver()->EMproperties()->setEfficiency(55);
    receiverPayload.Receiver()->EMproperties()->setGainMax(30);
    receiverPayload.Receiver()->EMproperties()->setTiltAngle(0);
    receiverPayload.Receiver()->EMproperties()->setPolarisation("Linear");
    receiverPayload.Receiver()->EMproperties()->setBandWidth(32000000.0);//in Hz


    receiverPayload.Receiver()->setDepointingLossRx(0);
    receiverPayload.Receiver()->setFeederLossRx(0);

    receiverPayload.Budget()->setFrequencyBand(14500000000.0);//It's in hertz


    receiverPayload.Receiver()->SystemTemperature()->setRxNoiseFigure(1);
    receiverPayload.Receiver()->SystemTemperature()->setThermoFeeder(290);
    receiverPayload.Receiver()->SystemTemperature()->setThermoReveicer(290);

    receiverPayload.Receiver()->SystemTemperature()->setChoiceTantenna("calculated");




    QDomDocument doc;
    return fragmentText(CreateReceiverPayloadElement(&receiverPayload, doc)).toUtf8();
}



// Guillermo on optical payloads
static QByteArray opticalPayloadFragment(const char* name)
{
    ScenarioOpticalPayloadType opticalPayload;

    QDomDocument doc;
    return fragmentText(CreateOpticalPayloadElement(&opticalPayload, doc)).toUtf8();
}

// Guillermo on optical payloads
static QByteArray radarPayloadFragment(const char* name)
{
    ScenarioRadarPayloadType radarPayload;

    QDomDocument doc;
    return fragmentText(CreateRadarPayloadElement(&radarPayload, doc)).toUtf8();
}



//////////////////////////// Methods for manipulating pre-created PARTICIPANTS ////////////////////////////////

////  Guillermo: Ground stations (ESA, NASA, etc)
static void
addGroundStationItem(QTreeWidgetItem* parent,
                     const char* name,
                     const char* centralBody,
                     double latitude,
                     double longitude,
                     double altitude)
{
    QTreeWidgetItem* groundStationItem   = new QTreeWidgetItem(parent);
    groundStationItem->setText(0, name);
    groundStationItem->setIcon(0, QIcon(":/icons/ParticipantSTATION.png"));
    setDragAndDropInfo(groundStationItem,
                       ScenarioElementBox::PARTICIPANT_MIME_TYPE,
                       groundStationFragment(name, centralBody,
                                             latitude, longitude, altitude));
}


// New method by Guillermo to handle special satellites from ESA
static void
addESASatelliteItem(QTreeWidgetItem* parent, const char* name)
{
    QTreeWidgetItem* myESASatelliteItem   = new QTreeWidgetItem(parent);
    myESASatelliteItem->setText(0, name);
    myESASatelliteItem->setIcon(0, QIcon(":/icons/ParticipantSATELLITE.png"));
    setDragAndDropInfo(myESASatelliteItem,
                       ScenarioElementBox::PARTICIPANT_MIME_TYPE,
                       spaceVehicleFragment(name, "Satellite"));
}



/////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////  Creating the palette of the scenario element box ///////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////

ScenarioElementBox::ScenarioElementBox(QWidget* parent) :
    QWidget(parent)
{

    /*
    // Changing the standard font of this box
    QFont font("Helvetica", 10);  //QFont font("Helvetica",10,QFont::Condensed);
    setFont(font);
    */


#if 0
    setupUi(this);

    int num = ScenarioElementTree->topLevelItemCount();
    ScenarioElementTree->setItemDelegate(new SheetDelegate(ScenarioElementTree));
    for(int i=0; i<num; i++)
    {
        ScenarioElementTree->setItemExpanded(ScenarioElementTree->topLevelItem(i), true);
    }
#else
    QVBoxLayout* layout = new QVBoxLayout(this);

    m_elementTreeWidget = new ScenarioElementWidget();
    layout->addWidget(m_elementTreeWidget);

    QTreeWidgetItem* participantsItem = new QTreeWidgetItem();
    participantsItem->setText(0, tr("Participants"));
    QTreeWidgetItem* missionArcsItem = new QTreeWidgetItem();
    missionArcsItem->setText(0, tr("Mission Arcs"));
    // Next lines patch by Ricardo Noriega to include payloads into the scenario tree box
    QTreeWidgetItem* payloadsItem = new QTreeWidgetItem();
    payloadsItem->setText(0, tr("Payloads"));
    QTreeWidgetItem* maneuversItem = new QTreeWidgetItem();
    maneuversItem->setText(0, tr("Maneuvers"));

    // Now creating optional widgets to ease the work of the user:
    // ESA satellites
    QTreeWidgetItem* ESASatellitesItem = new QTreeWidgetItem();
    ESASatellitesItem->setText(0, tr("ESA Satellites"));

    // Ground stations
    QTreeWidgetItem* ESAgroundStationsItem = new QTreeWidgetItem();
    ESAgroundStationsItem->setText(0, tr("ESA Ground Stations"));
    // Next lines patched by Guillermo to include NASA stations
    QTreeWidgetItem* NASAgroundStationsItem = new QTreeWidgetItem();
    NASAgroundStationsItem->setText(0, tr("NASA Ground Stations"));


    // Adding the widgets
    m_elementTreeWidget->clear();
    m_elementTreeWidget->addTopLevelItem(participantsItem);
    m_elementTreeWidget->addTopLevelItem(missionArcsItem);
    m_elementTreeWidget->addTopLevelItem(payloadsItem);
    m_elementTreeWidget->addTopLevelItem(maneuversItem);
    //m_elementTreeWidget->addTopLevelItem(ESASatellitesItem);
    m_elementTreeWidget->addTopLevelItem(ESAgroundStationsItem);
    m_elementTreeWidget->addTopLevelItem(NASAgroundStationsItem);


    ///////// Creating now the participants in orden of flight:
    /////////Launcher, satellite, re-entry, ground station, target, launch pad

    // Diabling the complete function for the time being. Patched by Guillermo
    //QTreeWidgetItem* launcherItem        = new QTreeWidgetItem(participantsItem);
    //launcherItem->setText(0, tr("Launcher"));
    //launcherItem->setIcon(0, QIcon(":/icons/ParticipantROCKET.png"));
    //setDragAndDropInfo(launcherItem,
    //                   PARTICIPANT_MIME_TYPE,
    //                   spaceVehicleFragment("New Launcher", "Launcher"));


    QTreeWidgetItem* satelliteItem       = new QTreeWidgetItem(participantsItem);
    satelliteItem->setText(0, tr("Satellite"));
    satelliteItem->setIcon(0, QIcon(":/icons/ParticipantSATELLITE.png"));
    setDragAndDropInfo(satelliteItem,
                       PARTICIPANT_MIME_TYPE,
                       spaceVehicleFragment("New Satellite", "Satellite"));

    QTreeWidgetItem* reentryVehicleItem  = new QTreeWidgetItem(participantsItem);
    reentryVehicleItem->setText(0, tr("Reentry vehicle"));
    reentryVehicleItem->setIcon(0, QIcon(":/icons/ParticipantENTRYVEHICLE.png"));
    setDragAndDropInfo(reentryVehicleItem,
		       PARTICIPANT_MIME_TYPE,
		       REVFragment("New Reentry Vehicle", "Reentry Vehicle"));

    QTreeWidgetItem* groundStationItem   = new QTreeWidgetItem(participantsItem);
    groundStationItem->setText(0, tr("Station"));
    groundStationItem->setIcon(0, QIcon(":/icons/ParticipantSTATION.png"));
    setDragAndDropInfo(groundStationItem,
		       PARTICIPANT_MIME_TYPE,
		       groundStationFragment("New Ground Station", "Earth", 0.0, 0.0, 0.0));

    QTreeWidgetItem* targetItem          = new QTreeWidgetItem(participantsItem);
    targetItem->setText(0, tr("Point"));
    targetItem->setIcon(0, QIcon(":/icons/ParticipantPOINT.png"));
    setDragAndDropInfo(targetItem,
                       PARTICIPANT_MIME_TYPE,
		       pointFragment("New Point", "Earth", 0.0, 0.0, 0.0));

    // Diabling the complete function for the time being. Patched by Guillermo
    //QTreeWidgetItem* launchPadItem       = new QTreeWidgetItem(participantsItem);
    //launchPadItem->setText(0, tr("Launch Pad"));
    //launchPadItem->setIcon(0, QIcon(":/icons/participantLaunchPad.png"));
    //setDragAndDropInfo(launchPadItem,
    //                   PARTICIPANT_MIME_TYPE,
    //                   groundElementFragment("New Launch Pad", "LaunchPad", "Earth"));



    /////////// Creating the widgets for the different payload: Tx, Rx, optical, X-ray, radar, etc

    //These lines are added by Ricardo to create the widget of the communication payload
    // Patched by Guillermo to change the icon
    QTreeWidgetItem* transmitterPayloadItem  = new QTreeWidgetItem(payloadsItem);
    transmitterPayloadItem->setText(0, tr("Transmitter"));
    transmitterPayloadItem->setIcon(0, QIcon(":/icons/ParticipantCOMMUNICATIONS.png"));
    setDragAndDropInfo(transmitterPayloadItem,
                       PAYLOAD_MIME_TYPE,
		       transmitterPayloadFragment("Tx 1"));

    QTreeWidgetItem* receiverPayloadItem  = new QTreeWidgetItem(payloadsItem);
    receiverPayloadItem->setText(0, tr("Receiver"));
    receiverPayloadItem->setIcon(0, QIcon(":/icons/ParticipantCOMMUNICATIONS.png"));
    setDragAndDropInfo(receiverPayloadItem,
		       PAYLOAD_MIME_TYPE,
		       receiverPayloadFragment("Rx 1"));

    QTreeWidgetItem* opticalPayloadItem  = new QTreeWidgetItem(payloadsItem);
    opticalPayloadItem->setText(0, tr("Telescope"));
    opticalPayloadItem->setIcon(0, QIcon(":/icons/ParticipantTELESCOPE.png"));
    setDragAndDropInfo(opticalPayloadItem,
		       PAYLOAD_MIME_TYPE,
		       opticalPayloadFragment("Optical 1"));

    QTreeWidgetItem* radarPayloadItem  = new QTreeWidgetItem(payloadsItem);
    radarPayloadItem->setText(0, tr("Radar"));
    radarPayloadItem->setIcon(0, QIcon(":/icons/ParticipantRADAR.png"));
    setDragAndDropInfo(radarPayloadItem,
		       PAYLOAD_MIME_TYPE,
		       radarPayloadFragment("Radar 1"));



    ///////////////////// Creating now the mission arcs: ascent, loitering, fly-by, re-entry, rendezvous

    // Disabling the complete function for the time being. Patched by Guillermo
    //QTreeWidgetItem* ascentItem       = new QTreeWidgetItem(missionArcsItem);
    //ascentItem->isDisabled();
    //ascentItem->setText(0, tr("Ascent"));
    //ascentItem->setIcon(0, QIcon(":/icons/mission-arcs-ascent.png"));

    QTreeWidgetItem* loiteringItem    = new QTreeWidgetItem(missionArcsItem);
    loiteringItem->setText(0, tr("Loitering"));
    loiteringItem->setIcon(0, QIcon(":/icons/mission-arcs-loitering.png"));
    setDragAndDropInfo(loiteringItem,
		       MISSION_ARC_MIME_TYPE,
		       loiteringFragment("Loitering 1"));

    QTreeWidgetItem* loiteringTLEItem    = new QTreeWidgetItem(missionArcsItem);
    loiteringTLEItem->setText(0, tr("TLE"));
    loiteringTLEItem->setIcon(0, QIcon(":/icons/TLE-icon-small.png"));
    setDragAndDropInfo(loiteringTLEItem,
		       MISSION_ARC_MIME_TYPE,
		       loiteringTLEFragment("LoiteringTLE 1"));

    // Diabling the complete function for the time being. Patched by Guillermo
    //QTreeWidgetItem* flybyItem        = new QTreeWidgetItem(missionArcsItem);
    //flybyItem->setText(0, tr("Fly-by"));
    //flybyItem->setIcon(0, QIcon(":/icons/mission-arcs-flyby.png"));
    //setDragAndDropInfo(flybyItem,
    //		       MISSION_ARC_MIME_TYPE,
    //		       flybyFragment("Fly-by 1"));


    QTreeWidgetItem* reentryItem      = new QTreeWidgetItem(missionArcsItem);
    reentryItem->setText(0, tr("Entry"));
    reentryItem->setIcon(0, QIcon(":/icons/mission-arcs-reentry.png"));
    setDragAndDropInfo(reentryItem,
                       MISSION_ARC_MIME_TYPE,
                       reentryFragment("ReEntry 1"));

    // Diabling the complete function for the time being. Patched by Guillermo
    //QTreeWidgetItem* rendezvousItem   = new QTreeWidgetItem(maneuversItem);
    //rendezvousItem->setText(0, tr("Rendezvous plan"));
    //rendezvousItem->setIcon(0, QIcon(":/icons/mission-arcs-rendezvous.png"));
    //setDragAndDropInfo(rendezvousItem,
    //                   MISSION_ARC_MIME_TYPE,
    //                   rendezvousFragment("Rendezvous 1"));




#ifdef OLDSCENARIO
    QTreeWidgetItem* externalItem    = new QTreeWidgetItem(missionArcsItem);
    externalItem->setText(0, tr("External"));
    externalItem->setIcon(0, QIcon(":/icons/external.png"));
    setDragAndDropInfo(externalItem,
		       MISSION_ARC_MIME_TYPE,
		       externalFragment("External 1"));

    QTreeWidgetItem* lagrangianItem    = new QTreeWidgetItem(missionArcsItem);
    lagrangianItem->setText(0, tr("Lagrangian"));
    lagrangianItem->setIcon(0, QIcon(":/icons/3rdbody_icon.png"));
    setDragAndDropInfo(lagrangianItem,
                       MISSION_ARC_MIME_TYPE,
                       lagrangianFragment("Lagrangian 1"));



    QTreeWidgetItem* impulseItem      = new QTreeWidgetItem(maneuversItem);
    impulseItem->setText(0, tr("Single impulse"));
    impulseItem->setIcon(0, QIcon(":/icons/engine.png"));
    setDragAndDropInfo(impulseItem,
		       MISSION_ARC_MIME_TYPE,
		       impulseFragment("Single-impulse 1"));
#endif

    //////////////// Adding now concrete widgets into the scenario

    // Some  ESA Satellites
    //addESASatelliteItem(ESASatellitesItem, "Cryosat-2");

    // The ESTRACK ground stations (LAT, LON, ALT)
    addGroundStationItem(ESAgroundStationsItem, "Cebreros",     "Earth",  40.45268986,  -4.36754881, 794.1);
    addGroundStationItem(ESAgroundStationsItem, "Kiruna",      "Earth",  67.8571252,   20.96434169, 402.275);
    addGroundStationItem(ESAgroundStationsItem, "Kourou",      "Earth",   5.25143694, -52.8046624,   14.561);
    addGroundStationItem(ESAgroundStationsItem, "Malargue",    "Earth",  -35.775972,     -69.398250,  1525.20);
    addGroundStationItem(ESAgroundStationsItem, "Malindi",     "Earth",  -2.9955576,   40.194505,    12.314);
    addGroundStationItem(ESAgroundStationsItem, "Maspalomas",  "Earth",  27.762892,   -15.6338072,  204.9);
    addGroundStationItem(ESAgroundStationsItem, "New Norcia",  "Earth", -31.048225,   116.191500,   252.26);
    addGroundStationItem(ESAgroundStationsItem, "Perth",       "Earth", -31.802525,   115.8851556,   22.156);
    addGroundStationItem(ESAgroundStationsItem, "Redu",        "Earth",  50.0020552,    5.146231,   385.583);
    addGroundStationItem(ESAgroundStationsItem, "Santa Maria",  "Earth",  36.99694,    -25.1361027,   276.00);
    addGroundStationItem(ESAgroundStationsItem, "Santiago",     "Earth",  -33.151478,    -70.668306,   730.853);
    addGroundStationItem(ESAgroundStationsItem, "Svalbard",     "Earth",   78.229772,    15.407786,   501.3);
    //addGroundStationItem(ESAgroundStationsItem, "Villafranca", "Earth",  40.4455932,   -3.95260078, 664.425);

    // The NASA ground stations
    addGroundStationItem(NASAgroundStationsItem, "Camberra",	 "Earth", -35.4047,  148.9831, 680.0);
    addGroundStationItem(NASAgroundStationsItem, "Cape Canaveral","Earth", 28.4472,   -80.5649, -7.21);
    addGroundStationItem(NASAgroundStationsItem, "Colorado",    "Earth", 38.8060, -104.5285,   1907.51);
    addGroundStationItem(NASAgroundStationsItem, "Diego Garcia","Earth", -7.27,    72.37,  -68.38);
    addGroundStationItem(NASAgroundStationsItem, "Edwards",     "Earth", 34.9607,   -117.9116,    12.314);
    addGroundStationItem(NASAgroundStationsItem, "Goldstone",   "Earth", 35.2999,   -116.8054,  969.67);
    addGroundStationItem(NASAgroundStationsItem, "Oahu",	"Earth", 21.5721,   -158.2666,   301.32);
    addGroundStationItem(NASAgroundStationsItem, "Robledo",	"Earth", 40.4256,   -4.2541,   883.73);
    addGroundStationItem(NASAgroundStationsItem, "Vanderberg",  "Earth", 34.5827,    -120.5616,   627.54);
    addGroundStationItem(NASAgroundStationsItem, "Wallops",     "Earth", 37.9455,    -75.4611,   -18.51);
    addGroundStationItem(NASAgroundStationsItem, "White Sands", "Earth", 32.5005,    -106.6086,   1450.17);


    // Set properties for all top level items: expanded = true
    for (int i = 0; i < m_elementTreeWidget->topLevelItemCount(); i++)
    {
        QTreeWidgetItem* item = m_elementTreeWidget->topLevelItem(i);
        m_elementTreeWidget->setItemExpanded(item, true);
        item->setExpanded(true);
    }


    // Contracting now some elements that should not be shown to the user at a first sight.
    //ESASatellitesItem->setExpanded(false);
    ESAgroundStationsItem->setExpanded(false);
    NASAgroundStationsItem->setExpanded(false);

    setLayout(layout);

#endif
}


ScenarioElementBox::~ScenarioElementBox()
{
}


#ifdef OLDSCENARIO

// Obsolete code. There's no need to construct objects from inline XML;
// we can just call the appropriate scenario object constructors and set
// the object properties.

static QByteArray interplanetaryFragment(const char* name)
{
    QByteArray data;

    data += "<?xml version=\"1.0\" encoding=\"UTF-8\"?>";

    data += "<Interplanetary Name=\"";
    data += name;
    data += "\">";
    data +=
    "<Mode>1</Mode>";

    return data;
}

static QByteArray lagrangianFragment(const char* name)
{
    QByteArray data;
    data += "<?xml version=\"1.0\" encoding=\"UTF-8\"?>";

    data += "<Lagrangian Name=\"";
    data += name;
    data += "\">";

    data +=
    "<Threebodyenvironment>"
    "<FirstBody Name=\"SUN\">"
    "</FirstBody>"
    "<SecondBody Name=\"EARTH\">"
    "</SecondBody>"
    "</Threebodyenvironment>"
    "<SimulationParameters>"
    "  <Timeline>"
    "  <StartTime>2011-01-01T00:00:00</StartTime>"
    "  <StepTime unit=\"s\">60.0</StepTime>"
    "  <EndTime>2011-01-02T12:00:00</EndTime>"
    "  </Timeline>"
    "  <InitialStatePosition>"
    "  <CoordinateSystem>INERTIAL J2000</CoordinateSystem>"
    "  <KeplerianElements>"
    "    <SemiMajorAxis>6772</SemiMajorAxis>"
    "    <Eccentricity>0</Eccentricity>"
    "    <Inclination>0</Inclination>"
    "    <RAAN>0</RAAN>"
    "    <ArgumentOfPeriapsis>0</ArgumentOfPeriapsis>"
    "    <TrueAnomaly>0</TrueAnomaly>"
    "  </KeplerianElements>"
        "   <CentralsystemBody Name=\"EARTH\"></CentralsystemBody>"
    "  </InitialStatePosition>"
    "  <Deltav>"
    "    <Time>0.5</Time>"
    "    <X-Deltav unit=\"km/s\">1</X-Deltav>"
    "    <Y-Deltav unit=\"km/s\">1</Y-Deltav>"
    "    <Z-Deltav unit=\"km/s\">2</Z-Deltav>"
    "  </Deltav>"
    "</SimulationParameters>"
    "<HaloOrbit>"
    "<Lpoint>L1</Lpoint>"
    "<InitialStatePosition>"
    "<CoordinateSystem>INERTIAL J2000</CoordinateSystem>"
    "<StateVector>"
    "<x>0</x>"
    "<y>0</y>"
    "<z>0</z>"
    "<vx>0</vx>"
    "<vy>0</vy>"
    "<vz>0</vz>"
    "</StateVector>"
    "<CentralsystemBody Name=\"EARTH\"></CentralsystemBody>"
    "</InitialStatePosition>"
    "<Amplitudes>"
    "<xAmplitude>550000</xAmplitude>"
    "<zAmplitude>-250000</zAmplitude>"
    "</Amplitudes>"
    "<Manifolds>"
    "<Branch>"
    "<State>true</State>"
    "<Parameters>"
    "<Deviation>0</Deviation>"
    "<EndCondition>"
    "<intersection>1</intersection>"
    "<integrationTime>0</integrationTime>"
    "<numPositions>0</numPositions>"
    "</EndCondition>"
    "</Parameters>"
    "</Branch>"
    "<Branch>"
    "<State>false</State>"
    "<Parameters>"
    "<Deviation>0</Deviation>"
    "<EndCondition>"
    "<intersection>0</intersection>"
    "<integrationTime>1</integrationTime>"
    "<numPositions>30</numPositions>"
    "</EndCondition>"
    "</Parameters>"
    "</Branch>"
    "<Branch>"
    "<State>false</State>"
    "<Parameters>"
    "<Deviation>0</Deviation>"
    "<EndCondition>"
    "<intersection>1</intersection>"
    "<integrationTime>0</integrationTime>"
    "<numPositions>0</numPositions>"
    "</EndCondition>"
    "</Parameters>"
    "</Branch>"
    "<Branch>"
    "<State>false</State>"
    "<Parameters>"
    "<Deviation>0</Deviation>"
    "<EndCondition>"
    "<intersection>1</intersection>"
    "<integrationTime>0</integrationTime>"
    "<numPositions>0</numPositions>"
    "</EndCondition>"
    "</Parameters>"
    "</Branch>"
    "</Manifolds>"
    "</HaloOrbit>"
    "<ThreebodyTransfer>"
    "<StartingEpoch>2011-01-02T12:00:00</StartingEpoch>"
    "<ParkingOrbit>"
    "<InitialStatePosition>"
    "  <CoordinateSystem>INERTIAL J2000</CoordinateSystem>"
    "  <KeplerianElements>"
    "    <SemiMajorAxis>6672</SemiMajorAxis>"
    "    <Eccentricity>0</Eccentricity>"
    "    <Inclination>5</Inclination>"
    "    <RAAN>0</RAAN>"
    "    <ArgumentOfPeriapsis>0</ArgumentOfPeriapsis>"
    "    <TrueAnomaly>0</TrueAnomaly>"
    "  </KeplerianElements>"
    "   <CentralsystemBody Name=\"EARTH\"></CentralsystemBody>"
    "  </InitialStatePosition>"
    "</ParkingOrbit>"
    "<HaloOrbit>"
    "<InitialStatePosition>"
    "<CoordinateSystem>INERTIAL J2000</CoordinateSystem>"
    "<StateVector>"
    "<x>1.0081748644</x>"
    "<y>0</y>"
    "<z>0.0013593836</z>"
    "<vx>0</vx>"
    "<vy>0.0104429106</vy>"
    "<vz>0</vz>"
    "</StateVector>"
    "<CentralsystemBody Name=\"EARTH\"></CentralsystemBody>"
    "</InitialStatePosition>"
    "</HaloOrbit>"
    "<OptimizationParameters>"
    "<timeofFlight>0</timeofFlight>"
    "<maximumToF units=\"days\">100</maximumToF>"
    "<propellant>1</propellant>"
    "</OptimizationParameters>"
    "</ThreebodyTransfer>"
    "</Lagrangian>";

    return data;
}


static QByteArray reentryFragment(const char* name)
{
    QByteArray data;

    QString xmlTemplate = QString(
    "<?xml version=\"1.0\" encoding=\"UTF-8\"?>"
    "<ReEntry Name=\"%1\">"
    "<Environment>"
    "<CentralBody Name=\"EARTH\"></CentralBody>"
    "</Environment>"
    "<SimulationMode>"
    " <SimulationParameters>"
    "  <Timeline>"
    "  <StartTime>2011-01-01T12:00:00</StartTime>"
    "  <StepTime unit=\"s\">60.0</StepTime>"
    "  </Timeline>"
    "  <InitialStatePosition>"
    "  <CoordinateSystem>INERTIAL J2000</CoordinateSystem>"
    "  <SphericalCoordinates>"
    "    <Altitude>120</Altitude>"
    "    <Longitude>20</Longitude>"
    "    <Latitude>30</Latitude>"
    "    <InertialVelocity>12</InertialVelocity>"
    "    <InertialFlightPathAngle>-8</InertialFlightPathAngle>"
    "    <InertialHeading>90</InertialHeading>"
    "  </SphericalCoordinates>"
    "   <CentralsystemBody Name=\"EARTH\"></CentralsystemBody>"
    "  </InitialStatePosition>"
    " </SimulationParameters>"
    "</SimulationMode>"
    "<TrajectoryPropagation>"
    "  <IntegratorType>RK4</IntegratorType>"
    "  <Timestep unit=\"s\">60.0</Timestep>"
    "  <PropagatorType>TWO BODY</PropagatorType>"
    "</TrajectoryPropagation>"
    "<AttitudePropagation>"
    "  <IntegratorType>RK4</IntegratorType>"
    "  <Timestep unit=\"s\">60.0</Timestep>"
    "  <ExternCouples>NONE</ExternCouples>"
    "</AttitudePropagation>"
    "</ReEntry>"
    );

    return xmlTemplate.arg(QString(name)).toUtf8();
}


static QByteArray impulseFragment(const char* name)
{
    QByteArray data;

    QString xmlTemplate = QString(
    "<?xml version=\"1.0\" encoding=\"UTF-8\"?>"
    "<Impulse Name=\"%1\">"
    "<Environment>"
    "<CentralBody Name=\"Earth\">"
    "<GravityModel>"
    "<ModelName>ModelName</ModelName>"
    "<NumberOfZonals>0</NumberOfZonals>"
    "<NumberOfTesserals>0</NumberOfTesserals>"
    "</GravityModel>"
    "<AtmosphereModel>AtmosphereModel</AtmosphereModel>"
    "</CentralBody>"
    "<AtmosphericDrag>0</AtmosphericDrag>"
    "<SolarPressure>0</SolarPressure>"
    "</Environment>"
    "<SimulationParameters>"
    "<Timeline>"
    "<StartTime>2011-00-00T00:00:00</StartTime>"
    "<StepTime units=\"s\">60</StepTime>"
    "</Timeline>"
    "<InitialStatePosition>"
    "<CoordinateSystem>INERTIAL J2000</CoordinateSystem>"
    "<StateVector>"
    "<x>0</x>"
    "<y>0</y>"
    "<z>0</z>"
    "<vx>0</vx>"
    "<vy>0</vy>"
    "<vz>0</vz>"
    "</StateVector>"
    "</InitialStatePosition>"
    "</SimulationParameters>"
    "<Point></Point>"
    "<ManoeuvrePlan></ManoeuvrePlan>"
    "</Rendezvous>"
    );

    return xmlTemplate.arg(QString(name)).toUtf8();
}


static QByteArray rendezvousFragment(const char* name)
{
    QByteArray data;

    QString xmlTemplate = QString(
    "<?xml version=\"1.0\" encoding=\"UTF-8\"?>"
    "<Rendezvous Name=\"%1\">"
    "<Environment>"
    "<CentralBody Name=\"Earth\">"
    "<GravityModel>"
    "<ModelName>ModelName</ModelName>"
    "<NumberOfZonals>0</NumberOfZonals>"
    "<NumberOfTesserals>0</NumberOfTesserals>"
    "</GravityModel>"
    "<AtmosphereModel>AtmosphereModel</AtmosphereModel>"
    "</CentralBody>"
    "<AtmosphericDrag>0</AtmosphericDrag>"
    "<SolarPressure>0</SolarPressure>"
    "</Environment>"
    "<SimulationParameters>"
    "<Timeline>"
    "<StartTime>2011-00-00T00:00:00</StartTime>"
    "<StepTime units=\"s\">60</StepTime>"
    "</Timeline>"
    "<InitialStatePosition>"
    "<CoordinateSystem>INERTIAL J2000</CoordinateSystem>"
    "<StateVector>"
    "<x>0</x>"
    "<y>0</y>"
    "<z>0</z>"
    "<vx>0</vx>"
    "<vy>0</vy>"
    "<vz>0</vz>"
    "</StateVector>"
    "</InitialStatePosition>"
    "</SimulationParameters>"
    "<Target></Target>"
    "<ManoeuvrePlan></ManoeuvrePlan>"
    "</Rendezvous>"
    );

    return xmlTemplate.arg(QString(name)).toUtf8();
}
#endif

