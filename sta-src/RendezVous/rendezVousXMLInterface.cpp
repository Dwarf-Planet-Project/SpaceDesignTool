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
 Interface class between XML and RV module.
 */

#include "rendezVousXMLInterface.h"

/* ---------------------------------------------------------------------------------
   loadRVScenario.- gets data from STA main. SpaceScenario and PropagatedScenario if
                   available. It sets also InterfXML.manPlan if previous manoeuvres
                   detected
   ---------------------------------------------------------------------------------*/

bool RVXMLInterface::loadRVScenario(SpaceScenario* scen,PropagatedScenario* prop){

    //First it checks if there's a scenario

    if (scen == NULL) return false;

    //Ok, there's scenario. Checking for RVScenarios only.
    if(scen->Name().compare("RendezVous Scenario")==0){

        //It's a RendezVous scenario.

        scenario=new SpaceScenario(*scen);

        //Setting descriptors. Adapted code from Analysis
        //First, target and chaser.
        //*target=dynamic_cast<ScenarioSC>(scenario->AbstractParticipant().first());
        //*chaser=dynamic_cast<ScenarioSC>(scenario->AbstractParticipant().last());

        QSharedPointer <ScenarioParticipantType> participant(scenario->AbstractParticipant().last());
        chaser=dynamic_cast<ScenarioSC*>(participant.data());
        QSharedPointer <ScenarioParticipantType> participant2(scenario->AbstractParticipant().first());
        target=dynamic_cast<ScenarioSC*>(participant2.data());

        //Cleaning manPlan before adding manoeuvres
        if(!manPlan.plan.isEmpty())
            manPlan.plan.clear();


        if(!chaser->SCMission()->TrajectoryPlan()->AbstractTrajectory().isEmpty())
        {
            //Loop around all chaser manoeuvres.
            for (int i=0;i<chaser->SCMission()->TrajectoryPlan()->AbstractTrajectory().size();i++)
            {
                //Extracting name and preparing descriptor for manPlan
                RVManoeuvreDescriptor *RVman=new RVManoeuvreDescriptor();

                RVman->fullName.append(chaser->SCMission()->TrajectoryPlan()->AbstractTrajectory().at(i)->elementName());

                if (RVman->fullName.compare("LoiteringType")==0)
                {

                    if ( dynamic_cast<ScenarioLoiteringType*>(chaser->SCMission()->TrajectoryPlan()->AbstractTrajectory().at(i).data()) )
                       {

                        RVman->descriptor=FREE_DRIFT;
                        QDateTime *dateTemp=new QDateTime(dynamic_cast<ScenarioLoiteringType*>
                                        (chaser->SCMission()->TrajectoryPlan()->AbstractTrajectory().
                                         at(i).data())->TimeLine()->StartTime());

                        RVman->initTime.setDate(dateTemp->date());
                        RVman->initTime.setTime(dateTemp->time());

                        QDateTime *dateTemp2=new QDateTime(dynamic_cast<ScenarioLoiteringType*>
                                        (chaser->SCMission()->TrajectoryPlan()->AbstractTrajectory().
                                         at(i).data())->TimeLine()->EndTime());


                        RVman->endTime.setDate(dateTemp2->date());
                        RVman->endTime.setTime(dateTemp2->time());


                    }
                }
                else if (dynamic_cast<ScenarioDeltaVType*>(chaser->SCMission()->TrajectoryPlan()->AbstractTrajectory().at(i).data()))
                {

                    RVman->descriptor=DELTA_V;

                    QDateTime *dateTemp=new QDateTime(dynamic_cast<ScenarioDeltaVType*>
                                    (chaser->SCMission()->TrajectoryPlan()->AbstractTrajectory().
                                     at(i).data())->TimeLine()->StartTime());

                    RVman->initTime.setDate(dateTemp->date());
                    RVman->initTime.setTime(dateTemp->time());


                    QDateTime *dateTemp2=new QDateTime(dynamic_cast<ScenarioDeltaVType*>
                                    (chaser->SCMission()->TrajectoryPlan()->AbstractTrajectory().
                                     at(i).data())->TimeLine()->EndTime());

                    RVman->endTime.setDate(dateTemp2->date());
                    RVman->endTime.setTime(dateTemp2->time());

                }
                else if(dynamic_cast<ScenarioRendezVousManoeuvreType*>(chaser->SCMission()->TrajectoryPlan()->AbstractTrajectory().at(i).data()))
                {


                    ScenarioRendezVousManoeuvreType* rvManoeuvre=new ScenarioRendezVousManoeuvreType();
                    rvManoeuvre=(dynamic_cast<ScenarioRendezVousManoeuvreType*>(chaser->SCMission()->TrajectoryPlan()->AbstractTrajectory().at(i).data()));
                    //NAME settings

                    QString name;
                    name.append(rvManoeuvre->ElementIdentifier()->Name());


                    if(name.compare("2TBTalong x-axis")==0)
                               RVman->descriptor=TBT_ALONG_X_AXIS;
                    if(name.compare("2TBTHohmann Transfer")==0)
                               RVman->descriptor=TBT_HOHMANN_TRANSFER;
                    if(name.compare("2TBT90 degrees fly-around")==0)
                               RVman->descriptor=TBT_90_DEGREES_FLY_AROUND;
                    if(name.compare("2RBTalong x-axis")==0)
                               RVman->descriptor=RBT_ALONG_X_AXIS;
                    if(name.compare("2RBT90 degrees radial fly-around")==0)
                               RVman->descriptor=RBT_90_DEGREES_FLY_AROUND;
                    if(name.compare("2RBT360 degrees radial fly-around")==0)
                               RVman->descriptor=RBT_360_DEGREES_FLY_AROUND;
                    if(name.compare("Hold Points outside V-bar")==0)
                               RVman->descriptor=HOLD_POINTS_VBAR;
                    if(name.compare("SLT V-bar")==0)
                               RVman->descriptor=SLT_VBAR;
                    if(name.compare("SLT R-bar")==0)
                               RVman->descriptor=SLT_RBAR;
                    if(name.compare("CFT Tangential")==0)
                               RVman->descriptor=CFT_TANGENTIAL;
                    if(name.compare("CFT Radial")==0)
                               RVman->descriptor=CFT_RADIAL;

                    QDateTime *dateTemp=new QDateTime(dynamic_cast<ScenarioRendezVousManoeuvreType*>
                                    (chaser->SCMission()->TrajectoryPlan()->AbstractTrajectory().
                                     at(i).data())->TimeLine()->StartTime());

                    RVman->initTime.setDate(dateTemp->date());
                    RVman->initTime.setTime(dateTemp->time());


                    QDateTime *dateTemp2=new QDateTime(dynamic_cast<ScenarioRendezVousManoeuvreType*>
                                    (chaser->SCMission()->TrajectoryPlan()->AbstractTrajectory().
                                     at(i).data())->TimeLine()->EndTime());

                    RVman->endTime.setDate(dateTemp2->date());
                    RVman->endTime.setTime(dateTemp2->time());
                }


                manPlan.plan.append(*RVman);

             }
        }

        if(prop==NULL){

            //Just useful if we have propagated scenario. If we don't, needed a new one.
            return false;
            }
         else if(!prop->spaceObjects().isEmpty())
            {

                propScenario=new PropagatedScenario(*prop);
                targetPropagated=propScenario->spaceObjects().first();
                chaserPropagated=propScenario->spaceObjects().last();
                return true;

            }


       }
        else //No name match.
          return false;

}






