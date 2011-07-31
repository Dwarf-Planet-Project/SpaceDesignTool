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
 ------ Copyright (C) 2011 STA Steering Board (space.trajectory.analysis AT gmail.com) ----

 */

/*
 ------------------ Author: J. Alonso     -------------------------------------------------
 ------------------ ETSIA                 ----------------------------
 Comments:
 1.- Manoeuvre auxiliar descriptors. Storage manoeuvres data.
 2.- Manoeuvre deltaV settings
 3.- RendezVous propagator
 4.- CWsolvers
 */


#include "rendezVousManoeuvres.h"

/**************************************************************************
    RVmanoeuvreDescriptor methods.-
***************************************************************************/

QString RVmanoeuvreDescriptor::getName(){
    return fullName;
}

void RVmanoeuvreDescriptor::setName(QString in){
    if (!fullName.isEmpty())
        fullName.clear();
    fullName.append(in);

}

int RVmanoeuvreDescriptor::getDescriptor(){
    return descriptor;
}

void RVmanoeuvreDescriptor::setDescriptor(int desc){
    descriptor=desc;
}

double RVmanoeuvreDescriptor::getDistance(){
    return distance;
}

void RVmanoeuvreDescriptor::setDistance(double dist){
    distance=dist;
}

void RVmanoeuvreDescriptor::setDurationInSecs(double in){
    durationInSecs=in;
}

double RVmanoeuvreDescriptor::getDurationInSecs(){
    return durationInSecs;
}

void RVmanoeuvreDescriptor::setRVinput(double in){
    inputRV=in;
}

double RVmanoeuvreDescriptor::getRVinput(){
    return inputRV;
}

void RVmanoeuvreDescriptor::setInitPosVel(sta::StateVector* in){

    if(in->position.x()<0.0001) in->position(0)=0.0;
    if(in->position.y()<0.0001) in->position(1)=0.0;
    if(in->position.z()<0.0001) in->position(2)=0.0;

    initPosVel.position(0)=in->position(0);
    initPosVel.position(1)=in->position(1);
    initPosVel.position(2)=in->position(2);

    initPosVel.velocity(0)=in->velocity(0);
    initPosVel.velocity(1)=in->velocity(1);
    initPosVel.velocity(2)=in->velocity(2);


}

sta::StateVector RVmanoeuvreDescriptor::getInitPosVel(){
    return initPosVel;
}

void RVmanoeuvreDescriptor::setEndPosVel(sta::StateVector* in){

    if(in->position.x()<0.0001) in->position(0)=0.0;
    if(in->position.y()<0.0001) in->position(1)=0.0;
    if(in->position.z()<0.0001) in->position(2)=0.0;

    endPosVel.position(0)=in->position(0);
    endPosVel.position(1)=in->position(1);
    endPosVel.position(2)=in->position(2);

    endPosVel.velocity(0)=in->velocity(0);
    endPosVel.velocity(1)=in->velocity(1);
    endPosVel.velocity(2)=in->velocity(2);


}

sta::StateVector RVmanoeuvreDescriptor::getEndPosVel(){
    return endPosVel;
}


double RVmanoeuvreDescriptor::getDeltaV1(){
    return deltaV1;
}

double RVmanoeuvreDescriptor::getDeltaV2(){
    return deltaV2;
}

double RVmanoeuvreDescriptor::getTotalDeltaV(){
    return totalDeltaV;
}

void RVmanoeuvreDescriptor::setDeltaV1(double in){
    deltaV1=in;
}

void RVmanoeuvreDescriptor::setDeltaV2(double in){
    deltaV2=in;
}

void RVmanoeuvreDescriptor::setTotalDeltaV(double in){
    totalDeltaV=in;
}


/******************************************************************/





/***************************************************************

  Manoeuvres.- Each of these objects are groups of RV manoeuvres
    TBTtangential, RBTradial and CFTtransfer

  **************************************************************/

/*---------------------------------------------------------------
 TBTtangential.- Includes TBT_ALONG_X_AXIS,TBT_HOHMANN_TRANSFER,
 TBT_90_DEGREES_FLY_AROUND
  -------------------------------------------------------------*/

/*TBTtangential::TBTtangential(){
}*/

