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
   loadRVScenario.- Gets data from STA mainwindow and XML structure SpaceScenario and
                    PropagatedScenario if available.
                    It sets also InterfXML.manPlan if previous manoeuvres detected
   ---------------------------------------------------------------------------------*/

bool RVdiagXMLinterface::loadRVScenario(SpaceScenario* scen,PropagatedScenario* prop){

    //First it checks if there's a scenario

    if (scen == NULL) return false;

    //Ok, there's scenario. Checking for RVScenarios only.
    if(scen->Name().compare("RendezVous Scenario")==0){

        //It's a RendezVous scenario.
        scenario=new SpaceScenario(*scen);

        //Chaser and target retrieval.

        QSharedPointer <ScenarioParticipantType> participant(scenario->AbstractParticipant().last());
        chaser=dynamic_cast<ScenarioSC*>(participant.data());
        QSharedPointer <ScenarioParticipantType> participant2(scenario->AbstractParticipant().first());
        target=dynamic_cast<ScenarioSC*>(participant2.data());

        //Cleaning manPlan before adding new manoeuvres
        if(!manPlan.plan.isEmpty())
            manPlan.plan.clear();

        //Setting up manPlan if manoeuvres detected from previos data in XML.

        if(!chaser->SCMission()->TrajectoryPlan()->AbstractTrajectory().isEmpty())
        {
            //Loop around all chaser manoeuvres.
            for (int i=0;i<chaser->SCMission()->TrajectoryPlan()->AbstractTrajectory().size();i++)
            {
                //Extracting name and preparing descriptor for manPlan
                RVmanoeuvreDescriptor *RVman=new RVmanoeuvreDescriptor();

                RVman->fullName.append(chaser->SCMission()->TrajectoryPlan()->AbstractTrajectory().at(i)->elementName());

                //Needed extra checking in lotering case
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
                    {
                        RVman->setDescriptor(TBT_ALONG_X_AXIS);
                        RVman->setName("2TBTalong x-axis");
                    }

                    if(name.compare("2TBTHohmann Transfer")==0)
                    {
                        RVman->setDescriptor(TBT_HOHMANN_TRANSFER);
                        RVman->setName("2TBTHohmann Transfer");
                    }
                    if(name.compare("2TBT90 degrees fly-around")==0)
                    {
                        RVman->setDescriptor(TBT_90_DEGREES_FLY_AROUND);
                        RVman->setName("2TBT90 degrees fly-around");
                    }
                    if(name.compare("2RBTalong x-axis")==0)
                    {
                        RVman->setDescriptor(RBT_ALONG_X_AXIS);
                        RVman->setName("2RBTalong x-axis");
                    }
                    if(name.compare("2RBT90 degrees radial fly-around")==0)
                    {
                        RVman->setDescriptor(RBT_90_DEGREES_FLY_AROUND);
                        RVman->setName("2RBT90 degrees radial fly-around");
                    }
                    if(name.compare("2RBT360 degrees radial fly-around")==0)
                    {
                        RVman->setDescriptor(RBT_360_DEGREES_FLY_AROUND);
                        RVman->setName("2RBT360 degrees radial fly-around");
                    }
                    if(name.compare("Hold Points outside V-bar")==0)
                    {
                        RVman->setDescriptor(HOLD_POINTS_VBAR);
                        RVman->setName("Hold Points outside V-bar");
                    }
                    if(name.compare("CFT Tangential")==0)
                    {
                        RVman->setDescriptor(CFT_TANGENTIAL);
                        RVman->setName("CFT Tangential");
                    }
                    if(name.compare("CFT Radial")==0)
                    {
                        RVman->setDescriptor(CFT_RADIAL);
                        RVman->setName("CFT Radial");
                    }
                    if(name.compare("SLT V-bar")==0)
                    {
                        RVman->setDescriptor(SLT_VBAR);
                        RVman->setName("SLT V-bar");
                    }
                    if(name.compare("SLT R-bar")==0)
                    {
                        RVman->setDescriptor(SLT_RBAR);
                        RVman->setName("SLT R-bar");
                    }
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
                    RVman->setDeltaV1(rvManoeuvre->DeltaV1());
                    RVman->setDeltaV2(rvManoeuvre->DeltaV2());

                    if(name.compare("Hold Points outside V-bar")==0 ||
                       name.compare("SLT R-bar")==0 ||
                       name.compare("SLT V-bar")==0 ||
                       name.compare("CFT Radial")==0 ||
                       name.compare("CFT Tangential")==0 )
                        RVman->setTotalDeltaV(5.6);
                    else
                        RVman->setTotalDeltaV(fabs(RVman->getDeltaV1())+fabs(RVman->getDeltaV2()));

                    name.clear();
                }
                manPlan.plan.append(*RVman);
             }
        }

        //Next thing to check is propagated scenario.
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
   createRVscenario.- Each time generate Scenario button is activated, or OK button
                pressed, available manPlan should update SpaceScenario. This method
                reserves memory and gives acces to interfXML to make it so.
                Note Dialog calls this method and passes mainwindow
                data. No other way to do so, because Dialog is the only object which
                can see mainwindow.
   ---------------------------------------------------------------------------------*/


void RVdiagXMLinterface::createRVscenario(SpaceScenario* scen,ScenarioSC* chas,ScenarioSC* targ,
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
    target->setName("Target");
    target->ElementIdentifier()->setName("Target");

    propScenario=new PropagatedScenario(*prop);
    //Needed for endingtime settings.
    newChaser();
    newTarget();
    //Target first, chaser last.
    scenario->AbstractParticipant().append(QSharedPointer<ScenarioParticipantType>(target));
    scenario->AbstractParticipant().append(QSharedPointer<ScenarioParticipantType>(chaser));
}



SpaceScenario* RVdiagXMLinterface::getScenario(){
    return scenario;
}

ScenarioSC* RVdiagXMLinterface::getChaser(){
    return chaser;
}

ScenarioSC* RVdiagXMLinterface::getTarget(){
    return target;
}

PropagatedScenario* RVdiagXMLinterface::getPropagatedScenario(){
    return propScenario;
}



/* ---------------------------------------------------------------------------------
    newChaser.- It makes sure no previous manoeuvres are there.
   ---------------------------------------------------------------------------------*/

void RVdiagXMLinterface::newChaser(){

    if(manPlan.plan.isEmpty()){
        setManoeuvrePlan();
    }
    else{
        manPlan.plan.clear();
        setManoeuvrePlan();
    }
}


/* ---------------------------------------------------------------------------------
    setManoeuvrePlan.- Sets manoeuvre plan in chaser SpaceScenario XML structure.
                       To do so, it gets data from every sub manPlan. Close range
                       is the only sub manPlan for now.
   ---------------------------------------------------------------------------------*/


void RVdiagXMLinterface::setManoeuvrePlan(){

    manPlan.plan.append(closeRangePlan.plan);

    //For all manPlan elements, cheking must be done


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

               }else if(manPlan.plan[i].getDescriptor()>DELTA_V) {
                ScenarioRendezVousManoeuvreType* genericRendezVous=new ScenarioRendezVousManoeuvreType();
                setManoeuvre(i,genericRendezVous);

                chaser->SCMission()->TrajectoryPlan()->AbstractTrajectory().append(QSharedPointer<ScenarioAbstractTrajectoryType>(genericRendezVous));

               }else{
                //DEFAULT
                //No descriptor means no manoeuvre.
                //Needed to erase in order not confusing target
                manPlan.plan.removeAt(i);
               }

        }


}


