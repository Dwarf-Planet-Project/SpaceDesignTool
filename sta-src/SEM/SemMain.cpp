#include "SemMain.h"

//----------------- Constructor -------------------------------------//
SemMain::SemMain()
{
//    SCMissionDetails.setSCMeanAltitude(36000000); //m
//    SCMissionDetails.setPlanetProperties("Earth");



    NewSCStructure.setMassEstimations("Light_Satellite");
}

//----------------- Functions ---------------------------------------//
void SemMain::setSCMissionDetails(MissionDetails *SCMission)
{
    SCMissionDetails = *SCMission;
}

MissionDetails * SemMain::getSCMissionDetails()
{
    return &SCMissionDetails;
}

void SemMain::setNewSCStructure(StructureSubsystem* SCStructure)
{
    NewSCStructure = *SCStructure;
}

StructureSubsystem* SemMain::getNewSCStructure()
{
    return &NewSCStructure;
}

void SemMain::setNewPayloads(PayloadSubsystem *NewPayloads)
{
    NewPayload[0] = *NewPayloads;
}

PayloadSubsystem* SemMain::getNewPayloads()
{
    return NewPayload;
}

void SemMain::setNewSCPower(PowerSubsystem *SCPower)
{
    NewSCPower = *SCPower;
}

PowerSubsystem* SemMain::getNewSCPower()
{
    return &NewSCPower;
}

void SemMain::setNewSCThermal(ThermalSubsystem *SCThermal)
{
    NewSCThermal = *SCThermal;
}

ThermalSubsystem* SemMain::getNewSCThermal()
{
    return &NewSCThermal;
}

void SemMain::setNewSCDataHandling(OBDHSubsystem *DataHandling)
{
    NewSCDataHandling = *DataHandling;
}

OBDHSubsystem* SemMain::getNewSCDataHandling()
{
    return &NewSCDataHandling;
}

void SemMain::setNewSCCommunication(TTCSubsystem *NewSCComm)
{
    NewSCCommunication = *NewSCComm;
}

TTCSubsystem* SemMain::getNewSCCommunication()
{
    return &NewSCCommunication;
}

void SemMain::PassPayloadOutputParameters()
{
    // setting Stucture parameters with payload
    //transfering PayloadSubsystem Info to StructureSubsystem
    int i;
    for (i=0; i<4; i++)
    {
        NewSCStructure.setPayloadsStructure
                (i,
                 (NewPayload+i)->getPayloadName(),
                 (NewPayload+i)->getPayloadDimensions(),
                 (NewPayload+i)->getPayloadVolume(),
                 (NewPayload+i)->getPayloadMass());
        qDebug()<<"PayloadMass"<<(NewPayload+i)->getPayloadMass();
    }

    //transfering PayloadSubsystem Info to PowerSubsystem
    for (i=0; i<4; i++)
    {
        NewSCPower.setPayloadsPower
                (i,
                 (NewPayload+i)->getPayloadName(),
                 (NewPayload+i)->getPayloadPowerInEclipse(),
                 (NewPayload+i)->getPayloadPowerOnPercentageInEclipse(),
                 (NewPayload+i)->getPayloadPowerInDaylight(),
                 (NewPayload+i)->getPayloadPowerOnPercentageInDaylight());
    }

    //transfering PayloadSubsystem Info to ThermalSubsystem
    for (i=0; i<4; i++)
    {
        NewSCThermal.setMaximumPayloadTemperature
                (i,
                 (NewPayload+i)->getPayloadName(),
                 (NewPayload+i)->getPayloadMaximumTemperature());
        NewSCThermal.setMinimumPayloadTemperature
                (i,
                 (NewPayload+i)->getPayloadName(),
                 (NewPayload+i)->getPayloadMinimumTemperature());
    }

    // transfering PayloadSubsystem Info to OBDHSubsystem
    for (i=0; i<4; i++)
    {
        NewSCDataHandling.setPayloadOBDHInfo
                (i,
                 (NewPayload+i)->getPayloadName(),
                 (NewPayload+i)->getPayloadDataRate(),
                 (NewPayload+i)->getPayloadPowerOnPercentageInEclipse()
                    /100
                    * SCMissionDetails.getAverageEclipseDuration() ,
                 (NewPayload+i)->getPayloadPowerOnPercentageInDaylight()
                    /100
                    * SCMissionDetails.getAverageDaylightDuration());
    }

    //make possible structural computations
    NewSCStructure.CalculateAndSetSCVolume();
    NewSCStructure.CalculateAndSetSCMass();

    //set the Heater and radiator power to zero to refresh with the new values
    NewSCThermal.setNeededHeaterToZero();
    NewSCThermal.setNeededRadiatorToZero();

}

