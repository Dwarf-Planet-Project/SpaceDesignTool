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
#include "constants.h"
#include <vector>
#include <map>
#include <algorithm>
#include <iomanip>
#include <sstream>
#include <cassert>

using namespace sta;

/*! While any event in recorded human history can be written as a positive
 * Julian day number, when working with contemporary events all those digits
 * can be cumbersome. A Modified Julian Day (MJD) is created by subtracting 2400000.5
 * from a Julian day number, and thus represents the number of days elapsed
 * since midnight (00:00) Universal Time on November 17, 1858.
 * Modified Julian Days are widely used to specify the epoch in tables of orbital
 * elements of artificial Earth satellites. Since no such objects existed prior to
 * October 4, 1957, all satellite-related MJDs are positive.
 */
double sta::MJDBase = 2400000.5;
double sta::J2000   = 2451545.0;



/*! Convert a Qt4 date-time object to a Julian date.
 *
 *  TODO: leap seconds are not handled by Qt4 yet.
 */
double sta::CalendarToJd(const QDateTime& calendarDate)
{
    Q_ASSERT(calendarDate.isValid());

    QDate d = calendarDate.date();
    QTime t = calendarDate.time();
    
    return calendarTOjulian(d.year(), d.month(), d.day(), t.hour(), t.minute(), t.second()) + secsToDays(t.msec() / 1000.0);
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

/*! Convert a modified Julian date (day offset from JD 2400000.0) to a
 *  standard Julian date.
 */
double sta::MjdToJd(double mjd)
{
    return mjd + sta::MJDBase;
}

/*! Convert a standard Julian date to a modified Julian date (day offset
 *  from JD 2400000.0.)
 */
double sta::JdToMjd(double jd)
{
    return jd - MJDBase;
}


/** Convert a modified Julian date (day offset from JD240000.0) to a
  * time offset in seconds since J2000.0. This function is provided because
  * toolkits such as SPICE and VESTA use seconds from J2000 as their basic
  * time parameter.
  */
double sta::MjdToSecJ2000(double mjd)
{
    return sta::daysToSecs(mjd + (MJDBase - sta::J2000));
}


/** Convert a time offset in seconds since J2000.0 to a modified Julian
  * date (day offset from JD240000.0). This function is provided because
  * toolkits such as SPICE and VESTA use seconds from J2000 as their
  * basic time parameter.
  */
double sta::SecJ2000ToMjd(double sec)
{
    return sta::secsToDays(sec) - (MJDBase - sta::J2000);
}


/** Convert a Julian date to a time offset in seconds since J2000.0.
  * This function is provided because some toolkits (such as SPICE and VESTA)
  * use seconds from J2000 as their basic time parameter.
  */
double sta::JdToSecJ2000(double jd)
{
    return sta::daysToSecs(jd - sta::J2000);
}


/** Convert a time offset in seconds since J2000.0 to a Julian date.
  * This function is provided because toolkits such as SPICE and VESTA
  * use seconds from J2000 as their basic time parameter.
  */
double sta::SecJ2000ToJd(double sec)
{
    return sta::J2000 + sta::secsToDays(sec);
}


/*! functions added by Ana Raposo to fulfill some Analysis Module requirements*/

bool sta::CheckIfLeapYear(int year)
{
    /*
      checks if the year in the input is a leap year, and returns true, if yes, and false if not
      */
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
    /*
      Transforms a date and time in the QdateTime format into Julian Date format; returns the date in JD
      */
    int Year=DateTime.date().year();
    int Month=DateTime.date().month();
    int Day=DateTime.date().day();
    int Hour=DateTime.time().hour();
    int Minute=DateTime.time().minute();
    int Second=DateTime.time().second();
    double JulianDate=calendarTOjulian(Year,Month,Day,Hour,Minute,Second);
    return JulianDate;
}
QString sta::calendarToDayOfYear(QDateTime DateTime)
{
    /*
      transforms a date and time in QDateTime format into a string containg the day of the year
      */
    int DaysInMonths=0;
    int Days=0;
    int DayOfYear=0;
    int MonthsLength[12];
    MonthsLength[0]=MonthsLength[2]=MonthsLength[4]=MonthsLength[6]=MonthsLength[7]=MonthsLength[9]=MonthsLength[11]=31;
    MonthsLength[3]=MonthsLength[5]=MonthsLength[8]=MonthsLength[10]=30;
    MonthsLength[1]=28;
    if(sta::CheckIfLeapYear(DateTime.date().year()))
    {
        MonthsLength[1]=29;
    }

    //if(DateTime.date().month()!=1)
    {
      for(int i=0;i<DateTime.date().month()-1;i++)
      {
        DaysInMonths=DaysInMonths+MonthsLength[i];

      }
    }

    Days=DateTime.date().day();

    DayOfYear=DaysInMonths+Days;

    QString DayInYear;
    DayInYear.sprintf("%1d",DayOfYear);

    QString DDD;
    for(int i=0;i<(3-DayInYear.size());i++)
    {
        DDD.append("0");
    }
    DDD.append(DayInYear);

    return DDD;
}
QString sta::MissionElapsedTime(double Date, double StartEpoch)
{
    /*
      Description: calculates the elapsed time between two dates, in terms of days, hours, minutes, and seconds.
      Inputs: Date- each time step, in MJD, StartEpoch-point from which the elapsed time is calculated
      Outputs: string with the elapsed time in the format day/hh:mm::ss
      */
    QString ETime;
    double ElapsedTime=Date-StartEpoch+0.00001;

    double DayJD=calendarTOjulian(1858,11,18,0,0,0); //convert 1day into MJD
    double DayMJD=sta::JdToMjd(DayJD);
    double ElapsedTimeDays=ElapsedTime/DayMJD;

    double HourJD=calendarTOjulian(1858,11,17,1,0,0); //convert 1hour into MJD
    double HourMJD=sta::JdToMjd(HourJD);
    double ElapsedTimeHours=ElapsedTime/HourMJD;

    double MinuteJD=calendarTOjulian(1858,11,17,0,1,0); //convert 1minute into MJD
    double MinuteMJD=sta::JdToMjd(MinuteJD);
    double ElapsedTimeMinutes=ElapsedTime/MinuteMJD;

    double SecondJD=calendarTOjulian(1858,11,17,0,0,1);//convert 1second into MJD
    double SecondMJD=sta::JdToMjd(SecondJD);
    double ElapsedTimeSeconds=ElapsedTime/SecondMJD;

    double Dayfractpart, Dayintpart;
    Dayfractpart = modf (ElapsedTimeDays , &Dayintpart);
    int day=(int)Dayintpart; //number of days

    double NumberOfHours=Dayfractpart*ElapsedTimeHours/ElapsedTimeDays;
    double Hourfractpart, Hourintpart;
    Hourfractpart = modf (NumberOfHours , &Hourintpart);
    int hour=(int)Hourintpart; //number of hours

    double NumberOfMinutes=Hourfractpart*ElapsedTimeMinutes/ElapsedTimeHours;
    double Minutefractpart, Minuteintpart;
    Minutefractpart = modf (NumberOfMinutes , &Minuteintpart);
    int minute=(int)Minuteintpart; //number of minutes

    double NumberOfSeconds=Minutefractpart*ElapsedTimeSeconds/ElapsedTimeMinutes;
    double Secondfractpart, Secondintpart;
    Secondfractpart = modf (NumberOfSeconds , &Secondintpart);
    int second=(int)Secondintpart; //number of minutes

    QString STime;
    QString Hour;
    Hour.sprintf("%1d",hour);
    (STime.append(Hour)).append(".");
    QString Minute;
    Minute.sprintf("%1d",minute);
    (STime.append(Minute)).append(".");
    QString Second;
    Second.sprintf("%1d",second);
    STime.append(Second);
    QTime Time = QTime::fromString(STime, "h.m.s");

    QString ToTime=Time.toString(Qt::TextDate);
    QString DayToTime;
    DayToTime.sprintf("%1d",day);
    ETime.append(DayToTime);
    ETime.append("/");
    ETime.append(ToTime);

    return ETime;

}



double sta::MjdToFromEpoch(double StartEpoch, double mjd, QString Units)
{
    /*
Description: transforms a certain time into the elapsed time from the beginning of the start epoch; the elapsed time can be calculated in terms of hours, minutes, seconds, or days, according to the Units input.
Inputs: StartEpoch- beginning of propagation, mjd- considered time, Units- units in which the elapsed time shall be calculated
      */
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

QList<QString> sta::TimeLayout(int day, int month)
{
    /*
      Description: writes day and month with 2 digits, adding a zero whenever the values only have one digit;
example: day 1-> day 01
      Inputs:day and month
      Outputs: QList Output, day and month with 2 digits, Output[0] is day and Output[1] is month
      */
    QString Day;
    Day.sprintf("%1d",day);

    QString ToDay;
    for(int i=0;i<(2-Day.size());i++)
    {
        ToDay.append("0");
    }
    ToDay.append(Day);

    QString Month;
    Month.sprintf("%1d",month);

    QString ToMonth;
    for(int i=0;i<(2-Month.size());i++)
    {
        ToMonth.append("0");
    }
    ToMonth.append(Month);

    QList<QString>Date;

    Date.append(ToDay);
    Date.append(ToMonth);

    return Date;
}
QList<double> sta::DayOfYearToDDD(double DayOfYear)
{
    /*
      Description: returns the day of the year with 3 digits in the integer part, adding zeros when the input has only 1 or 2 digits in the integer part.
      Inputs: day of the year to transform
      Outputs: day of year with 3 digits in the integer part
      */
    double fractpartDay;
    double intpartDay;
    fractpartDay = modf (DayOfYear , &intpartDay);
    int IntDay=(int) intpartDay;

    QList<double> DDD;
    QString Day;
    Day.sprintf("%1d",IntDay);

    for(int i=0;i<(3-Day.size());i++)
    {
        DDD.append(0);
    }
    DDD.append(DayOfYear);
    return DDD;
}


//----------------------------------------------------
/* The following code is extracted from GregorianDate.cpp
   of Vesta*/

using namespace std;

static const unsigned int DaysPerMonth[] =
{
    0,
    31, 28, 31, 30, 31, 30,
    31, 31, 30, 31, 30, 31,
};


static const unsigned int DaysBeforeMonth[] =
{
    0,
      0,  31,  59,  90, 120, 151,
    181, 212, 243, 273, 304, 334,
};

// Difference in seconds between International Atomic Time (TAI) and
// Terrestrial Time (TT): TT = TAI + DeltaTAI
static const double DeltaTAI = 32.184;
static const int DeltaGPS = 19;
// Constant values taken from SPICE leap second kernel naif0008.tls
static const double TDB_K  = 1.657e-3;
static const double TDB_EB = 1.671e-2;
static const double TDB_M0 = 6.239996;
static const double TDB_M1 = 1.99096871e-7;

// Convert from Terrestrial Time to Barycentric Dynamical Time. The argument and return
// value are both the number of seconds since J2000.0.
 double convertTTtoTDB(double ttSec)
{
    return ttSec + TDB_K * sin(TDB_M0 + TDB_M1 * ttSec + TDB_EB * sin(TDB_M0 + TDB_M1 * ttSec));
}

 //Convert from Barycentric Dynamical Time to Terrestrial Time. The argument and return
 //value are both the number of seconds since J2000.0.
 double convertTDBtoTT(double tdbSec)
{
    // We need to invert the expression in convertTTtoTDB. We'll approximate
    // a solution by iterating three times (which is what SPICE does). Note that
    // the maximum difference between the TT and TDB time scales is under two
    // milliseconds for any date within 1000 years of J2000.

    double ttSec = tdbSec;
    for (unsigned int i = 0; i < 3; ++i)
    {
        ttSec = tdbSec - TDB_K * sin(TDB_M0 + TDB_M1 * ttSec + TDB_EB * sin(TDB_M0 + TDB_M1 * ttSec));
    }

    return ttSec;
}

 double convertTAItoTT(double taiSec)
{
    return taiSec + DeltaTAI;
}

 double convertTTtoTAI(double ttSec)
{
    return ttSec - DeltaTAI;
}

 double convertTAItoTDB(double taiSec)
 {
     return convertTTtoTDB(convertTAItoTT(taiSec));
 }

 double convertTDBtoTAI(double tdbSec)
 {

     return convertTTtoTAI(convertTDBtoTT(tdbSec));
 }

double convertTDBtoGPS(double tdbSec)
{
    return convertTDBtoTAI(tdbSec)-DeltaGPS;
}

// Convert a uniform time from a Julian Date to a count of seconds since
// J2000.0 (12:00:00 1-Jan-2000)
static double convertJDToSec(double jd)
{
    return (jd - 2451545.0) * 86400.0;
}

static double convertSecToJD(double sec)
{
    return sec / 86400.0 + 2451545.0;
}


/** Convert two times in seconds from one uniform time scale
  * to another.
  */

static double convertUniformSec(double fromTime, TimeConversions::TimeScale fromScale, TimeConversions::TimeScale toScale)
{
    if (fromScale == toScale)
    {
        return fromTime;
    }

    // Convert to TAI
    double tai = 0.0;
    switch (fromScale)
    {
    case TimeConversions::TimeScale_TAI:
        tai = fromTime;
        break;

    case TimeConversions::TimeScale_TDB:
        tai = convertTDBtoTAI(fromTime);
        break;

    case TimeConversions::TimeScale_TT:
        tai = convertTTtoTAI(fromTime);
        break;

    default:
        assert(0);
    }

    switch (toScale)
    {
    case TimeConversions::TimeScale_TAI:
        return tai;

    case TimeConversions::TimeScale_TDB:
        return convertTAItoTDB(tai);

    case TimeConversions::TimeScale_TT:
        return convertTAItoTT(tai);

    default:
        assert(0);
        return 0.0;
    }
}


/** Convert a Julian day number from one uniform time scale
  * to another.
  */
static double convertUniformJD(double fromTime, TimeConversions::TimeScale fromScale, TimeConversions::TimeScale toScale)
{
    return convertSecToJD(convertUniformSec(convertJDToSec(fromTime), fromScale, toScale));
}


// Get the Julian day number at noon on the specified Gregorian calendar date.
// If a date before 15 Oct 1582 is given, the Julian calendar is assumed. Conversion
// algorithm in from Meeus, _Astronomical Algorithms_.
static int julianDayNumber(int year, unsigned int month, unsigned int day)
{
    if (month <= 2)
    {
        year -= 1;
        month += 12;
    }

    int b;
    if (year > 1582 || (year == 1582 && (month > 10 || (month == 10 && day >= 15))))
    {
        b = 2 - (year / 100) + (year / 100) / 4;
    }
    else
    {
        // The specified day is in the before the Gregorian calendar
        // transition in October 1582.
        b = 0;
    }

    return int(365.25 * (year + 4716)) + int(30.6001 * (month + 1)) + day + b - 1524;
}


// Convert a uniform calendar date to a Julian day number in the same uniform time scale
static double UniformCalendarToJD(int year, unsigned int month, unsigned int day,
                                  unsigned int hour, unsigned int minute, double second)
{
    int dayNumber = julianDayNumber(year, month, day);

    // -0.5 is required because julianDayNumber returns the Julian day number at noon.
    return double(dayNumber) + (hour + (minute + second / 60.0) / 60.0) / 24.0 - 0.5;
}


static void JDToCalendar(double jd,
                         int& year, unsigned int& month, unsigned int& day,
                         unsigned int& hour, unsigned int& minute, double& second)
{
    int a = int(floor(jd + 0.5));

    double c;
    if (a < 2299161)
    {
        c = double(a + 1524);
    }
    else
    {
        double b = double(int(floor((a - 1867216.25) / 36524.25)));
        c = a + b - int(floor(b / 4)) + 1525;
    }

    int d = int(floor((c - 122.1) / 365.25));
    int e = int(floor(365.25 * d));
    int f = int(floor((c - e) / 30.6001));

    double fracDay = c - e - floor(30.6001 * f) + jd + 0.5 - a;

    month = f - 1 - 12 * (f / 14);
    year = d - 4715 - (7 + month) / 10;
    day = int(fracDay);
    double fracHour = (fracDay - day) * 24.0;
    hour = int(fracHour);

    double fracMinute = (fracHour - hour) * 60.0;
    minute = int(fracMinute);

    second = (fracMinute - minute) * 60.0;
}


// Return true if the specified year is a leap year
static bool checkLeapYear(int y)
{
    return y % 4 == 0 && (y % 100 != 0 || y % 400 == 0);
}


// Get the number of days in the specified month of the specified
// year.
static unsigned int daysInMonth(int y, int m)
{
    if (m == 2 && checkLeapYear(y))
    {
        return DaysPerMonth[m] + 1;
    }
    else
    {
        return DaysPerMonth[m];
    }
}

namespace TimeConversions
{
struct LeapSecond
{
    int taiOffset;
    int year;
    unsigned int month;
    unsigned int day;
};

static const LeapSecond DefaultLeapSecondList[] =
{
    { 10, 1972,  1,  1 },
    { 11, 1972,  7,  1 },
    { 12, 1973,  1,  1 },
    { 13, 1974,  1,  1 },
    { 14, 1975,  1,  1 },
    { 15, 1976,  1,  1 },
    { 16, 1977,  1,  1 },
    { 17, 1978,  1,  1 },
    { 18, 1979,  1,  1 },
    { 19, 1980,  1,  1 },
    { 20, 1981,  7,  1 },
    { 21, 1982,  7,  1 },
    { 22, 1983,  7,  1 },
    { 23, 1985,  7,  1 },
    { 24, 1988,  1,  1 },
    { 25, 1990,  1,  1 },
    { 26, 1991,  1,  1 },
    { 27, 1992,  7,  1 },
    { 28, 1993,  7,  1 },
    { 29, 1994,  7,  1 },
    { 30, 1996,  1,  1 },
    { 31, 1997,  7,  1 },
    { 32, 1999,  1,  1 },
    { 33, 2006,  1,  1 },
    { 34, 2009,  1,  1 },
};


// Get a unique value for the day that can be used to lookup in the leap
// second table.
static unsigned int dateHash(int year, unsigned int month, unsigned int day)
{
    return day + 100 * (month + 100 * year);
}


struct UTCDifferenceRecord
{
    double tai;
    double diffSec;
};


bool operator<(const UTCDifferenceRecord& t0, const UTCDifferenceRecord& t1)
{
    return t0.tai < t1.tai;
}


bool operator<(const LeapSecond& ls0, const LeapSecond& ls1)
{
    if (ls0.year < ls1.year)
    {
        return true;
    }
    else if (ls0.year > ls1.year)
    {
        return false;
    }
    else
    {
        if (ls0.month < ls1.month)
        {
            return true;
        }
        else if (ls0.month > ls1.month)
        {
            return false;
        }
        else
        {
            return ls0.day < ls1.day;
        }
    }
}


/** LeapSecondTable is an internal class used to calculate the difference
  * between UTC and TAI at some instant in time. This class will eventually
  * be exposed so that VESTA users can install custom leap second tables.
  */

class LeapSecondTable
{
public:
    LeapSecondTable(const LeapSecond leapSeconds[], unsigned int leapSecondCount)
    {
        for (unsigned int i = 0; i < leapSecondCount; ++i)
        {
            const LeapSecond& ls = leapSeconds[i];
            m_leapSeconds.push_back(ls);
            m_calendarOffsets[dateHash(ls.year, ls.month, ls.day)] = ls.taiOffset;

            UTCDifferenceRecord utcDiff;
            utcDiff.diffSec = ls.taiOffset;
            utcDiff.tai = UniformCalendarToJD(ls.year, ls.month, ls.day, 0, 0, 0);
            m_utcDiffs.push_back(utcDiff);
       }
   }

   bool dateHasLeapSecond(const GregorianDate& d)
   {
       // The leap year offset table stores days *after* the ones
       // containing leap seconds. Advance one day to check the table
       // for a leap second...
       unsigned int year = d.year();
       unsigned int month = d.month();
       unsigned int day = d.day();

       if (d.day() == daysInMonth(year, month))
       {
           if (month == 12)
           {
               month = 1;
               year++;
           }
           else
           {
               month++;
           }

           day = 1;
       }
       else
       {
           day++;
       }

       return m_calendarOffsets.find(dateHash(year, month, day)) != m_calendarOffsets.end();
   }

   // Get the difference betweeen UTC and TAI at the specified instant
   // in the TAI time scale.
   double utcDifference(double taijd)
   {
       if (m_utcDiffs.empty())
       {
           return 0.0;
       }

       UTCDifferenceRecord comp;
       comp.tai = taijd;
       comp.diffSec = 0.0;
       vector<UTCDifferenceRecord>::const_iterator iter = lower_bound(m_utcDiffs.begin(), m_utcDiffs.end(), comp);

       if (iter == m_utcDiffs.end())
       {
           return m_utcDiffs.back().diffSec;
       }
       else if (taijd < iter->tai)
       {
           if (iter == m_utcDiffs.begin())
           {
               return 0.0;
           }
           else
           {
               --iter;
               return iter->diffSec;
           }
       }
       else
       {
           return iter->diffSec;
       }
   }

   // Get the difference betweeen UTC and TAI at the specified UTC
   // calendar day.
   double utcDifference(int year, unsigned int month, unsigned int day)
   {
       if (m_utcDiffs.empty())
       {
           return 0.0;
       }

       LeapSecond ls;
       ls.year = year;
       ls.month = month;
       ls.day = day;
       vector<LeapSecond>::const_iterator iter = lower_bound(m_leapSeconds.begin(), m_leapSeconds.end(), ls);

       if (iter == m_leapSeconds.end())
       {
           return m_leapSeconds.back().taiOffset;
       }
       else if (ls < *iter)
       {
           if (iter == m_leapSeconds.begin())
           {
               return 0.0;
           }
           else
           {
               --iter;
               return iter->taiOffset;
           }
       }
       else
       {
           return iter->taiOffset;
       }
   }

private:
    vector<LeapSecond> m_leapSeconds;
    map<unsigned int, unsigned int> m_calendarOffsets;
    vector<UTCDifferenceRecord> m_utcDiffs;
};
}

TimeConversions::LeapSecondTable* TimeConversions::GregorianDate::s_DefaultLeapSecondTable =
        new TimeConversions::LeapSecondTable(DefaultLeapSecondList,
                                   sizeof(DefaultLeapSecondList) / sizeof(DefaultLeapSecondList[0]));


/** Default constructor creates a date representing the instant at midnight, 1 January 2000 UTC.
  */
TimeConversions::GregorianDate::GregorianDate() :
    m_year(2000),
    m_month(1),
    m_day(1),
    m_hour(0),
    m_minute(0),
    m_second(0),
    m_usec(0),
    m_timeScale(TimeConversions::TimeScale_UTC)
{
}


/** Copy constructor.
  */
TimeConversions::GregorianDate::GregorianDate(const GregorianDate& other) :
    m_year(other.m_year),
    m_month(other.m_month),
    m_day(other.m_day),
    m_hour(other.m_hour),
    m_minute(other.m_minute),
    m_second(other.m_second),
    m_usec(other.m_usec),
    m_timeScale(other.m_timeScale)
{
}


/** Construct a new calendar date.
  *
  * The time of day and time scale are optional. If they are omitted, the time is set to midnight
  * and the time scale to UTC.
  *
  * \param year astronomical year number (for year < 1, year 0 = 1 BCE, year -1 = 2 BCE, etc.)
  * \param month month number (1 - 12)
  * \param day day number (1 - 31)
  * \param hour hour number (0 - 23)
  * \param minute minute number (0 - 59)
  * \param second second number (0 - 59, 60 allowed for leap seconds in UTC time scale)
  * \param usec microseconds (0 - 999999)
  */
TimeConversions::GregorianDate::GregorianDate(int year, unsigned int month, unsigned int day,
                             unsigned int hour, unsigned int minute, unsigned int second, unsigned int usec,
                             TimeConversions::TimeScale timeScale) :
    m_year(year),
    m_month(month),
    m_day(day),
    m_hour(hour),
    m_minute(minute),
    m_second(second),
    m_usec(usec),
    m_timeScale(timeScale)
{
}


/** Assignment operator.
  */
TimeConversions::GregorianDate& TimeConversions::GregorianDate::operator=(const TimeConversions::GregorianDate& other)
{
    m_year = other.m_year;
    m_month = other.m_month;
    m_day = other.m_day;
    m_hour = other.m_hour;
    m_minute = other.m_minute;
    m_second = other.m_second;
    m_usec = other.m_usec;
    m_timeScale = other.m_timeScale;

    return *this;
}


/** Return true if this calendar date names a real instant in time.
  */
bool
TimeConversions::GregorianDate::isValid() const
{
    unsigned int monthLength = m_month <= 12 ? DaysPerMonth[m_month] : 0;
    if (isLeapYear() && m_month == 2)
    {
        monthLength++;
    }

    unsigned int minuteLength = 60;
    if (s_DefaultLeapSecondTable->dateHasLeapSecond(*this) && m_timeScale == TimeConversions::TimeScale_UTC)
    {
        // TODO: Handle negative leap seconds so that the code doesn't break in the
        // event that one is ever added.
        minuteLength = 61;
    }

    if (m_year == 1582 && m_month == 10 && m_day > 4 && m_day < 15)
    {
        // Skipped days during the Julian to Gregorian calendar
        // transition.
        return false;
    }
    else
    {
        return (m_month > 0 && m_month <= 12) &&
               (m_day > 0 && m_day <= monthLength) &&
               m_hour < 24 &&
               m_minute < 60 &&
               m_second < minuteLength &&
               m_usec < 1000000;
    }
}


/** Return true if the date falls within a leap year.
  */
bool
TimeConversions::GregorianDate::isLeapYear() const
{
    return checkLeapYear(m_year);
}


/** Return the day number within the year. This will be a value
  * from 1 -- 365 (or 366 during a leap year.)
  */
unsigned int
TimeConversions::GregorianDate::dayOfYear() const
{
    unsigned int daysBefore = DaysBeforeMonth[m_month];
    if (m_month > 2 && isLeapYear())
    {
        ++daysBefore;
    }

    return daysBefore + m_day;
}


/** Returns the day of the week as an integer between 1 and 7.
  */
unsigned int
TimeConversions::GregorianDate::dayOfWeek() const
{
    return (julianDay() + 1) % 7 + 1;
}


/** Return the number of days in the month: 28, 29, 30, or 31 depending
  * on the month and whether the year is a leap year.
  */
unsigned int
TimeConversions::GregorianDate::daysInMonth() const
{
    return ::daysInMonth(m_year, m_month);
}


/** Get the Julian day number (days since 1 Nov 4713 BCE) of this
  * date.
  */
int
TimeConversions::GregorianDate::julianDay() const
{
    return julianDayNumber(m_year, m_month, m_day);
}


/** Convert the date to a Julian day number in the TDB time scale.
  */
double
TimeConversions::GregorianDate::toTDBJD() const
{
    return convertSecToJD(toTDBSec());
}


/** Convert the date to a Julian day number in the TAI time scale.
  */
double
TimeConversions::GregorianDate::toTAIJD() const
{
    double second = m_second + m_usec * 1.0e-6;

    double uniformTime = UniformCalendarToJD(m_year, m_month, m_day, m_hour, m_minute, second);
    TimeConversions::TimeScale timeScale = m_timeScale;
    if (m_timeScale == TimeConversions::TimeScale_UTC)
    {
        double utcOffset = s_DefaultLeapSecondTable->utcDifference(m_year, m_month, m_day);
        uniformTime += secsToDays(utcOffset);
        timeScale = TimeConversions::TimeScale_TAI;
    }

    return convertUniformJD(uniformTime, timeScale, TimeConversions::TimeScale_TAI);
}


/** Convert the date to a Julian day number in the TT time scale.
  */
double
TimeConversions::GregorianDate::toTTJD() const
{
    return toTAIJD() + secsToDays(DeltaTAI);
}


/** Convert the date to a number of seconds since J2000.0 in the TDB
  * (Barycentric Dynamical Time) time scale.
  */
double
TimeConversions::GregorianDate::toTDBSec() const
{
    return convertTAItoTDB(convertJDToSec(toTAIJD()));
}


/** Convert the date to a number of seconds since J2000.0 in the TT
  * (Terrestrial Time) time scale.
  */
double
TimeConversions::GregorianDate::toTTSec() const
{
    return convertTAItoTT(convertJDToSec(toTAIJD()));
}


/** Construct a UTC calendar date from a Julian day number in the TDB time scale.
  */
TimeConversions::GregorianDate
TimeConversions::GregorianDate::UTCDateFromTDBJD(double tdbjd)
{
    int year = 0;
    unsigned int month = 1;
    unsigned int day = 1;
    unsigned int hour = 0;
    unsigned int minute = 0;
    double second = 0;

    double tai = tdbjd - DeltaTAI / 86400.0;
    tai -= s_DefaultLeapSecondTable->utcDifference(tai) / 86400.0;

    JDToCalendar(tai, year, month, day, hour, minute, second);
    unsigned int s = unsigned(second);
    unsigned int usec = unsigned((second - s) * 1.0e6);

    return GregorianDate(year, month, day, hour, minute, s, usec, TimeScale_UTC);
}


/** Construct a TDB calendar date from a Julian day number in the TDB time scale.
  */
TimeConversions::GregorianDate
TimeConversions::GregorianDate::TDBDateFromTDBJD(double tdbjd)
{
    int year = 0;
    unsigned int month = 1;
    unsigned int day = 1;
    unsigned int hour = 0;
    unsigned int minute = 0;
    double second = 0;

    JDToCalendar(tdbjd, year, month, day, hour, minute, second);
    unsigned int s = unsigned(second);
    unsigned int usec = unsigned((second - s) * 1.0e6);

    return GregorianDate(year, month, day, hour, minute, s, usec, TimeScale_TDB);
}


TimeConversions::GregorianDate
TimeConversions::GregorianDate::UTCDateFromTDBSec(double tdbsec)
{
    return UTCDateFromTDBJD(convertSecToJD(tdbsec));
}


/** Construct a TDB calendar date from a Julian day number in the TDB time scale.
  */
TimeConversions::GregorianDate
TimeConversions::GregorianDate::TDBDateFromTDBSec(double tdbsec)
{
    return TDBDateFromTDBJD(convertSecToJD(tdbsec));
}


/** Convert the date to a string with the specified format.
  */
string
TimeConversions::GregorianDate::toString(Format format) const
{
    if (format == ISO8601_Combined)
    {
        ostringstream str;
        str << year() << '-'
            << setw(2) << setfill('0') << month() << '-'
            << setw(2) << setfill('0') << day() << 'T'
            << setw(2) << setfill('0') << hour() << ':'
            << setw(2) << setfill('0') << minute() << ':'
            << setw(2) << setfill('0') << second();

        switch (m_timeScale)
        {
        case TimeScale_TDB:
            str << " TDB";
            break;
        case TimeScale_TT:
            str << " TT";
            break;
        case TimeScale_TAI:
            str << " TAI";
            break;
        case TimeScale_UTC:
            str << " UTC";
            break;
        }

        return str.str();
    }
    else
    {
        return "";
    }
}

