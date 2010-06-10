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

#ifndef POWERSUBSYSTEM_H
#define POWERSUBSYSTEM_H

#include <QString>
using namespace std;

#define NumberOfPayloads 4

//-------------------------- Enumarations -----------------------------//

//enum SolarCellName{SCUndefined, Si, GaAs, Multijunction };
//enum BatteryName{BUndefined, LithiumIon, NickelCadmium, LeadAcid};

//------------------------- Structures ---------------------------------//
struct PayloadPowerInfo
{
    QString Name;
    double PowerConsumptionInEclipse;
    double PowerOnTimeInEclipse;
    double PowerOnPercentageInEclipse;
    double PowerConsumptionInDaylight;
    double PowerOnTimeInDaylight;
    double PowerOnPercentageInDaylight;
};

/*
  *The User will choose the type and the following properties of the battery
  *will be filled from the database.
    SCBattery
*/
struct BatteryType
{
    QString Name;
    double SpecificEnergyDensity;      // Cr
    double SpecificPower;              // Cp
    double VolumeEnergyDensity;        // Cv
};

/*
  *The User will choose the type and the properties of the solar cell
  *will be filled from the database.
 */
struct SolarCellType
{
    QString Name;
    double ConversionEfficiency;
    double DegradationPerYear;
};

struct PowerDetails
{
    double TotalPayloadPower;
    double ThermalSubsystemPower;
    double TTCSubsystemPower;
    double OBDHSubsystemPower;
    double SubsystemsTotalPower;
    double SCTotal;
};

class CSolarArray
{
public:

    //----------------- Constructor -------------------------------------//
    CSolarArray();

    //----------------- Functions ---------------------------------------//
    /**
        * It fills the properties of the solar cell from the database.
        * (Before the database creation it will be put constant values) It
        * sets the "SolarCell".
        * @param Name    the name of the solar cell
    */
    void setSolarCell(QString Name);

    /**
        * It gets the "SolarCell". Before this method programmer has to be
        * sure that "setSolarCell" method is called otherwise result is not
        * reliable.
    */
    SolarCellType getSolarCell();

    void CalculateSAEOLPower();
    void setSAEOLPower(double power);
    double getSAEOLPower();
    void CalculateSABOLPower();
    void setSABOLPower(double power);
    double getSABOLPower();

    void CalculateSAMass();
    void setSAMass(double mass);
    double getSAMass();

    double getLifeTimeDegradation();

    /*
    * Includes Power Control Unit and Regulators
    */
    void CalculatePCUMass();
    double getPCUMass();

    void CalculateArea();
    void setSAArea(double area);
    double getArea();

    void CalculateSolarCellBOLPower();
    /*
      *  It gets the "BOLPower" of solar cell. Before this method programmer has to be
      * sure that "CalculateBOLPower" method is called otherwise result
      * is not reliable. W/m2
    */
    double getSolarCellBOLPower();

    /**
      *@param MissionDuration should be in years
      */
    void CalculateSolarCellEOLPower();
    /*
      *  It gets the "EOLPower" of solar cell. Before this method programmer has to be
      * sure that "CalculateEOLPower" method is called otherwise result is
      * not reliable. W/m2
    */
    double getSolarCellEOLPower();

    void setSolarConstant(double Constant);
    double getSolarConstant();

    // in seconds
    void setEclipseDuration(double Eclipse);
    void setDaylightDuration(double Daylight);
    // in years
    void setMissionDuration(double Duration);

    void setPayloadWattHourInEclipse(double WattHour);
    void setPayloadWattHourInDaylight(double WattHour);

    //Power Consumption of all subsystems (W)
    void setSystemPower(double Power);

private:
     //----------------- Member objects ---------------------------------//
    //structural parameters
    double SAMass;
    double PCUMass;
    double SAArea;

    double SAEOLPower;
    double SABOLPower;
    double SolarCellBOLPower;
    double SolarCellEOLPower;
    double LifeTimeDegradation;

    // set from Power Subsystem
    double SystemPower;

    double SolarConstant;

