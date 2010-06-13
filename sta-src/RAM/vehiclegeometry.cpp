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

#include <string.h>
#include "vehiclegeometry.h"
#include "Astro-Core/stamath.h"
#include <QDebug>

using std::string;
using std::stringstream;

VehicleGeometry::VehicleGeometry()
{
}

VehicleGeometry::~VehicleGeometry()
{
    int i;
    for(i=0;i<nParts;i++)
    {
       PartsOut[i].deAllocateArrays();
    }
    delete [] PartsOut;


}

VehicleGeometry::VehicleGeometry(QString path)
{
    Filename=string((const char*)path.toLocal8Bit());
    wgsModified=0;
    QFile geom(path);
    geom.open(QIODevice::ReadOnly);
    QTextStream firststream(&geom);
    GeometryDescription=firststream.readLine();
    readNlines=new int[1];
    readNpoints=new int[1];
    int i;
    nParts=0;
    int LocSymCount=0;
    int GlobSymCount=0;
    int TempLoc;
    int TempGlob;
    double temp;
    while(!firststream.atEnd())//First read through the geometry file to determine amount of vehicle parts and mirrorings
    {
        firststream.readLine();
        firststream >> temp;
        if (nParts!=0)//Dynamically increase size of line and point size readers
        {
            readNlines = (int *) realloc (readNlines,(nParts+1) * sizeof(int));
            readNpoints= (int *) realloc (readNpoints,(nParts+1) * sizeof(int));
        }
        firststream>>readNlines[nParts];
        firststream>>readNpoints[nParts];
        firststream>>TempLoc;
        if(TempLoc!=0)//Count local symmetries
            LocSymCount++;
        for(i=0;i<9;i++)//Ignore next nine header variables
            firststream>>temp;
        firststream>>TempGlob;
        if(TempGlob!=0)//Count global symmetries
            GlobSymCount++;
        if(TempGlob!=0 && TempLoc!=0)//If a part has local and global symmetry, one part will yield *two* globally mirrored parts.
            GlobSymCount++;
        firststream.readLine();
        for(i=0;i<3*readNlines[nParts]*readNpoints[nParts];i++)//Ignore vehicle points for now
        {
            firststream>>temp;
        }
        nParts++;
        firststream.readLine();
        firststream>>temp;
    }
    PartGeometry* Parts;
    Parts=new PartGeometry[nParts+LocSymCount+GlobSymCount];//Create array of Part objects.
    geom.close();

    int j;
    int k=0;
    int l;
    QString partname;
    int HeaderInts[5];//Each part header contains 5 integers: id, # of lines, # of points, local symmetry identifier, global symmetry identifier
    double HeaderDoubles[9];//Each part header contains 9 doubles: rotation about x,y,z axes; translation in x,y,z direction; scaling in x,y,z direction
    int GlobSym[nParts];
    int LocSym[nParts];

    geom.open(QIODevice::ReadOnly);
    QTextStream pointstream(&geom);

    pointstream.readLine();//Ignore vehicle description

    for(k=0;k<nParts;k++)//read parts names and header info; points and create vehicle parts. Also separately store local and global symmetry ids
    {
        partname=pointstream.readLine();
        for(i=0;i<4;i++)
        {
            pointstream>>HeaderInts[i];
        }
        for(i=0;i<9;i++)
        {
            pointstream>>HeaderDoubles[i];
        }
        pointstream>>HeaderInts[4];

        pointstream.readLine();
        l=0;
        double * Points= new double[3*readNlines[k]*readNpoints[k]];//read points of a single part into array
        for(i=0;i<readNlines[k];i++)
        {
            for(j=0;j<readNpoints[k];j++)
            {
               pointstream>>Points[l];
               pointstream>>Points[l+1];
               pointstream>>Points[l+2];
               l=l+3;
            }
        }
        Parts[k]=PartGeometry(Points,partname,HeaderInts,HeaderDoubles);//Create part and put into Parts-array
        delete [] Points;
        LocSym[k]=HeaderInts[3];
        GlobSym[k]=HeaderInts[4];
        pointstream.readLine();
    }
    delete [] readNlines;
    delete [] readNpoints;

    int Nloc=0;
    int Nglob=0;
    for(k=0;k<nParts;k++)//Mirror all local symmetry parts
    {
        if(LocSym[k]!=0)
        {
            Parts[nParts+Nloc]=PartGeometry(Parts[k]);
            Parts[nParts+Nloc].Mirror(0);
            Nloc++;
        }
    }
    nParts=nParts+LocSymCount;//Number of parts has increased
    for(k=0;k<nParts;k++)//Perform all rotation, translation and scaling operations
    {
        Parts[k].TransformPart();
    }
    for(k=0;k<nParts-LocSymCount;k++)//Mirror all global symmetry parts
    {
        if(GlobSym[k]!=0)
        {
            Parts[nParts+Nglob]=PartGeometry(Parts[k]);
            Parts[nParts+Nglob].Mirror(1);
            Nglob++;
        }
    }
    nParts=nParts+GlobSymCount;
    for(k=0;k<nParts;k++)//Determine lcoal vehicle characteristics
    {
        Parts[k].PanelCalculations();
    }
    PartsOut=new PartGeometry[nParts];
    for(int i=0;i<nParts;i++)
    {
        PartsOut[i]=PartGeometry(Parts[i]);
        PartsOut[i].PanelCalculations();
        Parts[i].deAllocateArrays();
    }
    delete [] Parts;
    while(!pointstream.atEnd())//just to make sure
        pointstream.readLine();
    geom.close();//close

}

