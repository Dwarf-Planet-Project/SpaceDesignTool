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

#include "PlotScale.h"
#include <cmath>

using namespace std;


QList<double>
LinearPlotScale::ticks() const
{
    QList<double> t;
    const double maxTicks = 10;
    double range = m_maxValue - m_minValue;
    double idealSpacing = range / maxTicks;
    double exponent = floor(log10(idealSpacing));
    double p = pow(10.0, exponent);
    double mantissa = idealSpacing / p;

    // Choose an appropriate tick spacing with round numbers
    double tickSpacing = 1.0;
    if (mantissa < 2.0)
    {
        tickSpacing = 2.0 * p;
    }
    else if (mantissa < 3.0)
    {
        tickSpacing = 3.0 * p;
    }
    else if (mantissa < 5.0)
    {
        tickSpacing = 5.0 * p;
    }
    else
    {
        tickSpacing = 10.0 * p;
    }

    double firstTick = ceil(m_minValue / tickSpacing) * tickSpacing;
    double lastTick = floor(m_maxValue / tickSpacing) * tickSpacing;

    for (double tick = firstTick; tick <= lastTick; tick += tickSpacing)
    {
        t << tick;
    }

    return t;
}