void RVdiagXMLinterface::setManoeuvre(int order,ScenarioRendezVousManoeuvreType* RVmanoeuvre){


    double radius=userOrbitalDialogInputs[0];
    double input=manPlan.plan[order].getRVinput();



    RVmanoeuvre->setInput(input*1000.0);


    QDateTime time;
    if(order==0){
        time.setDate(userDateTime.date());
        time.setTime(userDateTime.time());}
    else

    time=manPlan.plan[order-1].endTime;
    //All manoeuvres are the same at this point
    ScenarioKeplerianElementsType* elements = new ScenarioKeplerianElementsType();
    ScenarioEulerType*  initAtt = new ScenarioEulerType();
    initAtt->setPhi(0.00000);
    initAtt->setTheta(0.00000);
    initAtt->setPsi(0.00000);
    initAtt->setPhiDot(0.00000);
    initAtt->setThetaDot(0.00000);
    initAtt->setPsiDot(0.00000);

    RVmanoeuvre->ElementIdentifier()->setColorName("Green");

    RVmanoeuvre->ElementIdentifier()->setModelName(chaserModelName);

    RVmanoeuvre->Environment()->CentralBody()->setName(centralBodyName);


    StaBody* centralBody = STA_SOLAR_SYSTEM->lookup(centralBodyName);


    if (!centralBody)
    {
        qWarning("Bad central Body at setManoeuvre");
    }
    double mu=centralBody->mu();

    RVmanoeuvre->InitialPosition()->setCoordinateSystem("INERTIAL J2000");
    RVmanoeuvre->PropagationPosition()->setTimeStep(userOrbitalDialogInputs[12]);
    RVmanoeuvre->PropagationPosition()->setPropagator("TWO BODY");
    RVmanoeuvre->PropagationPosition()->setIntegrator("RK4");
    RVmanoeuvre->InitialAttitude()->setCoordinateSystem("EULER 123");
    RVmanoeuvre->PropagationAttitude()->setIntegrator("");	// Not defined in STA yet
    RVmanoeuvre->PropagationAttitude()->setTimeStep(userOrbitalDialogInputs[12]);
    RVmanoeuvre->TimeLine()->setStepTime(userOrbitalDialogInputs[12]);

    //Duration. It deppends on manoeuvre.
    //A complete period.
    if(manPlan.plan[order].descriptor==TBT_ALONG_X_AXIS ||
       manPlan.plan[order].descriptor==RBT_360_DEGREES_FLY_AROUND||
       manPlan.plan[order].descriptor==CFT_TANGENTIAL ||
       manPlan.plan[order].descriptor==CFT_RADIAL)

        manPlan.plan[order].setDurationInSecs(int(2*sta::Pi()*sqrt(pow(radius,3)/mu)));

    //Half a period
    if(manPlan.plan[order].descriptor==TBT_HOHMANN_TRANSFER ||
       manPlan.plan[order].descriptor==TBT_90_DEGREES_FLY_AROUND||
       manPlan.plan[order].descriptor==RBT_ALONG_X_AXIS ||
       manPlan.plan[order].descriptor==CFT_TANGENTIAL)

        manPlan.plan[order].setDurationInSecs(int(2*sta::Pi()*sqrt(pow(radius,3)/mu)/2));

    //A quarter
    //CFT transfers here, but duration must be also an input
    if(manPlan.plan[order].descriptor==RBT_90_DEGREES_FLY_AROUND ||
       manPlan.plan[order].descriptor==SLT_VBAR)

        manPlan.plan[order].setDurationInSecs(int(2*sta::Pi()*sqrt(pow(radius,3)/mu)/4));

    //1/8
    if(manPlan.plan[order].descriptor==HOLD_POINTS_VBAR ||
       manPlan.plan[order].descriptor==SLT_RBAR)

        manPlan.plan[order].setDurationInSecs(int(2*sta::Pi()*sqrt(pow(radius,3)/mu)/4)/2);

    RVmanoeuvre->setDuration(manPlan.plan[order].getDurationInSecs());

    //This lines are in charge of setting endTime, according to step simulation.
    //In this way target and chaser have exactly the same JD's in every simulation step
    //This is important because simulation data in chaser deppends on target

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

    //NAME
    RVmanoeuvre->ElementIdentifier()->setName(manPlan.plan[order].getName());
    RVmanoeuvre->elementName().append(manPlan.plan[order].getName());


}




