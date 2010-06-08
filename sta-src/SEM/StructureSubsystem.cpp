#include "StructureSubsystem.h"

#include <math.h>

//to Read and write to file
#include <QFile>
#include <QTextStream>
#include <QDir>
#include <QProcess>

#include "Astro-Core/constants.h"

//----------------- Constructor -------------------------------------//
StructureSubsystem::StructureSubsystem()
{

    SCSizing.setZero();

//    SCVolume = 0.0;

//    BatteryVolume = 0.0;

    MomentsOfInertia.setZero();

    SecondMomentsOfArea.setZero();

    SCShape = Undefined;

    NaturalFrequency.Axial = 0.0;
    NaturalFrequency.Lateral = 0.0;

    SCMaterial.Name = "";
    SCMaterial.ModulusOfElasticity = 0.0;
    SCMaterial.Absorptivity = 0.0;
    SCMaterial.Emmissivity = 0.0;

    SCMassDetails.OBDHSubsystemMass = 0.0;
    SCMassDetails.StructureSubsystemMass = 0.0;
    SCMassDetails.ThermalSubsystemMass = 0.0;
    SCMassDetails.TotalPayloadMass = 0.0;
    SCMassDetails.TTCSubsystemMass = 0.0;
    SCMassDetails.PowerSubsystemMass = 0.0;
    SCMassDetails.SCTotal = 0.0;

    //initialize the estimate calculations
    SCMassEstimatePercentages.MissionType = "";
    SCMassEstimatePercentages.OBDHSubsystemPercentage = 0.0;
    SCMassEstimatePercentages.PayloadPercentage = 0.0;
    SCMassEstimatePercentages.PowerSubsystemPercentage = 0.0;
    SCMassEstimatePercentages.StructureSubsystemPercentage = 0.0;
    SCMassEstimatePercentages.ThermalSubsystemPercentage = 0.0;
    SCMassEstimatePercentages.TTCSubsystemPercentage = 0.0;


    //For thermal calculations set the size of areas
    SCThermalDetails.AreaOfColdFace = 0.0;
    SCThermalDetails.AreaOfHotFace = 0.0;
    SCThermalDetails.TotalAreaOfColdFace = 0.0;
    SCThermalDetails.TotalAreaOfHotFace = 0.0;

    //this setting is done by wizard but for now it is constant
    setMassEstimations("Light_Satellite");

    SCVolumeDetails.OBDHSubsystemVolume = 0.0;
    SCVolumeDetails.PowerSubsystemVolume = 0.0;
    SCVolumeDetails.SCTotalVolume = 0.0;
    SCVolumeDetails.StructureSubsystemVolume = 0.0;
    SCVolumeDetails.ThermalSubsystemVolume = 0.0;
    SCVolumeDetails.TTCSubsystemVolume = 0.0;

    int i ;
    for (i=0; i<NumberOfPayloads; i++)
    {
        PayloadStructureDetails[i].Name = "";
        PayloadStructureDetails[i].PayloadMass = 0.0;
        PayloadStructureDetails[i].PayloadVolume = 0.0;
        PayloadStructureDetails[i].PayloadStructure.setZero();
    }
}

