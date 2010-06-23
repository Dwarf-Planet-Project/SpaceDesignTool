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

#include "Help/HelpBrowser.h"

//to propogate after closing the wizard if the start and end time is changed
#include "Loitering/loitering.h"
#include "Scenario/propagationfeedback.h"
#include "Astro-Core/EclipseDuration.h"

#include "QDebug"
#include <QList>
#include <QSharedPointer>

SEMWizard::SEMWizard(ScenarioSC* SCVehicle,QString MissionArc, QWidget * parent, Qt::WindowFlags f)
    : QWizard(parent,f)
{
    setupUi(this);

//    Scenario = scenario;
    connect(this, SIGNAL(helpRequested()), this, SLOT(showHelp()));
    connect(this, SIGNAL(accepted()), this, SLOT(wizardIsCompleted()));

    //extract the scenario information of the SC that is wanted to be
    //analyse by SEM
    Vehicle = SCVehicle;

    const QList<QSharedPointer<ScenarioAbstractTrajectoryType> >& trajectoryList
            = Vehicle->SCMission().data()->TrajectoryPlan().data()->AbstractTrajectory();

    // Initial state is stored in the first trajectory (for now); so,
    // the empty trajectory plan case has to be treated specially.
    if (!trajectoryList.isEmpty())
    {
        //To store the Planet name in title case
        QString tempCentralBody = "";

        // details of the mission arcs when do they start when do they end
        foreach (QSharedPointer<ScenarioAbstractTrajectoryType> trajectory, trajectoryList)
        {
            if (dynamic_cast<ScenarioLoiteringType*>(trajectory.data()))
            {
                //for now SEM is only capable of doing systems engineering of loitering
                ScenarioLoiteringType* loitering
                        = dynamic_cast<ScenarioLoiteringType*>(trajectory.data());
                if (loitering->ElementIdentifier()->Name() == MissionArc)
                {
                    SCLoiteringArc = loitering;

                    //collect the mission details and display them in the wizard

                    //planet that SC is orbiting around
                    //convert the planet name to title case
                    tempCentralBody = SCLoiteringArc->Environment()->CentralBody()->Name().toLower();
                    tempCentralBody[0]=tempCentralBody[0].toUpper();
                    SC.getSCMissionDetails()->setPlanetProperties
                            (tempCentralBody);
                    //setting comboBox from a QString
                    int i;
                    for (i=0;i<PlanetNameComboBox->count();i++)
                    {
                        if (PlanetNameComboBox->itemText(i)
                            == SC.getSCMissionDetails()->getPlanetProperties()->Planet)
                        {
                            PlanetNameComboBox->setCurrentIndex(i);
                            break;
                        }
                    }


                    //mission start and mission end dateTime
                    SC.getSCMissionDetails()->setMissionStartTime
                            (SCLoiteringArc->TimeLine().data()->StartTime());
                    MissionStartTimeDateTimeEdit->setDateTime
                            (SC.getSCMissionDetails()->getMissionStartTime());

                    SC.getSCMissionDetails()->setMissionEndTime
                            (SCLoiteringArc->TimeLine().data()->EndTime());
                    MissionEndTimeDateTimeEdit->setDateTime
                            (SC.getSCMissionDetails()->getMissionEndTime());

                    //see if the mission has keplerian elements
                    if (!SCLoiteringArc->InitialPosition()->Abstract6DOFPosition().isNull())
                    {
                        if (dynamic_cast<ScenarioKeplerianElementsType*>
                            (SCLoiteringArc->InitialPosition()->Abstract6DOFPosition().data()))
                        {
                            //for now SEM is only capable of doing systems engineering for the
                            //missions who has keplerian elements
                            ScenarioKeplerianElementsType* keplerian
                                    = dynamic_cast<ScenarioKeplerianElementsType*>
                                      (SCLoiteringArc->InitialPosition()->Abstract6DOFPosition().data());
                            ArgumentOfPeriapsisLineEdit->setText
                                    (QString::number(keplerian->argumentOfPeriapsis()));
                            EccentricityLineEdit->setText
                                    (QString::number(keplerian->eccentricity()));
                            InclinationLineEdit->setText
                                    (QString::number(keplerian->inclination()));
                            RAANLineEdit->setText
                                    (QString::number(keplerian->RAAN()));
                            TrueAnomalyLineEdit->setText
                                    (QString::number(keplerian->trueAnomaly()));
                            SemimajorAxisLineEdit->setText
                                    (QString::number(keplerian->semiMajorAxis()));
                        }
                    }

                }
                break;
            }
        }
    }
}

SEMWizard::~SEMWizard()
{
}

void SEMWizard::showHelp()
{
    HelpBrowser::showPage("index.html");
}

