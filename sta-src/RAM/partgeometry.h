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

#ifndef PARTGEOMETRY_H
#define PARTGEOMETRY_H

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
//#include "aerodynamicmethods.h"

typedef Eigen::Matrix< double, 3, 3 > 	MyMatrix3d;
typedef Eigen::Matrix< double, 3, 1 > 	MyVector3d;


using namespace Eigen;

class PartAnalysis;
class PlanarPartAnalysis;
class FusiformPartAnalysis;

class PartGeometry
{
public:
    PartGeometry();//Default Constructor
    PartGeometry(double* Points, QString name, int* HeaderInts, double* HeaderDoubles);//Constructor from data read from .wgs file
    PartGeometry(MyVector3d** Points, int Nlines, int Npoints);
    PartGeometry(const PartGeometry & PartIn);//Copy Construtor
    PartGeometry(const PartAnalysis & SplitPart,int Type);//Constructor of split part
    PartGeometry(const FusiformPartAnalysis & SplitPart,int Type);//Constructor of split part
    PartGeometry(const PlanarPartAnalysis & SplitPart,int Type);//Constructor of split part

    virtual ~PartGeometry();
    void deAllocateArrays();
    bool PointsMatch(MyVector3d Point1, MyVector3d Point2);
    void Mirror(int idType);//Mirrors given part
    void PanelCalculations();//Calculates normals, areas and centroid locations of panels
    void TransformPart();//Apply .wgs rotation, translation and scaling operations
    void DetermineWGStype();//Determines if planar/fusiform (see STA-RAM-TN-1001/NASA TM-85767)
    void DetermineXbounds();//Determine extrema in x-direction

    //double MaxMinPanelComp(int contour, int point, int component, int minmax);
    //int ProjPanelOrientation(int i,int j);
    //double getLvalue(double ytest, double ztest, int j, int k, int side);


    int LocSym;
    int GlobSym;
    int Nlines;
    int Npoints;
    int idNumber;
    int reversed;
    int WGStype;
    QString name;

    int ErrorInt;

    bool BluntFront;
    int CurvatureType;

    MyVector3d** PointsVec;
    MyVector3d** Centroid;
    MyVector3d** Normal;

    double** Area;
    //double** ShadowedFraction;
    double** Cp;
    double** theta;

    double xmin;
    double xmax;
    int ixmin;
    int jxmin;
    int ixmax;
    int jxmax;

protected:
    double PointMean(int Component, int Index);
    double LineMean(int Component, int Line);
    double LineVariance(int Component, int Line, double Mean);


private:

    MyMatrix3d Rotate;
    MyMatrix3d Scale;
    MyVector3d Translate;

};

class PartAnalysis : public PartGeometry
{
public:
    struct SelectionStruct
    {
        double C[6];
        double k[3];
    };
    PartAnalysis(const PartGeometry & PartToAnalyze,const SelectionStruct * Parameters);
    PartAnalysis();
    virtual ~PartAnalysis();

    virtual void BluntDetermination();
    virtual void DetermineSplitPoint(bool ConcavityIgnore);
    void DetermineCurvatureType();
    bool split;
    int SplitIndex;//Index of LatCC where part is to be split
    int SplitIndexTop;
    int SplitIndexBottom;
    bool closed;

protected:
    virtual int CheckConcavities();
    double AverageLatCCInclination(int n);
    void MakeCentroidContours();
    int BestLineIndexApproximation(double xValue);
    int BestLineIndexApproximation(double xValue,int TopBottom);
    int sign(double n);
    bool DetermineLineCurvatureType(int n);
    double LineAlphaChange(int n);
    MyVector3d** LatCC;
    double** thetaSel;
    double** alphaSel;//Inclination of yz-projected panels w.r.t. xy plane.
    double** LineProj;//Projection of panel sizes onto yz-plane (LatCC indices)
    double* xMean;//Array with mean x-value of lateral centrid contours
    int LCClines;
    int LCCpoints;
    int GeometryType;

    double C1;
    double C2;
    double C3;
    double C4;
    double C5;
    double C6;
    double k1;
    double k2;
    double k3;

};

class PlanarPartAnalysis : public PartAnalysis
{
public:
    PlanarPartAnalysis();
    PlanarPartAnalysis(const PartGeometry & PartToAnalyze,const SelectionStruct * Parameters );
    ~PlanarPartAnalysis();
private:
    double AverageLatCCInclination(int n, bool BluntBool[]);
    int BestLineIndexApproximation(double xValue,int TopBottom);
    virtual int CheckConcavities(int StartingContour, int TopBottom);
    int OrderToBottomOrder(int OrderIndex);
    int OrderToTopOrder(int OrderIndex);
    void SetInclination();
    virtual void BluntDetermination();
    void OrderPlanarLCCs();
    void DetermineProjectedQuantities();
    void DetermineOpenClosed();
    void DetermineFrontBack();
    double DetermineBluntFraction(int n, bool BluntBool[]);
    void PlanarBluntFront();
    void FindBluntPanels(bool Blunt[], int Index);
    virtual void DetermineXLow(int iTop, int iBottom);
    virtual void DetermineSplitPoint(bool ConcavityIgnore);

    int* Order;
    int* InverseOrder;//Array that maps lateral contour indices to lateral contour indices
    int* TopOrder;
    int* BottomOrder;
    //bool TopBottom;
    int FrontBack;//For open parts, 1 if back, 0 if front
    int nTop;
    int nBottom;
    bool* BluntFrontTopLines;
    bool* BluntFrontBottomLines;
    bool BluntFrontTop;
    bool BluntFrontBottom;

    int xLowTopIndex;
    int xLowBottomIndex;

};

class FusiformPartAnalysis : public PartAnalysis
{
public:
    FusiformPartAnalysis();
    FusiformPartAnalysis(const PartGeometry & PartToAnalyze,const SelectionStruct * Parameters);
    ~FusiformPartAnalysis();
private:
    void SetInclination();
    virtual void BluntDetermination();
    void DetermineProjectedQuantities();
    virtual int CheckConcavities(int);
    virtual void DetermineXLow(int i);
    virtual void DetermineSplitPoint(bool ConcavityIgnore);
    void FusiformBluntFront();

    int xLowIndex;//Index of closest approximation of xlow (see STA-TUD-TN-1001)

};

#endif // PARTGEOMETRY_H
