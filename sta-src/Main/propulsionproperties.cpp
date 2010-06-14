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
------------------ Author: Tiziana Sabatini  -------------------------------------------------
 ------------------ E-mail: (tiziana.sabatini@yahoo.it) ----------------------------
 */

#include "scenariotree.h"
#include "propulsionproperties.h"
#include "Scenario/scenario.h"

#if OLDSCENARIO
PropulsionPropertiesDialog::PropulsionPropertiesDialog(ScenarioTree* parent) :
    QDialog(parent)
{
    setupUi(this);

    comboBoxTypePropulsion->addItem(tr("Cold gas"));
    comboBoxTypePropulsion->addItem(tr("Chemical"));
    comboBoxTypePropulsion->addItem(tr("Solid"));
    comboBoxTypePropulsion->addItem(tr("Electrical"));
    comboBoxTypePropulsion->addItem(tr("Nuclear"));

    QDoubleValidator* doubleValidator = new QDoubleValidator(this);
    QDoubleValidator* positiveDoubleValidator = new QDoubleValidator(this);
    positiveDoubleValidator->setBottom(0.0);
    QIntValidator* intValidator = new QIntValidator(this);

    lineEditNEngine->setValidator(intValidator);
    lineEditThrust->setValidator(doubleValidator);
    lineEditIsp->setValidator(doubleValidator);
    lineEditPropellant->setValidator(positiveDoubleValidator);
    lineEditDV->setValidator(positiveDoubleValidator);
}

bool PropulsionPropertiesDialog::loadValues(ScenarioPropulsionProperties* propulsionProperties)
{
    for(int i=0; i<comboBoxTypePropulsion->count(); i++)
    {
        if(comboBoxTypePropulsion->itemText(i) == propulsionProperties->type())
        {
            comboBoxTypePropulsion->setCurrentIndex(i);
            break;
        }
    }
    lineEditNEngine->setText(QString::number(propulsionProperties->engineCount()));
    lineEditThrust->setText(QString::number(propulsionProperties->thrustPerEngine()));
    lineEditIsp->setText(QString::number(propulsionProperties->specificImpulse()));
    lineEditPropellant->setText(QString::number(propulsionProperties->propellantMass()));
    lineEditDV->setText(QString::number(propulsionProperties->dV()));

    return true;
}

bool PropulsionPropertiesDialog::saveValues(ScenarioPropulsionProperties* propulsionProperties)
{
    propulsionProperties->setEngineCount(lineEditNEngine->text().toUInt());
    propulsionProperties->setPropellantMass(lineEditPropellant->text().toDouble());
    propulsionProperties->setSpecificImpulse(lineEditIsp->text().toDouble());
    propulsionProperties->setThrustPerEngine(lineEditThrust->text().toDouble());
    propulsionProperties->setType(comboBoxTypePropulsion->currentText());
    propulsionProperties->setDV(lineEditDV->text().toDouble());

    return true;
}
#endif // OLDSCENARIO

