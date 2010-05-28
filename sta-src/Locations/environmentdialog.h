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
 ------------------ Author: Ricardo Noriega  ----------------------------------------------
 ------------------ email: ricardonor@gmail.com  ------------------------------------------

 */

#ifndef ENVIRONMENTDIALOG_H
#define ENVIRONMENTDIALOG_H

#include "ui_environmentDialog.h"
#include <QDialog>
#include "Scenario/staschema.h"
#include "Coverage/commanalysis.h"

class environmentDialog : public QDialog , private Ui::environmentDialog
{
Q_OBJECT
public:
	environmentDialog( QWidget * parent = 0, Qt::WindowFlags f = 0 );
	~environmentDialog();

        bool loadValues(ScenarioGroundStationEnvironment* environment);
        bool saveValues(ScenarioGroundStationEnvironment* environment);
        ScenarioGroundStationEnvironment m_environment;

	
};
#endif //ENVIRONMENTDIALOG_H