//----------------- Functions ---------------------------------------//
void StructureSubsystem::CalculateAndSetSCVolume()
{
    double tempSCVolume = 0.0;
    int i;
    for(i=0; i<NumberOfPayloads; i++)
    {
        tempSCVolume += PayloadStructureDetails[i].PayloadVolume
                        * (1.0 + 0.2);
    }
    qDebug()<<"tempSC VOLUME"<<tempSCVolume;

    tempSCVolume += SCVolumeDetails.PowerSubsystemVolume * (1.0 + 0.2)
                    //20% margin -> (1.0 + 0.2)
                    + SCVolumeDetails.TTCSubsystemVolume * (1.0 + 0.2)
                    + SCVolumeDetails.OBDHSubsystemVolume * (1.0 + 0.2)
                    + SCVolumeDetails.StructureSubsystemVolume * (1.0 + 0.2)
                    + SCVolumeDetails.ThermalSubsystemVolume * (1.0 + 0.2);

    SCVolumeDetails.SCTotalVolume = tempSCVolume * (1.0 + 0.2);

    qDebug()<<"SCVolumeDetails.PowerSubsystemVolume"<<SCVolumeDetails.PowerSubsystemVolume;
    qDebug()<<"SCVolumeDetails.TTCSubsystemVolume"<<SCVolumeDetails.TTCSubsystemVolume;
    qDebug()<<"SCVolumeDetails.OBDHSubsystemVolume"<<SCVolumeDetails.OBDHSubsystemVolume;
    qDebug()<<"SCVolumeDetails.StructureSubsystemVolume"<<SCVolumeDetails.StructureSubsystemVolume;
    qDebug()<<"SCVolumeDetails.ThermalSubsystemVolume"<<SCVolumeDetails.ThermalSubsystemVolume;
    qDebug()<<"tempSC VOLUME"<<tempSCVolume;
}

void StructureSubsystem::setSCVolume(double volume)
{
    SCVolumeDetails.SCTotalVolume = volume;
}

double StructureSubsystem::getSCVolume()
{
    return SCVolumeDetails.SCTotalVolume;
}

void StructureSubsystem::CalculateAndSetSpacecraftDimension()
{
    switch (SCShape)
    {
    case Cube: // Cube
        {
            SCSizing.x() = cbrt(SCVolumeDetails.SCTotalVolume);
            SCSizing.y() = SCSizing.x();
            SCSizing.z() = SCSizing.y();

            //For thermal calculations set the size of areas
            SCThermalDetails.AreaOfColdFace = SCSizing.x() * SCSizing.y();
            SCThermalDetails.AreaOfHotFace = SCSizing.x() * SCSizing.y() ;
            SCThermalDetails.TotalAreaOfColdFace = 2 * SCSizing.x() * SCSizing.y();
            SCThermalDetails.TotalAreaOfHotFace = 4 * SCSizing.x() * SCSizing.y();

            qDebug()<<"cube";           
            break;
        }
    case Cylindrical:
        {
            SCSizing.x() = cbrt(SCVolumeDetails.SCTotalVolume/mypi)*2;    //Diameter
            SCSizing.y() = cbrt(SCVolumeDetails.SCTotalVolume/mypi);
            SCSizing.z() = 0.0;

            //For thermal calculations set the size of areas
            SCThermalDetails.AreaOfColdFace =  mypi * pow(SCSizing.x(),2.0) / 4;
            SCThermalDetails.AreaOfHotFace = mypi * SCSizing.x()/2 * SCSizing.y();
            SCThermalDetails.TotalAreaOfColdFace = 2 * SCThermalDetails.AreaOfColdFace ;
            SCThermalDetails.TotalAreaOfHotFace = 2 * SCThermalDetails.AreaOfHotFace;

            qDebug()<<"cylinder";
            break;
        }
    case Spherical:
        {
            SCSizing.x() = cbrt((3 * SCVolumeDetails.SCTotalVolume)
                                / (4 * mypi))*2;  //Diameter
            SCSizing.y() = 0.0;
            SCSizing.z() = 0.0;

            //For thermal calculations set the size of areas
            SCThermalDetails.AreaOfColdFace = 0.0;
            SCThermalDetails.AreaOfHotFace = 0.5 * mypi * pow(SCSizing.x(),2.0);
            SCThermalDetails.TotalAreaOfColdFace = 0.0;
            SCThermalDetails.TotalAreaOfHotFace = 2 * SCThermalDetails.AreaOfHotFace;

            qDebug()<<"sphere";
            break;
        }
    default:
        {
            SCSizing.x() = 0.0;
            SCSizing.y() = 0.0;
            SCSizing.z() = 0.0;
        }
    }

    qDebug()<<SCThermalDetails.AreaOfColdFace;
    qDebug()<<SCThermalDetails.AreaOfHotFace ;
    qDebug()<<SCThermalDetails.TotalAreaOfColdFace ;
    qDebug()<<SCThermalDetails.TotalAreaOfHotFace;
qWarning("TODO: %s	%d",__FILE__,__LINE__);
}

