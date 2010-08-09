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

#include "semmaingui.h"

#include "QDebug"

//to propogate after closing the wizard if the start and end time is changed
#include "Loitering/loitering.h"
#include "Scenario/propagationfeedback.h"
#include "Astro-Core/EclipseDuration.h"

#include "datacommgui.h"
//#include "launchergui.h"
#include "powergui.h"
#include "structuregui.h"
#include "thermalgui.h"

#include "Plotting/plottingtool.h"
#include "Plotting/PlotGraphFromFile.h"
#include "Plotting/PlotView.h"

SemMainGUI::SemMainGUI(ScenarioSC* SCVehicle,
                       QString missionArc,
                       QWidget * parent,
                       Qt::WindowFlags f) : QDialog(parent,f)
{
	setupUi(this);

        //collect the scenario information
        Vehicle = SCVehicle;

        //set the missionarc name
        MissionArc = missionArc;

        RetrieveScenarioSC();

        if (!(SC.getSCMissionDetails()->getPlanetProperties()->Planet == "Earth"))
            LauncherGroupBox->setEnabled(0);
        else
            LauncherGroupBox->setEnabled(1);

        // ---------------------------------- VALIDATION AND VERIFICATION START
        qDebug()<<"----------GUI---------------VALIDATION AND VERIFICATION START";
        qDebug()<<"Payload1 datarate"<<(SC.getNewSCDataHandling()->getPayloadOBDHInfo()+0)->DataRate;
        qDebug()<<"Payload2 datarate"<<(SC.getNewSCDataHandling()->getPayloadOBDHInfo()+1)->DataRate;
        qDebug()<<"MemorySizeForPayloads"<<SC.getNewSCDataHandling()->getMemorySizeForPayloads();
        qDebug()<<"OBDHSubsystemMass"<<SC.getNewSCDataHandling()->getOBDHSubsystemMass();
        qDebug()<<"OBDHSubsystemVolume"<<SC.getNewSCDataHandling()->getOBDHSubsystemVolume();
        qDebug()<<"OBDHSubsystemPower"<<SC.getNewSCDataHandling()->getOBDHSubsystemPower();
        qDebug()<<"";
        qDebug()<<"AntennaDiameter"<<SC.getNewSCCommunication()->getAntennaDiameter();
        qDebug()<<"ContactTimePerOrbit"<<SC.getNewSCCommunication()->getContactTimePerOrbit();
        qDebug()<<"DownLinkRate"<<SC.getNewSCCommunication()->getDownLinkRate();
        qDebug()<<"TTCSubsystemMass"<<SC.getNewSCCommunication()->getTTCSubsystemMass();
        qDebug()<<"TTCSubsystemVolume"<<SC.getNewSCCommunication()->getTTCSubsystemVolume();
        qDebug()<<"TTCSubsystemPower"<<SC.getNewSCCommunication()->getTTCSubsystemPower();
        qDebug()<<"";
        qDebug()<<"SCPower"<<SC.getNewSCPower()->getSCPower();
        qDebug()<<"SolarArrayArea"<<SC.getNewSCPower()->SolarArrays.getArea();
        qDebug()<<"PowersubsystemMass"<<SC.getNewSCPower()->getPowersubsystemMass();
        qDebug()<<"PowerSubsystemVolume"<<SC.getNewSCPower()->getPowerSubsystemVolume();
        qDebug()<<"";
        qDebug()<<"SCMass"<<SC.getNewSCStructure()->getSCMass();
        qDebug()<<"SCVolume"<<SC.getNewSCStructure()->getSCVolume();
        qDebug()<<"SpacecraftDimension().x()"<<SC.getNewSCStructure()->getSpacecraftDimension().x();
        qDebug()<<"SpacecraftDimension().y()"<<SC.getNewSCStructure()->getSpacecraftDimension().y();
        qDebug()<<"SpacecraftDimension().z()"<<SC.getNewSCStructure()->getSpacecraftDimension().z();
        qDebug()<<"AreaOfColdFace"<<SC.getNewSCStructure()->getSCThermalDetails()->AreaOfColdFace;
        qDebug()<<"AreaOfHotFace"<<SC.getNewSCStructure()->getSCThermalDetails()->AreaOfHotFace;
        qDebug()<<"TotalAreaOfColdFace"<<SC.getNewSCStructure()->getSCThermalDetails()->TotalAreaOfColdFace;
        qDebug()<<"TotalAreaOfHotFace"<<SC.getNewSCStructure()->getSCThermalDetails()->TotalAreaOfHotFace;
        qDebug()<<"";
        qDebug()<<"AlbedoHeat"<<SC.getNewSCThermal()->getAlbedoHeat();
        qDebug()<<"PlanetIRHeat"<<SC.getNewSCThermal()->getPlanetIRHeat();
        qDebug()<<"SolarFluxHeat"<<SC.getNewSCThermal()->getSolarFluxHeat();
        qDebug()<<"MaximumSCTemperature"<<SC.getNewSCThermal()->getMaximumSCTemperature();
        qDebug()<<"MinimumSCTemperature"<<SC.getNewSCThermal()->getMinimumSCTemperature();
        qDebug()<<"MaximumSCTempWithRadiatorOrHeater"<<SC.getNewSCThermal()->getMaximumSCTempWithRadiatorOrHeater();
        qDebug()<<"MinimumSCTempWithRadiatorOrHeater"<<SC.getNewSCThermal()->getMinimumSCTempWithRadiatorOrHeater();
        qDebug()<<"Cold-Absorptivity"<<SC.getNewSCThermal()->getColdFaceCoatingProperties().Absorptivity;
        qDebug()<<"Cold-Emmissivity"<<SC.getNewSCThermal()->getColdFaceCoatingProperties().Emmissivity;
        qDebug()<<"Hot-Absorptivity"<<SC.getNewSCThermal()->getHotFaceCoatingProperties().Absorptivity;
        qDebug()<<"Hot-Emmissivity"<<SC.getNewSCThermal()->getHotFaceCoatingProperties().Emmissivity;
        qDebug()<<"NeededHeater"<<SC.getNewSCThermal()->getNeededHeater();
        qDebug()<<"NeededRadiator"<<SC.getNewSCThermal()->getNeededRadiator();
        qDebug()<<"--------------------------VALIDATION AND VERIFICATION END";
        // ---------------------------------- VALIDATION AND VERIFICATION END

        if (!(SC.getSCMissionDetails()->getPlanetProperties()->Planet == "Earth"))
            LauncherGroupBox->setEnabled(0);
        RefreshSemMainGUI();
        RefreshSemMainGUIPayload();

}

SemMainGUI::SemMainGUI(SemMain* SCWizard,
                       ScenarioSC* SCVehicle,
                       QWidget *parent,
                       Qt::WindowFlags f)
{
    setupUi(this);

    //collect the data from wizard
    SC = *SCWizard;

    //collect the scenario information
    Vehicle = SCVehicle;

    if (!(SC.getSCMissionDetails()->getPlanetProperties()->Planet == "Earth"))
        LauncherGroupBox->setEnabled(0);
    else
        LauncherGroupBox->setEnabled(1);

    RefreshSemMainGUI();
    RefreshSemMainGUIPayload();

}

SemMainGUI::~SemMainGUI()
{
}

void SemMainGUI::on_MainSemTabWidget_currentChanged(int Index)
{
    if (Index == 0)
    {
        //user selected preliminary design window

        //for now get payload the names on the GUI
        (SC.getNewPayloads()+0)->setPayloadName
                (Payload1NameLabel->text());
        (SC.getNewPayloads()+1)->setPayloadName
                (Payload2NameLabel->text());
        (SC.getNewPayloads()+2)->setPayloadName
                (Payload3NameLabel->text());
        (SC.getNewPayloads()+3)->setPayloadName
                (Payload4NameLabel->text());

        SC.PassPayloadOutputParameters();

        RefreshSemMainGUI();
    }

        qWarning("TODO: %s	%d",__FILE__,__LINE__);
}

void SemMainGUI::on_Payload1WidthLineEdit_textEdited(const QString&)
{
    //connect the GUI value to the object
    SC.getNewPayloads()->setPayloadWidth
            (Payload1WidthLineEdit->text().toDouble());

    //Payload Volume Calculations
    SC.getNewPayloads()->CalculateAndSetPayloadVolume("Rectangular_Prism");
    Payload1VolumeLineEdit->setText
            (QString::number(SC.getNewPayloads()->getPayloadVolume()));

//    // SC dimension volume&dimension calculations and settings
//    SC.getNewSCStructure()->CalculateAndSetSCVolume(0.0);
//    SC.getNewSCStructure()->CalculateAndSetSpacecraftDimension();
//    SCWidthLineEdit->setText
//            (QString::number(SC.getNewSCStructure()->getSpacecraftDimension().x()));
//    SCHeightLineEdit->setText
//            (QString::number(SC.getNewSCStructure()->getSpacecraftDimension().y()));
//    SCLengthLineEdit->setText
//            (QString::number(SC.getNewSCStructure()->getSpacecraftDimension().z()));

	qWarning("TODO: %s	%d",__FILE__,__LINE__);
}

void SemMainGUI::on_Payload1LengthLineEdit_textEdited(const QString&)
{
    //connect the GUI value to the object
    SC.getNewPayloads()->setPayloadLength(Payload1LengthLineEdit->text().toDouble());

    //Payload Volume calculations
    SC.getNewPayloads()->CalculateAndSetPayloadVolume("Rectangular_Prism");
    Payload1VolumeLineEdit->setText
            (QString::number(SC.getNewPayloads()->getPayloadVolume()));

//    // SC dimension volume&dimension calculations and settings
//    SC.getNewSCStructure()->CalculateAndSetSCVolume(0.0);
//    SC.getNewSCStructure()->CalculateAndSetSpacecraftDimension();
//    SCWidthLineEdit->setText
//            (QString::number(SC.getNewSCStructure()->getSpacecraftDimension().x()));
//    SCHeightLineEdit->setText
//            (QString::number(SC.getNewSCStructure()->getSpacecraftDimension().y()));
//    SCLengthLineEdit->setText
//            (QString::number(SC.getNewSCStructure()->getSpacecraftDimension().z()));

	qWarning("TODO: %s	%d",__FILE__,__LINE__);
}

void SemMainGUI::on_Payload1HeightLineEdit_textEdited(const QString&)
{
    //connect the GUI value to the object
    SC.getNewPayloads()->setPayloadHeight(Payload1HeightLineEdit->text().toDouble());

    //Payload volume calculations
    SC.getNewPayloads()->CalculateAndSetPayloadVolume("Rectangular_Prism");
    Payload1VolumeLineEdit->setText
            (QString::number(SC.getNewPayloads()->getPayloadVolume()));

//    // SC dimension volume&dimension calculations and settings
//    SC.getNewSCStructure()->CalculateAndSetSCVolume(0.0);
//    SC.getNewSCStructure()->CalculateAndSetSpacecraftDimension();
//    SCWidthLineEdit->setText
//            (QString::number(SC.getNewSCStructure()->getSpacecraftDimension().x()));
//    SCHeightLineEdit->setText
//            (QString::number(SC.getNewSCStructure()->getSpacecraftDimension().y()));
//    SCLengthLineEdit->setText
//            (QString::number(SC.getNewSCStructure()->getSpacecraftDimension().z()));

	qWarning("TODO: %s	%d",__FILE__,__LINE__);
}

void SemMainGUI::on_Payload1MassLineEdit_textChanged(const QString&)
{
    //passing mass value from GUI to object
    SC.getNewPayloads()->setPayloadMass(Payload1MassLineEdit->text().toDouble());
    qDebug()<<"PayloadMassEntry"<<SC.getNewPayloads()->getPayloadMass();

        qWarning("TODO: %s	%d",__FILE__,__LINE__);
}

void SemMainGUI::on_Payload1PowerConsumptionInEclipseLineEdit_textChanged(const QString&)
{
    //passing power value from GUI to object
    (SC.getNewPayloads()+0)->setPayloadPowerInEclipse
            (Payload1PowerConsumptionInEclipseLineEdit->text().toDouble());

	qWarning("TODO: %s	%d",__FILE__,__LINE__);
}

void SemMainGUI::on_Payload1PowerConsumptionInDaylightLineEdit_textChanged(const QString &)
{
    //passing power value from GUI to object
    (SC.getNewPayloads()+0)->setPayloadPowerInDaylight
            (Payload1PowerConsumptionInDaylightLineEdit->text().toDouble());

    qWarning("TODO: %s	%d",__FILE__,__LINE__);
}

