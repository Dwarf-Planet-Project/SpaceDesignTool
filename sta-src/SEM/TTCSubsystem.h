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
