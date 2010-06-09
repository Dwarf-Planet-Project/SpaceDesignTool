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

#include "partgeometry.h"
#include "cmath"
#include "Astro-Core/stamath.h"
#include <QDebug>
#include <QMessageBox>

PartGeometry::PartGeometry()
{

}

PartGeometry::~PartGeometry()//deallocation of arrays done in void deAllocateArrays
{

}

PartGeometry::PartGeometry(double Points[] , QString _name, int HeaderInts[], double HeaderDoubles[])//Constructor for making a part from .wgs reading
{
    int i;
    int j;
    ErrorInt=0;
    reversed=1;
    name=_name;
    idNumber=HeaderInts[0];
    Nlines=HeaderInts[1];
    Npoints=HeaderInts[2];
    LocSym=HeaderInts[3];
    GlobSym=HeaderInts[4];
    Rotate = AngleAxisd(HeaderDoubles[2]*sta::Pi()/180, Vector3d::UnitZ())*AngleAxisd(HeaderDoubles[1]*sta::Pi()/180,Vector3d::UnitY())*AngleAxisd(HeaderDoubles[0]*sta::Pi()/180, Vector3d::UnitX());
    Translate=Vector3d(HeaderDoubles[3],HeaderDoubles[4],HeaderDoubles[5]);
    Vector3d ScaleVector=Vector3d(HeaderDoubles[6],HeaderDoubles[7],HeaderDoubles[8]);
    Scale=ScaleVector.asDiagonal();
    PointsVec=new Vector3d* [Nlines];

    for(i=0;i<Nlines;i++)
    {
        PointsVec[i]=new Vector3d[Npoints];
    }

    for(i=0;i<Nlines;i++)
    {
        for(j=0;j<Npoints;j++)
        {
            PointsVec[i][j]=Vector3d(Points[3*j+3*(Npoints*i)],Points[3*j+3*(Npoints*i)+1],Points[3*j+3*(Npoints*i)+2]);
        }
    }
}

PartGeometry::PartGeometry(Vector3d** Points, int _Nlines, int _Npoints)//Constructor for making a part from parametrization
{
    ErrorInt=0;
    reversed=1;
    name="Parametrized shape";
    int i;
    int j;

    Nlines=_Nlines+1;//Add 1 to close back of vehicle, make more general to accept already closed geometries.
    Npoints=_Npoints;
    LocSym=0;
    GlobSym=0;
    double xRear=0;
    for(i=0;i<Npoints;i++)
        xRear+=Points[_Nlines-1][i].x();
    xRear/=Npoints;
    PointsVec=new Vector3d*[Nlines];

    for(i=0;i<Nlines-1;i++)
    {
        PointsVec[i]=new Vector3d[Npoints];
        for(j=0;j<Npoints;j++)
            PointsVec[i][j]=Points[i][j];
    }
    for(i=0;i<Nlines-1;i++)
    {
        delete[] Points[i];
    }
    delete[] Points;
    PointsVec[Nlines-1]=new Vector3d[Npoints];

    for(i=0;i<Npoints;i++)
    {
        PointsVec[Nlines-1][i]=Vector3d(xRear,0,0);
    }
}


PartGeometry::PartGeometry(const PartGeometry & PartIn)//Copy constructor
{
    int i;
    int j;
    name=PartIn.name;
    reversed=PartIn.reversed;
    LocSym=PartIn.LocSym;
    GlobSym=PartIn.GlobSym;
    Nlines=PartIn.Nlines;
    Npoints=PartIn.Npoints;
    idNumber=PartIn.idNumber;
    ErrorInt=PartIn.ErrorInt;
    PointsVec=new Vector3d* [Nlines];
    for(i=0;i<Nlines;i++)
    {
        PointsVec[i]=new Vector3d[Npoints];
    }

    for(i=0;i<Nlines;i++)
    {
        for(j=0;j<Npoints;j++)
        {
            PointsVec[i][j]=PartIn.PointsVec[i][j];
        }
    }

    Rotate=PartIn.Rotate;
    Scale=PartIn.Scale;
    Translate=PartIn.Translate;
}



PartGeometry::PartGeometry(const PartAnalysis & SplitPart,int Type)//Constructor for splitting a part
        //in: Part that is to be split, type (0 for front, 1 for back)
{
    ErrorInt=SplitPart.ErrorInt;
    int i;
    int j;
    reversed=SplitPart.reversed;
    LocSym=SplitPart.LocSym;
    GlobSym=SplitPart.GlobSym;
    idNumber=SplitPart.idNumber;
    if (SplitPart.WGStype==0)
    {
        name=SplitPart.name;
        Nlines=SplitPart.Nlines;
        if (Type==0)
        {
            name.append("-front");
            Npoints=SplitPart.SplitIndexTop-SplitPart.SplitIndexBottom+2;
        }
        else if(Type==1)
        {
            name.append("-rear");
            Npoints=SplitPart.Npoints-(SplitPart.SplitIndexTop-SplitPart.SplitIndexBottom)-1;
        }
        PointsVec=new Vector3d* [Nlines];
        for(i=0;i<Nlines;i++)
        {
            PointsVec[i]=new Vector3d[Npoints];
        }
        int index;
        for(i=0;i<Nlines;i++)
        {
            for(j=0;j<Npoints;j++)
            {
                if(Type==0)
                    index=j+SplitPart.SplitIndexBottom;
                else if(Type==1)
                {
                    index=(j+SplitPart.SplitIndexTop)+1;
                    if (index>=SplitPart.Npoints)
                    {
                        index=(index+1)%(SplitPart.Npoints);
                    }
                }
                PointsVec[i][j]=SplitPart.PointsVec[i][index];
            }
        }
    }
    else if(SplitPart.WGStype==1)
    {
        name=SplitPart.name;
        Npoints=SplitPart.Npoints;
        if (Type==0)//Front part
        {
            name.append("-front");
            Nlines=SplitPart.SplitIndex+1;
        }
        else if (Type==1)//Rear part
        {
            Nlines=SplitPart.Nlines-SplitPart.SplitIndex;
            name.append("-rear");
        }

        PointsVec=new Vector3d* [Nlines];
        for(i=0;i<Nlines;i++)
        {
            PointsVec[i]=new Vector3d[Npoints];
        }
        int index;
        for(i=0;i<Nlines;i++)
        {
            for(j=0;j<Npoints;j++)
            {
                if(Type==0)
                   index=i;
                else if(Type==1)
                    index=i+SplitPart.SplitIndex;
                PointsVec[i][j]=SplitPart.PointsVec[index][j];
            }
        }
    }
    if(Type==0)
        BluntFront=1;
    else if(Type==1)
        BluntFront=0;
}

