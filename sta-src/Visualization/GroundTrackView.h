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
  Patched by Guillermo on June 2010 to include the constellations module
 */

#ifndef _STA_VISUALIZATION_GROUND_TRACK_VIEW_H_
#define _STA_VISUALIZATION_GROUND_TRACK_VIEW_H_

#include "GroundTrack.h"
#include "Constellations/canalysis.h"
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

    QPointF center() const { return m_center; }
    float zoomFactor() const { return m_zoomFactor; }
    double currentTime() const { return m_currentTime; }
    bool showGrid() const { return m_showGrid; }
    bool showEquator() const { return m_showEquator; }
    bool showTicks() const { return m_showTicks; }
    double tickInterval() const { return m_tickInterval; }

    // Analysis (Claas Grohnfeldt, Steffen Peter)
    void setAnalysis(ConstellationAnalysis* analysisOfConstellations);

 public slots:
    void setCurrentTime(double jd);
    void setShowGrid(bool show);
    void setShowEquator(bool show);
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

    // Analysis (Claas Grohnfeldt, Steffen Peter)
    void setDiscretizationVisible(bool visible);
    void setCoverageCurrentVisible(bool visible);
    void setCoverageHistoryVisible(bool visible);
    void setLinkSOVisible(bool visible);
    void setLinkGOVisible(bool visible);

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

    // Analysis (Claas Grohnfeldt, Steffen Peter)
    void drawDiscretization(QPainter& painter, QList<DiscretizationPoint> discretePoints);
    void drawCoverage(QPainter& painter, QColor asocolor, bool seenPoints, QList<DiscretizationPoint> discretePoints);

 private:
    PropagatedScenario* m_scenario;
    // Analysis (Claas Grohnfeldt, Steffen Peter)
    ConstellationAnalysis* m_analysis;


    double m_currentTime;
    QPixmap* m_pixmap;
    bool m_pixmapOk;
    QList<GroundTrack*> m_groundTrackList;
    QVector<QPointF> m_trackPoints;
    QMenu* m_contextMenu;
    bool m_showGrid;
    bool m_showEquator;
    bool m_showTicks;
    bool m_showTerminator;
    double m_tickInterval;
    const StaBody* m_body;
    double m_trackDisplayDuration;

    // Analysis (Claas Grohnfeldt, Steffen Peter)
    bool m_showAnalysis;
    bool m_showDiscretization;
    bool m_showCoverageCurrent;
    bool m_showCoverageHistory;
    bool m_showSOLink;
    bool m_showGOLink;

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

#endif // _STA_VISUALIZATION_GROUND_TRACK_VIEW_H_
