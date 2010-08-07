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
 ------------------ Guillermo Ortega (ESA) ---------------------------------------
 Patched by Guillermo on June 2010 to include the code of Claas and Stephen
 Patched by Guillermo August 2010 to add the equqtor line
 */

#include "Plotting/groundtrackplottool.h"
#include "Plotting/visualizationtoolbar.h"
#include "Main/propagatedscenario.h"
#include "Astro-Core/rectangularTOpolar.h"
#include "Astro-Core/stamath.h"
#include "Astro-Core/date.h"
#include "Scenario/scenario.h"
//#include "Scenario/scenariospacevehicle.h"
//#include "Scenario/scenariotrajectoryplan.h"
//#include "Scenario/scenarioabstracttrajectory.h"
#include "Constellations/constellationmodule.h"  // Analysis (Claas Grohnfeldt, Steffen Peter)
#include "Main/mainwindow.h"

#include <QTextStream>
#include <QtGui>
#include <QGLWidget>
#include <QDebug>
#include <Eigen/Geometry>
#include <cmath>
#include <limits>
#include <algorithm>

using namespace Eigen;

// The ground track view is a 2D or pseudo-3D projection of a planet that may panned
// and zoomed by the user.
//
// The coordinate system is set up so that most things can be drawn by specifying
// coordinates in degrees, with west left and north at the top of the view.
//
// The viewport is determined by two things:
//    m_center     - the coordinate at the center of the viewport
//    m_zoomFactor - the magnification level. At 1x the viewport will
//                   span 360 degrees in longitude, 180 degrees in
//                   latitude. At 2x, 180 and 90 degrees, etc.
//
// A note on wrapping:
// Special care must be taken when the viewport straddles the 180 degree meridian.
// The view center is always mapped to the interval [ 0, 360 ). At minimum zoom,
// the left edge can reach -180, and the right edge can approach 540 degrees. Before
// drawing, coordinates are remapped from [-180, 180] into the viewport range.
// Any point with a longitude smaller less than the west edge of the viewport is
// converted to ( longitude + 360, latitude ). Lines that lie completely within
// the viewport after mapping can be drawn in the straightforward way. Lines that
// overlap the east or wedge of the viewport may need to be drawn as _two_ line
// segments when the view is sufficiently wide.
//
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

// Possible spacings of longitude and latitude lines, in arc seconds:
static double LongLatSpacings[] =
{
    // degrees
    60 * 3600,
    45 * 3600,
    30 * 3600,
    15 * 3600,
    10 * 3600,
    5 * 3600,
    3 * 3600,
    2 * 3600,
    1 * 3600,

    // arcmin
    30 * 60,
    20 * 60,
    10 * 60,
    5 * 60,
    3 * 60,
    2 * 60,
    1 * 60,

    // arcsec
    30,
    20,
    10,
    5,
    3,
    2,
    1,
};


// Function to choose a nice value for grid spacings.
static double chooseGridSpacing(double range, double maxTicks)
{
    double idealSpacing = range / maxTicks;
    double exponent = floor(log10(idealSpacing));
    double p = pow(10.0, exponent);
    double mantissa = idealSpacing / p;

    if (mantissa < 2.0)
    {
        return 2.0 * p;
    }
    else if (mantissa < 3.0)
    {
        return 3.0 * p;
    }
    else if (mantissa < 5.0)
    {
        return 5.0 * p;
    }
    else
    {
        return 10.0 * p;
    }
}


// For now, use a constant minimum elevation angle for all ground stations
// Guillermo patch June 2010 to make it 5 degrees. TODO: make it generic
static const double GroundStationElevationAngle = 5.0;
static const QColor GroundStationColor(255, 64, 20);


GroundTrackView::GroundTrackView(QWidget* parent) :
	QGraphicsView(parent),
	m_scenario(NULL),
	m_currentTime(0.0),
	m_pixmap(NULL),
	m_pixmapOk(false),
	m_contextMenu(NULL),
	m_showEntireTracks(true),
	m_showGrid(false),
	m_showEquator(false),
	m_showTicks(false),
	m_showTerminator(false),
	m_tickInterval(300.0), // seconds
	m_body(NULL),
	m_zoomFactor(1.0f),
	m_center(0.0f, 0.0f),
	m_maxHeight(180.0f),
	m_lastMousePosition(0.0f, 0.0f),
	m_projection(Planar),
	m_scene(NULL),
	m_maxHeightSlider(NULL),
	m_showAnalysis(true), // Analysis (Claas Grohnfeldt, Steffen Peter)
	m_showDiscretization(false), // Analysis
	m_showCoverageCurrent(false), // Analysis
	m_showCoverageHistory(false), // Analysis
	m_showSOLink(false), // Analysis
	m_showGOLink(false) // Analysis
{
    setBody(STA_SOLAR_SYSTEM->lookup(STA_EARTH));

    QGLFormat glformat;
    QVBoxLayout wformat;
    glformat.setSampleBuffers(false);
    glformat.setSamples(4);
    glformat.setDepth(true);
    glformat.setDirectRendering(true);
    glformat.setDoubleBuffer(true);
    glformat.setAlpha(true);
    glformat.setDepthBufferSize(4);
    glformat.setOverlay(false);
    //setViewport(new QGLWidget(glformat, this));

    constrainCenter();

    m_scene = new QGraphicsScene();
    m_maxHeightSlider = new QSlider();
    // Next line hiden by Guillermo Nov 09 to allow natural OS widget
    //m_maxHeightSlider->setStyle(new QPlastiqueStyle());
    m_maxHeightSlider->move(10, 0);
    m_maxHeightSlider->setRange(0, 1000);
    m_maxHeightSlider->setTickPosition(QSlider::TicksBothSides);
    m_maxHeightSlider->setTickInterval(100);
    QGraphicsProxyWidget* sliderProxy = m_scene->addWidget(m_maxHeightSlider);
    QPalette palette;
    palette.setColor(m_maxHeightSlider->backgroundRole(), Qt::white);
    m_maxHeightSlider->setPalette(palette);
    m_maxHeightSlider->hide();
    connect(m_maxHeightSlider, SIGNAL(valueChanged(int)), this, SLOT(setMaxHeight(int)));
    m_maxHeightSlider->setValue(500);
    setScene(m_scene);
    setAlignment(Qt::AlignLeft | Qt::AlignTop);

    // Changing the color of the background. See http://www.colorcombos.com/combotester.html
    QColor myLightGrayColor(241, 241, 241, 255);
    QPalette pal = this->palette();
    //pal.setColor(this->backgroundRole(), myLightGrayColor);
    pal.setColor(this->backgroundRole(), Qt::transparent);
    this->setPalette(pal);

}

GroundTrackView::~GroundTrackView()
{
    setScenario(NULL);
}


void GroundTrackView::setScenario(PropagatedScenario* scenario)
{
    foreach (GroundTrack* path, m_groundTrackList)
    {
        delete path;
    }

    m_groundTrackList.clear();

    m_scenario = scenario;
    if (m_scenario != NULL)
    {
        foreach (SpaceObject* spaceObject, scenario->spaceObjects())
        {
            addGroundTrack(spaceObject);
        }
    }

    viewport()->update();
}


bool GroundTrackView::addGroundTrack(SpaceObject* vehicle)
{
    GroundTrack* track = new GroundTrack();
    track->color = vehicle->trajectoryColor();
    track->vehicle = vehicle;

    // Skip empty ground tracks
    computeGroundTrack(*track);
    if (track->samples.size() == 0)
    {
        delete track;
        return false;
    }

    m_groundTrackList.append(track);

    return true;
}



void GroundTrackView::computeGroundTrack(GroundTrack& track)
{
    track.samples.clear();

    foreach (MissionArc* arc, track.vehicle->mission())
    {
        for (int i = 0; i < arc->trajectorySampleCount(); ++i)
        {
            double mjd = arc->trajectorySampleTime(i);
            sta::StateVector v;

            if (track.vehicle->getStateVector(mjd, *m_body, sta::CoordinateSystem(sta::COORDSYS_BODYFIXED), &v))
            {
                GroundTrackSample sample;
                sample.mjd = mjd;
                planetographicCoords(v.position, m_body, &sample.longitude, &sample.latitude, &sample.altitude);

                track.samples << sample;
            }
        }
    }

    computeTicks(track, sta::secsToDays(m_tickInterval));
}


// Calculate ticks at regular time intervals on the ground track
void GroundTrackView::computeTicks(GroundTrack& track, double interval)
{
    track.ticks.clear();

    foreach (MissionArc* arc, track.vehicle->mission())
    {
        int nSamples = arc->trajectorySampleCount();
        if (nSamples > 1)
        {
            double startTime = arc->trajectorySampleTime(0);
            double endTime = arc->trajectorySampleTime(nSamples - 1);

            for (double mjd = startTime; mjd < endTime; mjd += interval)
            {
                // We want to draw the ticks so that they're perpendicular to the
                // ground track. We'll use simple differencing to compute the projected
                // direction.
                double t0 = mjd;
                double t1 = t0 + interval * 0.01;

                if (t1 < endTime)
                {
                    sta::StateVector v0;
                    sta::StateVector v1;
                    track.vehicle->getStateVector(t0, *m_body, sta::CoordinateSystem(sta::COORDSYS_BODYFIXED), &v0);
                    track.vehicle->getStateVector(t1, *m_body, sta::CoordinateSystem(sta::COORDSYS_BODYFIXED), &v1);
                    double long0, lat0, alt0;
                    double long1, lat1, alt1;
                    planetographicCoords(v0.position, m_body, &long0, &lat0, &alt0);
                    planetographicCoords(v1.position, m_body, &long1, &lat1, &alt1);

                    double diffLong = long1 - long0;
                    double diffLat = lat1 - lat0;
                    double l = sqrt(diffLong * diffLong + diffLat * diffLat);
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
                    tick.longitude = (float) long0;
                    tick.latitude = (float) lat0;
                    tick.altitude = (float) alt0;
                    tick.dx = (float) -diffLat;
                    tick.dy = (float) diffLong;

                    track.ticks << tick;
                }
            }
        }
    }
}


void GroundTrackView::setCurrentTime(double mjd)
{
    if (mjd != m_currentTime)
    {
        m_currentTime = mjd;
        viewport()->update();
    }
}


