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
 ------------------ Author: Guillermo Ortega  -------------------------------------------------
 ------------------ European Space Agency ----------------------------
 ------------------ Modified: J. Alonso     ------------------------------------------------    ---
 ------------------ ETSIA                 ----------------------------
 Comments:
 Dialog cpp file. Modified in order to complain Ordovican specs.
 */


#include "rendezVousDialog.h"

#include <QMessageBox>

RendezVousDialog::RendezVousDialog(SpaceScenario* scenario,PropagatedScenario*
                          propScenario,MainWindow* parent) : mainwindow(parent)
{
        int i;
        setupUi(this);

  //In this method, propagated scenario is loaded.

 // InitialDataSetup.

 setupTargetChaserInitialData(NEW_ORBITAL_DATA_REQUIRED);
 updateUserOrbitalInput();

  if(interfXML.loadRVScenario(scenario,propScenario)){

      /*if (!interfXML.manPlan.plan.isEmpty())
      {
          for(i=0;i<interfXML.manPlan.plan.size();i++)
           // this->testTextEditTarget->append(interfXML.manPlan.plan[i].getName());
      }*/

      //If there's scenario, initial data must be the same

      setupTargetChaserInitialData(PREVIOUS_ORBITAL_DATA_PRESENT);
      updateUserOrbitalInput();

      //this->generatePB->setDisabled(true);
      this->testTextEditChaser->append(interfXML.getPropagatedTrajectory(0));
      this->testTextEditChaser->append(interfXML.getPropagatedTrajectory(1));
      this->testTextEditChaser->append(interfXML.getPropagatedTrajectory(2));

      this->testTextEditTarget->append(interfXML.getPropagatedTrajectory(3));

      //In case propagation data is available, manPlan must be updated

      //Just with propagation data
      if (!interfXML.manPlan.plan.isEmpty())
      {

      for (i=0;i<interfXML.manPlan.plan.size();i++)
            {
            //RVManoeuvreDescriptor *temp=new RVManoeuvreDescriptor(interfXML.manPlan.plan.at(i));
            //Checking LHLV


            interfXML.getPropagatedManoeuvreInfoAt(i);
            }
      }


    }
    else{
    //So, no scenario is loaded. There isn't there or just it's another kind of scenario
      //Need for generate a new one.

        this->generatePB->setEnabled(true);

        //for(int i=0;i<interfXML.manPlan.plan.size();i++)
        // this->testTextEditChaser->append(interfXML.manPlan.plan[i].getName());

    }





  // Connect
  connect(generatePB, SIGNAL(pressed()),this,SLOT(generateScenario()));
  connect(propagatePB, SIGNAL(pressed()),this,SLOT(propagate()));


  connect(pushButtonAspect, SIGNAL(pressed()),this,SLOT(initAspectRV()));

  //manPlan update.

  //this->phasingManTable->updateTable(interfXML.manPlan.plan);

  this->closeManTable->updateTable(interfXML.manPlan.plan);

   //ASPECT DATA
   rendezVousAspect=new missionAspectDialog(this);
   rendezVousAspect->loadValueArcName("RendezVous");
   rendezVousAspect->loadValueArcColor("grey");
   rendezVousAspect->loadValueArcModel("");

}

RendezVousDialog::~RendezVousDialog()
{

}


void RendezVousDialog::updateUserOrbitalInput(){

    userOrbitalInput[0]=this->RadiusChaserEdit->text().toDouble();
    userOrbitalInput[1]=this->EccentricityChaserEdit->text().toDouble();
    userOrbitalInput[2]=this->InclinationChaserEdit->text().toDouble();
    userOrbitalInput[3]=this->RAANChaserEdit->text().toDouble();
    userOrbitalInput[4]=this->ArgumentPeriapChaserEdit->text().toDouble();
    userOrbitalInput[5]=this->TrueAnomalyChaserEdit->text().toDouble();

    userOrbitalInput[6]=this->RadiusTargetEdit->text().toDouble();
    userOrbitalInput[7]=this->EccentricityTargetEdit->text().toDouble();
    userOrbitalInput[8]=this->InclinationTargetEdit->text().toDouble();
    userOrbitalInput[9]=this->RAANTargetEdit->text().toDouble();
    userOrbitalInput[10]=this->ArgumentPeriapTargetEdit->text().toDouble();
    userOrbitalInput[11]=this->TrueAnomalyTargetEdit->text().toDouble();

    userOrbitalInput[12]=this->PropStepEdit->text().toDouble();

    interfXML.setUserDateTime(this->InitialEpochTargetEdit->dateTime());

    for (int i=0;i<13;i++){
        interfXML.setUserOrbitalDialogInputs(i,userOrbitalInput[i]);
    }

}


