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
#include "aeroanalysis.h"
#include "Astro-Core/stamath.h"
#include "Astro-Core/bodyTOwind.h"
#include <QDebug>
#include <QTextStream>

AeroAnalysis::AeroAnalysis(VehicleGeometry *_Vehicle, double _cref, double _Sref, Vector3d _MomentRef,int** _SelectedMethods, QList<double> _writeAeroList, bool _writeGeom)
{
    gamma=1.4;
    Vehicle=new VehicleGeometry(*_Vehicle);
    cref=_cref;
    Sref=_Sref;
    MomentRef=_MomentRef;
    writeAeroList=_writeAeroList;
    writeGeom=_writeGeom;

    SelectedMethods=new int* [Vehicle->nParts];

    for(int i=0;i<2;i++)
    {
        SelectedMethods[i]=new int[Vehicle->nParts];
    }
    for(int i=0;i<Vehicle->nParts;i++)
    {
        for(int j=0;j<2;j++)
        {
            SelectedMethods[j][i]=_SelectedMethods[j][i];
        }
    }

}

AeroAnalysis::AeroAnalysis(AeroAnalysis* Low, AeroAnalysis* High, double Mlow, double Mhigh)//Constructs a bridged aerodynamicAnalysis object from the low and high hypersonic ones
        //in: Low hypersonic AeroAnalysis object, High hypersonic AeroAnalysis object
        //out: constructed bridged AeroAnalysis object
        //note: simulation and independant variable settings must be the same for high and low AeroAnalysis objects
{
    double Mstart=High->Machs[0];
    double Mend=Low->Machs[Low->nMach-1];
    int i;
    int j;
    int k;
    //if(Low->CoefsOut!=High->CoefsOut || Low->cref!=High->cref || Low->MomentRef!=High->MomentRef || Low->nAlpha!=High->nAlpha || Low->nBeta != High->nBeta)
    //    qDebug()<<"Incomatible Low and High databases"<<endl;
    if(Low->writeGeom==1 || High->writeGeom==1)
        writeGeom=1;
    else
        writeGeom=0;
    CoefsOut=Low->CoefsOut;
    cref=Low->cref;
    Sref=Low->Sref;
    MomentRef=Low->MomentRef;
    Vehicle=new VehicleGeometry(*Low->Vehicle);
    nAlpha=Low->nAlpha;
    nBeta=Low->nBeta;
    //nMach=Low->nMach+High->nMach-4;//Four points overlap
    nMach=High->Machs[High->nMach-1]-Low->Machs[0]+1;
    Machs=new double[nMach];
    for(i=0;i<nMach;i++)
    {
        Machs[i]=Low->Machs[0]+i;
    }
    alphas=new double[nAlpha];
    for(i=0;i<nAlpha;i++)
        alphas[i]=Low->alphas[i];
    betas=new double[nBeta];
    for(i=0;i<nBeta;i++)
        betas[i]=Low->betas[i];

    nCases=nMach*nAlpha*nBeta;
    VehicleCoefs=new double*[nCases];

    for(i=0;i<nCases;i++)
    {
       VehicleCoefs[i]=new double[High->CoefsOut];
    }
    int klow;
    int khigh;
    int l=0;
    int m;
    for(i=0;i<nBeta;i++)
    {
        for(j=0;j<nAlpha;j++)
        {
            klow=0;
            khigh=0;
            for(k=0;k<nMach;k++)
            {
                if(Machs[k]<Mstart)
                {
                    for(m=0;m<CoefsOut;m++)
                    {
                        VehicleCoefs[l][m]=Low->VehicleCoefs[i*Low->nMach*nAlpha+j*Low->nMach+klow][m];
                    }
                    klow++;

                }

                else if(Machs[k]>Mend)
                {
                    for(m=0;m<CoefsOut;m++)
                    {
                        VehicleCoefs[l][m]=High->VehicleCoefs[i*High->nMach*nAlpha+j*High->nMach+khigh][m];
                    }
                    khigh++;

                }
                else
                {
                    for(m=0;m<CoefsOut;m++)
                    {
                        VehicleCoefs[l][m]=BridgeCoefs(Low->VehicleCoefs[i*Low->nMach*nAlpha+j*Low->nMach+klow][m],High->VehicleCoefs[i*High->nMach*nAlpha+j*High->nMach+khigh][m],Machs[k],Mlow,Mhigh);

                    }
                    khigh++;
                    klow++;
                }
                l++;
            }
        }
    }
}

AeroAnalysis::~AeroAnalysis()
{
    int i;
    delete Vehicle;
    for(i=0;i<nCases;i++)
    {
        delete [] VehicleCoefs[i];
    }
    delete[] VehicleCoefs;
    delete[] alphas;
    delete[] Machs;
    delete[] betas;
}

void AeroAnalysis::GenerateDatabase(int ForceCoord,int MomentCoord)
{
    nCases=nMach*nBeta*nAlpha;
    Cps=2;
    VehicleCoefs=new double* [nCases];
    double* tempCoefs= new double[CoefsOut];
    int m;
    for(int i=0;i<nCases;i++)
    {
       VehicleCoefs[i]=new double[CoefsOut];
    }
    int l=0;
    for(int i=0;i<nBeta;i++)
    {
        for(int j=0;j<nAlpha;j++)
        {
            for(int k=0;k<nMach;k++)
            {
                setStagnationPressure(Machs[k]);
                tempCoefs=DetermineVehicleCoefs(Machs[k],alphas[j],betas[i],ForceCoord,MomentCoord);
                for(m=0;m<CoefsOut;m++)
                {
                    VehicleCoefs[l][m]=tempCoefs[m];

                }
                l++;
                if(fromPressureWriteList(Machs[k]))
                {
                    writePressures(Machs[k],alphas[j],betas[i]);
                }
            }
        }
    }
}

