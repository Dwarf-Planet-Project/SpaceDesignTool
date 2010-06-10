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

#include "semwizard.h"
#include "semmaingui.h"

#include "QDebug"
#include <QList>
#include <QSharedPointer>

SEMWizard::SEMWizard(ScenarioSC* SCVehicle, QWidget * parent, Qt::WindowFlags f)
    : QWizard(parent,f)
{
    setupUi(this);

    //extract the scenario information of the SC that is wanted to be
    //analyse by SEM
    Vehicle = SCVehicle;

    const QList<QSharedPointer<ScenarioAbstractTrajectoryType> >& trajectoryList
            = Vehicle->SCMission().data()->TrajectoryPlan().data()->AbstractTrajectory();

    // Initial state is stored in the first trajectory (for now); so,
    // the empty trajectory plan case has to be treated specially.
    if (!trajectoryList.isEmpty())
    {
        // details of the mission arcs when do they start when do they end
        foreach (QSharedPointer<ScenarioAbstractTrajectoryType> trajectory, trajectoryList)
        {
            if (dynamic_cast<ScenarioLoiteringType*>(trajectory.data()))
            {
                //for now SEM is only capable of doing systems engineering of loitering
                ScenarioLoiteringType* loitering
                        = dynamic_cast<ScenarioLoiteringType*>(trajectory.data());

                //collect the mission details and display them in the wizard
                //mission start and mission end dateTime
                SC.getSCMissionDetails()->setMissionStartTime
                        (loitering->TimeLine().data()->StartTime());
                MissionStartTimeDateTimeEdit->setDateTime
                        (SC.getSCMissionDetails()->getMissionStartTime());

                SC.getSCMissionDetails()->setMissionEndTime
                        (loitering->TimeLine().data()->EndTime());
                MissionEndTimeDateTimeEdit->setDateTime
                        (SC.getSCMissionDetails()->getMissionEndTime());
            }
        }
    }
}

SEMWizard::~SEMWizard()
{
    //make the calculations

    //make calculaions of Data handling
    SC.getNewSCDataHandling()->CalculateAndSetMemorySizeForPayloads();

    SC.PassOBDHSubsystemOutputParameters();

    //make calculations of communication
    SC.getNewSCCommunication()->CalculateAndSetAntennaDiameter();
    SC.getNewSCCommunication()->CalculateAndSetAntennaVolume();
    SC.getNewSCCommunication()->CalculateAndSetContactTimePerOrbit();

    SC.PassTTCSubsystemOutputParameters();

    //make calclations for power

    //calculate SAEOLPower needed
    SC.getNewSCPower()->SolarArrays.CalculateSAEOLPower();
    SC.getNewSCPower()->SolarArrays.CalculateSolarCellEOLPower();

    //calculate and visualize array of the solar array
    SC.getNewSCPower()->SolarArrays.CalculateArea();
      qDebug()<<"BOL"<<SC.getNewSCPower()->SolarArrays.getSolarCellBOLPower();
    SC.getNewSCPower()->SolarArrays.CalculateSABOLPower();
    SC.getNewSCPower()->SolarArrays.CalculateSAMass();
    SC.getNewSCPower()->SolarArrays.CalculatePCUMass();

    SC.getNewSCPower()->CalculateAndSetPowerSubsystemMass();
    SC.getNewSCStructure()->setPowerSubsystemMass
            (SC.getNewSCPower()->getPowersubsystemMass());

    SC.getNewSCStructure()->setPowerSubsystemVolume
            (SC.getNewSCPower()->getPowerSubsystemVolume());

//    //make calculations of structure
////    SC
//    SC.getNewSCStructure()->CalculateAndSetSpacecraftDimension();
//
    SC.PassStructureSubsystemOutputParameters();
//
//    //make calculations of thermal
//    SC.getNewSCThermal()->CalculateAndSetSolarFluxHeat();
//    SC.getNewSCThermal()->CalculateAndSetAlbedoHeat();
//    SC.getNewSCThermal()->CalculateAndSetPlanetIRHeat();
//    SC.getNewSCThermal()->CalculateAndSetSCTemperatureRange();
//    SC.getNewSCThermal()->CalculateAndSetNeededHeater();
//    SC.getNewSCThermal()->CalculateAndSetNeededRadiator();
//
//    SC.PassThermalSubsystemOutputParameters();
//
//    //make calculations of power
//    SC.getNewSCPower()->CalculateAndSetTotalSCPower();
//    SC.getNewSCPower()->CalculateAndSetPowerSubsystemMass();
//    SC.getNewSCPower()->SolarArrays.CalculateSolarCellBOLPower();
//    SC.getNewSCPower()->SolarArrays.CalculateSolarCellEOLPower();
//    SC.getNewSCPower()->SolarArrays.CalculateSABOLPower();
//
//    SC.PassPowerSubsystemOutputParameters();

    //open the GUI
    SemMainGUI  MainGUI(&SC,Vehicle,this);
    MainGUI.exec();
}

void SEMWizard::on___qt__passive_wizardbutton0_clicked()
{
        qDebug()<<"this->currentId()"<<this->currentId();
	qWarning("TODO: %s	%d",__FILE__,__LINE__);
}

void SEMWizard::on___qt__passive_wizardbutton1_clicked()
{
    switch (this->currentId())
    {
    case 1:
        break;

    case 2:
        //set the mission details
        SC.PassMissionDetailsOutputParameters();
        break;

    case 3:
        break;

    case 4:
        break;

    case 5:
        break;

    case 6:
        break;

    case 7:
        break;

    case 8:
        break;

    case 9:
        break;

    case 10:
        //calculate volume of Payloads
        int i;
        for (i=0;i<4;i++)
        {
            if ((SC.getNewPayloads()+i)->getPayloadDimensions().x()>0.0)
            {
                if ((SC.getNewPayloads()+i)->getPayloadDimensions().y()>0.0)
                {
                    if ((SC.getNewPayloads()+i)->getPayloadDimensions().z()>0.0)
                        (SC.getNewPayloads()+i)->CalculateAndSetPayloadVolume("Rectangular_Prism");
                    else
                        (SC.getNewPayloads()+i)->CalculateAndSetPayloadVolume("Cylinder");
                }
                else
                    (SC.getNewPayloads()+i)->CalculateAndSetPayloadVolume("Sphere");
            }
            else
                (SC.getNewPayloads()+i)->CalculateAndSetPayloadVolume("");
        }
        SC.PassPayloadOutputParameters();
        break;
    }
        qDebug()<<"this->currentId()"<<this->currentId();
	qWarning("TODO: %s	%d",__FILE__,__LINE__);
}

void SEMWizard::on_MissionTypeComboBox_activated(const QString&)
{
    //choose the mission type for mass estimations
    //(in terms of percentages)
    if (MissionTypeComboBox->currentIndex()!= 0)
        SC.getNewSCStructure()->setMassEstimations
                (MissionTypeComboBox->currentText());
	qWarning("TODO: %s	%d",__FILE__,__LINE__);
}

void SEMWizard::on_MissionStartTimeDateTimeEdit_dateTimeChanged
        (const QDateTime &datetime)
{
    SC.getSCMissionDetails()->setMissionStartTime(datetime);
	qWarning("TODO: %s	%d",__FILE__,__LINE__);
}

void SEMWizard::on_MissionEndTimeDateTimeEdit_dateTimeChanged
        (const QDateTime &datetime)
{
    SC.getSCMissionDetails()->setMissionEndTime(datetime);
	qWarning("TODO: %s	%d",__FILE__,__LINE__);
}

void SEMWizard::on_SemimajorAxisLineEdit_textEdited(const QString&)
{
    //if we select a planet from the menu
    if ((PlanetNameComboBox->currentIndex()!= 0)
        &&(SemimajorAxisLineEdit->text().toDouble() > 0.0))
        {
        SC.getSCMissionDetails()->setSemiMajorAxis
                (SemimajorAxisLineEdit->text().toDouble());
        SC.getSCMissionDetails()->setPlanetProperties
                (PlanetNameComboBox->currentText());
        }
	qWarning("TODO: %s	%d",__FILE__,__LINE__);
}

void SEMWizard::on_EccentricityLineEdit_textEdited(const QString&)
{
    SC.getSCMissionDetails()->setEccentricity
            (EccentricityLineEdit->text().toDouble());
	qWarning("TODO: %s	%d",__FILE__,__LINE__);
}

void SEMWizard::on_InclinationLineEdit_textEdited(const QString&)
{
    SC.getSCMissionDetails()->setInclination
            (InclinationLineEdit->text().toDouble());
	qWarning("TODO: %s	%d",__FILE__,__LINE__);
}

void SEMWizard::on_RAANLineEdit_textEdited(const QString&)
{
    SC.getSCMissionDetails()->setRightAscensionNode
            (RAANLineEdit->text().toDouble());
	qWarning("TODO: %s	%d",__FILE__,__LINE__);
}

