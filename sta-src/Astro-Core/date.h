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
 ------------------ Author: Chris Laurel  -------------------------------------------------
 ------------------ E-mail: (claurel@gmail.com) ----------------------------
 */

#ifndef _ASTROCORE_DATE_H_
#define _ASTROCORE_DATE_H_

#include <QDateTime>

namespace sta
{
    int MjdToElapsed(double StartEpoch, int ArrayIndex); //added by Ana
    QDateTime JdToCalendar(double jd);
    double CalendarToJd(QDateTime calendarDate);
    double MjdToJd(double mjd);
    double JdToMjd(double jd);
    double MjdToFromEpoch(double StartEpoch, double mjd, QString Units); //added by Ana
    double DateTimeTOjulian(QDateTime DateTime); //Added by Ana
    bool CheckIfLeapYear(int year); //added by Ana
    QString calendarToDayOfYear(QDateTime DateTime); //added by Ana
    QList<QString> TimeLayout(int day, int month); //added by Ana
    QList<double> DayOfYearToDDD(double DayOfYear);

    template <typename T> T secsToDays(T secs)
    {
        return secs / 86400.0;
    }
    
    template <typename T> T daysToSecs(T days)
    {
        return days * 86400.0;
    }
    
    extern double J2000;
}

#endif // _ASTROCORE_STATEVECTOR_H_
