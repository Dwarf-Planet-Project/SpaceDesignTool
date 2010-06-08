#include "semmaingui.h"

#include "QDebug"

#include "datacommgui.h"
//#include "launchergui.h"
#include "powergui.h"
#include "structuregui.h"
#include "thermalgui.h"

SemMainGUI::SemMainGUI(ScenarioSC* SCVehicle,
                       QWidget * parent,
                       Qt::WindowFlags f) : QDialog(parent,f)
{
	setupUi(this);

        //collect the scenario information
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
                    //for now SEM is only capable of doing loitering systems engineering
                    ScenarioLoiteringType* loitering
                            = dynamic_cast<ScenarioLoiteringType*>(trajectory.data());

                    //collect the mission details and display them in the wizard
                    SC.getSCMissionDetails()->setMissionStartTime
                            (loitering->TimeLine().data()->StartTime());
                    MissionStartDateTimeEdit->setDateTime
                            (SC.getSCMissionDetails()->getMissionStartTime());

                    SC.getSCMissionDetails()->setMissionEndTime
                            (loitering->TimeLine().data()->EndTime());
                    MissionEndDateTimeEdit->setDateTime
                            (SC.getSCMissionDetails()->getMissionEndTime());
                    break;

                }
            }
        }
        //----------------end of mission definition

        RetrieveScenarioSC();

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

//    //display the mission details which are gathered from wizard
//    SemiMajorAxisLineEdit->setText
//            (SC.getSCMissionDetails()->getSemiMajorAxis());
//    EccentricityLineEdit->setText
//            (SC.getSCMissionDetails()->getEccentricity());
//    InclinationLineEdit->setText
//            (SC.getSCMissionDetails()->getInclination());
//    ArgumentOfPerigeeLineEdit->setText
//            (SC.getSCMissionDetails()->getArgumentOfPerigee());
//    RAANLineEdit->setText
//            (SC.getSCMissionDetails()->getRightAscensionNode());

    //setting comboBox from a QString
    int i;
    for (i=0;i<PlanetNameComboBox->count();i++)
        if (PlanetNameComboBox->itemText(i)
            == SC.getSCMissionDetails()->getPlanetProperties()->Planet)
            PlanetNameComboBox->setCurrentIndex(i);

    RefreshSemMainGUI();

    RefreshSemMainGUIPayload();

//    //structure subsystem
//    SCWidthLineEdit->setText
//            (QString::number
//             (SC.getNewSCStructure()->getSpacecraftDimension().x()));
//    SCHeightLineEdit->setText
//            (QString::number
//             (SC.getNewSCStructure()->getSpacecraftDimension().y()));
//    SCLengthLineEdit->setText
//            (QString::number
//             (SC.getNewSCStructure()->getSpacecraftDimension().z()));
//
//    SCMassLineEdit->setText
//            (QString::number
//             (SC.getNewSCStructure()->getSCMass()));
//    SCLateralFreqLineEdit->setText
//            (QString::number
//             (SC.getNewSCStructure()->getLateralFrequency()));
//    SCAxialFreqLineEdit->setText
//            (QString::number
//             (SC.getNewSCStructure()->getAxialFrequency()));
//
//    //datacomm subsystem
//    TotalPayloadDataRateLineEdit->setText
//            (QString::number
//             (SC.getNewSCDataHandling()->getTotalPayloadDataRate()));
//    MemorySizeLineEdit->setText
//            (QString::number
//             (SC.getNewSCDataHandling()->getMemorySizeForPayloads()));
//    TXPowerLineEdit->setText
//            (QString::number
//             (SC.getNewSCCommunication()->getTTCSubsystemPower()));
//    TotalLinkDurationLineEdit->setText
//            (QString::number
//             (SC.getNewSCCommunication()->getContactTimePerOrbit()));
//
//    //thermal subsystem
//    HeaterPowerLineEdit->setText
//            (QString::number
//             (SC.getNewSCThermal()->getNeededHeater()));
//    RadiatorLineEdit->setText
//            (QString::number
//             (SC.getNewSCThermal()->getNeededRadiator()));
//    ColdFaceCoatingLineEdit->setText
//            (SC.getNewSCThermal()->getColdFaceCoatingProperties().Type);
//    HotFaceCoatingLineEdit->setText
//            (SC.getNewSCThermal()->getHotFaceCoatingProperties().Type);
//
//    //power subsystem
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
	qWarning("TODO: %s	%d",__FILE__,__LINE__);
}

void SemMainGUI::on_EccentricityLineEdit_textChanged(const QString&)
{
    SC.getSCMissionDetails()->setEccentricity
            (EccentricityLineEdit->text().toDouble());
	qWarning("TODO: %s	%d",__FILE__,__LINE__);
}

void SemMainGUI::on_InclinationLineEdit_textChanged(const QString&)
{
    SC.getSCMissionDetails()->setInclination
            (InclinationLineEdit->text().toDouble());
	qWarning("TODO: %s	%d",__FILE__,__LINE__);
}

