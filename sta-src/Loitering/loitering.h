/* 
 File name: loitering.h
 This program is free software; you can redistribute it and/or modify it under
 the terms of the GNU Lesser General Public License as published by the Free Software
 Foundation; either version 2 of the License, or (at your option) any later
 version.
 
 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more
 details.
 
 You should have received a copy of the GNU Lesser General Public License along with
 this program; if not, write to the Free Software Foundation, Inc., 59 Temple
 Place - Suite 330, Boston, MA 02111-1307, USA.
 Further information about the GNU Lesser General Public License can also be found on
 the world wide web at http://www.gnu.org. 
 */

/*
 --- Copyright (C) 2008 European Space Agency (space.trajectory.analysis AT gmail.com) ---
 ------------------ Author: Annalisa Riccardi  -------------------------------------------
 ------------------ E-mail: (nina1983@gmail.com) -----------------------------------------
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
    bool loadValues(ScenarioSCEnvironmentType* environment);
    bool loadValues(ScenarioTimeLine* parameters);
    bool loadValues(ScenarioPropagationPositionType* propagation);
    bool loadValues(ScenarioInitialPositionType* initPosition);
    bool saveValues(ScenarioLoiteringType* loitering);
    bool saveValues(ScenarioSCEnvironmentType* environment);
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

