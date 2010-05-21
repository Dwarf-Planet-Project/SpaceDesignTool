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
 // Patched bu Guillermo June 20th 2009
 */

#include <Eigen/Core>
#include <Eigen/Geometry>
#include <QTemporaryFile>
#include <QtAlgorithms>
#include <QtDebug>

#include "propagatedscenario.h"
#include "celestiainterface.h"
#include <Astro-Core/RotationState.h>
#include <Astro-Core/Interpolators.h>
#include <Astro-Core/stamath.h>

using namespace sta;
using namespace Eigen;


/****** MissionArc ******/

MissionArc::MissionArc(const StaBody* centralBody,
                       sta::CoordinateSystem coordSys,
                       QList<double> sampleTimes,
                       QList<sta::StateVector> samples) :
    m_centralBody(centralBody),
    m_coordSys(coordSys),
    m_beginning(0.0),
    m_ending(0.0),
    m_sampleTimes(sampleTimes),
    m_samples(samples),
    m_ephemerisTempFile(NULL)
{
    Q_ASSERT(m_sampleTimes.size() == m_samples.size());
    Q_ASSERT(m_samples.size() > 0);

    m_beginning = m_sampleTimes.first();
    m_ending = m_sampleTimes.last();
}


MissionArc::~MissionArc()
{
    // deleting the QTemporaryFile objects will clean up the temp files
    delete m_ephemerisTempFile;
}


/*! Return the state vector for the trajectory at a specified time, using
 *  cubic interpolation between sample times.
 *
 *  @param mjd: a modified Julian Date, TDB
 *  @param result: pointer to a state vector in which the result will be stored
 *
 *  The function returns true if the state vector could be calculated, and false
 *  if there was an error. Errors may result if this function is called before
 *  the the trajectory has been propagated, or if the requested time is not
 *  within the range of time over which trajectory is valid. The returned
 *  state vector is in whatever coordinate system was defined for the trajectory.
 */
bool
MissionArc::getStateVector(double mjd,
                           sta::StateVector* result) const
{
    Q_ASSERT(m_sampleTimes.size() == m_samples.size());

    if (!m_sampleTimes.isEmpty() &&
        mjd >= m_sampleTimes.first() &&
        mjd <= m_sampleTimes.last())
    {
        int i = -1;
        if (m_lastSample > 0 &&
            m_lastSample < m_sampleTimes.size() - 1&&
            mjd >= m_sampleTimes[m_lastSample] &&
            mjd <= m_sampleTimes[m_lastSample + 1])
        {
            i = m_lastSample;
        }
        else if (mjd == m_sampleTimes.first())
        {
            // Need special handling for the situation when requested time
            // exactly equal to the first sample time.
            i = 1;
        }
        else
        {
            QList<double>::const_iterator iter = qLowerBound(m_sampleTimes, mjd);

            // This can only occur if the samples aren't sorted
            Q_ASSERT(iter != m_sampleTimes.end());
            Q_ASSERT(iter != m_sampleTimes.begin());

            i = iter - m_sampleTimes.begin();
        }

        double h = m_sampleTimes[i] - m_sampleTimes[i - 1];
        double t = (mjd - m_sampleTimes[i - 1]) / h;

        *result = cubicInterpolate(m_samples[i - 1],
                                   m_samples[i],
                                   t,
                                   sta::daysToSecs(h));

        m_lastSample = i;

        return true;
    }
    else
    {
        return false;
    }
}


/*! Return the number of samples in the trajectory.
 */
int
MissionArc::trajectorySampleCount() const
{
    return m_samples.size();
}


/*! Return the state vector at the specified sample index. The index
 *  must be >= 0 and less than the sample count.
 */
sta::StateVector
MissionArc::trajectorySample(int index) const
{
    return m_samples[index];
}


/*! Return the time stamp for the specified sample index. The index
 *  must be >= 0 and less than the sample count.
 */
double
MissionArc::trajectorySampleTime(int index) const
{
    return m_sampleTimes[index];
}


/*! Write an ASCII ephemeris file for this mission arc. One record
 *  is written for each sample. The format of each record is:
 *
 * <mjd> <x> <y> <z> <vx> <vy> <vz>
 *
 * mjd is a modified Julian date
 * x, y, and z are the position
 * vx, vy, and vz are the velocity
 *
 * Return true if the file was successfully created and written.
 */
