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

 */

#include "RVXMLInterface.h"

bool RVXMLInterface::loadRVScenario(SpaceScenario* scen,PropagatedScenario* prop){
    //First it checks if there's a scenario

    if (scen == NULL) return false;

    //Ok, there's scenario. Checking for RVScenarios only.

    if(scen->Name().compare("Rendezvous Scenario")==0){
        //Everything is OK.

        scenario=new SpaceScenario(*scen);
        //Setting descriptors. Adapted code from Analysis
        //First, target and chaser.
        //*target=dynamic_cast<ScenarioSC>(scenario->AbstractParticipant().first());
        //*chaser=dynamic_cast<ScenarioSC>(scenario->AbstractParticipant().last());
        QSharedPointer <ScenarioParticipantType> participant(scenario->AbstractParticipant().first());
        chaser=dynamic_cast<ScenarioSC*>(participant.data());

        chaser->SCMission()->TrajectoryPlan()->AbstractTrajectory();

        for (int i=0;i<chaser->SCMission()->TrajectoryPlan()->AbstractTrajectory().size();i++)
        {
            RVManoeuvreDescriptor *RVman=new RVManoeuvreDescriptor();

            RVman->fullName.append(chaser->SCMission()->TrajectoryPlan()->AbstractTrajectory().at(i)->elementName());


            if (dynamic_cast<ScenarioLoiteringType*>(chaser->SCMission()->TrajectoryPlan()->AbstractTrajectory().at(i).data()))
            {
                RVman->descriptor=FREE_DRIFT;
            }
            else if (dynamic_cast<ScenarioDeltaVType*>(chaser->SCMission()->TrajectoryPlan()->AbstractTrajectory().at(i).data()))
            {
                RVman->descriptor=DELTA_V;
            }


            manPlan.plan.append(*RVman);

        }


        if(prop->spaceObjects().isEmpty()){
            //Just useful if we have propagated scenario. If we don't, needed a new one.
            return true;
            }
         else
            {
                propScenario=new PropagatedScenario(*prop);
                targetPropagated=propScenario->spaceObjects().last();
                chaserPropagated=propScenario->spaceObjects().first();
            }

        return true;


       }
        else //No name match.
          return false;


}


void RVXMLInterface::createRVscenario(SpaceScenario* scen,ScenarioSC* chas,ScenarioSC* targ,PropagatedScenario* prop){

    //Init like analisys. It seemed safer.
    scenario=new SpaceScenario(*scen);
    //scenario=scen;
    scenario->setName("Rendezvous Scenario");

    //CHASER
    chaser=new ScenarioSC(*chas);
    //chaser=chas;
    chaser->setName("Chaser");
    chaser->ElementIdentifier()->setName("Chaser");

    //TARGET
    target=new ScenarioSC(*targ);
    //target=targ;
    target->setName("Target");
    target->ElementIdentifier()->setName("Target");

    propScenario=new PropagatedScenario(*prop);

    //Needed for endingtime settings.
    newChaser();
    newTarget();




}

SpaceScenario* RVXMLInterface::getScenario(){

    return scenario;
}
ScenarioSC* RVXMLInterface::getChaser(){

    return chaser;
}

ScenarioSC* RVXMLInterface::getTarget(){

    return target;
}