void GroundTrackView::setShowEntireTracks(bool show)
{
    m_showEntireTracks = show;
    viewport()->update();
}


void GroundTrackView::setShowGrid(bool show)
{
    m_showGrid = show;
    viewport()->update();
}


void GroundTrackView::setShowEquator(bool show)
{
    m_showEquator = show;
    viewport()->update();
}


void GroundTrackView::setShowTicks(bool show)
{
    m_showTicks = show;
    viewport()->update();
}


/*! Set the interval between ticks on all ground tracks (in units
 *  of seconds.) Setting the tick interval to zero will disable
 *  ticks.
 */
void GroundTrackView::setTickInterval(double seconds)
{
    if (seconds == 0.0)
    {
        setShowTicks(false);
    }
    else
    {
        setShowTicks(true);
        m_tickInterval = seconds;
        foreach (GroundTrack* track, m_groundTrackList)
        {
            computeTicks(*track, sta::secsToDays(m_tickInterval));
        }
    }

    viewport()->update();
}


void GroundTrackView::setBody(const StaBody* newBody)
{
    if (newBody && newBody != m_body)
    {
        m_body = newBody;

        // Clean up the old map.
        if (m_pixmap)
        {
            delete m_pixmap;
            m_pixmap = NULL;
        }

        m_pixmap = new QPixmap();
        if (m_pixmap->load(m_body->baseTexture()))
        {
            m_pixmapOk = true;
        }
        else
        {
            delete m_pixmap;
            m_pixmap = NULL;
            m_pixmapOk = false;
        }

        // Recompute all ground tracks
        foreach (GroundTrack* track, m_groundTrackList)
        {
            computeGroundTrack(*track);
        }

        viewport()->update();
    }
}


void GroundTrackView::setTerminatorVisible(bool visible)
{
    m_showTerminator = visible;
    viewport()->update();
}


void GroundTrackView::setProjection(ProjectionType projection)
{
    m_projection = projection;
    m_maxHeightSlider->setVisible(projection != Planar);
    viewport()->update();
}


void GroundTrackView::set2HalfDView(bool enabled)
{
    if (enabled)
        setProjection(Oblique);
    else
        setProjection(Planar);
}


void GroundTrackView::setMaxHeight(int logMaxHeight)
{
    m_maxHeight = (float) exp(log(30000.0) * (logMaxHeight / 1000.0));
    viewport()->update();
}


void GroundTrackView::saveImage()
{
    // The following code relies on the fact that the viewport is an OpenGL widget. Another
    // approach would be to create an image and draw using the painter instance for that
    // image.
    QGLWidget* glViewport = qobject_cast<QGLWidget*>(viewport());
    if (!glViewport)
    {
        QMessageBox::warning(this, tr("Save error"), tr("STA internal error when saving image."));
    }
    else
    {
        QImage image = glViewport->grabFrameBuffer();
        QString fileName = QFileDialog::getSaveFileName(this,
                                                        tr("Save Image of Plot"),
                                                        "",
                                                        tr("Images (*.png *.jpg *.tif)"));
        if (!fileName.isEmpty())
        {
            bool ok = image.save(fileName);
            if (!ok)
            {
                QMessageBox::warning(this, tr("Save error"), tr("Error saving image to %1").arg(fileName));
            }
        }
    }
}


// Private method to compute the planetographic latitude and longitude of
// a point in the body fixed frame of the current planet. Longitude and
// latitude are in degrees, altitude is in kilometers.
void
	GroundTrackView::planetographicCoords(const Vector3d& position,
					      const StaBody* body,
					      double* longitude,
					      double* latitude,
					      double* altitude) const
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


// Reduce an angle in degrees to [-180, 180]
double reduceLongitude(double degrees)
{
    if (degrees < 0.0)
        return std::fmod(degrees - 180.0, 360.0) + 180.0;
    else
        return std::fmod(degrees + 180.0, 360.0) - 180.0;
}


void GroundTrackView::setCenter(QPointF center)
{
    m_center = center;
    constrainCenter();
}


void GroundTrackView::setZoomFactor(float zoomFactor)
{
    m_zoomFactor = zoomFactor;
    constrainCenter();
}


// Clamp the center so that the map doesn't move out of view
void GroundTrackView::constrainCenter()
{
    float x = m_center.x();
    float y = m_center.y();
    float halfWidth = 180.0f / m_zoomFactor;
    float halfHeight = 90.0f / m_zoomFactor;

    x = reduceLongitude(x);
    if (x - halfWidth < -180.0f)
        x += 360.0f;

    if (y + halfHeight > 90.0f)
        y = 90.0f - halfHeight;
    if (y - halfHeight < -90.0f)
        y = -90.0f + halfHeight;

    m_center = QPointF(x, y);

    float invZoom = 1.0f / m_zoomFactor;
    m_east = m_center.x() + 180.0f * invZoom;
    m_west = m_center.x() - 180.0f * invZoom;
    m_north = -m_center.y() + 90.0f * invZoom;
    m_south = -m_center.y() - 90.0f * invZoom;
}



// Mouse wheel will zoom in and out
void GroundTrackView::wheelEvent(QWheelEvent* event)
{
    if (event->orientation() == Qt::Vertical)
    {
        // Delta in steps of 1/8 of degree; typical mouse rotation is 15 degrees per wheel click
        float zoomPerClick = 1.2f;
        float clicks = (float) event->delta() / 120.0f;
        float zoom = std::pow(zoomPerClick, clicks);

        setZoomFactor(std::max(1.0f, std::min(100.0f, m_zoomFactor * zoom)));

        viewport()->update();
    }
}


void GroundTrackView::mouseMoveEvent(QMouseEvent* event)
{
    if (itemAt(event->pos()))
    {
        QGraphicsView::mouseMoveEvent(event);
        if (event->isAccepted())
            return;
    }

    if (event->buttons() & Qt::LeftButton)
    {
        QPointF position = event->posF();
        QPointF delta = position - m_lastMousePosition;        
        m_lastMousePosition = position;

        int viewWidth = viewport()->size().width();
        float scale = 1.0f / (float) viewWidth / m_zoomFactor;

        delta = QPointF(delta.x() * scale * 360.0f, delta.y() * scale * 360.0f);
        setCenter(center() - delta);

        viewport()->update();
    }
    else
    {
        event->ignore();
    }
}


void GroundTrackView::mousePressEvent(QMouseEvent* event)
{
    if (itemAt(event->pos()))
    {
        QGraphicsView::mousePressEvent(event);
        if (event->isAccepted())
            return;
    }

    if (event->button() == Qt::LeftButton)
    {
        m_lastMousePosition = event->posF();
    }
    else
    {
        event->ignore();
    }
}


void GroundTrackView::resizeEvent(QResizeEvent* event)
{
}


void GroundTrackView::drawBackground(QPainter* painter, const QRectF& rect)
{
    QTransform savedTransform(painter->worldTransform());
    painter->setWorldTransform(QTransform());

    if (m_projection == Oblique)
    {
        paintObliqueView(*painter);
    }
    else
    {
        paint2DView(*painter);
    }

    // Restore the original transform
    painter->setWorldTransform(savedTransform);
}


// Draw a line in the oblique (2.5D) view. No clipping or wrapping is performed.
static void drawLine(QPainter& painter, const Transform3f& transform, const Vector3f& p0, const Vector3f& p1)
{
    Vector3f q0 = transform * p0;
    Vector3f q1 = transform * p1;
    painter.drawLine(QPointF(q0.x(), q0.y()), QPointF(q1.x(), q1.y()));
}


