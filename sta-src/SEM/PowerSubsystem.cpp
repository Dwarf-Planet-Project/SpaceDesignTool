#include "PowerSubsystem.h"
#include <math.h>

#include <QDebug>

#include <QDateTime>

//to Read and write to file
#include <QFile>
#include <QTextStream>
#include <QDir>
#include <QProcess>

#include "Astro-Core/constants.h"
#include "Astro-Core/date.h"


//----------------- Constructor -------------------------------------//
CSolarArray::CSolarArray()
{
    SolarCell.Name = "";
    SolarCell.ConversionEfficiency = 0.0;
    SolarCell.DegradationPerYear = 0.0;

    SAEOLPower = 0.0;
    SABOLPower = 0.0;
    SAMass = 0.0;
    PCUMass = 0.0;
    SAArea = 0.0;
    SolarCellBOLPower = 0.0;
    SolarCellEOLPower = 0.0;
    LifeTimeDegradation = 0.0;
    SystemPower = 0.0;

    SolarConstant = 0.0;

    EclipseDuration = 0.0;
    MissionDuration = 0.0;
    DaylightDuration = 0.0;
    PayloadWattHourInEclipse = 0.0;
    PayloadWattHourInDaylight = 0.0;

//    PowerSubsystemMass = 0.0;
//    PowerSubsystemDataRate = 0.0;
//    PowerSubsystemVolume = 0.0;


    qDebug()<<"SOLAR ARRAY CONSTRUCTOR ******";

}

//----------------- Functions ---------------------------------------//
void CSolarArray::setSolarCell(QString Name)
{
    QString path = QString("data/MaterialDatabase/SolarCellProperties.stad");

       QFile SolarCellProperties(path);
       SolarCellProperties.open(QIODevice::ReadOnly);
//       qDebug()<<SolarCellProperties.fileName();
//       qDebug()<<SolarCellProperties.isOpen();
       QTextStream SolarCellPropertiesStream(&SolarCellProperties);

       //to skip the first descriptive line of the "SolarCellProperties.stad"
       QString InfosOfStadFile;
       int NumberOfColumns = 3;
       int i;
       for (i=0;i<NumberOfColumns;i++)
       {
           SolarCellPropertiesStream >> InfosOfStadFile;
//           qDebug() << InfosOfStadFile;
       }

       //find the values from the database
       QString CName = "";
       double Efficiency;
       double Degradation;

       do
       {
           SolarCellPropertiesStream >> CName;
           qDebug()<< CName;
           SolarCellPropertiesStream >> Efficiency;
           qDebug()<< Efficiency;
           SolarCellPropertiesStream >> Degradation;
           qDebug()<< Degradation;

           qDebug()<< "while - " <<((!(CName==Name))&&(!(CName=="UNDEFINED")));
       }
       while ((!(CName==Name))&&(!(CName=="UNDEFINED")));

       // set the values from the database

       SolarCell.Name = CName;
       qDebug()<<SolarCell.Name;

       SolarCell.ConversionEfficiency = Efficiency;
       qDebug()<< SolarCell.ConversionEfficiency;

       SolarCell.DegradationPerYear = Degradation;
       qDebug()<<SolarCell.DegradationPerYear;

       SolarCellProperties.close();

       CalculateSolarCellBOLPower();
       CalculateSolarCellEOLPower();
}

SolarCellType CSolarArray::getSolarCell()
{
    return SolarCell;
}

void CSolarArray::CalculateSAEOLPower()
{
        double PathEfficiencyEclipse = 0.65;
        double PathEfficiencyDaylight = 0.85;

        CSolarArray::SAEOLPower =
                ((PayloadWattHourInEclipse * 3600 + SystemPower * EclipseDuration)
                  / PathEfficiencyEclipse
                   + (PayloadWattHourInDaylight * 3600 + SystemPower * DaylightDuration)
                  / PathEfficiencyDaylight)
                 / DaylightDuration;

        qDebug()<<"SAEOLPower"<<SAEOLPower;
        qDebug()<<"PayloadWattHourInEclipse"<<PayloadWattHourInEclipse;
        qDebug()<<"SystemPower"<<SystemPower;
        qDebug()<<"EclipseDuration"<<EclipseDuration;
        qDebug()<<"PayloadWattHourInDaylight"<<PayloadWattHourInDaylight;
        qDebug()<<"DaylightDuration"<<DaylightDuration;

//        CalculatePCUMass();
//        CalculateSAMass();
}

