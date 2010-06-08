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

    //input julian days set in years
    void setMissionDuration(double Duration);
    double getMissionDuration();

    void setPayloadOBDHInfo(int Index,
                            QString Name,
                            double DataRate,
                            double PowerOnTimeInEclipse,
                            double PowerOnTimeInDaylight);
    PayloadOBDHInfo * getPayloadOBDHInfo();

    //it should be percentage
    void setSafetyMargin(double Margin);

    //Later it can be expended to a database of codings
    void setCodingPercentage(QString Name);
    QString getCodingName();

    void setNumberOfOrbitWithNolink(int Number);
    int getNumberOfOrbitWithNoLink();

    //returns GB
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