void SEMWizard::on_ArgumentOfPeriapsisLineEdit_textEdited(const QString&)
{
    SC.getSCMissionDetails()->setArgumentOfPerigee
            (ArgumentOfPeriapsisLineEdit->text().toDouble());
	qWarning("TODO: %s	%d",__FILE__,__LINE__);
}

void SEMWizard::on_TrueAnomalyLineEdit_textEdited(const QString&)
{
    SC.getSCMissionDetails()->setTrueAnomaly
            (TrueAnomalyLineEdit->text().toDouble());
	qWarning("TODO: %s	%d",__FILE__,__LINE__);
}

void SEMWizard::on_NewPayloadRadioButton_toggled(bool)
{
    if (NewPayloadRadioButton->isChecked())
    {
        PayloadSetComboBox->setEnabled(0);

        //set the Payload entry lines editable which enables the user to
        //change the payload parameters as a consequence
        PayloadNameLineEdit->setEnabled(1);
        PayloadResolutionLineEdit->setEnabled(0);
        PayloadWavelengthLineEdit->setEnabled(0);
        NumberOfPixelLineEdit->setEnabled(0);
    }
}

void SEMWizard::on_DatabasePayloadRadioButton_toggled(bool)
{
    if (DatabasePayloadRadioButton->isChecked())
    {
        PayloadSetComboBox->setEnabled(1);
        PayloadNameLineEdit->setEnabled(0);
    }

	qWarning("TODO: %s	%d",__FILE__,__LINE__);
}

void SEMWizard::on_PayloadSetComboBox_activated(const QString&)
{
    if (PayloadSetComboBox->currentIndex()!=0)
    {
        (SC.getNewPayloads()+0)->setSelectedPayloadProperties
                (PayloadSetComboBox->currentText());

        //if the payload has wavlength properties user can change the values
        if ((SC.getNewPayloads()+0)->getPayloadWavelength()>0.0)
        {
            ResolutionLabel->setEnabled(1);
            PayloadResolutionLineEdit->setEnabled(1);
            WavelengthLabel->setEnabled(1);
            PayloadWavelengthLineEdit->setEnabled(1);
            if (PayloadSetComboBox->currentText()=="Photometric_Imaging")
            {
                NumberOfPixelLineEdit->setEnabled(1);
                NumberOfPixelsLabel->setEnabled(1);
            }
            else
            {
                NumberOfPixelLineEdit->setEnabled(0);
                NumberOfPixelsLabel->setEnabled(0);
            }
        }
        else
        {
            ResolutionLabel->setEnabled(0);
            PayloadResolutionLineEdit->setEnabled(0);
            WavelengthLabel->setEnabled(0);
            PayloadWavelengthLineEdit->setEnabled(0);
        }

        //fill the payload properties from database
        PayloadNameLineEdit->setText
                ((SC.getNewPayloads()+0)->getPayloadName());

        //set the GUI
        PayloadWidthLineEdit->setText
                (QString::number
                 ((SC.getNewPayloads()+0)->getPayloadWidth()));
        PayladLengthLineEdit->setText
                (QString::number
                 ((SC.getNewPayloads()+0)->getPayloadLength()));
        PayloadHeightLineEdit->setText
                (QString::number
                 ((SC.getNewPayloads()+0)->getPayloadHeight()));
        PayloadMassLineEdit->setText
                (QString::number
                 ((SC.getNewPayloads()+0)->getPayloadMass()));
        PayloadPowerEclipseLineEdit->setText
                (QString::number
                 ((SC.getNewPayloads()+0)->getPayloadPowerInEclipse()));
        PayloadPowerDaylightLineEdit->setText
                (QString::number
                 ((SC.getNewPayloads()+0)->getPayloadPowerInDaylight()));

        PayloadDataRateLineEdit->setText
                (QString::number
                 ((SC.getNewPayloads()+0)->getPayloadDataRate()));
        PayloadPowerTimeEclipseLineEdit->setText
                (QString::number
                 ((SC.getNewPayloads()+0)->getPayloadPowerOnPercentageInEclipse()));
        PayloadPowerTimeDaylightLineEdit->setText
                (QString::number
                 ((SC.getNewPayloads()+0)->getPayloadPowerOnPercentageInDaylight()));
        PayloadMinTempLineEdit->setText
                (QString::number
                 ((SC.getNewPayloads()+0)->getPayloadMinimumTemperature()));
        PayloadMaxTempLineEdit->setText
                (QString::number
                 ((SC.getNewPayloads()+0)->getPayloadMaximumTemperature()));
    }

	qWarning("TODO: %s	%d",__FILE__,__LINE__);
}

void SEMWizard::on_PayloadNameLineEdit_textEdited(const QString&)
{
    (SC.getNewPayloads()+0)->setPayloadName
            (PayloadNameLineEdit->text());
	qWarning("TODO: %s	%d",__FILE__,__LINE__);
}

void SEMWizard::on_PayloadResolutionLineEdit_textEdited(const QString&)
{
    //in case, at least one of them ýs zero that means user wants
    //to reset the values
    (SC.getNewPayloads()+0)->setSelectedPayloadProperties
            (PayloadSetComboBox->currentText());

    if ((PayloadWavelengthLineEdit->text().toDouble()>0.0)
        &&(PayloadResolutionLineEdit->text().toDouble()>0.0))
    {
        if (PayloadSetComboBox->currentText()=="Photometric_Imaging")
        {
            //for planet observation linear resolution
            (SC.getNewPayloads()+0)->CalculateNewDimensions
                    (PayloadWavelengthLineEdit->text().toDouble(),
                     PayloadResolutionLineEdit->text().toDouble(),
                     SC.getSCMissionDetails()->getSCMeanAltitude());
        }
        else
        {
            // for star observation angular resolution
            (SC.getNewPayloads()+0)->CalculateNewDimensions
                    (PayloadWavelengthLineEdit->text().toDouble(),
                     PayloadResolutionLineEdit->text().toDouble());
        }

        if ((SC.getNewPayloads()+0)->getPayloadDimensions().z()>0.0)
            (SC.getNewPayloads()+0)->CalculateAndSetPayloadVolume
                    ("Rectangular_Prism");
        else
            (SC.getNewPayloads()+0)->CalculateAndSetPayloadVolume
                    ("Cylinder");
    }

    //refresh the GUI with the changes in the parameters
    PayloadWidthLineEdit->setText
            (QString::number
             ((SC.getNewPayloads()+0)->getPayloadWidth()));
    PayladLengthLineEdit->setText
            (QString::number
             ((SC.getNewPayloads()+0)->getPayloadLength()));
    PayloadHeightLineEdit->setText
            (QString::number
             ((SC.getNewPayloads()+0)->getPayloadHeight()));
    PayloadMassLineEdit->setText
            (QString::number
             ((SC.getNewPayloads()+0)->getPayloadMass()));
    PayloadPowerEclipseLineEdit->setText
            (QString::number
             ((SC.getNewPayloads()+0)->getPayloadPowerInEclipse()));
    PayloadPowerDaylightLineEdit->setText
            (QString::number
             ((SC.getNewPayloads()+0)->getPayloadPowerInDaylight()));

	qWarning("TODO: %s	%d",__FILE__,__LINE__);
}

void SEMWizard::on_PayloadWavelengthLineEdit_textEdited(const QString&)
{
    //in case, at least one of them ýs zero that means user wants
    //to reset the values
    (SC.getNewPayloads()+0)->setSelectedPayloadProperties
            (PayloadSetComboBox->currentText());

    if ((PayloadWavelengthLineEdit->text().toDouble()>0.0)
        &&(PayloadResolutionLineEdit->text().toDouble()>0.0))
    {
        if (PayloadSetComboBox->currentText()=="Photometric_Imaging")
        {
            //for planet observation linear resolution
            (SC.getNewPayloads()+0)->CalculateNewDimensions
                    (PayloadWavelengthLineEdit->text().toDouble(),
                     PayloadResolutionLineEdit->text().toDouble(),
                     SC.getSCMissionDetails()->getSCMeanAltitude());
        }
        else
        {
            // for star observation angular resolution
            (SC.getNewPayloads()+0)->CalculateNewDimensions
                    (PayloadWavelengthLineEdit->text().toDouble(),
                     PayloadResolutionLineEdit->text().toDouble());
        }
        if ((SC.getNewPayloads()+0)->getPayloadDimensions().z()>0.0)
            (SC.getNewPayloads()+0)->CalculateAndSetPayloadVolume
                    ("Rectangular_Prism");
        else
            (SC.getNewPayloads()+0)->CalculateAndSetPayloadVolume
                    ("Cylinder");
    }

    //refresh the GUI
    PayloadWidthLineEdit->setText
            (QString::number
             ((SC.getNewPayloads()+0)->getPayloadWidth()));
    PayladLengthLineEdit->setText
            (QString::number
             ((SC.getNewPayloads()+0)->getPayloadLength()));
    PayloadHeightLineEdit->setText
            (QString::number
             ((SC.getNewPayloads()+0)->getPayloadHeight()));
    PayloadMassLineEdit->setText
            (QString::number
             ((SC.getNewPayloads()+0)->getPayloadMass()));
    PayloadPowerEclipseLineEdit->setText
            (QString::number
             ((SC.getNewPayloads()+0)->getPayloadPowerInEclipse()));
    PayloadPowerDaylightLineEdit->setText
            (QString::number
             ((SC.getNewPayloads()+0)->getPayloadPowerInDaylight()));

	qWarning("TODO: %s	%d",__FILE__,__LINE__);
}

