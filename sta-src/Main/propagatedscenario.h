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
 */

#ifndef _STA_PROPAGATED_SCENARIO_H_
#define _STA_PROPAGATED_SCENARIO_H_

#include <QColor>

#include <Astro-Core/statevector.h>
#include <Astro-Core/stacoordsys.h>
#include <Astro-Core/stabody.h>
#include <Astro-Core/date.h>
#include <Astro-Core/getGreenwichHourAngle.h>
#include <Astro-Core/inertialTOfixed.h>
#include <Astro-Core/constants.h>
#include <Astro-Core/attitudevector.h>

class ConstellationStudy;
class QTemporaryFile;

class ScenarioParticipantType;


/*! A mission arc is a trajectory in a propagated scenario.
 */
class MissionArc
{
public:
    MissionArc(const StaBody* centralBody,
               sta::CoordinateSystem coordSys,
               QList<double> sampleTimes,
               QList<sta::StateVector> samples);
    MissionArc(const StaBody* centralBody,
               sta::CoordinateSystem coordSys,
               QList<double> sampleTimes,
               QList<sta::StateVector> samples,
               QList<double> attitudeSampleTimes,
               QList<staAttitude::AttitudeVector> attitudeSamples);
    ~MissionArc();

	QString arcName() const { return m_ArcName; }
	void setArcName(const QString& name) { m_ArcName = name; }
	QString modelName() const { return m_ArcModelName; }
	void setModelName(const QString& modelFile) { m_ArcModelName = modelFile; }
	QColor arcTrajectoryColor() const { return m_ArcTrajectoryColor; }
	void setArcTrajectoryColor(QColor color) { m_ArcTrajectoryColor = color; }
    
    const StaBody* centralBody() const { return m_centralBody; }
    sta::CoordinateSystem coordinateSystem() const { return m_coordSys; }
    QString ephemerisFile() const { return m_ephemerisFile; }
    double beginning() const { return m_beginning; }
    double ending() const { return m_ending; }
    QList<double> sampleTimes() { return m_sampleTimes; }

    bool getStateVector(double mjd, sta::StateVector* result) const;
    int trajectorySampleCount() const;
    sta::StateVector trajectorySample(int index) const;
    double trajectorySampleTime(int index) const;

    int attitudeSampleCount() const;
    staAttitude::AttitudeVector attitudeSample(int index) const;
    double attitudeSampleTime(int index) const;
    bool getAttitude(double mjd, staAttitude::AttitudeVector* result) const;

    bool generateEphemerisFile();

    /** Return true if this arc contains attitude information.
      */
    bool hasAttitude() const
    {
        return !m_attitudeSampleTimes.isEmpty();
    }

private:
    const StaBody* m_centralBody;
    sta::CoordinateSystem m_coordSys;
    double m_beginning;
    double m_ending;

    QList<double> m_sampleTimes;
    QList<sta::StateVector> m_samples;
    QList<double> m_attitudeSampleTimes;
    QList<staAttitude::AttitudeVector> m_attitudeSamples;

    QString m_ephemerisFile;
    QTemporaryFile* m_ephemerisTempFile;
    
    // Cached value of last sample accessed when calling getStateVector
    mutable int m_lastSample; 

	QString m_ArcName;
	QString m_ArcModelName;
	QColor m_ArcTrajectoryColor;
};

// Claas and Steffen: Structure that represents Transmitter or Receiver Data
// this class is necessary to consider Antennas during studying constellations

class PSAntennaObject
{
public:
    PSAntennaObject();
    ~PSAntennaObject();

    double getAzimuth();
    double getElevation();
    int getConeShape();
    double getConeAngle();
    bool getObservationChecked();

    void setAzimuth(double az);
    void setElevation(double el);
    void setConeShape(int cs);
    void setConeAngle(double ca);
    void setObservationChecked(bool oc);

private:
    double m_azimuth;
    double m_elevation;
    int m_coneShape;
    double m_coneAngle;
    bool m_observationChecked;
};

// A SpaceObject instance represents an object that has
// an integrated trajectory through space.

class SpaceObject
{
public:
    SpaceObject(ScenarioParticipantType* participant);
    ~SpaceObject();

    double missionStartTime() const { return m_missionStartTime; }
    double missionEndTime() const { return m_missionEndTime; }
    const QList<MissionArc*>& mission() const { return m_missionArcs; }
    const QList<PSAntennaObject*>& receiver() const{ return m_receiver; }
    const QList<PSAntennaObject*>& transmitter() const{ return m_transmitter; }