// Draw a quadrilaterial in the oblique (2.5D) view. No clipping or wrapping is performed.
static void drawQuad(QPainter& painter, const Transform3f& transform,
                     const Vector3f& p0, const Vector3f& p1, const Vector3f& p2, const Vector3f& p3)
{
    Vector3f q0 = transform * p0;
    Vector3f q1 = transform * p1;
    Vector3f q2 = transform * p2;
    Vector3f q3 = transform * p3;

    QPointF points[4] =
    {
        QPointF(q0.x(), q0.y()),
        QPointF(q1.x(), q1.y()),
        QPointF(q2.x(), q2.y()),
        QPointF(q3.x(), q3.y()),
    };

    painter.drawConvexPolygon(points, 4);
}

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
static void clippedWrappedLine(const AlignedBox<float, 3>& clipBox,
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


bool
	GroundTrackView::inView(float longitude, float latitude, float altitude)
{
    if (latitude >= m_south && latitude <= m_north)
    {
        // TODO: add logic to handle wrapping in longitude
        if (longitude >= m_west && longitude <= m_east)
        {
            return true;
        }
    }

    return false;
}


void GroundTrackView::paintObliqueView(QPainter& painter)
{
    //QPainter painter(viewport());
    int viewWidth = viewport()->size().width();
    int viewHeight = viewport()->size().height();

    // Next two lines patched by Guillermo
    // Allowing natural OS font rendering on 2.5 View, specially for windows
    //QFont font("Helvetica", 12);
    //painter.setFont(font);

    painter.setRenderHints(QPainter::SmoothPixmapTransform);
    painter.setRenderHints(QPainter::Antialiasing);

    QRectF destRect;

    float leftInset   = 50.0f;
    float rightInset  = 10.0f;
    float bottomInset = 10.0f;
    float topInset    = 10.0f;
    float usableWidth = viewWidth - leftInset - rightInset;
    float usableHeight = viewHeight - bottomInset - topInset;
    float projectionAngle = sta::degToRad(40.0f);
    float sinProjAngle = std::sin(projectionAngle);
    float cosProjAngle = std::cos(projectionAngle);

    float shearFactor = cosProjAngle / sinProjAngle;
    float destRectWidth = usableWidth / (1.0f + cosProjAngle * 0.5f);
    float destRectHeight = destRectWidth * sinProjAngle * 0.5f;
    float xoffset = leftInset + destRectWidth * (0.5f * cosProjAngle);
    float yoffset = viewHeight - bottomInset - destRectHeight;

    QTransform xform;
    xform.translate(xoffset, yoffset);
    xform.shear(-shearFactor, 0.0f);
    xform.scale(destRectWidth / 360.0f, destRectHeight / 180.0f);
    painter.setWorldTransform(xform);

    if (m_pixmap)
    {
        int imageWidth = m_pixmap->size().width();
        int imageHeight = m_pixmap->size().height();
        float imageAspectRatio = (float) imageWidth / (float) imageHeight;
        float widthRatio = (float) viewWidth / (float) imageWidth;
        float heightRatio = (float) viewHeight / (float) imageHeight;
        if (heightRatio > widthRatio)
        {
            float height = viewWidth / imageAspectRatio;
            destRect = QRectF(0.0f, (viewHeight - height) / 2.0f, viewWidth, height);
        }
        else
        {
            float width = viewHeight * imageAspectRatio;
            destRect = QRectF((viewWidth - width) / 2.0f, 0.0f, width, viewHeight);
        }

        destRect = QRectF(0.0f, 0.0f, 360.0f, 180.0f);

        float srcWidth = (float) imageWidth / m_zoomFactor;
        float srcHeight = (float) imageHeight / m_zoomFactor;
        float left = (m_center.x() / 360.0f + 0.5f) * imageWidth - srcWidth / 2.0f;
        float top = (m_center.y() / 180.0f + 0.5f) * imageHeight - srcHeight / 2.0f;
        painter.drawPixmap(destRect,
                           *m_pixmap,
                           QRectF(left, top, srcWidth, srcHeight));

        // Handle wrapping when the viewport straddles the 180 degree meridian
        if (m_east > 180.0f)
        {
            float wrapFraction = (m_east - 180.0f) / 360.0f;
            float srcWrapWidth = imageWidth * wrapFraction;
            float destWrapWidth = destRect.width() * wrapFraction * m_zoomFactor;
            float destWrapLeft = destRect.width() - destWrapWidth;
            painter.drawPixmap(QRectF(destWrapLeft, destRect.top(), destWrapWidth, destRect.height()),
                               *m_pixmap,
                               QRectF(0.0f, top, srcWrapWidth, srcHeight));
        }
    }

    xform.reset();
    xform.translate(xoffset, yoffset);
    xform.shear(-shearFactor, 0.0f);
    xform.scale(1.0f, -1.0f);
    xform.scale(destRectWidth / 360.0f, destRectHeight / 180.0f);
    xform.scale(m_zoomFactor, m_zoomFactor);
    xform.translate((180.0f / m_zoomFactor - m_center.x()), (90.0f / m_zoomFactor - m_center.y()) * -1.0f);
    painter.setWorldTransform(xform);

    AlignedBox<float, 3> clipBox;
    clipBox.min() = Vector3f(m_west, m_south, 0.0f);
    clipBox.max() = Vector3f(m_east, m_north, m_maxHeight);

    // Set up the transformation that will map from 3D coordinates to 2D
    float zscale = (180.0f / destRectHeight) / m_maxHeight * (usableHeight - destRectHeight);
    float zshear = xform.m21() / xform.m11() * zscale;
    Transform3f proj;
    proj.matrix() << 1.0f, 0.0f, -zshear / m_zoomFactor, 0.0f,
    0.0f, 1.0f, zscale / m_zoomFactor,    0.0f,
    0.0f, 0.0f, 1.0f, 0.0f,
    0.0f, 0.0f, 0.0f, 1.0f;

    // Draw the left and back sides of a box
    painter.setBrush(QColor(200, 200, 200));
    drawQuad(painter, proj,
             Vector3f(m_west, m_north, 0.0f), Vector3f(m_east, m_north, 0.0f),
             Vector3f(m_east, m_north, m_maxHeight), Vector3f(m_west, m_north, m_maxHeight));
    drawQuad(painter, proj,
             Vector3f(m_west, m_south, 0.0f), Vector3f(m_west, m_north, 0.0f),
             Vector3f(m_west, m_north, m_maxHeight), Vector3f(m_west, m_south, m_maxHeight));

    // Add altitude lines
    const double maxAltLines = 10.0;
    double altLineSpacing = chooseGridSpacing(m_maxHeight, maxAltLines);
    double altitude = 0.0;

    for (double altitude = 0; altitude < m_maxHeight + altLineSpacing; altitude += altLineSpacing)
    {
        painter.setPen(QColor(50, 50, 50));

        float z = std::min((float) altitude, m_maxHeight);
        Vector3f p0 = proj * Vector3f(m_west, m_north, z);
        Vector3f p1 = proj * Vector3f(m_east, m_north, z);
        Vector3f p2 = proj * Vector3f(m_west, m_south, z);
        painter.drawLine(QPointF(p0.x(), p0.y()), QPointF(p1.x(), p1.y()));
        painter.drawLine(QPointF(p0.x(), p0.y()), QPointF(p2.x(), p2.y()));

        QTransform xform = painter.transform();
        painter.setTransform(QTransform());
        QPointF textPosition = QPointF(p2.x(), p2.y()) * xform;
        if (z < m_maxHeight)
        {
            painter.drawText(QRectF(textPosition.x() - 50, textPosition.y() - 10, 45, 20),
                             Qt::AlignRight | Qt::AlignVCenter,
                             // Next line patched by Guillermo to take away the units KM
                             //QString("%1 km").arg(z));
                             QString("%1").arg(z));
        }
        painter.setTransform(xform);
    }

    // Draw the corners of the box
    drawLine(painter, proj, Vector3f(m_west, m_north, 0.0f), Vector3f(m_west, m_north, m_maxHeight));
    drawLine(painter, proj, Vector3f(m_east, m_north, 0.0f), Vector3f(m_east, m_north, m_maxHeight));
    drawLine(painter, proj, Vector3f(m_west, m_south, 0.0f), Vector3f(m_west, m_south, m_maxHeight));

    if (m_showTerminator)
    {
        // Shade the night side of the planet first so that brightness of ground
        // tracks, labels, etc. are not affected.
        drawDaylitRegion(painter);
    }

    // Draw the longitude / latitude grid
    if (m_showGrid)
    {
        double maxLongitudeLines = 10.0;
        double longitudeSpacing = meridianSpacing(maxLongitudeLines);
        double latitudeSpacing = longitudeSpacing; // identical to meridian spacing (for now)

        QColor longLatColor(200, 200, 0, 100);
        QColor longLatColorOpaque(200, 200, 0);
        QColor boxLineColor(50, 50, 50, 100);

        double longitudeSpacingDeg = longitudeSpacing / 3600.0;
        double latitudeSpacingDeg = latitudeSpacing / 3600.0;
        float startLongitude = (float) (floor(m_west / longitudeSpacingDeg) + 1.0f) * longitudeSpacingDeg;
        float endLatitude = (float) (floor(m_north / latitudeSpacingDeg) + 1.0f) * latitudeSpacingDeg;
        float startLatitude = (float) (floor(m_south / latitudeSpacingDeg) + 1.0f) * latitudeSpacingDeg;

        // Draw lines of longitude
        for (float l = startLongitude; l < m_east; l += (float) longitudeSpacing / 3600.0f)
        {
            painter.setPen(longLatColor);
            drawLine(painter, proj, Vector3f(l, m_north, 0.0f), Vector3f(l, m_south, 0.0f));
            painter.setPen(boxLineColor);
            drawLine(painter, proj, Vector3f(l, m_north, 0.0f), Vector3f(l, m_north, m_maxHeight));
        }

        // Draw lines of latitude
        for (float l = startLatitude; l < m_north; l += (float) latitudeSpacing / 3600.0f)
        {
            painter.setPen(longLatColor);
            drawLine(painter, proj, Vector3f(m_east, l, 0.0f), Vector3f(m_west, l, 0.0f));
            painter.setPen(boxLineColor);
            drawLine(painter, proj, Vector3f(m_west, l, 0.0f), Vector3f(m_west, l, m_maxHeight));
        }
    }

    // Draw the Equator grid
    if (m_showEquator)
    {

	double maxLongitudeLines = 10.0;
	double longitudeSpacing = meridianSpacing(maxLongitudeLines);
	double latitudeSpacing = longitudeSpacing; // identical to meridian spacing (for now)

	QColor boxLineColor(50, 50, 50, 100);
	QColor equatorColor(220, 220, 0, 100);
	QColor equatorColorOpaque = equatorColor;
	equatorColorOpaque.setAlpha(255);
	QPen equatorPen(equatorColor);
	painter.setPen(equatorPen);

	double longitudeSpacingDeg = longitudeSpacing / 3600.0;
	double latitudeSpacingDeg = latitudeSpacing / 3600.0;
	float startLongitude = (float) (floor(m_west / longitudeSpacingDeg) + 1.0f) * longitudeSpacingDeg;
	float endLatitude = 0.0f;
	float startLatitude = 0.0f;

	float l = 0.0f;
	drawLine(painter, proj, Vector3f(m_east, l, 0.0f), Vector3f(m_west, l, 0.0f));
	painter.setPen(boxLineColor);
	drawLine(painter, proj, Vector3f(m_west, l, 0.0f), Vector3f(m_west, l, m_maxHeight));
    }

    float indicatorSize = 3.0f / m_zoomFactor;
    float labelOffset = 2.0f / m_zoomFactor;

    if (m_showTerminator)
    {
        drawSubsolarPoint(painter, Qt::white, indicatorSize);
    }

    // TODO: This should be set by the user. But for now, we just use the first
    // active spacecraft.
    const SpaceObject* selectedSpacecraft = NULL;
    float selectedSpacecraftAltitude = 0.0;
    foreach (GroundTrack* track, m_groundTrackList)
    {
        sta::StateVector v;
        if (track->vehicle->getStateVector(m_currentTime, *m_body, sta::CoordinateSystem(sta::COORDSYS_BODYFIXED), &v))
        {
            double longNow = 0.0;
            double latNow  = 0.0;
            double altNow  = 0.0;
            planetographicCoords(v.position, m_body, &longNow, &latNow, &altNow);
            selectedSpacecraft = track->vehicle;
            selectedSpacecraftAltitude = altNow;
            break;
        }
    }

    // Draw the ground elements
    if (m_scenario)
    {
        painter.setBrush(QBrush(Qt::NoBrush));
        foreach (GroundObject* groundObject, m_scenario->groundObjects())
        {
            // Only draw ground elements for the current body
            if (groundObject->centralBody == m_body)
            {
                double lon = groundObject->longitude;
                double lat = groundObject->latitude;
                if (lon < m_west)
                    lon += 360.0;

                if (inView(lon, lat, 0.0f))
                {
                    painter.setPen(GroundStationColor);
                    QPointF center((float) lon, (float) lat);
                    painter.drawPoint(center);
                    painter.drawEllipse(center, indicatorSize, indicatorSize);

                    // We don't want the transformation to scale the text. So, we temporarily
                    // reset the transformation to an identity matrix, draw the text, then restore
                    // the matrix.
                    QTransform xform = painter.worldTransform();
                    painter.setWorldTransform(QTransform());
                    QPointF textOrigin = QPointF((float) lon + labelOffset, (float) lat + labelOffset) * xform;
                    painter.drawText(textOrigin, groundObject->name);       // Guillermo says: this is the name of the ground object
                    painter.setWorldTransform(xform);
                }

                if (selectedSpacecraft)
                {
                    // TODO: Make this a property of the ground station
                    double minElevationAngle = sta::degToRad(GroundStationElevationAngle);
                    //painter.setPen(selectedSpacecraft->trajectoryColor());
                    painter.setPen(GroundStationColor);
                    drawCoverageFootprint(painter,
                                          lon, lat, m_body,
                                          minElevationAngle,
                                          selectedSpacecraftAltitude);
                }
            }
        }
    }

    // Draw each ground track
    foreach (GroundTrack* track, m_groundTrackList)
    {
        // Calculate the current latitude and longitude of the vehicle
        sta::StateVector v;
        double longNow = 0.0;
        double latNow  = 0.0;
        double altNow  = 0.0;
        bool activeNow = false;
        if (track->vehicle->getStateVector(m_currentTime, *m_body, sta::CoordinateSystem(sta::COORDSYS_BODYFIXED), &v))
        {
            planetographicCoords(v.position, m_body, &longNow, &latNow, &altNow);
            if (longNow < m_west)
                longNow += 360.0;
            activeNow = true;
        }

        // Calculate the ground track
        m_trackPoints.clear();

        bool trackVisible = false;
        if (track->samples.empty() || (m_currentTime <= track->samples[0].mjd && !m_showEntireTracks))
        {
            trackVisible = true;
        }

        double endTime = std::numeric_limits<double>::infinity();
        if (!m_showEntireTracks)
        {
            endTime = m_currentTime;
        }

        bool complete = trackVisible;

        // Draw the ground track
        for (int i = 1; i < track->samples.size() && !complete; i++)
        {
            double long0 = track->samples[i - 1].longitude;
            double lat0  = track->samples[i - 1].latitude;
            double long1 = track->samples[i].longitude;
            double lat1  = track->samples[i].latitude;

            // Only draw the track up to the specified endTime
            if (track->samples[i].mjd >= endTime)
            {
                long1 = longNow;
                lat1  = latNow;
                complete = true;
            }

            clippedWrappedLine(clipBox, (float) long0, (float) lat0, (float) long1, (float) lat1, m_trackPoints);
        }

        painter.setPen(track->color);
        painter.drawLines(m_trackPoints);
        m_trackPoints.clear();
        complete = trackVisible;

        // Draw the track through space
        for (int i = 1; i < track->samples.size() && !complete; i++)
        {
            float long0 = (float) track->samples[i - 1].longitude;
            float lat0  = (float) track->samples[i - 1].latitude;
            float alt0  = (float) track->samples[i - 1].altitude;
            float long1 = (float) track->samples[i].longitude;
            float lat1  = (float) track->samples[i].latitude;
            float alt1  = (float) track->samples[i].altitude;

            // Only draw the track up to the specified endTime
            if (track->samples[i].mjd >= endTime)
            {
                long1 = (float) longNow;
                lat1  = (float) latNow;
                alt1  = (float) altNow;
                complete = true;
            }

            if (long0 < m_west)
                long0 += 360.0f;
            if (long1 < m_west)
                long1 += 360.0f;

            if (std::abs(long0 - long1) > 180.0f)
            {
                // Handle segments that wrap across the edges of the map.
                if (long0 > long1)
                {
                    clippedLine(clipBox, proj,
                                long0, lat0, alt0, long1 + 360, lat1, alt1,
                                m_trackPoints);
                    clippedLine(clipBox, proj,
                                long0 - 360, lat0, alt0, long1, lat1, alt1,
                                m_trackPoints);
                }
                else
                {
                    clippedLine(clipBox, proj,
                                long1, lat1, alt1, long0 + 360, lat0, alt0,
                                m_trackPoints);
                    clippedLine(clipBox, proj,
                                long1 - 360, lat1, alt1, long0, lat0, alt0,
                                m_trackPoints);
                }
            }
            else
            {
                clippedLine(clipBox, proj, long0, lat0, alt0, long1, lat1, alt1, m_trackPoints);
            }
        }

        QPen trackPen(track->color);
        trackPen.setWidthF(3.0f / xform.m11());
        painter.setPen(trackPen);
        painter.drawLines(m_trackPoints);
        m_trackPoints.clear();

        // Draw ticks on the ground tracks
        if (m_showTicks)
        {
            double startTime = 0.0;
            if (track->samples.size() > 0)
                startTime = track->samples.first().mjd;
            int lastTick = track->ticks.size();
            if (!m_showEntireTracks)
            {
                lastTick = std::min(lastTick,
                                    (int) std::ceil((endTime - startTime) / sta::secsToDays(m_tickInterval)));
            }

            for (int i = 0; i < lastTick; i++)
            {
                GroundTrackTick& tick = track->ticks[i];
                float tickLongitude = tick.longitude;
                if (tickLongitude < m_west)
                    tickLongitude += 360.0f;
                unsigned int outcode0 = computeOutcode(clipBox, tickLongitude, tick.latitude);

                if (outcode0 == 0 && tick.altitude > 0.0f)
                {
                    Vector3f groundPoint = proj * Vector3f(tickLongitude, tick.latitude, 0.0f);
                    Vector3f skyPoint    = proj * Vector3f(tickLongitude, tick.latitude, std::min(m_maxHeight, tick.altitude));

                    m_trackPoints << QPointF(groundPoint.x(), groundPoint.y())
			    << QPointF(skyPoint.x(), skyPoint.y());
                }
            }
        }

        painter.setPen(track->color);
        painter.drawLines(m_trackPoints);

        // Draw an indicator at the current spacecraft subpoint
        if (activeNow && inView(longNow, latNow, 0.0f))
        {
            painter.drawLine(QPointF((float) longNow - indicatorSize, (float) latNow),
                             QPointF((float) longNow + indicatorSize, (float) latNow));
            painter.drawLine(QPointF((float) longNow, (float) latNow - indicatorSize),
                             QPointF((float) longNow, (float) latNow + indicatorSize));
        }
    }

}


void GroundTrackView::paint2DView(QPainter& painter)
{
    int viewWidth = viewport()->size().width();
    int viewHeight = viewport()->size().height();

    QFont font("Helvetica", 12);
    painter.setFont(font);
    painter.setRenderHints(QPainter::SmoothPixmapTransform);

    QRectF destRect;

    if (m_pixmap)
    {
        int imageWidth = m_pixmap->size().width();
        int imageHeight = m_pixmap->size().height();
        float imageAspectRatio = (float) imageWidth / (float) imageHeight;
        float widthRatio = (float) viewWidth / (float) imageWidth;
        float heightRatio = (float) viewHeight / (float) imageHeight;
        if (heightRatio > widthRatio)
        {
            float height = viewWidth / imageAspectRatio;
            destRect = QRectF(0.0f, (viewHeight - height) / 2.0f, viewWidth, height);
        }
        else
        {
            float width = viewHeight * imageAspectRatio;
            destRect = QRectF((viewWidth - width) / 2.0f, 0.0f, width, viewHeight);
        }

        float srcWidth = (float) imageWidth / m_zoomFactor;
        float srcHeight = (float) imageHeight / m_zoomFactor;
        float left = (m_center.x() / 360.0f + 0.5f) * imageWidth - srcWidth / 2.0f;
        float top = (m_center.y() / 180.0f + 0.5f) * imageHeight - srcHeight / 2.0f;
        painter.drawPixmap(destRect,
                           *m_pixmap,
                           QRectF(left, top, srcWidth, srcHeight));

        // Handle wrapping when the viewport straddles the 180 degree meridian
        if (m_east > 180.0f)
        {
            float wrapFraction = (m_east - 180.0f) / 360.0f;
            float srcWrapWidth = imageWidth * wrapFraction;
            float destWrapWidth = destRect.width() * wrapFraction * m_zoomFactor;
            float destWrapLeft = destRect.left() + destRect.width() - destWrapWidth;
            painter.drawPixmap(QRectF(destWrapLeft, destRect.top(), destWrapWidth, destRect.height()),
                               *m_pixmap,
                               QRectF(0.0f, top, srcWrapWidth, srcHeight));
        }
    }

    painter.setClipping(true);
    painter.setClipRect(destRect);

    // Set the world transform so that coordinates can be given in
    // degrees of longitude and latitude.
    float xscale = destRect.width() / 360.0f;
    float yscale = destRect.height() / 180.0f;

    QTransform xform;
    xform.translate((viewWidth - destRect.width()) * 0.5f, (viewHeight - destRect.height()) * 0.5f);
    xform.scale(xscale, -yscale);
    xform.scale(m_zoomFactor, m_zoomFactor);
    xform.translate((180.0f / m_zoomFactor - m_center.x()), (90.0f / m_zoomFactor - m_center.y()) * -1.0f);
    painter.setWorldTransform(xform);

    painter.setRenderHints(QPainter::Antialiasing);

    if (m_showTerminator)
    {
        // Shade the night side of the planet first so that brightness of ground
        // tracks, labels, etc. are not affected.
        drawDaylitRegion(painter);
    }

    QFont MeridianFont("Helvetica", 10); // making the font a bit smaller that the sats names fonts
    painter.setFont(MeridianFont);

    // Dimming out the meridian lines a bit to have more contrast with yellow trajectories
    QColor longLatColor(200, 200, 0, 100);
    //QColor longLatColor(255, 255, 0, 128);

    QColor longLatColorOpaque = longLatColor;
    longLatColorOpaque.setAlpha(255);
    QPen longLatPen(longLatColor);

    // Disable line style because it dramatically slows rendering performance at high
    // zoom levels.
    //longLatPen.setStyle(Qt::DotLine);

    //int fontHeight = painter.fontMetrics().height;

    // Draw the longitude / latitude grid
    if (m_showGrid)
    {
        double maxLongitudeLines = 10.0;
        double longitudeSpacing = meridianSpacing(maxLongitudeLines);
        double latitudeSpacing = longitudeSpacing; // identical to meridian spacing (for now)

        // Leave some space at the edge of the view for labels
        float labelLeftMarginPixels = 20.0f;
        float labelTopMarginPixels = 20.0f;

        // xform transforms long/lat to pixels
        QTransform xform = painter.worldTransform();
        QTransform invXform = xform.inverted();

        QPointF topLeftWorld = (destRect.topLeft() + QPointF(labelLeftMarginPixels, labelTopMarginPixels)) * invXform;
        double longitudeSpacingDeg = longitudeSpacing / 3600.0;
        double latitudeSpacingDeg = latitudeSpacing / 3600.0;

        // Calculate the start and end of long/lat lines so that a margin remains
        // clear for labels.
        float startLongitude = (float) (floor(topLeftWorld.x() / longitudeSpacingDeg) + 1.0f) * longitudeSpacingDeg;
        float endLongitude = startLongitude + (float) longitudeSpacingDeg * maxLongitudeLines;
        float endLatitude = (float) (floor(topLeftWorld.y() / latitudeSpacingDeg) + 1.0f) * latitudeSpacingDeg;
        float startLatitude = endLatitude - (float) latitudeSpacingDeg * maxLongitudeLines;

        // Draw lines of longitude
        for (float l = startLongitude; l < endLongitude; l += (float) longitudeSpacing / 3600.0f)
        {
            if (l != 0.0f)
                painter.setPen(longLatPen);
            else
                painter.setPen(longLatColor);

            // Calculate the top margin in world coordinates
            QPointF top(0.0f, labelTopMarginPixels + destRect.top());
            top = top * invXform;

            painter.drawLine(QPointF(l, top.y()), QPointF(l, -90.0f));
            // We don't want the transformation to scale the text. So, we temporarily
            // reset the transformation to an identity matrix, draw the text, then restore
            // the matrix.
            painter.setWorldTransform(QTransform());

            QPointF textOrigin = QPointF(l, 0.0f) * xform;
            QRectF textRect(textOrigin.x() - 20.0f, destRect.top(), 40.0f, labelTopMarginPixels);

            QString meridianLabel = QString::number(reduceLongitude(l));
            painter.setPen(longLatColorOpaque);
            painter.drawText(textRect, Qt::AlignCenter, meridianLabel);
            painter.setWorldTransform(xform);
        }

        // Draw lines of latitude
        for (float l = startLatitude; l < endLatitude; l += (float) latitudeSpacing / 3600.0f)
        {
            if (l != 0.0f)
                painter.setPen(longLatPen);
            else
                painter.setPen(longLatColor);

            // Calculate the top margin in world coordinates
            QPointF left(destRect.left() + labelLeftMarginPixels, 0.0f);
            left = left * invXform;

            painter.drawLine(QPointF(left.x(), l), QPointF(m_east, l));
            // We don't want the transformation to scale the text. So, we temporarily
            // reset the transformation to an identity matrix, draw the text, then restore
            // the matrix.
            painter.setWorldTransform(QTransform());

            QPointF textOrigin = QPointF(0.0f, l) * xform;
            QRectF textRect(destRect.left() + 5.0f, textOrigin.y() - labelTopMarginPixels / 2, 40.0f, labelTopMarginPixels);

            QString parallelLabel = QString::number(l);
            painter.setPen(longLatColorOpaque);
            painter.drawText(textRect, Qt::AlignVCenter | Qt::AlignLeft, parallelLabel);
            painter.setWorldTransform(xform);
        }
    }

    // Draw the Equatorial grid
    if (m_showEquator)
    {
	double maxLongitudeLines = 0.0;
	double longitudeSpacing = 0.0f;
	double latitudeSpacing = longitudeSpacing; // identical to meridian spacing (for now)

	// Leave some space at the edge of the view for labels
	float labelLeftMarginPixels = 20.0f;
	float labelTopMarginPixels = 20.0f;

	// xform transforms long/lat to pixels
	QTransform xform = painter.worldTransform();
	QTransform invXform = xform.inverted();

	QPointF topLeftWorld = (destRect.topLeft() + QPointF(labelLeftMarginPixels, labelTopMarginPixels)) * invXform;
	double longitudeSpacingDeg = longitudeSpacing / 3600.0;
	double latitudeSpacingDeg = latitudeSpacing / 3600.0;

	// Calculate the start and end of long/lat lines so that a margin remains
	// clear for labels.
	float startLongitude = (float) (floor(topLeftWorld.x() / longitudeSpacingDeg) + 0.0f) * longitudeSpacingDeg;
	float endLongitude = startLongitude + (float) longitudeSpacingDeg * maxLongitudeLines;
	float endLatitude = 0.0f;
	float startLatitude = 0.0f;

	/*
	// Draw lines of longitude
	for (float l = startLongitude; l < endLongitude; l += (float) longitudeSpacing / 3600.0f)
	{
	    if (l != 0.0f)
		//painter.setPen(longLatPen);
	        painter.setPen(equatorialPen);
	    else
		//painter.setPen(longLatColor);
		painter.setPen(equatorialPen);

	    // Calculate the top margin in world coordinates
	    QPointF top(0.0f, labelTopMarginPixels + destRect.top());
	    top = top * invXform;

	    painter.drawLine(QPointF(l, top.y()), QPointF(l, -90.0f));
	    // We don't want the transformation to scale the text. So, we temporarily
	    // reset the transformation to an identity matrix, draw the text, then restore
	    // the matrix.
	    painter.setWorldTransform(QTransform());

	    QPointF textOrigin = QPointF(l, 0.0f) * xform;
	    QRectF textRect(textOrigin.x() - 20.0f, destRect.top(), 40.0f, labelTopMarginPixels);

	    QString meridianLabel = QString::number(reduceLongitude(l));
	    painter.setPen(longLatColorOpaque);
	    painter.drawText(textRect, Qt::AlignCenter, meridianLabel);
	    painter.setWorldTransform(xform);
	}
	*/

	QColor equatorColor(220, 220, 0, 100);
	QColor equatorColorOpaque = equatorColor;
	equatorColorOpaque.setAlpha(255);
	QPen equatorPen(equatorColor);
	painter.setPen(equatorPen);

	// Calculate the top margin in world coordinates
	QPointF left(destRect.left() + labelLeftMarginPixels, 0.0f);
	left = left * invXform;

	painter.drawLine(QPointF(left.x(), 0.0f), QPointF(m_east, 0.0f));
	// We don't want the transformation to scale the text. So, we temporarily
	// reset the transformation to an identity matrix, draw the text, then restore
	// the matrix.
	painter.setWorldTransform(QTransform());

	QPointF textOrigin = QPointF(0.0f, 0.0f) * xform;
	QRectF textRect(destRect.left() + 5.0f, textOrigin.y() - labelTopMarginPixels / 2, 40.0f, labelTopMarginPixels);

	QString parallelLabel = QString::number(0.0f);
	painter.setPen(equatorColorOpaque);
	painter.drawText(textRect, Qt::AlignVCenter | Qt::AlignLeft, parallelLabel);
	painter.setWorldTransform(xform);
    }


    // Coming back to a 9 points font;
    painter.setFont(font);

    float indicatorSize = 3.0f / m_zoomFactor;
    float labelOffset = 2.0f / m_zoomFactor;

    if (m_showTerminator)
    {
        drawSubsolarPoint(painter, Qt::white, indicatorSize);
    }

    // TODO: This should be set by the user. But for now, we just use the first
    // active spacecraft.
    const SpaceObject* selectedSpacecraft = NULL;
    float selectedSpacecraftAltitude = 0.0;

    // Draw each ground track
    foreach (GroundTrack* track, m_groundTrackList)
    {
        // Calculate the current latitude and longitude of the vehicle
        sta::StateVector v;
        double longNow = 0.0;
        double latNow  = 0.0;
        double altNow  = 0.0;
        bool activeNow = false;
        if (track->vehicle->getStateVector(m_currentTime, *m_body, sta::CoordinateSystem(sta::COORDSYS_BODYFIXED), &v))
        {
            planetographicCoords(v.position, m_body, &longNow, &latNow, &altNow);
            activeNow = true;
            if (!selectedSpacecraft)
            {
                selectedSpacecraft = track->vehicle;
                selectedSpacecraftAltitude = altNow;
            }

            if (longNow < m_west)
                longNow += 360.0;
        }

        // Calculate the ground track
        m_trackPoints.clear();

        double endTime = std::numeric_limits<double>::infinity();
        bool complete = false;

        if (!m_showEntireTracks)
        {
            endTime = m_currentTime;
            if (track->samples.size() > 0 &&
                m_currentTime <= track->samples[0].mjd)
            {
                complete = true;
            }
        }

        for (int i = 1; i < track->samples.size() && !complete; i++)
        {
            double long0 = track->samples[i - 1].longitude;
            double lat0  = track->samples[i - 1].latitude;
            double long1 = track->samples[i].longitude;
            double lat1  = track->samples[i].latitude;

            // Only draw the track up to the specified endTime
            if (track->samples[i].mjd >= endTime)
            {
                long1 = longNow;
                lat1  = latNow;
                complete = true;
            }

            if (long0 < m_west)
                long0 += 360.0f;
            if (long1 < m_west)
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
                m_trackPoints << QPointF(long0, lat0)
			<< QPointF(long1, lat1);
            }
        }

        // Draw ticks on the ground tracks
        if (m_showTicks)
        {
            float tickScale = 2.5f / xscale / m_zoomFactor;
            double startTime = 0.0;
            if (track->samples.size() > 0)
                startTime = track->samples.first().mjd;
            int lastTick = track->ticks.size();
            if (!m_showEntireTracks)
            {
                lastTick = std::min(lastTick,
                                    (int) std::ceil((endTime - startTime) / sta::secsToDays(m_tickInterval)));
            }

            for (int i = 0; i < lastTick; i++)
            {
                GroundTrackTick& tick = track->ticks[i];
                float tickLongitude = tick.longitude;
                if (tickLongitude < m_west)
                    tickLongitude += 360.0f;
                m_trackPoints << QPointF(tickLongitude - tick.dx * tickScale,
                                         tick.latitude  - tick.dy * tickScale)
		<< QPointF(tickLongitude + tick.dx * tickScale,
			   tick.latitude  + tick.dy * tickScale);
            }
        }

        painter.setPen(track->color);
        painter.drawLines(m_trackPoints);

        // Draw an indicator at the current spacecraft subpoint
        if (activeNow)
        {
            painter.drawLine(QPointF((float) longNow - indicatorSize, (float) latNow),
                             QPointF((float) longNow + indicatorSize, (float) latNow));
            painter.drawLine(QPointF((float) longNow, (float) latNow - indicatorSize),
                             QPointF((float) longNow, (float) latNow + indicatorSize));

            // We don't want the transformation to scale the text. So, we temporarily
            // reset the transformation to an identity matrix, draw the text, then restore
            // the matrix.
            QTransform xform = painter.worldTransform();
            painter.setWorldTransform(QTransform());
            QPointF textOrigin = QPointF((float) longNow + labelOffset, (float) latNow + labelOffset) * xform;            
            painter.drawText(textOrigin, track->vehicle->name());       // Guillermo says: this is an attribute and should be removed
            painter.setWorldTransform(xform);
        }
    }

    // Draw the ground elements
    if (m_scenario)
    {
        foreach (GroundObject* groundObject, m_scenario->groundObjects())
        {
            // Only draw ground elements for the current body
            if (groundObject->centralBody == m_body)
            {
                double lon = groundObject->longitude;
                double lat = groundObject->latitude;
                if (lon < m_west)
                    lon += 360.0;
                painter.setPen(GroundStationColor);
                QPointF center((float) lon, (float) lat);
                painter.drawPoint(center);
                painter.drawEllipse(center, indicatorSize, indicatorSize);

                // We don't want the transformation to scale the text. So, we temporarily
                // reset the transformation to an identity matrix, draw the text, then restore
                // the matrix.
                QTransform xform = painter.worldTransform();
                painter.setWorldTransform(QTransform());
                QPointF textOrigin = QPointF((float) lon + labelOffset, (float) lat + labelOffset) * xform;
                painter.drawText(textOrigin, groundObject->name);           // Guillermo says: this is an attribute
                painter.setWorldTransform(xform);

                if (selectedSpacecraft)
                {
                    // TODO: Make this a property of the ground station
                    double minElevationAngle = sta::degToRad(GroundStationElevationAngle);

                    // Highlight the ground coverage region when it can see the selected satellite
                    QPen coveragePen(GroundStationColor);
                    if (groundObject->elevationAngle(selectedSpacecraft, m_currentTime) > minElevationAngle)
                    {
                        coveragePen.setColor(Qt::white);
                        coveragePen.setWidthF(1.0f / m_zoomFactor);
                    }

                    painter.setPen(coveragePen);
                    drawCoverageFootprint(painter,
                                          lon, lat, m_body,
                                          minElevationAngle,
                                          selectedSpacecraftAltitude);
                }
            }
        }
    }
    // show Analysis (Claas Grohnfeldt, Steffen Peter)
    if (m_showCoverageCurrent || m_showCoverageHistory)
    {
        // draw covered points
        for (int i=0; i < m_analysis->m_anaSpaceObjectList.length(); i++)
        {
            if (!(m_analysis->m_anaSpaceObjectList.at(i).coveragesample.back().curtime < m_currentTime ||
		  m_analysis->m_anaSpaceObjectList.at(i).coveragesample.first().curtime > m_currentTime))
            {
                CoverageSample tmpS;
                tmpS.curtime = m_currentTime;
                QList<CoverageSample>::const_iterator iter = qLowerBound(m_analysis->m_anaSpaceObjectList.at(i).coveragesample.begin(),
									 m_analysis->m_anaSpaceObjectList.at(i).coveragesample.end(),
									 tmpS,
									 coverageSampleLessThan);
                if (m_showCoverageHistory)
                {
                    drawCoverage(painter, m_analysis->m_anaSpaceObjectList.at(i).asocolor, false, iter->histpoints);
                }
                if (m_showCoverageCurrent)
                {
                    drawCoverage(painter, m_analysis->m_anaSpaceObjectList.at(i).asocolor, true, iter->curpoints);
                }
            }
        }
    }
    if (m_showDiscretization)
    {
        drawDiscretization(painter, m_analysis->m_discreteMesh->meshAsList);
    }
    AlignedBox<float, 3> clipBox;
    clipBox.min() = Vector3f(m_west, m_south, 0.0f);
    clipBox.max() = Vector3f(m_east, m_north, m_maxHeight);
    if(m_showAnalysis)
    {
        painter.setPen(Qt::NoPen);
        painter.setBrush(Qt::NoBrush);
        QPen linkPen = painter.pen();
        linkPen.setStyle(Qt::DotLine);
        linkPen.setColor(Qt::white);
        painter.setPen(linkPen);
        m_trackPoints.clear();   

        // Show Links between Space Objects
        // Todo: merge m_showSOLink and m_showGOLink (it is too much code duplication)
        if (m_showSOLink)
        {
            for (int i = 0; i < m_analysis->m_anaSpaceObjectList.length(); i++)
            {
                LinkSample tmpLS;
                tmpLS.curtime = m_currentTime;

                // search for closest time step of the Linksample
                QList<LinkSample>::const_iterator iter = qLowerBound(m_analysis->m_anaSpaceObjectList.at(i).linksamples.begin(),
                                                                     m_analysis->m_anaSpaceObjectList.at(i).linksamples.end(),
                                                                     tmpLS,
                                                                     linkSampleLessThan);
                if (iter >= m_analysis->m_anaSpaceObjectList.at(i).linksamples.begin() &&
                    iter < m_analysis->m_anaSpaceObjectList.at(i).linksamples.end())
                {
                    SpaceObject* spaceObject1 = m_analysis->m_anaSpaceObjectList.at(i).m_spaceObject;
                    // Calculate the current latitude and longitude of the vehicle
                    sta::StateVector v1;
                    double longNow1 = 0.0;
                    double latNow1  = 0.0;
                    double altNow1  = 0.0;
                    bool activeNow1 = false;
                    if (spaceObject1->getStateVector(m_currentTime, *m_body, sta::CoordinateSystem(sta::COORDSYS_BODYFIXED), &v1))
                    {
                        planetographicCoords(v1.position, m_body, &longNow1, &latNow1, &altNow1);
                        activeNow1 = true;
                        /*if (!selectedSpacecraft)
                        {
                            selectedSpacecraft = track1->vehicle;
                            selectedSpacecraftAltitude = altNow1;
                        }*/

                        if (longNow1 < m_west)
                            longNow1 += 360.0;
                    }

                    //for (int j = 0; j < m_analysis->m_anaSpaceObjectList.at(i).linksamples.at(*iter).connection.length(); j++)
                    for (int j = 0; j < iter->connection.length(); j++)
                    {
                        //SpaceObject* spaceObject2 = m_analysis->m_anaSpaceObjectList.at(i).linksamples.at((int) iter).connection.at(j);
                        SpaceObject* spaceObject2 = iter->connection.at(j);
                        sta::StateVector v2;
                        double longNow2 = 0.0;
                        double latNow2  = 0.0;
                        double altNow2  = 0.0;
                        bool activeNow2 = false;
                        if (spaceObject2->getStateVector(m_currentTime, *m_body, sta::CoordinateSystem(sta::COORDSYS_BODYFIXED), &v2))
                        {
                            planetographicCoords(v2.position, m_body, &longNow2, &latNow2, &altNow2);
                            activeNow2 = true;
                            /*if (!selectedSpacecraft)
                            {
                                selectedSpacecraft = track2->vehicle;
                                selectedSpacecraftAltitude = altNow2;
                            }*/

                            if (longNow2 < m_west)
                                longNow2 += 360.0;
                        }
                        clippedWrappedLine(clipBox, longNow1, latNow1, longNow2, latNow2, m_trackPoints);
                    }
                }
            }
        }
        // Show Links between Ground Objects and Space Objects
        if (m_showGOLink)
        {
            for (int i = 0; i < m_analysis->m_anaSpaceObjectList.length(); i++)
            {
                // linear search for time step (todo: replace by an efficient algorithm)
                int iter = 0;
                while ((iter < m_analysis->m_anaSpaceObjectList.at(i).groundlinksamples.length()-1) &&
                       (m_analysis->m_anaSpaceObjectList.at(i).groundlinksamples.at(iter).curtime < m_currentTime))
                {
                    iter++;
                }
                if (iter == m_analysis->m_anaSpaceObjectList.at(i).groundlinksamples.length())
                {
                    iter = 0; // Error
                }
                SpaceObject* spaceObject1 = m_analysis->m_anaSpaceObjectList.at(i).m_spaceObject;
                // Calculate the current latitude and longitude of the vehicle
                sta::StateVector v1;
                double longNow1 = 0.0;
                double latNow1  = 0.0;
                double altNow1  = 0.0;
                bool activeNow1 = false;
                if (spaceObject1->getStateVector(m_currentTime, *m_body, sta::CoordinateSystem(sta::COORDSYS_BODYFIXED), &v1))
                {
                    planetographicCoords(v1.position, m_body, &longNow1, &latNow1, &altNow1);
                    activeNow1 = true;
                    /*if (!selectedSpacecraft)
                    {
                        selectedSpacecraft = track1->vehicle;
                        selectedSpacecraftAltitude = altNow1;
                    }*/

                    if (longNow1 < m_west)
                        longNow1 += 360.0;
                }
                for (int j = 0; j < m_analysis->m_anaSpaceObjectList.at(i).groundlinksamples.at(iter).connection.length(); j++)
                {
                    GroundObject* groundObject = m_analysis->m_anaSpaceObjectList.at(i).groundlinksamples.at(iter).connection.at(j);
                    /*sta::StateVector v2;
                    double longNow2 = 0.0;
                    double latNow2  = 0.0;
                    double altNow2  = 0.0;
                    bool activeNow2 = false;
                    if (spaceObject2->getStateVector(m_currentTime, *m_body, sta::CoordinateSystem(sta::COORDSYS_BODYFIXED), &v2))
                    {
                        planetographicCoords(v2.position, m_body, &longNow2, &latNow2, &altNow2);
                        activeNow2 = true;
                        if (!selectedSpacecraft)
                        {
                            selectedSpacecraft = track2->vehicle;
                            selectedSpacecraftAltitude = altNow2;
                        }

                        if (longNow2 < m_west)
                            longNow2 += 360.0;
                    }*/
                    clippedWrappedLine(clipBox, longNow1, latNow1, groundObject->longitude, groundObject->latitude, m_trackPoints);
                }
            }
        }
        painter.drawLines(m_trackPoints);
        painter.setPen(Qt::NoPen);
        painter.setBrush(Qt::NoBrush);
    }

}