void CSolarArray::setSAEOLPower(double power)
{
    SAEOLPower = power;
}

double CSolarArray::getSAEOLPower()
{
    return SAEOLPower;
}

void CSolarArray::CalculateSABOLPower()
{
    SABOLPower = SAArea * SolarCellBOLPower;
}

void CSolarArray::setSABOLPower(double power)
{
    SABOLPower = power;
}

double CSolarArray::getSABOLPower()
{
    return SABOLPower;
}

void CSolarArray::CalculateSAMass()
{
    if (SAEOLPower > 0)
    SAMass = SAEOLPower * 0.04;
}

void CSolarArray::setSAMass(double mass)
{
    SAMass = mass;
}

double CSolarArray::getSAMass()
{
    CalculateSAMass();
    return SAMass;
}

double CSolarArray::getLifeTimeDegradation()
{
    return CSolarArray::LifeTimeDegradation;
}

void CSolarArray::CalculatePCUMass()
{
    if (SAEOLPower > 0)
    PCUMass = SAEOLPower * (0.02 + 0.025);
}

double CSolarArray::getPCUMass()
{
    CalculatePCUMass();
    return PCUMass;
}

void CSolarArray::CalculateSolarCellBOLPower()
{
    double InherentDegradation = 0.77;
    // Cos 23.5 degrees

    SolarCellBOLPower = (SolarCell.ConversionEfficiency
                         * SolarConstant
                         * InherentDegradation
                         * cos(23.5 * DEG2RAD)) ;

    //The BOL power Depends on SAARea and Solarcell BOL power
    CalculateSABOLPower();
}

double CSolarArray::getSolarCellBOLPower()
{
    return SolarCellBOLPower;
}

void CSolarArray::CalculateSolarCellEOLPower()
{
    LifeTimeDegradation = pow((1-SolarCell.DegradationPerYear),MissionDuration);
    SolarCellEOLPower = SolarCellBOLPower * LifeTimeDegradation;

    qDebug()<<"SolarCell.DegradationPerYear"<<SolarCell.DegradationPerYear;
    qDebug()<<"****************MissionDuration"<<MissionDuration;
    qDebug()<<"LifeTimeDegradation"<<LifeTimeDegradation;
    qDebug()<<"(1-SolarCell.DegradationPerYear)"<<(1-SolarCell.DegradationPerYear);
    qDebug()<<"pow((1-SolarCell.DegradationPerYear),MissionDuration)"<<pow((1-SolarCell.DegradationPerYear),MissionDuration);
    qDebug()<<"SolarCellBOLPower"<<SolarCellBOLPower;
    qDebug()<<"SolarCellEOLPower"<<SolarCellEOLPower;
}

double CSolarArray::getSolarCellEOLPower()
{
    return SolarCellEOLPower;
}

void CSolarArray::CalculateArea()
{
    if (SolarCellEOLPower > 0.0)
    SAArea = SAEOLPower / SolarCellEOLPower;

    //The BOL power Depends on SAARea and Solarcell BOL power
    CalculateSABOLPower();

    qDebug()<<"SAEOLPower"<<SAEOLPower;
    qDebug()<<"SolarCellEOLPower"<<SolarCellEOLPower;
    qDebug()<<"SAArea"<<SAArea;
    qDebug()<<"SolarCellBOLPower"<<SolarCellBOLPower;
    qDebug()<<"SABOLPower"<<SABOLPower;
}

void CSolarArray::setSAArea(double area)
{
    SAArea = area;
}

double CSolarArray::getArea()
{
    return SAArea;
}

void CSolarArray::setSolarConstant(double Constant)
{
    SolarConstant = Constant;
}

double CSolarArray::getSolarConstant()
{
    return SolarConstant;
}

void CSolarArray::setEclipseDuration(double Eclipse)
{
    EclipseDuration = Eclipse;
}

void CSolarArray::setDaylightDuration(double Daylight)
{
    DaylightDuration = Daylight;
}

void CSolarArray::setMissionDuration(double Duration)
{
    MissionDuration = Duration;
}

void CSolarArray::setPayloadWattHourInEclipse(double WattHour)
{
    PayloadWattHourInEclipse = WattHour;
}

