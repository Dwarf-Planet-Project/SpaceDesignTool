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

#ifndef PAYLOADSUBSYSTEM_H
#define PAYLOADSUBSYSTEM_H

#include <QDebug>

#include "Eigen/Core"
#include <QString>

using namespace std;
using namespace Eigen;

//#define NumberOfPayloads 4

//------------------------- Structures ---------------------------------//


class PayloadSubsystem
{
public:
    //----------------- Constructor -------------------------------------//
    PayloadSubsystem();

    //----------------- Functions ---------------------------------------//
    void setPayloadName(QString Name);
    QString getPayloadName();

    //input/output in meters
    void setPayloadWavelength(double Wavelength);
    double getPayloadWavelength();

    /*
    * If PayloadDimensions are set, it calculates the volume of
    * "Rectangular_Prism","Cylinder","Sphere"
    */
    void CalculateAndSetPayloadVolume(QString Shape);
    double getPayloadVolume();

    void setPayloadWidth(double Width);
    double getPayloadWidth();

    void setPayloadHeight(double Height);
    double getPayloadHeight();

    void setPayloadLength(double Length);
    double getPayloadLength();

    void setPayloadDimensions(double Width, double Height, double Length);
    Vector3d getPayloadDimensions();

    void setPayloadMass(double Mass);
    double getPayloadMass();

    void setPayloadPowerInEclipse(double Power);
    double getPayloadPowerInEclipse();

    void setPayloadPowerInDaylight(double Power);
    double getPayloadPowerInDaylight();

    void setPayloadPowerOnPercentageInEclipse
            (double PowerOnPercentageInEclipse);
    double getPayloadPowerOnPercentageInEclipse();

    void setPayloadPowerOnPercentageInDaylight
            (double PowerOnPercentageInDaylight);
    double getPayloadPowerOnPercentageInDaylight();

    void setPayloadMinimumTemperature(double MinimumTemperature);
    double getPayloadMinimumTemperature();

    void setPayloadMaximumTemperature(double MaximumTemperature);
    double getPayloadMaximumTemperature();

    void setPayloadDataRate(double DataRate);
    double getPayloadDataRate();

    // CREATE NEW PAYLOAD----------------------------------------------------
    /**
      It puts the parameters from the database called
      "data/Estimations/PayloadSet.stad"
      @param PayloadName     has to be in the database otherwise the name
                             will be set to 'UNDEFINED' and the parameters to '0'
    */
    void setSelectedPayloadProperties(QString PayloadName);

    /*
      * Before these functions
      *  void setSelectedPayloadProperties(QString PayloadName);
      * has to be called. The user has to be careful to use the following
      * functions. They do not have the ability make decisions
    */
    void CalculateNewDimensions(double wavelength, double angularResolution);
    void CalculateNewDimensions(double wavelength,
                                double linearResolution,
                                double altitude);

    void CalculateNewDataRate(double groundAverageVelocity,
                              double linearResolution,
                              double numberOfPixels);


private:
    //----------------- Member objects ---------------------------------//
    QString PayloadName;
    double ScalingFactor;
    double PayloadWavelength;


    //to Structure Subsystem
    double PayloadVolume;
    /*
    * Vector concept is just used in order to not create a new type.
    * Although one can think like the SC dimensions are given on the
    * positive side of the axis, they only represent the dimensions
    * Always in meters
    * in terms of x->width/radius
    *             y->height
    *             z->length
    */
    Vector3d PayloadDimensions;
//    double PayloadWidth;
//    double PayloadHeight;
//    double PayloadLength;

    double PayloadMass;

    // to Power Subsystem
    double PayloadPowerInEclipse;
    double PayloadPowerInDaylight;

    // to Power-Thermal-OBDH Subsystem
    double PayloadPowerOnPercentageInEclipse;
    double PayloadPowerOnPercentageInDaylight;

    //to Thermal Subsystem
    double PayloadMinimumTemperature;
    double PayloadMaximumTemperature;

    //to OBDH Subsystem
    double PayloadDataRate;
};

#endif // PAYLOADSUBSYSTEM_H
