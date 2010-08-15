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
------------------ Author: Guillermo Ortega  -------------------------------------------
*/

#ifndef MISSIONASPECTDIALOG_H
#define MISSIONASPECTDIALOG_H

#include "ui_missionAspectDialog.h"

#include <QDialog>

#include "Scenario/staschema.h"

class missionAspectDialog : public QDialog , private Ui::missionAspectDialog
{
    Q_OBJECT
public:
    missionAspectDialog( QWidget * parent = 0, Qt::WindowFlags f = 0);
    ~missionAspectDialog();

    bool loadValueArcName(const QString arcName);
    QString saveValueArcName();

    bool loadValueArcColor(const QString arcColor);
    QString saveValueArcColor();

    bool loadValueArcModel(const QString arcModel);
    QString saveValueArcModel();

    bool loadValueCentralBody(const QString centralBody);
    QString saveValueCentralBody();

	bool removePlanetsFromComboBoxForTLEs();

private:
    QString theCentralBody;
    QString theArcName;
    QString theArcColor;
    QString theArcModel;


	
};
#endif //MISSIONASPECTDIALOG_H
