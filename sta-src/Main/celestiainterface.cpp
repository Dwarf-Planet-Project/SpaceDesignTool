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

#include "celestiainterface.h"
#include "propagatedscenario.h"

#include "celengine/body.h"
#include "celengine/universe.h"
#include "celengine/selection.h"
#include "celengine/rotation.h"
#include "celengine/timeline.h"
#include "celengine/timelinephase.h"
#include "celengine/samporbit.h"
#include "celengine/meshmanager.h"
#include "celestia/celestiacore.h"

#include "Astro-Core/date.h"

#include <limits>
#include <QtDebug>



// private constructor
CelestiaInterface::CelestiaInterface() :
    m_universe(NULL),
    m_sun(NULL),
    m_ssb(NULL),
    m_solarSystem(NULL)
{
}


CelestiaInterface::~CelestiaInterface()
{
}


/*! Create a new Celestia interface.
 */
CelestiaInterface*
CelestiaInterface::Create(CelestiaCore* appCore)
{
    CelestiaInterface* cel = new CelestiaInterface();
    cel->m_appCore = appCore;
    cel->m_universe = appCore->getSimulation()->getUniverse();
    if (!cel->m_universe)
    {
        qDebug("Empty celestia configuration.\n");
        delete cel;
        return NULL;
    }
    
    // Look up the Sun and Solar System barycenter
    Selection sel;
    sel = cel->m_universe->find("Sol");
    cel->m_sun = sel.star();
    
    sel = cel->m_universe->find("SSB");
    cel->m_ssb = sel.star();

    if (!cel->m_sun || !cel->m_ssb)
    {
        qDebug("Couldn't find Sun and SSB in Celestia configuration.");
        delete cel;
        return NULL;
    }

    // Get the Solar System
    cel->m_solarSystem = cel->m_universe->getSolarSystem(cel->m_sun);
    if (!cel->m_solarSystem)
    {
        qDebug("Celestia solar system not found.");
        delete cel;
        return NULL;
    }

    return cel;
}


CelestiaBody*
CelestiaInterface::createBody(const QString& name,
                              const StaBody* parent)
{
    string parentCname = parent->name().toUtf8().data();

    Selection sun(m_sun);
    Selection ssb(m_ssb);
    Selection searchContexts[2] = { sun, ssb };
    Selection parentObject = m_universe->find(parentCname, searchContexts, 2);

    PlanetarySystem* parentSystem = NULL;
    if (parentObject.star() != NULL)
    {
        Star* parentStar = parentObject.star();
        // Disallow objects around other stars
        if (parentStar == m_sun && parentStar == m_ssb)
        {
            return NULL;
        }

        SolarSystem* solarSystem = m_universe->getSolarSystem(parentStar);
        if (solarSystem == NULL)
        {
            // No solar system defined for this star yet, so we need
            // to create it.
            solarSystem = m_universe->createSolarSystem(parentStar);
        }
        parentSystem = solarSystem->getPlanets();
    }
    else if (parentObject.body() != NULL)
    {
        // Parent is a solar system body
        Body* parentBody = parentObject.body();

        parentSystem = parentBody->getSatellites();
        if (parentSystem == NULL)
        {
            // If the planet doesn't already have any satellites, we
            // have to create a new planetary system for it.
            parentSystem = new PlanetarySystem(parentBody);
            parentBody->setSatellites(parentSystem);
        }
    }

    // If the parent system is NULL, we've got an invalid object
    if (parentSystem == NULL)
    {
        return NULL;
    }

    string cname = name.toUtf8().data();
    Body* body = new Body(parentSystem, cname);

    return new CelestiaBody(this, body);
}


CelestiaBody*
CelestiaInterface::createSpacecraft(const QString& name,
                                    const StaBody* parent)
{
    CelestiaBody* cbody = createBody(name, parent);

    cbody->body()->setClassification(Body::Spacecraft); 
    cbody->body()->setSemiAxes(Vec3f(0.005f, 0.005f, 0.005f));  // Default 5m radius

    return cbody;
}


/*! Create a new surface object in Celestia. Latitude and longitude are
 *  in degrees, altitude is a value in meters.
 */