void PartGeometry::deAllocateArrays()
{
    int j;
    for(j=0;j<Nlines-1;j++)
    {
        delete[] PointsVec[j];
        delete[] Centroid[j];
        delete[] Normal[j];
        delete[] Area[j];
        delete[] Cp[j];
        delete[] theta[j];


    }
    delete PointsVec[Nlines-1];
    delete[] PointsVec;
    delete[] Centroid;
    delete[] Normal;
    delete[] Area;
    delete[] Cp;
    delete[] theta;

}

void PartGeometry::Mirror(int idType)
{
    int i;
    int j;
    name.append(" -mirrored");

    reversed=-1*reversed;
    int idSym;
    if(idType==0)
        idSym=LocSym;
    else
        idSym=GlobSym;

    Matrix3d MirrorMat;
    if(idSym==1)
    {
        MirrorMat<<1,0,0,0,-1,0,0,0,1;
    }
    else if(idSym==2)
    {
        MirrorMat<<1,0,0,0,1,0,0,0,-1;
    }
    else if(idSym==3)
    {
        MirrorMat<<-1,0,0,0,1,0,0,0,1;
    }
    Vector3d test;
    for(i=0;i<Nlines;i++)
    {
        for(j=0;j<Npoints;j++)
        {

            PointsVec[i][j]=MirrorMat*PointsVec[i][j];

        }
    }
}

void PartGeometry::PanelCalculations()//Calculates the panel centroid, normal and areas; initializes Cp and inclination angles to 0
{
    int i;
    int j;
    Centroid=new Vector3d* [Nlines-1];
    Normal=new Vector3d* [Nlines-1];
    Area=new double* [Nlines-1];
    Cp=new double* [Nlines-1];
    theta=new double* [Nlines-1];

    for(i=0;i<Nlines-1;i++)
    {
        Centroid[i]=new Vector3d[Npoints-1];
        Normal[i]=new Vector3d[Npoints-1];
        Area[i]=new double[Npoints-1];
        Cp[i]=new double [Npoints-1];
        theta[i]=new double [Npoints-1];
    }
    Vector3d P31;
    Vector3d P42;
    Vector3d testnorm;
    for(i=0;i<Nlines-1;i++)
    {
        for(j=0;j<Npoints-1;j++)
        {
            Centroid[i][j]=(PointsVec[i][j]+PointsVec[i+1][j]+PointsVec[i][j+1]+PointsVec[i+1][j+1])/4;
            P31=PointsVec[i+1][j+1]-PointsVec[i][j];
            P42=PointsVec[i+1][j]-PointsVec[i][j+1];
            Normal[i][j]=P42.cross(P31);
            Area[i][j]=Normal[i][j].norm();
            if(Area[i][j]==0)
            {
                ErrorInt=1;
            }
            Normal[i][j]=reversed*Normal[i][j]/(Area[i][j]);
            Area[i][j]=0.5*Area[i][j];

            Cp[i][j]=0;
            theta[i][j]=0;
        }
    }
}

void PartGeometry::TransformPart()//Performs rotation, trnaslation and scaling operations per the the .wgs header informatioj
{
    int i;
    int j;
    for(i=0;i<Nlines;i++)
    {
        for(j=0;j<Npoints;j++)
        {
            PointsVec[i][j]=Scale*(Rotate*PointsVec[i][j]+Translate);
        }
    }
}


void PartGeometry::DetermineWGStype()//Determines whether the part is of the planar or fusiform .wgs type
{
    int i;
    double XmeanLine;
    double YmeanLine;
    double ZmeanLine;
    double MeanLineVarianceX=0;
    double MeanLineVarianceY=0;
    double MeanLineVarianceZ=0;

    for(i=0;i<Nlines;i++)
    {
        XmeanLine=LineMean(0,i);
        YmeanLine=LineMean(1,i);
        ZmeanLine=LineMean(2,i);
        MeanLineVarianceX+=LineVariance(0,i,XmeanLine);
        MeanLineVarianceY+=LineVariance(1,i,YmeanLine);
        MeanLineVarianceZ+=LineVariance(2,i,ZmeanLine);
    }
    MeanLineVarianceX=MeanLineVarianceX/Nlines;
    MeanLineVarianceY=MeanLineVarianceY/Nlines;
    MeanLineVarianceZ=MeanLineVarianceZ/Nlines;
    if(MeanLineVarianceX<MeanLineVarianceY && MeanLineVarianceX<MeanLineVarianceZ) //If smallest variations on a contour, averaged over all
                                                                                   //contours occurs in x-direction, part is determined as fusiform
        WGStype=1;//Fusiform
    else
        WGStype=0;//Planar
}


