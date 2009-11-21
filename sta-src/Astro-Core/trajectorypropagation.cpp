/*

 This program is free software; you can redistribute it and/or modify it under
 the terms of the GNU General Public License as published by the Free Software
 Foundation; either version 2 of the License, or (at your option) any later
 version.

 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE. See the GNU General Public License for more
 details.

 You should have received a copy of the GNU General Public License along with
 this program; if not, write to the Free Software Foundation, Inc., 59 Temple
 Place - Suite 330, Boston, MA 02111-1307, USA.
 Further information about the GNU General Public License can also be found on
 the world wide web at http://www.gnu.org.
 */


/*
 --- Copyright (C) 2009 STA Steering Board (space.trajectory.analysis AT gmail.com) ---
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
