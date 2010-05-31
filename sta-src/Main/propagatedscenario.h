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

class CelestiaBody;
class CelestiaLocation;
class CelestiaInterface;
class QTemporaryFile;


/*! A mission arc is a trajectory in a propagated scenario.
 *  It also corresponds to a TimelinePhase in Celestia.
 */
class MissionArc
{
public:
    MissionArc(const StaBody* centralBody,
               sta::CoordinateSystem coordSys,
               QList<double> sampleTimes,
               QList<sta::StateVector> samples);
    ~MissionArc();
    
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
};



// A SpaceObject instance represents an object that has
// an integrated trajectory through space.

class SpaceObject
{
public:
    SpaceObject();
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
    bool realize3DViewRepresentation(CelestiaInterface* celestia);
    
    CelestiaBody* celestiaObject() { return m_celestiaBody; }
    
private:
    double m_missionStartTime;
    double m_missionEndTime;
    QList<MissionArc*> m_missionArcs;
    
    QString m_name;
    QString m_modelFile;
    QColor m_trajectoryColor;
    
    CelestiaBody* m_celestiaBody;
};


// A GroundObject instance represents an object that is 
// fixed to the surface of a planet or moon.
class GroundObject
{
public:
    GroundObject();
    ~GroundObject();

    bool realize3DViewRepresentation(CelestiaInterface* celestia);
    double elevationAngle(const SpaceObject* spacecraft, double t) const;
    double azimuthAngle(const SpaceObject* spacecraft, double t) const;
    double getRange(const SpaceObject* spacecraft, double t) const;

    QString name;
    const StaBody* centralBody;
    double longitude;  // degrees
    double latitude;   // degrees
    double altitude;   // meters
    
private:
    CelestiaLocation* m_celestiaLocation;
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
