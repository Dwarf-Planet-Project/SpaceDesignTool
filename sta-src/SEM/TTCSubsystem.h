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

#ifndef TTCSUBSYSTEM_H
#define TTCSUBSYSTEM_H


#include <QString>
using namespace std;

//------------------------- Structures ---------------------------------//

class TTCSubsystem
{
public:
    //----------------- Constructor -------------------------------------//
    TTCSubsystem();
    //----------------- Functions ---------------------------------------//
    void setTTCSubsystemMass(double Mass);
    double getTTCSubsystemMass();

    void CalculateAndSetAntennaVolume();
    void CalculateAndSetAntennaDiameter();
    double getTTCSubsystemVolume();

    void setAntennaPower(double Power);
    double getAntennaPower();

    void setTTCSubsystemPower(double Power);
    double getTTCSubsystemPower();

    void setPercentageOfContactTimePerOrbit(double Percentage);
    void CalculateAndSetContactTimePerOrbit();
    double getPercentageContactTimePerOrbit();
    double getContactTimePerOrbit();

    void setDownlinkRate(double Rate);
    void CalculateAndSetDownLinkRate();
    double getDownLinkRate();

    void setAverageOrbitDuration(double Duration);

    void setAntennaGain(double Gain);
    double getAntennaGain();

    void setAntennaFrequency(double Frequency);
    /*
      * returns Frequency in GHz
*/
    double getAntennaFrequency();

    void setAntennaEfficiency(double Efficiency);
    double getAntennaEfficiency();

    void setMemorySizeForPayloads(double MemorySize);

    double getAntennaDiameter();

private:
    //----------------- Member objects ----------------------------------//
    double TTCSubsystemMass;
    double TTCSubsystemVolume;
    double TTCSubsystemPower;

    double ContactPercentage;
    double ContactTimePerOrbit;
    double TimeOfNoContactPerOrbit;
    double DownlinkRate;

    double MemorySizeForPayloads;

    double AntennaGain;
    double AntennaFrequency;
    double AntennaEfficiency;
    double AntennaDiameter;
    double AntennaPower;

    double AverageOrbitDuration;

};

#endif // TTCSUBSYSTEM_H
