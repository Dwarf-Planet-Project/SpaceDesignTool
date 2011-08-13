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
 ------------------ Affiliation:  European Space Agency (ESA)    -------------------
 -----------------------------------------------------------------------------------
*/

#ifndef STOPPINGCONDITIONSTRAJECTORYDIALOG_H
#define STOPPINGCONDITIONSTRAJECTORYDIALOG_H

#include "ui_stoppingConditionsTrajectory.h"
#include <QDialog>

#include "Scenario/scenario.h"
#include "Services/serviceDistanceUnit.h"
#include "Services/serviceEpoch.h"
#include "Services/serviceGenericParameter.h"

class stoppingConditionsTrajectoryDialog : public QDialog , private Ui::stoppingConditionsTrajectoryDialogClass
{
Q_OBJECT
public:
	stoppingConditionsTrajectoryDialog( QWidget * parent = 0, Qt::WindowFlags f = 0 );
	~stoppingConditionsTrajectoryDialog();

public:
    bool loadValues(const ScenarioTrajectoryStoppingConditionType* listOfStopConditions);
    bool saveValues(ScenarioTrajectoryStoppingConditionType* listOfStopConditions);

public slots:

protected slots:
    void addSelectedParameters();
    void removeSelectedParameters();

    void on_treeWidgetStoppingConditionsTrajectoryToChoose_itemDoubleClicked(QTreeWidgetItem* myItem, int theNumber);
    void on_treeWidgetStoppingConditionsTrajectorySelected_itemDoubleClicked(QTreeWidgetItem* myItem, int theNumber);

	void on_buttonBoxStoppingConditionsTrajectoryDialog_accepted();
	void on_buttonBoxStoppingConditionsTrajectoryDialog_rejected();

private:
    void addParameter(QTreeWidgetItem* item);
    void outputTableOptions(QTreeWidgetItem* item);
	
};
#endif //STOPPINGCONDITIONSTRAJECTORYDIALOG_H
