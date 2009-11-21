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
#include "Scenario/scenarioproperties.h"
#include "payloadproperties.h"

PayloadPropertiesDialog::PayloadPropertiesDialog(ScenarioTree* parent) :
    QDialog(parent)
{
    setupUi(this);

    QDoubleValidator* positiveDoubleValidator = new QDoubleValidator(this);
    positiveDoubleValidator->setBottom(0.0);

    lineEditMassPayload->setValidator(positiveDoubleValidator);
    lineEditPowerPayload->setValidator(positiveDoubleValidator);
}

bool PayloadPropertiesDialog::loadValues(ScenarioPayloadProperties* payloadProperties)
{

    lineEditMassPayload->setText(QString::number(payloadProperties->massPayload()));
    lineEditPowerPayload->setText(QString::number(payloadProperties->powerPayload()));

    return true;
}

bool PayloadPropertiesDialog::saveValues(ScenarioPayloadProperties* payloadProperties)
{
    payloadProperties->setMassPayload(lineEditMassPayload->text().toDouble());
    payloadProperties->setPowerPayload(lineEditPowerPayload->text().toDouble());

    return true;
}