void SEMWizard::on_PayloadWidthLineEdit_textEdited(const QString&)
{
    (SC.getNewPayloads()+0)->setPayloadWidth
            (PayloadWidthLineEdit->text().toDouble());
	qWarning("TODO: %s	%d",__FILE__,__LINE__);
}

void SEMWizard::on_PayladLengthLineEdit_textEdited(const QString&)
{
    (SC.getNewPayloads()+0)->setPayloadLength
            (PayladLengthLineEdit->text().toDouble());
	qWarning("TODO: %s	%d",__FILE__,__LINE__);
}

void SEMWizard::on_PayloadHeightLineEdit_textEdited(const QString&)
{
    (SC.getNewPayloads()+0)->setPayloadHeight
            (PayloadHeightLineEdit->text().toDouble());
	qWarning("TODO: %s	%d",__FILE__,__LINE__);
}

void SEMWizard::on_PayloadMassLineEdit_textEdited(const QString&)
{
    (SC.getNewPayloads()+0)->setPayloadMass
            (PayloadMassLineEdit->text().toDouble());
	qWarning("TODO: %s	%d",__FILE__,__LINE__);
}

void SEMWizard::on_PayloadDataRateLineEdit_textEdited(const QString&)
{
    (SC.getNewPayloads()+0)->setPayloadDataRate
            (PayloadDataRateLineEdit->text().toDouble());
	qWarning("TODO: %s	%d",__FILE__,__LINE__);
}

void SEMWizard::on_NumberOfPixelLineEdit_textEdited(const QString&)
{
    //fix it according to linear resolution
    (SC.getNewPayloads()+0)->CalculateNewDataRate
            (SC.getSCMissionDetails()->getGroundAverageVelocity(),
             PayloadResolutionLineEdit->text().toDouble(),
             NumberOfPixelLineEdit->text().toDouble());

    PayloadDataRateLineEdit->setText
            (QString::number
             ((SC.getNewPayloads()+0)->getPayloadDataRate()));
	qWarning("TODO: %s	%d",__FILE__,__LINE__);
}

void SEMWizard::on_PayloadPowerEclipseLineEdit_textEdited(const QString&)
{
    (SC.getNewPayloads()+0)->setPayloadPowerInEclipse
            (PayloadPowerEclipseLineEdit->text().toDouble());
	qWarning("TODO: %s	%d",__FILE__,__LINE__);
}

void SEMWizard::on_PayloadPowerDaylightLineEdit_textEdited(const QString&)
{
    (SC.getNewPayloads()+0)->setPayloadPowerInDaylight
            (PayloadPowerDaylightLineEdit->text().toDouble());
	qWarning("TODO: %s	%d",__FILE__,__LINE__);
}

void SEMWizard::on_PayloadPowerTimeEclipseLineEdit_textEdited(const QString&)
{
    (SC.getNewPayloads()+0)->setPayloadPowerOnPercentageInEclipse
            (PayloadPowerTimeEclipseLineEdit->text().toDouble());
	qWarning("TODO: %s	%d",__FILE__,__LINE__);
}

void SEMWizard::on_PayloadPowerTimeDaylightLineEdit_textEdited(const QString&)
{
    (SC.getNewPayloads()+0)->setPayloadPowerOnPercentageInDaylight
            (PayloadPowerTimeDaylightLineEdit->text().toDouble());
	qWarning("TODO: %s	%d",__FILE__,__LINE__);
}

void SEMWizard::on_PayloadMinTempLineEdit_textEdited(const QString&)
{
    (SC.getNewPayloads()+0)->setPayloadMinimumTemperature
            (PayloadMinTempLineEdit->text().toDouble());
	qWarning("TODO: %s	%d",__FILE__,__LINE__);
}

void SEMWizard::on_PayloadMaxTempLineEdit_textEdited(const QString&)
{
    (SC.getNewPayloads()+0)->setPayloadMaximumTemperature
            (PayloadMaxTempLineEdit->text().toDouble());
	qWarning("TODO: %s	%d",__FILE__,__LINE__);
}

void SEMWizard::on_NumberOfPayloadCheckBox_toggled(bool)
{
	qWarning("TODO: %s	%d",__FILE__,__LINE__);
}
//*******************************************************************************
void SEMWizard::on_NewPayloadRadioButton_2_toggled(bool)
{
    if (NewPayloadRadioButton_2->isChecked())
    {
        PayloadSetComboBox_2->setEnabled(0);

        //set the Payload entry lines editable
        PayloadNameLineEdit_2->setEnabled(1);
        PayloadResolutionLineEdit_2->setEnabled(0);
        PayloadWavelengthLineEdit_2->setEnabled(0);
        NumberOfPixelLineEdit_2->setEnabled(0);
    }
	qWarning("TODO: %s	%d",__FILE__,__LINE__);
}

void SEMWizard::on_PayloadSetComboBox_2_activated(const QString&)
{
    if (PayloadSetComboBox_2->currentIndex()!=0)
    {
        (SC.getNewPayloads()+1)->setSelectedPayloadProperties
                (PayloadSetComboBox_2->currentText());

        //if the payload has wavlength properties user can change the values
        if ((SC.getNewPayloads()+1)->getPayloadWavelength()>0.0)
        {
            ResolutionLabel_2->setEnabled(1);
            PayloadResolutionLineEdit_2->setEnabled(1);
            WavelengthLabel_2->setEnabled(1);
            PayloadWavelengthLineEdit_2->setEnabled(1);
            if (PayloadSetComboBox_2->currentText()=="Photometric_Imaging")
            {
                NumberOfPixelLineEdit_2->setEnabled(1);
                NumberOfPixelsLabel_2->setEnabled(1);
            }
            else
            {
                NumberOfPixelLineEdit_2->setEnabled(0);
                NumberOfPixelsLabel_2->setEnabled(0);
            }
        }
        else
        {
            ResolutionLabel_2->setEnabled(0);
            PayloadResolutionLineEdit_2->setEnabled(0);
            WavelengthLabel_2->setEnabled(0);
            PayloadWavelengthLineEdit_2->setEnabled(0);
        }

        //fill the payload properties from database
        PayloadNameLineEdit_2->setText
                ((SC.getNewPayloads()+1)->getPayloadName());

        //set the GUI
        PayloadWidthLineEdit_2->setText
                (QString::number
                 ((SC.getNewPayloads()+1)->getPayloadWidth()));
        PayladLengthLineEdit_2->setText
                (QString::number
                 ((SC.getNewPayloads()+1)->getPayloadLength()));
        PayloadHeightLineEdit_2->setText
                (QString::number
                 ((SC.getNewPayloads()+1)->getPayloadHeight()));
        PayloadMassLineEdit_2->setText
                (QString::number
                 ((SC.getNewPayloads()+1)->getPayloadMass()));
        PayloadPowerEclipseLineEdit_2->setText
                (QString::number
                 ((SC.getNewPayloads()+1)->getPayloadPowerInEclipse()));
        PayloadPowerDaylightLineEdit_2->setText
                (QString::number
                 ((SC.getNewPayloads()+1)->getPayloadPowerInDaylight()));

        PayloadDataRateLineEdit_2->setText
                (QString::number
                 ((SC.getNewPayloads()+1)->getPayloadDataRate()));
        PayloadPowerTimeEclipseLineEdit_2->setText
                (QString::number
                 ((SC.getNewPayloads()+1)->getPayloadPowerOnPercentageInEclipse()));
        PayloadPowerTimeDaylightLineEdit_2->setText
                (QString::number
                 ((SC.getNewPayloads()+1)->getPayloadPowerOnPercentageInDaylight()));
        PayloadMinTempLineEdit_2->setText
                (QString::number
                 ((SC.getNewPayloads()+1)->getPayloadMinimumTemperature()));
        PayloadMaxTempLineEdit_2->setText
                (QString::number
                 ((SC.getNewPayloads()+1)->getPayloadMaximumTemperature()));
    }
	qWarning("TODO: %s	%d",__FILE__,__LINE__);
}

