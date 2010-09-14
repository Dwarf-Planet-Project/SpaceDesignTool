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

#ifndef _THREED_VIEW_H_
#define _THREED_VIEW_H_

#include "Astro-Core/stabody.h"
#include <vesta/Object.h>
#include <QGLWidget>
#include <QHash>

namespace vesta
{
    class Universe;
    class UniverseRenderer;
    class Observer;
    class ObserverController;
    class Entity;
    class Body;
    class Frame;
    class TextureMap;
    class TextureMapLoader;
    class TextureFont;
    class MeshGeometry;
    class Atmosphere;
    class PlanetGridLayer;
    class CubeMapFramebuffer;
    class Spectrum;
}

class StaBody;
class PropagatedScenario;
class SpaceObject;
class GroundObject;
class MissionArc;


/** ThreeDView is a widget with a rendered 3D view of the current space scenario.
  */
class ThreeDView : public QGLWidget
{
public:
    Q_OBJECT

public:
    ThreeDView(const QGLFormat& format, QWidget *parent = 0);
    ~ThreeDView();

   QSize minimumSizeHint() const;
   QSize sizeHint() const;

   double currentTime() const
   {
       return m_currentTime;
   }

public slots:
   void setCurrentTime(double t);
   void tick(double dt);

   void setViewChanged();

   void gotoBody(const StaBody* body);
   void setScenario(PropagatedScenario* scenario);
   void selectParticipant(SpaceObject* spaceObject);

   void setEquatorialPlane(bool enabled);
   void setStars(bool enabled);
   void setAtmospheres(bool enabled);
   void setClouds(bool enabled);
   void setShadows(bool enabled);
   void setReflections(bool enabled);
   void setEquatorialGrid(bool enabled);
   void setPlanetGrid(bool enabled);

   void setSatelliteTrajectories(bool enabled);
   void setReentryTrajectories(bool enabled);
   void setPlanetOrbits(bool enabled);
   void setMoonOrbits(bool enabled);

   void setCurrentSkyLayer(int layerIndex);
   void setAmbientLight(float lightLevel);

protected:
    void initializeGL();
    void paintGL();
    void resizeGL(int width, int height);
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent* event);
    void mouseMoveEvent(QMouseEvent *event);
    void wheelEvent(QWheelEvent* event);
    void keyPressEvent(QKeyEvent* event);
    void keyReleaseEvent(QKeyEvent* event);

private:
    void initializeUniverse();
    void initializeLayers();
    void initializeStandardResources();
    void initializeStarCatalog(const QString& fileName);
    vesta::Body* addSolarSystemBody(const StaBody* body, vesta::Entity* center);
    vesta::Entity* findSolarSystemBody(const StaBody* body);
    vesta::Entity* findSolarSystemBody(StaBodyId id);
    vesta::Frame* createFrame(const MissionArc* arc);

    void createOrbitVisualizer(const StaBody* body, const vesta::Spectrum& color);

    void drawOverlay();

    vesta::Body* createSpaceObject(const SpaceObject* spaceObj);
    vesta::Body* createGroundObject(const GroundObject* groundObj);
    void clearScenarioObjects();

private:
    double m_currentTime;
    vesta::Universe* m_universe;
    vesta::UniverseRenderer* m_renderer;
    vesta::counted_ptr<vesta::Observer> m_observer;
    vesta::counted_ptr<vesta::ObserverController> m_controller;
    vesta::counted_ptr<vesta::TextureMapLoader> m_textureLoader;
    vesta::counted_ptr<vesta::TextureFont> m_labelFont;
    vesta::counted_ptr<vesta::MeshGeometry> m_defaultSpacecraftMesh;

    vesta::counted_ptr<vesta::TextureMap> m_spacecraftIcon;
    vesta::counted_ptr<vesta::CubeMapFramebuffer> m_reflectionMap;

    vesta::counted_ptr<vesta::PlanetGridLayer> m_planetGrid;

    double m_fov;

    QPointF m_mousePosition;

    QHash<SpaceObject*, vesta::Entity*> m_scenarioSpaceObjects;
    QHash<GroundObject*, vesta::Entity*> m_scenarioGroundObjects;
    QHash<vesta::Entity*, vesta::counted_ptr<vesta::Atmosphere> > m_atmospheres;
    QList<vesta::Atmosphere*> m_newAtmospheres;

    int m_viewChanged;
    SpaceObject* m_selectedSpacecraft;

    bool m_satelliteTrajectoriesEnabled;
    bool m_shadowsEnabled;
    bool m_reflectionsEnabled;
    bool m_glInitialized;
    bool m_shadowsInitialized;
};

#endif // _THREED_VIEW_H_
