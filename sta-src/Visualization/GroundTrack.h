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
#include <limits>

class MissionArc;
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
    double mjd;
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
    GroundTrackSegment();
    void updateSamples(SpaceObject* spaceObj, MissionArc* arc, const StaBody* body, double plotStartTime, double plotEndTime, double sampleSpacing);
    void clearSamples();
    void updateTicks(SpaceObject* spaceObj, MissionArc* arc, const StaBody* body, double plotStartTime, double plotEndTime, double tickInterval);
    void clearTicks();

    QColor color() const
    {
        return m_color;
    }

    void setColor(const QColor& color)
    {
        m_color = color;
    }

    MissionArc* missionArc() const
    {
        return m_missionArc;
    }

    void setMissionArc(MissionArc* missionArc)
    {
        m_missionArc = missionArc;
    }

    int sampleCount() const
    {
        return m_samples.size();
    }

    QList<GroundTrackSample>& samples()
    {
        return m_samples;
    }

    const QList<GroundTrackSample>& samples() const
    {
        return m_samples;
    }

    const GroundTrackSample& sample(int index) const
    {
        return m_samples[index];
    }

    int tickCount() const
    {
        return m_ticks.size();
    }

    QList<GroundTrackTick> ticks() const
    {
        return m_ticks;
    }

    const GroundTrackTick& tick(int index) const
    {
        return m_ticks[index];
    }

    /** Get the segment start time (MJD)
      */
    double startTime() const
    {
        if (m_samples.isEmpty())
        {
            return 0.0;
        }
        else
        {
            return m_samples.first().mjd;
        }
    }

    /** Get the segment end time (MJD)
      */
    double endTime() const
    {
        if (m_samples.isEmpty())
        {
            return 0.0;
        }
        else
        {
            return m_samples.last().mjd;
        }
    }

    double firstTickTime() const
    {
        if (m_ticks.isEmpty())
        {
            return 0.0;
        }
        else
        {
            return m_ticks.first().mjd;
        }
    }

    double lastTickTime() const
    {
        if (m_ticks.isEmpty())
        {
            return 0.0;
        }
        else
        {
            return m_ticks.last().mjd;
        }
    }

    bool includesTime(double mjd) const
    {
        return mjd >= startTime() && mjd < endTime();
    }

private:
    MissionArc* m_missionArc;
    QColor m_color;
    QList<GroundTrackSample> m_samples;
    QList<GroundTrackTick> m_ticks;

};

// There's one ground track per SpaceObject
class GroundTrack
{
public:
    GroundTrack();
    ~GroundTrack();
    void draw2D(QPainter& painter, const StaBody* body, double startTime, double endTime, const Eigen::AlignedBox<float, 2>& clipBox);
    void drawTicks(QPainter& painter, const StaBody* body, double startTime, double endTime, float tickScale, const Eigen::AlignedBox<float, 2>& clipBox);
    void draw(QPainter& painter, const StaBody* body, double startTime, double endTime, const Eigen::Transform3f& projection, const Eigen::AlignedBox<float, 3>& clipBox);
    void drawDropLines(QPainter& painter, const StaBody* body, double startTime, double endTime, float tickInterval, const Eigen::Transform3f& projection, const Eigen::AlignedBox<float, 3>& clipBox);

    void updateSamples(SpaceObject* spaceObj, const StaBody* body, double plotStartTime, double plotEndTime);
    void updateTicks(SpaceObject* spaceObj, const StaBody* body, double plotStartTime, double plotEndTime, double tickSpacing);
    void clearSamples();
    void clearTicks();

    SpaceObject* m_vehicle;
    QList<GroundTrackSegment*> m_segments;
    QVector<QPointF> m_trackPoints;
};

#endif _STA_VISUALIZATION_GROUND_TRACK_H_
