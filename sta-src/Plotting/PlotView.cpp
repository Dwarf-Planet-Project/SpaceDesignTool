/*

 This program is free software; you can redistribute it and/or modify it under
 the terms of the GNU General Public License as published by the Free Software
 Foundation; either version 2 of the License, or (at your option) any later
 version.

 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE. See the GNU General Public License for more
 details.

 You should have received a copy of the GNU General Public License along with
 this program; if not, write to the Free Software Foundation, Inc., 59 Temple
 Place - Suite 330, Boston, MA 02111-1307, USA.
 Further information about the GNU General Public License can also be found on
 the world wide web at http://www.gnu.org.
 */


/*
 --- Copyright (C) 2010 STA Steering Board (space.trajectory.analysis AT gmail.com) ---
*/


//------------------ Author:       Chris Laurel                   -------------------
//-----------------------------------------------------------------------------------


#include "PlotDataSource.h"
#include "PlotView.h"
#include <QPainter>

using namespace Eigen;


PlotView::PlotView(QWidget* parent) :
    QWidget(parent)
{
}


PlotView::~PlotView()
{
    removeAllPlots();
}


void
PlotView::addPlot(PlotDataSource* plotData)
{
    Q_ASSERT(plotData != NULL);
    if (plotData != NULL)
    {
        m_plots.append(plotData);
    }
}


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


void
PlotView::removeAllPlots()
{
    m_plots.clear();
}


void
PlotView::paintEvent(QPaintEvent* /* event */)
{
    QPainter painter(this);

    painter.fillRect(0, 0, width(), height(), QColor(0, 0, 255));

    painter.setPen(Qt::lightGray);
    painter.drawText(100, 100, m_title);

    // Draw grid

    // Draw plots
    foreach (PlotDataSource* plot, m_plots)
    {
        painter.setPen(Qt::red);
        QPointF lastPoint(0.0f, 0.0f);
        for (unsigned int i = 0; i < plot->getPointCount(); ++i)
        {
            Vector2d point = plot->getPoint(i);
            QPointF pf(float(point.x()), float(point.y()));
            if (i != 0)
            {
                painter.drawLine(lastPoint, pf);
            }
            lastPoint = pf;
        }
    }

    // Draw axes
}


void
PlotView::setTitle(const QString& title)
{
    m_title = title;
}