double PartGeometry::PointMean(int Component, int Index) //Determines the mean value of the Component-th component (0=x, 1=y, 2=z)
                                                         //of the index-th point on each contour, averaged over all contours
{
    int i;
    double mean=0;
    Vector3d Point;
    for(i=0;i<Nlines;i++)
    {
        Point=PointsVec[i][Index];
        mean=mean+Point(Component);
    }
    mean=mean/Nlines;
    return mean;
}

double PartGeometry::LineMean(int Component, int Line) //Determines the mean value of the Component-th component (0=x, 1=y, 2=z)
                                                        //of the Line-th contour, averaged over all points on the contour
{
    double mean=0;
    Vector3d Point;
    for(int i=0;i<Npoints;i++)
    {
        Point=PointsVec[Line][i];
        mean=mean+Point(Component);
    }
    mean=mean/Npoints;
    return mean;
}

double PartGeometry::LineVariance(int Component, int Line, double MeanLine)
{
    double Variance=0;
    Vector3d Point;
    for(int i=0;i<Npoints;i++)
    {
        Point=PointsVec[Line][i];
        Variance=Variance+pow((Point(Component)-MeanLine),2.0);
    }
    Variance=Variance/Npoints;
    return Variance;
}
/*
int PartGeometry::ProjPanelOrientation(int i, int j)
{
    int CCW;
    Vector3d P1;
    Vector3d P2;
    Vector3d P3;
    P1=PointsVec[i][j];
    P2=PointsVec[i][j+1];
    P3=PointsVec[i+1][j+1];
    double alpha=atan2((P2.z()-P1.z()),(P2.y()-P1.y()));
    double zTest1=-sin(alpha)*P2.y()+cos(alpha)*P2.z();
    double zTest2=-sin(alpha)*P3.y()+cos(alpha)*P3.z();
    if(zTest2>zTest1)
        CCW=1;
    else
        CCW=-1;
    return CCW;

}
*/
bool PartGeometry::PointsMatch(Vector3d Point1, Vector3d Point2)//Functions to determine if two points are 'equal' to an accuracy of 1E-6 times the length of the part
{
    bool out;
    double margin=(xmax-xmin)/1E6;
    double diff1=std::abs(Point1.x()-Point2.x());
    double diff2=std::abs(Point1.y()-Point2.y());
    double diff3=std::abs(Point1.z()-Point2.z());
    if(diff1<margin && diff2<margin && diff3<margin)
        out=1;
    else
        out=0;
    return out;
}


PartAnalysis::PartAnalysis()
{

}

PartAnalysis::PartAnalysis(const PartGeometry & PartToAnalyze,const SelectionStruct * Parameters):PartGeometry(PartToAnalyze)//constructor for part analysis, inherits copy constructor for partgeometry
{
    WGStype=PartToAnalyze.WGStype;
    DetermineXbounds();
    PanelCalculations();
    C1=Parameters->C[0];//sta::Pi()/3
    C2=Parameters->C[1];
    C3=Parameters->C[2];
    C4=Parameters->C[3];
    C5=Parameters->C[4];
    C6=Parameters->C[5];
    k1=Parameters->k[0];
    k2=Parameters->k[1];
    k3=Parameters->k[2];

}

PartAnalysis::~PartAnalysis()
{
    int i;
    for(i=0;i<LCClines;i++)
    {
        delete [] LatCC[i];
        delete [] LineProj[i];
        delete [] alphaSel[i];
    }
    delete [] LatCC;
    delete [] LineProj;
    delete [] alphaSel;
    delete [] xMean;
}

PlanarPartAnalysis::PlanarPartAnalysis(const PartGeometry & PartToAnalyze,const SelectionStruct * Parameters):PartAnalysis(PartToAnalyze,Parameters)//Constructor for planarPartAnalysis,
            //inherits constrcutor for PartAnalysis, which inherits copy constructor for PartGeometry
{
    ErrorInt=PartToAnalyze.ErrorInt;
    int i;
    int j;
    LCClines=Npoints-1;
    LCCpoints=Nlines-1;
    //Allocate all variables:
    xMean=new double[LCClines];
    LatCC=new Vector3d* [LCClines];
    for(i=0;i<LCClines;i++)
    {
        LatCC[i]=new Vector3d [LCCpoints];
    }
    LineProj=new double*[LCClines];
    alphaSel=new double*[LCClines];
    for(i=0;i<LCClines;i++)
    {
        LineProj[i]=new double[LCCpoints];
        alphaSel[i]=new double[LCCpoints];
    }

    Order=new int[LCClines];
    InverseOrder=new int[LCClines];
    int Index;
    OrderPlanarLCCs();

    for(j=0;j<LCClines;j++)
    {
        for(i=0;i<LCCpoints;i++)
        {
            Index=Order[j];
            LatCC[j][i]=Centroid[i][Index];
        }

    }
    DetermineProjectedQuantities();
    SetInclination();

}

PlanarPartAnalysis::~PlanarPartAnalysis()
{
    delete [] Order;
    delete [] InverseOrder;
    delete [] TopOrder;
    delete [] BottomOrder;
}

