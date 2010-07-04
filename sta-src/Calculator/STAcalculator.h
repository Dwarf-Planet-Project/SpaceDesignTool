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


#ifndef STAcalculator_H
#define STAcalculator_H

#include "ui_STAcalculator.h"
#include <QDialog>

class STAcalculator : public QDialog , private Ui::STACalculatorDialog
{
Q_OBJECT
public:
	STAcalculator( QWidget * parent = 0, Qt::WindowFlags f = 0 );
	~STAcalculator();
protected slots:
	//void on_buttonBox_helpRequested();
	//void on_buttonBox_clicked(QAbstractButton*);
	//void on_tabWidget_currentChanged(int);
	void on_comboBoxPlanets_activated(const QString&);
	//void on_AverageRadiusEdit_textChanged(const QString&);
	//void on_PeriodEdit_textChanged(const QString&);
	//void on_InclinationEdit_textChanged(const QString&);
	//void on_DistanceSunEdit_textChanged(const QString&);
	//void on_groupBox_toggled(bool);
	void on_dateTimeEdit_dateTimeChanged(const QDateTime&);
	void on_JDUTCEdit_textChanged(const QString&);
	void on_MJDUTCEdit_textChanged(const QString&);
	void on_JDTDBEdit_textEdited(const QString&);
	void on_MJDTDBEdit_textEdited(const QString&);
	void on_DayFractionEdit_textChanged(const QString&);
	
};
#endif //STAcalculator_H
