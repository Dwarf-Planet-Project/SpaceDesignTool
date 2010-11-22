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
------------------ Author: Chris Laurel  -------------------------------------------------
 ------------------ E-mail: (claurel@gmail.com) ----------------------------
  Modified by Guillermo on October 2009 to clean up the interface
 */

#include <QDialog>
#include "ui_locationeditor.h"
#include "Locations/environmentdialog.h"
#include "Coverage/commanalysis.h"
#include "Scenario/missionAspectDialog.h"

class ScenarioLocationType;

class LocationEditorDialog : public QDialog, private Ui_LocationEditor
{
    Q_OBJECT
    
public:
    LocationEditorDialog(QWidget* parent = NULL);
    ~LocationEditorDialog();
    
    bool loadValues(const ScenarioLocationType* location);
    bool saveValues(ScenarioLocationType* location);

    bool loadValues(ScenarioElementIdentifierType* arcIdentifier);
    bool saveValues(ScenarioElementIdentifierType* arcIdentifier);


public:
    missionAspectDialog locationAspect;

protected slots:
        void on_pushButtonAspect_clicked();


};
