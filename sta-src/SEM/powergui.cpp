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
 ------------------ Author: Ozgun YILMAZ      ---------------------------------------------
 ------------------ email: ozgunus@yahoo.com  ---------------------------------------------

 */

#include "powergui.h"

PowerGUI::PowerGUI(SemMain * SC, QWidget * parent, Qt::WindowFlags f) : QDialog(parent,f)
{
	setupUi(this);
        SCVirtual = SC;

        // set the initial values from powersubsystem (Payload 1-4)
        PowerGUIPayload1NameLineEdit->setText
                (SCVirtual->getNewSCPower()->getPayloadOfIndex(0).Name);
        PowerGUIPayload1PowerInEclipseLineEdit->setText
                (QString::number
                 (SCVirtual->getNewSCPower()->getPayloadOfIndex(0).PowerConsumptionInEclipse));
        PowerGUIPayload1PowerInDaylightLineEdit->setText
                (QString::number
                 (SCVirtual->getNewSCPower()->getPayloadOfIndex(0).PowerConsumptionInDaylight));
        PowerGUIPayload1PowerOnEclipseLineEdit->setText
                (QString::number
                 (SCVirtual->getNewSCPower()->getPayloadOfIndex(0).PowerOnTimeInEclipse));
        PowerGUIPayload1PowerOnDaylightLineEdit->setText
                (QString::number
                 (SCVirtual->getNewSCPower()->getPayloadOfIndex(0).PowerOnTimeInDaylight));

        PowerGUIPayload2NameLineEdit->setText
                (SCVirtual->getNewSCPower()->getPayloadOfIndex(1).Name);
        PowerGUIPayload2PowerInEclipseLineEdit->setText
                (QString::number
                 (SCVirtual->getNewSCPower()->getPayloadOfIndex(1).PowerConsumptionInEclipse));
        PowerGUIPayload2PowerInDaylightLineEdit->setText
                (QString::number
                 (SCVirtual->getNewSCPower()->getPayloadOfIndex(1).PowerConsumptionInDaylight));
        PowerGUIPayload2PowerOnEclipseLineEdit->setText
                (QString::number
                 (SCVirtual->getNewSCPower()->getPayloadOfIndex(1).PowerOnTimeInEclipse));
        PowerGUIPayload2PowerOnDaylightLineEdit->setText
                (QString::number
                 (SCVirtual->getNewSCPower()->getPayloadOfIndex(1).PowerOnTimeInDaylight));

        PowerGUIPayload3NameLineEdit->setText
                (SCVirtual->getNewSCPower()->getPayloadOfIndex(2).Name);
        PowerGUIPayload3PowerInEclipseLineEdit->setText
                (QString::number
                 (SCVirtual->getNewSCPower()->getPayloadOfIndex(2).PowerConsumptionInEclipse));
        PowerGUIPayload3PowerInDaylightLineEdit->setText
                (QString::number
                 (SCVirtual->getNewSCPower()->getPayloadOfIndex(2).PowerConsumptionInDaylight));
        PowerGUIPayload3PowerOnEclipseLineEdit->setText
                (QString::number
                 (SCVirtual->getNewSCPower()->getPayloadOfIndex(2).PowerOnTimeInEclipse));
        PowerGUIPayload3PowerOnDaylightLineEdit->setText
                (QString::number
                 (SCVirtual->getNewSCPower()->getPayloadOfIndex(2).PowerOnTimeInDaylight));

        PowerGUIPayload4NameLineEdit->setText
                (SCVirtual->getNewSCPower()->getPayloadOfIndex(3).Name);
        PowerGUIPayload4PowerInEclipseLineEdit->setText
                (QString::number
                 (SCVirtual->getNewSCPower()->getPayloadOfIndex(3).PowerConsumptionInEclipse));
        PowerGUIPayload4PowerInDaylightLineEdit->setText
                (QString::number
                 (SCVirtual->getNewSCPower()->getPayloadOfIndex(3).PowerConsumptionInDaylight));
        PowerGUIPayload4PowerOnEclipseLineEdit->setText
                (QString::number
                 (SCVirtual->getNewSCPower()->getPayloadOfIndex(3).PowerOnTimeInEclipse));
        PowerGUIPayload4PowerOnDaylightLineEdit->setText
                (QString::number
                 (SCVirtual->getNewSCPower()->getPayloadOfIndex(3).PowerOnTimeInDaylight));

        //Visualise the average power consumption
        PowerGUIAveragePowerInEclipseLineEdit->setText
                (QString::number(SCVirtual->getNewSCPower()->getAveragePowerConsumptionInEclipse()));
        PowerGUIAveragePowerInDaylightLineEdit->setText
                (QString::number(SCVirtual->getNewSCPower()->getAveragePowerConsumptionInDaylight()));

        //Settings of the comboBoxes
        //setting comboBox from a QString
        int i;
        for (i=0;i<PowerGUISolarCellTypeComboBox->count();i++)
            if (PowerGUISolarCellTypeComboBox->itemText(i)
                == SC->getNewSCPower()->SolarArrays.getSolarCell().Name)
                PowerGUISolarCellTypeComboBox->setCurrentIndex(i);
        //setting comboBox from a QString
        for (i=0;i<PowerGUIBatteryTypeComboBox->count();i++)
            if (PowerGUIBatteryTypeComboBox->itemText(i)
                == SC->getNewSCPower()->Battery.getBatteryProperties().Name)
                PowerGUIBatteryTypeComboBox->setCurrentIndex(i);

        //Initialize the second half of the GUI

        PowerGUIBOLPowerLineEdit->setText
                (QString::number(SCVirtual->getNewSCPower()->SolarArrays.getSolarCellBOLPower()));

        PowerGUIEOLPowerLineEdit->setText
                (QString::number(SCVirtual->getNewSCPower()->SolarArrays.getSolarCellEOLPower()));

        PowerGUISolarArrayAreaLineEdit->setText
                (QString::number(SCVirtual->getNewSCPower()->SolarArrays.getArea()));

        SolarArrayEOLPowerLineEdit->setText
                (QString::number(SCVirtual->getNewSCPower()->SolarArrays.getSAEOLPower()));

        SolarArrayBOLPowerLineEdit->setText
                (QString::number(SCVirtual->getNewSCPower()->SolarArrays.getSABOLPower()));

        PowerGUIBatteryNumberLineEdit->setText
                (QString::number(SCVirtual->getNewSCPower()->Battery.getNumberOfBatteries()));

        qWarning("TODO: %s	%d",__FILE__,__LINE__);

}