void StructureSubsystem::setSpacecraftDimension(double width,
                                                double height,
                                                double length)
{
    SCSizing.x() = width;
    SCSizing.y() = height;
    SCSizing.z() = length;
}

Vector3d StructureSubsystem::getSpacecraftDimension()
{
    return SCSizing;
}

void StructureSubsystem::CalculateAndSetSCMass()
{
    //calculate total payload mass
    int i;
    SCMassDetails.TotalPayloadMass = 0.0;

    for (i=0; i<4; i++)
    {
        SCMassDetails.TotalPayloadMass += PayloadStructureDetails[i].PayloadMass;
    }       

    // if the masses are Zero, it will estimate
    //otherwise it will set the existing values
    double estimate = 0.0;
    if (SCMassDetails.PowerSubsystemMass <= 0.0)
        estimate += SCMassDetails.TotalPayloadMass
                    / SCMassEstimatePercentages.PayloadPercentage
                    * SCMassEstimatePercentages.PowerSubsystemPercentage;

    qDebug()<<"estimate"<<estimate;

    if (SCMassDetails.StructureSubsystemMass <= 0.0)
        estimate += SCMassDetails.TotalPayloadMass
                    / SCMassEstimatePercentages.PayloadPercentage
                    * SCMassEstimatePercentages.StructureSubsystemPercentage;

    qDebug()<<"SCMassEstimatePercentages.PayloadPercentage"<<SCMassEstimatePercentages.PayloadPercentage;
    qDebug()<<"estimate"<<estimate;

    if (SCMassDetails.ThermalSubsystemMass <= 0.0)
        estimate += SCMassDetails.TotalPayloadMass
                    / SCMassEstimatePercentages.PayloadPercentage
                    * SCMassEstimatePercentages.ThermalSubsystemPercentage;

    qDebug()<<"estimate"<<estimate;

    if (SCMassDetails.TTCSubsystemMass <= 0.0)
        estimate += SCMassDetails.TotalPayloadMass
                    / SCMassEstimatePercentages.PayloadPercentage
                    * SCMassEstimatePercentages.TTCSubsystemPercentage;

    qDebug()<<"estimate"<<estimate;

    SCMassDetails.SCTotal = SCMassDetails.PowerSubsystemMass
                            + SCMassDetails.OBDHSubsystemMass
                            + SCMassDetails.StructureSubsystemMass
                            + SCMassDetails.ThermalSubsystemMass
                            + SCMassDetails.TTCSubsystemMass
                            + SCMassDetails.TotalPayloadMass
                            + estimate;

    qDebug()<<"SCMassDetails.Power"<<SCMassDetails.PowerSubsystemMass;
    qDebug()<<"SCMassDetails"<<SCMassDetails.OBDHSubsystemMass;
    qDebug()<<"SCMassDetails"<<SCMassDetails.StructureSubsystemMass;
    qDebug()<<"SCMassDetails"<<SCMassDetails.ThermalSubsystemMass;
    qDebug()<<"SCMassDetails"<<SCMassDetails.TTCSubsystemMass;
    qDebug()<<"SCMassDetails"<<SCMassDetails.TotalPayloadMass;
    qDebug()<<"SCMassDetails.TotalPayloadMass"<<SCMassDetails.TotalPayloadMass;
    qDebug()<<"estimate"<<estimate;
}

void StructureSubsystem::setSCMass(double Mass)
{
    SCMassDetails.SCTotal = Mass;
}