/* ---------------------------------------------------------------------------------
   createRVscenario.-
   ---------------------------------------------------------------------------------*/


void RVXMLInterface::createRVscenario(SpaceScenario* scen,ScenarioSC* chas,ScenarioSC* targ,
                                      PropagatedScenario* prop){

    //Init like analisys. It seemed safer.

    scenario=new SpaceScenario(*scen);

    scenario->setName("RendezVous Scenario");

    //CHASER
    chaser=new ScenarioSC(*chas);

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

    scenario->AbstractParticipant().append(QSharedPointer<ScenarioParticipantType>(target));
    //Chaser must be added last
    scenario->AbstractParticipant().append(QSharedPointer<ScenarioParticipantType>(chaser));



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

    // ******
    // CHASER
    // ******

    if(manPlan.plan.isEmpty()){

    //Not sure about loitering first manoeuvre, but it's a start up
    //Adding m1 keeps initial loitering data.
    /*    RVManoeuvreDescriptor *m1=new RVManoeuvreDescriptor();
        m1->descriptor=FREE_DRIFT;
        m1->endTime=chaserLoitering->TimeLine()->EndTime();
        manPlan.plan.append(*m1);*/
        setManoeuvrePlan();

    }
    else{
        manPlan.plan.clear();
        setManoeuvrePlan();
    }



}


/* ---------------------------------------------------------------------------------

    setManoeuvrePlan.- sets manoeuvre plan in chaser XML structure

   ---------------------------------------------------------------------------------*/


