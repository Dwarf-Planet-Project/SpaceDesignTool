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

#include "GroundTrack.h"
#include "ClippedDraw.h"
#include "Main/propagatedscenario.h"
#include "Astro-Core/statevector.h"
#include "Astro-Core/stabody.h"
#include "Astro-Core/rectangularTOpolar.h"
#include "Astro-Core/stamath.h"
#include <cmath>
#include <algorithm>

#include <QDebug>

using namespace sta;
using namespace Eigen;


GroundTrack::GroundTrack() :
    m_vehicle(NULL)
{
}


GroundTrack::~GroundTrack()
{
    foreach (GroundTrackSegment* segment, m_segments)
    {
        delete segment;
    }
}


// Private method to compute the planetographic latitude and longitude of
// a point in the body fixed frame of the current planet. Longitude and
// latitude are in degrees, altitude is in kilometers.
static void
planetographicCoords(const Vector3d& position,
                     const StaBody* body,
                     double* longitude,
                     double* latitude,
                     double* altitude)
{
    // TODO: This simple approximation doesn't account for oblateness
    double radius = 0.0;
    rectangularTOpolar(position, radius, *longitude, *latitude);

    *longitude = sta::radToDeg(*longitude);
    *latitude = sta::radToDeg(*latitude);

    // TODO: This approximation assumes the body is spherical. Need a point-to-ellipse
    // distance function to do this right.
    *altitude = radius - body->meanRadius();
}


static void
bodyFixedPosition(const SpaceObject* spaceObj,
                  const StaBody* body,
                  double mjd,
                  double* longitude, double* latitude, double* altitude)
{
    StateVector v;
    if (spaceObj->getStateVector(mjd, *body, sta::CoordinateSystem(sta::COORDSYS_BODYFIXED), &v))
    {
        planetographicCoords(v.position, body, longitude, latitude, altitude);
    }
}


static GroundTrackSample
computeGroundTrackSample(const SpaceObject* spaceObj, const StaBody* body, double mjd)
{
    double longitude = 0.0;
    double latitude = 0.0;
    double altitude = 0.0;

    bodyFixedPosition(spaceObj, body, mjd, &longitude, &latitude, &altitude);

    GroundTrackSample sample;
    sample.mjd = mjd;
    sample.latitude = latitude;
    sample.longitude = longitude;
    sample.altitude = altitude;

    return sample;
}


static GroundTrackTick
computeGroundTrackTick(const SpaceObject* spaceObj, const StaBody* body, double mjd, double diffDt)
{
    // We want to draw the ticks so that they're perpendicular to the
    // ground track. We'll use simple differencing to compute the projected
    // direction.
    double t0 = mjd;
    double t1 = t0 + diffDt;
    if (t1 > spaceObj->missionEndTime())
    {
        t1 = t0;
        t0 = t0 - diffDt;
    }

    double longitude0 = 0.0;
    double latitude0 = 0.0;
    double altitude0 = 0.0;
    bodyFixedPosition(spaceObj, body, t0, &longitude0, &latitude0, &altitude0);

    double longitude1 = 0.0;
    double latitude1 = 0.0;
    double altitude1 = 0.0;
    bodyFixedPosition(spaceObj, body, t1, &longitude1, &latitude1, &altitude1);

    double diffLong = longitude1 - longitude0;
    double diffLat = latitude1 - latitude0;
    double l = std::sqrt(diffLong * diffLong + diffLat * diffLat);
    if (l == 0.0)
    {
        diffLong = 0.0;
        diffLat = 1.0;
    }
    else
    {
        diffLong /= l;
        diffLat /= l;
    }

    GroundTrackTick tick;
    tick.mjd = mjd;
    tick.latitude = float(latitude0);
    tick.longitude = float(longitude0);
    tick.altitude = float(altitude0);
    tick.dx = (float) -diffLat;
    tick.dy = (float) diffLong;

    return tick;
}


GroundTrackSegment::GroundTrackSegment() :
    m_missionArc(NULL)
{
}


void
GroundTrackSegment::clearSamples()
{
    m_samples.clear();
}


void
GroundTrackSegment::clearTicks()
{
    m_ticks.clear();
}


/** Add ground track samples to cover the specified time window. Remove
  * old samples that fall outside of the time window.
  *
  * \param sampleSpacing the duration between ground track samples (in Julian days)
  */
