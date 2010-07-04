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
------------------ Author:       Guillermo Ortega               -------------------
------------------ Affiliation:  European Space Agency          -------------------
-----------------------------------------------------------------------------------
Created October 2009

*/



#include "STAcalculator.h"
#include "Astro-Core/calendarTOjulian.h"
#include "Astro-Core/date.h"
#include "Astro-Core/stabody.h"

STAcalculator::STAcalculator ( QWidget * parent, Qt::WindowFlags f) : QDialog(parent, f)
{
	setupUi(this);
	// Setting up the currect date into the calendar widget
	QDateTime TheCurrentDateAndTime = QDateTime::currentDateTime();
	dateTimeEdit->setDateTime(TheCurrentDateAndTime);
	// Setting Earth as favourite plannet
	int itemNumber = 0;
	QString centralBodyName = comboBoxPlanets->itemText(itemNumber);
	const StaBody* body = STA_SOLAR_SYSTEM->lookup(centralBodyName);
	double r = body->meanRadius();
	double p = body->orbitalPeriod();
	double i = body->inclination();
	double d = body->distance();
	double mu = body->mu();
	AverageRadiusEdit->setText(QString::number(r, 'f', 4));
	PeriodEdit->setText(QString::number(p, 'f', 4));
	InclinationEdit->setText(QString::number(i, 'f', 4));
	DistanceSunEdit->setText(QString::number(d, 'f', 4));
	gravityConstantEdit->setText(QString::number(mu, 'f', 4));
}



STAcalculator::~STAcalculator()
{
}

/*
  // No help for the time being
void STAcalculator::on_buttonBox_helpRequested()
{
	qWarning("TODO: %s	%d",__FILE__,__LINE__);
}
*/

/*
void STAcalculator::on_buttonBox_clicked(QAbstractButton*)
{
	qWarning("TODO: %s	%d",__FILE__,__LINE__);
}

void STAcalculator::on_tabWidget_currentChanged(int)
{
	qWarning("TODO: %s	%d",__FILE__,__LINE__);
}
*/


void STAcalculator::on_comboBoxPlanets_activated(const QString&)
{
    int itemNumber = comboBoxPlanets->currentIndex();
    QString centralBodyName = comboBoxPlanets->itemText(itemNumber);
    const StaBody* body = STA_SOLAR_SYSTEM->lookup(centralBodyName);
    double r = body->meanRadius();
    double p = body->orbitalPeriod();
    double i = body->inclination();
    double d = body->distance();
    double mu = body->mu();
    AverageRadiusEdit->setText(QString::number(r, 'f', 4));
    PeriodEdit->setText(QString::number(p, 'f', 4));
    InclinationEdit->setText(QString::number(i, 'f', 4));
    DistanceSunEdit->setText(QString::number(d, 'f', 4));
    gravityConstantEdit->setText(QString::number(mu, 'f', 4));
}


/*
void STAcalculator::on_AverageRadiusEdit_textChanged(const QString&)
{
	qWarning("TODO: %s	%d",__FILE__,__LINE__);
}

void STAcalculator::on_PeriodEdit_textChanged(const QString&)
{
	qWarning("TODO: %s	%d",__FILE__,__LINE__);
}

void STAcalculator::on_InclinationEdit_textChanged(const QString&)
{
	qWarning("TODO: %s	%d",__FILE__,__LINE__);
}

void STAcalculator::on_DistanceSunEdit_textChanged(const QString&)
{
	qWarning("TODO: %s	%d",__FILE__,__LINE__);
}

void STAcalculator::on_groupBox_toggled(bool)
{
	qWarning("TODO: %s	%d",__FILE__,__LINE__);
}

*/



