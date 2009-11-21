
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
 */

/*
------ Copyright (C) 2009 European Space Agency (space.trajectory.analysis AT gmail.com) ----
 ------------------ Author: Valentino Zuccarelli  -------------------------------------------------
 ------------------ E-mail: (Valentino.Zuccarelli@gmail.com) ----------------------------
 */

#include "scenariotrajectorypropagation.h"
#include "scenariosimulationparameters.h"
#include "scenariothreebodyenvironment.h"
#include "scenariothreebodytransfer.h"
#include "scenariomanoeuvreplan.h"
#include "scenarioinitialstate.h"
#include "scenariobody.h"
#include "propagationfeedback.h"
#include "scenarioparticipant.h"
#include "scenariolagrangian.h"
#include "scenariohalo.h"
#include "scenariomanifold.h"
#include "scenariomanoeuvre.h"
#include "Astro-Core/threebodyParametersComputation.h"
#include "Lagrangian/halorbitcomputation.h"
#include "Astro-Core/ephparms.h"
#include "Astro-Core/getGreenwichHourAngle.h"

// For trajectory propagation
#include "Astro-Core/date.h"
#include "Astro-Core/cartesianTOrotating.h"
#include "Astro-Core/cartesianTOorbital.h"
#include "Astro-Core/orbitalTOcartesian.h"
#include "Astro-Core/rotatingTOcartesian.h"
#include "Astro-Core/propagateTHREEbody.h"
#include "Astro-Core/propagateTWObody.h"
#include "Astro-Core/EODE/eode.h"
#include "Astro-Core/stamath.h"
#include "celestia-src/celmath/vecmath.h"
#include "Main/celestiainterface.h"
#include "Plotting/groundtrackplottool.h"

#include <QTreeWidgetItem>
#include <QtDebug>
#include <QTextStream>
#include <QFile>
#include <QDir>
#include <QMessageBox>

extern double Grav_Param;
extern int  coordinates_transformation, halo_num_positions;
extern QString halo_input_data;

using namespace Eigen;

ScenarioLagrangianTrajectory::ScenarioLagrangianTrajectory() :
    m_threebodyenvironment(NULL),
    m_simulationParameters(NULL),
    m_trajectoryPropagation(NULL),
    m_halo(NULL),
    m_threebodyTransfer(NULL)
{
    m_threebodyenvironment = new ScenarioThreebodyenvironment();
    attachChild(m_threebodyenvironment);
    m_simulationParameters = new ScenarioSimulationParameters(1);
    attachChild(m_simulationParameters);
    m_halo = new ScenarioHalo();
    attachChild (m_halo);
    m_threebodyTransfer = new ScenarioThreebodyTransfer();
    attachChild (m_threebodyTransfer);
}


void
ScenarioLagrangianTrajectory::writeElement(QXmlStreamWriter* out)
{
    out->writeStartElement(elementName());
    //out->writeAttribute("Name", name());
    writeContents(out);
    out->writeEndElement();
}


void
ScenarioLagrangianTrajectory::writeContents(QXmlStreamWriter* out)
{
    if (m_threebodyenvironment)
        m_threebodyenvironment->writeElement(out);
    if (m_simulationParameters)
        m_simulationParameters->writeElement(out);
    if (m_halo)
        m_halo->writeElement(out);
    if (m_threebodyTransfer)
        m_threebodyTransfer->writeElement(out);
}


void
ScenarioLagrangianTrajectory::createItemContents(QTreeWidgetItem* item)
{

    if (m_threebodyenvironment)
        m_threebodyenvironment->createItem(item);
    if (m_simulationParameters)
        m_simulationParameters->createItem(item);
    if (m_halo)
        m_halo->createItem(item);
    if (m_threebodyTransfer)
        m_threebodyTransfer->createItem(item);
}


sta::StateVector
ScenarioLagrangianTrajectory::computeInitialStateVector() const
{
    if (coordinates_transformation==1 || coordinates_transformation==0 || coordinates_transformation==3 || coordinates_transformation==5)
    {
        //qDebug()<<"FIRST BODY"<<threebodyenvironment()->firstBody()->body()->name();
        //qDebug()<<"initialStateVector body"<<threebodyenvironment()->firstBody()->body()->name();
        return m_simulationParameters->initialStatePosition()->computeStateVector(threebodyenvironment()->firstBody()->body());
    }
    else if (coordinates_transformation==2 || coordinates_transformation==4)
    {
        //qDebug()<<"initialStateVector body"<<threebodyenvironment()->secondBody()->body()->name();
        //qDebug()<<"SECOND BODY"<<threebodyenvironment()->secondBody()->body()->name();
        return m_simulationParameters->initialStatePosition()->computeStateVector(threebodyenvironment()->secondBody()->body());
    }
}



/*! Get the origin of the trajectory.
 */
const StaBody*
ScenarioLagrangianTrajectory::firstBody() const
{
    return threebodyenvironment()->firstBody()->body();
}


const StaBody*
ScenarioLagrangianTrajectory::secondBody() const
{
    return threebodyenvironment()->secondBody()->body();
}


