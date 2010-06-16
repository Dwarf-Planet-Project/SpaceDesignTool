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
 ------------------ Author: Guillermo Ortega  -------------------------------------------------
 ------------------ CoAuthor: Ozgun YILMAZ    ------------------------------------------
 */


#ifndef SEM_H
#define SEM_H

#include "Main/mainwindow.h"
#include "ui_sem.h"
#include <QDialog>

#include "Scenario/staschema.h"

class MainWindow;

class sem : public QDialog , private Ui::SEMDialogClass
{
Q_OBJECT
public:
        sem(SpaceScenario* SCscenario, MainWindow* parent);
	~sem();

protected slots:
        void on_SelectSpacecraftComboBox_activated(const QString&);
	void on_buttonBox_helpRequested();
	void on_buttonBox_clicked(QAbstractButton*);
	void on_buttonBox_destroyed(QObject*);
	void on_buttonBox_destroyed();
	void on_buttonBox_customContextMenuRequested(const QPoint&);
	void on_buttonBox_accepted();
	void on_buttonBox_rejected();
private:
       SpaceScenario* scenario;
       MainWindow* mainwindow;


};
#endif //SEM_H