void TBTtangential::setTangential(int designator,double mu,double radius,sta::StateVector vec,double desiredDistance){

    double omega;

    initialState.position(0)=vec.position(0);
    initialState.position(1)=vec.position(1);
    initialState.position(2)=vec.position(2);

    initialState.velocity(0)=vec.velocity(0);
    initialState.velocity(1)=vec.velocity(1);
    initialState.velocity(2)=vec.velocity(2);

    omega=sqrt(mu/pow(radius,3));


    if(designator==TBT_ALONG_X_AXIS){

        deltaV1=(omega*(initialState.position(0)-desiredDistance)/(6*sta::Pi()));
        deltaV1=deltaV1;
        deltaV2=-deltaV1;
        totalDeltaV=2*fabs(deltaV1);


    }else if (designator==TBT_HOHMANN_TRANSFER){

        deltaV1=(omega*(initialState.position(2)-desiredDistance)/4.0);
        deltaV1=deltaV1;
        deltaV2=deltaV1;
        totalDeltaV=fabs(2*deltaV1);

    }else if (designator==TBT_90_DEGREES_FLY_AROUND){

        deltaV1=-(omega*(initialState.position(0)-desiredDistance)/4.0);
        deltaV2=-7*(omega*(initialState.position(2)-desiredDistance)/4.0);
        totalDeltaV=fabs(deltaV1)+fabs(deltaV2);

    }



}


//TBTtangential::~TBTtangential(){}




double TBTtangential::getDeltaV1(){
    return deltaV1;
}

double TBTtangential::getDeltaV2(){
    return deltaV2;
}

double TBTtangential::getTotalDeltaV(){
    return totalDeltaV;
}


double TBTtangential::getDurationInSecs(){
    return myDurationInSecs;

}

sta::StateVector TBTtangential::getFinalState(){
    return finalState;
}




/*---------------------------------------------------------------
 TBTradial.- Includes RBT_ALONG_X_AXIS,RBT_90_DEGREES_FLY_AROUND,
 RBT_360_DEGREES_FLY_AROUND
  -------------------------------------------------------------*/

/*TBTradial::TBTradial(){
}*/

void TBTradial::setRadial(int designator,double mu,double radius,sta::StateVector vec,double desiredDistance){

    double omega;

    initialState.position(0)=vec.position(0);
    initialState.position(1)=vec.position(1);
    initialState.position(2)=vec.position(2);

    initialState.velocity(0)=vec.velocity(0);
    initialState.velocity(1)=vec.velocity(1);
    initialState.velocity(2)=vec.velocity(2);

    omega=sqrt(mu/pow(radius,3));

    //Duration of manoeuvre is a period


    if(designator==RBT_ALONG_X_AXIS){

        deltaV1=-(omega*(initialState.position(0)-desiredDistance)/4.0);
        deltaV1=deltaV1;
        deltaV2=deltaV1;
        totalDeltaV=2*fabs(deltaV1);



    }else if (designator==RBT_90_DEGREES_FLY_AROUND){

        deltaV1=-(omega*(initialState.position(2)-desiredDistance));
        deltaV1=deltaV1;
        deltaV2=2*deltaV1;
        totalDeltaV=fabs(3*deltaV1);


    }else if (designator==RBT_360_DEGREES_FLY_AROUND){

        deltaV1=-(omega*desiredDistance);
        deltaV1=deltaV1;
        deltaV2=-deltaV1;
        totalDeltaV=fabs(2*deltaV1);

    }



}


//TBTtangential::~TBTradial(){}




double TBTradial::getDeltaV1(){
    return deltaV1;
}

double TBTradial::getDeltaV2(){
    return deltaV2;
}

double TBTradial::getTotalDeltaV(){
    return totalDeltaV;
}


double TBTradial::getDurationInSecs(){
    return myDurationInSecs;

}

sta::StateVector TBTradial::getFinalState(){
    return finalState;
}

/*---------------------------------------------------------------
 CFTtransfer.- Includes HOLD_POINTS_VBAR,SLT_VBAR,SLT_RBAR,

    CFT_TANGENTIAL, CFT_RADIAL
  -------------------------------------------------------------*/