sta::StateVector
ScenarioLagrangianTrajectory::propagate(PropagationFeedback& propFeedback,
                                       const sta::StateVector& initialState,
                                       QList<double>& sampleTimes,
                                       QList<sta::StateVector>& samples)
{
    extern int Lagrmode;
    //qDebug()<<"3BM mode:"<<Lagrmode;
    double distance=secondBody()->distance();
    const ScenarioExtendedTimeline* timeline = simulationParameters()->timeline();
    double velocity=secondBody()->linearV();
    double Grav_Param=getGravParam_user (firstBody()->mu(),secondBody()->mu());
    double orbitalPeriod=(2*sta::Pi()*sqrt(pow(secondBody()->distance(),3)/STA_SOLAR_SYSTEM->sun()->mu()));
    if (firstBody()->id()!=10)
        orbitalPeriod=(2*sta::Pi()*sqrt(pow(secondBody()->distance(),3)/firstBody()->mu()));
    //qDebug()<<orbitalPeriod/3600/24<<sqrt(STA_SOLAR_SYSTEM->sun()->mu()/secondBody()->distance());
    sta::StateVector stateVector, newstateVector;


    if (Lagrmode==0)
{
    // Calling the propagation routine iteratively as to find each state vector per step size untill the elapsed time
    double dt = timeline->timeStep();

    //qDebug()<<"Starting time"<<timeline->startTime();
    //qDebug()<<"Final time"<<timeline->endTime();
    //qDebug()<<"Step size"<<timeline->timeStep();

    if (dt == 0.0)
    {
        propFeedback.raiseError(QObject::tr("Time step is zero!"));
        return initialState;
    }

    stateVector = initialState;    
    //qDebug()<<"input position"<<initialState.position.x()<<initialState.position.y()<<initialState.position.z();
    //qDebug()<<"input velocity"<<initialState.velocity.x()<<initialState.velocity.y()<<initialState.velocity.z();
    //qDebug()<<distance<<Grav_Param<<velocity;

    //qDebug()<<"coordinates input"<<coordinates_transformation;

    //qDebug()<<"deltaV"<<simulationParameters()->deltav()->Xvelocity()<<simulationParameters()->deltav()->Yvelocity()<<simulationParameters()->deltav()->Zvelocity();

    sta::StateVector initialState_rotating=initialState;

    // Transformed initial state
    sta::StateVector xformInitialState = initialState;

    if (coordinates_transformation!=0)
    {
        double sma, e, inclination, argOfPeriapsis, raan, meanAnomaly;

        /*
        cartesianTOorbital(Grav_Param,
                       initialState.position.x(), initialState.position.y(), initialState.position.z(),
                       initialState.velocity.x(), initialState.velocity.y(), initialState.velocity.z(),
                       sma, e, inclination, argOfPeriapsis, raan, trueAnomaly);
        */

        sta::KeplerianElements foundKeplerianElements = cartesianTOorbital(Grav_Param, initialState);

        sma            = foundKeplerianElements.SemimajorAxis;
        e              = foundKeplerianElements.Eccentricity;
        inclination    = foundKeplerianElements.Inclination;
        raan           = foundKeplerianElements.AscendingNode;
        argOfPeriapsis = foundKeplerianElements.ArgumentOfPeriapsis;
        meanAnomaly    = foundKeplerianElements.MeanAnomaly;

        double perigee=sma*(1-e);
        if (perigee<centralBody()->meanRadius())
        {
            QMessageBox::warning(NULL,
                              QObject::tr("The trajectory has been not propagated"),
                              QObject::tr("The perigee distance is smaller than the main body radius."));
            return stateVector.zero();
        }

        //double R1,R2;
      //  const StaBody* sun = STA_SOLAR_SYSTEM->sun();

    //ephemerides of the 2nd body relative to the Sun
    //Eigen::Vector3d positionBody2 =  secondBody()->stateVector(timeline->startTime(), sun, sta::COORDSYS_EME_J2000).position;
    //velocity=secondBody()->stateVector(timeline->startTime(), sun, sta::COORDSYS_EME_J2000).velocity.norm();
    //Eigen::Vector3d positionBody1 = firstBody()->stateVector(timeline->startTime(), sun, sta::COORDSYS_EME_J2000).position;
  //      Eigen::Vector3d positionBody1_Body2 = positionBody2 - positionBody1;
//    distance=positionBody1_Body2.norm();

      //  qDebug()<<"function check"<<coordinates_transformation;
        //qDebug()<<initialState.position.x()<<initialState.position.y()<<initialState.position.z()<<initialState.position.norm()<<initialState.velocity.x()<<initialState.velocity.y()<<initialState.velocity.z()<<initialState.velocity.norm();
        initialState_rotating=cartesianTOrotating(coordinates_transformation, firstBody(), secondBody(), initialState, timeline->startTime());
        //radiusss (Grav_Param, initialState_rotating.position.x(), initialState_rotating.position.y(), initialState_rotating.position.z(), R1, R2);
        //qDebug()<<initialState_rotating.position.x()<<initialState_rotating.position.y()<<initialState_rotating.position.z()<<initialState_rotating.velocity.x()<<initialState_rotating.velocity.y()<<initialState_rotating.velocity.z()<<initialState_rotating.velocity.norm()*velocity;
        //sta::StateVector initialState_check;
        //initialState_check=rotatingTOcartesian(coordinates_transformation, firstBody(), secondBody(), initialState_rotating, timeline->startTime());
        //qDebug()<<initialState_check.position.x()<<initialState_check.position.y()<<initialState_check.position.z()<<initialState_check.position.norm()<<initialState_check.velocity.x()<<initialState_check.velocity.y()<<initialState_check.velocity.z()<<initialState_check.velocity.norm();

    xformInitialState = initialState_rotating;
    //qDebug()<<xformInitialState.position.x()<<xformInitialState.position.y()<<xformInitialState.position.z()<<xformInitialState.velocity.x()<<xformInitialState.velocity.y()<<xformInitialState.velocity.z()<<xformInitialState.velocity.norm()*velocity;

    }
    else
    {
        stateVector=rotatingTOcartesian (coordinates_transformation, firstBody(), secondBody(), initialState, timeline->startTime());
    }

#if 0
     int digits_number;
        if (accuracy==1) 		//accuracy 1km
                {digits_number=(floor (log(distance)/log(10))+2)-1; //number of significant digits
                accuracy=pow(1/10.0,digits_number); //number of digit for the distance between the primaries. This number +1 gives the accuracy level for 1km
                }
        else if (accuracy==10)	//accuracy 10km
                {digits_number=floor (log(distance)/log(10)+1)-1;
                accuracy=pow(1/10.0,digits_number);
                }
        else if (accuracy==100)	//accuracy 100km
                {digits_number=floor (log(distance)/log(10))-1;
                accuracy=pow(1/10.0,digits_number);
                }
        else if (accuracy==1000)	//accuracy 1000km
                {digits_number=floor (log(distance)/log(10)-1)-1;
                accuracy=pow(1/10.0,digits_number);
                }
#endif
    double timefin=dt;
    double state[6];

    JulianDate jd=timeline->startTime();  int i=0;
    double accuracy=1E-15;
    int flag; double relerr, abserr, deriv[6]; double paramv[1]={0}; flag=1;
    abserr=relerr=accuracy;
    double timein=0;//double final[5];

    double endtime; double newtimein=timein; bool changed=false;

    if (simulationParameters()->deltav()->Time()<(timeline->endTime()-timeline->startTime()) && simulationParameters()->deltav()->Time()!=0)
        endtime=simulationParameters()->deltav()->Time()+timeline->startTime();
    else
        endtime=timeline->endTime();

    //qDebug()<<xformInitialState.position.x()<<xformInitialState.position.y()<<xformInitialState.position.z()<<xformInitialState.velocity.x()<<xformInitialState.velocity.y()<<xformInitialState.velocity.z()<<xformInitialState.velocity.norm()*velocity;

    state[0]=xformInitialState.velocity.x();
    state[1]=xformInitialState.velocity.y();
    state[2]=xformInitialState.velocity.z();
    state[3]=xformInitialState.position.x();
    state[4]=xformInitialState.position.y();
    state[5]=xformInitialState.position.z();
    sampleTimes << timeline->startTime();
    samples << stateVector;
            //qDebug()<<"initial state"<<state[3]<<state[4]<<state[5]<<state[0]<<state[1]<<state[2]<<timein;


    for (int p=0; p<6; p++)
            deriv[p]=0;
    while (jd < endtime)
    {
        i=i+1;
        jd = timeline->startTime() + i*sta::secsToDays(dt);

        timefin=(dt*i)*2*sta::Pi()/orbitalPeriod;

        if (timein==0)
        {
            state[0]=xformInitialState.velocity.x();
            state[1]=xformInitialState.velocity.y();
            state[2]=xformInitialState.velocity.z();
            state[3]=xformInitialState.position.x();
            state[4]=xformInitialState.position.y();
            state[5]=xformInitialState.position.z();
        }
        //qDebug()<<"jd"<<jd<<"---------------------";
        //qDebug()<<"initial state"<<state[3]<<state[4]<<state[5]<<state[0]<<state[1]<<state[2]<<timein;
        //qDebug()<<xformInitialState.position.x()<<xformInitialState.position.y()<<xformInitialState.position.z()<<xformInitialState.velocity.x()<<xformInitialState.velocity.y()<<xformInitialState.velocity.z()<<xformInitialState.velocity.norm()*velocity;

        flag=1;
        while (flag!=2)
        {
            flag=Runge_Kutta_Fehlberg(threebody_EOM, 6, state, paramv, deriv, &timein, timefin, &relerr, abserr, flag );

            if (flag==4)
                flag=Runge_Kutta_Fehlberg(threebody_EOM, 6, state, paramv, deriv, &timein, timefin, &relerr, abserr, flag );
        }
        //qDebug()<<"final state"<<timefin<<state[0]<<state[1]<<state[2]<<state[3]<<state[4]<<state[5];

        //double final[5];
        //for (int u=0; u<6; u++)
            //final[u]=state[u];
        //qDebug()<<"final state"<<timefin<<final[0]<<final[1]<<final[2]<<final[3]<<final[4]<<final[5];

        //qDebug()<<"final state"<<timefin<<state[0]<<state[1]<<state[2]<<state[3]<<state[4]<<state[5];

        //double C;
        //Jacobi(Grav_Param,state[3],state[4],state[5],state[0],state[1],state[2], C);
        //qDebug()<<"C"<<C<<jd;
        //qDebug()<<"final state"<<timefin<<state[0]<<state[1]<<state[2]<<state[3]<<state[4]<<state[5];

        sta::StateVector rotating;
        rotating.position.x()=state[3]; rotating.position.y()=state[4]; rotating.position.z()=state[5];
        rotating.velocity.x()=state[0]; rotating.velocity.y()=state[1]; rotating.velocity.z()=state[2];


        //if (coordinates_transformation==0)
            stateVector=rotatingTOcartesian (coordinates_transformation, firstBody(), secondBody(), rotating, jd);
            //rotating=cartesianTOrotating(coordinates_transformation, firstBody(), secondBody(), stateVector, jd);
            //qDebug()<<timefin<<rotating.velocity.x()<<rotating.velocity.y()<<rotating.velocity.z()<<rotating.position.x()<<rotating.position.y()<<rotating.position.z();
            //qDebug()<<stateVector.position.x()<<stateVector.position.y()<<stateVector.position.z()<<stateVector.velocity.x()<<stateVector.velocity.y()<<stateVector.velocity.z();

        /*
        else
        {
        //better transformation in case of keplerian elements, or body fixed inital coordinates
        if (coordinates_transformation==1 || coordinates_transformation==3)  // || coordinates_transformation==0)
            stateVector.position.x()=(+Grav_Param+final[3])*distance;
        else if (coordinates_transformation==2 || coordinates_transformation==4)
            stateVector.position.x()=(-1+Grav_Param+final[3])*distance;

        stateVector.position.y()=final[4]*distance;
        stateVector.position.z()=final[5]*distance;
        stateVector.velocity.x()=final[0]*velocity;
        stateVector.velocity.y()=final[1]*velocity;
        stateVector.velocity.z()=final[2]*velocity;        
        }
*/

        // Append a trajectory sample
        sampleTimes << jd;
        samples << stateVector;

        if (simulationParameters()->deltav()->Time()!=0 && changed == false)
        if (jd>=(simulationParameters()->deltav()->Time()))
        {
            //qDebug()<<"Man"<<simulationParameters()->deltav()->Xvelocity()<<simulationParameters()->deltav()->Yvelocity()<<simulationParameters()->deltav()->Zvelocity();
            endtime=timeline->endTime();
            //qDebug()<<state[0]<<state[1]<<state[2]<<state[3]<<state[4]<<state[5]<<jd;
            stateVector.velocity.x()=stateVector.velocity.x()+simulationParameters()->deltav()->Xvelocity();
            stateVector.velocity.y()=stateVector.velocity.y()+simulationParameters()->deltav()->Yvelocity();
            stateVector.velocity.z()=stateVector.velocity.z()+simulationParameters()->deltav()->Zvelocity();
            //qDebug()<<stateVector.position.x()<<stateVector.position.y()<<stateVector.position.z()<<stateVector.velocity.x()<<stateVector.velocity.y()<<stateVector.velocity.z();
            rotating=cartesianTOrotating(coordinates_transformation, firstBody(), secondBody(), stateVector, jd);
            state[0]=rotating.velocity.x();
            state[1]=rotating.velocity.y();
            state[2]=rotating.velocity.z();
            state[3]=rotating.position.x();
            state[4]=rotating.position.y();
            state[5]=rotating.position.z();
            //qDebug()<<state[0]<<state[1]<<state[2]<<state[3]<<state[4]<<state[5];
            //qDebug()<<"new initial conditions"<<state[0]<<state[1]<<state[2];
            newtimein=timefin;
            changed=true;
        }
    }
    //qDebug()<<samples.size();

    //computation of lagrangian points
    HaloOrbit lagrangian;
    lagrangian.lpointsComputation ();

    //file 3BMsystem.stam  
    QString file = "3BMsystem.stam";
    QFile threeBMsystemFile(file);
    threeBMsystemFile.open(QIODevice::WriteOnly | QIODevice::Text);

    QTextStream outsys(&threeBMsystemFile);
    outsys.setRealNumberNotation(QTextStream::FixedNotation);
    outsys.setRealNumberPrecision(16);
    int system=getSystem (firstBody()->id(),secondBody()->id());
    if (system==0)
        system=30; //user system, as ICD states

    outsys<<system<<"\n";
    outsys<<Grav_Param<<"\n";
    outsys<<distance<<"\n";
    outsys<<lagrangian.L1_xposition<<"\n";
    outsys<<lagrangian.L2_xposition<<"\n";
    threeBMsystemFile.close();
    //qDebug()<<file;
    return stateVector;
}
else if (Lagrmode==1 || Lagrmode==2 || Lagrmode==3 || Lagrmode==4) //halo orbit propagation
{

    //TO DO inserisci pop-up in caso il nome del file sia "", significa ke non e stata calcolata l'halo
    if (Lagrmode==3 || Lagrmode==4)
    {
        halo_input_data="3BMhalorbit00.stae";
        halo_num_positions=1000;
    }
    QFile file_halo_reader(halo_input_data);
    file_halo_reader.open(QIODevice::ReadOnly | QIODevice::Text);

    QTextStream in(&file_halo_reader);
    double jd, jd0=1;

    if (firstBody()->id()!=10) //sun
    {
        //qDebug()<<"evve"<<coordinates_transformation;
        coordinates_transformation=5;
        centralBody();
    }
    extern int transformationMode;

    if (firstBody()->id()!=10 && transformationMode==3)
    {
        qDebug()<<"evve"<<coordinates_transformation;
        coordinates_transformation=3;
        centralBody();
    }


    for(int i=1;i<halo_num_positions+1;i++)
    {
       in>>jd>>stateVector.position.x()>>stateVector.position.y()>>stateVector.position.z()>>stateVector.velocity.x()>>stateVector.velocity.y()>>stateVector.velocity.z();
       if (i==2)
           jd0=jd;

       if (Lagrmode==3 || Lagrmode==4)
       {
           extern double timefinTransfer;
           //to plot the Halo orbit at the end of the transfer trajectory
           extern double jdMission;
           if (firstBody()->id()==10)
               jd=sta::secsToDays((timefinTransfer)/(2*Pi())*orbitalPeriod)-2.8+jd*3/(jd0*(halo_num_positions-1))+jdMission;
           else
               jd=sta::secsToDays((timefinTransfer)/(2*Pi())*(orbitalPeriod))-0.8+jd*1/(jd0*(halo_num_positions-1))+jdMission;
       }

       //qDebug()<<jd<<stateVector.position.x()<<stateVector.position.y()<<stateVector.position.z()<<stateVector.velocity.x()<<stateVector.velocity.y()<<stateVector.velocity.z();
       newstateVector=rotatingTOcartesian(2, firstBody(), secondBody(),stateVector, jd);

#if 0
       //---------------------
       stateVector.position.x()=(+Grav_Param+stateVector.position.x())*distance;
       stateVector.position.y()=stateVector.position.y()*distance;//-sin(sta::degToRad(xls-meanSolarAnomaly))*149597887.5;
       stateVector.position.z()=stateVector.position.z()*distance;
       stateVector.velocity.x()=stateVector.velocity.x()*velocity;
       stateVector.velocity.y()=stateVector.velocity.y()*velocity;
       stateVector.velocity.z()=stateVector.velocity.z()*velocity;
       //-----------------------------------------------------------------
#endif
       //qDebug()<<"zero"<<sqrt(pow(newstateVector.position.x(),2.0)+pow(newstateVector.position.y(),2.0)+pow(newstateVector.position.z(),2.0))<<distance;
       samples<<newstateVector;
       sampleTimes<<jd;
       //qDebug()<<jd<<newstateVector.position.x()<<newstateVector.position.y()<<newstateVector.position.z()<<newstateVector.velocity.x()<<newstateVector.velocity.y()<<newstateVector.velocity.z();
    }

    file_halo_reader.close();
}

if (Lagrmode==3)
{
    extern int numObj;
    sta::StateVector stateVector;
    //if (numObj==2)
      //  return stateVector.zero();

    extern int transformationMode;
    qDebug()<<"transformationMode"<<transformationMode<<coordinates_transformation<<firstBody()->name()<<secondBody()->name();

    extern double twoBodyMu, trueAno;

//    extern ScenarioKeplerianElements* elementTransfer;
    extern double inclinationInit, eInit, trueAnomalyInit, smaInit, argOfPeriapsisInit, raanInit, jdMission;
    extern double raanReq, inclinationReq, n;


    double argOfPeriapsis      = sta::degToRad(argOfPeriapsisInit);
    double trueAnomaly         = sta::degToRad(trueAnomalyInit);
    double raan                = sta::degToRad(raanInit);
    double inclination         = sta::degToRad(inclinationInit);
    double sma                 = smaInit;
    double e = eInit;
    double argOfPeriapsisUpdated      = 0.0;
    double trueAnomalyUpdated         = 0.0;
    double raanUpdated                = 0.0;

    double sma2, e2, sma3, trueAnomalyManeouvre, trueAnomalyAfter, teta;
    double orbitalPeriod1=2*sta::Pi()*sqrt(pow(smaInit,3.0)/twoBodyMu);
    e2=(smaInit*n-smaInit)/(smaInit*n+smaInit);
    sma2=(smaInit+n*smaInit)/2;
    double orbitalPeriod2=Pi()*sqrt(pow(sma2,3.0)/twoBodyMu);
    sma3=n*smaInit;
    //qDebug()<<trueAno;
    double orbitalPeriod3=2*Pi()*sqrt(pow(sma3,3.0)/twoBodyMu);
    teta=acos(cos(inclination)*cos(sta::degToRad(inclinationReq))+sin(inclination)*sin(sta::degToRad(inclinationReq))*(cos(sta::degToRad(raanReq)-raan)));
    if (sta::degToRad(raanReq)>Pi())
            teta=-teta;
    trueAnomalyManeouvre=acos((sin(sta::degToRad(inclinationReq))*cos(sta::degToRad(raanReq)-raan)-cos(teta)*sin(inclination))/sin(teta)/cos(inclination));
    trueAnomalyAfter=acos((cos(inclination)*sin(sta::degToRad(inclinationReq))-sin(inclination)*cos(sta::degToRad(raanReq)-raan)*cos(sta::degToRad(inclinationReq)))/sin(teta));
    double initialJd = jdMission-sta::secsToDays(orbitalPeriod1)-sta::secsToDays(orbitalPeriod3)*trueAno/2/Pi()-sta::secsToDays(orbitalPeriod1)*(trueAnomalyManeouvre/2/Pi()+(2*Pi()-trueAnomalyAfter)/2/Pi())-sta::secsToDays(orbitalPeriod2);

    //qDebug()<<"period"<<elementTransfer->semimajorAxis()<<twoBodyMu<<orbitalPeriod1/3600;

    stateVector=orbitalTOcartesian(twoBodyMu,sma,e,inclination,argOfPeriapsis,raan,trueAnomaly);

    //qDebug()<<"TIME"<< initialJd<<jdMission<<sta::secsToDays(orbitalPeriod1)<<sta::secsToDays(orbitalPeriod2)<<sta::secsToDays(orbitalPeriod3);
    sampleTimes << initialJd;
    samples << stateVector;
    //qDebug()<<"eccentricity 0"<<e<<sta::radToDeg(raan)<<sta::radToDeg(inclination)<<sma<<argOfPeriapsis<<trueAnomaly;

    /*
       1 initial orbit around the central body
    */
    double jd=initialJd;
    for (int i=1;i<101;i++)
    {
        stateVector=propagateTWObody(twoBodyMu,
                     sma,
                     e,
                     inclination,
                     argOfPeriapsis,
                     raan,
                     trueAnomaly,
                     orbitalPeriod1/100,
                     raanUpdated,
                     argOfPeriapsisUpdated,
                     trueAnomalyUpdated);
        // Append a trajectory sample
        jd=jd+sta::secsToDays(orbitalPeriod1/100);
        sampleTimes << jd;
        samples << stateVector;
            // Exchanging the parameters for the next iteration loop
        argOfPeriapsis = argOfPeriapsisUpdated;
        trueAnomaly    = trueAnomalyUpdated;
        raan           = raanUpdated;
    }
    //qDebug()<<"TIME1"<< jd<<sta::secsToDays(orbitalPeriod1);

    //qDebug()<<"period1"<<jd<<sta::secsToDays(orbitalPeriod1);
      //  qDebug()<<"eccentricity1"<<e<<sta::radToDeg(raan)<<sta::radToDeg(inclination)<<sma<<argOfPeriapsis<<trueAnomaly;


    /*
       maneouvres: change of RAAN (istantaneous) and inclination-altitude
    */

   // qDebug()<<"inclinations"<<inclination<<degToRad(inclinationReq);
    //qDebug()<<"raanReq"<<raanReq;

    {
        //qDebug()<<"TETA"<<sta::radToDeg(teta);
        //qDebug()<<"si???"<<trueAnomaly<<trueAnomalyManeouvre;

        bool maneouvre=false;
        while (trueAnomaly<4*Pi()*(1-0.00005))
        {
            //qDebug()<<"raan change!"<<stateVector.position.z()<<stateVector.position.norm()*sin(inclination);
            if (trueAnomaly>=(trueAnomalyManeouvre*(1-0.00005)+2*Pi())&&maneouvre==false)
            {
                //qDebug()<<"position"<<trueAnomalyManeouvre<<trueAnomaly;
                //qDebug()<<"manovra!"<<jd<<twoBodyMu<<sma<<e<<inclination<<argOfPeriapsis<<raan<<degToRad(raanReq)<<trueAnomaly<<"raan change!"<<stateVector.position.z()<<stateVector.position.norm()*sin(inclination);
                raan=sta::degToRad(raanReq);
                inclination=sta::degToRad(inclinationReq);
                trueAnomaly=trueAnomalyAfter+2*Pi();

                maneouvre=true;
                //qDebug()<<twoBodyMu<<sma<<e<<inclination<<argOfPeriapsis<<raan<<trueAnomaly;
                //qDebug()<<"TIME2"<< jd<<sta::secsToDays(orbitalPeriod1)*(trueAnomalyManeouvre/2/Pi());
            }
            stateVector=propagateTWObody(twoBodyMu,
                     sma,
                     e,
                     inclination,
                     argOfPeriapsis,
                     raan,
                     trueAnomaly,
                     orbitalPeriod1/1200,
                     raanUpdated,
                     argOfPeriapsisUpdated,
                     trueAnomalyUpdated);
        // Append a trajectory sample
        jd=jd+sta::secsToDays(orbitalPeriod1/1200);
        sampleTimes << jd;
        samples << stateVector;
            // Exchanging the parameters for the next iteration loop
        argOfPeriapsis = argOfPeriapsisUpdated;
        trueAnomaly    = trueAnomalyUpdated;
        raan           = raanUpdated;
        }
        //qDebug()<<"trueAnomaly"<<trueAnomaly;
    }

    //qDebug()<<"boh"<<jd<<sta::secsToDays(orbitalPeriod1*2);

//qDebug()<<"TIME3"<< jd<<sta::secsToDays(orbitalPeriod1)*((2*Pi()-trueAnomalyAfter)/2/Pi());
//qDebug()<<"eccentricity2"<<e<<sta::radToDeg(raan)<<sta::radToDeg(inclination)<<sma<<argOfPeriapsis<<trueAnomaly;

    //qDebug()<<n<<raanReq<<inclinationReq;
    e = e2;
    sma                 = sma2;


    //qDebug()<<"eccentricity3"<<e<<sta::radToDeg(raan)<<sta::radToDeg(inclination)<<sma<<argOfPeriapsis<<trueAnomaly;
    //trueAnomaly=-sqrt(twoBodyMu/pow(sma,3.0))*orbitalPeriod2/101;
   // qDebug()<<"trueA"<<trueAnomaly;

    trueAnomaly=4*Pi();
    for (int i=1;i<101;i++)
    {
        stateVector=propagateTWObody(twoBodyMu,
                     sma,
                     e,
                     inclination,
                     argOfPeriapsis,
                     raan,
                     trueAnomaly,
                     orbitalPeriod2/100,
                     raanUpdated,
                     argOfPeriapsisUpdated,
                     trueAnomalyUpdated);
        // Append a trajectory sample
        jd=jd+sta::secsToDays(orbitalPeriod2/100);
        sampleTimes << jd;
        samples << stateVector;
            // Exchanging the parameters for the next iteration loop
        argOfPeriapsis = argOfPeriapsisUpdated;
        trueAnomaly    = trueAnomalyUpdated;
        raan           = raanUpdated;
    }
    //qDebug()<<"TIME4"<< jd<<sta::secsToDays(orbitalPeriod2);
    sma=sma3;
    e=0;
    while (trueAnomaly<5*Pi()+trueAno*(1-0.00005))
    {
        stateVector=propagateTWObody(twoBodyMu,
                     sma,
                     e,
                     inclination,
                     argOfPeriapsis,
                     raan,
                     trueAnomaly,
                     orbitalPeriod3/1200,
                     raanUpdated,
                     argOfPeriapsisUpdated,
                     trueAnomalyUpdated);
        // Append a trajectory sample
        jd=jd+sta::secsToDays(orbitalPeriod3/1200);
        sampleTimes << jd;
        samples << stateVector;
            // Exchanging the parameters for the next iteration loop
        argOfPeriapsis = argOfPeriapsisUpdated;
        trueAnomaly    = trueAnomalyUpdated;
        raan           = raanUpdated;
    }
    //qDebug()<<"boh2"<<jd<<sta::secsToDays(orbitalPeriod1*3+orbitalPeriod2);
    //qDebug()<<"final orbit"<<jd<<sma<<e<<sta::radToDeg(inclination)<<sta::radToDeg(raan)<<trueAno<<trueAnomaly;

    sta::StateVector patchingPointRotating;
    sta::StateVector patch_cart;
    extern double eigen0;

    extern double patchingPoint_rotating_x, patchingPoint_rotating_y, patchingPoint_rotating_z, patchingPoint_rotating_Vx, patchingPoint_rotating_Vy, patchingPoint_rotating_Vz;
    patchingPointRotating.position.x()=patchingPoint_rotating_x;
    patchingPointRotating.position.y()=patchingPoint_rotating_y;
    patchingPointRotating.position.z()=patchingPoint_rotating_z;
    if(eigen0>1)
    {
    patchingPointRotating.velocity.x()=-patchingPoint_rotating_Vx;
    patchingPointRotating.velocity.y()=-patchingPoint_rotating_Vy;
    patchingPointRotating.velocity.z()=-patchingPoint_rotating_Vz;
    }
    else
    {
    patchingPointRotating.velocity.x()=patchingPoint_rotating_Vx;
    patchingPointRotating.velocity.y()=patchingPoint_rotating_Vy;
    patchingPointRotating.velocity.z()=patchingPoint_rotating_Vz;
    }

    patch_cart=rotatingTOcartesian (transformationMode, firstBody(), secondBody(), patchingPointRotating, jdMission);
    //qDebug()<<"patch point rot"<<patchingPointRotating.position.x()<<patchingPointRotating.position.y()<<patchingPointRotating.position.z()<<patchingPointRotating.velocity.x()<<patchingPointRotating.velocity.y()<<patchingPointRotating.velocity.z();
    //qDebug()<<"final point hohmann"<<jd<<stateVector.position.x()<<stateVector.position.y()<<stateVector.position.z()<<stateVector.velocity.x()<<stateVector.velocity.y()<<stateVector.velocity.z()<<stateVector.velocity.norm();
    //qDebug()<<"patch cart"<<jdMission<<patch_cart.position.x()<<patch_cart.position.y()<<patch_cart.position.z()<<patch_cart.velocity.x()<<patch_cart.velocity.y()<<patch_cart.velocity.z();

    extern double timefinTransfer;

    double flag=1;
    double step=timefinTransfer/20000;
    double paramv[1]={0}; double deriv[6], initial_condition[6]; double timein, timefin=0;
    double relerr, abserr, accuracy; abserr=relerr=accuracy=1E-10;
    initial_condition[3]=patchingPoint_rotating_x;
    initial_condition[4]=patchingPoint_rotating_y;
    initial_condition[5]=patchingPoint_rotating_z;
    initial_condition[0]=patchingPoint_rotating_Vx;
    initial_condition[1]=patchingPoint_rotating_Vy;
    initial_condition[2]=patchingPoint_rotating_Vz;
    double limitDistance; //below this value there is an impact with the central body
    if (transformationMode==2 || transformationMode==4) //central body=2nd body
        limitDistance=secondBody()->meanRadius();
    else
        limitDistance=firstBody()->meanRadius();

    extern double finalHaloRotX,finalHaloRotY,finalHaloRotZ,finalHaloRotVX,finalHaloRotVY,finalHaloRotVZ,deviation;

    sta::StateVector finalHaloCart,finalHaloRot,finalHaloRotPerturbed,finalHaloCartPerturbed;
    //qDebug()<<eigen0<<deviation;
    finalHaloRot.position.x()=finalHaloRotX;    finalHaloRotPerturbed.position.x()=finalHaloRotX-finalHaloRotX*eigen0*deviation;
    finalHaloRot.position.y()=finalHaloRotY;    finalHaloRotPerturbed.position.y()=finalHaloRotY-finalHaloRotY*eigen0*deviation;
    finalHaloRot.position.z()=finalHaloRotZ;    finalHaloRotPerturbed.position.z()=finalHaloRotZ-finalHaloRotZ*eigen0*deviation;
    finalHaloRot.velocity.x()=finalHaloRotVX;   finalHaloRotPerturbed.velocity.x()=finalHaloRotVX-finalHaloRotVX*eigen0*deviation;
    finalHaloRot.velocity.y()=finalHaloRotVY;   finalHaloRotPerturbed.velocity.y()=finalHaloRotVY-finalHaloRotVY*eigen0*deviation;
    finalHaloRot.velocity.z()=finalHaloRotVZ;   finalHaloRotPerturbed.velocity.z()=finalHaloRotVZ-finalHaloRotVZ*eigen0*deviation;
    //finalHaloCart=rotatingTOcartesian (transformationMode, firstBody(), secondBody(), finalHaloRot, jdMission+sta::secsToDays((timefinTransfer)/(2*Pi())*(orbitalPeriod)));
    //qDebug()<<jdMission+sta::secsToDays((timefinTransfer)/(2*Pi())*(orbitalPeriod));

    timein=0;

    if (eigen0>1)
    {
        initial_condition[4]=-patchingPoint_rotating_y;
        initial_condition[5]=-patchingPoint_rotating_z;
        initial_condition[0]=-patchingPoint_rotating_Vx;
    }

    while (timefin<=timefinTransfer-step)
    {
     for (int p=0; p<6; p++)
        deriv[p]=0;
    flag=1;
    timefin=timefin+step;

    //qDebug()<<timein<<timefin;
    while (flag!=2)
    {
        //qDebug()<<jd<<timefin;
        flag=Runge_Kutta_Fehlberg(threebody_EOM, 6, initial_condition, paramv, deriv, &timein, timefin, &relerr, abserr, flag );
        if (flag==4)
            flag=Runge_Kutta_Fehlberg(threebody_EOM, 6, initial_condition, paramv, deriv, &timein, timefin, &relerr, abserr, flag );
    }
        patch_cart.position.x()=initial_condition[3];
        patch_cart.position.y()=initial_condition[4];
        patch_cart.position.z()=initial_condition[5];
        patch_cart.velocity.x()=initial_condition[0];
        patch_cart.velocity.y()=initial_condition[1];
        patch_cart.velocity.z()=initial_condition[2];
        if (eigen0>1)
        {
            patch_cart.position.y()=-initial_condition[4];
            patch_cart.position.z()=-initial_condition[5];
            patch_cart.velocity.x()=-initial_condition[0];
        }

        //qDebug()<<initial_condition[3]<<-initial_condition[4]<<-initial_condition[5];
        jd=sta::secsToDays(step/(2*Pi())*(orbitalPeriod))+jd;
        //qDebug()<<"jd-timeOfIntegration"<<jd<<timefin;
        stateVector=rotatingTOcartesian (transformationMode, firstBody(), secondBody(), patch_cart, jd);
        if (stateVector.position.norm()<limitDistance)
        {
            QMessageBox::warning(NULL,
                              QObject::tr("Transfer orbit not computed"),
                              QObject::tr("The trajectory impacts the central body. Maybe the starting epoch doesn't allow this kind of transfer orbit"));
            return stateVector.zero();
        }

        sampleTimes << jd;
        samples << stateVector;

    }
    double DeltaPosition_goal,DeltaVelocity_goal, DeltaPosition,DeltaVelocity;
    finalHaloCart=rotatingTOcartesian (transformationMode, firstBody(), secondBody(), finalHaloRot, jd);
    finalHaloCartPerturbed=rotatingTOcartesian (transformationMode, firstBody(), secondBody(), finalHaloRotPerturbed, jd);
    DeltaPosition=stateVector.position.norm()-finalHaloCart.position.norm();
    DeltaPosition_goal=stateVector.position.norm()-finalHaloCartPerturbed.position.norm();
    DeltaVelocity=stateVector.velocity.norm()-finalHaloCart.velocity.norm();
    DeltaVelocity_goal=stateVector.velocity.norm()-finalHaloCartPerturbed.velocity.norm();
    //qDebug()<<"final orbit position"<<stateVector.position.x()<<stateVector.position.y()<<stateVector.position.z()<<stateVector.velocity.x()<<stateVector.velocity.y()<<stateVector.velocity.z();
    //qDebug()<<"Halo orbit position"<<finalHaloCart.position.x()<<finalHaloCart.position.y()<<finalHaloCart.position.z()<<finalHaloCart.velocity.x()<<finalHaloCart.velocity.y()<<finalHaloCart.velocity.z();
    //qDebug()<<"delta R&V (halo-haloPerturbed)"<<DeltaPosition<<DeltaVelocity<<DeltaPosition_goal<<DeltaVelocity_goal<<"\n";
    qDebug()<<"\n\n"<<"Position deviation:"<<DeltaPosition<<"km";
    qDebug()<<"Velocity deviation:"<<DeltaVelocity<<"km/s";
    qDebug()<<"TOF:"<<jd-initialJd<<"days";
    extern double BestTotalDeltaV;
    qDebug()<<"Total DeltaV:"<<BestTotalDeltaV<<"km/s";
    //qDebug()<<finalHaloCart.position.norm()-finalHaloCartPerturbed.position.norm()<<finalHaloCart.velocity.norm()-finalHaloCartPerturbed.velocity.norm();
//qDebug()<<timefinTransfer<<jdMission<<sta::secsToDays((timefinTransfer)/(2*Pi())*(orbitalPeriod))<<jdMission+sta::secsToDays((timefinTransfer)/(2*Pi())*(orbitalPeriod));;

    return stateVector;
}

if (Lagrmode==5)
{
    extern double trueAno_Leg1, inclinationInit_Leg1, eInit_Leg1, trueAnomalyInit_Leg1, smaInit_Leg1, argOfPeriapsisInit_Leg1, raanInit_Leg1, jdMission_Leg1;
    extern double raanReq_Leg1, inclinationReq_Leg1, n_Leg1;
    extern double patchingPoint_rotating_x_Leg1, patchingPoint_rotating_y_Leg1, patchingPoint_rotating_z_Leg1, patchingPoint_rotating_Vx_Leg1, patchingPoint_rotating_Vy_Leg1, patchingPoint_rotating_Vz_Leg1;
    extern double trueAno_Leg2, inclinationInit_Leg2, eInit_Leg2, trueAnomalyInit_Leg2, smaInit_Leg2, argOfPeriapsisInit_Leg2, raanInit_Leg2, jdMission_Leg2;
    extern double raanReq_Leg2, inclinationReq_Leg2, n_Leg2;
    extern double patchingPoint_rotating_x_Leg2, patchingPoint_rotating_y_Leg2, patchingPoint_rotating_z_Leg2, patchingPoint_rotating_Vx_Leg2, patchingPoint_rotating_Vy_Leg2, patchingPoint_rotating_Vz_Leg2;

    //first leg Earth-L2 Earth-Sun
    extern int numObj;
    sta::StateVector stateVector;
    const StaBody* sun = STA_SOLAR_SYSTEM->sun();
    const StaBody* earth = STA_SOLAR_SYSTEM->earth();
    const StaBody* moon = STA_SOLAR_SYSTEM->lookup(STA_MOON);

    double jd=timeline->startTime();
    double twoBodyMu=earth->mu();

    extern double trueAno, inclinationInit, eInit, trueAnomalyInit, smaInit, argOfPeriapsisInit, raanInit, jdMission;
    extern double raanReq, inclinationReq, n;


    double argOfPeriapsis      = sta::degToRad(argOfPeriapsisInit);
    double trueAnomaly         = sta::degToRad(trueAnomalyInit);
    double raan                = sta::degToRad(raanInit);
    double inclination         = sta::degToRad(inclinationInit);
    double sma                 = smaInit;
    double e = eInit;
    double argOfPeriapsisUpdated      = 0.0;
    double trueAnomalyUpdated         = 0.0;
    double raanUpdated                = 0.0;

    double sma2, e2, sma3, trueAnomalyManeouvre, trueAnomalyAfter, teta;
    double orbitalPeriod1=2*sta::Pi()*sqrt(pow(smaInit,3.0)/twoBodyMu);
    e2=(smaInit*n-smaInit)/(smaInit*n+smaInit);
    sma2=(smaInit+n*smaInit)/2;
    double orbitalPeriod2=Pi()*sqrt(pow(sma2,3.0)/twoBodyMu);
    sma3=n*smaInit;
    //qDebug()<<trueAno;
    double orbitalPeriod3=2*Pi()*sqrt(pow(sma3,3.0)/twoBodyMu);
    teta=acos(cos(inclination)*cos(sta::degToRad(inclinationReq))+sin(inclination)*sin(sta::degToRad(inclinationReq))*(cos(sta::degToRad(raanReq)-raan)));

    //////// CHECK!
    //if (degToRad(inclinationReq)-inclination<0 && sta::degToRad(raanReq)>Pi())
    if (sta::degToRad(raanReq)>Pi())
            teta=-teta;
    else if (degToRad(inclinationReq)-inclination>0 && sta::degToRad(raanReq)>Pi())
        teta=teta+Pi()/2;
        

    trueAnomalyManeouvre=acos((sin(sta::degToRad(inclinationReq))*cos(sta::degToRad(raanReq)-raan)-cos(teta)*sin(inclination))/sin(teta)/cos(inclination));
    trueAnomalyAfter=acos((cos(inclination)*sin(sta::degToRad(inclinationReq))-sin(inclination)*cos(sta::degToRad(raanReq)-raan)*cos(sta::degToRad(inclinationReq)))/sin(teta));
    double initialJd = jdMission-sta::secsToDays(orbitalPeriod1)-sta::secsToDays(orbitalPeriod3)*trueAno/2/Pi()-sta::secsToDays(orbitalPeriod1)*(trueAnomalyManeouvre/2/Pi()+(2*Pi()-trueAnomalyAfter)/2/Pi())-sta::secsToDays(orbitalPeriod2);

    //qDebug()<<"period"<<elementTransfer->semimajorAxis()<<twoBodyMu<<orbitalPeriod1/3600;

    stateVector=orbitalTOcartesian(twoBodyMu,sma,e,inclination,argOfPeriapsis,raan,trueAnomaly);

    //qDebug()<<"TIME"<< initialJd<<jdMission<<sta::secsToDays(orbitalPeriod1)<<sta::secsToDays(orbitalPeriod2)<<sta::secsToDays(orbitalPeriod3);
    sampleTimes << initialJd;
    samples << stateVector;
    //qDebug()<<"eccentricity 0"<<e<<sta::radToDeg(raan)<<sta::radToDeg(inclination)<<sma<<argOfPeriapsis<<trueAnomaly;

    /*
       1 initial orbit around the central body
    */
    jd=initialJd;
    for (int i=1;i<101;i++)
    {
        stateVector=propagateTWObody(twoBodyMu,
                     sma,
                     e,
                     inclination,
                     argOfPeriapsis,
                     raan,
                     trueAnomaly,
                     orbitalPeriod1/100,
                     raanUpdated,
                     argOfPeriapsisUpdated,
                     trueAnomalyUpdated);
        // Append a trajectory sample
        jd=jd+sta::secsToDays(orbitalPeriod1/100);
        sampleTimes << jd;
        samples << stateVector;
            // Exchanging the parameters for the next iteration loop
        argOfPeriapsis = argOfPeriapsisUpdated;
        trueAnomaly    = trueAnomalyUpdated;
        raan           = raanUpdated;
    }
    //qDebug()<<"TIME1"<< jd<<sta::secsToDays(orbitalPeriod1);

    //qDebug()<<"period1"<<jd<<sta::secsToDays(orbitalPeriod1);
      //  qDebug()<<"eccentricity1"<<e<<sta::radToDeg(raan)<<sta::radToDeg(inclination)<<sma<<argOfPeriapsis<<trueAnomaly;


    /*
       maneouvres: change of RAAN (istantaneous) and inclination-altitude
    */

   // qDebug()<<"inclinations"<<inclination<<degToRad(inclinationReq);
    //qDebug()<<"raanReq"<<raanReq;

    {
        //qDebug()<<"TETA"<<sta::radToDeg(teta);
        //qDebug()<<"si???"<<trueAnomaly<<trueAnomalyManeouvre;

        bool maneouvre=false;
        while (trueAnomaly<4*Pi()*(1-0.00005))
        {
            //qDebug()<<"raan change!"<<stateVector.position.z()<<stateVector.position.norm()*sin(inclination);
            if (trueAnomaly>=(trueAnomalyManeouvre*(1-0.00005)+2*Pi())&&maneouvre==false)
            {
                //qDebug()<<"position"<<trueAnomalyManeouvre<<trueAnomaly;
                //qDebug()<<"manovra!"<<jd<<twoBodyMu<<sma<<e<<inclination<<argOfPeriapsis<<raan<<degToRad(raanReq)<<trueAnomaly<<"raan change!"<<stateVector.position.z()<<stateVector.position.norm()*sin(inclination);
                raan=sta::degToRad(raanReq);
                inclination=sta::degToRad(inclinationReq);
                trueAnomaly=trueAnomalyAfter+2*Pi();

                maneouvre=true;
                //qDebug()<<twoBodyMu<<sma<<e<<inclination<<argOfPeriapsis<<raan<<trueAnomaly;
                //qDebug()<<"TIME2"<< jd<<sta::secsToDays(orbitalPeriod1)*(trueAnomalyManeouvre/2/Pi());
            }
            stateVector=propagateTWObody(twoBodyMu,
                     sma,
                     e,
                     inclination,
                     argOfPeriapsis,
                     raan,
                     trueAnomaly,
                     orbitalPeriod1/1200,
                     raanUpdated,
                     argOfPeriapsisUpdated,
                     trueAnomalyUpdated);
        // Append a trajectory sample
        jd=jd+sta::secsToDays(orbitalPeriod1/1200);
        sampleTimes << jd;
        samples << stateVector;
            // Exchanging the parameters for the next iteration loop
        argOfPeriapsis = argOfPeriapsisUpdated;
        trueAnomaly    = trueAnomalyUpdated;
        raan           = raanUpdated;
        }
        //qDebug()<<"trueAnomaly"<<trueAnomaly;
    }

    //qDebug()<<"boh"<<jd<<sta::secsToDays(orbitalPeriod1*2);

//qDebug()<<"TIME3"<< jd<<sta::secsToDays(orbitalPeriod1)*((2*Pi()-trueAnomalyAfter)/2/Pi());
//qDebug()<<"eccentricity2"<<e<<sta::radToDeg(raan)<<sta::radToDeg(inclination)<<sma<<argOfPeriapsis<<trueAnomaly;

    //qDebug()<<n<<raanReq<<inclinationReq;
    e = e2;
    sma                 = sma2;


    //qDebug()<<"eccentricity3"<<e<<sta::radToDeg(raan)<<sta::radToDeg(inclination)<<sma<<argOfPeriapsis<<trueAnomaly;
    //trueAnomaly=-sqrt(twoBodyMu/pow(sma,3.0))*orbitalPeriod2/101;
   // qDebug()<<"trueA"<<trueAnomaly;

    trueAnomaly=4*Pi();
    for (int i=1;i<101;i++)
    {
        stateVector=propagateTWObody(twoBodyMu,
                     sma,
                     e,
                     inclination,
                     argOfPeriapsis,
                     raan,
                     trueAnomaly,
                     orbitalPeriod2/100,
                     raanUpdated,
                     argOfPeriapsisUpdated,
                     trueAnomalyUpdated);
        // Append a trajectory sample
        jd=jd+sta::secsToDays(orbitalPeriod2/100);
        sampleTimes << jd;
        samples << stateVector;
            // Exchanging the parameters for the next iteration loop
        argOfPeriapsis = argOfPeriapsisUpdated;
        trueAnomaly    = trueAnomalyUpdated;
        raan           = raanUpdated;
    }
    //qDebug()<<"TIME4"<< jd<<sta::secsToDays(orbitalPeriod2);
    sma=sma3;
    e=0;
    while (trueAnomaly<5*Pi()+trueAno*(1-0.00005))
    {
        stateVector=propagateTWObody(twoBodyMu,
                     sma,
                     e,
                     inclination,
                     argOfPeriapsis,
                     raan,
                     trueAnomaly,
                     orbitalPeriod3/1200,
                     raanUpdated,
                     argOfPeriapsisUpdated,
                     trueAnomalyUpdated);
        // Append a trajectory sample
        jd=jd+sta::secsToDays(orbitalPeriod3/1200);
        sampleTimes << jd;
        samples << stateVector;
            // Exchanging the parameters for the next iteration loop
        argOfPeriapsis = argOfPeriapsisUpdated;
        trueAnomaly    = trueAnomalyUpdated;
        raan           = raanUpdated;
    }
    //qDebug()<<"boh2"<<jd<<sta::secsToDays(orbitalPeriod1*3+orbitalPeriod2);
    //qDebug()<<"final orbit"<<jd<<sma<<e<<sta::radToDeg(inclination)<<sta::radToDeg(raan);

    sta::StateVector patchingPointRotating; extern int transformationMode;
    sta::StateVector patch_cart;

    extern double patchingPoint_rotating_x, patchingPoint_rotating_y, patchingPoint_rotating_z, patchingPoint_rotating_Vx, patchingPoint_rotating_Vy, patchingPoint_rotating_Vz;
    patchingPointRotating.position.x()=patchingPoint_rotating_x;
    patchingPointRotating.position.y()=patchingPoint_rotating_y;
    patchingPointRotating.position.z()=patchingPoint_rotating_z;
    patchingPointRotating.velocity.x()=patchingPoint_rotating_Vx;
    patchingPointRotating.velocity.y()=patchingPoint_rotating_Vy;
    patchingPointRotating.velocity.z()=patchingPoint_rotating_Vz;

    patch_cart=rotatingTOcartesian (transformationMode, firstBody(), secondBody(), patchingPointRotating, jdMission);

    //qDebug()<<"patch point rot"<<patchingPointRotating.position.x()<<patchingPointRotating.position.y()<<patchingPointRotating.position.z()<<patchingPointRotating.velocity.x()<<patchingPointRotating.velocity.y()<<patchingPointRotating.velocity.z();
    //qDebug()<<"final point"<<jd<<stateVector.position.x()<<stateVector.position.y()<<stateVector.position.z()<<stateVector.velocity.x()<<stateVector.velocity.y()<<stateVector.velocity.z()<<stateVector.velocity.norm();
    //qDebug()<<"patch cart"<<jdMission<<patch_cart.position.x()<<patch_cart.position.y()<<patch_cart.position.z()<<patch_cart.velocity.x()<<patch_cart.velocity.y()<<patch_cart.velocity.z();

    extern double timefinTransfer;

    double flag=1;
    double step=timefinTransfer/20000;
    double paramv[1]={0}; double deriv[6], initial_condition[6]; double timein, timefin=0;
    double relerr, abserr, accuracy; abserr=relerr=accuracy=1E-10;
    initial_condition[3]=patchingPoint_rotating_x;
    initial_condition[4]=-patchingPoint_rotating_y;
    initial_condition[5]=-patchingPoint_rotating_z;
    initial_condition[0]=-patchingPoint_rotating_Vx;
    initial_condition[1]=patchingPoint_rotating_Vy;
    initial_condition[2]=patchingPoint_rotating_Vz;
    double limitDistance; //below this value there is an impact with the central body
    if (transformationMode==2 || transformationMode==4) //central body=2nd body
        limitDistance=secondBody()->meanRadius();
    else
        limitDistance=firstBody()->meanRadius();

    extern double finalHaloRotX,finalHaloRotY,finalHaloRotZ,finalHaloRotVX,finalHaloRotVY,finalHaloRotVZ,eigen0,deviation;

    sta::StateVector finalHaloCart,finalHaloRot,finalHaloRotPerturbed,finalHaloCartPerturbed;
    //qDebug()<<eigen0<<deviation;
    finalHaloRot.position.x()=finalHaloRotX;    finalHaloRotPerturbed.position.x()=finalHaloRotX-finalHaloRotX*eigen0*deviation;
    finalHaloRot.position.y()=finalHaloRotY;    finalHaloRotPerturbed.position.y()=finalHaloRotY-finalHaloRotY*eigen0*deviation;
    finalHaloRot.position.z()=finalHaloRotZ;    finalHaloRotPerturbed.position.z()=finalHaloRotZ-finalHaloRotZ*eigen0*deviation;
    finalHaloRot.velocity.x()=finalHaloRotVX;   finalHaloRotPerturbed.velocity.x()=finalHaloRotVX-finalHaloRotVX*eigen0*deviation;
    finalHaloRot.velocity.y()=finalHaloRotVY;   finalHaloRotPerturbed.velocity.y()=finalHaloRotVY-finalHaloRotVY*eigen0*deviation;
    finalHaloRot.velocity.z()=finalHaloRotVZ;   finalHaloRotPerturbed.velocity.z()=finalHaloRotVZ-finalHaloRotVZ*eigen0*deviation;
    //finalHaloCart=rotatingTOcartesian (transformationMode, firstBody(), secondBody(), finalHaloRot, jdMission+sta::secsToDays((timefinTransfer)/(2*Pi())*(orbitalPeriod)));
    //qDebug()<<jdMission+sta::secsToDays((timefinTransfer)/(2*Pi())*(orbitalPeriod));

    timein=0;
    initial_condition[3]=patchingPoint_rotating_x;
    initial_condition[4]=-patchingPoint_rotating_y;
    initial_condition[5]=-patchingPoint_rotating_z;
    initial_condition[0]=-patchingPoint_rotating_Vx;
    initial_condition[1]=patchingPoint_rotating_Vy;
    initial_condition[2]=patchingPoint_rotating_Vz;

    while (timefin<=timefinTransfer-step)
    {
     for (int p=0; p<6; p++)
        deriv[p]=0;
    flag=1;
    timefin=timefin+step;

    //qDebug()<<timein<<timefin;
    while (flag!=2)
    {
        //qDebug()<<jd<<timefin;
        flag=Runge_Kutta_Fehlberg(threebody_EOM, 6, initial_condition, paramv, deriv, &timein, timefin, &relerr, abserr, flag );
        if (flag==4)
            flag=Runge_Kutta_Fehlberg(threebody_EOM, 6, initial_condition, paramv, deriv, &timein, timefin, &relerr, abserr, flag );
    }
        patch_cart.position.x()=initial_condition[3];
        patch_cart.position.y()=-initial_condition[4];
        patch_cart.position.z()=-initial_condition[5];
        patch_cart.velocity.x()=-initial_condition[0];
        patch_cart.velocity.y()=initial_condition[1];
        patch_cart.velocity.z()=initial_condition[2];
        jd=sta::secsToDays(step/(2*Pi())*(orbitalPeriod))+jd;
        //qDebug()<<"jd-timeOfIntegration"<<jd<<timefin;
        stateVector=rotatingTOcartesian (transformationMode, firstBody(), secondBody(), patch_cart, jd);
        if (stateVector.position.norm()<limitDistance)
        {
            QMessageBox::warning(NULL,
                              QObject::tr("Transfer orbit not computed"),
                              QObject::tr("The trajectory impacts the central body. Maybe the starting epoch doesn't allow this kind of transfer orbit"));
            return stateVector.zero();
        }

        sampleTimes << jd;
        samples << stateVector;

    }
    double DeltaPosition_goal,DeltaVelocity_goal, DeltaPosition,DeltaVelocity;
    finalHaloCart=rotatingTOcartesian (transformationMode, firstBody(), secondBody(), finalHaloRot, jd);
    finalHaloCartPerturbed=rotatingTOcartesian (transformationMode, firstBody(), secondBody(), finalHaloRotPerturbed, jd);
    DeltaPosition=stateVector.position.norm()-finalHaloCart.position.norm();
    DeltaPosition_goal=stateVector.position.norm()-finalHaloCartPerturbed.position.norm();
    DeltaVelocity=stateVector.velocity.norm()-finalHaloCart.velocity.norm();
    DeltaVelocity_goal=stateVector.velocity.norm()-finalHaloCartPerturbed.velocity.norm();
    //qDebug()<<"final orbit position"<<stateVector.position.x()<<stateVector.position.y()<<stateVector.position.z()<<stateVector.velocity.x()<<stateVector.velocity.y()<<stateVector.velocity.z();
    //qDebug()<<"Halo orbit position"<<finalHaloCart.position.x()<<finalHaloCart.position.y()<<finalHaloCart.position.z()<<finalHaloCart.velocity.x()<<finalHaloCart.velocity.y()<<finalHaloCart.velocity.z();
    //qDebug()<<"delta R&V (halo-haloPerturbed)"<<DeltaPosition<<DeltaVelocity<<DeltaPosition_goal<<DeltaVelocity_goal<<"\n";
    qDebug()<<"\n\n"<<"Position deviation:"<<DeltaPosition<<"km";
    qDebug()<<"Velocity deviation:"<<DeltaVelocity<<"km/s";
    qDebug()<<"TOF:"<<jd<<"days";
    extern double BestTotalDeltaV;
    qDebug()<<"Total DeltaV:"<<BestTotalDeltaV<<"km/s";
    //qDebug()<<finalHaloCart.position.norm()-finalHaloCartPerturbed.position.norm()<<finalHaloCart.velocity.norm()-finalHaloCartPerturbed.velocity.norm();
//qDebug()<<timefinTransfer<<jdMission<<sta::secsToDays((timefinTransfer)/(2*Pi())*(orbitalPeriod))<<jdMission+sta::secsToDays((timefinTransfer)/(2*Pi())*(orbitalPeriod));;

    return stateVector;
}

if (Lagrmode==2)
{
    QFile file_manifolds_settings ("3BMmanifolds_settings.stam");
    if (!file_manifolds_settings.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QMessageBox::warning(NULL,
                              QObject::tr("Manifolds not computed"),
                              QObject::tr("Manifolds settings data file %1 not found.").arg("3BMmanifolds_settings.stam"));
        stateVector.zero();
        return stateVector;
    }

    QTextStream settings(&file_manifolds_settings);
    int numberPositions, numberTrajectories, numberPositions2;

    QFile file_manifold_readerRS ("3BMmanifold_rightStable.stae");
    file_manifold_readerRS.open(QIODevice::ReadOnly | QIODevice::Text);
    QTextStream inRS(&file_manifold_readerRS);

    QFile file_manifold_readerRU ("3BMmanifold_rightUnstable.stae");
    file_manifold_readerRU.open(QIODevice::ReadOnly | QIODevice::Text);
    QTextStream inRU(&file_manifold_readerRU);

    QFile file_manifold_readerLS ("3BMmanifold_leftStable.stae");
    file_manifold_readerLS.open(QIODevice::ReadOnly | QIODevice::Text);
    QTextStream inLS(&file_manifold_readerLS);

    QFile file_manifold_readerLU ("3BMmanifold_leftUnstable.stae");
    file_manifold_readerLU.open(QIODevice::ReadOnly | QIODevice::Text);
    QTextStream inLU(&file_manifold_readerLU);
    
    int memory, kind=-1;
    settings>>kind;
    while (kind!=0)
    {
        //qDebug()<<"kind"<<kind;
        if (kind==1) //information about the manifolds required
        {
            memory=kind;
            settings>>numberTrajectories>>numberPositions>>numberPositions2;  //the number of trajectories for each manifold and the number of positions for each trajectory are read
            kind=numberPositions2;
            if (numberPositions2>4)
            {
                numberTrajectories=1;
            }
            //qDebug()<<"file:"<<"3BMmanifold_rightStable.stae";
        }
        else if (kind==2)
        {
            memory=kind;
            settings>>numberTrajectories>>numberPositions>>numberPositions2;
            kind=numberPositions2;
            if (numberPositions2>4)//the number of trajectories for each manifold and the number of positions for each trajectory are read
            {
                numberTrajectories=1;
            }
            //qDebug()<<"file:"<<"3BMmanifold_rightUnstable.stae";
        }
        else if (kind==3)
        {
            memory=kind;
            settings>>numberTrajectories>>numberPositions>>numberPositions2;
            kind=numberPositions2;
            if (numberPositions2>4)//the number of trajectories for each manifold and the number of positions for each trajectory are read
            {
                numberTrajectories=1;
            }
            //qDebug()<<"file:"<<"3BMmanifold_leftStable.stae";
        }
        else if (kind==4)
        {
            memory=kind;
            settings>>numberTrajectories>>numberPositions>>numberPositions2;
            kind=numberPositions2;
            if (numberPositions2>4)//the number of trajectories for each manifold and the number of positions for each trajectory are read
            {
                numberTrajectories=1;
            }
            //qDebug()<<"file:"<<"3BMmanifold_leftUnstable.stae";
        }
        else if (kind>4)
        {
            //the intersection with a main body is the stopping condition
            settings>>numberPositions2;
            numberPositions=kind;
            kind=numberPositions2;
            if (numberPositions2>4)//the number of trajectories for each manifold and the number of positions for each trajectory are read
            {
                numberTrajectories=1;
            }
        }
        else if (kind==0)
        {
            break;
        }
        else
        {
            QMessageBox::warning(NULL,
                              QObject::tr("Manifolds not computed"),
                              QObject::tr("Wrong manifolds settings data file %1.").arg("3BMmanifolds_settings.stam"));
            return stateVector;
        }

        double jd;        
        //qDebug()<<"numberTrajectories"<<numberTrajectories<<"numberPositions"<<numberPositions;
        for(int i=1;i<(numberTrajectories)*numberPositions+1;i++)
        {
            //qDebug()<<"memory"<<memory;
            if (memory==1)
                inRS>>jd>>stateVector.position.x()>>stateVector.position.y()>>stateVector.position.z()>>stateVector.velocity.x()>>stateVector.velocity.y()>>stateVector.velocity.z();
            else if (memory==2)
                inRU>>jd>>stateVector.position.x()>>stateVector.position.y()>>stateVector.position.z()>>stateVector.velocity.x()>>stateVector.velocity.y()>>stateVector.velocity.z();
            else if (memory==3)
                inLS>>jd>>stateVector.position.x()>>stateVector.position.y()>>stateVector.position.z()>>stateVector.velocity.x()>>stateVector.velocity.y()>>stateVector.velocity.z();
            else if (memory==4)
                inLU>>jd>>stateVector.position.x()>>stateVector.position.y()>>stateVector.position.z()>>stateVector.velocity.x()>>stateVector.velocity.y()>>stateVector.velocity.z();

            //qDebug()<<i<<"\n"<<jd<<stateVector.position.x()<<stateVector.position.y()<<stateVector.position.z()<<stateVector.velocity.x()<<stateVector.velocity.y()<<stateVector.velocity.z();
            sta::StateVector newstateVector;
            newstateVector=rotatingTOcartesian(2, firstBody(), secondBody(), stateVector, jd);
           //-------------------------------------------------
           // stateVector.position.x()=(+Grav_Param+stateVector.position.x())*distance;
           // stateVector.position.y()=stateVector.position.y()*distance;
           // stateVector.position.z()=stateVector.position.z()*distance;
           // stateVector.velocity.x()=stateVector.velocity.x()*velocity;
           // stateVector.velocity.y()=stateVector.velocity.y()*velocity;
           // stateVector.velocity.z()=stateVector.velocity.z()*velocity;
           //-----------------------------------------------------------------
            samples<<newstateVector;
            sampleTimes<<jd;
            //qDebug()<<jd<<newstateVector.position.x()<<newstateVector.position.y()<<newstateVector.position.z()<<newstateVector.velocity.x()<<newstateVector.velocity.y()<<newstateVector.velocity.z();
        }   
    }
    //qDebug()<<"positions"<<sampleTimes.count();
    file_manifold_readerRS.close();
    file_manifold_readerRU.close();
    file_manifold_readerLS.close();
    file_manifold_readerLU.close();
    file_manifolds_settings.close();
}
return newstateVector;
}



