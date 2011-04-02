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
 ------------------ Author: Guillermo Ortega  -------------------------------------------------
 ------------------ European Space Agency ----------------------------

 */



#ifndef LAGRANGIANDIALOG_H
#define LAGRANGIANDIALOG_H

#include "ui_lagrangianDialog.h"

#include "Main/scenariotree.h"
#include "Main/mainwindow.h"
#include "Scenario/scenario.h"

#include <QDialog>

class lagrangianDialog : public QDialog , private Ui::lagrangianDialog
{
Q_OBJECT
public:
    lagrangianDialog(MainWindow* parent);
	~lagrangianDialog();
protected slots:
	void on_buttonBox_accepted();
	void on_buttonBox_rejected();

private:
    MainWindow* mainwindow;
	
};
#endif //LAGRANGIANDIALOG_H