FusiformPartAnalysis::FusiformPartAnalysis(const PartGeometry & PartToAnalyze,const SelectionStruct * Parameters):PartAnalysis(PartToAnalyze, Parameters)
{
    ErrorInt=PartToAnalyze.ErrorInt;
    int i;
    int j;
    closed=1;
    LCClines=Nlines-1;
    LCCpoints=Npoints-1;
    xMean=new double[LCClines];
    LatCC=new Vector3d* [LCClines];
    for(i=0;i<Nlines-1;i++)
    {
        LatCC[i]=new Vector3d [Npoints-1];
    }

    LineProj=new double*[LCClines];
    alphaSel=new double*[LCClines];
    for(i=0;i<LCClines;i++)
    {
        LineProj[i]=new double[LCCpoints];
        alphaSel[i]=new double[LCCpoints];
    }

    for(i=0;i<Nlines-1;i++)
    {
        for(j=0;j<Npoints-1;j++)
        {
            LatCC[i][j]=Centroid[i][j];
        }
        xMean[i]=0.5*(LineMean(0,i)+LineMean(0,i+1));
    }
    DetermineProjectedQuantities();
    SetInclination();
}

FusiformPartAnalysis::~FusiformPartAnalysis()
{

}

void PartAnalysis::BluntDetermination()
{

}

void PlanarPartAnalysis::BluntDetermination()
{
        DetermineProjectedQuantities();
        PlanarBluntFront();
        if (BluntFront==1)
            DetermineXLow(TopOrder[0],BottomOrder[0]);
}

void FusiformPartAnalysis::BluntDetermination()
{
        DetermineProjectedQuantities();
        FusiformBluntFront();
        if (BluntFront==1)
            DetermineXLow(0);
}


void PartGeometry::DetermineXbounds()
{
    int i;
    int j;
    xmin=PointsVec[0][0].x();
    xmax=PointsVec[0][0].x();
    ixmin=0;
    jxmin=0;
    ixmax=0;
    jxmax=0;
    for(i=0;i<Nlines;i++)
    {
        for(j=0;j<Npoints;j++)
        {
            if(PointsVec[i][j].x()<xmin)
            {
                xmin=PointsVec[i][j].x();
                ixmin=i;
                jxmin=j;

            }
            else if(PointsVec[i][j].x()>xmax)
            {
                xmax=PointsVec[i][j].x();
                ixmax=i;
                jxmax=j;
            }
        }
    }
}




void PlanarPartAnalysis::OrderPlanarLCCs()
{
    int i;
    double MeanXArr[LCClines];
    double IndexArr[LCClines];
    for(i=0;i<LCClines;i++)
    {
        MeanXArr[i]=0.5*(PointMean(0,i)+PointMean(0,i+1));
        IndexArr[i]=i;
    }
    bool IsSorted=0;
    int tempInd;
    double tempMean;
    bool swapped;
    while(IsSorted==0)
    {
        i=0;
        swapped=0;
        while(i<LCClines-1)
        {
            if(MeanXArr[i+1]<MeanXArr[i])
            {
                tempInd=IndexArr[i];
                tempMean=MeanXArr[i];
                MeanXArr[i]=MeanXArr[i+1];
                IndexArr[i]=IndexArr[i+1];
                MeanXArr[i+1]=tempMean;
                IndexArr[i+1]=tempInd;
                if (swapped==0)
                    swapped=1;
            }
            i++;
        }
        if(swapped==0)
            IsSorted=1;
    }
    int Index;

    for(i=0;i<LCClines;i++)
    {
        Order[i]=IndexArr[i];
        xMean[i]=MeanXArr[i];
        Index=Order[i];
        InverseOrder[Index]=i;
    }
    DetermineOpenClosed();
    if(closed==1 || (closed==0 && FrontBack==0))
    {
        int topCount=1;
        int bottomCount=1;
        nBottom=jxmin;
        nTop=LCClines-nBottom;
        if(nBottom==0)
        {
            ErrorInt=2;
        }
        else if(nTop==0)
        {
            ErrorInt=3;
        }
        TopOrder=new int[nTop];
        BottomOrder=new int[nBottom];
        TopOrder[0]=InverseOrder[jxmin];
        BottomOrder[0]=InverseOrder[jxmin-1];
        for (int i=0;i<LCClines;i++)
        {
            if(Order[i]<(jxmin-1))
            {
                BottomOrder[bottomCount]=i;
                bottomCount++;
            }
            else if(Order[i]>jxmin)
            {
                TopOrder[topCount]=i;
                topCount++;
            }
        }
    }
    else if(closed==0 && FrontBack==1)
    {
        int topCount=0;
        int bottomCount=0;
        nTop=jxmax-1;
        nBottom=LCClines-nTop;
        TopOrder=new int[nTop];
        BottomOrder=new int[nBottom];
        //TopOrder[0]=0;
        //BottomOrder[0]=InverseOrder[jxmax];
        for (int i=0;i<LCClines;i++)
        {
            if(Order[i]>=jxmax-1)
            {
                BottomOrder[bottomCount]=i;
                bottomCount++;
            }
            else if(Order[i]<jxmax-1)
            {
                TopOrder[topCount]=i;
                topCount++;
            }
        }
    }

}


void FusiformPartAnalysis::SetInclination()
{
    Vector3d Vinf;

    double cosdel;
    Vinf=Vector3d(1,0,0);
    thetaSel=new double* [LCClines];
    for(int i=0;i<LCClines;i++)
    {
        thetaSel[i]=new double [LCCpoints];
    }
    for(int i=0;i<LCClines;i++)
    {
        for(int j=0;j<LCCpoints;j++)
        {
            cosdel=-1*Normal[i][j].dot(Vinf);
            thetaSel[i][j]=sta::Pi()/2-acos(cosdel);
        }
    }
}