void RVXMLInterface::newChaser(){

    // Create the initial position (Keplerian elements)
    ScenarioKeplerianElementsType* elements = new ScenarioKeplerianElementsType();
    elements->setSemiMajorAxis(87000.0);
    elements->setEccentricity(0.0045);
    elements->setInclination(27.456);
    elements->setRAAN(132.34);
    elements->setArgumentOfPeriapsis(231.34);
    elements->setTrueAnomaly(45.32);

    // Create the initial attitude (Euler elements)
    ScenarioEulerBIType*  initAtt = new ScenarioEulerBIType();
    initAtt->setPhi(0.00000);
    initAtt->setTheta(0.00000);
    initAtt->setPsi(0.00000);
    initAtt->setPhiDot(0.00000);
    initAtt->setThetaDot(0.00000);
    initAtt->setPsiDot(0.00000);

    // Create the trajectory arc
    ScenarioLoiteringType* loitering = new ScenarioLoiteringType();
    loitering->ElementIdentifier()->setName("ChaserLoitering");
    loitering->ElementIdentifier()->setColorName("Yellow");
    loitering->Environment()->CentralBody()->setName("Earth");
    loitering->InitialPosition()->setCoordinateSystem("INERTIAL J2000");
    loitering->PropagationPosition()->setTimeStep(60.0);
    loitering->PropagationPosition()->setPropagator("TWO BODY");
    loitering->PropagationPosition()->setIntegrator("RK4");
    loitering->InitialAttitude()->setCoordinateSystem("EULER 123");
    loitering->PropagationAttitude()->setIntegrator("");	// Not defined in STA yet
    loitering->PropagationAttitude()->setTimeStep(60.0);


    // Time-line
    QDateTime TheCurrentDateAndTime = QDateTime::currentDateTime(); // Get the current epoch
    loitering->TimeLine()->setStartTime(TheCurrentDateAndTime);
    loitering->TimeLine()->setEndTime(TheCurrentDateAndTime.addDays(1));
    loitering->TimeLine()->setStepTime(60.0);

    loitering->InitialPosition()->setAbstract6DOFPosition(QSharedPointer<ScenarioAbstract6DOFPositionType>(elements));
    loitering->InitialAttitude()->setAbstract6DOFAttitude(QSharedPointer<ScenarioAbstract6DOFAttitudeType>(initAtt));

    // Create the spacecraft loitering arc
    chaser->SCMission()->TrajectoryPlan()->AbstractTrajectory().append(QSharedPointer<ScenarioAbstractTrajectoryType>(loitering));

    //ADDIn m1 keeps initial loitering data.
    RVManoeuvreDescriptor m1,m2;
    m1.descriptor=FREE_DRIFT;
    m1.endTime=loitering->TimeLine()->EndTime();
    //m1 must be first always
    if(manPlan.plan.isEmpty())
        manPlan.plan.append(m1);
    manPlan.plan.replace(0,m1);


    //manPlan.plan.replace(0,m1);

    for (int i=1;i<manPlan.plan.size();i++)
        {

        if(manPlan.plan[i].getDescriptor()==FREE_DRIFT){
            ScenarioLoiteringType* freeDrift = new ScenarioLoiteringType();
            setFreeDrift(freeDrift,i);
            chaser->SCMission()->TrajectoryPlan()->AbstractTrajectory().append(QSharedPointer<ScenarioAbstractTrajectoryType>(freeDrift));
           }

        if(manPlan.plan[i].getDescriptor()==DELTA_V){
            ScenarioDeltaVType* deltaV = new ScenarioDeltaVType();
            setDeltaV(deltaV,i);
            chaser->SCMission()->TrajectoryPlan()->AbstractTrajectory().append(QSharedPointer<ScenarioAbstractTrajectoryType>(deltaV));
           }

        }





    scenario->AbstractParticipant().append(QSharedPointer<ScenarioParticipantType>(chaser));
    //Putting target endtime online

}


void RVXMLInterface::setFreeDrift(ScenarioLoiteringType* freeDrift,int order){
    QDateTime time;
    if (order==0)
        time = QDateTime::currentDateTime();
    else
        time = manPlan.plan[order-1].endTime;

    // Create the initial position (Keplerian elements)

    ScenarioKeplerianElementsType* elements = new ScenarioKeplerianElementsType();
    /*elements->setSemiMajorAxis(87000.0);
    elements->setEccentricity(0.0045);
    elements->setInclination(27.456);
    elements->setRAAN(132.34);
    elements->setArgumentOfPeriapsis(231.34);
    elements->setTrueAnomaly(45.32);*/

    // Create the initial attitude (Euler elements)
    ScenarioEulerBIType*  initAtt = new ScenarioEulerBIType();
    initAtt->setPhi(0.00000);
    initAtt->setTheta(0.00000);
    initAtt->setPsi(0.00000);
    initAtt->setPhiDot(0.00000);
    initAtt->setThetaDot(0.00000);
    initAtt->setPsiDot(0.00000);

    // Create the trajectory arc

    freeDrift->ElementIdentifier()->setName("ChaserLoitering");
    freeDrift->ElementIdentifier()->setColorName("Yellow");
    freeDrift->Environment()->CentralBody()->setName("Earth");
    freeDrift->InitialPosition()->setCoordinateSystem("INERTIAL J2000");
    freeDrift->PropagationPosition()->setTimeStep(60.0);
    freeDrift->PropagationPosition()->setPropagator("TWO BODY");
    freeDrift->PropagationPosition()->setIntegrator("RK4");
    freeDrift->InitialAttitude()->setCoordinateSystem("EULER 123");
    freeDrift->PropagationAttitude()->setIntegrator("");	// Not defined in STA yet
    freeDrift->PropagationAttitude()->setTimeStep(60.0);


    // Time-line

    freeDrift->TimeLine()->setStartTime(time);
    freeDrift->TimeLine()->setEndTime(time.addDays(1));
    freeDrift->TimeLine()->setStepTime(60.0);

    freeDrift->InitialPosition()->setAbstract6DOFPosition(QSharedPointer<ScenarioAbstract6DOFPositionType>(elements));
    freeDrift->InitialAttitude()->setAbstract6DOFAttitude(QSharedPointer<ScenarioAbstract6DOFAttitudeType>(initAtt));

    manPlan.plan[order].endTime=freeDrift->TimeLine()->EndTime();

}