void RVXMLInterface::setManoeuvrePlan(){



    //manPlan.plan.append(phasingPlan.plan);
    manPlan.plan.append(closeRangePlan.plan);

    //For all manPlan elements, cheking must be done
    //In the end, everything is DeltaV and FreeDrift, but it should be treated
    //in different ways

    for (int i=0;i<manPlan.plan.size();i++)
        {

            if(manPlan.plan[i].getDescriptor()==FREE_DRIFT){
                ScenarioLoiteringType* freeDrift = new ScenarioLoiteringType();
                setFreeDrift(freeDrift,i);
                chaser->SCMission()->TrajectoryPlan()->AbstractTrajectory().append(QSharedPointer<ScenarioAbstractTrajectoryType>(freeDrift));

               }else if(manPlan.plan[i].getDescriptor()==DELTA_V){
                ScenarioDeltaVType* deltaV = new ScenarioDeltaVType();
                setDeltaV(deltaV,i);
                chaser->SCMission()->TrajectoryPlan()->AbstractTrajectory().append(QSharedPointer<ScenarioAbstractTrajectoryType>(deltaV));

               }else if(manPlan.plan[i].getDescriptor()==SB_TANGENTIAL){
                /*ScenarioDeltaVType* deltaVx = newScenarioDeltaVType();
                setDeltaV(deltaVx,i);
                chaser->SCMission()->TrajectoryPlan()->AbstractTrajectory().append(QSharedPointer<ScenarioAbstractTrajectoryType>(deltaVx));*/
               }else if(manPlan.plan[i].getDescriptor()==TBT_ALONG_X_AXIS){

                   ScenarioRendezVousManoeuvreType* tbtAlongXaxis=new ScenarioRendezVousManoeuvreType();

                   setManoeuvre(TBT_ALONG_X_AXIS,i,tbtAlongXaxis);

                   chaser->SCMission()->TrajectoryPlan()->AbstractTrajectory().append(QSharedPointer<ScenarioAbstractTrajectoryType>(tbtAlongXaxis));


                }else if(manPlan.plan[i].getDescriptor()==TBT_HOHMANN_TRANSFER){

                    ScenarioRendezVousManoeuvreType* tbtHohmann=new ScenarioRendezVousManoeuvreType();

                    setManoeuvre(TBT_HOHMANN_TRANSFER,i,tbtHohmann);

                    chaser->SCMission()->TrajectoryPlan()->AbstractTrajectory().append(QSharedPointer<ScenarioAbstractTrajectoryType>(tbtHohmann));


                }else if(manPlan.plan[i].getDescriptor()==TBT_90_DEGREES_FLY_AROUND){

                    ScenarioRendezVousManoeuvreType* tbt90degrees=new ScenarioRendezVousManoeuvreType();

                    setManoeuvre(TBT_90_DEGREES_FLY_AROUND,i,tbt90degrees);

                    chaser->SCMission()->TrajectoryPlan()->AbstractTrajectory().append(QSharedPointer<ScenarioAbstractTrajectoryType>(tbt90degrees));


                }else if(manPlan.plan[i].getDescriptor()==RBT_ALONG_X_AXIS){

                    ScenarioRendezVousManoeuvreType* rbtAlongXaxis=new ScenarioRendezVousManoeuvreType();

                    setManoeuvre(RBT_ALONG_X_AXIS,i,rbtAlongXaxis);

                    chaser->SCMission()->TrajectoryPlan()->AbstractTrajectory().append(QSharedPointer<ScenarioAbstractTrajectoryType>(rbtAlongXaxis));


                }else if(manPlan.plan[i].getDescriptor()==RBT_90_DEGREES_FLY_AROUND){

                    ScenarioRendezVousManoeuvreType* rbt90degrees=new ScenarioRendezVousManoeuvreType();

                    setManoeuvre(RBT_90_DEGREES_FLY_AROUND,i,rbt90degrees);

                    chaser->SCMission()->TrajectoryPlan()->AbstractTrajectory().append(QSharedPointer<ScenarioAbstractTrajectoryType>(rbt90degrees));


                }else if(manPlan.plan[i].getDescriptor()==RBT_360_DEGREES_FLY_AROUND){

                    ScenarioRendezVousManoeuvreType* rbt360degrees=new ScenarioRendezVousManoeuvreType();

                    setManoeuvre(RBT_360_DEGREES_FLY_AROUND,i,rbt360degrees);

                    chaser->SCMission()->TrajectoryPlan()->AbstractTrajectory().append(QSharedPointer<ScenarioAbstractTrajectoryType>(rbt360degrees));


                }else if(manPlan.plan[i].getDescriptor()==CFT_TANGENTIAL){

                    ScenarioRendezVousManoeuvreType* cftTangential=new ScenarioRendezVousManoeuvreType();

                    setManoeuvre(CFT_TANGENTIAL,i,cftTangential);

                    chaser->SCMission()->TrajectoryPlan()->AbstractTrajectory().append(QSharedPointer<ScenarioAbstractTrajectoryType>(cftTangential));


                }else if(manPlan.plan[i].getDescriptor()==CFT_RADIAL){

                    ScenarioRendezVousManoeuvreType* cftRadial=new ScenarioRendezVousManoeuvreType();

                    setManoeuvre(CFT_RADIAL,i,cftRadial);

                    chaser->SCMission()->TrajectoryPlan()->AbstractTrajectory().append(QSharedPointer<ScenarioAbstractTrajectoryType>(cftRadial));


                }



                else{
                //DEFAULT
                //No descriptor means no manoeuvre.
                //Needed to erase in order not confusing target
                manPlan.plan.removeAt(i);

            }

        }


}


