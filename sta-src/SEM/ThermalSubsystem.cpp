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

#include "ThermalSubsystem.h"

#include <QDebug>

//to Read and write to file
#include <QFile>
#include <QTextStream>
#include <QDir>
#include <QProcess>

#include "Astro-Core/constants.h"
#include <math.h>

//----------------- Constructor -------------------------------------//
ThermalSubsystem::ThermalSubsystem()
{
    AreaOfHotFace = 0.0;
    AreaOfColdFace = 0.0;
    TotalAreaOfHotFace = 0.0;
    TotalAreaOfColdFace = 0.0;

    SolarFluxHeat = 0.0;
    AlbedoHeat = 0.0;
    ReceivedPlanetIRHeat = 0.0;
    MinimumSCElectronicsHeatInEclipse = 0.0;
    MaximumSCElectronicsHeatInDaylight = 0.0;
    MinimumSCElectronicsHeatInDaylight = 0.0;
    MaximumSCElectronicsHeatInEclipse = 0.0;

    MinimumSCTemperature = 0.0;
    MaximumSCTemperature = 0.0;

    MinimumSCTempWithRadiatorOrHeater = 0.0;
    MaximumSCTempWithRadiatorOrHeater = 0.0;

    DesiredMinimumTemperature = 273;
    DesiredMaximumTemperature = 323;

    NeededHeater = 0.0;
    NeededRadiator = 0.0;

    PlanetThermalProperties.AlbedoConstantOfPlanet = 0.0;
    PlanetThermalProperties.CollimatedSolarEnergyOfPlanet = 0.0;
    PlanetThermalProperties.PlanetAngularRadius= 0.0;
    PlanetThermalProperties.PlanetIREnergy = 0.0;
    PlanetThermalProperties.SolarConstantOfPlanet = 0.0;

    ColdFaceCoatingProperties.Type = "";
    ColdFaceCoatingProperties.Absorptivity = 0.0;
    ColdFaceCoatingProperties.Emmissivity = 0.0;

    HotFaceCoatingProperties.Type = "";
    HotFaceCoatingProperties.Absorptivity = 0.0;
    HotFaceCoatingProperties.Emmissivity = 0.0;

    int i;
    for (i=0; i<4; i++)
    {
        PayloadThermalDetails[i].Name = "";
        PayloadThermalDetails[i].MaximumTemperature = 0.0;
        PayloadThermalDetails[i].MinimumTemperature = 0.0;
        PayloadThermalDetails[i].Alert = false;
    }

    SCStructuralProperties.Name = "";
    SCStructuralProperties.Absorptivity = 0.0;
    SCStructuralProperties.Emmissivity = 0.0;

    ThermalSubsystemMass = 0.0;
    ThermalSubsystemVolume = 0.0;
    ThermalSubsystemPower = 0.0;

    ThermalControl = NoControl;
}

//----------------- Functions ---------------------------------------//
void ThermalSubsystem::setPlanetThermalProperties(QString PName,
                                                  double AngularRadius,
                                                  double SolarConstant,
                                                  double AlbedoConstant,
                                                  double IREnergy,
                                                  double collimatedSolarEnergy)
{

    PlanetThermalProperties.Planet = PName;
    qDebug()<<PlanetThermalProperties.Planet;

    PlanetThermalProperties.PlanetAngularRadius = AngularRadius;
    qDebug()<<PlanetThermalProperties.PlanetAngularRadius;

    PlanetThermalProperties.SolarConstantOfPlanet = SolarConstant;
    qDebug()<< PlanetThermalProperties.SolarConstantOfPlanet;

    PlanetThermalProperties.AlbedoConstantOfPlanet = AlbedoConstant;
    qDebug()<<PlanetThermalProperties.AlbedoConstantOfPlanet;

    PlanetThermalProperties.PlanetIREnergy = IREnergy;
    qDebug()<<"PlanetIR"<<PlanetThermalProperties.PlanetIREnergy;

    PlanetThermalProperties.CollimatedSolarEnergyOfPlanet =
            collimatedSolarEnergy;
    qDebug()<<"PlanetThermalProperties.CollimatedSolarEnergyOfPlanet"<<PlanetThermalProperties.CollimatedSolarEnergyOfPlanet;

}

PlanetThermalInfo ThermalSubsystem::getPlanetThermalProperties()
{
    return PlanetThermalProperties;
}

