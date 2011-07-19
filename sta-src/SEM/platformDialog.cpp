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

#include "SEM/platformDialog.h"

#include "Main/scenariotree.h"
#include "Scenario/scenario.h"

#include <QDebug>

//PlatformDialog::PlatformDialog( QWidget * parent, Qt::WindowFlags f) : QDialog(parent,f)
PlatformDialog::PlatformDialog(ScenarioTree* parent):
        QDialog(parent)
{
	setupUi(this);

    // Set up the input validators
    QDoubleValidator* doubleValidator = new QDoubleValidator(this);
    QDoubleValidator* angleValidator = new QDoubleValidator(this);
    angleValidator->setBottom(0.0);
    angleValidator->setTop(360.0);
    QDoubleValidator* positiveDoubleValidator = new QDoubleValidator(this);
    positiveDoubleValidator->setBottom(0.0);
    QDoubleValidator* zeroToOneValidator = new QDoubleValidator(this);
    zeroToOneValidator->setBottom(0.0);
    zeroToOneValidator->setTop(0.9999);
    QDoubleValidator* minusOneToOneValidator = new QDoubleValidator(this);
    minusOneToOneValidator->setBottom(-1.0);
    minusOneToOneValidator->setTop(1.0);
}

PlatformDialog::~PlatformDialog()
{
}


