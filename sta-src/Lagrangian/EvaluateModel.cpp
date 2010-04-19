
/*
This program is free software; you can redistribute it and/or modify it under
the terms of the GNU Lesser General Public License as published by the Free Software
Foundation; either version 2 of the License, or (at your option) any later
version.

This program is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more
details.

You should have received a copy of the GNU Lesser General Public License along with
this program; if not, write to the Free Software Foundation, Inc., 59 Temple
Place - Suite 330, Boston, MA 02111-1307, USA.
Further information about the GNU Lesser General Public License can also be found on
the world wide web at http://www.gnu.org.

 ------ Copyright (C) 2009 European Space Agency (space.trajectory.analysis AT gmail.com) ----
 ------------------ Author: Valentino Zuccarelli  -------------------------------------------------
 ------------------ E-mail: (Valentino.Zuccarelli@gmail.com) ----------------------------
 */

#include "Optimization/GlobalOptimizers.h"
#include "Lagrangian/trajectoryprinting.h"
#include "Lagrangian/halorbitcomputation.h"
#include "Lagrangian/lagrangianAdvanced.h"
#include "Scenario/scenario.h"
#include "Eigen/Core"
#include "Astro-Core/EODE/eode.h"
#include "Astro-Core/date.h"
#include "Astro-Core/stabody.h"
#include "Astro-Core/propagateTHREEbody.h"
#include "Astro-Core/propagateTHREEbody.h"
#include "Astro-Core/rotatingTOcartesian.h"
#include "Astro-Core/stamath.h"
#include "Astro-Core/threebodyParametersComputation.h"
#include <QDebug>
#include <QMessageBox>

using namespace Eigen;
using namespace std;