double StructureSubsystem::getSCMass()
{
    return SCMassDetails.SCTotal;
}

void StructureSubsystem::CalculateAndSetMomentsOfInertia()
{
    switch (SCShape)
    {
    case Cube:
        {
            MomentsOfInertia.x() = (SCMassDetails.SCTotal * pow(SCSizing.x(),2))/6;
            MomentsOfInertia.y() = (SCMassDetails.SCTotal * pow(SCSizing.y(),2))/6;
            MomentsOfInertia.z() = (SCMassDetails.SCTotal * pow(SCSizing.z(),2))/6;
            break;
        }
    case Cylindrical:
        {
            MomentsOfInertia.x() = SCMassDetails.SCTotal *
                                   ( ( pow(SCSizing.x(),2)/16 )+(SCSizing.y()/12) );
            MomentsOfInertia.y() = SCMassDetails.SCTotal *
                                   ( ( pow(SCSizing.x(),2)/16 )+(SCSizing.y()/12) );
            MomentsOfInertia.z() = SCMassDetails.SCTotal * pow(SCSizing.x(),2) / 8;
            break;
        }
    case Spherical:
        {
            MomentsOfInertia.x() = 0.1 * SCMassDetails.SCTotal * pow(SCSizing.x(),2);
            MomentsOfInertia.y() = 0.1 * SCMassDetails.SCTotal * pow(SCSizing.x(),2);
            MomentsOfInertia.z() = 0.1 * SCMassDetails.SCTotal * pow(SCSizing.x(),2);
            break;
        }
    default:
        {
            MomentsOfInertia.x() = 0.0;
            MomentsOfInertia.y() = 0.0;
            MomentsOfInertia.z() = 0.0;
            break;
        }
    }
}

void StructureSubsystem::setMomentsOfInertia(double x, double y, double z)
{
    MomentsOfInertia.x() = x;
    MomentsOfInertia.y() = y;
    MomentsOfInertia.z() = z;
}

Vector3d StructureSubsystem::getMomentsOfInertia()
{
    return MomentsOfInertia;
}

void StructureSubsystem::CalculateAndSetSecondMomentsOfArea()
{
    switch (SCShape)
    {
    case Cube:
        {
            SecondMomentsOfArea.x() = (pow(SCSizing.x(),4.0)/ 12);
            SecondMomentsOfArea.y() = (pow(SCSizing.y(),4.0)/ 12);
            SecondMomentsOfArea.z() = (pow(SCSizing.x(),4.0)/ 12);
            break;
        }
    case Cylindrical:
        {
            SecondMomentsOfArea.x() =(mypi * pow(SCSizing.x(),4.0) / 64);
            SecondMomentsOfArea.y() =(mypi * pow(SCSizing.x(),4.0) / 64);
            SecondMomentsOfArea.z() =(mypi * pow(SCSizing.x(),4.0) / 32);
            break;
        }
    case Spherical:
        {
            SecondMomentsOfArea.x() =(mypi * pow(SCSizing.x(),4.0) / 64);
            SecondMomentsOfArea.y() =(mypi * pow(SCSizing.x(),4.0) / 64);
            SecondMomentsOfArea.z() =(mypi * pow(SCSizing.x(),4.0) / 32);
            break;
        }
    default:
        {
            SecondMomentsOfArea.x() = 0.0;
            SecondMomentsOfArea.y() = 0.0;
            SecondMomentsOfArea.z() = 0.0;
            break;
        }
    }
}

void StructureSubsystem::setSecondMomentsOfArea(double x, double y, double z)
{
    SecondMomentsOfArea.x() = x;
    SecondMomentsOfArea.y() = y;
    SecondMomentsOfArea.z() = z;
}

Vector3d StructureSubsystem::getSecondMomentsOfArea()
{
    return SecondMomentsOfArea;
}

void StructureSubsystem::setSCShape(Shape DefinedShape)
{
    SCShape = DefinedShape;
}