void PlanarPartAnalysis::SetInclination()
{
    Vector3d Vinf;

    double cosdel;
    Vinf=Vector3d(1,0,0);
    thetaSel=new double* [LCClines];
    for(int i=0;i<LCClines;i++)
    {
        thetaSel[i]=new double [LCCpoints];
    }
    for(int i=0;i<LCClines;i++)
    {
        for(int j=0;j<LCCpoints;j++)
        {
            cosdel=-1*Normal[j][Order[i]].dot(Vinf);
            thetaSel[i][j]=sta::Pi()/2-acos(cosdel);
        }
    }
}


double PlanarPartAnalysis::DetermineBluntFraction(int n, bool BluntBool[])
{
    double meanL=0;
    double LineProjTot=0;
    for(int i=0;i<LCCpoints;i++)
    {
        meanL+=BluntBool[i]*LineProj[n][i];
        LineProjTot+=LineProj[n][i];
    }
    meanL/=LineProjTot;
    return meanL;

}

double PlanarPartAnalysis::AverageLatCCInclination(int n, bool BluntBool[])//Determines the average inclination of a given LatCC lines, weighted using panel areas.
                                                    //Only panels which are blunt on the front are considered for planar parts
{
    double meaninclin=0;
    double meanL=0;
    for(int i=0;i<LCCpoints;i++)
    {
        meaninclin+=BluntBool[i]*thetaSel[n][i]*LineProj[n][i];
        meanL+=BluntBool[i]*LineProj[n][i];
    }
    if (meanL==0)
        meaninclin=0;
    else
        meaninclin=meaninclin/meanL;
    return meaninclin;
}

double PartAnalysis::AverageLatCCInclination(int n)//Determines the average inclination of a given LatCC lines, weighted using panel areas.
{
    double meaninclin=0;
    double meanA=0;
    for(int i=0;i<LCCpoints;i++)
    {
        meaninclin+=thetaSel[n][i]*Area[n][i];
        meanA+=Area[n][i];
    }
    if (meanA==0)
        meaninclin==0;
    else
        meaninclin/=meanA;

    return meaninclin;
}

void PlanarPartAnalysis::DetermineProjectedQuantities()
{

    double y1;
    double y2;
    double z1;
    double z2;
        for(int i=0;i<LCClines;i++)
        {
            for(int j=0;j<LCCpoints;j++)
            {
                y1=0.5*(PointsVec[j][Order[i]].y()+PointsVec[j][Order[i]+1].y());
                y2=0.5*(PointsVec[j+1][Order[i]].y()+PointsVec[j+1][Order[i]+1].y());
                z1=0.5*(PointsVec[j][Order[i]].z()+PointsVec[j][Order[i]+1].z());
                z2=0.5*(PointsVec[j+1][Order[i]].z()+PointsVec[j+1][Order[i]+1].z());
                alphaSel[i][j]=atan2(z2-z1,y2-y1);
                LineProj[i][j]=sqrt((pow(y2-y1,2.0))+pow(z2-z1,2.0));
            }
        }


}


void FusiformPartAnalysis::DetermineProjectedQuantities()
{

    double y1;
    double y2;
    double z1;
    double z2;

        for(int i=0;i<LCClines;i++)
        {
            for(int j=0;j<LCCpoints;j++)
            {
                y1=0.5*(PointsVec[i][j].y()+PointsVec[i+1][j].y());
                y2=0.5*(PointsVec[i][j+1].y()+PointsVec[i+1][j+1].y());
                z1=0.5*(PointsVec[i][j].z()+PointsVec[i+1][j].z());
                z2=0.5*(PointsVec[i][j+1].z()+PointsVec[i+1][j+1].z());
                alphaSel[i][j]=atan2(z2-z1,y2-y1);
                LineProj[i][j]=sqrt((pow(y2-y1,2.0))+pow(z2-z1,2.0));
            }
       }
}

void PlanarPartAnalysis::PlanarBluntFront()
{
    int i;
    double MeanInclinationTop;
    double MeanInclinationBottom;
    double BluntFractionTop;
    double BluntFractionBottom;
    BluntFrontTopLines=new bool[LCCpoints];
    BluntFrontBottomLines=new bool[LCCpoints];
    FindBluntPanels(BluntFrontTopLines,TopOrder[0]);
    FindBluntPanels(BluntFrontBottomLines,BottomOrder[0]);
    MeanInclinationTop=AverageLatCCInclination(TopOrder[0],BluntFrontTopLines);
    BluntFractionTop=DetermineBluntFraction(TopOrder[0],BluntFrontTopLines);
    BluntFractionBottom=DetermineBluntFraction(BottomOrder[0],BluntFrontBottomLines);
    qDebug()<<BluntFractionTop<<BluntFractionBottom<<" blunt frac top bottom";
    qDebug()<<MeanInclinationTop<<"mean inc top"<<TopOrder[0];
    MeanInclinationBottom=AverageLatCCInclination(BottomOrder[0],BluntFrontBottomLines);
    qDebug()<<MeanInclinationBottom<<"mean inc bottom"<<BottomOrder[0];
    if (MeanInclinationTop<C3 || BluntFractionTop<C2/100)
        BluntFrontTop=0;
    else
        BluntFrontTop=1;

    if(MeanInclinationBottom<C3 || BluntFractionBottom<C2/100)
        BluntFrontBottom=0;
    else
        BluntFrontBottom=1;

    if(BluntFrontTop==0&&BluntFrontBottom==0)
        BluntFront=0;
    else
        BluntFront=1;
}

