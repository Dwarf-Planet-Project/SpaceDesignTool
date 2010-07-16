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
*/

#ifndef _PLOTTING_PLOTSTYLE_H_
#define _PLOTTING_PLOTSTYLE_H_

#include <QPen>


class PlotStyle
{
public:
    enum PlotType
    {
        LinePlot,
        ScatterPlot,
    };

    PlotStyle(PlotType type);
    PlotStyle(PlotType type, const QPen strokeStyle);
    PlotStyle(const PlotStyle& other);

    PlotStyle& operator=(const PlotStyle& other);

    PlotType type() const
    {
        return m_type;
    }

    QPen strokeStyle() const
    {
        return m_strokeStyle;
    }

private:
    PlotType m_type;
    QPen m_strokeStyle;
};

#endif // _PLOTTING_PLOTSTYLE_H_

