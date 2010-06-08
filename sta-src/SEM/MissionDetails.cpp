#include "MissionDetails.h"

//to Read and write to file
#include <QFile>
#include <QTextStream>
#include <QDir>
#include <QProcess>

#include <QDebug>

#include <math.h>

#include "Astro-Core/constants.h"
#include "Astro-Core/date.h"

using namespace sta;

//----------------- Constructor -------------------------------------//
MissionDetails::MissionDetails()
{
    PlanetProperties.Planet = "";
    PlanetProperties.AlbedoConstantOfPlanet = 0.0;
    PlanetProperties.PlanetAngularRadius = 0.0;
    PlanetProperties.CollimatedSolarEnergyOfPlanet = 0.0;
    PlanetProperties.PlanetIREnergy = 0.0;
    PlanetProperties.SolarConstantOfPlanet = 0.0;
    PlanetProperties.GravityParameter = 0.0;

    SCDetails.Name = "";
    SCDetails.SCMeanAltitude = 0.0;
    SCDetails.AverageEclipseDuration = 0.0;//s
    SCDetails.AverageDaylightDuration = 0.0;//s
    SCDetails.AverageOrbitDuration = 0.0;//s
    SCDetails.MissionDuration = 0.0; //days
    SCDetails.ArgumentOfPerigee = 0.0;
    SCDetails.Eccentricity = 0.0;
    SCDetails.Inclination = 0.0;
    SCDetails.RightAscensionNode = 0.0;
    SCDetails.SemiMajorAxis = 0.0;
    SCDetails.TrueAnomaly = 0.0;
    SCDetails.MissionStartTime = 0.0;
    SCDetails.MissionEndTime = 0.0;

}

//----------------- Functions ---------------------------------------//
void MissionDetails::setPlanetProperties(QString PlanetName)
{
    QString path = QString("data/PlanetThermalProperties.stad");

    QFile PlanetThermal(path);
    PlanetThermal.open(QIODevice::ReadOnly);
    //       qDebug()<<PlanetThermal.fileName();
    //       qDebug()<<PlanetThermal.isOpen();
    QTextStream PlanetThermalStream(&PlanetThermal);

    //to skip the first descriptive line of the "PlanetThermalProperties.stad"
    QString InfosOfStadFile;
    int NumberOfColumns = 6;
    int i;
    for (i=0;i<NumberOfColumns;i++)
    {
        PlanetThermalStream >> InfosOfStadFile;
        //           qDebug() << InfosOfStadFile;
    }

    //find the values from the database - http://nssdc.gsfc.nasa.gov/planetary/factsheet/
    QString PName = "";
    double Radius;
    double GravityParameter;
    double SolarConstant;
    double AlbedoConstant;
    double BlackBodyTemperature;

    do
    {
        PlanetThermalStream >> PName;
//        qDebug()<< PName;
        PlanetThermalStream >> Radius;
//        qDebug()<< Radius;
        PlanetThermalStream >> GravityParameter;
//        qDebug()<< GravityParameter;
        PlanetThermalStream >> SolarConstant;
//        qDebug()<< SolarConstant;
        PlanetThermalStream >> AlbedoConstant;
//        qDebug()<< AlbedoConstant;
        PlanetThermalStream >> BlackBodyTemperature;
//        qDebug()<< BlackBodyTemperature;

//        qDebug()<< "while - " <<((!(PName==PlanetName))&&(!(PName=="UNDEFINED")));
    }
    while ((!(PName==PlanetName))&&(!(PName=="UNDEFINED")));
    PlanetThermal.close();

    // set the values from the database

    PlanetProperties.Planet = PName;
    qDebug()<<"Planet -"<<PlanetProperties.Planet;

    PlanetProperties.Radius = Radius;
    qDebug()<<"Radius -"<<PlanetProperties.Radius;

    PlanetProperties.GravityParameter = GravityParameter;
    qDebug()<<"GravityParameter -"<<PlanetProperties.GravityParameter;

    PlanetProperties.SolarConstantOfPlanet = SolarConstant;
    qDebug()<<"Solar Constant -"<< PlanetProperties.SolarConstantOfPlanet;

    PlanetProperties.AlbedoConstantOfPlanet = AlbedoConstant;
    qDebug()<<"Albedo Constant -"<<PlanetProperties.AlbedoConstantOfPlanet;

    PlanetProperties.PlanetIREnergy =
            BlackBodyTemperature * STEFAN_BOLTZMANN_CONSTANT;
    qDebug()<<"Planet IR Energy -"<<PlanetProperties.PlanetIREnergy;

    // set SC mean altitude if the semiMajor axis is set
    if ((PlanetProperties.Radius > 0.0)&&(SCDetails.SemiMajorAxis > 0.0))
        SCDetails.SCMeanAltitude
                = SCDetails.SemiMajorAxis
                  - PlanetProperties.Radius;

    //set the average orbital period in seconds
    if ((PlanetProperties.GravityParameter > 0.0)
        &&(SCDetails.SemiMajorAxis > 0.0))
    SCDetails.AverageOrbitDuration = 2 * mypi
                                     * sqrt(pow(SCDetails.SemiMajorAxis,3)
                                            /PlanetProperties.GravityParameter);
    qDebug()<<"SCDetails.AverageOrbitDuration"<<SCDetails.AverageOrbitDuration;

    //calculate Angular radius and collimated energy which is depending on SCMeanAltitude
    PlanetProperties.PlanetAngularRadius =
           fabs(asin(PlanetProperties.Radius/
                 (PlanetProperties.Radius + SCDetails.SCMeanAltitude)));

    qDebug()<<"Planet Angular Radius -"<<PlanetProperties.PlanetAngularRadius;

    //Collimated is only known for Earth
    if (PlanetProperties.Planet=="Earth")
    {
        PlanetProperties.CollimatedSolarEnergyOfPlanet =
                0.664
                + 0.521 * PlanetProperties.PlanetAngularRadius
                - 0.203 * pow(PlanetProperties.PlanetAngularRadius,2);
    }
    else
    {
        PlanetProperties.CollimatedSolarEnergyOfPlanet = 1.0;
    }

    qDebug()<<"Planet Collimated Solar Energy -"<<PlanetProperties.CollimatedSolarEnergyOfPlanet;
}

