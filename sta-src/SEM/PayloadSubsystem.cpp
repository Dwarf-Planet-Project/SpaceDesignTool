#include "PayloadSubsystem.h"

#include "Astro-Core/constants.h"
#include "math.h"

//to Read and write to file
#include <QFile>
#include <QTextStream>
#include <QDir>
#include <QProcess>

//----------------- Constructor -------------------------------------//
PayloadSubsystem::PayloadSubsystem()
{
    PayloadName = "";
    PayloadVolume = 0;
    PayloadDimensions.setZero(3);
    PayloadMass = 0;
    PayloadPowerInEclipse = 0;
    PayloadPowerInDaylight = 0;
    PayloadPowerOnPercentageInEclipse = 0;
    PayloadPowerOnPercentageInDaylight = 0;
    PayloadMinimumTemperature = 0;
    PayloadMaximumTemperature = 0;
    PayloadDataRate = 0;


    //**************
    //setSelectedPayloadProperties("SAR");
}

//----------------- Functions ---------------------------------------//
void PayloadSubsystem::setPayloadName(QString Name)
{
    PayloadName = Name;
}

QString PayloadSubsystem::getPayloadName()
{
    return PayloadName;
}

void PayloadSubsystem::setPayloadWavelength(double Wavelength)
{
    PayloadWavelength = Wavelength;
}

double PayloadSubsystem::getPayloadWavelength()
{
    return PayloadWavelength;
}

void PayloadSubsystem::CalculateAndSetPayloadVolume(QString Shape)
{
    if (Shape == "Rectangular_Prism")
    {PayloadVolume =  PayloadDimensions.x()   //width
                         * PayloadDimensions.y() //Height
                         * PayloadDimensions.z();//Length
        qDebug()<<"PayloadVOLUME"<<PayloadVolume;
    }
    else
    {if(Shape == "Cylinder")
        {PayloadVolume =  mypi
                         * pow(PayloadDimensions.x(),2)   //diameter
                         * PayloadDimensions.y(); //Height
        qDebug()<<"PayloadVOLUME"<<PayloadVolume;
        }
        else
        { if (Shape == "Sphere")
            {PayloadVolume = 4/3
                            * mypi
                            * pow(PayloadDimensions.x(),3)   //diameter
                            / 8;
            qDebug()<<"PayloadVOLUME"<<PayloadVolume;
            }
            else
            {PayloadVolume = 0.0;
            qDebug()<<"PayloadVOLUME"<<PayloadVolume;
            }
        }
    }
    qDebug()<<"PayloadVOLUME--FINAL"<<PayloadVolume;
}

double PayloadSubsystem::getPayloadVolume()
{
    return PayloadVolume;
}

void PayloadSubsystem::setPayloadWidth(double Width)
{
    PayloadDimensions.x() = Width;
}

double PayloadSubsystem::getPayloadWidth()
{
    return PayloadDimensions.x();
}

void PayloadSubsystem::setPayloadHeight(double Height)
{
    PayloadDimensions.y() = Height;
}

double PayloadSubsystem::getPayloadHeight()
{
    return PayloadDimensions.y();
}

void PayloadSubsystem::setPayloadLength(double Length)
{
    PayloadDimensions.z() = Length;
}

double PayloadSubsystem::getPayloadLength()
{
    return PayloadDimensions.z();
}

void PayloadSubsystem::setPayloadDimensions(double Width,
                                            double Height,
                                            double Length)
{
    PayloadDimensions.x()= Width;
    PayloadDimensions.y()= Height;
    PayloadDimensions.z()= Length;
}

Vector3d PayloadSubsystem::getPayloadDimensions()
{
    return PayloadDimensions;
}

void PayloadSubsystem::setPayloadMass(double Mass)
{
    PayloadMass = Mass;
    qDebug()<<"PayloadMASSsubsystem"<<PayloadMass;
}

double PayloadSubsystem::getPayloadMass()
{
    return PayloadMass;
}

void PayloadSubsystem::setPayloadPowerInEclipse(double Power)
{
    PayloadPowerInEclipse = Power;
}

double PayloadSubsystem::getPayloadPowerInEclipse()
{
    return PayloadPowerInEclipse;
}

void PayloadSubsystem::setPayloadPowerInDaylight(double Power)
{
    PayloadPowerInDaylight = Power;
}

double PayloadSubsystem::getPayloadPowerInDaylight()
{
    return PayloadPowerInDaylight;
}