void SEMWizard::wizardIsCompleted()
{
    //put the values of last WizardPage
    SC.getNewSCDataHandling()->setCodingPercentage
            (CodingTypeComboBox->currentText());
    SC.getNewSCDataHandling()->setNumberOfOrbitWithNolink
            (NumberOfNoLinkOrbitLineEdit->text().toDouble());

    //make calculaions of Data handling
    SC.getNewSCDataHandling()->CalculateAndSetMemorySizeForPayloads();

//    SC.getNewSCDataHandling()->setOBDHSubsystemMass
//            (SC.getNewSCStructure()->getTotalPayloadMass());

    SC.PassOBDHSubsystemOutputParameters();

    // ---------------------------------- VALIDATION AND VERIFICATION START
    qDebug()<<"VALIDATION AND VERIFICATION START";
    qDebug()<<"Payload1 datarate"<<(SC.getNewSCDataHandling()->getPayloadOBDHInfo()+0)->DataRate;
    qDebug()<<"Payload2 datarate"<<(SC.getNewSCDataHandling()->getPayloadOBDHInfo()+1)->DataRate;
    qDebug()<<"MemorySizeForPayloads"<<SC.getNewSCDataHandling()->getMemorySizeForPayloads();
    qDebug()<<"OBDHSubsystemMass"<<SC.getNewSCDataHandling()->getOBDHSubsystemMass();
    qDebug()<<"VALIDATION AND VERIFICATION END";
    // ---------------------------------- VALIDATION AND VERIFICATION END

    //fill the scenario

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

                if (loitering->ElementIdentifier()->Name()== SCLoiteringArc->ElementIdentifier()->Name())
                {
                    //set mission start time - end time - keplerian element - orbitting body
                    SCLoiteringArc->TimeLine()->setStartTime
                            (SC.getSCMissionDetails()->getMissionStartTime());
                    SCLoiteringArc->TimeLine()->setEndTime
                            (SC.getSCMissionDetails()->getMissionEndTime());
                    SCLoiteringArc->Environment()->CentralBody()->setName
                            (SC.getSCMissionDetails()->getPlanetProperties()->Planet.toUpper());
                    //see if the mission has keplerian elements to be filled
                    if (!SCLoiteringArc->InitialPosition()->Abstract6DOFPosition().isNull())
                    {
                        if (dynamic_cast<ScenarioKeplerianElementsType*>
                            (SCLoiteringArc->InitialPosition()->Abstract6DOFPosition().data()))
                        {
                            //for now SEM is only capable of doing systems engineering for the
                            //missions who has keplerian elements
                            ScenarioKeplerianElementsType* keplerian
                                    = dynamic_cast<ScenarioKeplerianElementsType*>
                                      (SCLoiteringArc->InitialPosition()->Abstract6DOFPosition().data());
                            keplerian->setArgumentOfPeriapsis
                                    (SC.getSCMissionDetails()->getArgumentOfPerigee());
                            keplerian->setEccentricity
                                    (SC.getSCMissionDetails()->getEccentricity());
                            keplerian->setInclination
                                    (SC.getSCMissionDetails()->getInclination());
                            keplerian->setRAAN
                                    (SC.getSCMissionDetails()->getRightAscensionNode());
                            keplerian->setSemiMajorAxis
                                    (SC.getSCMissionDetails()->getSemiMajorAxis());
                            keplerian->setTrueAnomaly
                                    (SC.getSCMissionDetails()->getTrueAnomaly());
                        }
                    }

                    // propogate the scenario
                    PropagateLoiteringTrajectory(SCLoiteringArc, sampleTimes, samples, feedback);

                    //******************************************************************** /OZGUN
                    // Eclipse function is called and the "data/EclipseStarLight.stad" is generated
                    EclipseDuration* Eclipse = new EclipseDuration();

                    Eclipse->StarLightTimeFunction(sampleTimes,
                                                   samples,
                                                   STA_SOLAR_SYSTEM->lookup("Earth"),
                                                   STA_SOLAR_SYSTEM->lookup("Sun"));
                    //******************************************************************** OZGUN/
                }
            }
        }
    }


    //---------------------------------------------------------------------------
    //First Clear all the payloads from th list
    Vehicle->SCMission()->PayloadSet()->AbstractPayload().clear();
    //add the payloads to the scenario
    int i;
    int PayloadNumber=4;
    for (i=0;i<PayloadNumber;i++)
    {
        if(!(SC.getNewPayloads()+i)->getPayloadName().isEmpty())
        {
            if ((SC.getNewPayloads()+i)->getPayloadName()=="Transmitter")
            {
                ScenarioTransmitterPayloadType* myPayload = new ScenarioTransmitterPayloadType();
                myPayload->ElementIdentifier()->setName((SC.getNewPayloads()+i)->getPayloadName());
                // fill the parameters of created Payload
                myPayload->Budget()->setMass
                        ((SC.getNewPayloads()+i)->getPayloadMass());
                myPayload->Budget()->Size()->setHeight
                        ((SC.getNewPayloads()+i)->getPayloadHeight());
                myPayload->Budget()->Size()->setLength
                        ((SC.getNewPayloads()+i)->getPayloadLength());
                myPayload->Budget()->Size()->setWidth
                        ((SC.getNewPayloads()+i)->getPayloadWidth());

                myPayload->Budget()->Power()->setPowerConsumptionInDaylight
                        ((SC.getNewPayloads()+i)->getPayloadPowerInDaylight());
                myPayload->Budget()->Power()->setPowerConsumptionInEclipse
                        ((SC.getNewPayloads()+i)->getPayloadPowerInEclipse());
                myPayload->Budget()->Power()->setPowerOnPercentageInDaylight
                        ((SC.getNewPayloads()+i)->getPayloadPowerOnPercentageInDaylight());
                myPayload->Budget()->Power()->setPowerOnPercentageInEclipse
                        ((SC.getNewPayloads()+i)->getPayloadPowerOnPercentageInEclipse());

                myPayload->Budget()->setDataRate
                        ((SC.getNewPayloads()+i)->getPayloadDataRate());

                myPayload->Budget()->setFrequencyBand
                        ((SC.getNewPayloads()+i)->getPayloadWavelength());

                myPayload->Budget()->TemperatureRange()->setMaximumTemperature
                        ((SC.getNewPayloads()+i)->getPayloadMaximumTemperature());
                myPayload->Budget()->TemperatureRange()->setMinimumTemperature
                        ((SC.getNewPayloads()+i)->getPayloadMinimumTemperature());

                //add the payload to the scenario
                Vehicle->SCMission()->PayloadSet()->AbstractPayload().append
                            (QSharedPointer<ScenarioAbstractPayloadType>(myPayload));


                qDebug()<<"PAYLOAD"<<myPayload->ElementIdentifier()->Name();
                qDebug()<<"PAYLOAD Mass"<<myPayload->Budget()->Mass();
            }
            else
            {
                if ((SC.getNewPayloads()+i)->getPayloadName()=="Receiver")
                {
                    ScenarioReceiverPayloadType* myPayload = new ScenarioReceiverPayloadType();
                    myPayload->ElementIdentifier()->setName((SC.getNewPayloads()+i)->getPayloadName());
                    // fill the parameters of created Payload
                    myPayload->Budget()->setMass
                            ((SC.getNewPayloads()+i)->getPayloadMass());
                    myPayload->Budget()->Size()->setHeight
                            ((SC.getNewPayloads()+i)->getPayloadHeight());
                    myPayload->Budget()->Size()->setLength
                            ((SC.getNewPayloads()+i)->getPayloadLength());
                    myPayload->Budget()->Size()->setWidth
                            ((SC.getNewPayloads()+i)->getPayloadWidth());

                    myPayload->Budget()->Power()->setPowerConsumptionInDaylight
                            ((SC.getNewPayloads()+i)->getPayloadPowerInDaylight());
                    myPayload->Budget()->Power()->setPowerConsumptionInEclipse
                            ((SC.getNewPayloads()+i)->getPayloadPowerInEclipse());
                    myPayload->Budget()->Power()->setPowerOnPercentageInDaylight
                            ((SC.getNewPayloads()+i)->getPayloadPowerOnPercentageInDaylight());
                    myPayload->Budget()->Power()->setPowerOnPercentageInEclipse
                            ((SC.getNewPayloads()+i)->getPayloadPowerOnPercentageInEclipse());

                    myPayload->Budget()->setDataRate
                            ((SC.getNewPayloads()+i)->getPayloadDataRate());

                    myPayload->Budget()->setFrequencyBand
                            ((SC.getNewPayloads()+i)->getPayloadWavelength());

                    myPayload->Budget()->TemperatureRange()->setMaximumTemperature
                            ((SC.getNewPayloads()+i)->getPayloadMaximumTemperature());
                    myPayload->Budget()->TemperatureRange()->setMinimumTemperature
                            ((SC.getNewPayloads()+i)->getPayloadMinimumTemperature());

                    //add the payload to the scenario
                    Vehicle->SCMission()->PayloadSet()->AbstractPayload().append
                                (QSharedPointer<ScenarioAbstractPayloadType>(myPayload));


                    qDebug()<<"PAYLOAD"<<myPayload->ElementIdentifier()->Name();
                    qDebug()<<"PAYLOAD Mass"<<myPayload->Budget()->Mass();
                }
                else
                {
                    if (((SC.getNewPayloads()+i)->getPayloadName()=="SAR")||
                        ((SC.getNewPayloads()+i)->getPayloadName()=="Radar"))
                    {
                        ScenarioRadarPayloadType* myPayload = new ScenarioRadarPayloadType();
                        myPayload->ElementIdentifier()->setName((SC.getNewPayloads()+i)->getPayloadName());
                        // fill the parameters of created Payload
                        myPayload->Budget()->setMass
                                ((SC.getNewPayloads()+i)->getPayloadMass());
                        myPayload->Budget()->Size()->setHeight
                                ((SC.getNewPayloads()+i)->getPayloadHeight());
                        myPayload->Budget()->Size()->setLength
                                ((SC.getNewPayloads()+i)->getPayloadLength());
                        myPayload->Budget()->Size()->setWidth
                                ((SC.getNewPayloads()+i)->getPayloadWidth());

                        myPayload->Budget()->Power()->setPowerConsumptionInDaylight
                                ((SC.getNewPayloads()+i)->getPayloadPowerInDaylight());
                        myPayload->Budget()->Power()->setPowerConsumptionInEclipse
                                ((SC.getNewPayloads()+i)->getPayloadPowerInEclipse());
                        myPayload->Budget()->Power()->setPowerOnPercentageInDaylight
                                ((SC.getNewPayloads()+i)->getPayloadPowerOnPercentageInDaylight());
                        myPayload->Budget()->Power()->setPowerOnPercentageInEclipse
                                ((SC.getNewPayloads()+i)->getPayloadPowerOnPercentageInEclipse());

                        myPayload->Budget()->setDataRate
                                ((SC.getNewPayloads()+i)->getPayloadDataRate());

                        myPayload->Budget()->setFrequencyBand
                                ((SC.getNewPayloads()+i)->getPayloadWavelength());

                        myPayload->Budget()->TemperatureRange()->setMaximumTemperature
                                ((SC.getNewPayloads()+i)->getPayloadMaximumTemperature());
                        myPayload->Budget()->TemperatureRange()->setMinimumTemperature
                                ((SC.getNewPayloads()+i)->getPayloadMinimumTemperature());

                        //add the payload to the scenario
                        Vehicle->SCMission()->PayloadSet()->AbstractPayload().append
                                    (QSharedPointer<ScenarioAbstractPayloadType>(myPayload));


                        qDebug()<<"PAYLOAD"<<myPayload->ElementIdentifier()->Name();
                        qDebug()<<"PAYLOAD Mass"<<myPayload->Budget()->Mass();
                    }
                    else
                    {
                        ScenarioOpticalPayloadType* myPayload = new ScenarioOpticalPayloadType();
                        myPayload->ElementIdentifier()->setName((SC.getNewPayloads()+i)->getPayloadName());
                        // fill the parameters of created Payload
                        myPayload->Budget()->setMass
                                ((SC.getNewPayloads()+i)->getPayloadMass());
                        myPayload->Budget()->Size()->setHeight
                                ((SC.getNewPayloads()+i)->getPayloadHeight());
                        myPayload->Budget()->Size()->setLength
                                ((SC.getNewPayloads()+i)->getPayloadLength());
                        myPayload->Budget()->Size()->setWidth
                                ((SC.getNewPayloads()+i)->getPayloadWidth());

                        myPayload->Budget()->Power()->setPowerConsumptionInDaylight
                                ((SC.getNewPayloads()+i)->getPayloadPowerInDaylight());
                        myPayload->Budget()->Power()->setPowerConsumptionInEclipse
                                ((SC.getNewPayloads()+i)->getPayloadPowerInEclipse());
                        myPayload->Budget()->Power()->setPowerOnPercentageInDaylight
                                ((SC.getNewPayloads()+i)->getPayloadPowerOnPercentageInDaylight());
                        myPayload->Budget()->Power()->setPowerOnPercentageInEclipse
                                ((SC.getNewPayloads()+i)->getPayloadPowerOnPercentageInEclipse());

                        myPayload->Budget()->setDataRate
                                ((SC.getNewPayloads()+i)->getPayloadDataRate());

                        myPayload->Budget()->setFrequencyBand
                                ((SC.getNewPayloads()+i)->getPayloadWavelength());

                        myPayload->Budget()->TemperatureRange()->setMaximumTemperature
                                ((SC.getNewPayloads()+i)->getPayloadMaximumTemperature());
                        myPayload->Budget()->TemperatureRange()->setMinimumTemperature
                                ((SC.getNewPayloads()+i)->getPayloadMinimumTemperature());

                        //add the payload to the scenario
                        Vehicle->SCMission()->PayloadSet()->AbstractPayload().append
                                    (QSharedPointer<ScenarioAbstractPayloadType>(myPayload));


                        qDebug()<<"PAYLOAD"<<myPayload->ElementIdentifier()->Name();
                        qDebug()<<"PAYLOAD Mass"<<myPayload->Budget()->Mass();
                    }
                }
            }
        }
    }
    //-----------------------------------------------------------------------------

    //make calculations of communication
    SC.getNewSCCommunication()->CalculateAndSetAntennaDiameter();
    SC.getNewSCCommunication()->CalculateAndSetAntennaVolume();
    SC.getNewSCCommunication()->CalculateAndSetContactTimePerOrbit();

    SC.PassTTCSubsystemOutputParameters();

    //make calclations for power

    //make calculations of power
    SC.getNewSCPower()->CalculateAndSetTotalSCPower();
    SC.getNewSCPower()->CalculateAndSetPowerSubsystemMass();
    SC.getNewSCPower()->SolarArrays.CalculateSolarCellBOLPower();
    SC.getNewSCPower()->SolarArrays.CalculateSolarCellEOLPower();
    SC.getNewSCPower()->SolarArrays.CalculateSABOLPower();

    SC.PassPowerSubsystemOutputParameters();