void SEMWizard::on_DatabasePayloadRadioButton_2_toggled(bool)
{
    if (DatabasePayloadRadioButton_2->isChecked())
    {
        PayloadSetComboBox_2->setEnabled(1);
        PayloadNameLineEdit_2->setEnabled(0);
    }
	qWarning("TODO: %s	%d",__FILE__,__LINE__);
}

void SEMWizard::on_PayloadNameLineEdit_2_textEdited(const QString&)
{
    (SC.getNewPayloads()+1)->setPayloadName
            (PayloadNameLineEdit_2->text());
	qWarning("TODO: %s	%d",__FILE__,__LINE__);
}

void SEMWizard::on_PayloadResolutionLineEdit_2_textEdited(const QString&)
{
    //in case, at least one of them ýs zero that means user wants
    //to reset the values
    (SC.getNewPayloads()+1)->setSelectedPayloadProperties
            (PayloadSetComboBox_2->currentText());

    if ((PayloadWavelengthLineEdit_2->text().toDouble()>0.0)
        &&(PayloadResolutionLineEdit_2->text().toDouble()>0.0))
    {
        if (PayloadSetComboBox_2->currentText()=="Photometric_Imaging")
        {
            //for planet observation linear resolution
            (SC.getNewPayloads()+1)->CalculateNewDimensions
                    (PayloadWavelengthLineEdit_2->text().toDouble(),
                     PayloadResolutionLineEdit_2->text().toDouble(),
                     SC.getSCMissionDetails()->getSCMeanAltitude());
        }
        else
        {
            // for star observation angular resolution
            (SC.getNewPayloads()+1)->CalculateNewDimensions
                    (PayloadWavelengthLineEdit_2->text().toDouble(),
                     PayloadResolutionLineEdit_2->text().toDouble());
        }

        if ((SC.getNewPayloads()+1)->getPayloadDimensions().z()>0.0)
            (SC.getNewPayloads()+1)->CalculateAndSetPayloadVolume
                    ("Rectangular_Prism");
        else
            (SC.getNewPayloads()+1)->CalculateAndSetPayloadVolume
                    ("Cylinder");
    }

    //refresh the GUI
    PayloadWidthLineEdit_2->setText
            (QString::number
             ((SC.getNewPayloads()+1)->getPayloadWidth()));
    PayladLengthLineEdit_2->setText
            (QString::number
             ((SC.getNewPayloads()+1)->getPayloadLength()));
    PayloadHeightLineEdit_2->setText
            (QString::number
             ((SC.getNewPayloads()+1)->getPayloadHeight()));
    PayloadMassLineEdit_2->setText
            (QString::number
             ((SC.getNewPayloads()+1)->getPayloadMass()));
    PayloadPowerEclipseLineEdit_2->setText
            (QString::number
             ((SC.getNewPayloads()+1)->getPayloadPowerInEclipse()));
    PayloadPowerDaylightLineEdit_2->setText
            (QString::number
             ((SC.getNewPayloads()+1)->getPayloadPowerInDaylight()));

	qWarning("TODO: %s	%d",__FILE__,__LINE__);
}

void SEMWizard::on_PayloadWavelengthLineEdit_2_textEdited(const QString&)
{
    //in case, at least one of them ýs zero that means user wants
    //to reset the values
    (SC.getNewPayloads()+1)->setSelectedPayloadProperties
            (PayloadSetComboBox_2->currentText());

    if ((PayloadWavelengthLineEdit_2->text().toDouble()>0.0)
        &&(PayloadResolutionLineEdit_2->text().toDouble()>0.0))
    {
        if (PayloadSetComboBox_2->currentText()=="Photometric_Imaging")
        {
            //for planet observation linear resolution
            (SC.getNewPayloads()+1)->CalculateNewDimensions
                    (PayloadWavelengthLineEdit_2->text().toDouble(),
                     PayloadResolutionLineEdit_2->text().toDouble(),
                     SC.getSCMissionDetails()->getSCMeanAltitude());
        }
        else
        {
            // for star observation angular resolution
            (SC.getNewPayloads()+1)->CalculateNewDimensions
                    (PayloadWavelengthLineEdit_2->text().toDouble(),
                     PayloadResolutionLineEdit_2->text().toDouble());
        }
        if ((SC.getNewPayloads()+1)->getPayloadDimensions().z()>0.0)
            (SC.getNewPayloads()+1)->CalculateAndSetPayloadVolume
                    ("Rectangular_Prism");
        else
            (SC.getNewPayloads()+1)->CalculateAndSetPayloadVolume
                    ("Cylinder");
    }

    //refresh the GUI
    PayloadWidthLineEdit_2->setText
            (QString::number
             ((SC.getNewPayloads()+1)->getPayloadWidth()));
    PayladLengthLineEdit_2->setText
            (QString::number
             ((SC.getNewPayloads()+1)->getPayloadLength()));
    PayloadHeightLineEdit_2->setText
            (QString::number
             ((SC.getNewPayloads()+1)->getPayloadHeight()));
    PayloadMassLineEdit_2->setText
            (QString::number
             ((SC.getNewPayloads()+1)->getPayloadMass()));
    PayloadPowerEclipseLineEdit_2->setText
            (QString::number
             ((SC.getNewPayloads()+1)->getPayloadPowerInEclipse()));
    PayloadPowerDaylightLineEdit_2->setText
            (QString::number
             ((SC.getNewPayloads()+1)->getPayloadPowerInDaylight()));

	qWarning("TODO: %s	%d",__FILE__,__LINE__);
}

void SEMWizard::on_PayloadWidthLineEdit_2_textEdited(const QString&)
{
    (SC.getNewPayloads()+1)->setPayloadWidth
            (PayloadWidthLineEdit_2->text().toDouble());
	qWarning("TODO: %s	%d",__FILE__,__LINE__);
}

void SEMWizard::on_PayladLengthLineEdit_2_textEdited(const QString&)
{
    (SC.getNewPayloads()+1)->setPayloadLength
            (PayladLengthLineEdit_2->text().toDouble());
	qWarning("TODO: %s	%d",__FILE__,__LINE__);
}

void SEMWizard::on_PayloadHeightLineEdit_2_textEdited(const QString&)
{
    (SC.getNewPayloads()+1)->setPayloadHeight
            (PayloadHeightLineEdit_2->text().toDouble());
	qWarning("TODO: %s	%d",__FILE__,__LINE__);
}

void SEMWizard::on_PayloadMassLineEdit_2_textEdited(const QString&)
{
    (SC.getNewPayloads()+1)->setPayloadMass
            (PayloadMassLineEdit_2->text().toDouble());
	qWarning("TODO: %s	%d",__FILE__,__LINE__);
}

void SEMWizard::on_PayloadDataRateLineEdit_2_textEdited(const QString&)
{
    (SC.getNewPayloads()+1)->setPayloadDataRate
            (PayloadDataRateLineEdit_2->text().toDouble());
	qWarning("TODO: %s	%d",__FILE__,__LINE__);
}

void SEMWizard::on_NumberOfPixelLineEdit_2_textEdited(const QString&)
{
    //fix it according to linear resolution
    (SC.getNewPayloads()+1)->CalculateNewDataRate
            (SC.getSCMissionDetails()->getGroundAverageVelocity(),
             PayloadResolutionLineEdit_2->text().toDouble(),
             NumberOfPixelLineEdit_2->text().toDouble());

    PayloadDataRateLineEdit_2->setText
            (QString::number
             ((SC.getNewPayloads()+1)->getPayloadDataRate()));
	qWarning("TODO: %s	%d",__FILE__,__LINE__);
}

void SEMWizard::on_PayloadPowerEclipseLineEdit_2_textEdited(const QString&)
{
    (SC.getNewPayloads()+1)->setPayloadPowerInEclipse
            (PayloadPowerEclipseLineEdit_2->text().toDouble());
	qWarning("TODO: %s	%d",__FILE__,__LINE__);
}

void SEMWizard::on_PayloadPowerDaylightLineEdit_2_textEdited(const QString&)
{
    (SC.getNewPayloads()+1)->setPayloadPowerInDaylight
            (PayloadPowerDaylightLineEdit_2->text().toDouble());
	qWarning("TODO: %s	%d",__FILE__,__LINE__);
}

void SEMWizard::on_PayloadPowerTimeEclipseLineEdit_2_textEdited(const QString&)
{
    (SC.getNewPayloads()+1)->setPayloadPowerOnPercentageInEclipse
            (PayloadPowerTimeEclipseLineEdit_2->text().toDouble());
	qWarning("TODO: %s	%d",__FILE__,__LINE__);
}

void SEMWizard::on_PayloadPowerTimeDaylightLineEdit_2_textEdited(const QString&)
{
    (SC.getNewPayloads()+1)->setPayloadPowerOnPercentageInDaylight
            (PayloadPowerTimeDaylightLineEdit_2->text().toDouble());
	qWarning("TODO: %s	%d",__FILE__,__LINE__);
}

