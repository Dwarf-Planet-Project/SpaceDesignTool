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
 */

#ifndef _LOITERING_H_
#define _LOITERING_H_

#include "ui_loitering.h"
#include "Scenario/scenario.h"
#include "Scenario/propagationfeedback.h"
#include "Astro-Core/statevector.h"
#include <QDialog>

class ScenarioTree;
class TesseralBox;


class LoiteringDialog : public QDialog, private Ui_LoiteringDialogClass
{
    Q_OBJECT

public:
    LoiteringDialog(ScenarioTree* parent = NULL);
    ~LoiteringDialog();
    bool loadValues(ScenarioLoiteringType* loitering);
    bool loadValues(ScenarioEnvironmentType* environment);
    bool loadValues(ScenarioTimeLine* parameters);
    bool loadValues(ScenarioPropagationPositionType* propagation);
    bool loadValues(ScenarioInitialPositionType* initPosition);
    bool saveValues(ScenarioLoiteringType* loitering);
    bool saveValues(ScenarioEnvironmentType* environment);
    bool saveValues(ScenarioTimeLine* parameters);
    bool saveValues(ScenarioPropagationPositionType* propagation);
    bool saveValues(ScenarioInitialPositionType* initPosition);

    TesseralBox* TesseralSpinBox;
    int m_tesserals;

public slots:
    void addPerturbingPlanet(QListWidgetItem* item);
    void addPerturbingPlanet();
    void removePerturbingPlanet();
    void setTesserals(int i);
    void disableIntegratorComboBox(int i);
};

/**
 * Class to create a custom spinBox object; this class has been created for the tesserals spinBox
 * whose maximum value is function of the zonals spinBox value
 */
class TesseralBox : public QSpinBox
{
    Q_OBJECT

    public:
    TesseralBox(QDialog* parent);

    public slots:

    /**
     * Slot to define the maximum value of the tesseralSpinBox as function of i
     */
    void setVariableMaximum(int i);

};

extern bool
PropagateLoiteringTrajectory(ScenarioLoiteringType* loitering,
                             QList<double>& sampleTimes,
                             QList<sta::StateVector>& samples,
                             PropagationFeedback& propFeedback);

#endif // _LOITERING_H_

