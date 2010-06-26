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


#include "PlotGraphFromFile.h"

#include <QFile>
#include <QTextStream>

#include <QDebug>


PlotGraphFromFile::PlotGraphFromFile()
{
}


unsigned int PlotGraphFromFile::getPointCount() const
{
    return m_points.size();
}

Eigen::Vector2d PlotGraphFromFile::getPoint(unsigned int index) const
{
    return m_points[index];
}

void PlotGraphFromFile::setPoints(QString FileNameWithExtension)
{
    // first column is the x-axis
    // second column is the y-axis

    QString path = "data/SystemsEngineeringReports/";
    path.append(FileNameWithExtension);

    QFile TimePlot(path);
    TimePlot.open(QIODevice::ReadOnly);
    qDebug()<<"TimePlot.fileName()"<<TimePlot.fileName();
    qDebug()<<"TimePlot.isOpen()"<<TimePlot.isOpen();
    QTextStream TimePlotStream(&TimePlot);

    m_points.clear();

    while (TimePlotStream.status() == QTextStream::Ok)
    {
        Vector2d p;
        TimePlotStream >> p.x();
        TimePlotStream >> p.y();
        qDebug() << p.x() << ", " << p.y();

        if (TimePlotStream.status() == QTextStream::Ok)
        {
            m_points << p;
        }
    }
}