void PlanarPartAnalysis::FindBluntPanels(bool Blunt[], int Index)
{
    for(int i=0;i<LCCpoints;i++)
    {
        if(thetaSel[Index][i]>C3)
            Blunt[i]=1;
        else
            Blunt[i]=0;
    }
}

void FusiformPartAnalysis::FusiformBluntFront()
{
    double ThetaMean;
    ThetaMean=AverageLatCCInclination(0);
    if (ThetaMean>C1)
        BluntFront=1;
    else
        BluntFront=0;
}

void FusiformPartAnalysis::DetermineXLow(int i)
{
    bool found=0;
    double meantheta;
    int Index;

    while(found==0 && i<LCClines)
    {
        meantheta=AverageLatCCInclination(i);
        if (meantheta<C4)
            found=1;
        else
            i++;
    }
    xLowIndex=i;
    qDebug()<<xLowIndex<<"xlowindex";
}

void PlanarPartAnalysis::DetermineXLow(int iTop, int iBottom)
{
    bool found=0;
    bool topfound=0;
    bool bottomfound=0;
    if(BluntFrontTop==0 || iTop==(nTop-1))
        topfound=1;
    if(BluntFrontBottom==0 || iBottom==(nBottom-1))
        bottomfound=1;
    double meanthetaTop;
    double meanthetaBottom;
    int Index;
    int i=0;
    while(found==0 && i<LCClines)
    {
            Index=i;
            if(TopOrder[iTop]==Index && topfound==0)
            {
                meanthetaTop=AverageLatCCInclination(Index,BluntFrontTopLines);
                if (meanthetaTop<C4)
                {
                    topfound=1;
                    xLowTopIndex=iTop;

                }
                else
                    iTop++;
            }
            else if(BottomOrder[iBottom]==Index && bottomfound==0)
            {
                meanthetaBottom=AverageLatCCInclination(Index,BluntFrontBottomLines);
                if (meanthetaBottom<C4)
                {
                    bottomfound=1;
                    xLowBottomIndex=iBottom;
                }
                else
                    iBottom++;
            }

            else
                i++;
    }
    qDebug()<<xLowBottomIndex<<xLowTopIndex<<"x low bottom top index";
}

void PartAnalysis::DetermineSplitPoint(bool ConcavityIgnore)
{
}

void FusiformPartAnalysis::DetermineSplitPoint(bool ConcavityIgnore)
{
    double xMinMean=xMean[ixmin];
    double MeanxLow=xMean[xLowIndex];
    qDebug()<<MeanxLow<<" x low point";
    double SplitPoint=xMinMean+C5*(MeanxLow-xMinMean);
    SplitIndex=BestLineIndexApproximation(SplitPoint);
    qDebug()<<SplitPoint<<"split point"<<SplitIndex<<" split index";
    if(SplitIndex!=-1 && ConcavityIgnore==0)
    {
        int ConcContour=CheckConcavities(SplitIndex);
        if(ConcContour!=-1)
        {
            DetermineXLow(ConcContour+1);
            MeanxLow=xMean[xLowIndex];
            SplitPoint=xMean[ConcContour]+C5*(MeanxLow-xMean[ConcContour]);
            SplitIndex=BestLineIndexApproximation(SplitPoint);
        }
    }
    if(SplitIndex==-1)
        split=0;
    else
        split=1;
}

void PlanarPartAnalysis::DetermineSplitPoint(bool ConcavityIgnore)
{
    int SplitPointError=-1;
    double xMinMean;
    if(closed!=0)
    {
        if(BluntFrontTop==1)
        {
            double MeanxLowTop=xMean[TopOrder[xLowTopIndex]];
            qDebug()<<MeanxLowTop<<"mean x low top";
            xMinMean=xMean[TopOrder[0]];
            double SplitPointTop=xMinMean+C5*(MeanxLowTop-xMinMean);
            SplitIndexTop=Order[BestLineIndexApproximation(SplitPointTop,1)];
            if(SplitIndexTop==-1)
                SplitPointError=1;

            if(SplitIndexTop!=-1)
            {
                int TopOrderSplit=OrderToTopOrder(InverseOrder[SplitIndexTop]);
                if(ConcavityIgnore==0)
                {
                    int TopConcContour=CheckConcavities(TopOrderSplit,1);
                    if(TopConcContour!=-1)
                    {
                        DetermineXLow(TopConcContour,nBottom-1);
                        MeanxLowTop=xMean[xLowTopIndex];
                        SplitPointTop=xMean[TopConcContour]+C5*(MeanxLowTop-xMean[TopConcContour]);
                        SplitIndexTop=Order[BestLineIndexApproximation(SplitPointTop,1)];
                    }

                }
            }

        }
        else
            SplitIndexTop=Order[TopOrder[0]];
        if(BluntFrontBottom==1)
        {
            double MeanxLowBottom=xMean[BottomOrder[xLowBottomIndex]];
            qDebug()<<MeanxLowBottom<<"mean x low bottom";
            xMinMean=xMean[BottomOrder[0]];
            double SplitPointBottom=xMinMean+C5*(MeanxLowBottom-xMinMean);
            SplitIndexBottom=Order[BestLineIndexApproximation(SplitPointBottom,0)];
            if (SplitIndexBottom==-1)
                SplitPointError==2;

            if(SplitIndexBottom!=-1)
            {
                int BottomOrderSplit=OrderToTopOrder(InverseOrder[SplitIndexBottom]);
                if(ConcavityIgnore==0)
                {
                    int BottomConcContour=CheckConcavities(BottomOrderSplit,0);
                    if(BottomConcContour!=-1)
                    {
                        DetermineXLow(nTop-1,BottomConcContour);
                        MeanxLowBottom=xMean[xLowBottomIndex];
                        SplitPointBottom=xMean[BottomConcContour]+C5*(MeanxLowBottom-xMean[BottomConcContour]);
                        SplitIndexBottom=Order[BestLineIndexApproximation(SplitPointBottom,0)];
                    }
                }
            }

        }
        else
            SplitIndexBottom=Order[BottomOrder[0]];
        if(SplitIndexTop==-1&&SplitIndexBottom==-1)
            split=0;
        else if(SplitIndexTop==-1 && SplitIndexBottom!=-1)
        {
            SplitIndexTop=Order[TopOrder[nTop-1]];
            split=1;
        }
        else if(SplitIndexTop!=-1 && SplitIndexBottom==-1)
        {
            SplitIndexBottom=Order[BottomOrder[nBottom-1]];
            split=1;
        }
        else if(SplitIndexTop!=-1 && SplitIndexBottom!=-1)
            split=1;
        qDebug()<<SplitIndexTop<<"split index top";
        qDebug()<<SplitIndexBottom<<"split index bottom";

    }
    else
        split=0;
}