void SemMainGUI::on_ArgumentOfPerigeeLineEdit_textChanged(const QString&)
{
    SC.getSCMissionDetails()->setArgumentOfPerigee
            (ArgumentOfPerigeeLineEdit->text().toDouble());
	qWarning("TODO: %s	%d",__FILE__,__LINE__);
}

void SemMainGUI::on_RAANLineEdit_textChanged(const QString&)
{
    SC.getSCMissionDetails()->setRightAscensionNode
            (RAANLineEdit->text().toDouble());
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

    SC.PassTTCSubsystemOutputParameters();
    SC.PassOBDHSubsystemOutputParameters();

    RefreshSemMainGUI();
	qWarning("TODO: %s	%d",__FILE__,__LINE__);
}

void SemMainGUI::on_ThermalSubsystemGraphComboBox_2_activated(const QString&)
{
	qWarning("TODO: %s	%d",__FILE__,__LINE__);
}

void SemMainGUI::on_ThermalGraphPushButton_2_clicked()
{
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

void SemMainGUI::on_PowerGraphComboBox_2_activated(const QString&)
{
	qWarning("TODO: %s	%d",__FILE__,__LINE__);
}

void SemMainGUI::on_PowerGraphPushButton_clicked()
{
    SC.getNewSCPower()->CreateGeneratedPowerTimeFunctionOfSpacecraft();
	qWarning("TODO: %s	%d",__FILE__,__LINE__);
}

void SemMainGUI::on_PowerDetailsButton_clicked()
{
    PowerGUI dialog(&SC,this);
    dialog.exec();

    RefreshSemMainGUI();
	qWarning("TODO: %s	%d",__FILE__,__LINE__);
}

void SemMainGUI::on_LauncherSelectionComboBox_2_activated(const QString&)
{
	qWarning("TODO: %s	%d",__FILE__,__LINE__);
}

void SemMainGUI::on_LauncherDetailsButton_clicked()
{
//    LauncherGUI dialog(this);
//    dialog.exec();

	qWarning("TODO: %s	%d",__FILE__,__LINE__);
}

void SemMainGUI::on_PlanetNameComboBox_activated(const QString &)
{
    SC.getSCMissionDetails()->setPlanetProperties
            (PlanetNameComboBox->currentText());

    SC.PassMissionDetailsOutputParameters();
}

void SemMainGUI::on_SemMainGUICalculatePushButton_clicked()
{
//    *****************************************CALCULATE


    qDebug()<<"everything fine";

//    NewSCStructure.setSCShape(Cube);



	qWarning("TODO: %s	%d",__FILE__,__LINE__);
}

void SemMainGUI::on_SemMainGUISavePushButton_clicked()
{   
    //save the values to scenario -------------------
    Vehicle->System().data()->SystemBudgets().data()->MassOfSystem().data()->setDryMass
            (SC.getNewSCStructure()->getSCMass());
    Vehicle->System().data()->SystemBudgets().data()->PowerOfSystem().data()->setTotalPowerEoL
            (SC.getNewSCPower()->SolarArrays.getSAEOLPower());
    Vehicle->System().data()->SystemBudgets().data()->PowerOfSystem().data()->setTotalPowerBoL
            (SC.getNewSCPower()->SolarArrays.getSABOLPower());

    //Structural shape has to be inserted ********** it has to be string
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

    Vehicle->System().data()->TCS().data()->Temperature().data()->setMaximumSCTemperature
            (SC.getNewSCThermal()->getMaximumSCTemperature());
    Vehicle->System().data()->TCS().data()->Temperature().data()->setMinimumSCTemperature
            (SC.getNewSCThermal()->getMinimumSCTemperature());
    Vehicle->System().data()->TCS().data()->setHeaterPower
            (SC.getNewSCThermal()->getNeededHeater());
    Vehicle->System().data()->TCS().data()->setRadiatedPower
            (SC.getNewSCThermal()->getNeededRadiator());

    //the name of coating has to be in scenario
    Vehicle->System().data()->TCS().data()->CoatingArea().data()->ColdSurface().data()->ColdCoating().data()->setEmissivity
            (SC.getNewSCThermal()->getColdFaceCoatingProperties().Emmissivity);
    Vehicle->System().data()->TCS().data()->CoatingArea().data()->ColdSurface().data()->ColdCoating().data()->setAbsorptivity
            (SC.getNewSCThermal()->getColdFaceCoatingProperties().Absorptivity);

    Vehicle->System().data()->TCS().data()->CoatingArea().data()->HotSurface().data()->HotCoating().data()->setEmissivity
            (SC.getNewSCThermal()->getHotFaceCoatingProperties().Emmissivity);
    Vehicle->System().data()->TCS().data()->CoatingArea().data()->HotSurface().data()->HotCoating().data()->setAbsorptivity
            (SC.getNewSCThermal()->getHotFaceCoatingProperties().Absorptivity);

    Vehicle->System().data()->TCS().data()->CoatingArea().data()->ColdSurface().data()->setTotalColdArea
            (SC.getNewSCThermal()->getTotalAreaOfColdFace());
    Vehicle->System().data()->TCS().data()->CoatingArea().data()->HotSurface().data()->setTotalHotArea
            (SC.getNewSCThermal()->getTotalAreaOfHotFace());

    Vehicle->System().data()->TCS().data()->setTotalTCSMass
            (SC.getNewSCThermal()->getThermalSubsystemMass());

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

    Vehicle->System().data()->TTC().data()->setTotalTTCMass
            (SC.getNewSCCommunication()->getTTCSubsystemMass());
    Vehicle->System().data()->TTC().data()->TTCAntenna().data()->setTransmitterPower
            (SC.getNewSCCommunication()->getAntennaPower());
    Vehicle->System().data()->TTC().data()->TTCAntenna().data()->setTransmitterFrequency
            (SC.getNewSCCommunication()->getAntennaFrequency());
    Vehicle->System().data()->TTC().data()->TTCAntenna().data()->setTransmitterGain
            (SC.getNewSCCommunication()->getAntennaGain());
    Vehicle->System().data()->TTC().data()->TTCAntenna().data()->setLinkDuration
            (SC.getNewSCCommunication()->getPercentageContactTimePerOrbit());
    Vehicle->System().data()->TTC().data()->TTCAntenna().data()->setDataRate
            (SC.getNewSCCommunication()->getDownLinkRate());

    Vehicle->System().data()->OBDH().data()->setTotalOBDHMass
            (SC.getNewSCDataHandling()->getOBDHSubsystemMass());
    Vehicle->System().data()->OBDH().data()->setTotalSizeOfMemory
            (SC.getNewSCDataHandling()->getMemorySizeForPayloads());

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
            (QString::number(SC.getNewSCCommunication()->getTTCSubsystemPower()));
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
    //Structural shape has to be inserted ********** it has to be string
    SC.getNewSCStructure()->setSpacecraftDimension
            (Vehicle->System().data()->Structure().data()->Sizing().data()->width(),
             Vehicle->System().data()->Structure().data()->Sizing().data()->height(),
             Vehicle->System().data()->Structure().data()->Sizing().data()->length());

    SC.getNewSCStructure()->setSCVolume
            (Vehicle->System().data()->Structure().data()->Sizing().data()->volume());
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

    SC.getNewSCThermal()->setMaximumSCTemperature
            (Vehicle->System().data()->TCS().data()->Temperature().data()->maximumSCTemperature());
    SC.getNewSCThermal()->setMinimumSCTemperature
            (Vehicle->System().data()->TCS().data()->Temperature().data()->minimumSCTemperature());
    SC.getNewSCThermal()->setNeededHeater
            (Vehicle->System().data()->TCS().data()->heaterPower());
    SC.getNewSCThermal()->setNeededRadiator
            (Vehicle->System().data()->TCS().data()->radiatedPower());
    SC.getNewSCThermal()->setColdFaceCoatingProperties //the name has to be in scenario
            ("ozgun",
             Vehicle->System().data()->TCS().data()->CoatingArea().data()->ColdSurface().data()->ColdCoating().data()->emissivity(),
             Vehicle->System().data()->TCS().data()->CoatingArea().data()->ColdSurface().data()->ColdCoating().data()->absorptivity());
    SC.getNewSCThermal()->setHotFaceCoatingProperties //the name has to be in scenario
            ("ozgun",
             Vehicle->System().data()->TCS().data()->CoatingArea().data()->HotSurface().data()->HotCoating().data()->emissivity(),
             Vehicle->System().data()->TCS().data()->CoatingArea().data()->HotSurface().data()->HotCoating().data()->absorptivity());
    // the cold and hot face area has to be included
    SC.getNewSCThermal()->setThermalSubsystemMass
            (Vehicle->System().data()->TCS().data()->totalTCSMass());

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

    SC.getNewSCCommunication()->setTTCSubsystemMass
            (Vehicle->System().data()->TTC().data()->totalTTCMass());
    SC.getNewSCCommunication()->setAntennaPower
            (Vehicle->System().data()->TTC().data()->TTCAntenna().data()->TransmitterPower());
    SC.getNewSCCommunication()->setAntennaFrequency
            (Vehicle->System().data()->TTC().data()->TTCAntenna().data()->TransmitterFrequency());
    SC.getNewSCCommunication()->setAntennaGain
            (Vehicle->System().data()->TTC().data()->TTCAntenna().data()->TransmitterGain());
    SC.getNewSCCommunication()->setPercentageOfContactTimePerOrbit
            (Vehicle->System().data()->TTC().data()->TTCAntenna().data()->LinkDuration());
    SC.getNewSCCommunication()->setDownlinkRate
            (Vehicle->System().data()->TTC().data()->TTCAntenna().data()->DataRate());

    SC.getNewSCDataHandling()->setOBDHSubsystemMass
            (Vehicle->System().data()->OBDH().data()->totalOBDHMass());
    SC.getNewSCDataHandling()->setMemorySizeForPayloads
            (Vehicle->System().data()->OBDH().data()->totalSizeOfMemory());
//--------------------------------system budget transfer from the scenario is completed -------
}