// Calculate the spacing between longitude lines. Find the value in the spacings table that
// gives as many lines as possible without exceeding the maximum number of lines.
double
	GroundTrackView::meridianSpacing(int maxMeridians)
{
    double visibleLongitudeRange = 360.0 / m_zoomFactor;
    double idealLongitudeSpacing = visibleLongitudeRange * 3600.0 / maxMeridians;
    unsigned int spacingCount = sizeof(LongLatSpacings) / sizeof(LongLatSpacings[0]);

    unsigned int spacingIndex = 1;
    while (spacingIndex < spacingCount && idealLongitudeSpacing <= LongLatSpacings[spacingIndex])
    {
        spacingIndex++;
    }

    return LongLatSpacings[spacingIndex - 1];
}


void GroundTrackView::contextMenuEvent(QContextMenuEvent* event)
{
    if (!m_contextMenu)
        m_contextMenu = new QMenu(this);
    m_contextMenu->clear();

    QAction* entireAction = new QAction(tr("Show &Entire Tracks"), m_contextMenu);
    entireAction->setCheckable(true);
    entireAction->setChecked(m_showEntireTracks);
    connect(entireAction, SIGNAL(triggered(bool)), this, SLOT(setShowEntireTracks(bool)));

    QAction* gridAction = new QAction(tr("Show &Grid"), m_contextMenu);
    connect(gridAction, SIGNAL(triggered(bool)), this, SLOT(setShowGrid(bool)));
    gridAction->setCheckable(true);
    gridAction->setChecked(m_showGrid);
    MainWindow myMainWindow;
    myMainWindow.m_groundTrackPlotTool->m_toolBar->m_gridAction->setChecked(m_showGrid);
    myMainWindow.m_groundTrackPlotTool->show();

    QAction* equatorAction = new QAction(tr("Show &Equator"), m_contextMenu);
    connect(equatorAction, SIGNAL(triggered(bool)), this, SLOT(setShowEquator(bool)));
    equatorAction->setCheckable(true);
    equatorAction->setChecked(m_showEquator);
    myMainWindow.m_groundTrackPlotTool->m_toolBar->m_equatorAction->setChecked(m_showEquator);
    myMainWindow.m_groundTrackPlotTool->show();


    m_contextMenu->addAction(entireAction);
    m_contextMenu->addAction(gridAction);
    m_contextMenu->addAction(equatorAction);

    m_contextMenu->popup(event->globalPos());
}