VehicleGeometry::VehicleGeometry(const VehicleGeometry & vehicle)
{
    int i;
    nParts=vehicle.nParts;
    PartsOut=new PartGeometry[nParts];
    for(int i=0;i<nParts;i++)
    {
        PartsOut[i]=PartGeometry(vehicle.PartsOut[i]);
        PartsOut[i].PanelCalculations();
    }
    Volume=vehicle.Volume;
    Surface=vehicle.Surface;
    NoseRadius=vehicle.NoseRadius;
    Filename=vehicle.Filename;

}


VehicleGeometry::VehicleGeometry(int VehicleType, double* params, int Nlines, int Npoints)//Creates a paramtrized vehicle
        //vehicle type and parameters (see STA-TUD-TN-1001)
{
    Vector3d** VehiclePoints;
    //int Nlines=10;//Number of contours, change for finer mesh
    //int Npoints=11;//Number of points/contour, change for finer mesh
    int i;
    stringstream temp;

    switch(VehicleType)//Make vehicle points and contours, also make filename for .wgs
    {
    case 0: VehiclePoints=MakeBluntedCone(params,Nlines,Npoints);
        temp<<"BlCone"<<params[0]<<"_"<<params[1]<<"_"<<params[2]*180/sta::Pi()<<".wgs";
            break;
    case 1: VehiclePoints=MakeCapsule(params,Nlines,Npoints);
        temp<<"Capsule"<<params[0]<<"_"<<params[1]<<"_"<<params[2]<<"_"<<params[3]*180/sta::Pi()<<".wgs";
            break;
    case 2: VehiclePoints=MakeBiCone(params,Nlines,Npoints);
            if(params[4]>=0)
                temp<<"BiCone";
            else
                temp<<"Probe" ;
            temp<<params[0]<<"_"<<params[1]<<"_"<<params[2]<<"_"<<params[3]*180/sta::Pi()<<"_"<<params[4]*180/sta::Pi()<<".wgs";
            break;
    }
    Filename=temp.str();

    PartsOut=new PartGeometry[1];//So far, all paramterizations have just 1 vehicle part

    PartsOut[0]=PartGeometry(VehiclePoints,Nlines,Npoints);//create vehicle part

    nParts=1;
    PartsOut[0].PanelCalculations();

}