void CSolarArray::setPayloadWattHourInDaylight(double WattHour)
{
    PayloadWattHourInDaylight = WattHour;
}

void CSolarArray::setSystemPower(double Power)
{
    SystemPower = Power;
    CalculateSAEOLPower();
}

//----------------- End of CSolarArray Class Declaration -----------------//
//----------------- Constructor -------------------------------------//
CBattery::CBattery()
{
    SCBattery.Name = "";
    SCBattery.SpecificEnergyDensity = 0.0;
    SCBattery.SpecificPower = 0.0;
    SCBattery.VolumeEnergyDensity = 0.0;

    BMass = 0.0;
    BVolume = 0.0;
    NumberOfBatteries = 0;

    EclipseDuration = 0.0;
    MissionDuration = 0.0;
    DaylightDuration = 0.0;
    PayloadWattHourInEclipse = 0.0;
    PayloadWattHourInDaylight = 0.0;

    qDebug()<<"BATTERY CONSTRUCTOR ******";
}

//----------------- Functions ---------------------------------------//
void CBattery::setBatteryProperties(QString Name)
{
    QString path = QString("data/MaterialDatabase/BatteryProperties.stad");

       QFile BatteryProperties(path);
       BatteryProperties.open(QIODevice::ReadOnly);
//       qDebug()<<BatteryProperties.fileName();
//       qDebug()<<BatteryProperties.isOpen();
       QTextStream BatteryPropertiesStream(&BatteryProperties);

       //to skip the first descriptive line of the "BatteryProperties.stad"
       QString InfosOfStadFile;
       int NumberOfColumns = 4;
       int i;
       for (i=0;i<NumberOfColumns;i++)
       {
           BatteryPropertiesStream >> InfosOfStadFile;
           qDebug() << InfosOfStadFile;
       }

       //find the values from the database
       QString CName = "";
       double SpecificEnergyDensity;
       double SpecificPower;
       double VolumeEnergyDensity;

       do
       {
           BatteryPropertiesStream >> CName;
           qDebug()<< CName;
           BatteryPropertiesStream >> SpecificEnergyDensity;
           qDebug()<< SpecificEnergyDensity;
           BatteryPropertiesStream >> SpecificPower;
           qDebug()<< SpecificPower;
           BatteryPropertiesStream >> VolumeEnergyDensity;
           qDebug()<< VolumeEnergyDensity;

           qDebug()<< "while - " <<((!(CName==Name))&&(!(CName=="UNDEFINED")));
       }
       while ((!(CName==Name))&&(!(CName=="UNDEFINED")));

       // set the values from the database

       SCBattery.Name = CName;
       qDebug()<< SCBattery.Name;

       SCBattery.SpecificEnergyDensity = SpecificEnergyDensity;
       SCBattery.SpecificPower = SpecificPower;
       SCBattery.VolumeEnergyDensity = VolumeEnergyDensity;

       BatteryProperties.close();

       CalculateAndSetNumberOfBatteries();
       CalculateAndSetBMass();
       CalculateAndSetBVolume();
}

BatteryType CBattery::getBatteryProperties()
{
    return CBattery::SCBattery;
}

void CBattery::CalculateAndSetBMass()
{
    if (SCBattery.SpecificEnergyDensity > 0.0)
        CBattery::BMass = PayloadWattHourInEclipse
                          / SCBattery.SpecificEnergyDensity;

    qDebug()<<"PayloadWattHourInEclipse"<<PayloadWattHourInEclipse;
    qDebug()<<"SCBattery.SpecificEnergyDensity"<<SCBattery.SpecificEnergyDensity;
}

void CBattery::setBMass(double mass)
{
    BMass = mass;
}

double CBattery::getBMass()
{
    CalculateAndSetBMass();
    return CBattery::BMass;
}

void CBattery::CalculateAndSetBVolume()
{
    if (SCBattery.VolumeEnergyDensity > 0.0)
        CBattery::BVolume = ((SCBattery.SpecificEnergyDensity
                              / SCBattery.VolumeEnergyDensity)
                                * BMass)
                            * 0.001;  //convert the liter to m3
}

void CBattery::setBVolume(double volume)
{
    BVolume = volume;
}