QList<QString> AeroAnalysis::writeAeroDatabase(int ForceCoord, int MomentCoord)
{
    //qDebug()<<"writing aero database";
    int coef;
    QList<QString> fileList;
    QString vehicleName=((Vehicle->Filename).c_str());
    vehicleName.remove(0,16);
    vehicleName.remove(vehicleName.length()-4,4);
    QString folder="data/aerodynamics/";
    QString file;
    for(coef=0;coef<CoefsOut;coef++)
     {
        switch(coef)
        {
        case 0:if(ForceCoord==0)
                file=vehicleName+"_f_CA"+".stad";
            else if(ForceCoord==1)
                file=vehicleName+"_f_CD"+".stad";
            writeCoefFile(folder+file,coef);
            fileList.append(file);
            break;
        case 1:if(ForceCoord==0)
                file=vehicleName+"_f_CY"+".stad";
            else if(ForceCoord==1)
                file=vehicleName+"_f_CS"+".stad";
            writeCoefFile(folder+file,coef);
            fileList.append(file);
            break;
        case 2:if(ForceCoord==0)
                file=vehicleName+"_f_CN"+".stad";
            else if(ForceCoord==1)
                file=vehicleName+"_f_CL"+".stad";
            writeCoefFile(folder+file,coef);
            fileList.append(file);
            break;
        case 3:if(MomentCoord==0)
                file=vehicleName+"_m_Cl"+".stad";
            else if(MomentCoord==1)
                file=vehicleName+"_m_Clpr"+".stad";
            writeCoefFile(folder+file,coef);
            fileList.append(file);
            break;
        case 4:if(MomentCoord==0)
                file=vehicleName+"_m_Cm"+".stad";
            else if(MomentCoord==1)
                file=vehicleName+"_m_Cmpr"+".stad";
            writeCoefFile(folder+file,coef);
            fileList.append(file);
            break;
        case 5:if(MomentCoord==0)
                file=vehicleName+"_m_Cn"+".stad";
            else if(MomentCoord==1)
                file=vehicleName+"_m_Cnpr"+".stad";
            writeCoefFile(folder+file,coef);
            fileList.append(file);
            break;

        }

    }
    if (writeGeom==1)
    {
        writeDetailGeomInfo();
    }
    return fileList;
}

void AeroAnalysis::writeCoefFile(QString filename, int coef)
{
    QFile aeroFile(filename);
    aeroFile.open(QIODevice::WriteOnly);
    QTextStream aeroStream(&aeroFile);
    int i,j,k,l;
    l=0;

    for(i=0;i<nMach;i++)
    {
        aeroStream << Machs[i]<<" ";
    }
    aeroStream << endl;
    if(nAlpha>1)
    {
        for(i=0;i<nAlpha;i++)
            aeroStream << alphas[i]*180/sta::Pi() << " ";
        aeroStream << endl;
    }
    if(nBeta>1)
    {
        for(i=0;i<nBeta;i++)
            aeroStream << betas[i]*180/sta::Pi() << " ";
        aeroStream << endl;
    }

    for(k=0;k<nBeta;k++)
    {
        for(i=0;i<nAlpha;i++)
        {
            for(j=0;j<nMach;j++)
            {
                aeroStream << VehicleCoefs[l][coef]<< " ";
                l++;
            }
            aeroStream << endl;
        }
    }
    aeroFile.close();
}

void AeroAnalysis::writeDetailGeomInfo()
{
    QString VehicleName=QString(Vehicle->Filename.c_str());
    VehicleName.remove(0,16);
    VehicleName.remove(VehicleName.length()-4,4);
    QString File="data/ramoutput/"+VehicleName+"panelDetails.stad";
    QFile geomFile(File);
    geomFile.open(QIODevice::WriteOnly);
    QTextStream geomStream(&geomFile);
    int i,j,k;
    for(i=0; i<Vehicle->nParts;i++)
    {
        DetermineInclination(i,0.0,0.0);//Also write inclination angles for 0 angle of attack and sideslip
        for(j=0; j<Vehicle->PartsOut[i].Nlines-1;j++)
        {
            for(k=0; k<Vehicle->PartsOut[i].Npoints-1; k++)
            {
                geomStream << i << " ";
                geomStream << j << " ";
                geomStream << k << " ";
                geomStream << Vehicle->PartsOut[i].Centroid[j][k].x() << " ";
                geomStream << Vehicle->PartsOut[i].Centroid[j][k].y() << " ";
                geomStream << Vehicle->PartsOut[i].Centroid[j][k].z() << " ";
                geomStream << Vehicle->PartsOut[i].Normal[j][k].x() << " ";
                geomStream << Vehicle->PartsOut[i].Normal[j][k].y() << " ";
                geomStream << Vehicle->PartsOut[i].Normal[j][k].z() << " ";
                geomStream << Vehicle->PartsOut[i].Area[j][k] << " ";
                geomStream << Vehicle->PartsOut[i].theta[j][k] << " ";
                geomStream<<endl;

            }
        }
    }
    geomFile.close();
}

