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
 ------------------ Author: Claas Grohnfeldt, Steffen Peter  -------------------------------------------------
 ------------------ E-mail: (claasgr@math.uni-bremen.de, spezisteffen@gmx.de) ----------------------------
 Patched by Guillermo June 2010 to make it compatible with Cambrian

*/


#include "Constellations/constellationwizard.h"

#include "Constellations/constellationmodule.h"

//#include "Scenario/scenario.h"

#include "Loitering/loitering.h"

#include "Astro-Core/stamath.h"

#include "Main/mainwindow.h"
#include "Main/scenariotree.h"
//#include "Main/scenarioelementbox.h"

#include <QMessageBox>
#include <QDebug>
#include <iostream>
#include <QTextStream>




ConstellationWizardDialog::ConstellationWizardDialog(MainWindow* parent)
    : mainwindow(parent)
{
    setupUi(this);

    // Set up the input validators
    QDoubleValidator* doublePosValidator = new QDoubleValidator(this);
    doublePosValidator->setBottom(0.0);
    QDoubleValidator* angleValidator = new QDoubleValidator(0.0,359.9999,4,this);
    angleValidator->setRange(0.0, 365.9999,4);
    QDoubleValidator* angleRangeValidator = new QDoubleValidator(0.0,360.0,4,this);

    numberOfOrbitPlanesLineEdit->setValidator(new QIntValidator(1,10,this));
    numberOfSatellitesLineEdit->setValidator(new QIntValidator(1,20,this));
    semiMajoraxisLineEdit->setValidator(doublePosValidator);
    inclinationLineEdit->setValidator(new QDoubleValidator(0.0, 179.9999, 4, this));
    relSpacingLineEdit->setValidator(new QIntValidator(0,9,this));

    numberOfPetalsLineEdit->setValidator(new QIntValidator(0,12,this));
    numberOfDaysToRepeatGroundTrackLineEdit->setValidator(new QIntValidator(0,50,this));
    numberOfSatellitesLineEdit_2->setValidator(new QIntValidator(0,36,this));
    perigeeLineEdit->setValidator(angleValidator);
    heightOfPerigeeLineEdit->setValidator(doublePosValidator);
    raanOfFirstSatelliteLineEdit->setValidator(angleValidator);
    initialTrueAnomalyOfFirstSatelliteLineEdit->setValidator(angleValidator);
    limitedOrbitNodeRangeLineEdit->setValidator(angleRangeValidator);

    numberOfSatellitesLineEdit_3->setValidator(new QIntValidator(1,36,this));

    // signals
    QObject::connect(schemeComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(disableflower()));


}




ConstellationWizardDialog::~ConstellationWizardDialog()
{
}

void ConstellationWizardDialog::disableflower()
{
    if (schemeComboBox->currentIndex()==0) limitedOrbitNodeRangeLineEdit->setEnabled(false);
    else limitedOrbitNodeRangeLineEdit->setEnabled(true);
}