void RVXMLInterface::setManoeuvre(int descriptor,int order,ScenarioRendezVousManoeuvreType* RVmanoeuvre){


    double mu=Earth_GRAVITY_PARAMETER/1000000000.0;


    double radius=userOrbitalDialogInputs[0];

    ScenarioStateVectorType* initialState=new ScenarioStateVectorType();
    //Not sure about this.---- Multilayer complaining?

    double input=manPlan.plan[order].getRVinput();

    initialState->setX(0.0);
    initialState->setY(0.0);
    initialState->setZ(0.0);
    initialState->setVx(0.0);
    initialState->setVy(0.0);
    initialState->setVz(0.0);

    QDateTime time;
    if(order==0){
        time.setDate(userDateTime.date());
        time.setTime(userDateTime.time());}
    else

    time=manPlan.plan[order-1].endTime;


    //All manoeuvres are the same at this point

    manPlan.plan[order].setAssociatedManoeuvre(mu,radius,*initialState,input);
    ScenarioKeplerianElementsType* elements = new ScenarioKeplerianElementsType();
    ScenarioEulerBIType*  initAtt = new ScenarioEulerBIType();
    initAtt->setPhi(0.00000);
    initAtt->setTheta(0.00000);
    initAtt->setPsi(0.00000);
    initAtt->setPhiDot(0.00000);
    initAtt->setThetaDot(0.00000);
    initAtt->setPsiDot(0.00000);

    RVmanoeuvre->ElementIdentifier()->setColorName("Green");
    RVmanoeuvre->Environment()->CentralBody()->setName("Earth");
    RVmanoeuvre->InitialPosition()->setCoordinateSystem("INERTIAL J2000");
    RVmanoeuvre->PropagationPosition()->setTimeStep(userOrbitalDialogInputs[12]);
    RVmanoeuvre->PropagationPosition()->setPropagator("TWO BODY");
    RVmanoeuvre->PropagationPosition()->setIntegrator("RK4");
    RVmanoeuvre->InitialAttitude()->setCoordinateSystem("EULER 123");
    RVmanoeuvre->PropagationAttitude()->setIntegrator("");	// Not defined in STA yet


    RVmanoeuvre->PropagationAttitude()->setTimeStep(userOrbitalDialogInputs[12]);


    RVmanoeuvre->TimeLine()->setStepTime(userOrbitalDialogInputs[12]);

    // Time-line
    int a,b,c;
    a=int(manPlan.plan[order].getDurationInSecs());
    b=int(userOrbitalDialogInputs[12]);
    c=a%b;


    c=userOrbitalDialogInputs[12]-c;
    RVmanoeuvre->TimeLine()->setEndTime(
     time.addSecs(manPlan.plan[order].getDurationInSecs()+c));
    RVmanoeuvre->TimeLine()->setStartTime(time);

    manPlan.plan[order].endTime=RVmanoeuvre->TimeLine()->EndTime();


    RVmanoeuvre->InitialPosition()->setAbstract6DOFPosition(QSharedPointer<ScenarioAbstract6DOFPositionType>(elements));
    RVmanoeuvre->InitialAttitude()->setAbstract6DOFAttitude(QSharedPointer<ScenarioAbstract6DOFAttitudeType>(initAtt));

//tangential

    if(descriptor==TBT_ALONG_X_AXIS){
        RVmanoeuvre->ElementIdentifier()->setName("2TBTalong x-axis");
        RVmanoeuvre->elementName().append("2TBTalong x-axis");

        RVmanoeuvre->setDeltaV1(manPlan.plan[order].tangential->getDeltaV1());
        RVmanoeuvre->setDeltaV2(manPlan.plan[order].tangential->getDeltaV2());
    }

    if(descriptor==TBT_HOHMANN_TRANSFER){
        RVmanoeuvre->ElementIdentifier()->setName("2TBTHohmann Transfer");
        RVmanoeuvre->elementName().append("2TBTHohmann Transfer");

        RVmanoeuvre->setDeltaV1(manPlan.plan[order].tangential->getDeltaV1());
        RVmanoeuvre->setDeltaV2(manPlan.plan[order].tangential->getDeltaV2());
    }

    if(descriptor==TBT_90_DEGREES_FLY_AROUND){
        RVmanoeuvre->ElementIdentifier()->setName("2TBT90 degrees fly-around");
        RVmanoeuvre->elementName().append("2TBT90 degrees fly-around");

        RVmanoeuvre->setDeltaV1(manPlan.plan[order].tangential->getDeltaV1());
        RVmanoeuvre->setDeltaV2(manPlan.plan[order].tangential->getDeltaV2());


    }

//radial

    if(descriptor==RBT_ALONG_X_AXIS){
        RVmanoeuvre->ElementIdentifier()->setName("2RBTalong x-axis");
        RVmanoeuvre->elementName().append("2RBTalong x-axis");

        RVmanoeuvre->setDeltaV1(manPlan.plan[order].radial->getDeltaV1());
        RVmanoeuvre->setDeltaV2(manPlan.plan[order].radial->getDeltaV2());
    }

    if(descriptor==RBT_90_DEGREES_FLY_AROUND){
        RVmanoeuvre->ElementIdentifier()->setName("2RBT90 degrees radial fly-around");
        RVmanoeuvre->elementName().append("2RBT90 degrees radial fly-around");

        RVmanoeuvre->setDeltaV1(manPlan.plan[order].radial->getDeltaV1());
        RVmanoeuvre->setDeltaV2(manPlan.plan[order].radial->getDeltaV2());
    }

    if(descriptor==RBT_360_DEGREES_FLY_AROUND){
        RVmanoeuvre->ElementIdentifier()->setName("2RBT360 degrees radial fly-around");
        RVmanoeuvre->elementName().append("2RBT360 degrees radial fly-around");

        RVmanoeuvre->setDeltaV1(manPlan.plan[order].radial->getDeltaV1());
        RVmanoeuvre->setDeltaV2(manPlan.plan[order].radial->getDeltaV2());
    }

//constant force

    if(descriptor==CFT_TANGENTIAL){
        RVmanoeuvre->ElementIdentifier()->setName("CFT Tangential");
        RVmanoeuvre->elementName().append("CFT Tangential");

        //CAUTION. There should be a new variable in schema for this.

        RVmanoeuvre->setDeltaV1(manPlan.plan[order].cftTrans->getConstantForce());

    }

    if(descriptor==CFT_RADIAL){
        RVmanoeuvre->ElementIdentifier()->setName("CFT Radial");
        RVmanoeuvre->elementName().append("CFT Radial");

        //CAUTION. There should be a new variable in schema for this.

        RVmanoeuvre->setDeltaV1(manPlan.plan[order].cftTrans->getConstantForce());

    }

}




