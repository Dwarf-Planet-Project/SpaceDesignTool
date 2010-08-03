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


#include <vesta/Universe.h>
#include <vesta/UniverseRenderer.h>
#include <vesta/Observer.h>
#include <vesta/Body.h>
#include <vesta/Arc.h>
#include <vesta/Trajectory.h>
#include <vesta/FixedPointTrajectory.h>
#include <vesta/RotationModel.h>
#include <vesta/WorldGeometry.h>
#include <vesta/InertialFrame.h>
#include <vesta/BodyFixedFrame.h>
#include <vesta/CelestialCoordinateGrid.h>
#include <vesta/StarsLayer.h>
#include <vesta/DataChunk.h>
#include <vesta/DDSLoader.h>
#include <vesta/TextureMapLoader.h>
#include <vesta/TextureFont.h>
#include <vesta/Visualizer.h>
#include <vesta/TrajectoryGeometry.h>
#include <vesta/LabelGeometry.h>
#include <vesta/Units.h>
#include <vesta/interaction/ObserverController.h>

#include "ThreeDView.h"

#include <Main/propagatedscenario.h>
#include <Astro-Core/date.h>
#include <Astro-Core/stamath.h>
#include <Astro-Core/stabody.h>
#include <Astro-Core/ephemeris.h>

#include <QWheelEvent>
#include <QFile>
#include <QFileInfo>
#include <QDataStream>
#include <QDebug>

#include <cmath>

using namespace vesta;
using namespace Eigen;
using namespace std;

static double MJDBase = 2400000.5;

static double
yearsToSecs(double julianYears)
{
    return sta::daysToSecs(365.25 * julianYears);
}

static double
mjdToSecsSinceJ2000(double mjd)
{
    return sta::daysToSecs(mjd + (MJDBase - vesta::J2000));
}

static double
secsSinceJ2000ToMjd(double secs)
{
    return sta::secsToDays(secs) - (MJDBase - vesta::J2000);
}

static double BeginningOfTime = -yearsToSecs(200.0);  // 1900 CE
static double EndOfTime = yearsToSecs(100.0);         // 2100 CE
static double ValidTimeSpan = EndOfTime - BeginningOfTime;


/** Trajectory subclass that adapt's an STA mission arc for VESTA
  */
class StaMissionArcTrajectory : public Trajectory
{
public:
    StaMissionArcTrajectory(const MissionArc* missionArc) :
        m_missionArc(missionArc)
    {
    }

    virtual StateVector state(double t) const
    {
        // VESTA provides the time as the number of seconds since 1 Jan 2000 12:00:00 UTC.
        double mjd = secsSinceJ2000ToMjd(t);

        sta::StateVector sv;
        m_missionArc->getStateVector(mjd, &sv);
        return StateVector(sv.position, sv.velocity);
    }

    virtual double boundingSphereRadius() const
    {
        // Just return a very large value. For better performance, we should eventually return
        // a much tighter bounding sphere.
        return 1.0e10;
    }

    const MissionArc* missionArc()
    {
        return m_missionArc;
    }

private:
    const MissionArc* m_missionArc;

};


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


// Compute an orientation that will make an observer face in the directom from the 'from' point
// to the 'to' point, with the specified 'up' vector.
static Quaterniond
LookAt(const Vector3d& from, const Vector3d& to, const Vector3d& up)
{
    Vector3d direction = (to - from).normalized();
    Vector3d right = up.cross(direction).normalized();
    Vector3d u = direction.cross(right);

    Matrix3d m;
    m << right, u, direction;
    return Quaterniond(m);
}



