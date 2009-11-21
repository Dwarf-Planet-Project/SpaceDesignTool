/* 
This program is free software; you can redistribute it and/or modify it under
 the terms of the GNU Lesser General Public License as published by the Free Software
 Foundation; either version 2 of the License, or (at your option) any later
 version.
 
 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more
 details.
 
 You should have received a copy of the GNU Lesser General Public License along with
 this program; if not, write to the Free Software Foundation, Inc., 59 Temple
 Place - Suite 330, Boston, MA 02111-1307, USA.
 Further information about the GNU Lesser General Public License can also be found on
 the world wide web at http://www.gnu.org. 
 */

/*
 ------ Copyright (C) 2008 European Space Agency (space.trajectory.analysis AT gmail.com) ----
 ------------------ Author: Chris Laurel  -------------------------------------------------
 ------------------ E-mail: (claurel@gmail.com) ----------------------------
 */

#ifndef _PLOTTING_GROUNDTRACKPLOTTOOL_H_
#define _PLOTTING_GROUNDTRACKPLOTTOOL_H_

#include "Astro-Core/stabody.h"
#include <QWidget>
#include <QAbstractScrollArea>
#include <QGraphicsView>
#include <Eigen/Geometry>

class QPixmap;
class QMenu;
class QSlider;
class QGraphicsScene;

class SpaceObject;
class PropagatedScenario;


class GroundTrackSample
{
public:
    double mjd;
    double latitude;
    double longitude;
    double altitude;
};

class GroundTrackTick
{
public:
    float latitude;
    float longitude;
    float altitude;
    float dx;
    float dy;
};

class GroundTrack
{
public:
    SpaceObject* vehicle;
    QColor color;
    QList<GroundTrackSample> samples;
    QList<GroundTrackTick> ticks;
};


class GroundTrackView : public QGraphicsView
{
Q_OBJECT
 public:
    enum ProjectionType
    {
        Planar,
        Oblique,
    };

 public:
    GroundTrackView(QWidget* parent);
    ~GroundTrackView();
    
    void setScenario(PropagatedScenario* scenario);
    void computeTicks(GroundTrack& track, double interval);
    
    QPointF center() const { return m_center; }
    float zoomFactor() const { return m_zoomFactor; }
    double currentTime() const { return m_currentTime; }
    bool showEntireTracks() const { return m_showEntireTracks; }
    bool showGrid() const { return m_showGrid; }
    bool showTicks() const { return m_showTicks; }
    double tickInterval() const { return m_tickInterval; }
        
 public slots:
    void setCurrentTime(double jd);
    void setShowEntireTracks(bool show);
    void setShowGrid(bool show);
    void setShowTicks(bool show);
    void setTerminatorVisible(bool visible);
    void setTickInterval(double seconds);
    void setBody(const StaBody* body);
    void setProjection(ProjectionType projection);
    void set2HalfDView(bool enabled);
    void setMaxHeight(int logMaxHeight);
    void saveImage();
    void setZoomFactor(float zoomFactor);
    void setCenter(QPointF center);

 protected:
    void mouseMoveEvent(QMouseEvent* event);
    void mousePressEvent(QMouseEvent* event);
    void wheelEvent(QWheelEvent* event);
    void resizeEvent(QResizeEvent* event);
    void drawBackground(QPainter* painter, const QRectF& rect);
    void contextMenuEvent(QContextMenuEvent* event);
    
 private:
    bool addGroundTrack(SpaceObject* vehicle);
    void planetographicCoords(const Eigen::Vector3d& position,
                              const StaBody* body,
                              double* longitude, double* latitude, double* altitude) const;
    Eigen::Vector3d planetographicToCartesian(const StaBody* body,
                                              double longitude, double latitude, double altitude = 0.0) const;
    void constrainCenter();
    void computeGroundTrack(GroundTrack& track);
    void paintObliqueView(QPainter& painter);
    void paint2DView(QPainter& painter);
    double meridianSpacing(int maxMeridians);
    bool inView(float longitude, float latitude, float altitude);
    bool computeCoverageFootprint(double stationLongitude,
                                  double stationLatitude,
                                  const StaBody* planet,
                                  double minElevation,
                                  double spacecraftAltitude,
                                  unsigned int profilePoints,
                                  QVector<Eigen::Vector3d>& profile) const;
    void drawCoverageFootprint(QPainter& painter,
                               double stationLongitude,
                               double stationLatitude,
                               const StaBody* planet,
                               double minElevation,
                               double spacecraftAltitude);
    void drawSubsolarPoint(QPainter& painter, QColor color, float size);
    void drawDaylitRegion(QPainter& painter);

 private:
    PropagatedScenario* m_scenario;
    double m_currentTime;
    QPixmap* m_pixmap;
    bool m_pixmapOk;
    QList<GroundTrack*> m_groundTrackList;
    QVector<QPointF> m_trackPoints;
    QMenu* m_contextMenu;
    bool m_showEntireTracks;
    bool m_showGrid;
    bool m_showTicks;
    bool m_showTerminator;
    double m_tickInterval;
    const StaBody* m_body;

    float m_zoomFactor;
    QPointF m_center;
    float m_north;
    float m_south;
    float m_east;
    float m_west;
    float m_maxHeight;

    QPointF m_lastMousePosition;

    ProjectionType m_projection;

    QVector<Eigen::Vector3d> m_coverageProfile;

    // Elements of the graphics scene, which are overlayed on the map canvas
    QGraphicsScene* m_scene;
    QSlider* m_maxHeightSlider;
};


class GroundTrackPlotTool : public QWidget
{
Q_OBJECT
public:
    GroundTrackPlotTool(QWidget* parent);
    ~GroundTrackPlotTool();
    
    GroundTrackView* view() const { return m_view; }
    
private:
    GroundTrackView* m_view;
};

#endif // _PLOTTING_GROUNDTRACKPLOTTOOL_H_
