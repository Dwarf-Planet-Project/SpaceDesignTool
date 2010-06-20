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

#include "OBDHSubsystem.h"

#include "math.h"
#include <QDebug>

//------------------------- Structures ---------------------------------//


//-------------------- Constructor -------------------------------------//
OBDHSubsystem::OBDHSubsystem()
{ 
    //system budgets - output
    OBDHSubsystemMass = 0.0;
    OBDHSubsystemVolume = 0.0;
    OBDHSubsystemPower = 0.0;

    TotalSCPower = 0.0;

    //mission information - input
    OrbitDuration = 0.0;
    MissionDuration = 0.0;

    int i;
    for (i=0; i<4; i++)
    {
        Payloads[i].Name = "";
        Payloads[i].DataRate = 0.0;
        Payloads[i].PowerOnTimeInDaylight = 0.0;
        Payloads[i].PowerOnTimeInEclipse = 0.0;
    }

    //parameters of calculations
    MemorySizeForPayloads = 0.0;
    HousekeepingDataRateOfPayloads = 0.0;
    Coding.Name = "";
    Coding.CodingPercentage = 0.0;
    NumberOfOrbitWithNolink = 0.0;

    SafetyMargin = 0.2; // 20%

    TotalPayloadDataRate = 0.0;
}

//-------------------- Functions ---------------------------------------//
void OBDHSubsystem::setOBDHSubsystemMass(double PayloadMass)
{    //Payload is bigger than this number, not to give negative result
    if (PayloadMass > 0.0)
    {
        double tempAdjust = 0.0;
        // if the payload mass is too low for this equation and it
        //gives negative result
        if (PayloadMass < 27.0)
            tempAdjust = 27.0;

        // Total spacecraft Mass (0.2582 * PayloadMass - 2.84965)
        OBDHSubsystemMass = 0.055
                            * (0.2582 * (PayloadMass + tempAdjust) - 2.84965)
                            - 0.2266;
    }
    else
        OBDHSubsystemMass = 0.0;
//    qDebug()<<"Payload Mass"<<PayloadMass;
//    qDebug()<<"OBDHSubsystemMass "<<OBDHSubsystemMass;
}

double OBDHSubsystem::getOBDHSubsystemMass()
{
//    qDebug()<<"OBDHSubsystemMass -"<<OBDHSubsystemMass;
    if (MemorySizeForPayloads > 0.0)
        return OBDHSubsystemMass;
    else
        return 0.0;
}

void OBDHSubsystem::setOBDHSubsystemPower(double SCPower)
{
   if (MemorySizeForPayloads > 0.0)
       {
       TotalSCPower = SCPower;
       OBDHSubsystemPower
               = 0.0817 * SCPower + 1.584;
       }
    qDebug()<<"OBDHSubsystemPower -*"<<OBDHSubsystemPower;
    qDebug()<<"MemorySizeForPayloads -*"<<MemorySizeForPayloads;
    qDebug()<<"SCPower -*"<<SCPower;
    qDebug()<<"TotalSCPower -*"<<TotalSCPower;
}

double OBDHSubsystem::getOBDHSubsystemPower()
{
//    qDebug()<<"OBDHSubsystemPower -"<<OBDHSubsystemPower;
    return OBDHSubsystemPower;
}

void OBDHSubsystem::setOBDHSubsystemVolume(double Volume)
{
    OBDHSubsystemVolume = Volume;
    qDebug()<<"OBDHSubsystemVolume -"<<OBDHSubsystemVolume;
}

double OBDHSubsystem::getOBDHSubsystemVolume()
{
//    qDebug()<<"OBDHSubsystemVolume -"<<OBDHSubsystemVolume;
    return OBDHSubsystemVolume;
}

void OBDHSubsystem::setOrbitDuration(double Duration)
{
    OrbitDuration = Duration;
    qDebug()<<"OBDH OrbitDuration -"<<OrbitDuration;
}

double OBDHSubsystem::getOrbitDuration()
{
    return OrbitDuration;
}

void OBDHSubsystem::setMissionDuration(double Duration)
{
    //make the aproximation of 1 year is 12 month and
    // every month is 30 days
    MissionDuration = Duration / (12 * 30);
    qDebug()<<"MissionDuration -"<<MissionDuration;
}