void AeroAnalysis::writePressures(double M, double a, double b)
{
    QString VehicleName=QString(Vehicle->Filename.c_str());
    VehicleName.remove(0,16);
    VehicleName.remove(VehicleName.length()-4,4);
    QString presFileName="data/ramoutput/"+VehicleName+"_pressure_M"+QString::number(M)+"_a"+QString::number(a)+"_b"+QString::number(b)+".stad";
    QFile presFile(presFileName);
    presFile.open(QIODevice::Append);
    QTextStream presStream(&presFile);
    int i,j,k;
    for(i=0; i<Vehicle->nParts;i++)
    {
        for(j=0; j<Vehicle->PartsOut[i].Nlines;j++)
        {
            for(k=0; k<Vehicle->PartsOut[i].Npoints; k++)
            {
                //presStream<<Vehicle->PartsOut[i].Cp[j][k]<<" ";

                presStream<<k+1<<" ";
                presStream<<i+1<<" ";
                presStream<<j+1<<" ";
                presStream<<Vehicle->PartsOut[i].PointsVec[j][k].x()<<" ";
                presStream<<Vehicle->PartsOut[i].PointsVec[j][k].y()<<" ";
                presStream<<Vehicle->PartsOut[i].PointsVec[j][k].z()<<" ";

                if((j!=Vehicle->PartsOut[i].Nlines-1) && (k!=Vehicle->PartsOut[i].Npoints-1))
                {
                    presStream<<Vehicle->PartsOut[i].Cp[j][k]<<" ";
                    presStream<<Vehicle->PartsOut[i].theta[j][k]<<" ";

                }
                else
                {
                    presStream<<0.0<<" "<<0.0<<" ";//Write something to make loading into Matlab more user friendly
                }
                presStream<<endl;

            }
        }
    }
    presFile.close();
}



double* AeroAnalysis::DetermineVehicleCoefs(double M, double alpha, double beta, int ForceCoord, int MomentCoord)
{
    double* VehicleCoefs=new double[CoefsOut];
    double* PartCoefs=new double[CoefsOut];
    int i;
    int j;
    for(int i=0;i<CoefsOut;i++)
        VehicleCoefs[i]=0.0;
    for(i=0;i<Vehicle->nParts;i++)
    {
        DetermineInclination(i,alpha,beta);
    }
    //Vehicle->ShadowDetermination(); to be implemented in future expansion
    for(i=0;i<Vehicle->nParts;i++)
    {
        PartCoefs=DeterminePartCoefs(i,M,alpha,beta,ForceCoord,MomentCoord);
        for(j=0;j<CoefsOut;j++)
            VehicleCoefs[j]=VehicleCoefs[j]+PartCoefs[j];
    }
    delete [] PartCoefs;
    return VehicleCoefs;
}



void AeroAnalysis::setIndepVariablesTranslation(double* AttitudeRefs, double Mmin, double Mmax, int RegimeType)
{
    CoefsOut=6;
    nAlpha=1;
    nBeta=1;
    alphas=new double[1];
    betas=new double[1];
    alphas[0]=AttitudeRefs[0];
    betas[0]=AttitudeRefs[1];
    setMachRange(Mmin,Mmax);
}

void AeroAnalysis::setMachRange(double Mmin,double Mmax)//type:0=high hypersonic, 1=low hypersonic, 2=full hypersonic
{
    double Mstart=(double)std::floor(Mmin);
    double Mend=(double)std::ceil(Mmax);
    nMach=Mend-Mstart+1;
    int i;
    Machs=new double[nMach];
    for(i=0;i<nMach;i++)
    {
        Machs[i]=(double)i+Mstart;
    }
}

void AeroAnalysis::setIndepVariablesFull(int SimType, double* AttitudeBounds, double Mmin, double Mmax, int RegimeType)
{
    if(SimType==1)
        CoefsOut=3;
    else
        CoefsOut=6;

    double dAlpha=5*sta::Pi()/180;
    double dBeta=5*sta::Pi()/180;
    nAlpha=ceil((AttitudeBounds[1]-AttitudeBounds[0])/dAlpha)+1;
    nBeta=ceil((AttitudeBounds[3]-AttitudeBounds[2])/dBeta)+1;
    alphas=new double[nAlpha];
    betas=new double[nBeta];
    int i;
    for(i=0;i<nAlpha;i++)
        alphas[i]=AttitudeBounds[0]+((double)i)/(nAlpha-1)*(AttitudeBounds[1]-AttitudeBounds[0]);
    for(i=0;i<nBeta;i++)
        betas[i]=AttitudeBounds[2]+((double)i)/(nBeta-1)*(AttitudeBounds[3]-AttitudeBounds[2]);
    setMachRange(Mmin,Mmax);

    //Horus settings
    /*
    nMach=4;
    Machs=new double[nMach];
    Machs[0]=3;
    Machs[1]=5;
    Machs[2]=10;
    Machs[3]=20;
    nBeta=2;
    betas=new double[nBeta];
    betas[0]=0;
    betas[1]=2*sta::Pi()/180;
    nAlpha=10;
    alphas=new double[nAlpha];
    for(int i=0;i<nAlpha;i++)
    {
        alphas[i]=5*sta::Pi()*i/180;
    }
    */
    //EXPERT settings
    /*
    int nAlphaT=5;
    double phiA=0*sta::Pi()/180;
    double * alphaT=new double[nAlphaT];
    nAlpha=nAlphaT;
    nBeta=nAlphaT;
    alphas=new double[nAlpha];
    betas=new double[nBeta];
    for(int i=0;i<nAlphaT;i++)
    {
        alphaT[i]=i*5*sta::Pi()/180;
        alphas[i]=atan(tan(alphaT[i]*cos(phiA)));
        betas[i]=asin(sin(phiA)*sin(alphaT[i]));
    }
    setMachRange(Mmin,Mmax);
    */
}


