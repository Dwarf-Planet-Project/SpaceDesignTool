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
------------------ Author: Guillermo Ortega  -------------------------------------------
*/

#include "Scenario/missionAspectDialog.h"
#include "Astro-Core/stabody.h"

#include <QDebug>

missionAspectDialog::missionAspectDialog( QWidget * parent, Qt::WindowFlags f) : QDialog(parent,f)
{
	setupUi(this);
}

missionAspectDialog::~missionAspectDialog()
{
}




bool missionAspectDialog::loadValueArcName(const QString arcName)
{
    lineEditArcName->setText(arcName);
    theArcName = lineEditArcName->text();
}



bool missionAspectDialog::loadValueArcColor(const QString arcColor)
{
    if (arcColor == "Yellow")
        comboBoxColorPicker->setCurrentIndex(0);
    else if (arcColor == "Red")
        comboBoxColorPicker->setCurrentIndex(1);
    else if (arcColor == "Green")
        comboBoxColorPicker->setCurrentIndex(2);
    else if (arcColor == "Blue")
        comboBoxColorPicker->setCurrentIndex(3);
    else if (arcColor == "Magenta")
        comboBoxColorPicker->setCurrentIndex(4);
    else if (arcColor == "White")
        comboBoxColorPicker->setCurrentIndex(5);
    else if (arcColor == "Cyan")
        comboBoxColorPicker->setCurrentIndex(6);
    else if (arcColor == "Gray")
        comboBoxColorPicker->setCurrentIndex(7);
    else if (arcColor == "Dark Red")
        comboBoxColorPicker->setCurrentIndex(8);
    else if (arcColor == "Dark Yellow")
        comboBoxColorPicker->setCurrentIndex(9);
    else if (arcColor == "Dark Blue")
        comboBoxColorPicker->setCurrentIndex(10);
    else if (arcColor == "Dark Magenta")
        comboBoxColorPicker->setCurrentIndex(11);
    else if (arcColor == "Light Gray")
        comboBoxColorPicker->setCurrentIndex(12);
    else if (arcColor == "Drak Gray")
        comboBoxColorPicker->setCurrentIndex(13);
    else if (arcColor == "Dark Cyan")
        comboBoxColorPicker->setCurrentIndex(14);
    else if (arcColor == "Black")
        comboBoxColorPicker->setCurrentIndex(15);
    else
        return false;

    theArcColor = arcColor;
    return true;
}




bool missionAspectDialog::loadValueArcModel(const QString arcModel)
{
    if (arcModel == "Default")
        comboBoxModel->setCurrentIndex(0);
    else if (arcModel == "XMM")
        comboBoxModel->setCurrentIndex(1);
    else if (arcModel == "ISS")
        comboBoxModel->setCurrentIndex(2);
    else if (arcModel == "Rosetta")
        comboBoxModel->setCurrentIndex(3);
    else
        return false;

    theArcModel = arcModel;
    return true;
}




bool missionAspectDialog::loadValueCentralBody(const QString centralBody)
{
    // Moons of the solar system at http://www.windows.ucar.edu/tour/link=/our_solar_system/moons_table.html
    if (centralBody == "Earth")
        comboBoxCentralBody->setCurrentIndex(0);
    else if (centralBody == "Moon")
        comboBoxCentralBody->setCurrentIndex(1);
    else if (centralBody == "Mercury")
        comboBoxCentralBody->setCurrentIndex(2);
    else if (centralBody == "Venus")
        comboBoxCentralBody->setCurrentIndex(3);
    else if (centralBody == "Mars")
        comboBoxCentralBody->setCurrentIndex(4);
    else if (centralBody == "Jupiter")
        comboBoxCentralBody->setCurrentIndex(5);
    else if (centralBody == "Saturn")
        comboBoxCentralBody->setCurrentIndex(6);
    else if (centralBody == "Uranus")
        comboBoxCentralBody->setCurrentIndex(7);
    else if (centralBody == "Neptune")
        comboBoxCentralBody->setCurrentIndex(8);
    else if (centralBody == "Pluto")
        comboBoxCentralBody->setCurrentIndex(9);
    else if (centralBody == "Sun")
        comboBoxCentralBody->setCurrentIndex(10);
    else if (centralBody == "Titan")
        comboBoxCentralBody->setCurrentIndex(11);
    else if (centralBody == "Europa")
        comboBoxCentralBody->setCurrentIndex(12);
    else if (centralBody == "Triton")
        comboBoxCentralBody->setCurrentIndex(13);
    else if (centralBody == "Enceladus")
        comboBoxCentralBody->setCurrentIndex(14);
    else
        return false;

    theCentralBody = centralBody;
    return true;
}




QString missionAspectDialog::saveValueCentralBody()
{
    theCentralBody = comboBoxCentralBody->currentText();
    return theCentralBody;
}


QString missionAspectDialog::saveValueArcName()
{
    theArcName = lineEditArcName->text();
    return theArcName;
}



QString missionAspectDialog::saveValueArcColor()
{
    theArcColor = comboBoxColorPicker->currentText();
    return theArcColor;
}


QString missionAspectDialog::saveValueArcModel()
{
    theArcModel = comboBoxModel->currentText();
    return theArcModel;
}



bool missionAspectDialog::removePlanetsFromComboBoxForTLEs()
{
	// Just left the Earth
	comboBoxCentralBody->removeItem(1);
	comboBoxCentralBody->removeItem(2);
	comboBoxCentralBody->removeItem(3);
	comboBoxCentralBody->removeItem(4);
	comboBoxCentralBody->removeItem(5);
	comboBoxCentralBody->removeItem(6);
	comboBoxCentralBody->removeItem(7);
	comboBoxCentralBody->removeItem(8);
	comboBoxCentralBody->removeItem(9);
	comboBoxCentralBody->removeItem(10);
	comboBoxCentralBody->removeItem(11);
	comboBoxCentralBody->removeItem(12);
	comboBoxCentralBody->removeItem(13);
	comboBoxCentralBody->removeItem(14);
	return true;
}



/*
void missionAspectDialog::on_missionAspectDialog_rejected()
{
    theCentralBody = comboBoxCentralBody->currentText();
}

void missionAspectDialog::on_missionAspectDialog_accepted()
{
    theCentralBody = comboBoxCentralBody->currentText();
}

*/

