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
 Modified by: J Alonso.
 ETSIA
 Changes : Support new Ordovican specs.
 */


#include "rendezvousDialog.h"

#include <QMessageBox>

rendezvousDialog::rendezvousDialog(SpaceScenario* scenario,PropagatedScenario*
                          propScenario,MainWindow* parent) : mainwindow(parent)
{
	setupUi(this);

    //In this method, propagated scenario is loaded.
  if(interfXML.loadRVScenario(scenario,propScenario)){
      if (!interfXML.manPlan.plan.isEmpty())
      {
          for(int i=0;i<interfXML.manPlan.plan.size();i++)
            this->testTextEditChaser->append(interfXML.manPlan.plan[i].getName());
      }

      this->generatePB->setDisabled(true);
      this->testTextEditChaser->append(interfXML.getPropagatedTrajectory(0));
      this->testTextEditTarget->append(interfXML.getPropagatedTrajectory(1));


    }
    else{
        this->generatePB->setEnabled(true);
    }





  //Connect
  connect(generatePB, SIGNAL(pressed()),this,SLOT(generateScenario()));
  connect(propagatePB, SIGNAL(pressed()),this,SLOT(propagate()));
  connect(deltaVPB, SIGNAL(pressed()),this,SLOT(addDeltaV()));
  connect(freeDriftPB, SIGNAL(pressed()),this,SLOT(addFreeDrift()));
}

rendezvousDialog::~rendezvousDialog()
{
}


void rendezvousDialog::generateScenario(){
    // Create a new scenario
    SpaceScenario* scenario=new SpaceScenario();
    ScenarioSC* chaser=new ScenarioSC();
    ScenarioSC* target=new ScenarioSC();
    PropagatedScenario* propScenario=new PropagatedScenario();

    interfXML.createRVscenario(scenario,chaser,target,propScenario);
    //Adds manoeuvre plan.
    //interfXML.addManPlan();

    mainwindow->setScenario(interfXML.getScenario());
    //Needed for cleaning previous data.
    //mainwindow->setPropagatedScenario(NULL);

}

void rendezvousDialog::propagate(){
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


void rendezvousDialog::on_buttonBox_accepted()
{

    return;
}

void rendezvousDialog::on_buttonBox_rejected()
{

    qWarning("TODO: %s	%d",__FILE__,__LINE__);
}

void rendezvousDialog::addDeltaV(){
    RVManoeuvreDescriptor m1;
    m1.descriptor=DELTA_V;
    interfXML.manPlan.plan.append(m1);
    generateScenario();

}

void rendezvousDialog::addFreeDrift(){
    RVManoeuvreDescriptor m1;
    m1.descriptor=FREE_DRIFT;
    interfXML.manPlan.plan.append(m1);
    generateScenario();
}



//Drag and drop management