void RVXMLInterface::setFreeDrift(ScenarioLoiteringType* freeDrift,int order){

    QDateTime time;
    ScenarioKeplerianElementsType* chaserElements = new ScenarioKeplerianElementsType();

    if (order==0)
        {
         time.setDate(userDateTime.date());
         time.setTime(userDateTime.time());

         chaserElements->setSemiMajorAxis(userOrbitalDialogInputs[0]);
         chaserElements->setEccentricity(userOrbitalDialogInputs[1]);
         chaserElements->setInclination(userOrbitalDialogInputs[2]);
         chaserElements->setRAAN(userOrbitalDialogInputs[3]);
         chaserElements->setArgumentOfPeriapsis(userOrbitalDialogInputs[4]);
         chaserElements->setTrueAnomaly(userOrbitalDialogInputs[5]);

        }

    else
        time=manPlan.plan[order-1].endTime;


    // Create the initial attitude (Euler elements)
    ScenarioEulerBIType*  initAtt = new ScenarioEulerBIType();
    initAtt->setPhi(0.00000);
    initAtt->setTheta(0.00000);
    initAtt->setPsi(0.00000);
    initAtt->setPhiDot(0.00000);
    initAtt->setThetaDot(0.00000);
    initAtt->setPsiDot(0.00000);

    // Create the trajectory arc

    freeDrift->ElementIdentifier()->setName("Free Drift");
    freeDrift->ElementIdentifier()->setColorName("Yellow");
    freeDrift->Environment()->CentralBody()->setName("Earth");
    freeDrift->InitialPosition()->setCoordinateSystem("INERTIAL J2000");
    freeDrift->PropagationPosition()->setPropagator("TWO BODY");
    freeDrift->PropagationPosition()->setIntegrator("RK4");
    freeDrift->InitialAttitude()->setCoordinateSystem("EULER 123");
    freeDrift->PropagationAttitude()->setIntegrator("");	// Not defined in STA yet

    freeDrift->PropagationPosition()->setTimeStep(userOrbitalDialogInputs[12]);
    freeDrift->PropagationAttitude()->setTimeStep(userOrbitalDialogInputs[12]);
    freeDrift->TimeLine()->setStepTime(userOrbitalDialogInputs[12]);


    // Time-line

    freeDrift->TimeLine()->setStartTime(time);

     //always a period

   /* QString centralBodyName = freeDrift->Environment()->CentralBody()->Name();
    StaBody* centralBody = STA_SOLAR_SYSTEM->lookup(centralBodyName);*/

     double mu=Earth_GRAVITY_PARAMETER/1000000000.0;

     int a,b,c;
     a=int(2*sta::Pi()*sqrt(pow(userOrbitalDialogInputs[0],3)/mu));
     b=int(userOrbitalDialogInputs[12]);
     c=a%b;


     c=userOrbitalDialogInputs[12]-c;
     freeDrift->TimeLine()->setEndTime(
      time.addSecs(2*sta::Pi()*sqrt(pow(userOrbitalDialogInputs[0],3)/mu)+c));


    //freeDrift->TimeLine()->setEndTime(time.addSecs(2*sta::Pi()*sqrt(pow(userOrbitalDialogInputs[0],3)/mu)));


    freeDrift->InitialPosition()->setAbstract6DOFPosition(QSharedPointer<ScenarioAbstract6DOFPositionType>(chaserElements));
    freeDrift->InitialAttitude()->setAbstract6DOFAttitude(QSharedPointer<ScenarioAbstract6DOFAttitudeType>(initAtt));

    manPlan.plan[order].endTime=freeDrift->TimeLine()->EndTime();


}



