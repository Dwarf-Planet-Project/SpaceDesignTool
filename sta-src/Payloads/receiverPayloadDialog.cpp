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
#include <QDebug>
#include <Astro-Core/constants.h>


int antennaRadioButtonReceiver;
QString polarisationTypeReceiver="Linear";

receiverPayloadDialog::receiverPayloadDialog( QWidget * parent, Qt::WindowFlags f) : QDialog(parent,f)
{
        setupUi(this);


        QDoubleValidator* positiveDoubleValidator = new QDoubleValidator(this);
        positiveDoubleValidator->setBottom(0.0);

        QDoubleValidator* efficiencyValidator = new QDoubleValidator(this);
        efficiencyValidator->setBottom(0.0);
        efficiencyValidator->setTop(100.0);

        ElLineEdit->setValidator(positiveDoubleValidator);

        RxFeederLossLineEdit->setValidator(positiveDoubleValidator);
        RxDepointingLossLineEdit->setValidator(positiveDoubleValidator);
        RxFeederTmpLineEdit->setValidator(positiveDoubleValidator);
        RxTempLineEdit->setValidator(positiveDoubleValidator);
        RxNoiseFigureLineEdit->setValidator(positiveDoubleValidator);

        GainLineEdit->setValidator(positiveDoubleValidator);
        DiameterLineEdit->setValidator(positiveDoubleValidator);
        BeamLineEdit->setValidator(positiveDoubleValidator);
        TiltLineEdit->setValidator(positiveDoubleValidator);
        EfficiencyLineEdit->setValidator(efficiencyValidator);
        FrequencyLineEdit->setValidator(positiveDoubleValidator);


        ConeAngleLineEdit->setValidator(positiveDoubleValidator);
        HorAngleLineEdit->setValidator(positiveDoubleValidator);
        VertAngleLineEdit->setValidator(positiveDoubleValidator);
}

receiverPayloadDialog::~receiverPayloadDialog()
{
}


// This is the function that load the values from the XML schema to the GUI
bool receiverPayloadDialog::loadValues(ScenarioReceiverPayloadType* receiverPayload)
{

    m_receiverPayload=receiverPayload;

    //These lines allow the GUI to remember which choice the user did for the antenna parameters
    if(antennaRadioButtonReceiver==1)
        DiameterRadioButton->toggle();
    if(antennaRadioButtonReceiver==2)
        BeamWidthRadioButton->toggle();

    ElLineEdit->setText(QString::number((receiverPayload->Receiver()->PointingDirection()->elevation())*57.2957794));
    AzLineEdit->setText(QString::number(receiverPayload->Receiver()->PointingDirection()->azimuth()));

    //The cones are missing
    RxFeederLossLineEdit->setText(QString::number(receiverPayload->Receiver()->FeederLossRx()));
    RxDepointingLossLineEdit->setText(QString::number(receiverPayload->Receiver()->DepointingLossRx()));

    GainLineEdit->setText(QString::number(receiverPayload->Receiver()->EMproperties()->GainMax()));
    DiameterLineEdit->setText(QString::number(receiverPayload->Receiver()->EMproperties()->Diameter()));
    BeamLineEdit->setText(QString::number(receiverPayload->Receiver()->EMproperties()->AngularBeamWidth()));
    EfficiencyLineEdit->setText(QString::number(receiverPayload->Receiver()->EMproperties()->Efficiency()));
    TiltLineEdit->setText(QString::number(receiverPayload->Receiver()->EMproperties()->TiltAngle()));

    double frequency=receiverPayload->Budget()->FrequencyBand();
    frequency=frequency/1000000000;//from Hz to GHz
    FrequencyLineEdit->setText(QString::number(frequency));

    RxFeederTmpLineEdit->setText(QString::number(receiverPayload->Receiver()->SystemTemperature()->ThermoFeeder()));
    RxTempLineEdit->setText(QString::number(receiverPayload->Receiver()->SystemTemperature()->ThermoReveicer()));
    RxNoiseFigureLineEdit->setText(QString::number(receiverPayload->Receiver()->SystemTemperature()->RxNoiseFigure()));


    antennaCalculations(receiverPayload);

    //These lines allow the GUI to remember which choice the user did for the type of polarisation
    if(polarisationTypeReceiver=="Linear")
        PolarisationComboBox->setCurrentIndex(0);
    if(polarisationTypeReceiver=="Right")
        PolarisationComboBox->setCurrentIndex(1);
    if(polarisationTypeReceiver=="Left")
        PolarisationComboBox->setCurrentIndex(2);


    return true;
}