// Analysis (Claas Grohnfeldt, Steffen Peter)
void GroundTrackView::setAnalysis(ConstellationAnalysis* m_analysisOfConstellations)
{
    m_showDiscretization = false;
    m_showCoverageCurrent = false;
    m_showCoverageHistory = false;
    m_showSOLink = false;
    m_showGOLink = false;
    m_analysis = m_analysisOfConstellations;
}


void GroundTrackView::setDiscretizationVisible(bool visible)
{
    m_showDiscretization = visible;
    viewport()->update();
}

void GroundTrackView::setCoverageCurrentVisible(bool visible)
{
    m_showCoverageCurrent = visible;
    viewport()->update();
}

void GroundTrackView::setCoverageHistoryVisible(bool visible)
{
    m_showCoverageHistory = visible;
    viewport()->update();
}

void GroundTrackView::setLinkSOVisible(bool visible)
{
    m_showSOLink = visible;
    viewport()->update();
}

void GroundTrackView::setLinkGOVisible(bool visible)
{
    m_showGOLink = visible;
    viewport()->update();
}

void GroundTrackView::drawDiscretization(QPainter& painter, QList<DiscretizationPoint> discretePoints)
{
    painter.setBrush(Qt::NoBrush);
    painter.setPen(Qt::NoPen);
    // plots the whole mesh
    painter.setPen(QColor(0, 0, 0, 30));
    int numRows = m_analysis->m_discreteMesh->numberRows;
    //plot the list of all discrete points
    int numberDiscretePoints = discretePoints.length();
    float ellipseHeight = (4.0/5.0)*90.0/(double)numRows;
    for (int k = 0; k < numberDiscretePoints; k++)
    {
        //float ellipseWidth = (discretePoints.at(k).rectangleWidth)/2;
        float lon = discretePoints.at(k).longitude;
        float lat = discretePoints.at(k).latitude;
        if (lon < m_west)
        {
            lon += 360.0f;
        }
        if( lat -ellipseHeight > m_south &&
            lat +ellipseHeight < m_north &&
            lon -ellipseHeight > m_west &&
            lon +ellipseHeight < m_east )
        {
            QPointF circleCenter = QPointF(lon, lat);
            painter.drawEllipse(circleCenter, ellipseHeight, ellipseHeight);
        }
    }
    painter.setBrush(Qt::NoBrush);
    painter.setPen(Qt::NoPen);
}

