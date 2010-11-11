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

#ifndef _STA_VISUALIZATION_GROUND_TRACK_H_
#define _STA_VISUALIZATION_GROUND_TRACK_H_

#include <QList>
#include <QColor>
#include <QPainter>
#include <Eigen/Core>
#include <Eigen/Geometry>
#include <deque>

class SpaceObject;
class StaBody;

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

// A GroundTrackSegment corresponds to a mission arc
class GroundTrackSegment
{
public:
    QColor color;
    QList<GroundTrackSample> samples;
    QList<GroundTrackTick> ticks;

};

// There's one ground track per SpaceObject
class GroundTrack
{
public:
    GroundTrack();
    ~GroundTrack();
    void draw2D(QPainter& painter, const StaBody* body, double mjd, const Eigen::AlignedBox<float, 2>& clipBox);
    void drawTicks(QPainter& painter, const StaBody* body, double mjd, float tickInterval, float tickScale, const Eigen::AlignedBox<float, 2>& clipBox);
    void draw(QPainter& painter, const StaBody* body, double mjd, const Eigen::Transform3f& projection, const Eigen::AlignedBox<float, 3>& clipBox);
    void drawDropLines(QPainter& painter, const StaBody* body, double mjd, float tickInterval, const Eigen::Transform3f& projection, const Eigen::AlignedBox<float, 3>& clipBox);

    SpaceObject* vehicle;
    QList<GroundTrackSegment*> segments;
    QVector<QPointF> m_trackPoints;
};

#endif _STA_VISUALIZATION_GROUND_TRACK_H_
