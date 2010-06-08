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

    //m
    void setPayloadWavelength(double Wavelength);
    double getPayloadWavelength();

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

    // CREATE NEW PAYLOAD
    void setSelectedPayloadProperties(QString PayloadName);

    /*
      * Before these functions
      *  void setSelectedPayloadProperties(QString PayloadName);
      * has to be called to understand what type of payload from the
      * list is going to be used
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
