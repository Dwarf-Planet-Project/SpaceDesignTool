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
#include <vesta/FixedRotationModel.h>
#include <vesta/RotationModel.h>
#include <vesta/WorldGeometry.h>
#include <vesta/InertialFrame.h>
#include <vesta/BodyFixedFrame.h>
#include <vesta/TwoBodyRotatingFrame.h>
#include <vesta/CelestialCoordinateGrid.h>
#include <vesta/StarsLayer.h>
#include <vesta/SkyImageLayer.h>
#include <vesta/DataChunk.h>
#include <vesta/DDSLoader.h>
#include <vesta/TextureMapLoader.h>
#include <vesta/TextureFont.h>
#include <vesta/Visualizer.h>
#include <vesta/TrajectoryGeometry.h>
#include <vesta/LabelGeometry.h>
#include <vesta/PlaneVisualizer.h>
#include <vesta/PlanetGridLayer.h>
#include <vesta/AxesVisualizer.h>
#include <vesta/VelocityVisualizer.h>
#include <vesta/BodyDirectionVisualizer.h>
#include <vesta/SensorVisualizer.h>
#include <vesta/Units.h>
#include <vesta/MeshGeometry.h>
#include <vesta/PlanetaryRings.h>
#include <vesta/Atmosphere.h>
#include <vesta/CubeMapFramebuffer.h>
#include <vesta/HierarchicalTiledMap.h>
#include <vesta/interaction/ObserverController.h>

#include <qtvesta/NetworkTextureLoader.h>
#include <qtvesta/WMSTiledMap.h>
#include <qtvesta/MultiWMSTiledMap.h>

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
#include <QMenu>
#include <QCoreApplication>

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

static double BeginningOfTime = -yearsToSecs(200.0);  // 1900 CE
static double EndOfTime = yearsToSecs(100.0);         // 2100 CE
static double ValidTimeSpan = EndOfTime - BeginningOfTime;

static const int ReflectionMapSize = 512;
static const string DefaultSpacecraftMeshFile = "models/sorce.obj";

static StaBodyId PlanetIds[] = {
    STA_MERCURY, STA_VENUS, STA_EARTH, STA_MARS,
    STA_JUPITER, STA_SATURN, STA_URANUS, STA_NEPTUNE,
    STA_PLUTO
};

static const unsigned int PlanetCount = sizeof(PlanetIds) / sizeof(PlanetIds[0]);

static StaBodyId MoonIds[] = {
    STA_MOON,
    STA_IO, STA_EUROPA, STA_GANYMEDE, STA_CALLISTO,
    STA_MIMAS, STA_ENCELADUS, STA_TETHYS, STA_DIONE, STA_RHEA, STA_TITAN, STA_HYPERION, STA_IAPETUS, STA_PHOEBE
};

static const unsigned int MoonCount = sizeof(MoonIds) / sizeof(MoonIds[0]);


// Frame string must be one of equatorial, ecliptic, or galactic.
struct SkyLayerProperties
{
    string layerName;
    string textureFile;
    float opacity;
    string frame;
};

static SkyLayerProperties SkyLayers[] =
{
    { "milky way",  "textures/medres/milkyway.jpg",   0.3f, "galactic" },
    { "iras 100um", "textures/medres/iras-100um.jpg", 0.3f, "galactic" },
};

struct ModelProperties
{
    QString name;
    QString modelFileName;
    float scale;
    Vector3d nadirDirection;
    Vector3d leadingDirection;
};

static ModelProperties SpacecraftModels[] =
{
    { "default", "models/sorce.obj", 0.004f, -Vector3d::UnitY(),                    Vector3d::UnitX() },
    { "iss",     "models/iss.3ds",   0.025f,  Vector3d::UnitZ(),                    Vector3d::UnitX() },
    { "xmm",     "models/xmm.3ds",   0.004,  -Vector3d(0.0, 1.0, 1.0).normalized(), Vector3d::UnitX() },
};


// Get texture properties appropriate for planet maps: the map should
// wrap in longitude (so there's no seam on a meridian), but not in
// latitude.
TextureProperties planetTextureProperties()
{
    TextureProperties texProps;
    texProps.addressS = TextureProperties::Wrap;
    texProps.addressT = TextureProperties::Clamp;
    return texProps;
}


Spectrum
BodyLabelColor(StaBodyId id)
{
    switch (id)
    {
    case STA_SUN:       return Spectrum(1.0f, 1.0f, 0.0f);

    case STA_MERCURY:   return Spectrum(0.7f, 0.5f, 0.4f);
    case STA_VENUS:     return Spectrum(0.7f, 0.7f, 0.6f);
    case STA_EARTH:     return Spectrum(0.4f, 0.5f, 0.7f);
    case STA_MARS:      return Spectrum(0.7f, 0.4f, 0.3f);
    case STA_JUPITER:   return Spectrum(0.7f, 0.6f, 0.3f);
    case STA_SATURN:    return Spectrum(0.7f, 0.7f, 0.6f);
    case STA_URANUS:    return Spectrum(0.55f, 0.7f, 0.55f);
    case STA_NEPTUNE:   return Spectrum(0.5f, 0.5f, 0.7f);

    case STA_PLUTO:     return Spectrum(0.5f, 0.5f, 0.5f);

    case STA_MOON:      return Spectrum(0.6f, 0.6f, 0.6f);

    case STA_IO:        return Spectrum(0.9f, 0.9f, 0.6f);
    case STA_EUROPA:    return Spectrum(0.8f, 0.8f, 0.8f);
    case STA_GANYMEDE:  return Spectrum(0.8f, 0.5f, 0.4f);
    case STA_CALLISTO:  return Spectrum(0.6f, 0.5f, 0.3f);

    case STA_MIMAS:     return Spectrum(0.9f, 0.9f, 0.9f);
    case STA_ENCELADUS: return Spectrum(0.8f, 0.8f, 1.0f);
    case STA_TETHYS:    return Spectrum(0.8f, 0.5f, 0.4f);
    case STA_DIONE:     return Spectrum(0.6f, 0.5f, 0.3f);
    case STA_RHEA:      return Spectrum(0.8f, 0.8f, 0.6f);
    case STA_TITAN:     return Spectrum(0.8f, 0.6f, 0.3f);
    case STA_HYPERION:  return Spectrum(0.6f, 0.5f, 0.4f);
    case STA_IAPETUS:   return Spectrum(0.6f, 0.5f, 0.3f);
    case STA_PHOEBE:    return Spectrum(0.5f, 0.5f, 0.5f);

    default:            return Spectrum(0.5f, 0.5f, 0.5f);
    }
}


/** Trajectory subclass that adapt's an STA mission arc for VESTA
  */
class StaMissionArcTrajectory : public Trajectory
{
public:
    StaMissionArcTrajectory(const MissionArc* missionArc, double boundingRadius) :
        m_missionArc(missionArc),
        m_boundingRadius(boundingRadius)
    {
    }

    virtual StateVector state(double t) const
    {
        // VESTA provides the time as the number of seconds since 1 Jan 2000 12:00:00 UTC.
        double mjd = sta::SecJ2000ToMjd(t);

        sta::StateVector sv;
        if (!m_missionArc->getStateVector(mjd, &sv))
        {
            qDebug() << "Failed to get state vector for mission arc (probably because of a bad trajectory)";
        }

        return StateVector(sv.position, sv.velocity);
    }

    virtual double boundingSphereRadius() const
    {
        return m_boundingRadius;
    }