void StructureSubsystem::setSCShape(QString Shape)
{
    if (Shape=="Cube")
        SCShape = Cube;
    else
    {
        if (Shape =="Cylinder")
            SCShape = Cylindrical;
        else
        {
            if (Shape == "Sphere")
                SCShape = Spherical;
            else
                SCShape = Undefined;
        }
    }
}

Shape StructureSubsystem::getSCShape()
{
    return SCShape;
}

void StructureSubsystem::CalculateAndSetLateralFrequency()
{
    switch (SCShape)
    {
    case Cube:
        {
            NaturalFrequency.Lateral = (0.560 *
                        sqrt((SCMaterial.ModulusOfElasticity * SecondMomentsOfArea.x())
                             /
                             (SCMassDetails.SCTotal * pow(SCSizing.z(),3.0))
                            )   );
            break;
        }
    case Cylindrical:
        {
            NaturalFrequency.Lateral = (0.560 *
                    sqrt((SCMaterial.ModulusOfElasticity * SecondMomentsOfArea.x())
                         /
                         (SCMassDetails.SCTotal * pow(SCSizing.y(),3))
                        )   );
            break;
        }
    case Spherical:
        {
            NaturalFrequency.Lateral = (0.560 *
                    sqrt((SCMaterial.ModulusOfElasticity * SecondMomentsOfArea.x())
                         /
                         (SCMassDetails.SCTotal * pow(SCSizing.x(),3))
                        )   );
            break;
        }
    default:
        NaturalFrequency.Lateral = 0;
        break;
    }
}

void StructureSubsystem::setLateralFrequency(double Frequency)
{
    NaturalFrequency.Lateral = Frequency;
}

double StructureSubsystem::getLateralFrequency()
{
    return NaturalFrequency.Lateral;
}

void StructureSubsystem::CalculateAndSetAxialFrequency()
{
    //first find the area of the SC that is going to bound to launcher
    double AreaOfLauncherInterface;

    switch(SCShape)
    {
    case Cube:
        {
            AreaOfLauncherInterface = SCSizing.x() * SCSizing.y();
            NaturalFrequency.Axial = (0.250 *
                                      sqrt((AreaOfLauncherInterface * SCMaterial.ModulusOfElasticity )
                                           / (SCMassDetails.SCTotal * SCSizing.z()) ) );
            break;
        }
    case Cylindrical:
        {
            AreaOfLauncherInterface = mypi * pow(SCSizing.x(),2.0);
            NaturalFrequency.Axial = (0.250 *
                                      sqrt((AreaOfLauncherInterface * SCMaterial.ModulusOfElasticity )
                                           / (SCMassDetails.SCTotal * SCSizing.y()) ));
            break;
        }
    case Spherical:
        {
            // As there is a point interface, area is averaged to half radius
            AreaOfLauncherInterface = mypi * pow((SCSizing.x()/2),2.0);
            NaturalFrequency.Axial = (0.250 *
                                      sqrt((AreaOfLauncherInterface * SCMaterial.ModulusOfElasticity )
                                           / (SCMassDetails.SCTotal * SCSizing.x()) ));
            break;
        }
    default:
        {
           NaturalFrequency.Axial = 0.0;
           break;
        }
    }
}

void StructureSubsystem::setAxialFrequency(double Frequency)
{
    NaturalFrequency.Axial = Frequency;
}

double StructureSubsystem::getAxialFrequency()
{
    return NaturalFrequency.Axial;
}

void StructureSubsystem::setPayloadsStructure(int      Index,
                                              QString  Name,
                                              Vector3d PayloadStructure,
                                              double   PayloadVolume,
                                              double   PayloadMass)
{
    PayloadStructureDetails[Index].Name = Name;
    PayloadStructureDetails[Index].PayloadStructure = PayloadStructure;
    PayloadStructureDetails[Index].PayloadVolume = PayloadVolume;
    PayloadStructureDetails[Index].PayloadMass = PayloadMass;
}