void VehicleGeometry::CalculateGlobalCharacteristics()
        //Sets the vehicle global variables surface area, volume and nose radius of the vehicle
{
    NoseRadius=0;
    Volume=0;
    Surface=0;
    int PartxMin=0;
    int LinexMin=0;
    int PointxMin=0;
    double xMinTot=PartsOut[0].PointsVec[0][0].x();

    Vector3d dsdt1;
    Vector3d d2sdt21;

    int i;
    int j;
    int k;

    for(k=0;k<nParts;k++)//triple for loop to go over all parts and add the individual contributions to surface and volume of each panel
                         //also find the most forward point for determination of the nose radius
    {
        for(i=0;i<PartsOut[k].Nlines-1;i++)
        {
            PartsOut[k].DetermineXbounds();
            if(PartsOut[k].xmin<xMinTot)
            {
                PartxMin=k;
                LinexMin=PartsOut[k].ixmin;
                PointxMin=PartsOut[k].jxmin;
                xMinTot=PartsOut[k].xmin;

            }
            for(j=0;j<PartsOut[k].Npoints-1;j++)
            {
                Surface=Surface+PartsOut[k].Area[i][j];
                Volume=Volume+PartsOut[k].Normal[i][j].dot(PartsOut[k].Centroid[i][j])*PartsOut[k].Area[i][j]/3;


            }
        }
    }

    Vector3d P0;
    Vector3d P1;
    Vector3d P2;
    bool ContourIsPoint=0;
    int PointCount=0;
    //Following determines if all forward points on most forward contours are at same point in space
    for(i=0;i<PartsOut[PartxMin].Npoints-1;i++)
    {
        if(PartsOut[PartxMin].PointsMatch(PartsOut[PartxMin].PointsVec[LinexMin][i],PartsOut[PartxMin].PointsVec[LinexMin][i+1])==1)
        {
            PointCount++;
        }
    }
    if(PointCount==PartsOut[PartxMin].Npoints-1)
        ContourIsPoint=1;
    if(ContourIsPoint==1)
    {
        P1=PartsOut[PartxMin].PointsVec[LinexMin][PointxMin];
    }

    int zMax=0;
    int zMin=0;
    for(i=0;i<PartsOut[PartxMin].Npoints;i++)//Find the maximum and minimum z-points on contour just aft of nose
    {
        if(PartsOut[PartxMin].PointsVec[1][i].z()>PartsOut[PartxMin].PointsVec[1][zMax].z())
            zMax=i;
        else if(PartsOut[PartxMin].PointsVec[1][i].z()<PartsOut[PartxMin].PointsVec[1][zMin].z())
            zMin=i;
    }

    P0=PartsOut[PartxMin].PointsVec[1][zMin];
    P2=PartsOut[PartxMin].PointsVec[1][zMax];
    dsdt1=0.5*(P2-P0);//Approximation of 1st derivative
    d2sdt21=P2-2*P1+P0;//Approximation of 2nd derivative
    NoseRadius=pow(dsdt1.norm(),3.0)/((dsdt1.cross(d2sdt21)).norm());//Nose radius approximation
}