void SemMainGUI::on_Payload1PowerOnTimeEclipseLineEdit_textChanged(const QString&)
{
    //passing power time value from GUI to object
    (SC.getNewPayloads()+0)->setPayloadPowerOnPercentageInEclipse
            (Payload1PowerOnTimeEclipseLineEdit->text().toDouble());

	qWarning("TODO: %s	%d",__FILE__,__LINE__);
}

void SemMainGUI::on_Payload1PowerOnTimeDaylightLineEdit_textChanged(const QString&)
{
    //passing power time value from GUI to object
    (SC.getNewPayloads()+0)->setPayloadPowerOnPercentageInDaylight
            (Payload1PowerOnTimeDaylightLineEdit->text().toDouble());

	qWarning("TODO: %s	%d",__FILE__,__LINE__);
}

void SemMainGUI::on_Payload1MaxTempLimitLineEdit_textChanged(const QString&)
{
    (SC.getNewPayloads()+0)->setPayloadMaximumTemperature
            (Payload1MaxTempLimitLineEdit->text().toDouble());

//    //transfering PayloadSubsystem Info to ThermalSubsystem
//    int i;
//    for (i=0; i<4; i++)
//    {
//        SC.getNewSCThermal()->setMaximumPayloadTemperature
//                (i,(SC.getNewPayloads()+i)->getPayloadMaximumTemperature());
//    }

	qWarning("TODO: %s	%d",__FILE__,__LINE__);
}

void SemMainGUI::on_Payload1MinTempLimitLineEdit_textChanged(const QString&)
{
    (SC.getNewPayloads()+0)->setPayloadMinimumTemperature
            (Payload1MinTempLimitLineEdit->text().toDouble());

//    //transfering PayloadSubsystem Info to ThermalSubsystem
//    int i;
//    for (i=0; i<4; i++)
//    {
//        SC.getNewSCThermal()->setMinimumPayloadTemperature
//                (i,(SC.getNewPayloads()+i)->getPayloadMinimumTemperature());
//    }

	qWarning("TODO: %s	%d",__FILE__,__LINE__);
}

void SemMainGUI::on_Payload1DataRateLineEdit_textChanged(const QString&)
{
    (SC.getNewPayloads()+0)->setPayloadDataRate
            (Payload1DataRateLineEdit->text().toDouble());
	qWarning("TODO: %s	%d",__FILE__,__LINE__);
}

void SemMainGUI::on_Payload2WidthLineEdit_textChanged(const QString&)
{
    //connect the GUI value to the object
    (SC.getNewPayloads()+1)->setPayloadWidth
            (Payload2WidthLineEdit->text().toDouble());

    //Payload Volume Calculations
    (SC.getNewPayloads()+1)->CalculateAndSetPayloadVolume("Rectangular_Prism");
    Payload2VolumeLineEdit->setText
            (QString::number((SC.getNewPayloads()+1)->getPayloadVolume()));

//    // SC dimension volume&dimension calculations and settings
//    SC.getNewSCStructure()->CalculateAndSetSCVolume(0.0);
//    SC.getNewSCStructure()->CalculateAndSetSpacecraftDimension();
//    SCWidthLineEdit->setText
//            (QString::number(SC.getNewSCStructure()->getSpacecraftDimension().x()));
//    SCHeightLineEdit->setText
//            (QString::number(SC.getNewSCStructure()->getSpacecraftDimension().y()));
//    SCLengthLineEdit->setText
//            (QString::number(SC.getNewSCStructure()->getSpacecraftDimension().z()));
	qWarning("TODO: %s	%d",__FILE__,__LINE__);
}

void SemMainGUI::on_Payload2LengthLineEdit_textChanged(const QString&)
{
    //connect the GUI value to the object
    (SC.getNewPayloads()+1)->setPayloadLength(Payload2LengthLineEdit->text().toDouble());

    //Payload Volume calculations
    (SC.getNewPayloads()+1)->CalculateAndSetPayloadVolume("Rectangular_Prism");
    Payload2VolumeLineEdit->setText
            (QString::number((SC.getNewPayloads()+1)->getPayloadVolume()));

//    // SC dimension volume&dimension calculations and settings
//    SC.getNewSCStructure()->CalculateAndSetSCVolume(0.0);
//    SC.getNewSCStructure()->CalculateAndSetSpacecraftDimension();
//    SCWidthLineEdit->setText
//            (QString::number(SC.getNewSCStructure()->getSpacecraftDimension().x()));
//    SCHeightLineEdit->setText
//            (QString::number(SC.getNewSCStructure()->getSpacecraftDimension().y()));
//    SCLengthLineEdit->setText
//            (QString::number(SC.getNewSCStructure()->getSpacecraftDimension().z()));

	qWarning("TODO: %s	%d",__FILE__,__LINE__);
}

void SemMainGUI::on_Payload2HeightLineEdit_textChanged(const QString&)
{
    //connect the GUI value to the object
    (SC.getNewPayloads()+1)->setPayloadHeight(Payload2HeightLineEdit->text().toDouble());

    //Payload volume calculations
    (SC.getNewPayloads()+1)->CalculateAndSetPayloadVolume("Rectangular_Prism");
    Payload2VolumeLineEdit->setText
            (QString::number((SC.getNewPayloads()+1)->getPayloadVolume()));

//    // SC dimension volume&dimension calculations and settings
//    SC.getNewSCStructure()->CalculateAndSetSCVolume(0.0);
//    SC.getNewSCStructure()->CalculateAndSetSpacecraftDimension();
//    SCWidthLineEdit->setText
//            (QString::number(SC.getNewSCStructure()->getSpacecraftDimension().x()));
//    SCHeightLineEdit->setText
//            (QString::number(SC.getNewSCStructure()->getSpacecraftDimension().y()));
//    SCLengthLineEdit->setText
//            (QString::number(SC.getNewSCStructure()->getSpacecraftDimension().z()));
	qWarning("TODO: %s	%d",__FILE__,__LINE__);
}

void SemMainGUI::on_Payload2MassLineEdit_textChanged(const QString&)
{
    //passing mass value from GUI to object
    (SC.getNewPayloads()+1)->setPayloadMass(Payload2MassLineEdit->text().toDouble());

	qWarning("TODO: %s	%d",__FILE__,__LINE__);
}

void SemMainGUI::on_Payload2PowerConsumptionInEclipseLineEdit_textChanged(const QString &)
{
    //passing power value from GUI to object
    (SC.getNewPayloads()+1)->setPayloadPowerInEclipse
            (Payload2PowerConsumptionInEclipseLineEdit->text().toDouble());

    qWarning("TODO: %s	%d",__FILE__,__LINE__);
}

void SemMainGUI::on_Payload2PowerConsumptionInDaylightLineEdit_textChanged(const QString &)
{
    //passing power value from GUI to object
    (SC.getNewPayloads()+1)->setPayloadPowerInDaylight
            (Payload2PowerConsumptionInDaylightLineEdit->text().toDouble());

    qWarning("TODO: %s	%d",__FILE__,__LINE__);
}

void SemMainGUI::on_Payload2PowerOnTimeEclipseLineEdit_textChanged(const QString&)
{
    //passing power time value from GUI to object
    (SC.getNewPayloads()+1)->setPayloadPowerOnPercentageInEclipse
            (Payload2PowerOnTimeEclipseLineEdit->text().toDouble());

	qWarning("TODO: %s	%d",__FILE__,__LINE__);
}

void SemMainGUI::on_Payload2PowerOnTimeDaylightLineEdit_textChanged(const QString&)
{
    //passing power time value from GUI to object
    (SC.getNewPayloads()+1)->setPayloadPowerOnPercentageInDaylight
            (Payload2PowerOnTimeDaylightLineEdit->text().toDouble());

	qWarning("TODO: %s	%d",__FILE__,__LINE__);
}

void SemMainGUI::on_Payload2MaxTempLimitLineEdit_textChanged(const QString&)
{
    (SC.getNewPayloads()+1)->setPayloadMaximumTemperature
            (Payload2MaxTempLimitLineEdit->text().toDouble());

	qWarning("TODO: %s	%d",__FILE__,__LINE__);
}

void SemMainGUI::on_Payload2MinTempLimitLineEdit_textChanged(const QString&)
{
    (SC.getNewPayloads()+1)->setPayloadMinimumTemperature
            (Payload2MinTempLimitLineEdit->text().toDouble());

	qWarning("TODO: %s	%d",__FILE__,__LINE__);
}

void SemMainGUI::on_Payload2DataRateLineEdit_textChanged(const QString&)
{
    (SC.getNewPayloads()+1)->setPayloadDataRate
            (Payload2DataRateLineEdit->text().toDouble());
	qWarning("TODO: %s	%d",__FILE__,__LINE__);
}

void SemMainGUI::on_Payload3WidthLineEdit_textChanged(const QString&)
{
    //connect the GUI value to the object
    (SC.getNewPayloads()+2)->setPayloadWidth
            (Payload3WidthLineEdit->text().toDouble());

    //Payload Volume Calculations
    (SC.getNewPayloads()+2)->CalculateAndSetPayloadVolume("Rectangular_Prism");
    Payload3VolumeLineEdit->setText
            (QString::number((SC.getNewPayloads()+2)->getPayloadVolume()));

//    // SC dimension volume&dimension calculations and settings
//    SC.getNewSCStructure()->CalculateAndSetSCVolume(0.0);
//    SC.getNewSCStructure()->CalculateAndSetSpacecraftDimension();
//    SCWidthLineEdit->setText
//            (QString::number(SC.getNewSCStructure()->getSpacecraftDimension().x()));
//    SCHeightLineEdit->setText
//            (QString::number(SC.getNewSCStructure()->getSpacecraftDimension().y()));
//    SCLengthLineEdit->setText
//            (QString::number(SC.getNewSCStructure()->getSpacecraftDimension().z()));

	qWarning("TODO: %s	%d",__FILE__,__LINE__);
}

void SemMainGUI::on_Payload3LengthLineEdit_textChanged(const QString&)
{
    //connect the GUI value to the object
    (SC.getNewPayloads()+2)->setPayloadLength(Payload3LengthLineEdit->text().toDouble());

    //Payload Volume calculations
    (SC.getNewPayloads()+2)->CalculateAndSetPayloadVolume("Rectangular_Prism");
    Payload3VolumeLineEdit->setText
            (QString::number((SC.getNewPayloads()+2)->getPayloadVolume()));

//    // SC dimension volume&dimension calculations and settings
//    SC.getNewSCStructure()->CalculateAndSetSCVolume(0.0);
//    SC.getNewSCStructure()->CalculateAndSetSpacecraftDimension();
//    SCWidthLineEdit->setText
//            (QString::number(SC.getNewSCStructure()->getSpacecraftDimension().x()));
//    SCHeightLineEdit->setText
//            (QString::number(SC.getNewSCStructure()->getSpacecraftDimension().y()));
//    SCLengthLineEdit->setText
//            (QString::number(SC.getNewSCStructure()->getSpacecraftDimension().z()));

	qWarning("TODO: %s	%d",__FILE__,__LINE__);
}

void SemMainGUI::on_Payload3HeightLineEdit_textChanged(const QString&)
{
    //connect the GUI value to the object
    (SC.getNewPayloads()+2)->setPayloadHeight(Payload3HeightLineEdit->text().toDouble());

    //Payload volume calculations
    (SC.getNewPayloads()+2)->CalculateAndSetPayloadVolume("Rectangular_Prism");
    Payload3VolumeLineEdit->setText
            (QString::number((SC.getNewPayloads()+2)->getPayloadVolume()));

//    // SC dimension volume&dimension calculations and settings
//    SC.getNewSCStructure()->CalculateAndSetSCVolume(0.0);
//    SC.getNewSCStructure()->CalculateAndSetSpacecraftDimension();
//    SCWidthLineEdit->setText
//            (QString::number(SC.getNewSCStructure()->getSpacecraftDimension().x()));
//    SCHeightLineEdit->setText
//            (QString::number(SC.getNewSCStructure()->getSpacecraftDimension().y()));
//    SCLengthLineEdit->setText
//            (QString::number(SC.getNewSCStructure()->getSpacecraftDimension().z()));

	qWarning("TODO: %s	%d",__FILE__,__LINE__);
}