void GroundTrackView::drawCoverage(QPainter& painter, QColor asocolor, bool colored, QList<DiscretizationPoint> discretePoints)
{
    painter.setBrush(Qt::NoBrush);
    painter.setPen(Qt::NoPen);
    //convert the space object color to a more transparent one
    int redpart = int(double(asocolor.red())*0.96);
    int greenpart = int(double(asocolor.green())*0.96);
    int bluepart = int(double(asocolor.blue())*0.96);
    painter.setPen(QColor(0, 0, 0, 30));
    if(colored)
    {
        painter.setBrush(QColor(redpart, greenpart, bluepart,100));//QColor(0, 0, 0, 80));
    }else{
        painter.setBrush(QColor(0, 0, 0, 60));
    }
    int numRows = m_analysis->m_discreteMesh->numberRows;
    //plot the list of all discrete points
    int numberDiscretePoints = discretePoints.length();
    float ellipseHeight = (4.0/5.0)*90.0/(double)numRows;
    for (int k = 0; k < numberDiscretePoints; k++)
    {
        //float ellipseWidth = (discretePoints.at(k).rectangleWidth)/2;
        float lon = discretePoints.at(k).longitude;
        float lat = discretePoints.at(k).latitude;
        if (lon < m_west)
        {
            lon += 360.0f;
        }
        if( lat -ellipseHeight > m_south &&
            lat +ellipseHeight < m_north &&
            lon -ellipseHeight > m_west &&
            lon +ellipseHeight < m_east )
        {
            QPointF circleCenter = QPointF(lon, lat);
            painter.drawEllipse(circleCenter, ellipseHeight, ellipseHeight);
        }
    }
    painter.setBrush(Qt::NoBrush);
    painter.setPen(Qt::NoPen);
}


