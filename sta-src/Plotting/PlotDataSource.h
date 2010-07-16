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



#ifndef _PLOTTING_PLOTDATASOURCE_H_
#define _PLOTTING_PLOTDATASOURCE_H_

#include <Eigen/Core>
#include <QObject>


/** PlotDataSource is an interface used by PlotView. All data to be shown in
  * a PlotView widget must be wrapped in a PlotDataSource object. At a minimum,
  * subclasses of PlotDataSource must return the number of data points and the
  * position of any point in the data set.
  */
class PlotDataSource : public QObject
{
    Q_OBJECT
public:
    virtual ~PlotDataSource() {};
    virtual unsigned int getPointCount() const = 0;
    virtual Eigen::Vector2d getPoint(unsigned int index) const = 0;
};


class PlotDataSource3D : public QObject
{
    virtual ~PlotDataSource3D() {};
    virtual unsigned int getPointCount3D() const = 0;
    virtual Eigen::Vector3d getPoint3D(unsigned int index) const = 0;
};

#endif // _PLOTTING_PLOTDATASOURCE_H_