void SemMainGUI::on_Payload3MassLineEdit_textChanged(const QString&)
{
    //passing mass value from GUI to object
    (SC.getNewPayloads()+2)->setPayloadMass(Payload3MassLineEdit->text().toDouble());

	qWarning("TODO: %s	%d",__FILE__,__LINE__);
}

void SemMainGUI::on_Payload3PowerConsumptionInEclipseLineEdit_textChanged(const QString &)
{
    //passing power value from GUI to object
    (SC.getNewPayloads()+2)->setPayloadPowerInEclipse
            (Payload3PowerConsumptionInEclipseLineEdit->text().toDouble());

    qWarning("TODO: %s	%d",__FILE__,__LINE__);
}

void SemMainGUI::on_Payload3PowerConsumptionInDaylightLineEdit_textChanged(const QString &)
{
    //passing power value from GUI to object
    (SC.getNewPayloads()+2)->setPayloadPowerInDaylight
            (Payload3PowerConsumptionInDaylightLineEdit->text().toDouble());

    qWarning("TODO: %s	%d",__FILE__,__LINE__);
}

void SemMainGUI::on_Payload3PowerOnTimeEclipseLineEdit_textChanged(const QString&)
{
    //passing power time value from GUI to object
    (SC.getNewPayloads()+2)->setPayloadPowerOnPercentageInEclipse
            (Payload3PowerOnTimeEclipseLineEdit->text().toDouble());

	qWarning("TODO: %s	%d",__FILE__,__LINE__);
}

void SemMainGUI::on_Payload3PowerOnTimeDaylightLineEdit_textChanged(const QString&)
{
    //passing power time value from GUI to object
    (SC.getNewPayloads()+2)->setPayloadPowerOnPercentageInDaylight
            (Payload3PowerOnTimeDaylightLineEdit->text().toDouble());

	qWarning("TODO: %s	%d",__FILE__,__LINE__);
}

void SemMainGUI::on_Payload3MaxTempLimitLineEdit_textChanged(const QString&)
{
    (SC.getNewPayloads()+2)->setPayloadMaximumTemperature
            (Payload3MaxTempLimitLineEdit->text().toDouble());

	qWarning("TODO: %s	%d",__FILE__,__LINE__);
}

void SemMainGUI::on_Payload3MinTempLimitLineEdit_textChanged(const QString&)
{
    (SC.getNewPayloads()+2)->setPayloadMinimumTemperature
            (Payload3MinTempLimitLineEdit->text().toDouble());

	qWarning("TODO: %s	%d",__FILE__,__LINE__);
}

void SemMainGUI::on_Payload3DataRateLineEdit_textChanged(const QString&)
{
    (SC.getNewPayloads()+2)->setPayloadDataRate
            (Payload3DataRateLineEdit->text().toDouble());
	qWarning("TODO: %s	%d",__FILE__,__LINE__);
}

void SemMainGUI::on_Payload4WidthLineEdit_textChanged(const QString&)
{
    //connect the GUI value to the object
    (SC.getNewPayloads()+3)->setPayloadWidth
            (Payload4WidthLineEdit->text().toDouble());

    //Payload Volume Calculations
    (SC.getNewPayloads()+3)->CalculateAndSetPayloadVolume("Rectangular_Prism");
    Payload4VolumeLineEdit->setText
            (QString::number((SC.getNewPayloads()+3)->getPayloadVolume()));

//    // SC dimension volume&dimension calculations and settings
//    SC.getNewSCStructure()->CalculateAndSetSCVolume(0.0);
//    SC.getNewSCStructure()->CalculateAndSetSpacecraftDimension();
//    SCWidthLineEdit->setText
//            (QString::number(SC.getNewSCStructure()->getSpacecraftDimension().x()));
//    SCHeightLineEdit->setText
//            (QString::number(SC.getNewSCStructure()->getSpacecraftDimension().y()));
//    SCLengthLineEdit->setText
//            (QString::number(SC.getNewSCStructure()->getSpacecraftDimension().z()));

	qWarning("TODO: %s	%d",__FILE__,__LINE__);
}

void SemMainGUI::on_Payload4LengthLineEdit_textChanged(const QString&)
{
    //connect the GUI value to the object
    (SC.getNewPayloads()+3)->setPayloadLength(Payload4LengthLineEdit->text().toDouble());

    //Payload Volume calculations
    (SC.getNewPayloads()+3)->CalculateAndSetPayloadVolume("Rectangular_Prism");
    Payload4VolumeLineEdit->setText
            (QString::number((SC.getNewPayloads()+3)->getPayloadVolume()));

//    // SC dimension volume&dimension calculations and settings
//    SC.getNewSCStructure()->CalculateAndSetSCVolume(0.0);
//    SC.getNewSCStructure()->CalculateAndSetSpacecraftDimension();
//    SCWidthLineEdit->setText
//            (QString::number(SC.getNewSCStructure()->getSpacecraftDimension().x()));
//    SCHeightLineEdit->setText
//            (QString::number(SC.getNewSCStructure()->getSpacecraftDimension().y()));
//    SCLengthLineEdit->setText
//            (QString::number(SC.getNewSCStructure()->getSpacecraftDimension().z()));

	qWarning("TODO: %s	%d",__FILE__,__LINE__);
}

void SemMainGUI::on_Payload4HeightLineEdit_textChanged(const QString&)
{
    //connect the GUI value to the object
    (SC.getNewPayloads()+3)->setPayloadHeight(Payload4HeightLineEdit->text().toDouble());

    //Payload volume calculations
    (SC.getNewPayloads()+3)->CalculateAndSetPayloadVolume("Rectangular_Prism");
    Payload4VolumeLineEdit->setText
            (QString::number((SC.getNewPayloads()+3)->getPayloadVolume()));

//    // SC dimension volume&dimension calculations and settings
//    SC.getNewSCStructure()->CalculateAndSetSCVolume(0.0);
//    SC.getNewSCStructure()->CalculateAndSetSpacecraftDimension();
//    SCWidthLineEdit->setText
//            (QString::number(SC.getNewSCStructure()->getSpacecraftDimension().x()));
//    SCHeightLineEdit->setText
//            (QString::number(SC.getNewSCStructure()->getSpacecraftDimension().y()));
//    SCLengthLineEdit->setText
//            (QString::number(SC.getNewSCStructure()->getSpacecraftDimension().z()));

	qWarning("TODO: %s	%d",__FILE__,__LINE__);
}

void SemMainGUI::on_Payload4MassLineEdit_textChanged(const QString&)
{
    //passing mass value from GUI to object
    (SC.getNewPayloads()+3)->setPayloadMass(Payload4MassLineEdit->text().toDouble());

	qWarning("TODO: %s	%d",__FILE__,__LINE__);
}

void SemMainGUI::on_Payload4PowerConsumptionInEclipseLineEdit_textChanged(const QString &)
{
    //passing power value from GUI to object
    (SC.getNewPayloads()+3)->setPayloadPowerInEclipse
            (Payload4PowerConsumptionInEclipseLineEdit->text().toDouble());

    qWarning("TODO: %s	%d",__FILE__,__LINE__);
}

void SemMainGUI::on_Payload4PowerConsumptionInDaylightLineEdit_textChanged(const QString &)
{
    //passing power value from GUI to object
    (SC.getNewPayloads()+3)->setPayloadPowerInDaylight
            (Payload4PowerConsumptionInDaylightLineEdit->text().toDouble());

    qWarning("TODO: %s	%d",__FILE__,__LINE__);
}

void SemMainGUI::on_Payload4PowerOnTimeEclipseLineEdit_textChanged(const QString&)
{
    //passing power time value from GUI to object
    (SC.getNewPayloads()+3)->setPayloadPowerOnPercentageInEclipse
            (Payload4PowerOnTimeEclipseLineEdit->text().toDouble());

	qWarning("TODO: %s	%d",__FILE__,__LINE__);
}

void SemMainGUI::on_Payload4PowerOnTimeDaylightLineEdit_textChanged(const QString&)
{
    //passing power time value from GUI to object
    (SC.getNewPayloads()+3)->setPayloadPowerOnPercentageInDaylight
            (Payload4PowerOnTimeDaylightLineEdit->text().toDouble());

	qWarning("TODO: %s	%d",__FILE__,__LINE__);
}

void SemMainGUI::on_Payload4MaxTempLimitLineEdit_textChanged(const QString&)
{
    (SC.getNewPayloads()+3)->setPayloadMaximumTemperature
            (Payload4MaxTempLimitLineEdit->text().toDouble());

	qWarning("TODO: %s	%d",__FILE__,__LINE__);
}

void SemMainGUI::on_Payload4MinTempLimitLineEdit_textChanged(const QString&)
{
    (SC.getNewPayloads()+3)->setPayloadMinimumTemperature
            (Payload4MinTempLimitLineEdit->text().toDouble());

	qWarning("TODO: %s	%d",__FILE__,__LINE__);
}

void SemMainGUI::on_Payload4DataRateLineEdit_textChanged(const QString&)
{
    (SC.getNewPayloads()+3)->setPayloadDataRate
            (Payload4DataRateLineEdit->text().toDouble());
	qWarning("TODO: %s	%d",__FILE__,__LINE__);
}

void SemMainGUI::on_MissionStartDateTimeEdit_dateTimeChanged(const QDateTime &datetime)
{
    SC.getSCMissionDetails()->setMissionStartTime
            (datetime);
qDebug()<<"start time"<<SC.getSCMissionDetails()->getMissionStartTime();

SC.PassMissionDetailsOutputParameters();//**************************************
//    qDebug()<<"&start time"<<&datetime;
    qWarning("TODO: %s	%d",__FILE__,__LINE__);
}

void SemMainGUI::on_MissionEndDateTimeEdit_dateTimeChanged(const QDateTime &datetime)
{
    SC.getSCMissionDetails()->setMissionEndTime
            (datetime);

    SC.PassMissionDetailsOutputParameters();//**************************************
//    qDebug()<<"end time"<<datetime;
        qWarning("TODO: %s	%d",__FILE__,__LINE__);
}

void SemMainGUI::on_SemiMajorAxisLineEdit_textChanged(const QString&)
{
    SC.getSCMissionDetails()->setSemiMajorAxis
            (SemiMajorAxisLineEdit->text().toDouble());
    SC.PassMissionDetailsOutputParameters();
	qWarning("TODO: %s	%d",__FILE__,__LINE__);
}

void SemMainGUI::on_EccentricityLineEdit_textChanged(const QString&)
{
    SC.getSCMissionDetails()->setEccentricity
            (EccentricityLineEdit->text().toDouble());
    SC.PassMissionDetailsOutputParameters();
	qWarning("TODO: %s	%d",__FILE__,__LINE__);
}

void SemMainGUI::on_InclinationLineEdit_textChanged(const QString&)
{
    SC.getSCMissionDetails()->setInclination
            (InclinationLineEdit->text().toDouble());
    SC.PassMissionDetailsOutputParameters();
	qWarning("TODO: %s	%d",__FILE__,__LINE__);
}

void SemMainGUI::on_ArgumentOfPerigeeLineEdit_textChanged(const QString&)
{
    SC.getSCMissionDetails()->setArgumentOfPerigee
            (ArgumentOfPerigeeLineEdit->text().toDouble());
    SC.PassMissionDetailsOutputParameters();
	qWarning("TODO: %s	%d",__FILE__,__LINE__);
}

void SemMainGUI::on_RAANLineEdit_textChanged(const QString&)
{
    SC.getSCMissionDetails()->setRightAscensionNode
            (RAANLineEdit->text().toDouble());
    SC.PassMissionDetailsOutputParameters();
	qWarning("TODO: %s	%d",__FILE__,__LINE__);
}

void SemMainGUI::on_StructureDetailsButton_clicked()
{
    StructureGUI dialog(&SC, this);
    dialog.exec();

    RefreshSemMainGUI();

	qWarning("TODO: %s	%d",__FILE__,__LINE__);
}

void SemMainGUI::on_DataCommDetailsButton_clicked()
{
    DataCommGUI dialog(&SC, this);
    dialog.exec();

    RefreshSemMainGUI();
	qWarning("TODO: %s	%d",__FILE__,__LINE__);
}

void SemMainGUI::on_ThermalSubsystemGraphComboBox_2_activated(const QString&)
{
	qWarning("TODO: %s	%d",__FILE__,__LINE__);
}