void
GroundTrackSegment::updateSamples(SpaceObject* spaceObj,
                                  MissionArc* arc,
                                  const StaBody* body,
                                  double plotStartTime,
                                  double plotEndTime,
                                  double sampleSpacing)
{
    double dt = sampleSpacing;
    double windowStartTime = std::max(arc->beginning(), plotStartTime - dt);
    double windowEndTime = std::min(arc->ending(), plotEndTime + dt);

    if (plotEndTime <= startTime() || plotStartTime >= endTime())
    {
        // Requested time window is disjoint with time window of samples, so
        // just replace everything.
        clearSamples();

        if (windowEndTime <= windowStartTime)
        {
            return;
        }

        double t = windowStartTime;
        for (;;)
        {
            t = std::min(t, windowEndTime);
            m_samples.push_back(computeGroundTrackSample(spaceObj, body, t));

            if (t == windowEndTime)
            {
                break;
            }

            t += dt;
        }
    }
    else
    {
        // Add ground track samples at the beginning
        if (plotStartTime < startTime())
        {
            double t = startTime() - dt;
            for (;;)
            {
                t = std::max(t, windowStartTime);
                m_samples.push_front(computeGroundTrackSample(spaceObj, body, t));

                if (t == windowStartTime)
                {
                    break;
                }

                t -= dt;
            }
        }

        // Add ground track samples at the end
        if (plotEndTime > endTime())
        {
            double t = endTime() + dt;
            for (;;)
            {
                t = std::min(t, windowEndTime);
                m_samples.push_back(computeGroundTrackSample(spaceObj, body, t));

                if (t == windowEndTime)
                {
                    break;
                }

                t += dt;
            }
        }

        // Remove samples before the current time window
        while (!m_samples.isEmpty() && m_samples.first().mjd < windowStartTime)
        {
            m_samples.pop_front();
        }

        while (!m_samples.isEmpty() && m_samples.last().mjd > windowEndTime)
        {
            m_samples.pop_back();
        }
    }
}


/** Add ground track samples to cover the specified time window. Remove
  * old samples that fall outside of the time window.
  */
void
GroundTrackSegment::updateTicks(SpaceObject* spaceObj,
                                MissionArc* arc,
                                const StaBody* body,
                                double plotStartTime,
                                double plotEndTime,
                                double tickSpacing)
{
    double dt = tickSpacing;
    double windowStartTime = std::max(arc->beginning(), plotStartTime - dt);
    double windowEndTime = std::min(arc->ending(), plotEndTime + dt);

    double firstTick = firstTickTime();
    double lastTick = lastTickTime();

    // Delta to use when calculating tick orientation
    double diffDt = tickSpacing * 0.01;

    if (plotEndTime <= firstTick || plotStartTime >= lastTick)
    {
        // Requested time window is disjoint with time window of samples, so
        // just replace everything.
        clearTicks();

        if (windowEndTime <= windowStartTime)
        {
            return;
        }

        double t = windowStartTime;
        for (;;)
        {
            t = std::min(t, windowEndTime);
            m_ticks.push_back(computeGroundTrackTick(spaceObj, body, t, diffDt));

            if (t == windowEndTime)
            {
                break;
            }

            t += dt;
        }
    }
    else
    {
        // Add ticks at the beginning
        if (plotStartTime < firstTick)
        {
            for (double t = firstTick - dt; t > windowStartTime; t -= dt)
            {
                m_ticks.push_front(computeGroundTrackTick(spaceObj, body, t, diffDt));
            }
        }

        // Add ticks at the end
        if (plotEndTime > lastTick)
        {
            for (double t = lastTick + dt; t < windowEndTime; t += dt)
            {
                m_ticks.push_back(computeGroundTrackTick(spaceObj, body, t, diffDt));
            }
        }

        // Remove ticks before the current time window
        while (!m_ticks.isEmpty() && m_ticks.first().mjd < windowStartTime)
        {
            m_ticks.pop_front();
        }

        // Remove ticks after the current time window
        while (!m_ticks.isEmpty() && m_ticks.last().mjd > windowEndTime)
        {
            m_ticks.pop_back();
        }
    }
}


