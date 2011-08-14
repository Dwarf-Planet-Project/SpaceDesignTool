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
 ------ Copyright (C) 2011 STA Steering Board (space.trajectory.analysis AT gmail.com) ----
*/
/*
------------------ Author: Guillermo Ortega  -------------------------------------------
 */

#ifndef PERTURBATIONFORCESDIALOG_H
#define PERTURBATIONFORCESDIALOG_H

#include "ui_perturbationForces.h"

#include "Scenario/scenario.h"

#include <QDialog>

class TesseralBox;

class perturbationForcesDialog : public QDialog , private Ui::perturbationForcesDialog
{
Q_OBJECT
public:
	perturbationForcesDialog( QWidget * parent = 0, Qt::WindowFlags f = 0 );
	~perturbationForcesDialog();


public:
    bool loadValues(const ScenarioCentralBodyType* centralBody, const ScenarioPerturbationsForceType* perturbationsToCentralBody);
    bool saveValues(ScenarioCentralBodyType* centralBody, ScenarioPerturbationsForceType* perturbationsToCentralBody);

    TesseralBox* TesseralSpinBox;
    int m_tesserals;

public slots:
    void addPerturbingPlanet(QListWidgetItem* item);
    void addPerturbingPlanet();
    void removePerturbingPlanet();
    void setTesserals(int i);
	
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

#endif //PERTURBATIONFORCESDIALOG_H