double CBattery::getBVolume()
{
    CalculateAndSetBVolume();
    return CBattery::BVolume;
}

void CBattery::CalculateAndSetNumberOfBatteries()
{
    //values has to be round up
    CBattery::NumberOfBatteries = ceil((EclipseDuration/86400)
                                       / (SCBattery.SpecificEnergyDensity
                                         / SCBattery.SpecificPower));
}

void CBattery::setNumberOfBatteries(int number)
{
    NumberOfBatteries = number;
}

int CBattery::getNumberOfBatteries()
{
    return CBattery::NumberOfBatteries;
}

void CBattery::setEclipseDuration(double Eclipse)
{
    EclipseDuration = Eclipse;
}

void CBattery::setDaylightDuration(double Daylight)
{
    DaylightDuration = Daylight;
}

void CBattery::setMissionDuration(double Duration)
{
    MissionDuration = Duration;
}

void CBattery::setPayloadWattHourInEclipse(double WattHour)
{
    PayloadWattHourInEclipse = WattHour;
}

void CBattery::setPayloadWattHourInDaylight(double WattHour)
{
    PayloadWattHourInDaylight = WattHour;
}


//----------------- End of CBattery Class Declaration -----------------//

//----------------- Constructor -------------------------------------//

PowerSubsystem::PowerSubsystem()
{
    int i;

    for (i=0; i<NumberOfPayloads; i++)
    {
        PowerSubsystem::Payloads[i].Name = "";
        PowerSubsystem::Payloads[i].PowerConsumptionInEclipse = 0.0;
        PowerSubsystem::Payloads[i].PowerOnTimeInEclipse =0.0;
        PowerSubsystem::Payloads[i].PowerConsumptionInDaylight = 0.0;
        PowerSubsystem::Payloads[i].PowerOnTimeInDaylight = 0.0;
    }

    PossibleMaximumPowerConsumptionsOfPayloads= 0.0;
    MinimumPowerConsumptionInEclipse= 0.0;
    MaximumPowerConsumptionInDaylight= 0.0;

    //values are only to visualize the calculations before mission class
    //integrated to STA. after that they will be set to zero
    EclipseDuration = 0.0;// in seconds
    DaylightDuration = 0.0;
    MissionDuration = 0.0;//years
    PayloadWattHourInEclipse = 0.0;
    PayloadWattHourInDaylight = 0.0;

    AveragePowerConsumptionInEclipse = 0.0;
    AveragePowerConsumptionInDaylight = 0.0;

    SCPowerDetails.SCTotal = 0.0;
    SCPowerDetails.SubsystemsTotalPower = 0.0;
    SCPowerDetails.ThermalSubsystemPower = 0.0;
    SCPowerDetails.TotalPayloadPower = 0.0;
    SCPowerDetails.TTCSubsystemPower = 0.0;
    SCPowerDetails.OBDHSubsystemPower = 0.0;

    qDebug()<<"POWER1 CONSTRUCTOR ******";

    //Passing values to subClasses
    SolarArrays.setEclipseDuration(EclipseDuration);
    Battery.setEclipseDuration(EclipseDuration);

    SolarArrays.setDaylightDuration(DaylightDuration);
    Battery.setDaylightDuration(DaylightDuration);

    SolarArrays.setMissionDuration(MissionDuration);
    Battery.setMissionDuration(MissionDuration);

    qDebug()<<"POWER2 CONSTRUCTOR ******";
}