void
GroundTrack::draw2D(QPainter& painter,
                    const StaBody* body,
                    double startMjd,
                    double endMjd,
                    const AlignedBox<float, 2>& clipBox)
{
    double west = clipBox.min().x();

    foreach (GroundTrackSegment* segment, m_segments)
    {
        const MissionArc* arc = segment->missionArc();

        if (endMjd <= arc->beginning() || startMjd >= arc->ending())
        {
            continue;
        }

        if (segment->sampleCount() == 0)
        {
            qDebug() << "Empty segment " << arc->arcName() << ", duration: " << arc->ending() - arc->beginning();
            continue;
        }

        m_trackPoints.clear();

        int firstSample = 0;
        for (int i = 0; i < segment->samples().size(); ++i)
        {
            if (startMjd > segment->sample(i).mjd)
            {
                firstSample = i;
                break;
            }
        }

        double longNow = 0.0;
        double latNow  = 0.0;
        if (firstSample == 0)
        {
            longNow = segment->sample(0).longitude;
            latNow = segment->sample(0).latitude;
        }
        else
        {
            StateVector v;
            if (m_vehicle->getStateVector(startMjd, *body, sta::CoordinateSystem(sta::COORDSYS_BODYFIXED), &v))
            {
                double altNow = 0.0;
                planetographicCoords(v.position, body, &longNow, &latNow, &altNow);
            }
        }

        if (longNow < west)
        {
            longNow += 360.0;
        }

        bool complete = false;
        for (int i = firstSample + 1; i < segment->samples().size() && !complete; i++)
        {
            double long0 = segment->sample(i - 1).longitude;
            double lat0  = segment->sample(i - 1).latitude;
            double long1 = segment->sample(i).longitude;
            double lat1  = segment->sample(i).latitude;

            // Only draw the track up to the specified endTime
            if (segment->sample(i).mjd >= endMjd)
            {
                StateVector v;
                if (m_vehicle->getStateVector(endMjd, *body, sta::CoordinateSystem(sta::COORDSYS_BODYFIXED), &v))
                {
                    double alt = 0.0;
                    planetographicCoords(v.position, body, &long1, &lat1, &alt);
                }
                complete = true;
            }

            if (long0 < west)
                long0 += 360.0f;
            if (long1 < west)
                long1 += 360.0f;

            double longDiff = std::abs(long0 - long1);
            if (longDiff > 90.0)
            {
                // Adjacent ground track points have very different longitudes. This
                // can arise from three things:
                //   1. The two points straddle longitude 180 degrees. In this case, the
                //      difference will be around 360 degrees.
                //   2. The orbit crosses near a pole. The difference will be approximately
                //      180 degrees.
                //   3. The ground track doesn't have enough samples. Nothing we can do
                //      about this--the user should have specified a smaller output interval.
                //
                // We will treat differences of 90 - 270 degrees as polar crossings, and
                // differences > 270 degrees as crossings of the prime meridian. This is
                // effective except in the case where there are far too few ground track
                // samples.
                if (longDiff > 270.0)
                {
                    // Handle segments that wrap across the edges of the map.
                    if (long0 > long1)
                    {
                        m_trackPoints << QPointF(long0, lat0) << QPointF(long1 + 360, lat1);
                        m_trackPoints << QPointF(long0 - 360, lat0) << QPointF(long1, lat1);
                    }
                    else
                    {
                        m_trackPoints << QPointF(long1, lat1) << QPointF(long0 + 360, lat0);
                        m_trackPoints << QPointF(long1 - 360, lat1) << QPointF(long0, lat0);
                    }
                }
                else
                {
                    double sign = long0 - long1 < 0.0 ? -1.0 : 0.0;
                    if (lat0 > 0)
                    {
                        // Crossing the north pole
                        m_trackPoints << QPointF(long0, lat0) << QPointF(long0 + sign * (180.0 - longDiff), 180.0 - lat1);
                        m_trackPoints << QPointF(long1, lat1) << QPointF(long1 + sign * (180.0 - longDiff), 180.0 - lat0);
                    }
                    else
                    {
                        // Crossing the south pole
                        m_trackPoints << QPointF(long0, lat0) << QPointF(long0 - sign * (180.0 - longDiff), lat1 - 180.0);
                        m_trackPoints << QPointF(long1, lat1) << QPointF(long1 - sign * (180.0 - longDiff), lat0 - 180.0);
                    }
                }
            }
            else
            {
                m_trackPoints << QPointF(long0, lat0) << QPointF(long1, lat1);
            }
        }

        painter.setPen(segment->color());
        painter.drawLines(m_trackPoints);
    }
}