void RVXMLInterface::setDeltaV(ScenarioDeltaVType* deltaV,int order){
    QDateTime time;

    if (order==0){
        time.setDate(userDateTime.date());
        time.setTime(userDateTime.time());
    }

    else
        time=manPlan.plan[order-1].endTime;

    deltaV->TimeLine()->setStartTime(time);
    deltaV->setDeltaVx(1);
    deltaV->setDeltaVy(1);
    deltaV->setDeltaVz(1);
    deltaV->setMagnitude(sqrt(3));
    deltaV->TimeLine()->setEndTime(time.addSecs(3));
    deltaV->Environment()->CentralBody()->setName("Earth");
    deltaV->InitialPosition()->setCoordinateSystem("INERTIAL J2000");
    deltaV->ElementIdentifier()->setName("ChaserDeltaV");
    deltaV->ElementIdentifier()->setColorName("Red");
    deltaV->ElementIdentifier()->setModelName("DEFAULT");
    manPlan.plan[order].endTime=deltaV->TimeLine()->EndTime();

}


void RVXMLInterface::newTarget(){
    //Putting target endtime online
    // ******
    // TARGET
    // ******
    // Create the initial position (Keplerian elements)
    ScenarioKeplerianElementsType* targetElements = new ScenarioKeplerianElementsType();

    targetElements->setSemiMajorAxis(userOrbitalDialogInputs[6]);

    targetElements->setEccentricity(userOrbitalDialogInputs[7]);
    targetElements->setInclination(userOrbitalDialogInputs[8]);
    targetElements->setRAAN(userOrbitalDialogInputs[9]);
    targetElements->setArgumentOfPeriapsis(userOrbitalDialogInputs[10]);
    targetElements->setTrueAnomaly(userOrbitalDialogInputs[11]);

    // Create the initial attitude (Euler elements)
    ScenarioEulerBIType*  targetInitAtt = new ScenarioEulerBIType();
    targetInitAtt->setPhi(0.00000);
    targetInitAtt->setTheta(0.00000);
    targetInitAtt->setPsi(0.00000);
    targetInitAtt->setPhiDot(0.00000);
    targetInitAtt->setThetaDot(0.00000);
    targetInitAtt->setPsiDot(0.00000);


    // Create the trajectory arc
    ScenarioLoiteringType* targetLoitering = new ScenarioLoiteringType();
    targetLoitering->ElementIdentifier()->setName("TargetLoitering");
    targetLoitering->ElementIdentifier()->setColorName("White");
    targetLoitering->Environment()->CentralBody()->setName("Earth");
    targetLoitering->InitialPosition()->setCoordinateSystem("INERTIAL J2000");


    targetLoitering->PropagationPosition()->setPropagator("TWO BODY");
    targetLoitering->PropagationPosition()->setIntegrator("RK4");
    targetLoitering->InitialAttitude()->setCoordinateSystem("EULER 123");
    targetLoitering->PropagationAttitude()->setIntegrator("");	// Not defined in STA yet

    targetLoitering->PropagationPosition()->setTimeStep(userOrbitalDialogInputs[12]);
    targetLoitering->PropagationAttitude()->setTimeStep(userOrbitalDialogInputs[12]);
    targetLoitering->TimeLine()->setStepTime(userOrbitalDialogInputs[12]);


    // Time-line
   // QDateTime TheCurrentDateAndTime = QDateTime::currentDateTime(); // Get the current epoch
    targetLoitering->TimeLine()->setStartTime(userDateTime);

    //Setting endingtime in target acording to ending time
    //in last manoeuvre.
    targetLoitering->TimeLine()->setEndTime(manPlan.plan.last().endTime);

    //loitering->TimeLine()->setEndTime(TheCurrentDateAndTime.addDays(1));


    targetLoitering->InitialPosition()->setAbstract6DOFPosition(QSharedPointer<ScenarioAbstract6DOFPositionType>(targetElements));
    targetLoitering->InitialAttitude()->setAbstract6DOFAttitude(QSharedPointer<ScenarioAbstract6DOFAttitudeType>(targetInitAtt));

    // Create the spacecraft loitering arc
    target->SCMission()->TrajectoryPlan()->AbstractTrajectory().append(QSharedPointer<ScenarioAbstractTrajectoryType>(targetLoitering));

}




