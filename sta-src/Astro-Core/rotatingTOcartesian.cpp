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
 ------------------ Author: Valentino Zuccarelli  -------------------------------------------------
 ------------------ E-mail: (Valentino.Zuccarelli@gmail.com) ----------------------------
 */

//#include "rotatingTOcartesian.h"
#include "threebodyParametersComputation.h"
#include "ascendingNode.h"
#include "stamath.h"
#include <QDebug>

/* this subroutine allows to go from the inertial body centred (usually: Sun or Earth)
   to the normalized co-rotating r.f. defined in the 3b-problem.
*/

sta::StateVector
rotatingTOcartesian (int mode, const StaBody* firstbody, const StaBody* secondbody, sta::StateVector rotating, double JD)
{

    //qDebug()<<"rotatingTOcartesian"<<firstbody->name()<<secondbody->name();

    const StaBody* sun = STA_SOLAR_SYSTEM->sun();
    double mu1=getGravParam_user(sun->mu(), secondbody->mu());
    double mu2=getGravParam_user(firstbody->mu(),secondbody->mu());
    //double velocity1=secondbody->linearV();
    double velocity1=sqrt(sun->mu()/secondbody->distance());

    //ephemerides of the 2nd body relative to the Sun
    Eigen::Vector3d positionBody2 =  secondbody->stateVector(JD, sun, sta::COORDSYS_EME_J2000).position;
    double distance1=positionBody2.norm();
    double i;

    //distance1=positionBody2.norm();
    //velocity1=  secondbody->stateVector(JD, sun, sta::COORDSYS_EME_J2000).velocity.norm();
    //distance1=secondbody->distance()-firstbody->distance();
    //qDebug()<<"norm"<<distance1<<"positions"<<positionBody2.x()<<positionBody2.y()<<positionBody2.z();

    sta::StateVector cartesian;
    double alfa=0; double beta=0;
    Eigen::Vector3d baricenter;

    if (firstbody != sun) //we are considering a system like Earth-Moon or Saturn-Titan (the Sun is NOT the first body!)
    {
        //qDebug()<<"CASE 1: NO SUN";

        Eigen::Vector3d positionBody1 = firstbody->stateVector(JD, sun, sta::COORDSYS_EME_J2000).position;
        Eigen::Vector3d positionBody1_Body2 = positionBody2 - positionBody1;
        distance1=positionBody1_Body2.norm();
        velocity1=sqrt(firstbody->mu()/secondbody->distance());
       //alfa = acos (positionBody1_Body2.z()/distance2);

       double nodeJD;
       ascendingNodeAngle (firstbody, secondbody, JD, nodeJD, alfa);
       beta=trueAnomalyFromAscNode (firstbody, secondbody, JD, nodeJD, alfa);
       //beta = atan2 (sqrt(pow(positionBody1_Body2.x(),2.0)+pow(positionBody1_Body2.y(),2.0)),positionBody1_Body2.z());
       //alfa = atan2 (positionBody1_Body2.y(), positionBody1_Body2.x());

    Eigen::Vector3d velocityCart=secondbody->stateVector(JD, firstbody, sta::COORDSYS_EME_J2000).velocity;
    Eigen::Vector3d crossProd=positionBody1_Body2.cross(velocityCart);
    //double inclinationReq;
    i=acos(crossProd(2)/crossProd.norm());
    //qDebug()<<"inclination"<<sta::radToDeg(i);

       //qDebug()<<sta::radToDeg(alfa)<<sta::radToDeg(sta::Pi()/2-beta)<<positionBody1_Body2.x()<<positionBody1_Body2.y()<<positionBody1_Body2.z();

    if (mode==0 || mode==1 || mode==3) //the initial orbit was around the 1st body
    {
        baricenter(0)= (mu2);//*cos(beta)*sin(alfa); baricenter(1)= (mu2)*sin(beta)*sin(alfa); baricenter(2)= (mu2)*cos(alfa);
        //qDebug()<<"A";
        /*
        cartesian.position.x()=(baricenter(0)+rotating.position.x()*cos(beta)*sin(alfa)*cos(i)-rotating.position.z()*cos(beta)*cos(alfa)*cos(i)-rotating.position.y()*sin(beta)*cos(i))*distance1;
        cartesian.position.y()=(baricenter(1)+rotating.position.x()*sin(beta)*sin(alfa)*cos(i)-rotating.position.z()*sin(beta)*cos(alfa)*cos(i)+rotating.position.y()*cos(beta)*cos(i))*distance1;
        cartesian.position.z()=(baricenter(2)+rotating.position.x()*cos(alfa)*cos(i)+rotating.position.z()*sin(alfa)*cos(i)+rotating.position.y()*sin(i))*distance1;
        */

        //cartesian.position.x()=((baricenter(0)+rotating.position.x())*sin(beta)*cos(alfa)+rotating.position.z()*cos(beta)+rotating.position.y()*sin(beta)*sin(alfa))*distance1;
        //cartesian.position.y()=((baricenter(0)+rotating.position.x())*(-cos(beta)*cos(alfa)*sin(i)-cos(i)*sin(alfa))+rotating.position.z()*sin(beta)*sin(i)+rotating.position.y()*(cos(alfa)*cos(i)-sin(i)*sin(alfa)*cos(beta)))*distance1;
        //cartesian.position.z()=((baricenter(0)+rotating.position.x())*(sin(i)*sin(alfa)-cos(alfa)*cos(i)*cos(beta))+rotating.position.z()*sin(beta)*cos(i)+rotating.position.y()*(-cos(i)*sin(alfa)*cos(beta)-sin(i)*cos(alfa)))*distance1;
        cartesian.position.x()=((baricenter(0)+rotating.position.x())*(cos(beta)*cos(alfa)-cos(i)*sin(alfa)*sin(beta))+rotating.position.z()*(sin(i)*sin(alfa))+rotating.position.y()*(-cos(i)*sin(alfa)*cos(beta)-cos(alfa)*sin(beta)))*distance1;
        cartesian.position.y()=((baricenter(0)+rotating.position.x())*(cos(i)*cos(alfa)*sin(beta)+sin(alfa)*cos(beta))+rotating.position.z()*(-sin(i)*cos(alfa))+rotating.position.y()*(cos(alfa)*cos(i)*cos(beta)-sin(alfa)*sin(beta)))*distance1;
        cartesian.position.z()=((baricenter(0)+rotating.position.x())*(sin(i)*sin(beta))+rotating.position.z()*cos(i)+rotating.position.y()*cos(beta)*sin(i))*distance1;
    }
    else if (mode==2 || mode==4) //the initial orbit was around the 2nd body
    {
        baricenter(0)= (-1+mu2);//*cos(beta)*sin(alfa); baricenter(1)= (-1+mu2)*sin(beta)*sin(alfa); baricenter(2)= (-1+mu2)*cos(alfa);
        //qDebug()<<"B";
        //cartesian.position.x()=(baricenter(0)+rotating.position.x()*cos(beta)*sin(alfa)-rotating.position.z()*cos(beta)*cos(alfa)-rotating.position.y()*sin(beta))*distance1;
        //cartesian.position.y()=(baricenter(1)+rotating.position.x()*sin(beta)*sin(alfa)-rotating.position.z()*sin(beta)*cos(alfa)+rotating.position.y()*cos(beta))*distance1;
        //cartesian.position.z()=(baricenter(2)+rotating.position.x()*cos(alfa)+rotating.position.z()*sin(alfa))*distance1;

        cartesian.position.x()=((baricenter(0)+rotating.position.x())*(cos(beta)*cos(alfa)-cos(i)*sin(alfa)*sin(beta))+rotating.position.z()*(sin(i)*sin(alfa))+rotating.position.y()*(-cos(i)*sin(alfa)*cos(beta)-cos(alfa)*sin(beta)))*distance1;
        cartesian.position.y()=((baricenter(0)+rotating.position.x())*(cos(i)*cos(alfa)*sin(beta)+sin(alfa)*cos(beta))+rotating.position.z()*(-sin(i)*cos(alfa))+rotating.position.y()*(cos(alfa)*cos(i)*cos(beta)-sin(alfa)*sin(beta)))*distance1;
        cartesian.position.z()=((baricenter(0)+rotating.position.x())*(sin(i)*sin(beta))+rotating.position.z()*cos(i)+rotating.position.y()*cos(beta)*sin(i))*distance1;
    }
    }
    else
    {

    //same transformation applied to a Sun-body system. This has to be done in any case
    //position of body 2 relative to body1

       //qDebug()<<"\nCASE 2: SUN"<<distance1;
       //alfa = acos (positionBody2.z()/distance1);
       double nodeJD;
       ascendingNodeAngle (firstbody, secondbody, JD, nodeJD, alfa);
       beta=trueAnomalyFromAscNode (firstbody, secondbody, JD, nodeJD, alfa);
       //beta = atan2 (sqrt(pow(positionBody2.x(),2.0)+pow(positionBody2.y(),2.0)),positionBody2.z());
       //alfa = atan2 (positionBody2.y(), positionBody2.x());
       Eigen::Vector3d velocityCart=secondbody->stateVector(JD, firstbody, sta::COORDSYS_EME_J2000).velocity;
    Eigen::Vector3d crossProd=positionBody2.cross(velocityCart);
    //double inclinationReq;
    i=acos(crossProd(2)/crossProd.norm());
    //qDebug()<<"inclination computed"<<sta::radToDeg(i);

    if (mode==0 || mode==1 || mode==3) //the initial orbit was around the Sun
    {
        
        //qDebug()<<"A";
        baricenter(0)= (mu1);//*cos(beta)*sin(alfa); baricenter(1)= (mu1)*sin(beta)*sin(alfa); baricenter(2)= (mu1)*cos(alfa);
        //cartesian.position.x()=(baricenter(0)+rotating.position.x()*cos(beta)*sin(alfa)-rotating.position.z()*cos(beta)*cos(alfa)-rotating.position.y()*sin(beta))*distance1;
        //cartesian.position.y()=(baricenter(1)+rotating.position.x()*sin(beta)*sin(alfa)-rotating.position.z()*sin(beta)*cos(alfa)+rotating.position.y()*cos(beta))*distance1;
        //cartesian.position.z()=(baricenter(2)+rotating.position.x()*cos(alfa)+rotating.position.z()*sin(alfa))*distance1;
       
        cartesian.position.x()=((baricenter(0)+rotating.position.x())*(cos(beta)*cos(alfa)-cos(i)*sin(alfa)*sin(beta))+rotating.position.z()*(sin(i)*sin(alfa))+rotating.position.y()*(-cos(i)*sin(alfa)*cos(beta)-cos(alfa)*sin(beta)))*distance1;
        cartesian.position.y()=((baricenter(0)+rotating.position.x())*(cos(i)*cos(alfa)*sin(beta)+sin(alfa)*cos(beta))+rotating.position.z()*(-sin(i)*cos(alfa))+rotating.position.y()*(cos(alfa)*cos(i)*cos(beta)-sin(alfa)*sin(beta)))*distance1;
        cartesian.position.z()=((baricenter(0)+rotating.position.x())*(sin(i)*sin(beta))+rotating.position.z()*cos(i)+rotating.position.y()*cos(beta)*sin(i))*distance1;
    }
    else if (mode==2 || mode==4) //the initial orbit was around the 2nd body
    {
        
        //qDebug()<<"B";
        baricenter(0)= (-1+mu1);//*cos(beta)*sin(alfa); baricenter(1)= (-1+mu1)*sin(beta)*sin(alfa); baricenter(2)= (-1+mu1)*cos(alfa);
        //cartesian.position.x()=(baricenter(0)+rotating.position.x()*cos(beta)*sin(alfa)-rotating.position.z()*cos(beta)*cos(alfa)-rotating.position.y()*sin(beta))*distance1;
        //cartesian.position.y()=(baricenter(1)+rotating.position.x()*sin(beta)*sin(alfa)-rotating.position.z()*sin(beta)*cos(alfa)+rotating.position.y()*cos(beta))*distance1;
        //cartesian.position.z()=(baricenter(2)+rotating.position.x()*cos(alfa)+rotating.position.z()*sin(alfa))*distance1;

        cartesian.position.x()=((baricenter(0)+rotating.position.x())*(cos(beta)*cos(alfa)-cos(i)*sin(alfa)*sin(beta))+rotating.position.z()*(sin(i)*sin(alfa))+rotating.position.y()*(-cos(i)*sin(alfa)*cos(beta)-cos(alfa)*sin(beta)))*distance1;
        cartesian.position.y()=((baricenter(0)+rotating.position.x())*(cos(i)*cos(alfa)*sin(beta)+sin(alfa)*cos(beta))+rotating.position.z()*(-sin(i)*cos(alfa))+rotating.position.y()*(cos(alfa)*cos(i)*cos(beta)-sin(alfa)*sin(beta)))*distance1;
        cartesian.position.z()=((baricenter(0)+rotating.position.x())*(sin(i)*sin(beta))+rotating.position.z()*cos(i)+rotating.position.y()*cos(beta)*sin(i))*distance1;
        
    }
    }

    //cartesian.velocity.x()=(rotating.velocity.x()*cos(beta)*sin(alfa)-rotating.velocity.z()*cos(beta)*cos(alfa)-rotating.velocity.y()*sin(beta))*velocity1;
    //cartesian.velocity.y()=(rotating.velocity.x()*sin(beta)*sin(alfa)-rotating.velocity.z()*sin(beta)*cos(alfa)+rotating.velocity.y()*cos(beta))*velocity1;
    //cartesian.velocity.z()=(rotating.velocity.x()*cos(alfa)+rotating.velocity.z()*sin(alfa))*velocity1;
    //qDebug()<<"distance cart"<<cartesian.position.norm();

    cartesian.velocity.x()=(rotating.velocity.x()*(cos(beta)*cos(alfa)-cos(i)*sin(alfa)*sin(beta))+rotating.velocity.z()*(sin(i)*sin(alfa))+rotating.velocity.y()*(-cos(i)*sin(alfa)*cos(beta)-cos(alfa)*sin(beta)))*velocity1;
    cartesian.velocity.y()=(rotating.velocity.x()*(cos(i)*cos(alfa)*sin(beta)+sin(alfa)*cos(beta))+rotating.velocity.z()*(-sin(i)*cos(alfa))+rotating.velocity.y()*(cos(alfa)*cos(i)*cos(beta)-sin(alfa)*sin(beta)))*velocity1;
    cartesian.velocity.z()=(rotating.velocity.x()*(sin(i)*sin(beta))+rotating.velocity.z()*cos(i)+rotating.velocity.y()*cos(beta)*sin(i))*velocity1;

    return cartesian;
}