//    //calculate SAEOLPower needed
//    SC.getNewSCPower()->SolarArrays.CalculateSAEOLPower();
//    SC.getNewSCPower()->SolarArrays.CalculateSolarCellEOLPower();

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

    // calculate the size of SC
    SC.getNewSCStructure()->CalculateAndSetSCVolume();
    SC.getNewSCStructure()->CalculateAndSetSpacecraftDimension();
//
    SC.PassStructureSubsystemOutputParameters();

//    // calculate the size of SC
//    SC.getNewSCStructure()->CalculateAndSetSCVolume();
//    SC.getNewSCStructure()->CalculateAndSetSpacecraftDimension();



    //make calculations of thermal
    SC.getNewSCThermal()->CalculateAndSetSolarFluxHeat();
    SC.getNewSCThermal()->CalculateAndSetAlbedoHeat();
    SC.getNewSCThermal()->CalculateAndSetPlanetIRHeat();
    SC.getNewSCThermal()->CalculateAndSetSCTemperatureRange();
    SC.getNewSCThermal()->CalculateAndSetNeededHeater();
    SC.getNewSCThermal()->CalculateAndSetNeededRadiator();

    //open the GUI
    SemMainGUI  MainGUI(&SC,Vehicle,this);
    MainGUI.exec();
	qWarning("TODO: %s	%d",__FILE__,__LINE__);
}

