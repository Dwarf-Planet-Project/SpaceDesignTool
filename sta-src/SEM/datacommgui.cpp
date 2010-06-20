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

#include "datacommgui.h"

DataCommGUI::DataCommGUI( SemMain * SC, QWidget * parent, Qt::WindowFlags f) : QDialog(parent,f)
{
	setupUi(this);
        SCVirtual = SC;

        //Display the payload subsystem properties
        DataHandlingPayload1NameLineEdit->setText
                ((SCVirtual->getNewSCDataHandling()->getPayloadOBDHInfo()+0)->Name);
        DataHandlingPayload1DataRateLineEdit->setText
                (QString::number
                 ((SCVirtual->getNewSCDataHandling()->getPayloadOBDHInfo()+0)->DataRate));
        DataHandlingPayload1PowerOnPerOrbitLineEdit->setText
                (QString::number
                 ((SCVirtual->getNewSCDataHandling()->getPayloadOBDHInfo()+0)->PowerOnTimeInDaylight
                  + (SCVirtual->getNewSCDataHandling()->getPayloadOBDHInfo()+0)->PowerOnTimeInEclipse));

        DataHandlingPayload2NameLineEdit->setText
                ((SCVirtual->getNewSCDataHandling()->getPayloadOBDHInfo()+1)->Name);
        DataHandlingPayload2DataRateLineEdit->setText
                (QString::number
                 ((SCVirtual->getNewSCDataHandling()->getPayloadOBDHInfo()+1)->DataRate));
        DataHandlingPayload2PowerOnPerOrbitLineEdit->setText
                (QString::number
                 ((SCVirtual->getNewSCDataHandling()->getPayloadOBDHInfo()+1)->PowerOnTimeInDaylight
                  + (SCVirtual->getNewSCDataHandling()->getPayloadOBDHInfo()+1)->PowerOnTimeInEclipse));

        DataHandlingPayload3NameLineEdit->setText
                ((SCVirtual->getNewSCDataHandling()->getPayloadOBDHInfo()+2)->Name);
        DataHandlingPayload3DataRateLineEdit->setText
                (QString::number
                 ((SCVirtual->getNewSCDataHandling()->getPayloadOBDHInfo()+2)->DataRate));
        DataHandlingPayload3PowerOnPerOrbitLineEdit->setText
                (QString::number
                 ((SCVirtual->getNewSCDataHandling()->getPayloadOBDHInfo()+2)->PowerOnTimeInDaylight
                  + (SCVirtual->getNewSCDataHandling()->getPayloadOBDHInfo()+2)->PowerOnTimeInEclipse));

        DataHandlingPayload4NameLineEdit->setText
                ((SCVirtual->getNewSCDataHandling()->getPayloadOBDHInfo()+3)->Name);
        DataHandlingPayload4DataRateLineEdit->setText
                (QString::number
                 ((SCVirtual->getNewSCDataHandling()->getPayloadOBDHInfo()+3)->DataRate));
        DataHandlingPayload4PowerOnPerOrbitLineEdit->setText
                (QString::number
                 ((SCVirtual->getNewSCDataHandling()->getPayloadOBDHInfo()+3)->PowerOnTimeInDaylight
                  + (SCVirtual->getNewSCDataHandling()->getPayloadOBDHInfo()+3)->PowerOnTimeInEclipse));

        //Display the existing values of the OBDH subsystem
        NumberOfOrbitWithNoLinkLineEdit->setText
                (QString::number
                 (SCVirtual->getNewSCDataHandling()->getNumberOfOrbitWithNoLink()));
        DataHandlingMemorySizeLineEdit->setText
                (QString::number
                 (SCVirtual->getNewSCDataHandling()->getMemorySizeForPayloads()));

        //Transfer the calculated data to TTC subsystem
        SCVirtual->getNewSCCommunication()->setMemorySizeForPayloads
                (SCVirtual->getNewSCDataHandling()->getMemorySizeForPayloads());

        //Display the existing values of the TTC subsystem
        TTC_TXPowerLineEdit->setText
                (QString::number
                 (SCVirtual->getNewSCCommunication()->getAntennaPower()));
        TTCLinkDurationLineEdit->setText
                (QString::number
                 (SCVirtual->getNewSCCommunication()->getPercentageContactTimePerOrbit()));
        SCVirtual->getNewSCCommunication()->CalculateAndSetDownLinkRate();
        TTCLinkDataRateLineEdit->setText
                (QString::number
                 (SCVirtual->getNewSCCommunication()->getDownLinkRate()));

        TTCAntennaEfficiencyLineEdit->setText
                (QString::number
                 (SCVirtual->getNewSCCommunication()->getAntennaEfficiency()));
        TTCAntennaDiameterLineEdit->setText
                (QString::number
                 (SCVirtual->getNewSCCommunication()->getAntennaDiameter()));
        TTCAntennaGainLineEdit->setText
                (QString::number
                 (SCVirtual->getNewSCCommunication()->getAntennaGain()));
        TTCAntennaFrequencyLineEdit->setText
                (QString::number
                 (SCVirtual->getNewSCCommunication()->getAntennaFrequency()));

        //setting comboBox from a QString
        int i;
        for (i=0;i<CodingTypeComboBox->count();i++)
            if (CodingTypeComboBox->itemText(i)
                == SC->getNewSCDataHandling()->getCodingName())
                CodingTypeComboBox->setCurrentIndex(i);
}

DataCommGUI::~DataCommGUI()
{
}

void DataCommGUI::on_CodingTypeComboBox_activated(const QString &)
{
    SCVirtual->getNewSCDataHandling()->setCodingPercentage
            (CodingTypeComboBox->currentText());
        qWarning("TODO: %s	%d",__FILE__,__LINE__);
}