const sta::CoordinateSystem
ScenarioLagrangianTrajectory::coordinateSystem() const
{
    // TODO: should this be the initial state coord sys?
    return sta::CoordinateSystem(sta::COORDSYS_EME_J2000);
}



void
ScenarioLagrangianTrajectory::setThreebodyenvironment(ScenarioThreebodyenvironment* threebodyenvironment)
{
    if (threebodyenvironment != m_threebodyenvironment)
    {
        detachChild(m_threebodyenvironment);
        m_threebodyenvironment = threebodyenvironment;
        attachChild(m_threebodyenvironment);
    }
}


void
ScenarioLagrangianTrajectory::setSimulationParameters(ScenarioSimulationParameters* simulationParameters)
{
    if (simulationParameters != m_simulationParameters)
    {
        detachChild(m_simulationParameters);
        m_simulationParameters = simulationParameters;
        attachChild(m_simulationParameters);
    }
}


void
ScenarioLagrangianTrajectory::setTrajectoryPropagation(ScenarioTrajectoryPropagation* trajectoryPropagation)
{
    if (trajectoryPropagation != m_trajectoryPropagation)
    {
        detachChild(m_trajectoryPropagation);
        m_trajectoryPropagation = trajectoryPropagation;
        attachChild(m_trajectoryPropagation);
    }
}

