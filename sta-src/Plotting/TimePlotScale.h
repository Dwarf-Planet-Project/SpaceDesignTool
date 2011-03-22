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

#ifndef _PLOTTING_TIME_PLOT_SCALE_H_
#define _PLOTTING_TIME_PLOT_SCALE_H_

#include "PlotScale.h"


/** TimePlotScale is a plot scale that generates plot ticks that fall on
  * integer values of days, hours, minutes, or seconds depending on the
  * time span covered.
  */
class TimePlotScale : public PlotScale
{
public:
    TimePlotScale(double minValue, double maxValue);

    virtual ~TimePlotScale() {};

    virtual double scaled(double x) const;
    virtual double unscaled(double x) const;
    virtual QList<double> ticks() const;

    double minValue() const
    {
        return m_minValue;
    }

    double maxValue() const
    {
        return m_maxValue;
    }

protected:
    virtual PlotScale* cloneContents() const;

private:
    double m_minValue;
    double m_maxValue;
};

#endif // _PLOTTING_TIME_PLOT_SCALE_H_