void SemMainGUI::on_ThermalGraphPushButton_2_clicked()
{
    SC.getNewSCThermal()->CreateTemperatureTimeFunction();

    //plotting process starts

    QString FileNameWithExtension;
    if (PowerGraphComboBox->currentIndex()!=0)
    {
        FileNameWithExtension = ("SCTemperatureTimeFunction.stad");
        QDialog plotDialog(this);
        QVBoxLayout* layout = new QVBoxLayout(&plotDialog);
        PlotView* plotView = new PlotView(&plotDialog);
        layout->addWidget(plotView);
        plotDialog.setLayout(layout);

        qWarning("TODO: %s	%d",__FILE__,__LINE__);

        PlotGraphFromFile* Data = new PlotGraphFromFile();
        qWarning("TODO: %s	%d",__FILE__,__LINE__);
        Data->setPoints(FileNameWithExtension);
        qWarning("TODO: %s	%d",__FILE__,__LINE__);
        plotView->addPlot(Data);
        qWarning("TODO: %s	%d",__FILE__,__LINE__);
        FileNameWithExtension.chop(5);
        plotView->setTitle(FileNameWithExtension);
        plotView->setLeftLabel("Temperature (K)");
        plotView->setBottomLabel("Time (Mjd)");
        plotView->autoScale();
        plotView->setMinimumSize(500,500);
        plotDialog.exec();
    }
	qWarning("TODO: %s	%d",__FILE__,__LINE__);
}

void SemMainGUI::on_ThermalDetailsButton_clicked()
{
    ThermalGUI dialog(&SC, this);
    dialog.exec();

    RefreshSemMainGUI();
    // refresh the thermal segment of GUI
//    switch (SC.getNewSCThermal()->getColdFaceCoatingPorperties().Type)
//    {
    //    case CoatUndefined:
    //        ColdFaceCoatingLineEdit->setText("Undefined");
    //        break;
    //    case UserDefined:
    //        ColdFaceCoatingLineEdit->setText("User Defined");
    //        qWarning("TODO: %s	%d",__FILE__,__LINE__);
    //        break;
    //    case NoCoating:
    //        ColdFaceCoatingLineEdit->setText("No Coating");
    //        qWarning("TODO: %s	%d",__FILE__,__LINE__);
    //        break;
    //    case WhitePaint:
    //        ColdFaceCoatingLineEdit->setText("White Paint");
    //        break;
    //    case BlackPaint:
    //        ColdFaceCoatingLineEdit->setText("UBlack Paint");
    //        break;
    //    }

    qWarning("TODO: %s	%d",__FILE__,__LINE__);
}

//void SemMainGUI::on_PowerGraphComboBox_2_activated(const QString&)
//{
//    qWarning("TODO: %s	%d",__FILE__,__LINE__);
//}

void SemMainGUI::on_PowerGraphPushButton_clicked()
{
    //create and propogate the scenario for graphs(because you dont want to save temp files)
    // Create a new scenario
    SpaceScenario* tempScenario = new SpaceScenario();

    // create new Participants, Properties and trajectories
    ScenarioSC* tempSc = new ScenarioSC();

    // Create the initial position (Keplerian elements)
    ScenarioKeplerianElementsType* elements = new ScenarioKeplerianElementsType();
    elements->setSemiMajorAxis(SemiMajorAxisLineEdit->text().toDouble());
    elements->setEccentricity(EccentricityLineEdit->text().toDouble());
    elements->setInclination(InclinationLineEdit->text().toDouble());
    elements->setRAAN(RAANLineEdit->text().toDouble());
    elements->setArgumentOfPeriapsis(ArgumentOfPerigeeLineEdit->text().toDouble());
    elements->setTrueAnomaly(0.0);

    // Create the trajectory arc
    ScenarioLoiteringType* loitering = new ScenarioLoiteringType();
    loitering->Environment()->CentralBody()->setName(PlanetNameLineEdit->text().toUpper());
    loitering->InitialPosition()->setCoordinateSystem("INERTIAL J2000");
    loitering->PropagationPosition()->setTimeStep(60.0);
    loitering->PropagationPosition()->setPropagator("TWO BODY");

    loitering->TimeLine()->setStartTime(MissionStartDateTimeEdit->dateTime());
    loitering->TimeLine()->setEndTime(MissionEndDateTimeEdit->dateTime());
    loitering->TimeLine()->setStepTime(60.0);

    loitering->InitialPosition()->setAbstract6DOFPosition
            (QSharedPointer<ScenarioAbstract6DOFPositionType>(elements));

    // Create the spacecraft
    tempSc->SCMission()->TrajectoryPlan()->AbstractTrajectory().append
            (QSharedPointer<ScenarioAbstractTrajectoryType>(loitering));

    // Add it to the scenario
    tempScenario->AbstractParticipant().append(QSharedPointer<ScenarioParticipantType>(tempSc));


    //Propogate the scenario with the new start and end time  of the defined loitering
    //also with new additional keplerian elements and the orbitting body
    const QList<QSharedPointer<ScenarioAbstractTrajectoryType> >& trajectoryList
            = tempSc->SCMission()->TrajectoryPlan()->AbstractTrajectory();
    PropagationFeedback feedback;
    // Initial state is stored in the first trajectory (for now); so,
    // the empty trajectory plan case has to be treated specially.
    if (!trajectoryList.isEmpty())
    {
        // Propagate all segments of the trajectory plan.
        foreach (QSharedPointer<ScenarioAbstractTrajectoryType> trajectory, trajectoryList)
        {
            QList<double> sampleTimes;
            QList<sta::StateVector> samples;

            if (dynamic_cast<ScenarioLoiteringType*>(trajectory.data()))
            {
                //out << "ScenarioLoiteringType in process " << endl;
                ScenarioLoiteringType* tempLloitering = dynamic_cast<ScenarioLoiteringType*>(trajectory.data());

                // propogate the scenario
                PropagateLoiteringTrajectory(tempLloitering, sampleTimes, samples, feedback);

                //******************************************************************** /OZGUN
                // Eclipse function is called and the "data/EclipseStarLight.stad" is generated
                EclipseDuration* Eclipse = new EclipseDuration();

                Eclipse->StarLightTimeFunction(sampleTimes,
                                               samples,
                                               STA_SOLAR_SYSTEM->lookup(PlanetNameLineEdit->text()),
                                               STA_SOLAR_SYSTEM->lookup("Sun"));
                //******************************************************************** OZGUN/
            }
        }
    }


    //create the files of the functions
    SC.getNewSCPower()->CreateGeneratedPowerTimeFunctionOfSpacecraft();
    SC.getNewSCPower()->CreatePowerConsumptionFunctionOfSpacecraft();
    SC.getNewSCPower()->CreateNetPowerTimeFunctionOfSpacecraft();

    //plotting process starts

    QString FileNameWithExtension;
    switch (PowerGraphComboBox->currentIndex())
    {
    case 0://SelectGraph
        break;
    case 1://consumed power - time
        FileNameWithExtension = ("PowerConsumptionTimeFunction.stad");
        break;
    case 2://generate power - time
        FileNameWithExtension = ("GeneratedPowerTimeFunction.stad");
        break;
    case 3://net power - time
        FileNameWithExtension = ("NetPowerTimeFunction.stad");
        break;
    }

    if (PowerGraphComboBox->currentIndex() != 0)
    {
        QDialog plotDialog(this);
        QVBoxLayout* layout = new QVBoxLayout(&plotDialog);
        PlotView* plotView = new PlotView(&plotDialog);
        layout->addWidget(plotView);
        plotDialog.setLayout(layout);

        qWarning("TODO: %s	%d",__FILE__,__LINE__);

        PlotGraphFromFile* Data = new PlotGraphFromFile();
        qWarning("TODO: %s	%d",__FILE__,__LINE__);
        Data->setPoints(FileNameWithExtension);
        qWarning("TODO: %s	%d",__FILE__,__LINE__);
        plotView->addPlot(Data);
        qWarning("TODO: %s	%d",__FILE__,__LINE__);
        FileNameWithExtension.chop(5);
        plotView->setTitle(FileNameWithExtension);
        plotView->setLeftLabel("Power (W)");
        plotView->setBottomLabel("Time (Mjd)");
        plotView->autoScale();
        plotView->setMinimumSize(500,500);
        plotDialog.exec();
    }

        qWarning("TODO: %s	%d",__FILE__,__LINE__);
}

void SemMainGUI::on_PowerDetailsButton_clicked()
{
    PowerGUI dialog(&SC,this);
    dialog.exec();

    RefreshSemMainGUI();
	qWarning("TODO: %s	%d",__FILE__,__LINE__);
}

void SemMainGUI::on_LauncherSelectionComboBox_activated(const QString&)
{
    if ((SC.getSCMissionDetails()->getPlanetProperties()->Planet.toUpper()=="EARTH")
        && (LauncherSelectionComboBox->currentIndex()!=0))
    {
        SC.getNewLauncher()->setLauncher(LauncherSelectionComboBox->currentText());

        LauncherAxialFreqCapabilityLineEdit->setText
                (QString::number
                 (SC.getNewLauncher()->getAxialFreq()));
        LauncherLateralFreqCapabilityLineEdit->setText
                (QString::number
                 (SC.getNewLauncher()->getLateralFreq()));
        CargoBayDiameterLineEdit->setText
                (QString::number
                 (SC.getNewLauncher()->getCargoBayDiameter()));
        CargoBayLengthLineEdit->setText
                (QString::number
                 (SC.getNewLauncher()->getCargoBayLength()));
        LauncherPayloadMassLineEdit->setText
                (QString::number
                 (SC.getNewLauncher()->getMass
                  (SC.getSCMissionDetails()->getSCMeanAltitude())));
    }
	qWarning("TODO: %s	%d",__FILE__,__LINE__);
}

void SemMainGUI::on_LauncherAxialFreqCapabilityLineEdit_textChanged(const QString &)
{
    if(LauncherAxialFreqCapabilityLineEdit->text().toDouble()>
       SC.getNewSCStructure()->getAxialFrequency())
        AxialFreqLabel->setEnabled(1);
    else
        AxialFreqLabel->setEnabled(0);
        qWarning("TODO: %s	%d",__FILE__,__LINE__);
}

void SemMainGUI::on_LauncherLateralFreqCapabilityLineEdit_textChanged(const QString &)
{
    if (LauncherLateralFreqCapabilityLineEdit->text().toDouble()>
        SC.getNewSCStructure()->getLateralFrequency())
        LateralFreqLabel->setEnabled(1);
    else
        LateralFreqLabel->setEnabled(0);
        qWarning("TODO: %s	%d",__FILE__,__LINE__);
}

void SemMainGUI::on_LauncherPayloadMassLineEdit_textChanged(const QString &)
{
    if (LauncherPayloadMassLineEdit->text().toDouble()>
        SC.getNewSCStructure()->getSCMass())
        MassLabel->setEnabled(0);
    else
        MassLabel->setEnabled(1);
        qWarning("TODO: %s	%d",__FILE__,__LINE__);
}

void SemMainGUI::on_CargoBayDiameterLineEdit_textChanged(const QString &)
{
    if ((CargoBayDiameterLineEdit->text().toDouble()
            <SC.getNewSCStructure()->getSpacecraftDimension().x())
        ||(CargoBayDiameterLineEdit->text().toDouble()
            <SC.getNewSCStructure()->getSpacecraftDimension().y())
        ||(CargoBayDiameterLineEdit->text().toDouble()
            <SC.getNewSCStructure()->getSpacecraftDimension().z()))
        CargoBayDiameterLabel->setEnabled(1);
    else
        CargoBayDiameterLabel->setEnabled(0);
        qWarning("TODO: %s	%d",__FILE__,__LINE__);
}

void SemMainGUI::on_CargoBayLengthLineEdit_textChanged(const QString &)
{
    if ((CargoBayLengthLineEdit->text().toDouble()
            <SC.getNewSCStructure()->getSpacecraftDimension().x())
        ||(CargoBayLengthLineEdit->text().toDouble()
            <SC.getNewSCStructure()->getSpacecraftDimension().y())
        ||(CargoBayLengthLineEdit->text().toDouble()
            <SC.getNewSCStructure()->getSpacecraftDimension().z()))
        CargoBayLengthLabel->setEnabled(1);
    else
        CargoBayLengthLabel->setEnabled(0);
        qWarning("TODO: %s	%d",__FILE__,__LINE__);
}

//void SemMainGUI::on_LauncherDetailsButton_clicked()
//{
////    LauncherGUI dialog(this);
////    dialog.exec();
//
//	qWarning("TODO: %s	%d",__FILE__,__LINE__);
//}