    QString name() const { return m_name; }
    void setName(const QString& name) { m_name = name; }
    QString modelFile() const { return m_modelFile; }
    void setModelFile(const QString& modelFile) { m_modelFile = modelFile; }
    QColor trajectoryColor() const { return m_trajectoryColor; }
    void setTrajectoryColor(QColor color) { m_trajectoryColor = color; }
    
    void addMissionArc(MissionArc* arc);
    void addReceiver(PSAntennaObject* rec);
    void addTransmitter(PSAntennaObject* tra);

    bool getStateVector(double mjd,
                        const StaBody& center,
                        const sta::CoordinateSystem& coordSys,
                        sta::StateVector* result) const;
    
    bool generateEphemerisFiles();

    ScenarioParticipantType* participant() const
    {
        return m_participant;
    }
    
private:
    double m_missionStartTime;
    double m_missionEndTime;
    QList<MissionArc*> m_missionArcs;
    QList<PSAntennaObject*> m_receiver;
    QList<PSAntennaObject*> m_transmitter;
    
    QString m_name;
    QString m_modelFile;
    QColor m_trajectoryColor;

    ScenarioParticipantType* m_participant;
};


// A GroundObject instance represents an object that is 
// fixed to the surface of a planet or moon.
class GroundObject
{
public:
    GroundObject();
    ~GroundObject();

    double elevationAngle(const SpaceObject* spacecraft, double t) const;
    double azimuthAngle(const SpaceObject* spacecraft, double t) const;
    double getRange(const SpaceObject* spacecraft, double t) const;

    const QList<PSAntennaObject*>& receiver() const{ return m_receiver; }
    const QList<PSAntennaObject*>& transmitter() const{ return m_transmitter; }

    void addReceiver(PSAntennaObject* rec);
    void addTransmitter(PSAntennaObject* tra);

    QString name;
    const StaBody* centralBody;
    double longitude;  // degrees
    double latitude;   // degrees
    double altitude;   // meters

private:
    QList<PSAntennaObject*> m_receiver;
    QList<PSAntennaObject*> m_transmitter;
};


/** RegionObject represents a region from a scenario after propagation.
  */
class RegionObject
{
public:
    RegionObject(const QString& name, const StaBody* centralBody);
    ~RegionObject();

    QString name() const
    {
        return m_name;
    }

    const StaBody* centralBody() const
    {
        return m_centralBody;
    }

    const QList<Eigen::Vector2d>& boundary() const
    {
        return m_boundary;
    }

    QColor color() const
    {
        return m_color;
    }

    void setBoundary(const QList<Eigen::Vector2d>& boundary);
    void setColor(const QColor& color);

private:
    QString m_name;
    const StaBody* m_centralBody;
    QList<Eigen::Vector2d> m_boundary;
    QColor m_color;
};


class PropagatedScenario
{
public:
    PropagatedScenario();
    ~PropagatedScenario();
    
    const QList<SpaceObject*>& spaceObjects() const { return m_spaceObjectList; }
    const QList<GroundObject*>& groundObjects() const { return m_groundObjectList; }
    const QList<RegionObject*>& regionObjects() const { return m_regionObjectList; }

    /*! The start time of a propagated scenario is the earliest start time of
     *  any of the objects in the scenario.
     */
    double startTime() const { return m_startTime; }
    
    /*! The end time of a propagated scenario is the last end time of any of
     *  the objects in the scenario.
     */
    double endTime() const { return m_endTime; }
    
    bool coverageTriggered() const {return m_coverageTriggered;}
    void setCoverageTriggered() {m_coverageTriggered = true;}
    ConstellationStudy*studyOfConstellations() {return m_studyOfConstellations;}
    //void setStudyOfConstellations() {m_studyOfConstellations = new ConstellationStudy(propScenario, true, false, false);}
    void setStudyOfConstellations();

    void addSpaceObject(SpaceObject* spaceObject);
    void addGroundObject(GroundObject* groundObject);
    void addRegionObject(RegionObject* regionObject);
    
private:
    QList<SpaceObject*> m_spaceObjectList;
    QList<GroundObject*> m_groundObjectList;
    QList<RegionObject*> m_regionObjectList;
    double m_startTime;
    double m_endTime;
    bool m_coverageTriggered;
    ConstellationStudy*m_studyOfConstellations;
};

#endif // _STA_PROPAGATED_SCENARIO_H_