Vector3d** VehicleGeometry::MakeBluntedCone(double* params, int Nlines, int Npoints)
        //in: Blunted Cone parameters, number of lines, number of points in blunted cone
        //out: matrix with points (analogous to PointsVec in PartGeometry) on vehicle
        //note: this function is also called by the MakeBiCone function
{
    //retrieve parameters from input
    double Rn=params[0];
    double Rb=params[1];
    double thc=params[2];
    double ths=sta::Pi()/2-thc;
    double R1=Rn*cos(thc);//See STA-TUD-TN-1001
    double R2=Rb;//See STA-TUD-TN-1001
    double MatchPoint=Rn-Rn*sin(thc);//See STA-TUD-TN-1001
    double xEnd=Rn-Rn*sin(thc)+(R2-R1)/tan(thc);
    int nxtot=Nlines;//total number of points in x-direction
    int nphi=Npoints;//total number of points in circumferential direction
    Vector3d** Front;
    Vector3d** Back;
    int nxfront=ceil(2*MatchPoint/xEnd*nxtot);//ceil(ths/(sta::Pi()/20));//sort of random, make more logical
    if (nxfront<2)
        nxfront=2;
    if (nxfront>nxtot-2)
        nxfront=nxtot-2;
    int nxback=nxtot-nxfront;
    Front=GeneratePartialSphere(Rn,ths,0,nxfront,nphi);//get points for spherical nose
    Back=GeneratePartialCone(R1,R2,thc,MatchPoint,nxback+1,nphi);//get points for conical rear
    Vector3d** BluntedCone;
    BluntedCone=new Vector3d*[nxtot];
    int i;
    int j;
    //The following combines the points from the spherical and conical parts
    for(i=0;i<nxtot;i++)
        BluntedCone[i]=new Vector3d[nphi];
    for(i=0;i<nxfront;i++)
    {
        for(j=0;j<nphi;j++)
            BluntedCone[i][j]=Front[i][j];
    }
    for(i=0;i<nxback;i++)
    {
        for(j=0;j<nphi;j++)
            BluntedCone[i+nxfront][j]=Back[i+1][j];//i+1 because last contour of spherical and first contour of conical parts are same
    }
    for(i=0;i<nxfront;i++)
        delete [] Front[i];
    for(i=0;i<nxback+1;i++)
        delete [] Back[i];
    delete [] Front;
    delete [] Back;
    return BluntedCone;
}
Vector3d** VehicleGeometry::MakeCapsule(double* params, int Nlines, int Npoints)
{
    double Rn=params[0];
    double Rb=params[1];
    double Re=params[2];
    double thb=params[3];
    double ths=asin(Rb/Rn);
    double MatchPoint=Rn*(1-sqrt(1-pow(Rb/Rn,2.0)));
    int nxtot=Nlines;
    int nphi=Npoints;
    Vector3d** Front;
    Vector3d** Back;
    int nxfront=ceil(0.75*nxtot);
    int nxback=nxtot-nxfront;
    Front=GeneratePartialSphere(Rn,ths,0,nxfront,nphi);
    Back=GeneratePartialCone(Rb,Re,-thb,MatchPoint,nxback+1,nphi);
    Vector3d** Capsule;
    Capsule=new Vector3d*[nxtot];
    int i;
    int j;
    for(i=0;i<nxtot;i++)
        Capsule[i]=new Vector3d[nphi];
    for(i=0;i<nxfront;i++)
    {
        for(j=0;j<nphi;j++)
            Capsule[i][j]=Front[i][j];
    }
    for(i=0;i<nxback;i++)
    {
        for(j=0;j<nphi;j++)
            Capsule[i+nxfront][j]=Back[i+1][j];
    }
    for(i=0;i<nxfront;i++)
        delete [] Front[i];
    for(i=0;i<nxback+1;i++)
        delete [] Back[i];
    delete [] Front;
    delete [] Back;
    return Capsule;


}
Vector3d** VehicleGeometry::MakeBiCone(double* params, int Nlines, int Npoints)
{
    double Rn=params[0];
    double Rb1=params[1];
    double Rb2=params[2];
    double thc1=params[3];
    double thc2=params[4];
    double BlConeIn[3]={Rn,Rb1,thc1};
    double MatchPoint=Rn*(1-sin(thc1)-cos(thc1)/tan(thc1))+Rb1/tan(thc1);
    int nxtot=Nlines;
    int nphi=Npoints;
    int nxfront=floor(0.75*nxtot);
    int nxback=nxtot-nxfront;
    Vector3d** Front;
    Vector3d** Back;
    Vector3d** BiCone;
    Front=MakeBluntedCone(BlConeIn,nxfront,Npoints);
    Back=GeneratePartialCone(Rb1,Rb2,thc2,MatchPoint,nxback+1,nphi);
    BiCone=new Vector3d*[nxtot];
    int i;
    int j;
    for(i=0;i<nxtot;i++)
        BiCone[i]=new Vector3d[nphi];
    for(i=0;i<nxfront;i++)
    {
        for(j=0;j<nphi;j++)
            BiCone[i][j]=Front[i][j];
    }
    for(i=0;i<nxback;i++)
    {
        for(j=0;j<nphi;j++)
            BiCone[i+nxfront][j]=Back[i+1][j];
    }
    for(i=0;i<nxfront;i++)
        delete [] Front[i];
    for(i=0;i<nxback+1;i++)
        delete [] Back[i];
    delete [] Front;
    delete [] Back;
    return BiCone;

}