PlanetInfo * MissionDetails::getPlanetProperties()
{
    return &PlanetProperties;
}

void MissionDetails::setSCMeanAltitude(double altitude)
{
    SCDetails.SCMeanAltitude = altitude;
    qDebug()<<"SCDetails.SCMeanAltitude -"<<SCDetails.SCMeanAltitude;
}

double MissionDetails::getSCMeanAltitude()
{
    return SCDetails.SCMeanAltitude;
}

void MissionDetails::setMissionStartTime(QDateTime calendarDate)
{
    //convert the time to seconds
    SCDetails.MissionStartTime
            = sta::CalendarToJd
              (calendarDate);

    // Calculate the MissionDuration, in days
    if ((SCDetails.MissionStartTime > 0.0)
            &&(SCDetails.MissionEndTime > 0.0))
        SCDetails.MissionDuration
                = (SCDetails.MissionEndTime
                    - SCDetails.MissionStartTime);
    qDebug()<<"SCDetails.MissionStartTime"<<SCDetails.MissionStartTime;
    qDebug()<<"SCDetails.MissionEndTime"<<SCDetails.MissionEndTime;
    qDebug()<<"SCDetails.MissionDuration"<<SCDetails.MissionDuration;
}

QDateTime MissionDetails::getMissionStartTime()
{
    return sta::JdToCalendar(SCDetails.MissionStartTime);
}

void MissionDetails::setMissionEndTime(QDateTime calendarDate)
{
    SCDetails.MissionEndTime
            = sta::CalendarToJd
              (calendarDate);

    // Calculate the MissionDuration, in days
    if ((SCDetails.MissionStartTime > 0.0)
            &&(SCDetails.MissionEndTime > 0.0))
        SCDetails.MissionDuration
                = (SCDetails.MissionEndTime
                    - SCDetails.MissionStartTime);
    qDebug()<<"SCDetails.MissionStartTime"<<SCDetails.MissionStartTime;
    qDebug()<<"SCDetails.MissionEndTime"<<SCDetails.MissionEndTime;
    qDebug()<<"SCDetails.MissionDuration"<<SCDetails.MissionDuration;
}