PropagatedScenario* RVXMLInterface::getPropagatedScenario(){

    return propScenario;
}



//gets propagated manoeuvre at i in spaceObj propagated data
//This handles updating manoeuvre tables.



bool RVXMLInterface::getPropagatedManoeuvreInfoAt(int posInPropagation){

    double startingTime;
    double endingTime;

    double xInertial,vxInertial,xLHLV,vxLocal;
    double yInertial,vyInertial,yLHLV,vyLocal;
    double zInertial,vzInertial,zLHLV,vzLocal;

    double mu=Earth_GRAVITY_PARAMETER/1000000000.0;

    SpaceObject* spaceObj=new SpaceObject(*chaserPropagated);
    SpaceObject* spaceObjTarget=new SpaceObject(*targetPropagated);

    MissionArc* arc=spaceObj->mission().at(posInPropagation);
    MissionArc* arcTarget=spaceObjTarget->mission().first();

    sta::StateVector result;
    sta::StateVector resultTarget;

    startingTime=sta::CalendarToJd(manPlan.plan[posInPropagation].initTime);
    endingTime=sta::CalendarToJd(manPlan.plan[posInPropagation].endTime);

    startingTime=sta::JdToMjd(startingTime);
    endingTime=sta::JdToMjd(endingTime);

    endingTime=endingTime-(1.0/3600.0)/24.0;
    startingTime=startingTime+(1.0/3600.0)/24.0;


    arcTarget->getStateVector(startingTime,&resultTarget);
    arc->getStateVector(startingTime,&result);


    sta::KeplerianElements e =cartesianTOorbital(mu,resultTarget);


    xInertial=result.position.x()-resultTarget.position.x();
    yInertial=result.position.y()-resultTarget.position.y();
    zInertial=result.position.z()-resultTarget.position.z();

    vxInertial=result.velocity.x()-resultTarget.velocity.x();
    vyInertial=result.velocity.y()-resultTarget.velocity.y();
    vzInertial=result.velocity.z()-resultTarget.velocity.z();

    inertialTOlocal(xInertial,yInertial, zInertial,
                    e.Inclination, e.AscendingNode,
                    e.TrueAnomaly,e.ArgumentOfPeriapsis,
                    xLHLV,yLHLV,zLHLV);

    inertialTOlocal(vxInertial,vyInertial, vzInertial,
                    e.Inclination, e.AscendingNode,
                    e.TrueAnomaly,e.ArgumentOfPeriapsis,
                    vxLocal,vyLocal,vzLocal);





    result.position.x()=xLHLV;
    result.position.y()=yLHLV;
    result.position.z()=zLHLV;

    result.velocity.x()=vxLocal;
    result.velocity.y()=vyLocal;
    result.velocity.z()=vzLocal;

    ScenarioStateVectorType* auxVector=new ScenarioStateVectorType();

    auxVector->setX(xLHLV);
    auxVector->setY(yLHLV);
    auxVector->setZ(zLHLV);

    auxVector->setVx(vxLocal);
    auxVector->setVy(vyLocal);
    auxVector->setVz(vzLocal);

    manPlan.plan[posInPropagation].setInitPosVel(auxVector);

    //End Time

    arc->getStateVector(endingTime,&result);

    arcTarget->getStateVector(endingTime,&resultTarget);


    e =cartesianTOorbital(mu,resultTarget);




    xInertial=result.position.x()-resultTarget.position.x();
    yInertial=result.position.y()-resultTarget.position.y();
    zInertial=result.position.z()-resultTarget.position.z();


    inertialTOlocal(xInertial,yInertial, zInertial,
                    e.Inclination, e.AscendingNode,
                    e.TrueAnomaly,e.ArgumentOfPeriapsis,
                    xLHLV,yLHLV,zLHLV);

    inertialTOlocal(vxInertial,vyInertial, vzInertial,
                    e.Inclination, e.AscendingNode,
                    e.TrueAnomaly,e.ArgumentOfPeriapsis,
                    vxLocal,vyLocal,vzLocal);




       /* xLHLV=(cos(phi)*cos(Omega) - sin(Omega)*cos(i)*sin(phi))*result->position.x()+
              (cos(phi)*sin(Omega) + cos(Omega)*cos(i)*sin(phi))*result->position.y()+
              (sin(i)*sin(phi))*result->position.z();

        yLHLV=(- sin(phi)*cos(Omega) - sin(Omega)*cos(i)*cos(phi))*result->position.x()+
              (- sin(phi)*sin(Omega) + cos(Omega)*cos(i)*cos(phi))*result->position.y()+
              (sin(i)*cos(phi))*result->position.z();

        zLHLV=( sin(i)*sin(Omega))*result->position.x()+
              ( - sin(i)*cos(Omega))*result->position.y()+
              cos(i)*result->position.z();*/


 /*   if(xLHLV<0.00000001) xLHLV=0.0;
    if(yLHLV<0.00000001) yLHLV=0.0;
    if(zLHLV<0.00000001) zLHLV=0.0;*/


    result.velocity.x()=vxLocal;
    result.velocity.y()=vyLocal;
    result.velocity.z()=vzLocal;


    auxVector->setX(xLHLV);
    auxVector->setY(yLHLV);
    auxVector->setZ(zLHLV);
    auxVector->setVx(result.velocity.x());
    auxVector->setVy(result.velocity.y());
    auxVector->setVz(result.velocity.z());

    manPlan.plan[posInPropagation].setEndPosVel(auxVector);

}





