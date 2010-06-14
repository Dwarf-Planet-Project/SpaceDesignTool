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

#include "payloadproperties.h"
#include "scenariotree.h"
#include "Scenario/scenario.h"


PayloadPropertiesDialog::PayloadPropertiesDialog(ScenarioTree* parent) :
    QDialog(parent)
{
    setupUi(this);

    QDoubleValidator* positiveDoubleValidator = new QDoubleValidator(this);
    positiveDoubleValidator->setBottom(0.0);

    lineEditMassPayload->setValidator(positiveDoubleValidator);
    lineEditPowerPayload->setValidator(positiveDoubleValidator);
}

bool PayloadPropertiesDialog::loadValues(ScenarioPayload* payload)
{
    //lineEditMassPayload->setText(QString::number(payload->mass()));
    //lineEditPowerPayload->setText(QString::number(payload->power()));

    return true;
}

bool PayloadPropertiesDialog::saveValues(ScenarioPayload* payload)
{
   // payload->setMass(lineEditMassPayload->text().toDouble());
    //payload->setPower(lineEditPowerPayload->text().toDouble());

    return true;
}