double* AeroAnalysis::DeterminePartCoefs(int PartNumber, double M, double alpha, double beta, int ForceCoord, int MomentCoord)
{
    int i;
    double* ForceCoefs=new double[3];
    double* MomentCoefs;

    double* outCoefs;
    if (CoefsOut==3)
    {
        outCoefs=new double[3];
    }
    else if (CoefsOut==6)
    {
        MomentCoefs=new double[3];
        outCoefs=new double[6];
    }
    DeterminePressureCoefs(PartNumber,M);
    ForceCoefs=getForceCoefs(PartNumber);
    if(ForceCoord==1)
        bodyTOwind(alpha,beta,ForceCoefs);
    for(i=0;i<3;i++)
    {
        outCoefs[i]=ForceCoefs[i];
    }
    if (CoefsOut==6)
    {
        MomentCoefs=getMomentCoefs(PartNumber);
        if(MomentCoord==1)
        {
            bodyTOwind(alpha,beta,MomentCoefs);
        }
        for(i=3;i<6;i++)
        {
            outCoefs[i]=MomentCoefs[i-3];
        }
    }

    delete [] ForceCoefs;
    if (CoefsOut==6)
    {
        delete [] MomentCoefs;
    }
    return outCoefs;
}

void AeroAnalysis::DeterminePressureCoefs(int PartNumber, double M)
{
    UpdateCompressionCp(M,PartNumber);
    UpdateExpansionCp(M,PartNumber);
}

double* AeroAnalysis::getForceCoefs(int PartNumber)
{
    int i;
    int j;
    Vector3d ForceCoef=Vector3d(0,0,0);
    for(i=0;i<Vehicle->PartsOut[PartNumber].Nlines-1;i++)
    {
        for(j=0;j<Vehicle->PartsOut[PartNumber].Npoints-1;j++)
        {
            ForceCoef=ForceCoef-Vehicle->PartsOut[PartNumber].Cp[i][j]*Vehicle->PartsOut[PartNumber].Area[i][j]*Vehicle->PartsOut[PartNumber].Normal[i][j];
        }
    }
    ForceCoef=ForceCoef/Sref;
    double* ForceCoefDouble=new double[3];
    for(i=0;i<3;i++)
    {
        ForceCoefDouble[i]=ForceCoef(i);
    }
    return ForceCoefDouble;
}

double* AeroAnalysis::getMomentCoefs(int PartNumber)
{
    int i;
    int j;
    Vector3d MomentCoef=Vector3d(0,0,0);
    Vector3d r;
    for(i=0;i<Vehicle->PartsOut[PartNumber].Nlines-1;i++)
    {
        for(j=0;j<Vehicle->PartsOut[PartNumber].Npoints-1;j++)
        {
            r=(Vehicle->PartsOut[PartNumber].Centroid[i][j]-MomentRef);
            MomentCoef=MomentCoef-Vehicle->PartsOut[PartNumber].Cp[i][j]*Vehicle->PartsOut[PartNumber].Area[i][j]*(r.cross(Vehicle->PartsOut[PartNumber].Normal[i][j]));

        }
    }
    MomentCoef=MomentCoef/(cref*Sref);
    double* MomentCoefDouble=new double[3];
    for(i=0;i<3;i++)
        MomentCoefDouble[i]=MomentCoef(i);
    return MomentCoefDouble;
}

void AeroAnalysis::DetermineInclination(int PartNumber, double alpha, double beta)
{
    Vector3d Vinf;
    int i,j;
    double cosdel;
    double Vinfx=cos(alpha)*cos(beta);
    double Vinfy=-sin(beta);
    double Vinfz=sin(alpha)*cos(beta);
    Vinf=Vector3d(Vinfx,Vinfy,Vinfz);
    for(i=0;i<Vehicle->PartsOut[PartNumber].Nlines-1;i++)
    {
        for(j=0;j<Vehicle->PartsOut[PartNumber].Npoints-1;j++)
        {
            cosdel=-1*Vehicle->PartsOut[PartNumber].Normal[i][j].dot(Vinf);
            Vehicle->PartsOut[PartNumber].theta[i][j]=sta::Pi()/2-acos(cosdel);
        }
    }
}

void AeroAnalysis::UpdateCompressionCp(double M, int PartNumber)
{
    switch(SelectedMethods[0][PartNumber])
    {
    case 0: Newtonian(M,PartNumber);
        break;
    case 1: ModifiedNewtonian(M,PartNumber,1.0);
        break;
    case 2: ModifiedNewtonianPrandtlMeyer(M,PartNumber,1.0);
        break;
    case 3: TangentWedge(M,PartNumber);
        break;
    case 4: TangentWedgeEmpirical(M,PartNumber);
        break;
    case 5: TangentConeEmpirical(M,PartNumber);
        break;
    case 6: ModifiedDahlemBuck(M,PartNumber);
        break;
    case 7: VanDykeUnified(M,PartNumber,1.0);
        break;
    case 8: SmythDeltaWing(M,PartNumber);
        break;
    case 9: HankeyFlatSurface(M,PartNumber);
        break;
    }

}

void AeroAnalysis::UpdateExpansionCp(double M, int PartNumber)
{
    switch(SelectedMethods[1][PartNumber])
    {
    case 0: Vacuum(M,PartNumber);
        break;
    case 1: ModifiedNewtonian(M,PartNumber,-1);
        break;
    case 2: ModifiedNewtonianPrandtlMeyer(M,PartNumber,-1);
        break;
    case 3: PrandtlMeyerFreestream(M,PartNumber);
        break;
    case 4: HighMachBase(M,PartNumber);
        break;
    case 5: VanDykeUnified(M,PartNumber,-1);
        break;
    case 6: ACMempirical(M,PartNumber);
        break;
    }
}

