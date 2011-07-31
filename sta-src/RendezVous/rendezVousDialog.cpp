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


 // InitialDataSetup.

 setupTargetChaserInitialData(NEW_ORBITAL_DATA_REQUIRED);
 updateUserOrbitalInput();

 //In this method, propagated scenario is loaded.

  if(interfXML.loadRVScenario(scenario,propScenario)){
      //In case propagation data is available, manPlan must be updated

      //Just with propagation data
      if (!interfXML.manPlan.plan.isEmpty())
      {
          //If there's a propagated scenario, initial data must be the same
          setupTargetChaserInitialData(PREVIOUS_ORBITAL_DATA_PRESENT);
          updateUserOrbitalInput();


      for (i=0;i<interfXML.manPlan.plan.size();i++)
            {
            //Checking LHLV
            interfXML.getPropagatedManoeuvreInfoAt(i);
            }
      }


    }
    else{
    //So, no scenario is loaded. There isn't there or just it's another kind of scenario
      //Need for generate a new one.

        this->generatePB->setEnabled(true);



    }





  // Connect
  connect(generatePB, SIGNAL(pressed()),this,SLOT(generateScenario()));
  connect(okPB, SIGNAL(pressed()),this,SLOT(okActivated()));

  //manPlan update.

  this->closeManTable->updateTable(interfXML.manPlan.plan);



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
    QString temp1,temp2,temp3;
    temp1.append(this->comboBoxTargetModel->currentText());
    temp2.append(this->comboBoxChaserModel->currentText());
    temp3.append(this->comboBoxCentralBody->currentText());



    interfXML.setUserArcAspect(temp1,temp2,temp3);

    temp1.clear();
    temp2.clear();
    temp3.clear();

    for (int i=0;i<13;i++){
        interfXML.setUserOrbitalDialogInputs(i,userOrbitalInput[i]);
    }

}


void RendezVousDialog::generateScenario(){

    // Create a new scenario. Memory allocatation in here

    SpaceScenario* scenario=new SpaceScenario();
    ScenarioSC* chaser=new ScenarioSC();
    ScenarioSC* target=new ScenarioSC();
    PropagatedScenario* propScenario=new PropagatedScenario();
    //UPDATE for close range man list.
    this->closeManTable->updateManoeuvreList();
    //To interface with XML

    interfXML.closeRangePlan.plan.append(this->closeManTable->manoeuvreList);

    //UPDATE userOrbitalInputs
    updateUserOrbitalInput();

    //Create scenario
    interfXML.createRVscenario(scenario,chaser,target,propScenario);

    mainwindow->setScenario(interfXML.getScenario());

    //Once mainwindow has data, cleaning is needed. All changes will come from outside
    interfXML.closeRangePlan.plan.clear();
    this->closeManTable->manoeuvreList.clear();

}

void RendezVousDialog::okActivated(){
   this->generateScenario();

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

 if (origin==NEW_ORBITAL_DATA_REQUIRED){

    this->InitialEpochTargetEdit->setDateTime(initial.currentDateTime());
    this->RadiusTargetEdit->setText("7650");
    this->EccentricityTargetEdit->setText("0.0000001");
    this->InclinationTargetEdit->setText("23");
    this->RAANTargetEdit->setText("60");
    this->ArgumentPeriapTargetEdit->setText("75");
    this->TrueAnomalyTargetEdit->setText("0.0");

    this->RadiusChaserEdit->setText("7650");
    this->EccentricityChaserEdit->setText("0.0000001");
    this->InclinationChaserEdit->setText("23");
    this->RAANChaserEdit->setText("60");
    this->ArgumentPeriapChaserEdit->setText("75");
    this->TrueAnomalyChaserEdit->setText("0.001");

    this->PropStepEdit->setText("10.0");
}

   if(origin==PREVIOUS_ORBITAL_DATA_PRESENT){

        QList<double> list;

        list.append(interfXML.getInitialDialogData());

        QString temp;

        this->RadiusTargetEdit->setText(temp.setNum(list.at(0)));
        this->EccentricityTargetEdit->setText(temp.setNum(list.at(1),'f',7));
        this->InclinationTargetEdit->setText(temp.setNum(list.at(2)));
        this->RAANTargetEdit->setText(temp.setNum(list.at(3)));
        this->ArgumentPeriapTargetEdit->setText(temp.setNum(list.at(4)));
        this->TrueAnomalyTargetEdit->setText(temp.setNum(list.at(5),'f',7));

        this->RadiusChaserEdit->setText(temp.setNum(list.at(6)));
        this->EccentricityChaserEdit->setText(temp.setNum(list.at(7),'f',7));
        this->InclinationChaserEdit->setText(temp.setNum(list.at(8)));
        this->RAANChaserEdit->setText(temp.setNum(list.at(9)));
        this->ArgumentPeriapChaserEdit->setText(temp.setNum(list.at(10)));
        this->TrueAnomalyChaserEdit->setText(temp.setNum(list.at(11),'f',7));

        this->InitialEpochTargetEdit->setDate(interfXML.manPlan.plan[0].initTime.date());
        this->InitialEpochTargetEdit->setTime(interfXML.manPlan.plan[0].initTime.time());

    }

}