void ThermalSubsystem::setColdFaceCoatingProperties(QString Coating)
{
    QString path = QString("data/MaterialDatabase/CoatingProperties.stad");

       QFile CoatingProperties(path);
       CoatingProperties.open(QIODevice::ReadOnly);
//       qDebug()<<CoatingProperties.fileName();
//       qDebug()<<CoatingProperties.isOpen();
       QTextStream CoatingPropertiesStream(&CoatingProperties);

       //to skip the first descriptive line of the "CoatingProperties.stad"
       QString InfosOfStadFile;
       int NumberOfColumns = 3;
       int i;
       for (i=0;i<NumberOfColumns;i++)
       {
           CoatingPropertiesStream >> InfosOfStadFile;
//           qDebug() << InfosOfStadFile;
       }

       //find the values from the database - http://nssdc.gsfc.nasa.gov/planetary/factsheet/
       QString CName = "";
       double Emmissivity;
       double Absorptivity;

       do
       {
           CoatingPropertiesStream >> CName;
           qDebug()<< CName;
           CoatingPropertiesStream >> Emmissivity;
           qDebug()<< Emmissivity;
           CoatingPropertiesStream >> Absorptivity;
           qDebug()<< Absorptivity;

           qDebug()<< "while - " <<((!(CName==Coating))&&(!(CName=="UNDEFINED")));
       }
       while ((!(CName==Coating))&&(!(CName=="UNDEFINED")));

       //close the file reading finished
       CoatingProperties.close();

       // set the values from the database

       ColdFaceCoatingProperties.Type = CName;
       qDebug()<<ColdFaceCoatingProperties.Type;
       ColdFaceCoatingProperties.Emmissivity = Emmissivity;
       qDebug()<<ColdFaceCoatingProperties.Emmissivity;
       ColdFaceCoatingProperties.Absorptivity = Absorptivity;
       qDebug()<<ColdFaceCoatingProperties.Absorptivity;

//       //set if it is active or passive control, mass will be changed according to this
//       if (!(ColdFaceCoatingProperties.Type == "UNDEFINED"))
//       {
//           if (ThermalControl == Active)
//               ThermalControl = PassiveAndActive;
//           else
//               ThermalControl = Passive;
//       }
//       else
//           ThermalControl = NoControl;
}

void ThermalSubsystem::setColdFaceCoatingProperties(QString Coating,
                                                    double Emmissivity,
                                                    double Absorptivity)
{
    ColdFaceCoatingProperties.Type = Coating;
    ColdFaceCoatingProperties.Emmissivity = Emmissivity;
    ColdFaceCoatingProperties.Absorptivity = Absorptivity;

    qDebug()<<"************************* 173";

//    //set if it is active or passive control, mass will be changed according to this
//    if (ColdFaceCoatingProperties.Type == "User_Defined")
//    {
//        if (ThermalControl == Active)
//            ThermalControl = PassiveAndActive;
//        else
//            ThermalControl = Passive;
//    }
//    else
//        ThermalControl = NoControl;
}

CoatingProperties ThermalSubsystem::getColdFaceCoatingProperties()
{
    return ColdFaceCoatingProperties;
}

void ThermalSubsystem::setHotFaceCoatingProperties(QString Coating)
{
    QString path = QString("data/MaterialDatabase/CoatingProperties.stad");

       QFile CoatingProperties(path);
       CoatingProperties.open(QIODevice::ReadOnly);
//       qDebug()<<CoatingProperties.fileName();
//       qDebug()<<CoatingProperties.isOpen();
       QTextStream CoatingPropertiesStream(&CoatingProperties);

       //to skip the first descriptive line of the "CoatingProperties.stad"
       QString InfosOfStadFile;
       int NumberOfColumns = 3;
       int i;
       for (i=0;i<NumberOfColumns;i++)
       {
           CoatingPropertiesStream >> InfosOfStadFile;
//           qDebug() << InfosOfStadFile;
       }

       //find the values from the database - http://nssdc.gsfc.nasa.gov/planetary/factsheet/
       QString CName = "";
       double Emmissivity;
       double Absorptivity;

       do
       {
           CoatingPropertiesStream >> CName;
           qDebug()<< CName;
           CoatingPropertiesStream >> Emmissivity;
           qDebug()<< Emmissivity;
           CoatingPropertiesStream >> Absorptivity;
           qDebug()<< Absorptivity;

           qDebug()<< "while - " <<((!(CName==Coating))&&(!(CName=="UNDEFINED")));
       }
       while ((!(CName==Coating))&&(!(CName=="UNDEFINED")));

       //close the file reading finished
       CoatingProperties.close();

        // set the values from the database

       HotFaceCoatingProperties.Type = CName;
       qDebug()<<HotFaceCoatingProperties.Type;
       HotFaceCoatingProperties.Emmissivity = Emmissivity;
       qDebug()<<HotFaceCoatingProperties.Emmissivity;
       HotFaceCoatingProperties.Absorptivity = Absorptivity;
       qDebug()<<HotFaceCoatingProperties.Absorptivity;

//       //set if it is active or passive control, mass will be changed according to this
//       if (!(HotFaceCoatingProperties.Type == "UNDEFINED"))
//       {
//           if (ThermalControl == Active)
//               ThermalControl = PassiveAndActive;
//           else
//               ThermalControl = Passive;
//       }
//       else
//           ThermalControl = NoControl;

}

