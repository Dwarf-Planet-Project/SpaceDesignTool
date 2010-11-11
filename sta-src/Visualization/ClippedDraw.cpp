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

#include "ClippedDraw.h"

using namespace Eigen;


// The following functions are used by the ground track view to draw geometry
// that is clipped to the edge of the view.

// A note on clipping:
// Lines in the 2.5D view must be clipped in software, because Qt has no way to clip
// in three dimensions. (Not even all OpenGL drivers/hardware actually support user
// clip planes, which would be necessary in order to avoid having to clip in software.)
// It is possible to clip lines drawn on the ground plane (z = 0) using a 2D clip region
// set to the projected outline of the ground rectangle. However, the performance
// of this option needs to be investigated. If Qt implements clipping by using OpenGL's
// stencil buffer, performance should be very good. But, if clipping in software is
// necessary, the general algorithm required by Qt would be considerably slower than
// the special case of clipping to an axis-aligned view box, which is what is implemented
// here.


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


/** Draw a line clipped in 3D to the specified box and append the resulting
  * points to the specified buffer.
  *
  * \param points the output points buffer.
  */
void
ClippedLine(const AlignedBox<float, 3>& clipBox,
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


/** Clip a line and 3D and emit the endpoints into the specified vector
  *  (unless the line was clipped completely.) This function is required
  * the 2.5D view: since the Qt canvas is strictly 2D, we need to take
  * care of doing our own line clipping in three dimensions. If we switched
  * to using OpenGL for drawing, we could use GL's clip planes for this
  * task, but those aren't supported by all drivers.
  */
void ClippedLine(const AlignedBox<float, 3>& clipBox,
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



/** Utility function for easier line drawing. This handles the complexities of
  * clipping and wrapping. From zero to two line segments will be emitted into
  * the points vector:
  *  * 0 if the line was completely clipped
  *  * 1 if the line was partially clipped or not clipped at all
  *  * 2 if the line was clipped at the east or west edge of a wide view
  */
void
ClippedWrappedLine(const AlignedBox<float, 3>& clipBox,
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
            ClippedLine(clipBox, (float) long0, (float) lat0, (float) long1 + 360, (float) lat1, points);
            ClippedLine(clipBox, (float) long0 - 360, (float) lat0, (float) long1, (float) lat1, points);
        }
        else
        {
            ClippedLine(clipBox, (float) long1, (float) lat1, (float) long0 + 360, (float) lat0, points);
            ClippedLine(clipBox, (float) long1 - 360, (float) lat1, (float) long0, (float) lat0, points);
        }
    }
    else
    {
        ClippedLine(clipBox, (float) long0, (float) lat0, (float) long1, (float) lat1, points);
    }
}
