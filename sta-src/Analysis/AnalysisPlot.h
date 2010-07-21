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
 ------------------ Author: Ana Raposo, June 2010------------------------------
*/

#ifndef ANALYSISPLOT_H
#define ANALYSISPLOT_H
#include <QObject>
#include "Eigen/Core"
#include "Plotting/PlotDataSource.h"
#include "Plotting/PlotView3D.h"
#include <QString>
#include "Analysis/analysis.h"

using namespace Eigen;

class AnalysisPlot: public PlotDataSource
{
    Q_OBJECT
public:

    EIGEN_MAKE_ALIGNED_OPERATOR_NEW

    AnalysisPlot();

    virtual ~AnalysisPlot() {};
    virtual unsigned int getPointCount() const;
    virtual Eigen::Vector2d getPoint(unsigned int index) const;

    void setPoints(QList<analysis::AnalysisData>,int lines,int columns);

protected:
    QList<Eigen::Vector2d> m_points2d;


};

class Analysis3D: public PlotDataSource3D
{
Q_OBJECT

public:

    Analysis3D();
    virtual ~Analysis3D() {};

    virtual unsigned int getPointCount() const;

    virtual Vector3d getPoint(unsigned int index) const;

    void setPoints(QList<analysis::AnalysisData>,int lines,int columns);

protected:
    QList<Eigen::Vector3d> m_points3d;

};

#endif // ANALYSISPLOT_H
