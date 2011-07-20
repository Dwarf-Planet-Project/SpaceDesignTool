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
 Manoeuvre auxiliar descriptors. Storage manoeuvres data.

 */


#include "rendezVousManoeuvres.h"

/**************************************************************************

    RVManoeuvreDescriptor methods.-

***************************************************************************/


void RVManoeuvreDescriptor::setAssociatedManoeuvre(double mu,double radius,ScenarioStateVectorType vec,double input){

    if(descriptor==TBT_ALONG_X_AXIS){
        tangential=new TBTtangential();
        tangential->setTangential(TBT_ALONG_X_AXIS,mu,radius,vec,input);
        durationInSecs=tangential->getDurationInSecs();
    }

    if(descriptor==TBT_HOHMANN_TRANSFER){
        tangential=new TBTtangential();
        tangential->setTangential(TBT_HOHMANN_TRANSFER,mu,radius,vec,input);
        durationInSecs=tangential->getDurationInSecs();
    }

    if(descriptor==TBT_90_DEGREES_FLY_AROUND){
        tangential=new TBTtangential();
        tangential->setTangential(TBT_90_DEGREES_FLY_AROUND,mu,radius,vec,input);
        durationInSecs=tangential->getDurationInSecs();
    }

    //Radial

    if(descriptor==RBT_ALONG_X_AXIS){
        radial=new TBTradial();
        radial->setRadial(RBT_ALONG_X_AXIS,mu,radius,vec,input);
        durationInSecs=radial->getDurationInSecs();
    }

    if(descriptor==RBT_90_DEGREES_FLY_AROUND){
        radial=new TBTradial();
        radial->setRadial(RBT_90_DEGREES_FLY_AROUND,mu,radius,vec,input);
        durationInSecs=radial->getDurationInSecs();
    }

    if(descriptor==RBT_360_DEGREES_FLY_AROUND){
        radial=new TBTradial();
        radial->setRadial(RBT_360_DEGREES_FLY_AROUND,mu,radius,vec,input);
        durationInSecs=radial->getDurationInSecs();
    }

    //Constant Force

    if(descriptor==CFT_TANGENTIAL){
        cftTrans=new CFTtransfer();
        cftTrans->setTransfer(CFT_TANGENTIAL,mu,radius,vec,input);
        durationInSecs=cftTrans->getDurationInSecs();
    }

    if(descriptor==CFT_RADIAL){
        cftTrans=new CFTtransfer();
        cftTrans->setTransfer(CFT_RADIAL,mu,radius,vec,input);
        durationInSecs=cftTrans->getDurationInSecs();
    }






}

QString RVManoeuvreDescriptor::getName(){
    return fullName;
}

int RVManoeuvreDescriptor::getDescriptor(){
    return descriptor;
}

void RVManoeuvreDescriptor::setDescriptor(int desc){
    descriptor=desc;
}

double RVManoeuvreDescriptor::getDistance(){
    return distance;
}

void RVManoeuvreDescriptor::setDistance(double dist){
    distance=dist;
}

double RVManoeuvreDescriptor::getDurationInSecs(){
    return durationInSecs;
}

void RVManoeuvreDescriptor::setRVinput(double in){
    inputRV=in;
}

double RVManoeuvreDescriptor::getRVinput(){
    return inputRV;
}

void RVManoeuvreDescriptor::setInitPosVel(ScenarioStateVectorType* in){
    initPosVel.setX(in->x());
    initPosVel.setY(in->y());
    initPosVel.setZ(in->z());

    initPosVel.setVx(in->vx());
    initPosVel.setVy(in->vy());
    initPosVel.setVz(in->vz());

}

ScenarioStateVectorType RVManoeuvreDescriptor::getInitPosVel(){
    return initPosVel;
}

void RVManoeuvreDescriptor::setEndPosVel(ScenarioStateVectorType* in){

    endPosVel.setX(in->x());
    endPosVel.setY(in->y());
    endPosVel.setZ(in->z());

    endPosVel.setVx(in->vx());
    endPosVel.setVy(in->vy());
    endPosVel.setVz(in->vz());

}

ScenarioStateVectorType RVManoeuvreDescriptor::getEndPosVel(){
    return endPosVel;
}


/******************************************************************/





/***************************************************************
  Manoeuvres.- Each of these objects are groups of RV manoeuvres

  **************************************************************/

/*---------------------------------------------------------------
 TBTtangential.- Includes TBT_ALONG_X_AXIS,TBT_HOHMANN_TRANSFER,
 TBT_90_DEGREES_FLY_AROUND
  -------------------------------------------------------------*/

/*TBTtangential::TBTtangential(){
}*/