// This is the function that load the values of the GUI into the XML schema
bool receiverPayloadDialog::saveValues(ScenarioReceiverPayloadType* receiverPayload)
{

    receiverPayload->Receiver()->PointingDirection()->setElevation(ElLineEdit->text().toDouble());
    receiverPayload->Receiver()->PointingDirection()->setAzimuth(AzLineEdit->text().toDouble());
    //The cones are missing
    receiverPayload->Receiver()->EMproperties()->setEfficiency(EfficiencyLineEdit->text().toDouble());
    receiverPayload->Receiver()->EMproperties()->setTiltAngle(TiltLineEdit->text().toDouble());
    receiverPayload->Receiver()->EMproperties()->setGainMax(GainLineEdit->text().toDouble());
    receiverPayload->Receiver()->EMproperties()->setDiameter(DiameterLineEdit->text().toDouble());
    receiverPayload->Receiver()->EMproperties()->setAngularBeamWidth(BeamLineEdit->text().toDouble());

    double frequency=FrequencyLineEdit->text().toDouble();
    frequency=frequency*1000000000;
    receiverPayload->Budget()->setFrequencyBand(frequency);

    receiverPayload->Receiver()->SystemTemperature()->setRxNoiseFigure(RxNoiseFigureLineEdit->text().toDouble());
    receiverPayload->Receiver()->SystemTemperature()->setThermoFeeder(RxFeederTmpLineEdit->text().toDouble());
    receiverPayload->Receiver()->SystemTemperature()->setThermoReveicer(RxTempLineEdit->text().toDouble());

    receiverPayload->Receiver()->setFeederLossRx(RxFeederLossLineEdit->text().toDouble());
    receiverPayload->Receiver()->setDepointingLossRx(RxDepointingLossLineEdit->text().toDouble());


    antennaCalculations(receiverPayload);

    //These lines allow the GUI to remember which choice the user did for the antenna parameters
    if(GainMaxRadioButton->isChecked()==true)
        antennaRadioButtonReceiver=0;
    if(DiameterRadioButton->isChecked()==true)
        antennaRadioButtonReceiver=1;
    if(BeamWidthRadioButton->isChecked()==true)
        antennaRadioButtonReceiver=2;


    //These lines allow the GUI to remember which choice the user did for the type of polarisation
    if(PolarisationComboBox->currentText()=="Linear")
        polarisationTypeReceiver="Linear";
    if(PolarisationComboBox->currentText()=="Right Circular")
        polarisationTypeReceiver="Right";
    if(PolarisationComboBox->currentText()=="Left Circular")
        polarisationTypeReceiver="Left";


    return true;
}


void receiverPayloadDialog::antennaCalculations(ScenarioReceiverPayloadType* receiverPayload){

    double Pi=3.141592;
    double lightSpeed=SPEED_OF_LIGHT;
    double efficiency=receiverPayload->Receiver()->EMproperties()->Efficiency();
    double frequency=receiverPayload->Budget()->FrequencyBand();



    double diameter, beamwidth, gainMaxDb, gainMax;

    if(GainMaxRadioButton->isChecked()==true)
    {
        qDebug()<<"This means that GainRadioButton is checked";
        //Since we have the Gain max
        gainMaxDb=receiverPayload->Receiver()->EMproperties()->GainMax();
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
        diameter=receiverPayload->Receiver()->EMproperties()->Diameter();


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
        beamwidth=receiverPayload->Receiver()->EMproperties()->AngularBeamWidth();

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
