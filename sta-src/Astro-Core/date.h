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
#include <iostream>


double convertTDBtoTAI(double ttSec);
double convertTTtoTAI(double ttSec);
double convertTDBtoTT(double tdbSec);
double convertTAItoTDB(double taiSec);
double convertTAItoTT(double taiSec);
double convertTTtoTDB(double ttSec);
double convertTDBtoGPS(double tdbSec);

double convertToMJDUTC(double mjd[], int num);              //Added by Catarina
double convertToJulianTDB(double mjd[], int num);           //Added by Catarina
double convertToJulianDate(double mjd[], int num);          //Added by Catarina
double convertToJulianEphDate(double mjd[], int num);       //Added by Catarina
double convertToJulianGPS(double mjd[], int num);           //Added by Catarina
//double convertToEarthCanTime(double mjd[], int num);        //Added by Catarina


namespace sta
{
    int MjdToElapsed(double StartEpoch, int ArrayIndex); //added by Ana
    QDateTime JdToCalendar(double jd);
    double CalendarToJd(const QDateTime& calendarDate);
    double MjdToJd(double mjd);
    double JdToMjd(double jd);
    double MjdToSecJ2000(double mjd);
    double SecJ2000ToMjd(double sec);
    double JdToSecJ2000(double jd);
    double SecJ2000ToJd(double sec);
    double MjdToFromEpoch(double StartEpoch, double mjd, QString Units); //added by Ana
    double DateTimeTOjulian(QDateTime DateTime); //Added by Ana
    bool CheckIfLeapYear(int year); //added by Ana
    QString calendarToDayOfYear(QDateTime DateTime); //added by Ana
    QList<QString> TimeLayout(int day, int month); //added by Ana
    QList<double> DayOfYearToDDD(double DayOfYear);//added by Ana
    QString MissionElapsedTime(double Date, double StartEpoch);//added by Ana

//    double convertToMJDUTC(double *MJDdate, int index);


    template <typename T> T secsToDays(T secs)
    {
        return secs / 86400.0;
    }
    
    template <typename T> T daysToSecs(T days)
    {
        return days * 86400.0;
    }
    
    extern double J2000;
    extern double MJDBase;

    //----------------------------------
}
namespace TimeConversions
    {
    class LeapSecondTable;

    /** TimeScale
      */
    enum TimeScale
    {
        TimeScale_TDB  = 0, /**< Barycentric Dynamical Time */
        TimeScale_TT   = 1, /**< Terrestrial Time */
        TimeScale_TAI  = 2, /**< International Atomic Time */
        TimeScale_UTC  = 3, /**< Coordinated Universal Time */
    };


    /** An instance of GregorianDate the 'name' of an instant in time. The name can
      * be converted to a numeric value in one of VESTA's supported uniform time scales.
      * VESTA uses Barycentric Dynamical Time (TDB) as the time variable for planetary
      * ephemerides and spacecraft trajectories. Terrestrial Time (TT) and International
      * Atomic Time (TAI) are supported for conversions, but should not be used as inputs
      * for Trajectory and RotationModel classes. There's no conversion to a purely numeric
      * representation of UTC as the insertion of leap seconds means that it isn't a
      * uniform time scale.
      *
      * The most typical uses of GregorianDate are to convert from UTC to TDB and vice
      * versa. To convert a UTC calendar date to a time in seconds since J2000.0 TDB
      * (suitable for input to Trajectory::state()):
      *
      * \code
      * GregorianDate date(2010, 8, 28, 11, 30, 0);
      * double tdbsec = date.toTDBSec();
      * \endcode
      *
      * To create a human-readable date string from a time in seconds since J2000.0 TDB:
      * \code
      * string dateStr = GregorianDate::UTCDateFromTDBSec(tsec).toString();
      * \endcode
      */
    class GregorianDate
    {
    public:
        enum Format
        {
            ISO8601_Combined = 0,
        };

        GregorianDate();
        GregorianDate(const GregorianDate& other);
        GregorianDate(int year, unsigned int month, unsigned int day,
                      unsigned int hour = 0, unsigned int minute = 0, unsigned int second = 0, unsigned int usec = 0,
                      TimeScale timeScale = TimeScale_UTC);

        GregorianDate& operator=(const GregorianDate& other);
        int year() const
        {
            return m_year;
        }

        unsigned int month() const
        {
            return m_month;
        }

        unsigned int day() const
        {
            return m_day;
        }

        unsigned int hour() const
        {
            return m_hour;
        }

        unsigned int minute() const
        {
            return m_minute;
        }

        unsigned int second() const
        {
            return m_second;
        }

        unsigned int usec() const
        {
            return m_usec;
        }

        TimeScale timeScale() const
        {
            return m_timeScale;
        }



        /** Change the time scale of this date. No conversion is applied, thus
          * calling this method with a different time scale means that the time
          * object will represent a different instant.
          */
        void setTimeScale(TimeScale timeScale)
        {
            m_timeScale = timeScale;
        }

        unsigned int dayOfWeek() const;
        unsigned int dayOfYear() const;
        unsigned int daysInMonth() const;
        int julianDay() const;

        bool isValid() const;

        bool isLeapYear() const;

        double toTDBJD() const;
        double toTAIJD() const;
        double toTTJD() const;
        double toTDBSec() const;
        double toTTSec() const;

        std::string toString(Format format = ISO8601_Combined) const;

        static GregorianDate UTCDateFromTDBJD(double tdbjd);
        static GregorianDate TDBDateFromTDBJD(double tdbjd);
        static GregorianDate UTCDateFromTDBSec(double tdbsec);
        static GregorianDate TDBDateFromTDBSec(double tdbsec);
        static GregorianDate convertToGregLCL(double mjd[], int num);       //Added by Catarina

    private:
        int m_year;
        unsigned int m_month;
        unsigned int m_day;
        unsigned int m_hour;
        unsigned int m_minute;
        unsigned int m_second;
        unsigned int m_usec;
        TimeScale m_timeScale;

        static LeapSecondTable* s_DefaultLeapSecondTable;

    };


}
//std::ostream& operator<<(std::ostream& out, const TimeConversions::GregorianDate& date);
#endif // _ASTROCORE_STATEVECTOR_H_