void CFTtransfer::setTransfer(int designator,double mu,double radius,sta::StateVector vec,double desiredDistance,double duration){

    double omega;

    initialState.position(0)=vec.position(0);
    initialState.position(1)=vec.position(1);
    initialState.position(2)=vec.position(2);

    initialState.velocity(0)=vec.velocity(0);
    initialState.velocity(1)=vec.velocity(1);
    initialState.velocity(2)=vec.velocity(2);

    omega=sqrt(mu/pow(radius,3));
    //a quarter of period


    if(designator==CFT_TANGENTIAL){
        constantForce=-omega*omega*(desiredDistance- initialState.position(2))/(4*sta::Pi());
        totalDeltaV=omega*(desiredDistance-initialState.position(2))/2.0;


    }else if (designator==CFT_RADIAL){
        constantForce=omega*omega*(desiredDistance-initialState.position(0))/(4*sta::Pi());
        totalDeltaV=omega*(desiredDistance-initialState.position(0))/2.0;


    }
    else if (designator==SLT_VBAR){
        deltaV1=((desiredDistance-initialState.position(0))/duration);
        deltaV2=-deltaV1;
        //Duration and final position are data.
        //DeltaV and accel comes from initial conditions.
        constantForce=2*omega*(desiredDistance-initialState.position(0))/duration;
        totalDeltaV=fabs(constantForce*duration)+2*fabs(deltaV1);


        }
    else if (designator==SLT_RBAR){
        int step=10;
        deltaV1=((desiredDistance-initialState.position(2))/duration);
        deltaV2=-deltaV1;
        //DeltaV and accel comes from initial conditions.

        constantForce=-2*omega*(desiredDistance-initialState.position(2))/duration;

        //CAUTION.- Needed step instead of ten

        for (int i=0;i<=int(duration/step)+1;i++){
            Vector3d *sVec=new Vector3d();
            //Cambiar x por z
            sVec->x()=constantForce;
            sVec->y()=0.0;
            sVec->z()=3*omega*omega*(initialState.position(2)+deltaV1*i*step);
            accelList.append(*sVec);
         }

        //CAUTION.- Check math here.
        totalDeltaV=fabs(constantForce*duration)+
                   2*fabs(deltaV1)+fabs(3*omega*omega*(initialState.position(2))*duration/2)+
                   fabs(3*omega*omega*initialState.velocity(2)*duration*duration/2);


        }
    else if (designator==HOLD_POINTS_VBAR){
            //Desired distance on z
            constantForce=3*omega*omega*desiredDistance;
            totalDeltaV=(constantForce*duration);

        }





}


//TBTtangential::~TBTtangential(){}




double CFTtransfer::getDeltaV1(){
    return deltaV1;
}

double CFTtransfer::getDeltaV2(){
    return deltaV2;
}

double CFTtransfer::getTotalDeltaV(){
    return totalDeltaV;
}


double CFTtransfer::getDurationInSecs(){
    return myDurationInSecs;

}

double CFTtransfer::getConstantForce(){
    return constantForce;

}



sta::StateVector CFTtransfer::getFinalState(){
    return finalState;
}

QList <Vector3d>& CFTtransfer::getAccelList(){
    return accelList;

}

//As similar possible as loitering propagation


/////////////////////////////////////// PropagateRendezVousTrajectory /////////////////////////////