    //To set the Powersubsystem private members
    double EclipseDuration;
    double MissionDuration;
    double DaylightDuration;
    double PayloadWattHourInEclipse;
    double PayloadWattHourInDaylight;

    SolarCellType SolarCell;

};
//-------------- End of CSolarArray class definition -------------------//

class CBattery
{
public:
    //----------------- Constructor -------------------------------------//
    CBattery();
    //----------------- Functions ---------------------------------------//
    /**
        * It fills the properties of the solar cell from the database.
        * (Before the database creation we will put constant values) It
        * sets the "SCBattery".
        * @param Name    the name of the solar cell
    */
    void setBatteryProperties(QString Name);
    BatteryType getBatteryProperties();

    void CalculateAndSetBMass();
    void setBMass(double mass);
    double getBMass();

    void CalculateAndSetBVolume();
    void setBVolume(double volume);
    double getBVolume();

    void CalculateAndSetNumberOfBatteries();
    void setNumberOfBatteries(int number);
    int getNumberOfBatteries();

    // in seconds
    void setEclipseDuration(double Eclipse);
    void setDaylightDuration(double Daylight);
    // in years
    void setMissionDuration(double Duration);

    void setPayloadWattHourInEclipse(double WattHour);
    void setPayloadWattHourInDaylight(double WattHour);

private:
    //----------------- Member objects ----------------------------------//
    double BMass;
    double BVolume;
    int NumberOfBatteries;

    //To set the Powersubsystem private members
    double EclipseDuration;
    double MissionDuration;
    double DaylightDuration;
    double PayloadWattHourInEclipse;
    double PayloadWattHourInDaylight;

    BatteryType SCBattery;
};
//-------------- End of CBattery class definition -------------------//

class PowerSubsystem
{
public:

    //----------------- Constructor -------------------------------------//
    /*
    *  Fill the "Payloads" array's power info properties with 0's and name
    *as "Undefined".
    */
    PowerSubsystem();

    //----------------- Member objects ----------------------------------//
    CSolarArray SolarArrays;
    CBattery Battery;
    //----------------- Functions ---------------------------------------//

    /**
        * The function places power information of the payload in the
        * specified index of the "Payloads" Array.
        * @param Index                      It is the index of "Payloads"
        *                                   Array of each Payload (1-4)
        * @param Name                       The descriptive Name of the
        *                                   Payload.
        * @param PowerConsumptionInEclipse  Average Power that Payload will
        *                                   consume in Eclipse(W).
        * @param PowerOnPercentageInEclipse Amount of time that the Payload
        *                                   will be turned on @ Eclipse(%).
        * @param PowerConsumptionInDaylight Average Power that Payload will
        *                                   consume in Daylight(W).
        * @param PowerOnPercentageInDayligh Amount of time that Payload will
        *                                   be turned on @ Daylight(%).
    */
    void setPayloadsPower(int    Index,
                          QString Name,
                          double PowerConsumptionInEclipse,
                          double PowerOnPercentageInEclipse,
                          double PowerConsumptionInDaylight,
                          double PowerOnPercentageInDaylight);

    /**
        * Brings the Payload with the specified Index
        * @param Index It is the index of "Payloads" Array
    */
    PayloadPowerInfo getPayloadOfIndex(int Index);

    double getPossibleMaximumPowerConsumptionsOfPayloads();

    double getMinimumPowerConsumptionInEclipse();

    double getMaximumPowerConsumptionInDaylight();

    /**
      * It produce the Power Consumption Function of SC for an orbit
      * The Details of the Function can be found in STA-OY-TN-1001 1.0
      * Figure 2.4.2
      * @param EclipseDuration         Eclipse Duration of S/C in seconds
      * @param DaylightDuration        Daylight Duration of S/C in seconds
      * @param TimeStep                It will try to be link to the time
      *                                step of propogator to make a standart
      *                                output. Power consumption of the S/C
      *                                at a "index of ConsumptionFunction"
      *                                times "TimeStep" is ConsumptionFunction[Index]
      * @return derefrence of ConsumptionFunction
    */
    void CreatePowerConsumptionFunctionOfSpacecraft();