bool PlatformDialog::loadValues(ScenarioSCSystemType* spacecraftPlatformSystem)
{
    //  ----------- Structures ---------------
    DryMass->setText(QString::number(spacecraftPlatformSystem->SystemBudgets()->MassOfSystem()->dryMass()));
    WetMass->setText(QString::number(spacecraftPlatformSystem->SystemBudgets()->MassOfSystem()->wetMass()));
    TotalMass->setText(QString::number(spacecraftPlatformSystem->SystemBudgets()->MassOfSystem()->dryMass() + spacecraftPlatformSystem->SystemBudgets()->MassOfSystem()->wetMass()));
    Ixx->setText(QString::number(spacecraftPlatformSystem->Structure()->MomentsOfInertia()->xAxis()));
    Iyy->setText(QString::number(spacecraftPlatformSystem->Structure()->MomentsOfInertia()->yAxis()));
    Izz->setText(QString::number(spacecraftPlatformSystem->Structure()->MomentsOfInertia()->zAxis()));
    Ixy->setText(QString::number(spacecraftPlatformSystem->Structure()->SecondMomentsOfArea()->xAxis()));
    Ixz->setText(QString::number(spacecraftPlatformSystem->Structure()->SecondMomentsOfArea()->yAxis()));
    Iyz->setText(QString::number(spacecraftPlatformSystem->Structure()->SecondMomentsOfArea()->zAxis()));
    Iyx->setText(QString::number(spacecraftPlatformSystem->Structure()->SecondMomentsOfArea()->xAxis()));
    Izx->setText(QString::number(spacecraftPlatformSystem->Structure()->SecondMomentsOfArea()->yAxis()));
    Izy->setText(QString::number(spacecraftPlatformSystem->Structure()->SecondMomentsOfArea()->zAxis()));
    Width->setText(QString::number(spacecraftPlatformSystem->Structure()->Sizing()->width()));
    Length->setText(QString::number(spacecraftPlatformSystem->Structure()->Sizing()->length()));
    Height->setText(QString::number(spacecraftPlatformSystem->Structure()->Sizing()->height()));
    Volume->setText(QString::number(spacecraftPlatformSystem->Structure()->Sizing()->volume()));
    AxialFrequency->setText(QString::number(spacecraftPlatformSystem->Structure()->NaturalFrequency()->axialFrequency()));
    LateralFrequency->setText(QString::number(spacecraftPlatformSystem->Structure()->NaturalFrequency()->lateralFrequency()));
    materialComboBox->setCurrentText(spacecraftPlatformSystem->Structure()->StructuralMaterial());
    shapeComboBox->setCurrentText(spacecraftPlatformSystem->Structure()->StructuralShape());

    //  ----------- Propulsion ---------------
    numberOfEngines->setText(QString::number(spacecraftPlatformSystem->Propulsion()->numberOfEngines()));
    thrustPerEngine->setText(QString::number(spacecraftPlatformSystem->Propulsion()->thrustPerEngine()));
    specificImpulse->setText(QString::number(spacecraftPlatformSystem->Propulsion()->specificImpulse()));
    propellantMass->setText(QString::number(spacecraftPlatformSystem->Propulsion()->propellantMass()));
    propulsionTotalDryMass->setText(QString::number(spacecraftPlatformSystem->Propulsion()->totalPropulsionDryMass()));
    //propulsionTypeComboBox->setCurrentText(spacecraftPlatformSystem->Propulsion()->

    //  ----------- Thermal ---------------
    ThermalGUISCMinTemperatureHeaterRadiatorLineEdit_3->setText(QString::number(spacecraftPlatformSystem->TCS()->Temperature()->minimumSCTemperature()));
    ThermalGUISCMaxTemperatureHeaterRadiatorLineEdit_3->setText(QString::number(spacecraftPlatformSystem->TCS()->Temperature()->maximumSCTemperature()));
    ThermalGUISCMinTemperatureLineEdit_3->setText(QString::number(spacecraftPlatformSystem->TCS()->Temperature()->minimumSCTemperature()));
    ThermalGUISCMaxTemperatureLineEdit_3->setText(QString::number(spacecraftPlatformSystem->TCS()->Temperature()->maximumSCTemperature()));
    ThermalGUIHeaterPowerLineEdit->setText(QString::number(spacecraftPlatformSystem->TCS()->heaterPower()));
    ThermalGUIRadiatorLineEdit->setText(QString::number(spacecraftPlatformSystem->TCS()->radiatedPower()));
    ThermalGUIColdCoatingComboBox_2->setCurrentText(spacecraftPlatformSystem->TCS()->CoatingArea()->ColdSurface()->ColdCoating()->ElementIdentifier()->elementName());
    qDebug() << ThermalGUIColdCoatingComboBox_2->currentText() << endl;
    ThermalGUIColdEmmissivityLineEdit_2->setText(QString::number(spacecraftPlatformSystem->TCS()->CoatingArea()->ColdSurface()->ColdCoating()->emissivity()));
    ThermalGUIColdAbsorptivityLineEdit_2->setText(QString::number(spacecraftPlatformSystem->TCS()->CoatingArea()->ColdSurface()->ColdCoating()->absorptivity()));
    ThermalGUIHotCoatingComboBox_2->setCurrentText(spacecraftPlatformSystem->TCS()->CoatingArea()->HotSurface()->HotCoating()->ElementIdentifier()->elementName());
    ThermalGUIHotEmmissivityLineEdit_2->setText(QString::number(spacecraftPlatformSystem->TCS()->CoatingArea()->HotSurface()->HotCoating()->emissivity()));
    ThermalGUIHotAbsorptivityLineEdit_2->setText(QString::number(spacecraftPlatformSystem->TCS()->CoatingArea()->HotSurface()->HotCoating()->absorptivity()));

    //  ----------- Power ---------------


    //  ----------- Aerodynamics ---------------


    //  ----------- TTC ---------------


    //  ----------- GNC ---------------


    //  ----------- Data handling ---------------


    return true;

}