bool PropagateRendezVousTrajectory(ScenarioRendezVousManoeuvreType* rvManoeuvre,
                                     QList<double>& sampleTimes,
                                     QList<sta::StateVector>& samples,
                                     PropagationFeedback& propFeedback,
                                     PropagatedScenario* propScen)
{
    //Preliminar checkings

    QString centralBodyName = rvManoeuvre->Environment()->CentralBody()->Name();
    StaBody* centralBody = STA_SOLAR_SYSTEM->lookup(centralBodyName);

    if (!centralBody)
    {
        propFeedback.raiseError(QObject::tr("Unrecognized central body '%1'").arg(centralBodyName));
        return false;
    }


    QString coordSysName = rvManoeuvre->InitialPosition()->CoordinateSystem();
    sta::CoordinateSystem coordSys(coordSysName);
    if (coordSys.type() == sta::COORDSYS_INVALID)
    {
        propFeedback.raiseError(QObject::tr("Unrecognized coordinate system '%1'").arg(coordSysName));
        return false;
    }

    // Get the initial state in two forms:
    //   - Keplerian elements for simple two-body propagation
    //   - A state vector for any other sort of propagation


    double startTime = sta::JdToMjd(sta::CalendarToJd(rvManoeuvre->TimeLine()->StartTime()));
    ScenarioAbstract6DOFPositionType* position = rvManoeuvre->InitialPosition()->Abstract6DOFPosition().data();
    double endTime = sta::JdToMjd(sta::CalendarToJd(rvManoeuvre->TimeLine()->EndTime()));


    sta::StateVector chaserInitial = AbstractPositionToStateVector(position, centralBody);
    sta::StateVector targetInitial;
    sta::StateVector targetFinal;

    propScen->spaceObjects().first()->mission().first()->getStateVector(startTime,&targetInitial);

    propScen->spaceObjects().first()->mission().first()->getStateVector(endTime,&targetFinal);


    sta::KeplerianElements initialStateKeplerian = AbstractPositionToKeplerianElements(position, centralBody);


    double mu = centralBody->mu();
    // Get the timeline information


    double timelineDuration = rvManoeuvre->TimeLine()->StartTime().secsTo(rvManoeuvre->TimeLine()->EndTime());
    double dt = rvManoeuvre->PropagationPosition()->timeStep();

    if (dt == 0.0)
    {
        propFeedback.raiseError(QObject::tr("Time step is zero!"));
        return false;
    }

    // We don't output values at every integration step. Instead use the time step
    // from simulation parameters. The actual output step used will not necessarily
    // match the requested output step: the code below sets it to be an integer
    // multiple of the integration step.

    double requestedOutputTimeStep = rvManoeuvre->TimeLine()->StepTime();
    double outputTimeStep;
    unsigned int outputRate;
    if (requestedOutputTimeStep < dt)
    {
        outputRate = 1;
        outputTimeStep = dt;
    }
    else
    {
        outputRate = (unsigned int) floor(requestedOutputTimeStep / dt + 0.5);
        outputTimeStep = outputRate * dt;
    }

    if (timelineDuration / outputTimeStep > MAX_OUTPUT_STEPS)
    {
        propFeedback.raiseError(QObject::tr("Number of propagation steps exceeds %1. Try increasing the simulation time step.").arg(MAX_OUTPUT_STEPS));
        return false;
    }

        //QList<Perturbations*> perturbationsList; // Create the list of perturbations that will influence the propagation
    //ScenarioStateVectorType* initialState=new ScenarioStateVectorType();

    // deviation, reference, and q will be used only in Encke propagation
    //sta::StateVector deviation(Eigen::Vector3d::Zero(), Eigen::Vector3d::Zero());
    //sta::StateVector reference = initialState;
    double q = 0.0;


    //sampleTimes << startTime;
    //samples << stateVector;

    //QFile ciccio("data/PerturbationsData.stae");
    //QTextStream cicciostream(&ciccio);
    //ciccio.open(QIODevice::WriteOnly);

    //unsigned int steps = 0;
    unsigned int steps = 1; // patched by Ana on 18th June 2010

    QString propagator = rvManoeuvre->PropagationPosition()->propagator();
    QString integrator = rvManoeuvre->PropagationPosition()->integrator();



    double sma            = initialStateKeplerian.SemimajorAxis;

    double omega=sqrt(mu/pow(sma,3));



        double e              = initialStateKeplerian.Eccentricity;
        double inclination    = initialStateKeplerian.Inclination*Pi()/180.0;
        double raan           = initialStateKeplerian.AscendingNode*Pi()/180.0;
        double argOfPeriapsis = initialStateKeplerian.ArgumentOfPeriapsis*Pi()/180.0;
        double trueAnomaly    = initialStateKeplerian.TrueAnomaly*Pi()/180.0;
        double meanAnomaly    = trueAnomalyTOmeanAnomaly(trueAnomaly, e);


        double perigee = sma * (1 - e);


    //This depends on the type of manoeuvre

    //State and accel is the vector needed by the propagator. By default
    //all accels should be zero.
    double stateANDaccel[9];
    stateANDaccel[6]=0.0;
    stateANDaccel[7]=0.0;
    stateANDaccel[8]=0.0;

       sta::StateVector* initialStateDefined=new sta::StateVector();

       chaserInitial.position(0)=chaserInitial.position(0)-targetInitial.position(0);
       chaserInitial.position(1)=chaserInitial.position(1)-targetInitial.position(1);
       chaserInitial.position(2)=chaserInitial.position(2)-targetInitial.position(2);

       chaserInitial.velocity(0)=chaserInitial.velocity(0)-targetInitial.velocity(0);
       chaserInitial.velocity(1)=chaserInitial.velocity(1)-targetInitial.velocity(1);
       chaserInitial.velocity(2)=chaserInitial.velocity(2)-targetInitial.velocity(2);

       inertialTOlhlv(mu,targetInitial,chaserInitial,initialStateDefined);

       //To meters to calculate deltaV
       if(initialStateDefined->position(0)<0.0001)
           initialStateDefined->position(0)=0.0;
       if(initialStateDefined->position(1)<0.0001)
           initialStateDefined->position(1)=0.0;
       if(initialStateDefined->position(2)<0.0001)
           initialStateDefined->position(2)=0.0;

       initialStateDefined->position(0)=initialStateDefined->position(0)*1000.0;
       initialStateDefined->position(1)=initialStateDefined->position(1)*1000.0;
       initialStateDefined->position(2)=initialStateDefined->position(2)*1000.0;
       initialStateDefined->velocity(0)=0.0;
       initialStateDefined->velocity(1)=0.0;
       initialStateDefined->velocity(2)=0.0;




       QList<Vector3d> accelListPropagator;

       //Tangential

       if(rvManoeuvre->ElementIdentifier()->Name().compare("2TBTalong x-axis")==0){
           TBTtangential* tangential=new TBTtangential();
           tangential->setTangential(TBT_ALONG_X_AXIS,mu,sma,*initialStateDefined,rvManoeuvre->Input());
           rvManoeuvre->setDeltaV1(tangential->getDeltaV1());
           rvManoeuvre->setDeltaV2(tangential->getDeltaV2());
           rvManoeuvre->setTotalDeltaV(tangential->getTotalDeltaV());
           initialStateDefined->velocity(0)=(rvManoeuvre->DeltaV1()+initialStateDefined->velocity(0));

       }


       if(rvManoeuvre->ElementIdentifier()->Name().compare("2TBTHohmann Transfer")==0){
           TBTtangential* tangential=new TBTtangential();
           tangential->setTangential(TBT_HOHMANN_TRANSFER,mu,sma,*initialStateDefined,rvManoeuvre->Input());
           rvManoeuvre->setDeltaV1(tangential->getDeltaV1());
           rvManoeuvre->setDeltaV2(tangential->getDeltaV2());
           rvManoeuvre->setTotalDeltaV(tangential->getTotalDeltaV());

           initialStateDefined->velocity(0)=(rvManoeuvre->DeltaV1()+initialStateDefined->velocity(0));

       }

       if(rvManoeuvre->ElementIdentifier()->Name().compare("2TBT90 degrees fly-around")==0){
           TBTtangential* tangential=new TBTtangential();
           tangential->setTangential(TBT_90_DEGREES_FLY_AROUND,mu,sma,*initialStateDefined,rvManoeuvre->Input());
           rvManoeuvre->setDeltaV1(tangential->getDeltaV1());
           rvManoeuvre->setDeltaV2(tangential->getDeltaV2());
           rvManoeuvre->setTotalDeltaV(tangential->getTotalDeltaV());

           initialStateDefined->velocity(0)=(rvManoeuvre->DeltaV1()+initialStateDefined->velocity(0));

       }
       //Radial

       if(rvManoeuvre->ElementIdentifier()->Name().compare("2RBTalong x-axis")==0){
           TBTradial* radial=new TBTradial();
           radial->setRadial(RBT_ALONG_X_AXIS,mu,sma,*initialStateDefined,rvManoeuvre->Input());
           rvManoeuvre->setDeltaV1(radial->getDeltaV1());
           rvManoeuvre->setDeltaV2(radial->getDeltaV2());
           rvManoeuvre->setTotalDeltaV(radial->getTotalDeltaV());

           initialStateDefined->velocity(2)=(rvManoeuvre->DeltaV1()+initialStateDefined->velocity(2));

       }

       if(rvManoeuvre->ElementIdentifier()->Name().compare("2RBT90 degrees radial fly-around")==0){
           TBTradial* radial=new TBTradial();
           radial->setRadial(RBT_90_DEGREES_FLY_AROUND,mu,sma,*initialStateDefined,rvManoeuvre->Input());
           rvManoeuvre->setDeltaV1(radial->getDeltaV1());
           rvManoeuvre->setDeltaV2(radial->getDeltaV2());
           rvManoeuvre->setTotalDeltaV(radial->getTotalDeltaV());

           initialStateDefined->velocity(2)=(rvManoeuvre->DeltaV1()+initialStateDefined->velocity(2));

       }

       if(rvManoeuvre->ElementIdentifier()->Name().compare("2RBT360 degrees radial fly-around")==0){
           TBTradial* radial=new TBTradial();
           radial->setRadial(RBT_360_DEGREES_FLY_AROUND,mu,sma,*initialStateDefined,rvManoeuvre->Input());
           rvManoeuvre->setDeltaV1(radial->getDeltaV1());
           rvManoeuvre->setDeltaV2(radial->getDeltaV2());
           rvManoeuvre->setTotalDeltaV(radial->getTotalDeltaV());

           initialStateDefined->velocity(2)=(rvManoeuvre->DeltaV1()+initialStateDefined->velocity(2));

       }

       //Constant force

       if(rvManoeuvre->ElementIdentifier()->Name().compare("Hold Points outside V-bar")==0){
           CFTtransfer* transfer=new CFTtransfer();
           transfer->setTransfer(HOLD_POINTS_VBAR,mu,sma,*initialStateDefined,rvManoeuvre->Input(),rvManoeuvre->Duration());
           stateANDaccel[8]=(transfer->getConstantForce());
           rvManoeuvre->setTotalDeltaV(transfer->getTotalDeltaV());


       }

       if(rvManoeuvre->ElementIdentifier()->Name().compare("CFT Tangential")==0){
           CFTtransfer* transfer=new CFTtransfer();
           transfer->setTransfer(CFT_TANGENTIAL,mu,sma,*initialStateDefined,rvManoeuvre->Input(),rvManoeuvre->Duration());

           stateANDaccel[6]=(transfer->getConstantForce());
           rvManoeuvre->setTotalDeltaV(transfer->getTotalDeltaV());


       }

       if(rvManoeuvre->ElementIdentifier()->Name().compare("CFT Radial")==0){
           CFTtransfer* transfer=new CFTtransfer();
           transfer->setTransfer(CFT_RADIAL,mu,sma,*initialStateDefined,rvManoeuvre->Input(),rvManoeuvre->Duration());
           stateANDaccel[8]=(transfer->getConstantForce());
           rvManoeuvre->setTotalDeltaV(transfer->getTotalDeltaV());


       }

       if(rvManoeuvre->ElementIdentifier()->Name().compare("SLT V-bar")==0){
           CFTtransfer* transfer=new CFTtransfer();
           transfer->setTransfer(SLT_VBAR,mu,sma,*initialStateDefined,rvManoeuvre->Input(),rvManoeuvre->Duration());
           rvManoeuvre->setDeltaV1(transfer->getDeltaV1());
           rvManoeuvre->setDeltaV2(transfer->getDeltaV2());
           rvManoeuvre->setTotalDeltaV(transfer->getTotalDeltaV());

           initialStateDefined->velocity(0)=(initialStateDefined->velocity(0)+rvManoeuvre->DeltaV1());

           stateANDaccel[8]=(transfer->getConstantForce());

       }

       if(rvManoeuvre->ElementIdentifier()->Name().compare("SLT R-bar")==0){
           //R-Bar has a variable acceleration applied.
           CFTtransfer* transfer=new CFTtransfer();
           transfer->setTransfer(SLT_RBAR,mu,sma,*initialStateDefined,rvManoeuvre->Input(),rvManoeuvre->Duration());
           rvManoeuvre->setDeltaV1(transfer->getDeltaV1());
           rvManoeuvre->setDeltaV2(transfer->getDeltaV2());
           rvManoeuvre->setTotalDeltaV(transfer->getTotalDeltaV());

           initialStateDefined->velocity(2)=(initialStateDefined->velocity(2)+rvManoeuvre->DeltaV1());
           accelListPropagator.append(transfer->getAccelList());
           //Constant x-acceleration already treated in CWpertb method

       }


    //Preparing data for propagator

    stateANDaccel[0]=initialStateDefined->position(0);
    stateANDaccel[1]=initialStateDefined->position(1);
    stateANDaccel[2]=initialStateDefined->position(2);
    stateANDaccel[3]=initialStateDefined->velocity(0);
    stateANDaccel[4]=initialStateDefined->velocity(1);
    stateANDaccel[5]=initialStateDefined->velocity(2);


        if (perigee < centralBody->meanRadius())
        {
            propFeedback.raiseError(QObject::tr("The perigee distance is smaller than the main body radius."));
            qWarning("Perigee %.20e",perigee);
            return false;
        }

        else if(rvManoeuvre->ElementIdentifier()->Name().compare("SLT R-bar")==0){
            //Special case for vertical trajectory
            CWperturbedPropagator* perturbedPropagator=new CWperturbedPropagator(omega, stateANDaccel,accelListPropagator, timelineDuration, dt);
            perturbedPropagator->solverCW(startTime);
            samples.append(perturbedPropagator->getResults());
            samples.last().velocity(2)=(samples.last().velocity(2)+rvManoeuvre->DeltaV2()/1000.0);
            sampleTimes.append(perturbedPropagator->getSampleTimes());
        }

        else
        {
            CWpropagator* unperturbedPropagator=new CWpropagator(omega, stateANDaccel, timelineDuration, dt);
            unperturbedPropagator->solverCW(startTime);
            samples.append(unperturbedPropagator->getResults());
            //DELTAV2 and endings adjustments
            if(rvManoeuvre->ElementIdentifier()->Name().compare("2TBTalong x-axis")==0 ||
               rvManoeuvre->ElementIdentifier()->Name().compare("2TBTHohmann Transfer")==0 ||
               rvManoeuvre->ElementIdentifier()->Name().compare("2TBT90 degrees fly-around")==0 ||
               rvManoeuvre->ElementIdentifier()->Name().compare("2RBT90 degrees radial fly-around")==0 ||
               rvManoeuvre->ElementIdentifier()->Name().compare("SLT V-bar"))
                samples.last().velocity(0)=(samples.last().velocity(0)+rvManoeuvre->DeltaV2()/1000.0);


            if(rvManoeuvre->ElementIdentifier()->Name().compare("2RBTalong x-axis")==0 ||
               rvManoeuvre->ElementIdentifier()->Name().compare("2RBT360 degrees radial fly-around")==0 ||
               rvManoeuvre->ElementIdentifier()->Name().compare("SLT R-bar"))
                samples.last().velocity(2)=(samples.last().velocity(2)+rvManoeuvre->DeltaV2()/1000.0);

            sampleTimes.append(unperturbedPropagator->getSampleTimes());


        }


    return true;
} // ------------------------------- End of the propagation method -----------------------





