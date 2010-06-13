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
 */

#include "trajectorypropagation.h"


TrajectoryPropagationDialog::TrajectoryPropagationDialog(QWidget* parent) :
    QDialog(parent)
{
    setupUi(this);
    
    propagatorCombo->addItem(tr("Two body"), "TWO BODY");
    propagatorCombo->addItem(tr("J2"), "J2");
    propagatorCombo->addItem(tr("J4"), "J4");

    integratorCombo->addItem(tr("Runge-Kutta 4"), "RK4");
    integratorCombo->addItem(tr("Runge-Kutta 4-5"), "RK4-5");
}


TrajectoryPropagationDialog::~TrajectoryPropagationDialog()
{
}


#if OLDSCENARIO
bool TrajectoryPropagationDialog::loadValues(ScenarioTrajectoryPropagation* prop)
{
    QString currentPropagator = prop->propagator();
    for (int i = 0; i < propagatorCombo->count(); i++)
    {
        if (propagatorCombo->itemData(i) == currentPropagator)
        {
            propagatorCombo->setCurrentIndex(i);
            break;
        }
    }

    QString currentIntegrator = prop->integrator();
    for (int i = 0; i < integratorCombo->count(); i++)
    {
        if (integratorCombo->itemData(i) == currentIntegrator)
        {
            integratorCombo->setCurrentIndex(i);
            break;
        }
    }
    
    timeStepSpinBox->setValue((int) prop->timeStep());
    return true;
}


bool TrajectoryPropagationDialog::saveValues(ScenarioTrajectoryPropagation* prop)
{
    prop->setIntegrator(integratorCombo->itemData(integratorCombo->currentIndex()).toString());
    prop->setPropagator(propagatorCombo->itemData(propagatorCombo->currentIndex()).toString());
    prop->setTimeStep(timeStepSpinBox->value());

    return true;
}
#endif