bool PlatformDialog::saveValues(ScenarioSCSystemType* spacecraftPlatformSystem)
{
    //  ----------- Structures ---------------
    spacecraftPlatformSystem->SystemBudgets()->MassOfSystem()->setDryMass(DryMass->text().toDouble());
    spacecraftPlatformSystem->SystemBudgets()->MassOfSystem()->setWetMass(WetMass->text().toDouble());
    spacecraftPlatformSystem->Structure()->MomentsOfInertia()->setXAxis(Ixx->text().toDouble());
    spacecraftPlatformSystem->Structure()->MomentsOfInertia()->setYAxis(Iyy->text().toDouble());
    spacecraftPlatformSystem->Structure()->MomentsOfInertia()->setZAxis(Izz->text().toDouble());
    spacecraftPlatformSystem->Structure()->SecondMomentsOfArea()->setXAxis(Ixy->text().toDouble());
    spacecraftPlatformSystem->Structure()->SecondMomentsOfArea()->setYAxis(Ixz->text().toDouble());
    spacecraftPlatformSystem->Structure()->SecondMomentsOfArea()->setZAxis(Iyz->text().toDouble());
    //spacecraftPlatformSystem->Structure()->SecondMomentsOfArea()->setXAxis(Iyx->text().toDouble());
    //spacecraftPlatformSystem->Structure()->SecondMomentsOfArea()->setYAxis(Izx->text().toDouble());
    //spacecraftPlatformSystem->Structure()->SecondMomentsOfArea()->setZAxis(Izy->text().toDouble());
    spacecraftPlatformSystem->Structure()->Sizing()->setHeight(Width->text().toDouble());
    spacecraftPlatformSystem->Structure()->Sizing()->setLength(Length->text().toDouble());
    spacecraftPlatformSystem->Structure()->Sizing()->setHeight(Height->text().toDouble());
    spacecraftPlatformSystem->Structure()->Sizing()->setVolume(Volume->text().toDouble());
    spacecraftPlatformSystem->Structure()->NaturalFrequency()->setAxialFrequency(AxialFrequency->text().toDouble());
    spacecraftPlatformSystem->Structure()->NaturalFrequency()->setLateralFrequency(LateralFrequency->text().toDouble());
    spacecraftPlatformSystem->Structure()->setStructuralMaterial(materialComboBox->currentText());
    spacecraftPlatformSystem->Structure()->setStructuralShape(shapeComboBox->currentText());

    //  ----------- Propulsion ---------------
    spacecraftPlatformSystem->Propulsion()->setNumberOfEngines(numberOfEngines->text().toInt());
    spacecraftPlatformSystem->Propulsion()->setPropellantMass(propellantMass->text().toDouble());
    spacecraftPlatformSystem->Propulsion()->setSpecificImpulse(specificImpulse->text().toDouble());
    spacecraftPlatformSystem->Propulsion()->setThrustPerEngine(thrustPerEngine->text().toDouble());
    spacecraftPlatformSystem->Propulsion()->setTotalPropulsionDryMass(propulsionTotalDryMass->text().toDouble());

    //  ----------- Thermal ---------------
    spacecraftPlatformSystem->TCS()->Temperature()->setMinimumSCTemperature(ThermalGUISCMinTemperatureHeaterRadiatorLineEdit_3->text().toDouble());
    spacecraftPlatformSystem->TCS()->Temperature()->setMaximumSCTemperature(ThermalGUISCMaxTemperatureHeaterRadiatorLineEdit_3->text().toDouble());
    spacecraftPlatformSystem->TCS()->Temperature()->setMinimumSCTemperature(ThermalGUISCMinTemperatureLineEdit_3->text().toDouble());
    spacecraftPlatformSystem->TCS()->Temperature()->setMaximumSCTemperature(ThermalGUISCMaxTemperatureLineEdit_3->text().toDouble());
    spacecraftPlatformSystem->TCS()->setHeaterPower(ThermalGUIHeaterPowerLineEdit->text().toDouble());
    spacecraftPlatformSystem->TCS()->setRadiatedPower(ThermalGUIRadiatorLineEdit->text().toDouble());
    spacecraftPlatformSystem->TCS()->CoatingArea()->ColdSurface()->ColdCoating()->ElementIdentifier()->setName(ThermalGUIColdCoatingComboBox_2->currentText());
    spacecraftPlatformSystem->TCS()->CoatingArea()->ColdSurface()->ColdCoating()->setEmissivity(ThermalGUIColdEmmissivityLineEdit_2->text().toDouble());
    spacecraftPlatformSystem->TCS()->CoatingArea()->ColdSurface()->ColdCoating()->setAbsorptivity(ThermalGUIColdAbsorptivityLineEdit_2->text().toDouble());
    spacecraftPlatformSystem->TCS()->CoatingArea()->HotSurface()->HotCoating()->ElementIdentifier()->setName(ThermalGUIHotCoatingComboBox_2->currentText());
    spacecraftPlatformSystem->TCS()->CoatingArea()->HotSurface()->HotCoating()->setEmissivity(ThermalGUIHotEmmissivityLineEdit_2->text().toDouble());
    spacecraftPlatformSystem->TCS()->CoatingArea()->HotSurface()->HotCoating()->setAbsorptivity(ThermalGUIHotAbsorptivityLineEdit_2->text().toDouble());

    //  ----------- Power ---------------


    //  ----------- Aerodynamics ---------------


    //  ----------- TTC ---------------


    //  ----------- GNC ---------------


    //  ----------- Data handling ---------------


    return true;

}