void SEMWizard::on_PayloadMinTempLineEdit_2_textEdited(const QString&)
{
    (SC.getNewPayloads()+1)->setPayloadMinimumTemperature
            (PayloadMinTempLineEdit_2->text().toDouble());
	qWarning("TODO: %s	%d",__FILE__,__LINE__);
}

void SEMWizard::on_PayloadMaxTempLineEdit_2_textEdited(const QString&)
{
    (SC.getNewPayloads()+1)->setPayloadMaximumTemperature
            (PayloadMaxTempLineEdit_2->text().toDouble());
	qWarning("TODO: %s	%d",__FILE__,__LINE__);
}

void SEMWizard::on_NumberOfPayloadCheckBox_2_toggled(bool)
{
	qWarning("TODO: %s	%d",__FILE__,__LINE__);
}

//************************************************************************
void SEMWizard::on_NewPayloadRadioButton_3_toggled(bool)
{
    if (NewPayloadRadioButton_3->isChecked())
    {
        PayloadSetComboBox_3->setEnabled(0);

        //set the Payload entry lines editable
        PayloadNameLineEdit_3->setEnabled(1);
        PayloadResolutionLineEdit_3->setEnabled(0);
        PayloadWavelengthLineEdit_3->setEnabled(0);
        NumberOfPixelLineEdit_3->setEnabled(0);
    }
	qWarning("TODO: %s	%d",__FILE__,__LINE__);
}

void SEMWizard::on_PayloadSetComboBox_3_activated(const QString&)
{
    if (PayloadSetComboBox_3->currentIndex()!=0)
    {
        (SC.getNewPayloads()+2)->setSelectedPayloadProperties
                (PayloadSetComboBox_3->currentText());

        //if the payload has wavlength properties user can change the values
        if ((SC.getNewPayloads()+2)->getPayloadWavelength()>0.0)
        {
            ResolutionLabel_3->setEnabled(1);
            PayloadResolutionLineEdit_3->setEnabled(1);
            WavelengthLabel_3->setEnabled(1);
            PayloadWavelengthLineEdit_3->setEnabled(1);
            if (PayloadSetComboBox_3->currentText()=="Photometric_Imaging")
            {
                NumberOfPixelLineEdit_3->setEnabled(1);
                NumberOfPixelsLabel_3->setEnabled(1);
            }
            else
            {
                NumberOfPixelLineEdit_3->setEnabled(0);
                NumberOfPixelsLabel_3->setEnabled(0);
            }
        }
        else
        {
            ResolutionLabel_3->setEnabled(0);
            PayloadResolutionLineEdit_3->setEnabled(0);
            WavelengthLabel_3->setEnabled(0);
            PayloadWavelengthLineEdit_3->setEnabled(0);
        }

        //fill the payload properties from database
        PayloadNameLineEdit_3->setText
                ((SC.getNewPayloads()+2)->getPayloadName());

        //set the GUI
        PayloadWidthLineEdit_3->setText
                (QString::number
                 ((SC.getNewPayloads()+2)->getPayloadWidth()));
        PayladLengthLineEdit_3->setText
                (QString::number
                 ((SC.getNewPayloads()+2)->getPayloadLength()));
        PayloadHeightLineEdit_3->setText
                (QString::number
                 ((SC.getNewPayloads()+2)->getPayloadHeight()));
        PayloadMassLineEdit_3->setText
                (QString::number
                 ((SC.getNewPayloads()+2)->getPayloadMass()));
        PayloadPowerEclipseLineEdit_3->setText
                (QString::number
                 ((SC.getNewPayloads()+2)->getPayloadPowerInEclipse()));
        PayloadPowerDaylightLineEdit_3->setText
                (QString::number
                 ((SC.getNewPayloads()+2)->getPayloadPowerInDaylight()));

        PayloadDataRateLineEdit_3->setText
                (QString::number
                 ((SC.getNewPayloads()+2)->getPayloadDataRate()));
        PayloadPowerTimeEclipseLineEdit_3->setText
                (QString::number
                 ((SC.getNewPayloads()+2)->getPayloadPowerOnPercentageInEclipse()));
        PayloadPowerTimeDaylightLineEdit_3->setText
                (QString::number
                 ((SC.getNewPayloads()+2)->getPayloadPowerOnPercentageInDaylight()));
        PayloadMinTempLineEdit_3->setText
                (QString::number
                 ((SC.getNewPayloads()+2)->getPayloadMinimumTemperature()));
        PayloadMaxTempLineEdit_3->setText
                (QString::number
                 ((SC.getNewPayloads()+2)->getPayloadMaximumTemperature()));

	qWarning("TODO: %s	%d",__FILE__,__LINE__);
    }
}

void SEMWizard::on_DatabasePayloadRadioButton_3_toggled(bool)
{
    if (DatabasePayloadRadioButton_3->isChecked())
    {
        PayloadSetComboBox_3->setEnabled(1);
        PayloadNameLineEdit_3->setEnabled(0);
    }
	qWarning("TODO: %s	%d",__FILE__,__LINE__);
}

void SEMWizard::on_PayloadNameLineEdit_3_textEdited(const QString&)
{
    (SC.getNewPayloads()+2)->setPayloadName
            (PayloadNameLineEdit_3->text());
	qWarning("TODO: %s	%d",__FILE__,__LINE__);
}

void SEMWizard::on_PayloadResolutionLineEdit_3_textEdited(const QString&)
{
    //in case, at least one of them ýs zero that means user wants
    //to reset the values
    (SC.getNewPayloads()+2)->setSelectedPayloadProperties
            (PayloadSetComboBox_3->currentText());

    if ((PayloadWavelengthLineEdit_3->text().toDouble()>0.0)
        &&(PayloadResolutionLineEdit_3->text().toDouble()>0.0))
    {
        if (PayloadSetComboBox_3->currentText()=="Photometric_Imaging")
        {
            //for planet observation linear resolution
            (SC.getNewPayloads()+2)->CalculateNewDimensions
                    (PayloadWavelengthLineEdit_3->text().toDouble(),
                     PayloadResolutionLineEdit_3->text().toDouble(),
                     SC.getSCMissionDetails()->getSCMeanAltitude());
        }
        else
        {
            // for star observation angular resolution
            (SC.getNewPayloads()+2)->CalculateNewDimensions
                    (PayloadWavelengthLineEdit_3->text().toDouble(),
                     PayloadResolutionLineEdit_3->text().toDouble());
        }

        if ((SC.getNewPayloads()+2)->getPayloadDimensions().z()>0.0)
            (SC.getNewPayloads()+2)->CalculateAndSetPayloadVolume
                    ("Rectangular_Prism");
        else
            (SC.getNewPayloads()+2)->CalculateAndSetPayloadVolume
                    ("Cylinder");
    }

    //refresh the GUI
    PayloadWidthLineEdit_3->setText
            (QString::number
             ((SC.getNewPayloads()+2)->getPayloadWidth()));
    PayladLengthLineEdit_3->setText
            (QString::number
             ((SC.getNewPayloads()+2)->getPayloadLength()));
    PayloadHeightLineEdit_3->setText
            (QString::number
             ((SC.getNewPayloads()+2)->getPayloadHeight()));
    PayloadMassLineEdit_3->setText
            (QString::number
             ((SC.getNewPayloads()+2)->getPayloadMass()));
    PayloadPowerEclipseLineEdit_3->setText
            (QString::number
             ((SC.getNewPayloads()+2)->getPayloadPowerInEclipse()));
    PayloadPowerDaylightLineEdit_3->setText
            (QString::number
             ((SC.getNewPayloads()+2)->getPayloadPowerInDaylight()));

	qWarning("TODO: %s	%d",__FILE__,__LINE__);
}

void SEMWizard::on_PayloadWavelengthLineEdit_3_textEdited(const QString&)
{
    //in case, at least one of them ýs zero that means user wants
    //to reset the values
    (SC.getNewPayloads()+2)->setSelectedPayloadProperties
            (PayloadSetComboBox_3->currentText());

    if ((PayloadWavelengthLineEdit_3->text().toDouble()>0.0)
        &&(PayloadResolutionLineEdit_3->text().toDouble()>0.0))
    {// for star observation assume no linear resolution
        (SC.getNewPayloads()+2)->CalculateNewDimensions
                (PayloadWavelengthLineEdit_3->text().toDouble(),
                 PayloadResolutionLineEdit_3->text().toDouble());
        if ((SC.getNewPayloads()+2)->getPayloadDimensions().z()>0.0)
            (SC.getNewPayloads()+2)->CalculateAndSetPayloadVolume("Rectangular_Prism");
        else
            (SC.getNewPayloads()+2)->CalculateAndSetPayloadVolume("Cylinder");
    }

    //refresh the GUI
    PayloadWidthLineEdit_3->setText
            (QString::number
             ((SC.getNewPayloads()+2)->getPayloadWidth()));
    PayladLengthLineEdit_3->setText
            (QString::number
             ((SC.getNewPayloads()+2)->getPayloadLength()));
    PayloadHeightLineEdit_3->setText
            (QString::number
             ((SC.getNewPayloads()+2)->getPayloadHeight()));
    PayloadMassLineEdit_3->setText
            (QString::number
             ((SC.getNewPayloads()+2)->getPayloadMass()));
    PayloadPowerEclipseLineEdit_3->setText
            (QString::number
             ((SC.getNewPayloads()+2)->getPayloadPowerInEclipse()));
    PayloadPowerDaylightLineEdit_3->setText
            (QString::number
             ((SC.getNewPayloads()+2)->getPayloadPowerInDaylight()));

	qWarning("TODO: %s	%d",__FILE__,__LINE__);
}

