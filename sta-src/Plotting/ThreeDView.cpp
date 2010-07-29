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
#include <Astro-Core/stabody.h>
#include <Astro-Core/ephemeris.h>
#include <vesta/Universe.h>
#include <vesta/UniverseRenderer.h>
#include <vesta/Observer.h>
#include <vesta/Body.h>
#include <vesta/Arc.h>
#include <vesta/Trajectory.h>
#include <vesta/RotationModel.h>
#include <vesta/WorldGeometry.h>
#include <vesta/InertialFrame.h>
#include <vesta/CelestialCoordinateGrid.h>
#include <vesta/StarsLayer.h>
#include <vesta/DataChunk.h>
#include <vesta/DDSLoader.h>
#include <vesta/TextureMapLoader.h>
#include <vesta/interaction/ObserverController.h>

#include <QWheelEvent>
#include <QFile>
#include <QFileInfo>
#include <QDataStream>
#include <QDebug>

#include <cmath>

using namespace vesta;
using namespace Eigen;
using namespace std;

static double J2000 = 2451545.0;
static double MJDBase = 2400000.5;

static double
yearsToSecs(double julianYears)
{
    return sta::daysToSecs(365.25 * julianYears);
}

static double
mjdToSecsSinceJ2000(double mjd)
{
    return sta::daysToSecs(mjd + (MJDBase - J2000));
}

static double
secsSinceJ2000ToMjd(double secs)
{
    return sta::secsToDays(secs) - (MJDBase - J2000);
}

static double BeginningOfTime = -yearsToSecs(100.0);  // 1900 CE
static double EndOfTime = yearsToSecs(100.0);         // 2100 CE
static double ValidTimeSpan = EndOfTime - BeginningOfTime;


/** Trajectory subclass that adapts STA's ephemerides for VESTA.
  */
class StaBodyTrajectory : public Trajectory
{
public:
    StaBodyTrajectory(const StaBody* body, const StaBody* center) :
        m_body(body),
        m_center(center)
    {
    }

    virtual StateVector state(double t) const
    {
        // VESTA provides the time as the number of seconds since 1 Jan 2000 12:00:00 UTC.
        double mjd = secsSinceJ2000ToMjd(t);
        const sta::Ephemeris* ephemeris = m_body->ephemeris();
        if (ephemeris)
        {
            sta::StateVector sv = m_body->stateVector(mjd, m_center, sta::COORDSYS_EME_J2000);
            return StateVector(sv.position, sv.velocity);
        }
        else
        {
            return StateVector(Vector3d::Zero(), Vector3d::Zero());
        }
    }

    virtual double boundingSphereRadius() const
    {
        // Just return a very large value. For better performance, we should eventually return
        // a much tighter bounding sphere.
        return 1.0e10;
    }

    const StaBody* body()
    {
        return m_body;
    }

private:
    const StaBody* m_body;
    const StaBody* m_center;
};


/** RotationModel subclass that adapts STA's planet orientation information for VESTA.
  */
class StaBodyRotationModel : public RotationModel
{
public:
    StaBodyRotationModel(const StaBody* body) :
        m_body(body)
    {
    }

    virtual Quaterniond orientation(double t) const
    {
        // VESTA provides the time as the number of seconds since 1 Jan 2000 12:00:00 UTC.
        double mjd = secsSinceJ2000ToMjd(t);
        return m_body->orientation(mjd, sta::COORDSYS_EME_J2000);
    }

    virtual Vector3d angularVelocity(double /* t */) const
    {
        // TODO: Implement this
        return Vector3d::UnitZ();
    }

    const StaBody* body()
    {
        return m_body;
    }

private:
    const StaBody* m_body;
};


// QImage based TextureLoader implementation
class QtTextureLoader : public TextureMapLoader
{
public:
    bool handleMakeResident(TextureMap* texture)
    {
        QString fileName = texture->name().c_str();

        if (QFileInfo(fileName).suffix() == "dds")
        {
            QFile ddsFile(fileName);
            ddsFile.open(QIODevice::ReadOnly);
            QByteArray data = ddsFile.readAll();
            DataChunk chunk(data.data(), data.size());

            DDSLoader ddsLoader;
            if (!ddsLoader.load(texture, &chunk))
            {
                texture->setStatus(TextureMap::LoadingFailed);
                return false;
            }
            else
            {
                return true;
            }
        }
        else
        {
            QImage image(fileName);
            if (image.isNull())
            {
                texture->setStatus(TextureMap::LoadingFailed);
                return false;
            }

            TextureMap::ImageFormat format = TextureMap::B8G8R8;
            if (image.depth() == 24)
            {
                format = TextureMap::B8G8R8;
            }
            else if (image.depth() == 32)
            {
                format = TextureMap::B8G8R8A8;
            }
            else
            {
                return false;
            }

            int dataSize = image.bytesPerLine() * image.height();
            return texture->generate(image.bits(), dataSize, image.width(), image.height(), format);
        }
    }
};