Vector3d** VehicleGeometry::GeneratePartialCone(double R1, double R2, double thc, double xstart, int nx, int nphi)
{
    Vector3d** Points;
    Points=new Vector3d* [nx];
    int i;
    for(i=0;i<nx;i++)
        Points[i]=new Vector3d[nphi];

    Matrix3d RotateCurve;
    int j;
    double dx=(R2-R1)/(tan(thc)*(nx-1));
    double dphi=2*sta::Pi()/(nphi-1);
    for(i=0;i<nx;i++)
    {
        Points[i][0]=Vector3d(xstart+i*dx,R1+i*tan(thc)*dx,0);
    }
    for(j=1;j<nphi;j++)
    {
        RotateCurve=AngleAxisd(-j*dphi, Vector3d::UnitX());
        for(i=0;i<nx;i++)
        {
            Points[i][j]=RotateCurve*Points[i][0];
        }
    }
    return Points;
}

Vector3d** VehicleGeometry::GeneratePartialSphere(double R, double ths, double xstart, int nx, int nphi)
{

    Vector3d** Points;
    Points=new Vector3d* [nx];
    int i;
    for(i=0;i<nx;i++)
    {
        Points[i]=new Vector3d[nphi];
    }
    Matrix3d RotateCurve;
    int j;
    double dth=ths/(nx-1);
    double dphi=2*sta::Pi()/(nphi-1);
    for(i=0;i<nx;i++)
    {
        Points[i][0]=Vector3d(xstart+R*(1-cos(i*dth)),R*sin(i*dth),0);
    }
    for(j=1;j<nphi;j++)
    {
        RotateCurve=AngleAxisd(-j*dphi, Vector3d::UnitX());
        for(i=0;i<nx;i++)
        {
            Points[i][j]=RotateCurve*Points[i][0];
        }
    }
    return Points;
}