void AeroAnalysis::setStagnationPressure(double M)
{
    Cps=2/(gamma*pow(M,2.0))*(pow(pow((gamma+1)*M,2.0)/(4*gamma*pow(M,2.0)-2*(gamma-1)),gamma/(gamma-1))*((1-gamma+2*gamma*pow(M,2.0))/(gamma+1))-1);
}

void AeroAnalysis::Newtonian(double M, int PartNumber)
{
    for(int i=0;i<Vehicle->PartsOut[PartNumber].Nlines-1;i++)
    {
        for(int j=0;j<Vehicle->PartsOut[PartNumber].Npoints-1;j++)
        {
            if(Vehicle->PartsOut[PartNumber].theta[i][j]>=0)
            {
                Vehicle->PartsOut[PartNumber].Cp[i][j]=2*pow(sin(Vehicle->PartsOut[PartNumber].theta[i][j]),2.0);
            }
        }
    }
}

void AeroAnalysis::ModifiedNewtonian(double M, int PartNumber, int Type)
{
    for(int i=0;i<Vehicle->PartsOut[PartNumber].Nlines-1;i++)
    {
        for(int j=0;j<Vehicle->PartsOut[PartNumber].Npoints-1;j++)
        {
            if(Vehicle->PartsOut[PartNumber].theta[i][j]>=0 && Type==1)
            {
                Vehicle->PartsOut[PartNumber].Cp[i][j]=Cps*pow(sin(Vehicle->PartsOut[PartNumber].theta[i][j]),2.0);
            }
            else if(Vehicle->PartsOut[PartNumber].theta[i][j]<0 && Type==-1)
            {
                Vehicle->PartsOut[PartNumber].Cp[i][j]=0;
            }
        }
    }
}

void AeroAnalysis::ModifiedNewtonianPrandtlMeyer(double M, int PartNumber, int Type)
{
    double Msq=pow(M,2.0);
    double pcap=pow((2/((gamma+1)*Msq)),gamma/(gamma-1))*pow((2*gamma*Msq-gamma+1)/(gamma+1),1/(gamma-1));
    double Mlow=0.91+0.3125*gamma;
    double Mup=Mlow+0.4;
    double Mq;
    double pc;
    double m1;
    double m2;
    double q;
    double p1;
    double p2;
    double Cpcheck;
    double Cpvac=-2/(gamma*Msq);
    double Mloc;
    double thetalim;
    double numq;
    int k;
    m1=Mlow;
    m2=Mup;
    p2=0;
    k=0;
    while (k<MaxIter)
    {
        q=pow(2/(2+(gamma-1)*pow(m1,2.0)),gamma/(gamma-1));
        p1=q*(1-(pow(gamma,2.0)*pow(Mq,4.0)*q)/(4*(1-q)*(pow(Mq,2.0)-1)));
        q=pow(2/(2+(gamma-1)*pow(m2,2.0)),gamma/(gamma-1));
        p2=q*(1-(pow(gamma,2.0)*pow(Mq,4.0)*q)/(4*(1-q)*(pow(Mq,2.0)-1)));
        Mq=m1+(pcap-p1)/(p2-p1)*(m2-m1);
        if(Mq>Mup)
            Mq=Mup;
        else if(Mq<Mlow)
            Mq=Mlow;
        m2=Mq;
        k++;

    }
    q=pow(2/(2+(gamma-1)*pow(Mq,2.0)),gamma/(gamma-1));
    thetalim=asin(pow((q-pcap)/(1-pcap),0.5));
    numq=sqrt((gamma+1)/(gamma-1))*atan(sqrt((gamma-1)/(gamma+1)*(pow(Mq,2.0)-1)))-atan(sqrt(pow(Mq,2.0)-1));
    for(int i=0;i<Vehicle->PartsOut[PartNumber].Nlines-1;i++)
    {
        for(int j=0;j<Vehicle->PartsOut[PartNumber].Npoints-1;j++)
        {
            if(Vehicle->PartsOut[PartNumber].theta[i][j]>=-0.01 && Type==1)
            {
                if(Vehicle->PartsOut[PartNumber].theta[i][j]>thetalim)
                {
                    Vehicle->PartsOut[PartNumber].Cp[i][j]=Cps*pow(sin(Vehicle->PartsOut[PartNumber].theta[i][j]),2.0);
                }
                else
                {
                    Mloc=InversePrandtlMeyer(numq+(sta::Pi()/2-Vehicle->PartsOut[PartNumber].theta[i][j])-(sta::Pi()/2-thetalim));
                    Vehicle->PartsOut[PartNumber].Cp[i][j]=2/(gamma*Msq)*((1/pcap)*pow(1+(gamma-1)/2*pow(Mloc,2.0),-gamma/(gamma-1))-1);
                }
            }
            else if(Vehicle->PartsOut[PartNumber].theta[i][j]<0 && Type==-1)
            {
                Mloc=InversePrandtlMeyer(thetalim-Vehicle->PartsOut[PartNumber].theta[i][j]+numq);
                Cpcheck=(1/pcap)*pow(1+(gamma-1)/2*pow(Mloc,2.0),-gamma/(gamma-1));
                if (Cpcheck>Cpvac)
                    Vehicle->PartsOut[PartNumber].Cp[i][j]=Cpcheck;
                else
                    Vehicle->PartsOut[PartNumber].Cp[i][j]=Cpvac;
            }
        }
    }
}