void ThermalSubsystem::setHotFaceCoatingProperties(QString Coating,
                                                   double Emmissivity,
                                                   double Absorptivity)
{
    HotFaceCoatingProperties.Type = Coating;
    HotFaceCoatingProperties.Emmissivity = Emmissivity;
    HotFaceCoatingProperties.Absorptivity = Absorptivity;

//    //set if it is active or passive control, mass will be changed according to this
//    if (HotFaceCoatingProperties.Type == "User_Defined")
//    {
//        if (ThermalControl == Active)
//            ThermalControl = PassiveAndActive;
//        else
//            ThermalControl = Passive;
//    }
//    else
//        ThermalControl = NoControl;
}

CoatingProperties ThermalSubsystem::getHotFaceCoatingProperties()
{
    return HotFaceCoatingProperties;
}

void ThermalSubsystem::CalculateAndSetSolarFluxHeat()
{
    // cos 23.5 degrees
    SolarFluxHeat = PlanetThermalProperties.SolarConstantOfPlanet
                    * AreaOfHotFace
                    * cos(23.5 * DEG2RAD)
                    * HotFaceCoatingProperties.Absorptivity;
}
double ThermalSubsystem::getSolarFluxHeat()
{
    return SolarFluxHeat;
}

void ThermalSubsystem::CalculateAndSetAlbedoHeat()
{
    AlbedoHeat = PlanetThermalProperties.SolarConstantOfPlanet
                 * PlanetThermalProperties.AlbedoConstantOfPlanet
                 * AreaOfHotFace
                 * PlanetThermalProperties.CollimatedSolarEnergyOfPlanet
                 * pow(sin(PlanetThermalProperties.PlanetAngularRadius),2);
}
double ThermalSubsystem::getAlbedoHeat()
{
    return AlbedoHeat;
}

void ThermalSubsystem::CalculateAndSetPlanetIRHeat()
{
    ReceivedPlanetIRHeat = PlanetThermalProperties.PlanetIREnergy
                           * pow(sin(PlanetThermalProperties.PlanetAngularRadius),2)
                           * AreaOfHotFace
                           * HotFaceCoatingProperties.Emmissivity;
}
double ThermalSubsystem::getPlanetIRHeat()
{
    return ReceivedPlanetIRHeat;
}

void ThermalSubsystem::setMinimumSCElectronicsHeatInEclipse(double SCElectronicsHeat)
{
    MinimumSCElectronicsHeatInEclipse = SCElectronicsHeat;
}
double ThermalSubsystem::getMinimumSCElectronicsHeatInEclipse()
{
    return  MinimumSCElectronicsHeatInEclipse;
}

void ThermalSubsystem::setMaximumSCElectronicsHeatInDaylight(double SCElectronicsHeat)
{
     MaximumSCElectronicsHeatInDaylight = SCElectronicsHeat;
}
double ThermalSubsystem::getMaximumSCElectronicsHeatInDaylight()
{
    return  MaximumSCElectronicsHeatInDaylight;
}

void ThermalSubsystem::setMinimumSCElectronicsHeatInDaylight(double SCElectronicsHeat)
{
    MinimumSCElectronicsHeatInDaylight = SCElectronicsHeat;
}
double ThermalSubsystem::getMinimumSCElectronicsHeatInDaylight()
{
    return  MinimumSCElectronicsHeatInDaylight;
}

