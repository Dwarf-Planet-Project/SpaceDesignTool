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
 ------------------ Author: Ricardo Noriega  ----------------------------------------------
 ------------------ email: ricardonor@gmail.com  ------------------------------------------

 */

#include "environmentdialog.h"

environmentDialog::environmentDialog( QWidget * parent, Qt::WindowFlags f) : QDialog(parent,f)
{
    setupUi(this);

    QDoubleValidator* positiveDoubleValidator = new QDoubleValidator(this);
    positiveDoubleValidator->setBottom(0.0);
        
    OutageLineEdit->setValidator(positiveDoubleValidator);
        

}

environmentDialog::~environmentDialog()
{
}
///////////////////////// VARIABLES TO SAVE THE CHECKBOXES CHOICE//////////////////////////




bool environmentDialog::loadValues(ScenarioGroundStationEnvironment* environment)
{
    OutageLineEdit->setText(QString::number(environment->Rain()->PercentageExceededLimit()));
    if(environment->OxChoice()=="true")
        oxygenCheckBox->setChecked(1);
    if(environment->OxChoice()=="false")
        oxygenCheckBox->setChecked(0);

    if(environment->WaterVapourChoice()=="true")
        waterVapourCheckBox->setChecked(1);
    if(environment->WaterVapourChoice()=="false")
        waterVapourCheckBox->setChecked(0);

    if(environment->Rain()->RainChoice()=="true")
        groupBox->setChecked(1);
    if(environment->Rain()->RainChoice()=="false")
        groupBox->setChecked(0);





    return true;
}

bool environmentDialog::saveValues(ScenarioGroundStationEnvironment* environment)
{
    environment->Rain()->setPercentageExceededLimit(OutageLineEdit->text().toDouble());

    if(oxygenCheckBox->isChecked()==true)
        environment->setOxChoice("true");
    if(oxygenCheckBox->isChecked()==false)
        environment->setOxChoice("false");

    if(waterVapourCheckBox->isChecked()==true)
        environment->setWaterVapourChoice("true");
    if(waterVapourCheckBox->isChecked()==false)
        environment->setWaterVapourChoice("false");

    if(groupBox->isChecked()==true)
        environment->Rain()->setRainChoice("true");
    if(groupBox->isChecked()==false)
        environment->Rain()->setRainChoice("false");




    return true;
}


