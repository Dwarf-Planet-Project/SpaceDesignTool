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

#ifndef _STA_MANEUVERS_DELTAV_H_
#define _STA_MANEUVERS_DELTAV_H_

#include "ui_deltaVDialog.h"

#include "Scenario/scenario.h"
#include "Scenario/propagationfeedback.h"
#include "Astro-Core/statevector.h"
#include "Scenario/missionAspectDialog.h"

#include <QDialog>

class ScenarioTree;


class DeltaVDialog : public QDialog, private Ui_deltaVDialogClass
{
    Q_OBJECT

public:
    DeltaVDialog(ScenarioTree* parent = NULL);
    ~DeltaVDialog();

    bool loadValues(ScenarioDeltaVType* deltaV);
    bool loadValues(ScenarioElementIdentifierType* arcIdentifier);
    bool loadValues(ScenarioTimeLine* timeLine);

    bool saveValues(ScenarioDeltaVType* deltaV);
    bool saveValues(ScenarioElementIdentifierType* arcIdentifier);
    bool saveValues(ScenarioTimeLine* timeLine);

public:
    missionAspectDialog deltaVAspect;

protected slots:
        void on_pushButtonAspect_clicked();
        void on_endDateTimeEdit_dateTimeChanged(const QDateTime&);
        void on_durationLineEdit_textChanged(const QString&);
};

#endif // _STA_MANEUVERS_DELTAV_H_

