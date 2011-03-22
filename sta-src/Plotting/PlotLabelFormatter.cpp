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
 ------ Copyright (C) 2010-2011 STA Steering Board (space.trajectory.analysis AT gmail.com) ----
*/

/*
------------------ Author:       Chris Laurel                   -------------------
-----------------------------------------------------------------------------------
*/

#include "PlotLabelFormatter.h"
#include "Astro-Core/date.h"
#include <vesta/GregorianDate.h>
#include <QDateTime>
#include <QDebug>

using namespace vesta;


QString
NumericLabelFormatter::format(double value) const
{
    return QString("%1").arg(value);
}


PlotLabelFormatter*
NumericLabelFormatter::clone() const
{
    return new NumericLabelFormatter();
}


QString
DateLabelFormatter::format(double value) const
{
    double jd = sta::MjdToJd(value);
    GregorianDate utcDate = GregorianDate::UTCDateFromTDBJD(jd);

    qDebug() << m_dateFormat << ", " << jd;
    switch (m_dateFormat)
    {
    case GregorianUTC:
        {
            QDateTime d(QDate(utcDate.year(), utcDate.month(), utcDate.day()),
                        QTime(utcDate.hour(), utcDate.minute(), utcDate.second()), Qt::UTC);
            return d.toString("hh:mm:ss yyyy-MM-dd");
        }
        break;
    case YearAndDayNumber:
        return QString("%1%2").arg(utcDate.year() % 100, 2, 10, QChar('0')).arg((int) utcDate.dayOfYear(), 3, 10, QChar('0'));
        break;
    default:
        return "";
    }
}


PlotLabelFormatter*
DateLabelFormatter::clone() const
{
    return new DateLabelFormatter(m_dateFormat);
}
