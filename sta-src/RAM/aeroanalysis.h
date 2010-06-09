#ifndef AEROANALYSIS_H
#define AEROANALYSIS_H

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

 */
/*
 ------------------ Author: Dominic Dirkx  ------------------------------------------
 ------------------ E-mail: (d.dirkxATstudent.tudelft.nl) ----------------------------
*/

#include <iostream>
#include <fstream>
#include <string>
#include <QString>
#include <math.h>
#include <QFile>
#include <QTextStream>
#include <string.h>
#include <Eigen/Core>
#include <Eigen/Geometry>
#include <stdio.h>
#include <sstream>
#include "vehiclegeometry.h"
#include "partgeometry.h"


USING_PART_OF_NAMESPACE_EIGEN

class AeroAnalysis
{
public:
    AeroAnalysis(VehicleGeometry *_Vehicle, double _cref, double _Sref, Vector3d _MomentRef,int** _SelectedMethods, QList<double> _writeAeroList, bool _writeGeom);
    AeroAnalysis(AeroAnalysis* Low, AeroAnalysis* High,double Mlow,double Mhigh);
    ~AeroAnalysis();
    void GenerateDatabase(int ForceCoord, int MomentCoord);
    QList<QString> writeAeroDatabase(int ForceCoord, int MomentCoord);
    void writeDetailGeomInfo();
    void writePressures(double M, double a, double b);
    void setIndepVariablesTranslation(double* AttitudeRefs, double Mmin, double Mmax, int RegimeType);
    void setIndepVariablesFull(int SimType, double* AttitudeBounds, double Mmin, double Mmax, int RegimeType);
private:
    double* DetermineVehicleCoefs(double M, double alpha, double beta, int ForceCoord, int MomentCoord);
    double* DeterminePartCoefs(int PartNumber, double M, double alpha, double beta, int ForceCoord, int MomentCoord);
    void DeterminePressureCoefs(int PartNumber,double M);
    void PrintCoeffcients(FILE * fid,int coef);
    bool fromPressureWriteList(double currentMach);

    void DetermineInclination(int PartNumber, double alpha, double beta);

    void setMachRange(double Mmin, double Mmax);
    double BridgeCoefs(double LowCoef, double HighCoef,double Mach,double Mlow, double Mhigh);

    double* getForceCoefs(int PartNumber);
    double* getMomentCoefs(int PartNumber);

    void UpdateCompressionCp(double M, int PartNumber);
    void UpdateExpansionCp(double M, int PartNumber);
    void setStagnationPressure(double M);

    void Newtonian(double M, int PartNumber);
    void ModifiedNewtonian(double M, int PartNumber, int Type);
    void ModifiedNewtonianPrandtlMeyer(double M, int PartNumber,int Type);
    void TangentWedge(double M, int PartNumber);
    void TangentWedgeEmpirical(double M, int PartNumber);
    void TangentConeEmpirical(double M, int PartNumber);
    void ModifiedDahlemBuck(double M, int PartNumber);
    void VanDykeUnified(double M, int PartNumber,int Type);
    void SmythDeltaWing(double M, int PartNumber);
    void HankeyFlatSurface(double M, int PartNumber);
    void Vacuum(double M, int PartNumber);
    void PrandtlMeyerFreestream(double M, int PartNumber);
    void HighMachBase(double M, int PartNumber);
    void ACMempirical(double M, int PartNumber);

    double InversePrandtlMeyer(double nu);

public:
    double InversePrandtlMeyerPrecise(double Mach);

    int CoefsOut;
    static const double baseMach[11];
    double* alphas;
    double* betas;
    double* Machs;
    int nMach;
    int nBeta;
    int nAlpha;
    int nCases;
    int AnalysisType; //2: bridged 1: high hypersonic, 0:low hypersonic
    VehicleGeometry Vehicle;
    double** VehicleCoefs;
    Vector3d MomentRef;
    double cref;
    double Sref;
    int** SelectedMethods;

private:
    void writeCoefFile(QString filename, int coef);

    double gamma;
    double Cps;
    QList<double> writeAeroList;
    bool writeGeom;
    static const double IPM1=1.3604;
    static const double IPM2=0.0962;
    static const double IPM3=-0.5127;
    static const double IPM4=-0.6722;
    static const double IPM5=-0.3278;
    static const double MaxIter=20;


};

#endif // AEROANALYSIS_H