double fractionOrbit;
extern double inclinationReq, raanReq, n;
Eigen::VectorXd GlobalOptimizers::EvaluateModel(Eigen::VectorXd x,int)
{
        Eigen::VectorXd y(NOBJ+NCONS);
#ifdef OLDSCENARIO
        extern ScenarioThreebodyTransfer* threebodyTransfer;
        extern sta::StateVector initialPositionRotating;
        extern ScenarioStateVector* stateVector_HaloTransfer;
        extern double Grav_Param, bodies_distance;

        double R1,R2,Rparking,timefinTransfer0,Rapproaching=0;
        radius(Grav_Param,initialPositionRotating.position.x(),initialPositionRotating.position.y(),initialPositionRotating.position.z(),R1,R2);
        Rparking=min(R1,R2)*bodies_distance; // S/C distance from the central body

        extern int Lagrmode;
        extern HaloOrbit haloTransfer;

        fractionOrbit=x[1]; //2nd variable: position along Halo orbit
        haloTransfer.manifolds.error=1;
//qDebug()<<"Debug| pre0!"<<haloTransfer.trajectory.init_data.State.velocity.x()<<haloTransfer.trajectory.init_data.State.velocity.y()<<haloTransfer.trajectory.init_data.State.velocity.z()<<haloTransfer.trajectory.init_data.State.position.x()<<haloTransfer.trajectory.init_data.State.position.y()<<haloTransfer.trajectory.init_data.State.position.z();
//qDebug()<<"Debug| pre1!"<<haloTransfer.trajectory.final_data.State.velocity.x()<<haloTransfer.trajectory.final_data.State.velocity.y()<<haloTransfer.trajectory.final_data.State.velocity.z()<<haloTransfer.trajectory.final_data.State.position.x()<<haloTransfer.trajectory.final_data.State.position.y()<<haloTransfer.trajectory.final_data.State.position.z();

        extern double timefinTransfer;
        double paramv[1]={0}; double deriv[6]; double timein=0;
        double relerr, abserr, accuracy; abserr=relerr=accuracy=1E-8;
        double initial_condition_perturbed[6],initial_condition_perturbed_0[6], initial_condition_0[6];

        for (int p=0; p<6; p++)
            deriv[p]=0;
        int flag=1;

        timefinTransfer0=haloTransfer.trajectory.final_data.period*fractionOrbit;
        initial_condition_0[3]=haloTransfer.trajectory.init_data.State.position.x();
        initial_condition_0[4]=haloTransfer.trajectory.init_data.State.position.y();
        initial_condition_0[5]=haloTransfer.trajectory.init_data.State.position.z();
        initial_condition_0[0]=haloTransfer.trajectory.init_data.State.velocity.x();
        initial_condition_0[1]=haloTransfer.trajectory.init_data.State.velocity.y();
        initial_condition_0[2]=haloTransfer.trajectory.init_data.State.velocity.z();
        //qDebug()<<"timefinTransfer0"<<timefinTransfer0<<initial_condition_0[3]<<initial_condition_0[4]<<initial_condition_0[5]<<initial_condition_0[0]<<initial_condition_0[1]<<initial_condition_0[2];
//qDebug()<<"Debug| 1"<<initial_condition_0[0]<<initial_condition_0[1]<<initial_condition_0[2]<<initial_condition_0[3]<<initial_condition_0[4]<<initial_condition_0[5];
//qDebug()<<timein;

        while (flag!=2)
        {
            flag=Runge_Kutta_Fehlberg(threebody_EOM, 6, initial_condition_0, paramv, deriv, &timein, timefinTransfer0, &relerr, abserr, flag );
            if (flag==4)
                flag=Runge_Kutta_Fehlberg(threebody_EOM, 6, initial_condition_0, paramv, deriv, &timein, timefinTransfer0, &relerr, abserr, flag );
  //      qDebug()<<timein;
        }

//qDebug()<<"Debug| 2"<<initial_condition_0[0]<<initial_condition_0[1]<<initial_condition_0[2]<<initial_condition_0[3]<<initial_condition_0[4]<<initial_condition_0[5];
double finalHaloRotX_actual,finalHaloRotY_actual,finalHaloRotZ_actual,finalHaloRotVX_actual,finalHaloRotVY_actual,finalHaloRotVZ_actual;
finalHaloRotX_actual=initial_condition_0[3];
finalHaloRotY_actual=initial_condition_0[4];
finalHaloRotZ_actual=initial_condition_0[5];
finalHaloRotVX_actual=initial_condition_0[0];
finalHaloRotVY_actual=initial_condition_0[1];
finalHaloRotVZ_actual=initial_condition_0[2];

        haloTransfer.manifolds.epsilon=haloTransfer.manifolds.epsilon*(1+x[0]); //1st variable: deviation
        double eigen=haloTransfer.manifolds.eigen1; //eigenvalue in use (if 1 stable manifolds, if 2 unstable)
        //extern int flagHalo;
        //if (flagHalo==2)
        //    eigen=haloTransfer.manifolds.eigen2;

        //qDebug()<<"eigens"<<haloTransfer.manifolds.eigen1<<haloTransfer.manifolds.eigen2;
        for (int j=0;j<6;j++)
            {initial_condition_perturbed[j]=initial_condition_perturbed_0[j]=initial_condition_0[j]-haloTransfer.manifolds.epsilon*eigen*initial_condition_0[j];}
        //qDebug()<<"Debug| 3"<<initial_condition_perturbed[0]<<initial_condition_perturbed[1]<<initial_condition_perturbed[2]<<initial_condition_perturbed[3]<<initial_condition_perturbed[4]<<initial_condition_perturbed[5];
        //qDebug()<<"eigen"<<eigen;

        if (eigen<1) //condition in case of stable manifolds
        {
            initial_condition_perturbed[0]=-initial_condition_perturbed[0];
            initial_condition_perturbed[4]=-initial_condition_perturbed[4];
            initial_condition_perturbed[5]=-initial_condition_perturbed[5];
        }

    //    qDebug()<<"Debug| final"<<initial_condition_perturbed[0]<<initial_condition_perturbed[1]<<initial_condition_perturbed[2]<<initial_condition_perturbed[3]<<initial_condition_perturbed[4]<<initial_condition_perturbed[5]<<haloTransfer.manifolds.error<<haloTransfer.trajectory.Lpoint_Xposition<<haloTransfer.manifolds.kind;

    timein=0;
    double step=0.2;
    double initial_condition_memory[6], initial_interval[6],initial[6];
    double secondBorderTime,firstBorderTime=0;
    extern ScenarioBody* firstBody; extern ScenarioBody* secondBody;
    //qDebug()<<"Bodies"<<firstBody->body()->name()<<secondBody->body()->name();
    double limitTOF; //to speed up the process
    extern int maxTOF;
    limitTOF=maxTOF*2*sta::Pi()/(secondBody->body()->orbitalPeriod());
    double targetX, radiusLimit, stopcondition;
    extern int System,transformationMode;

    if (firstBody->body()->id()==10 || ((System==9 || System==10 || System==11) && transformationMode==4)) //Sun case
    {
        targetX=1-Grav_Param;
        radiusLimit=secondBody->body()->meanRadius()+100;
        stopcondition=3*Rparking;
    }
    else
    {
        targetX=-Grav_Param;
        radiusLimit=firstBody->body()->meanRadius()+100;
        stopcondition=3*Rparking;
    }

    if (Lagrmode==4 && firstBody->body()->id()!=10) //Moon case for mode 4
    {
        targetX=1-Grav_Param;
        radiusLimit=secondBody->body()->meanRadius()+100;
        stopcondition=3*Rparking;
    }

    for (int p=0; p<6; p++)
        initial[p]=initial_condition_perturbed[p];
    double  LastRapproaching=0;
    //qDebug()<<"goals"<<targetX<<radiusLimit<<stopcondition<<firstBody->body()->id();

    /*
       Reducing the suitable search space of the manifold trajectory, depending on the distance from the central body
    */

    timefinTransfer0=0;
    while (step>0.00000001 && fabs(Rapproaching-stopcondition)>100)
    {
        for (int p=0; p<6; p++)
            initial_condition_perturbed[p]=initial[p];
        timein=0;
        flag=1;
        for (int p=0; p<6; p++)
            deriv[p]=0;
        timefinTransfer0=timefinTransfer0+step;
        while (flag!=2)
        {
            flag=Runge_Kutta_Fehlberg(threebody_EOM, 6, initial_condition_perturbed, paramv, deriv, &timein, timefinTransfer0, &relerr, abserr, flag );
            if (flag==4)
                flag=Runge_Kutta_Fehlberg(threebody_EOM, 6, initial_condition_perturbed, paramv, deriv, &timein, timefinTransfer0, &relerr, abserr, flag );
        }
        if (eigen>1)
            radius(Grav_Param,initial_condition_perturbed[3],initial_condition_perturbed[4], initial_condition_perturbed[5], R1,R2);
        else
            radius(Grav_Param,initial_condition_perturbed[3],-initial_condition_perturbed[4], -initial_condition_perturbed[5], R1,R2);
//        qDebug()<<timein;
        LastRapproaching=min(R1,R2)*bodies_distance;
      //  qDebug()<<"Rapproaching"<<Rapproaching<<Rparking;
        if (LastRapproaching<stopcondition)

        {
            timefinTransfer0=timefinTransfer0-step;
            step=step/2;
                
            if(fabs(LastRapproaching-stopcondition)<fabs(Rapproaching-stopcondition) && LastRapproaching>radiusLimit)
            {
                Rapproaching=LastRapproaching;
                firstBorderTime=timefinTransfer0+2*step;
            }
                //qDebug()<<"Rapproaching1"<<Rapproaching<<firstBorderTime<<step<<0.00000001;
        }
        else if ((initial_condition_perturbed[3]<targetX && haloTransfer.trajectory.Lpoint_selected==2) || (initial_condition_perturbed[3]>targetX && haloTransfer.trajectory.Lpoint_selected==1 && firstBody->body()->id()==10))
        {
            timefinTransfer0=timefinTransfer0-step;
            step=step/2;

            /*
               exit condition, the s/c is already too far away from the main body
            */
            if (fabs(initial_condition_perturbed[3]-targetX)<50000/bodies_distance && LastRapproaching>300000)
            {
                //qDebug()<<"exit!"<<step<<0.00000001<<fabs(initial_condition_perturbed[3]-targetX)*bodies_distance<<LastRapproaching;
                step=0.0000000001;
                firstBorderTime=0;
            }
        }
    }
   // if(firstBorderTime!=0)
   // {
   //     qDebug()<<"initial interval!"<<Rapproaching<<3*Rparking;
   // }
    timefinTransfer0=firstBorderTime;
    step=0.05;
    LastRapproaching=50000;
    while (step>0.00001)//&& fabs((Rapproaching-2*Rparking))>100)
    {
        if (firstBorderTime==0)
        {
            step=0.0000000000001;
        }
        for (int p=0; p<6; p++)
            initial_condition_perturbed[p]=initial[p];
        timein=0;
        flag=1;
        for (int p=0; p<6; p++)
            deriv[p]=0;
        timefinTransfer0=timefinTransfer0+step;
        //qDebug()<<41<<step;
        while (flag!=2)
        {
            flag=Runge_Kutta_Fehlberg(threebody_EOM, 6, initial_condition_perturbed, paramv, deriv, &timein, timefinTransfer0, &relerr, abserr, flag );
            if (flag==4)
                flag=Runge_Kutta_Fehlberg(threebody_EOM, 6, initial_condition_perturbed, paramv, deriv, &timein, timefinTransfer0, &relerr, abserr, flag );
        }
        //qDebug()<<42;
        if (eigen>1)
            radius(Grav_Param,initial_condition_perturbed[3],initial_condition_perturbed[4], initial_condition_perturbed[5], R1,R2);
        else
            radius(Grav_Param,initial_condition_perturbed[3],-initial_condition_perturbed[4], -initial_condition_perturbed[5], R1,R2);
    //    qDebug()<<"qui"<<timein<<step;
        Rapproaching=min(R1,R2)*bodies_distance;
    //    qDebug()<<"Rapproaching"<<Rapproaching;
        if (Rapproaching<radiusLimit)
        {//qDebug()<<"Rapproaching"<<Rapproaching<<timefinTransfer0;
            timefinTransfer0=timefinTransfer0-step;
            step=step/2;
            secondBorderTime=timefinTransfer0+step*2;
         //   qDebug()<<"secondBorderTime"<<secondBorderTime;
        }
        else if (Rapproaching>LastRapproaching && Rapproaching>stopcondition)
        {
            timefinTransfer0=timefinTransfer0-step;
            step=step/2;
            secondBorderTime=timefinTransfer0+step*2;
        }
        else
        {
            LastRapproaching=Rapproaching;
        }

//        if(firstBorderTime!=0)
//            {
//                qDebug()<<"Rapproaching2 again"<<Rapproaching<<secondBorderTime;
//            }
    }
    /*
      selection of the patching point along the manifold trajectory
    */

    double totalDeltaV, partialBestTransferTime, partialBestDeltaV=1000;
    if (firstBorderTime==0)
    {
        timefinTransfer0=limitTOF+1;
        secondBorderTime=0;
        totalDeltaV=100;
        partialBestTransferTime=10000;
    }

    else
    {
        //qDebug()<<"final interval!"<<Rapproaching<<3*Rparking<<secondBorderTime;
        //timefinTransfer0=firstBorderTime+x[2]*(secondBorderTime-firstBorderTime); //variable 3: position of the manifold trajectory
    }


    /*
       after this iterative process a good interval next to the main body has been found. If not the borderTimes have been set to 0
       Now 200 points of this interval will be evaluated in order to find the best connection (lowest DeltaV) for this particular orbit.
    */

    //*******************************
    extern double startTime, finalTime, jdMission;
    //finalTime=startTime+1;
    /* in this way the optimal raan is selected, because it changes in 1day
       in order to optimized globally the launch epoch, the search should be conducted over 1year,
       because the relative inclination of the Earth w.r.t. the halo orbit changes in 1year.
    */
    double jdMissionActual;
    jdMissionActual=(finalTime-startTime)*x[2]+startTime; //3rd variable: epoch for the mission
    //jdMission=startTime;

    if (firstBorderTime!=0)
    {
    for (int l=0;l<101;l++)
    {
    flag=1; timein=0;
    timefinTransfer0=firstBorderTime+l*(secondBorderTime-firstBorderTime)/100;
    double newInitial_condition[6];
    for (int p=0;p<6;p++)
    {
        newInitial_condition[p]=initial_condition_perturbed_0[p];
    }
    if (eigen<1)
    {
        newInitial_condition[0]=-initial_condition_perturbed_0[0];
        newInitial_condition[4]=-initial_condition_perturbed_0[4];
        newInitial_condition[5]=-initial_condition_perturbed_0[5];
    }

        for (int p=0; p<6; p++)
            deriv[p]=0;
        while (flag!=2)
        {
            flag=Runge_Kutta_Fehlberg(threebody_EOM, 6, newInitial_condition, paramv, deriv, &timein, timefinTransfer0, &relerr, abserr, flag );
            if (flag==4)
                flag=Runge_Kutta_Fehlberg(threebody_EOM, 6, newInitial_condition, paramv, deriv, &timein, timefinTransfer0, &relerr, abserr, flag );
        }
        if (eigen<1)
            radius(Grav_Param,newInitial_condition[3],-newInitial_condition[4], -newInitial_condition[5], R1,R2);
        else
            radius(Grav_Param,newInitial_condition[3],newInitial_condition[4], newInitial_condition[5], R1,R2);

        Rapproaching=min(R1,R2)*bodies_distance;
        if (Rapproaching<radiusLimit)
        {
            timefinTransfer0=10000;
        }

    sta::StateVector patchingPoint_rotating, patchingPoint_cartesian;
    extern sta::StateVector initialPosition;
    extern double patchingPoint_rotating_x, patchingPoint_rotating_y, patchingPoint_rotating_z, patchingPoint_rotating_Vx, patchingPoint_rotating_Vy, patchingPoint_rotating_Vz;

    patchingPoint_rotating.position.x()=newInitial_condition[3];
    patchingPoint_rotating.position.y()=newInitial_condition[4];
    patchingPoint_rotating.position.z()=newInitial_condition[5];
    patchingPoint_rotating.velocity.x()=newInitial_condition[0];
    patchingPoint_rotating.velocity.y()=newInitial_condition[1];
    patchingPoint_rotating.velocity.z()=newInitial_condition[2];
    if (eigen<1)
    {
        patchingPoint_rotating.position.y()=-newInitial_condition[4];
        patchingPoint_rotating.position.z()=-newInitial_condition[5];
        patchingPoint_rotating.velocity.x()=-newInitial_condition[0];
    }
    else
    {
    patchingPoint_rotating.velocity.x()=-patchingPoint_rotating.velocity.x();
    patchingPoint_rotating.velocity.y()=-patchingPoint_rotating.velocity.y();
    patchingPoint_rotating.velocity.z()=-patchingPoint_rotating.velocity.z();
    }


    patchingPoint_cartesian=rotatingTOcartesian(transformationMode,firstBody->body(), secondBody->body(),patchingPoint_rotating, jdMissionActual);
    extern double twoBodyMu;
    //qDebug()<<firstBody->body()->name()<<secondBody->body()->name()<<patchingPoint_rotating.position.x()<<patchingPoint_cartesian.position.x()<<patchingPoint_rotating.position.y()<<patchingPoint_cartesian.position.y()<<patchingPoint_rotating.position.z()<<patchingPoint_cartesian.position.z()<<transformationMode<<jdMissionActual;

    Rapproaching=patchingPoint_cartesian.position.norm();
    Eigen::Vector3d positionCart,velocityCart,crossProd, nVector;
    positionCart<<patchingPoint_cartesian.position.x(),patchingPoint_cartesian.position.y(),patchingPoint_cartesian.position.z();
    
    double velocityModule=sqrt(twoBodyMu/positionCart.norm());
    double lat,lon;
    lat=asin(positionCart(2)/positionCart.norm());
    lon=atan2(positionCart(1),positionCart(0));
    velocityCart<<-velocityModule*sin(lon)*cos(lat),velocityModule*cos(lon)*cos(lat),velocityModule*sin(lat);
    //velocityCart<<patchingPoint_cartesian.velocity.x(),patchingPoint_cartesian.velocity.y(),patchingPoint_cartesian.velocity.z();
    crossProd=positionCart.cross(velocityCart);
    double inclinationReqActual, raanReqActual, nActual;
    inclinationReqActual=acos(crossProd(2)/crossProd.norm());
    //qDebug()<<"patch cart in evaluate"<<patchingPoint_cartesian.position.x()<<patchingPoint_cartesian.position.y()<<patchingPoint_cartesian.position.z();

    nVector<<-crossProd(1),crossProd(0),0;
    
    //CHECK IT!!!
    //////////////////////////////////////////////////
    if (nVector(1)>=0)
        raanReqActual=acos(nVector(0)/nVector.norm());
    else
        raanReqActual=2*sta::Pi()-acos(nVector(0)/nVector.norm());
    ////////////////////////////////////////////////////

    extern double trueAno;
    double trueAnom;
    trueAnom=acos(nVector.dot(positionCart)/(nVector.norm()*positionCart.norm()));
    if (nVector.dot(velocityCart)>0)
        trueAnom=2*sta::Pi()-trueAnom;  //check if 2*Pi is correct
    //if (raanReqActual>sta::Pi())
        trueAnom=trueAnom+sta::Pi();
    ////////////////////////////////////////////////////

    nActual=Rapproaching/initialPosition.position.norm(); //altitudeRatio
    //qDebug()<<nActual<<Rapproaching<<initialPosition.position.norm();
    extern ScenarioKeplerianElements* elementTransfer;
    //qDebug()<<"old parameters"<<elementTransfer->inclination()<<elementTransfer->raan();
    //qDebug()<<"new parameters"<<radToDeg(inclinationReqActual)<<radToDeg(raanReqActual);

    /*
      maneouvres
    */

    double deltaV1,deltaV2,deltaV3;
    
    //qDebug()<<elementTransfer->inclination()<<initialPosition.position.norm()<<sta::radToDeg(inclinationReqActual)<<Rapproaching;
    double teta=acos(cos(sta::degToRad(elementTransfer->inclination()))*cos(inclinationReqActual)+sin(sta::degToRad(elementTransfer->inclination()))*sin(inclinationReqActual)*(cos(raanReqActual-sta::degToRad(elementTransfer->raan()))));

    //deltaV1=initialPosition.velocity.norm()*(sqrt((3*nActual+1)/(nActual+1)-2*sqrt(2*nActual/(nActual+1)))+sqrt(1/nActual*((nActual+3)/(nActual+1)-2*sqrt(2/(nActual+1))*cos(fabs(sta::degToRad(elementTransfer->inclination())-inclinationReqActual)))));
    deltaV1=2*initialPosition.velocity.norm()*sin(teta/2);
    //qDebug()<<initialPosition.velocity.norm();
    deltaV2=2*sin(sta::degToRad(elementTransfer->inclination()))*sin(fabs(raanReqActual-sta::degToRad(elementTransfer->raan()))/2)*initialPosition.velocity.norm();
    //deltaV3=fabs(patchingPoint_cartesian.velocity.norm()-initialPosition.velocity.norm());
    deltaV3=sqrt(pow(patchingPoint_cartesian.velocity.x()-initialPosition.velocity.x(),2)+pow(patchingPoint_cartesian.velocity.y()-initialPosition.velocity.y(),2)+pow(patchingPoint_cartesian.velocity.z()-initialPosition.velocity.z(),2));
    totalDeltaV=deltaV1+deltaV2+deltaV3;

    /*
      the partial best solution (actually the best solution for the actual transfer trajectory) is that one stored
    */
    if (totalDeltaV<partialBestDeltaV)
    {
        partialBestDeltaV=totalDeltaV;
        partialBestTransferTime=timefinTransfer0;
    }
    extern double BestTotalDeltaV,finalHaloRotX,finalHaloRotY,finalHaloRotZ,finalHaloRotVX,finalHaloRotVY,finalHaloRotVZ,deviation,eigen0;

    /*
      if a global best solution has been found, it's saved in the extern variables to plot it
    */
    if (totalDeltaV<=BestTotalDeltaV && timefinTransfer0<limitTOF && firstBorderTime!=0)
    {
       // qDebug()<<nActual<<raanReqActual<<inclinationReqActual<<totalDeltaV;
        BestTotalDeltaV=totalDeltaV;
        jdMission=jdMissionActual;
        qDebug()<<"Best total DeltaV"<<totalDeltaV<<deltaV1<<deltaV2<<deltaV3<<Rapproaching<<inclinationReq<<jdMission;
        n=nActual;
        raanReq=sta::radToDeg(raanReqActual);
        inclinationReq=sta::radToDeg(inclinationReqActual);
        trueAno=trueAnom;

        patchingPoint_rotating_x=patchingPoint_rotating.position.x();
        patchingPoint_rotating_y=patchingPoint_rotating.position.y();
        patchingPoint_rotating_z=patchingPoint_rotating.position.z();
        patchingPoint_rotating_Vx=patchingPoint_rotating.velocity.x();
        patchingPoint_rotating_Vy=patchingPoint_rotating.velocity.y();
        patchingPoint_rotating_Vz=patchingPoint_rotating.velocity.z();

        /*
        if (flagHalo==2)
        {
            patchingPoint_rotating_x=initial_condition_perturbed_0[3];
            patchingPoint_rotating_y=initial_condition_perturbed_0[4];
            patchingPoint_rotating_z=initial_condition_perturbed_0[5];
            patchingPoint_rotating_Vx=initial_condition_perturbed_0[0];
            patchingPoint_rotating_Vy=initial_condition_perturbed_0[1];
            patchingPoint_rotating_Vz=initial_condition_perturbed_0[2];
        }
        */
        //qDebug()<<"final point inside"<<patchingPoint_cartesian.position.x()<<patchingPoint_cartesian.position.y()<<patchingPoint_cartesian.position.z()<<patchingPoint_cartesian.velocity.x()<<patchingPoint_cartesian.velocity.y()<<patchingPoint_cartesian.velocity.z()<<patchingPoint_cartesian.velocity.norm();
        //qDebug()<<positionCart.norm()<<velocityModule<<velocityCart(0)<<velocityCart(1)<<velocityCart(2);
        //qDebug()<<"trueAno"<<trueAno;
        finalHaloRotX=finalHaloRotX_actual;
        finalHaloRotY=finalHaloRotY_actual;
        finalHaloRotZ=finalHaloRotZ_actual;
        finalHaloRotVX=finalHaloRotVX_actual;
        finalHaloRotVY=finalHaloRotVY_actual;
        finalHaloRotVZ=finalHaloRotVZ_actual;
        deviation=haloTransfer.manifolds.epsilon;
        eigen0=eigen;
        timefinTransfer=timefinTransfer0;
    }
    }
    }

    if (NOBJ==1)
    {
    y[1]=-partialBestTransferTime+limitTOF;
    if (y[1]>0)
        y[1]=0;
    }
    else if (NOBJ==2)
    {
        y[1]=sta::secsToDays(partialBestTransferTime/(2*sta::Pi())*(secondBody->body()->orbitalPeriod()*24*3600));
        y[2]=-partialBestTransferTime+limitTOF;
        if (y[2]>0)
            y[2]=0;
    }

    extern double BestTotalDeltaV;
    //Constraint
    y[0]=partialBestDeltaV;

    /*
    if (firstBorderTime!=0)
    {
        if (NOBJ==1)
            qDebug()<<"sol"<<y[0]<<y[1]<<BestTotalDeltaV<<totalDeltaV;
        else
            qDebug()<<"sol"<<y[0]<<y[1]<<y[2];
    }
    */

    extern int op;
    op=op+1;
    qDebug()<<op;
    //qDebug()<<"variables"<<x[0]<<x[1]<<x[2]<<"fitness"<<y[0]<<y[1];
#endif // OLDSCENARIO

    return y;
}