PowerGUI::~PowerGUI()
{
}

void PowerGUI::on_PowerGUIPayload1PowerOnEclipseLineEdit_textChanged(const QString&)
{
	qWarning("TODO: %s	%d",__FILE__,__LINE__);
}

void PowerGUI::on_PowerGUIPayload1PowerOnDaylightLineEdit_textChanged(const QString&)
{
	qWarning("TODO: %s	%d",__FILE__,__LINE__);
}

void PowerGUI::on_PowerGUIPayload2PowerOnEclipseLineEdit_textChanged(const QString&)
{
	qWarning("TODO: %s	%d",__FILE__,__LINE__);
}

void PowerGUI::on_PowerGUIPayload2PowerOnDaylightLineEdit_textChanged(const QString&)
{
	qWarning("TODO: %s	%d",__FILE__,__LINE__);
}

void PowerGUI::on_PowerGUIPayload3PowerOnEclipseLineEdit_textChanged(const QString&)
{
	qWarning("TODO: %s	%d",__FILE__,__LINE__);
}

void PowerGUI::on_PowerGUIPayload3PowerOnDaylightLineEdit_textChanged(const QString&)
{
	qWarning("TODO: %s	%d",__FILE__,__LINE__);
}

void PowerGUI::on_PowerGUIPayload4PowerOnEclipseLineEdit_textChanged(const QString&)
{
	qWarning("TODO: %s	%d",__FILE__,__LINE__);
}

void PowerGUI::on_PowerGUIPayload4PowerOnDaylightLineEdit_textChanged(const QString&)
{
	qWarning("TODO: %s	%d",__FILE__,__LINE__);
}

void PowerGUI::on_PowerGUISolarCellTypeComboBox_activated(const QString&)
{
    SCVirtual->getNewSCPower()->SolarArrays.setSolarCell
            (PowerGUISolarCellTypeComboBox->currentText());

    qDebug()<<"CURRENT TEXT" << PowerGUISolarCellTypeComboBox->currentText();

    // calculate and visualize BOL power of solar cell
//    SCVirtual->getNewSCPower()->SolarArrays.CalculateSolarCellBOLPower();
    PowerGUIBOLPowerLineEdit->setText
            (QString::number(SCVirtual->getNewSCPower()->SolarArrays.getSolarCellBOLPower()));

    //calculate and visualize EOL power of solar cell
//    SCVirtual->getNewSCPower()->SolarArrays.CalculateSolarCellEOLPower();
    PowerGUIEOLPowerLineEdit->setText
            (QString::number(SCVirtual->getNewSCPower()->SolarArrays.getSolarCellEOLPower()));

//    //calculate SAEOLPower needed
    SCVirtual->getNewSCPower()->SolarArrays.CalculateSAEOLPower();
    SolarArrayEOLPowerLineEdit->setText
            (QString::number(SCVirtual->getNewSCPower()->SolarArrays.getSAEOLPower()));

//    //calculate and visualize array of the solar array
    SCVirtual->getNewSCPower()->SolarArrays.CalculateArea();
    PowerGUISolarArrayAreaLineEdit->setText
            (QString::number(SCVirtual->getNewSCPower()->SolarArrays.getArea()));

    //calculate SABOLPower needed
    SolarArrayBOLPowerLineEdit->setText
            (QString::number(SCVirtual->getNewSCPower()->SolarArrays.getSABOLPower()));

    qDebug()<< "*-" <<SCVirtual->getNewSCPower()->SolarArrays.getPCUMass();
    qDebug()<< "*--" <<SCVirtual->getNewSCPower()->SolarArrays.getSAMass();

	qWarning("TODO: %s	%d",__FILE__,__LINE__);
}

void PowerGUI::on_PowerGUIBatteryTypeComboBox_activated(const QString&)
{ 
    SCVirtual->getNewSCPower()->Battery.setBatteryProperties
            (PowerGUIBatteryTypeComboBox->currentText());

      qDebug()<<"CURRENT TEXT" << PowerGUIBatteryTypeComboBox->currentText();

//    SCVirtual->getNewSCPower()->Battery.CalculateAndSetNumberOfBatteries();
    PowerGUIBatteryNumberLineEdit->setText
            (QString::number(SCVirtual->getNewSCPower()->Battery.getNumberOfBatteries()));

//    SCVirtual->getNewSCPower()->Battery.CalculateAndSetBMass();
    qDebug()<< "*---" <<SCVirtual->getNewSCPower()->Battery.getBMass();

	qWarning("TODO: %s	%d",__FILE__,__LINE__);
}

void PowerGUI::on_PowerGUISavePushButton_clicked()
{
    SCVirtual->PassPowerSubsystemOutputParameters();
    this->close();
	qWarning("TODO: %s	%d",__FILE__,__LINE__);
}


