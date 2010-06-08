#ifndef SEMMAIN_H
#define SEMMAIN_H

#include "PayloadSubsystem.h"
#include "StructureSubsystem.h"
#include "PowerSubsystem.h"
#include "ThermalSubsystem.h"
#include "MissionDetails.h"
#include "OBDHSubsystem.h"
#include "TTCSubsystem.h"

class SemMain
{
public:
    //----------------- Constructor -------------------------------------//
    SemMain();

    //----------------- Functions ---------------------------------------//
    void setSCMissionDetails(MissionDetails* SCMission);
    MissionDetails* getSCMissionDetails();

    void setNewSCStructure(StructureSubsystem* SCStructure);
    StructureSubsystem* getNewSCStructure();

    void setNewPayloads(PayloadSubsystem* NewPayloads);
    PayloadSubsystem* getNewPayloads();

    void setNewSCPower(PowerSubsystem* SCPower);
    PowerSubsystem* getNewSCPower();

    void setNewSCThermal(ThermalSubsystem* SCThermal);
    ThermalSubsystem* getNewSCThermal();

    void setNewSCDataHandling(OBDHSubsystem* DataHandling);
    OBDHSubsystem* getNewSCDataHandling();

    void setNewSCCommunication(TTCSubsystem* NewSCComm);
    TTCSubsystem* getNewSCCommunication();
    /*
      *It passes the outputs of every subsystem to the necessary subsystem
      */
    void PassPayloadOutputParameters();
    void PassPowerSubsystemOutputParameters();
    void PassThermalSubsystemOutputParameters();
    void PassStructureSubsystemOutputParameters();
    void PassTTCSubsystemOutputParameters();
    void PassMissionDetailsOutputParameters();
    void PassOBDHSubsystemOutputParameters();

private:
        MissionDetails SCMissionDetails;
        StructureSubsystem NewSCStructure;
        PayloadSubsystem NewPayload[4];
        PowerSubsystem NewSCPower;
        ThermalSubsystem NewSCThermal;
        OBDHSubsystem NewSCDataHandling;
        TTCSubsystem NewSCCommunication;
};
#endif // SEMMAIN_H
