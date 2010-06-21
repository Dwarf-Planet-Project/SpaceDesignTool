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

#ifndef SEMMAIN_H
#define SEMMAIN_H

#include "PayloadSubsystem.h"
#include "StructureSubsystem.h"
#include "PowerSubsystem.h"
#include "ThermalSubsystem.h"
#include "MissionDetails.h"
#include "OBDHSubsystem.h"
#include "TTCSubsystem.h"
#include "Launcher.h"

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

    void setNewLauncher(Launcher* SCLaunch);
    Launcher* getNewLauncher();
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
        Launcher    NewLauncher;
};
#endif // SEMMAIN_H