void RendezVousDialog::generateScenario(){
    // Create a new scenario
    SpaceScenario* scenario=new SpaceScenario();
    ScenarioSC* chaser=new ScenarioSC();
    ScenarioSC* target=new ScenarioSC();
    PropagatedScenario* propScenario=new PropagatedScenario();

    //UPDATE for close range man list.

    //this->phasingManTable->updateManoeuvreList();
    this->closeManTable->updateManoeuvreList();
    //To interface with XML

    //interfXML.phasingPlan.plan.append(this->phasingManTable->manoeuvreList);
    interfXML.closeRangePlan.plan.append(this->closeManTable->manoeuvreList);

    //UPDATE userOrbitalInputs
    updateUserOrbitalInput();

    interfXML.createRVscenario(scenario,chaser,target,propScenario);

    mainwindow->setScenario(interfXML.getScenario());

    //Needed for cleaning previous data.
    //mainwindow->setPropagatedScenario(NULL);

    //Cleaning data

    //interfXML.phasingPlan.plan.clear();

    interfXML.closeRangePlan.plan.clear();
    this->closeManTable->manoeuvreList.clear();


    //this->phasingManTable->manoeuvreList.clear();
    return;

}

void RendezVousDialog::propagate(){
    //Set to disabled until propagation policy stablished.

    /*PropagatedScenario* propScenario = new PropagatedScenario();
    interfXML.setPropagatedScenario(propScenario);*/

    PropagationFeedback feedbackChaser,feedbackTarget;

    PropagatedScenario* mPropagatedScenario=new PropagatedScenario(*(interfXML.getPropagatedScenario()));
    //Chaser propagation
    ScenarioSC* mChaser=interfXML.getChaser();

    scenarioPropagatorSatellite(mChaser, feedbackChaser, mPropagatedScenario, this);

    //Target propagation
    ScenarioSC* mTarget=interfXML.getTarget();
    scenarioPropagatorSatellite(mTarget, feedbackTarget,mPropagatedScenario, this);

    if ( (feedbackChaser.status() != PropagationFeedback::PropagationOk) &&
         (feedbackTarget.status() != PropagationFeedback::PropagationOk))
    {
        QMessageBox::critical(this, tr("Propagation Error."), tr("Propagation failed: %1").arg(feedbackChaser.errorString()));
        QMessageBox::critical(this, tr("Propagation Error."), tr("Propagation failed: %1").arg(feedbackTarget.errorString()));
        //delete propScenario;
        //setPropagatedScenario works in private.
        //mainwindow->setPropagatedScenario(NULL);
    }
    else
    {
       // mainwindow->setPropagatedScenario(interfXML.getPropagatedScenario());

    }

    //TEMPORAL 0 for chaser, 1 for target


    this->testTextEditChaser->append(interfXML.getPropagatedTrajectory(0));

    this->testTextEditTarget->append(interfXML.getPropagatedTrajectory(1));
}




void RendezVousDialog::addDeltaV(){
    RVManoeuvreDescriptor m1;
    m1.descriptor=DELTA_V;
    interfXML.manPlan.plan.append(m1);
    generateScenario();


}

void RendezVousDialog::addFreeDrift(){
    RVManoeuvreDescriptor m1;
    m1.descriptor=FREE_DRIFT;
    interfXML.manPlan.plan.append(m1);
    generateScenario();
}


