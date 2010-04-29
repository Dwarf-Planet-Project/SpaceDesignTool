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

#include "transmitterPayloadDialog.h"
#include <QDebug>


transmitterPayloadDialog::transmitterPayloadDialog( QWidget * parent, Qt::WindowFlags f) : QDialog(parent,f)
{
	setupUi(this);
}

transmitterPayloadDialog::~transmitterPayloadDialog()
{
}



// This is the function that load the values of the GUI into the XML schema
bool transmitterPayloadDialog::loadValues(ScenarioTransmitterPayloadType* commPayload)
{

    ElLineEdit->setText(QString::number(commPayload->Transmitter()->PointingDirection()->elevation()));
    AzLineEdit->setText(QString::number(commPayload->Transmitter()->PointingDirection()->azimuth()));

    TxFeederLossLineEdit->setText(QString::number(commPayload->Transmitter()->FedderLossTx()));
    TxDepointingLossLineEdit->setText(QString::number(commPayload->Transmitter()->DepointingLossTx()));

    GainLineEdit->setText(QString::number(commPayload->Transmitter()->EMproperties()->GainMax()));
    DiameterLineEdit->setText(QString::number(commPayload->Transmitter()->EMproperties()->Diameter()));
    BeamLineEdit->setText(QString::number(commPayload->Transmitter()->EMproperties()->AngularBeamWidth()));
    EfficiencyLineEdit->setText(QString::number(commPayload->Transmitter()->EMproperties()->Efficiency()));
    TiltLineEdit->setText(QString::number(commPayload->Transmitter()->EMproperties()->TiltAngle()));

    //POWER AND FREQUENCY IS MISSING BY NOW, SEE THE SCHEMA AND BUDGETS...


    DataRateLineEdit->setText(QString::number(commPayload->Transmitter()->Modulation()->DataRate()));
    //For the Modulation combo box
    QString modType;
    int i=0;
    modType=commPayload->Transmitter()->Modulation()->ModulationType();


   if(modType=="BPSK")
       i=0;
   else if (modType=="DE-BPSK")
        i=1;
   else if (modType=="D-BPSK")
        i=2;
   else if (modType=="QPSK")
        i=3;
   else if (modType=="DE-QPSK")
        i=4;
   else if (modType=="D-QPSK")
        i=5;
   else if (modType=="OPSK")
        i=6;
   else if (modType=="MSK")
        i=7;


    TypeModComboBox->setCurrentIndex(i);
    //qDebug()<<TypeModComboBox->currentText()<< "   Este es la modulación del esquema por defecto!!!!";

    return true;
}

// This is the function that load the values of the GUI into the XML schema
bool transmitterPayloadDialog::saveValues(ScenarioTransmitterPayloadType* commPayload)
{

    commPayload->Transmitter()->PointingDirection()->setElevation(ElLineEdit->text().toDouble());
    commPayload->Transmitter()->PointingDirection()->setAzimuth(AzLineEdit->text().toDouble());

    commPayload->Transmitter()->EMproperties()->setGainMax(GainLineEdit->text().toDouble());
    commPayload->Transmitter()->EMproperties()->setDiameter(DiameterLineEdit->text().toDouble());
    commPayload->Transmitter()->EMproperties()->setAngularBeamWidth(BeamLineEdit->text().toDouble());
    commPayload->Transmitter()->EMproperties()->setEfficiency(EfficiencyLineEdit->text().toDouble());
    commPayload->Transmitter()->EMproperties()->setTiltAngle(TiltLineEdit->text().toDouble());
     //POWER AND FREQUENCY IS MISSING BY NOW, SEE THE SCHEMA AND BUDGETS...
    commPayload->Transmitter()->setFedderLossTx(TxFeederLossLineEdit->text().toDouble());
    commPayload->Transmitter()->setDepointingLossTx(TxDepointingLossLineEdit->text().toDouble());

    commPayload->Transmitter()->Modulation()->setDataRate(DataRateLineEdit->text().toDouble());
    commPayload->Transmitter()->Modulation()->setModulationType(TypeModComboBox->currentText());


    return true;
}






void transmitterPayloadDialog::on_buttonBox_helpRequested()
{
	qWarning("TODO: %s	%d",__FILE__,__LINE__);
}

void transmitterPayloadDialog::on_buttonBox_clicked(QAbstractButton*)
{
	qWarning("TODO: %s	%d",__FILE__,__LINE__);
}

void transmitterPayloadDialog::on_tabWidget_currentChanged(int)
{
	qWarning("TODO: %s	%d",__FILE__,__LINE__);
}

void transmitterPayloadDialog::on_MassLineEdit_textChanged(const QString&)
{
	qWarning("TODO: %s	%d",__FILE__,__LINE__);
}

void transmitterPayloadDialog::on_LengthLineEdit_textChanged(const QString&)
{
	qWarning("TODO: %s	%d",__FILE__,__LINE__);
}

void transmitterPayloadDialog::on_WidthLineEdit_textChanged(const QString&)
{
	qWarning("TODO: %s	%d",__FILE__,__LINE__);
}

