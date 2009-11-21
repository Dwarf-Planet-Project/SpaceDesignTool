/*
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
 ------ Copyright (C) 2008 European Space Agency (space.trajectory.analysis AT gmail.com) ----
 ------------------ Author: Tiziana Sabatini  -------------------------------------------------
 ------------------ E-mail: (tiziana.sabatini@yahoo.it) ----------------------------
 */

#include "scenariotree.h"
#include "propulsionproperties.h"
#include "Scenario/scenarioproperties.h"

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
