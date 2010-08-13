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






bool missionAspectDialog::loadValues(const ScenarioElementIdentifierType identifier)
{

	// The arc name
    qDebug() << identifier.elementName() << endl;
	lineEditArcName->setText(identifier.elementName());

	// The arc color
	if (identifier.colorName() == "Yellow")
		comboBoxColorPicker->setCurrentIndex(0);
	else if (identifier.colorName() == "Red")
        comboBoxColorPicker->setCurrentIndex(1);
	else if (identifier.colorName() == "Green")
        comboBoxColorPicker->setCurrentIndex(2);
	else if (identifier.colorName() == "Blue")
        comboBoxColorPicker->setCurrentIndex(3);
	else if (identifier.colorName() == "Magenta")
        comboBoxColorPicker->setCurrentIndex(4);
	else if (identifier.colorName() == "White")
        comboBoxColorPicker->setCurrentIndex(5);
	else if (identifier.colorName() == "Cyan")
        comboBoxColorPicker->setCurrentIndex(6);
	else if (identifier.colorName() == "Gray")
        comboBoxColorPicker->setCurrentIndex(7);
	else if (identifier.colorName() == "Dark Red")
        comboBoxColorPicker->setCurrentIndex(8);
	else if (identifier.colorName() == "Dark Yellow")
        comboBoxColorPicker->setCurrentIndex(9);
	else if (identifier.colorName() == "Dark Blue")
        comboBoxColorPicker->setCurrentIndex(10);
	else if (identifier.colorName() == "Dark Magenta")
        comboBoxColorPicker->setCurrentIndex(11);
	else if (identifier.colorName() == "Light Gray")
        comboBoxColorPicker->setCurrentIndex(12);
	else if (identifier.colorName() == "Drak Gray")
        comboBoxColorPicker->setCurrentIndex(13);
	else if (identifier.colorName() == "Dark Cyan")
        comboBoxColorPicker->setCurrentIndex(14);
	else if (identifier.colorName() == "Black")
        comboBoxColorPicker->setCurrentIndex(15);

    // The model name
    if (identifier.modelName() == "Default")
        comboBoxModel->setCurrentIndex(0);
    else if (identifier.modelName() == "XMM")
        comboBoxModel->setCurrentIndex(1);
    else if (identifier.modelName() == "ISS")
        comboBoxModel->setCurrentIndex(2);
}


bool missionAspectDialog::loadValueCentralBody(const QString centralBody)
{
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



ScenarioElementIdentifierType missionAspectDialog::saveValues()
{
    ScenarioElementIdentifierType miIdentifier;   
    // the arc name
    miIdentifier.elementName() = lineEditArcName->text();   
    // the color name
    miIdentifier.colorName() = comboBoxColorPicker->currentText();   
    // The model name
    miIdentifier.modelName() = comboBoxModel->currentText();   
    return miIdentifier;
}




QString missionAspectDialog::saveValueCentralBody()
{
    theCentralBody = comboBoxCentralBody->currentText();
    return theCentralBody;
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