CelestiaBody*
CelestiaInterface::createSurfaceObject(const QString& name,
                                       const StaBody* centralBody,
                                       double latitude,
                                       double longitude,
                                       double altitude)
{
    // Find the Celestia object for the central body
    Selection center;
    
    Body* body = findBody(centralBody);
    if (body != NULL)
        center = Selection(body);
    else if (centralBody->id() == STA_SUN)
        center = Selection(sun());
    
    if (center.empty())
    {
        qDebug() << "Error creating Celestia timeline. Body " << centralBody->name() << " not in Celestia.";
        return NULL;
    }
    
    CelestiaBody* cbody = createBody(name, centralBody);
    
    cbody->body()->setClassification(Body::SurfaceFeature);
    cbody->body()->setVisibleAsPoint(false);
    cbody->body()->setSemiAxes(Vec3f(0.005f, 0.005f, 0.005f));  // Default 5m radius

    Timeline* timeline = new Timeline();
    
    // Create the timeline
    ReferenceFrame* fixedFrame = new BodyFixedFrame(center, center);        
    fixedFrame->addRef();
    
    Vec3d position = body->planetocentricToCartesian(longitude - 180.0, latitude, altitude / 1000.0);
    Orbit* trajectory = new FixedOrbit(Point3d(position.x, position.y, position.z));        
    RotationModel* rotationModel = new ConstantOrientation(Quatd(1.0, 0.0, 0.0, 0.0));       
        
    TimelinePhase* phase = TimelinePhase::CreateTimelinePhase(*universe(),
                                                              cbody->body(),
                                                              -numeric_limits<double>::infinity(),
                                                              numeric_limits<double>::infinity(),
                                                              *fixedFrame,
                                                              *trajectory,
                                                              *fixedFrame,
                                                              *rotationModel);
    fixedFrame->release(); // now owned by timeline phase
        
    timeline->appendPhase(phase);
    cbody->body()->setTimeline(timeline);
    
    return cbody;
}


/*! Create a new location object in Celestia. Latitude and longitude are
 *  in degrees, altitude is a value in meters.
 */
CelestiaLocation*
CelestiaInterface::createLocation(const QString& name,
                                  const StaBody* centralBody,
                                  double latitude,
                                  double longitude,
                                  double altitude)
{
    // Find the Celestia object for the central body
    Selection center;
    
    Body* body = findBody(centralBody);
    if (body == NULL)
    {
        qDebug() << "Error creating Celestia location. Body " << centralBody->name() << " not in Celestia.";
        return NULL;
    }
    
    Vec3d position = body->planetocentricToCartesian(longitude, latitude, altitude / 1000.0);

    Location* loc = new Location();
    loc->setParentBody(body);
    loc->setName(name.toUtf8().data());
    loc->setPosition(Vec3f((float) position.x, (float) position.y, (float) position.z));
    loc->setLabelColorOverridden(true);
    loc->setLabelColor(Color(1.0f, 0.0f, 0.0f));
    loc->setFeatureType(Location::Observatory);
    loc->setImportance(5000.0f);
    
    body->addLocation(loc);
    
    return new CelestiaLocation(loc);
}


/*! Set Celestia's selection to the specified body. If the body is NULL,
 *  the selection will be cleared.
 */
void
CelestiaInterface::select(CelestiaBody* body)
{
    if (body)
        m_appCore->getSimulation()->setSelection(Selection(body->body()));
    else
        m_appCore->getSimulation()->setSelection(Selection());
}


// Find the Celestia body corresponding the the specified STA body.
Body*
CelestiaInterface::findBody(const StaBody* body)
{
    string cname = body->name().toUtf8().data();

    Selection sun(m_sun);
    Selection ssb(m_ssb);
    Selection searchContexts[2] = { sun, ssb };
    Selection object = m_universe->find(cname, searchContexts, 2);

    return object.body();
}



CelestiaBody::CelestiaBody(CelestiaInterface* iface, Body* body) :
    m_interface(iface),
    m_body(body),
    m_marked(false)
{
}


CelestiaBody::~CelestiaBody()
{
    if (m_marked)
        unmark();
    
    // TODO: We should actually delete the Celestia objects
    m_body->setVisible(false);
}


/*! Set the timeline of the object. Return whether the
 *  timeline was valid.
 */