void AeroAnalysis::TangentWedge(double M, int PartNumber)
        //in: Mach number, Part of the vehicle to analyze
        //out: -
        //Global variables changed: Pressure coefficients on windward panels on said part updated with tangent wedge method
{
    double b;
    double c;
    double d;
    double p;
    double q;
    double R;
    double omega;
    double sinth2;
    double Msq=pow(M,2.0);
    double Mns;
    for(int i=0;i<Vehicle->PartsOut[PartNumber].Nlines-1;i++)
    {
        for(int j=0;j<Vehicle->PartsOut[PartNumber].Npoints-1;j++)
        {
            if(Vehicle->PartsOut[PartNumber].theta[i][j]>=0)
            {
                if(Vehicle->PartsOut[PartNumber].theta[i][j]>0.7956)
                {
                    Mns=M*sin(Vehicle->PartsOut[PartNumber].theta[i][j]);
                    Vehicle->PartsOut[PartNumber].Cp[i][j]=(pow(1.2*Mns+exp(-0.6*Mns),2.0)-1)/(0.6*pow(M,2.0));
                }
                else
                {
                    b=-1*(Msq+2)/Msq-gamma*pow(sin(Vehicle->PartsOut[PartNumber].theta[i][j]),2.0);
                    c=(2*Msq+1)/pow(Msq,2.0)+(pow(gamma+1,2.0)/4+(gamma-1)/Msq)*pow(sin(Vehicle->PartsOut[PartNumber].theta[i][j]),2.0);
                    d=-1*pow(cos(Vehicle->PartsOut[PartNumber].theta[i][j]),2.0)/pow(Msq,2.0);
                    p=(-b*b/3+c);
                    q=2*pow((b/3),3.0)-b*c/3+d;
                    if (pow(p/3,3.0)<-1*pow((q/2),2.0))
                    {
                        omega=acos(-1*q/(2*sqrt(-1*pow(p/3,3.0))));
                        R=-2*sqrt(-p/3)*cos(omega/3+sta::Pi()/3)-b/3;
                        Vehicle->PartsOut[PartNumber].Cp[i][j]=4*(Msq*R-1)/((gamma+1)*Msq);
                    }
                    else
                    {
                        Mns=M*sin(Vehicle->PartsOut[PartNumber].theta[i][j]);
                        Vehicle->PartsOut[PartNumber].Cp[i][j]=(pow(1.2*Mns+exp(-0.6*Mns),2.0)-1)/(0.6*pow(M,2.0));
                    }
                }
            }
        }
    }
}

void AeroAnalysis::TangentWedgeEmpirical(double M, int PartNumber)
        //in: Mach number, Part of the vehicle to analyze
        //out: -
        //Global variables changed: Pressure coefficients on windward panels on said part updated with empirical tangent wedge method
        //Note: relation is very close for high Mach Numbers, not quite as good for Low hypersonic, see STA-TUD-TN-1001
{
    double Mns;
    for(int i=0;i<Vehicle->PartsOut[PartNumber].Nlines-1;i++)
    {
        for(int j=0;j<Vehicle->PartsOut[PartNumber].Npoints-1;j++)
        {
            if(Vehicle->PartsOut[PartNumber].theta[i][j]>=0)
            {
                Mns=M*sin(Vehicle->PartsOut[PartNumber].theta[i][j]);
                Vehicle->PartsOut[PartNumber].Cp[i][j]=(pow(1.2*Mns+exp(-0.6*Mns),2.0)-1)/(0.6*pow(M,2.0));
            }
        }
    }
}

void AeroAnalysis::TangentConeEmpirical(double M, int PartNumber)
        //in: Mach number, Part of the vehicle to analyze
        //out: -
        //Global variables changed: Pressure coefficients on windward panels on said part updated with empirical tangent cone method
        //Note: relation is very close for high Mach Numbers, not quite as good for Low hypersonic, see STA-TUD-TN-1001
{
    double Mns;
    double temp;
    for(int i=0;i<Vehicle->PartsOut[PartNumber].Nlines-1;i++)
    {
        for(int j=0;j<Vehicle->PartsOut[PartNumber].Npoints-1;j++)
        {
            if(Vehicle->PartsOut[PartNumber].theta[i][j]>0)
            {
                Mns=M*sin(Vehicle->PartsOut[PartNumber].theta[i][j]);
                temp=pow((1.090909*Mns+exp(-0.5454545*Mns)),2);
                Vehicle->PartsOut[PartNumber].Cp[i][j]=(48*temp*pow(sin(Vehicle->PartsOut[PartNumber].theta[i][j]),2))/(23*temp-5);
            }
        }
    }
}

void AeroAnalysis::ModifiedDahlemBuck(double M, int PartNumber)
        //in: Mach number, Part of the vehicle to analyze
        //out: -
        //Global variables changed: Pressure coefficients on windward panels on said part updated with Modified Dahlem Buck method
{
    double thCheck=22.5*sta::Pi()/180;
    double f;
    double a;
    double n;

    for(int i=0;i<Vehicle->PartsOut[PartNumber].Nlines-1;i++)
    {
        for(int j=0;j<Vehicle->PartsOut[PartNumber].Npoints-1;j++)
        {
            if(Vehicle->PartsOut[PartNumber].theta[i][j]>=0)
            {
                if(Vehicle->PartsOut[PartNumber].theta[i][j]>thCheck)
                    Vehicle->PartsOut[PartNumber].Cp[i][j]=2*pow(sin(Vehicle->PartsOut[PartNumber].theta[i][j]),2.0);
                else
                    Vehicle->PartsOut[PartNumber].Cp[i][j]=(1+sin(4*pow(Vehicle->PartsOut[PartNumber].theta[i][j],0.75)))/(pow(4*cos(Vehicle->PartsOut[PartNumber].theta[i][j])*cos(2*Vehicle->PartsOut[PartNumber].theta[i][j]),0.75))*pow(sin(Vehicle->PartsOut[PartNumber].theta[i][j]),1.25);
                if(M>20)
                    f=1.0;
                else
                {
                    a=(6-0.3*M)+sin(sta::Pi()*(log(M)-0.588)/1.20);
                    n=1.15+0.5*sin(sta::Pi()*(log(M)-0.916)/3.29);
                    f=1+a*pow(Vehicle->PartsOut[PartNumber].theta[i][j]*180/sta::Pi(),-1*n);
                }
                Vehicle->PartsOut[PartNumber].Cp[i][j]=Vehicle->PartsOut[PartNumber].Cp[i][j]*f;
            }
        }
    }
}

