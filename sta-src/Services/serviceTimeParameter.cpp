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
/*
------------------ Author: Guillermo Ortega  ----------------------------------------
 May 2011

 */

#include "serviceTimeParameter.h"
#include "Astro-Core/date.h"
#include "Astro-Core/calendarTOjulian.h"
#include <cmath>
#include <QDebug>

#include <vector>
#include <map>
#include <algorithm>
#include <iomanip>
#include <sstream>
#include <cassert>

using namespace sta;

DialogServiceTimeParameterFrame::DialogServiceTimeParameterFrame( QWidget * parent, Qt::WindowFlags f) : QFrame(parent,f)
{
	setupUi(this);
}

DialogServiceTimeParameterFrame::~DialogServiceTimeParameterFrame()
{
}

void DialogServiceTimeParameterFrame::on_comboBoxTimeParameterChoice_currentIndexChanged(int)
{
	qWarning("TODO: %s	%d",__FILE__,__LINE__);
}

// ---------------- Modified Julian Date in UTC -------------
double convertToMJDUTC(double mjd[], int num)
{
    //Convert Modified Julian Date into Julian Date, in TDB
    double julianDate=sta::MjdToJd(mjd[num]+0.00001);
    //Convert JD in TDB into a Gregorian Date in UTC
    TimeConversions::GregorianDate gregUTC=TimeConversions::GregorianDate::UTCDateFromTDBJD(julianDate);
    //Converts Gregorian Date in UTC into Julian Date UTC
    double julUTC= calendarTOjulian(gregUTC.year(), gregUTC.month(), gregUTC.day(), gregUTC.hour(), gregUTC.minute(),gregUTC.second());
    //Converts Julian Date into Modified Julian Date in UTC
    double mjdUTC=sta::JdToMjd(julUTC);

    return mjdUTC;

}

//---------------- Julian Date in TDB ----------------------
double convertToJulianTDB(double mjd[], int num)
{
    //Convert Modified Julian Date into Julian Date, in TDB
    double julianDate=sta::MjdToJd(mjd[num]+0.00001);

    return julianDate;
}

//----------------- Gregorian UTC ----------------------------
QString convertToGregUTC(double mjd[], int num)
{
    double julianDate=sta::MjdToJd(mjd[num]+0.00001);
    TimeConversions::GregorianDate utc=TimeConversions::GregorianDate::UTCDateFromTDBJD(julianDate);

    // Convert the GregorianDate to a QDateTime for formatting
    QDateTime qtUTC(QDate(utc.year(), utc.month(), utc.day()),
                    QTime(utc.hour(), utc.minute(), utc.second()), Qt::UTC);

    return qtUTC.toString("dd/MM/yyyy hh:mm:ss");
}

//----------------------- Gregorian TDB ------------------------
QString convertToGregTDB(double mjd[], int num)
{
    //Convert Modified Julian Date into Julian Date, in TDB
    double julianDate=sta::MjdToJd(mjd[num]+0.00001);
    //Converts Julian Date into Seconds
    double jdSec = sta::JdToSecJ2000(julianDate);
    //Converts SecondsJ2000 in TDB into Gregorian TDB
    TimeConversions::GregorianDate gregTDB=TimeConversions::GregorianDate::TDBDateFromTDBSec(jdSec);

    // Convert the GregorianDate to a QDateTime for formatting
    QDateTime qtTDB(QDate(gregTDB.year(), gregTDB.month(), gregTDB.day()),
                    QTime(gregTDB.hour(), gregTDB.minute(), gregTDB.second()));         //, Qt::UTC);

    return qtTDB.toString("dd/MM/yyyy hh:mm:ss");
}

//--------------------- Julian Ephemeris Date ----------------------
double convertToJulianEphDate(double mjd[], int num)
{
    //Convert from MJD to JD, in TDB
    double julianDate=sta::MjdToJd(mjd[num]+0.00001);
    // Convert from JD to JD in Seconds
    double TDBSeconds=sta::JdToSecJ2000(julianDate);
    //Convert seconds TDB to seconds TDT
    double TDTDate= convertTDBtoTT(TDBSeconds);
    //Convert the seconds back to JD or directly to Gregorian
    double jdTDT=sta::SecJ2000ToJd(TDTDate);

    return jdTDT;
}

// ----------------------- Julian GPS ------------------------------
double convertToJulianGPS(double mjd[], int num)
{
    //Convert from MJD to JD
    double julianDate=sta::MjdToJd(mjd[num])+0.00001;
    // Convert from JD to JD in Seconds
    double tdbSec=sta::JdToSecJ2000(julianDate);
    //Convert seconds TDB to seconds TDT
    double gpsDate =  convertTDBtoGPS(tdbSec);
    //Convert the seconds back to JD or directly to Gregorian
    double jdGPS=sta::SecJ2000ToJd(gpsDate);

    return jdGPS;
}

