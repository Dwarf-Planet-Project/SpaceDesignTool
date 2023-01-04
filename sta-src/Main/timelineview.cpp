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
 Patched by Guillermo July 2010 to tune up the sizes of the bars, time-now, etc in Windows and MAC
 */

#include "timelineview.h"
#include "Astro-Core/date.h"
#include <cmath>
#include <algorithm>
#include <QPainter>
#include <QMouseEvent>
#include <QScrollBar>
#include <QDateTime>
#include <QDebug>
#include <QTimer>

#include <iostream>


static const float TimeHeaderHeight = 40.0f;
static const float MissionSegmentBarThickness = 18.0f; // pixels
static const float MissionSegmentBarSpacing = 8.0f;    // pixels

// Set to zero to disable timeline zoom animations
#define ANIMATED_ZOOM 1


TimelineView::TimelineView(QWidget* parent) :
		QAbstractScrollArea(parent),
		m_currentTime(0.0),
		m_startTime(0.0),
		m_endTime(1.0),
		m_visibleSpan(1.0 / 24.0),
        m_participantCount(0),
        m_mouseMotion(0),
        m_timer(NULL),
        m_autoScrollRate(0.0),
        m_targetVisibleSpan(0.0)
{
    updateScrollBars();

    // The timer is used for autoscrolling and for animated zooms
    m_timer = new QTimer(this);
    connect(m_timer, SIGNAL(timeout()), this, SLOT(animate()));
}


TimelineView::~TimelineView()
{
}


void
TimelineView::setTimeRange(double startTime, double endTime)
{
    m_startTime = startTime;
    m_endTime = endTime;
    updateScrollBars();

    viewport()->update();
}


void
TimelineView::setVisibleSpan(double duration)
{
#if ANIMATED_ZOOM
    // The code below triggers an animated zoom. Choose the zoom scale factor
    // so that the zoom animation completes in 1/3 second: slow enough for the
    // user to understand the visual cue, fast enough to not be annoying.
    m_timer->start(16);  // ~60 fps
    m_targetVisibleSpan = std::min(duration, m_endTime - m_startTime);
    m_zoomScaleFactor = std::pow(m_targetVisibleSpan / m_visibleSpan, 1.0 / 20.0);
#else
    m_visibleSpan = duration;
    updateScrollBars();

    double viewStartTime = sta::secsToDays(horizontalScrollBar()->value()) + m_startTime;

    // When zooming, recenter the view if the current time is too far to the left or right
    bool recenter = m_currentTime < viewStartTime + m_visibleSpan * 0.1 || m_currentTime > viewStartTime + m_visibleSpan * 0.9;

    if (recenter)
    {
        double middle = m_currentTime - duration / 2.0;
        horizontalScrollBar()->setValue(int(sta::daysToSecs(middle - m_startTime)));
    }

    viewport()->update();
#endif
    horizontalScrollBar()->setSingleStep(int(sta::daysToSecs(duration / 100.0)));
}


void
TimelineView::setCurrentTime(double mjd)
{
    if (mjd != m_currentTime)
    {
        m_currentTime = mjd;
        emit currentTimeChanged(m_currentTime);
        viewport()->update();
    }
}