PayloadStructureInfo* StructureSubsystem::getPayloadStructure()
{
    return PayloadStructureDetails;
}

void StructureSubsystem::setMaterialProperties(QString Name)
{
    QString path = QString("data/MaterialDatabase/MaterialsStructuralProperties.stad");

       QFile MaterialsStructureProperties(path);
       MaterialsStructureProperties.open(QIODevice::ReadOnly);
//       qDebug()<<MaterialsStructureProperties.fileName();
//       qDebug()<<MaterialsStructureProperties.isOpen();
       QTextStream MaterialsStructurePropertiesStream(&MaterialsStructureProperties);

       //to skip the first descriptive line of the "MaterialsStructureProperties.stad"
       QString InfosOfStadFile;
       int NumberOfColumns = 5;
       int i;
       for (i=0;i<NumberOfColumns;i++)
       {
           MaterialsStructurePropertiesStream >> InfosOfStadFile;
           qDebug() << InfosOfStadFile;
       }

       //find the values from the database
       QString CName = "";
       double Emmissivity;
       double Absorptivity;
       double ModulusOfElasticity;
       double Density;

       do
       {
           MaterialsStructurePropertiesStream >> CName;
           qDebug()<< CName;
           MaterialsStructurePropertiesStream >> ModulusOfElasticity;
           qDebug()<< ModulusOfElasticity;
           MaterialsStructurePropertiesStream >> Emmissivity;
           qDebug()<< Emmissivity;
           MaterialsStructurePropertiesStream >> Absorptivity;
           qDebug()<< Absorptivity;
           MaterialsStructurePropertiesStream >> Density;
           qDebug()<< Density;

           qDebug()<< "while - " <<((!(CName==Name))&&(!(CName=="UNDEFINED")));
       }
       while ((!(CName==Name))&&(!(CName=="UNDEFINED")));

       // set the values from the database

       SCMaterial.Name = CName;
       qDebug()<<"SCMATerial" <<SCMaterial.Name;
       qDebug()<<"enquiry" <<Name;

       SCMaterial.ModulusOfElasticity = ModulusOfElasticity;
       SCMaterial.Emmissivity = Emmissivity;
       SCMaterial.Absorptivity = Absorptivity;
       SCMaterial.Density = Density;

       qDebug()<<"SCMaterial.Density"<<SCMaterial.Density;

       MaterialsStructureProperties.close();

}

double StructureSubsystem::getModulusOfElasticity()
{
    return SCMaterial.ModulusOfElasticity;
}

MaterialProperties StructureSubsystem::getSCMaterial()
{
    return SCMaterial;
}