void PayloadSubsystem::setPayloadPowerOnPercentageInEclipse
        (double PowerOnPercentageInEclipse)
{
    PayloadPowerOnPercentageInEclipse = PowerOnPercentageInEclipse;
}

double PayloadSubsystem::getPayloadPowerOnPercentageInEclipse()
{
    return PayloadPowerOnPercentageInEclipse;
}

void PayloadSubsystem::setPayloadPowerOnPercentageInDaylight
        (double PowerOnPercentageInDaylight)
{
    PayloadPowerOnPercentageInDaylight = PowerOnPercentageInDaylight;
}

double PayloadSubsystem::getPayloadPowerOnPercentageInDaylight()
{
    return PayloadPowerOnPercentageInDaylight;
}

void PayloadSubsystem::setPayloadMinimumTemperature(double MinimumTemperature)
{
    PayloadMinimumTemperature = MinimumTemperature;
}

double PayloadSubsystem::getPayloadMinimumTemperature()
{
    return PayloadMinimumTemperature;
}

void PayloadSubsystem::setPayloadMaximumTemperature(double MaximumTemperature)
{
    PayloadMaximumTemperature = MaximumTemperature;
}

double PayloadSubsystem::getPayloadMaximumTemperature()
{
    return PayloadMaximumTemperature;
}

void PayloadSubsystem::setPayloadDataRate(double DataRate)
{
    PayloadDataRate = DataRate;
}

double PayloadSubsystem::getPayloadDataRate()
{
    return PayloadDataRate;
}

// WIZARD PAYLOAD----------------------------------------------------
void PayloadSubsystem::setSelectedPayloadProperties(QString PName)
{
    QString path = QString("data/Estimations/PayloadSet.stad");

       QFile PayloadSetProperties(path);
       PayloadSetProperties.open(QIODevice::ReadOnly);
//       qDebug()<<PayloadSetProperties.fileName();
//       qDebug()<<PayloadSetProperties.isOpen();
       QTextStream PayloadSetPropertiesStream(&PayloadSetProperties);

       //to skip the first descriptive line of the "PayloadSet.stad"
       QString InfosOfStadFile;
       int NumberOfColumns = 13;
       int i;
       for (i=0;i<NumberOfColumns;i++)
       {
           PayloadSetPropertiesStream >> InfosOfStadFile;
//           qDebug() << InfosOfStadFile;
       }

       //find the values from the database
       QString CName = "";
       double CWidth;
       double CHeight;
       double CLength;
       double CMass;
       double CDataRate;
       double CPower;
       double CPowerOnEclipse;
       double CPowerOnDaylight;
       QString CSpectrumRange;
       double CWavelength;
       double CMinimumTemp;
       double CMaximumTemp;

       do
       {
           PayloadSetPropertiesStream >> CName;
           qDebug()<<"NAME"<< CName;
           PayloadSetPropertiesStream >> CWidth;
           qDebug()<< CWidth;
           PayloadSetPropertiesStream >> CHeight;
           qDebug()<< CHeight;
           PayloadSetPropertiesStream >> CLength;
           qDebug()<< CLength;
           PayloadSetPropertiesStream >> CMass;
           qDebug()<< CMass;
           PayloadSetPropertiesStream >> CDataRate;
           qDebug()<< CDataRate;
           PayloadSetPropertiesStream >> CPower;
           qDebug()<< CPower;
           PayloadSetPropertiesStream >> CPowerOnEclipse;
           qDebug()<< CPowerOnEclipse;
           PayloadSetPropertiesStream >> CPowerOnDaylight;
           qDebug()<< CPowerOnDaylight;
           PayloadSetPropertiesStream >> CSpectrumRange;
           qDebug()<< CSpectrumRange;
           PayloadSetPropertiesStream >> CWavelength;
           qDebug()<< CWavelength;
           PayloadSetPropertiesStream >> CMinimumTemp;
           qDebug()<< CMinimumTemp;
           PayloadSetPropertiesStream >> CMaximumTemp;
           qDebug()<< CMaximumTemp;

           qDebug()<< "while - " <<((!(CName==PName))&&(!(CName=="UNDEFINED")));
       }
       while ((!(CName==PName))&&(!(CName=="UNDEFINED")));

       // set the values from the database----------------------------------

       setPayloadName(CName);
       qDebug()<<"SCPayload" <<CName;
       qDebug()<<"enquiry" <<PName;

       setPayloadWavelength(CWavelength);

       setPayloadDimensions(CWidth, CHeight, CLength);

       if (PayloadDimensions.z() > 0.0)
           CalculateAndSetPayloadVolume("Rectangular_Prism");
       else
       {
           if (PayloadDimensions.y() > 0.0)
               CalculateAndSetPayloadVolume("Cylinder");
           else
           {
               if (PayloadDimensions.x() > 0.0)
                   CalculateAndSetPayloadVolume("Sphere");
               else
                   CalculateAndSetPayloadVolume("Undefined");
           }
       }

       setPayloadMass(CMass);
       setPayloadPowerInEclipse(CPower);

       setPayloadPowerInDaylight(CPower);

       setPayloadPowerOnPercentageInEclipse
               (CPowerOnEclipse);

       setPayloadPowerOnPercentageInDaylight
               (CPowerOnDaylight);

       setPayloadMinimumTemperature(CMinimumTemp);

       setPayloadMaximumTemperature(CMaximumTemp);

       setPayloadDataRate(CDataRate);

       PayloadSetProperties.close();

}