void ThermalSubsystem::setMaximumSCElectronicsHeatInEclipse(double SCElectronicsHeat)
{
     MaximumSCElectronicsHeatInEclipse = SCElectronicsHeat;
}
double ThermalSubsystem::getMaximumSCElectronicsHeatInEclipse()
{
    return  MaximumSCElectronicsHeatInEclipse;
}

void ThermalSubsystem::CalculateAndSetSCTemperatureRange()
{
    //to make sure that following necessary properties are calculated
    CalculateAndSetPlanetIRHeat();

    double tempMinimumSCTemperature = 0.0;
    tempMinimumSCTemperature = pow((
            (ReceivedPlanetIRHeat + MinimumSCElectronicsHeatInEclipse)
            / (TotalAreaOfColdFace
                * STEFAN_BOLTZMANN_CONSTANT
                * ColdFaceCoatingProperties.Emmissivity
               + TotalAreaOfHotFace
                * STEFAN_BOLTZMANN_CONSTANT
                * HotFaceCoatingProperties.Emmissivity))
                               ,0.25);

    //to make sure that following necessary properties are calculated
    CalculateAndSetSolarFluxHeat();
    CalculateAndSetAlbedoHeat();

    double tempMaximumSCTemperature = 0.0;
    tempMaximumSCTemperature = pow((
            (ReceivedPlanetIRHeat
             + SolarFluxHeat
             + AlbedoHeat
             + MaximumSCElectronicsHeatInDaylight)
            / (TotalAreaOfColdFace
               * STEFAN_BOLTZMANN_CONSTANT
               * ColdFaceCoatingProperties.Emmissivity
               +
               TotalAreaOfHotFace
               * STEFAN_BOLTZMANN_CONSTANT
               * HotFaceCoatingProperties.Emmissivity))
                               ,0.25);

    if (tempMaximumSCTemperature >= tempMinimumSCTemperature)
    {
        MaximumSCTemperature = tempMaximumSCTemperature ;
        MinimumSCTemperature = tempMinimumSCTemperature;

        //temperature with the efect of Heater and radiator
        MinimumSCTempWithRadiatorOrHeater = pow((
                (ReceivedPlanetIRHeat
                 + MinimumSCElectronicsHeatInEclipse
                 + NeededHeater
                 - NeededRadiator)
                    / (TotalAreaOfColdFace
                       * STEFAN_BOLTZMANN_CONSTANT
                       * ColdFaceCoatingProperties.Emmissivity
                           + TotalAreaOfHotFace
                               * STEFAN_BOLTZMANN_CONSTANT
                               * HotFaceCoatingProperties.Emmissivity))
                                                ,0.25);

        MaximumSCTempWithRadiatorOrHeater = pow((
                (ReceivedPlanetIRHeat
                 + SolarFluxHeat
                 + AlbedoHeat
                 + MaximumSCElectronicsHeatInDaylight
                 + NeededHeater
                 - NeededRadiator)
                    / (TotalAreaOfColdFace
                       * STEFAN_BOLTZMANN_CONSTANT
                       * ColdFaceCoatingProperties.Emmissivity
                           + TotalAreaOfHotFace
                               * STEFAN_BOLTZMANN_CONSTANT
                               * HotFaceCoatingProperties.Emmissivity))
                                                ,0.25);
    }
    else
    {
        MaximumSCTemperature = pow((
                    (ReceivedPlanetIRHeat + MaximumSCElectronicsHeatInEclipse)
                    / (TotalAreaOfColdFace
                        * STEFAN_BOLTZMANN_CONSTANT
                        * ColdFaceCoatingProperties.Emmissivity
                       + TotalAreaOfHotFace
                        * STEFAN_BOLTZMANN_CONSTANT
                        * HotFaceCoatingProperties.Emmissivity))
                                       ,0.25);
        MinimumSCTemperature = pow((
                    (ReceivedPlanetIRHeat
                     + SolarFluxHeat
                     + AlbedoHeat
                     + MinimumSCElectronicsHeatInDaylight)
                    / (TotalAreaOfColdFace
                       * STEFAN_BOLTZMANN_CONSTANT
                       * ColdFaceCoatingProperties.Emmissivity
                       +
                       TotalAreaOfHotFace
                       * STEFAN_BOLTZMANN_CONSTANT
                       * HotFaceCoatingProperties.Emmissivity))
                                       ,0.25);

        //temperature with the efect of Heater and radiator
        MaximumSCTempWithRadiatorOrHeater = pow((
                (ReceivedPlanetIRHeat
                 + MaximumSCElectronicsHeatInEclipse
                 + NeededHeater
                 - NeededRadiator)
                    / (TotalAreaOfColdFace
                       * STEFAN_BOLTZMANN_CONSTANT
                       * ColdFaceCoatingProperties.Emmissivity
                           + TotalAreaOfHotFace
                               * STEFAN_BOLTZMANN_CONSTANT
                               * HotFaceCoatingProperties.Emmissivity))
                                                ,0.25);

        MinimumSCTempWithRadiatorOrHeater = pow((
                (ReceivedPlanetIRHeat
                 + SolarFluxHeat
                 + AlbedoHeat
                 + MinimumSCElectronicsHeatInDaylight
                 + NeededHeater
                 - NeededRadiator)
                    / (TotalAreaOfColdFace
                       * STEFAN_BOLTZMANN_CONSTANT
                       * ColdFaceCoatingProperties.Emmissivity
                           + TotalAreaOfHotFace
                               * STEFAN_BOLTZMANN_CONSTANT
                               * HotFaceCoatingProperties.Emmissivity))
                                                ,0.25);
    }


    qDebug()<<"MinimumSCTemperature"<<MinimumSCTemperature;
    qDebug()<<"ReceivedPlanetIRHeat"<<ReceivedPlanetIRHeat;
    qDebug()<<"TotalAreaOfColdFace"<<TotalAreaOfColdFace;
    qDebug()<<"TotalAreaOfHotFace "<<TotalAreaOfHotFace ;
}

