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

 */


#ifndef RADARPAYLOADDIALOG_H
#define RADARPAYLOADDIALOG_H

#include "ui_radarPayloadDialog.h"
#include <QDialog>

#include "sta-src/Scenario/staschema.h"



class radarPayloadDialog : public QDialog , private Ui::radarPayloadDialog
{
Q_OBJECT
public:
	radarPayloadDialog( QWidget * parent = 0, Qt::WindowFlags f = 0 );
	~radarPayloadDialog();

	bool loadValues(ScenarioRadarPayloadType*);
	bool saveValues(ScenarioRadarPayloadType*);

protected slots:
	void on_radarPayloadDialog_accepted();
	void on_radarPayloadDialog_rejected();
	
};
#endif //RADARPAYLOADDIALOG_H
