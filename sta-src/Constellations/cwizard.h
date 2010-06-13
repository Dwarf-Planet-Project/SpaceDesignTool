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

// Authors: Class and Steffen


#ifndef CWIZARD_H
#define CWIZARD_H

#include "ui_cwizard.h"
#include <QWizard>

class cwizard : public QWizard , private Ui::cwizard
{
Q_OBJECT
public:
	cwizard( QWidget * parent = 0, Qt::WindowFlags f = 0 );
	~cwizard();
protected slots:
	void on___qt__passive_wizardbutton0_clicked();
	void on___qt__passive_wizardbutton1_clicked();
	void on_comboBoxConstellationType_activated(const QString&);
	void on_StartingEpochdateTimeEdit_dateTimeChanged(const QDateTime&);
	void on_EndingEpochdateTimeEdit_dateTimeChanged(const QDateTime&);
	void on_lineEdit_textChanged(const QString&);
	void on_centralBodyComboBox_activated(const QString&);
	void on_lineEdit_2_textChanged(const QString&);
	void on_lineEdit_3_textChanged(const QString&);
	void on_lineEdit_4_textChanged(const QString&);
	void on_GravityFieldRadioButton_toggled(bool);
	void on_ZonalsSpinBox_valueChanged(int);
	void on_GravityComboBox_activated(const QString&);
	void on_AtmDragRadioButton_toggled(bool);
	void on_AtmosphereComboBox_activated(const QString&);
	void on_PertBodyRadioButton_toggled(bool);
	void on_AddPushButton_clicked();
	void on_RemovePushButton_clicked();
	void on_PropagatorComboBox_activated(const QString&);
	void on_IntegratorComboBox_activated(const QString&);
	void on_IntStepEdit_textChanged(const QString&);
	void on___qt__passive_wizardbutton5_clicked();
	
};
#endif //CWIZARD_H