void ThermalSubsystem::setMinimumSCTemperature(double temperature)
{
    MinimumSCTemperature = temperature;
}

double ThermalSubsystem::getMinimumSCTemperature()
{
    return MinimumSCTemperature;
}

//void ThermalSubsystem::CalculateAndSetMaximumSCTemperature()
//{
//    //to make sure that following necessary properties are calculated
//    CalculateAndSetPlanetIRHeat();
//    CalculateAndSetSolarFluxHeat();
//    CalculateAndSetAlbedoHeat();
//
//    MaximumSCTemperature = pow((
//            (ReceivedPlanetIRHeat
//             + SolarFluxHeat
//             + AlbedoHeat
//             + MaximumSCElectronicsHeatInDaylight)
//
//               / (TotalAreaOfColdFace
//                  * STEFAN_BOLTZMANN_CONSTANT
//                  * ColdFaceCoatingProperties.Emmissivity
//                    +
//                    TotalAreaOfHotFace
//                  * STEFAN_BOLTZMANN_CONSTANT
//                  * HotFaceCoatingProperties.Emmissivity))
//                               ,0.25);
//}

void ThermalSubsystem::setMaximumSCTemperature(double temperature)
{
    MaximumSCTemperature = temperature;
}

double ThermalSubsystem::getMaximumSCTemperature()
{
    return MaximumSCTemperature;
}

void ThermalSubsystem::setMaximumSCTempWithRadiatorOrHeater(double temp)
{
    MaximumSCTempWithRadiatorOrHeater = temp;
}

void ThermalSubsystem::setMinimumSCTempWithRadiatorOrHeater(double temp)
{
    MinimumSCTempWithRadiatorOrHeater = temp;
}

double ThermalSubsystem::getMaximumSCTempWithRadiatorOrHeater()
{
    return MaximumSCTempWithRadiatorOrHeater;
}

double ThermalSubsystem::getMinimumSCTempWithRadiatorOrHeater()
{
    return MinimumSCTempWithRadiatorOrHeater;
}

void ThermalSubsystem::setMinimumPayloadTemperature(int Index,
                                                    QString Name,
                                                    double MinTemperature)
{
    PayloadThermalDetails[Index].Name = Name;
    PayloadThermalDetails[Index].MinimumTemperature = MinTemperature;
}

void ThermalSubsystem::setMaximumPayloadTemperature(int Index,
                                                    QString Name,
                                                    double MaxTemperature)
{
    PayloadThermalDetails[Index].Name = Name;
    PayloadThermalDetails[Index].MaximumTemperature = MaxTemperature;
}

PayloadThermalInfo * ThermalSubsystem::getPayloadTemperature()
{
    return PayloadThermalDetails;
}