QString RVXMLInterface::getPropagatedTrajectory(int order){
    //This method is a temporal fix to take out data from propagated
    //scenario

    if (order==3){
        SpaceObject* spaceObj=new SpaceObject(*targetPropagated);

        MissionArc* arc=new MissionArc(*spaceObj->mission().first());

        sta::StateVector* result=new sta::StateVector();
        Eigen::Vector3d vector;
        double x,y,z;

        QString temp1,temp2,temp3;
        //CAUTION with sample time. Where is it?

        for (double i=arc->beginning();i<arc->ending();i=i+((userOrbitalDialogInputs[12]/3600.0)/24.0))
        {

            char f;
            arc->getStateVector(i,result);
            vector=result->position;
            z=vector.z();
            y=vector.y();
            x=vector.x();
            temp1.setNum(i,f,8);
            temp1.setNum(x);
            temp1.append(" -- ");
            temp2.setNum(i,f,8);
            temp2.setNum(y);

            temp1.append(temp2);
            temp1.append(" -- ");
            temp2.setNum(i,f,8);
            temp2.setNum(z);
            temp1.append(temp2);

            temp1.append("  ---  ");

            temp2.setNum(i,f,6);

            temp1.append(temp2);

            temp3.append(temp1);
            temp3.append("\n");



        }

    return temp3;

    }else
    {

       SpaceObject* spaceObj=new SpaceObject(*chaserPropagated);

       MissionArc* arc=new MissionArc(*spaceObj->mission().at(order));



    sta::StateVector* result=new sta::StateVector();
    Eigen::Vector3d vector;
    double x,y,z;

    QString temp1,temp2,temp3;
    //CAUTION with sample time. Where is it?

    for (double i=arc->beginning();i<arc->ending();i=i+((userOrbitalDialogInputs[12]/3600.0)/24.0))
    {
        char f;

        arc->getStateVector(i,result);
        vector=result->position;
        z=vector.z();
        y=vector.y();
        x=vector.x();

        temp1.setNum(i,f,8);
        temp1.setNum(x);
        temp1.append(" -- ");

        temp2.setNum(i,f,8);
        temp2.setNum(y);

        temp1.append(temp2);
        temp1.append(" -- ");

        temp2.setNum(i,f,8);
        temp2.setNum(z);
        temp1.append(temp2);


        temp1.append("  ---  ");
        temp2.setNum(i,f,6);

        temp1.append(temp2);

        temp3.append(temp1);
        temp3.append("\n");


    }

return temp3;
}
}

//Sets user inputs in XMLInterface

void RVXMLInterface::setUserOrbitalDialogInputs(int i,double in){
    userOrbitalDialogInputs[i]=in;

}

void RVXMLInterface::setUserDateTime(QDateTime dateTime){
    userDateTime.setDate(dateTime.date());
    userDateTime.setTime(dateTime.time());
}

