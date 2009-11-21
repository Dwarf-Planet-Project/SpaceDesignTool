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


#include <QList>
#include <QMap>
#include <QString>
#include <QXmlStreamReader>
#include <Astro-Core/stabody.h>
#include <Astro-Core/vector.h>

//class QXmlStreamReader;
class QXmlStreamWriter;

class QTreeWidgetItem;
class ScenarioTreeItemFactory;

// ScenarioObject is the base class for all objects that may appear in an STA scenario

class SpaceScenario;

class ScenarioObject
{
public:
    ScenarioObject() : m_parent(0) {}
    virtual ~ScenarioObject();
    
    ScenarioObject* parent() const
    {
        return m_parent;
    }
    
    SpaceScenario* scenario() const
    {
        return m_scenario;
    }
    
    bool isAttached() const
    {
        return m_parent != 0;
    }

    virtual bool read(QXmlStreamReader& /* in */) { return true; }
    virtual bool write(QXmlStreamWriter& /* out */) { return true; }
    
    QTreeWidgetItem* createItem(QTreeWidgetItem* parent);
    virtual QTreeWidgetItem* createItemSelf(QTreeWidgetItem* parent);
    virtual void createItemContents(QTreeWidgetItem* item);
    
    virtual QString displayName() const { return ""; }
    
protected:
    void attach(ScenarioObject* parent);
    void detach();
    void attachChild(ScenarioObject* child);
    void detachChild(ScenarioObject* child);
    
private:
    ScenarioObject* m_parent;
    SpaceScenario* m_scenario;
};


enum { ScenarioObjectRole = Qt::UserRole + 100 };


class ScenarioParticipant;

class SpaceScenario : public ScenarioObject
{
public:
    SpaceScenario() : ScenarioObject() {}
    ~SpaceScenario();
    
    QString name() const { return m_name; }
    void setName(const QString& name) { m_name = name; }
    
    QString displayName() const { return QObject::tr("Scenario"); }

    void addParticipant(ScenarioParticipant* participant);
    
    virtual QTreeWidgetItem* createItemSelf(QTreeWidgetItem* parent);
    virtual void createItemContents(QTreeWidgetItem* item);
    
private:
    QList<ScenarioParticipant*> m_participantList;
    QMap<QTreeWidgetItem*, ScenarioObject>* m_itemMap;
    QString m_name;
};


// ABSTRACT
class ScenarioParticipant : public ScenarioObject
{
public:
    ScenarioParticipant() {}
    virtual ~ScenarioParticipant() {};
    
    void setName(const QString& name)
    {
        m_name = name;
    }
    
    QString name() const { return m_name; }
        
private:
    QString m_name;    
};


class ScenarioLocation;

// ABSTRACT
class ScenarioGroundElement : public ScenarioParticipant
{
public:
    ScenarioGroundElement();
    virtual ~ScenarioGroundElement();
    
    virtual void createItemContents(QTreeWidgetItem* item);

    ScenarioLocation* location() const { return m_location; }
    void setLocation(ScenarioLocation* location);
    
private:
    ScenarioLocation* m_location;
};


class ScenarioTrajectoryPlan;
class ScenarioAdditionalComponents;

// ABSTRACT
class ScenarioSpaceVehicle : public ScenarioParticipant
{
public:
    ScenarioSpaceVehicle();
    virtual ~ScenarioSpaceVehicle();
    
    virtual void createItemContents(QTreeWidgetItem* item);

    ScenarioTrajectoryPlan* trajectoryPlan() const { return m_trajectoryPlan; }
    void setTrajectoryPlan(ScenarioTrajectoryPlan* trajectoryPlan);
    ScenarioAdditionalComponents* additionalComponents() const { return m_additionalComponents; }
    void setAdditionalComponents(ScenarioAdditionalComponents* additionalComponents);
    
private:
    ScenarioTrajectoryPlan* m_trajectoryPlan;
    ScenarioAdditionalComponents* m_additionalComponents;
};