void ThermalSubsystem::CalculateAndSetNeededHeater()
{
    //If there is a need of heater it is going to work full power all period
    // of time if the maximum SCtemp also below the desired value
    if (MaximumSCTemperature < DesiredMinimumTemperature)
    {
        NeededHeater = TotalAreaOfHotFace
                       * STEFAN_BOLTZMANN_CONSTANT
                       * HotFaceCoatingProperties.Emmissivity
                       * (pow(DesiredMaximumTemperature,4)- pow(MaximumSCTemperature,4))
                         +
                         TotalAreaOfColdFace
                       * STEFAN_BOLTZMANN_CONSTANT
                       * ColdFaceCoatingProperties.Emmissivity
                       * (pow(DesiredMaximumTemperature,4) - pow(MaximumSCTemperature,4));

//        CalculateAndSetMaximumSCTemperature();
    }

    if (MinimumSCTemperature < DesiredMinimumTemperature)
    {
        NeededHeater = TotalAreaOfHotFace
                       * STEFAN_BOLTZMANN_CONSTANT
                       * HotFaceCoatingProperties.Emmissivity
                       * (pow(DesiredMinimumTemperature,4)- pow(MinimumSCTemperature,4))
                         +
                         TotalAreaOfColdFace
                       * STEFAN_BOLTZMANN_CONSTANT
                       * ColdFaceCoatingProperties.Emmissivity
                       * (pow(DesiredMinimumTemperature,4) - pow(MinimumSCTemperature,4));

//        CalculateAndSetMinimumSCTemperature();
    }    

    CalculateAndSetSCTemperatureRange();

//    //set if it is active or passive control, mass will be changed according to this
//    if (NeededHeater > 0.0)
//    {
//        if (ThermalControl == Passive)
//            ThermalControl = PassiveAndActive;
//        else
//            ThermalControl = Active;
//    }

    ThermalSubsystemPower = NeededHeater;
}

void ThermalSubsystem::setNeededHeaterToZero()
{
    NeededHeater = 0.0;
    ThermalSubsystemPower = NeededHeater;
}

void ThermalSubsystem::setNeededHeater(double heater)
{
    NeededHeater = heater;
    ThermalSubsystemPower = NeededHeater;
}

double ThermalSubsystem::getNeededHeater()
{
    return NeededHeater;
}

void ThermalSubsystem::CreateTemperatureTimeFunction()
{
    //reach to the Eclipse information file------------
    QString path = QString("data/EclipseStarLight.stad");

    QFile EclipseStarLight(path);
    EclipseStarLight.open(QIODevice::ReadOnly);
    //       qDebug()<<EclipseStarLight.fileName();
    //       qDebug()<<EclipseStarLight.isOpen();
    QTextStream EclipseStarLightStream(&EclipseStarLight);

    //open the consumed power file -----------------------------
    QString path2 = QString
                   ("data/SystemsEngineeringReports/PowerConsumptionTimeFunction.stad");

    QFile ConsumedPowerTime(path2);
    ConsumedPowerTime.open(QIODevice::ReadOnly);
//           qDebug()<<"ConsumedPowerTime.fileName()"<<ConsumedPowerTime.fileName();
//           qDebug()<<"ConsumedPowerTime.isOpen()"<<ConsumedPowerTime.isOpen();
    QTextStream ConsumedPowerTimeStream(&ConsumedPowerTime);

    //open the file you want to put your results-----------------------------
    QString path3 = QString
                   ("data/SystemsEngineeringReports/SCTemperatureTimeFunction.stad");

    QFile TemperatureTime(path3);
    TemperatureTime.open(QIODevice::ReadWrite);
           qDebug()<<"TemperatureTime.fileName()"<<TemperatureTime.fileName();
           qDebug()<<"TemperatureTime.isOpen()"<<TemperatureTime.isOpen();
    QTextStream TemperatureTimeStream(&TemperatureTime);
    TemperatureTimeStream.setRealNumberPrecision(16);

    double mjd = 0.0;
    double powerConsumption = 0.0;
    double eclipseState = 0.0;
    double temp = 0.0;

    ConsumedPowerTimeStream >> mjd;
    ConsumedPowerTimeStream >> powerConsumption;

    EclipseStarLightStream >> mjd;
    EclipseStarLightStream >> eclipseState;

    //calculate the temperature at a time
    while((!ConsumedPowerTimeStream.atEnd()&&(!EclipseStarLightStream.atEnd())))
    {       
        temp = pow(
                ((ReceivedPlanetIRHeat
                  + (SolarFluxHeat+ AlbedoHeat)*eclipseState
                  + powerConsumption)
                 /
                 (TotalAreaOfHotFace
                    * STEFAN_BOLTZMANN_CONSTANT
                    * HotFaceCoatingProperties.Emmissivity
                  + TotalAreaOfColdFace
                    * STEFAN_BOLTZMANN_CONSTANT
                    * ColdFaceCoatingProperties.Emmissivity)
                        ),0.25);

        TemperatureTimeStream << mjd<<"\t";
        TemperatureTimeStream << temp<<"\n";

        ConsumedPowerTimeStream >> mjd;
        ConsumedPowerTimeStream >> powerConsumption;

        EclipseStarLightStream >> mjd;
        EclipseStarLightStream >> eclipseState;
    }

    //close the files you opened
    EclipseStarLight.close();
    ConsumedPowerTime.close();
    TemperatureTime.close();
}