void SEMWizard::on___qt__passive_wizardbutton1_clicked()
{
    //first check if the payload set check box is set
    //if it is checked go to power subsystem wizard page
    if ((NumberOfPayloadCheckBox->isChecked())
        &&(this->currentPage()==wizardPage3b))
    {
        this->next();
        this->next();
        this->next();
        this->next();
        this->next();
        this->next();
        qDebug()<<"Current Id"<<this->currentId();
    }
    else
    {
        if ((NumberOfPayloadCheckBox_2->isChecked())
            &&(this->currentPage()==wizardPage3c))
        {
            this->next();
            this->next();
            this->next();
            this->next();
        }
        else
        {
            if ((NumberOfPayloadCheckBox_3->isChecked())
                &&(this->currentPage()==wizardPage3d))
            {
                this->next();
                this->next();
            }
        }
    }

    switch (this->currentId())
    {
    case 1:
        // WizardPage1 is executed ----------------------

        //choose the mission type for mass estimations
        //(in terms of percentages)
        SC.getNewSCStructure()->setMassEstimations
                (MissionTypeComboBox->currentText());
        //set mission time information
        SC.getSCMissionDetails()->setMissionStartTime
                (MissionStartTimeDateTimeEdit->dateTime());
        SC.getSCMissionDetails()->setMissionEndTime
                (MissionEndTimeDateTimeEdit->dateTime());

        qDebug()<<"Start"<<SC.getSCMissionDetails()->getMissionStartTime();
        qDebug()<<"End"<<SC.getSCMissionDetails()->getMissionEndTime();
        break;//-----------------------------------------

    case 2:
        // WizardPage2 is executed ----------------------

        //set the orbital parameters and planet details
        if (SemimajorAxisLineEdit->text().toDouble() > 0.0)
        {
            SC.getSCMissionDetails()->setSemiMajorAxis
                    (SemimajorAxisLineEdit->text().toDouble());
            SC.getSCMissionDetails()->setPlanetProperties
                    (PlanetNameComboBox->currentText());
        }
        SC.getSCMissionDetails()->setEccentricity
                (EccentricityLineEdit->text().toDouble());
        SC.getSCMissionDetails()->setInclination
                (InclinationLineEdit->text().toDouble());
        SC.getSCMissionDetails()->setRightAscensionNode
                (RAANLineEdit->text().toDouble());
        SC.getSCMissionDetails()->setArgumentOfPerigee
                (ArgumentOfPeriapsisLineEdit->text().toDouble());
        SC.getSCMissionDetails()->setTrueAnomaly
                (TrueAnomalyLineEdit->text().toDouble());

        //set the mission details
        SC.PassMissionDetailsOutputParameters();
        break;//-----------------------------------------

    case 3:
        // WizardPage3 is executed ----------------------

        // set the values of payload in the right places
        // if it is from the database
        if (DatabasePayloadRadioButton->isChecked())
        {
            (SC.getNewPayloads()+0)->setSelectedPayloadProperties
                    (PayloadSetComboBox->currentText());

            //if the payload has wavlength properties user can
            //change the values
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
            break;
        }
        // if user wants to create his/her own payload

        break;//-----------------------------------------

    case 4:
        break;

    case 5:
        // WizardPage3b is executed ----------------------

        // set the values of payload in the right places
        // if it is from the database
        if (DatabasePayloadRadioButton_2->isChecked())
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
            break;
        }
        // if user wants to create his/her own payload
        break;//-----------------------------------------

    case 6:
        break;

    case 7:
        //WizardPage3c is executed------------------------
        if(DatabasePayloadRadioButton_3->isChecked())
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
            break;
        }
        //if user wants to create his/her own payload
        break;//-----------------------------------------

    case 8:
        break;

    case 9:
        //WizardPage3d is executed------------------------
        if(DatabasePayloadRadioButton_4->isChecked())
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
            break;
        }
        //if user wants to create his/her own payload
        break;//-----------------------------------------

    case 10:
        //collect the payload information from the wizard---
        //------------- PAYLOAD 0 --------------------------
        (SC.getNewPayloads()+0)->setPayloadName
                (PayloadNameLineEdit->text());
        (SC.getNewPayloads()+0)->setPayloadWidth
                (PayloadWidthLineEdit->text().toDouble());
        (SC.getNewPayloads()+0)->setPayloadLength
                (PayladLengthLineEdit->text().toDouble());
        (SC.getNewPayloads()+0)->setPayloadHeight
                (PayloadHeightLineEdit->text().toDouble());
        (SC.getNewPayloads()+0)->setPayloadMass
                (PayloadMassLineEdit->text().toDouble());
        (SC.getNewPayloads()+0)->setPayloadDataRate
                (PayloadDataRateLineEdit->text().toDouble());
        (SC.getNewPayloads()+0)->setPayloadPowerInEclipse
                (PayloadPowerEclipseLineEdit->text().toDouble());
        (SC.getNewPayloads()+0)->setPayloadPowerInDaylight
                (PayloadPowerDaylightLineEdit->text().toDouble());
        (SC.getNewPayloads()+0)->setPayloadPowerOnPercentageInEclipse
                (PayloadPowerTimeEclipseLineEdit->text().toDouble());
        (SC.getNewPayloads()+0)->setPayloadPowerOnPercentageInDaylight
                (PayloadPowerTimeDaylightLineEdit->text().toDouble());
        (SC.getNewPayloads()+0)->setPayloadMinimumTemperature
                (PayloadMinTempLineEdit->text().toDouble());
        (SC.getNewPayloads()+0)->setPayloadMaximumTemperature
                (PayloadMaxTempLineEdit->text().toDouble());
        //------------- PAYLOAD 1 --------------------------
        (SC.getNewPayloads()+1)->setPayloadName
                (PayloadNameLineEdit_2->text());
        (SC.getNewPayloads()+1)->setPayloadWidth
                (PayloadWidthLineEdit_2->text().toDouble());
        (SC.getNewPayloads()+1)->setPayloadLength
                (PayladLengthLineEdit_2->text().toDouble());
        (SC.getNewPayloads()+1)->setPayloadHeight
                (PayloadHeightLineEdit_2->text().toDouble());
        (SC.getNewPayloads()+1)->setPayloadMass
                (PayloadMassLineEdit_2->text().toDouble());
        (SC.getNewPayloads()+1)->setPayloadDataRate
                (PayloadDataRateLineEdit_2->text().toDouble());
        (SC.getNewPayloads()+1)->setPayloadPowerInEclipse
                (PayloadPowerEclipseLineEdit_2->text().toDouble());
        (SC.getNewPayloads()+1)->setPayloadPowerInDaylight
                (PayloadPowerDaylightLineEdit_2->text().toDouble());
        (SC.getNewPayloads()+1)->setPayloadPowerOnPercentageInEclipse
                (PayloadPowerTimeEclipseLineEdit_2->text().toDouble());
        (SC.getNewPayloads()+1)->setPayloadPowerOnPercentageInDaylight
                (PayloadPowerTimeDaylightLineEdit_2->text().toDouble());
        qWarning("TODO: %s	%d",__FILE__,__LINE__);
        (SC.getNewPayloads()+1)->setPayloadMinimumTemperature
                (PayloadMinTempLineEdit_2->text().toDouble());
        (SC.getNewPayloads()+1)->setPayloadMaximumTemperature
                (PayloadMaxTempLineEdit_2->text().toDouble());
        //------------- PAYLOAD 2 --------------------------
        (SC.getNewPayloads()+2)->setPayloadName
                (PayloadNameLineEdit_3->text());
        (SC.getNewPayloads()+2)->setPayloadWidth
                (PayloadWidthLineEdit_3->text().toDouble());
        (SC.getNewPayloads()+2)->setPayloadLength
                (PayladLengthLineEdit_3->text().toDouble());
        (SC.getNewPayloads()+2)->setPayloadHeight
                (PayloadHeightLineEdit_3->text().toDouble());
        (SC.getNewPayloads()+2)->setPayloadMass
                (PayloadMassLineEdit_3->text().toDouble());
        (SC.getNewPayloads()+2)->setPayloadDataRate
                (PayloadDataRateLineEdit_3->text().toDouble());
        (SC.getNewPayloads()+2)->setPayloadPowerInEclipse
                (PayloadPowerEclipseLineEdit_3->text().toDouble());
        (SC.getNewPayloads()+2)->setPayloadPowerInDaylight
                (PayloadPowerDaylightLineEdit_3->text().toDouble());
        (SC.getNewPayloads()+2)->setPayloadPowerOnPercentageInEclipse
                (PayloadPowerTimeEclipseLineEdit_3->text().toDouble());
        (SC.getNewPayloads()+2)->setPayloadPowerOnPercentageInDaylight
                (PayloadPowerTimeDaylightLineEdit_3->text().toDouble());
        (SC.getNewPayloads()+2)->setPayloadMinimumTemperature
                (PayloadMinTempLineEdit_3->text().toDouble());
        (SC.getNewPayloads()+2)->setPayloadMaximumTemperature
                (PayloadMaxTempLineEdit_3->text().toDouble());
        //------------- PAYLOAD 3 --------------------------
        (SC.getNewPayloads()+3)->setPayloadName
                (PayloadNameLineEdit_4->text());
        (SC.getNewPayloads()+3)->setPayloadWidth
                (PayloadWidthLineEdit_4->text().toDouble());
        (SC.getNewPayloads()+3)->setPayloadLength
                (PayladLengthLineEdit_4->text().toDouble());
        (SC.getNewPayloads()+3)->setPayloadHeight
                (PayloadHeightLineEdit_4->text().toDouble());
        (SC.getNewPayloads()+3)->setPayloadMass
                (PayloadMassLineEdit_4->text().toDouble());
        (SC.getNewPayloads()+3)->setPayloadDataRate
                (PayloadDataRateLineEdit_4->text().toDouble());
        (SC.getNewPayloads()+3)->setPayloadPowerInEclipse
                (PayloadPowerEclipseLineEdit_4->text().toDouble());
        (SC.getNewPayloads()+3)->setPayloadPowerInDaylight
                (PayloadPowerDaylightLineEdit_4->text().toDouble());
        (SC.getNewPayloads()+3)->setPayloadPowerOnPercentageInEclipse
                (PayloadPowerTimeEclipseLineEdit_4->text().toDouble());
        (SC.getNewPayloads()+3)->setPayloadPowerOnPercentageInDaylight
                (PayloadPowerTimeDaylightLineEdit_4->text().toDouble());
        (SC.getNewPayloads()+3)->setPayloadMinimumTemperature
                (PayloadMinTempLineEdit_4->text().toDouble());
        (SC.getNewPayloads()+3)->setPayloadMaximumTemperature
                (PayloadMaxTempLineEdit_4->text().toDouble());
        //--------------------------------------------------

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

    case 11:
        //set the solar cell and the battery
        SC.getNewSCPower()->SolarArrays.setSolarCell
                (SolarCellTypeComboBox->currentText());
        SC.getNewSCPower()->Battery.setBatteryProperties
                (BatteryTypeComboBox->currentText());
        break;
    case 12:
        SC.getNewSCStructure()->setMaterialProperties
                (StructuralMaterialComboBox->currentText());
        SC.getNewSCStructure()->setSCShape
                (SCShapeComboBox->currentText());
        break;
    case 13:
        break;
    case 14:
        SC.getNewSCCommunication()->setAntennaEfficiency
                (AntennaEfficiencyLineEdit->text().toDouble());
        SC.getNewSCCommunication()->setAntennaGain
                (AntennaGainLineEdit->text().toDouble());
        SC.getNewSCCommunication()->setAntennaFrequency
                (TXFrequencyLineEdit->text().toDouble());
        SC.getNewSCCommunication()->setAntennaPower
                (TXPowerLineEdit->text().toDouble());
        SC.getNewSCCommunication()->setPercentageOfContactTimePerOrbit
                (CommLinDurationPercentageLineEdit->text().toDouble());
        SC.getNewSCCommunication()->setRXAntennaGain
                (RXAntennaGainLineEdit->text().toDouble());
        SC.getNewSCCommunication()->setRXAntennaPower
                (RXPowerLineEdit->text().toDouble());
        break;
    case 15:
        break;
    }
        qDebug()<<"this->currentId()"<<this->currentId();
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

