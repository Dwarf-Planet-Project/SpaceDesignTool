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
#include "PlotView3D.h"
#include <qwt3d_surfaceplot.h>
#include <QDebug>
#include <QVBoxLayout>
#include <cmath>
#include <limits>

using namespace Qwt3D;
using namespace Eigen;
using namespace std;


/** Create a new PlotView.
  */
PlotView3D::PlotView3D(QWidget* parent) :
    QWidget(parent),
    m_horizontalScale(NULL),
    m_verticalScale(NULL),
    m_topMargin(50.0f),
    m_bottomMargin(50.0f),
    m_leftMargin(80.0f),
    m_rightMargin(30.0f)
{
    m_plot = new SurfacePlot();
    m_plot->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);

    // Set axis style
    for (unsigned int i = 0; i < m_plot->coordinates()->axes.size(); ++i)
    {
        // Set the major and minor tick intervales
        m_plot->coordinates()->axes[i].setMajors(7);
        m_plot->coordinates()->axes[i].setMinors(4);

        m_plot->coordinates()->axes[i].setLabelFont("Helvetica", 14);
        //m_plot->coordinates()->axes[i].setAutoScale(false);
    }

    m_plot->coordinates()->setGridLines(true, true, FLOOR);
    m_plot->coordinates()->setGridLinesColor(RGBA(0.7, 0.7, 0.7, 1.0));
    m_plot->setTitleFont("Helvetica", 18);

    m_horizontalScale = new LinearPlotScale(0.0, 1.0);
    m_verticalScale = new LinearPlotScale(0.0, 1.0);

    m_plot->setMeshColor(RGBA(0.0, 0.0, 1.0, 1.0));

    m_plot->updateData();
    m_plot->updateGL();

    setLayout(new QVBoxLayout(this));
    layout()->addWidget(m_plot);
}


PlotView3D::~PlotView3D()
{
    removeAllPlots();
    delete m_horizontalScale;
    delete m_verticalScale;
}


/** Add a plot to the view.
  */
void
PlotView3D::addPlot(PlotDataSource3D* plotData, const PlotStyle& plotStyle)
{
    Q_ASSERT(plotData != NULL);
    if (plotData != NULL)
    {
        Plot* plot = new Plot;
        plot->data = plotData;
        plot->style = plotStyle;
        m_plots.append(plot);

        updatePlotData();
    }
}


/** Remove the specified plot from the view. This has no
  * effect if the plot isn't part of the view.
  */
void
PlotView3D::removePlot(PlotDataSource3D* plotData)
{
    Q_ASSERT(plotData != NULL);
    if (plotData != NULL)
    {
        int index = 0;
        while (index < m_plots.size() && m_plots[index]->data != plotData)
        {
            index++;
        }

        if (index < m_plots.size())
        {
            m_plots.remove(index);
            updatePlotData();
        }
    }
}


/** Remove all plots from the view.
  */
void
PlotView3D::removeAllPlots()
{
    foreach (Plot* p, m_plots)
    {
        delete p;
    }

    m_plots.clear();
    updatePlotData();
}


/** Set the title string for the plot.
  */
void
PlotView3D::setTitle(const QString& title)
{
    m_plot->setTitle(title);
}


/** Set the label that will be displayed on the left side of the plot.
  */
void
PlotView3D::setLeftLabel(const QString& label)
{
    m_leftLabel = label;
}


/** Set the label that will be displayed on the right side of the plot.
  */
void
PlotView3D::setRightLabel(const QString& label)
{
    m_rightLabel = label;
}


/** Set the label that will be displayed above the plot (and beneath the title.)
  */
void
PlotView3D::setTopLabel(const QString& label)
{
    m_topLabel = label;
}


/** Set the label that will be displayed beneat the plot.
  */
void
PlotView3D::setBottomLabel(const QString& label)
{
    m_bottomLabel = label;
}


/** Set the horizontal scale for this plot view.
  */