static bool ellipsoidIntersection(const Vector3d& semiAxes, const Vector3d& p, const Vector3d& dir, double* solution1, double* solution2)
{
    Vector3d diff = p; // p - ellipsoidCenter;
    Vector3d s = semiAxes.cwise().inverse().cwise().square();
    Vector3d sdir = dir.cwise() * s;
    Vector3d sdiff = diff.cwise() * s;
    double a = dir.dot(sdir);
    double b = dir.dot(sdiff);
    double c = diff.dot(sdiff) - 1.0;
    double disc = b * b - a * c;

    if (disc < 0.0)
    {
        // No real solutions means no intersection
        return false;
    }

    disc = sqrt(disc);
    if (solution1)
    {
        *solution1 = (-b + disc) / a;
    }

    if (solution2)
    {
        *solution2 = (-b - disc) / a;
    }

    return true;
}


// Find the closest point to p on the origin-centered, axis-aligned ellipsoid with the
// given semi-axes.
static Vector3d ellipsoidClosestPoint(const Vector3d& semiAxes, const Vector3d& p)
{
    // ...except not really. This function just calculates the intersection between
    // a ray from p to the center of the ellipse, which is not generally the closest
    // point except in the case of a sphere.
    // TODO: Implement this function for real!

    double t0 = 0.0;
    double t1 = 0.0;
    ellipsoidIntersection(semiAxes, p, -p, &t0, &t1);
    if (abs(t0) < abs(t1))
    {
        return p - p * t0;
    }
    else
    {
        return p - p * t1;
    }
}


/** Calculate the outline of the region in which a spacecraft at the
  * specified altitude would be visible to a ground station.
  *
  * @param stationLongitude planetographic longitude (in degrees) of the ground station
  * @param stationLatitude  planetocentric latitude (in degrees) of the ground station
  * @param planet planet on which the ground station is located
  * @param minElevation minimum angle (in radians) above horizon required for visibility
  * @param spacecraft altitude (in kilometers) of the hypothetical spacecraft
  * @param profilePointCount number of points to generate
  * @param profile contain in which to save the points of the coverage profile
  *
  * @return true if some area of coverage exists, false if not (e.g. minElevation >= 90 degrees)
  */