int VehicleGeometry::AutomaticMethodSelection(int** SelectedMethods, const PartAnalysis::SelectionStruct * SelectionParameters, bool ConcavityIgnore, int** defaultMethods)//Outputs selected aerodynamic methods, modifies geometry if necessary
{
    int ErrorMessage;
    int i,j;
    SplitCount=0;
    PartAnalysis * PartsAnArr[nParts];//Create array of PartAnalysis which perform analysis of each part,
    for(i=0;i<nParts;i++)
    {
        PartsOut[i].DetermineWGStype();//Analysis methods differ between fusiform and planar parts, distinguish between two
        if(PartsOut[i].WGStype==0)
        {
            PartsAnArr[i]=new PlanarPartAnalysis(PartsOut[i],SelectionParameters);
        }
        else if(PartsOut[i].WGStype==1)
        {
            PartsAnArr[i]=new FusiformPartAnalysis(PartsOut[i],SelectionParameters);
        }
        if (PartsAnArr[i]->ErrorInt!=0)
        {

            ErrorMessage=PartsAnArr[i]->ErrorInt;
            for(j=0;j<=i;j++)
            {
                delete PartsAnArr[j];
            }
            return ErrorMessage;

        }
        PartsAnArr[i]->BluntDetermination();//Determine if part is blunt

        if(PartsAnArr[i]->BluntFront==1 && PartsAnArr[i]->closed==1)//Don't split an open vehicle part
        {
            PartsAnArr[i]->DetermineSplitPoint(ConcavityIgnore);//Determine split point, set 'split' global variable of PartAnalysis
            if(PartsAnArr[i]->split==1)
            {
                SplitCount++;
            }
        }
        else
            PartsAnArr[i]->split=0;
    }
    int sp=0;
    bool SplitBoolArr[nParts];//Sets true/false whether part has been split
    double tempWGStype;
    if(SplitCount!=0)
    {
        PartsSplit=new PartGeometry[2*SplitCount];//create array of new vehicle parts, to be merged into new PartsOut later
    }
    for(i=0;i<nParts;i++)
    {
        if(PartsAnArr[i]->split==1)
        {
            PartsSplit[sp]=PartGeometry(*PartsAnArr[i],0);//create front part of split
            PartsSplit[sp].PanelCalculations();

            sp++;
            PartsSplit[sp]=PartGeometry(*PartsAnArr[i],1);//create rear part of split
            PartsSplit[sp].PanelCalculations();

            SplitBoolArr[i]=1;
            tempWGStype=PartsAnArr[i]->WGStype;
            delete PartsAnArr[i];//no longer needed, remove

            //the following only puts the rear part of the split into the PartsAnArr array, since only this will need to be further analyzed (curved/uncurved)
            if(tempWGStype==0)
                PartsAnArr[i]=new PlanarPartAnalysis(PartsSplit[sp],SelectionParameters);
            else if(tempWGStype==1)
                PartsAnArr[i]=new FusiformPartAnalysis(PartsSplit[sp],SelectionParameters);
            if (PartsAnArr[i]->ErrorInt!=0)
            {
                ErrorMessage= PartsAnArr[i]->ErrorInt;
                for(j=0;j<=nParts;j++)
                {
                    delete PartsAnArr[j];

                }
                return ErrorMessage;

            }
            sp++;

        }
        else
        {
            SplitBoolArr[i]=0;
            PartsOut[i].BluntFront=PartsAnArr[i]->BluntFront;//Set valuefor method selection later
        }
    }
    sp=1;//Reset for putting curvaturetypes in PartsSplit in next loop. necessary for making new PartsOut array in UpdateParts()

    for(i=0;i<nParts;i++)//Determine for each part (except fronts of split parts) whether it is curved/uncurved
    {
        PartsAnArr[i]->DetermineCurvatureType();
        if(PartsAnArr[i]->split==0)
            PartsOut[i].CurvatureType=PartsAnArr[i]->CurvatureType;
        else if(PartsAnArr[i]->split==1)
        {
            PartsSplit[sp].CurvatureType=PartsAnArr[i]->CurvatureType;//Rear parts, which ar to be analyzed, are in odd entries of PartsAnArr
            sp+=2;
        }
    }
    int oldnParts=nParts;

    UpdateParts(SplitBoolArr);//Updates the PartsOut array with new split parts
    for(i=0;i<oldnParts;i++)
    {
        delete PartsAnArr[i];
    }
    for(i=0;i<2;i++)
    {
        SelectedMethods[i]=new int[2*nParts];
    }
    for(i=0;i<nParts;i++)//Put in selected methods for High Hypersonic first
    {
        if (PartsOut[i].BluntFront==1)
        {
            SelectedMethods[0][i]=defaultMethods[0][0];
            SelectedMethods[1][i]=defaultMethods[0][1];
        }
        else if(PartsOut[i].CurvatureType==1)
        {
            SelectedMethods[0][i]=defaultMethods[1][0];
            SelectedMethods[1][i]=defaultMethods[1][1];

        }
        else if(PartsOut[i].CurvatureType==0)
        {
            SelectedMethods[0][i]=defaultMethods[2][0];
            SelectedMethods[1][i]=defaultMethods[2][1];
        }
    }
    for(i=nParts;i<2*nParts;i++)//Put in selecte methds for Low hypersonic next
    {
        if (PartsOut[i-nParts].BluntFront==1)
        {
            SelectedMethods[0][i]=defaultMethods[0][2];
            SelectedMethods[1][i]=defaultMethods[0][3];
        }
        else if(PartsOut[i-nParts].CurvatureType==1)
        {
            SelectedMethods[0][i]=defaultMethods[1][2];
            SelectedMethods[1][i]=defaultMethods[1][3];

        }
        else if(PartsOut[i-nParts].CurvatureType==0)
        {
            SelectedMethods[0][i]=defaultMethods[2][2];
            SelectedMethods[1][i]=defaultMethods[2][3];
        }
    }
    if(SplitCount!=0)
    {
        wgsModified=1;
        QString vehicleName=(Filename.c_str());
        vehicleName.remove(0,16);
        int length=vehicleName.length();
        vehicleName.insert(length-4,"modified");
        Filename="data/vehiclewgs/";
        Filename.append(string((const char *)vehicleName.toLocal8Bit()));
        WriteWGSfile();
    }
    return 0;
}