void
GroundTrack::drawTicks(QPainter& painter,
                       const StaBody* body,
                       double startMjd,
                       double endMjd,
                       float tickScale,
                       const AlignedBox<float, 2>& clipBox)
{
    double west = clipBox.min().x();

    foreach (GroundTrackSegment* segment, m_segments)
    {
        const MissionArc* arc = segment->missionArc();

        if (endMjd <= arc->beginning() || startMjd >= arc->ending())
        {
            continue;
        }

        m_trackPoints.clear();

        foreach (GroundTrackTick tick, segment->ticks())
        {
            if (tick.mjd > endMjd)
            {
                break;
            }
            else if (tick.mjd >= startMjd)
            {
                float tickLongitude = tick.longitude;
                if (tickLongitude < west)
                {
                    tickLongitude += 360.0f;
                }
                m_trackPoints << QPointF(tickLongitude - tick.dx * tickScale,
                                         tick.latitude  - tick.dy * tickScale)
                              << QPointF(tickLongitude + tick.dx * tickScale,
                                         tick.latitude  + tick.dy * tickScale);
            }
        }

        painter.setPen(segment->color());
        painter.drawLines(m_trackPoints);
    }
}


void
GroundTrack::draw(QPainter& painter,
                  const StaBody* body,
                  double startMjd,
                  double endMjd,
                  const Transform3f& projection,
                  const AlignedBox<float, 3>& clipBox)
{
    double mjd = endMjd; // TODO: FIX
    // Calculate the current latitude and longitude of the vehicle
    StateVector v;
    double longNow = 0.0;
    double latNow  = 0.0;
    double altNow  = 0.0;
    bool activeNow = false;

    float west = clipBox.min().x();
    float east = clipBox.max().x();
    float north = clipBox.max().y();
    float south = clipBox.min().y();
    float maxHeight = clipBox.max().z();

    if (m_vehicle->getStateVector(mjd, *body, sta::CoordinateSystem(sta::COORDSYS_BODYFIXED), &v))
    {
        planetographicCoords(v.position, body, &longNow, &latNow, &altNow);
        if (longNow < west)
            longNow += 360.0;
        activeNow = true;
    }

    foreach (GroundTrackSegment* segment, m_segments)
    {
        m_trackPoints.clear();

        QPen pen = painter.pen();
        pen.setColor(segment->color());
        painter.setPen(pen);

        bool trackVisible = false;
        if (segment->samples().empty() || (mjd <= segment->startTime()))
        {
            trackVisible = true;
        }

        double endTime = mjd;

        bool complete = trackVisible;

        // Draw the ground track
        for (int i = 1; i < segment->samples().size() && !complete; i++)
        {
            double long0 = segment->sample(i - 1).longitude;
            double lat0  = segment->sample(i - 1).latitude;
            double long1 = segment->sample(i).longitude;
            double lat1  = segment->sample(i).latitude;

            // Only draw the track up to the specified endTime
            if (segment->sample(i).mjd >= endTime)
            {
                long1 = longNow;
                lat1  = latNow;
                complete = true;
            }

            ClippedWrappedLine(clipBox, (float) long0, (float) lat0, (float) long1, (float) lat1, m_trackPoints);
        }

        painter.setPen(segment->color());
        painter.drawLines(m_trackPoints);
        m_trackPoints.clear();
        complete = trackVisible;

        // Draw the track through space
        for (int i = 1; i < segment->sampleCount() && !complete; i++)
        {
            float long0 = (float) segment->sample(i - 1).longitude;
            float lat0  = (float) segment->sample(i - 1).latitude;
            float alt0  = (float) segment->sample(i - 1).altitude;
            float long1 = (float) segment->sample(i).longitude;
            float lat1  = (float) segment->sample(i).latitude;
            float alt1  = (float) segment->sample(i).altitude;

            // Only draw the track up to the specified endTime
            if (segment->sample(i).mjd >= endTime)
            {
                long1 = (float) longNow;
                lat1  = (float) latNow;
                alt1  = (float) altNow;
                complete = true;
            }

            if (long0 < west)
                long0 += 360.0f;
            if (long1 < west)
                long1 += 360.0f;

            if (std::abs(long0 - long1) > 180.0f)
            {
                // Handle segments that wrap across the edges of the map.
                if (long0 > long1)
                {
                    ClippedLine(clipBox, projection,
                                long0, lat0, alt0, long1 + 360, lat1, alt1,
                                m_trackPoints);
                    ClippedLine(clipBox, projection,
                                long0 - 360, lat0, alt0, long1, lat1, alt1,
                                m_trackPoints);
                }
                else
                {
                    ClippedLine(clipBox, projection,
                                long1, lat1, alt1, long0 + 360, lat0, alt0,
                                m_trackPoints);
                    ClippedLine(clipBox, projection,
                                long1 - 360, lat1, alt1, long0, lat0, alt0,
                                m_trackPoints);
                }
            }
            else
            {
                ClippedLine(clipBox, projection, long0, lat0, alt0, long1, lat1, alt1, m_trackPoints);
            }
        }
        painter.setPen(segment->color());
        painter.drawLines(m_trackPoints);
    }
}