void RVdiagXMLinterface::setFreeDrift(ScenarioLoiteringType* freeDrift,int order){

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
    ScenarioEulerType*  initAtt = new ScenarioEulerType();
    initAtt->setPhi(0.00000);
    initAtt->setTheta(0.00000);
    initAtt->setPsi(0.00000);
    initAtt->setPhiDot(0.00000);
    initAtt->setThetaDot(0.00000);
    initAtt->setPsiDot(0.00000);

    // Create the trajectory arc

    freeDrift->ElementIdentifier()->setName(manPlan.plan[order].getName());
    freeDrift->elementName().append(manPlan.plan[order].getName());

    freeDrift->ElementIdentifier()->setColorName("Yellow");

    freeDrift->ElementIdentifier()->setModelName(chaserModelName);


    freeDrift->Environment()->CentralBody()->setName(centralBodyName);


    StaBody* centralBody = STA_SOLAR_SYSTEM->lookup(centralBodyName);

    if (!centralBody)
    {
        qWarning("Bad centralbody at setFreeDrift");

    }
    double mu=centralBody->mu();

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


    //This lines are in charge of setting endTime, according to step simulation.
    //In this way target and chaser have exactly the same JD's in every simulation step
    //This is important because simulation data in chaser deppends on target

     int a,b,c;
     a=int(2*sta::Pi()*sqrt(pow(userOrbitalDialogInputs[0],3)/mu));
     b=int(userOrbitalDialogInputs[12]);
     c=a%b;
     c=userOrbitalDialogInputs[12]-c;
     freeDrift->TimeLine()->setEndTime(
      time.addSecs(2*sta::Pi()*sqrt(pow(userOrbitalDialogInputs[0],3)/mu)+c));

    freeDrift->InitialPosition()->setAbstract6DOFPosition(QSharedPointer<ScenarioAbstract6DOFPositionType>(chaserElements));
    freeDrift->InitialAttitude()->setAbstract6DOFAttitude(QSharedPointer<ScenarioAbstract6DOFAttitudeType>(initAtt));

    manPlan.plan[order].endTime=freeDrift->TimeLine()->EndTime();
    manPlan.plan[order].setRVinput(0.0);


}



