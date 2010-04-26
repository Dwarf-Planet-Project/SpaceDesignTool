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

#ifndef TRANSMITTERPAYLOADDIALOG_H
#define TRANSMITTERPAYLOADDIALOG_H

#include "ui_transmitterPayloadDialog.h"
#include <QDialog>

#include "sta-src/Scenario/staschema.h"



class transmitterPayloadDialog : public QDialog , private Ui::transmitterPayloadDialog
{
Q_OBJECT
public:
	transmitterPayloadDialog( QWidget * parent = 0, Qt::WindowFlags f = 0 );
	~transmitterPayloadDialog();

	bool loadValues(ScenarioTransmitterPayloadType*);
	bool saveValues(ScenarioTransmitterPayloadType*);


protected slots:
	void on_transmitterPayloadDialog_accepted();
	void on_transmitterPayloadDialog_rejected();
	
};
#endif //TRANSMITTERPAYLOADDIALOG_H