void TBTtangential::setTangential(int designator,double mu,double radius,ScenarioStateVectorType vec,double desiredDistance){

    double omega;
    double period;

    initialState.setX(vec.x());
    initialState.setY(vec.y());
    initialState.setZ(vec.z());
    initialState.setVx(vec.vx());
    initialState.setVy(vec.vy());
    initialState.setVz(vec.vz());

    omega=sqrt(mu/pow(radius,3));

    //Duration of manoeuvre is a period

    period=2*sta::Pi()*sqrt(pow(radius,3)/mu);


    if(designator==TBT_ALONG_X_AXIS){

        //USE CONSTANTS!!!
        deltaV1=(omega*(initialState.x()-desiredDistance)/(6*sta::Pi()));

        deltaV1=deltaV1*1000.0;

        deltaV2=-deltaV1;
        totalDeltaV=2*fabs(deltaV1);

        myDurationInSecs=int(period);


    }else if (designator==TBT_HOHMANN_TRANSFER){
        //In relative Z. z input positive, means we want target down

        deltaV1=(omega*(initialState.z()-desiredDistance)/4.0);

        deltaV1=deltaV1*1000.0;

        deltaV2=deltaV1;
        qWarning("deltaV1 %f \n",deltaV1);

        totalDeltaV=fabs(2*deltaV1);
        //Supossed duration T/2.
        myDurationInSecs=period/2.0;


    }else if (designator==TBT_90_DEGREES_FLY_AROUND){


        deltaV1=-(omega*(initialState.z()-desiredDistance)/4.0);

        deltaV1=deltaV1*1000.0;

        deltaV2=-7*(omega*(initialState.z()-desiredDistance)/4.0);

        deltaV2=deltaV2*1000.0;

        totalDeltaV=fabs(deltaV1)+fabs(deltaV2);
        //Supossed duration T/2.
        myDurationInSecs=period/2.0;
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

ScenarioStateVectorType TBTtangential::getFinalState(){
    return finalState;
}




/*---------------------------------------------------------------
 TBTradial.- Includes RBT_ALONG_X_AXIS,RBT_90_DEGREES_FLY_AROUND,
 RBT_360_DEGREES_FLY_AROUND
  -------------------------------------------------------------*/

/*TBTradial::TBTradial(){
}*/

void TBTradial::setRadial(int designator,double mu,double radius,ScenarioStateVectorType vec,double desiredDistance){

    double omega;
    double period;

    initialState.setX(vec.x());
    initialState.setY(vec.y());
    initialState.setZ(vec.z());
    initialState.setVx(vec.vx());
    initialState.setVy(vec.vy());
    initialState.setVz(vec.vz());

    omega=sqrt(mu/pow(radius,3));

    //Duration of manoeuvre is a period

    period=2*sta::Pi()*sqrt(pow(radius,3)/mu);


    if(designator==RBT_ALONG_X_AXIS){

        deltaV1=-(omega*(initialState.x()-desiredDistance)/4.0);

        deltaV1=deltaV1*1000.0;

        deltaV2=deltaV1;
        totalDeltaV=2*fabs(deltaV1);

        myDurationInSecs=int(period/2);


    }else if (designator==RBT_90_DEGREES_FLY_AROUND){
        //In relative Z. z input positive, means we want target down

        deltaV1=-(omega*(initialState.z()-desiredDistance));

        deltaV1=deltaV1*1000.0;

        deltaV2=2*deltaV1;
        qWarning("deltaV1 %f \n",deltaV1);

        totalDeltaV=fabs(3*deltaV1);
        //Supossed duration T/2.
        myDurationInSecs=period/4;


    }else if (designator==RBT_360_DEGREES_FLY_AROUND){

        deltaV1=-(omega*desiredDistance);

        deltaV1=deltaV1*1000.0;

        deltaV2=-deltaV1;

        totalDeltaV=fabs(2*deltaV1);
        //Supossed duration T/2.
        myDurationInSecs=period;
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

ScenarioStateVectorType TBTradial::getFinalState(){
    return finalState;
}

/*---------------------------------------------------------------
 CFTtransfer.- Includes HOLD_POINTS_VBAR,SLT_VBAR,SLT_RBAR,

    CFT_TANGENTIAL, CFT_RADIAL
  -------------------------------------------------------------*/


void CFTtransfer::setTransfer(int designator,double mu,double radius,ScenarioStateVectorType vec,double desiredDistance){

    double omega;
    double period;

    initialState.setX(vec.x());
    initialState.setY(vec.y());
    initialState.setZ(vec.z());
    initialState.setVx(vec.vx());
    initialState.setVy(vec.vy());
    initialState.setVz(vec.vz());

    omega=sqrt(mu/pow(radius,3));

    //Duration of manoeuvre is a period

    period=2*sta::Pi()*sqrt(pow(radius,3)/mu);


    if(designator==CFT_TANGENTIAL){

        constantForce=omega*omega*(desiredDistance-initialState.z())/(4*sta::Pi());
        constantForce=constantForce*1000.0;
        totalDeltaV=omega*(desiredDistance-initialState.z())/2.0;
        totalDeltaV=totalDeltaV*1000.0;
        myDurationInSecs=int(period);

    }else if (designator==CFT_RADIAL){

        constantForce=omega*omega*(desiredDistance-initialState.x())/(4*sta::Pi());
        constantForce=constantForce*1000.0;
        totalDeltaV=omega*(desiredDistance-initialState.x())/2.0;
        totalDeltaV=totalDeltaV*1000.0;
        myDurationInSecs=int(period);



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



ScenarioStateVectorType CFTtransfer::getFinalState(){
    return finalState;
}













//As similar possible as loitering propagation


/////////////////////////////////////// PropagateRendezVousTrajectory /////////////////////////////

bool PropagateRendezVousTrajectory(ScenarioRendezVousManoeuvreType* rvManoeuvre,
                                     QList<double>& sampleTimes,
                                     QList<ScenarioStateVectorType>& samples,
                                     PropagationFeedback& propFeedback)
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

    ScenarioAbstract6DOFPositionType* position = rvManoeuvre->InitialPosition()->Abstract6DOFPosition().data();
    //ScenarioAbstract6DOFPositionType* targetPosition = targetLoitering->InitialPosition()->Abstract6DOFPosition().data();
    //Needed here target data for coordinate system change

    //sta::StateVector initialState = AbstractPositionToStateVector(position, centralBody);
    //sta::StateVector targetInitialState=AbstractPositionToStateVector(targetPosition,centralBody);

    //CAUTION AGAIN
    //initialState=targetInitialState-initialState;
    sta::KeplerianElements initialStateKeplerian = AbstractPositionToKeplerianElements(position, centralBody);


    //double mu = centralBody->mu();
 double mu=Earth_GRAVITY_PARAMETER/1000000000.0;
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
    ScenarioStateVectorType* initialState=new ScenarioStateVectorType();

    // deviation, reference, and q will be used only in Encke propagation
    //sta::StateVector deviation(Eigen::Vector3d::Zero(), Eigen::Vector3d::Zero());
    //sta::StateVector reference = initialState;
    double q = 0.0;

    double startTime = sta::JdToMjd(sta::CalendarToJd(rvManoeuvre->TimeLine()->StartTime()));

    //sampleTimes << startTime;
    //samples << stateVector;

    //QFile ciccio("data/PerturbationsData.stae");
    //QTextStream cicciostream(&ciccio);
    //ciccio.open(QIODevice::WriteOnly);

    //unsigned int steps = 0;
    unsigned int steps = 1; // patched by Ana on 18th June 2010

    QString propagator = rvManoeuvre->PropagationPosition()->propagator();
    QString integrator = rvManoeuvre->PropagationPosition()->integrator();

    //This depends on the type of manoeuvre

    //State and accel is the vector needed by the propagator. By default
    //all accels should be zero.
    double stateANDaccel[9];
    stateANDaccel[6]=0.0;
    stateANDaccel[7]=0.0;
    stateANDaccel[8]=0.0;

    initialState->setX(0.0);
    initialState->setY(0.0);
    initialState->setZ(0.0);
    initialState->setVy(0.0);
    initialState->setVx(0.0);
    initialState->setVz(0.0);


    if(rvManoeuvre->ElementIdentifier()->Name().compare("2TBTalong x-axis")==0 ||
       rvManoeuvre->ElementIdentifier()->Name().compare("2TBTHohmann Transfer")==0 ||
       rvManoeuvre->ElementIdentifier()->Name().compare("2TBT90 degrees fly-around")==0)
    {
        initialState->setVx(rvManoeuvre->DeltaV1());

    }

    if(rvManoeuvre->ElementIdentifier()->Name().compare("2RBTalong x-axis")==0 ||
       rvManoeuvre->ElementIdentifier()->Name().compare("2RBT90 degrees radial fly-around")==0 ||
       rvManoeuvre->ElementIdentifier()->Name().compare("2RBT360 degrees radial fly-around")==0)
    {
        initialState->setVz(rvManoeuvre->DeltaV1());
    }


    if(rvManoeuvre->ElementIdentifier()->Name().compare("CFT Tangential")==0){
        stateANDaccel[6]=rvManoeuvre->DeltaV1();
    }


    if(rvManoeuvre->ElementIdentifier()->Name().compare("CFT Radial")==0){
        stateANDaccel[8]=rvManoeuvre->DeltaV1();
    }




    //Preparing data for propagator

    stateANDaccel[0]=initialState->x();
    stateANDaccel[1]=initialState->y();
    stateANDaccel[2]=initialState->z();
    stateANDaccel[3]=initialState->vx();
    stateANDaccel[4]=initialState->vy();
    stateANDaccel[5]=initialState->vz();


    double sma            = initialStateKeplerian.SemimajorAxis;

    double omega=sqrt(mu/pow(sma,3));



        double e              = initialStateKeplerian.Eccentricity;
        double inclination    = initialStateKeplerian.Inclination*Pi()/180.0;
        double raan           = initialStateKeplerian.AscendingNode*Pi()/180.0;
        double argOfPeriapsis = initialStateKeplerian.ArgumentOfPeriapsis*Pi()/180.0;
        double trueAnomaly    = initialStateKeplerian.TrueAnomaly*Pi()/180.0;
        double meanAnomaly    = trueAnomalyTOmeanAnomaly(trueAnomaly, e);


        double perigee = sma * (1 - e);

        if (perigee < centralBody->meanRadius())
        {
            propFeedback.raiseError(QObject::tr("The perigee distance is smaller than the main body radius."));
            return false;
        }
        else
        {
            CWpropagator* unperturbedPropagator=new CWpropagator(omega, stateANDaccel, timelineDuration, dt);

            unperturbedPropagator->solverCW(startTime);

            samples.append(unperturbedPropagator->getResults());

            //DELTAV2 and endings adjustments

            if(rvManoeuvre->ElementIdentifier()->Name().compare("2TBTalong x-axis")==0 ||
               rvManoeuvre->ElementIdentifier()->Name().compare("2TBTHohmann Transfer")==0 ||
               rvManoeuvre->ElementIdentifier()->Name().compare("2TBT90 degrees fly-around")==0)
                samples.last().setVx(samples.last().vx()+rvManoeuvre->DeltaV2());


            if(rvManoeuvre->ElementIdentifier()->Name().compare("2RBTalong x-axis")==0 ||
               rvManoeuvre->ElementIdentifier()->Name().compare("2RBT90 degrees radial fly-around")==0 ||
               rvManoeuvre->ElementIdentifier()->Name().compare("2RBT360 degrees radial fly-around")==0)
                samples.last().setVz(samples.last().vz()+rvManoeuvre->DeltaV2());

            sampleTimes.append(unperturbedPropagator->getSampleTimes());

        }


    return true;
} // ------------------------------- End of the propagation method -----------------------





/****************************************************
  CWPropagators
  **************************************************/

// CWpropagator

CWpropagator::CWpropagator(double omega, double vector[9],double enTime,double size)
{
    int i;
    targetOmega=omega;
    for (i=0;i<9;i++){

        initialStateVector[i]=vector[i];

    }

    endTime=enTime;
    stepSize=size;

}

CWpropagator::~CWpropagator(){}

void CWpropagator::solverCW(double startTime){
    int t,i;
    ScenarioStateVectorType sVector;
    double x0=initialStateVector[0];
    double y0=initialStateVector[1];
    double z0=initialStateVector[2];
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

        sVector.setX(initialStateVector[0]);
        sVector.setY(initialStateVector[1]);
        sVector.setZ(initialStateVector[2]);
        sVector.setVx(initialStateVector[3]);
        sVector.setVy(initialStateVector[4]);
        sVector.setVz(initialStateVector[5]);

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


        sVector.setX(X1*sinOmegaT-X2*cosOmegaT+X3*t+X4+
                     X5*(targetOmegaT-sinOmegaT)+
                     X6*(4*(1-cosOmegaT)/(targetOmega*targetOmega))-
                     X6*(3.0*t*t/2.0));
        //km
        sVector.setX(sVector.x()/1000.0);

        sVector.setY(y0*cosOmegaT+Y1*sinOmegaT+
                               Y2*(1-cosOmegaT));
        sVector.setY(sVector.y()/1000.0);


        sVector.setZ(Z1*cosOmegaT+Z2*sinOmegaT+Z3+
                               Z4*(sinOmegaT-targetOmegaT)+
                               Z5*(1-cosOmegaT));
        sVector.setZ(sVector.z()/1000.0);


        //Speeds

        sVector.setVx((sVector.x()-results[i-1].x())/stepSize);
        sVector.setVy((sVector.y()-results[i-1].y())/stepSize);
        sVector.setVz((sVector.z()-results[i-1].z())/stepSize);

        //Accelerations.
        //Suposed to be constants.
        //sVector.stateVector[6]=ax;
        //sVector.stateVector[7]=ay;
        //sVector.stateVector[8]=az;

        results.append(sVector);
        jd = startTime + ((t/3600.0)/24.0);

        //qWarning(" X en CW, sampleo %d %f\n",i,sVector.x());
        sampleTimes.append(jd);



    }






}

QList <ScenarioStateVectorType>& CWpropagator::getResults(){
    return results;

}

QList<double>& CWpropagator::getSampleTimes(){
    return sampleTimes;
}