void PlatformDialog::on_buttonBoxLoitering_helpRequested()
{
	qWarning("TODO: %s	%d",__FILE__,__LINE__);
}

void PlatformDialog::on_buttonBoxLoitering_clicked(QAbstractButton*)
{
	qWarning("TODO: %s	%d",__FILE__,__LINE__);
}

void PlatformDialog::on_PlatformProperties_currentChanged(int)
{
	qWarning("TODO: %s	%d",__FILE__,__LINE__);
}

void PlatformDialog::on_groupBox_toggled(bool)
{
	qWarning("TODO: %s	%d",__FILE__,__LINE__);
}

void PlatformDialog::on_comboBox_2_activated(const QString&)
{
	qWarning("TODO: %s	%d",__FILE__,__LINE__);
}

void PlatformDialog::on_TotalMass_textChanged(const QString&)
{
	qWarning("TODO: %s	%d",__FILE__,__LINE__);
}

void PlatformDialog::on_comboBox_activated(const QString&)
{
	qWarning("TODO: %s	%d",__FILE__,__LINE__);
}

void PlatformDialog::on_Ixx_textChanged(const QString&)
{
	qWarning("TODO: %s	%d",__FILE__,__LINE__);
}

void PlatformDialog::on_Ixy_textChanged(const QString&)
{
	qWarning("TODO: %s	%d",__FILE__,__LINE__);
}

void PlatformDialog::on_Ixz_textChanged(const QString&)
{
	qWarning("TODO: %s	%d",__FILE__,__LINE__);
}

void PlatformDialog::on_Iyy_textChanged(const QString&)
{
	qWarning("TODO: %s	%d",__FILE__,__LINE__);
}

void PlatformDialog::on_Iyz_textChanged(const QString&)
{
	qWarning("TODO: %s	%d",__FILE__,__LINE__);
}

void PlatformDialog::on_Izz_textChanged(const QString&)
{
	qWarning("TODO: %s	%d",__FILE__,__LINE__);
}

void PlatformDialog::on_Izy_textChanged(const QString&)
{
	qWarning("TODO: %s	%d",__FILE__,__LINE__);
}

void PlatformDialog::on_Izx_textChanged(const QString&)
{
	qWarning("TODO: %s	%d",__FILE__,__LINE__);
}

void PlatformDialog::on_Iyx_textChanged(const QString&)
{
	qWarning("TODO: %s	%d",__FILE__,__LINE__);
}

void PlatformDialog::on_groupBox_2_toggled(bool)
{
	qWarning("TODO: %s	%d",__FILE__,__LINE__);
}

void PlatformDialog::on_Width_textChanged(const QString&)
{
	qWarning("TODO: %s	%d",__FILE__,__LINE__);
}

void PlatformDialog::on_Length_textChanged(const QString&)
{
	qWarning("TODO: %s	%d",__FILE__,__LINE__);
}

void PlatformDialog::on_Height_textChanged(const QString&)
{
	qWarning("TODO: %s	%d",__FILE__,__LINE__);
}

void PlatformDialog::on_Volume_textChanged(const QString&)
{
	qWarning("TODO: %s	%d",__FILE__,__LINE__);
}

void PlatformDialog::on_AxialFrequency_textChanged(const QString&)
{
	qWarning("TODO: %s	%d",__FILE__,__LINE__);
}

void PlatformDialog::on_LateralFrequency_textChanged(const QString&)
{
	qWarning("TODO: %s	%d",__FILE__,__LINE__);
}

void PlatformDialog::on_PlatformDialogClass_accepted()
{
	qWarning("TODO: %s	%d",__FILE__,__LINE__);
}

void PlatformDialog::on_PlatformDialogClass_rejected()
{
	qWarning("TODO: %s	%d",__FILE__,__LINE__);
}