    const MissionArc* missionArc()
    {
        return m_missionArc;
    }

private:
    const MissionArc* m_missionArc;
    double m_boundingRadius;
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
        const sta::Ephemeris* ephemeris = m_body->ephemeris();
        if (ephemeris)
        {
            sta::Ephemeris::TimeInterval interval = ephemeris->validTimeInterval(m_body);
            setValidTimeRange(sta::MjdToSecJ2000(interval.start()), sta::MjdToSecJ2000(interval.end()));
        }
    }

    virtual StateVector state(double t) const
    {
        // VESTA provides the time as the number of seconds since 1 Jan 2000 12:00:00 UTC.
        double mjd = sta::SecJ2000ToMjd(t);
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
        // Return a conservative estimate for orbit size; use the mean
        // distance of the object from the central body plus 50%.
        return m_body->distance() * 1.5;
    }

    virtual double period() const
    {
        return daysToSeconds(m_body->orbitalPeriod());
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
        double mjd = sta::SecJ2000ToMjd(t);
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


// Special version of VESTA's label geometry that changes color according
// to the currently active mission arc.
//
// TODO: VESTA will eventually have a TimeSwitchedGeometry class which will
// make this special case unnecessary.
class MulticolorLabelGeometry : public LabelGeometry
{
public:
    MulticolorLabelGeometry(const SpaceObject* spaceObj, LabelGeometry* label) :
        m_spaceObject(spaceObj),
        m_label(label)
    {
    }

    LabelGeometry* label() const
    {
        return m_label.ptr();
    }

    virtual void render(RenderContext& rc,
                        double t) const
    {
        double mjd = sta::SecJ2000ToMjd(t);

        // Linear search for the current arc
        Spectrum color;
        foreach (MissionArc* arc, m_spaceObject->mission())
        {
            if (mjd >= arc->beginning() && mjd < arc->ending())
            {
                QColor arcColor = arc->arcTrajectoryColor();
                color = Spectrum(arcColor.redF(), arcColor.greenF(), arcColor.blueF());
            }
        }

        // Set the label and icon to the same color
        m_label->setColor(color);
        m_label->setIconColor(color);

        m_label->render(rc, t);
    }

    virtual float boundingSphereRadius() const
    {
        return m_label->boundingSphereRadius();
    }

    virtual bool isOpaque() const
    {
        return m_label->isOpaque();
    }

    virtual float apparentSize() const
    {
        return m_label->apparentSize();
    }

private:
    const SpaceObject* m_spaceObject;
    counted_ptr<LabelGeometry> m_label;
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


// Orient spacecraft model so that the nadir direction of the model points in the local
// x- direction and leading direction points in the direction of local +y.
static Quaterniond
SpacecraftOrientation(const Vector3d& nadirDirection, const Vector3d& leadingDirection)
{
    Vector3d xAxis = nadirDirection;
    Vector3d zAxis = xAxis.cross(leadingDirection);
    if (zAxis.isZero())
    {
        return Quaterniond::Identity();
    }

    zAxis.normalize();
    Vector3d yAxis = zAxis.cross(xAxis);

    Matrix3d m;
    m << xAxis, yAxis, zAxis;
    return Quaterniond(m).conjugate();
}


ThreeDView::ThreeDView(const QGLFormat& format, QWidget* parent) :
    QGLWidget(format, parent),
    m_currentTime(0.0),
    m_universe(NULL),
    m_renderer(NULL),
    m_fov(sta::degToRad(50.0)),
    m_mouseMotion(0.0f),
    m_viewChanged(1),
    m_selectedSpacecraft(NULL),
    m_satelliteTrajectoriesEnabled(false),
    m_shadowsEnabled(false),
    m_reflectionsEnabled(false),
    m_glInitialized(false),
    m_shadowsInitialized(false),
    m_sensorFovsEnabled(false),
    m_stereoMode(NoStereo),
    m_rollLeftKey(false),
    m_rollRightKey(false),
    m_pitchDownKey(false),
    m_pitchUpKey(false)
{
    m_textureLoader = counted_ptr<NetworkTextureLoader>(new NetworkTextureLoader(this, true));
    m_textureLoader->addLocalSearchPath("models");

    m_labelFont = new TextureFont();
    initializeStandardResources();

    m_universe = new Universe();
    m_universe->addRef();
    initializeUniverse();
    initializeLayers();
    initializeMapLayers();

    m_renderer = new UniverseRenderer();
    m_renderer->setAmbientLight(Spectrum::Flat(0.2f));

    Entity* earth = findSolarSystemBody(STA_SOLAR_SYSTEM->earth());
    m_observer = counted_ptr<Observer>(new Observer(earth));
    m_controller = counted_ptr<ObserverController>(new ObserverController());
    m_controller->setObserver(m_observer.ptr());

    gotoBody(STA_SOLAR_SYSTEM->earth());

    setMouseTracking(true);
    setFocusPolicy(Qt::ClickFocus);
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


static void
LoadTextureFont(TextureFont* font, const QString& fileName)
{
    QFile fontFile(fileName);
    if (fontFile.open(QIODevice::ReadOnly))
    {
        QByteArray fontData = fontFile.readAll();
        DataChunk chunk(fontData.data(), fontData.size());
        font->loadTxf(&chunk);
    }
    else
    {
        qDebug() << "Error loading font file " << fileName;
    }
}


void
ThreeDView::initializeGL()
{
    if (!m_renderer->initializeGraphics())
    {
        qDebug() << "Error initializing VESTA graphics";
    }

    LoadTextureFont(m_labelFont.ptr(), "vis3d/sans12.txf");

    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glEnable(GL_CULL_FACE);

    m_glInitialized = true;
}


void
ThreeDView::paintGL()
{
    m_textureLoader->incrementFrameCount();
    m_textureLoader->evictTextures();

    if (m_textureLoader->realizeLoadedTextures() > 0)
    {
        setViewChanged();
    }

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

    // Shadow setup is deferred until they are actually enabled
    if (m_shadowsEnabled && !m_shadowsInitialized)
    {
        m_shadowsInitialized = true;
        if (m_renderer->shadowsSupported())
        {
            m_renderer->initializeShadowMaps(2048);
            m_renderer->setShadowsEnabled(true);
        }
    }

    // Reflection map setup is deferred until they are actually enabled
    if (m_reflectionsEnabled && m_reflectionMap.isNull())
    {
        if (CubeMapFramebuffer::supported())
        {
            m_reflectionMap = CubeMapFramebuffer::CreateCubicReflectionMap(ReflectionMapSize, TextureMap::R8G8B8A8);
        }
    }

    // Process atmospheres that were just loaded
    if (!m_newAtmospheres.isEmpty())
    {
        foreach (Atmosphere* atmosphere, m_newAtmospheres)
        {
            atmosphere->generateTextures();
        }
        m_newAtmospheres.clear();
    }

    updateSolarSystemTrajectoryPlots();

    glDepthMask(GL_TRUE);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    m_renderer->beginViewSet(m_universe, m_currentTime);

    // Draw the reflection map if reflections are enabled
    if (m_reflectionsEnabled && !m_reflectionMap.isNull())
    {
        // Render without visualizers or other non-physical geometry. Draw
        // from the observer's position, which will produce approximately
        // correct reflections for realistically sized spacecraft.
        Vector3d reflectionCenter = m_observer->absolutePosition(m_currentTime);
        m_renderer->setVisualizersEnabled(false);
        m_renderer->setSkyLayersEnabled(false);

        // Set the near clip distance to a large value so that only background
        // objects (like planets) are shown.
        m_renderer->renderCubeMap(NULL, reflectionCenter, m_reflectionMap.ptr(), 1.0f);

        // TODO: worth generating mipmaps?

        m_renderer->setVisualizersEnabled(true);
        m_renderer->setSkyLayersEnabled(true);
    }

    // Render a single view covering the entire widget
    Viewport viewport(size().width(), size().height());

    LightingEnvironment lighting;
    if (m_reflectionsEnabled && m_reflectionMap.isValid())
    {
        // Add reflection map info to the lighting evironment
        lighting.reset();
        ReflectionRegion cameraRegion;
        cameraRegion.cubeMap = m_reflectionMap->colorTexture();
        cameraRegion.region = BoundingSphere<float>(Vector3f::Zero(), 1.0f);
        lighting.reflectionRegions().push_back(cameraRegion);
    }

    if (m_stereoMode == NoStereo)
    {
        m_renderer->renderView(&lighting, m_observer.ptr(), m_fov, viewport);
    }
    else
    {
        Quaterniond cameraOrientation = m_observer->absoluteOrientation(m_currentTime);
        Vector3d cameraPosition = m_observer->absolutePosition(m_currentTime);
        double eyeSeparation = m_observer->position().norm() / 50.0;
        float focalPlaneDistance = eyeSeparation * 25.0f;
        float nearDistance = 0.00001f;
        float farDistance = 1.0e12f;
        float y = tan(0.5f * m_fov) * nearDistance;
        float x = y * viewport.aspectRatio();

        float stereoOffset = float(eyeSeparation) * nearDistance / focalPlaneDistance;

        PlanarProjection leftProjection(PlanarProjection::Perspective,  -x + float(stereoOffset), x, -y, y, nearDistance, farDistance);
        PlanarProjection rightProjection(PlanarProjection::Perspective, -x, x -float(stereoOffset), -y, y, nearDistance, farDistance);

        Vector3d leftEyePosition = cameraPosition + cameraOrientation * (Vector3d::UnitX() * -eyeSeparation);
        Vector3d rightEyePosition = cameraPosition + cameraOrientation * (Vector3d::UnitX() * eyeSeparation);

        switch (m_stereoMode)
        {
        case AnaglyphRedCyan:
            glColorMask(GL_TRUE, GL_FALSE, GL_FALSE, GL_TRUE); break;
        case AnaglyphCyanRed:
            glColorMask(GL_FALSE, GL_TRUE, GL_TRUE, GL_TRUE); break;
        case AnaglyphGreenMagenta:
            glColorMask(GL_FALSE, GL_TRUE, GL_FALSE, GL_TRUE); break;
        case AnaglyphMagentaGreen:
            glColorMask(GL_TRUE, GL_FALSE, GL_TRUE, GL_TRUE); break;
        default:
            break;
        }
        m_renderer->renderView(&lighting, leftEyePosition, cameraOrientation, leftProjection, viewport);

        glDepthMask(GL_TRUE);
        glClear(GL_DEPTH_BUFFER_BIT);

        switch (m_stereoMode)
        {
        case AnaglyphRedCyan:
            glColorMask(GL_FALSE, GL_TRUE, GL_TRUE, GL_TRUE); break;
        case AnaglyphCyanRed:
            glColorMask(GL_TRUE, GL_FALSE, GL_FALSE, GL_TRUE); break;
        case AnaglyphGreenMagenta:
            glColorMask(GL_TRUE, GL_FALSE, GL_TRUE, GL_TRUE); break;
        case AnaglyphMagentaGreen:
            glColorMask(GL_FALSE, GL_TRUE, GL_FALSE, GL_TRUE); break;
        default:
            break;
        }
        m_renderer->renderView(&lighting, rightEyePosition, cameraOrientation, rightProjection, viewport);

        glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
    }


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
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
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

    // Offset by a fraction of a pixel in order to avoid blurring of the text
    glTranslatef(0.125f, 0.125f, 0);

    glEnable(GL_TEXTURE_2D);
    glColor4f(1.0f, 1.0f, 1.0f, 1.0f);

    if (m_labelFont.isValid())
    {
        float fontHeight = floor(m_labelFont->lookupGlyph('M')->size.y() * 1.5f);

        // Display the time
        QDateTime J2000(QDate(2000, 1, 1), QTime(12, 0, 0));
        J2000.setTimeSpec(Qt::UTC);
        QDateTime currentDateTime = J2000.addMSecs((qint64) (m_currentTime * 1000.0));
        m_labelFont->bind();
        m_labelFont->render(currentDateTime.toString("dd MMM yyyy hh:mm:ss 'UTC'").toLatin1().data(), Vector2f(10.0f, viewHeight - 25.0f));
        m_labelFont->render(QString("JD %1").arg(2451545.0 + sta::secsToDays(m_currentTime), 0, 'f', 5).toLatin1().data(),
                            Vector2f(10.0f, viewHeight - 25.0f - fontHeight));

        bool showDistance = true;
        bool showSpeed = true;

        unsigned int textRows = 0;
        if (showDistance)
            textRows++;
        if (showSpeed)
            textRows++;

        if (m_selectedSpacecraft)
        {
            if (m_scenarioSpaceObjects.contains(m_selectedSpacecraft))
            {
                Entity* selectedBody = m_scenarioSpaceObjects.value(m_selectedSpacecraft);
                if (selectedBody)
                {
                    unsigned int row = textRows;
                    m_labelFont->render(selectedBody->name().c_str(), Vector2f(10.0f, 10 + row * fontHeight));
                    row--;

                    double beginning = selectedBody->chronology()->beginning();
                    if (m_currentTime >= beginning && m_currentTime < beginning + selectedBody->chronology()->duration())
                    {
                        StateVector sv = selectedBody->state(m_currentTime);

                        vesta::Arc* currentArc = selectedBody->chronology()->activeArc(m_currentTime);
                        Entity* centralBody = currentArc->center();
                        StateVector centerSv = centralBody->state(m_currentTime);

                        double r = (sv.position() - centerSv.position()).norm();
                        double speed = (sv.velocity() - centerSv.velocity()).norm();
                        if (dynamic_cast<WorldGeometry*>(centralBody->geometry()))
                        {
                            WorldGeometry* globe = dynamic_cast<WorldGeometry*>(centralBody->geometry());
                            r -= globe->meanRadius();
                        }

                        if (showDistance)
                        {
                            m_labelFont->render(QString("Distance to %1: %2 km").arg(centralBody->name().c_str()).arg(r, 0, 'f', 2).toLatin1().data(),
                                                Vector2f(10.0f, 10.0f + row * fontHeight));
                            row--;
                        }

                        if (showSpeed)
                        {
                            m_labelFont->render(QString("Speed: %1 km/s").arg(speed, 0, 'f', 3).toLatin1().data(),
                                                Vector2f(10.0f, 10.0f + row * fontHeight));
                            row--;
                        }
                    }
                }
            }
        }
        // Display the position of the viewer relative to the center
        // Vector3d position = m_observer->position();
        // m_labelFont->render(QString("%1 %2 %3").arg(position.x()).arg(position.y()).arg(position.z()).toUtf8().data(), Vector2f(10.0f, 10.0f));

        // Show the number of texture map tiles being loaded
        {
            unsigned int tileCount = m_textureLoader->wmsHandler()->pendingTileCount();
            if (tileCount > 0)
            {
                QString status = QString("Downloading map tiles: %1").arg(tileCount);
                m_labelFont->render(status.toLatin1().data(), Vector2f(float(width() / 2), 10.0f));
            }
        }
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
    setViewChanged();
}


void
ThreeDView::mousePressEvent(QMouseEvent *event)
{
    m_mousePosition = event->posF();
    m_mouseMotion = 0.0f;
}


void
ThreeDView::mouseReleaseEvent(QMouseEvent* event)
{
    if (m_mouseMotion < 3.0f)
    {
        if (event->button() == Qt::RightButton)
        {
            // Right click brings up the context menu
            QContextMenuEvent menuEvent(QContextMenuEvent::Other, event->pos(), event->globalPos());
            QCoreApplication::sendEvent(this, &menuEvent);
        }
        else if (event->button() == Qt::LeftButton)
        {
            // Left click selects a spacecraft
            Entity* hit = pickObject(event->pos());
            if (hit)
            {
                foreach (SpaceObject* spaceObj, m_scenarioSpaceObjects.keys())
                {
                    if (m_scenarioSpaceObjects[spaceObj] == hit)
                    {
                        selectParticipant(spaceObj);
                        break;
                    }
                }
            }
        }
    }
}


void
ThreeDView::mouseMoveEvent(QMouseEvent *event)
{
    Vector2f delta(event->posF().x() - m_mousePosition.x(),
                   event->posF().y() - m_mousePosition.y());
    m_mousePosition = event->posF();
    m_mouseMotion += delta.norm();

    bool leftButton = (event->buttons() & Qt::LeftButton) != 0;
    bool rightButton = (event->buttons() & Qt::RightButton) != 0;
    bool pan = false;
    bool orbit = false;

    if (rightButton || (event->modifiers() & Qt::AltModifier) != 0)
    {
        pan = true;
    }
    else if (leftButton || (event->modifiers() & Qt::ShiftModifier) != 0)
    {
        orbit = true;
    }

    if (orbit)
    {
        double xrot = delta.y() / 80.0;
        double yrot = delta.x() / 80.0;

        // Slow the movement when the observer is near the surface of a planet
        Entity* center = m_observer->center();
        if (center)
        {
            WorldGeometry* world = dynamic_cast<WorldGeometry*>(center->geometry());
            if (world)
            {
                double distance = m_observer->position().norm() - world->maxRadius();
                float scale = distance / (world->maxRadius() * 0.1f);
                scale = min(1.0f, max(0.0f, scale));
                xrot *= scale;
                yrot *= scale;
            }
        }

        m_controller->applyOrbitTorque(Vector3d::UnitX() * -xrot);
        m_controller->applyOrbitTorque(Vector3d::UnitY() * -yrot);
    }

    if (pan)
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
        double zoomFactor = pow(1.03f, clicks / 50.0f);

        m_controller->dolly(zoomFactor);

        update();
    }
}


void
ThreeDView::keyPressEvent(QKeyEvent* event)
{
    setViewChanged();
    switch (event->key())
    {
    case Qt::Key_Left:
        m_rollLeftKey = true;
        break;
    case Qt::Key_Right:
        m_rollRightKey = true;
        break;
    case Qt::Key_Up:
        m_pitchUpKey = true;
        break;
    case Qt::Key_Down:
        m_pitchDownKey = true;
        break;
    default:
        QWidget::keyPressEvent(event);
        break;
    }
}


void
ThreeDView::keyReleaseEvent(QKeyEvent* event)
{
    switch (event->key())
    {
    case Qt::Key_Left:
        m_rollLeftKey = false;
        break;
    case Qt::Key_Right:
        m_rollRightKey = false;
        break;
    case Qt::Key_Up:
        m_pitchUpKey = false;
        break;
    case Qt::Key_Down:
        m_pitchDownKey = false;
        break;
    default:
        QWidget::keyReleaseEvent(event);
        break;
    }
}


void
ThreeDView::contextMenuEvent(QContextMenuEvent* event)
{
    if (event->reason() == QContextMenuEvent::Mouse)
    {
        // Don't popup the context menu when triggered by the mouse,
        // as this interferes with right dragging to pan the view.
        return;
    }

    Entity* hit = pickObject(event->pos());
    if (hit)
    {
        QMenu* menu = new QMenu(this);
        QAction* nameAction = menu->addAction(hit->name().c_str());
        nameAction->setEnabled(false);

        menu->addSeparator();

        QAction* bodyAxesAction       = menu->addAction("Show Body Axes");
        QAction* frameAxesAction      = menu->addAction("Show Frame Axes");
        QAction* velocityDirectionAction       = menu->addAction("Show Velocity Direction");
        QAction* sunDirectionAction   = menu->addAction("Show Sun Direction");
        QAction* earthDirectionAction = menu->addAction("Show Earth Direction");

        menu->addSeparator();

        QAction* centerAction         = menu->addAction(tr("%1 Centered Observer").arg(hit->name().c_str()));
        QAction* fixedAction          = menu->addAction(tr("%1 Fixed Observer").arg(hit->name().c_str()));
        QAction* rotatingAction       = menu->addAction(tr("%1 Rotating Observer").arg(hit->name().c_str()));

        bool hasBodyAxes = hit->visualizer("body axes") != NULL;
        bool hasFrameAxes = hit->visualizer("frame axes") != NULL;
        bool hasVelocityDirection = hit->visualizer("velocity") != NULL;
        bool hasSunDirection = hit->visualizer("sun direction") != NULL;
        bool hasEarthDirection = hit->visualizer("earth direction") != NULL;
        bodyAxesAction->setCheckable(true);
        bodyAxesAction->setChecked(hasBodyAxes);
        frameAxesAction->setCheckable(true);
        frameAxesAction->setChecked(hasFrameAxes);
        velocityDirectionAction->setCheckable(true);
        velocityDirectionAction->setChecked(hasVelocityDirection);
        sunDirectionAction->setCheckable(true);
        sunDirectionAction->setChecked(hasSunDirection);
        earthDirectionAction->setCheckable(true);
        earthDirectionAction->setChecked(hasEarthDirection);

        // Disable Sun direction action for the Sun, etc.
        if (hit->name() == "Sun")
        {
            sunDirectionAction->setDisabled(true);
        }

        if (hit->name() == "Earth")
        {
            earthDirectionAction->setDisabled(true);
        }

        // Visualizer size is based on the geometry size
        double arrowSize = 1.0;
        if (hit->geometry())
        {
            arrowSize = hit->geometry()->boundingSphereRadius() * 2.0;
        }

        QAction* chosenAction = menu->exec(event->globalPos(), bodyAxesAction);
        if (chosenAction == bodyAxesAction)
        {
            if (chosenAction->isChecked())
            {
                AxesVisualizer* axes = new AxesVisualizer(AxesVisualizer::BodyAxes, arrowSize);
                axes->setVisibility(true);
                hit->setVisualizer("body axes", axes);
            }
            else
            {
                hit->removeVisualizer("body axes");
            }
        }
        else if (chosenAction == frameAxesAction)
        {
            if (chosenAction->isChecked())
            {
                AxesVisualizer* axes = new AxesVisualizer(AxesVisualizer::FrameAxes, arrowSize);
                axes->setVisibility(true);
                axes->arrows()->setOpacity(0.33f);
                hit->setVisualizer("frame axes", axes);
            }
            else
            {
                hit->removeVisualizer("frame axes");
            }
        }
        else if (chosenAction == velocityDirectionAction)
        {
            if (chosenAction->isChecked())
            {
                VelocityVisualizer* velocityVis = new VelocityVisualizer(arrowSize);
                velocityVis->setVisibility(true);
                hit->setVisualizer("velocity", velocityVis);
            }
            else
            {
                hit->removeVisualizer("velocity");
            }
        }
        else if (chosenAction == sunDirectionAction)
        {
            if (chosenAction->isChecked())
            {
                BodyDirectionVisualizer* vis = new BodyDirectionVisualizer(arrowSize, m_universe->findFirst("Sun"));
                vis->setColor(Spectrum(1.0f, 1.0f, 0.5f));
                vis->setVisibility(true);
                hit->setVisualizer("sun direction", vis);
            }
            else
            {
                hit->removeVisualizer("sun direction");
            }
        }
        else if (chosenAction == earthDirectionAction)
        {
            if (chosenAction->isChecked())
            {
                BodyDirectionVisualizer* vis = new BodyDirectionVisualizer(arrowSize, m_universe->findFirst("Earth"));
                vis->setColor(Spectrum(0.5f, 0.5f, 1.0f));
                vis->setVisibility(true);
                hit->setVisualizer("earth direction", vis);
            }
            else
            {
                hit->removeVisualizer("earth direction");
            }
        }
        else if (chosenAction == centerAction)
        {
            setObserverCenter(hit);
            setInertialObserver();
        }
        else if (chosenAction == fixedAction)
        {
            setObserverCenter(hit);
            setBodyFixedObserver();
        }
        else if (chosenAction == fixedAction)
        {
            setObserverCenter(hit);
            setBodyFixedObserver();
        }
        else if (chosenAction == rotatingAction)
        {
            vesta::Arc* currentArc = hit->chronology()->activeArc(m_currentTime);
            if (currentArc)
            {
                Entity* center = currentArc->center();
                TwoBodyRotatingFrame* f = new TwoBodyRotatingFrame(hit, center);
                setObserverCenter(center);
                m_observer->updatePositionFrame(f, m_currentTime);
                m_observer->updatePointingFrame(f, m_currentTime);
            }
        }

        setViewChanged();
    }
}


void
ThreeDView::initializeUniverse()
{
    // Initialize planet grid layer
    m_planetGrid = new PlanetGridLayer();

    // Create the Solar System Barycenter
    Entity* ssb = new Entity();
    ssb->setName("SSB");
    ssb->chronology()->addArc(new vesta::Arc());
    ssb->chronology()->setBeginning(BeginningOfTime);
    ssb->chronology()->firstArc()->setDuration(ValidTimeSpan);
    m_universe->addEntity(ssb);

    Body* sun = addSolarSystemBody(STA_SOLAR_SYSTEM->lookup(STA_SUN), ssb);
    Body* earth = addSolarSystemBody(STA_SOLAR_SYSTEM->lookup(STA_EARTH), sun);
    Body* moon = addSolarSystemBody(STA_SOLAR_SYSTEM->lookup(STA_MOON), earth);

    addSolarSystemBody(STA_SOLAR_SYSTEM->lookup(STA_MERCURY), sun);
    addSolarSystemBody(STA_SOLAR_SYSTEM->lookup(STA_VENUS),   sun);
    addSolarSystemBody(STA_SOLAR_SYSTEM->lookup(STA_MARS),    sun);
    Body* jupiter = addSolarSystemBody(STA_SOLAR_SYSTEM->lookup(STA_JUPITER), sun);
    Body* saturn  = addSolarSystemBody(STA_SOLAR_SYSTEM->lookup(STA_SATURN),  sun);
    addSolarSystemBody(STA_SOLAR_SYSTEM->lookup(STA_URANUS),  sun);
    addSolarSystemBody(STA_SOLAR_SYSTEM->lookup(STA_NEPTUNE), sun);
    addSolarSystemBody(STA_SOLAR_SYSTEM->lookup(STA_PLUTO),   sun);

    // Jovian system
    addSolarSystemBody(STA_SOLAR_SYSTEM->lookup(STA_IO),       jupiter);
    addSolarSystemBody(STA_SOLAR_SYSTEM->lookup(STA_EUROPA),   jupiter);
    addSolarSystemBody(STA_SOLAR_SYSTEM->lookup(STA_GANYMEDE), jupiter);
    addSolarSystemBody(STA_SOLAR_SYSTEM->lookup(STA_CALLISTO), jupiter);

    // Saturnian system
    addSolarSystemBody(STA_SOLAR_SYSTEM->lookup(STA_MIMAS),     saturn);
    addSolarSystemBody(STA_SOLAR_SYSTEM->lookup(STA_ENCELADUS), saturn);
    addSolarSystemBody(STA_SOLAR_SYSTEM->lookup(STA_TETHYS),    saturn);
    addSolarSystemBody(STA_SOLAR_SYSTEM->lookup(STA_DIONE),     saturn);
    addSolarSystemBody(STA_SOLAR_SYSTEM->lookup(STA_RHEA),      saturn);
    addSolarSystemBody(STA_SOLAR_SYSTEM->lookup(STA_TITAN),     saturn);
    addSolarSystemBody(STA_SOLAR_SYSTEM->lookup(STA_HYPERION),  saturn);
    addSolarSystemBody(STA_SOLAR_SYSTEM->lookup(STA_IAPETUS),   saturn);
    addSolarSystemBody(STA_SOLAR_SYSTEM->lookup(STA_PHOEBE),    saturn);

    initializeStarCatalog("vis3d/tycho2.stars");

    // Add rings for Saturn
    if (saturn && dynamic_cast<WorldGeometry*>(saturn->geometry()))
    {
        PlanetaryRings* rings = new PlanetaryRings(74500.0f, 140220.0f);
        rings->setTexture(m_textureLoader->loadTexture("textures/medres/saturn-rings.png", TextureProperties(TextureProperties::Clamp)));
        dynamic_cast<WorldGeometry*>(saturn->geometry())->setRingSystem(rings);
    }

    // Make the Sun glow
    dynamic_cast<WorldGeometry*>(sun->geometry())->setEmissive(true);

    // Add high resolution tiled texture for the Earth. The map uses BMNG for levels 0-6, and the global
    // mosaic for more detailed levels.
    dynamic_cast<WorldGeometry*>(earth->geometry())->setBaseMap(new MultiWMSTiledMap(m_textureLoader.ptr(),
                                                                                     "bmng-apr-nb", 7,
                                                                                     "earth-global-mosaic", 13,
                                                                                     480));

    // Add high resolution tiled texture for the Moon: 6 levels, 512x512 tiles
    dynamic_cast<WorldGeometry*>(moon->geometry())->setBaseMap(new WMSTiledMap(m_textureLoader.ptr(),
                                                                               "moon-clementine", 512, 6));


    createOrbitVisualizer(STA_SOLAR_SYSTEM->sun(), BodyLabelColor(STA_SUN));
    for (unsigned int i = 0; i < PlanetCount; ++i)
    {
        createOrbitVisualizer(STA_SOLAR_SYSTEM->lookup(PlanetIds[i]), BodyLabelColor(PlanetIds[i]));
    }

    for (unsigned int i = 0; i < MoonCount; ++i)
    {
        createOrbitVisualizer(STA_SOLAR_SYSTEM->lookup(MoonIds[i]), BodyLabelColor(MoonIds[i]));
    }
}


Body*
ThreeDView::addSolarSystemBody(const StaBody* body, Entity* center)
{
    if (!body->ephemeris())
    {
        // No ephemeris defined for the body, so don't create it in VESTA
        qDebug() << "No ephemeris for " << body->name();
        return NULL;
    }

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
    vesta::Arc* arc = new vesta::Arc();
    arc->setDuration(ValidTimeSpan);
    arc->setCenter(center);
    arc->setTrajectory(new StaBodyTrajectory(body, staCenter));
    arc->setRotationModel(new StaBodyRotationModel(body));
    b->chronology()->setBeginning(BeginningOfTime);
    b->chronology()->addArc(arc);

    WorldGeometry* globe = new WorldGeometry();
    if (body->id() < STA_JUPITER)
    {
        // For now, make the inner planets spherical
        globe->setSphere(body->meanRadius());
    }
    else
    {
        globe->setEllipsoid(body->radii().cast<float>() * 2.0f);
    }
    if (!body->baseTexture().isEmpty())
    {
        std::string textureName = body->baseTexture().toUtf8().data();
        globe->setBaseMap(m_textureLoader->loadTexture(textureName, planetTextureProperties()));
    }
    b->setGeometry(globe);

    // Add planetographic grid for all bodies except for the Sun
    if (body->id() != STA_SUN)
    {
        globe->setLayer("grid", m_planetGrid.ptr());
    }

    m_universe->addEntity(b);

    return b;
}


void
ThreeDView::createOrbitVisualizer(const StaBody* stabody, const Spectrum& color)
{
    Entity* body = findSolarSystemBody(stabody);
    if (!body)
    {
        return;
    }

    vesta::Arc* arc = body->chronology()->firstArc();

    if (stabody->id() != STA_SUN)
    {
        TrajectoryGeometry* trajGeom = new TrajectoryGeometry();
        trajGeom->setDisplayedPortion(TrajectoryGeometry::WindowBeforeCurrentTime);
        trajGeom->setWindowDuration(arc->trajectory()->period() * 0.98);
        trajGeom->setColor(color);
        trajGeom->setFadeFraction(0.25);
        //trajGeom->setLineWidth(1.5f);

        Visualizer* trajVisualizer = new Visualizer(trajGeom);
        trajVisualizer->setVisibility(false);

        arc->center()->setVisualizer(string("traj ") + body->name(), trajVisualizer);
    }

    LabelGeometry* label = new LabelGeometry(stabody->name().toUtf8().data(), m_labelFont.ptr(), color, 7.0f);
    label->setIcon(m_planetIcon.ptr());
    label->setIconColor(color);

    if (stabody->id() == STA_SUN)
    {
        label->setFadeSize(stabody->meanRadius());
        label->setFadeRange(new FadeRange(0.0f, 20.0f, 0.0f, 20.0f));
    }
    else
    {
        // To avoid clutter, set the labels to fade out when then viewer is far
        // away or very close to a planet.
        float fadeSize = arc->trajectory()->boundingSphereRadius();
        float minPixels = 20.0f;
        float maxPixels = 20.0f * fadeSize / stabody->meanRadius();
        label->setFadeSize(fadeSize);
        label->setFadeRange(new FadeRange(minPixels, maxPixels, minPixels, maxPixels));
    }

    Visualizer* visualizer = new Visualizer(label);
    visualizer->setDepthAdjustment(Visualizer::AdjustToFront);
    visualizer->setVisibility(true);
    body->setVisualizer("label", visualizer);
}


void
ThreeDView::updateSolarSystemTrajectoryPlots()
{
    for (unsigned int i = 0; i < PlanetCount + MoonCount; ++i)
    {
        StaBodyId id = i < PlanetCount ? PlanetIds[i] : MoonIds[i - PlanetCount];

        Entity* body = findSolarSystemBody(id);
        if (body)
        {
            vesta::Arc* arc = body->chronology()->firstArc();
            Visualizer* vis = arc->center()->visualizer(string("traj ") + body->name());
            if (vis)
            {
                TrajectoryGeometry* plot = dynamic_cast<TrajectoryGeometry*>(vis->geometry());
                plot->updateSamples(arc->trajectory(), m_currentTime - plot->windowDuration(), m_currentTime, 100);
            }
        }
    }
}


// Find the VESTA body matching the specified STA body
vesta::Entity*
ThreeDView::findSolarSystemBody(const StaBody* body)
{
    return m_universe->findFirst(body->name().toUtf8().data());
}


// Find the VESTA body matching the specified STA body id
vesta::Entity*
ThreeDView::findSolarSystemBody(StaBodyId id)
{
    return findSolarSystemBody(STA_SOLAR_SYSTEM->lookup(id));
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


// Set up WMS map layers
// TODO: This should be set up in a config file instead of hardcoded
void
ThreeDView::initializeMapLayers()
{
    WMSRequester* wms = m_textureLoader->wmsHandler();
    if (!wms)
    {
        return;
    }

    wms->addSurfaceDefinition("bmng-apr-nb",
                              "http://wms.jpl.nasa.gov/wms.cgi?request=GetMap&layers=BMNG&srs=EPSG:4326&format=image/jpeg&styles=Apr_nb",
                              WMSRequester::LatLongBoundingBox(-180.0, -166.0, 76.0, 90.0),
                              480, 480);

    wms->addSurfaceDefinition("earth-global-mosaic",
                              "http://wms.jpl.nasa.gov/wms.cgi?request=GetMap&layers=global_mosaic&srs=EPSG:4326&format=image/jpeg&styles=visual",
                              WMSRequester::LatLongBoundingBox(-180.0, -166.0, 76.0, 90.0),
                              512, 512);
    wms->addSurfaceDefinition("moon-lo",
                              "http://onmoon.jpl.nasa.gov/wms.cgi?version=1.1.1&service=wms&request=GetMap&styles=&srs=IAU2000:30100&layers=LO&format=image/jpeg",
                              WMSRequester::LatLongBoundingBox(-180.0, -198.0, 108.0, 90.0),
                              512, 512);
    wms->addSurfaceDefinition("moon-clementine",
                              "http://onmoon.jpl.nasa.gov/wms.cgi?version=1.1.1&service=wms&request=GetMap&styles=&srs=IAU2000:30100&layers=Clementine&format=image/jpeg",
                              WMSRequester::LatLongBoundingBox(-180.0, -150.0, 60.0, 90.0),
                              512, 512);
    wms->addSurfaceDefinition("mars-mdim",
                              "http://onmars.jpl.nasa.gov/wms.cgi?request=GetMap&layers=mars&srs=IAU2000:49900&format=image/jpeg&styles=",
                              WMSRequester::LatLongBoundingBox(-180.0, -166.0, 76.0, 90.0),
                              512, 512);
    wms->addSurfaceDefinition("mars-mdim-moc_na",
                              "http://onmars.jpl.nasa.gov/wms.cgi?request=GetMap&layers=mars,moc_na&srs=IAU2000:49900&format=image/jpeg&styles=",
                              WMSRequester::LatLongBoundingBox(-180.0, -166.0, 76.0, 90.0),
                              512, 512);
}


void
ThreeDView::initializeLayers()
{
    // Stars layer
    if (m_universe->starCatalog())
    {
        StarsLayer* stars = new StarsLayer();
        stars->setStarCatalog(m_universe->starCatalog());
        stars->setLimitingMagnitude(8.5);
        m_universe->setLayer("stars", stars);
    }

    // Equatorial grid layer
    CelestialCoordinateGrid* equatorialGrid = new CelestialCoordinateGrid();
    equatorialGrid->setColor(Spectrum(0.0f, 0.0f, 1.0f));
    m_universe->setLayer("equatorial grid", equatorialGrid);

    // Image layers
    unsigned int layerCount = sizeof(SkyLayers) / sizeof(SkyLayers[0]);
    for (unsigned int i = 0; i < layerCount; ++i)
    {
        const SkyLayerProperties& layerProps = SkyLayers[i];
        SkyImageLayer* layer = new SkyImageLayer();
        layer->setDrawOrder(-1); // behind non-image layers
        layer->setOpacity(layerProps.opacity);
        layer->setTexture(m_textureLoader->loadTexture(layerProps.textureFile, planetTextureProperties()));

        const InertialFrame* frame = InertialFrame::equatorJ2000();
        if (layerProps.frame == "galactic")
        {
            frame = InertialFrame::galactic();
        }
        else if (layerProps.frame == "ecliptic")
        {
            frame = InertialFrame::eclipticJ2000();
        }
        layer->setOrientation(frame->orientation());
        m_universe->setLayer(layerProps.layerName, layer);
    }
}


// Create the standard set textures and models required for visualization of any scenario
void
ThreeDView::initializeStandardResources()
{
    // Create icon textures
    m_spacecraftIcon = m_textureLoader->loadTexture(":/icons/Icon3DViewSpacecraft", TextureProperties(TextureProperties::Clamp));
    m_planetIcon = m_textureLoader->loadTexture(":/icons/Icon3DViewSpacecraft", TextureProperties(TextureProperties::Clamp));

    // Load the default spacecraft model
    m_defaultSpacecraftMesh = MeshGeometry::loadFromFile(DefaultSpacecraftMeshFile, m_textureLoader.ptr());
    if (m_defaultSpacecraftMesh.isValid())
    {
        m_defaultSpacecraftMesh->setMeshScale(0.002f / m_defaultSpacecraftMesh->boundingSphereRadius());
    }
    m_spacecraftMeshes["default"] = m_defaultSpacecraftMesh;
}


static ModelProperties& GetModelProperties(const QString& name)
{
    QString lname = name.toLower();
    for (unsigned int i = 0; i < sizeof(SpacecraftModels) / sizeof(SpacecraftModels[0]); ++i)
    {
        if (lname == SpacecraftModels[i].name)
        {
            return SpacecraftModels[i];
        }
    }

    return SpacecraftModels[0];
}


MeshGeometry*
ThreeDView::getSpacecraftModel(const QString& name)
{
    QString lname = name.toLower();
    for (unsigned int i = 0; i < sizeof(SpacecraftModels) / sizeof(SpacecraftModels[0]); ++i)
    {
        const ModelProperties& props = SpacecraftModels[i];
        if (lname == props.name)
        {
            if (!m_spacecraftMeshes.contains(lname))
            {
                MeshGeometry* mesh = MeshGeometry::loadFromFile(props.modelFileName.toUtf8().data(), m_textureLoader.ptr());
                if (mesh)
                {
                    mesh->setMeshScale(props.scale / mesh->boundingSphereRadius());
                }
                m_spacecraftMeshes[lname] = vesta::counted_ptr<MeshGeometry>(mesh);
            }
            return m_spacecraftMeshes[lname].ptr();
        }
    }

    // Nothing found; return the default mesh
    return m_defaultSpacecraftMesh.ptr();
}


void
ThreeDView::setCurrentTime(double mjd)
{
    double t = sta::MjdToSecJ2000(mjd);
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
ThreeDView::setEquatorialPlane(bool enabled)
{
    Entity* earth = findSolarSystemBody(STA_SOLAR_SYSTEM->earth());
    if (earth)
    {
        Visualizer* vis = earth->visualizer("equatorial plane");
        if (!vis)
        {
            PlaneVisualizer* planeVis = new PlaneVisualizer(earth->geometry()->boundingSphereRadius() * 5.0);
            planeVis->setFrame(InertialFrame::equatorJ2000());
            planeVis->plane()->setGridLineSpacing(3000.0);
            planeVis->plane()->setOpacity(0.35f);
            earth->setVisualizer("equatorial plane", planeVis);
            vis = planeVis;
        }
        vis->setVisibility(enabled);
        setViewChanged();
    }
}


void
ThreeDView::setSensorFovs(bool enabled)
{
    m_sensorFovsEnabled = enabled;

    foreach (Entity* e, m_scenarioSpaceObjects.values())
    {
        // Set visibility of trajectory visualizers for all arcs
        Visualizer* vis = e->visualizer("sensor fov");
        if (vis)
        {
            vis->setVisibility(enabled);
        }
    }

    setViewChanged();
}


void
ThreeDView::tick(double dt)
{
    Vector3d lastPosition = m_observer->absolutePosition(m_currentTime);
    Quaterniond lastOrientation = m_observer->absoluteOrientation(m_currentTime);

    const double KeyboardAccel = 3.0;
    if (m_rollLeftKey)
    {
        m_controller->roll(dt * KeyboardAccel);
    }
    else if (m_rollRightKey)
    {
        m_controller->roll(-dt * KeyboardAccel);
    }
    else if (m_pitchUpKey)
    {
        m_controller->pitch(dt * KeyboardAccel);
    }
    else if (m_pitchDownKey)
    {
        m_controller->pitch(-dt * KeyboardAccel);
    }

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
        setInertialObserver();
        m_observer->setPosition((sun->position(m_currentTime) - e->position(m_currentTime)).normalized() * distance);
        m_observer->setOrientation(LookAt(Vector3d::Zero(), m_observer->position(), Vector3d::UnitZ()));

        setViewChanged();
        update();
    }
}


/** Set the camera to any one of the builtin viewpoints.
  *
  * The valid names are:
  *   - Any Solar System body (with ephemeris data)
  *   - "earth-moon"
  *   - "inner solar system"
  *   - "outer solar system"
  *   - "S#name" - spaceobject with the specified index, e.g. "S#Herschel"
  */
void
ThreeDView::setCameraViewpoint(const QString& viewName)
{
    QString name = viewName.toLower();

    // Get the center of the camera frame
    Entity* center = NULL;
    if (name == "earth-moon")
    {
        center = m_universe->findFirst("Earth");
    }
    else if (name == "inner solar system" ||
             name == "outer solar system")
    {
        center = m_universe->findFirst("Sun");
    }
    else if (name =="jupiter system")
    {
        center = m_universe->findFirst("Jupiter");
    }
    else if (name =="saturn system")
    {
        center = m_universe->findFirst("Saturn");
    }
    else if (name.startsWith("s#"))
    {
        QString spaceObjName = name.mid(2);
        foreach (SpaceObject* spaceObj, m_scenarioSpaceObjects.keys())
        {
            if (spaceObj->name().toLower() == spaceObjName)
            {
                center = m_scenarioSpaceObjects[spaceObj];
            }
        }
    }
    else
    {
        StaBody* body = STA_SOLAR_SYSTEM->lookup(name);
        if (body)
        {
            center = m_universe->findFirst(body->name().toUtf8().data());
        }
    }

    // Abort if the center doesn't exist, or if it isn't visible at the current time
    if (!center || !center->chronology()->includesTime(m_currentTime))
    {
        return;
    }

    Vector3d eclNorth = InertialFrame::eclipticJ2000()->orientation() * Vector3d::UnitZ();
    Vector3d centerNorth = center->orientation(m_currentTime) * Vector3d::UnitZ();

    // Next, get the offset and camera up direction.
    double distance = 1.0;
    Vector3d offsetDirection = Vector3d::UnitZ();
    Vector3d upDirection = Vector3d::UnitX();
    if (name == "earth-moon")
    {
        distance = 8.0e5;
    }
    else if (name == "inner solar system")
    {
        offsetDirection = eclNorth;
        distance = 7.5e8;
    }
    else if (name == "outer solar system")
    {
        offsetDirection = eclNorth;
        distance = 1.0e10;
    }
    else if (name =="jupiter system")
    {
        offsetDirection = centerNorth;
        distance = 2.0e6;
    }
    else if (name =="saturn system")
    {
        offsetDirection = centerNorth;
        distance = 3.0e6;
    }
    else if (name.startsWith("s#"))
    {
        // Position the camera so that it's on a line from the spacecraft to the
        // spacecraft's current central body.
        if (center->geometry())
        {
            distance = center->geometry()->boundingSphereRadius() * 3.0;
        }

        vesta::Entity* origin = center->chronology()->activeArc(m_currentTime)->center();
        if (origin)
        {
            offsetDirection = (center->position(m_currentTime) - origin->position(m_currentTime)).normalized();
            upDirection = origin->orientation(m_currentTime) * Vector3d::UnitZ();
        }
        else
        {
            offsetDirection = center->position(m_currentTime).normalized();
            upDirection = Vector3d::UnitZ();
        }
    }
    else
    {
        // Position the camera so that it is facing the sunlit side of the planet. The up
        // direction is set to the planet's north pole (rotational north, not ecliptical
        // north.)
        if (center->geometry())
        {
            distance = center->geometry()->boundingSphereRadius() * 4.0;
        }

        Entity* sun = m_universe->findFirst("Sun");
        if (sun)
        {
            offsetDirection = (sun->position(m_currentTime) - center->position(m_currentTime)).normalized();
        }

        upDirection = centerNorth;
    }

    m_observer->setCenter(center);
    setInertialObserver();
    m_observer->setPosition(offsetDirection * distance);
    m_observer->setOrientation(LookAt(Vector3d::Zero(), m_observer->position(), upDirection));

    setViewChanged();
    update();
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

    Entity* earth = findSolarSystemBody(STA_EARTH);

    // Set the size of the equatorial plane visualizer.
    double maxDistanceFromCenter = 0.0;
    foreach (Entity* body, m_scenarioSpaceObjects.values())
    {
        for (unsigned int i = 0; i < body->chronology()->arcCount(); ++i)
        {
            vesta::Arc* arc = body->chronology()->arc(i);
            if (arc->center() == earth)
            {
                maxDistanceFromCenter = max(maxDistanceFromCenter, arc->trajectory()->boundingSphereRadius());
            }
        }
    }

    if (earth)
    {
        double planeSize = max(15000.0, min(200000.0, maxDistanceFromCenter * 1.5));
        Visualizer* vis = earth->visualizer("equatorial plane");
        bool visible = (vis && vis->isVisible());

        PlaneVisualizer* planeVis = new PlaneVisualizer(planeSize);
        planeVis->setFrame(InertialFrame::equatorJ2000());
        planeVis->plane()->setGridLineSpacing(planeSize / 10.0);
        planeVis->plane()->setOpacity(0.35f);
        planeVis->setVisibility(visible);
        earth->setVisualizer("equatorial plane", planeVis);
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
                f = new BodyFixedFrame(e);
            }
        }
        break;

    default:
        qDebug() << "ThreeDView::createFrame(): Unsupported coordinate system.";
        break;
    }

    return f;
}


/** Determine which VESTA object was is closest to the viewer at the
  * specified 2D position.
  */
vesta::Entity*
ThreeDView::pickObject(const QPoint& pos)
{
    double pixelAngle = m_fov / size().height();

    // Get the click point in normalized device coordinaes
    Vector2d ndc = Vector2d(double(pos.x()) / double(size().width()),
                            double(pos.y()) / double(size().height())) * 2.0 - Vector2d::Ones();
    ndc.y() = -ndc.y();

    // Get the pick direction
    double aspectRatio = double(size().width()) / double(size().height());
    double h = tan(m_fov / 2.0f);
    Vector3d pickDirection = Vector3d(h * aspectRatio * ndc.x(), h * ndc.y(), -1.0).normalized();

    // Convert to world coordinates
    pickDirection = m_observer->absoluteOrientation(m_currentTime) * pickDirection;
    Vector3d pickOrigin = m_observer->absolutePosition(m_currentTime);

    PickResult pickResult;
    if (m_universe->pickObject(m_currentTime, pickOrigin, pickDirection, pixelAngle, &pickResult))
    {
        return pickResult.hitObject();
    }
    else
    {
        return NULL;
    }
}


static string TrajectoryVisualizerTag(const Entity* spaceObj, unsigned int arcIndex)
{
    return string(QString("%1 - %2 trajectory").arg(qHash(spaceObj)).arg(arcIndex).toUtf8().data());
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

    TwoBodyRotatingFrame* bodyFrame = NULL;
    RotationModel* rotation = NULL;

    foreach (MissionArc* missionArc, spaceObj->mission())
    {
        // Calculate the radius of a sphere that contains all the points in the trajectory.
        // We'll use this to optimize rendering and to set the size of some visualizers.
        double boundingRadius = 0.0;
        for (unsigned int i = 0; i < missionArc->trajectorySampleCount(); ++i)
        {
            boundingRadius = max(boundingRadius, missionArc->trajectorySample(i).position.norm());
        }

        Entity* center = findSolarSystemBody(missionArc->centralBody());
        if (center)
        {
            Frame* frame = createFrame(missionArc);
            if (frame)
            {
                vesta::Arc* arc = new vesta::Arc();
                arc->setCenter(center);
                arc->setDuration(sta::daysToSecs(missionArc->ending() - missionArc->beginning()));
                arc->setTrajectoryFrame(frame);
                arc->setTrajectory(new StaMissionArcTrajectory(missionArc, boundingRadius * 1.1));
                body->chronology()->addArc(arc);

                if (bodyFrame == NULL)
                {
                    // Create a frame for the spacecraft that will maintain a nadir pointing attitude. This
                    // makes the visualization somewhat more realistic in the absence of an attitude module.
                    // We create a single body frame for the entire trajectory; the secondary body in the
                    // two-body frame is the center of the first arc.
                    bodyFrame = new TwoBodyRotatingFrame(body, center);
                }
                arc->setBodyFrame(bodyFrame);

                if (rotation == NULL)
                {
                    // Create a fixed rotation model to orient the model in the right direction
                    const ModelProperties& props = GetModelProperties(spaceObj->mission().first()->modelName());
                    Quaterniond q = SpacecraftOrientation(props.nadirDirection, props.leadingDirection);
                    rotation = new FixedRotationModel(q);
                }
                arc->setRotationModel(rotation);
            }
        }
    }

    body->chronology()->setBeginning(sta::MjdToSecJ2000(spaceObj->mission().first()->beginning()));

    //body->setGeometry(m_defaultSpacecraftMesh.ptr());
    // For now, use the model in the first arc for the whole mission
    body->setGeometry(getSpacecraftModel(spaceObj->mission().first()->modelName()));

    Spectrum spaceObjColor(spaceObj->mission().front()->arcTrajectoryColor().redF(),
                           spaceObj->mission().front()->arcTrajectoryColor().greenF(),
                           spaceObj->mission().front()->arcTrajectoryColor().blueF());

    LabelGeometry* label = new LabelGeometry(spaceObj->name().toUtf8().data(), m_labelFont.ptr(), spaceObjColor, 7.0f);
    label->setIcon(m_spacecraftIcon.ptr());
    label->setIconColor(spaceObjColor);
    Visualizer* visualizer = new Visualizer(new MulticolorLabelGeometry(spaceObj, label));
    visualizer->setDepthAdjustment(Visualizer::AdjustToFront);
    body->setVisualizer("label", visualizer);

    // Add a trajectory visualizer for each arc
    double arcStartTime = body->chronology()->beginning();
    for (unsigned int arcIndex = 0; arcIndex < body->chronology()->arcCount(); ++arcIndex)
    {
        vesta::Arc* arc = body->chronology()->arc(arcIndex);
        MissionArc* missionArc = spaceObj->mission().at(arcIndex);
        QColor arcColor = missionArc->arcTrajectoryColor();

        TrajectoryGeometry* trajGeom = new TrajectoryGeometry();
        trajGeom->setFrame(createFrame(missionArc));
        trajGeom->setDisplayedPortion(TrajectoryGeometry::StartToCurrentTime);
        trajGeom->setColor(Spectrum(arcColor.redF(), arcColor.greenF(), arcColor.blueF()));

        for (unsigned int i = 0; i < missionArc->trajectorySampleCount(); ++i)
        {
            sta::StateVector state = missionArc->trajectorySample(i);
            trajGeom->addSample(sta::MjdToSecJ2000(missionArc->trajectorySampleTime(i)), StateVector(state.position, state.velocity));
        }

        Visualizer* trajVisualizer = new Visualizer(trajGeom);
        trajVisualizer->setVisibility(m_satelliteTrajectoriesEnabled);
        arc->center()->setVisualizer(TrajectoryVisualizerTag(body, arcIndex), trajVisualizer);

        arcStartTime += arc->duration();
    }

    // Add a visualizer for the sensor FOV
    SensorVisualizer* sensor = new SensorVisualizer();
    sensor->setFrustumAngles(toRadians(5.0), toRadians(25.0));
    sensor->setRange(35000.0);
    sensor->setSource(body);
    sensor->setOpacity(0.3f);
    QColor sensorColor = spaceObj->mission().first()->arcTrajectoryColor();
    sensor->setColor(Spectrum(sensorColor.redF(), sensorColor.greenF(), sensorColor.blueF()));
    Quaterniond sensorRotation;

    // Get the nadir direction in the spacecraft's body fixed frame
    Vector3d localNadir = body->chronology()->firstArc()->rotationModel()->orientation(0.0).conjugate() * Vector3d::UnitX();
    sensorRotation.setFromTwoVectors(Vector3d::UnitZ(), localNadir);

    sensor->setSensorOrientation(sensorRotation);
    sensor->setTarget(findSolarSystemBody(spaceObj->mission().first()->centralBody()));
    sensor->setVisibility(m_sensorFovsEnabled);
    body->setVisualizer("sensor fov", sensor);

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

        WorldGeometry* globe = dynamic_cast<WorldGeometry*>(center->geometry());
        if (globe)
        {
            // Position the object using the shape of the planet in VESTA. Currently,
            // we're depicting the terrestrial planets as spherical in order to avoid
            // problems maintaining a constant camera altitude.
            position = position.cwise() * globe->ellipsoidAxes().cast<double>() / 2.0;
        }
        else
        {
            position = position.cwise() * groundObj->centralBody->radii();
        }

        vesta::Arc* arc = new vesta::Arc();
        arc->setCenter(center);
        arc->setDuration(ValidTimeSpan);
        arc->setTrajectory(new FixedPointTrajectory(position));
        arc->setTrajectoryFrame(new BodyFixedFrame(center));

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
    m_selectedSpacecraft = NULL;

    // Reset the observer position if a spacecraft view was active
    gotoBody(STA_SOLAR_SYSTEM->earth());

    // Remove VESTA entities for all space and ground objects
    foreach (Entity* e, m_scenarioSpaceObjects.values())
    {
        // Remove trajectory visualizers from central body
        for (unsigned int arcIndex = 0; arcIndex < e->chronology()->arcCount(); ++arcIndex)
        {
            e->chronology()->arc(arcIndex)->center()->removeVisualizer(TrajectoryVisualizerTag(e, arcIndex));
        }

        m_universe->removeEntity(e);
    }

    foreach (Entity* e, m_scenarioGroundObjects.values())
    {
        m_universe->removeEntity(e);
    }

    m_scenarioSpaceObjects.clear();
    m_scenarioGroundObjects.clear();
}


static void
setSpacecraftView(Observer* observer, Entity* e, double t)
{
    observer->setCenter(e);
    Geometry* geom = e->geometry();
    float distance = 1.0f;
    if (geom)
    {
        distance = 3.0f * geom->boundingSphereRadius();
    }

    vesta::Arc* activeArc = e->chronology()->activeArc(t);
    if (!activeArc)
    {
        activeArc = e->chronology()->firstArc();
    }

    Vector3d toCenter = activeArc->center()->position(t) - e->position(t);
    observer->setPosition(-toCenter.normalized() * distance);
    observer->setOrientation(LookAt(Vector3d::Zero(), -toCenter, Vector3d::UnitZ()));
}


void
ThreeDView::selectParticipant(SpaceObject* spaceObj)
{
    m_selectedSpacecraft = spaceObj;
    setViewChanged();
}


void
ThreeDView::setStars(bool enabled)
{
    SkyLayer* grid = m_universe->layer("stars");
    if (grid)
    {
        grid->setVisibility(enabled);
        setViewChanged();
    }
}


void ThreeDView::setAtmospheres(bool enabled)
{
    Entity* e = findSolarSystemBody(STA_EARTH);
    if (e)
    {
        WorldGeometry* globe = dynamic_cast<WorldGeometry*>(e->geometry());
        if (globe)
        {
            if (enabled)
            {
                Atmosphere* atmosphere = NULL;
                if (!m_atmospheres.contains(e))
                {
                    QFile atmosphereFile("vis3d/earth.atmscat");
                    if (atmosphereFile.open(QIODevice::ReadOnly))
                    {
                        QByteArray data = atmosphereFile.readAll();
                        DataChunk chunk(data.data(), data.size());
                        atmosphere = Atmosphere::LoadAtmScat(&chunk);
                    }
                    m_atmospheres[e] = atmosphere;
                    if (atmosphere)
                    {
                        m_newAtmospheres << atmosphere;
                    }
                }
                else
                {
                    atmosphere = m_atmospheres[e].ptr();
                }

                globe->setAtmosphere(atmosphere);
            }
            else
            {
                globe->setAtmosphere(NULL);
            }
        }
    }
    setViewChanged();

}


void ThreeDView::setClouds(bool enabled)
{
    Entity* e = findSolarSystemBody(STA_EARTH);
    if (e)
    {
        WorldGeometry* globe = dynamic_cast<WorldGeometry*>(e->geometry());
        if (globe)
        {
            if (enabled)
            {
                globe->setCloudMap(m_textureLoader->loadTexture("textures/medres/earth-clouds.png", planetTextureProperties()));
                globe->setCloudAltitude(7.0f);
            }
            else
            {
                globe->setCloudMap(NULL);
            }
        }
    }
    setViewChanged();
}


void ThreeDView::setShadows(bool enabled)
{
    m_shadowsEnabled = enabled;
    if (m_shadowsInitialized && m_renderer->shadowsSupported())
    {
        // Only enable shadows if we've already initialized the shadow maps
        m_renderer->setShadowsEnabled(m_shadowsEnabled);
    }
    setViewChanged();
}


void
ThreeDView::setReflections(bool enabled)
{
    m_reflectionsEnabled = enabled;
    setViewChanged();
}


void
ThreeDView::setEquatorialGrid(bool enabled)
{
    SkyLayer* grid = m_universe->layer("equatorial grid");
    if (grid)
    {
        grid->setVisibility(enabled);
        setViewChanged();
    }
}


void
ThreeDView::setPlanetGrid(bool enabled)
{
    m_planetGrid->setVisibility(enabled);
    setViewChanged();
}


void
ThreeDView::setSatelliteTrajectories(bool enabled)
{
    m_satelliteTrajectoriesEnabled = enabled;
    foreach (Entity* e, m_scenarioSpaceObjects.values())
    {
        // Set visibility of trajectory visualizers for all arcs
        for (unsigned int arcIndex = 0; arcIndex < e->chronology()->arcCount(); ++arcIndex)
        {
            Visualizer* vis = e->chronology()->arc(arcIndex)->center()->visualizer(TrajectoryVisualizerTag(e, arcIndex));
            if (vis)
            {
                vis->setVisibility(m_satelliteTrajectoriesEnabled);
            }
        }
    }

    setViewChanged();
}


void
ThreeDView::setReentryTrajectories(bool enabled)
{
}


void
ThreeDView::setPlanetOrbits(bool enabled)
{
    for (unsigned int i = 0; i < PlanetCount; ++i)
    {
        Entity* planet = findSolarSystemBody(PlanetIds[i]);
        if (planet)
        {
            Entity* center = planet->chronology()->firstArc()->center();
            Visualizer* vis = center->visualizer(string("traj ") + planet->name());
            if (vis)
            {
                vis->setVisibility(enabled);
            }
        }
    }

    setViewChanged();
}


void
ThreeDView::setMoonOrbits(bool enabled)
{
    for (unsigned int i = 0; i < MoonCount; ++i)
    {
        Entity* planet = findSolarSystemBody(MoonIds[i]);
        if (planet)
        {
            Entity* center = planet->chronology()->firstArc()->center();
            Visualizer* vis = center->visualizer(string("traj ") + planet->name());
            if (vis)
            {
                vis->setVisibility(enabled);
            }
        }
    }

    setViewChanged();
}


void
ThreeDView::setPlanetLabels(bool enabled)
{
    Entity* sun = findSolarSystemBody(STA_SUN);
    if (sun)
    {
        Visualizer* vis = sun->visualizer("label");
        if (vis)
        {
            vis->setVisibility(enabled);
        }
    }

    for (unsigned int i = 0; i < PlanetCount; ++i)
    {
        Entity* planet = findSolarSystemBody(PlanetIds[i]);
        if (planet)
        {
            Visualizer* vis = planet->visualizer("label");
            if (vis)
            {
                vis->setVisibility(enabled);
            }
        }
    }

    for (unsigned int i = 0; i < MoonCount; ++i)
    {
        Entity* planet = findSolarSystemBody(MoonIds[i]);
        if (planet)
        {
            Visualizer* vis = planet->visualizer("label");
            if (vis)
            {
                vis->setVisibility(enabled);
            }
        }
    }

    setViewChanged();
}


/** Set the current sky layer. An index of 0 will disable all layers.
  */
void
ThreeDView::setCurrentSkyLayer(int layerIndex)
{
    unsigned int layerCount = sizeof(SkyLayers) / sizeof(SkyLayers[0]);

    // Disable all layers
    for (unsigned int i = 0; i < layerCount; ++i)
    {
        SkyLayer* layer = m_universe->layer(SkyLayers[i].layerName);
        if (layer)
        {
            layer->setVisibility(false);
        }
    }

    // Only enable a layer if the index is in range
    if (layerIndex > 0 && layerIndex <= layerCount)
    {
        SkyLayer* activeLayer = m_universe->layer(SkyLayers[layerIndex - 1].layerName);
        if (activeLayer)
        {
            activeLayer->setVisibility(true);
        }
    }

    setViewChanged();
}


void
ThreeDView::setAmbientLight(float lightLevel)
{
    m_renderer->setAmbientLight(Spectrum::Flat(max(0.0f, min(1.0f, lightLevel))));
    setViewChanged();
}


void
ThreeDView::setInertialObserver()
{
    m_observer->updatePositionFrame(InertialFrame::equatorJ2000(), m_currentTime);
    m_observer->updatePointingFrame(InertialFrame::equatorJ2000(), m_currentTime);
}


void
ThreeDView::setBodyFixedObserver()
{
    BodyFixedFrame* f = new BodyFixedFrame(m_observer->center());
    m_observer->updatePositionFrame(f, m_currentTime);
    m_observer->updatePointingFrame(f, m_currentTime);
}


void
ThreeDView::setObserverCenter(vesta::Entity* center)
{
    m_observer->updateCenter(center, m_currentTime);
    if (dynamic_cast<BodyFixedFrame*>(m_observer->positionFrame()))
    {
        // Need to update the center of a body fixed frame
        BodyFixedFrame* f = new BodyFixedFrame(center);
        m_observer->updatePositionFrame(f, m_currentTime);
        m_observer->updatePointingFrame(f, m_currentTime);
    }
}


void
ThreeDView::setStereoMode(int mode)
{
    m_stereoMode = (StereoMode) mode;
    setViewChanged();
    emit stereoModeChanged(m_stereoMode);  // Guillermo
}


void
ThreeDView::setVisibleTrajectoryPortion(double days)
{
    // Remove VESTA entities for all space and ground objects
    foreach (Entity* e, m_scenarioSpaceObjects.values())
    {
        // Remove trajectory visualizers from central body
        for (unsigned int arcIndex = 0; arcIndex < e->chronology()->arcCount(); ++arcIndex)
        {
            Visualizer* vis = e->chronology()->arc(arcIndex)->center()->visualizer(TrajectoryVisualizerTag(e, arcIndex));
            if (vis)
            {
                TrajectoryGeometry* geom = dynamic_cast<TrajectoryGeometry*>(vis->geometry());
                if (geom)
                {
                    geom->setDisplayedPortion(TrajectoryGeometry::WindowBeforeCurrentTime);
                    geom->setWindowDuration(sta::daysToSecs(days));
                }
            }
        }
    }

    setViewChanged();
}
