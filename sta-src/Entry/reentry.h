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
 ------------------ Author: Annalisa Riccardi  -------------------------------------------------
 ------------------ E-mail: (nina1983@gmail.com) ----------------------------
 Modified by Tiziana Sabatini   May 2009
 */

#ifndef _REENTRY_H_
#define _REENTRY_H_

#include <QDialog>
#include "Loitering/loitering.h"
#include "ui_reentry.h"

class ScenarioTree;
class ScenarioEnvironment;
class ScenarioReEntryTrajectory;
class ScenarioProperties;
class ScenarioSimulationMode;
class ScenarioWindowMode;
class ScenarioTrajectoryPropagation;

class ReEntryDialog : public QDialog, private Ui_ReEntryDialogClass
{
   Q_OBJECT

public:
    ReEntryDialog(ScenarioTree* parent = NULL);
    ~ReEntryDialog();

    bool loadValues(ScenarioEntryArcType* reentry);
    bool loadValues(ScenarioTimeLine* timeLine);
    bool loadValues(ScenarioPropagationPositionType* propagation);
    bool loadValues(ScenarioInitialPositionType* initPosition, ScenarioEnvironmentType* environment);
    bool loadValues(ScenarioEnvironmentType* environment);
    bool loadValues(ScenarioREVConstraintsType* constraints);
/*
    bool loadValues(ScenarioProperties* vehicleproperties);
    bool loadValues(ScenarioSimulationMode* simulationmode);
    bool loadValues(ScenarioWindowMode* windowmode);
    bool loadValues(ScenarioTrajectoryPropagation* propagation);
*/

    bool saveValues(ScenarioPropagationPositionType* propagation);
    bool saveValues(ScenarioEntryArcType* reentry);
    bool saveValues(ScenarioEnvironmentType* environment);
    bool saveValues(ScenarioTimeLine* parameters);
    bool saveValues(ScenarioInitialPositionType* initPosition, ScenarioEnvironmentType* environment);
    bool saveValues(ScenarioREVConstraintsType* constraints);


    bool saveValues(ScenarioProperties* vehicleproperties);
    bool saveValues(ScenarioSimulationMode* simulationmode);
    bool saveValues(ScenarioWindowMode* windowmode);
    bool saveValues(ScenarioTrajectoryPropagation* propagation);


    double getLocalRadius(ScenarioEnvironmentType* environment, double latitude);

    TesseralBox* TesseralSpinBox;
    int m_tesserals;

};
extern bool PropagateEntryTrajectory(ScenarioREV* vehicle, ScenarioEntryArcType* entry, QList<double>& sampleTimes,
                              QList<sta::StateVector>& samples,
                              PropagationFeedback& propFeedback);

#endif // _REENTRY_H_


