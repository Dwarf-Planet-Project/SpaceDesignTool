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
 ------------------ Author: Ana Raposo, June 2010  ------------------------------
*/
#include "AnalysisPlot.h"
#include "Analysis/analysis.h"

#include <QFile>
#include <QTextStream>

#include <QDebug>


AnalysisPlot::AnalysisPlot()
{
}


unsigned int AnalysisPlot::getPointCount() const
{
    return m_points2d.size();
}

Eigen::Vector2d AnalysisPlot::getPoint(unsigned int index) const
{
    return m_points2d[index];
}

void AnalysisPlot::setPoints(QList<analysis::AnalysisData>DataStructure, int index, int parameters)
{
    // DataStructure[0] x-axis data
    // DataStructure[1] y-axis data
    //for the time being, only one parameter per axis
if(DataStructure.size()==2)
    {
       m_points2d.clear();

        Vector2d p;
        for (int j=0;j<parameters;j++)
        {
        for(int i=0;i<index;i++)
        {
        p.x()=DataStructure[0].Data.at(j).at(i);
        p.y()=DataStructure[1].Data.at(j).at(i);

            m_points2d << p;
        }
    }
    }
}