void StructureSubsystem::setMassEstimations(QString SCMission)
{
    QString path = QString("data/Estimations/MassEstimations.stad");

       QFile MassEstimations(path);
       MassEstimations.open(QIODevice::ReadOnly);
//       qDebug()<<MaterialsStructureProperties.fileName();
//       qDebug()<<MaterialsStructureProperties.isOpen();
       QTextStream MassEstimationsStream(&MassEstimations);

       //to skip the first descriptive line of the "MaterialsStructureProperties.stad"
       QString InfosOfStadFile;
       int NumberOfColumns = 8;
       int i;
       for (i=0;i<NumberOfColumns;i++)
       {
           MassEstimationsStream >> InfosOfStadFile;
//           qDebug() << InfosOfStadFile;
       }

       //find the values from the database
       QString Mission = "";
       double Payload;
       double Structure;
       double Thermal;
       double Power;
       double TTC;
       double ADCS;
       double Propulsion;

       do
       {
           MassEstimationsStream >> Mission;
//           qDebug()<< Mission;
           MassEstimationsStream >> Payload;
//           qDebug()<< Payload;
           MassEstimationsStream >> Structure;
//           qDebug()<< Structure;
           MassEstimationsStream >> Thermal;
//           qDebug()<< Thermal;
           MassEstimationsStream >> Power;
//           qDebug()<< Power;
           MassEstimationsStream >> TTC;
//           qDebug()<< TTC;
           MassEstimationsStream >> ADCS;
//           qDebug()<< ADCS;
           MassEstimationsStream >> Propulsion;
//           qDebug()<< Propulsion;

//           qDebug()<< "while - " <<((!(Mission==SCMission))&&(!(Mission=="UNDEFINED")));
       }
       while ((!(Mission==SCMission))&&(!(Mission=="UNDEFINED")));

       // set the values from the database

       SCMassEstimatePercentages.MissionType = Mission;
       qDebug()<<"SC Mission" <<SCMassEstimatePercentages.MissionType;
//       qDebug()<<"enquiry" <<SCMission;

       SCMassEstimatePercentages.PayloadPercentage = Payload;
       SCMassEstimatePercentages.PowerSubsystemPercentage = Power;
       SCMassEstimatePercentages.StructureSubsystemPercentage = Structure;
       SCMassEstimatePercentages.ThermalSubsystemPercentage =  Thermal;
       SCMassEstimatePercentages.TTCSubsystemPercentage = TTC;

       MassEstimations.close();
}

void StructureSubsystem::setPowerSubsystemMass(double mass)
{
    SCMassDetails.PowerSubsystemMass = mass;

    StructureSubsystem::CalculateAndSetSCMass();
}

double StructureSubsystem::getPowerSubsystemMass()
{
    if (SCMassDetails.PowerSubsystemMass > 0.0)
        return SCMassDetails.PowerSubsystemMass;
    else
        return (SCMassDetails.TotalPayloadMass
                / SCMassEstimatePercentages.PayloadPercentage
                * SCMassEstimatePercentages.PowerSubsystemPercentage);
}

void StructureSubsystem::setOBDHSubsystemMass(double mass)
{
    SCMassDetails.OBDHSubsystemMass = mass;

    StructureSubsystem::CalculateAndSetSCMass();
}

double StructureSubsystem::getOBDHSubsystemMass()
{
    if (SCMassDetails.OBDHSubsystemMass > 0.0)
        return SCMassDetails.OBDHSubsystemMass;
    else
        return SCMassDetails.OBDHSubsystemMass; // has to be added to database
}

void StructureSubsystem::setThermalSubsystemMass(double mass)
{
    SCMassDetails.ThermalSubsystemMass = mass;

    StructureSubsystem::CalculateAndSetSCMass();
}

double StructureSubsystem::getThermalSubsystemMass()
{
    if (SCMassDetails.ThermalSubsystemMass > 0.0)
        return SCMassDetails.ThermalSubsystemMass;
    else
        return (SCMassDetails.TotalPayloadMass
                / SCMassEstimatePercentages.PayloadPercentage
                * SCMassEstimatePercentages.ThermalSubsystemPercentage);
}

void StructureSubsystem::setTTCSubsystemMass(double mass)
{
    SCMassDetails.TTCSubsystemMass = mass;

    StructureSubsystem::CalculateAndSetSCMass();
}

double StructureSubsystem::getTTCSubsystemMass()
{
    if (SCMassDetails.TTCSubsystemMass > 0.0)
        return SCMassDetails.TTCSubsystemMass;
    else
        return (SCMassDetails.TotalPayloadMass
                / SCMassEstimatePercentages.PayloadPercentage
                * SCMassEstimatePercentages.TTCSubsystemPercentage);
}

void StructureSubsystem::setStructureSubsystemMass(double mass)
{
    SCMassDetails.StructureSubsystemMass = mass;
}