//----------------- Functions ---------------------------------------//
void PowerSubsystem::setPayloadsPower(int    Index,
                                      QString Name,
                                      double PowerConsumptionInEclipse,
                                      double PowerOnPercentageInEclipse,
                                      double PowerConsumptionInDaylight,
                                      double PowerOnPercentageInDaylight)
{
    PowerSubsystem::Payloads[Index].Name = Name;
    PowerSubsystem::Payloads[Index].PowerConsumptionInEclipse =
            PowerConsumptionInEclipse;
    PowerSubsystem::Payloads[Index].PowerOnTimeInEclipse =
            PowerOnPercentageInEclipse / 100 * EclipseDuration;
    PowerSubsystem::Payloads[Index].PowerConsumptionInDaylight =
            PowerConsumptionInDaylight;
    PowerSubsystem::Payloads[Index].PowerOnTimeInDaylight =
            PowerOnPercentageInDaylight / 100 * DaylightDuration;

    SCPowerDetails.TotalPayloadPower = 0.0;

    int i;
    PayloadWattHourInEclipse = 0.0;
    PayloadWattHourInDaylight = 0.0;
    for (i=0; i<NumberOfPayloads; i++)
    {
        PayloadWattHourInEclipse += Payloads[i].PowerConsumptionInEclipse
                       * Payloads[i].PowerOnTimeInEclipse / 3600;
        PayloadWattHourInDaylight += Payloads[i].PowerConsumptionInDaylight
                       * Payloads[i].PowerOnTimeInDaylight / 3600;
        SCPowerDetails.TotalPayloadPower
                += (Payloads[i].PowerConsumptionInEclipse
                   + Payloads[i].PowerConsumptionInDaylight);
    }

    //Passing calculated values to sub classes
    SolarArrays.setPayloadWattHourInDaylight(PayloadWattHourInDaylight);
    Battery.setPayloadWattHourInDaylight(PayloadWattHourInDaylight);

    SolarArrays.setPayloadWattHourInEclipse(PayloadWattHourInEclipse);
    Battery.setPayloadWattHourInEclipse(PayloadWattHourInEclipse);

    //Calculate any power relations of payload

    CalculateAndSetTotalSCPower();
    CalculateAndSetPossibleMaximumPowerConsumptionsOfPayloads();
    CalculateAndSetMinimumPowerConsumptionInEclipse();
    CalculateAndSetMaximumPowerConsumptionInDaylight();
    CalculateAndSetAveragePowerConsumptionInEclipseAndDaylight();
}

PayloadPowerInfo PowerSubsystem::getPayloadOfIndex(int Index)
{
    return PowerSubsystem::Payloads[Index];
}

void PowerSubsystem::CalculateAndSetPossibleMaximumPowerConsumptionsOfPayloads()
{
    int i ;
    double tempEclipse = 0.0;
    double tempDaylight = 0.0;
    for (i = 0; i<NumberOfPayloads; i++)
    {
        tempEclipse +=
                PowerSubsystem::Payloads[i].PowerConsumptionInEclipse;
        tempDaylight +=
                PowerSubsystem::Payloads[i].PowerConsumptionInDaylight;
    }

    if (tempEclipse > tempDaylight)
    {
        PowerSubsystem::PossibleMaximumPowerConsumptionsOfPayloads =
                tempEclipse;
    }else
    {
        PowerSubsystem::PossibleMaximumPowerConsumptionsOfPayloads =
                tempDaylight;
    }

    SCPowerDetails.TotalPayloadPower
            = PowerSubsystem::PossibleMaximumPowerConsumptionsOfPayloads;
}

double PowerSubsystem::getPossibleMaximumPowerConsumptionsOfPayloads()
{
    return PowerSubsystem::PossibleMaximumPowerConsumptionsOfPayloads;
}

void PowerSubsystem::CalculateAndSetMinimumPowerConsumptionInEclipse()
{
    int i;
    double temp = SCPowerDetails.SubsystemsTotalPower;

    for (i=0; i<NumberOfPayloads; i++)
    {
        if (PowerSubsystem::Payloads[i].PowerOnTimeInEclipse >= EclipseDuration)
        {
            temp += PowerSubsystem::Payloads[i].PowerConsumptionInEclipse;
        }
    }

    PowerSubsystem::MinimumPowerConsumptionInEclipse = temp;

}

double PowerSubsystem::getMinimumPowerConsumptionInEclipse()
{
    return PowerSubsystem::MinimumPowerConsumptionInEclipse;
}

void PowerSubsystem::CalculateAndSetMaximumPowerConsumptionInDaylight()
{
    int i;
    double temp = SCPowerDetails.SubsystemsTotalPower;

    for (i=0; i<NumberOfPayloads; i++)
    {
        temp += PowerSubsystem::Payloads[i].PowerConsumptionInDaylight;
    }
}

double PowerSubsystem::getMaximumPowerConsumptionInDaylight()
{
    return PowerSubsystem::MaximumPowerConsumptionInDaylight;
}

