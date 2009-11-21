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
  Modified by Tiziana Sabatini May 2009
  Modified by Guillermo June 3rd 2009
  Modified by Valentino Zuccarelli June 14th 2009
  Extensvely modified by Guillermo on October 2009 to include TLEs
 */

#include <QPushButton>
#include <QHeaderView>
#include <QMimeData>
#include <QTextStream>
#include <QtDebug>
#include "scenarioelementbox.h"
#include "sheetDelegate.h"

static const QString SCENARIO_MIME_TYPE("application/sta.scenario.xml");

const QString ScenarioElementBox::PARTICIPANT_MIME_TYPE("application/sta.participant.xml");
const QString ScenarioElementBox::MISSION_ARC_MIME_TYPE("application/sta.missionarc.xml");
const QString ScenarioElementBox::MANEUVER_MIME_TYPE("application/sta.maneuver.xml");

static const int ScenarioFragmentRole     = Qt::UserRole + 1;
static const int ScenarioFragmentTypeRole = Qt::UserRole + 2;

ScenarioElementWidget::ScenarioElementWidget(QWidget* parent) :
    QTreeWidget(parent)
{
    setHeaderHidden(true);
    setRootIsDecorated(false);
    //setIconSize(QSize(32, 32));
    setIconSize(QSize(30, 30)); // Smaller icons are a bit better
    //QFont font("Helvetica", 10); setFont(font);
    setItemDelegate(new SheetDelegate(this));

    setDragDropMode(DragOnly);
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
                    mimeType = itemMimeType;

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


// Create a scenario fragment representing a single space vehicle.
static QByteArray spaceVehicleFragment(const char* name, const char* vehicleType)
{
    QByteArray encodedData;

    encodedData +=
    "<?xml version=\"1.0\" encoding=\"UTF-8\"?>"
    "<SpaceVehicle Name=\"";
    encodedData += name;
    encodedData +="\" Type=\"";
    encodedData += vehicleType;
    encodedData +="\">"
    "  <Properties>"
    "    <PhysicalProperties>"
    "      <PhysicalCharacteristics>"
    "        <volume unit=\"m^3\">1.0</volume>"
    "        <mass unit=\"Kg\">100.0</mass>"
    "        <surface unit=\"m^2\">1.0</surface>"
    "      </PhysicalCharacteristics>"
    "      <GeometricalCharacteristics>"
    "        <radiusBase unit=\"m\">0.5</radiusBase>"
    "        <radiusNose unit=\"m\">0.33</radiusNose>"
    "      </GeometricalCharacteristics>"
    "    </PhysicalProperties>"
    "    <AerodynamicProperties>"
    "      <CDCoefficients>CdM.stad</CDCoefficients>"
    "      <GLoadLimit unit=\"-\">40</GLoadLimit>"
    "      <SPHeatRateLimit unit=\"MJ/m^2\">1200</SPHeatRateLimit>"
    "      <ParachuteProperties>"
    "        <SurfaceArea unit=\"m2\">0</SurfaceArea>"
    "        <CDCoefficients>CdMp.stad</CDCoefficients>"
    "        <DeploymentMach unit=\"-\">0</DeploymentMach>"
    "      </ParachuteProperties>"
    "    </AerodynamicProperties>"
    "    <PropulsionProperties>"
    "    </PropulsionProperties>"
    "    <PayloadProperties>"
    "    </PayloadProperties>"
    "  </Properties>"
    "  <TrajectoryPlan>"
    "  </TrajectoryPlan>"
    "</SpaceVehicle>";

    return encodedData;
}


static QByteArray loiteringFragment(const char* name)
{
    QByteArray data;

    data += "<?xml version=\"1.0\" encoding=\"UTF-8\"?>";

    data += "<Loitering Name=\"";
    data += name;
    data += "\">";

    data +=
    "<Environment>"
    "<CentralBody Name=\"EARTH\"></CentralBody>"
    "</Environment>"
    "<SimulationParameters>"
    "  <Timeline>"
    "  <StartTime>2011-01-01T12:00:00</StartTime>"
    "  <StepTime unit=\"s\">60.0</StepTime>"
    "  <EndTime>2011-01-02T12:00:00</EndTime>"
    "  </Timeline>"
    "  <InitialStatePosition>"
    "  <CoordinateSystem>INERTIAL J2000</CoordinateSystem>"
    "  <KeplerianElements>"
    "    <SemiMajorAxis>10000</SemiMajorAxis>"
    "    <Eccentricity>0</Eccentricity>"
    "    <Inclination>45</Inclination>"
    "    <RAAN>0</RAAN>"
    "    <ArgumentOfPeriapsis>0</ArgumentOfPeriapsis>"
    "    <TrueAnomaly>0</TrueAnomaly>"
    "  </KeplerianElements>"
    "   <CentralsystemBody Name=\"EARTH\"></CentralsystemBody>"
    "  </InitialStatePosition>"
    "  <InitialStateAttitude></InitialStateAttitude>"
    "</SimulationParameters>"
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
    "</Loitering>";

    return data;
}




static QByteArray loiteringTLEFragment(const char* name)
{
    QByteArray data;

    data += "<?xml version=\"1.0\" encoding=\"UTF-8\"?>";

    data += "<LoiteringTLE Name=\"";
    data += name;
    data += "\">";

    data +=
    "<Timeline>"
    "  <StartTime>2009-10-11T12:00:00</StartTime>"
    "  <StepTime unit=\"s\">60.0</StepTime>"
    "  <EndTime>2009-10-12T12:00:00</EndTime>"
    "</Timeline>"
    "<Line0>ISS (ZARYA)</Line0>"
    "<Line1>1 25544U 98067A   09282.91732946  .00013034  00000-0  90850-4 0  7559</Line1>"
    "<Line2>2 25544  51.6398  77.7469 0007731 180.0753 317.5762 15.74717336624025</Line2>"
    "</LoiteringTLE>";

    return data;
}




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


// Set the mime type and data for drag and drop
static void setDragAndDropInfo(QTreeWidgetItem* item,
                               const QString& mimeType,
                               const QByteArray& mimeData)
{
    item->setData(0, ScenarioFragmentTypeRole, mimeType);
    item->setData(0, ScenarioFragmentRole, mimeData);
}


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
    groundStationItem->setIcon(0, QIcon(":/icons/participantGroundStation.png"));
    setDragAndDropInfo(groundStationItem,
                       ScenarioElementBox::PARTICIPANT_MIME_TYPE,
                       groundElementFragment(name, "GroundStation", centralBody,
                                             latitude, longitude, altitude));
}



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
    QTreeWidgetItem* maneuversItem = new QTreeWidgetItem();
    maneuversItem->setText(0, tr("Maneuvers"));
    QTreeWidgetItem* groundStationsItem = new QTreeWidgetItem();
    groundStationsItem->setText(0, tr("Ground Stations"));

    m_elementTreeWidget->clear();
    m_elementTreeWidget->addTopLevelItem(participantsItem);
    m_elementTreeWidget->addTopLevelItem(missionArcsItem);
    m_elementTreeWidget->addTopLevelItem(maneuversItem);
    m_elementTreeWidget->addTopLevelItem(groundStationsItem);


    /////////// Creating the widgets
    QTreeWidgetItem* satelliteItem       = new QTreeWidgetItem(participantsItem);
    satelliteItem->setText(0, tr("Satellite"));
    satelliteItem->setIcon(0, QIcon(":/icons/participantSatellite.png"));
    setDragAndDropInfo(satelliteItem,
                       PARTICIPANT_MIME_TYPE,
                       spaceVehicleFragment("New Satellite", "Satellite"));

    QTreeWidgetItem* targetItem          = new QTreeWidgetItem(participantsItem);
    targetItem->setText(0, tr("Point"));
    targetItem->setIcon(0, QIcon(":/icons/participantTarget.png"));
    setDragAndDropInfo(targetItem,
                       PARTICIPANT_MIME_TYPE,
		       groundElementFragment("New Point", "Point", "Earth"));

    // Diabling the complete function for the time being. Patched by Guillermo
    //QTreeWidgetItem* launchPadItem       = new QTreeWidgetItem(participantsItem);
    //launchPadItem->setText(0, tr("Launch Pad"));
    //launchPadItem->setIcon(0, QIcon(":/icons/participantLaunchPad.png"));
    //setDragAndDropInfo(launchPadItem,
    //                   PARTICIPANT_MIME_TYPE,
    //                   groundElementFragment("New Launch Pad", "LaunchPad", "Earth"));

    // Diabling the complete function for the time being. Patched by Guillermo
    //QTreeWidgetItem* launcherItem        = new QTreeWidgetItem(participantsItem);
    //launcherItem->setText(0, tr("Launcher"));
    //launcherItem->setIcon(0, QIcon(":/icons/participantRocket.png"));
    //setDragAndDropInfo(launcherItem,
    //                   PARTICIPANT_MIME_TYPE,
    //                   spaceVehicleFragment("New Launcher", "Launcher"));

    // Diabling the complete function for the time being. Patched by Guillermo
    //QTreeWidgetItem* reentryVehicleItem  = new QTreeWidgetItem(participantsItem);
    //reentryVehicleItem->setText(0, tr("Reentry vehicle"));
    //reentryVehicleItem->setIcon(0, QIcon(":/icons/participantEntryVehicle.png"));
    //setDragAndDropInfo(reentryVehicleItem,
    //                   PARTICIPANT_MIME_TYPE,
    //                   spaceVehicleFragment("New Reentry Vehicle", "Reentry Vehicle"));

    QTreeWidgetItem* groundStationItem   = new QTreeWidgetItem(participantsItem);
    groundStationItem->setText(0, tr("Station"));
    groundStationItem->setIcon(0, QIcon(":/icons/participantGroundStation.png"));
    setDragAndDropInfo(groundStationItem,
                       PARTICIPANT_MIME_TYPE,
                       groundElementFragment("New Ground Station", "GroundStation", "Earth"));

    // Diabling the complete function for the time being. Patched by Guillermo
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

    QTreeWidgetItem* impulseItem      = new QTreeWidgetItem(maneuversItem);
    impulseItem->setText(0, tr("Single impulse"));
    impulseItem->setIcon(0, QIcon(":/icons/engine.png"));
    setDragAndDropInfo(impulseItem,
		       MISSION_ARC_MIME_TYPE,
		       impulseFragment("Single-impulse 1"));

    // Diabling the complete function for the time being. Patched by Guillermo
    //QTreeWidgetItem* rendezvousItem   = new QTreeWidgetItem(maneuversItem);
    //rendezvousItem->setText(0, tr("Rendezvous plan"));
    //rendezvousItem->setIcon(0, QIcon(":/icons/mission-arcs-rendezvous.png"));
    //setDragAndDropInfo(rendezvousItem,
    //                   MISSION_ARC_MIME_TYPE,
    //                   rendezvousFragment("Rendezvous 1"));

    // The ESTRACK ground stations
    addGroundStationItem(groundStationsItem, "Cebreros",     "Earth",  40.45268986,  -4.36754881, 794.1);
    addGroundStationItem(groundStationsItem, "Kiruna",      "Earth",  67.8571252,   20.96434169, 402.275);
    addGroundStationItem(groundStationsItem, "Kourou",      "Earth",   5.25143694, -52.8046624,   14.561);
    addGroundStationItem(groundStationsItem, "Malargue",    "Earth",  -35.775972,     -69.398250,  1525.20);
    addGroundStationItem(groundStationsItem, "Malindi",     "Earth",  -2.9955576,   40.194505,    12.314);
    addGroundStationItem(groundStationsItem, "Maspalomas",  "Earth",  27.762892,   -15.6338072,  204.9);
    addGroundStationItem(groundStationsItem, "New Norcia",  "Earth", -31.048225,   116.191500,   252.26);
    addGroundStationItem(groundStationsItem, "Perth",       "Earth", -31.802525,   115.8851556,   22.156);
    addGroundStationItem(groundStationsItem, "Redu",        "Earth",  50.0020552,    5.146231,   385.583);
    addGroundStationItem(groundStationsItem, "Santa Maria",  "Earth",  36.99694,    -25.1361027,   276.00);
    addGroundStationItem(groundStationsItem, "Santiago",     "Earth",  -33.151478,    -70.668306,   730.853);
    addGroundStationItem(groundStationsItem, "Svalbard",     "Earth",   78.229772,    15.407786,   501.3);
    //addGroundStationItem(groundStationsItem, "Villafranca", "Earth",  40.4455932,   -3.95260078, 664.425);


    // Set properties for all top level items: expanded = true
    for (int i = 0; i < m_elementTreeWidget->topLevelItemCount(); i++)
    {
        QTreeWidgetItem* item = m_elementTreeWidget->topLevelItem(i);
        m_elementTreeWidget->setItemExpanded(item, true);
        item->setExpanded(true);
    }

    setLayout(layout);
#endif
}


ScenarioElementBox::~ScenarioElementBox()
{
}

