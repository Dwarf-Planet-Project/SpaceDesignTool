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

#ifndef MISSIONDETAILS_H
#define MISSIONDETAILS_H

#include <QString>
#include <QDateTime>

using namespace std;

//------------------------- Structures ---------------------------------//
struct PlanetInfo
{
    QString Planet;
    double Radius;
    double GravityParameter;
    double PlanetAngularRadius;
    double SolarConstantOfPlanet;
    double AlbedoConstantOfPlanet;
    double PlanetIREnergy;
    double CollimatedSolarEnergyOfPlanet;
};

struct SCInfo
{
    QString Name;
    double SCMeanAltitude;//m
    double AverageEclipseDuration;//s
    double AverageDaylightDuration;//s
    double AverageOrbitDuration;//s
    double MissionStartTime;//julian date
    double MissionEndTime;//julian date
    double MissionDuration;//days
    double Eccentricity;
    double SemiMajorAxis;//m
    double Inclination;//degree
    double ArgumentOfPerigee;
    double RightAscensionNode;
    double TrueAnomaly;
};

class MissionDetails
{
public:
    //----------------- Constructor -------------------------------------//
    MissionDetails();
    //----------------- Functions ---------------------------------------//
    /**
      * It sets the Name of Planet,
      *             Mean Radius,
      *             GravityParameter(mu),
      *             SolarConstant (average),
      *             AlbedoConstant
      *             BlackBodyTemperature(temp without atmosphere)
      * data of the SolarSystem Planet that is enquired.
      * If SemiMajor Axis is set before this method, it also sets
      *             SCMeanAltitude,
      *             AverageOrbitDuration,
      *             PlanetAngularRadius,
      *             CollimatedSolarEnergyOfPlanet
      * data of the SolarSystem planet that is enquired
      * If SCMeanAltitude is set before this method, it also sets
      *             SCMeanAltitude,
      *             PlanetAngularRadius,
      *             CollimatedSolarEnergyOfPlanet
      *@param PlanetName    the name of a solar system planet
    */
    void setPlanetProperties(QString PlanetName);
    PlanetInfo * getPlanetProperties();

    /*
    * It has to be modified if it is used to calculate other then angular
    * radius of the SC. For now it takes the mean of apogee and perigee
    * altitude
    */
    void setSCMeanAltitude(double altitude);
    double getSCMeanAltitude();

    /*
      * Enter calendar date but it will convert to modified julian date
      * and save as MissionStartTime
      */

    void setMissionStartTime
            (QDateTime calendarDate);
    QDateTime getMissionStartTime();
    void setMissionEndTime
            (QDateTime calendarDate);
    QDateTime getMissionEndTime();

    void setEccentricity(double eccentricity);
    double getEccentricity();

    /*
     * input/output is in 'km' but SemiMajorAxis will be in 'meters'
     * for internal calculations
     */
    void setSemiMajorAxis(double semiMajor);
    double getSemiMajorAxis();

    // input/output is in degrees
    void setInclination(double inclination);
    double getInclination();

    // input/output is in degrees
    void setArgumentOfPerigee(double argumentOfPerigee);
    double getArgumentOfPerigee();

    // input/output is in degrees
    void setRightAscensionNode(double RAAN);
    double getRightAscensionNode();

    // input/output is in degrees
    void setTrueAnomaly(double Anomaly);
    double getTrueAnomaly();

    // Eclipse And Daylight Durations...etc are assuming circular
    // orbit and doing rough calculations for now
    double getAverageEclipseDuration();
    double getAverageDaylightDuration();
    double getAverageOrbitDuration();
    double getMissionDuration();
    double getGroundAverageVelocity();

private:
    //----------------- Member objects ----------------------------------//
    PlanetInfo PlanetProperties;
    SCInfo  SCDetails;
};

#endif // MISSIONDETAILS_H