void RVXMLInterface::setDeltaV(ScenarioDeltaVType* deltaV,int order){
    QDateTime time;
    if (order==0)
        time = QDateTime::currentDateTime();
    else
        time = manPlan.plan[order-1].endTime;

    deltaV->TimeLine()->setStartTime(time);
    deltaV->setDeltaVx(500);
    deltaV->setDeltaVy(0);
    deltaV->setDeltaVz(0);
    deltaV->setMagnitude(500);

    deltaV->TimeLine()->setEndTime(time.addSecs(3));

    deltaV->Environment()->CentralBody()->setName("Earth");
    deltaV->InitialPosition()->setCoordinateSystem("INERTIAL J2000");

    deltaV->ElementIdentifier()->setName("ChaserDeltaV");
    deltaV->ElementIdentifier()->setColorName("Red");

    deltaV->ElementIdentifier()->setModelName("DEFAULT");


    manPlan.plan[order].endTime=deltaV->TimeLine()->EndTime();

}




void RVXMLInterface:: newTarget(){


     // Create the initial position (Keplerian elements)
     ScenarioKeplerianElementsType* elements = new ScenarioKeplerianElementsType();
     elements->setSemiMajorAxis(95000.0);
     elements->setEccentricity(0.0045);
     elements->setInclination(27.456);
     elements->setRAAN(132.34);
     elements->setArgumentOfPeriapsis(231.34);
     elements->setTrueAnomaly(45.32);

     // Create the initial attitude (Euler elements)
     ScenarioEulerBIType*  initAtt = new ScenarioEulerBIType();
     initAtt->setPhi(0.00000);
     initAtt->setTheta(0.00000);
     initAtt->setPsi(0.00000);
     initAtt->setPhiDot(0.00000);
     initAtt->setThetaDot(0.00000);
     initAtt->setPsiDot(0.00000);


     // Create the trajectory arc
     ScenarioLoiteringType* loitering = new ScenarioLoiteringType();
     loitering->ElementIdentifier()->setName("TargetLoitering");
     loitering->ElementIdentifier()->setColorName("White");
     loitering->Environment()->CentralBody()->setName("Earth");
     loitering->InitialPosition()->setCoordinateSystem("INERTIAL J2000");
     loitering->PropagationPosition()->setTimeStep(60.0);
     loitering->PropagationPosition()->setPropagator("TWO BODY");
     loitering->PropagationPosition()->setIntegrator("RK4");
     loitering->InitialAttitude()->setCoordinateSystem("EULER 123");
     loitering->PropagationAttitude()->setIntegrator("");	// Not defined in STA yet
     loitering->PropagationAttitude()->setTimeStep(60.0);

     // Time-line
     QDateTime TheCurrentDateAndTime = QDateTime::currentDateTime(); // Get the current epoch
     loitering->TimeLine()->setStartTime(TheCurrentDateAndTime);
     //Setting endingtime in target acording to ending time
     //in last manoeuvre.
     loitering->TimeLine()->setEndTime(manPlan.plan.last().endTime);
     //loitering->TimeLine()->setEndTime(TheCurrentDateAndTime.addDays(1));

     loitering->TimeLine()->setStepTime(60.0);

     loitering->InitialPosition()->setAbstract6DOFPosition(QSharedPointer<ScenarioAbstract6DOFPositionType>(elements));
     loitering->InitialAttitude()->setAbstract6DOFAttitude(QSharedPointer<ScenarioAbstract6DOFAttitudeType>(initAtt));

     // Create the spacecraft loitering arc
     target->SCMission()->TrajectoryPlan()->AbstractTrajectory().append(QSharedPointer<ScenarioAbstractTrajectoryType>(loitering));
     scenario->AbstractParticipant().append(QSharedPointer<ScenarioParticipantType>(target));


}

PropagatedScenario* RVXMLInterface::getPropagatedScenario(){

    return propScenario;
}
/*
void RVXMLInterface::setPropagatedScenario(PropagatedScenario* prop){
    propScenario=prop;

}*/

QString RVXMLInterface::getPropagatedTrajectory(int order){
    //This method is a temporal fix to take out data from propagated
    //scenario
    SpaceObject* spaceObj;
    if (order==1)
            spaceObj=targetPropagated;
    else
            spaceObj=chaserPropagated;


    MissionArc* arc=spaceObj->mission().first();

    sta::StateVector* result=new sta::StateVector();
    Vector3d vector;
    double x,y,z;

    QString temp1,temp2,temp3;
    //CAUTION with sample time. Where is it?
    for (double i=spaceObj->missionStartTime();i<spaceObj->missionEndTime();i=i+0.01)
    {
        arc->getStateVector(i,result);
        vector=result->position;
        z=vector.z();
        y=vector.y();
        x=vector.x();

        temp1.setNum(x);
        temp1.append(" -- ");
        temp2.setNum(y);

        temp1.append(temp2);
        temp1.append(" -- ");
        temp2.setNum(z);
        temp1.append(temp2);
        temp1.append(" \n");
        temp3.append(temp1);
        temp2.clear();

    }
return temp3;
}




