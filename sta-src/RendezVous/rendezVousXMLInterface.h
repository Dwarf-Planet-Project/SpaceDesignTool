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

#ifndef _RENDEZVOUSXMLINTERFACE_H_
#define _RENDEZVOUSXMLINTERFACE_H_

#include "Main/mainwindow.h"
#include "Main/scenariotree.h"
#include "Main/propagatedscenario.h"

#include "Scenario/missionsDefaults.h"
#include "Scenario/propagationfeedback.h"
#include "Scenario/scenarioPropagator.h"

#include "Astro-Core/eci2lhlv.h"
#include "Astro-Core/cartesianTOorbital.h"
#include "Astro-Core/stabody.h"
#include "Astro-Core/stamath.h"
#include "Astro-Core/date.h"

#include "Loitering/loitering.h"

#include "rendezVousManoeuvres.h"



class RVdiagXMLinterface{


public:
    double userOrbitalDialogInputs[13];
    QString centralBodyName;
    QString targetModelName;
    QString chaserModelName;

    QDateTime userDateTime;
    RVmanoeuvrePlan manPlan;

    RVmanoeuvrePlan closeRangePlan;

    bool loadRVScenario(SpaceScenario*,PropagatedScenario*);
    void createRVscenario(SpaceScenario*,ScenarioSC*,ScenarioSC*,
                          PropagatedScenario*);
    SpaceScenario* getScenario();
    ScenarioSC* getChaser();
    ScenarioSC* getTarget();

    void newChaser();
    void newTarget();

    void setManoeuvrePlan();

    void setManoeuvre(int,ScenarioRendezVousManoeuvreType*);
    void setFreeDrift(ScenarioLoiteringType*,int);
    void setDeltaV(ScenarioDeltaVType*,int);

    //Propagated Scenario
    PropagatedScenario* getPropagatedScenario();
    bool getPropagatedManoeuvreInfoAt(int);


    void setUserOrbitalDialogInputs(int,double);

    void setUserArcAspect(QString,QString,QString);

    void setUserDateTime(QDateTime);

    QList<double> getInitialDialogData();




private:

    SpaceScenario*      scenario;
    ScenarioSC*         target;
    ScenarioSC*         chaser;
    PropagatedScenario* propScenario;
    SpaceObject*        targetPropagated;
    SpaceObject*        chaserPropagated;


};







#endif // _RENDEZVOUSXMLINTERFACE_H_