bool
	GroundTrackView::computeCoverageFootprint(double stationLongitude,
						  double stationLatitude,
						  const StaBody* planet,
						  double minElevation,
						  double spacecraftAltitude,
						  unsigned int profilePointCount,
						  QVector<Eigen::Vector3d>& profile) const
{
    if (std::abs(minElevation) >= sta::Pi() || spacecraftAltitude <= 0.0)
    {
        return false;
    }

    Vector3d stationPosition = planet->planetographicToCartesian(stationLatitude, stationLongitude, 0.0);

    // Get the horizon based coordinate system for the station. Not quite correct for
    // non-spherical bodies, where the normal to the tangent plane will not necessarily
    // be the direction to the center.
    //
    // The horizontal coordinate system has +x east, +y north, +z up (i.e. normal to the
    // local tangent plane.)
    Vector3d up = stationPosition.normalized();
    Vector3d northPole = Vector3d::UnitZ();
    Vector3d east = northPole.cross(up);
    Vector3d north;
    if (east.isZero())
    {
        // Station is at or very close to the pole. Choose an arbitrary direction orthogonal
        // to the up direction.
        north = Vector3d::UnitX();
    }
    else
    {
        east.normalize();
        north = up.cross(east);
    }

    // Create a rotation matrix to transform from local horizontal coordinates to
    // planetocentric cartesian.
    Matrix3d fromHorizontal;
    fromHorizontal << east, north, up;

    double sinElevation = sin(minElevation);
    double cosElevation = cos(minElevation);

    // Compute the length of one side of the visibility cone for the ground
    // station. Just use the law of cosines...
    double gamma = sta::Pi() / 2.0 + minElevation;
    double a = planet->meanRadius();
    double c = planet->meanRadius() + spacecraftAltitude;
    double coneSide = a * cos(gamma) + sqrt(-pow(a * sin(gamma), 2.0) + c * c);

    // Generate the profile points
    for (unsigned int i = 0; i < profilePointCount; ++i)
    {
        double theta = (double) i / (double) profilePointCount * sta::Pi() * 2.0;
        Vector3d v = Vector3d(cosElevation * cos(theta), cosElevation * sin(theta), sinElevation) * coneSide;

        // Transform to planetocentric
        v = stationPosition + fromHorizontal * v;

        // Find the subpoint
        profile << ellipsoidClosestPoint(planet->radii(), v);
    }

    return true;
}


void
	GroundTrackView::drawCoverageFootprint(QPainter& painter,
					       double stationLongitude,
					       double stationLatitude,
					       const StaBody* planet,
					       double minElevation,
					       double spacecraftAltitude)
{
    m_coverageProfile.clear();
    computeCoverageFootprint(stationLongitude, stationLatitude, planet,
                             minElevation, spacecraftAltitude,
                             100, m_coverageProfile);

#if 0
    foreach (Vector3d p, m_coverageProfile)
    {
        double longitude, latitude, altitude;
        planetographicCoords(p, planet, &longitude, &latitude, &altitude);
        if (longitude < m_west)
            longitude += 360.0;
        painter.drawPoint(QPointF((float) longitude, (float) latitude));
    }
#endif
    AlignedBox<float, 3> clipBox;
    clipBox.min() = Vector3f(m_west, m_south, 0.0f);
    clipBox.max() = Vector3f(m_east, m_north, 1.0f);

    double lastLongitude;
    double lastLatitude;
    double altitude;

    planetographicCoords(m_coverageProfile.last(), planet, &lastLongitude, &lastLatitude, &altitude);
    if (lastLongitude < m_west)
        lastLongitude += 360.0;

    m_trackPoints.clear();
    foreach (Vector3d p, m_coverageProfile)
    {
        double longitude, latitude;
        planetographicCoords(p, planet, &longitude, &latitude, &altitude);
        if (longitude < m_west)
            longitude += 360.0;

        // Look out for wrapping
        if (std::abs(longitude - lastLongitude) < 180.0)
        {
            clippedLine(clipBox, lastLongitude, lastLatitude, longitude, latitude, m_trackPoints);
        }
        else
        {
        }

        lastLongitude = longitude;
        lastLatitude = latitude;
    }

    painter.drawLines(m_trackPoints);
    m_trackPoints.clear();
}


void GroundTrackView::drawSubsolarPoint(QPainter& painter, QColor color, float size)
{
    // Get the position of the Sun in the current planet-fixed coordinate frame.
    sta::StateVector sunState = STA_SOLAR_SYSTEM->sun()->stateVector(m_currentTime, m_body, sta::COORDSYS_BODYFIXED);
    Vector3d subpointPos = sunState.position.normalized();
    double subLong = sta::radToDeg(atan2(subpointPos.y(), subpointPos.x()));
    double subLat  = sta::radToDeg(asin(subpointPos.z()));
    if (subLong < m_west)
        subLong += 360.0;

    if (subLong >= m_west && subLong <= m_east && subLat >= m_south && subLat <= m_north)
    {
        painter.setPen(color);

        painter.setBrush(Qt::white);

        // Draw a sunlike icon with a circle and eight rays
        painter.drawEllipse(QPointF((float) subLong, (float) subLat), size * 0.5f, size * 0.5f);
        painter.setBrush(Qt::NoBrush);
        painter.drawLine(QPointF((float) subLong - size, (float) subLat),
                         QPointF((float) subLong + size, (float) subLat));
        painter.drawLine(QPointF((float) subLong, (float) subLat - size),
                         QPointF((float) subLong, (float) subLat + size));
        painter.drawLine(QPointF((float) subLong - size * 0.7f, (float) subLat - size * 0.7f),
                         QPointF((float) subLong + size * 0.7f, (float) subLat + size * 0.7f));
        painter.drawLine(QPointF((float) subLong + size * 0.7f, (float) subLat - size * 0.7f),
                         QPointF((float) subLong - size * 0.7f, (float) subLat + size * 0.7f));
    }
}


// Draw the region of the planet illuminated by the Sun.
// TODO: This function still approximates the body as a sphere
void
	GroundTrackView::drawDaylitRegion(QPainter& painter)
{
    // Get the position of the Sun in the current planet-fixed coordinate frame.
    sta::StateVector sunState = STA_SOLAR_SYSTEM->sun()->stateVector(m_currentTime, m_body, sta::COORDSYS_BODYFIXED);
    Vector3d subpointPos = sunState.position.normalized();

    // The method is called drawDaylitRegion, but we're actually going to shade
    // the night side of the planet.
    painter.setPen(Qt::NoPen);
    painter.setBrush(QColor(0, 0, 0, 80));

    // TODO: Special care is required when the subsolar point is almost exactly over the
    // equator (i.e. subpointPos.z() is very close to zero.) In this configuration, the
    // projection of the night side is a square centered on the solar subpoint. Add a
    // special case to draw it (the general algorithm may produce a divide-by-zero.)

    // Subdivide the viewport into this many vertical slices
    int steps = 300;
    bool northPoleShadowed = subpointPos.z() < 0.0;

    double lastLatitude = 0.0;
    double lastLongitude = 0.0;
    for (int i = 0; i < steps; i++)
    {
        double longitude = m_west + (m_east - m_west) * (double) i / (steps - 1);
        double phi = sta::degToRad(longitude);

        // Calculate the latitude on this meridian at which the Sun has an elevation of
        // zero degrees.
        double theta = atan((subpointPos.x() * cos(phi) + subpointPos.y() * sin(phi)) / -subpointPos.z());
        double latitude = sta::radToDeg(theta);

        QPointF points[4];
        if (i != 0)
        {
            if (northPoleShadowed)
            {
                points[0] = QPointF((float) lastLongitude, m_north);
                points[1] = QPointF((float) lastLongitude, std::min((float) lastLatitude, m_north));
                points[2] = QPointF((float) longitude, std::min((float) latitude, m_north));
                points[3] = QPointF((float) longitude, m_north);
            }
            else
            {
                points[0] = QPointF((float) lastLongitude, m_south);
                points[1] = QPointF((float) lastLongitude, std::max((float) lastLatitude, m_south));
                points[2] = QPointF((float) longitude, std::max((float) latitude, m_south));
                points[3] = QPointF((float) longitude, m_south);
            }

            painter.drawConvexPolygon(points, 4);
        }

        lastLatitude = latitude;
        lastLongitude = longitude;
    }

    painter.setBrush(Qt::NoBrush);
}


/*** GroundTrackPlotTool implementation ***/

GroundTrackPlotTool::GroundTrackPlotTool(QWidget* parent) :
	QWidget(parent),
	m_view(NULL),
	m_toolBar(NULL)
{
    m_view = new GroundTrackView(this);

    // Create and hook up the tool bar
    m_toolBar = new VisualizationToolBar(tr("View Controls"), this);
    connect(m_toolBar, SIGNAL(bodyChanged(const StaBody*)),  m_view, SLOT(setBody(const StaBody*)));
    connect(m_toolBar, SIGNAL(gridToggled(bool)),            m_view, SLOT(setShowGrid(bool)));
    connect(m_toolBar, SIGNAL(equatorToggled(bool)),         m_view, SLOT(setShowEquator(bool)));
    connect(m_toolBar, SIGNAL(terminatorToggled(bool)),      m_view, SLOT(setTerminatorVisible(bool)));
    connect(m_toolBar, SIGNAL(tickIntervalChanged(double)),  m_view, SLOT(setTickInterval(double)));
    connect(m_toolBar, SIGNAL(projectionChanged(bool)),      m_view, SLOT(set2HalfDView(bool)));
    connect(m_toolBar, SIGNAL(saveImageRequested()),         m_view, SLOT(saveImage()));

    // Analysis Button signals (Claas Grohnfeldt, Steffen Peter)
    connect(m_toolBar, SIGNAL(discretizationToggled(bool)),  m_view, SLOT(setDiscretizationVisible(bool)));
    connect(m_toolBar, SIGNAL(coverageCurrentToggled(bool)), m_view, SLOT(setCoverageCurrentVisible(bool)));
    connect(m_toolBar, SIGNAL(coverageHistoryToggled(bool)), m_view, SLOT(setCoverageHistoryVisible(bool)));
    connect(m_toolBar, SIGNAL(linkSOToggled(bool)),          m_view, SLOT(setLinkSOVisible(bool)));
    connect(m_toolBar, SIGNAL(linkGOToggled(bool)),          m_view, SLOT(setLinkGOVisible(bool)));

    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->setSpacing(1);
    layout->setContentsMargins(3, 3, 3, 3);
    layout->addWidget(m_view);
    layout->addWidget(m_toolBar);
    setLayout(layout);

    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
}


GroundTrackPlotTool::~GroundTrackPlotTool()
{
}



// Analysis (Claas Grohnfeldt, Steffen Peter)
void GroundTrackPlotTool::setAnalysis(ConstellationAnalysis* analysisOfConstellations)
{
    m_toolBar->enableAnalysisTools(analysisOfConstellations);
    m_view->setAnalysis(analysisOfConstellations);
}




