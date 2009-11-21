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

#ifndef _MAIN_TIMELINEVIEW_H_
#define _MAIN_TIMELINEVIEW_H_

#include <QAbstractScrollArea>
#include <QList>


class TimelineView : public QAbstractScrollArea
{
Q_OBJECT
 public:
    TimelineView(QWidget *parent = 0);
    ~TimelineView();
    
    void setTimeRange(double startTime, double endTime);
    double startTime() const { return m_startTime; }
    double endTime() const { return m_endTime; }
    
    void setVisibleSpan(double duration);
    double visibleSpan() const { return m_visibleSpan; }
    
    void clearMission();
    void addMissionSegment(int participantIndex,
                           double startTime,
                           double endTime,
                           QColor color);

    double currentTime() const { return m_currentTime; }

 public slots:
    void setCurrentTime(double jd);
    
 signals:
    void timelineClicked(double jd);
    void participantClicked(int participantIndex);
    
 protected:
    void paintEvent(QPaintEvent* event);
    void mouseReleaseEvent(QMouseEvent* event);
    void mouseMoveEvent(QMouseEvent* event);
    void resizeEvent(QResizeEvent* event);
    
 private:
    void updateScrollBars();
    
 private:
    double m_startTime;
    double m_endTime;
    double m_visibleSpan;
    double m_currentTime;
    int m_participantCount;
    
    struct MissionSegment
    {
        int participantIndex;
        double startTime;
        double endTime;
        QColor color;
    };
    
    QList<MissionSegment> mission;
};

#endif // _MAIN_TIMELINEVIEW_H_