void PowerSubsystem::CreateGeneratedPowerTimeFunctionOfSpacecraft()
{
//***************************************************************************
    if ((SolarArrays.getSolarCellBOLPower()>0.0)
        &&(SolarArrays.getArea()>0.0)
        &&(SolarArrays.getLifeTimeDegradation()>=0.0))
    {
        //open the file you want to put the results--------
        QString path = QString
                       ("data/SystemsEngineeringReports/GeneratedPowerTimeFunction.stad");

        QFile GeneratedPowerTime(path);
        GeneratedPowerTime.open(QIODevice::ReadWrite);
               qDebug()<<"GeneratedPowerTime.fileName()"<<GeneratedPowerTime.fileName();
               qDebug()<<"GeneratedPowerTime.isOpen()"<<GeneratedPowerTime.isOpen();
        QTextStream GeneratedPowerTimeStream(&GeneratedPowerTime);

        //reach to the Eclipse information file------------
        QString path2 = QString("data/EclipseStarLight.stad");

        QFile EclipseStarLight(path2);
        EclipseStarLight.open(QIODevice::ReadOnly);
        //       qDebug()<<EclipseStarLight.fileName();
        //       qDebug()<<EclipseStarLight.isOpen();
        QTextStream EclipseStarLightStream(&EclipseStarLight);

        //get the beginning of the mission
        QString sampleTimeString = "";
        double MissionStart;
        double StarLightExitance;

        //1 column is for the date and time next one for if it is in umbra or penumbra
        //       int NumberOfColumns = 2;

        EclipseStarLightStream >> sampleTimeString;
        qDebug()<<"sampleTimeString; "<<sampleTimeString;
//        MissionStart = sta::CalendarToJd
//                       (QDateTime::fromString(sampleTimeString,Qt::ISODate));
        MissionStart = sampleTimeString.toDouble();
        qDebug()<<"MissionStart "<<MissionStart;

        EclipseStarLightStream >> StarLightExitance;
        qDebug()<<"StarLightExitance;  "<<StarLightExitance;

        double sampleTime;

        while(!EclipseStarLightStream.atEnd())
        {
            EclipseStarLightStream >> sampleTimeString;
//            sampleTime = sta::CalendarToJd
//                         (QDateTime::fromString(sampleTimeString,Qt::ISODate));
            sampleTime = sampleTimeString.toDouble();
            EclipseStarLightStream >> StarLightExitance;

            //Calculate the short time calculations
            double powerShortTime =
                    SolarArrays.getSolarCellBOLPower()
                    * StarLightExitance
                    * SolarArrays.getArea();

            //Calculate and place the generated power function in .stad
            GeneratedPowerTimeStream << sampleTimeString <<"          ";
            double powerLongTime =
                    powerShortTime
                    * pow((1.0 - SolarArrays.getLifeTimeDegradation()),((sampleTime-MissionStart)/365));
//            qDebug()<<"lifetime dega "<<SolarArrays.getLifeTimeDegradation();
//            qDebug()<<"(sampleTime-MissionStart) "<<(sampleTime-MissionStart);
//            qDebug()<<"(sampleTime-MissionStart)/365) "<<(sampleTime-MissionStart)/365;
//            qDebug()<<"POWER OF "<<pow((1.0 - SolarArrays.getLifeTimeDegradation()),((sampleTime-MissionStart)/365));
            GeneratedPowerTimeStream << powerLongTime <<"        ";

            qDebug()<<"EclipseStarLightStream END"<<EclipseStarLightStream.atEnd();
        }

        GeneratedPowerTimeStream <<endl;
        GeneratedPowerTime.close();
        EclipseStarLight.close();

    }
}

void PowerSubsystem::CalculateAndSetPowerSubsystemMass()
{
//    SolarArrays.CalculatePCUMass();
//    SolarArrays.CalculateSAMass();

//    Battery.CalculateAndSetBMass();

    PowerSubsystemMass = SolarArrays.getPCUMass()
                         + SolarArrays.getSAMass()
                         + Battery.getBMass();

    qDebug()<< "*-calculatesubMass B-" <<Battery.getBMass();
    qDebug()<< "*-calculatesubMass S-" <<SolarArrays.getSAMass();
    qDebug()<< "*-calculatesubMass P-" << SolarArrays.getPCUMass();
    qDebug()<< "*-calculatesubMass T-" << PowerSubsystemMass;
}

double PowerSubsystem::getPowersubsystemMass()
{
    return PowerSubsystemMass;
}