void
ScenarioLagrangianTrajectory::setHalo(ScenarioHalo* halo)
{
    if (halo != m_halo)
    {
        detachChild(m_halo);
        m_halo = halo;
        attachChild(m_halo);
    }
}


void
ScenarioLagrangianTrajectory::setThreebodyTransfer(ScenarioThreebodyTransfer* threebodyTransfer)
{
    if (threebodyTransfer != m_threebodyTransfer)
    {
        detachChild(m_threebodyTransfer);
        m_threebodyTransfer = threebodyTransfer;
        attachChild(m_threebodyTransfer);
    }
}


/*! Get the origin of the trajectory.
 */
const StaBody*
ScenarioLagrangianTrajectory::centralBody() const
{
    extern int luna;
    if (luna==1)
    {
        luna=0;
        return threebodyenvironment()->secondBody()->body();
    }
           // return threebodyenvironment()->secondBody()->body();
    if (coordinates_transformation==1 || coordinates_transformation==0 || coordinates_transformation==3 )
    {
        //qDebug()<<"FIRST BODY"<<threebodyenvironment()->firstBody()->body()->name();
        //qDebug()<<"central body"<<threebodyenvironment()->firstBody()->body()->name();
        return threebodyenvironment()->firstBody()->body();
    }
    else if (coordinates_transformation==2 || coordinates_transformation==4 || coordinates_transformation==5)
        {
        //qDebug()<<"SECOND BODY"<<threebodyenvironment()->secondBody()->body()->name();
        //qDebug()<<"central body"<<threebodyenvironment()->secondBody()->body()->name();
            return threebodyenvironment()->secondBody()->body();
        }
}