void ThermalSubsystem::CalculateAndSetNeededRadiator()
{
//    double tempHeat;

    if (MinimumSCTemperature > DesiredMaximumTemperature)
    {
        NeededRadiator = TotalAreaOfHotFace
                       * STEFAN_BOLTZMANN_CONSTANT
                       * HotFaceCoatingProperties.Emmissivity
                       * (pow(MinimumSCTemperature,4) - pow(DesiredMinimumTemperature,4))
                         +
                         TotalAreaOfColdFace
                       * STEFAN_BOLTZMANN_CONSTANT
                       * ColdFaceCoatingProperties.Emmissivity
                       * (pow(MinimumSCTemperature,4) - pow(DesiredMinimumTemperature,4));

//        tempHeat = getMinimumSCElectronicsHeatInEclipse();
//        setMinimumSCElectronicsHeatInEclipse(tempHeat - NeededRadiator);

//        CalculateAndSetMinimumSCTemperature();
    }

    if (MaximumSCTemperature > DesiredMaximumTemperature)
    {
        NeededRadiator = TotalAreaOfHotFace
                       * STEFAN_BOLTZMANN_CONSTANT
                       * HotFaceCoatingProperties.Emmissivity
                       * (pow(MaximumSCTemperature,4) - pow(DesiredMaximumTemperature,4))
                         +
                         TotalAreaOfColdFace
                       * STEFAN_BOLTZMANN_CONSTANT
                       * ColdFaceCoatingProperties.Emmissivity
                       * (pow(MaximumSCTemperature,4) - pow(DesiredMaximumTemperature,4));

//        tempHeat = getMaximumSCElectronicsHeatInDaylight();
//        setMaximumSCElectronicsHeatInDaylight(tempHeat - NeededRadiator);

//        CalculateAndSetMaximumSCTemperature();
    }

//    //set if it is active or passive control, mass will be changed according to this
//    if (NeededRadiator > 0.0)
//    {
//        if (ThermalControl == Passive)
//            ThermalControl = PassiveAndActive;
//        else
//            ThermalControl = Active;
//    }

    CalculateAndSetSCTemperatureRange();
}

void ThermalSubsystem::setNeededRadiatorToZero()
{
    NeededRadiator = 0.0;
}

void ThermalSubsystem::setNeededRadiator(double radiator)
{
    NeededRadiator = radiator;
}

double ThermalSubsystem::getNeededRadiator()
{
    return NeededRadiator;
}

bool ThermalSubsystem::IsPayloadInAlert(int Index)
{
    if (PayloadThermalDetails[Index].Name == "")
    {
        return false;
    }
    else
    {
        if ( (PayloadThermalDetails[Index].MaximumTemperature
                < MaximumSCTempWithRadiatorOrHeater)||
             (PayloadThermalDetails[Index].MinimumTemperature
                > MinimumSCTempWithRadiatorOrHeater) )
        {
            PayloadThermalDetails[Index].Alert = true;
        }
        else PayloadThermalDetails[Index].Alert = false;

        return PayloadThermalDetails[Index].Alert;
    }
}

void ThermalSubsystem::setAreaOfHotFace(double Area)
{
    AreaOfHotFace = Area;
}

double ThermalSubsystem::getAreaOfHotFace()
{
    return AreaOfHotFace;
}

void ThermalSubsystem::setAreaOfColdFace(double Area)
{
    AreaOfColdFace = Area;
}

double ThermalSubsystem::getAreaOfColdFace()
{
    return AreaOfColdFace;
}

void ThermalSubsystem::setTotalAreaOfHotFace(double Area)
{
    TotalAreaOfHotFace = Area;
}

double ThermalSubsystem::getTotalAreaOfHotFace()
{
    return TotalAreaOfHotFace;
}