void
TimelineView::paintEvent(QPaintEvent* /* event */)
{
    QPainter painter(viewport());
    int viewWidth = viewport()->size().width();
    int viewHeight = viewport()->size().height();
    
    //QFont font("Helvetica", 9);
    //painter.setFont(font);

    painter.fillRect(0, 0, viewWidth, viewHeight, QBrush(Qt::white));
    
    int top = verticalScrollBar()->value();
    double viewStartTime = horizontalScrollBar()->value() / 86400.0 + m_startTime;
    double subdivMins = 0.0;
    const double julianDay = 1440.0;

    // Select the tick interval
    if (m_visibleSpan <= 0.25 / 24.0)
        subdivMins = 1.0;
    else if (m_visibleSpan <= 0.5 / 24.0)
        subdivMins = 2.0;
    else if (m_visibleSpan <= 1.0 / 24.0)
        subdivMins = 5.0;
    else if (m_visibleSpan <= 1.0 / 12.0)
        subdivMins = 15.0;
    else if (m_visibleSpan <= 0.25)
        subdivMins = 30.0;
    else if (m_visibleSpan <= 0.5)
        subdivMins = 60.0;
    else if (m_visibleSpan <= 1.0)
        subdivMins = 120.0;
    else if (m_visibleSpan <= 2.0)
        subdivMins = 240.0;
    else if (m_visibleSpan <= 5.0)
        subdivMins = 720.0;
    else if (m_visibleSpan <= 10.0)
        subdivMins = 1 * julianDay;
    else if (m_visibleSpan <= 20.0)
        subdivMins = 2 * julianDay;
    else if (m_visibleSpan <= 50.0)
        subdivMins = 5 * julianDay;
    else if (m_visibleSpan <= 100.0)
        subdivMins = 10 * julianDay;
    else if (m_visibleSpan <= 200.0)
        subdivMins = 20 * julianDay;
    else if (m_visibleSpan <= 500.0)
        subdivMins = 50 * julianDay;
    else if (m_visibleSpan <= 1000.0)
        subdivMins = 100 * julianDay;
    else
        subdivMins = 365 * julianDay;
    
    double subdiv = subdivMins / 1440.0;
    double t0 = std::floor(viewStartTime / subdiv) * subdiv;

    QBrush lightGrayBrush(QColor(230, 230, 230));
    QBrush darkGrayBrush(QColor(210, 210, 210));
    bool shade = ((qint64) (std::floor(t0 / subdiv + 0.5)) & 0x1) == 1;

    double viewEndTime = std::min(m_endTime, viewStartTime + m_visibleSpan);

    // Fill the background
    painter.fillRect(QRect(0, 0, viewWidth, viewHeight), lightGrayBrush);

    // Draw ticks
    painter.setPen(QColor(180, 180, 180));
    for (double t = t0; t < viewEndTime; t += subdiv)
    {
        float blockWidth = (subdiv / m_visibleSpan) * viewWidth;
        float x = (float) (viewWidth * (t - viewStartTime) / m_visibleSpan);
        painter.drawLine(QPointF(x, 15.0f), QPointF(x, float(viewHeight)));

        // Draw small 'subticks', 5 for each labeled tick
        for (unsigned int i = 1; i < 6; i++)
        {
            float tickX = x + i * blockWidth / 6.0f;
            painter.drawLine(QPointF(tickX, 10.0f), QPointF(tickX, 15.0f));
        }
    }

    QFontMetrics fontMetrics = painter.fontMetrics();

    // Draw the mission duration bars for each participant
    foreach (MissionSegment segment, mission)
    {
        if (segment.endTime > viewStartTime && segment.startTime < viewEndTime)
        {
            float y0 = TimeHeaderHeight + segment.participantIndex * (MissionSegmentBarThickness + MissionSegmentBarSpacing);
            float y1 = y0 + MissionSegmentBarThickness;
            float x0 = (float) (viewWidth * (segment.startTime - viewStartTime) / m_visibleSpan);
            float x1 = (float) (viewWidth * (segment.endTime - viewStartTime) / m_visibleSpan);
            
            QRectF missionSegmentRect(x0, y0 - top, x1 - x0, y1 - y0);
            painter.fillRect(missionSegmentRect, QBrush(segment.color));
            painter.setPen(segment.color.darker(130));
            painter.drawRect(missionSegmentRect);

            // Draw labels for the mission segments (as long as they fit in
            // the visible area of the bar)
            const int labelWidth = fontMetrics.width(segment.name);
            const float margin = 5.0f;
            const float minSpace = labelWidth + margin * 2.0f;
            const float leftEdge = margin;
            const float rightEdge = width() - margin;

            float span = x1 - x0;
            if (span > minSpace)
            {
                painter.setPen(segment.color.darker(200));

                // Keep the label centered in the visible portion of the mission segment bar
                QRectF textRect = missionSegmentRect;
                textRect.setLeft(std::max(leftEdge, x0));
                textRect.setRight(std::min(rightEdge, x1));
                if (textRect.width() > minSpace)
                {
                    painter.drawText(textRect, segment.name, QTextOption(Qt::AlignCenter));
                }
            }
        }
    }

    QDateTime indicatorDateTime = sta::JdToCalendar(sta::MjdToJd(m_currentTime));

    // Draw a line indicating the current time
    // Show the current time and date next to the indicator
    {
        float x = (float) (viewWidth * (m_currentTime - viewStartTime) / m_visibleSpan);
        QPen guillermosPen(QColor(128, 0, 0), 2);
        painter.setPen(guillermosPen);
        painter.drawLine(QPointF(x, 2), QPointF(x, viewHeight - 2));

        float labelWidth = painter.fontMetrics().width("00 Mmm 00:00:00");
        int alignment = Qt::AlignBottom;

        // Draw the date label on the right of the indicator except when it
        // is position close to the right edge of the window.
        float left;
        if (x + labelWidth > viewWidth)
        {
            alignment |= Qt::AlignRight;
            left = x - 5 - labelWidth;
        }
        else
        {
            alignment |= Qt::AlignLeft;
            left = x + 5;
        }

        // Show the current time next to the indicator line
        painter.drawText(QRectF(left, viewHeight - 20.0f, labelWidth, 20.0f),
                         alignment,
                         indicatorDateTime.toString("\ndd MMM hh:mm:ss"));
    }

    // Draw the time labels
	painter.setPen(Qt::black);
	for (double t = t0; t < viewEndTime; t += subdiv)
    {
        double decimalTime = t - std::floor(t);
        decimalTime += 0.01 / 86400.0; // Adjust for rounding errors
        
        int hour = (int) (std::floor(decimalTime * 24)) % 24;
        int minute = (int) std::floor(decimalTime * 1440) % 60;
        int second = (int) std::floor(decimalTime * 86400) % 60;
        
        if (subdiv >= 1.0 || (hour == 0 && minute == 0 && second == 0))
        {
            // Show the date and time of day
            QDateTime dateTime = sta::JdToCalendar(sta::MjdToJd(t));
            
            float x = (float) (viewWidth * (t - viewStartTime) / m_visibleSpan);
            painter.drawText(QRectF(x - 40.0f, 0.0f, 80.0f, TimeHeaderHeight),
                             Qt::AlignHCenter | Qt::AlignTop,
                             dateTime.toString("hh:mm\ndd MMM yyyy"));
        }
        else
        {
            // Show the time of day
            QTime tm(hour, minute, second);
            float x = (float) (viewWidth * (t - viewStartTime) / m_visibleSpan);
            painter.drawText(QRectF(x - 20.0f, 0.0f, 40.0f, TimeHeaderHeight),
                             Qt::AlignHCenter | Qt::AlignTop,
							 tm.toString("hh:mm"));
        }
    }


}


