/*

 This program is free software; you can redistribute it and/or modify it under
 the terms of the GNU General Public License as published by the Free Software
 Foundation; either version 2 of the License, or (at your option) any later
 version.

 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE. See the GNU General Public License for more
 details.

 You should have received a copy of the GNU General Public License along with
 this program; if not, write to the Free Software Foundation, Inc., 59 Temple
 Place - Suite 330, Boston, MA 02111-1307, USA.
 Further information about the GNU General Public License can also be found on
 the world wide web at http://www.gnu.org.
 */


/*
 --- Copyright (C) 2009 STA Steering Board (space.trajectory.analysis AT gmail.com) ---
*/

/*
 ------------------ Author: Chris Laurel  -------------------------------------------------
 ------------------ E-mail: (claurel@gmail.com) ----------------------------
 // Patched by Guillermo June 12 2009 to modify how STA inteprets the dates (all now as UTC)
 */

#include "Astro-Core/date.h"
#include "Astro-Core/calendarTOjulian.h"
#include <cmath>


double sta::J2000 = 2451545.0;


/*! Convert a Qt4 date-time object to a Julian date.
 *
 *  TODO: leap seconds are not handled by Qt4 yet.
 *  TODO: milliseconds are not handled in the calendarTOjulian function yet.
 */
double sta::CalendarToJd(QDateTime calendarDate)
{
    Q_ASSERT(calendarDate.isValid());
    
    // Convert the timespec to UTC
    // QDateTime utcDate = calendarDate.toUTC();  // We assume for the time being that all times are UTC
    QDateTime utcDate = calendarDate;
    QDate d = utcDate.date();
    QTime t = utcDate.time();
    
    return calendarTOjulian(d.year(), d.month(), d.day(), t.hour(), t.minute(), t.second());
}


/*! Convert a Julian date to a Qt4 date-time object. The returned QDateTime
 *  object will use the UTC timespec.
 *
 * @param(jd): A Julian date greater than or equal to zero.
 *
 *  TODO: leap seconds are not handled by Qt4 yet!
 */
QDateTime sta::JdToCalendar(double jd)
{
    Q_ASSERT(jd >= 0.0);
    
    int a = (int) floor(jd + 0.5);
    double c;
    if (a < 2299161)
    {
        c = (double) (a + 1524);
    }
    else
    {
        double b = (double) ((int) floor((a - 1867216.25) / 36524.25));
        c = a + b - (int) floor(b / 4) + 1525;
    }
    
    int d = (int) floor((c - 122.1) / 365.25);
    int e = (int) floor(365.25 * d);
    int f = (int) floor((c - e) / 30.6001);
    
    double dday = c - e - (int) floor(30.6001 * f) + ((jd + 0.5) - a);
    
    // This following used to be 14.0, but gcc was computing it incorrectly, so
    // it was changed to 14
    int month = (int) (f - 1 - 12 * (int) (f / 14));
    int year = (int) (d - 4715 - (int) ((7.0 + month) / 10.0));
    int day = (int) dday;
    
    double dhour = (dday - day) * 24;
    int hour = (int) dhour;
    
    double dminute = (dhour - hour) * 60;
    int minute = (int) dminute;
    
    double dsecond = (dminute - minute) * 60;
    int second = (int) dsecond;
    
    int msec = (int) ((dsecond - second) * 1000);
    
    return QDateTime(QDate(year, month, day), QTime(hour, minute, second, msec), Qt::UTC);
}


/*! Convert a modified Julian date (day offset from J2000.0) to a 
 *  standard Julian date.
 */
double sta::MjdToJd(double mjd)
{
    return mjd + sta::J2000;
}


/*! Convert a standard Julian date to a modified Julian date (day offset
 *  from J2000.0.)
 */
double sta::JdToMjd(double jd)
{
    return jd - sta::J2000;
}


