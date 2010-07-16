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
    m_xScale(NULL),
    m_yScale(NULL),
    m_zScale(NULL),
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
    }

    m_plot->coordinates()->setGridLines(true, true, FLOOR);
    m_plot->coordinates()->setGridLinesColor(RGBA(0.7, 0.7, 0.7, 1.0));
    m_plot->setTitleFont("Helvetica", 18);

    m_xScale = new LinearPlotScale(0.0, 1.0);
    m_yScale = new LinearPlotScale(0.0, 1.0);
    m_zScale = new LinearPlotScale(0.0, 1.0);

    m_plot->setMeshColor(RGBA(0.0, 0.0, 1.0, 1.0));

    m_plot->updateData();
    m_plot->updateGL();

    setLayout(new QVBoxLayout(this));
    layout()->addWidget(m_plot);
}


PlotView3D::~PlotView3D()
{
    removeAllPlots();
    delete m_xScale;
    delete m_yScale;
    delete m_zScale;
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


/** Set the label that will be displayed on the x-axis of the plot.
  */
void
PlotView3D::setXLabel(const QString& label)
{
    m_xLabel = label;
    m_plot->coordinates()->axes[X1].setLabelString(label);
    m_plot->coordinates()->axes[X2].setLabelString(label);
    m_plot->coordinates()->axes[X3].setLabelString(label);
    m_plot->coordinates()->axes[X4].setLabelString(label);
    m_plot->updateGL();
}


/** Set the label that will be displayed on the y-axis of the plot.
  */
void
PlotView3D::setYLabel(const QString& label)
{
    m_yLabel = label;
    m_plot->coordinates()->axes[Y1].setLabelString(label);
    m_plot->coordinates()->axes[Y2].setLabelString(label);
    m_plot->coordinates()->axes[Y3].setLabelString(label);
    m_plot->coordinates()->axes[Y4].setLabelString(label);
    m_plot->updateGL();
}


/** Set the label that will be displayed on the z-axis of the plot.
  */
void
PlotView3D::setZLabel(const QString& label)
{
    m_zLabel = label;
    m_plot->coordinates()->axes[Z1].setLabelString(label);
    m_plot->coordinates()->axes[Z2].setLabelString(label);
    m_plot->coordinates()->axes[Z3].setLabelString(label);
    m_plot->coordinates()->axes[Z4].setLabelString(label);
    m_plot->updateGL();
}



/** Set the x-axis scale for this plot view.
  */
void
PlotView3D::setXScale(const PlotScale& scale)
{
    delete m_xScale;
    m_xScale = scale.clone();
    updatePlotData();
}


/** Set the y-axis scale for this plot view.
  */
void
PlotView3D::setYScale(const PlotScale& scale)
{
    delete m_yScale;
    m_yScale = scale.clone();
    updatePlotData();
}


/** Set the z-axis scale for this plot view.
  */
void
PlotView3D::setZScale(const PlotScale& scale)
{
    delete m_zScale;
    m_zScale = scale.clone();
    updatePlotData();
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
            minZ = min(minZ, v.z());
            maxZ = max(maxZ, v.z());
        }
    }

    if (!empty)
    {
        setXScale(LinearPlotScale(minX, maxX));
        setYScale(LinearPlotScale(minY, maxY));
        setZScale(LinearPlotScale(minZ, maxZ));
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
            p = Vector3d(m_xScale->scaled(p.x()), m_yScale->scaled(p.y()), m_zScale->scaled(p.z()));
            triples << Triple(p.x(), p.y(), p.z());
        }

        QColor color = plot->style.strokeStyle().color();
        float width = plot->style.strokeStyle().widthF();
        m_plot->setMeshColor(RGBA(color.redF(), color.greenF(), color.blueF(), 1.0f));
        m_plot->setMeshLineWidth(width);
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

    // Set the scale on all the axes
    double minX = m_xScale->unscaled(0.0);
    double maxX = m_xScale->unscaled(1.0);
    double minY = m_yScale->unscaled(0.0);
    double maxY = m_yScale->unscaled(1.0);
    double minZ = m_zScale->unscaled(0.0);
    double maxZ = m_zScale->unscaled(1.0);

    m_plot->coordinates()->axes[X1].setLimits(minX, maxX);
    m_plot->coordinates()->axes[X2].setLimits(minX, maxX);
    m_plot->coordinates()->axes[X3].setLimits(minX, maxX);
    m_plot->coordinates()->axes[X4].setLimits(minX, maxX);

    m_plot->coordinates()->axes[Y1].setLimits(minY, maxY);
    m_plot->coordinates()->axes[Y2].setLimits(minY, maxY);
    m_plot->coordinates()->axes[Y3].setLimits(minY, maxY);
    m_plot->coordinates()->axes[Y4].setLimits(minY, maxY);

    m_plot->coordinates()->axes[Z1].setLimits(minZ, maxZ);
    m_plot->coordinates()->axes[Z2].setLimits(minZ, maxZ);
    m_plot->coordinates()->axes[Z3].setLimits(minZ, maxZ);
    m_plot->coordinates()->axes[Z4].setLimits(minZ, maxZ);

    setXLabel(m_xLabel);
    setYLabel(m_yLabel);
    setZLabel(m_zLabel);

    m_plot->updateGL();
}
