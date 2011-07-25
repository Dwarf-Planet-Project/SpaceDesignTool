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
 Modified by Tiziana Sabatini on July 2009 to support the new perturbations layer
 Patched by Catarina to add attitude, July 2011
 */

#ifndef _LOITERING_H_
#define _LOITERING_H_

#include "ui_loitering.h"

#include "Scenario/scenario.h"
#include "Scenario/propagationfeedback.h"
#include "Scenario/missionAspectDialog.h"

#include "Astro-Core/statevector.h"
#include "Astro-Core/attitudevector.h"

#include "Services/perturbationForcesDialog.h"
#include "Services/stoppingConditionsTrajectoryDialog.h"

#include <QDialog>

class ScenarioTree;
//class TesseralBox;


class LoiteringDialog : public QDialog, private Ui_LoiteringDialogClass
{
    Q_OBJECT

public:
    LoiteringDialog(ScenarioTree* parent = NULL);
    ~LoiteringDialog();

    bool loadValues(ScenarioLoiteringType* loitering);
    bool loadValues(ScenarioElementIdentifierType* arcIdentifier);
    bool loadValues(ScenarioEnvironmentType* environment);
    bool loadValues(ScenarioTimeLine* parameters);

    bool loadValues(ScenarioInitialPositionType* initPosition);
    bool loadValues(ScenarioPropagationPositionType* propagation);

    bool loadValues(ScenarioInitialAttitudeType* initAtt);
    bool loadValues(ScenarioInitialAttitudeUsingQuaternionsType* initAttQuaternions);
    bool loadValues(ScenarioPropagationAttitudeType* propagation);

    bool saveValues(ScenarioLoiteringType* loitering);
    bool saveValues(ScenarioElementIdentifierType* arcIdentifier);
    bool saveValues(ScenarioEnvironmentType* environment);
    bool saveValues(ScenarioTimeLine* parameters);
    bool saveValues(ScenarioPropagationPositionType* propagation);

    bool saveValues(ScenarioInitialPositionType* initPosition);
    bool saveValues(ScenarioPropagationAttitudeType* propagation);

    bool saveValues(ScenarioInitialAttitudeType* initAtt);
    bool saveValues(ScenarioInitialAttitudeUsingQuaternionsType* initAttQuaternions);

public:
    missionAspectDialog loiteringAspect;
    perturbationForcesDialog myPerturbationsForces;
    //perturbationTorquesDialog myPerturbationsTorques;  // To be finihsed by Catarina
    stoppingConditionsTrajectoryDialog myStoppingConditionsTrajetory;
    //stoppingConditionsAttitudeDialog myStoppingConditionsAttitude; // To be finihsed by Catarina


public slots:
    void disableIntegratorComboBox(int i);

protected slots:
    void on_pushButtonAspect_clicked();
    void on_perturbationForcesPushButton_clicked();
    void on_perturbationTorquesPushButton_clicked();
    void on_trajectoryStopConditionsPushButton_clicked();
    void on_attitudeStopConditionsPushButton_clicked();


};



extern bool
        PropagateLoiteringTrajectory(ScenarioLoiteringType* loitering,
                                     QList<double>& sampleTimes,
                                     QList<sta::StateVector>& samples,
                                     PropagationFeedback& propFeedback);

extern bool
        PropagateLoiteringAttitude(ScenarioLoiteringType* loitering,
                                   QList<double>& sampleTimes,
                                   QList<staAttitude::AttitudeVector>& samplesAtt,
                                   PropagationFeedback& propFeedback);

#endif // _LOITERING_H_

