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

#include "TTCSubsystem.h"

#include "math.h"
#include "Astro-Core/constants.h"

#include <QDebug>

//------------------------- Structures ---------------------------------//


//-------------------- Constructor -------------------------------------//
TTCSubsystem::TTCSubsystem()
{    
    TTCSubsystemMass = 0.0;
    TTCSubsystemVolume = 0.0;
    TTCSubsystemPower = 0.0;

    ContactTimePerOrbit = 0.0;
    ContactPercentage = 0.0;
    TimeOfNoContactPerOrbit = 0.0;
    DownlinkRate = 0.0;

    MemorySizeForPayloads = 0.0;

    AntennaGain= 0.0;
    AntennaFrequency= 0.0;
    AntennaEfficiency= 0.0;
    AntennaPower = 0.0;

    AverageOrbitDuration= 0.0;//s

    AntennaDiameter = 0.0;

    RXAntennaGain = 0.0;
    RXAntennaPower = 0.0;
}

//-------------------- Functions ---------------------------------------//
void TTCSubsystem::setTTCSubsystemMass(double Mass)
{
    TTCSubsystemMass = Mass;
}

double TTCSubsystem::getTTCSubsystemMass()
{
    return TTCSubsystemMass;
}

void TTCSubsystem::CalculateAndSetAntennaVolume()
{
    //for commanly used antennas -focal length/ diameter- ratio
    //is between 0.25 - 0.65. Therefore we will take this value 0.45
    double FocalLength = 0.45 * AntennaDiameter;

    //assuming there will be a feeder in focal length we can estimate
    //the volume as a cylinder whose hight is Focal Length
    TTCSubsystemVolume = mypi
                         * AntennaDiameter * AntennaDiameter / 4
                         * FocalLength;
    qDebug()<<"**       TTCSubsystemVolume "<<TTCSubsystemVolume;
}

void TTCSubsystem::CalculateAndSetAntennaDiameter()
{
    if ((AntennaEfficiency>0.0)&&(AntennaFrequency>0.0))
    {
        AntennaDiameter = sqrt(AntennaGain
                               / AntennaEfficiency
                               * SPEED_OF_LIGHT
                               / mypi
                               / AntennaFrequency);

        CalculateAndSetAntennaVolume();
    }    
}

double TTCSubsystem::getTTCSubsystemVolume()
{
    return TTCSubsystemVolume;
}

void TTCSubsystem::setAntennaPower(double Power)
{
    AntennaPower = Power;
    TTCSubsystemPower
            = (AntennaPower + RXAntennaPower) * (1 + 0.2);  //20% margin
}

double TTCSubsystem::getAntennaPower()
{
    return AntennaPower;
}

void TTCSubsystem::setTTCSubsystemPower(double Power)
{
    TTCSubsystemPower = Power;
}

double TTCSubsystem::getTTCSubsystemPower()
{
    return TTCSubsystemPower;
}

void TTCSubsystem::setPercentageOfContactTimePerOrbit(double Percentage)
{
    if ((Percentage<=100)&&(Percentage>=0))
    {
        ContactPercentage = Percentage;
        ContactTimePerOrbit = Percentage/100 * AverageOrbitDuration;
        TimeOfNoContactPerOrbit = AverageOrbitDuration - ContactTimePerOrbit;
    }    
    else
    {
        ContactPercentage = 0.0;
        ContactTimePerOrbit = 0.0;
        TimeOfNoContactPerOrbit = AverageOrbitDuration - ContactTimePerOrbit;
    }
}

void TTCSubsystem::CalculateAndSetContactTimePerOrbit()
{
    if (DownlinkRate > 0.0)
        ContactTimePerOrbit = ceil(MemorySizeForPayloads / DownlinkRate ) ;
    qDebug()<<"C DownlinkRate"<<DownlinkRate;
    if (ContactTimePerOrbit > AverageOrbitDuration)
        ContactTimePerOrbit = AverageOrbitDuration;

   qDebug()<<"**              C2 DownlinkRate"<<DownlinkRate;
   qDebug()<<"C MemorySizeForPayloads"<<MemorySizeForPayloads;
   qDebug()<<"C ContactTimePerOrbit"<<ContactTimePerOrbit;
}

double TTCSubsystem::getPercentageContactTimePerOrbit()
{
    return ContactPercentage;
}

double TTCSubsystem::getContactTimePerOrbit()
{    
    return ContactTimePerOrbit;
}

void TTCSubsystem::setDownlinkRate(double Rate)
{
    DownlinkRate = Rate;
}

void TTCSubsystem::CalculateAndSetDownLinkRate()
{
    if (ContactTimePerOrbit > 0.0)
        DownlinkRate = ceil(MemorySizeForPayloads / ContactTimePerOrbit) ;
    qDebug()<<"**         D DownlinkRate"<<DownlinkRate;
    qDebug()<<"D MemorySizeForPayloads"<<MemorySizeForPayloads;
    qDebug()<<"D ContactTimePerOrbit"<<ContactTimePerOrbit;
}

double TTCSubsystem::getDownLinkRate()
{
    return DownlinkRate;
}

void TTCSubsystem::setAverageOrbitDuration(double Duration)
{
    AverageOrbitDuration = Duration;
}

void TTCSubsystem::setAntennaGain(double Gain)
{
    AntennaGain = Gain;
    qDebug()<<"AntennaGain"<<AntennaGain;
}

double TTCSubsystem::getAntennaGain()
{
    return AntennaGain;
}

void TTCSubsystem::setAntennaFrequency(double Frequency)
{
    //Hz
    AntennaFrequency = Frequency * 1.0e+9;
}

double TTCSubsystem::getAntennaFrequency()
{
    return (AntennaFrequency / 1.0e+9);
}

void TTCSubsystem::setAntennaEfficiency(double Efficiency)
{
    //it will take as percentage
    AntennaEfficiency = Efficiency/100;
}

double TTCSubsystem::getAntennaEfficiency()
{
    return AntennaEfficiency*100;
}

void TTCSubsystem::setMemorySizeForPayloads(double MemorySize)
{
    //1 byte = 8 bit G = 1024*M
    MemorySizeForPayloads = MemorySize * 8 * 1024;
}

double TTCSubsystem::getAntennaDiameter()
{
    return AntennaDiameter;
}

void TTCSubsystem::setRXAntennaGain(double Gain)
{
    RXAntennaGain = Gain;
    qDebug()<<"RXAntennaGain"<<RXAntennaGain;
}

double TTCSubsystem::getRXAntennaGain()
{
    return RXAntennaGain;
}

void TTCSubsystem::setRXAntennaPower(double Power)
{
    RXAntennaPower = Power;
    TTCSubsystemPower
            = (AntennaPower + RXAntennaPower) * (1 + 0.2);  //20% margin
    qDebug()<<"RXAntennaPower"<<RXAntennaPower;
}

double TTCSubsystem::getRXAntennaPower()
{
    return RXAntennaPower;
}