//----------------------- Gregorian TAI ------------------------
QString convertToGregTAI(double mjd[], int num)
{
    //Convert from MJD to JD
    double julianDate=sta::MjdToJd(mjd[num]+0.00001);
    // Convert from JD to JD in Seconds
    double TDBSeconds=sta::JdToSecJ2000(julianDate);
    //Convert seconds TDB to seconds TAI
    double TAIDate= convertTDBtoTAI(TDBSeconds);
    //Convert the seconds back to JD or directly to Gregorian
    double jdTAI=sta::SecJ2000ToJd(TAIDate);
    QDateTime gregTAI = sta::JdToCalendar(jdTAI);

    // Convert the GregorianDate to a QDateTime for formatting
    QDateTime qtTAI(QDate(gregTAI.date().year(), gregTAI.date().month(), gregTAI.date().day()),
                    QTime(gregTAI.time().hour(), gregTAI.time().minute(), gregTAI.time().second()));         //, Qt::UTC);

    return qtTAI.toString("dd/MM/yyyy hh:mm:ss");
}


//----------------------- Gregorian TDT ------------------------
QString convertToGregTDT(double mjd[], int num)
{
    //Convert from MJD to JD
    double julianDate=sta::MjdToJd(mjd[num]+0.00001);
    // Convert from JD to JD in Seconds
    double TDBSeconds=sta::JdToSecJ2000(julianDate);
    //Convert seconds TDB to seconds TDT
    double TDTDate= convertTDBtoTT(TDBSeconds);
    //Convert the seconds back to JD or directly to Gregorian
    double jdTDT=sta::SecJ2000ToJd(TDTDate);
    QDateTime gregTDT = sta::JdToCalendar(jdTDT);

    // Convert the GregorianDate to a QDateTime for formatting
    QDateTime qtTDT(QDate(gregTDT.date().year(), gregTDT.date().month(), gregTDT.date().day()),
                    QTime(gregTDT.time().hour(), gregTDT.time().minute(), gregTDT.time().second()));         //, Qt::UTC);

    return qtTDT.toString("dd/MM/yyyy hh:mm:ss");

}

//----------------------- Gregorian GPS ------------------------
QString convertToGregGPS(double mjd[], int num)
{
    // GPS = TAI - 19s
    //Convert from MJD to JD
    double julianDate=sta::MjdToJd(mjd[num])+0.00001;
    // Convert from JD to JD in Seconds
    double tdbSec=sta::JdToSecJ2000(julianDate);
    //Convert seconds TDB to seconds TDT
    double gpsDate =  convertTDBtoGPS(tdbSec);
    //Convert the seconds back to JD or directly to Gregorian
    double JDGPS=sta::SecJ2000ToJd(gpsDate);
    QDateTime gregGPS = sta::JdToCalendar(JDGPS);

    // Convert the GregorianDate to a QDateTime for formatting
    QDateTime qtGPS(QDate(gregGPS.date().year(), gregGPS.date().month(), gregGPS.date().day()),
                    QTime(gregGPS.time().hour(), gregGPS.time().minute(), gregGPS.time().second()));         //, Qt::UTC);

    return qtGPS.toString("dd/MM/yyyy hh:mm:ss");

}

//----------------------- Earth Canonical Time ------------------------
double convertToEarthCanTime(double mjd[], int num, double StartEpoch)
{
    // Earth canonical time = mjsdtoseconds[index]/orbitalperiod
    // Convert MJD date into Seconds
    double dateSec=sta::MjdToFromEpoch(StartEpoch, mjd[num], "Seconds");
    // Earth orbital period
    double Unit=sqrt(6378.137*6378.137*6378.137/398600.441);
    // Earth canonical time
    double EarthCanTime = dateSec/Unit;

    return EarthCanTime;
}

//----------------------- YYDDD ------------------------
void convertToYYDDD(double MJDdate[],int index, int * Last2Digits , QList<double> *DDD)
{
    double julianDate=sta::MjdToJd(MJDdate[index])+0.00001;
    QDateTime dateTime = sta::JdToCalendar(julianDate);

    int Year=dateTime.date().year();
    *Last2Digits = Year - floor(Year/100)*100;

    QDateTime FirstDayCurrentYear(QDate(Year,1,1),QTime(0,0,0));
    double StartYearTime=sta::JdToMjd(sta::CalendarToJd(FirstDayCurrentYear));
    double DayOfYear=floor(sta::MjdToFromEpoch(StartYearTime,MJDdate[index],"Days")+1);
    *DDD=sta::DayOfYearToDDD(DayOfYear);


}

//--------------------- Julian UTC --------------------------
void convertToJulianUTC(double mjd[],int num, int * Last2Digits , QList<double> *DDD, double *hour, double *minute, double *second)
{
    //Convert Modified Julian Date into Julian Date, in TDB
    double julianDate=sta::MjdToJd(mjd[num]+0.00001);
    //Convert JD in TDB into a Gregorian Date in UTC
    TimeConversions::GregorianDate gregUTC=TimeConversions::GregorianDate::UTCDateFromTDBJD(julianDate);

    *hour = gregUTC.hour();
    *minute = gregUTC.minute();
    *second = gregUTC.second();
    int Year=gregUTC.year();

    *Last2Digits = Year - floor(Year/100)*100;

    QDateTime FirstDayCurrentYear(QDate(Year,1,1),QTime(0,0,0));
    double StartYearTime=sta::JdToMjd(sta::CalendarToJd(FirstDayCurrentYear));
    double DayOfYear=floor(sta::MjdToFromEpoch(StartYearTime,mjd[num],"Days")+1);
    *DDD=sta::DayOfYearToDDD(DayOfYear);
}