void SEMWizard::on_PayloadWidthLineEdit_3_textEdited(const QString&)
{
    (SC.getNewPayloads()+2)->setPayloadWidth
            (PayloadWidthLineEdit_3->text().toDouble());
	qWarning("TODO: %s	%d",__FILE__,__LINE__);
}

void SEMWizard::on_PayladLengthLineEdit_3_textEdited(const QString&)
{
    (SC.getNewPayloads()+2)->setPayloadLength
            (PayladLengthLineEdit_3->text().toDouble());
	qWarning("TODO: %s	%d",__FILE__,__LINE__);
}

void SEMWizard::on_PayloadHeightLineEdit_3_textEdited(const QString&)
{
    (SC.getNewPayloads()+2)->setPayloadHeight
            (PayloadHeightLineEdit_3->text().toDouble());
	qWarning("TODO: %s	%d",__FILE__,__LINE__);
}

void SEMWizard::on_PayloadMassLineEdit_3_textEdited(const QString&)
{
    (SC.getNewPayloads()+2)->setPayloadMass
            (PayloadMassLineEdit_3->text().toDouble());
	qWarning("TODO: %s	%d",__FILE__,__LINE__);
}

void SEMWizard::on_PayloadDataRateLineEdit_3_textEdited(const QString&)
{
    (SC.getNewPayloads()+2)->setPayloadDataRate
            (PayloadDataRateLineEdit_3->text().toDouble());
	qWarning("TODO: %s	%d",__FILE__,__LINE__);
}

void SEMWizard::on_NumberOfPixelLineEdit_3_textEdited(const QString&)
{
    //fix it according to linear resolution
    (SC.getNewPayloads()+2)->CalculateNewDataRate
            (SC.getSCMissionDetails()->getGroundAverageVelocity(),
             PayloadResolutionLineEdit_3->text().toDouble(),
             NumberOfPixelLineEdit_3->text().toDouble());

    PayloadDataRateLineEdit_3->setText
            (QString::number
             ((SC.getNewPayloads()+2)->getPayloadDataRate()));
	qWarning("TODO: %s	%d",__FILE__,__LINE__);
}

void SEMWizard::on_PayloadPowerEclipseLineEdit_3_textEdited(const QString&)
{
    (SC.getNewPayloads()+2)->setPayloadPowerInEclipse
            (PayloadPowerEclipseLineEdit_3->text().toDouble());
	qWarning("TODO: %s	%d",__FILE__,__LINE__);
}

void SEMWizard::on_PayloadPowerDaylightLineEdit_3_textEdited(const QString&)
{
    (SC.getNewPayloads()+2)->setPayloadPowerInDaylight
            (PayloadPowerDaylightLineEdit_3->text().toDouble());
	qWarning("TODO: %s	%d",__FILE__,__LINE__);
}

void SEMWizard::on_PayloadPowerTimeEclipseLineEdit_3_textEdited(const QString&)
{
    (SC.getNewPayloads()+2)->setPayloadPowerOnPercentageInEclipse
            (PayloadPowerTimeEclipseLineEdit_3->text().toDouble());
	qWarning("TODO: %s	%d",__FILE__,__LINE__);
}

void SEMWizard::on_PayloadPowerTimeDaylightLineEdit_3_textEdited(const QString&)
{
    (SC.getNewPayloads()+2)->setPayloadPowerOnPercentageInDaylight
            (PayloadPowerTimeDaylightLineEdit_3->text().toDouble());
	qWarning("TODO: %s	%d",__FILE__,__LINE__);
}

void SEMWizard::on_PayloadMinTempLineEdit_3_textEdited(const QString&)
{    
    (SC.getNewPayloads()+2)->setPayloadMinimumTemperature
            (PayloadMinTempLineEdit_3->text().toDouble());
	qWarning("TODO: %s	%d",__FILE__,__LINE__);
}

void SEMWizard::on_PayloadMaxTempLineEdit_3_textEdited(const QString&)
{
    (SC.getNewPayloads()+2)->setPayloadMaximumTemperature
            (PayloadMaxTempLineEdit_2->text().toDouble());
	qWarning("TODO: %s	%d",__FILE__,__LINE__);
}

void SEMWizard::on_NumberOfPayloadCheckBox_3_toggled(bool)
{//--------------------------------------------------------
	qWarning("TODO: %s	%d",__FILE__,__LINE__);
}
//**************************************************************
void SEMWizard::on_NewPayloadRadioButton_4_toggled(bool)
{
    if (NewPayloadRadioButton_4->isChecked())
    {
        PayloadSetComboBox_4->setEnabled(0);

        //set the Payload entry lines editable
        PayloadNameLineEdit_4->setEnabled(1);
        PayloadResolutionLineEdit_4->setEnabled(0);
        PayloadWavelengthLineEdit_4->setEnabled(0);
        NumberOfPixelLineEdit_4->setEnabled(0);
    }
	qWarning("TODO: %s	%d",__FILE__,__LINE__);
}

void SEMWizard::on_PayloadSetComboBox_4_activated(const QString&)
{
    if (PayloadSetComboBox_4->currentIndex()!=0)
    {
        (SC.getNewPayloads()+3)->setSelectedPayloadProperties
                (PayloadSetComboBox_4->currentText());

        //if the payload has wavlength properties user can change the values
        if ((SC.getNewPayloads()+3)->getPayloadWavelength()>0.0)
        {
            ResolutionLabel_4->setEnabled(1);
            PayloadResolutionLineEdit_4->setEnabled(1);
            WavelengthLabel_4->setEnabled(1);
            PayloadWavelengthLineEdit_4->setEnabled(1);
            if (PayloadSetComboBox_4->currentText()=="Photometric_Imaging")
            {
                NumberOfPixelLineEdit_4->setEnabled(1);
                NumberOfPixelsLabel_4->setEnabled(1);
            }
            else
            {
                NumberOfPixelLineEdit_4->setEnabled(0);
                NumberOfPixelsLabel_4->setEnabled(0);
            }
        }
        else
        {
            ResolutionLabel_4->setEnabled(0);
            PayloadResolutionLineEdit_4->setEnabled(0);
            WavelengthLabel_4->setEnabled(0);
            PayloadWavelengthLineEdit_4->setEnabled(0);
        }
        //fill the payload properties from database
        PayloadNameLineEdit_4->setText
                ((SC.getNewPayloads()+3)->getPayloadName());

        //set the GUI
        PayloadWidthLineEdit_4->setText
                (QString::number
                 ((SC.getNewPayloads()+3)->getPayloadWidth()));
        PayladLengthLineEdit_4->setText
                (QString::number
                 ((SC.getNewPayloads()+3)->getPayloadLength()));
        PayloadHeightLineEdit_4->setText
                (QString::number
                 ((SC.getNewPayloads()+3)->getPayloadHeight()));
        PayloadMassLineEdit_4->setText
                (QString::number
                 ((SC.getNewPayloads()+3)->getPayloadMass()));
        PayloadPowerEclipseLineEdit_4->setText
                (QString::number
                 ((SC.getNewPayloads()+3)->getPayloadPowerInEclipse()));
        PayloadPowerDaylightLineEdit_4->setText
                (QString::number
                 ((SC.getNewPayloads()+3)->getPayloadPowerInDaylight()));

        PayloadDataRateLineEdit_4->setText
                (QString::number
                 ((SC.getNewPayloads()+3)->getPayloadDataRate()));
        PayloadPowerTimeEclipseLineEdit_4->setText
                (QString::number
                 ((SC.getNewPayloads()+3)->getPayloadPowerOnPercentageInEclipse()));
        PayloadPowerTimeDaylightLineEdit_4->setText
                (QString::number
                 ((SC.getNewPayloads()+3)->getPayloadPowerOnPercentageInDaylight()));
        PayloadMinTempLineEdit_4->setText
                (QString::number
                 ((SC.getNewPayloads()+3)->getPayloadMinimumTemperature()));
        PayloadMaxTempLineEdit_4->setText
                (QString::number
                 ((SC.getNewPayloads()+4)->getPayloadMaximumTemperature()));
	qWarning("TODO: %s	%d",__FILE__,__LINE__);
    }
}