void PowerSubsystem::setEclipseDuration(double EclipseTime)
{
    EclipseDuration = EclipseTime;

    //Passing values to subClasses
    SolarArrays.setEclipseDuration(EclipseDuration);
    Battery.setEclipseDuration(EclipseDuration);
}

double PowerSubsystem::getEclipseDuration()
{
    return EclipseDuration;
}

void PowerSubsystem::setDaylightDuration(double DaylightTime)
{
    DaylightDuration = DaylightTime;

    //Passing values to subClasses
    SolarArrays.setDaylightDuration(DaylightDuration);
    Battery.setDaylightDuration(DaylightDuration);
}

double PowerSubsystem::getDaylightDuration()
{
    return DaylightDuration;
}

void PowerSubsystem::setMissionDuration(double Duration)
{
    MissionDuration = Duration;

    //Passing values to subClasses
    SolarArrays.setMissionDuration(MissionDuration);
    Battery.setMissionDuration(MissionDuration);
}

double PowerSubsystem::getMissionDuration()
{
    return MissionDuration;
}

double PowerSubsystem::getPayloadWattHourInEclipse()
{
    return PayloadWattHourInEclipse;
}

double PowerSubsystem::getPayloadWattHourInDaylight()
{
    return PayloadWattHourInDaylight;
}

void PowerSubsystem::CalculateAndSetAveragePowerConsumptionInEclipseAndDaylight()
{
    AveragePowerConsumptionInEclipse
            = PayloadWattHourInEclipse * 3600 / EclipseDuration;
    AveragePowerConsumptionInDaylight
            = PayloadWattHourInDaylight * 3600 / DaylightDuration;
}

double PowerSubsystem::getAveragePowerConsumptionInEclipse()
{
    return AveragePowerConsumptionInEclipse;
}

double PowerSubsystem::getAveragePowerConsumptionInDaylight()
{
    return AveragePowerConsumptionInDaylight;
}

void PowerSubsystem::CalculateAndSetTotalSCPower()
{
    SCPowerDetails.SubsystemsTotalPower =
            SCPowerDetails.TTCSubsystemPower
            + SCPowerDetails.OBDHSubsystemPower
            + SCPowerDetails.ThermalSubsystemPower;

    SCPowerDetails.SCTotal = SCPowerDetails.TotalPayloadPower
                             + SCPowerDetails.SubsystemsTotalPower;

    qDebug()<<"SCPowerDetails.TTCSubsystemPower"<<SCPowerDetails.TTCSubsystemPower;
    qDebug()<<"SCPowerDetails.SubsystemsTotalPower"<<SCPowerDetails.SubsystemsTotalPower;
    qDebug()<<"SCPowerDetails.SCTotal"<<SCPowerDetails.SCTotal;

    SolarArrays.setSystemPower(SCPowerDetails.SubsystemsTotalPower);

    SolarArrays.CalculateSAEOLPower();
}

void PowerSubsystem::setThermalSubsystemPower(double Power)
{
    SCPowerDetails.ThermalSubsystemPower = Power;

    CalculateAndSetTotalSCPower();
    SolarArrays.CalculateArea();
}

void PowerSubsystem::setTTCSubsystemPower(double Power)
{
    SCPowerDetails.TTCSubsystemPower = Power;

    CalculateAndSetTotalSCPower();
    SolarArrays.CalculateArea();
}

void PowerSubsystem::setOBDHSubsystemPower(double Power)
{
    SCPowerDetails.OBDHSubsystemPower = Power;

    CalculateAndSetTotalSCPower();
    SolarArrays.CalculateArea();
}

double PowerSubsystem::getSCPower()
{
    return SCPowerDetails.SCTotal;
}

void PowerSubsystem::setPowerSubsystemDataRate(double DataRate)
{
    PowerSubsystemDataRate = DataRate;
}

double PowerSubsystem::getPowerSubsystemDataRate()
{
    return PowerSubsystemDataRate;
}

double PowerSubsystem::getPowerSubsystemVolume()
{
    PowerSubsystemVolume
            = Battery.getBVolume()
              + SolarArrays.getArea() * 0.02; //assume 0.02m thick solar cells
    return PowerSubsystemVolume;
}

//----------------- End of PowerSubsystem Class Declaration -----------------//