void
TimelineView::mousePressEvent(QMouseEvent* event)
{
    m_mouseMotion = 0.0f;
    m_lastMousePosition = event->pos();
}


void 
TimelineView::mouseReleaseEvent(QMouseEvent* event)
{
    // Stop autoscrolling
    m_timer->stop();
    m_autoScrollRate = 0;

    int viewWidth = viewport()->size().width();
    double viewStartTime = horizontalScrollBar()->value() / 86400.0 + m_startTime;

    double mjd = viewStartTime + m_visibleSpan * (double) event->x() / (double) viewWidth;
    setCurrentTime(mjd);

    // Emit a signal if one of the mission segment bars was clicked and the mouse
    // didn't move much since it was pressed.
    bool mouseMoved = m_mouseMotion > 4;

    if (!mouseMoved)
    {
        int participantIndex = -1;
        int y = event->y() - TimeHeaderHeight + verticalScrollBar()->value();
        int h = (int) (MissionSegmentBarSpacing + MissionSegmentBarThickness);
        if (y >= 0 && y % h < MissionSegmentBarThickness)
            participantIndex = y / h;

        foreach (MissionSegment segment, mission)
        {
            if (mjd >= segment.startTime && mjd <= segment.endTime &&
                participantIndex == segment.participantIndex)
            {
                emit participantClicked(participantIndex);
            }
        }
    }
}