void VehicleGeometry::UpdateParts(bool SplitBoolArr[])
        //In: bool array with true/false whether original part has been split
        //Changes global variable PartsOut to include split parts
{
    int i;
    PartGeometry * PartsTemp;
    PartsTemp=new PartGeometry[nParts];
    for(i=0;i<nParts;i++)//Copy old entries from PartsOut into temporary array
    {
        PartsTemp[i]=PartGeometry(PartsOut[i]);
        PartsTemp[i].PanelCalculations();
        PartsOut[i].deAllocateArrays();
    }
    delete [] PartsOut;//delete old array
    PartsOut=new PartGeometry[nParts+SplitCount];//allocate new array
    int OldParts=0;//Will count how many parts remain unsplit
    for(i=0;i<nParts;i++)
    {
        if(SplitBoolArr[i]==0)//Copy old part into new partsOut array if unmodified
        {
            PartsOut[OldParts]=PartGeometry(PartsTemp[i]);
            PartsOut[OldParts].PanelCalculations();
            OldParts++;
        }
        PartsTemp[i].deAllocateArrays();
    }
    delete [] PartsTemp;
    if(SplitCount!=0)
    {
        for(i=0;i<2*SplitCount;i++)//Copy all split parts into new PartsOut array
        {
            PartsOut[i+OldParts]=PartGeometry(PartsSplit[i]);
            PartsOut[i+OldParts].PanelCalculations();
            PartsSplit[i].deAllocateArrays();
        }
        delete [] PartsSplit; //deallocate
        nParts=nParts+SplitCount;//update number of parts in geometry
    }
}

void VehicleGeometry::WriteWGSfile()//Writes geometry to .wgs, no header info used, assumed untranslated and un-mirrored
{
    FILE * fid;
    fid=fopen((char*)Filename.c_str(),"w");
    fprintf(fid,"%s\n","VehiclePrint");

    int n;
    int i;
    int j;
    for(n=0;n<nParts;n++)
    {
        QString name1=PartsOut[n].name;
        const char* name2;
        name2=(const char *)name1.toLocal8Bit().data();
        fprintf(fid,"%s\n",name2);
        fprintf(fid,"%-6.0i", 1);
        fprintf(fid,"%-6.0i %-6.0i", PartsOut[n].Nlines, PartsOut[n].Npoints);
        fprintf(fid, "%-6.0f", 0.0);
        fprintf(fid, "%-6.0f %-6.0f %-6.0f", 0.0, 0.0, 0.0);
        fprintf(fid, "%-6.0f %-6.0f %-6.0f", 0.0, 0.0, 0.0);
        fprintf(fid, "%-6.0f %-6.0f %-6.0f", 1.0, 1.0, 1.0);
        fprintf(fid, "%-6.0f\n", 0.0);
        for (i=0;i<PartsOut[n].Nlines;i++)
        {
            for (j=0;j<PartsOut[n].Npoints;j++)
            {
                if(PartsOut[n].reversed==-1)//a 'reversed' part needs to be written in reverse
                    fprintf(fid, "%-9.6e %-9.6e %-9.6e \n", PartsOut[n].PointsVec[i][PartsOut[n].Npoints-j-1].x(), PartsOut[n].PointsVec[i][PartsOut[n].Npoints-j-1].y(), PartsOut[n].PointsVec[i][PartsOut[n].Npoints-j-1].z());
                else
                    fprintf(fid, "%-9.6e %-9.6e %-9.6e \n", PartsOut[n].PointsVec[i][j].x(), PartsOut[n].PointsVec[i][j].y(), PartsOut[n].PointsVec[i][j].z());
            }
        }
    }
    fclose(fid);
}


bool VehicleGeometry::SameSign(double n1, double n2)//Checks to see if two doubles have the same sign, abs values below ACCURACY are aslways set as 'same sign'
{
    bool out=0;
    double ACCURACY=1E-15;
    if (std::abs(n1)<ACCURACY || std::abs(n2)<ACCURACY)
        out=1;
    else if(sign(n1)==sign(n2))
        out=1;
    else
        out=0;
    return out;
}

int VehicleGeometry::sign(double n)
{
    int out;
    if(n>=0)
        out=1;
    else
        out=-1;
    return out;
}

