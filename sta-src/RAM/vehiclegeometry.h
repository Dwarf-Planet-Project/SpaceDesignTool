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

#ifndef VEHICLEGEOMETRY_H
#define VEHICLEGEOMETRY_H

#include <iostream>
#include <fstream>
#include <string>
#include <QString>
#include <math.h>
#include <QFile>
#include <QTextStream>
#include <Eigen/Core>
#include <Eigen/Geometry>
#include <stdio.h>
#include <sstream>
#include "partgeometry.h"

using namespace Eigen;
using std::string;


class VehicleGeometry
{
public:
    struct PanelInfo
    {
        double y[4];
        double z[4];
        double Area;
        double xmin;
        int Part;
        int Contour;
        int Point;
    };

    struct CurrentPoint
    {
        int Category;
        int Number;
        int IntersectNumber;
    };

    VehicleGeometry(QString path);
    VehicleGeometry(int VehicleType, double* params, int Nlines, int Npoints);
    VehicleGeometry(const VehicleGeometry & vehicle);
    VehicleGeometry();
    ~VehicleGeometry();
    void CalculateGlobalCharacteristics();
    void WriteWGSfile();
    //void GenerateVehicleGeometry();
    Vector3d** MakeBluntedCone(double* params, int Nlines, int Npoints);
    Vector3d** MakeCapsule(double* params, int Nlines, int Npoints);
    Vector3d** MakeBiCone(double* params, int Nlines, int Npoints);
    Vector3d** GeneratePartialCone(double R1, double R2, double thc, double xstart, int nx, int nphi);
    Vector3d** GeneratePartialSphere(double R, double ths, double xstart, int nx, int nphi);
    void UpdateParts(bool SplitBoolArr[]);
    int AutomaticMethodSelection(int** SelectedMethods, const PartAnalysis::SelectionStruct * SelectionParameters,bool ConcavityIgnore,int** defaultMethods);
    void ShadowDetermination();
    void SetTestPoints(double & ytest, double & ztest,int i, int j, int k, int p);
    bool FirstShadowCheck(int i, int j, int k, int l, int m, int n);
    bool SameSign(double n1, double n2);
    void ProcessIntersectingPanels(PanelInfo Panel1,PanelInfo Panel2,CurrentPoint CheckPoint,int Intersects[4][4]);
    void DetermineNextVertex(double & ynew, double & znew, PanelInfo Panel1, PanelInfo Panel2, CurrentPoint& CheckPoint, int Intersects[4][4]);
    void DetermineIntersectPoint(double& ynew, double& znew, PanelInfo Panel1, PanelInfo Panel2, int i, int j);
    int ChooseIntersectionPoint(PanelInfo Panel1, PanelInfo Panel2, int LineOn1, int*LinesOn2);

    void PanelContained(PanelInfo Panel1, PanelInfo Panel2);
    int sign (double n);

    PartGeometry* PartsOut;
    PartGeometry* PartsSplit;
    int* readNlines;
    int* readNpoints;
    int nParts;
    QString GeometryDescription;
    bool wgsModified;
    int SplitCount;
    bool selectionMade;
    double Volume;
    double Surface;
    double NoseRadius;
    string Filename;


    /*
    struct Parametrization
    {
        QString name;
        double params[];
    };
    */
};

#endif // VEHICLEGEOMETRY_H