void AeroAnalysis::HankeyFlatSurface(double M, int PartNumber)
        //in: Mach number, Part of the vehicle to analyze
        //out: -
        //Global variables changed: Pressure coefficients on windward panels on said part updated with Hankey flat surface method
{
    double Cps;
    for(int i=0;i<Vehicle->PartsOut[PartNumber].Nlines-1;i++)
    {
        for(int j=0;j<Vehicle->PartsOut[PartNumber].Npoints-1;j++)
        {
            if(Vehicle->PartsOut[PartNumber].theta[i][j]>=0)
            {
                if(Vehicle->PartsOut[PartNumber].theta[i][j]<sta::Pi()/18)
                    Cps=(0.195+0.222594/pow(M,0.3)-0.4)*Vehicle->PartsOut[PartNumber].theta[i][j]*180/sta::Pi()+4;
                else
                    Cps=1.95+0.3925/(pow(M,0.3)*tan(Vehicle->PartsOut[PartNumber].theta[i][j]));
                Vehicle->PartsOut[PartNumber].Cp[i][j]=Cps*pow(sin(Vehicle->PartsOut[PartNumber].theta[i][j]),2.0);
            }
        }
    }
}

void AeroAnalysis::SmythDeltaWing(double M, int PartNumber)
        //in: Mach number, Part of the vehicle to analyze
        //out: -
        //Global variables changed: Pressure coefficients on windward panels on said part updated with Smyth Delta Wing method
{
    double Mns;
    double thetac;
    for(int i=0;i<Vehicle->PartsOut[PartNumber].Nlines-1;i++)
    {
        for(int j=0;j<Vehicle->PartsOut[PartNumber].Npoints-1;j++)
        {
            if(Vehicle->PartsOut[PartNumber].theta[i][j]>=0)
            {
                if(Vehicle->PartsOut[PartNumber].theta[i][j]<sta::Pi()/180)
                    thetac=sta::Pi()/180;
                else
                    thetac=Vehicle->PartsOut[PartNumber].theta[i][j];
                Mns=M*sin(thetac);
                Vehicle->PartsOut[PartNumber].Cp[i][j]=1.66667*(pow(1.09*Mns+exp(-0.49*Mns),2.0)-1)/pow(M,2.0);
            }
        }
    }
}

void AeroAnalysis::VanDykeUnified(double M, int PartNumber, int Type)
        //in: Mach number, Part of the vehicle to analyze, windward/leeward type
        //out: -
        //Global variables changed: Pressure coefficients on windward (Type=1) or leeward (Type=-1) panels on said part updated with Van Dyke Unified method
{
    double gammaterm=(gamma+1)/2;
    double beta=pow(pow(M,2.0)-1,0.5);
    double CpVac=-2/(gamma*pow(M,2.0));
    double expon=2*gamma/(gamma-1);
    for(int i=0;i<Vehicle->PartsOut[PartNumber].Nlines-1;i++)
    {
        for(int j=0;j<Vehicle->PartsOut[PartNumber].Npoints-1;j++)
        {
            if(Vehicle->PartsOut[PartNumber].theta[i][j]>=0 && Type==1)
            {
                Vehicle->PartsOut[PartNumber].Cp[i][j]=pow(Vehicle->PartsOut[PartNumber].theta[i][j],2.0)*(gammaterm+pow(pow(gammaterm,2.0)+4/(pow(Vehicle->PartsOut[PartNumber].theta[i][j]*beta,2.0)),0.5));

            }
            else if(Vehicle->PartsOut[PartNumber].theta[i][j]<0 && Type==-1)
            {
                if(-1*Vehicle->PartsOut[PartNumber].theta[i][j]*beta>2/(gamma-1))
                        Vehicle->PartsOut[PartNumber].Cp[i][j]=CpVac;
                else
                {
                    Vehicle->PartsOut[PartNumber].Cp[i][j]=2/(gamma*pow(beta,2.0))*(pow(1-(gamma-1)/2*-1*Vehicle->PartsOut[PartNumber].theta[i][j]*beta,expon)-1);
                    if ( Vehicle->PartsOut[PartNumber].Cp[i][j]<CpVac)
                    {
                       Vehicle->PartsOut[PartNumber].Cp[i][j]=CpVac;
                    }
                }
            }
            //else
              //  Vehicle->PartsOut[PartNumber].Cp[i][j]=0;
        }
    }
}