//void SemMainGUI::on_PlanetNameComboBox_activated(const QString &)
//{
//    SC.getSCMissionDetails()->setPlanetProperties
//            (PlanetNameComboBox->currentText());
//
//    SC.PassMissionDetailsOutputParameters();
//}

//void SemMainGUI::on_SemMainGUICalculatePushButton_clicked()
//{
////    *****************************************CALCULATE
//
//
//    qDebug()<<"everything fine";
//
////    NewSCStructure.setSCShape(Cube);
//
//
//
//	qWarning("TODO: %s	%d",__FILE__,__LINE__);
//}

void SemMainGUI::on_SemMainGUISavePushButton_clicked()
{   
    Vehicle->System().data()->Structure().data()->setStructuralShape
            (SC.getNewSCStructure()->getSCShapeString());
    Vehicle->System().data()->Structure().data()->setStructuralMaterial
            (SC.getNewSCStructure()->getSCMaterial().Name);

    Vehicle->System().data()->Structure().data()->Sizing().data()->setWidth
            (SC.getNewSCStructure()->getSpacecraftDimension().x());
    Vehicle->System().data()->Structure().data()->Sizing().data()->setHeight
            (SC.getNewSCStructure()->getSpacecraftDimension().y());
    Vehicle->System().data()->Structure().data()->Sizing().data()->setLength
            (SC.getNewSCStructure()->getSpacecraftDimension().z());

    Vehicle->System().data()->Structure().data()->Sizing().data()->setVolume
            (SC.getNewSCStructure()->getSCVolume());
    Vehicle->System().data()->Structure().data()->MomentsOfInertia().data()->setXAxis
            (SC.getNewSCStructure()->getMomentsOfInertia().x());
    Vehicle->System().data()->Structure().data()->MomentsOfInertia().data()->setYAxis
            (SC.getNewSCStructure()->getMomentsOfInertia().y());
    Vehicle->System().data()->Structure().data()->MomentsOfInertia().data()->setZAxis
            (SC.getNewSCStructure()->getMomentsOfInertia().z());

    Vehicle->System().data()->Structure().data()->SecondMomentsOfArea().data()->setXAxis
            (SC.getNewSCStructure()->getSecondMomentsOfArea().x());
    Vehicle->System().data()->Structure().data()->SecondMomentsOfArea().data()->setYAxis
            (SC.getNewSCStructure()->getSecondMomentsOfArea().y());
    Vehicle->System().data()->Structure().data()->SecondMomentsOfArea().data()->setZAxis
            (SC.getNewSCStructure()->getSecondMomentsOfArea().z());

    Vehicle->System().data()->Structure().data()->NaturalFrequency().data()->setLateralFrequency
            (SC.getNewSCStructure()->getLateralFrequency());
    Vehicle->System().data()->Structure().data()->NaturalFrequency().data()->setAxialFrequency
            (SC.getNewSCStructure()->getAxialFrequency());
    Vehicle->System().data()->Structure().data()->setTotalStructureMass
            (SC.getNewSCStructure()->getStructureSubsystemMass());
    //to calculate correct vehicle total mass-------------------------------
    SC.getNewSCStructure()->setStructureSubsystemMass
            (Vehicle->System()->Structure()->totalStructureMass());

    Vehicle->System().data()->TCS().data()->Temperature().data()->setMaximumSCTemperature
            (SC.getNewSCThermal()->getMaximumSCTempWithRadiatorOrHeater());
    Vehicle->System().data()->TCS().data()->Temperature().data()->setMinimumSCTemperature
            (SC.getNewSCThermal()->getMinimumSCTempWithRadiatorOrHeater());
    Vehicle->System().data()->TCS().data()->setHeaterPower
            (SC.getNewSCThermal()->getNeededHeater());
    Vehicle->System().data()->TCS().data()->setRadiatedPower
            (SC.getNewSCThermal()->getNeededRadiator());

    Vehicle->System()->TCS()->CoatingArea()->ColdSurface()->ColdCoating()->ElementIdentifier()->setName
            (SC.getNewSCThermal()->getColdFaceCoatingProperties().Type);
    Vehicle->System().data()->TCS().data()->CoatingArea().data()->ColdSurface().data()->ColdCoating().data()->setEmissivity
            (SC.getNewSCThermal()->getColdFaceCoatingProperties().Emmissivity);
    Vehicle->System().data()->TCS().data()->CoatingArea().data()->ColdSurface().data()->ColdCoating().data()->setAbsorptivity
            (SC.getNewSCThermal()->getColdFaceCoatingProperties().Absorptivity);

    Vehicle->System()->TCS()->CoatingArea()->HotSurface()->HotCoating()->ElementIdentifier()->setName
            (SC.getNewSCThermal()->getHotFaceCoatingProperties().Type);
    Vehicle->System().data()->TCS().data()->CoatingArea().data()->HotSurface().data()->HotCoating().data()->setEmissivity
            (SC.getNewSCThermal()->getHotFaceCoatingProperties().Emmissivity);
    Vehicle->System().data()->TCS().data()->CoatingArea().data()->HotSurface().data()->HotCoating().data()->setAbsorptivity
            (SC.getNewSCThermal()->getHotFaceCoatingProperties().Absorptivity);

    Vehicle->System().data()->TCS().data()->CoatingArea().data()->ColdSurface().data()->setTotalColdArea
            (SC.getNewSCThermal()->getTotalAreaOfColdFace());
    Vehicle->System().data()->TCS().data()->CoatingArea().data()->HotSurface().data()->setTotalHotArea
            (SC.getNewSCThermal()->getTotalAreaOfHotFace());

    Vehicle->System().data()->TCS().data()->setTotalTCSMass
            (SC.getNewSCStructure()->getThermalSubsystemMass());    
    //to calculate correct vehicle total mass-------------------------------
    SC.getNewSCStructure()->setThermalSubsystemMass
            (Vehicle->System()->TCS()->totalTCSMass());

    Vehicle->System().data()->EPS().data()->SolarArray().data()->setSACellType
            (SC.getNewSCPower()->SolarArrays.getSolarCell().Name);
    Vehicle->System().data()->EPS().data()->SolarArray().data()->setAreaOfSolarArray
            (SC.getNewSCPower()->SolarArrays.getArea());
    Vehicle->System().data()->EPS().data()->SolarArray().data()->setMassOfSolarArray
            (SC.getNewSCPower()->SolarArrays.getSAMass());
    Vehicle->System().data()->EPS().data()->BatteryType().data()->setBatteryType
            (SC.getNewSCPower()->Battery.getBatteryProperties().Name);
    Vehicle->System().data()->EPS().data()->BatteryType().data()->setTotalMassOfBatteries
            (SC.getNewSCPower()->Battery.getBMass());
    Vehicle->System().data()->EPS().data()->BatteryType().data()->setTotalVolumeOfBatteries
            (SC.getNewSCPower()->Battery.getBVolume());
    Vehicle->System().data()->EPS().data()->BatteryType().data()->setNumberOfBatteries
            (SC.getNewSCPower()->Battery.getNumberOfBatteries());
    Vehicle->System().data()->EPS().data()->setMassOfPCU
            (SC.getNewSCPower()->SolarArrays.getPCUMass());

    Vehicle->System().data()->EPS().data()->setTotalEPSMass
            (SC.getNewSCStructure()->getPowerSubsystemMass());
    //to calculate correct vehicle total mass-------------------------------
    SC.getNewSCStructure()->setPowerSubsystemMass
            (Vehicle->System()->EPS()->totalEPSMass());

    Vehicle->System().data()->TTC().data()->setTotalTTCMass
            (SC.getNewSCStructure()->getTTCSubsystemMass());
   //to calculate correct vehicle total mass-------------------------------
    SC.getNewSCStructure()->setTTCSubsystemMass
            (Vehicle->System()->TTC()->totalTTCMass());

    Vehicle->System().data()->TTC().data()->TTCAntenna().data()->setTransmitterPower
            (SC.getNewSCCommunication()->getAntennaPower());
    Vehicle->System().data()->TTC().data()->TTCAntenna().data()->setTransmitterFrequency
            (SC.getNewSCCommunication()->getAntennaFrequency());
    Vehicle->System().data()->TTC().data()->TTCAntenna().data()->setTransmitterGain
            (SC.getNewSCCommunication()->getAntennaGain());
    Vehicle->System().data()->TTC().data()->TTCAntenna().data()->setTransmitterEfficiency
            (SC.getNewSCCommunication()->getAntennaEfficiency());
    Vehicle->System().data()->TTC().data()->TTCAntenna().data()->setLinkDuration
            (SC.getNewSCCommunication()->getPercentageContactTimePerOrbit());
    Vehicle->System().data()->TTC().data()->TTCAntenna().data()->setDataRate
            (SC.getNewSCCommunication()->getDownLinkRate());

    Vehicle->System().data()->OBDH().data()->setTotalOBDHMass
            (SC.getNewSCStructure()->getOBDHSubsystemMass());
    //to calculate correct vehicle total mass-------------------------------
     SC.getNewSCStructure()->setOBDHSubsystemMass
             (Vehicle->System()->OBDH()->totalOBDHMass());

    Vehicle->System().data()->OBDH().data()->setTotalSizeOfMemory
            (SC.getNewSCDataHandling()->getMemorySizeForPayloads());

    SC.getNewSCStructure()->CalculateAndSetSCMass();
    //save the values to scenario -------------------
    Vehicle->System().data()->SystemBudgets().data()->MassOfSystem().data()->setDryMass
            (SC.getNewSCStructure()->getSCMass());
    qDebug()<<"SAVE DRY MASS"<<Vehicle->System()->SystemBudgets()->MassOfSystem()->dryMass();

    Vehicle->System().data()->SystemBudgets().data()->PowerOfSystem().data()->setTotalPowerEoL
            (SC.getNewSCPower()->SolarArrays.getSAEOLPower());
    Vehicle->System().data()->SystemBudgets().data()->PowerOfSystem().data()->setTotalPowerBoL
            (SC.getNewSCPower()->SolarArrays.getSABOLPower());



    //save the changes in the payloads
    int i;
    for (i=0;i<    Vehicle->SCMission()->PayloadSet()->AbstractPayload().count();i++)
    {
        Vehicle->SCMission()->PayloadSet()->AbstractPayload().at(i)->Budget()->setMass
                ((SC.getNewPayloads()+i)->getPayloadMass());

        Vehicle->SCMission()->PayloadSet()->AbstractPayload().at(i)->Budget()->Size()->setWidth
        ((SC.getNewPayloads()+i)->getPayloadWidth());
        Vehicle->SCMission()->PayloadSet()->AbstractPayload().at(i)->Budget()->Size()->setLength
        ((SC.getNewPayloads()+i)->getPayloadLength());
        Vehicle->SCMission()->PayloadSet()->AbstractPayload().at(i)->Budget()->Size()->setHeight
        ((SC.getNewPayloads()+i)->getPayloadHeight());

        Vehicle->SCMission()->PayloadSet()->AbstractPayload().at(i)->Budget()->setDataRate
                ((SC.getNewPayloads()+i)->getPayloadDataRate());

        Vehicle->SCMission()->PayloadSet()->AbstractPayload().at(i)->Budget()->TemperatureRange()->setMaximumTemperature
                ((SC.getNewPayloads()+i)->getPayloadMaximumTemperature());
        Vehicle->SCMission()->PayloadSet()->AbstractPayload().at(i)->Budget()->TemperatureRange()->setMinimumTemperature
                ((SC.getNewPayloads()+i)->getPayloadMinimumTemperature());

        Vehicle->SCMission()->PayloadSet()->AbstractPayload().at(i)->Budget()->setFrequencyBand
                ((SC.getNewPayloads()+i)->getPayloadWavelength());

        Vehicle->SCMission()->PayloadSet()->AbstractPayload().at(i)->Budget()->Power()->setPowerConsumptionInDaylight
                ((SC.getNewPayloads()+i)->getPayloadPowerInDaylight());
        Vehicle->SCMission()->PayloadSet()->AbstractPayload().at(i)->Budget()->Power()->setPowerConsumptionInEclipse
                ((SC.getNewPayloads()+i)->getPayloadPowerInEclipse());
        Vehicle->SCMission()->PayloadSet()->AbstractPayload().at(i)->Budget()->Power()->setPowerOnPercentageInDaylight
                ((SC.getNewPayloads()+i)->getPayloadPowerOnPercentageInDaylight());
        Vehicle->SCMission()->PayloadSet()->AbstractPayload().at(i)->Budget()->Power()->setPowerOnPercentageInEclipse
                ((SC.getNewPayloads()+i)->getPayloadPowerOnPercentageInEclipse());
    }

    //save the mission details to scenario
    //Propogate the scenario with the new start and end time  of the defined loitering
    //also with new additional keplerian elements and the orbitting body
    const QList<QSharedPointer<ScenarioAbstractTrajectoryType> >& trajectoryList
            = Vehicle->SCMission()->TrajectoryPlan()->AbstractTrajectory();
    PropagationFeedback feedback;
    // Initial state is stored in the first trajectory (for now); so,
    // the empty trajectory plan case has to be treated specially.
    if (!trajectoryList.isEmpty())
    {
        // Propagate all segments of the trajectory plan.
        foreach (QSharedPointer<ScenarioAbstractTrajectoryType> trajectory, trajectoryList)
        {
            QList<double> sampleTimes;
            QList<sta::StateVector> samples;

            if (dynamic_cast<ScenarioLoiteringType*>(trajectory.data()))
            {
                //out << "ScenarioLoiteringType in process " << endl;
                ScenarioLoiteringType* loitering = dynamic_cast<ScenarioLoiteringType*>(trajectory.data());

                if (loitering->ElementIdentifier()->Name()== MissionArc)
                {
                    //set mission start time - end time - keplerian element - orbitting body
                    loitering->TimeLine()->setStartTime
                            (MissionStartDateTimeEdit->dateTime());
                    loitering->TimeLine()->setEndTime
                            (MissionEndDateTimeEdit->dateTime());
                    loitering->Environment()->CentralBody()->setName
                            (SC.getSCMissionDetails()->getPlanetProperties()->Planet.toUpper());
                    //see if the mission has keplerian elements to be filled
                    if (!loitering->InitialPosition()->Abstract6DOFPosition().isNull())
                    {
                        if (dynamic_cast<ScenarioKeplerianElementsType*>
                            (loitering->InitialPosition()->Abstract6DOFPosition().data()))
                        {
                            //for now SEM is only capable of doing systems engineering for the
                            //missions who has keplerian elements
                            ScenarioKeplerianElementsType* keplerian
                                    = dynamic_cast<ScenarioKeplerianElementsType*>
                                      (loitering->InitialPosition()->Abstract6DOFPosition().data());
                            keplerian->setArgumentOfPeriapsis
                                    (ArgumentOfPerigeeLineEdit->text().toDouble());
                            keplerian->setEccentricity
                                    (EccentricityLineEdit->text().toDouble());
                            keplerian->setInclination
                                    (InclinationLineEdit->text().toDouble());
                            keplerian->setRAAN
                                    (RAANLineEdit->text().toDouble());
                            keplerian->setSemiMajorAxis
                                    (SemiMajorAxisLineEdit->text().toDouble());
                            keplerian->setTrueAnomaly
                                    (SC.getSCMissionDetails()->getTrueAnomaly());
                            break;
                        }
                    }
                }
            }
        }
    }


    //close the window
    this->close();
	qWarning("TODO: %s	%d",__FILE__,__LINE__);
}

