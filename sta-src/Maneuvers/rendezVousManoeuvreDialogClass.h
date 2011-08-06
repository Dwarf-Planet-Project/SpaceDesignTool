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
------------------ Author: Guillermo Ortega August 2011 -------------------------------------------
*/

#ifndef RENDEZVOUSMANOEUVREDIALOGCLASS_H
#define RENDEZVOUSMANOEUVREDIALOGCLASS_H

#include "ui_rendezVousManoeuvreDialog.h"
#include <QDialog>

#include "Scenario/scenario.h"
//#include "Scenario/propagationfeedback.h"
//#include "Scenario/missionAspectDialog.h"

//#include "Astro-Core/statevector.h"
//#include "Astro-Core/attitudevector.h"

//#include "Services/perturbationForcesDialog.h"
//#include "Services/stoppingConditionsTrajectoryDialog.h"
//#include "Services/serviceDistanceUnit.h"
//#include "Services/serviceDistanceRateUnit.h"
//#include "Services/serviceAngleUnit.h"
//#include "Services/serviceAngleRateUnit.h"

class rendezVousManoeuvreDialogClass : public QDialog , private Ui::rendezVousManoeuvreDialogClass
{
Q_OBJECT
public:
	rendezVousManoeuvreDialogClass( QWidget * parent = 0, Qt::WindowFlags f = 0 );
	~rendezVousManoeuvreDialogClass();

public:
    bool loadValues(ScenarioRendezVousManoeuvreType* rendezvous);
    bool saveValues(ScenarioRendezVousManoeuvreType* rendezvous);


protected slots:
	void on_AddRendezVousManoeuvrePushButton_clicked();
	void on_RemoveRendezVousManoeuvrePushButton_clicked();
	void on_buttonBoxRendezVousManoeuvreDialog_helpRequested();
	void on_buttonBoxRendezVousManoeuvreDialog_clicked(QAbstractButton*);
	void on_treeWidgetRendezVousManoeuvreToChoose_itemDoubleClicked(QTreeWidgetItem*,int);
	void on_treeWidgetRendezVousManoeuvreToChoose_itemClicked(QTreeWidgetItem*,int);
	void on_treeWidgetRendezVousManoeuvreSelected_itemClicked(QTreeWidgetItem*,int);
	void on_treeWidgetRendezVousManoeuvreSelected_itemDoubleClicked(QTreeWidgetItem*,int);
	
};
#endif //RENDEZVOUSMANOEUVREDIALOGCLASS_H