class ScenarioGroundStation : public ScenarioGroundElement
{
public:
    ScenarioGroundStation() {}
    virtual ~ScenarioGroundStation() {}
    
    QString displayName() const { return QObject::tr("Ground Station"); }
};


class ScenarioLaunchPad : public ScenarioGroundElement
{
public:
    ScenarioLaunchPad() : m_clearingAltitude(0.0) {}
    virtual ~ScenarioLaunchPad() {}

    QString displayName() const { return QObject::tr("Launch Pad"); }

    virtual void createItemContents(QTreeWidgetItem* item);
    
    double clearingAltitude() const { return m_clearingAltitude; }
    void setClearingAltitude(double clearingAltitude) { m_clearingAltitude = clearingAltitude; }

private:
    double m_clearingAltitude;
};


class ScenarioTarget : public ScenarioGroundElement
{
public:
    ScenarioTarget() : m_clearingAltitude(0.0) {}
    virtual ~ScenarioTarget() {}
    
    QString displayName() const { return QObject::tr("Target"); }

    // Next line patched by Guillermo to make the target appear with different icon on the scenario view
    virtual void createItemContents(QTreeWidgetItem* item);

    double clearingAltitude() const { return m_clearingAltitude; }
    void setClearingAltitude(double clearingAltitude) { m_clearingAltitude = clearingAltitude; }

private:
    double m_clearingAltitude;


};


class ScenarioProperties;

class ScenarioSatellite : public ScenarioSpaceVehicle
{
public:
    ScenarioSatellite();
    virtual ~ScenarioSatellite();
    
    QString displayName() const { return QObject::tr("Satellite"); }
    
    virtual void createItemContents(QTreeWidgetItem* item);
    
    ScenarioProperties* properties() const { return m_properties; }
    void setProperties(ScenarioProperties* properties);
    
private:
    ScenarioProperties* m_properties;
};


class ScenarioReentryVehicle : public ScenarioSpaceVehicle
{
public:
    ScenarioReentryVehicle() {}
    ~ScenarioReentryVehicle() {}
    
    QString displayName() const { return QObject::tr("Reentry Vehicle"); }
};


class ScenarioLauncher : public ScenarioSpaceVehicle
{
public:
    ScenarioLauncher() {}
    ~ScenarioLauncher() {}
    
    QString displayName() const { return QObject::tr("Launcher"); }
};


struct GroundPosition
{
public:
    GroundPosition() :
        latitude(0.0), longitude(0.0), altitude(0.0) 
    {
    }
    
    GroundPosition(double _latitude, double _longitude, double _altitude = 0.0) :
        latitude(_latitude), longitude(_longitude), altitude(_altitude)
    {
    }
    
public:
    double latitude;
    double longitude;
    double altitude;
};


class ScenarioLocation : public ScenarioObject
{
public:
    ScenarioLocation();
    ~ScenarioLocation();
    
    virtual void createItemContents(QTreeWidgetItem* item);
    
    const StaBody* centralBody() const { return m_centralBody; }
    void setCentralBody(const StaBody* centralBody) { m_centralBody = centralBody; }
    GroundPosition getGroundPosition() const { return m_groundPosition; }
    void setGroundPosition(const GroundPosition& groundPosition) { m_groundPosition = groundPosition; }

    QString displayName() const { return QObject::tr("Location"); }
    
private:
    const StaBody* m_centralBody;
    GroundPosition m_groundPosition;
};


class ScenarioTrajectory;

class ScenarioTrajectoryPlan : public ScenarioObject
{
public:
    ScenarioTrajectoryPlan() {}
    ~ScenarioTrajectoryPlan();
    
    QString displayName() const { return QObject::tr("Trajectory Plan"); }
    virtual void createItemContents(QTreeWidgetItem* item);
    
    void addTrajectory(ScenarioTrajectory* participant);
        
private:
    QList<ScenarioTrajectory*> m_trajectoryList;
};


