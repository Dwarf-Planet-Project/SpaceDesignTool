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

 */




#include "interplanetaryDialog.h"

#include "Scenario/scenarioPropagator.h"
#include "Scenario/missionsDefaults.h"

#include "Loitering/loitering.h"

#include "Astro-Core/stamath.h"

#include "Main/mainwindow.h"
#include "Main/scenariotree.h"


interplanetaryDialog::interplanetaryDialog(MainWindow* parent) : mainwindow(parent)
{
	setupUi(this);
}

interplanetaryDialog::~interplanetaryDialog()
{
}

void interplanetaryDialog::on_buttonBox_accepted()
{
    // Create a new scenario
    SpaceScenario* scenario = new SpaceScenario();
    scenario->setName("This is a demo");

    ScenarioSC* sc = new ScenarioSC();
    sc->setName("my satellite demo");
    sc->ElementIdentifier()->setName("my satellite demo");

    // Create the initial position (Keplerian elements)
    ScenarioKeplerianElementsType* elements = new ScenarioKeplerianElementsType();
    elements->setSemiMajorAxis(87000.0);
    elements->setEccentricity(0.0045);
    elements->setInclination(27.456);
    elements->setRAAN(132.34);
    elements->setArgumentOfPeriapsis(231.34);
    elements->setTrueAnomaly(45.32);

    // Create the initial attitude (Euler elements)
    ScenarioEulerType*  initAtt = new ScenarioEulerType();
    initAtt->setPhi(0.00000);
    initAtt->setTheta(0.00000);
    initAtt->setPsi(0.00000);
    initAtt->setPhiDot(0.00000);
    initAtt->setThetaDot(0.00000);
    initAtt->setPsiDot(0.00000);

    // Create the trajectory arc
    ScenarioLoiteringType* loitering = new ScenarioLoiteringType();
    loitering->ElementIdentifier()->setName("loitering");
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
    //loitering->InitialAttitude()->setAbstract6DOFAttitude(initAtt);
    loitering->InitialAttitude()->setAbstract6DOFAttitude(QSharedPointer<ScenarioAbstract6DOFAttitudeType>(initAtt));

    // Create the spacecraft
    sc->SCMission()->TrajectoryPlan()->AbstractTrajectory().append(QSharedPointer<ScenarioAbstractTrajectoryType>(loitering));

    // Add it to the scenario
    scenario->AbstractParticipant().append(QSharedPointer<ScenarioParticipantType>(sc));

    mainwindow->setScenario(scenario);
    return;
}

void interplanetaryDialog::on_buttonBox_rejected()
{
	qWarning("TODO: %s	%d",__FILE__,__LINE__);
}