void SemMainGUI::on_SemMainGUICancelPushButton_clicked()
{
    this->close();
	qWarning("TODO: %s	%d",__FILE__,__LINE__);
}

void SemMainGUI::RefreshSemMainGUI()
{
    //put the values in MainGUI
    //preliminary design
    PlanetNameLineEdit->setText
            (SC.getSCMissionDetails()->getPlanetProperties()->Planet);
    MissionStartDateTimeEdit->setDateTime
            (SC.getSCMissionDetails()->getMissionStartTime());
    MissionEndDateTimeEdit->setDateTime
            (SC.getSCMissionDetails()->getMissionEndTime());
    SemiMajorAxisLineEdit->setText
            (QString::number
             (SC.getSCMissionDetails()->getSemiMajorAxis()));
    EccentricityLineEdit->setText
            (QString::number
             (SC.getSCMissionDetails()->getEccentricity()));
    InclinationLineEdit->setText
            (QString::number
             (SC.getSCMissionDetails()->getInclination()));
    ArgumentOfPerigeeLineEdit->setText
            (QString::number
             (SC.getSCMissionDetails()->getArgumentOfPerigee()));
    RAANLineEdit->setText
            (QString::number
             (SC.getSCMissionDetails()->getRightAscensionNode()));

    // refresh the structure part of main GUI;
    SCWidthLineEdit->setText
            (QString::number(SC.getNewSCStructure()->getSpacecraftDimension().x()));
    SCHeightLineEdit->setText
            (QString::number(SC.getNewSCStructure()->getSpacecraftDimension().y()));
    SCLengthLineEdit->setText
            (QString::number(SC.getNewSCStructure()->getSpacecraftDimension().z()));
    SCMassLineEdit->setText
            (QString::number(SC.getNewSCStructure()->getSCMass()));
    SCLateralFreqLineEdit->setText
            (QString::number(SC.getNewSCStructure()->getLateralFrequency()));
    SCAxialFreqLineEdit->setText
            (QString::number(SC.getNewSCStructure()->getAxialFrequency()));

    // refresh the datacomm part of main GUI;
    TotalPayloadDataRateLineEdit->setText
            (QString::number(SC.getNewSCDataHandling()->getTotalPayloadDataRate()));
    MemorySizeLineEdit->setText
            (QString::number(SC.getNewSCDataHandling()->getMemorySizeForPayloads()));

    TXPowerLineEdit->setText
            (QString::number(SC.getNewSCCommunication()->getAntennaPower()));
    TotalLinkDurationLineEdit->setText
            (QString::number(SC.getNewSCCommunication()->getContactTimePerOrbit()));

    // refresh the thermal part of main GUI;
    HeaterPowerLineEdit->setText
            (QString::number(SC.getNewSCThermal()->getNeededHeater()));
    RadiatorLineEdit->setText
            (QString::number(SC.getNewSCThermal()->getNeededRadiator()));
    ColdFaceCoatingLineEdit->setText
            (SC.getNewSCThermal()->getColdFaceCoatingProperties().Type);
    HotFaceCoatingLineEdit->setText
            (SC.getNewSCThermal()->getHotFaceCoatingProperties().Type);

    //set the Power section of SemMainGUI
    PowerConsumptionInEclipseLineEdit->setText
            (QString::number(SC.getNewSCPower()->getAveragePowerConsumptionInEclipse()));
    PowerConsumptionInDaylightLineEdit->setText
            (QString::number(SC.getNewSCPower()->getAveragePowerConsumptionInDaylight()));
    SAEOLPowerLineEdit->setText
            (QString::number(SC.getNewSCPower()->SolarArrays.getSAEOLPower()));
    SABOLPowerLineEdit->setText
            (QString::number(SC.getNewSCPower()->SolarArrays.getSABOLPower()));

    SolarCellTypeLineEdit->setText(SC.getNewSCPower()->SolarArrays.getSolarCell().Name);

    BatteryTypeLineEdit->setText(SC.getNewSCPower()->Battery.getBatteryProperties().Name);
}

void SemMainGUI::RefreshSemMainGUIPayload()
{
    if ((SC.getNewPayloads()+0)->getPayloadName()=="")
    {
        Payload1GroupBox->setEnabled(0);
    }
    else
    {
        Payload1GroupBox->setEnabled(1);
    }

    if ((SC.getNewPayloads()+1)->getPayloadName()=="")
    {
        Payload2GroupBox->setEnabled(0);
    }
    else
    {
        Payload2GroupBox->setEnabled(1);
    }

    if ((SC.getNewPayloads()+2)->getPayloadName()=="")
    {
        Payload3GroupBox->setEnabled(0);
    }
    else
    {
        Payload3GroupBox->setEnabled(1);
    }

    if ((SC.getNewPayloads()+3)->getPayloadName()=="")
    {
        Payload4GroupBox->setEnabled(0);
    }
    else
    {
        Payload4GroupBox->setEnabled(1);
    }

    Payload1NameLabel->setText((SC.getNewPayloads()+0)->getPayloadName());
    Payload2NameLabel->setText((SC.getNewPayloads()+1)->getPayloadName());
    Payload3NameLabel->setText((SC.getNewPayloads()+2)->getPayloadName());
    Payload4NameLabel->setText((SC.getNewPayloads()+3)->getPayloadName());

    Payload1WidthLineEdit->setText
            (QString::number
             ((SC.getNewPayloads()+0)->getPayloadWidth()));
    Payload2WidthLineEdit->setText
            (QString::number
             ((SC.getNewPayloads()+1)->getPayloadWidth()));
    Payload3WidthLineEdit->setText
            (QString::number
             ((SC.getNewPayloads()+2)->getPayloadWidth()));
    Payload4WidthLineEdit->setText
            (QString::number
             ((SC.getNewPayloads()+3)->getPayloadWidth()));

    Payload1LengthLineEdit->setText
            (QString::number
             ((SC.getNewPayloads()+0)->getPayloadLength()));
    Payload2LengthLineEdit->setText
            (QString::number
             ((SC.getNewPayloads()+1)->getPayloadLength()));
    Payload3LengthLineEdit->setText
            (QString::number
             ((SC.getNewPayloads()+2)->getPayloadLength()));
    Payload4LengthLineEdit->setText
            (QString::number
             ((SC.getNewPayloads()+3)->getPayloadLength()));

    Payload1HeightLineEdit->setText
            (QString::number
             ((SC.getNewPayloads()+0)->getPayloadHeight()));
    Payload2HeightLineEdit->setText
            (QString::number
             ((SC.getNewPayloads()+1)->getPayloadHeight()));
    Payload3HeightLineEdit->setText
            (QString::number
             ((SC.getNewPayloads()+2)->getPayloadHeight()));
    Payload4HeightLineEdit->setText
            (QString::number
             ((SC.getNewPayloads()+3)->getPayloadHeight()));

    Payload1MassLineEdit->setText
            (QString::number
             ((SC.getNewPayloads()+0)->getPayloadMass()));
    Payload2MassLineEdit->setText
            (QString::number
             ((SC.getNewPayloads()+1)->getPayloadMass()));
    Payload3MassLineEdit->setText
            (QString::number
             ((SC.getNewPayloads()+2)->getPayloadMass()));
    Payload4MassLineEdit->setText
            (QString::number
             ((SC.getNewPayloads()+3)->getPayloadMass()));

    Payload1PowerConsumptionInEclipseLineEdit->setText
            (QString::number
             ((SC.getNewPayloads()+0)->getPayloadPowerInEclipse()));
    Payload2PowerConsumptionInEclipseLineEdit->setText
            (QString::number
             ((SC.getNewPayloads()+1)->getPayloadPowerInEclipse()));
    Payload3PowerConsumptionInEclipseLineEdit->setText
            (QString::number
             ((SC.getNewPayloads()+2)->getPayloadPowerInEclipse()));
    Payload4PowerConsumptionInEclipseLineEdit->setText
            (QString::number
             ((SC.getNewPayloads()+3)->getPayloadPowerInEclipse()));

    Payload1PowerConsumptionInDaylightLineEdit->setText
            (QString::number
             ((SC.getNewPayloads()+0)->getPayloadPowerInDaylight()));
    Payload2PowerConsumptionInDaylightLineEdit->setText
            (QString::number
             ((SC.getNewPayloads()+1)->getPayloadPowerInDaylight()));
    Payload3PowerConsumptionInDaylightLineEdit->setText
            (QString::number
             ((SC.getNewPayloads()+2)->getPayloadPowerInDaylight()));
    Payload4PowerConsumptionInDaylightLineEdit->setText
            (QString::number
             ((SC.getNewPayloads()+3)->getPayloadPowerInDaylight()));

    Payload1PowerOnTimeEclipseLineEdit->setText
            (QString::number
             ((SC.getNewPayloads()+0)->getPayloadPowerOnPercentageInEclipse()));
    Payload2PowerOnTimeEclipseLineEdit->setText
            (QString::number
             ((SC.getNewPayloads()+1)->getPayloadPowerOnPercentageInEclipse()));
    Payload3PowerOnTimeEclipseLineEdit->setText
            (QString::number
             ((SC.getNewPayloads()+2)->getPayloadPowerOnPercentageInEclipse()));
    Payload4PowerOnTimeEclipseLineEdit->setText
            (QString::number
             ((SC.getNewPayloads()+3)->getPayloadPowerOnPercentageInEclipse()));

    Payload1PowerOnTimeDaylightLineEdit->setText
            (QString::number
             ((SC.getNewPayloads()+0)->getPayloadPowerOnPercentageInDaylight()));
    Payload2PowerOnTimeDaylightLineEdit->setText
            (QString::number
             ((SC.getNewPayloads()+1)->getPayloadPowerOnPercentageInDaylight()));
    Payload3PowerOnTimeDaylightLineEdit->setText
            (QString::number
             ((SC.getNewPayloads()+2)->getPayloadPowerOnPercentageInDaylight()));
    Payload4PowerOnTimeDaylightLineEdit->setText
            (QString::number
             ((SC.getNewPayloads()+3)->getPayloadPowerOnPercentageInDaylight()));

    Payload1MaxTempLimitLineEdit->setText
            (QString::number
             ((SC.getNewPayloads()+0)->getPayloadMaximumTemperature()));
    Payload2MaxTempLimitLineEdit->setText
            (QString::number
             ((SC.getNewPayloads()+1)->getPayloadMaximumTemperature()));
    Payload3MaxTempLimitLineEdit->setText
            (QString::number
             ((SC.getNewPayloads()+2)->getPayloadMaximumTemperature()));
    Payload4MaxTempLimitLineEdit->setText
            (QString::number
             ((SC.getNewPayloads()+3)->getPayloadMaximumTemperature()));

    Payload1MinTempLimitLineEdit->setText
            (QString::number
             ((SC.getNewPayloads()+0)->getPayloadMinimumTemperature()));
    Payload2MinTempLimitLineEdit->setText
            (QString::number
             ((SC.getNewPayloads()+1)->getPayloadMinimumTemperature()));
    Payload3MinTempLimitLineEdit->setText
            (QString::number
             ((SC.getNewPayloads()+2)->getPayloadMinimumTemperature()));
    Payload4MinTempLimitLineEdit->setText
            (QString::number
             ((SC.getNewPayloads()+3)->getPayloadMinimumTemperature()));

    Payload1DataRateLineEdit->setText
            (QString::number
             ((SC.getNewPayloads()+0)->getPayloadDataRate()));
    Payload2DataRateLineEdit->setText
            (QString::number
             ((SC.getNewPayloads()+1)->getPayloadDataRate()));
    Payload3DataRateLineEdit->setText
            (QString::number
             ((SC.getNewPayloads()+2)->getPayloadDataRate()));
    Payload4DataRateLineEdit->setText
            (QString::number
             ((SC.getNewPayloads()+3)->getPayloadDataRate()));
}

