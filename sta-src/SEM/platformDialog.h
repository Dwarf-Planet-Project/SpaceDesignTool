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
------------------ Author: Guillermo Ortega  -------------------------------------------
*/

#ifndef PLATFORMDIALOG_H
#define PLATFORMDIALOG_H

#include "ui_platform.h"

#include "Scenario/scenario.h"
#include "Scenario/propagationfeedback.h"
#include "Astro-Core/statevector.h"

#include <QDialog>

class ScenarioTree;

class PlatformDialog : public QDialog , private Ui::PlatformDialogClass
{
Q_OBJECT

public:
    //PlatformDialog( QWidget * parent = 0, Qt::WindowFlags f = 0 );
    PlatformDialog(ScenarioTree* parent = NULL);
	~PlatformDialog();

    bool loadValues(ScenarioSCSystemType* spacecraftPlatformSystem);
    bool saveValues(ScenarioSCSystemType* spacecraftPlatformSystem);

protected slots:
	void on_buttonBoxLoitering_helpRequested();
	void on_buttonBoxLoitering_clicked(QAbstractButton*);
	void on_PlatformProperties_currentChanged(int);
	void on_groupBox_toggled(bool);
	void on_comboBox_2_activated(const QString&);
	void on_TotalMass_textChanged(const QString&);
	void on_comboBox_activated(const QString&);
	void on_Ixx_textChanged(const QString&);
	void on_Ixy_textChanged(const QString&);
	void on_Ixz_textChanged(const QString&);
	void on_Iyy_textChanged(const QString&);
	void on_Iyz_textChanged(const QString&);
	void on_Izz_textChanged(const QString&);
	void on_Izy_textChanged(const QString&);
	void on_Izx_textChanged(const QString&);
	void on_Iyx_textChanged(const QString&);
	void on_groupBox_2_toggled(bool);
	void on_Width_textChanged(const QString&);
	void on_Length_textChanged(const QString&);
	void on_Height_textChanged(const QString&);
	void on_Volume_textChanged(const QString&);
	void on_AxialFrequency_textChanged(const QString&);
	void on_LateralFrequency_textChanged(const QString&);
	void on_PlatformDialogClass_accepted();
	void on_PlatformDialogClass_rejected();
	
};
#endif //PLATFORMDIALOG_H
