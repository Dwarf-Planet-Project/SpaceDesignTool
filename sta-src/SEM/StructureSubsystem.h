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

#ifndef STRUCTURESUBSYSTEM_H
#define STRUCTURESUBSYSTEM_H

#include <QDebug>

#include "Eigen/Core"
#include <QString>

using namespace Eigen;
using namespace std;

typedef Eigen::Matrix< double, 3, 3 > 	MyMatrix3d;
typedef Eigen::Matrix< double, 3, 1 > 	MyVector3d;

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
    MyVector3d PayloadStructure;
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
    MyVector3d getSpacecraftDimension();

    // It includes the system margin of 20% has to implement database
    void CalculateAndSetSCMass();
    void setSCMass(double Mass);
    double getSCMass();

    void CalculateAndSetMomentsOfInertia();
    void setMomentsOfInertia(double x,double y,double z);
    MyVector3d getMomentsOfInertia();

    void CalculateAndSetSecondMomentsOfInertia();
    void setSecondMomentsOfInertia(double x,double y,double z);
    MyVector3d getSecondMomentsOfInertia();

    void setSCShape(Shape DefinedShape);
    void setSCShape(QString Shape);
    Shape getSCShape();
    QString getSCShapeString();

    void CalculateAndSetLateralFrequency();
    void setLateralFrequency(double Frequency);
    double getLateralFrequency();

    void CalculateAndSetAxialFrequency();
    void setAxialFrequency(double Frequency);
    double getAxialFrequency();

    void setPayloadsStructure(int      Index,
                              QString Name,
                              MyVector3d PayloadStructure,
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
    MyVector3d SCSizing;
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
    MyVector3d MomentsOfInertia;
    MyVector3d SecondMomentsOfInertia;
    Shape SCShape;
    NaturalFreq NaturalFrequency;

    // a database has to be created then the material will select and set the value
    MaterialProperties SCMaterial;

    PayloadStructureInfo PayloadStructureDetails[NumberOfPayloads];


//-------------- End of StructureSubsystem class definition -------------------//

};

#endif // STRUCTURESUBSYSTEM_H
