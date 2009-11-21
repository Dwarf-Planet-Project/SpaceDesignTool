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

#include <QDialog>
#include "ui_loitering.h"

class ScenarioTree;
class ScenarioLoiteringTrajectory;
class ScenarioEnvironment;
class ScenarioSimulationParameters;
class ScenarioTrajectoryPropagation;
class TesseralBox;

class LoiteringDialog : public QDialog, private Ui_LoiteringDialogClass
{
    Q_OBJECT

public:
    LoiteringDialog(ScenarioTree* parent=NULL);
    ~LoiteringDialog();
    bool loadValues(ScenarioLoiteringTrajectory* loitering);
    bool loadValues(ScenarioEnvironment* environment);
    bool loadValues(ScenarioSimulationParameters* parameters);
    bool loadValues(ScenarioTrajectoryPropagation* propagation);
    bool saveValues(ScenarioLoiteringTrajectory* loitering);
    bool saveValues(ScenarioEnvironment* environment);
    bool saveValues(ScenarioSimulationParameters* parameters);
    bool saveValues(ScenarioTrajectoryPropagation* propagation);
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

#endif // _LOITERING_H_

