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

    bool getStateVector(double mjd, sta::StateVector* result) const;
    int trajectorySampleCount() const;
    sta::StateVector trajectorySample(int index) const;
    double trajectorySampleTime(int index) const;
    
    bool generateEphemerisFile();

private:
    const StaBody* m_centralBody;
    sta::CoordinateSystem m_coordSys;
    double m_beginning;
    double m_ending;

    QList<double> m_sampleTimes;
    QList<sta::StateVector> m_samples;

    QString m_ephemerisFile;
    QTemporaryFile* m_ephemerisTempFile;
    
    // Cached value of last sample accessed when calling getStateVector
    mutable int m_lastSample; 

	QString m_ArcName;
	QString m_ArcModelName;
	QColor m_ArcTrajectoryColor;
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

    QString name() const { return m_name; }
    void setName(const QString& name) { m_name = name; }
    QString modelFile() const { return m_modelFile; }
    void setModelFile(const QString& modelFile) { m_modelFile = modelFile; }
    QColor trajectoryColor() const { return m_trajectoryColor; }
    void setTrajectoryColor(QColor color) { m_trajectoryColor = color; }
    
    void addMissionArc(MissionArc* arc);
    
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

    QString name;
    const StaBody* centralBody;
    double longitude;  // degrees
    double latitude;   // degrees
    double altitude;   // meters
};


class PropagatedScenario
{
public:
    PropagatedScenario();
    ~PropagatedScenario();
    
    const QList<SpaceObject*>& spaceObjects() const { return m_spaceObjectList; }
    const QList<GroundObject*>& groundObjects() const { return m_groundObjectList; }
    
    /*! The start time of a propagated scenario is the earliest start time of
     *  any of the objects in the scenario.
     */
    double startTime() const { return m_startTime; }
    
    /*! The end time of a propagated scenario is the last end time of any of
     *  the objects in the scenario.
     */
    double endTime() const { return m_endTime; }
    
    void addSpaceObject(SpaceObject* spaceObject);
    void addGroundObject(GroundObject* groundObject);
    
private:
    QList<SpaceObject*> m_spaceObjectList;
    QList<GroundObject*> m_groundObjectList;
    double m_startTime;
    double m_endTime;
};

#endif // _STA_PROPAGATED_SCENARIO_H_
