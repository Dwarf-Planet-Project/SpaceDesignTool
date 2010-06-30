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
 // Patched by Guillermo June 12 2009 to modify how STA inteprets the dates (all now as UTC)
 // Patched by Guillermo to correct the conversion to modified Julian Dates

 //Patched by Ana Raposo to include Mission Elapsed Time, Time from Epoch, transformations between a calendar date and JulianDate and Day of the Year;
   There is now a function to check for leap years.
 */

#include "Astro-Core/date.h"
#include "Astro-Core/calendarTOjulian.h"
#include <cmath>
#include <QDebug>



/*! While any event in recorded human history can be written as a positive
 * Julian day number, when working with contemporary events all those digits
 * can be cumbersome. A Modified Julian Day (MJD) is created by subtracting 2400000.5
 * from a Julian day number, and thus represents the number of days elapsed
 * since midnight (00:00) Universal Time on November 17, 1858.
 * Modified Julian Days are widely used to specify the epoch in tables of orbital
 * elements of artificial Earth satellites. Since no such objects existed prior to
 * October 4, 1957, all satellite-related MJDs are positive.
 */
//double sta::J2000 = 2451545.0;
double sta::J2000 = 2400000.5;



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

/*! functions added by Ana Raposo to fulfill some Analysis Module requirements*/

bool sta::CheckIfLeapYear(int year)
{
    if((year%4==0)&&(year%100!=0))
    {
        return (true);
    }
    if((year%4==0)&&(year%100==0)&&(year%400)!=0)
    {
        return(false);
    }
    if((year%4==0)&&(year%100==0)&&(year%400)==0)
    {
        return(true);
    }
    return(false);
}
double sta::DateTimeTOjulian(QDateTime DateTime)
{
    int Year=DateTime.date().year();
    int Month=DateTime.date().month();
    int Day=DateTime.date().day();
    int Hour=DateTime.time().hour();
    int Minute=DateTime.time().minute();
    int Second=DateTime.time().second();
    double JulianDate=calendarTOjulian(Year,Month,Day,Hour,Minute,Second);
    return JulianDate;
}
double sta::calendarToDayOfYear(QDateTime DateTime)
{
    int DaysInMonths=0;
    int Days=0;
    int DayOfYear=0;
    int MonthsLength[12];
    MonthsLength[0]=MonthsLength[2]=MonthsLength[4]=MonthsLength[6]=MonthsLength[7]=MonthsLength[9]=MonthsLength[11];
    MonthsLength[3]=MonthsLength[5]=MonthsLength[8]=MonthsLength[10]=30;
    MonthsLength[1]=28;
    if(sta::CheckIfLeapYear(DateTime.date().year()))
    {
        MonthsLength[1]=29;
    }

    if(DateTime.date().month()!=1)
    {
      for(int i=0;i<DateTime.date().month();i++)
      {
        DaysInMonths=DaysInMonths+MonthsLength[i];

      }
    }


    Days=DateTime.date().day();


    DayOfYear=DaysInMonths+Days;

    return DayOfYear;
}
/*double sta::MissionElapsedTime(QDateTime Date)
{
    int DaysInMonths=0;
    int Days=0;
    int DayOfYear=0;
    int MonthsLength[12];
    MonthsLength[0]=MonthsLength[2]=MonthsLength[4]=MonthsLength[6]=MonthsLength[7]=MonthsLength[9]=MonthsLength[11];
    MonthsLength[3]=MonthsLength[5]=MonthsLength[8]=MonthsLength[10]=30;
    MonthsLength[1]=28;

    JulianDate[index]=sta::MjdToJd(MJDdate[index]+0.00001);
    TimeDateVector[index]=sta::JdToCalendar(JulianDate[index]);
    int Year=TimeDateVector[index].date().year();


    if(sta::CheckIfLeapYear(DateTime.date().year()))
    {
        MonthsLength[1]=29;
    }

    if(DateTime.date().month()!=1)
    {
      for(int i=0;i<DateTime.date().month();i++)
      {
        DaysInMonths=DaysInMonths+MonthsLength[i];

      }
    }
}*/

double sta::MjdToFromEpoch(double StartEpoch, double mjd, QString Units)
{
    double ElapsedTime=mjd-StartEpoch;

    if(Units=="Seconds")
    {
      double SecondJD=calendarTOjulian(1858,11,17,0,0,1);//convert 1second into MJD
      double Second=sta::JdToMjd(SecondJD);
      double ElapsedTimeSeconds=ElapsedTime/Second;
   return(ElapsedTimeSeconds);
   }
    if(Units=="Minutes")
    {
       double MinuteJD=calendarTOjulian(1858,11,17,0,1,0); //convert 1minute into MJD
       double MinuteMJD=sta::JdToMjd(MinuteJD);
       double ElapsedTimeMinutes=ElapsedTime/MinuteMJD;
  return(ElapsedTimeMinutes);
}
    if(Units=="Hours")
    {
        double HourJD=calendarTOjulian(1858,11,17,1,0,0); //convert 1hour into MJD
        double HourMJD=sta::JdToMjd(HourJD);
        double ElapsedTimeHours=ElapsedTime/HourMJD;
  return(ElapsedTimeHours);
    }
    if(Units=="Days")
    {
        double DayJD=calendarTOjulian(1858,11,18,0,0,0); //convert 1day into MJD
        double DayMJD=sta::JdToMjd(DayJD);
        double ElapsedTimeDays=ElapsedTime/DayMJD;
  return(ElapsedTimeDays);
    }
}