int PlanarPartAnalysis::OrderToTopOrder(int OrderIndex)
{
    int i=0;
    bool found=0;
    while(found==0&&i<nTop)
    {
        if(TopOrder[i]==OrderIndex)
            found=1;
        i++;
    }
    return i;
}

int PlanarPartAnalysis::OrderToBottomOrder(int OrderIndex)
{
    int i=0;
    bool found=0;
    while(found==0&&i<nBottom)
    {
        if(BottomOrder[i]==OrderIndex)
            found=1;
        i++;
    }
    return i;
}

void PlanarPartAnalysis::DetermineOpenClosed()
{
    closed=-1;//Initialize to nonsense value
    int i;
    for(i=0;i<Nlines;i++)
    {
        if(PointsMatch(PointsVec[i][0],PointsVec[i][Npoints-1])==0)
        {
            closed=0;
        }
    }
    if(closed==-1)
        closed=1;
    if(closed==0)
        DetermineFrontBack();
}

void PlanarPartAnalysis::DetermineFrontBack()
{
    int i;
    int j;
    double dx;
    double dxnew;
    int SignChange;
    int FrontBackArr[Nlines];
    for(i=0;i<Nlines;i++)
    {
        FrontBackArr[i]=0.5;//Initialize with 'neutral'value (see end of function)
    }
    for(i=0;i<Nlines;i++)
    {
        SignChange=0;
        for(j=0;j<Npoints-1;j++)
        {
            dxnew=PointsVec[i][j+1].x()-PointsVec[i][j].x();
            if(j!=0 && (sign(dx)!=sign(dxnew)))
            {
                SignChange++;
                if(dx>=0 && dxnew <0)
                {
                    FrontBackArr[i]=1;//Back
                }
                else
                {
                    FrontBackArr[i]=0;
                }
            }
            dx=dxnew;

        }
    }
    double test=0;
    for(int i=0;i<Nlines;i++)
    {
        test+=FrontBackArr[i];
    }
    test/=Nlines;
    if(test>0.5)//Majority wins, should be (near) unanimous for proper geometries..
        FrontBack=1;
    else
        FrontBack=0;
}


int PartAnalysis::BestLineIndexApproximation(double xValue)//returns the index of the LCC line for which the mean x value ios closest to the input value, returns -1 if behind the rearmost line
{
    bool found=0;
    double MinDiff=std::abs(xMean[0]-xValue);
    double NewMinDiff;
    int i=0;
    int iout;
    while(found==0 && i<(LCClines-1))
    {
        NewMinDiff=std::fabs(xMean[i]-xValue);
        if(NewMinDiff<=MinDiff)
        {
            MinDiff=NewMinDiff;
            iout=i;
        }
        else
            found=1;
        i++;
    }
    if(found==0)
        iout=-1;
    return iout;
}

int PlanarPartAnalysis::BestLineIndexApproximation(double xValue,int TopBottom)//returns the index of the LCC line for which the mean x value ios closest to the input value, returns -1 if behind the rearmost line                                                    //overloaded to allow search only at top or bottom, TopBottom is 1 for top, 0 for bottom
{
    bool found=0;
    double MinDiff=std::abs(xMean[0]-xValue);
    double NewMinDiff;
    int i=0;
    int iout;
    int iTop=0;
    int iBottom=0;
    if(TopBottom==1)
    {
        while(found==0 && i<(LCClines-1))
        {
            if(TopOrder[iTop]==i)
            {
                NewMinDiff=std::abs(xMean[i]-xValue);
                if(NewMinDiff<=MinDiff)
                {
                    MinDiff=NewMinDiff;
                    iout=i;
                }
                else
                    found=1;
                iTop++;
            }
            i++;
        }
    }
    else if(TopBottom==0)
    {
        while(found==0 && i<(LCClines-1))
        {
            if(BottomOrder[iBottom]==i)
            {
                NewMinDiff=std::abs(xMean[i]-xValue);
                if(NewMinDiff<=MinDiff)
                {
                    MinDiff=NewMinDiff;
                    iout=i;
                }
                else
                    found=1;
                iBottom++;
            }
            i++;
        }
    }
    i++;
    if(found==0)
        iout=-1;
    return iout;
}

