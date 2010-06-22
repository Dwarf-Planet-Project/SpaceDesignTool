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

//------------------ Author:       Chris Laurel                   -------------------
//-----------------------------------------------------------------------------------


#include "PlotDataSource.h"
#include "PlotView.h"
#include <QPainter>
#include <cmath>

using namespace Eigen;
using namespace std;


/** Create a new PlotView.
  */
PlotView::PlotView(QWidget* parent) :
    QWidget(parent),
    m_horizontalScale(NULL),
    m_verticalScale(NULL),
    m_topMargin(50.0f),
    m_bottomMargin(25.0f),
    m_leftMargin(50.0f),
    m_rightMargin(20.0f)
{
    m_horizontalScale = new LinearPlotScale(0.0, 1.0);
    m_verticalScale = new LinearPlotScale(0.0, 1.0);
}


PlotView::~PlotView()
{
    removeAllPlots();
    delete m_horizontalScale;
    delete m_verticalScale;
}


/** Add a plot to the view.
  */
void
PlotView::addPlot(PlotDataSource* plotData)
{
    Q_ASSERT(plotData != NULL);
    if (plotData != NULL)
    {
        m_plots.append(plotData);
    }
}


/** Remove the specified plot from the view. This has no
  * effect if the plot isn't part of the view.
  */
void
PlotView::removePlot(PlotDataSource* plotData)
{
    Q_ASSERT(plotData != NULL);
    if (plotData != NULL)
    {
        int index = m_plots.indexOf(plotData);
        if (index >= 0)
        {
            m_plots.remove(index);
        }
    }
}


/** Remove all plots from the view.
  */
void
PlotView::removeAllPlots()
{
    m_plots.clear();
}


/** Redraw the plot in response to a Qt paint event.
  */
void
PlotView::paintEvent(QPaintEvent* /* event */)
{
    QPainter painter(this);

    painter.fillRect(0, 0, width(), height(), Qt::white);

    // Draw the title
    painter.setPen(Qt::black);
    painter.setFont(QFont("Helvetica", 14, QFont::Bold));
    painter.drawText(QRectF(m_leftMargin, 0.0f, width() - m_rightMargin - m_leftMargin, m_topMargin),
                     Qt::AlignLeft | Qt::AlignVCenter,
                     m_title);
    painter.setFont(QFont("Helvetica", 10, QFont::Normal));

    // Scale factors to map values in the unit interval to physical coordinates
    float xscale = width() - m_leftMargin - m_rightMargin;
    float yscale = height() - m_topMargin - m_bottomMargin;

    // Draw grid
    painter.setPen(Qt::lightGray);
    QList<double> xTicks = m_horizontalScale->ticks();
    QList<double> yTicks = m_verticalScale->ticks();
    foreach (double x, xTicks)
    {
        float sx = float(m_horizontalScale->scaled(x)) * xscale;
        painter.drawLine(QPointF(m_leftMargin + sx, m_topMargin), QPointF(m_leftMargin + sx, m_topMargin + yscale));
    }

    foreach (double y, yTicks)
    {
        float sy = float(m_verticalScale->scaled(y)) * yscale;
        painter.drawLine(QPointF(m_leftMargin, m_topMargin + yscale - sy), QPointF(m_leftMargin + xscale, m_topMargin + yscale - sy));
    }

    // Draw the grid labels
    painter.setPen(Qt::black);
    float maxLabelWidth = 100.0f;
    float labelHeight = 30.0f;

    foreach (double x, xTicks)
    {
        float sx = float(m_horizontalScale->scaled(x)) * xscale + m_leftMargin;
        painter.drawText(QRectF(sx - maxLabelWidth / 2.0f, height() - m_bottomMargin + 5, maxLabelWidth, labelHeight),
                         Qt::AlignHCenter | Qt::AlignTop,
                         QString("%1").arg(x));
    }

    foreach (double y, yTicks)
    {
        float sy = (1.0f - float(m_verticalScale->scaled(y))) * yscale + m_topMargin;
        painter.drawText(QRectF(0.0f, sy - labelHeight / 2.0f, m_leftMargin - 5.0f, labelHeight),
                         Qt::AlignVCenter | Qt::AlignRight,
                         QString("%1").arg(y));
    }

    // Draw plots
    foreach (PlotDataSource* plot, m_plots)
    {
        painter.setPen(QPen(Qt::red, 2.0f));
        QPointF lastPoint(0.0f, 0.0f);
        for (unsigned int i = 0; i < plot->getPointCount(); ++i)
        {
            QPointF pf = toViewCoords(plot->getPoint(i));
            if (i != 0)
            {
                painter.drawLine(lastPoint, pf);
            }
            lastPoint = pf;
        }
    }

    // Draw axes
}


/** Set the title string for the plot.
  */
void
PlotView::setTitle(const QString& title)
{
    m_title = title;
}


/** Set the horizontal scale for this plot view.
  */
void
PlotView::setHorizontalScale(const PlotScale& scale)
{
    delete m_horizontalScale;
    m_horizontalScale = scale.clone();
}


/** Set the vertical scale for this plot view.
  */
void
PlotView::setVerticalScale(const PlotScale& scale)
{
    delete m_verticalScale;
    m_verticalScale = scale.clone();
}


// Map a point from data coordinates to view coordinates
QPointF
PlotView::toViewCoords(const Vector2d& p)
{
    float xscale = width() - m_leftMargin - m_rightMargin;
    float yscale = height() - m_topMargin - m_bottomMargin;
    return QPointF(m_leftMargin + float(m_horizontalScale->scaled(p.x())) * xscale,
                   m_topMargin + yscale * (1.0f - float(m_verticalScale->scaled(p.y()))));
}


QList<double>
LinearPlotScale::ticks() const
{
    QList<double> t;
    const double maxTicks = 10;
    double range = m_maxValue - m_minValue;
    double idealSpacing = range / maxTicks;
    double exponent = floor(log10(idealSpacing));
    double p = pow(10.0, exponent);
    double mantissa = idealSpacing / p;

    // Choose an appropriate tick spacing with round numbers
    double tickSpacing = 1.0;
    if (mantissa < 2.0)
    {
        tickSpacing = 2.0 * p;
    }
    else if (mantissa < 3.0)
    {
        tickSpacing = 3.0 * p;
    }
    else if (mantissa < 5.0)
    {
        tickSpacing = 5.0 * p;
    }
    else
    {
        tickSpacing = 10.0 * p;
    }

    double firstTick = ceil(m_minValue / tickSpacing) * tickSpacing;
    double lastTick = floor(m_maxValue / tickSpacing) * tickSpacing;

    /*
    for (unsigned int i = 0; i < tickCount; i++)
    {
        t << m_minValue + (m_maxValue - m_minValue) * double(i) / double(tickCount - 1);
    }
    */
    for (double tick = firstTick; tick <= lastTick; tick += tickSpacing)
    {
        t << tick;
    }

    return t;
}