void transmitterPayloadDialog::on_HightLineEdit_textChanged(const QString&)
{
	qWarning("TODO: %s	%d",__FILE__,__LINE__);
}

void transmitterPayloadDialog::on_VertAngleLineEdit_textChanged(const QString&)
{
	qWarning("TODO: %s	%d",__FILE__,__LINE__);
}

void transmitterPayloadDialog::on_HorAngleLineEdit_textChanged(const QString&)
{
	qWarning("TODO: %s	%d",__FILE__,__LINE__);
}

void transmitterPayloadDialog::on_ConeAngleLineEdit_textChanged(const QString&)
{
	qWarning("TODO: %s	%d",__FILE__,__LINE__);
}

void transmitterPayloadDialog::on_ElLineEdit_textChanged(const QString&)
{
	qWarning("TODO: %s	%d",__FILE__,__LINE__);
}

void transmitterPayloadDialog::on_AzLineEdit_textChanged(const QString&)
{
	qWarning("TODO: %s	%d",__FILE__,__LINE__);
}

void transmitterPayloadDialog::on_ConeShapeComboBox_activated(const QString&)
{

	qWarning("TODO: %s	%d",__FILE__,__LINE__);

       if(ConeShapeComboBox->currentText()=="Circular")
            stackedWidget->setCurrentIndex(0);
       else if(ConeShapeComboBox->currentText()=="Rectangular")
            stackedWidget->setCurrentIndex(1);
}


void transmitterPayloadDialog::on_PolarisationGroupBox_toggled(bool)
{
	qWarning("TODO: %s	%d",__FILE__,__LINE__);
}

void transmitterPayloadDialog::on_PolarisationComboBox_activated(const QString&)
{
	qWarning("TODO: %s	%d",__FILE__,__LINE__);
}

void transmitterPayloadDialog::on_TiltLineEdit_textChanged(const QString&)
{
	qWarning("TODO: %s	%d",__FILE__,__LINE__);
}

void transmitterPayloadDialog::on_ModulationGroupBox_toggled(bool)
{
       qWarning("TODO: %s	%d",__FILE__,__LINE__);
}

void transmitterPayloadDialog::on_TypeModComboBox_activated(const QString&)
{
        qDebug()<<TypeModComboBox->currentText()<<"   Modulacion actual";
	qWarning("TODO: %s	%d",__FILE__,__LINE__);
}

void transmitterPayloadDialog::on_DataRateLineEdit_textChanged(const QString&)
{
	qWarning("TODO: %s	%d",__FILE__,__LINE__);
}

void transmitterPayloadDialog::on_EquipmentGroupBox_toggled(bool)
{
	qWarning("TODO: %s	%d",__FILE__,__LINE__);
}

void transmitterPayloadDialog::on_PowerLineEdit_textChanged(const QString&)
{
	qWarning("TODO: %s	%d",__FILE__,__LINE__);
}

void transmitterPayloadDialog::on_TxFeederLossLineEdit_textChanged(const QString&)
{
	qWarning("TODO: %s	%d",__FILE__,__LINE__);
}

void transmitterPayloadDialog::on_TxDepointingLossLineEdit_textChanged(const QString&)
{
	qWarning("TODO: %s	%d",__FILE__,__LINE__);
}

void transmitterPayloadDialog::on_AntennaSizeGroupBox_toggled(bool)
{
	qWarning("TODO: %s	%d",__FILE__,__LINE__);
}

void transmitterPayloadDialog::on_GainMaxRadioButton_toggled(bool)
{
	qWarning("TODO: %s	%d",__FILE__,__LINE__);
}

void transmitterPayloadDialog::on_DiameterRadioButton_toggled(bool)
{
	qWarning("TODO: %s	%d",__FILE__,__LINE__);
}

void transmitterPayloadDialog::on_BeamWidthRadioButton_toggled(bool)
{
	qWarning("TODO: %s	%d",__FILE__,__LINE__);
}

void transmitterPayloadDialog::on_GainLineEdit_textChanged(const QString&)
{
	qWarning("TODO: %s	%d",__FILE__,__LINE__);
}

void transmitterPayloadDialog::on_DiameterLineEdit_textChanged(const QString&)
{
	qWarning("TODO: %s	%d",__FILE__,__LINE__);
}

void transmitterPayloadDialog::on_BeamLineEdit_textChanged(const QString&)
{
	qWarning("TODO: %s	%d",__FILE__,__LINE__);
}

void transmitterPayloadDialog::on_groupBox_toggled(bool)
{
	qWarning("TODO: %s	%d",__FILE__,__LINE__);
}

void transmitterPayloadDialog::on_EfficiencyLineEdit_textChanged(const QString&)
{
	qWarning("TODO: %s	%d",__FILE__,__LINE__);
}

void transmitterPayloadDialog::on_FrequencyLineEdit_textChanged(const QString&)
{
	qWarning("TODO: %s	%d",__FILE__,__LINE__);
}

void transmitterPayloadDialog::on_TypeBeamComboBox_activated(const QString&)
{
	qWarning("TODO: %s	%d",__FILE__,__LINE__);
}

