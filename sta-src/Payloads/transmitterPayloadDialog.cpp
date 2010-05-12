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
#include <Astro-Core/constants.h>
#include <Coverage/commanalysis.h>


int antennaRadioButtonTransmitter;
QString polarisationTypeTransmitter="Linear";



transmitterPayloadDialog::transmitterPayloadDialog( QWidget * parent, Qt::WindowFlags f) : QDialog(parent,f)
{
	setupUi(this);

        QDoubleValidator* positiveDoubleValidator = new QDoubleValidator(this);
        positiveDoubleValidator->setBottom(0.0);

        ElLineEdit->setValidator(positiveDoubleValidator);
        TxFeederLossLineEdit->setValidator(positiveDoubleValidator);
        TxDepointingLossLineEdit->setValidator(positiveDoubleValidator);
        GainLineEdit->setValidator(positiveDoubleValidator);
        DiameterLineEdit->setValidator(positiveDoubleValidator);
        BeamLineEdit->setValidator(positiveDoubleValidator);
        TiltLineEdit->setValidator(positiveDoubleValidator);
        EfficiencyLineEdit->setValidator(positiveDoubleValidator);
        FrequencyLineEdit->setValidator(positiveDoubleValidator);
        PowerLineEdit->setValidator(positiveDoubleValidator);
        DataRateLineEdit->setValidator(positiveDoubleValidator);

        ConeAngleLineEdit->setValidator(positiveDoubleValidator);
        HorAngleLineEdit->setValidator(positiveDoubleValidator);
        VertAngleLineEdit->setValidator(positiveDoubleValidator);
}

transmitterPayloadDialog::~transmitterPayloadDialog()
{
}



// This is the function that load the values of the XML schema on the GUI
bool transmitterPayloadDialog::loadValues(ScenarioTransmitterPayloadType* commPayload)
{
    //These lines allow the GUI to remember which choice the user did for the antenna parameters
    if(antennaRadioButtonTransmitter==1)
        DiameterRadioButton->toggle();
    if(antennaRadioButtonTransmitter==2)
        BeamWidthRadioButton->toggle();


    ElLineEdit->setText(QString::number(commPayload->Transmitter()->PointingDirection()->elevation()));
    AzLineEdit->setText(QString::number(commPayload->Transmitter()->PointingDirection()->azimuth()));

    TxFeederLossLineEdit->setText(QString::number(commPayload->Transmitter()->FedderLossTx()));
    TxDepointingLossLineEdit->setText(QString::number(commPayload->Transmitter()->DepointingLossTx()));

    GainLineEdit->setText(QString::number(commPayload->Transmitter()->EMproperties()->GainMax()));
    DiameterLineEdit->setText(QString::number(commPayload->Transmitter()->EMproperties()->Diameter()));
    BeamLineEdit->setText(QString::number(commPayload->Transmitter()->EMproperties()->AngularBeamWidth()));
    EfficiencyLineEdit->setText(QString::number(commPayload->Transmitter()->EMproperties()->Efficiency()));
    TiltLineEdit->setText(QString::number(commPayload->Transmitter()->EMproperties()->TiltAngle()));
    PowerLineEdit->setText(QString::number(commPayload->Transmitter()->TransmittingPower()));
    //FREQUENCY IS MISSING BY NOW, SEE THE SCHEMA AND BUDGETS...


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

     antennaCalculations(commPayload);

     //These lines allow the GUI to remember which choice the user did for the type of polarisation
     if(polarisationTypeTransmitter=="Linear")
         PolarisationComboBox->setCurrentIndex(0);
     if(polarisationTypeTransmitter=="Right")
         PolarisationComboBox->setCurrentIndex(1);
     if(polarisationTypeTransmitter=="Left")
         PolarisationComboBox->setCurrentIndex(2);



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
    //FREQUENCY IS MISSING BY NOW, SEE THE SCHEMA AND BUDGETS...
    commPayload->Transmitter()->setTransmittingPower(PowerLineEdit->text().toDouble());
    commPayload->Transmitter()->setFedderLossTx(TxFeederLossLineEdit->text().toDouble());
    commPayload->Transmitter()->setDepointingLossTx(TxDepointingLossLineEdit->text().toDouble());

    commPayload->Transmitter()->Modulation()->setDataRate(DataRateLineEdit->text().toDouble());
    commPayload->Transmitter()->Modulation()->setModulationType(TypeModComboBox->currentText());


    antennaCalculations(commPayload);

    //These lines allow the GUI to remember which choice the user did for the antenna parameters
    if(GainMaxRadioButton->isChecked()==true)
        antennaRadioButtonTransmitter=0;
    if(DiameterRadioButton->isChecked()==true)
        antennaRadioButtonTransmitter=1;
    if(BeamWidthRadioButton->isChecked()==true)
        antennaRadioButtonTransmitter=2;

    //These lines allow the GUI to remember which choice the user did for the type of polarisation
    if(PolarisationComboBox->currentText()=="Linear")
        polarisationTypeTransmitter="Linear";
    if(PolarisationComboBox->currentText()=="Right Circular")
        polarisationTypeTransmitter="Right";
    if(PolarisationComboBox->currentText()=="Left Circular")
        polarisationTypeTransmitter="Left";

    return true;
}


