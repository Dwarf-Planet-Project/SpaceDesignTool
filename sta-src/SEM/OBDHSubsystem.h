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

#ifndef OBDHSUBSYSTEM_H
#define OBDHSUBSYSTEM_H

#include <QString>
using namespace std;

//------------------------- Structures ---------------------------------//
struct PayloadOBDHInfo
{
    QString Name;
    double DataRate;
    double PowerOnTimeInEclipse;
    double PowerOnTimeInDaylight;
};

struct CodingType
{
    QString Name;
    double CodingPercentage;
};

class OBDHSubsystem
{
public:
    //----------------- Constructor -------------------------------------//
    OBDHSubsystem();
    //----------------- Functions ---------------------------------------//
    void setOBDHSubsystemMass(double PayloadMass);
    double getOBDHSubsystemMass();

    void setOBDHSubsystemVolume(double Volume);
    double getOBDHSubsystemVolume();

    void setOBDHSubsystemPower(double SCPower);
    double getOBDHSubsystemPower();

    //in seconds
    void setOrbitDuration(double Duration);
    double getOrbitDuration();

    /*
     * input days
     * set in years
     * output in years
     *
     */
    void setMissionDuration(double Duration);
    double getMissionDuration();

    void setPayloadOBDHInfo(int Index,
                            QString Name,
                            double DataRate,
                            double PowerOnTimeInEclipse,
                            double PowerOnTimeInDaylight);
    PayloadOBDHInfo * getPayloadOBDHInfo();

    /*
     * it should be percentage like 20% margin
     * setSafetyMargin(20);
    */
    void setSafetyMargin(double Margin);

    /*
     * If you give the defined name of the coding it will
     * set the amount of coding as percentage of the data
     * Later it can be expended to a database of codings
     */
    void setCodingPercentage(QString Name);
    QString getCodingName();

    void setNumberOfOrbitWithNolink(int Number);
    int getNumberOfOrbitWithNoLink();

    //input/output in GBytes
    void CalculateAndSetMemorySizeForPayloads();
    void setMemorySizeForPayloads(double memory);
    double getMemorySizeForPayloads();

    double getTotalPayloadDataRate();


private:
    //----------------- Member objects ----------------------------------//
    //system budgets - output
    double OBDHSubsystemMass;
    double OBDHSubsystemVolume;
    double OBDHSubsystemPower;

    double TotalSCPower;

    //mission information - input
    double OrbitDuration;
    double MissionDuration;

    PayloadOBDHInfo Payloads[4];

    // I/O parameters of internal calculations
    double MemorySizeForPayloads;
    double HousekeepingDataRateOfPayloads;
    CodingType Coding;
    int NumberOfOrbitWithNolink;
    double TotalPayloadDataRate;

    double SafetyMargin;

    //----------------- Functions ---------------------------------------//

};

#endif // OBDHSUBSYSTEM_H