void SEMWizard::on_DatabasePayloadRadioButton_2_toggled(bool)
{
    if (DatabasePayloadRadioButton_2->isChecked())
    {
        PayloadSetComboBox_2->setEnabled(1);
        PayloadNameLineEdit_2->setEnabled(0);
    }
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

void SEMWizard::on_DatabasePayloadRadioButton_3_toggled(bool)
{
    if (DatabasePayloadRadioButton_3->isChecked())
    {
        PayloadSetComboBox_3->setEnabled(1);
        PayloadNameLineEdit_3->setEnabled(0);
    }
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

void SEMWizard::on_DatabasePayloadRadioButton_4_toggled(bool)
{
    if (DatabasePayloadRadioButton_4->isChecked())
    {
        PayloadSetComboBox_4->setEnabled(1);
        PayloadNameLineEdit_4->setEnabled(0);
    }
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

void SEMWizard::on_SolarCellTypeComboBox_activated(const QString&)
{
//    SC.getNewSCPower()->SolarArrays.setSolarCell
//            (SolarCellTypeComboBox->currentText());
//
////    SC.getNewSCPower()->SolarArrays.CalculateSolarCellBOLPower();
//    qDebug()<<"BOL"<<SC.getNewSCPower()->SolarArrays.getSolarCellBOLPower();
	qWarning("TODO: %s	%d",__FILE__,__LINE__);
}

void SEMWizard::on_BatteryTypeComboBox_activated(const QString&)
{
//    SC.getNewSCPower()->Battery.setBatteryProperties
//            (BatteryTypeComboBox->currentText());
	qWarning("TODO: %s	%d",__FILE__,__LINE__);
}

void SEMWizard::on_StructuralMaterialComboBox_activated(const QString&)
{
//    SC.getNewSCStructure()->setMaterialProperties
//            (StructuralMaterialComboBox->currentText());
	qWarning("TODO: %s	%d",__FILE__,__LINE__);
}

void SEMWizard::on_SCShapeComboBox_activated(const QString&)
{
//    SC.getNewSCStructure()->setSCShape
//            (SCShapeComboBox->currentText());
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
//    SC.getNewSCCommunication()->setAntennaEfficiency
//            (AntennaEfficiencyLineEdit->text().toDouble());
	qWarning("TODO: %s	%d",__FILE__,__LINE__);
}

void SEMWizard::on_AntennaGainLineEdit_textEdited(const QString&)
{
//    SC.getNewSCCommunication()->setAntennaGain
//            (AntennaGainLineEdit->text().toDouble());
	qWarning("TODO: %s	%d",__FILE__,__LINE__);
}

void SEMWizard::on_TXFrequencyLineEdit_textEdited(const QString&)
{
//    SC.getNewSCCommunication()->setAntennaFrequency
//            (TXFrequencyLineEdit->text().toDouble());
	qWarning("TODO: %s	%d",__FILE__,__LINE__);
}

void SEMWizard::on_TXPowerLineEdit_textEdited(const QString&)
{
//    SC.getNewSCCommunication()->setTTCSubsystemPower
//            (TXPowerLineEdit->text().toDouble());
	qWarning("TODO: %s	%d",__FILE__,__LINE__);
}

void SEMWizard::on_CommLinDurationPercentageLineEdit_textEdited(const QString&)
{
//    SC.getNewSCCommunication()->setPercentageOfContactTimePerOrbit
//            (CommLinDurationPercentageLineEdit->text().toDouble());
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
