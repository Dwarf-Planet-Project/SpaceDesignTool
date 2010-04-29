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

#include "receiverPayloadDialog.h"

receiverPayloadDialog::receiverPayloadDialog( QWidget * parent, Qt::WindowFlags f) : QDialog(parent,f)
{
        setupUi(this);
}

receiverPayloadDialog::~receiverPayloadDialog()
{
}


// This is the function that load the values from the XML schema to the GUI
bool receiverPayloadDialog::loadValues(ScenarioReceiverPayloadType* commPayload)
{

    ElLineEdit->setText(QString::number((commPayload->Receiver()->PointingDirection()->elevation())*57.2957794));
    AzLineEdit->setText(QString::number(commPayload->Receiver()->PointingDirection()->azimuth()));

    //The cones are missing
    RxFeederLossLineEdit->setText(QString::number(commPayload->Receiver()->FeederLossRx()));
    RxDepointingLossLineEdit->setText(QString::number(commPayload->Receiver()->DepointingLossRx()));

    GainLineEdit->setText(QString::number(commPayload->Receiver()->EMproperties()->GainMax()));
    DiameterLineEdit->setText(QString::number(commPayload->Receiver()->EMproperties()->Diameter()));
    BeamLineEdit->setText(QString::number(commPayload->Receiver()->EMproperties()->AngularBeamWidth()));
    EfficiencyLineEdit->setText(QString::number(commPayload->Receiver()->EMproperties()->Efficiency()));
    TiltLineEdit->setText(QString::number(commPayload->Receiver()->EMproperties()->TiltAngle()));

    //FREQUENCY IS MISSING BY NOW, SEE THE SCHEMA AND BUDGETS...

    RxFeederTmpLineEdit->setText(QString::number(commPayload->Receiver()->SystemTemperature()->ThermoFeeder()));
    RxTempLineEdit->setText(QString::number(commPayload->Receiver()->SystemTemperature()->ThermoReveicer()));
    RxNoiseFigureLineEdit->setText(QString::number(commPayload->Receiver()->SystemTemperature()->RxNoiseFigure()));


    return true;
}

// This is the function that load the values of the GUI into the XML schema
bool receiverPayloadDialog::saveValues(ScenarioReceiverPayloadType* commPayload)
{

    commPayload->Receiver()->PointingDirection()->setElevation(ElLineEdit->text().toDouble());
    commPayload->Receiver()->PointingDirection()->setAzimuth(AzLineEdit->text().toDouble());
    //The cones are missing
    commPayload->Receiver()->EMproperties()->setEfficiency(EfficiencyLineEdit->text().toDouble());
    commPayload->Receiver()->EMproperties()->setTiltAngle(TiltLineEdit->text().toDouble());
    commPayload->Receiver()->EMproperties()->setGainMax(GainLineEdit->text().toDouble());
    commPayload->Receiver()->EMproperties()->setDiameter(DiameterLineEdit->text().toDouble());
    commPayload->Receiver()->EMproperties()->setAngularBeamWidth(BeamLineEdit->text().toDouble());

    commPayload->Receiver()->SystemTemperature()->setRxNoiseFigure(RxNoiseFigureLineEdit->text().toDouble());
    commPayload->Receiver()->SystemTemperature()->setThermoFeeder(RxFeederTmpLineEdit->text().toDouble());
    commPayload->Receiver()->SystemTemperature()->setThermoReveicer(RxTempLineEdit->text().toDouble());

    commPayload->Receiver()->setFeederLossRx(RxFeederLossLineEdit->text().toDouble());
    commPayload->Receiver()->setDepointingLossRx(RxDepointingLossLineEdit->text().toDouble());

    return true;
}





void receiverPayloadDialog::on_buttonBox_helpRequested()
{
        qWarning("TODO: %s	%d",__FILE__,__LINE__);
}

void receiverPayloadDialog::on_buttonBox_clicked(QAbstractButton*)
{
        qWarning("TODO: %s	%d",__FILE__,__LINE__);
}

void receiverPayloadDialog::on_tabWidget_currentChanged(int)
{
        qWarning("TODO: %s	%d",__FILE__,__LINE__);
}

void receiverPayloadDialog::on_MassLineEdit_textChanged(const QString&)
{
        qWarning("TODO: %s	%d",__FILE__,__LINE__);
}

void receiverPayloadDialog::on_LengthLineEdit_textChanged(const QString&)
{
        qWarning("TODO: %s	%d",__FILE__,__LINE__);
}

void receiverPayloadDialog::on_WidthLineEdit_textChanged(const QString&)
{
        qWarning("TODO: %s	%d",__FILE__,__LINE__);
}

void receiverPayloadDialog::on_HightLineEdit_textChanged(const QString&)
{
        qWarning("TODO: %s	%d",__FILE__,__LINE__);
}

void receiverPayloadDialog::on_VertAngleLineEdit_textChanged(const QString&)
{
        qWarning("TODO: %s	%d",__FILE__,__LINE__);
}

void receiverPayloadDialog::on_HorAngleLineEdit_textChanged(const QString&)
{
        qWarning("TODO: %s	%d",__FILE__,__LINE__);
}