ThreeDView::ThreeDView(QWidget* parent) :
    m_currentTime(0.0),
    m_universe(NULL),
    m_renderer(NULL),
    m_fov(sta::degToRad(60.0)),
    m_viewChanged(true)
{
    m_textureLoader = counted_ptr<TextureMapLoader>(new QtTextureLoader());

    m_universe = new Universe();
    m_universe->addRef();
    initializeUniverse();

    m_observer = counted_ptr<Observer>(new Observer(*m_universe->findFirst("Earth")));
    m_observer->setPosition(Vector3d(0.0, 0.0, 25000.0));
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
        // Assuming 15 degrees per wheel click; not sure how this maps
        // to other devices such as trackpads.
        float clicks = -event->delta() / 120.0f;
        double zoomFactor = pow(1.03f, clicks);

        m_controller->dolly(zoomFactor);

        update();
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

    Body* sun = addSolarSystemBody(STA_SOLAR_SYSTEM->lookup(STA_SUN), ssb);
    Body* earth = addSolarSystemBody(STA_SOLAR_SYSTEM->lookup(STA_EARTH), sun);
    addSolarSystemBody(STA_SOLAR_SYSTEM->lookup(STA_MOON), earth);

    addSolarSystemBody(STA_SOLAR_SYSTEM->lookup(STA_MERCURY), sun);
    addSolarSystemBody(STA_SOLAR_SYSTEM->lookup(STA_VENUS),   sun);
    addSolarSystemBody(STA_SOLAR_SYSTEM->lookup(STA_MARS),    sun);
    addSolarSystemBody(STA_SOLAR_SYSTEM->lookup(STA_JUPITER), sun);
    addSolarSystemBody(STA_SOLAR_SYSTEM->lookup(STA_SATURN),  sun);
    addSolarSystemBody(STA_SOLAR_SYSTEM->lookup(STA_URANUS),  sun);
    addSolarSystemBody(STA_SOLAR_SYSTEM->lookup(STA_NEPTUNE), sun);
    addSolarSystemBody(STA_SOLAR_SYSTEM->lookup(STA_PLUTO),   sun);

    initializeStarCatalog("vis3d/tycho2.stars");
}


Body*
ThreeDView::addSolarSystemBody(const StaBody* body, Entity* center)
{
    const StaBody* staCenter = STA_SOLAR_SYSTEM->ssb();
    if (center->chronology()->firstArc())
    {
        StaBodyTrajectory* centerTraj = dynamic_cast<StaBodyTrajectory*>(center->chronology()->firstArc()->trajectory());
        if (centerTraj)
        {
            staCenter = centerTraj->body();
        }
    }

    Body* b = new Body();
    b->setName(body->name().toUtf8().data());
    Arc* arc = new Arc();
    arc->setDuration(ValidTimeSpan);
    arc->setCenter(center);
    arc->setTrajectory(new StaBodyTrajectory(body, staCenter));
    arc->setRotationModel(new StaBodyRotationModel(body));
    b->chronology()->setBeginning(BeginningOfTime);
    b->chronology()->addArc(arc);

    TextureProperties texProps;
    texProps.addressS = TextureProperties::Wrap;
    texProps.addressT = TextureProperties::Clamp;
    WorldGeometry* globe = new WorldGeometry();
    globe->setSphere(body->meanRadius());
    if (!body->baseTexture().isEmpty())
    {
        std::string textureName = body->baseTexture().toUtf8().data();
        globe->setBaseMap(m_textureLoader->loadTexture(textureName, texProps));
    }
    b->setGeometry(globe);

    m_universe->addEntity(b);

    return b;
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
    //equatorialGrid->setVisibility(true);
    equatorialGrid->setColor(Spectrum(0.0f, 0.0f, 1.0f));
    m_renderer->addSkyLayer(equatorialGrid);
}


void
ThreeDView::setCurrentTime(double mjd)
{
    double t = mjdToSecsSinceJ2000(mjd);
    qDebug() << "mjd: " << mjd << ", t: " << t << ", " << BeginningOfTime;
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