void
TimelineView::mouseMoveEvent(QMouseEvent* event)
{
    // Track total mouse motion so that we can distinguish click/release from drags
    m_mouseMotion += (m_lastMousePosition - event->pos()).manhattanLength();
    m_lastMousePosition = event->pos();


    int viewWidth = viewport()->size().width();
    double viewStartTime = horizontalScrollBar()->value() / 86400.0 + m_startTime;
    
    double mjd = viewStartTime + m_visibleSpan * (double) event->x() / (double) viewWidth;
    setCurrentTime(mjd);

    // Autoscroll if the time indicator is dragged into the leftmost or rightmost region of
    // the timeline.
    int autoScrollRegionWidth = viewWidth / 10;
    double x = event->pos().x();
    if (x < autoScrollRegionWidth || x > viewWidth - autoScrollRegionWidth)
    {
        if (!m_timer->isActive())
        {
            m_timer->start(50);  // Fire 20 times/second
        }
        if (x < autoScrollRegionWidth)
        {
            m_autoScrollRate = -horizontalScrollBar()->singleStep() / 2.0 * (1.0 - x / autoScrollRegionWidth);
        }
        else
        {
            m_autoScrollRate = horizontalScrollBar()->singleStep() / 2.0 * ((x - viewWidth) / autoScrollRegionWidth + 1.0);
        }
    }
    else
    {
        m_autoScrollRate = 0.0;
    }
}


void
TimelineView::resizeEvent(QResizeEvent* event)
{
    updateScrollBars();
}


void
TimelineView::updateScrollBars()
{
    horizontalScrollBar()->setRange(0, (int) ((m_endTime - m_startTime) * 86400));
    horizontalScrollBar()->setPageStep((int) (m_visibleSpan * 86400));

    int height = (int) (MissionSegmentBarSpacing + MissionSegmentBarThickness) * m_participantCount + TimeHeaderHeight;
    if (height > viewport()->size().height())
    {
        verticalScrollBar()->setRange(0, height - viewport()->size().height());
    }
    else
    {
        verticalScrollBar()->setRange(0, 0);
    }
}


void
TimelineView::clearMission()
{
    mission.clear();
    viewport()->update();
    m_participantCount = 0;
}


void
TimelineView::addMissionSegment(int participantIndex,
                                double startTime,
                                double endTime,
                                QColor color,
                                const QString& name)
{
    MissionSegment segment;
    segment.participantIndex = participantIndex;
    segment.startTime = startTime;
    segment.endTime = endTime;
    segment.color = color;
    segment.name = name;
    
    mission.append(segment);

    m_participantCount = std::max(m_participantCount, participantIndex + 1);
    updateScrollBars();
}


void
TimelineView::animate()
{
    // Handle auto scrolling
    if (m_autoScrollRate != 0.0)
    {
        int viewWidth = viewport()->size().width();
        double viewStartTime = horizontalScrollBar()->value() / 86400.0 + m_startTime;
        double mjd = viewStartTime + m_visibleSpan * (double) m_lastMousePosition.x() / (double) viewWidth;
        setCurrentTime(mjd);

        horizontalScrollBar()->setSliderPosition(horizontalScrollBar()->sliderPosition() + m_autoScrollRate);
    }

    // Handle animated zooming
    if (m_targetVisibleSpan != 0.0 && m_visibleSpan != m_targetVisibleSpan)
    {
        double x = sta::daysToSecs(m_currentTime - m_startTime);
        double f = (x - horizontalScrollBar()->value()) / sta::daysToSecs(m_visibleSpan);

        if (m_targetVisibleSpan > m_visibleSpan)
        {
            m_visibleSpan = std::min(m_visibleSpan * m_zoomScaleFactor, m_targetVisibleSpan);
        }
        else
        {
            m_visibleSpan = std::max(m_visibleSpan * m_zoomScaleFactor, m_targetVisibleSpan);
        }

        horizontalScrollBar()->setValue(int(x - sta::daysToSecs(m_visibleSpan) * f));

        updateScrollBars();
        viewport()->update();

        if (m_visibleSpan == m_targetVisibleSpan)
        {
            m_timer->stop();
        }
    }
}