bool
MissionArc::generateEphemerisFile()
{
    // Create a new temporary file for the ephemeris
    if (m_ephemerisTempFile != NULL)
        delete m_ephemerisTempFile;

    m_ephemerisTempFile = new QTemporaryFile("ephem-XXXXXX.stae");
    if (!m_ephemerisTempFile->open())
    {
        delete m_ephemerisTempFile;
        m_ephemerisTempFile = NULL;
        m_ephemerisFile = "";
        return false;
    }

    //m_ephemerisFile = QDir::tempPath() + "/" + m_ephemerisTempFile->fileName();
    m_ephemerisFile = m_ephemerisTempFile->fileName();
    //qDebug() << "ephemeris file: " << m_ephemerisTempFile->fileName();

    QTextStream ephemerisStream(m_ephemerisTempFile);

    // Output values at full double precision
    ephemerisStream.setRealNumberPrecision(16);

    // Write all samples
    for (int i = 0; i < m_sampleTimes.size(); ++i)
    {
        ephemerisStream << m_sampleTimes[i] << " "
                        << m_samples[i].position.x() << " "
                        << m_samples[i].position.y() << " "
                        << m_samples[i].position.z() << " "
                        << m_samples[i].velocity.x() << " "
                        << m_samples[i].velocity.y() << " "
                        << m_samples[i].velocity.z() << endl;
    }

    m_ephemerisTempFile->close();

    return true;
}


/****** SpaceObject        ******/

SpaceObject::SpaceObject() :
    m_missionStartTime(0.0),
    m_missionEndTime(0.0),
    m_celestiaBody(NULL)
{
}


SpaceObject::~SpaceObject()
{
    foreach (MissionArc* arc, m_missionArcs)
    {
        delete arc;
    }

    delete m_celestiaBody;
}


void
SpaceObject::addMissionArc(MissionArc* arc)
{
    // Extend the time range of the mission to include the span of the newly added arc
    // TODO: enforce contiguity of mission arcs
    if (m_missionArcs.empty() || arc->beginning() < m_missionStartTime)
        m_missionStartTime = arc->beginning();
    if (m_missionArcs.empty() || arc->ending() > m_missionEndTime)
        m_missionEndTime = arc->ending();

    m_missionArcs.append(arc);
}



/*! Return the state vector for this space vehicle at the specified time.
 *  @param mjd: a modified Julian Date, TDB
 *  @param center: the returned state vector is relative to the center object
 *  @param coordSys:
 *  @param result: pointer to a state vector in which the result will be stored
 *
 *  The function returns true if the state vector could be calculated, and false
 *  if there was an error. Errors may result if  the requested time is not
 *  within the range of times where the space vehicle's trajectory is defined.
 *
 *  The returned state vector is not corrected for light time or aberration.
 */
bool
SpaceObject::getStateVector(double mjd,
                            const StaBody& center,
                            const sta::CoordinateSystem& coordSys,
                            sta::StateVector* result) const
{
    if (mission().empty())
    {
        return false;
    }

    // Find the mission arc that contains the requested time.
    // TODO: The simple linear search is adequate for a small number of mission arcs.
    MissionArc* arc = NULL;
    foreach (arc, mission())
    {
        if (mjd >= arc->beginning() && mjd <= arc->ending())
        {
            break;
        }
    }

    // Requested time does not lie within any mission arcs
    if (!arc)
    {
        return false;
    }

    sta::StateVector v;
    if (!arc->getStateVector(mjd, &v))
    {
        return false;
    }

    // Convert the state vector to the requested coordinate system
    *result = CoordinateSystem::convert(v, mjd,
                                        arc->centralBody(), arc->coordinateSystem(),
                                        &center, coordSys);

    return true;
}


/*! Generate ephemeris file for all mission arcs. Return true if all segments
 *  were written succesfully. This method aborts and return false immediately
 *  after the first unsuccessful attempt to write an ephemeris.
 */
bool
SpaceObject::generateEphemerisFiles()
{
    foreach (MissionArc* arc, mission())
    {
        if (!arc->generateEphemerisFile())
            return false;
    }

    return true;
}


/*! Create a representation of this space object in the 3D view. Return true
 *  if a Celestia object was successfully created, false if not.
 */