class ScenarioEnvironment;
class ScenarioSimulationParameters;
class ScenarioOptimizationParameters;
class ScenarioTrajectoryPropagation;

class ScenarioTrajectory : public ScenarioObject
{
public:
    ScenarioTrajectory();
    ~ScenarioTrajectory() {}
    
    QString displayName() const { return QObject::tr("Trajectory"); }
    virtual void createItemContents(QTreeWidgetItem* item);
    
    ScenarioEnvironment* environment() const { return m_environment; }
    void setEnvironment(ScenarioEnvironment* environment);
    ScenarioSimulationParameters* simulationParameters() const { return m_simulationParameters; }
    void setSimulationParameters(ScenarioSimulationParameters* simulationParameters);
    ScenarioTrajectoryPropagation* trajectoryPropagation() const { return m_trajectoryPropagation; }
    void setTrajectoryPropagation(ScenarioTrajectoryPropagation* trajectoryPropagation);
    
private:
    ScenarioEnvironment* m_environment;
    
    // Only one of the following should be non-NULL
    ScenarioSimulationParameters* m_simulationParameters;
    ScenarioOptimizationParameters* m_optimizationParameters;
    
    ScenarioTrajectoryPropagation* m_trajectoryPropagation;
    // TODO: ScenarioAttitudePropagation* m_attitudePropagation;
    // TODO: ScenarioManeuver* m_maneuver;
};


class ScenarioEnvironment : public ScenarioObject
{
public:
    ScenarioEnvironment();
    virtual ~ScenarioEnvironment() {}
    
    QString displayName() const { return QObject::tr("Environment"); }
    virtual void createItemContents(QTreeWidgetItem* item);
    
    const StaBody* centralBody() const { return m_centralBody; }
    void setCentralBody(const StaBody* centralBody) { m_centralBody = centralBody; }
    
private:
    const StaBody* m_centralBody;
    QList<const StaBody*> m_perturbingBodyList;
};


class ScenarioTimeline;
class ScenarioInitialStatePosition;
class ScenarioInitialStateAttitude;

class ScenarioSimulationParameters : public ScenarioObject
{
public:
    ScenarioSimulationParameters();
    virtual ~ScenarioSimulationParameters();
    
    QString displayName() const { return QObject::tr("Simulation Parameters"); }
    virtual void createItemContents(QTreeWidgetItem* item);
    
    ScenarioTimeline* timeline() const { return m_timeline; }
    void setTimeline(ScenarioTimeline* timeline);
    ScenarioInitialStatePosition* initialStatePosition() const { return m_initialStatePosition; }
    void setInitialStatePosition(ScenarioInitialStatePosition* initialStatePosition);
    
private:
    ScenarioTimeline* m_timeline;
    ScenarioInitialStatePosition* m_initialStatePosition;
    ScenarioInitialStateAttitude* m_initialStateAttitude;
};


class ScenarioTimeline : public ScenarioObject
{
public:
    ScenarioTimeline();
    virtual ~ScenarioTimeline() {}

    QString displayName() const { return QObject::tr("Timeline"); }

private:
    double m_startTime;
    double m_endTime;
    double m_timeStep;
};


// Abstract
class ScenarioAbstractInitialState : public ScenarioObject
{
};


enum ScenarioCoordinateSystem
{
    SCENARIO_BODYFIXED                 =  1,
    SCENARIO_EME_J2000                 =  2,
    SCENARIO_EME_B1950                 =  3,
    SCENARIO_ECLIPTIC_J2000            =  4,
    SCENARIO_INVALID_COORDINATE_SYSTEM = -1,
};


QString CoordinateSystemToString(ScenarioCoordinateSystem coordSys);
ScenarioCoordinateSystem StringToCoordinateSystem(QString coordSysName);

class ScenarioInitialStatePosition : public ScenarioObject
{
public:
    ScenarioInitialStatePosition();