void SemMainGUI::RetrieveScenarioSC()
{
    //retreive the mission details from the scenario

    const QList<QSharedPointer<ScenarioAbstractTrajectoryType> >& trajectoryList
            = Vehicle->SCMission()->TrajectoryPlan()->AbstractTrajectory();

    // Initial state is stored in the first trajectory (for now); so,
    // the empty trajectory plan case has to be treated specially.
    if (!trajectoryList.isEmpty())
    {
        // details of the mission arcs when do they start when do they end
        foreach (QSharedPointer<ScenarioAbstractTrajectoryType> trajectory, trajectoryList)
        {
            if (dynamic_cast<ScenarioLoiteringType*>(trajectory.data()))
            {
                //for now SEM is only capable of doing loitering systems engineering
                ScenarioLoiteringType* loitering
                        = dynamic_cast<ScenarioLoiteringType*>(trajectory.data());
                if (loitering->ElementIdentifier()->Name()== MissionArc)
                {
                    //see if the mission has keplerian elements
                    if (!loitering->InitialPosition()->Abstract6DOFPosition().isNull())
                    {
                        if (dynamic_cast<ScenarioKeplerianElementsType*>
                            (loitering->InitialPosition()->Abstract6DOFPosition().data()))
                        {
                            //for now SEM is only capable of doing systems engineering for the
                            //missions who has keplerian elements
                            ScenarioKeplerianElementsType* keplerian
                                    = dynamic_cast<ScenarioKeplerianElementsType*>
                                      (loitering->InitialPosition()->Abstract6DOFPosition().data());
                            SC.getSCMissionDetails()->setArgumentOfPerigee
                                    (keplerian->argumentOfPeriapsis());
                            SC.getSCMissionDetails()->setEccentricity
                                    (keplerian->eccentricity());
                            SC.getSCMissionDetails()->setInclination
                                    (keplerian->inclination());
                            SC.getSCMissionDetails()->setRightAscensionNode
                                    (keplerian->RAAN());
                            SC.getSCMissionDetails()->setTrueAnomaly
                                    (keplerian->trueAnomaly());
                            SC.getSCMissionDetails()->setSemiMajorAxis
                                    (keplerian->semiMajorAxis());

                            //propogate the scenario
                            QList<double> sampleTimes;
                            QList<sta::StateVector> samples;
                            PropagationFeedback feedback;
                            PropagateLoiteringTrajectory(loitering,
                                                         sampleTimes,
                                                         samples,
                                                         feedback);

                            //collect the mission details and display them in the wizard
                            SC.getSCMissionDetails()->setMissionStartTime
                                    (loitering->TimeLine().data()->StartTime());
                            MissionStartDateTimeEdit->setDateTime
                                    (SC.getSCMissionDetails()->getMissionStartTime());

                            SC.getSCMissionDetails()->setMissionEndTime
                                    (loitering->TimeLine().data()->EndTime());
                            MissionEndDateTimeEdit->setDateTime
                                    (SC.getSCMissionDetails()->getMissionEndTime());

                            //planet name that SC is orbiting around
                            //convert the planet name to title case
                            //To store the Planet name in title case
                            QString tempCentralBody = "";
                            tempCentralBody = loitering->Environment()->CentralBody()->Name().toLower();
                            tempCentralBody[0]=tempCentralBody[0].toUpper();
                            SC.getSCMissionDetails()->setPlanetProperties
                                    (tempCentralBody);

                            PlanetNameLineEdit->setText
                                    (SC.getSCMissionDetails()->getPlanetProperties()->Planet);


                            SC.PassMissionDetailsOutputParameters();
                            //******************************************************************** /OZGUN
                            // Eclipse function is called and the "data/EclipseStarLight.stad" is generated
                            EclipseDuration* Eclipse = new EclipseDuration();

                            Eclipse->StarLightTimeFunction(sampleTimes,
                                                           samples,
                                                           STA_SOLAR_SYSTEM->lookup(SC.getSCMissionDetails()->getPlanetProperties()->Planet),
                                                           STA_SOLAR_SYSTEM->lookup("Sun"));
                            //******************************************************************** OZGUN/
                            break;
                        }
                    }
                }
            }
        }
    }

    // transfer the payload information from scenario to GUI
    int i=0;

    const QList<QSharedPointer<ScenarioAbstractPayloadType> >& payloadList
            = Vehicle->SCMission()->PayloadSet()->AbstractPayload();

    if (!payloadList.isEmpty())
    {
        foreach (QSharedPointer<ScenarioAbstractPayloadType> payload, payloadList)
        {
            if (dynamic_cast<ScenarioTransmitterPayloadType*>(payload.data()))
            {
                ScenarioTransmitterPayloadType* myPayload
                        = dynamic_cast<ScenarioTransmitterPayloadType*>(payload.data());
                (SC.getNewPayloads()+i)->setPayloadName
                        (myPayload->ElementIdentifier()->Name());
		myPayload->ElementIdentifier()->setTheOrder(i);

                (SC.getNewPayloads()+i)->setPayloadHeight
                        (myPayload->Budget()->Size()->Height());
                (SC.getNewPayloads()+i)->setPayloadLength
                        (myPayload->Budget()->Size()->Length());
                (SC.getNewPayloads()+i)->setPayloadWidth
                        (myPayload->Budget()->Size()->Width());
                (SC.getNewPayloads()+i)->CalculateAndSetPayloadVolume
                        ("Rectangular_Prism");

                (SC.getNewPayloads()+i)->setPayloadDataRate
                        (myPayload->Budget()->DataRate());

                (SC.getNewPayloads()+i)->setPayloadMass
                        (myPayload->Budget()->Mass());

                (SC.getNewPayloads()+i)->setPayloadMaximumTemperature
                        (myPayload->Budget()->TemperatureRange()->MaximumTemperature());
                (SC.getNewPayloads()+i)->setPayloadMinimumTemperature
                        (myPayload->Budget()->TemperatureRange()->MinimumTemperature());

                (SC.getNewPayloads()+i)->setPayloadPowerInDaylight
                        (myPayload->Budget()->Power()->powerConsumptionInDaylight());
                (SC.getNewPayloads()+i)->setPayloadPowerInEclipse
                        (myPayload->Budget()->Power()->powerConsumptionInEclipse());
                (SC.getNewPayloads()+i)->setPayloadPowerOnPercentageInDaylight
                        (myPayload->Budget()->Power()->powerOnPercentageInDaylight());
                (SC.getNewPayloads()+i)->setPayloadPowerOnPercentageInEclipse
                        (myPayload->Budget()->Power()->powerOnPercentageInEclipse());

                (SC.getNewPayloads()+i)->setPayloadWavelength
                        (myPayload->Budget()->FrequencyBand());
            }
            if (dynamic_cast<ScenarioReceiverPayloadType*>(payload.data()))
            {
                ScenarioReceiverPayloadType* myPayload
                        = dynamic_cast<ScenarioReceiverPayloadType*>(payload.data());
                (SC.getNewPayloads()+i)->setPayloadName
                        (myPayload->ElementIdentifier()->Name());
		myPayload->ElementIdentifier()->setTheOrder(i);

                (SC.getNewPayloads()+i)->setPayloadHeight
                        (myPayload->Budget()->Size()->Height());
                (SC.getNewPayloads()+i)->setPayloadLength
                        (myPayload->Budget()->Size()->Length());
                (SC.getNewPayloads()+i)->setPayloadWidth
                        (myPayload->Budget()->Size()->Width());
                (SC.getNewPayloads()+i)->CalculateAndSetPayloadVolume
                        ("Rectangular_Prism");

                (SC.getNewPayloads()+i)->setPayloadDataRate
                        (myPayload->Budget()->DataRate());

                (SC.getNewPayloads()+i)->setPayloadMass
                        (myPayload->Budget()->Mass());

                (SC.getNewPayloads()+i)->setPayloadMaximumTemperature
                        (myPayload->Budget()->TemperatureRange()->MaximumTemperature());
                (SC.getNewPayloads()+i)->setPayloadMinimumTemperature
                        (myPayload->Budget()->TemperatureRange()->MinimumTemperature());

                (SC.getNewPayloads()+i)->setPayloadPowerInDaylight
                        (myPayload->Budget()->Power()->powerConsumptionInDaylight());
                (SC.getNewPayloads()+i)->setPayloadPowerInEclipse
                        (myPayload->Budget()->Power()->powerConsumptionInEclipse());
                (SC.getNewPayloads()+i)->setPayloadPowerOnPercentageInDaylight
                        (myPayload->Budget()->Power()->powerOnPercentageInDaylight());
                (SC.getNewPayloads()+i)->setPayloadPowerOnPercentageInEclipse
                        (myPayload->Budget()->Power()->powerOnPercentageInEclipse());

                (SC.getNewPayloads()+i)->setPayloadWavelength
                        (myPayload->Budget()->FrequencyBand());
            }
            if (dynamic_cast<ScenarioOpticalPayloadType*>(payload.data()))
            {
                ScenarioOpticalPayloadType* myPayload
                        = dynamic_cast<ScenarioOpticalPayloadType*>(payload.data());
                (SC.getNewPayloads()+i)->setPayloadName
                        (myPayload->ElementIdentifier()->Name());
		myPayload->ElementIdentifier()->setTheOrder(i);

                (SC.getNewPayloads()+i)->setPayloadHeight
                        (myPayload->Budget()->Size()->Height());
                (SC.getNewPayloads()+i)->setPayloadLength
                        (myPayload->Budget()->Size()->Length());
                (SC.getNewPayloads()+i)->setPayloadWidth
                        (myPayload->Budget()->Size()->Width());
                (SC.getNewPayloads()+i)->CalculateAndSetPayloadVolume
                        ("Rectangular_Prism");

                (SC.getNewPayloads()+i)->setPayloadDataRate
                        (myPayload->Budget()->DataRate());

                (SC.getNewPayloads()+i)->setPayloadMass
                        (myPayload->Budget()->Mass());

                (SC.getNewPayloads()+i)->setPayloadMaximumTemperature
                        (myPayload->Budget()->TemperatureRange()->MaximumTemperature());
                (SC.getNewPayloads()+i)->setPayloadMinimumTemperature
                        (myPayload->Budget()->TemperatureRange()->MinimumTemperature());

                (SC.getNewPayloads()+i)->setPayloadPowerInDaylight
                        (myPayload->Budget()->Power()->powerConsumptionInDaylight());
                (SC.getNewPayloads()+i)->setPayloadPowerInEclipse
                        (myPayload->Budget()->Power()->powerConsumptionInEclipse());
                (SC.getNewPayloads()+i)->setPayloadPowerOnPercentageInDaylight
                        (myPayload->Budget()->Power()->powerOnPercentageInDaylight());
                (SC.getNewPayloads()+i)->setPayloadPowerOnPercentageInEclipse
                        (myPayload->Budget()->Power()->powerOnPercentageInEclipse());

                (SC.getNewPayloads()+i)->setPayloadWavelength
                        (myPayload->Budget()->FrequencyBand());
            }
            if (dynamic_cast<ScenarioRadarPayloadType*>(payload.data()))
            {
                ScenarioRadarPayloadType* myPayload
                        = dynamic_cast<ScenarioRadarPayloadType*>(payload.data());
                (SC.getNewPayloads()+i)->setPayloadName
                        (myPayload->ElementIdentifier()->Name());
		myPayload->ElementIdentifier()->setTheOrder(i);

                (SC.getNewPayloads()+i)->setPayloadHeight
                        (myPayload->Budget()->Size()->Height());
                (SC.getNewPayloads()+i)->setPayloadLength
                        (myPayload->Budget()->Size()->Length());
                (SC.getNewPayloads()+i)->setPayloadWidth
                        (myPayload->Budget()->Size()->Width());
                (SC.getNewPayloads()+i)->CalculateAndSetPayloadVolume
                        ("Rectangular_Prism");

                (SC.getNewPayloads()+i)->setPayloadDataRate
                        (myPayload->Budget()->DataRate());

                (SC.getNewPayloads()+i)->setPayloadMass
                        (myPayload->Budget()->Mass());

                (SC.getNewPayloads()+i)->setPayloadMaximumTemperature
                        (myPayload->Budget()->TemperatureRange()->MaximumTemperature());
                (SC.getNewPayloads()+i)->setPayloadMinimumTemperature
                        (myPayload->Budget()->TemperatureRange()->MinimumTemperature());

                (SC.getNewPayloads()+i)->setPayloadPowerInDaylight
                        (myPayload->Budget()->Power()->powerConsumptionInDaylight());
                (SC.getNewPayloads()+i)->setPayloadPowerInEclipse
                        (myPayload->Budget()->Power()->powerConsumptionInEclipse());
                (SC.getNewPayloads()+i)->setPayloadPowerOnPercentageInDaylight
                        (myPayload->Budget()->Power()->powerOnPercentageInDaylight());
                (SC.getNewPayloads()+i)->setPayloadPowerOnPercentageInEclipse
                        (myPayload->Budget()->Power()->powerOnPercentageInEclipse());

                (SC.getNewPayloads()+i)->setPayloadWavelength
                        (myPayload->Budget()->FrequencyBand());
            }

            i++;//next payload
        }
    }

    SC.PassPayloadOutputParameters();

    //Structural shape has to be inserted ********** it has to be string
    SC.getNewSCStructure()->setSpacecraftDimension
            (Vehicle->System().data()->Structure().data()->Sizing().data()->width(),
             Vehicle->System().data()->Structure().data()->Sizing().data()->height(),
             Vehicle->System().data()->Structure().data()->Sizing().data()->length());

    SC.getNewSCStructure()->setSCVolume
            (Vehicle->System().data()->Structure().data()->Sizing().data()->volume());


//    SC.getNewSCStructure()->setPowerSubsystemMass
//            (Vehicle->System()->EPS()->massOfPCU()
//             + Vehicle->System()->EPS()->SolarArray()->massOfSolarArray()
//             + Vehicle->System()->EPS()->BatteryType()->totalMassOfBatteries());

    SC.getNewSCStructure()->setSCShape
            (Vehicle->System()->Structure()->StructuralShape());
    SC.getNewSCStructure()->setMaterialProperties
            (Vehicle->System()->Structure()->StructuralMaterial());
    SC.getNewSCStructure()->setThermalSubsystemMass
            (Vehicle->System()->TCS()->totalTCSMass());
    SC.getNewSCStructure()->setTTCSubsystemMass
            (Vehicle->System()->TTC()->totalTTCMass());
       SC.getNewSCCommunication()->setTTCSubsystemMass
            (Vehicle->System()->TTC()->totalTTCMass());
    SC.getNewSCStructure()->setOBDHSubsystemMass
            (Vehicle->System().data()->OBDH().data()->totalOBDHMass());
    SC.getNewSCStructure()->setPowerSubsystemMass
            (Vehicle->System()->EPS()->totalEPSMass());

    SC.getNewSCStructure()->setSCMass
            (Vehicle->System()->SystemBudgets()->MassOfSystem()->dryMass());
    qDebug()<<"DRY MASS"<<Vehicle->System()->SystemBudgets()->MassOfSystem()->dryMass();


    SC.getNewSCStructure()->setMomentsOfInertia
            (Vehicle->System().data()->Structure().data()->MomentsOfInertia().data()->xAxis(),
             Vehicle->System().data()->Structure().data()->MomentsOfInertia().data()->yAxis(),
             Vehicle->System().data()->Structure().data()->MomentsOfInertia().data()->zAxis());
    SC.getNewSCStructure()->setSecondMomentsOfArea
            (Vehicle->System().data()->Structure().data()->SecondMomentsOfArea().data()->xAxis(),
             Vehicle->System().data()->Structure().data()->SecondMomentsOfArea().data()->yAxis(),
             Vehicle->System().data()->Structure().data()->SecondMomentsOfArea().data()->zAxis());
    SC.getNewSCStructure()->setLateralFrequency
            (Vehicle->System().data()->Structure().data()->NaturalFrequency().data()->lateralFrequency());
    SC.getNewSCStructure()->setAxialFrequency
            (Vehicle->System().data()->Structure().data()->NaturalFrequency().data()->axialFrequency());
    SC.getNewSCStructure()->setStructureSubsystemMass
            (Vehicle->System().data()->Structure().data()->totalStructureMass());

//    SC.PassStructureSubsystemOutputParameters();

    SC.getNewSCThermal()->setMaximumSCTempWithRadiatorOrHeater
            (Vehicle->System().data()->TCS().data()->Temperature().data()->maximumSCTemperature());
    SC.getNewSCThermal()->setMinimumSCTempWithRadiatorOrHeater
            (Vehicle->System().data()->TCS().data()->Temperature().data()->minimumSCTemperature());
    SC.getNewSCThermal()->setNeededHeater
            (Vehicle->System().data()->TCS().data()->heaterPower());
    SC.getNewSCThermal()->setNeededRadiator
            (Vehicle->System().data()->TCS().data()->radiatedPower());

    //**********************************************************************----


    SC.getNewSCThermal()->setColdFaceCoatingProperties //the name has to be in scenario
            (Vehicle->System()->TCS()->CoatingArea()->ColdSurface()->ColdCoating()->ElementIdentifier()->Name(),
             Vehicle->System().data()->TCS().data()->CoatingArea().data()->ColdSurface().data()->ColdCoating().data()->emissivity(),
             Vehicle->System().data()->TCS().data()->CoatingArea().data()->ColdSurface().data()->ColdCoating().data()->absorptivity());
    SC.getNewSCThermal()->setHotFaceCoatingProperties //the name has to be in scenario
            (Vehicle->System()->TCS()->CoatingArea()->HotSurface()->HotCoating()->ElementIdentifier()->Name(),
             Vehicle->System().data()->TCS().data()->CoatingArea().data()->HotSurface().data()->HotCoating().data()->emissivity(),
             Vehicle->System().data()->TCS().data()->CoatingArea().data()->HotSurface().data()->HotCoating().data()->absorptivity());
    // the cold and hot face area has to be included

//    SC.PassThermalSubsystemOutputParameters();

    SC.getNewSCPower()->SolarArrays.setSolarCell
            (Vehicle->System().data()->EPS().data()->SolarArray().data()->SACellType());
    SC.getNewSCPower()->SolarArrays.setSAArea
            (Vehicle->System().data()->EPS().data()->SolarArray().data()->areaOfSolarArray());
    SC.getNewSCPower()->SolarArrays.setSAMass
            (Vehicle->System().data()->EPS().data()->SolarArray().data()->massOfSolarArray());
    SC.getNewSCPower()->Battery.setBatteryProperties
            (Vehicle->System().data()->EPS().data()->BatteryType().data()->BatteryType());
    SC.getNewSCPower()->Battery.setBMass
            (Vehicle->System().data()->EPS().data()->BatteryType().data()->totalMassOfBatteries());
    SC.getNewSCPower()->Battery.setBVolume
            (Vehicle->System().data()->EPS().data()->BatteryType().data()->totalVolumeOfBatteries());
    SC.getNewSCPower()->Battery.setNumberOfBatteries
            (Vehicle->System().data()->EPS().data()->BatteryType().data()->numberOfBatteries());
    SC.getNewSCPower()->SolarArrays.setPCUMass
            (Vehicle->System().data()->EPS().data()->massOfPCU());
    SC.getNewSCPower()->setThermalSubsystemPower
            (Vehicle->System()->TCS()->heaterPower());
    SC.getNewSCPower()->setTTCSubsystemPower
            (Vehicle->System()->TTC()->TTCAntenna()->TransmitterPower());

//    SC.PassPowerSubsystemOutputParameters();

    SC.getNewSCCommunication()->setTTCSubsystemMass
            (Vehicle->System().data()->TTC().data()->totalTTCMass());
    SC.getNewSCCommunication()->setAntennaPower
            (Vehicle->System().data()->TTC().data()->TTCAntenna().data()->TransmitterPower());
    SC.getNewSCCommunication()->setAntennaFrequency
            (Vehicle->System().data()->TTC().data()->TTCAntenna().data()->TransmitterFrequency());
    SC.getNewSCCommunication()->setAntennaGain
            (Vehicle->System().data()->TTC().data()->TTCAntenna().data()->TransmitterGain());
    SC.getNewSCCommunication()->setAntennaEfficiency
            (Vehicle->System().data()->TTC().data()->TTCAntenna().data()->TransmitterEfficiency());
    SC.getNewSCCommunication()->setPercentageOfContactTimePerOrbit
            (Vehicle->System().data()->TTC().data()->TTCAntenna().data()->LinkDuration());
    SC.getNewSCCommunication()->setDownlinkRate
            (Vehicle->System().data()->TTC().data()->TTCAntenna().data()->DataRate());
    SC.getNewSCCommunication()->setMemorySizeForPayloads
            (Vehicle->System().data()->OBDH().data()->totalSizeOfMemory());

    SC.getNewSCPower()->CalculateAndSetTotalSCPower();
    SC.getNewSCPower()->SolarArrays.CalculateSABOLPower();

    SC.getNewSCDataHandling()->setMemorySizeForPayloads
            (Vehicle->System().data()->OBDH().data()->totalSizeOfMemory());

//    SC.PassOBDHSubsystemOutputParameters();
    //--------------------------------system budget transfer from the scenario is completed -------


    //calculate the missing values---------------------------------

    //make calculations of TTC
    SC.getNewSCCommunication()->CalculateAndSetAntennaDiameter();
//    SC.PassTTCSubsystemOutputParameters();

    SC.PassOBDHSubsystemOutputParameters();
    SC.PassTTCSubsystemOutputParameters();
    SC.PassPowerSubsystemOutputParameters();
    SC.PassStructureSubsystemOutputParameters();
    SC.PassThermalSubsystemOutputParameters();
    SC.PassPowerSubsystemOutputParameters();
    SC.PassStructureSubsystemOutputParameters();

//    SC.getNewSCThermal()->CalculateAndSetAlbedoHeat();
//    SC.getNewSCThermal()->CalculateAndSetPlanetIRHeat();
//    SC.getNewSCThermal()->CalculateAndSetSolarFluxHeat();

    qDebug()<<"SC MASS after retreive"<<SC.getNewSCStructure()->getSCMass();
}