void transmitterPayloadDialog::antennaCalculations(ScenarioTransmitterPayloadType* commPayload){

    double Pi=3.141592;
    double lightSpeed=SPEED_OF_LIGHT;
    double efficiency=commPayload->Transmitter()->EMproperties()->Efficiency();
    double frequency=14.5E9;


    double diameter, beamwidth, gainMaxDb, gainMax;

    if(GainMaxRadioButton->isChecked()==true)
    {
        qDebug()<<"This means that GainRadioButton is checked";
        //Since we have the Gain max
        gainMaxDb=commPayload->Transmitter()->EMproperties()->GainMax();
        gainMax=pow(10, gainMaxDb/10);
        diameter=(lightSpeed/(Pi*frequency))*sqrt(gainMax/(efficiency/100));
        beamwidth=70*(lightSpeed/(frequency*diameter));


        //Put the values in the GUI
        DiameterLineEdit->setText(QString::number(diameter));
        BeamLineEdit->setText(QString::number(beamwidth));
        qDebug()<<"The value of the diameter is: "<<diameter;
        qDebug()<<"The value of the beam is: "<<beamwidth;

    }
    if(DiameterRadioButton->isChecked()==true)
    {
        qDebug()<<"This means that DiameterRadioButton is checked";
        diameter=commPayload->Transmitter()->EMproperties()->Diameter();


        gainMax=(efficiency/100)*pow((Pi*diameter*frequency/lightSpeed),2); //natural units
        gainMaxDb=10*log10(gainMax); //in dB
        beamwidth=70*(lightSpeed/(frequency*diameter));

        //Put the values in the GUI
        BeamLineEdit->setText(QString::number(beamwidth));
        GainLineEdit->setText(QString::number(gainMaxDb));
        qDebug()<<"The value of the gainMax in dB is: "<<gainMaxDb;
        qDebug()<<"The value of the beam is: "<<beamwidth;
    }
    if(BeamWidthRadioButton->isChecked()==true)
    {
        qDebug()<<"This means that BeamRadioButton is checked";
        beamwidth=commPayload->Transmitter()->EMproperties()->AngularBeamWidth();

        gainMax=(efficiency/100)*48360/pow(beamwidth,2);
        gainMaxDb=10*log10(gainMax); //in dB
        diameter=(lightSpeed/(Pi*frequency))*sqrt(gainMax/(efficiency/100));



        //Put the values in the GUI
        DiameterLineEdit->setText(QString::number(diameter));
        GainLineEdit->setText(QString::number(gainMaxDb));
        qDebug()<<"The value of the gainMax in dB is: "<<gainMaxDb;
        qDebug()<<"The value of the diameter is: "<<diameter;


    }



}



void transmitterPayloadDialog::on_buttonBox_helpRequested()
{
	qWarning("TODO: %s	%d",__FILE__,__LINE__);
}

void transmitterPayloadDialog::on_buttonBox_clicked(QAbstractButton*)
{
	qWarning("TODO: %s	%d",__FILE__,__LINE__);
        CommAnalysis commanalysis;
        commanalysis.FreeSpaceLoss();
        commanalysis.OxygenSpecificAttenuation();
        //commanalysis.getRange();
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