    QString displayName() const { return QObject::tr("Initial State"); }
    virtual void createItemContents(QTreeWidgetItem* item);

    ScenarioCoordinateSystem coordinateSystem() const { return m_coordinateSystem; }
    void setCoordinateSystem(ScenarioCoordinateSystem coordinateSystem) { m_coordinateSystem = coordinateSystem; }
    ScenarioAbstractInitialState* initialState() const { return m_initialState; }
    void setInitialState(ScenarioAbstractInitialState* initialState);
    
private:
    ScenarioCoordinateSystem m_coordinateSystem;
    ScenarioAbstractInitialState* m_initialState;
};


class ScenarioStateVector : public ScenarioAbstractInitialState
{
public:
    QString displayName() const { return QObject::tr("State Vector"); }
    
public:
    sta::Vector3 m_position;
    sta::Vector3 m_velocity;
};


class ScenarioKeplerianElements : public ScenarioAbstractInitialState
{
public:
    ScenarioKeplerianElements();
    
    QString displayName() const { return QObject::tr("Keplerian Elements"); }
    virtual void createItemContents(QTreeWidgetItem* item);
    
public:
    double m_semimajorAxis;
    double m_eccentricity;
    double m_inclination;
    double m_raan;
    double m_argumentOfPeriapsis;
    double m_trueAnomaly;
};


class ScenarioInitialStateAttitude : public ScenarioObject
{
};


class ScenarioAdditionalComponents : public ScenarioObject
{
public:
    QString displayName() const { return QObject::tr("Additional Components"); }
};


class ScenarioProperties : public ScenarioObject
{
public:
    QString displayName() const { return QObject::tr("Properties"); }
};


class ScenarioTrajectoryPropagation : public ScenarioObject
{
public:
    ScenarioTrajectoryPropagation();
    virtual ~ScenarioTrajectoryPropagation() {};

    QString displayName() const { return QObject::tr("Trajectory Propagation"); }
    virtual void createItemContents(QTreeWidgetItem* item);
    
private:
    QString m_integrator;  // TODO: use an enum
    double m_timeStep;
    QString m_propagator;  // TODO: use an enum
};


/*** SpaceScenarioReader ***/

class SpaceScenarioReader : public QXmlStreamReader
{
public:
    SpaceScenarioReader(QIODevice* device) :
        QXmlStreamReader(device)
    {
    }
    
    SpaceScenario* readSpaceScenario();
    SpaceScenario* readParticipantsList();
    
    // Participants
    ScenarioParticipant* readParticipant();
    ScenarioSatellite* readSatellite();
    ScenarioGroundStation* readGroundStation();
    ScenarioLaunchPad* readLaunchPad();
    ScenarioTarget* readTarget();
    ScenarioLauncher* readLauncher();
    ScenarioReentryVehicle* readReentryVehicle();
    
    ScenarioTrajectoryPlan* readTrajectoryPlan();
    ScenarioTrajectory* readTrajectory();
    ScenarioAdditionalComponents* readAdditionalComponents();
    ScenarioProperties* readProperties();
    
    ScenarioLocation* readLocation();
    ScenarioEnvironment* readEnvironment();
    ScenarioSimulationParameters* readSimulationParameters();
    ScenarioTrajectoryPropagation* readTrajectoryPropagation();
    ScenarioInitialStatePosition* readInitialStatePosition();
    ScenarioAbstractInitialState* readInitialStateType();
    ScenarioStateVector* readStateVector();
    ScenarioKeplerianElements* readKeplerianElements();
    
    void readGroundElement(ScenarioGroundElement* groundElement);
    void readSpaceVehicle(ScenarioSpaceVehicle* spaceVehicle);
    
    StaBody* readBodyElement();
    
    void skipUnknownElement();
    void nextElement();
    bool readDouble(double* value);
    bool readDoubleElement(const QString& elementName, double* value);
    StaBody* readBody();
    bool expectedElement(const QString& elementName);
};