bool
CelestiaBody::setTimeline(const QList<MissionArc*>& missionArcs)
{
    bool ok = true;
    Universe* universe = m_interface->universe();

    Timeline* timeline = new Timeline();
    MissionArc* previousArc = NULL;

    // Create the timeline, with one phase for each mission arc
    foreach (MissionArc* arc, missionArcs)
    {
        // Find the Celestia object for the central body
        Selection center;

        Body* body = m_interface->findBody(arc->centralBody());
        if (body != NULL)
            center = Selection(body);
        else if (arc->centralBody()->id() == STA_SUN)
            center = Selection(m_interface->sun());
        if (center.empty())
        {
            qDebug() << "Error creating Celestia timeline. Body " << arc->centralBody()->name() << " not in Celestia.";
            ok = false;
            break;
        }

        // Create the reference frame
        ReferenceFrame* trajectoryFrame = NULL;
        switch (arc->coordinateSystem().type())
        {
        case sta::COORDSYS_EME_J2000:
            trajectoryFrame = new J2000EquatorFrame(center);
            break;
        case sta::COORDSYS_EME_B1950:
            // TODO: Need to add support for this
            break;
        case sta::COORDSYS_ECLIPTIC_J2000:
            trajectoryFrame = new J2000EclipticFrame(center);
            break;
        case sta::COORDSYS_BODYFIXED:
            trajectoryFrame = new BodyFixedFrame(center, center);
            break;
        case sta::COORDSYS_INVALID:
        default:
            break;
        }

        if (trajectoryFrame == NULL)
        {
            qDebug() << "Error creating Celestia timeline. Coordinate system " << arc->coordinateSystem().name() << " not supported.";
            ok = false;
            break;
        }
        trajectoryFrame->addRef();

        string trajFilename = arc->ephemerisFile().toUtf8().data();
        Orbit* trajectory = LoadXYZVTrajectoryDoublePrec(trajFilename, TrajectoryInterpolationCubic, sta::J2000);
        if (trajectory == NULL)
        {
            qDebug() << "Error creating Celestia timeline, loading " << trajFilename.c_str();
            trajectoryFrame->release();
            ok = false;
            break;
        }

        if (previousArc)
        {
            // Celestia's timelines must be contiguous, but STA trajectories
            // may not be (at the moment...) Fill in any gaps in the timeline
            // with fixed positions.
            if (previousArc->ending() < arc->beginning())
            {
                const TimelinePhase* previousPhase = timeline->getPhase(timeline->phaseCount() - 1);
                Orbit* trajectory = new FixedOrbit(previousPhase->orbit()->positionAtTime(previousPhase->endTime()));
                RotationModel* rotationModel = new ConstantOrientation(Quatd(1.0, 0.0, 0.0, 0.0));
                TimelinePhase* phase = TimelinePhase::CreateTimelinePhase(*universe,
                                                                          m_body,
                                                                          previousPhase->endTime(),
                                                                          sta::MjdToJd(arc->beginning()),
                                                                          *trajectoryFrame,
                                                                          *trajectory,
                                                                          *trajectoryFrame,
                                                                          *rotationModel);
                timeline->appendPhase(phase);
            }
        }

        // For now, always use a simple fixed rotation model.
        RotationModel* rotationModel = new ConstantOrientation(Quatd(1.0, 0.0, 0.0, 0.0));       

        TimelinePhase* phase = TimelinePhase::CreateTimelinePhase(*universe,
                                                                  m_body,
                                                                  sta::MjdToJd(arc->beginning()),
                                                                  sta::MjdToJd(arc->ending()),
                                                                  *trajectoryFrame,
                                                                  *trajectory,
                                                                  *trajectoryFrame,
                                                                  *rotationModel);
        trajectoryFrame->release(); // now owned by timeline phase

        timeline->appendPhase(phase);

        previousArc = arc;
    }

    if (ok)
    {
        m_body->setTimeline(timeline);
    }
    else
    {
        delete timeline;
    }

    return ok;
}


/*! Set the geometry of the body to the contents of the specified geometry file.
 *  An empty string indicates that the default spherical geometry should be used
 *  for the body.
 */
void
CelestiaBody::setGeometry(const QString& geometryFile)
{
    if (geometryFile.isEmpty())
    {
        m_body->setGeometry(InvalidResource);
    }
    else
    {
        string filename(geometryFile.toUtf8().data());
        ResourceHandle geometryHandle = GetGeometryManager()->getHandle(GeometryInfo(filename));
        m_body->setGeometry(geometryHandle);
    }
}


bool
CelestiaBody::isVisible() const
{
    return m_body->isVisible();
}

void
CelestiaBody::setVisible(bool visible)
{
    m_body->setVisible(visible);
}


void
CelestiaBody::setOrbitAlwaysVisible(bool visible)
{
    Body::VisibilityPolicy visPolicy = visible ? Body::AlwaysVisible : Body::UseClassVisibility;
    m_body->setOrbitVisibility(visPolicy);
}


void
CelestiaBody::setOrbitColor(QColor color)
{
    m_body->setOrbitColor(Color((float) color.redF(), (float) color.greenF(), (float) color.blueF()));
}


void
CelestiaBody::setOrbitColorOverridden(bool override)
{
    m_body->setOrbitColorOverridden(override);
}


void
CelestiaBody::mark(const QString& symbolName,
                   QColor color,
                   float size,
                   const QString& label)
{
    MarkerRepresentation::Symbol symbol = MarkerRepresentation::Diamond;
    if (symbolName == "circle")
        symbol = MarkerRepresentation::Circle;
    else if (symbolName == "x")
        symbol = MarkerRepresentation::X;
    
    Color celColor((float) color.redF(), (float) color.greenF(), (float) color.blueF());
    
    m_interface->universe()->markObject(Selection(m_body),
                                        MarkerRepresentation(symbol,
                                                             size,
                                                             celColor,
                                                             label.toUtf8().data()),
                                        1);
    m_marked = true;
}


void
CelestiaBody::unmark()
{
    if (m_marked)
    {
        m_interface->universe()->unmarkObject(Selection(m_body), 1);
        m_marked = false;
    }
}


/****** CelestiaLocation ******/

CelestiaLocation::CelestiaLocation(Location* location) :
    m_location(location)
{
}


CelestiaLocation::~CelestiaLocation()
{
    m_location->setImportance(0.0001f);
    m_location->setSize(0.0001f);
}