void
PlotView3D::setHorizontalScale(const PlotScale& scale)
{
    delete m_horizontalScale;
    m_horizontalScale = scale.clone();
}


/** Set the vertical scale for this plot view.
  */
void
PlotView3D::setVerticalScale(const PlotScale& scale)
{
    delete m_verticalScale;
    m_verticalScale = scale.clone();
}


/** Automatically set the horizontal and vertical scales to ranges
  * just large enough to contain all plots. The scales are *not*
  * automatically updated if the plot data changes after autoScale()
  * is called; to ensure that all data is displayed, autoScale()
  * must be called again.
  *
  * autoScale() has no effect if there are no PlotDataSources, or if
  * all PlotDataSources are empty.
  *
  * TODO: Currently, autoScale() always uses a linear scale; add option
  * to specify whether a linear or log scale should be used.
  */
void
PlotView3D::autoScale()
{
    bool empty = true;

    double minX =  numeric_limits<double>::infinity();
    double maxX = -numeric_limits<double>::infinity();
    double minY =  numeric_limits<double>::infinity();
    double maxY = -numeric_limits<double>::infinity();
    double minZ =  numeric_limits<double>::infinity();
    double maxZ = -numeric_limits<double>::infinity();

    foreach (const Plot* p, m_plots)
    {
        if (p->data->getPointCount() > 0)
        {
            empty = false;
        }

        for (unsigned int i = 0; i < p->data->getPointCount(); ++i)
        {
            Vector3d v = p->data->getPoint(i);
            minX = min(minX, v.x());
            maxX = max(maxX, v.x());
            minY = min(minY, v.y());
            maxY = max(maxY, v.y());
        }
    }

    if (!empty)
    {
        setHorizontalScale(LinearPlotScale(minX, maxX));
        setVerticalScale(LinearPlotScale(minY, maxY));
    }
}


void
PlotView3D::updatePlotData()
{
    QVector<Triple> triples;

    foreach (Plot* plot, m_plots)
    {
        for (unsigned int i = 0; i < plot->data->getPointCount(); ++i)
        {
            Vector3d p = plot->data->getPoint(i);
            triples << Triple(p.x(), p.y(), p.z());
        }
    }

    // No data in the graph. It's likely that a bad time range was provided.
    if (triples.empty())
    {
        return;
    }

    const Triple* data[2];
    data[0] = triples.constData();
    data[1] = triples.constData();

    // TODO: Ugly const cast here. Need to patch QwtPlot3D to take const data instead.
    m_plot->loadFromData(const_cast<Triple**>(data), 2, triples.size(), false, false);
    m_plot->updateData();

#if 0
    // Set the scale on all the axes

    m_plot->coordinates()->axes[X1].setLimits(axisMin.x(), axisMax.x());
    m_plot->coordinates()->axes[X2].setLimits(axisMin.x(), axisMax.x());
    m_plot->coordinates()->axes[X3].setLimits(axisMin.x(), axisMax.x());
    m_plot->coordinates()->axes[X4].setLimits(axisMin.x(), axisMax.x());

    m_plot->coordinates()->axes[Y1].setLimits(axisMin.y(), axisMax.y());
    m_plot->coordinates()->axes[Y2].setLimits(axisMin.y(), axisMax.y());
    m_plot->coordinates()->axes[Y3].setLimits(axisMin.y(), axisMax.y());
    m_plot->coordinates()->axes[Y4].setLimits(axisMin.y(), axisMax.y());

    m_plot->coordinates()->axes[Z1].setLimits(axisMin.z(), axisMax.z());
    m_plot->coordinates()->axes[Z2].setLimits(axisMin.z(), axisMax.z());
    m_plot->coordinates()->axes[Z3].setLimits(axisMin.z(), axisMax.z());
    m_plot->coordinates()->axes[Z4].setLimits(axisMin.z(), axisMax.z());
#endif

    m_plot->updateGL();
}