void SEMWizard::on_DatabasePayloadRadioButton_4_toggled(bool)
{
    if (DatabasePayloadRadioButton_4->isChecked())
    {
        PayloadSetComboBox_4->setEnabled(1);
        PayloadNameLineEdit_4->setEnabled(0);
    }
	qWarning("TODO: %s	%d",__FILE__,__LINE__);
}

void SEMWizard::on_PayloadNameLineEdit_4_textEdited(const QString&)
{
    (SC.getNewPayloads()+3)->setPayloadName
            (PayloadNameLineEdit_4->text());
	qWarning("TODO: %s	%d",__FILE__,__LINE__);
}

void SEMWizard::on_PayloadResolutionLineEdit_4_textEdited(const QString&)
{
    //in case, at least one of them ýs zero that means user wants
    //to reset the values
    (SC.getNewPayloads()+3)->setSelectedPayloadProperties
            (PayloadSetComboBox_4->currentText());

    if ((PayloadWavelengthLineEdit_4->text().toDouble()>0.0)
        &&(PayloadResolutionLineEdit_4->text().toDouble()>0.0))
    {
        if (PayloadSetComboBox_4->currentText()=="Photometric_Imaging")
        {
            //for planet observation linear resolution
            (SC.getNewPayloads()+3)->CalculateNewDimensions
                    (PayloadWavelengthLineEdit_4->text().toDouble(),
                     PayloadResolutionLineEdit_4->text().toDouble(),
                     SC.getSCMissionDetails()->getSCMeanAltitude());
        }
        else
        {
            // for star observation angular resolution
            (SC.getNewPayloads()+3)->CalculateNewDimensions
                    (PayloadWavelengthLineEdit_4->text().toDouble(),
                     PayloadResolutionLineEdit_4->text().toDouble());
        }

        if ((SC.getNewPayloads()+3)->getPayloadDimensions().z()>0.0)
            (SC.getNewPayloads()+3)->CalculateAndSetPayloadVolume
                    ("Rectangular_Prism");
        else
            (SC.getNewPayloads()+3)->CalculateAndSetPayloadVolume
                    ("Cylinder");
    }

    //refresh the GUI
    PayloadWidthLineEdit_4->setText
            (QString::number
             ((SC.getNewPayloads()+3)->getPayloadWidth()));
    PayladLengthLineEdit_4->setText
            (QString::number
             ((SC.getNewPayloads()+3)->getPayloadLength()));
    PayloadHeightLineEdit_4->setText
            (QString::number
             ((SC.getNewPayloads()+3)->getPayloadHeight()));
    PayloadMassLineEdit_4->setText
            (QString::number
             ((SC.getNewPayloads()+3)->getPayloadMass()));
    PayloadPowerEclipseLineEdit_4->setText
            (QString::number
             ((SC.getNewPayloads()+3)->getPayloadPowerInEclipse()));
    PayloadPowerDaylightLineEdit_4->setText
            (QString::number
             ((SC.getNewPayloads()+3)->getPayloadPowerInDaylight()));

	qWarning("TODO: %s	%d",__FILE__,__LINE__);
}

void SEMWizard::on_PayloadWavelengthLineEdit_4_textEdited(const QString&)
{
    //in case, at least one of them ýs zero that means user wants
    //to reset the values
    (SC.getNewPayloads()+3)->setSelectedPayloadProperties
            (PayloadSetComboBox_4->currentText());

    if ((PayloadWavelengthLineEdit_4->text().toDouble()>0.0)
        &&(PayloadResolutionLineEdit_4->text().toDouble()>0.0))
        {// for star observation assume no linear resolution
        (SC.getNewPayloads()+3)->CalculateNewDimensions
                (PayloadWavelengthLineEdit_4->text().toDouble(),
                 PayloadResolutionLineEdit_4->text().toDouble());
        if ((SC.getNewPayloads()+3)->getPayloadDimensions().z()>0.0)
            (SC.getNewPayloads()+3)->CalculateAndSetPayloadVolume
                    ("Rectangular_Prism");
        else
            (SC.getNewPayloads()+3)->CalculateAndSetPayloadVolume
                    ("Cylinder");
    }

    //refresh the GUI
    PayloadWidthLineEdit_4->setText
            (QString::number
             ((SC.getNewPayloads()+3)->getPayloadWidth()));
    PayladLengthLineEdit_4->setText
            (QString::number
             ((SC.getNewPayloads()+3)->getPayloadLength()));
    PayloadHeightLineEdit_4->setText
            (QString::number
             ((SC.getNewPayloads()+3)->getPayloadHeight()));
    PayloadMassLineEdit_4->setText
            (QString::number
             ((SC.getNewPayloads()+3)->getPayloadMass()));
    PayloadPowerEclipseLineEdit_4->setText
            (QString::number
             ((SC.getNewPayloads()+3)->getPayloadPowerInEclipse()));
    PayloadPowerDaylightLineEdit_4->setText
            (QString::number
             ((SC.getNewPayloads()+3)->getPayloadPowerInDaylight()));
    qWarning("TODO: %s	%d",__FILE__,__LINE__);
}

void SEMWizard::on_PayloadWidthLineEdit_4_textEdited(const QString&)
{
    (SC.getNewPayloads()+3)->setPayloadWidth
            (PayloadWidthLineEdit_4->text().toDouble());
	qWarning("TODO: %s	%d",__FILE__,__LINE__);
}

void SEMWizard::on_PayladLengthLineEdit_4_textEdited(const QString&)
{
    (SC.getNewPayloads()+3)->setPayloadLength
            (PayladLengthLineEdit_4->text().toDouble());
	qWarning("TODO: %s	%d",__FILE__,__LINE__);
}

void SEMWizard::on_PayloadHeightLineEdit_4_textEdited(const QString&)
{
    (SC.getNewPayloads()+3)->setPayloadHeight
            (PayloadHeightLineEdit_4->text().toDouble());
	qWarning("TODO: %s	%d",__FILE__,__LINE__);
}

void SEMWizard::on_PayloadMassLineEdit_4_textEdited(const QString&)
{
    (SC.getNewPayloads()+3)->setPayloadMass
            (PayloadMassLineEdit_4->text().toDouble());
	qWarning("TODO: %s	%d",__FILE__,__LINE__);
}

void SEMWizard::on_PayloadDataRateLineEdit_4_textEdited(const QString&)
{
    (SC.getNewPayloads()+3)->setPayloadDataRate
            (PayloadDataRateLineEdit_4->text().toDouble());
	qWarning("TODO: %s	%d",__FILE__,__LINE__);
}

void SEMWizard::on_NumberOfPixelLineEdit_4_textEdited(const QString&)
{
    //fix it according to linear resolution
    (SC.getNewPayloads()+3)->CalculateNewDataRate
            (SC.getSCMissionDetails()->getGroundAverageVelocity(),
             PayloadResolutionLineEdit_4->text().toDouble(),
             NumberOfPixelLineEdit_4->text().toDouble());

    PayloadDataRateLineEdit_4->setText
            (QString::number
             ((SC.getNewPayloads()+3)->getPayloadDataRate()));
	qWarning("TODO: %s	%d",__FILE__,__LINE__);
}

void SEMWizard::on_PayloadPowerEclipseLineEdit_4_textEdited(const QString&)
{
    (SC.getNewPayloads()+3)->setPayloadPowerInEclipse
            (PayloadPowerEclipseLineEdit_4->text().toDouble());
	qWarning("TODO: %s	%d",__FILE__,__LINE__);
}

void SEMWizard::on_PayloadPowerDaylightLineEdit_4_textEdited(const QString&)
{
    (SC.getNewPayloads()+3)->setPayloadPowerInDaylight
            (PayloadPowerDaylightLineEdit_4->text().toDouble());
	qWarning("TODO: %s	%d",__FILE__,__LINE__);
}

void SEMWizard::on_PayloadPowerTimeEclipseLineEdit_4_textEdited(const QString&)
{
    (SC.getNewPayloads()+3)->setPayloadPowerOnPercentageInEclipse
            (PayloadPowerTimeEclipseLineEdit_4->text().toDouble());
	qWarning("TODO: %s	%d",__FILE__,__LINE__);
}

void SEMWizard::on_PayloadPowerTimeDaylightLineEdit_4_textEdited(const QString&)
{
    (SC.getNewPayloads()+3)->setPayloadPowerOnPercentageInDaylight
            (PayloadPowerTimeDaylightLineEdit_4->text().toDouble());
	qWarning("TODO: %s	%d",__FILE__,__LINE__);
}