void AeroAnalysis::PrandtlMeyerFreestream(double M, int PartNumber)
{
    //in: Mach number, Part of the vehicle to analyze
    //out: -
    //Global variables changed: Pressure coefficients on leeward panels on said part updated with PM method
    //Note: currently the gamma=1.4 PM inversion is called, change if other gammas are used
    double numax=sta::Pi()/2*(pow(6,0.5)-1);
    double beta=sqrt(pow(M,2.0)-1);
    double nu0=sqrt(6)*atan(beta/sqrt(6))-atan(beta);
    double nu;
    double Mloc;
    for(int i=0;i<Vehicle->PartsOut[PartNumber].Nlines-1;i++)
    {
        for(int j=0;j<Vehicle->PartsOut[PartNumber].Npoints-1;j++)
        {
            if(Vehicle->PartsOut[PartNumber].theta[i][j]<0)
            {
                nu=nu0-Vehicle->PartsOut[PartNumber].theta[i][j];
                if(nu>numax)
                {
                    Vehicle->PartsOut[PartNumber].Cp[i][j]=-2/(gamma*pow(M,2.0));
                }
                else
                {
                    Mloc=InversePrandtlMeyer(nu);
                    Vehicle->PartsOut[PartNumber].Cp[i][j]=2/(gamma*pow(M,2.0))*(pow((1+(gamma-1)/2*pow(Mloc,2.0))/(1+(gamma-1)/2*pow(M,2.0)),-gamma/(gamma-1))-1);
                }
            }
        }
    }
}

void AeroAnalysis::Vacuum(double M, int PartNumber)
{
    for(int i=0;i<Vehicle->PartsOut[PartNumber].Nlines-1;i++)
    {
        for(int j=0;j<Vehicle->PartsOut[PartNumber].Npoints-1;j++)
        {
            if(Vehicle->PartsOut[PartNumber].theta[i][j]<0)
                Vehicle->PartsOut[PartNumber].Cp[i][j]=-2/(gamma*pow(M,2.0));
        }
    }
}

void AeroAnalysis::HighMachBase(double M, int PartNumber)
        //in: Mach number, Part of the vehicle to analyze
        //out: -
        //Global variables changed: Pressure coefficients on leeward panels on said part updated with High Mach base pressure methods
{
    for(int i=0;i<Vehicle->PartsOut[PartNumber].Nlines-1;i++)
    {
        for(int j=0;j<Vehicle->PartsOut[PartNumber].Npoints-1;j++)
        {
            if(Vehicle->PartsOut[PartNumber].theta[i][j]<0)
                Vehicle->PartsOut[PartNumber].Cp[i][j]=-1/pow(M,2.0);
        }
    }
}

void AeroAnalysis::ACMempirical(double M, int PartNumber)
        //in: Mach number, Part of the vehicle to analyze
        //out: -
        //Global variables changed: Pressure coefficients on leeward panels on said part updated with ACM empirical method
{
    double Cpmin=-1/pow(M,2.0);
    double Cptest;
    for(int i=0;i<Vehicle->PartsOut[PartNumber].Nlines-1;i++)
    {
        for(int j=0;j<Vehicle->PartsOut[PartNumber].Npoints-1;j++)
        {
            if(Vehicle->PartsOut[PartNumber].theta[i][j]<0)
            {
                Cptest=180/sta::Pi()*Vehicle->PartsOut[PartNumber].theta[i][j]/(16*pow(M,2.0));
                if(Cpmin>Cptest)
                    Vehicle->PartsOut[PartNumber].Cp[i][j]=Cpmin;
                else
                    Vehicle->PartsOut[PartNumber].Cp[i][j]=Cptest;
            }
        }
    }
}

double AeroAnalysis::InversePrandtlMeyer(double nu)
        //in: Prandtl Meyer function value
        //out: Corresponding Mach number
        //note: Correlation accurate to 5-6 digits, based on gamma=1.4, should not be used for non-terrestrial atmospheres or reacting gas
{
    double numax=sta::Pi()/2*(pow(6,0.5)-1);
    double y=pow(nu/numax,0.666667);
    double Mach=(1+y*(IPM1+y*(IPM2+y*IPM3)))/(1+y*(IPM4+y*IPM5));
    return Mach;
}
/*
double AeroAnalysis::InversePrandtlMeyerPrecise(double nu)

{
    double term=pow((gamma+1)/(gamma-1),0.5);
    double M0=InversePrandtlMeyer(nu);//Getting first guess from Hall equation (from gamma=1.4);
    double beta0=pow(pow(M0,2),0.5);
    double eta0=term*atan(beta0*term)-atan(beta0)-nu;
    double M1=M0;
    double Mnew;
    if(eta0>0)
    {
        eta1=1;
        while(eta1>0)
        {
            M1=M1-1;
            eta1=term*atan(beta1*term)-atan(beta1)-nu;
        }
        Mnew=M0;
        M0=M1;
        M1=Mnew;
    }
    else if(eta0<0)
    {
        eta0=-1;
        while(eta1<0)
        {
            M1=M1+1;
            eta1=term*atan(beta1*term)-atan(beta1)-nu;
        }
    }
    while(std::fabs(M1-M0)>1E-8)
    {
        Mnew= M0+(M1-M0)/2;

    }
    return pow(pow(beta,2)+1,0.5);
}
*/

double AeroAnalysis::BridgeCoefs(double LowCoef, double HighCoef,double Mach,double Mlow, double Mhigh)//Bridges the low and high aerodynamic regimes
        //In: Low hypersonic coefficient, High Hypersonic coefficient, Mach number
        //Out: (cubically) Bridged Coefficient
{
    double t=(Mach-Mlow)/(Mhigh-Mlow);
    if(t>1)
        t=1;
    else if(t<0)
        t=0;
    double y=-2*pow(t,3.0)+3*pow(t,2.0);
    double Coef=y*HighCoef+(1-y)*LowCoef;
    return Coef;
}

bool AeroAnalysis::fromPressureWriteList(double currentMach)
{
    bool fromList=0;
    int i=0;
    while(fromList==0 && i<writeAeroList.length())
    {
        if(currentMach==writeAeroList[i])
        {
            fromList=1;
            return fromList;
        }
        i++;
    }
    return fromList;
}