double OBDHSubsystem::getMissionDuration()
{
    return MissionDuration;
}

void OBDHSubsystem::setPayloadOBDHInfo(int Index,
                                       QString Name,
                                       double DataRate,
                                       double PowerOnTimeInEclipse,
                                       double PowerOnTimeInDaylight)
{
    Payloads[Index].Name = Name;
    Payloads[Index].DataRate = DataRate;
    Payloads[Index].PowerOnTimeInEclipse = PowerOnTimeInEclipse;
    Payloads[Index].PowerOnTimeInDaylight = PowerOnTimeInDaylight;

    qDebug()<<Index<<" OBDH DataRate"<<Payloads[Index].DataRate;
    qDebug()<<"PowerOnTimeInEclipse"<<Payloads[Index].PowerOnTimeInEclipse;
    qDebug()<<"PowerOnTimeInDaylight"<<Payloads[Index].PowerOnTimeInDaylight;
}

PayloadOBDHInfo * OBDHSubsystem::getPayloadOBDHInfo()
{
    return Payloads;
}

void OBDHSubsystem::setSafetyMargin(double Margin)
{
    SafetyMargin = Margin / 100;
}

void OBDHSubsystem::setCodingPercentage(QString Name)
{
    if (Name == "EDAC")
    {
        Coding.Name = Name;
        Coding.CodingPercentage = 0.2;   //20%
    }
    else
    {
        Coding.Name = Name;
        Coding.CodingPercentage = 0.0;
    }
    qDebug()<<"Coding"<<Name<<Coding.Name;

}

QString OBDHSubsystem::getCodingName()
{
    return Coding.Name;
}

void OBDHSubsystem::setNumberOfOrbitWithNolink(int Number)
{
    NumberOfOrbitWithNolink = Number;

    qDebug()<<"number no link"<< NumberOfOrbitWithNolink;
}

int OBDHSubsystem::getNumberOfOrbitWithNoLink()
{
    return NumberOfOrbitWithNolink;
}

void OBDHSubsystem::CalculateAndSetMemorySizeForPayloads()
{
    double tempPayloadData = 0.0;

    int i;
    for (i=0; i<4; i++)
    {
        tempPayloadData += Payloads[i].DataRate
                           * (Payloads[i].PowerOnTimeInEclipse
                              + Payloads[i].PowerOnTimeInDaylight);
        qDebug()<<"PowerOnTimeInEclipse"<<Payloads[i].PowerOnTimeInEclipse;
        qDebug()<<"PowerOnTimeInDaylight"<<Payloads[i].PowerOnTimeInDaylight;
    }

    double totalDataWithNoCoding = 0.0;

    if (tempPayloadData > 0.0) // if there is payload data calculate the memory
    {
        totalDataWithNoCoding = tempPayloadData * (1 + SafetyMargin)
                                + HousekeepingDataRateOfPayloads * (1 + SafetyMargin);

        qDebug()<<"totalDataWithNoCoding"<<totalDataWithNoCoding;

        double totalDataWithCoding = 0.0;
        totalDataWithCoding = totalDataWithNoCoding * (1 + Coding.CodingPercentage);

        qDebug()<<"totalDataWithCoding"<<totalDataWithCoding;

        //The effect of degradation during the mission is added 2.5Gbits/yr ->2560Mbits/yr
        // MemorySizeForPayloads in GB dividing 8 is for bytes,
        //1024 to conversion from megabits to GB
        MemorySizeForPayloads = ((NumberOfOrbitWithNolink + 1) * totalDataWithCoding
                                + 2560 * MissionDuration)/(8 * 1024);
    }

    qDebug()<<"MemorySizeForPayloads"<<MemorySizeForPayloads;
}

void OBDHSubsystem::setMemorySizeForPayloads(double memory)
{
    MemorySizeForPayloads = memory;
}

double OBDHSubsystem::getMemorySizeForPayloads()
{
    return MemorySizeForPayloads;
}

double OBDHSubsystem::getTotalPayloadDataRate()
{
    TotalPayloadDataRate = 0.0;
    int i;
    for (i=0; i<4; i++)
        TotalPayloadDataRate += Payloads[i].DataRate;

    return TotalPayloadDataRate;
}