bool
SpaceObject::realize3DViewRepresentation(CelestiaInterface* celestia)
{
    // Don't create a Celestia object if there are no mission arcs
    if (mission().empty())
    {
        return false;
    }

    // Don't create a Celestia object if one already exists
    if (m_celestiaBody)
    {
        return false;
    }
    m_celestiaBody = celestia->createSpacecraft(m_name, STA_SOLAR_SYSTEM->lookup(STA_SUN));

    // Set the timeline and other properties of the body.
    if (m_celestiaBody)
    {
        m_celestiaBody->setTimeline(mission());
        m_celestiaBody->setGeometry(m_modelFile);
        m_celestiaBody->setVisible(true);
        m_celestiaBody->setOrbitAlwaysVisible(true);
        m_celestiaBody->setOrbitColorOverridden(true);
        m_celestiaBody->setOrbitColor(m_trajectoryColor);

        m_celestiaBody->mark("diamond", m_trajectoryColor, 5.0f, "");
    }
    return m_celestiaBody != NULL;
}


/****** GroundObject ******/

GroundObject::GroundObject() :
    m_celestiaLocation(NULL)
{
}


GroundObject::~GroundObject()
{
    delete m_celestiaLocation;
}


/*! Create a representation of this space object in the 3D view. Return true
 *  if a Celestia object was successfully created, false if not.
 */
bool
GroundObject::realize3DViewRepresentation(CelestiaInterface* celestia)
{
    // Don't create a Celestia object if one already exists
    if (m_celestiaLocation)
        return false;

    m_celestiaLocation = celestia->createLocation(name, centralBody,
                                                  latitude, longitude, altitude);

    return true;
}


/** Return the apparent angle of a spacecraft above a ground station's horizon.
  *
  * @return the elevation angle in radians
  */
double
GroundObject::elevationAngle(const SpaceObject* spacecraft, double t) const
{
    sta::StateVector spacecraftState;

    spacecraft->getStateVector(t, *centralBody, sta::CoordinateSystem(sta::COORDSYS_BODYFIXED), &spacecraftState);
    Vector3d stationPos = centralBody->planetographicToCartesian(latitude, longitude, 0.0);

    // TODO: should use ellipsoid normal
    Vector3d up = stationPos.normalized();

    Vector3d toSpacecraft = (spacecraftState.position - stationPos).normalized();
    double cosAngle = up.dot(toSpacecraft);
    double angleFromZenith;

    // Be careful about arccos domain...
    if (cosAngle < -1.0 + Eigen::precision<double>())
    {
        // Satellite is directly behind the antenna
        angleFromZenith = sta::Pi();
    }
    else if (cosAngle > 1.0 - Eigen::precision<double>())
    {
        // Satellite exactly aligned with antenna
        angleFromZenith = 0.0;
    }
    else
    {
        angleFromZenith = std::acos(cosAngle);
    }

    return sta::halfPi() - angleFromZenith;
}

//This function is added by Ricardo Noriega to get the range between a groundStation and a spacecraft
double GroundObject::getRange(const SpaceObject *spacecraft, double t) const{

    sta::StateVector spacecraftState;

    spacecraft->getStateVector(t, *centralBody, sta::CoordinateSystem(sta::COORDSYS_BODYFIXED), &spacecraftState);
    Vector3d stationPos = centralBody->planetographicToCartesian(latitude, longitude, 0.0);

    // TODO: should use ellipsoid normal

    Vector3d toSpacecraft = (spacecraftState.position - stationPos);

    double range=toSpacecraft.norm();

    return range; //Km

}

/****** PropagatedScenario ******/

/*! Create a new, empty propagated scenario.
 */
PropagatedScenario::PropagatedScenario() :
    m_startTime(0.0),
    m_endTime(0.0)
{
}


PropagatedScenario::~PropagatedScenario()
{
    foreach (SpaceObject *spaceObj, m_spaceObjectList)
    {
        delete spaceObj;
    }

    foreach (GroundObject* groundObj, m_groundObjectList)
    {
        delete groundObj;
    }
}


void
PropagatedScenario::addSpaceObject(SpaceObject* spaceObject)
{
    // Extend the time range of the scenario to include the mission
    // span of the space object.
    if (m_spaceObjectList.empty() || spaceObject->missionStartTime() < m_startTime)
        m_startTime = spaceObject->missionStartTime();
    if (m_spaceObjectList.empty() || spaceObject->missionEndTime() > m_endTime)
        m_endTime = spaceObject->missionEndTime();

    m_spaceObjectList.append(spaceObject);
}


void
PropagatedScenario::addGroundObject(GroundObject* groundObject)
{
    m_groundObjectList.append(groundObject);
}


