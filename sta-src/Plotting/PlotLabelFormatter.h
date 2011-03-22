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

#ifndef _PLOTTING_PLOT_LABEL_FORMATTER_H_
#define _PLOTTING_PLOT_LABEL_FORMATTER_H_

#include <QString>


/** Abstract base class for a class that converts plot values to labels.
  */
class PlotLabelFormatter
{
public:
    virtual ~PlotLabelFormatter() {};

    virtual QString format(double value) const = 0;
    virtual PlotLabelFormatter* clone() const = 0;
};


/** The numeric label formatter does converts values into standard
  * string representations of numbers.
  */
class NumericLabelFormatter : public PlotLabelFormatter
{
public:
    NumericLabelFormatter() {}
    virtual ~NumericLabelFormatter() {};

    virtual QString format(double value) const;
    virtual PlotLabelFormatter* clone() const;
};


/** The label formatter interprets numeric values as Modified Julian Dates
  * and generates labels using one of several date formats.
  */
class DateLabelFormatter : public PlotLabelFormatter
{
public:
    enum DateFormat
    {
        GregorianUTC,
        YearAndDayNumber,
    };

    DateLabelFormatter(DateFormat format) : m_dateFormat(format) {}
    ~DateLabelFormatter() {};

    virtual QString format(double value) const;
    virtual PlotLabelFormatter* clone() const;

private:
    DateFormat m_dateFormat;
};

#endif // _PLOTTING_PLOT_LABEL_FORMATTER_H_