void SemMain::PassPowerSubsystemOutputParameters()
{
    //to Structure Subsystem
    NewSCPower.CalculateAndSetPowerSubsystemMass();
    NewSCStructure.setPowerSubsystemMass
            (NewSCPower.getPowersubsystemMass());

    NewSCStructure.setPowerSubsystemVolume
            (NewSCPower.getPowerSubsystemVolume());

    //to Thermal Subsystem
    NewSCThermal.setMaximumSCElectronicsHeatInDaylight
            (NewSCPower.getMaximumPowerConsumptionInDaylight());
    NewSCThermal.setMinimumSCElectronicsHeatInEclipse
            (NewSCPower.getMinimumPowerConsumptionInEclipse());


        qDebug()<<"powersubsystem mass"<< NewSCPower.getPowersubsystemMass();
}

void SemMain::PassThermalSubsystemOutputParameters()
{
    //to Thermal Subsystem
    NewSCPower.setThermalSubsystemPower(NewSCThermal.getThermalSubsystemPower());

    //?????????????????????????????????
//    NewSCPower.SolarArrays.CalculateSAEOLPower();

    //to Structure Subsystem
    NewSCThermal.setThermalSubsystemMass
            (NewSCStructure.getPayloadStructure()->PayloadMass);
    NewSCStructure.setThermalSubsystemMass
            (NewSCThermal.getThermalSubsystemMass());
    NewSCStructure.setThermalSubsystemVolume
            (NewSCStructure.getThermalSubsystemVolume());
}

void SemMain::PassStructureSubsystemOutputParameters()
{
    //to Thermal Subsystem
    NewSCThermal.setSCStructuralProperties
            (NewSCStructure.getSCMaterial().Name,
             NewSCStructure.getSCMaterial().Emmissivity,
             NewSCStructure.getSCMaterial().Absorptivity);

    NewSCThermal.setAreaOfColdFace
            (NewSCStructure.getSCThermalDetails()->AreaOfColdFace);
    NewSCThermal.setAreaOfHotFace
            (NewSCStructure.getSCThermalDetails()->AreaOfHotFace);
    NewSCThermal.setTotalAreaOfColdFace
            (NewSCStructure.getSCThermalDetails()->TotalAreaOfColdFace);
    NewSCThermal.setTotalAreaOfHotFace
            (NewSCStructure.getSCThermalDetails()->TotalAreaOfHotFace);

}

void SemMain::PassTTCSubsystemOutputParameters()
{
    //to Power subsystem
    NewSCPower.setTTCSubsystemPower(NewSCCommunication.getTTCSubsystemPower());
    NewSCPower.SolarArrays.CalculateSAEOLPower();

    //to Structure Subsystem
    NewSCStructure.setTTCSubsystemVolume
            (NewSCCommunication.getTTCSubsystemVolume());
}

void SemMain::PassMissionDetailsOutputParameters()
{
    //to thermal Subsystem
    NewSCThermal.setPlanetThermalProperties
            (SCMissionDetails.getPlanetProperties()->Planet,
             SCMissionDetails.getPlanetProperties()->PlanetAngularRadius,
             SCMissionDetails.getPlanetProperties()->SolarConstantOfPlanet,
             SCMissionDetails.getPlanetProperties()->AlbedoConstantOfPlanet,
             SCMissionDetails.getPlanetProperties()->PlanetIREnergy);

    //to Power Subsystem
    NewSCPower.SolarArrays.setSolarConstant
            (SCMissionDetails.getPlanetProperties()->SolarConstantOfPlanet);

    //convert to years by dividing 365 days
    NewSCPower.setMissionDuration
            (ceil(SCMissionDetails.getMissionDuration()) / 365);

    NewSCPower.setEclipseDuration
            (SCMissionDetails.getAverageEclipseDuration());

    NewSCPower.setDaylightDuration
            (SCMissionDetails.getAverageDaylightDuration());

    //to Data Handling Subsystem
    NewSCDataHandling.setMissionDuration
            (SCMissionDetails.getMissionDuration());

    //to TTC Subsystem
    NewSCCommunication.setAverageOrbitDuration
            (SCMissionDetails.getAverageOrbitDuration());
    qDebug()<<"SCMissionDetails.getAverageOrbitDuration()"<<SCMissionDetails.getAverageOrbitDuration();
}

void SemMain::PassOBDHSubsystemOutputParameters()
{
    //to structure Subsystem
    NewSCDataHandling.setOBDHSubsystemMass
            (NewSCStructure.getPayloadStructure()->PayloadMass);
    NewSCStructure.setOBDHSubsystemMass
            (NewSCDataHandling.getOBDHSubsystemMass());

    //to power subsystem
    NewSCPower.CalculateAndSetTotalSCPower();
    NewSCDataHandling.setOBDHSubsystemPower
            (NewSCPower.getSCPower());
    NewSCPower.setOBDHSubsystemPower
            (NewSCDataHandling.getOBDHSubsystemPower());
    NewSCPower.SolarArrays.CalculateSAEOLPower();
}