void STAcalculator::on_dateTimeEdit_dateTimeChanged(const QDateTime&)
{
    double Epoch, ModifiedEpoch, FractionOfADay;
    //Epoch.setRealNumberPrecision(16);
    // Defining the separator
    QRegExp separator("\\s+");  // the + Means one or more spaces!

    QDateTime EpochDateTime = dateTimeEdit->dateTime();
    // Converting from QDateTime into integers via QString
    QString Temporal = EpochDateTime.toString("yyyy MM dd hh mm ss.zzzz");
    int TheYear           = int (Temporal.section(separator, 0, 0).toDouble());    //out << "YYYY: " << TheYear << endl;
    int TheMonth          = int (Temporal.section(separator, 1, 1).toDouble());    //out << "MM: " << TheMonth << endl;
    int TheDay            = int (Temporal.section(separator, 2, 2).toDouble());    //out << "dd: " << TheDay << endl;
    int TheHour           = int (Temporal.section(separator, 3, 3).toDouble());    //out << "hh: " << TheHour << endl;
    int TheMinute         = int (Temporal.section(separator, 4, 4).toDouble());    //out << "mm: " << TheMinute << endl;
    double TheSecond      = int (Temporal.section(separator, 5, 5).toDouble());    //out << "ss: " << TheSecond << endl;
    // Making the conversion to Julian days.
    Epoch = calendarTOjulian (TheYear, TheMonth, TheDay, TheHour, TheMinute, TheSecond);
    // Notice that currently convertion to JulianDate handles fractions of seconds in UTC
    JDUTCEdit->setText(QString::number(Epoch, 'f', 8));

    FractionOfADay = sta::secsToDays(Epoch);
    DayFractionEdit->setText(QString::number(FractionOfADay, 'f', 8));

    ModifiedEpoch = sta::JdToMjd (Epoch);
    MJDUTCEdit->setText(QString::number(ModifiedEpoch, 'f', 8));

    Epoch = calendarTOjulianTDB (TheYear, TheMonth, TheDay, TheHour, TheMinute, TheSecond);
    JDTDBEdit->setText(QString::number(Epoch, 'f', 8));

    ModifiedEpoch = sta::JdToMjd (Epoch);
    MJDTDBEdit->setText(QString::number(ModifiedEpoch, 'f', 8));

}

void STAcalculator::on_JDUTCEdit_textChanged(const QString&)
{

    double Epoch, EpochTDB, ModifiedEpoch, FractionOfADay;

    Epoch = JDUTCEdit->text().toDouble();

    QDateTime myDateAndTime = sta::JdToCalendar(Epoch);
    dateTimeEdit->setDateTime(myDateAndTime);

    /*
    FractionOfADay = sta::secsToDays(Epoch);
    DayFractionEdit->setText(QString::number(FractionOfADay, 'f', 8));

    ModifiedEpoch = sta::JdToMjd (Epoch);
    MJDUTCEdit->setText(QString::number(ModifiedEpoch, 'f', 8));

    JDTDBEdit->setText(QString::number(EpochTDB, 'f', 8));

    ModifiedEpoch = sta::JdToMjd (EpochTDB);
    MJDTDBEdit->setText(QString::number(ModifiedEpoch, 'f', 8));
    */

}

void STAcalculator::on_MJDUTCEdit_textChanged(const QString&)
{
    double Epoch, JulianDateUTC;

    Epoch = MJDUTCEdit->text().toDouble();
    JulianDateUTC = sta::MjdToJd(Epoch);

    QDateTime myDateAndTime = sta::JdToCalendar(JulianDateUTC);
    dateTimeEdit->setDateTime(myDateAndTime);
}

void STAcalculator::on_JDTDBEdit_textEdited(const QString&)
{
    double Epoch, JulianDateUTC;

    Epoch = JDTDBEdit->text().toDouble();

    JulianDateUTC = Epoch - 0.00076604;

    QDateTime myDateAndTime = sta::JdToCalendar(JulianDateUTC);
    dateTimeEdit->setDateTime(myDateAndTime);

    JDTDBEdit->setText(QString::number(Epoch, 'f', 8));


}

void STAcalculator::on_MJDTDBEdit_textEdited(const QString&)
{
    double Epoch = MJDTDBEdit->text().toDouble();
    double JulianDateUTC = sta::MjdToJd(Epoch);

    QDateTime myDateAndTime = sta::JdToCalendar(JulianDateUTC - 0.00076604);
    dateTimeEdit->setDateTime(myDateAndTime);

    MJDTDBEdit->setText(QString::number(Epoch, 'f', 8));
}

void STAcalculator::on_DayFractionEdit_textChanged(const QString&)
{
	//qWarning("TODO: %s	%d",__FILE__,__LINE__);
    // Do nothing
}

