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


#include "ThreeDView.h"
#include <Astro-Core/date.h>
#include <Astro-Core/stamath.h>
#include <vesta/Universe.h>
#include <vesta/UniverseRenderer.h>
#include <vesta/Observer.h>
#include <vesta/Body.h>
#include <vesta/Arc.h>
#include <vesta/InertialFrame.h>
#include <vesta/CelestialCoordinateGrid.h>
#include <vesta/StarsLayer.h>
#include <vesta/interaction/ObserverController.h>
#include <QWheelEvent>
#include <QFile>
#include <QDataStream>
#include <QDebug>

using namespace vesta;
using namespace Eigen;


static double
yearsToSecs(double julianYears)
{
    return sta::daysToSecs(365.25 * julianYears);
}

static double BeginningOfTime = -yearsToSecs(100.0);  // 1900 CE
static double EndOfTime = yearsToSecs(100.0);         // 2100 CE
static double ValidTimeSpan = EndOfTime - BeginningOfTime;


ThreeDView::ThreeDView(QWidget* parent) :
    m_currentTime(0.0),
    m_universe(NULL),
    m_renderer(NULL),
    m_fov(sta::degToRad(60.0)),
    m_viewChanged(true)
{
    m_universe = new Universe();
    m_universe->addRef();
    initializeUniverse();

    m_observer = counted_ptr<Observer>(new Observer(*m_universe->findFirst("SSB")));
    m_controller = counted_ptr<ObserverController>(new ObserverController());
    m_controller->setObserver(m_observer.ptr());
}


ThreeDView::~ThreeDView()
{
    if (m_universe)
    {
        m_universe->release();
    }

    delete m_renderer;
}


QSize
ThreeDView::minimumSizeHint() const
{
    return QSize(100, 100);
}


QSize
ThreeDView::sizeHint() const
{
    return QSize(800, 500);
}


void
ThreeDView::initializeGL()
{
    m_renderer = new UniverseRenderer();
    initializeLayers();

    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glEnable(GL_CULL_FACE);
}


void
ThreeDView::paintGL()
{
    if (!m_viewChanged)
    {
        // No need to repaint
        return;
    }
    else
    {
        // Mark the view up to date
        m_viewChanged = false;
    }

    glDepthMask(GL_TRUE);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    m_renderer->beginViewSet(*m_universe, m_currentTime);

    // Render a single view covering the entire widget
    Viewport viewport(size().width(), size().height());
    m_renderer->renderView(NULL, m_observer.ptr(), m_fov, viewport);

    m_renderer->endViewSet();
}


void
ThreeDView::resizeGL(int width, int height)
{
    glViewport(0, 0, width, height);
}


void
ThreeDView::mousePressEvent(QMouseEvent *event)
{
    m_mousePosition = event->posF();
}


void
ThreeDView::mouseReleaseEvent(QMouseEvent* event)
{
}


void
ThreeDView::mouseMoveEvent(QMouseEvent *event)
{
    Vector2f delta(event->posF().x() - m_mousePosition.x(),
                   event->posF().y() - m_mousePosition.y());
    m_mousePosition = event->posF();

    bool leftButton = (event->buttons() & Qt::LeftButton) != 0;

    if (leftButton)
    {
        double xrot = delta.y() / 80.0;
        double yrot = delta.x() / 80.0;

        m_controller->applyOrbitTorque(Vector3d::UnitX() * -xrot);
        m_controller->applyOrbitTorque(Vector3d::UnitY() * -yrot);
    }
}


void
ThreeDView::wheelEvent(QWheelEvent* event)
{
    if (event->orientation() == Qt::Vertical)
    {
    }
}


void
ThreeDView::keyPressEvent(QKeyEvent* event)
{
}


void
ThreeDView::keyReleaseEvent(QKeyEvent* event)
{
}


void
ThreeDView::initializeUniverse()
{
    // Create the Solar System Barycenter
    Entity* ssb = new Entity();
    ssb->setName("SSB");
    ssb->chronology()->addArc(new Arc());
    ssb->chronology()->setBeginning(BeginningOfTime);
    ssb->chronology()->firstArc()->setDuration(ValidTimeSpan);
    m_universe->addEntity(ssb);

    initializeStarCatalog("vis3d/tycho2.stars");
}


// Load the star catalog from a file
void
ThreeDView::initializeStarCatalog(const QString& fileName)
{
    QFile starFile(fileName);

    if (!starFile.open(QFile::ReadOnly))
    {
        qDebug() << "Error opening star file " << starFile.fileName();
        return;
    }

    QDataStream in(&starFile);
    in.setVersion(QDataStream::Qt_4_4);
    in.setByteOrder(QDataStream::BigEndian);

    StarCatalog* stars = new StarCatalog();
    bool done = false;
    while (!done)
    {
        quint32 id = 0;
        float ra = 0.0f;
        float dec = 0.0f;
        float apparentMagnitude = 0.0f;
        float bvColorIndex = 0.0f;
        in >> id >> ra >> dec >> apparentMagnitude >> bvColorIndex;

        if (in.status() == QDataStream::Ok)
        {
            stars->addStar(float(sta::degToRad(ra)), float(sta::degToRad(dec)), apparentMagnitude, bvColorIndex);
        }
        else
        {
            done = true;
        }
    }

    m_universe->setStarCatalog(stars);
}


void
ThreeDView::initializeLayers()
{
    // Layer 0 contains the stars
    if (m_universe->starCatalog())
    {
        StarsLayer* stars = new StarsLayer();
        stars->setStarCatalog(m_universe->starCatalog());
        stars->setVisibility(true);
        m_renderer->addSkyLayer(stars);
    }

    // Layer 1 is an equatorial grid
    CelestialCoordinateGrid* equatorialGrid = new CelestialCoordinateGrid();
    equatorialGrid->setVisibility(true);
    equatorialGrid->setColor(Spectrum(0.0f, 0.0f, 1.0f));
    m_renderer->addSkyLayer(equatorialGrid);
}


void
ThreeDView::setCurrentTime(double t)
{
    if (t != m_currentTime)
    {
        setViewChanged();
        m_currentTime = t;
        update();
    }
}


double
ThreeDView::tick(double dt)
{
    Vector3d lastPosition = m_observer->absolutePosition(m_currentTime);
    Quaterniond lastOrientation = m_observer->absoluteOrientation(m_currentTime);
    m_controller->tick(dt);
    Vector3d newPosition = m_observer->absolutePosition(m_currentTime);
    Quaterniond newOrientation = m_observer->absoluteOrientation(m_currentTime);
    if (lastPosition != newPosition || lastOrientation.coeffs() != newOrientation.coeffs())
    {
        setViewChanged();
    }

    update();
}
