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
 ------------------ Author: J. Alonso     -------------------------------------------------
 ------------------ ETSIA                 ----------------------------

 */

#ifndef RVXMLINTERFACE_H
#define RVXMLINTERFACE_H

#include "Scenario/scenarioPropagator.h"
#include "Scenario/missionsDefaults.h"

#include "Loitering/loitering.h"

#include "Astro-Core/stamath.h"

#include "Main/mainwindow.h"
#include "Main/scenariotree.h"

#include "Scenario/scenario.h"

#include "Scenario/propagationfeedback.h"
#include "Scenario/scenarioPropagator.h"
#include "Main/propagatedscenario.h"
#include "RVManoeuvres.h"

//ManouvrePlan will have a complete list of
//manouvres.




class RVXMLInterface{


public:
    RVManoeuvrePlan manPlan;

    bool loadRVScenario(SpaceScenario*,PropagatedScenario*);
    void createRVscenario(SpaceScenario*,ScenarioSC*,ScenarioSC*,PropagatedScenario*);
    SpaceScenario* getScenario();
    ScenarioSC* getChaser();
    ScenarioSC* getTarget();

    void newChaser();
    void newTarget();

    void setDeltaV(ScenarioDeltaVType*,int);
    void setFreeDrift(ScenarioLoiteringType*,int);

    //Propagated Scenario
    PropagatedScenario* getPropagatedScenario();
   // void setPropagatedScenario();
    QString getPropagatedTrajectory(int);



private:

    SpaceScenario*      scenario;
    ScenarioSC*         target;
    ScenarioSC*         chaser;
    PropagatedScenario* propScenario;
    SpaceObject*        targetPropagated;
    SpaceObject*        chaserPropagated;


};





#endif // RVXMLINTERFACE_H