void ThermalSubsystem::setTotalAreaOfColdFace(double Area)
{
    TotalAreaOfColdFace = Area;
}

double ThermalSubsystem::getTotalAreaOfColdFace()
{
    return TotalAreaOfColdFace;
}

void ThermalSubsystem::setSCStructuralProperties(QString Name,
                                                   double Emmissivity,
                                                   double Absorptivity)
{
    SCStructuralProperties.Name = Name;
    SCStructuralProperties.Emmissivity = Emmissivity;
    SCStructuralProperties.Absorptivity = Absorptivity;
}

SCStructure * ThermalSubsystem::getSCStructuralProperties()
{
    return &SCStructuralProperties;
}

void ThermalSubsystem::setThermalSubsystemMass(double PayloadMass)
{
    //initialize
    ThermalControl = NoControl;

    qDebug()<<"HotFaceCoatingProperties.Type" <<HotFaceCoatingProperties.Type;
    qDebug()<<"*************---ColdFaceCoatingProperties.Type" <<ColdFaceCoatingProperties.Type;

//    qDebug()<<(!(HotFaceCoatingProperties.Type == "No_Coating"));
//    qDebug()<< (!(HotFaceCoatingProperties.Type == "UNDEFINED"));
//    qDebug()<< (!(HotFaceCoatingProperties.Type == ""));
//    qDebug()<<(!(ColdFaceCoatingProperties.Type == "No_Coating"));
//    qDebug()<< (!(ColdFaceCoatingProperties.Type == "UNDEFINED"));
//    qDebug()<< (!(ColdFaceCoatingProperties.Type == "")        ) ;

    //set if it is active or passive control, mass will be changed according to this
    //check if it is passive
    if ((!(HotFaceCoatingProperties.Type == "No_Coating")
        && !(HotFaceCoatingProperties.Type == "UNDEFINED")
        && !(HotFaceCoatingProperties.Type == ""))
            ||
        (!(ColdFaceCoatingProperties.Type == "No_Coating")
        && !(ColdFaceCoatingProperties.Type == "UNDEFINED")
        && !(ColdFaceCoatingProperties.Type == "")        ) )
    {
            ThermalControl = Passive;            
    }
    qDebug()<<"ThermalControl" << ThermalControl;

//check if it is active
    if ((NeededRadiator > 0.0) || (NeededHeater > 0.0))
    {
        if (ThermalControl == Passive){
            ThermalControl = PassiveAndActive;
            qDebug()<<"ThermalControl" << ThermalControl;}
        else
            ThermalControl = Active;
        qDebug()<<"ThermalControl" << ThermalControl;
    }

    if ((ThermalControl != Passive)
        && (ThermalControl != Active)
        && (ThermalControl != PassiveAndActive))
        ThermalControl = NoControl;
    qDebug()<<"ThermalControl" << ThermalControl;

    // aproximated SC mass - > (PayloadMass + 2.84965) / 0.2582
    switch (ThermalControl)
    {
    case Passive:
                ThermalSubsystemMass
                        = 0.0208 * (PayloadMass + 2.84965) / 0.2582
                          + 0.1826;
                break;
    case Active:
                ThermalSubsystemMass
                        = 0.0195 * (PayloadMass + 2.84965) / 0.2582
                          + 1.486;
                break;
    case PassiveAndActive:
                ThermalSubsystemMass
                        = (0.0208 * (PayloadMass + 2.84965) / 0.2582
                             + 0.1826)
                          +(0.0195 * (PayloadMass + 2.84965) / 0.2582
                             + 1.486);
                break;
    default:
                ThermalSubsystemMass = 0.0;
                break;
    }
    qDebug()<<"ThermalSubsystemMass"<< ThermalSubsystemMass;
    qDebug()<<"THermalcontrol"<<ThermalControl;
}

double ThermalSubsystem::getThermalSubsystemMass()
{
    return ThermalSubsystemMass;
}

void ThermalSubsystem::setThermalSubsystemVolume(double Volume)
{
    ThermalSubsystemVolume = Volume;
}

double ThermalSubsystem::getThermalSubsystemVolume()
{
    return ThermalSubsystemVolume ;
}

void ThermalSubsystem::setThermalSubsystemPower(double Power)
{
    ThermalSubsystemPower = Power;
}

double ThermalSubsystem::getThermalSubsystemPower()
{
    return ThermalSubsystemPower;
}