/****************************************************

  CWPropagators. Two different kinds
  Propagator .- No constants accelerations added
  PerturbedPropagator.- vector accelerations needed for SLT-rBAR

  **************************************************/

// CWpropagator

CWpropagator::CWpropagator(double omega, double vector[9],double enTime,double size)
{
    int i;
    targetOmega=omega;
    for (i=0;i<9;i++)
        initialStateVector[i]=vector[i];

    endTime=enTime;
    stepSize=size;

}

CWpropagator::~CWpropagator(){}

void CWpropagator::solverCW(double startTime){
    int t,i;
    sta::StateVector sVector;
    double x0=initialStateVector[0]/1000.0;
    double y0=initialStateVector[1]/1000.0;
    double z0=initialStateVector[2]/1000.0;
    double vx0=initialStateVector[3];
    double vy0=initialStateVector[4];
    double vz0=initialStateVector[5];
    double ax=initialStateVector[6];
    double ay=initialStateVector[7];
    double az=initialStateVector[8];
    double sinOmegaT,cosOmegaT,targetOmegaT;

    double X1,X2,X3,X4,X5,X6;
    double Y1,Y2;
    double Z1,Z2,Z3,Z4,Z5;


    //All steps using the same parameters.

    X1=4*vx0/targetOmega-6*z0;
    X2=2*vz0/targetOmega;
    X3=6*targetOmega*z0-3*vx0;
    X4=x0+2*vz0/targetOmega;


    Y1=vy0/targetOmega;

    Z1=2*vx0/targetOmega-3*z0;
    Z2=vz0/targetOmega;
    Z3=4*z0-2*vx0/targetOmega;

    if((ax==0.0) && (ay==0.0) && (az==0.0)){
        X5=0.0;
        X6=0.0;
        Y2=0.0;
        Z4=0.0;
        Z5=0.0;

    }else{

        X5=2*az/(targetOmega*targetOmega);
        X6=ax;
        Y2=ay/(targetOmega*targetOmega);
        Z4=2*ax/(targetOmega*targetOmega);
        Z5=az/(targetOmega*targetOmega);

    }


    //Initial state to results

        sVector.position(0)=x0;
        sVector.position(1)=y0;
        sVector.position(2)=z0;
        sVector.velocity(0)=(initialStateVector[3]);
        sVector.velocity(1)=(initialStateVector[4]);
        sVector.velocity(2)=(initialStateVector[5]);


    if(!results.isEmpty())
        results.clear();
    results.append(sVector);

    JulianDate jd=startTime;

    sampleTimes.append(jd);
    i=0;

    for (t=stepSize;t<=endTime;t=t+stepSize){
    //In order to solve computation time, trigonometric calculations, must be done just once
        i=i+1;
        //Previous calculations

        targetOmegaT=targetOmega*t;
        sinOmegaT=sin(targetOmegaT);
        cosOmegaT=cos(targetOmegaT);

        //Position


        sVector.position(0)=(X1*sinOmegaT-X2*cosOmegaT+X3*t+X4+
                     X5*(targetOmegaT-sinOmegaT)+
                     X6*(4*(1-cosOmegaT)/(targetOmega*targetOmega))-
                     X6*(3.0*t*t/2.0));
        //km



        sVector.position(0)=((sVector.position(0)/1000.0)+x0);

        sVector.position(1)=(y0*cosOmegaT+Y1*sinOmegaT+
                               Y2*(1-cosOmegaT));
        sVector.position(1)=((sVector.position(1)/1000.0)+y0);


        sVector.position(2)=(Z1*cosOmegaT+Z2*sinOmegaT+Z3+
                               Z4*(sinOmegaT-targetOmegaT)+
                               Z5*(1-cosOmegaT));
        sVector.position(2)=((sVector.position(2)/1000.0)+z0);


        //Speeds

        /*sVector.velocity(0)=(((sVector.position(0)-results[i-1].position(0))/stepSize)/1000.0);
        sVector.velocity(1)=(((sVector.position(1)-results[i-1].position(1))/stepSize)/1000.0);
        sVector.velocity(2)=(((sVector.position(2)-results[i-1].position(2))/stepSize)/1000.0);*/

        sVector.velocity(0)=0.0;
        sVector.velocity(1)=0.0;
        sVector.velocity(2)=0.0;

        results.append(sVector);
        jd = startTime + ((t/3600.0)/24.0);

        sampleTimes.append(jd);



    }






}

