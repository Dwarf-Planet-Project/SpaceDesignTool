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

using namespace sta;
using namespace Eigen;


GroundTrack::GroundTrack() :
    vehicle(NULL)
{
}


GroundTrack::~GroundTrack()
{
    foreach (GroundTrackSegment* segment, segments)
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


#if 0
enum
{
    OUT_WEST   = 0x01,
    OUT_EAST   = 0x02,
    OUT_SOUTH  = 0x04,
    OUT_NORTH  = 0x08,
    OUT_ABOVE  = 0x10,
    OUT_BELOW  = 0x20
};


// Compute outcodes for Cohen-Sutherland line clipping
static unsigned int computeOutcode(const AlignedBox<float, 3>& clipBox, float longitude, float latitude)
{
    unsigned int outcode = 0;

    outcode |= (longitude < clipBox.min().x() ) ? OUT_WEST   : 0x0;
    outcode |= (longitude > clipBox.max().x() ) ? OUT_EAST   : 0x0;
    outcode |= (latitude  < clipBox.min().y())  ? OUT_SOUTH  : 0x0;
    outcode |= (latitude  > clipBox.max().y())  ? OUT_NORTH  : 0x0;

    return outcode;
}


static void clippedLine(const AlignedBox<float, 3>& clipBox,
                        float long0, float lat0,
                        float long1, float lat1,
                        QVector<QPointF>& points)
{
    bool done = false;
    unsigned int outcode0 = computeOutcode(clipBox, long0, lat0);
    unsigned int outcode1 = computeOutcode(clipBox, long1, lat1);

    // Cohen-Sutherland line clipping. Lots of code here, but the common
    // cases of unclipped and invisible line segments are handled very
    // quickly.
    while (!done)
    {
        bool accept = (outcode0 | outcode1) == 0;
        bool reject = (outcode0 & outcode1) != 0;
        bool clippingRequired = !accept && !reject;

        if (accept)
        {
            points << QPointF(long0, lat0) << QPointF(long1, lat1);
            done = true;
        }
        else if (reject)
        {
            done = true;
        }
        else if (clippingRequired)
        {
            float clipX;
            float clipY;

            // One or both endpoints outside the clip volume. Pick one of them.
            unsigned int outcodeOut = outcode0 ? outcode0: outcode1;

            // Find the intersection point.
            if (outcodeOut & OUT_NORTH)
            {
                clipX = long0 + (long1 - long0) * (clipBox.max().y() - lat0) / (lat1 - lat0);
                clipY = clipBox.max().y();
            }
            else if (outcodeOut & OUT_SOUTH)
            {
                clipX = long0 + (long1 - long0) * (clipBox.min().y() - lat0) / (lat1 - lat0);
                clipY = clipBox.min().y();
            }
            else if (outcodeOut & OUT_EAST)
            {
                clipY = lat0 + (lat1 - lat0) * (clipBox.max().x() - long0) / (long1 - long0);
                clipX = clipBox.max().x();
            }
            else if (outcodeOut & OUT_WEST)
            {
                clipY = lat0 + (lat1 - lat0) * (clipBox.min().x() - long0) / (long1 - long0);
                clipX = clipBox.min().x();
            }

            // Now we move outside point to intersection point to clip
            // and get ready for next pass.
            if (outcodeOut == outcode0)
            {
                long0 = clipX;
                lat0 = clipY;
                outcode0 = computeOutcode(clipBox, long0, lat0);
            }
            else
            {
                long1 = clipX;
                lat1 = clipY;
                outcode1 = computeOutcode(clipBox, long1, lat1);
            }
        }
    }
}


// Compute outcodes for Cohen-Sutherland line clipping in 3D
static unsigned int computeOutcode(const AlignedBox<float, 3>& clipBox, float longitude, float latitude, float altitude)
{
    // Clip in 2D first
    unsigned int outcode = computeOutcode(clipBox, longitude, latitude);

    outcode |= (altitude < clipBox.min().z() ) ? OUT_BELOW   : 0x0;
    outcode |= (altitude > clipBox.max().z() ) ? OUT_ABOVE   : 0x0;

    return outcode;
}


// Clip a line and 3D and emit the endpoints into the specified vector
// (unless the line was clipped completely.) This function is required
// the 2.5D view: since the Qt canvas is strictly 2D, we need to take
// care of doing our own line clipping in three dimensions. If we switched
// to using OpenGL for drawing, we could use GL's clip planes for this
// task, but those aren't supported by all drivers.
static void clippedLine(const AlignedBox<float, 3>& clipBox,
                        const Transform3f& projection,
                        float long0, float lat0, float alt0,
                        float long1, float lat1, float alt1,
                        QVector<QPointF>& points)
{
    bool done = false;
    unsigned int outcode0 = computeOutcode(clipBox, long0, lat0, alt0);
    unsigned int outcode1 = computeOutcode(clipBox, long1, lat1, alt1);

    // Cohen-Sutherland line clipping, extended to a third dimension.
    // Lots of code here, but the common cases of unclipped and invisible
    // line segments are handled very quickly.
    while (!done)
    {
        bool accept = (outcode0 | outcode1) == 0;
        bool reject = (outcode0 & outcode1) != 0;
        bool clippingRequired = !accept && !reject;

        if (accept)
        {
            Vector3f p0    = projection * Vector3f(long0, lat0, alt0);
            Vector3f p1    = projection * Vector3f(long1, lat1, alt1);
            points << QPointF(p0.x(), p0.y()) << QPointF(p1.x(), p1.y());
            done = true;
        }
        else if (reject)
        {
            done = true;
        }
        else if (clippingRequired)
        {
            float clipX;
            float clipY;
            float clipZ;

            // One or both endpoints outside the clip volume. Pick one of them.
            unsigned int outcodeOut = outcode0 ? outcode0: outcode1;

            // Now find the intersection point;
            // use formulas y = y0 + slope * (x - x0), x = x0 + (1/slope)* (y - y0)
            if (outcodeOut & OUT_NORTH)
            {
                float w = (clipBox.max().y() - lat0) / (lat1 - lat0);
                clipX = long0 + (long1 - long0) * w;
                clipZ = alt0 + (alt1 - alt0) * w;
                clipY = clipBox.max().y();
            }
            else if (outcodeOut & OUT_SOUTH)
            {
                float w = (clipBox.min().y() - lat0) / (lat1 - lat0);
                clipX = long0 + (long1 - long0) * w;
                clipZ = alt0 + (alt1 - alt0) * w;
                clipY = clipBox.min().y();
            }
            else if (outcodeOut & OUT_EAST)
            {
                float w = (clipBox.max().x() - long0) / (long1 - long0);
                clipY = lat0 + (lat1 - lat0) * w;
                clipZ = alt0 + (alt1 - alt0) * w;
                clipX = clipBox.max().x();
            }
            else if (outcodeOut & OUT_WEST)
            {
                float w = (clipBox.min().x() - long0) / (long1 - long0);
                clipY = lat0 + (lat1 - lat0) * w;
                clipZ = alt0 + (alt1 - alt0) * w;
                clipX = clipBox.min().x();
            }
            else if (outcodeOut & OUT_ABOVE)
            {
                float w = (clipBox.max().z() - alt0) / (alt1 - alt0);
                clipX = long0 + (long1 - long0) * w;
                clipY = lat0 + (lat1 - lat0) * w;
                clipZ = clipBox.max().z();
            }
            else if (outcodeOut & OUT_BELOW)
            {
                float w = (clipBox.min().z() - alt0) / (alt1 - alt0);
                clipX = long0 + (long1 - long0) * w;
                clipY = lat0 + (lat1 - lat0) * w;
                clipZ = clipBox.min().z();
            }

            // Now we move outside point to intersection point to clip
            // and get ready for next pass.
            if (outcodeOut == outcode0)
            {
                long0 = clipX;
                lat0 = clipY;
                alt0 = clipZ;
                outcode0 = computeOutcode(clipBox, long0, lat0, alt0);
            }
            else
            {
                long1 = clipX;
                lat1 = clipY;
                alt1 = clipZ;
                outcode1 = computeOutcode(clipBox, long1, lat1, alt1);
            }
        }
    }
}



// Utility function for easier line drawing. This handles the complexities of
// clipping and wrapping. From zero to two line segments will be emitted into
// the points vector:
//   * 0 if the line was completely clipped
//   * 1 if the line was partially clipped or not clipped at all
//   * 2 if the line was clipped at the east or west edge of a wide view
static void
clippedWrappedLine(const AlignedBox<float, 3>& clipBox,
                   float long0, float lat0,
                   float long1, float lat1,
                   QVector<QPointF>& points)
{
    if (long0 < clipBox.min().x())
        long0 += 360.0;
    if (long1 < clipBox.min().x())
        long1 += 360.0;

    if (std::abs(long0 - long1) > 180.0)
    {
        // Handle segments that wrap across the edges of the map.
        if (long0 > long1)
        {
            clippedLine(clipBox, (float) long0, (float) lat0, (float) long1 + 360, (float) lat1, points);
            clippedLine(clipBox, (float) long0 - 360, (float) lat0, (float) long1, (float) lat1, points);
        }
        else
        {
            clippedLine(clipBox, (float) long1, (float) lat1, (float) long0 + 360, (float) lat0, points);
            clippedLine(clipBox, (float) long1 - 360, (float) lat1, (float) long0, (float) lat0, points);
        }
    }
    else
    {
        clippedLine(clipBox, (float) long0, (float) lat0, (float) long1, (float) lat1, points);
    }
}
#endif


void
GroundTrack::draw2D(QPainter& painter,
                    const StaBody* body,
                    double mjd,
                    const AlignedBox<float, 2>& clipBox)
{
    bool showEntireTrack = false;

    double west = clipBox.min().x();

    foreach (GroundTrackSegment* segment, segments)
    {
        // Calculate the ground track
        double endTime = std::numeric_limits<double>::infinity();
        bool complete = false;

        m_trackPoints.clear();

        if (!showEntireTrack)
        {
            endTime = mjd;
            if (segment->samples.size() > 0 &&
                mjd <= segment->samples[0].mjd)
            {
                complete = true;
            }
        }

        StateVector v;
        double longNow = 0.0;
        double latNow  = 0.0;
        if (vehicle->getStateVector(mjd, *body, sta::CoordinateSystem(sta::COORDSYS_BODYFIXED), &v))
        {
            double altNow = 0.0;
            planetographicCoords(v.position, body, &longNow, &latNow, &altNow);
            if (longNow < west)
                longNow += 360.0;
        }

        for (int i = 1; i < segment->samples.size() && !complete; i++)
        {
            double long0 = segment->samples[i - 1].longitude;
            double lat0  = segment->samples[i - 1].latitude;
            double long1 = segment->samples[i].longitude;
            double lat1  = segment->samples[i].latitude;

            // Only draw the track up to the specified endTime
            if (segment->samples[i].mjd >= endTime)
            {
                long1 = longNow;
                lat1  = latNow;
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

        painter.setPen(segment->color);
        painter.drawLines(m_trackPoints);
    }
}


void
GroundTrack::drawTicks(QPainter& painter,
                       const StaBody* body,
                       double mjd,
                       float tickInterval,
                       float tickScale,
                       const AlignedBox<float, 2>& clipBox)
{
    bool showEntireTrack = false;

    double west = clipBox.min().x();

    foreach (GroundTrackSegment* segment, segments)
    {
        double endTime = std::numeric_limits<double>::infinity();
        bool complete = false;

        m_trackPoints.clear();

        if (!showEntireTrack)
        {
            endTime = mjd;
        }

        m_trackPoints.clear();

        double startTime = 0.0;
        if (segment->samples.size() > 0)
            startTime = segment->samples.first().mjd;
        int lastTick = segment->ticks.size();
        if (!showEntireTrack)
        {
            lastTick = std::min(lastTick,
                                (int) std::ceil((endTime - startTime) / sta::secsToDays(tickInterval)));
        }

        for (int i = 0; i < lastTick; i++)
        {
            GroundTrackTick& tick = segment->ticks[i];
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

        painter.setPen(segment->color);
        painter.drawLines(m_trackPoints);
    }
}


void
GroundTrack::draw(QPainter& painter,
                  const StaBody* body,
                  double mjd,
                  const Transform3f& projection,
                  const AlignedBox<float, 3>& clipBox)
{
    // Calculate the current latitude and longitude of the vehicle
    StateVector v;
    double longNow = 0.0;
    double latNow  = 0.0;
    double altNow  = 0.0;
    bool activeNow = false;

    bool showEntireTrack = false;

    float west = clipBox.min().x();
    float east = clipBox.max().x();
    float north = clipBox.max().y();
    float south = clipBox.min().y();
    float maxHeight = clipBox.max().z();

    if (vehicle->getStateVector(mjd, *body, sta::CoordinateSystem(sta::COORDSYS_BODYFIXED), &v))
    {
        planetographicCoords(v.position, body, &longNow, &latNow, &altNow);
        if (longNow < west)
            longNow += 360.0;
        activeNow = true;
    }

    foreach (GroundTrackSegment* segment, segments)
    {
        // Calculate the ground track
        m_trackPoints.clear();

        QPen pen = painter.pen();
        pen.setColor(segment->color);
        painter.setPen(pen);

        bool trackVisible = false;
        if (segment->samples.empty() || (mjd <= segment->samples[0].mjd && !showEntireTrack))
        {
            trackVisible = true;
        }

        double endTime = std::numeric_limits<double>::infinity();
        if (!showEntireTrack)
        {
            endTime = mjd;
        }

        bool complete = trackVisible;

        // Draw the ground track
        for (int i = 1; i < segment->samples.size() && !complete; i++)
        {
            double long0 = segment->samples[i - 1].longitude;
            double lat0  = segment->samples[i - 1].latitude;
            double long1 = segment->samples[i].longitude;
            double lat1  = segment->samples[i].latitude;

            // Only draw the track up to the specified endTime
            if (segment->samples[i].mjd >= endTime)
            {
                long1 = longNow;
                lat1  = latNow;
                complete = true;
            }

            ClippedWrappedLine(clipBox, (float) long0, (float) lat0, (float) long1, (float) lat1, m_trackPoints);
        }

        painter.setPen(segment->color);
        painter.drawLines(m_trackPoints);
        m_trackPoints.clear();
        complete = trackVisible;

        // Draw the track through space
        for (int i = 1; i < segment->samples.size() && !complete; i++)
        {
            float long0 = (float) segment->samples[i - 1].longitude;
            float lat0  = (float) segment->samples[i - 1].latitude;
            float alt0  = (float) segment->samples[i - 1].altitude;
            float long1 = (float) segment->samples[i].longitude;
            float lat1  = (float) segment->samples[i].latitude;
            float alt1  = (float) segment->samples[i].altitude;

            // Only draw the track up to the specified endTime
            if (segment->samples[i].mjd >= endTime)
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
        painter.setPen(segment->color);
        painter.drawLines(m_trackPoints);
    }
}


void
GroundTrack::drawDropLines(QPainter& painter,
                           const StaBody* body,
                           double mjd,
                           float tickInterval,
                           const Eigen::Transform3f& projection,
                           const Eigen::AlignedBox<float, 3>& clipBox)
{
    bool showEntireTrack = false;

    float west = clipBox.min().x();
    float maxHeight = clipBox.max().z();

    double endTime = std::numeric_limits<double>::infinity();
    if (!showEntireTrack)
    {
        endTime = mjd;
    }

    foreach (GroundTrackSegment* segment, segments)
    {
        // Calculate the ground track
        m_trackPoints.clear();

        QPen pen = painter.pen();
        pen.setColor(segment->color);
        painter.setPen(pen);

        double startTime = 0.0;
        if (segment->samples.size() > 0)
            startTime = segment->samples.first().mjd;
        int lastTick = segment->ticks.size();
        if (!showEntireTrack)
        {
            lastTick = std::min(lastTick,
                                (int) std::ceil((endTime - startTime) / sta::secsToDays(tickInterval)));
        }

        for (int i = 0; i < lastTick; i++)
        {
            GroundTrackTick& tick = segment->ticks[i];
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

        painter.setPen(segment->color);
        painter.drawLines(m_trackPoints);
    }
}