void
GroundTrack::drawDropLines(QPainter& painter,
                           const StaBody* body,
                           double startMjd,
                           double endMjd,
                           float tickInterval,
                           const Eigen::Transform3f& projection,
                           const Eigen::AlignedBox<float, 3>& clipBox)
{
    double mjd = endMjd;

    float west = clipBox.min().x();
    float maxHeight = clipBox.max().z();

    double endTime = mjd;

    foreach (GroundTrackSegment* segment, m_segments)
    {
        m_trackPoints.clear();

        QPen pen = painter.pen();
        pen.setColor(segment->color());
        painter.setPen(pen);

        double startTime = 0.0;
        if (segment->sampleCount() > 0)
            startTime = segment->startTime();
        int lastTick = std::min(lastTick,
                                (int) std::ceil((endTime - startTime) / sta::secsToDays(tickInterval)));

        for (int i = 0; i < lastTick; i++)
        {
            const GroundTrackTick& tick = segment->tick(i);
            float tickLongitude = tick.longitude;
            if (tickLongitude < west)
                tickLongitude += 360.0f;

            if (clipBox.contains(Vector3f(tickLongitude, tick.latitude, 0.0f)) && tick.altitude > 0.0f)
            {
                Vector3f groundPoint = projection * Vector3f(tickLongitude, tick.latitude, 0.0f);
                Vector3f skyPoint    = projection * Vector3f(tickLongitude, tick.latitude, std::min(maxHeight, tick.altitude));

                m_trackPoints << QPointF(groundPoint.x(), groundPoint.y())
                              << QPointF(skyPoint.x(), skyPoint.y());
            }
        }

        painter.setPen(segment->color());
        painter.drawLines(m_trackPoints);
    }
}


void
GroundTrack::clearSamples()
{
    foreach (GroundTrackSegment* segment, m_segments)
    {
        segment->clearSamples();
    }
}


void
GroundTrack::clearTicks()
{
    foreach (GroundTrackSegment* segment, m_segments)
    {
        segment->clearTicks();
    }
}


/** Update the samples in a ground track for the specified time interval.
  *
  * \param spaceObj the space object
  * \param body the planet on which we're displaying the ground track
  * \param plotStartTime start of the time interval (MJD)
  * \param plotEndTime end of the time interval (MJD)
  */
void
GroundTrack::updateSamples(SpaceObject* spaceObj, const StaBody* body, double plotStartTime, double plotEndTime)
{
    unsigned int index = 0;
    foreach (GroundTrackSegment* segment, m_segments)
    {
        if (plotStartTime < segment->startTime() || plotEndTime > segment->endTime())
        {
            double maxSampleSpacing = (segment->missionArc()->ending() - segment->missionArc()->beginning()) / segment->missionArc()->trajectorySampleCount();
            double sampleSpacing = std::min(maxSampleSpacing, 1.0 / 1440.0);

            segment->updateSamples(spaceObj, spaceObj->mission().at(index), body, plotStartTime, plotEndTime, sampleSpacing);
        }
        index++;
    }
}


/** Update the ticks in a ground track for the specified time interval.
  *
  * \param spaceObj the space object
  * \param body the planet on which we're displaying the ground track
  * \param plotStartTime start of the time interval (MJD)
  * \param plotEndTime end of the time interval (MJD)
  * \param tickSpacing time between ticks (in Julian days)
  */
void
GroundTrack::updateTicks(SpaceObject* spaceObj, const StaBody* body, double plotStartTime, double plotEndTime, double tickSpacing)
{
    unsigned int index = 0;
    foreach (GroundTrackSegment* segment, m_segments)
    {
        if (plotStartTime < segment->firstTickTime() || plotEndTime > segment->lastTickTime())
        {
            segment->updateTicks(spaceObj, spaceObj->mission().at(index), body, plotStartTime, plotEndTime, tickSpacing);
        }
        index++;
    }
}