double StructureSubsystem::getStructureSubsystemMass()
{
    /*calculate the mass of the Structure sum of the TotalAreaOfColdFace
    * and TotalAreaOfHotFace is the Total Area of the Spacecraft
    * if we assume the shell of the SC is 0.5cm. the mass will be as the following
    * 20% margin is to consider the internal structure of the SC for the stability
    */

    if (((SCThermalDetails.TotalAreaOfHotFace + SCThermalDetails.TotalAreaOfColdFace) > 0.0)
        &&
        (SCMaterial.Density > 0.0))
        SCMassDetails.StructureSubsystemMass
                = (SCThermalDetails.TotalAreaOfHotFace
                       + SCThermalDetails.TotalAreaOfColdFace)
                    * 0.005 //0.5cm thickness
                    * SCMaterial.Density
                    * 1.2; //20% margin

    if (SCMassDetails.StructureSubsystemMass > 0.0)
        return SCMassDetails.StructureSubsystemMass;
    else
        return (SCMassDetails.TotalPayloadMass
                / SCMassEstimatePercentages.PayloadPercentage
                * SCMassEstimatePercentages.StructureSubsystemPercentage);
}

void StructureSubsystem::setPowerSubsystemVolume(double volume)
{
    SCVolumeDetails.PowerSubsystemVolume = volume;

    // re-calculate the values with the new SC volume
    CalculateAndSetSCVolume();
    CalculateAndSetSpacecraftDimension();
    CalculateAndSetMomentsOfInertia();
    CalculateAndSetSecondMomentsOfArea();
    CalculateAndSetLateralFrequency();
    CalculateAndSetAxialFrequency();
}

double StructureSubsystem::getPowerSubsystemVolume()
{
    return SCVolumeDetails.PowerSubsystemVolume;
}

void StructureSubsystem::setOBDHSubsystemVolume(double volume)
{
    SCVolumeDetails.OBDHSubsystemVolume = volume;

    // re-calculate the values with the new SC volume
    CalculateAndSetSCVolume();
    CalculateAndSetSpacecraftDimension();
    CalculateAndSetMomentsOfInertia();
    CalculateAndSetSecondMomentsOfArea();
    CalculateAndSetLateralFrequency();
    CalculateAndSetAxialFrequency();
}

double StructureSubsystem::getOBDHSubsystemVolume()
{
    return SCVolumeDetails.OBDHSubsystemVolume;
}

void StructureSubsystem::setThermalSubsystemVolume(double volume)
{
    SCVolumeDetails.ThermalSubsystemVolume = volume;

    // re-calculate the values with the new SC volume
    CalculateAndSetSCVolume();
    CalculateAndSetSpacecraftDimension();
    CalculateAndSetMomentsOfInertia();
    CalculateAndSetSecondMomentsOfArea();
    CalculateAndSetLateralFrequency();
    CalculateAndSetAxialFrequency();
}

double StructureSubsystem::getThermalSubsystemVolume()
{
    return SCVolumeDetails.ThermalSubsystemVolume;
}

void StructureSubsystem::setTTCSubsystemVolume(double volume)
{
    SCVolumeDetails.TTCSubsystemVolume = volume;

    // re-calculate the values with the new SC volume
    CalculateAndSetSCVolume();
    CalculateAndSetSpacecraftDimension();
    CalculateAndSetMomentsOfInertia();
    CalculateAndSetSecondMomentsOfArea();
    CalculateAndSetLateralFrequency();
    CalculateAndSetAxialFrequency();
}

double StructureSubsystem::getTTCSubsystemVolume()
{
    return SCVolumeDetails.TTCSubsystemVolume;
}

double StructureSubsystem::getStructureSubsystemVolume()
{
    return SCVolumeDetails.StructureSubsystemVolume;
}

ThermalDetails * StructureSubsystem::getSCThermalDetails()
{
    return &SCThermalDetails;
}

double StructureSubsystem::getTotalPayloadMass()
{
    return SCMassDetails.TotalPayloadMass;
}


//----------------- End of StructureSubsystem Class Declaration -----------------//

