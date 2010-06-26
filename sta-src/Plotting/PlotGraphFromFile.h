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
 ------------------ Author: Ozgun, Guillermo, Chris      ----------------------------------
Patched by Guillermo on June 2010 to allow multiple row plotting

 */


#ifndef PLOTGRAPHFROMFILE_H
#define PLOTGRAPHFROMFILE_H

#include <QObject>
#include "Eigen/Core"
#include "Plotting/PlotDataSource.h"
#include <QString>

using namespace Eigen;

class PlotGraphFromFile: public PlotDataSource
{
    Q_OBJECT
public:
    /*when an Eigen object is the member of a class.
    But the workaround is easy: just put
    EIGEN_MAKE_ALIGNED_OPERATOR_NEW in the definition
    of any class that uses Eigen objects*/
    EIGEN_MAKE_ALIGNED_OPERATOR_NEW

    PlotGraphFromFile();

    virtual ~PlotGraphFromFile() {};
    virtual unsigned int getPointCount() const;
    virtual Eigen::Vector2d getPoint(unsigned int index) const;

    void setPoints(QString FileNameWithExtension);

protected:
    QList<Eigen::Vector2d> m_points;

};

#endif // PLOTGRAPHFROMFILE_H
