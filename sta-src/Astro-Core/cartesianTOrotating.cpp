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

#include "cartesianTOrotating.h"
#include "stabody.h"
#include "stamath.h"
#include "threebodyParametersComputation.h"
#include "ascendingNode.h"
#include <QDebug>

/* this subroutine allows to go from the inertial body centred (usually: Sun or Earth)
   to the normalized co-rotating r.f. defined in the 3b-problem.
*/

sta::StateVector
cartesianTOrotating (int mode, const StaBody* firstbody, const StaBody* secondbody, sta::StateVector cartesian, double JD)

{
    sta::StateVector rotating;
    
    const StaBody* sun = STA_SOLAR_SYSTEM->sun();
    double mu1=getGravParam_user(sun->mu(), secondbody->mu());
    double mu2=getGravParam_user(firstbody->mu(),secondbody->mu());
    //double velocity1=secondbody->linearV();
    double velocity1=sqrt(sun->mu()/secondbody->distance());

    //ephemerides of the 2nd body relative to the Sun
    Eigen::Vector3d positionBody2 =  secondbody->stateVector(JD, sun, sta::COORDSYS_EME_J2000).position;
    Eigen::Vector3d velocityBody2 =  secondbody->stateVector(JD, sun, sta::COORDSYS_EME_J2000).velocity;
    velocity1=velocityBody2.norm();
    //velocity1=secondbody->stateVector(JD, sun, sta::COORDSYS_EME_J2000).velocity.norm();
    double distance1=positionBody2.norm();

    double i;
    double alfa=0; double beta=0;
    Eigen::Vector3d baricenter;

    //coefficients for coming computations
    //double A, B, a, b, c;

    if (firstbody != sun) //we are considering a system like Earth-Moon or Saturn-Titan (the Sun is NOT the first body!)
    {
        //qDebug()<<"CASE 1: NO SUN";
        //velocity1=sqrt(firstbody->mu()/secondbody->distance());

        Eigen::Vector3d positionBody1 = firstbody->stateVector(JD, sun, sta::COORDSYS_EME_J2000).position;
        Eigen::Vector3d positionBody1_Body2 = positionBody2 - positionBody1;
        //distance1=secondbody->distance()-firstbody->distance();
        distance1=positionBody1_Body2.norm();
        Eigen::Vector3d velocityBody1 = firstbody->stateVector(JD, sun, sta::COORDSYS_EME_J2000).velocity;
        Eigen::Vector3d velocityBody2 =  secondbody->stateVector(JD, sun, sta::COORDSYS_EME_J2000).velocity;
        Eigen::Vector3d velocityBody1_Body2 = velocityBody2 - velocityBody1;

        velocity1=velocityBody1_Body2.norm();

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
       //qDebug()<<sta::radToDeg(i);

       //mu1=mu2;
       if (mode==0 || mode==1 || mode==3) //the initial orbit was around the 1st body
       {
           baricenter(0)= (-mu2);//*cos(beta)*sin(alfa); baricenter(1)= (mu2)*sin(beta)*sin(alfa); baricenter(2)= (mu2)*cos(alfa);
           //a=b=c=0;
       }
       else if (mode==2 || mode==4) //the initial orbit was around the 2nd body
       {
           baricenter(0)= (+1-mu2);//*cos(beta)*sin(alfa); baricenter(1)= (-1+mu2)*sin(beta)*sin(alfa); baricenter(2)= (-1+mu2)*cos(alfa);
           //a=b=c=0;
       }
       //A=-1/cos(alfa)*((cartesian.position.x()/distance1-baricenter(0))*cos(beta)+(cartesian.position.y()/distance1-baricenter(1))*sin(beta));
       //B=1/(cos(beta)*sin(alfa))*((cartesian.position.y()/distance1-baricenter(1))*sin(alfa)+(cartesian.position.z()/distance1-baricenter(2))*sin(beta)*cos(alfa));
       rotating.position.x()=(cartesian.position.x()*(cos(alfa)*cos(beta)-cos(i)*sin(alfa)*sin(beta))+cartesian.position.y()*(cos(alfa)*sin(beta)*cos(i)+sin(alfa)*cos(beta))+cartesian.position.z()*sin(i)*sin(alfa))/distance1+baricenter(0);
       rotating.position.y()=(cartesian.position.x()*(-cos(i)*sin(alfa)*cos(beta)-cos(alfa)*sin(beta))+cartesian.position.y()*(cos(i)*cos(alfa)*cos(beta)-sin(alfa)*sin(beta))-cartesian.position.z()*(-sin(i)*cos(alfa)))/distance1;
       rotating.position.z()=(cartesian.position.x()*(sin(i)*sin(beta))+cartesian.position.y()*(sin(i)*cos(beta))+cartesian.position.z()*cos(i))/distance1;
   }
   else
   {
    //same transformation applied to a Sun-body system. This has to be done in any case
    //position of body 2 relative to body1

       //qDebug()<<"CASE 2: SUN";
       //alfa = acos (positionBody2.z()/distance1);
       double nodeJD;
       ascendingNodeAngle (firstbody, secondbody, JD, nodeJD, alfa);
       beta=trueAnomalyFromAscNode (firstbody, secondbody, JD, nodeJD, alfa);
       Eigen::Vector3d velocityCart=secondbody->stateVector(JD, firstbody, sta::COORDSYS_EME_J2000).velocity;
       Eigen::Vector3d crossProd=positionBody2.cross(velocityCart);
       //double inclinationReq;
       i=acos(crossProd(2)/crossProd.norm());

       if (mode==0 || mode==1 || mode==3) //the initial orbit was around the 1st body
       {
           baricenter(0)= (-mu1);//*cos(beta)*sin(alfa); baricenter(1)= (mu1)*sin(beta)*sin(alfa); baricenter(2)= (mu1)*cos(alfa);
           //a=b=c=0;
       }
       else if (mode==2 || mode==4) //the initial orbit was around the 2nd body
       {
           baricenter(0)= (+1-mu1);//*cos(beta)*sin(alfa); baricenter(1)= (-1+mu1)*sin(beta)*sin(alfa); baricenter(2)= (-1+mu1)*cos(alfa);
           //a=b=c=0;
       }
       //A=-1/cos(alfa)*((cartesian.position.x()/distance1-baricenter(0))*cos(beta)+(cartesian.position.y()/distance1-baricenter(1))*sin(beta));
       //B=1/(cos(beta)*sin(alfa))*((cartesian.position.y()/distance1-baricenter(1))*sin(alfa)+(cartesian.position.z()/distance1-baricenter(2))*sin(beta)*cos(alfa));
       rotating.position.x()=(cartesian.position.x()*(cos(alfa)*cos(beta)-cos(i)*sin(alfa)*sin(beta))+cartesian.position.y()*(cos(alfa)*sin(beta)*cos(i)+sin(alfa)*cos(beta))+cartesian.position.z()*sin(i)*sin(alfa))/distance1+baricenter(0);
       rotating.position.y()=(cartesian.position.x()*(-cos(i)*sin(alfa)*cos(beta)-cos(alfa)*sin(beta))+cartesian.position.y()*(cos(i)*cos(alfa)*cos(beta)-sin(alfa)*sin(beta))-cartesian.position.z()*(-sin(i)*cos(alfa)))/distance1;
       rotating.position.z()=(cartesian.position.x()*(sin(i)*sin(beta))+cartesian.position.y()*(sin(i)*cos(beta))+cartesian.position.z()*cos(i))/distance1;
   }

   //A=-1/cos(alfa)*((cartesian.velocity.x()/velocity1)*cos(beta)+(cartesian.velocity.y()/velocity1)*sin(beta));
   //B=1/(cos(beta)*sin(alfa))*((cartesian.velocity.y()/velocity1)*sin(alfa)+(cartesian.velocity.z()/velocity1)*sin(beta)*cos(alfa));
   rotating.velocity.x()=(cartesian.velocity.x()*(cos(alfa)*cos(beta)-cos(i)*sin(alfa)*sin(beta))+cartesian.velocity.y()*(cos(alfa)*sin(beta)*cos(i)+sin(alfa)*cos(beta))+cartesian.velocity.z()*sin(i)*sin(alfa))/velocity1;
   rotating.velocity.y()=(cartesian.velocity.x()*(-cos(i)*sin(alfa)*cos(beta)-cos(alfa)*sin(beta))+cartesian.velocity.y()*(cos(i)*cos(alfa)*cos(beta)-sin(alfa)*sin(beta))-cartesian.velocity.z()*(-sin(i)*cos(alfa)))/velocity1;
   rotating.velocity.z()=(cartesian.velocity.x()*(sin(i)*sin(beta))+cartesian.velocity.y()*(sin(i)*cos(beta))+cartesian.velocity.z()*cos(i))/velocity1;

   return rotating;

}

