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
 ------ Copyright (C) 2011 STA Steering Board (space.trajectory.analysis AT gmail.com) ----
*/

//------------------ Author:       Chris Laurel                   -------------------
//-----------------------------------------------------------------------------------

#include "TimePlotScale.h"
#include <cmath>

using namespace std;


/** Create a new time plot scale covering the specified time range.
  * TimePlotScale will use tick intervals that fall on integer minute,
  * hour, or day values. It should be used for plot axes with ticks that have
  * date labels, not axes that use Julian day numnbers or some other decimal
  * date representation.
  *
  * \param minValue minimum time (as a modified Julian date)
  * \param maxValue maximum time (as a modified Julian date)
  */
TimePlotScale::TimePlotScale(double minValue, double maxValue) :
    m_minValue(minValue),
    m_maxValue(maxValue)
{
}


double
TimePlotScale::scaled(double x) const
{
    return (x - m_minValue) / (m_maxValue - m_minValue);
}


double
TimePlotScale::unscaled(double x) const
{
    return x * (m_maxValue - m_minValue) + m_minValue;
}


PlotScale*
TimePlotScale::cloneContents() const
{
    return new TimePlotScale(m_minValue, m_maxValue);
}


static const double Seconds = 1.0 / 86400.0;
static const double Minutes = Seconds * 60;
static const double Hours = Minutes * 60;
static const double Days = Hours * 24.0;

static double AllowedTimeIntervals[] =
{
     1 * Seconds,
     2 * Seconds,
     5 * Seconds,
    10 * Seconds,
    15 * Seconds,
    30 * Seconds,
     1 * Minutes,
     2 * Minutes,
     5 * Minutes,
    10 * Minutes,
    15 * Minutes,
    30 * Minutes,
     1 * Hours,
     2 * Hours,
     3 * Hours,
     6 * Hours,
    12 * Hours,
     1 * Days,
};


// Find an appropriate spacing between ticks from the AllowedTimeIntervals table
// The duration is in units of Julian Days
static double ChooseTickSpacing(double duration, unsigned int maxTicks)
{
    double idealSpacing = duration / maxTicks;
    double spacing = 0.0;

    // Scan the table for the first interval greater than the ideal spacing
    unsigned int tableSize = sizeof(AllowedTimeIntervals) / sizeof(AllowedTimeIntervals[0]);
    for (unsigned int i = 0; i < tableSize; ++i)
    {
        if (AllowedTimeIntervals[i] > idealSpacing)
        {
            spacing = AllowedTimeIntervals[i];
            break;
        }
    }

    // If we didn't find anything in the table (because the duration is very long),
    // Choose a integer number of days that's close to the ideal spacing
    if (spacing <= 0.0)
    {
        spacing = ceil(idealSpacing);
    }

    return spacing;
}


QList<double> TimePlotScale::ticks() const
{
    QList<double> t;
    const unsigned int MaxTicks = 10;

    double tickSpacing = ChooseTickSpacing(m_maxValue - m_minValue, MaxTicks);

    double firstTick = ceil(m_minValue / tickSpacing) * tickSpacing;
    double lastTick = floor(m_maxValue / tickSpacing) * tickSpacing;

    for (double tick = firstTick; tick <= lastTick; tick += tickSpacing)
    {
        t << tick;
    }

    return t;
}