void SEMWizard::on_PayloadMinTempLineEdit_4_textEdited(const QString&)
{
    (SC.getNewPayloads()+3)->setPayloadMinimumTemperature
            (PayloadMinTempLineEdit_4->text().toDouble());
	qWarning("TODO: %s	%d",__FILE__,__LINE__);
}

void SEMWizard::on_PayloadMaxTempLineEdit_4_textEdited(const QString&)
{
    (SC.getNewPayloads()+3)->setPayloadMaximumTemperature
            (PayloadMaxTempLineEdit_4->text().toDouble());
	qWarning("TODO: %s	%d",__FILE__,__LINE__);
}

void SEMWizard::on_SolarCellTypeComboBox_activated(const QString&)
{
    SC.getNewSCPower()->SolarArrays.setSolarCell
            (SolarCellTypeComboBox->currentText());

//    SC.getNewSCPower()->SolarArrays.CalculateSolarCellBOLPower();
    qDebug()<<"BOL"<<SC.getNewSCPower()->SolarArrays.getSolarCellBOLPower();
	qWarning("TODO: %s	%d",__FILE__,__LINE__);
}

void SEMWizard::on_BatteryTypeComboBox_activated(const QString&)
{
    SC.getNewSCPower()->Battery.setBatteryProperties
            (BatteryTypeComboBox->currentText());
	qWarning("TODO: %s	%d",__FILE__,__LINE__);
}

void SEMWizard::on_StructuralMaterialComboBox_activated(const QString&)
{
    SC.getNewSCStructure()->setMaterialProperties
            (StructuralMaterialComboBox->currentText());
	qWarning("TODO: %s	%d",__FILE__,__LINE__);
}

void SEMWizard::on_SCShapeComboBox_activated(const QString&)
{
    SC.getNewSCStructure()->setSCShape
            (SCShapeComboBox->currentText());
	qWarning("TODO: %s	%d",__FILE__,__LINE__);
}

void SEMWizard::on_ColdFaceEmmissivityLineEdit_textEdited(const QString&)
{
    double temp =
            SC.getNewSCThermal()->getColdFaceCoatingProperties().Absorptivity;
    SC.getNewSCThermal()->setColdFaceCoatingProperties
            ("User_Defined",
             ColdFaceEmmissivityLineEdit->text().toDouble(),
             temp);

	qWarning("TODO: %s	%d",__FILE__,__LINE__);
}

void SEMWizard::on_ColdFaceAbsorptivityLineEdit_textEdited(const QString&)
{
    double temp =
            SC.getNewSCThermal()->getColdFaceCoatingProperties().Emmissivity;
    SC.getNewSCThermal()->setColdFaceCoatingProperties
            ("User_Defined",
             temp,
             ColdFaceAbsorptivityLineEdit->text().toDouble());
	qWarning("TODO: %s	%d",__FILE__,__LINE__);
}

void SEMWizard::on_HotFaceEmmissivityLineEdit_textEdited(const QString&)
{
    double temp =
            SC.getNewSCThermal()->getHotFaceCoatingProperties().Absorptivity;
    SC.getNewSCThermal()->setHotFaceCoatingProperties
            ("User_Defined",
             HotFaceEmmissivityLineEdit->text().toDouble(),
             temp);
	qWarning("TODO: %s	%d",__FILE__,__LINE__);
}

void SEMWizard::on_HotFaceAbsorptivityLineEdit_textEdited(const QString&)
{
    double temp =
            SC.getNewSCThermal()->getHotFaceCoatingProperties().Emmissivity;
    SC.getNewSCThermal()->setHotFaceCoatingProperties
            ("User_Defined",
             temp,
             HotFaceAbsorptivityLineEdit->text().toDouble());
	qWarning("TODO: %s	%d",__FILE__,__LINE__);
}

void SEMWizard::on_ColdFaceCoatingComboBox_activated(const QString&)
{
    ColdFaceEmmissivityLineEdit->setEnabled(0);
    ColdFaceAbsorptivityLineEdit->setEnabled(0);

    if (ColdFaceCoatingComboBox->currentIndex()!=0)
    {
        if (ColdFaceCoatingComboBox->currentText() == "No_Coating")
        {
            SC.getNewSCThermal()->setColdFaceCoatingProperties
                    ("No_Coating",
                     SC.getNewSCStructure()->getSCMaterial().Emmissivity,
                     SC.getNewSCStructure()->getSCMaterial().Absorptivity);
        }
        else
        {
            if (ColdFaceCoatingComboBox->currentText() == "User_Defined")
            {
                ColdFaceEmmissivityLineEdit->setEnabled(1);
                ColdFaceAbsorptivityLineEdit->setEnabled(1);
                ColdFaceEmmissivityLineEdit->setText("0.0");
                ColdFaceAbsorptivityLineEdit->setText("0.0");
            }
            else
            {
                SC.getNewSCThermal()->setColdFaceCoatingProperties
                        (ColdFaceCoatingComboBox->currentText());
            }
        }
        ColdFaceEmmissivityLineEdit->setText
                (QString::number
                 (SC.getNewSCThermal()->getColdFaceCoatingProperties().Emmissivity));
        ColdFaceAbsorptivityLineEdit->setText
                (QString::number
                 (SC.getNewSCThermal()->getColdFaceCoatingProperties().Absorptivity));
    }

	qWarning("TODO: %s	%d",__FILE__,__LINE__);
}

void SEMWizard::on_HotFaceCoatingComboBox_activated(const QString&)
{
    HotFaceEmmissivityLineEdit->setEnabled(0);
    HotFaceAbsorptivityLineEdit->setEnabled(0);

    if (HotFaceCoatingComboBox->currentIndex()!=0)
    {
        if (HotFaceCoatingComboBox->currentText() == "No_Coating")
        {
            SC.getNewSCThermal()->setHotFaceCoatingProperties
                    ("No_Coating",
                     SC.getNewSCStructure()->getSCMaterial().Emmissivity,
                     SC.getNewSCStructure()->getSCMaterial().Absorptivity);
        }
        else
        {
            if (HotFaceCoatingComboBox->currentText() == "User_Defined")
            {
                HotFaceEmmissivityLineEdit->setEnabled(1);
                HotFaceAbsorptivityLineEdit->setEnabled(1);
                HotFaceEmmissivityLineEdit->setText("0.0");
                HotFaceAbsorptivityLineEdit->setText("0.0");
            }
            else
            {
                SC.getNewSCThermal()->setHotFaceCoatingProperties
                        (HotFaceCoatingComboBox->currentText());
            }
        }
        HotFaceEmmissivityLineEdit->setText
                (QString::number
                 (SC.getNewSCThermal()->getHotFaceCoatingProperties().Emmissivity));
        HotFaceAbsorptivityLineEdit->setText
                (QString::number
                 (SC.getNewSCThermal()->getHotFaceCoatingProperties().Absorptivity));
    }
	qWarning("TODO: %s	%d",__FILE__,__LINE__);
}

void SEMWizard::on_AntennaEfficiencyLineEdit_textEdited(const QString&)
{
    SC.getNewSCCommunication()->setAntennaEfficiency
            (AntennaEfficiencyLineEdit->text().toDouble());
	qWarning("TODO: %s	%d",__FILE__,__LINE__);
}

void SEMWizard::on_AntennaGainLineEdit_textEdited(const QString&)
{
    SC.getNewSCCommunication()->setAntennaGain
            (AntennaGainLineEdit->text().toDouble());
	qWarning("TODO: %s	%d",__FILE__,__LINE__);
}

void SEMWizard::on_TXFrequencyLineEdit_textEdited(const QString&)
{
    SC.getNewSCCommunication()->setAntennaFrequency
            (TXFrequencyLineEdit->text().toDouble());
	qWarning("TODO: %s	%d",__FILE__,__LINE__);
}

void SEMWizard::on_TXPowerLineEdit_textEdited(const QString&)
{
    SC.getNewSCCommunication()->setTTCSubsystemPower
            (TXPowerLineEdit->text().toDouble());
	qWarning("TODO: %s	%d",__FILE__,__LINE__);
}

void SEMWizard::on_CommLinDurationPercentageLineEdit_textEdited(const QString&)
{
    SC.getNewSCCommunication()->setPercentageOfContactTimePerOrbit
            (CommLinDurationPercentageLineEdit->text().toDouble());
	qWarning("TODO: %s	%d",__FILE__,__LINE__);
}

void SEMWizard::on_NumberOfNoLinkOrbitLineEdit_textEdited(const QString&)
{
    SC.getNewSCDataHandling()->setNumberOfOrbitWithNolink
            (NumberOfNoLinkOrbitLineEdit->text().toInt());
	qWarning("TODO: %s	%d",__FILE__,__LINE__);
}

void SEMWizard::on_CodingTypeComboBox_activated(const QString&)
{
    SC.getNewSCDataHandling()->setCodingPercentage
            (CodingTypeComboBox->currentText());
	qWarning("TODO: %s	%d",__FILE__,__LINE__);
}
