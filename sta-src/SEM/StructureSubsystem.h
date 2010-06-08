#ifndef STRUCTURESUBSYSTEM_H
#define STRUCTURESUBSYSTEM_H

#include <QDebug>

#include "Eigen/Core"
#include <QString>

using namespace Eigen;
using namespace std;

#define NumberOfPayloads 4

enum Shape{Undefined=0, Cube, Cylindrical, Spherical};
//enum Material{YMUndefined=0, Aluminium, Steel};

//------------------------- Structures ---------------------------------//
struct NaturalFreq
{
    double Axial;
    double Lateral;
};

struct PayloadStructureInfo
{
    QString Name;
    Vector3d PayloadStructure;
    double   PayloadVolume;
    double   PayloadMass;
};

struct MaterialProperties
{
    QString Name;
    double  ModulusOfElasticity;
    double  Emmissivity;
    double  Absorptivity;
    double  Density;
};

struct MassDetails
{
    double TotalPayloadMass;
    double PowerSubsystemMass;
    double TTCSubsystemMass;
    double OBDHSubsystemMass;
    double StructureSubsystemMass;
    double ThermalSubsystemMass;
    double SCTotal;
};

struct MassEstimate
{
    QString MissionType;
    double PayloadPercentage;
    double PowerSubsystemPercentage;
    double TTCSubsystemPercentage;
    double OBDHSubsystemPercentage;
    double StructureSubsystemPercentage;
    double ThermalSubsystemPercentage;
};

struct ThermalDetails
{
    double AreaOfHotFace;
    double AreaOfColdFace;
    double TotalAreaOfHotFace;
    double TotalAreaOfColdFace;
};

struct VolumeDetails
{
    double TTCSubsystemVolume;
    double PowerSubsystemVolume;
    double OBDHSubsystemVolume;
    double StructureSubsystemVolume;
    double ThermalSubsystemVolume;
    double SCTotalVolume;
};

class StructureSubsystem
{
public:
    //----------------- Constructor -------------------------------------//
    StructureSubsystem();

    //----------------- Functions ---------------------------------------//

    // It includes the system margin of 20%
    void CalculateAndSetSCVolume();
    void setSCVolume(double volume);
    double getSCVolume();

    /*
      * It gives the dimensions of the S/C. For now shapes are represented by
      * numbers.
      * Fills SCSizing as follows:For Cube:         x = width
      *                                             y = length
      *                                             z = depth
      *                           For Cylindrical   x = diameter
      *                                             y = length
      *                                             z = 0
      *                           For Spherical     x = diameter
      *                                             y = 0
      *                                             z = 0
      */
    void CalculateAndSetSpacecraftDimension();
    void setSpacecraftDimension(double width, double height, double length);
    Vector3d getSpacecraftDimension();

    // It includes the system margin of 20% has to implement database
    void CalculateAndSetSCMass();
    void setSCMass(double Mass);
    double getSCMass();

    void CalculateAndSetMomentsOfInertia();
    void setMomentsOfInertia(double x,double y,double z);
    Vector3d getMomentsOfInertia();

    void CalculateAndSetSecondMomentsOfArea();
    void setSecondMomentsOfArea(double x,double y,double z);
    Vector3d getSecondMomentsOfArea();

    void setSCShape(Shape DefinedShape);
    void setSCShape(QString Shape);
    Shape getSCShape();

    void CalculateAndSetLateralFrequency();
    void setLateralFrequency(double Frequency);
    double getLateralFrequency();

    void CalculateAndSetAxialFrequency();
    void setAxialFrequency(double Frequency);
    double getAxialFrequency();

    void setPayloadsStructure(int      Index,
                              QString Name,
                              Vector3d PayloadStructure,
                              double   PayloadVolume,
                              double   PayloadMass);
    PayloadStructureInfo* getPayloadStructure();

    void setMaterialProperties(QString Name);
    double getModulusOfElasticity();
    MaterialProperties getSCMaterial();

    void setMassEstimations(QString SCMissionType);

    void setPowerSubsystemMass(double mass);
    double getPowerSubsystemMass();

    void setOBDHSubsystemMass(double mass);
    double getOBDHSubsystemMass();

    void setStructureSubsystemMass(double mass);
    double getStructureSubsystemMass();

    void setTTCSubsystemMass(double mass);
    double getTTCSubsystemMass();

    void setThermalSubsystemMass(double mass);
    double getThermalSubsystemMass();

    void setPowerSubsystemVolume(double volume);
    double getPowerSubsystemVolume();

    void setOBDHSubsystemVolume(double volume);
    double getOBDHSubsystemVolume();

    double getStructureSubsystemVolume();

    void setTTCSubsystemVolume(double volume);
    double getTTCSubsystemVolume();

    void setThermalSubsystemVolume(double volume);
    double getThermalSubsystemVolume();

    double getTotalPayloadMass();

    ThermalDetails * getSCThermalDetails();


private:
    //----------------- Member objects ---------------------------------//
    /*
    * Vector concept is just used in order to not create a new type.
    * Although one can think like the SC dimensions are given on the
    * positive side of the axis, they only represent the dimensions
    * which are x->width/radius
    *           y->height
    *           z->length
    */
    Vector3d SCSizing;
   // double   SCVolume;
    MassDetails SCMassDetails;
    MassEstimate SCMassEstimatePercentages;
    ThermalDetails SCThermalDetails;
    VolumeDetails SCVolumeDetails;

//    double BatteryVolume; //m3

    /*
    * Vector concept is just used in order to not create a new type.
    * They only represent value of the moment of the body on the specified
    * plane.
    */
    Vector3d MomentsOfInertia;
    Vector3d SecondMomentsOfArea;
    Shape SCShape;
    NaturalFreq NaturalFrequency;

    // a database has to be created then the material will select and set the value
    MaterialProperties SCMaterial;

    PayloadStructureInfo PayloadStructureDetails[NumberOfPayloads];


//-------------- End of StructureSubsystem class definition -------------------//

};

#endif // STRUCTURESUBSYSTEM_H