void DataCommGUI::on_NumberOfOrbitWithNoLinkLineEdit_textChanged(const QString &)
{
    SCVirtual->getNewSCDataHandling()->setNumberOfOrbitWithNolink
            (NumberOfOrbitWithNoLinkLineEdit->text().toInt());
            qWarning("TODO: %s	%d",__FILE__,__LINE__);
}

void DataCommGUI::on_TTC_TXPowerLineEdit_textChanged(const QString&)
{
    SCVirtual->getNewSCCommunication()->setAntennaPower
            (TTC_TXPowerLineEdit->text().toDouble());

	qWarning("TODO: %s	%d",__FILE__,__LINE__);
}

void DataCommGUI::on_TTCLinkDurationLineEdit_textEdited(const QString&)
{
    SCVirtual->getNewSCCommunication()->setPercentageOfContactTimePerOrbit
            (TTCLinkDurationLineEdit->text().toDouble());

    qDebug()<<"PercenATGAGAG";
    qDebug()<< SCVirtual->getNewSCCommunication()->getPercentageContactTimePerOrbit();

    //calculate neded mininumum downlink datarate for this configuration
    SCVirtual->getNewSCCommunication()->CalculateAndSetDownLinkRate();
    TTCLinkDataRateLineEdit->setText
            (QString::number
             (SCVirtual->getNewSCCommunication()->getDownLinkRate()));

	qWarning("TODO: %s	%d",__FILE__,__LINE__);
}

void DataCommGUI::on_TTCLinkDataRateLineEdit_textEdited(const QString&)
{
    SCVirtual->getNewSCCommunication()->setDownlinkRate
            (TTCLinkDataRateLineEdit->text().toDouble());

    qDebug()<<"Data RATATATATE";
    qDebug()<< SCVirtual->getNewSCCommunication()->getDownLinkRate();

//    //calculate neded mininumum contact duration time for this configuration
    SCVirtual->getNewSCCommunication()->CalculateAndSetContactTimePerOrbit();
    TTCLinkDurationLineEdit->setText
            (QString::number
             (SCVirtual->getNewSCCommunication()->getPercentageContactTimePerOrbit()));

	qWarning("TODO: %s	%d",__FILE__,__LINE__);
}

void DataCommGUI::on_TTCAntennaEfficiencyLineEdit_textChanged(const QString&)
{
    SCVirtual->getNewSCCommunication()->setAntennaEfficiency
            (TTCAntennaEfficiencyLineEdit->text().toDouble());

//    qDebug()<<SCVirtual->getNewSCCommunication()->getAntennaEfficiency();

    SCVirtual->getNewSCCommunication()->CalculateAndSetAntennaDiameter();

    TTCAntennaDiameterLineEdit->setText
            (QString::number
             (SCVirtual->getNewSCCommunication()->getAntennaDiameter()));

    qDebug()<<SCVirtual->getNewSCCommunication()->getAntennaDiameter();

        qWarning("TODO: %s	%d",__FILE__,__LINE__);
}

void DataCommGUI::on_TTCAntennaGainLineEdit_textChanged(const QString&)
{
    SCVirtual->getNewSCCommunication()->setAntennaGain
            (TTCAntennaGainLineEdit->text().toDouble());

//    qDebug()<<SCVirtual->getNewSCCommunication()->getAntennaGain();

    SCVirtual->getNewSCCommunication()->CalculateAndSetAntennaDiameter();

    TTCAntennaDiameterLineEdit->setText
            (QString::number
             (SCVirtual->getNewSCCommunication()->getAntennaDiameter()));

    qDebug()<<SCVirtual->getNewSCCommunication()->getAntennaDiameter();

        qWarning("TODO: %s	%d",__FILE__,__LINE__);
}

void DataCommGUI::on_TTCAntennaFrequencyLineEdit_textChanged(const QString&)
{
    SCVirtual->getNewSCCommunication()->setAntennaFrequency
            (TTCAntennaFrequencyLineEdit->text().toDouble());

//    qDebug()<<SCVirtual->getNewSCCommunication()->getAntennaFrequency();

    SCVirtual->getNewSCCommunication()->CalculateAndSetAntennaDiameter();

    qDebug()<<SCVirtual->getNewSCCommunication()->getAntennaDiameter();

    TTCAntennaDiameterLineEdit->setText
            (QString::number
             (SCVirtual->getNewSCCommunication()->getAntennaDiameter()));

        qWarning("TODO: %s	%d",__FILE__,__LINE__);
}

void DataCommGUI::on_DataCommCalculatePushButton_clicked()
{
    SCVirtual->getNewSCDataHandling()->CalculateAndSetMemorySizeForPayloads();

    //Transfer the calculated data to TTC subsystem
    SCVirtual->getNewSCCommunication()->setMemorySizeForPayloads
            (SCVirtual->getNewSCDataHandling()->getMemorySizeForPayloads());

    DataHandlingMemorySizeLineEdit->setText
            (QString::number(SCVirtual->getNewSCDataHandling()->getMemorySizeForPayloads()));
	qWarning("TODO: %s	%d",__FILE__,__LINE__);
}

void DataCommGUI::on_DataCommSavePushButton_clicked()
{
    SCVirtual->PassOBDHSubsystemOutputParameters();
    SCVirtual->PassTTCSubsystemOutputParameters();

    this->close();
	qWarning("TODO: %s	%d",__FILE__,__LINE__);
}

//void DataCommGUI::on_DataCommClosePushButton_clicked()
//{
//	qWarning("TODO: %s	%d",__FILE__,__LINE__);
//}