QList <sta::StateVector>& CWpropagator::getResults(){
    return results;

}

QList<double>& CWpropagator::getSampleTimes(){
    return sampleTimes;
}



// CWperturbedPropagator

CWperturbedPropagator::CWperturbedPropagator(double omega, double vector[9],QList<Vector3d> accel,double enTime,double size)
{
    int i;
    targetOmega=omega;
    for (i=0;i<9;i++)
        initialStateVector[i]=vector[i];

    accelList.append(accel);
    endTime=enTime;
    stepSize=size;

}

CWperturbedPropagator::~CWperturbedPropagator(){}

void CWperturbedPropagator::solverCW(double startTime){
    int t,i;
    sta::StateVector sVector;
    double x0=initialStateVector[0]/1000.0;
    double y0=initialStateVector[1]/1000.0;
    double z0=initialStateVector[2]/1000.0;
    double vx0=initialStateVector[3];
    double vy0=initialStateVector[4];
    double vz0=initialStateVector[5];
    double ax=accelList.at(0).x();
    double ay=accelList.at(0).y();
    double az=accelList.at(0).z();
    double sinOmegaT,cosOmegaT,targetOmegaT;

    double X1,X2,X3,X4,X5,X6;
    double Y1,Y2;
    double Z1,Z2,Z3,Z4,Z5;


    //All steps using the same parameters.

    X1=4*vx0/targetOmega-6*z0;
    X2=2*vz0/targetOmega;
    X3=6*targetOmega*z0-3*vx0;
    X4=x0+2*vz0/targetOmega;

    Y1=vy0/targetOmega;

    Z1=2*vx0/targetOmega-3*z0;
    Z2=vz0/targetOmega;
    Z3=4*z0-2*vx0/targetOmega;

    if((ax==0.0) && (ay==0.0) && (az==0.0)){
        X5=0.0;
        X6=0.0;
        Y2=0.0;
        Z4=0.0;
        Z5=0.0;

    }

    //Initial state to results

        sVector.position(0)=x0;
        sVector.position(1)=y0;
        sVector.position(2)=z0;
        sVector.velocity(0)=(initialStateVector[3]);
        sVector.velocity(1)=(initialStateVector[4]);
        sVector.velocity(2)=(initialStateVector[5]);


    if(!results.isEmpty())
        results.clear();
    results.append(sVector);

    JulianDate jd=startTime;

    sampleTimes.append(jd);
    i=0;

    for (t=stepSize;t<=endTime;t=t+stepSize){
    //In order to solve computation time, trigonometric calculations, must be done just once
        i=i+1;
        //Previous calculations

        targetOmegaT=targetOmega*t;
        sinOmegaT=sin(targetOmegaT);
        cosOmegaT=cos(targetOmegaT);

        //accelerations.
        ax=accelList[i-1].x();
        ay=accelList[i-1].y();
        az=accelList[i-1].z();

        X5=2*az/(targetOmega*targetOmega);
        X6=ax;
        Y2=ay/(targetOmega*targetOmega);
        Z4=2*ax/(targetOmega*targetOmega);
        Z5=az/(targetOmega*targetOmega);



        //Position


        sVector.position(0)=(X1*sinOmegaT-X2*cosOmegaT+X3*t+X4+
                     X5*(targetOmegaT-sinOmegaT)+
                     X6*(4*(1-cosOmegaT)/(targetOmega*targetOmega))-
                     X6*(3.0*t*t/2.0));
        //km



        sVector.position(0)=((sVector.position(0)/1000.0)+x0);

        sVector.position(1)=(y0*cosOmegaT+Y1*sinOmegaT+
                               Y2*(1-cosOmegaT));
        sVector.position(1)=((sVector.position(1)/1000.0)+y0);


        sVector.position(2)=(Z1*cosOmegaT+Z2*sinOmegaT+Z3+
                               Z4*(sinOmegaT-targetOmegaT)+
                               Z5*(1-cosOmegaT));
        sVector.position(2)=((sVector.position(2)/1000.0)+z0);


        //Speeds

       /* sVector.velocity(0)=(((sVector.position(0)-results[i-1].position(0))/stepSize)/1000.0);
        sVector.velocity(1)=(((sVector.position(1)-results[i-1].position(1))/stepSize)/1000.0);
        sVector.velocity(2)=(((sVector.position(2)-results[i-1].position(2))/stepSize)/1000.0);*/


        sVector.velocity(0)=0.0;
        sVector.velocity(1)=0.0;
        sVector.velocity(2)=0.0;


        results.append(sVector);
        jd = startTime + ((t/3600.0)/24.0);

        sampleTimes.append(jd);



    }






}

QList <sta::StateVector>& CWperturbedPropagator::getResults(){
    return results;

}

QList<double>& CWperturbedPropagator::getSampleTimes(){
    return sampleTimes;
}