ThreeDView::ThreeDView(const QGLFormat& format, QWidget* parent) :
    QGLWidget(format, parent),
    m_currentTime(0.0),
    m_universe(NULL),
    m_renderer(NULL),
    m_fov(sta::degToRad(60.0)),
    m_viewChanged(1)
{
    m_textureLoader = counted_ptr<TextureMapLoader>(new QtTextureLoader());

    m_universe = new Universe();
    m_universe->addRef();
    initializeUniverse();

    Entity* earth = findSolarSystemBody(STA_SOLAR_SYSTEM->earth());
    m_observer = counted_ptr<Observer>(new Observer(*earth));
    m_controller = counted_ptr<ObserverController>(new ObserverController());
    m_controller->setObserver(m_observer.ptr());

    gotoBody(STA_SOLAR_SYSTEM->earth());
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


static TextureFont*
LoadTextureFont(const QString& fileName)
{
    QFile fontFile(fileName);
    if (fontFile.open(QIODevice::ReadOnly))
    {
        QByteArray fontData = fontFile.readAll();
        DataChunk chunk(fontData.data(), fontData.size());
        return TextureFont::LoadTxf(&chunk);
    }
    else
    {
        qDebug() << "Error loading font file " << fileName;
        return NULL;
    }
}


void
ThreeDView::initializeGL()
{
    m_labelFont = LoadTextureFont("fonts/sans12.txf");

    m_renderer = new UniverseRenderer();
    initializeLayers();

    m_renderer->setAmbientLight(Spectrum::Flat(0.2f));

    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glEnable(GL_CULL_FACE);
}


void
ThreeDView::paintGL()
{
    if (m_viewChanged <= 0)
    {
        // No need to repaint
        return;
    }
    else
    {
        // Mark the view up to date
        m_viewChanged--;
    }

    glDepthMask(GL_TRUE);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    m_renderer->beginViewSet(*m_universe, m_currentTime);

    // Render a single view covering the entire widget
    Viewport viewport(size().width(), size().height());
    m_renderer->renderView(NULL, m_observer.ptr(), m_fov, viewport);

    m_renderer->endViewSet();

    drawOverlay();
}


void
ThreeDView::drawOverlay()
{
    int viewWidth = size().width();
    int viewHeight = size().height();
    glViewport(0, 0, viewWidth, viewHeight);

    glDisable(GL_DEPTH_TEST);
    glDisable(GL_LIGHTING);
    glEnable(GL_BLEND);
    if (GLEW_ARB_multisample)
    {
        glDisable(GL_MULTISAMPLE_ARB);
    }

    // Set up projection and model view matrices so that we're
    // drawing in window coordinates.
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D(0, viewWidth, 0, viewHeight);
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();
    glTranslatef(0.125f, 0.125f, 0);

    glEnable(GL_TEXTURE_2D);

    if (m_labelFont.isValid())
    {
        // Display the time
        QDateTime J2000(QDate(2000, 1, 1), QTime(12, 0, 0));
        J2000.setTimeSpec(Qt::UTC);
        QDateTime currentDateTime = J2000.addMSecs((qint64) (m_currentTime * 1000.0));
        m_labelFont->bind();
        m_labelFont->render(currentDateTime.toString("dd MMM yyyy hh:mm:ss").toLatin1().data(), Vector2f(10.0f, viewHeight - 25.0f));
        m_labelFont->render(QString("JD %1").arg(2451545.0 + sta::secsToDays(m_currentTime), 0, 'f', 5).toLatin1().data(),
                            Vector2f(10.0f, viewHeight - 40.0f));

        // Display the position of the viewer relative to the center
        // Vector3d position = m_observer->position();
        // m_labelFont->render(QString("%1 %2 %3").arg(position.x()).arg(position.y()).arg(position.z()).toUtf8().data(), Vector2f(10.0f, 10.0f));
    }

    // Restore graphics state and matrices
    glEnable(GL_DEPTH_TEST);
    if (GLEW_ARB_multisample)
    {
        glEnable(GL_MULTISAMPLE_ARB);
    }

    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
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
    bool rightButton = (event->buttons() & Qt::RightButton) != 0;

    if (leftButton)
    {
        double xrot = delta.y() / 80.0;
        double yrot = delta.x() / 80.0;

        m_controller->applyOrbitTorque(Vector3d::UnitX() * -xrot);
        m_controller->applyOrbitTorque(Vector3d::UnitY() * -yrot);
    }
    else if (rightButton)
    {
        double fovFactor = 1.0;//toDegrees(m_fovY) / 60.0;
        double xrot = delta.y() / 100.0 * fovFactor;
        double yrot = delta.x() / 100.0 * fovFactor;

        m_controller->pitch(xrot);
        m_controller->yaw(yrot);
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


// Find the VESTA body matching the specified STA body
vesta::Entity*
ThreeDView::findSolarSystemBody(const StaBody* body)
{
    return m_universe->findFirst(body->name().toUtf8().data());
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
            stars->addStar(id, float(sta::degToRad(ra)), float(sta::degToRad(dec)), apparentMagnitude, bvColorIndex);
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
    if (t != m_currentTime)
    {
        setViewChanged();
        m_currentTime = t;
        update();
    }
}


/** Mark the view as requiring a repaint.
  */
void
ThreeDView::setViewChanged()
{
    // Need to keep a counter rather than just a flag. Otherwise, both buffers in a double
    // buffered swap chain may not get repainted.
    m_viewChanged = 2;
}


void
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


void
ThreeDView::gotoBody(const StaBody* body)
{
    Entity* e = m_universe->findFirst(body->name().toUtf8().data());
    Entity* sun = m_universe->findFirst("Sun");

    if (e && sun)
    {
        float distance = 25000.0f;
        if (e->geometry())
        {
            distance = e->geometry()->boundingSphereRadius() * 4.0f;
        }

        // Position the observer on the sunlit side of the body
        m_observer->setCenter(e);
        m_observer->setPosition((sun->position(m_currentTime) - e->position(m_currentTime)).normalized() * distance);
        m_observer->setOrientation(LookAt(Vector3d::Zero(), m_observer->position(), Vector3d::UnitZ()));

        setViewChanged();
        update();
    }
}


void
ThreeDView::setScenario(PropagatedScenario* scenario)
{
    setViewChanged();

    clearScenarioObjects();
    if (!scenario)
    {
        return;
    }

    foreach (SpaceObject* spaceObj, scenario->spaceObjects())
    {
        Body* body = createSpaceObject(spaceObj);
        if (body)
        {
            m_universe->addEntity(body);
            m_scenarioSpaceObjects.insert(spaceObj, body);
        }
    }

    foreach (GroundObject* groundObj, scenario->groundObjects())
    {
        Body* body = createGroundObject(groundObj);
        if (body)
        {
            m_universe->addEntity(body);
            m_scenarioGroundObjects.insert(groundObj, body);
        }
    }
}


// Create a VESTA frame for the specified mission arc
Frame*
ThreeDView::createFrame(const MissionArc* arc)
{
    Frame* f = NULL;

    switch (arc->coordinateSystem().type())
    {
    case sta::COORDSYS_ECLIPTIC_J2000:
        f = InertialFrame::eclipticJ2000();
        break;

    case sta::COORDSYS_EME_J2000:
        f = InertialFrame::equatorJ2000();
        break;

    case sta::COORDSYS_BODYFIXED:
        {
            Entity* e = findSolarSystemBody(arc->centralBody());
            if (e)
            {
                f = new BodyFixedFrame(*e);
            }
        }
        break;

    default:
        qDebug() << "ThreeDView::createFrame(): Unsupported coordinate system.";
        break;
    }

    return f;
}


/** Create a VESTA body representing an STA SpaceObject.
  */
Body*
ThreeDView::createSpaceObject(const SpaceObject* spaceObj)
{
    // Don't realize space objects with empty trajectories
    if (spaceObj->mission().isEmpty())
    {
        return NULL;
    }

    Body* body = new Body();
    body->setName(spaceObj->name().toUtf8().data());

    foreach (MissionArc* missionArc, spaceObj->mission())
    {
        Entity* center = findSolarSystemBody(missionArc->centralBody());
        if (center)
        {
            Frame* frame = createFrame(missionArc);
            if (frame)
            {
                Arc* arc = new Arc();
                arc->setCenter(center);
                arc->setDuration(sta::daysToSecs(missionArc->ending() - missionArc->beginning()));
                arc->setTrajectoryFrame(frame);
                arc->setTrajectory(new StaMissionArcTrajectory(missionArc));
                body->chronology()->addArc(arc);
            }
        }
    }

    body->chronology()->setBeginning(mjdToSecsSinceJ2000(spaceObj->mission().first()->beginning()));

    Spectrum spaceObjColor(spaceObj->trajectoryColor().redF(),
                           spaceObj->trajectoryColor().greenF(),
                           spaceObj->trajectoryColor().blueF());

    LabelGeometry* label = new LabelGeometry(spaceObj->name().toUtf8().data(), m_labelFont.ptr(), spaceObjColor);
    body->setVisualizer("label", new Visualizer(label));

    // Add a trajectory visualizer for the first arc
    Arc* firstArc = body->chronology()->firstArc();
    TrajectoryGeometry* trajGeom = new TrajectoryGeometry();
    trajGeom->setDisplayedPortion(TrajectoryGeometry::StartToCurrentTime);
    trajGeom->setColor(spaceObjColor);
    trajGeom->computeSamples(firstArc->trajectory(),
                             body->chronology()->beginning(),
                             body->chronology()->beginning() + firstArc->duration(), 1000);
    string visName = QString("%1 trajectory").arg(spaceObj->name()).toUtf8().data();
    firstArc->center()->setVisualizer(visName, new Visualizer(trajGeom));

    return body;
}


/** Create a VESTA body representing an STA GroundObject.
  */
Body*
ThreeDView::createGroundObject(const GroundObject* groundObj)
{
    Body* body = new Body();
    body->setName(groundObj->name.toUtf8().data());

    Entity* center = findSolarSystemBody(groundObj->centralBody);
    if (center)
    {
        // Convert spherical coordinates to rectangular
        double latRadians = toRadians(groundObj->latitude);
        double lonRadians = toRadians(groundObj->longitude);
        Vector3d position(cos(latRadians) * cos(lonRadians),
                          cos(latRadians) * sin(lonRadians),
                          sin(latRadians));
        position = position.cwise() * groundObj->centralBody->radii();
        qDebug() << "ground object: " << position.x() << ", " << position.y() << ", " << position.z();

        Arc* arc = new Arc();
        arc->setCenter(center);
        arc->setDuration(ValidTimeSpan);
        arc->setTrajectory(new FixedPointTrajectory(position));
        arc->setTrajectoryFrame(new BodyFixedFrame(*center));

        body->chronology()->addArc(arc);
        body->chronology()->setBeginning(BeginningOfTime);
    }
    else
    {
        delete body;
        return NULL;
    }

    LabelGeometry* label = new LabelGeometry(groundObj->name.toUtf8().data(), m_labelFont.ptr(), Spectrum(1.0f, 1.0f, 1.0f));
    Visualizer* visualizer = new Visualizer(label);
    visualizer->setDepthAdjustment(Visualizer::AdjustToFront);
    body->setVisualizer("label", visualizer);

    return body;
}


void
ThreeDView::clearScenarioObjects()
{
    // Remove VESTA entities for all space and ground objects
    foreach (Entity* e, m_scenarioSpaceObjects.values())
    {
        m_universe->removeEntity(e);
    }

    foreach (Entity* e, m_scenarioGroundObjects.values())
    {
        m_universe->removeEntity(e);
    }

    m_scenarioSpaceObjects.clear();
    m_scenarioGroundObjects.clear();
}
