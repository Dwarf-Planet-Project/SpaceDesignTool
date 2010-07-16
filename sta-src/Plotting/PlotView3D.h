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
------------------ Author:       Chris Laurel                   -------------------
-----------------------------------------------------------------------------------
Patched by Guillermo on June 2010 to allow multiple row plotting
*/


#ifndef _PLOTTING_PLOTVIEW3D_H_
#define _PLOTTING_PLOTVIEW3D_H_

#include "PlotScale.h"
#include "PlotStyle.h"
#include <QWidget>
#include <QVector>
#include <Eigen/Core>


class PlotDataSource3D;
namespace Qwt3D
{
    class SurfacePlot;
}


/** PlotView is a widget for displaying one or more plots. The data
  * for each of the plots is provided by a PlotDataSource object.
  */
class PlotView3D : public QWidget
{
    Q_OBJECT
public:
    PlotView3D(QWidget* parent);
    ~PlotView3D();

    void addPlot(PlotDataSource3D* plotData, const PlotStyle& plotStyle = PlotStyle(PlotStyle::LinePlot));
    void removePlot(PlotDataSource3D* plotData);
    void removeAllPlots();

    QString title() const
    {
        return m_title;
    }

    QString leftLabel() const
    {
        return m_leftLabel;
    }

    QString rightLabel() const
    {
        return m_rightLabel;
    }

    QString topLabel() const
    {
        return m_topLabel;
    }

    QString bottomLabel() const
    {
        return m_bottomLabel;
    }

    const PlotScale* horizontalScale() const
    {
        return m_horizontalScale;
    }

    const PlotScale* verticalScale() const
    {
        return m_verticalScale;
    }

    void setHorizontalScale(const PlotScale& scale);
    void setVerticalScale(const PlotScale& scale);


public slots:
    void setTitle(const QString& title);
    void setLeftLabel(const QString& label);
    void setRightLabel(const QString& label);
    void setTopLabel(const QString& label);
    void setBottomLabel(const QString& label);
    void autoScale();

private:
    void updatePlotData();

private:
    struct Plot
    {
        Plot() : data(NULL), style(PlotStyle::LinePlot) {}
        Plot(const Plot& other) : data(other.data), style(other.style) {}
        Plot& operator=(const Plot& other) { data = other.data; style = other.style; return *this; }

        PlotDataSource3D* data;
        PlotStyle style;
    };

    Qwt3D::SurfacePlot* m_plot;

    QVector<Plot*> m_plots;

    QString m_title;
    QString m_leftLabel;
    QString m_rightLabel;
    QString m_topLabel;
    QString m_bottomLabel;

    PlotScale* m_horizontalScale;
    PlotScale* m_verticalScale;

    float m_topMargin;
    float m_bottomMargin;
    float m_leftMargin;
    float m_rightMargin;
};

#endif // _PLOTTING_PLOTVIEW3D_H_