void PayloadSubsystem::CalculateNewDimensions(double wavelength,
                                              double angularResolution)
{
    double ApetureDiameter = 1.22
                             * wavelength
                             / angularResolution;

    ScalingFactor = ApetureDiameter / PayloadDimensions.x();

    PayloadDimensions.x() = ScalingFactor * PayloadDimensions.x();
    PayloadDimensions.y() = ScalingFactor * PayloadDimensions.y();
    PayloadDimensions.z() = ScalingFactor * PayloadDimensions.z();

    PayloadVolume =  mypi
                     * pow(PayloadDimensions.x(),2)   //diameter
                     * PayloadDimensions.y(); //Height

    double K = 0.0;

    // SMADs rule
    if (ScalingFactor < 0.5)
        K = 2.0;
    else
        K = 1.0;

    PayloadMass = K * pow(ScalingFactor,3.0) * PayloadMass;
    PayloadPowerInEclipse = K
                            * pow(ScalingFactor,3.0)
                            * PayloadPowerInEclipse;
    PayloadPowerInDaylight = K
                             * pow(ScalingFactor,3.0)
                             * PayloadPowerInDaylight;

    /*
       * Following stays as the database
       * PayloadPowerOnPercentageInEclipse
       * PayloadPowerOnPercentageInDaylight;
       * PayloadMinimumTemperature;
       * PayloadMaximumTemperature;
       * PayloadDataRate; <- only this one can be change by another method
      */
}

void PayloadSubsystem::CalculateNewDimensions(double wavelength,
                                              double linearResolution,
                                              double altitude)
{
    double ApetureDiameter = 2.44
                             * altitude
                             * wavelength
                             / linearResolution;

    ScalingFactor = ApetureDiameter / PayloadDimensions.x();

    PayloadDimensions.x() = ScalingFactor * PayloadDimensions.x();
    PayloadDimensions.y() = ScalingFactor * PayloadDimensions.y();
    PayloadDimensions.z() = ScalingFactor * PayloadDimensions.z();

    PayloadVolume =  mypi
                     * pow(PayloadDimensions.x(),2)   //diameter
                     * PayloadDimensions.y(); //Height

    double K = 0.0;

    // SMADs rule
    if (ScalingFactor < 0.5)
        K = 2.0;
    else
        K = 1.0;

    PayloadMass = K * pow(ScalingFactor,3.0) * PayloadMass;
    PayloadPowerInEclipse = K
                            * pow(ScalingFactor,3.0)
                            * PayloadPowerInEclipse;
    PayloadPowerInDaylight = K
                             * pow(ScalingFactor,3.0)
                             * PayloadPowerInDaylight;

    /*
       * Following stays as the database
       * PayloadPowerOnPercentageInEclipse
       * PayloadPowerOnPercentageInDaylight;
       * PayloadMinimumTemperature;
       * PayloadMaximumTemperature;
       * PayloadDataRate; <- only this one can be change by another method
      */
}

void PayloadSubsystem::CalculateNewDataRate(double groundAverageVelocity,
                                            double linearResolution,
                                            double numberOfPixels)
{
    if (linearResolution > 0.0)
    {
        double numberOfBitsPerPixels = 10.0; // this is common for satelite images
        PayloadDataRate = numberOfPixels
                          * groundAverageVelocity / linearResolution
                          * numberOfBitsPerPixels;
    }
}