void PartAnalysis::DetermineCurvatureType()
{
    int i;
    double LineCurvatures=0;
    for(i=0;i<LCClines;i++)
    {
        LineCurvatures+=DetermineLineCurvatureType(i);
    }
    LineCurvatures/=LCClines;
    if (LineCurvatures>k2/100)
        CurvatureType=1;
    else
        CurvatureType=0;
}


bool PartAnalysis::DetermineLineCurvatureType(int n)
{
    double alphaMaxDiff;

    alphaMaxDiff=LineAlphaChange(n);
    if (alphaMaxDiff<k1)
        return 0;
    else
    {
        double CurvedFraction=0;
        double dalphadlp;
        bool curved;
        double LineProjTot=0;
        for(int i=0;i<LCCpoints-1;i++)//Modify to include closed contours!
        {
            LineProjTot+=LineProj[n][i];
        }
        for(int i=0;i<LCCpoints-1;i++)
        {   dalphadlp=(std::abs(alphaSel[n][i]-alphaSel[n][i+1]))/(LineProj[n][i+1]+LineProj[n][i])*2*LineProjTot;

            if(std::abs(alphaSel[n][i]<0-alphaSel[n][i+1])>1.5*sta::Pi())//Dirty Fix, not very robust, change!!!
            {
                dalphadlp=(std::abs(alphaSel[n][i]-alphaSel[n][i+1]+2*sta::Pi()))/(LineProj[n][i+1]+LineProj[n][i])*2*LineProjTot;
            }
            if (dalphadlp>k3)
                CurvedFraction+=LineProj[n][i];
        }
        CurvedFraction/=LineProjTot;
        if(CurvedFraction>k2/100)
            curved=1;
        else
            curved=0;
        return curved;
    }
}

double PartAnalysis::LineAlphaChange(int n)
{
    double alphamin=alphaSel[n][0];
    double alphamax=alphaSel[n][0];
    double alphaChange;
    int i;
    for(i=1;i<LCCpoints;i++)
    {
        if(alphaSel[n][i]>alphamax)
            alphamax=alphaSel[n][i];
        else if(alphaSel[n][i]<alphamin)
            alphamin=alphaSel[n][i];
    }
    alphaChange=alphamax-alphamin;
    return alphaChange;


}

int PartAnalysis::sign(double n)
{
    int out;
    if(n>=0)
        out=1;
    else
        out=-1;
    return out;
}

int PartAnalysis::CheckConcavities()
{
}


int FusiformPartAnalysis::CheckConcavities(int StartingLine)
{
    double xMinMean=xMean[ixmin];
    double xMaxMean=xMean[ixmax];
    int ConcLine=-1;
    int i;
    double dth;
    double xstop=xMinMean+C6*(xMaxMean-xMinMean);
    int ContourCheck=StartingLine;
    while(xMean[ContourCheck]<xstop && ContourCheck<LCClines)
    {
        for(i=0;i<LCCpoints;i++)
        {
            dth=thetaSel[ContourCheck][i]-thetaSel[ContourCheck+1][i];
            if(dth<-0.1*sta::Pi()/180)//Can be slightly smaller than zero due to machine accurcay etc
                ConcLine=ContourCheck;
        }
        ContourCheck++;

    }
    return ConcLine;
}

int PlanarPartAnalysis::CheckConcavities(int StartingContour, int TopBottom)//Input index in TopOrder or BottomOrder, and TopBottom (1 for top, 0 for bottom)
{
    int ConcLine=-1;
    int i;
    double dth;
    double xMinMean=0.5*(xMean[TopOrder[0]]+xMean[BottomOrder[0]]);
    double xMaxMean=0.5*(xMean[TopOrder[nTop-1]]+xMean[BottomOrder[nBottom-1]]);
    double xstop=xmin+C6*(xMaxMean-xMinMean);//This function is only called for a closed contour, so no need to worry about different xmin/xmax for top/bottom
    int ContourCheck;
    int ContourCheck2;
    if (TopBottom==1)
    {
        int TopContour=StartingContour;
        ContourCheck=TopOrder[TopContour];
        while(xMean[ContourCheck]<xstop && ContourCheck!=TopOrder[nTop-1])
        {
            ContourCheck=TopOrder[TopContour];
            ContourCheck2=TopOrder[TopContour+1];
            for(i=0;i<LCCpoints;i++)
            {
                dth=thetaSel[ContourCheck][i]-thetaSel[ContourCheck2][i];
                if(dth<0)
                    ConcLine=ContourCheck;
            }
            TopContour++;
        }
    }
    else if(TopBottom==0)
    {
        int BottomContour=StartingContour;
        ContourCheck=BottomOrder[BottomContour];
        while(xMean[ContourCheck]<xstop && ContourCheck!=BottomOrder[nBottom-1])
        {
            ContourCheck=BottomOrder[BottomContour];
            ContourCheck2=BottomOrder[BottomContour+1];
            for(i=0;i<LCCpoints;i++)
            {
                dth=thetaSel[ContourCheck][i]-thetaSel[ContourCheck2][i];
                if(dth<0)
                    ConcLine=ContourCheck;
            }
            BottomContour++;
        }
    }
    return ConcLine;
}