void RendezVousDialog::setupTargetChaserInitialData(int origin){

    // Set up the input validators
    QDoubleValidator* doubleValidator = new QDoubleValidator(this);

    QDoubleValidator* angleValidator = new QDoubleValidator(this);
    angleValidator->setBottom(0.0);
    angleValidator->setTop(360.0);

    QDoubleValidator* positiveDoubleValidator = new QDoubleValidator(this);
    positiveDoubleValidator->setBottom(0.0);

    QDoubleValidator* zeroToOneValidator = new QDoubleValidator(this);
    zeroToOneValidator->setBottom(0.0);
    zeroToOneValidator->setTop(0.9999);

    QDoubleValidator* minusOneToOneValidator = new QDoubleValidator(this);
    minusOneToOneValidator->setBottom(-1.0);
    minusOneToOneValidator->setTop(1.0);

    QDoubleValidator* secondsValidator=new QDoubleValidator(this);
    secondsValidator->setBottom(1.0);
    secondsValidator->setTop(120.0);

    this->RadiusTargetEdit->setValidator(doubleValidator);
    this->EccentricityTargetEdit->setValidator(zeroToOneValidator);
    this->InclinationTargetEdit->setValidator(angleValidator);
    this->RAANTargetEdit->setValidator(angleValidator);
    this->ArgumentPeriapTargetEdit->setValidator(angleValidator);
    this->TrueAnomalyTargetEdit->setValidator(angleValidator);

    this->RadiusChaserEdit->setValidator(doubleValidator);
    this->EccentricityChaserEdit->setValidator(zeroToOneValidator);
    this->InclinationChaserEdit->setValidator(angleValidator);
    this->RAANChaserEdit->setValidator(angleValidator);
    this->ArgumentPeriapChaserEdit->setValidator(angleValidator);
    this->TrueAnomalyChaserEdit->setValidator(angleValidator);

    this->PropStepEdit->setValidator(secondsValidator);


    //setup default values
    QDateTime initial;

  // if (origin==NEW_ORBITAL_DATA_REQUIRED){

    this->InitialEpochTargetEdit->setDateTime(initial.currentDateTime());
    this->RadiusTargetEdit->setText("7650");
    this->EccentricityTargetEdit->setText("0.0001");
    this->InclinationTargetEdit->setText("23");
    this->RAANTargetEdit->setText("60");
    this->ArgumentPeriapTargetEdit->setText("75");
    this->TrueAnomalyTargetEdit->setText("0.0");

    this->RadiusChaserEdit->setText("7650");
    this->EccentricityChaserEdit->setText("0.0001");
    this->InclinationChaserEdit->setText("23");
    this->RAANChaserEdit->setText("60");
    this->ArgumentPeriapChaserEdit->setText("75");
    this->TrueAnomalyChaserEdit->setText("0.0");

    this->PropStepEdit->setText("10.0");
//}

    /*if(origin==PREVIOUS_ORBITAL_DATA_PRESENT){

        sta::KeplerianElements eTarget;
        sta::KeplerianElements eChaser;


        QString temp;
        this->RadiusTargetEdit->setText(temp.setNum(eTarget.SemimajorAxis));
        this->EccentricityTargetEdit->setText(temp.setNum(eTarget.Eccentricity));
        this->InclinationTargetEdit->setText(temp.setNum(eTarget.Inclination));
        this->RAANTargetEdit->setText(temp.setNum(eTarget.AscendingNode));
        this->ArgumentPeriapTargetEdit->setText(temp.setNum(eTarget.ArgumentOfPeriapsis));
        this->TrueAnomalyTargetEdit->setText(temp.setNum(eTarget.TrueAnomaly));

        this->RadiusChaserEdit->setText(temp.setNum(eChaser.SemimajorAxis));
        this->EccentricityChaserEdit->setText(temp.setNum(eChaser.Eccentricity));
        this->InclinationChaserEdit->setText(temp.setNum(eChaser.Inclination));
        this->RAANChaserEdit->setText(temp.setNum(eChaser.AscendingNode));
        this->ArgumentPeriapChaserEdit->setText(temp.setNum(eChaser.ArgumentOfPeriapsis));
        this->TrueAnomalyChaserEdit->setText(temp.setNum(eChaser.TrueAnomaly));
    }*/


}


//ASPECT button

void RendezVousDialog::initAspectRV()
{
    rendezVousAspect->exec();
}



//Drag and drop management