void RVdiagXMLinterface::setDeltaV(ScenarioDeltaVType* deltaV,int order){
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
    deltaV->TimeLine()->setEndTime(time.addSecs(1));

    deltaV->ElementIdentifier()->setModelName(chaserModelName);

    deltaV->Environment()->CentralBody()->setName(centralBodyName);
    deltaV->InitialPosition()->setCoordinateSystem("INERTIAL J2000");

    deltaV->ElementIdentifier()->setName(manPlan.plan[order].getName());
    deltaV->elementName().append(manPlan.plan[order].getName());

    deltaV->ElementIdentifier()->setColorName("Red");
    manPlan.plan[order].endTime=deltaV->TimeLine()->EndTime();

}


void RVdiagXMLinterface::newTarget(){

    // Create the initial position (Keplerian elements)
    ScenarioKeplerianElementsType* targetElements = new ScenarioKeplerianElementsType();
    targetElements->setSemiMajorAxis(userOrbitalDialogInputs[6]);
    targetElements->setEccentricity(userOrbitalDialogInputs[7]);
    targetElements->setInclination(userOrbitalDialogInputs[8]);
    targetElements->setRAAN(userOrbitalDialogInputs[9]);
    targetElements->setArgumentOfPeriapsis(userOrbitalDialogInputs[10]);
    targetElements->setTrueAnomaly(userOrbitalDialogInputs[11]);

    // Create the initial attitude (Euler elements)
    ScenarioEulerType*  targetInitAtt = new ScenarioEulerType();
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


    targetLoitering->ElementIdentifier()->setModelName(targetModelName);
    targetLoitering->Environment()->CentralBody()->setName(centralBodyName);

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







//gets propagated manoeuvre at i in spaceObj propagated data
//This handles updating manoeuvre tables.

/* ---------------------------------------------------------------------------------
    getPropagatedManoeuvreInfoAt.- It gets data from propagatedScenario, in order
                        to display information in RV Wizard. Every arc is a propagated
                        manoeuvre.
   ---------------------------------------------------------------------------------*/



bool RVdiagXMLinterface::getPropagatedManoeuvreInfoAt(int posInPropagation){

    double startingTime;
    double endingTime;


    //Accesing propagated data

    SpaceObject* spaceObj=new SpaceObject(*chaserPropagated);
    SpaceObject* spaceObjTarget=new SpaceObject(*targetPropagated);

    MissionArc* arc=spaceObj->mission().at(posInPropagation);
    MissionArc* arcTarget=spaceObjTarget->mission().first();

    double mu=arc->centralBody()->mu();

    //Time calculation.

    sta::StateVector chaserStateVector;
    sta::StateVector targetStateVector;

    startingTime=sta::CalendarToJd(manPlan.plan[posInPropagation].initTime);
    endingTime=sta::CalendarToJd(manPlan.plan[posInPropagation].endTime);

    startingTime=sta::JdToMjd(startingTime);
    endingTime=sta::JdToMjd(endingTime);

    //Accesing last propagated data and first is done with other methods. Using interpolation
    //give us accurate data anyway, so starting time is actually a second later of starting time
    //and ending time is a second before ending.

    endingTime=endingTime-(1.0/3600.0)/24.0;
    startingTime=startingTime+(1.0/3600.0)/24.0;

    //Getting data into stateVectors for initial start Time.

    arcTarget->getStateVector(startingTime,&targetStateVector);
    arc->getStateVector(startingTime,&chaserStateVector);

    sta::StateVector* lhlvRelative=new sta::StateVector;

    chaserStateVector.position(0)=chaserStateVector.position(0)-targetStateVector.position(0);
    chaserStateVector.position(1)=chaserStateVector.position(1)-targetStateVector.position(1);
    chaserStateVector.position(2)=chaserStateVector.position(2)-targetStateVector.position(2);

    chaserStateVector.velocity(0)=chaserStateVector.velocity(0)-targetStateVector.velocity(0);
    chaserStateVector.velocity(1)=chaserStateVector.velocity(1)-targetStateVector.velocity(1);
    chaserStateVector.velocity(2)=chaserStateVector.velocity(2)-targetStateVector.velocity(2);


    inertialTOlhlv(mu,targetStateVector,chaserStateVector,lhlvRelative);

    manPlan.plan[posInPropagation].setInitPosVel(lhlvRelative);

    //Getting data into stateVectors for final end Time.

    arc->getStateVector(endingTime,&chaserStateVector);
    arcTarget->getStateVector(endingTime,&targetStateVector);

    chaserStateVector.position(0)=chaserStateVector.position(0)-targetStateVector.position(0);
    chaserStateVector.position(1)=chaserStateVector.position(1)-targetStateVector.position(1);
    chaserStateVector.position(2)=chaserStateVector.position(2)-targetStateVector.position(2);

    chaserStateVector.velocity(0)=chaserStateVector.velocity(0)-targetStateVector.velocity(0);
    chaserStateVector.velocity(1)=chaserStateVector.velocity(1)-targetStateVector.velocity(1);
    chaserStateVector.velocity(2)=chaserStateVector.velocity(2)-targetStateVector.velocity(2);

    inertialTOlhlv(mu,targetStateVector,chaserStateVector,lhlvRelative);


    manPlan.plan[posInPropagation].setEndPosVel(lhlvRelative);

}


//Sets user inputs in XMLInterface

void RVdiagXMLinterface::setUserOrbitalDialogInputs(int i,double in){
    userOrbitalDialogInputs[i]=in;

}

void RVdiagXMLinterface::setUserDateTime(QDateTime dateTime){
    userDateTime.setDate(dateTime.date());
    userDateTime.setTime(dateTime.time());
}


void RVdiagXMLinterface::setUserArcAspect(QString targetModel,QString chaserModel,QString centralBody){
    if(!targetModelName.isEmpty()){
        targetModelName.clear();
    }
    targetModelName.append(targetModel);

    if(!chaserModelName.isEmpty()){
        chaserModelName.clear();
    }
    chaserModelName.append(chaserModel);

    if(!centralBody.isEmpty()){
        centralBodyName.clear();
    }
    centralBodyName.append(centralBody);



}

QList<double> RVdiagXMLinterface::getInitialDialogData(){

    QList <double> list;
 double argumentOfPeriapsis;
 double ascendingNode;
 double inclination;
 double trueAnomaly;
 double meanAnomaly;

    sta::KeplerianElements keplerianData;

    sta::StateVector* auxVectorState=new sta::StateVector();

    //Chaser first

    propScenario->spaceObjects()
                .at(0)->mission().at(0)->getStateVector
                (propScenario->spaceObjects().at(0)->mission().at(0)
                 ->beginning(),auxVectorState);

        keplerianData = cartesianTOorbital(propScenario->spaceObjects()
                      .at(0)->mission().at(0)->centralBody()->mu(), *auxVectorState);

        argumentOfPeriapsis=sta::radToDeg(keplerianData.ArgumentOfPeriapsis);
        ascendingNode=sta::radToDeg(keplerianData.AscendingNode);
        inclination=sta::radToDeg(keplerianData.Inclination);
        trueAnomaly=sta::radToDeg(keplerianData.TrueAnomaly);
        meanAnomaly=sta::radToDeg(keplerianData.MeanAnomaly);


        list.append(keplerianData.SemimajorAxis);
        list.append(keplerianData.Eccentricity);
        list.append(inclination);
        list.append(ascendingNode);
        list.append(argumentOfPeriapsis);
        list.append(trueAnomaly);


        //Chaser

        propScenario->spaceObjects()
                .at(1)->mission().at(0)->getStateVector
                (propScenario->spaceObjects().at(0)->mission().at(0)
                 ->beginning(),auxVectorState);

        keplerianData = cartesianTOorbital(propScenario->spaceObjects()
                      .at(0)->mission().at(0)->centralBody()->mu(), *auxVectorState);

        argumentOfPeriapsis=sta::radToDeg(keplerianData.ArgumentOfPeriapsis);
        ascendingNode=sta::radToDeg(keplerianData.AscendingNode);
        inclination=sta::radToDeg(keplerianData.Inclination);
        trueAnomaly=sta::radToDeg(keplerianData.TrueAnomaly);
        meanAnomaly=sta::radToDeg(keplerianData.MeanAnomaly);

        list.append(keplerianData.SemimajorAxis);
        list.append(keplerianData.Eccentricity);
        list.append(inclination);
        list.append(ascendingNode);
        list.append(argumentOfPeriapsis);
        list.append(trueAnomaly);

    return list;


}
