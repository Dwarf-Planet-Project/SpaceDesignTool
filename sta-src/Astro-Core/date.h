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
    double calendarToDayOfYear(QDateTime DateTime);

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