    /**
      * It produce the Generated Power Function of SC by SolarArrays for an mission
      * @param EclipseDuration         Eclipse Duration of S/C in seconds
      * @param DaylightDuration        Daylight Duration of S/C in seconds
      * @param MissionDuration         Mission Duration of S/C in seconds
      * @param TimeStep                It will try to be link to the time
      *                                step of propogator to make a standart
      *                                output. Power consumption of the S/C
      *                                at a "index of ConsumptionFunction"
      *                                times "TimeStep" is ConsumptionFunction[Index]
      * @return derefrence of ConsumptionFunction
    */
    void CreateGeneratedPowerTimeFunctionOfSpacecraft();

    void CreateNetPowerTimeFunctionOfSpacecraft();

    void CalculateAndSetPowerSubsystemMass();
    double getPowersubsystemMass();

    void setEclipseDuration(double EclipseTime);
    double getEclipseDuration();

    void setDaylightDuration(double DaylightTime);
    double getDaylightDuration();

    void setMissionDuration(double Duration);
    double getMissionDuration();

    double getPayloadWattHourInEclipse();
    double getPayloadWattHourInDaylight();

    double getAveragePowerConsumptionInEclipse();
    double getAveragePowerConsumptionInDaylight();

    //setting subsystem Power requirements
    void CalculateAndSetTotalSCPower();

    void setThermalSubsystemPower(double Power);
    void setTTCSubsystemPower(double Power);
    void setOBDHSubsystemPower(double Power);

    double getSCPower();

    void setPowerSubsystemDataRate(double DataRate);
    double getPowerSubsystemDataRate();

    double getPowerSubsystemVolume();

private:

    //----------------- Member objects ----------------------------------//
    //- system budgets - output
    double PowerSubsystemMass;
    double PowerSubsystemDataRate;
    double PowerSubsystemVolume;

    //- mission information - input
    //Has to be set from mission window (s)
    double EclipseDuration;
    double DaylightDuration;
    double MissionDuration;

    //- I/O parameters of internal calculations
    double PossibleMaximumPowerConsumptionsOfPayloads;

    //Possible Minimum Power that the SC systems and all payloads can
    //consume in Eclipse:
    double MinimumPowerConsumptionInEclipse;

    //Possible Maximum Power that the SC systems and all payloads can
    //consume in Daylight.
    double MaximumPowerConsumptionInDaylight;

    //The information will be extracted from the Payload Subsystem.
    //A Satelite can have at most 4 Payload.
    PayloadPowerInfo Payloads [NumberOfPayloads];

    // power * (time of payload working duration)
    double PayloadWattHourInEclipse;
    double PayloadWattHourInDaylight;

    double AveragePowerConsumptionInEclipse;
    double AveragePowerConsumptionInDaylight;

    PowerDetails SCPowerDetails;

    //---------------functions--------------------------------------------//
    /**
      * It calculates the minimum possible power consumption
      * of the system plus the payload's in Eclipse. It sets the
      * MinimumPowerConsumptionInEclipse
      * --@param Eclipse          The Eclipse duration of SC in seconds
    */
    void CalculateAndSetMinimumPowerConsumptionInEclipse();
    /*
      *   It compares the maximum power consumption of payloads in
      * Eclipse and the Daylight. And set the PossibleMaximumPower-
      * ConsumptionsOfPayloads to the maximum value according to comparison.
    */
    void CalculateAndSetPossibleMaximumPowerConsumptionsOfPayloads();
    /*
      *  It calculates the maximum possible power consumption
      * of the SC as system plus the payload's in Daylight. It sets the
      * MaximumPowerConsumptionInDaylight
    */
   void CalculateAndSetMaximumPowerConsumptionInDaylight();
   /*
     * Simple arithmetic average calculations of power consumption
     * in Daylight and Eclipse
     */
   void CalculateAndSetAveragePowerConsumptionInEclipseAndDaylight();


};
//-------------- End of PowerSubsystem class definition -----------------//



#endif // POWERSUBSYSTEM_H