void ConstellationWizardDialog::accept()
{


    // close Constellation Wizard
    close();

    /*
    // open Loitering Dialog and get additional Properties
    LoiteringDialog* loiteringdialog = new LoiteringDialog();
    loiteringdialog->setFocus();
    //loiteringdialog->groupBox->enabledChange(true);
    //loiteringdialog->groupBox->hide();
    //loiteringdialog->disableInitialStateParameters();
    loiteringdialog->exec();
    loiteringdialog->clearFocus();
    */

    // generate keplerian elements of the satellites
    SatelliteKeplerian* satellitekeplerian;
    int n; // number of Satellites


    if (constTypeComboBox->currentIndex() == 0) // Walker
    {
        bool ok;
        int npl = numberOfOrbitPlanesLineEdit->text().toInt(&ok,10);
        int nspl = numberOfSatellitesLineEdit->text().toInt(&ok,10);
        double a = semiMajoraxisLineEdit->text().toDouble(&ok);
        double inc = inclinationLineEdit->text().toDouble(&ok);
        int f = relSpacingLineEdit->text().toInt(&ok,10);
        n = npl * nspl;
        satellitekeplerian = new SatelliteKeplerian[n];
        if (!walkerConstellation(npl, nspl, a, inc, f, satellitekeplerian))
        {
            QMessageBox::warning(this, "Error", "Not able to generate Walker Constellation!");
            return;
        }
    }
    else if (constTypeComboBox->currentIndex() == 1) // Flower
    {
        bool ok;
        double fakdg = sta::Pi()/180.0; // transformation factor from deg -> rad
        // get parameters from Wizard
        int np = numberOfPetalsLineEdit->text().toInt(&ok,10);
        int nd = numberOfDaysToRepeatGroundTrackLineEdit->text().toInt(&ok,10);
        n = numberOfSatellitesLineEdit_2->text().toInt(&ok,10);
        double om = fakdg * perigeeLineEdit->text().toDouble(&ok);
        double inc;
        if (inclinationComboBox->currentIndex()==0) inc = 1.1065; // 63.4 degree in rad
        else inc = 2.0351; // 116.6 degree in rad
        double hp = heightOfPerigeeLineEdit->text().toDouble(&ok);
        bool scheme;
        if (schemeComboBox->currentIndex()==0) scheme = true; // symmetric
        else scheme = false; // asymmetric
        double omega1 = fakdg * raanOfFirstSatelliteLineEdit->text().toDouble(&ok);
        double psi1 = fakdg * initialTrueAnomalyOfFirstSatelliteLineEdit->text().toDouble(&ok);
        double domega;
        if (!scheme) domega = fakdg * limitedOrbitNodeRangeLineEdit->text().toDouble(&ok);
        // get central body data
	// ScenarioEnvironment* env = new ScenarioEnvironment();    // Guillermo comented out
	ScenarioSCEnvironmentType* env = new ScenarioSCEnvironmentType();
	//loiteringdialog->saveValues(env);
	// const StaBody* body =  env->centralBody()->body(); // Guillermo comented out
	const StaBody* body = STA_SOLAR_SYSTEM->lookup("Earth");
	//loitering.Environment()->CentralBody()->setName("EARTH");
	//loitering.InitialPosition()->setCoordinateSystem("INERTIAL J2000");
	//loitering.InitialAttitude()->setCoordinateSystem("EULER 123");


        // rotation angular velocity
        double rotV = 1/(body->rotationState()->rotationPeriod()*86400);
        if (body->name()=="Earth") {
            rotV = 2*sta::Pi()/(body->rotationState()->rotationPeriod()*86400);
        }
        // generate Flower Constellation
        satellitekeplerian = new SatelliteKeplerian[n];
        if (!flowerConstellation(np, nd, n, om, inc, hp, scheme, omega1, psi1, domega,
                                 body->meanRadius(), rotV, body->mu(),body->gravityModel().J2(), satellitekeplerian)){
            QMessageBox::warning(this, "Error", "Not able to generate Flower Constellation!");
            return;
        }
    }
    else if (constTypeComboBox->currentIndex() == 2) // Self-Defined
    {
        bool ok;
        n = numberOfSatellitesLineEdit_3->text().toInt(&ok,10);
        satellitekeplerian = new SatelliteKeplerian[n];
        if (!selfDefinedConstellation(n, satellitekeplerian))
        {
            QMessageBox::warning(this, "Error", "Not able to generate Flower Constellation!");
            return;
        }
    }
    else if (constTypeComboBox->currentIndex() == 3)
    {
        QMessageBox::warning(this, "", "Feature is not implemented yet!");
        return;
    }
    else if (constTypeComboBox->currentIndex() == 4)
    {
        QMessageBox::warning(this, "", "Feature is not implemented yet!");
        return;
    }

    // Create a new scenario
    SpaceScenario* scenario = new SpaceScenario();
    scenario->setName(constTypeComboBox->currentText());

    // create new Participants, Properties and trajectories
    for (int i = 0; i < n; i++)
    {
	ScenarioSC* sc = new ScenarioSC();
	sc->setName(satellitekeplerian[i].name);
	sc->ElementIdentifier()->setName(satellitekeplerian[i].name);

        // Create the initial position (Keplerian elements)
        ScenarioKeplerianElementsType* elements = new ScenarioKeplerianElementsType();
        elements->setSemiMajorAxis(satellitekeplerian[i].param[0]);
        elements->setEccentricity(satellitekeplerian[i].param[1]);
        elements->setInclination(satellitekeplerian[i].param[2]);
        elements->setRAAN(satellitekeplerian[i].param[3]);
        elements->setArgumentOfPeriapsis(satellitekeplerian[i].param[4]);
        elements->setTrueAnomaly(satellitekeplerian[i].param[5]);

	// Create the initial attitude (Euler elements)
	ScenarioEulerBIType*  initAtt = new ScenarioEulerBIType();
	//QSharedPointer<ScenarioEulerBIType> initAtt(new ScenarioEulerBIType());
	initAtt->setPhi(0.00000);
	initAtt->setTheta(0.00000);
	initAtt->setPsi(0.00000);
	initAtt->setPhiDot(0.00000);
	initAtt->setThetaDot(0.00000);
	initAtt->setPsiDot(0.00000);

        // Create the trajectory arc
        ScenarioLoiteringType* loitering = new ScenarioLoiteringType();
	loitering->ElementIdentifier()->setName("loitering");
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
    }

    mainwindow->setScenario(scenario);
    return;


}