void receiverPayloadDialog::on_ConeAngleLineEdit_textChanged(const QString&)
{
        qWarning("TODO: %s	%d",__FILE__,__LINE__);
}

void receiverPayloadDialog::on_ElLineEdit_textChanged(const QString&)
{
        qWarning("TODO: %s	%d",__FILE__,__LINE__);
}

void receiverPayloadDialog::on_AzLineEdit_textChanged(const QString&)
{
        qWarning("TODO: %s	%d",__FILE__,__LINE__);
}

void receiverPayloadDialog::on_ConeShapeComboBox_activated(const QString&)
{
        qWarning("TODO: %s	%d",__FILE__,__LINE__);
        if(ConeShapeComboBox->currentText()=="Circular")
             stackedWidget->setCurrentIndex(0);
        else if(ConeShapeComboBox->currentText()=="Rectangular")
             stackedWidget->setCurrentIndex(1);
}

void receiverPayloadDialog::on_PolarisationGroupBox_toggled(bool)
{
        qWarning("TODO: %s	%d",__FILE__,__LINE__);
}

void receiverPayloadDialog::on_PolarisationComboBox_activated(const QString&)
{
        qWarning("TODO: %s	%d",__FILE__,__LINE__);
}

void receiverPayloadDialog::on_TiltLineEdit_textChanged(const QString&)
{
        qWarning("TODO: %s	%d",__FILE__,__LINE__);
}

void receiverPayloadDialog::on_SystemTempGroupBox_toggled(bool)
{
        qWarning("TODO: %s	%d",__FILE__,__LINE__);
}

void receiverPayloadDialog::on_RxNoiseFigureLineEdit_textChanged(const QString&)
{
        qWarning("TODO: %s	%d",__FILE__,__LINE__);
}

void receiverPayloadDialog::on_RxFeederTmpLineEdit_textChanged(const QString&)
{
        qWarning("TODO: %s	%d",__FILE__,__LINE__);
}

void receiverPayloadDialog::on_RxTempLineEdit_textChanged(const QString&)
{
        qWarning("TODO: %s	%d",__FILE__,__LINE__);
}

void receiverPayloadDialog::on_ConstantTempRadioButton_toggled(bool)
{
        qWarning("TODO: %s	%d",__FILE__,__LINE__);
}

void receiverPayloadDialog::on_AntennaTempLineEdit_textChanged(const QString&)
{
        qWarning("TODO: %s	%d",__FILE__,__LINE__);
}

void receiverPayloadDialog::on_CalculatedTempRadioButton_toggled(bool)
{
        qWarning("TODO: %s	%d",__FILE__,__LINE__);
}

void receiverPayloadDialog::on_EquipmentGroupBox_toggled(bool)
{
        qWarning("TODO: %s	%d",__FILE__,__LINE__);
}

void receiverPayloadDialog::on_RxFeederLossLineEdit_textChanged(const QString&)
{
        qWarning("TODO: %s	%d",__FILE__,__LINE__);
}

void receiverPayloadDialog::on_RxDepointingLossLineEdit_textChanged(const QString&)
{
        qWarning("TODO: %s	%d",__FILE__,__LINE__);
}

void receiverPayloadDialog::on_AntennaSizeGroupBox_toggled(bool)
{
        qWarning("TODO: %s	%d",__FILE__,__LINE__);
}

void receiverPayloadDialog::on_GainMaxRadioButton_toggled(bool)
{
        qWarning("TODO: %s	%d",__FILE__,__LINE__);
}

void receiverPayloadDialog::on_DiameterRadioButton_toggled(bool)
{
        qWarning("TODO: %s	%d",__FILE__,__LINE__);
}

void receiverPayloadDialog::on_BeamWidthRadioButton_toggled(bool)
{
        qWarning("TODO: %s	%d",__FILE__,__LINE__);
}

void receiverPayloadDialog::on_GainLineEdit_textChanged(const QString&)
{
        qWarning("TODO: %s	%d",__FILE__,__LINE__);
}

void receiverPayloadDialog::on_DiameterLineEdit_textChanged(const QString&)
{
        qWarning("TODO: %s	%d",__FILE__,__LINE__);
}

void receiverPayloadDialog::on_BeamLineEdit_textChanged(const QString&)
{
        qWarning("TODO: %s	%d",__FILE__,__LINE__);
}

void receiverPayloadDialog::on_groupBox_toggled(bool)
{
        qWarning("TODO: %s	%d",__FILE__,__LINE__);
}

void receiverPayloadDialog::on_EfficiencyLineEdit_textChanged(const QString&)
{
        qWarning("TODO: %s	%d",__FILE__,__LINE__);
}

void receiverPayloadDialog::on_FrequencyLineEdit_textChanged(const QString&)
{
        qWarning("TODO: %s	%d",__FILE__,__LINE__);
}

void receiverPayloadDialog::on_TypeBeamComboBox_activated(const QString&)
{
        qWarning("TODO: %s	%d",__FILE__,__LINE__);
}