QDateTime MissionDetails::getMissionEndTime()
{
    return sta::JdToCalendar(SCDetails.MissionEndTime);
}

void MissionDetails::setEccentricity(double eccentricity)
{
    SCDetails.Eccentricity = eccentricity;
}

double MissionDetails::getEccentricity()
{
    return SCDetails.Eccentricity;
}

void MissionDetails::setSemiMajorAxis(double semiMajor)
{
    SCDetails.SemiMajorAxis = semiMajor * 1000; //in meters

    //set the average orbital period in seconds
    if ((PlanetProperties.GravityParameter > 0.0)
        &&(SCDetails.SemiMajorAxis > 0.0))
    SCDetails.AverageOrbitDuration = 2 * mypi
                                     * sqrt(pow(SCDetails.SemiMajorAxis,3)
                                            /PlanetProperties.GravityParameter);
    qDebug()<<"SCDetails.AverageOrbitDuration"<<SCDetails.AverageOrbitDuration;

    // set SC mean altitude if the planet radius is set
    if ((PlanetProperties.Radius > 0.0)&&(SCDetails.SemiMajorAxis > 0.0))
        SCDetails.SCMeanAltitude
                = SCDetails.SemiMajorAxis
                  - PlanetProperties.Radius;


}

double MissionDetails::getSemiMajorAxis()
{
    return SCDetails.SemiMajorAxis / 1000;  //return the values in km
}

void MissionDetails::setInclination(double inclination)
{
    SCDetails.Inclination = inclination;
}

double MissionDetails::getInclination()
{
    return SCDetails.Inclination;
}

void MissionDetails::setArgumentOfPerigee(double argumentOfPerigee)
{
    SCDetails.ArgumentOfPerigee = argumentOfPerigee;
}

double MissionDetails::getArgumentOfPerigee()
{
    return SCDetails.ArgumentOfPerigee;
}

void MissionDetails::setRightAscensionNode(double RAAN)
{
    SCDetails.RightAscensionNode = RAAN;
}

double MissionDetails::getRightAscensionNode()
{
    return SCDetails.RightAscensionNode;
}

void MissionDetails::setTrueAnomaly(double Anomaly)
{
    SCDetails.TrueAnomaly = Anomaly;
}

double MissionDetails::getTrueAnomaly()
{
    return SCDetails.TrueAnomaly;
}

double MissionDetails::getAverageEclipseDuration()
{
    SCDetails.AverageEclipseDuration
            = SCDetails.AverageOrbitDuration
              * (2 * PlanetProperties.PlanetAngularRadius)
              / (2 * mypi);
    return SCDetails.AverageEclipseDuration;
}

double MissionDetails::getAverageDaylightDuration()
{
    if (SCDetails.AverageEclipseDuration <= SCDetails.AverageOrbitDuration)
    SCDetails.AverageDaylightDuration
            = SCDetails.AverageOrbitDuration
              - SCDetails.AverageEclipseDuration;

        qDebug()<<"SCDetails.AverageDaylightDuration"<<SCDetails.AverageDaylightDuration;
        qDebug()<<"SCDetails.AverageOrbitDuration"<<SCDetails.AverageOrbitDuration;
        qDebug()<<"SCDetails.AverageEclipseDuration"<<SCDetails.AverageEclipseDuration;

    return SCDetails.AverageDaylightDuration;
}

double MissionDetails::getAverageOrbitDuration()
{
    return SCDetails.AverageOrbitDuration;
}

double MissionDetails::getMissionDuration()
{
    return SCDetails.MissionDuration;
}

double MissionDetails::getGroundAverageVelocity()
{
    return (2 * mypi
            * (SCDetails.SCMeanAltitude + PlanetProperties.Radius)
            / SCDetails.AverageOrbitDuration);
}
