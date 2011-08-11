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
 ------------------ Author:       Guillermo Ortega               -------------------
 ------------------ Affiliation:  European Space Agency (ESA)    -------------------
 -----------------------------------------------------------------------------------
 Modified by Tiziana Sabatini on July 2009 to support the new perturbations layer
 Extensively modified by Guillermo on August 2010 to comply with the new schema v2.0
 Patched by Catarina to add attitude, July 2011
 Patched by Guilelrmo to allow on the fly conversions, August 2011
*/

#include "Loitering/loitering.h"

#include "Main/scenariotree.h"

//#include "Scenario/scenario.h"
#include "Scenario/propagationfeedback.h"
#include "Scenario/missionAspectDialog.h"
#include "Scenario/missionAspectDialog.h"

#include "Astro-Core/date.h"
#include "Astro-Core/stacoordsys.h"
#include "Astro-Core/stabody.h"
#include "Astro-Core/stamath.h"
#include "Astro-Core/trueAnomalyTOmeanAnomaly.h"
#include "Astro-Core/cartesianTOorbital.h"
#include "Astro-Core/orbitalTOcartesian.h"
#include "Astro-Core/propagateTWObody.h"
#include "Astro-Core/propagateCOWELL.h"
#include "Astro-Core/propagateENCKE.h"
#include "Astro-Core/propagateGAUSS.h"
#include "Astro-Core/attitudevector.h"
#include "Astro-Core/attitudeintegration.h"
#include "Astro-Core/attitudetransformations.h"
#include "Astro-Core/trueAnomalyTOmeanAnomaly.h"

#include "ui_missionAspectDialog.h"

#include "Eigen/Geometry"

#include <QWidget>
#include <QTextStream>
#include <QDebug>
#include <QFile>

using namespace Eigen;

static const unsigned int MAX_OUTPUT_STEPS = 1000000;


LoiteringDialog::LoiteringDialog(ScenarioTree* parent) :
        QDialog(parent)
{
    setupUi(this);

    // Set up generic input validators
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

    // Set up the combo boxes: position and velocity
    CoordSystemComboBox->addItem(tr("Planet Fixed"), (int) sta::COORDSYS_BODYFIXED);
    CoordSystemComboBox->addItem(tr("Inertial (J2000)"), (int) sta::COORDSYS_EME_J2000);
    CoordSystemComboBox->addItem(tr("Inertial (B1950)"), (int) sta::COORDSYS_EME_B1950);
    CoordSystemComboBox->addItem(tr("Ecliptic (J2000)"), (int) sta::COORDSYS_ECLIPTIC_J2000);
    PropagatorComboBox->addItem(tr("Two Body"), "TWO BODY");
    PropagatorComboBox->addItem(tr("Gauss"), "GAUSS");
    PropagatorComboBox->addItem(tr("Cowell"), "COWELL");
    PropagatorComboBox->addItem(tr("Encke"), "ENCKE");
    IntegratorComboBox->addItem(tr("Runge-Kutta 3-4"), "RK4");
    IntegratorComboBox->addItem(tr("Runge-Kutta-Fehlberg"), "RKF");

    //  Setting up units for the position panels: Keplerian
    serviceDistanceUnitWidgetKeplerianA = new DialogServiceDistanceUnitFrame();
    gridLayoutKeplerianElements->addWidget(serviceDistanceUnitWidgetKeplerianA, 0, 2); serviceDistanceUnitWidgetKeplerianA->show(); // Units of Semimajor axis
    connect(serviceDistanceUnitWidgetKeplerianA->distanceUnitWidget, SIGNAL(currentIndexChanged(int)), this, SLOT(updateInputKeplerianA(int)));
    serviceAngleUnitWidgetKeplerianI = new DialogServiceAngleUnitFrame();
    gridLayoutKeplerianElements->addWidget(serviceAngleUnitWidgetKeplerianI, 2, 2); serviceAngleUnitWidgetKeplerianI->show(); // Units of Inclination
    connect(serviceAngleUnitWidgetKeplerianI->angleUnitWidget, SIGNAL(currentIndexChanged(int)), this, SLOT(updateInputKeplerianI(int)));
    serviceAngleUnitWidgetKeplerianRAAN = new DialogServiceAngleUnitFrame();
    gridLayoutKeplerianElements->addWidget(serviceAngleUnitWidgetKeplerianRAAN, 3, 2); serviceAngleUnitWidgetKeplerianRAAN->show(); // Units of RANN
    connect(serviceAngleUnitWidgetKeplerianRAAN->angleUnitWidget, SIGNAL(currentIndexChanged(int)), this, SLOT(updateInputKeplerianRAAN(int)));
    serviceAngleUnitWidgetKeplerianAoP = new DialogServiceAngleUnitFrame();
    gridLayoutKeplerianElements->addWidget(serviceAngleUnitWidgetKeplerianAoP, 4, 2); serviceAngleUnitWidgetKeplerianAoP->show(); // Units of Argument of Periapsis
    connect(serviceAngleUnitWidgetKeplerianAoP->angleUnitWidget, SIGNAL(currentIndexChanged(int)), this, SLOT(updateInputKeplerianAoP(int)));
    serviceAngleUnitWidgetKeplerianTrueAnomaly = new DialogServiceAngleUnitFrame();
    gridLayoutKeplerianElements->addWidget(serviceAngleUnitWidgetKeplerianTrueAnomaly, 5, 2); serviceAngleUnitWidgetKeplerianTrueAnomaly->show(); // Units of True Anomaly
    connect(serviceAngleUnitWidgetKeplerianTrueAnomaly->angleUnitWidget, SIGNAL(currentIndexChanged(int)), this, SLOT(updateInputKeplerianTrueAnomaly(int)));

    //  Setting up units for the position panels: Cartesian coordinates
    serviceDistanceUnitWidgetCartesianX = new DialogServiceDistanceUnitFrame();
    gridLayoutStateVector->addWidget(serviceDistanceUnitWidgetCartesianX, 0, 2); serviceDistanceUnitWidgetCartesianX->show(); // Units of X
    connect(serviceDistanceUnitWidgetCartesianX->distanceUnitWidget, SIGNAL(currentIndexChanged(int)), this, SLOT(updateInputCartesianX(int)));
    serviceDistanceUnitWidgetCartesianY = new DialogServiceDistanceUnitFrame();
    gridLayoutStateVector->addWidget(serviceDistanceUnitWidgetCartesianY, 1, 2); serviceDistanceUnitWidgetCartesianY->show(); // Units of Y
    connect(serviceDistanceUnitWidgetCartesianY->distanceUnitWidget, SIGNAL(currentIndexChanged(int)), this, SLOT(updateInputCartesianY(int)));
    serviceDistanceUnitWidgetCartesianZ = new DialogServiceDistanceUnitFrame();
    gridLayoutStateVector->addWidget(serviceDistanceUnitWidgetCartesianZ, 2, 2); serviceDistanceUnitWidgetCartesianZ->show(); // Units of Z
    connect(serviceDistanceUnitWidgetCartesianZ->distanceUnitWidget, SIGNAL(currentIndexChanged(int)), this, SLOT(updateInputCartesianZ(int)));
    serviceDistanceRateUnitWidgetCartesianVx = new DialogServiceDistanceRateUnitFrame();
    gridLayoutStateVector->addWidget(serviceDistanceRateUnitWidgetCartesianVx, 3, 2); serviceDistanceRateUnitWidgetCartesianVx->show(); // Units of Vx
    connect(serviceDistanceRateUnitWidgetCartesianVx->distanceRateUnitWidget, SIGNAL(currentIndexChanged(int)), this, SLOT(updateInputCartesianVx(int)));
    serviceDistanceRateUnitWidgetCartesianVy = new DialogServiceDistanceRateUnitFrame();
    gridLayoutStateVector->addWidget(serviceDistanceRateUnitWidgetCartesianVy, 4, 2); serviceDistanceRateUnitWidgetCartesianVy->show(); // Units of Vy
    connect(serviceDistanceRateUnitWidgetCartesianVy->distanceRateUnitWidget, SIGNAL(currentIndexChanged(int)), this, SLOT(updateInputCartesianVy(int)));
    serviceDistanceRateUnitWidgetCartesianVz = new DialogServiceDistanceRateUnitFrame();
    gridLayoutStateVector->addWidget(serviceDistanceRateUnitWidgetCartesianVz, 5, 2); serviceDistanceRateUnitWidgetCartesianVz->show(); // Units of Vz
    connect(serviceDistanceRateUnitWidgetCartesianVz->distanceRateUnitWidget, SIGNAL(currentIndexChanged(int)), this, SLOT(updateInputCartesianVz(int)));

    // Set up the combo boxes: Attitude
    CoordSystemAttitudeComboBox->addItem(tr("Center of Gravity"), (int) sta::COORDSYS_COG);
    CoordSystemAttitudeComboBox->addItem(tr("Center of Pressure"), (int) sta::COORDSYS_COP);
    AttitudeTypeComboBox->addItem(tr("Euler Angles 123"), "Euler123");
    AttitudeTypeComboBox->addItem(tr("Euler Angles 321"), "Euler321");
    AttitudeTypeComboBox->addItem(tr("Euler Angles 313"), "Euler313");
    AttitudeTypeComboBox->addItem(tr("Quaternions ESA"), "QuaternionsESA");
    AttitudeTypeComboBox->addItem(tr("Quaternions JPL"), "QuaternionsJPL");
    PropagatorAttitudeComboBox->addItem(tr("Euler 1->2->3"), "PEuler123");
    PropagatorAttitudeComboBox->addItem(tr("Euler 3->2->1"), "PEuler321");
    PropagatorAttitudeComboBox->addItem(tr("Euler 3->1->3"), "PEuler313");
    PropagatorAttitudeComboBox->addItem(tr("Quaternions ESA"), "PQuaternionsESA");
    PropagatorAttitudeComboBox->addItem(tr("Quaternions JPL"), "PQuaternionsJPL");
    IntegratorAttitudeComboBox->addItem(tr("Runge-Kutta 3-4"),"RK4"); //Catarina: Integrator for Attitude

    //  Setting up units for the Attitude panels: Euler 123
    serviceAngleUnitWidgetEuler123Phi = new DialogServiceAngleUnitFrame();
    gridLayoutEuler123->addWidget(serviceAngleUnitWidgetEuler123Phi, 0, 2); serviceAngleUnitWidgetEuler123Phi->show(); // Units of Phi
    connect(serviceAngleUnitWidgetEuler123Phi->angleUnitWidget, SIGNAL(currentIndexChanged(int)), this, SLOT(updateInputEuler123Phi(int)));
    serviceAngleUnitWidgetEuler123Theta = new DialogServiceAngleUnitFrame();
    gridLayoutEuler123->addWidget(serviceAngleUnitWidgetEuler123Theta, 1, 2); serviceAngleUnitWidgetEuler123Theta->show(); // Units of Theta
    connect(serviceAngleUnitWidgetEuler123Theta->angleUnitWidget, SIGNAL(currentIndexChanged(int)), this, SLOT(updateInputEuler123Theta(int)));
    serviceAngleUnitWidgetEuler123Psi = new DialogServiceAngleUnitFrame();
    gridLayoutEuler123->addWidget(serviceAngleUnitWidgetEuler123Psi, 2, 2); serviceAngleUnitWidgetEuler123Psi->show(); // Units of Psi
    connect(serviceAngleUnitWidgetEuler123Psi->angleUnitWidget, SIGNAL(currentIndexChanged(int)), this, SLOT(updateInputEuler123Psi(int)));
    serviceAngleRateUnitWidgetEuler123OmegaX = new DialogServiceAngleRateUnitFrame();
    gridLayoutEuler123->addWidget(serviceAngleRateUnitWidgetEuler123OmegaX, 3, 2); serviceAngleRateUnitWidgetEuler123OmegaX->show(); // Units of OmegaX
    connect(serviceAngleRateUnitWidgetEuler123OmegaX->angleRateUnitWidget, SIGNAL(currentIndexChanged(int)), this, SLOT(updateInputEuler123OmegaX(int)));
    serviceAngleRateUnitWidgetEuler123OmegaY = new DialogServiceAngleRateUnitFrame();
    gridLayoutEuler123->addWidget(serviceAngleRateUnitWidgetEuler123OmegaY, 4, 2); serviceAngleRateUnitWidgetEuler123OmegaY->show(); // Units of OmegaY
    connect(serviceAngleRateUnitWidgetEuler123OmegaY->angleRateUnitWidget, SIGNAL(currentIndexChanged(int)), this, SLOT(updateInputEuler123OmegaY(int)));
    serviceAngleRateUnitWidgetEuler123OmegaZ = new DialogServiceAngleRateUnitFrame();
    gridLayoutEuler123->addWidget(serviceAngleRateUnitWidgetEuler123OmegaZ, 5, 2); serviceAngleRateUnitWidgetEuler123OmegaZ->show(); // Units of OmegaZ
    connect(serviceAngleRateUnitWidgetEuler123OmegaZ->angleRateUnitWidget, SIGNAL(currentIndexChanged(int)), this, SLOT(updateInputEuler123OmegaZ(int)));

    //  Setting up units for the Attitude panels: Euler 321
    serviceAngleUnitWidgetEuler321Phi = new DialogServiceAngleUnitFrame();
    gridLayoutEuler321->addWidget(serviceAngleUnitWidgetEuler321Phi, 0, 2); serviceAngleUnitWidgetEuler321Phi->show(); // Units of Phi
    connect(serviceAngleUnitWidgetEuler321Phi->angleUnitWidget, SIGNAL(currentIndexChanged(int)), this, SLOT(updateInputEuler321Phi(int)));
    serviceAngleUnitWidgetEuler321Theta = new DialogServiceAngleUnitFrame();
    gridLayoutEuler321->addWidget(serviceAngleUnitWidgetEuler321Theta, 1, 2); serviceAngleUnitWidgetEuler321Theta->show(); // Units of Theta
    connect(serviceAngleUnitWidgetEuler321Theta->angleUnitWidget, SIGNAL(currentIndexChanged(int)), this, SLOT(updateInputEuler321Theta(int)));
    serviceAngleUnitWidgetEuler321Psi = new DialogServiceAngleUnitFrame();
    gridLayoutEuler321->addWidget(serviceAngleUnitWidgetEuler321Psi, 2, 2); serviceAngleUnitWidgetEuler321Psi->show(); // Units of Psi
    connect(serviceAngleUnitWidgetEuler321Psi->angleUnitWidget, SIGNAL(currentIndexChanged(int)), this, SLOT(updateInputEuler321Psi(int)));
    serviceAngleRateUnitWidgetEuler321OmegaX = new DialogServiceAngleRateUnitFrame();
    gridLayoutEuler321->addWidget(serviceAngleRateUnitWidgetEuler321OmegaX, 3, 2); serviceAngleRateUnitWidgetEuler321OmegaX->show(); // Units of OmegaX
    connect(serviceAngleRateUnitWidgetEuler321OmegaX->angleRateUnitWidget, SIGNAL(currentIndexChanged(int)), this, SLOT(updateInputEuler321OmegaX(int)));
    serviceAngleRateUnitWidgetEuler321OmegaY = new DialogServiceAngleRateUnitFrame();
    gridLayoutEuler321->addWidget(serviceAngleRateUnitWidgetEuler321OmegaY, 4, 2); serviceAngleRateUnitWidgetEuler321OmegaY->show(); // Units of OmegaY
    connect(serviceAngleRateUnitWidgetEuler321OmegaY->angleRateUnitWidget, SIGNAL(currentIndexChanged(int)), this, SLOT(updateInputEuler321OmegaY(int)));
    serviceAngleRateUnitWidgetEuler321OmegaZ = new DialogServiceAngleRateUnitFrame();
    gridLayoutEuler321->addWidget(serviceAngleRateUnitWidgetEuler321OmegaZ, 5, 2); serviceAngleRateUnitWidgetEuler321OmegaZ->show(); // Units of OmegaZ
    connect(serviceAngleRateUnitWidgetEuler321OmegaZ->angleRateUnitWidget, SIGNAL(currentIndexChanged(int)), this, SLOT(updateInputEuler321OmegaZ(int)));

    //  Setting up units for the Attitude panels: Euler 313
    serviceAngleUnitWidgetEuler313Phi = new DialogServiceAngleUnitFrame();
    gridLayoutEuler313->addWidget(serviceAngleUnitWidgetEuler313Phi, 0, 2); serviceAngleUnitWidgetEuler313Phi->show(); // Units of Phi
    connect(serviceAngleUnitWidgetEuler313Phi->angleUnitWidget, SIGNAL(currentIndexChanged(int)), this, SLOT(updateInputEuler313Phi(int)));
    serviceAngleUnitWidgetEuler313Theta = new DialogServiceAngleUnitFrame();
    gridLayoutEuler313->addWidget(serviceAngleUnitWidgetEuler313Theta, 1, 2); serviceAngleUnitWidgetEuler313Theta->show(); // Units of Theta
    connect(serviceAngleUnitWidgetEuler313Theta->angleUnitWidget, SIGNAL(currentIndexChanged(int)), this, SLOT(updateInputEuler313Theta(int)));
    serviceAngleUnitWidgetEuler313Psi = new DialogServiceAngleUnitFrame();
    gridLayoutEuler313->addWidget(serviceAngleUnitWidgetEuler313Psi, 2, 2); serviceAngleUnitWidgetEuler313Psi->show(); // Units of Psi
    connect(serviceAngleUnitWidgetEuler313Psi->angleUnitWidget, SIGNAL(currentIndexChanged(int)), this, SLOT(updateInputEuler313Psi(int)));
    serviceAngleRateUnitWidgetEuler313OmegaX = new DialogServiceAngleRateUnitFrame();
    gridLayoutEuler313->addWidget(serviceAngleRateUnitWidgetEuler313OmegaX, 3, 2); serviceAngleRateUnitWidgetEuler313OmegaX->show(); // Units of OmegaX
    connect(serviceAngleRateUnitWidgetEuler313OmegaX->angleRateUnitWidget, SIGNAL(currentIndexChanged(int)), this, SLOT(updateInputEuler313OmegaX(int)));
    serviceAngleRateUnitWidgetEuler313OmegaY = new DialogServiceAngleRateUnitFrame();
    gridLayoutEuler313->addWidget(serviceAngleRateUnitWidgetEuler313OmegaY, 4, 2); serviceAngleRateUnitWidgetEuler313OmegaY->show(); // Units of OmegaY
    connect(serviceAngleRateUnitWidgetEuler313OmegaY->angleRateUnitWidget, SIGNAL(currentIndexChanged(int)), this, SLOT(updateInputEuler313OmegaY(int)));
    serviceAngleRateUnitWidgetEuler313OmegaZ = new DialogServiceAngleRateUnitFrame();
    gridLayoutEuler313->addWidget(serviceAngleRateUnitWidgetEuler313OmegaZ, 5, 2); serviceAngleRateUnitWidgetEuler313OmegaZ->show(); // Units of OmegaZ
    connect(serviceAngleRateUnitWidgetEuler313OmegaZ->angleRateUnitWidget, SIGNAL(currentIndexChanged(int)), this, SLOT(updateInputEuler313OmegaZ(int)));

    // -------- Setting up specific validators
    //TimeStepLineEdit->setValidator(positiveDoubleValidator);
    IntAttitudeStep->setValidator(positiveDoubleValidator); //Catarina: Validator for the attitude time step
    //Set validator for the state vector
    positionXEdit->setValidator(doubleValidator); positionYEdit->setValidator(doubleValidator); positionZEdit->setValidator(doubleValidator);
    velocityXEdit->setValidator(doubleValidator); velocityYEdit->setValidator(doubleValidator); velocityZEdit->setValidator(doubleValidator);
    //Set validator for the Keplerian elements
    semimajorAxisEdit->setValidator(positiveDoubleValidator);
    eccentricityEdit->setValidator(zeroToOneValidator);
    inclinationEdit->setValidator(angleValidator);
    raanEdit->setValidator(angleValidator);
    argOfPeriapsisEdit->setValidator(angleValidator);
    trueAnomalyEdit->setValidator(angleValidator);
    //Catarina: Set validator for the attitude
    //123 sequence
    Euler123phi->setValidator(angleValidator); Euler123theta->setValidator(angleValidator); Euler123psi->setValidator(angleValidator);
    Euler123omegaX->setValidator(doubleValidator); Euler123omegaY->setValidator(doubleValidator); Euler123omegaZ->setValidator(doubleValidator);
    //321 sequence
    Euler321phi->setValidator(angleValidator); Euler321theta->setValidator(angleValidator); Euler321psi->setValidator(angleValidator);
    Euler321omegaX->setValidator(doubleValidator); Euler321omegaY->setValidator(doubleValidator); Euler321omegaZ->setValidator(doubleValidator);
    //313 sequence
    Euler313phi->setValidator(angleValidator); Euler313theta->setValidator(angleValidator); Euler313psi->setValidator(angleValidator);
    Euler313omegaX->setValidator(doubleValidator); Euler313omegaY->setValidator(doubleValidator); Euler313omegaZ->setValidator(doubleValidator);
    //Quaternions
    q1->setValidator(minusOneToOneValidator); q2->setValidator(minusOneToOneValidator); q3->setValidator(minusOneToOneValidator); q4->setValidator(minusOneToOneValidator);
    q1dot->setValidator(doubleValidator); q2dot->setValidator(doubleValidator); q3dot->setValidator(doubleValidator); q4dot->setValidator(doubleValidator);
    // Step sizes
    IntStepEdit->setValidator(positiveDoubleValidator);

    //--- Connecting the Elements types combo widgets
    connect(InitialStateComboBox, SIGNAL(activated(int)), this, SLOT(updateElementSetTrajectory(int)));
    connect(AttitudeTypeComboBox, SIGNAL(activated(int)), this, SLOT(updateElementSetAttitude(int)));

}

LoiteringDialog::~LoiteringDialog()
{
    //delete TesseralSpinBox;
}


///////////////////////////////////// Methods that react after a unit combo has been activated ///////////////////////////////////////
void LoiteringDialog::updateInputKeplerianA(int myIndex)
{
    serviceDistanceUnitWidgetKeplerianA->setInputDistance(semimajorAxisEdit->text().toDouble());
    serviceDistanceUnitWidgetKeplerianA->on_comboBoxDistanceUnitsChoice_currentIndexChanged(myIndex);
    semimajorAxisEdit->setText(QString::number(serviceDistanceUnitWidgetKeplerianA->myFutureDistance));
}

void LoiteringDialog::updateInputKeplerianI(int myIndex)
{
    serviceAngleUnitWidgetKeplerianI->setInputAngle(inclinationEdit->text().toDouble());
    serviceAngleUnitWidgetKeplerianI->on_comboBoxAngleUnitsChoice_currentIndexChanged(myIndex);
    inclinationEdit->setText(QString::number(serviceAngleUnitWidgetKeplerianI->myFutureAngle));
}

void LoiteringDialog::updateInputKeplerianRAAN(int myIndex)
{
    serviceAngleUnitWidgetKeplerianRAAN->setInputAngle(raanEdit->text().toDouble());
    serviceAngleUnitWidgetKeplerianRAAN->on_comboBoxAngleUnitsChoice_currentIndexChanged(myIndex);
    raanEdit->setText(QString::number(serviceAngleUnitWidgetKeplerianRAAN->myFutureAngle));
}

void LoiteringDialog::updateInputKeplerianAoP(int myIndex)
{
    serviceAngleUnitWidgetKeplerianAoP->setInputAngle(argOfPeriapsisEdit->text().toDouble());
    serviceAngleUnitWidgetKeplerianAoP->on_comboBoxAngleUnitsChoice_currentIndexChanged(myIndex);
    argOfPeriapsisEdit->setText(QString::number(serviceAngleUnitWidgetKeplerianAoP->myFutureAngle));
}

void LoiteringDialog::updateInputKeplerianTrueAnomaly(int myIndex)
{
    serviceAngleUnitWidgetKeplerianTrueAnomaly->setInputAngle(trueAnomalyEdit->text().toDouble());
    serviceAngleUnitWidgetKeplerianTrueAnomaly->on_comboBoxAngleUnitsChoice_currentIndexChanged(myIndex);
    trueAnomalyEdit->setText(QString::number(serviceAngleUnitWidgetKeplerianTrueAnomaly->myFutureAngle));
}

void LoiteringDialog::updateInputCartesianX(int myIndex)
{
    serviceDistanceUnitWidgetCartesianX->setInputDistance(positionXEdit->text().toDouble());
    serviceDistanceUnitWidgetCartesianX->on_comboBoxDistanceUnitsChoice_currentIndexChanged(myIndex);
    positionXEdit->setText(QString::number(serviceDistanceUnitWidgetCartesianX->myFutureDistance));
}

void LoiteringDialog::updateInputCartesianY(int myIndex)
{
    serviceDistanceUnitWidgetCartesianY->setInputDistance(positionYEdit->text().toDouble());
    serviceDistanceUnitWidgetCartesianY->on_comboBoxDistanceUnitsChoice_currentIndexChanged(myIndex);
    positionYEdit->setText(QString::number(serviceDistanceUnitWidgetCartesianY->myFutureDistance));
}

void LoiteringDialog::updateInputCartesianZ(int myIndex)
{
    serviceDistanceUnitWidgetCartesianZ->setInputDistance(positionZEdit->text().toDouble());
    serviceDistanceUnitWidgetCartesianZ->on_comboBoxDistanceUnitsChoice_currentIndexChanged(myIndex);
    positionZEdit->setText(QString::number(serviceDistanceUnitWidgetCartesianZ->myFutureDistance));
}

void LoiteringDialog::updateInputCartesianVx(int myIndex)
{
    serviceDistanceRateUnitWidgetCartesianVx->setInputDistanceRate(velocityXEdit->text().toDouble());
    serviceDistanceRateUnitWidgetCartesianVx->on_comboBoxDistanceRateUnitsChoice_currentIndexChanged(myIndex);
    velocityXEdit->setText(QString::number(serviceDistanceRateUnitWidgetCartesianVx->myFutureDistanceRate));
}

void LoiteringDialog::updateInputCartesianVy(int myIndex)
{
    serviceDistanceRateUnitWidgetCartesianVy->setInputDistanceRate(velocityYEdit->text().toDouble());
    serviceDistanceRateUnitWidgetCartesianVy->on_comboBoxDistanceRateUnitsChoice_currentIndexChanged(myIndex);
    velocityYEdit->setText(QString::number(serviceDistanceRateUnitWidgetCartesianVy->myFutureDistanceRate));
}

void LoiteringDialog::updateInputCartesianVz(int myIndex)
{
    serviceDistanceRateUnitWidgetCartesianVz->setInputDistanceRate(velocityZEdit->text().toDouble());
    serviceDistanceRateUnitWidgetCartesianVz->on_comboBoxDistanceRateUnitsChoice_currentIndexChanged(myIndex);
    velocityZEdit->setText(QString::number(serviceDistanceRateUnitWidgetCartesianVz->myFutureDistanceRate));
}


void LoiteringDialog::updateInputEuler123Phi(int myIndex)
{
    serviceAngleUnitWidgetEuler123Phi->setInputAngle(Euler123phi->text().toDouble());
    serviceAngleUnitWidgetEuler123Phi->on_comboBoxAngleUnitsChoice_currentIndexChanged(myIndex);
    Euler123phi->setText(QString::number(serviceAngleUnitWidgetEuler123Phi->myFutureAngle));
}

void LoiteringDialog::updateInputEuler123Theta(int myIndex)
{
    serviceAngleUnitWidgetEuler123Theta->setInputAngle(Euler123theta->text().toDouble());
    serviceAngleUnitWidgetEuler123Theta->on_comboBoxAngleUnitsChoice_currentIndexChanged(myIndex);
    Euler123theta->setText(QString::number(serviceAngleUnitWidgetEuler123Theta->myFutureAngle));
}

void LoiteringDialog::updateInputEuler123Psi(int myIndex)
{
    serviceAngleUnitWidgetEuler123Psi->setInputAngle(Euler123psi->text().toDouble());
    serviceAngleUnitWidgetEuler123Psi->on_comboBoxAngleUnitsChoice_currentIndexChanged(myIndex);
    Euler123psi->setText(QString::number(serviceAngleUnitWidgetEuler123Psi->myFutureAngle));
}

void LoiteringDialog::updateInputEuler123OmegaX(int myIndex)
{
    serviceAngleRateUnitWidgetEuler123OmegaX->setInputAngleRate(Euler123omegaX->text().toDouble());
    serviceAngleRateUnitWidgetEuler123OmegaX->on_comboBoxAngleRateUnitsChoice_currentIndexChanged(myIndex);
    Euler123omegaX->setText(QString::number(serviceAngleRateUnitWidgetEuler123OmegaX->myFutureAngleRate));
}

void LoiteringDialog::updateInputEuler123OmegaY(int myIndex)
{
    serviceAngleRateUnitWidgetEuler123OmegaY->setInputAngleRate(Euler123omegaY->text().toDouble());
    serviceAngleRateUnitWidgetEuler123OmegaY->on_comboBoxAngleRateUnitsChoice_currentIndexChanged(myIndex);
    Euler123omegaY->setText(QString::number(serviceAngleRateUnitWidgetEuler123OmegaY->myFutureAngleRate));
}

void LoiteringDialog::updateInputEuler123OmegaZ(int myIndex)
{
    serviceAngleRateUnitWidgetEuler123OmegaZ->setInputAngleRate(Euler123omegaZ->text().toDouble());
    serviceAngleRateUnitWidgetEuler123OmegaZ->on_comboBoxAngleRateUnitsChoice_currentIndexChanged(myIndex);
    Euler123omegaZ->setText(QString::number(serviceAngleRateUnitWidgetEuler123OmegaZ->myFutureAngleRate));
}


void LoiteringDialog::updateInputEuler321Phi(int myIndex)
{
    serviceAngleUnitWidgetEuler321Phi->setInputAngle(Euler321phi->text().toDouble());
    serviceAngleUnitWidgetEuler321Phi->on_comboBoxAngleUnitsChoice_currentIndexChanged(myIndex);
    Euler321phi->setText(QString::number(serviceAngleUnitWidgetEuler321Phi->myFutureAngle));
}

void LoiteringDialog::updateInputEuler321Theta(int myIndex)
{
    serviceAngleUnitWidgetEuler321Theta->setInputAngle(Euler321theta->text().toDouble());
    serviceAngleUnitWidgetEuler321Theta->on_comboBoxAngleUnitsChoice_currentIndexChanged(myIndex);
    Euler321theta->setText(QString::number(serviceAngleUnitWidgetEuler321Theta->myFutureAngle));
}

void LoiteringDialog::updateInputEuler321Psi(int myIndex)
{
    serviceAngleUnitWidgetEuler321Psi->setInputAngle(Euler321psi->text().toDouble());
    serviceAngleUnitWidgetEuler321Psi->on_comboBoxAngleUnitsChoice_currentIndexChanged(myIndex);
    Euler321psi->setText(QString::number(serviceAngleUnitWidgetEuler321Psi->myFutureAngle));
}

void LoiteringDialog::updateInputEuler321OmegaX(int myIndex)
{
    serviceAngleRateUnitWidgetEuler321OmegaX->setInputAngleRate(Euler321omegaX->text().toDouble());
    serviceAngleRateUnitWidgetEuler321OmegaX->on_comboBoxAngleRateUnitsChoice_currentIndexChanged(myIndex);
    Euler321omegaX->setText(QString::number(serviceAngleRateUnitWidgetEuler321OmegaX->myFutureAngleRate));
}

void LoiteringDialog::updateInputEuler321OmegaY(int myIndex)
{
    serviceAngleRateUnitWidgetEuler321OmegaY->setInputAngleRate(Euler321omegaY->text().toDouble());
    serviceAngleRateUnitWidgetEuler321OmegaY->on_comboBoxAngleRateUnitsChoice_currentIndexChanged(myIndex);
    Euler321omegaY->setText(QString::number(serviceAngleRateUnitWidgetEuler321OmegaY->myFutureAngleRate));
}

void LoiteringDialog::updateInputEuler321OmegaZ(int myIndex)
{
    serviceAngleRateUnitWidgetEuler321OmegaZ->setInputAngleRate(Euler321omegaZ->text().toDouble());
    serviceAngleRateUnitWidgetEuler321OmegaZ->on_comboBoxAngleRateUnitsChoice_currentIndexChanged(myIndex);
    Euler321omegaZ->setText(QString::number(serviceAngleRateUnitWidgetEuler321OmegaZ->myFutureAngleRate));
}


void LoiteringDialog::updateInputEuler313Phi(int myIndex)
{
    serviceAngleUnitWidgetEuler313Phi->setInputAngle(Euler313phi->text().toDouble());
    serviceAngleUnitWidgetEuler313Phi->on_comboBoxAngleUnitsChoice_currentIndexChanged(myIndex);
    Euler313phi->setText(QString::number(serviceAngleUnitWidgetEuler313Phi->myFutureAngle));
}

void LoiteringDialog::updateInputEuler313Theta(int myIndex)
{
    serviceAngleUnitWidgetEuler313Theta->setInputAngle(Euler313theta->text().toDouble());
    serviceAngleUnitWidgetEuler313Theta->on_comboBoxAngleUnitsChoice_currentIndexChanged(myIndex);
    Euler313theta->setText(QString::number(serviceAngleUnitWidgetEuler313Theta->myFutureAngle));
}

void LoiteringDialog::updateInputEuler313Psi(int myIndex)
{
    serviceAngleUnitWidgetEuler313Psi->setInputAngle(Euler313psi->text().toDouble());
    serviceAngleUnitWidgetEuler313Psi->on_comboBoxAngleUnitsChoice_currentIndexChanged(myIndex);
    Euler313psi->setText(QString::number(serviceAngleUnitWidgetEuler313Psi->myFutureAngle));
}

void LoiteringDialog::updateInputEuler313OmegaX(int myIndex)
{
    serviceAngleRateUnitWidgetEuler313OmegaX->setInputAngleRate(Euler313omegaX->text().toDouble());
    serviceAngleRateUnitWidgetEuler313OmegaX->on_comboBoxAngleRateUnitsChoice_currentIndexChanged(myIndex);
    Euler313omegaX->setText(QString::number(serviceAngleRateUnitWidgetEuler313OmegaX->myFutureAngleRate));
}

void LoiteringDialog::updateInputEuler313OmegaY(int myIndex)
{
    serviceAngleRateUnitWidgetEuler313OmegaY->setInputAngleRate(Euler313omegaY->text().toDouble());
    serviceAngleRateUnitWidgetEuler313OmegaY->on_comboBoxAngleRateUnitsChoice_currentIndexChanged(myIndex);
    Euler313omegaY->setText(QString::number(serviceAngleRateUnitWidgetEuler313OmegaY->myFutureAngleRate));
}

void LoiteringDialog::updateInputEuler313OmegaZ(int myIndex)
{
    serviceAngleRateUnitWidgetEuler313OmegaZ->setInputAngleRate(Euler313omegaZ->text().toDouble());
    serviceAngleRateUnitWidgetEuler313OmegaZ->on_comboBoxAngleRateUnitsChoice_currentIndexChanged(myIndex);
    Euler313omegaZ->setText(QString::number(serviceAngleRateUnitWidgetEuler313OmegaZ->myFutureAngleRate));
}



///////////////////////////////////// Methods that react after a type of element set combo has been activated ///////////////////////////////////////
void LoiteringDialog::updateElementSetTrajectory(int myIndex)
{
    InitialStateComboBox->setCurrentIndex(myIndex);

    if (myIndex == 0) InitialStateStackedWidget->setCurrentWidget(keplerianPage);
    else if (myIndex == 1) InitialStateStackedWidget->setCurrentWidget(stateVectorPage);
    else if (myIndex == 2) InitialStateStackedWidget->setCurrentWidget(TwoLineElementPage);

}

void LoiteringDialog::updateElementSetAttitude(int myIndex)
{
    AttitudeTypeComboBox->setCurrentIndex(myIndex);

    if (myIndex == 0)      InitAttitudeStackedWidget->setCurrentWidget(Euler123Page);
    else if (myIndex == 1) InitAttitudeStackedWidget->setCurrentWidget(Euler321Page);
    else if (myIndex == 2) InitAttitudeStackedWidget->setCurrentWidget(Euler313Page);
    else if (myIndex == 3) InitAttitudeStackedWidget->setCurrentWidget(QuaternionESAPage);
    else if (myIndex == 4) InitAttitudeStackedWidget->setCurrentWidget(QuaternionJPLPage);
}


///////////////////////////////////// Methods to load and save the data to the GUI  ///////////////////////////////////////

void LoiteringDialog::disableIntegratorComboBox(int i)
{
    if (i == 0)
        IntegratorComboBox->setEnabled(false);
    else IntegratorComboBox->setEnabled(true);
}

bool LoiteringDialog::loadValues(ScenarioLoiteringType* loitering)
{
    ScenarioElementIdentifierType* arcIdentifier         = loitering->ElementIdentifier().data();
    ScenarioEnvironmentType* environment                 = loitering->Environment().data();
    ScenarioTimeLine* parameters                         = loitering->TimeLine().data();
    ScenarioPropagationPositionType* propagation         = loitering->PropagationPosition().data();
    ScenarioInitialPositionType* initPosition            = loitering->InitialPosition().data();  // Dominik
    ScenarioPropagationAttitudeType* attitudePropagation = loitering->PropagationAttitude().data();  //Added by Catarina
    ScenarioInitialAttitudeType* initAttitude            = loitering->InitialAttitude().data();  //Added by Catarina
    ScenarioInitialAttitudeUsingQuaternionsType* initAttQuaternions = loitering->InitialAttitudeUsingQuaternions().data(); // Guillermo

    if (loadValues(arcIdentifier) && loadValues(environment) && loadValues(parameters) &&
        loadValues(propagation) && loadValues(initPosition) && loadValues(attitudePropagation) &&
        loadValues(initAttQuaternions) && loadValues(initAttitude))
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool LoiteringDialog::loadValues(ScenarioElementIdentifierType* arcIdentifier)
{
    QString theArcName = arcIdentifier->Name();
    loiteringAspect.loadValueArcName(theArcName);

    QString theArcColor = arcIdentifier->colorName();
    loiteringAspect.loadValueArcColor(theArcColor);

    QString theArcModel = arcIdentifier->modelName();
    loiteringAspect.loadValueArcModel(theArcModel);

    return true;
}

bool LoiteringDialog::loadValues(ScenarioEnvironmentType* environment) //---------------------- <== Environment
{

    ScenarioCentralBodyType* myCentralBody = environment->CentralBody().data();
    ScenarioPerturbationsType* perturbationsToCentralBody = environment->PerturbationsToCentralBody().data();

    // The central body
    QString centralBodyName = environment->CentralBody()->Name();
    comboBoxCentralBody->setCurrentText(centralBodyName);

    // The perturbations
    myPerturbationsForces.loadValues(myCentralBody, perturbationsToCentralBody);

    return true;
}

bool LoiteringDialog::loadValues(ScenarioTimeLine* timeLine)
{
    trajectoryStartingEpoch->setDateTime(timeLine->StartTime());
    trajectoryEndingEpoch->setDateTime(timeLine->EndTime());
    //TimeStepLineEdit->setText(QString::number(timeLine->StepTime()));
    attitudeStartingEpoch->setDateTime(timeLine->StartTime());
    attitudeEndingEpoch->setDateTime(timeLine->EndTime());

    return true;
}

bool LoiteringDialog::loadValues(ScenarioPropagationPositionType* propagation)
{
    QString currentPropagator = propagation->propagator().trimmed();
    for (int i = 0; i < PropagatorComboBox->count(); i++)
    {
        if (PropagatorComboBox->itemData(i) == currentPropagator)
        {
            PropagatorComboBox->setCurrentIndex(i);
            break;
        }
    }
    QString currentIntegrator = propagation->integrator().trimmed();
    for (int i = 0; i < IntegratorComboBox->count(); i++)
    {
        if (IntegratorComboBox->itemData(i) == currentIntegrator)
        {
            IntegratorComboBox->setCurrentIndex(i);
            break;
        }
    }

    IntStepEdit->setText(QString::number(propagation->timeStep()));
    return true;
}

bool LoiteringDialog::loadValues(ScenarioInitialPositionType* initPosition)
{
    QString coordSysName = initPosition->CoordinateSystem().trimmed();
    sta::CoordinateSystem coordSys(coordSysName);
    if (!coordSys.valid())
    {
        qDebug() << "Bad coordinate system '" << coordSysName << "' in scenario ";
        coordSys = sta::CoordinateSystem(sta::COORDSYS_EME_J2000);
    }

    // Set the coordinate system combo box value
    for (int i = 0; i < CoordSystemComboBox->count(); i++)
    {
        if (CoordSystemComboBox->itemData(i) == coordSys.type())
        {
            CoordSystemComboBox->setCurrentIndex(i);
            break;
        }
    }

    QString myBody = comboBoxCentralBody->currentText();
    StaBody* mySTABody = STA_SOLAR_SYSTEM->lookup(myBody);

    // Load values from the schema into the GUI to the Keplerian elements
    ScenarioAbstract6DOFPositionType* position = initPosition->Abstract6DOFPosition().data();

    ScenarioKeplerianElementsType* elements = dynamic_cast<ScenarioKeplerianElementsType*>(position);
    semimajorAxisEdit->setText(QString::number(elements->semiMajorAxis()));
    eccentricityEdit->setText(QString::number(elements->eccentricity()));
    inclinationEdit->setText(QString::number(sta::radToDeg(elements->inclination())));
    raanEdit->setText(QString::number(sta::radToDeg(elements->RAAN())));
    argOfPeriapsisEdit->setText(QString::number(sta::radToDeg(elements->argumentOfPeriapsis())));
    trueAnomalyEdit->setText(QString::number(sta::radToDeg(elements->trueAnomaly())));

    // We need to convert the Keplerian elements into Cartesian and load the corresponding edit-label-combos of the GUI
    double meanAnomaly = trueAnomalyTOmeanAnomaly(elements->trueAnomaly(), elements->eccentricity());

    sta::StateVector myStateVector = orbitalTOcartesian(mySTABody->mu(),
                                                        elements->semiMajorAxis(),
                                                        elements->eccentricity(),
                                                        elements->inclination(),
                                                        elements->argumentOfPeriapsis(),
                                                        elements->RAAN(),
                                                        meanAnomaly);

    positionXEdit->setText(QString::number(myStateVector.position(0)));
    positionYEdit->setText(QString::number(myStateVector.position(1)));
    positionZEdit->setText(QString::number(myStateVector.position(2)));
    velocityXEdit->setText(QString::number(myStateVector.velocity(0)));
    velocityYEdit->setText(QString::number(myStateVector.velocity(1)));
    velocityZEdit->setText(QString::number(myStateVector.velocity(2)));

    return true;
}

bool LoiteringDialog::loadValues(ScenarioPropagationAttitudeType *propagation)
{
    QString currentPropagator = propagation->propagator().trimmed();
    for (int i = 0; i < PropagatorAttitudeComboBox->count(); i++)
    {
        if (PropagatorAttitudeComboBox->itemData(i) == currentPropagator)
        {
            PropagatorAttitudeComboBox->setCurrentIndex(i);
            break;
        }
    }

    QString currentIntegrator = propagation->integrator().trimmed();
    for (int i = 0; i < IntegratorAttitudeComboBox->count(); i++)
    {
        if (IntegratorAttitudeComboBox->itemData(i) == currentIntegrator)
        {
            IntegratorAttitudeComboBox->setCurrentIndex(i);
            break;
        }
    }

    IntAttitudeStep->setText(QString::number(propagation->timeStep()));
    return true;
}

bool LoiteringDialog::loadValues(ScenarioInitialAttitudeType* initAttitude)
{
    // Load Euler angles 123 from schema into the GUI
    ScenarioAbstract6DOFAttitudeType* attitude = initAttitude->Abstract6DOFAttitude().data();
    ScenarioEulerType* Euler123 = dynamic_cast<ScenarioEulerType*>(attitude);
    Euler123phi->setText(QString::number(sta::radToDeg(Euler123->phi())));
    Euler123theta->setText(QString::number(sta::radToDeg(Euler123->theta())));
    Euler123psi->setText(QString::number(sta::radToDeg(Euler123->psi())));
    Euler123omegaX->setText(QString::number(Euler123->phiDot()));
    Euler123omegaY->setText(QString::number(Euler123->thetaDot()));
    Euler123omegaZ->setText(QString::number(Euler123->psiDot()));

    // Converting now from Euler 123 (nominal representation) into the other representations
    Vector3d eulerAngles(sta::radToDeg(Euler123->phi()), sta::radToDeg(Euler123->theta()), sta::radToDeg(Euler123->psi()));
    Vector3d bodyRates(Euler123->phiDot(), Euler123->thetaDot(), Euler123->psiDot());
    Quaterniond initQuaternion = ToQuaternions(eulerAngles, 1, 2, 3);
    // Euler 321
    Euler321phi->setText(QString::number(sta::radToDeg(ToEulerAngles(initQuaternion, 3, 2, 1)[0])));
    Euler321psi->setText(QString::number(sta::radToDeg(ToEulerAngles(initQuaternion, 3, 2, 1)[2])));
    Euler321theta->setText(QString::number(sta::radToDeg(ToEulerAngles(initQuaternion, 3, 2, 1)[1])));
    Euler321omegaX->setText(QString::number(bodyRates.x()));     //these are not the Euler rates, but the BODY rates
    Euler321omegaY->setText(QString::number(bodyRates.y()));
    Euler321omegaZ->setText(QString::number(bodyRates.z()));
    // Euler 313
    Euler313phi->setText(QString::number(ToEulerAngles(initQuaternion, 3, 1, 3)[0]));
    Euler313psi->setText(QString::number(ToEulerAngles(initQuaternion, 3, 1, 3)[2]));
    Euler313theta->setText(QString::number(ToEulerAngles(initQuaternion, 3, 1, 3)[1]));
    Euler313omegaX->setText(QString::number(bodyRates.x()));     //these are not the Euler rates, but the BODY rates
    Euler313omegaY->setText(QString::number(bodyRates.y()));
    Euler313omegaZ->setText(QString::number(bodyRates.z()));

    return true;
}

bool LoiteringDialog::loadValues(ScenarioInitialAttitudeUsingQuaternionsType* initAttQuaternions)
{
    // Load quaternions ESA from schema into the GUI
    ScenarioAbstract8DOFAttitudeType* attitude = initAttQuaternions->Abstract8DOFAttitude().data();
    ScenarioQuaternionType* myQuaternion = dynamic_cast<ScenarioQuaternionType*>(attitude);
    q1->setText(QString::number(myQuaternion->q1()));
    q2->setText(QString::number(myQuaternion->q2()));
    q3->setText(QString::number(myQuaternion->q3()));
    q4->setText(QString::number(myQuaternion->q4()));
    q1dot->setText(QString::number(myQuaternion->q1Dot()));
    q2dot->setText(QString::number(myQuaternion->q2Dot()));
    q3dot->setText(QString::number(myQuaternion->q3Dot()));
    q4dot->setText(QString::number(myQuaternion->q4Dot()));
    // Converting from Quetrnions ESA (nominal representation) into the other representations (e.g. Quaternions JPL)
    q1JPL->setText(QString::number(myQuaternion->q4()));
    q2JPL->setText(QString::number(myQuaternion->q2()));
    q3JPL->setText(QString::number(myQuaternion->q3()));
    q4JPL->setText(QString::number(myQuaternion->q1()));
    q1dotJPL->setText(QString::number(myQuaternion->q4Dot()));
    q2dotJPL->setText(QString::number(myQuaternion->q2Dot()));
    q3dotJPL->setText(QString::number(myQuaternion->q3Dot()));
    q4dotJPL->setText(QString::number(myQuaternion->q1Dot()));

    return true;
}

bool LoiteringDialog::saveValues(ScenarioLoiteringType* loitering)
{
    ScenarioElementIdentifierType*     identifier           =   loitering->ElementIdentifier().data();
    ScenarioEnvironmentType*           environment	         =   loitering->Environment().data();
    ScenarioTimeLine*                  parameters	         =   loitering->TimeLine().data();
    ScenarioPropagationPositionType*   propagation          =   loitering->PropagationPosition().data();
    ScenarioInitialPositionType*       initPosition         =   loitering->InitialPosition().data(); //Dominik
    ScenarioPropagationAttitudeType*   attitudePropagation  =   loitering->PropagationAttitude().data(); //Added by Catarina
    ScenarioInitialAttitudeType*       initAttitude  =   loitering->InitialAttitude().data(); //Added by Catarina
    ScenarioInitialAttitudeUsingQuaternionsType * initAttQuaternions = loitering->InitialAttitudeUsingQuaternions().data();   // Guillermo

    if (saveValues(identifier) && saveValues(environment) && saveValues(parameters) && saveValues(propagation) &&
        saveValues(initPosition) && saveValues(attitudePropagation) && saveValues(initAttitude, initAttQuaternions))
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool LoiteringDialog::saveValues(ScenarioElementIdentifierType* arcIdentifier)
{
    // The arc name
    QString theArcName = loiteringAspect.saveValueArcName();
    arcIdentifier->setName(theArcName);

    // The color
    QString theColorName = loiteringAspect.saveValueArcColor();
    arcIdentifier->setColorName(theColorName);

    // The model
    QString theModelName = loiteringAspect.saveValueArcModel();
    arcIdentifier->setModelName(theModelName);

    return true;
}

bool LoiteringDialog::saveValues(ScenarioEnvironmentType* environment)
{
    // Create the perturbations object if it doesn't exist already (it is
    // empty by default.)
    if (environment->PerturbationsToCentralBody().isNull())
    {
        environment->setPerturbationsToCentralBody(QSharedPointer<ScenarioPerturbationsType>(new ScenarioPerturbationsType()));
    }

    // The central body
    QString centralBodyName = comboBoxCentralBody->currentText();
    environment->CentralBody()->setName(centralBodyName);

    myPerturbationsForces.saveValues(environment->CentralBody().data(),
                                     environment->PerturbationsToCentralBody().data());

    return true;
}

bool LoiteringDialog::saveValues(ScenarioTimeLine* timeline)
{
    timeline->setStartTime(trajectoryStartingEpoch->dateTime());
    timeline->setEndTime(trajectoryEndingEpoch->dateTime());
    //timeline->setStepTime(TimeStepLineEdit->text().toDouble());
    timeline->setStartTime(attitudeStartingEpoch->dateTime());
    timeline->setEndTime(attitudeEndingEpoch->dateTime());

    return true;
}

bool LoiteringDialog::saveValues(ScenarioInitialPositionType* initPos)
{
    sta::CoordinateSystemType coordSysType =
            sta::CoordinateSystemType(CoordSystemComboBox->itemData(CoordSystemComboBox->currentIndex()).toInt());
    initPos->setCoordinateSystem(sta::CoordinateSystem(coordSysType).name());

    switch (InitialStateComboBox->currentIndex())
    {
    case 0: // --- KEPLERIAN
        {
            // Takes the GUI values and stores them into the scenario
            ScenarioKeplerianElementsType* keplerianElements = new ScenarioKeplerianElementsType;
            keplerianElements->setSemiMajorAxis(semimajorAxisEdit->text().toDouble());
            keplerianElements->setEccentricity(eccentricityEdit->text().toDouble());
            keplerianElements->setInclination(sta::degToRad(inclinationEdit->text().toDouble()));
            keplerianElements->setRAAN(sta::degToRad(raanEdit->text().toDouble()));
            keplerianElements->setArgumentOfPeriapsis(sta::degToRad(argOfPeriapsisEdit->text().toDouble()));
            keplerianElements->setTrueAnomaly(sta::degToRad(trueAnomalyEdit->text().toDouble()));

            initPos->setAbstract6DOFPosition(QSharedPointer<ScenarioAbstract6DOFPositionType>(keplerianElements));
            return true;
        }

    case 1:  // --- STATE VECTOR
        {
            sta::StateVector myStateVector; sta::KeplerianElements myKeplerianElements;

            QString myBody = comboBoxCentralBody->currentText();
            StaBody* mySTABody = STA_SOLAR_SYSTEM->lookup(myBody);

            //-- Read state vector and write keplerian
            myStateVector.position(0) = positionXEdit->text().toDouble();
            myStateVector.position(1) = positionYEdit->text().toDouble();
            myStateVector.position(2) = positionZEdit->text().toDouble();
            myStateVector.velocity(0) = velocityXEdit->text().toDouble();
            myStateVector.velocity(1) = velocityYEdit->text().toDouble();
            myStateVector.velocity(2) = velocityZEdit->text().toDouble();

            myKeplerianElements = cartesianTOorbital(mySTABody->mu(), myStateVector);

            // write keplerian
            semimajorAxisEdit->setText(QString::number(myKeplerianElements.SemimajorAxis));
            eccentricityEdit->setText(QString::number(sta::radToDeg(myKeplerianElements.Eccentricity)));
            inclinationEdit->setText(QString::number(sta::radToDeg(myKeplerianElements.Inclination)));
            raanEdit->setText(QString::number(sta::radToDeg(myKeplerianElements.AscendingNode)));
            argOfPeriapsisEdit->setText(QString::number(sta::radToDeg(myKeplerianElements.ArgumentOfPeriapsis)));
            trueAnomalyEdit->setText(QString::number(sta::radToDeg(myKeplerianElements.TrueAnomaly)));

            ScenarioKeplerianElementsType* keplerianElements = new ScenarioKeplerianElementsType;
            keplerianElements->setSemiMajorAxis(myKeplerianElements.SemimajorAxis);
            keplerianElements->setEccentricity(myKeplerianElements.Eccentricity);
            keplerianElements->setInclination(myKeplerianElements.Inclination);
            keplerianElements->setRAAN(myKeplerianElements.AscendingNode);
            keplerianElements->setArgumentOfPeriapsis(myKeplerianElements.ArgumentOfPeriapsis);
            keplerianElements->setTrueAnomaly(myKeplerianElements.TrueAnomaly);

            initPos->setAbstract6DOFPosition(QSharedPointer<ScenarioAbstract6DOFPositionType>(keplerianElements));
            return true;
        }
    default:
        return false;
    }
}

bool LoiteringDialog::saveValues(ScenarioInitialAttitudeType* initAtt, ScenarioInitialAttitudeUsingQuaternionsType* initAttQuaternions)
{
//    sta::CoordinateSystemType coordSysType =
//            sta::CoordinateSystemType(CoordSystemAttitudeComboBox->itemData(CoordSystemAttitudeComboBox->currentIndex()).toInt());
//    initAtt->setCoordinateSystem(sta::CoordinateSystem(coordSysType).name());

    //qDebug() << "Comboboc index: " << AttitudeTypeComboBox->currentIndex() <<endl;

    switch (AttitudeTypeComboBox->currentIndex())
    {

    case 0:  // --- Euler 123
        {
            // Get the Euler 123 from the GUI
            ScenarioEulerType* euler = new ScenarioEulerType;
            euler->setPhi(sta::degToRad(Euler123phi->text().toDouble()));
            euler->setTheta(sta::degToRad(Euler123theta->text().toDouble()));
            euler->setPsi(sta::degToRad(Euler123psi->text().toDouble()));
            euler->setPhiDot(sta::degToRad(Euler123omegaX->text().toDouble()));
            euler->setThetaDot(sta::degToRad(Euler123omegaY->text().toDouble()));
            euler->setPsiDot(sta::degToRad(Euler123omegaZ->text().toDouble()));
            // Save the result of Euler123 into the schema
            initAtt->setAbstract6DOFAttitude(QSharedPointer<ScenarioAbstract6DOFAttitudeType>(euler));

            return true;
        }
    case 1:  // --- Euler 321
        {
            // Get the Euler 321 from the GUI
            ScenarioEulerType* euler = new ScenarioEulerType;
            euler->setPhi(sta::degToRad(Euler321phi->text().toDouble()));
            euler->setTheta(sta::degToRad(Euler321theta->text().toDouble()));
            euler->setPsi(sta::degToRad(Euler321psi->text().toDouble()));
            euler->setPhiDot(sta::degToRad(Euler321omegaX->text().toDouble()));
            euler->setThetaDot(sta::degToRad(Euler321omegaY->text().toDouble()));
            euler->setPsiDot(sta::degToRad(Euler321omegaZ->text().toDouble()));

            // Transform Euler321->Euler123 and load the GUI of Euler123
            Vector3d eulerAngles(sta::radToDeg(euler->phi()), sta::radToDeg(euler->theta()), sta::radToDeg(euler->psi()));
            Vector3d bodyRates(euler->phiDot(), euler->thetaDot(), euler->psiDot());
            Quaterniond initQuaternion = ToQuaternions(eulerAngles, 3, 2, 1);
            Euler123phi->setText(QString::number(sta::radToDeg(ToEulerAngles(initQuaternion, 1, 2, 3)[0])));
            Euler123psi->setText(QString::number(sta::radToDeg(ToEulerAngles(initQuaternion, 1, 2, 3)[2])));
            Euler123theta->setText(QString::number(sta::radToDeg(ToEulerAngles(initQuaternion, 1, 2, 3)[1])));
            Euler123omegaX->setText(QString::number(bodyRates.x()));     //these are not the Euler rates, but the BODY rates
            Euler123omegaY->setText(QString::number(bodyRates.y()));
            Euler123omegaZ->setText(QString::number(bodyRates.z()));

            // Transform Euler321->Euler313 and load the GUI of Euler313
            Euler313phi->setText(QString::number(sta::radToDeg(ToEulerAngles(initQuaternion, 3, 1, 3)[0])));
            Euler313psi->setText(QString::number(sta::radToDeg(ToEulerAngles(initQuaternion, 3, 1, 3)[2])));
            Euler313theta->setText(QString::number(sta::radToDeg(ToEulerAngles(initQuaternion, 3, 1, 3)[1])));
            Euler313omegaX->setText(QString::number(bodyRates.x()));     //these are not the Euler rates, but the BODY rates
            Euler313omegaY->setText(QString::number(bodyRates.y()));
            Euler313omegaZ->setText(QString::number(bodyRates.z()));

            // Save the result of Euler123 into the schema
            euler->setPhi(sta::degToRad(Euler123phi->text().toDouble()));
            euler->setTheta(sta::degToRad(Euler123theta->text().toDouble()));
            euler->setPsi(sta::degToRad(Euler123psi->text().toDouble()));
            euler->setPhiDot(sta::degToRad(Euler123omegaX->text().toDouble()));
            euler->setThetaDot(sta::degToRad(Euler123omegaY->text().toDouble()));
            euler->setPsiDot(sta::degToRad(Euler123omegaZ->text().toDouble()));
            initAtt->setAbstract6DOFAttitude(QSharedPointer<ScenarioAbstract6DOFAttitudeType>(euler));

            // Update the GUI of the quaternions ESA
            q1->setText(QString::number(initQuaternion.w()));
            q2->setText(QString::number(initQuaternion.x()));
            q3->setText(QString::number(initQuaternion.y()));
            q4->setText(QString::number(initQuaternion.z()));
            q1dot->setText(QString::number(initQuaternion.w()));
            q2dot->setText(QString::number(initQuaternion.x()));
            q3dot->setText(QString::number(initQuaternion.y()));
            q4dot->setText(QString::number(initQuaternion.z()));
            q1JPL->setText(QString::number(initQuaternion.z()));
            q2JPL->setText(QString::number(initQuaternion.x()));
            q3JPL->setText(QString::number(initQuaternion.y()));
            q4JPL->setText(QString::number(initQuaternion.w()));
            q1dotJPL->setText(QString::number(initQuaternion.z()));
            q2dotJPL->setText(QString::number(initQuaternion.x()));
            q3dotJPL->setText(QString::number(initQuaternion.y()));
            q4dotJPL->setText(QString::number(initQuaternion.w()));

            // and save the quaternions into the schema
            ScenarioQuaternionType* quat = new ScenarioQuaternionType;
            quat->setQ1(q1->text().toDouble());
            quat->setQ2(q2->text().toDouble());
            quat->setQ3(q3->text().toDouble());
            quat->setQ4(q4->text().toDouble());
            quat->setQ1Dot(q1dot->text().toDouble());
            quat->setQ2Dot(q2dot->text().toDouble());
            quat->setQ3Dot(q3dot->text().toDouble());
            quat->setQ4Dot(q4dot->text().toDouble());
            initAttQuaternions->setAbstract8DOFAttitude(QSharedPointer<ScenarioAbstract8DOFAttitudeType>(quat));

            return true;
        }
    case 2:  // --- Euler 313
        {
            // Get the Euler 313 from the GUI
            ScenarioEulerType* euler = new ScenarioEulerType;
            euler->setPhi(sta::degToRad(Euler313phi->text().toDouble()));
            euler->setTheta(sta::degToRad(Euler313theta->text().toDouble()));
            euler->setPsi(sta::degToRad(Euler313psi->text().toDouble()));
            euler->setPhiDot(sta::degToRad(Euler313omegaX->text().toDouble()));
            euler->setThetaDot(sta::degToRad(Euler313omegaY->text().toDouble()));
            euler->setPsiDot(sta::degToRad(Euler313omegaZ->text().toDouble()));

            // Transform Euler313->Euler123 and load the GUI of Euler123
            Vector3d eulerAngles(sta::radToDeg(euler->phi()), sta::radToDeg(euler->theta()), sta::radToDeg(euler->psi()));
            Vector3d bodyRates(euler->phiDot(), euler->thetaDot(), euler->psiDot());
            Quaterniond initQuaternion = ToQuaternions(eulerAngles, 3, 1, 3);
            Euler123phi->setText(QString::number(sta::radToDeg(ToEulerAngles(initQuaternion, 1, 2, 3)[0])));
            Euler123psi->setText(QString::number(sta::radToDeg(ToEulerAngles(initQuaternion, 1, 2, 3)[2])));
            Euler123theta->setText(QString::number(sta::radToDeg(ToEulerAngles(initQuaternion, 1, 2, 3)[1])));
            Euler123omegaX->setText(QString::number(bodyRates.x()));     //these are not the Euler rates, but the BODY rates
            Euler123omegaY->setText(QString::number(bodyRates.y()));
            Euler123omegaZ->setText(QString::number(bodyRates.z()));

            // Transform Euler313->Euler321 and load the GUI of Euler321
            Euler321phi->setText(QString::number(sta::radToDeg(ToEulerAngles(initQuaternion, 3, 2, 1)[0])));
            Euler321psi->setText(QString::number(sta::radToDeg(ToEulerAngles(initQuaternion, 3, 2, 1)[2])));
            Euler321theta->setText(QString::number(sta::radToDeg(ToEulerAngles(initQuaternion, 3, 2, 1)[1])));
            Euler321omegaX->setText(QString::number(bodyRates.x()));     //these are not the Euler rates, but the BODY rates
            Euler321omegaY->setText(QString::number(bodyRates.y()));
            Euler321omegaZ->setText(QString::number(bodyRates.z()));

            // Update the GUI of the quaternions ESA
            q1->setText(QString::number(initQuaternion.w()));
            q2->setText(QString::number(initQuaternion.x()));
            q3->setText(QString::number(initQuaternion.y()));
            q4->setText(QString::number(initQuaternion.z()));
            q1dot->setText(QString::number(initQuaternion.w()));
            q2dot->setText(QString::number(initQuaternion.x()));
            q3dot->setText(QString::number(initQuaternion.y()));
            q4dot->setText(QString::number(initQuaternion.z()));
            q1JPL->setText(QString::number(initQuaternion.z()));
            q2JPL->setText(QString::number(initQuaternion.x()));
            q3JPL->setText(QString::number(initQuaternion.y()));
            q4JPL->setText(QString::number(initQuaternion.w()));
            q1dotJPL->setText(QString::number(initQuaternion.z()));
            q2dotJPL->setText(QString::number(initQuaternion.x()));
            q3dotJPL->setText(QString::number(initQuaternion.y()));
            q4dotJPL->setText(QString::number(initQuaternion.w()));

            // and save the quaternions into the schema
            ScenarioQuaternionType* quat = new ScenarioQuaternionType;
            quat->setQ1(q1->text().toDouble());
            quat->setQ2(q2->text().toDouble());
            quat->setQ3(q3->text().toDouble());
            quat->setQ4(q4->text().toDouble());
            quat->setQ1Dot(q1dot->text().toDouble());
            quat->setQ2Dot(q2dot->text().toDouble());
            quat->setQ3Dot(q3dot->text().toDouble());
            quat->setQ4Dot(q4dot->text().toDouble());
            initAttQuaternions->setAbstract8DOFAttitude(QSharedPointer<ScenarioAbstract8DOFAttitudeType>(quat));

            // Save the result of Euler123 into the schema
            euler->setPhi(sta::degToRad(Euler123phi->text().toDouble()));
            euler->setTheta(sta::degToRad(Euler123theta->text().toDouble()));
            euler->setPsi(sta::degToRad(Euler123psi->text().toDouble()));
            euler->setPhiDot(sta::degToRad(Euler123omegaX->text().toDouble()));
            euler->setThetaDot(sta::degToRad(Euler123omegaY->text().toDouble()));
            euler->setPsiDot(sta::degToRad(Euler123omegaZ->text().toDouble()));
            initAtt->setAbstract6DOFAttitude(QSharedPointer<ScenarioAbstract6DOFAttitudeType>(euler));

            return true;
        }
    case 3:
        {
            // Get the quaternions ESA from the GUI
            ScenarioQuaternionType* quat = new ScenarioQuaternionType;
            quat->setQ1(q1->text().toDouble());
            quat->setQ2(q2->text().toDouble());
            quat->setQ3(q3->text().toDouble());
            quat->setQ4(q4->text().toDouble());
            quat->setQ1Dot(q1dot->text().toDouble());
            quat->setQ2Dot(q2dot->text().toDouble());
            quat->setQ3Dot(q3dot->text().toDouble());
            quat->setQ4Dot(q4dot->text().toDouble());
            // and save them to the schema
            initAttQuaternions->setAbstract8DOFAttitude(QSharedPointer<ScenarioAbstract8DOFAttitudeType>(quat));

            // Update the other panels
            Quaterniond initQuaternion; Quaterniond bodyRates;
            initQuaternion.w() = quat->q1(); initQuaternion.x() = quat->q2(); initQuaternion.y() = quat->q3(); initQuaternion.z() = quat->q4();
            bodyRates.w() = quat->q1Dot(); bodyRates.x() = quat->q2Dot(); bodyRates.y() = quat->q3Dot(); bodyRates.z() = quat->q4Dot();
            // Euler 123
            Euler123phi->setText(QString::number(sta::radToDeg(ToEulerAngles(initQuaternion, 1, 2, 3)[0])));
            Euler123psi->setText(QString::number(sta::radToDeg(ToEulerAngles(initQuaternion, 1, 2, 3)[2])));
            Euler123theta->setText(QString::number(sta::radToDeg(ToEulerAngles(initQuaternion, 1, 2, 3)[1])));
            Euler123omegaX->setText(QString::number(bodyRates.x()));     //these are not the Euler rates, but the BODY rates
            Euler123omegaY->setText(QString::number(bodyRates.y()));
            Euler123omegaZ->setText(QString::number(bodyRates.z()));
            // Euler 321
            Euler321phi->setText(QString::number(sta::radToDeg(ToEulerAngles(initQuaternion, 3, 2, 1)[0])));
            Euler321psi->setText(QString::number(sta::radToDeg(ToEulerAngles(initQuaternion, 3, 2, 1)[2])));
            Euler321theta->setText(QString::number(sta::radToDeg(ToEulerAngles(initQuaternion, 3, 2, 1)[1])));
            Euler321omegaX->setText(QString::number(bodyRates.x()));     //these are not the Euler rates, but the BODY rates
            Euler321omegaY->setText(QString::number(bodyRates.y()));
            Euler321omegaZ->setText(QString::number(bodyRates.z()));
            // Euler 313
            Euler313phi->setText(QString::number(sta::radToDeg(ToEulerAngles(initQuaternion, 3, 2, 1)[0])));
            Euler313psi->setText(QString::number(sta::radToDeg(ToEulerAngles(initQuaternion, 3, 2, 1)[2])));
            Euler313theta->setText(QString::number(sta::radToDeg(ToEulerAngles(initQuaternion, 3, 2, 1)[1])));
            Euler313omegaX->setText(QString::number(bodyRates.x()));     //these are not the Euler rates, but the BODY rates
            Euler313omegaY->setText(QString::number(bodyRates.y()));
            Euler313omegaZ->setText(QString::number(bodyRates.z()));

        }
    case 4:
        {
            // Get the quaternions JPL from the GUI
            ScenarioQuaternionType* quat = new ScenarioQuaternionType;
            quat->setQ1(q1JPL->text().toDouble());
            quat->setQ2(q2JPL->text().toDouble());
            quat->setQ3(q3JPL->text().toDouble());
            quat->setQ4(q4JPL->text().toDouble());
            quat->setQ1Dot(q1dotJPL->text().toDouble());
            quat->setQ2Dot(q2dotJPL->text().toDouble());
            quat->setQ3Dot(q3dotJPL->text().toDouble());
            quat->setQ4Dot(q4dotJPL->text().toDouble());
            // and transform them into quaternions ESA and updating the GUI
            q1->setText(QString::number(quat->q4()));
            q2->setText(QString::number(quat->q2()));
            q3->setText(QString::number(quat->q3()));
            q4->setText(QString::number(quat->q1()));
            q1dot->setText(QString::number(quat->q4Dot()));
            q2dot->setText(QString::number(quat->q2Dot()));
            q3dot->setText(QString::number(quat->q3Dot()));
            q4dot->setText(QString::number(quat->q1Dot()));
            // Saving the result in the schema
            quat->setQ1(q1->text().toDouble());
            quat->setQ2(q2->text().toDouble());
            quat->setQ3(q3->text().toDouble());
            quat->setQ4(q4->text().toDouble());
            quat->setQ1Dot(q1dot->text().toDouble());
            quat->setQ2Dot(q2dot->text().toDouble());
            quat->setQ3Dot(q3dot->text().toDouble());
            quat->setQ4Dot(q4dot->text().toDouble());
            initAttQuaternions->setAbstract8DOFAttitude(QSharedPointer<ScenarioAbstract8DOFAttitudeType>(quat));

            // Update the other panels
            Quaterniond initQuaternion; Quaterniond bodyRates;
            initQuaternion.w() = quat->q1(); initQuaternion.x() = quat->q2(); initQuaternion.y() = quat->q3(); initQuaternion.z() = quat->q4();
            bodyRates.w() = quat->q1Dot(); bodyRates.x() = quat->q2Dot(); bodyRates.y() = quat->q3Dot(); bodyRates.z() = quat->q4Dot();
            // Euler 123
            Euler123phi->setText(QString::number(sta::radToDeg(ToEulerAngles(initQuaternion, 1, 2, 3)[0])));
            Euler123psi->setText(QString::number(sta::radToDeg(ToEulerAngles(initQuaternion, 1, 2, 3)[2])));
            Euler123theta->setText(QString::number(sta::radToDeg(ToEulerAngles(initQuaternion, 1, 2, 3)[1])));
            Euler123omegaX->setText(QString::number(bodyRates.x()));     //these are not the Euler rates, but the BODY rates
            Euler123omegaY->setText(QString::number(bodyRates.y()));
            Euler123omegaZ->setText(QString::number(bodyRates.z()));
            // Euler 321
            Euler321phi->setText(QString::number(sta::radToDeg(ToEulerAngles(initQuaternion, 3, 2, 1)[0])));
            Euler321psi->setText(QString::number(sta::radToDeg(ToEulerAngles(initQuaternion, 3, 2, 1)[2])));
            Euler321theta->setText(QString::number(sta::radToDeg(ToEulerAngles(initQuaternion, 3, 2, 1)[1])));
            Euler321omegaX->setText(QString::number(bodyRates.x()));     //these are not the Euler rates, but the BODY rates
            Euler321omegaY->setText(QString::number(bodyRates.y()));
            Euler321omegaZ->setText(QString::number(bodyRates.z()));
            // Euler 313
            Euler313phi->setText(QString::number(sta::radToDeg(ToEulerAngles(initQuaternion, 3, 2, 1)[0])));
            Euler313psi->setText(QString::number(sta::radToDeg(ToEulerAngles(initQuaternion, 3, 2, 1)[2])));
            Euler313theta->setText(QString::number(sta::radToDeg(ToEulerAngles(initQuaternion, 3, 2, 1)[1])));
            Euler313omegaX->setText(QString::number(bodyRates.x()));     //these are not the Euler rates, but the BODY rates
            Euler313omegaY->setText(QString::number(bodyRates.y()));
            Euler313omegaZ->setText(QString::number(bodyRates.z()));
        }
    default:
        return false;
    }
}


bool LoiteringDialog::saveValues(ScenarioPropagationPositionType* propagation)
{
    propagation->setPropagator(PropagatorComboBox->itemData(PropagatorComboBox->currentIndex()).toString());
    propagation->setIntegrator(IntegratorComboBox->itemData(IntegratorComboBox->currentIndex()).toString());  
    propagation->setTimeStep(IntStepEdit->text().toDouble());

    return true;
}

bool LoiteringDialog::saveValues(ScenarioPropagationAttitudeType* propagation)
{
    propagation->setPropagator(PropagatorAttitudeComboBox->itemData(PropagatorAttitudeComboBox->currentIndex()).toString());
    propagation->setIntegrator(IntegratorAttitudeComboBox->itemData(IntegratorAttitudeComboBox->currentIndex()).toString());
    propagation->setTimeStep(IntAttitudeStep->text().toDouble());

    return true;
}




/////////////////////////////////////// PropagateLoiteringTrajectory /////////////////////////////
bool PropagateLoiteringTrajectory(ScenarioLoiteringType* loitering,
                                  QList<double>& sampleTimes,
                                  QList<sta::StateVector>& samples,
                                  PropagationFeedback& propFeedback)
{
    //Check the coordinate system of position
    QString loiteringLabel = loitering->ElementIdentifier()->Name();

    QString centralBodyName = loitering->Environment()->CentralBody()->Name();
    StaBody* centralBody = STA_SOLAR_SYSTEM->lookup(centralBodyName);
    if (!centralBody)
    {
        propFeedback.raiseError(QObject::tr("Unrecognized central body '%1'").arg(centralBodyName));
        return false;
    }

    QString coordSysName = loitering->InitialPosition()->CoordinateSystem();
    sta::CoordinateSystem coordSys(coordSysName);
    if (coordSys.type() == sta::COORDSYS_INVALID)
    {
        propFeedback.raiseError(QObject::tr("Unrecognized coordinate system '%1'").arg(coordSysName));
        return false;
    }

    // Get the initial state in two forms:
    //   - Keplerian elements for simple two-body propagation
    //   - A state vector for any other sort of propagation
    ScenarioAbstract6DOFPositionType* position = loitering->InitialPosition()->Abstract6DOFPosition().data();
    sta::KeplerianElements initialStateKeplerian = AbstractPositionToKeplerianElements(position, centralBody);                                   
    // We need to convert the Keplerian elements into Cartesian
    double meanAnomaly = trueAnomalyTOmeanAnomaly(initialStateKeplerian.TrueAnomaly, initialStateKeplerian.Eccentricity);
    sta::StateVector initialState = orbitalTOcartesian(centralBody->mu(),
                                                       initialStateKeplerian.SemimajorAxis,
                                                       initialStateKeplerian.Eccentricity,
                                                       initialStateKeplerian.Inclination,
                                                       initialStateKeplerian.ArgumentOfPeriapsis,
                                                       initialStateKeplerian.AscendingNode,
                                                       meanAnomaly);

    double mu = centralBody->mu();

    // Get the timeline information
    const ScenarioTimeLine* timeline = loitering->TimeLine().data();
    double timelineDuration = timeline->StartTime().secsTo(timeline->EndTime());
    double dt = loitering->PropagationPosition()->timeStep();

    if (dt == 0.0)
    {
        propFeedback.raiseError(QObject::tr("Time step is zero!"));
        return false;
    }

    // We don't output values at every integration step. Instead use the time step
    // from simulation parameters. The actual output step used will not necessarily
    // match the requested output step: the code below sets it to be an integer
    // multiple of the integration step.
    double requestedOutputTimeStep = timeline->StepTime();
    double outputTimeStep;
    unsigned int outputRate;
    if (requestedOutputTimeStep < dt)
    {
        outputRate = 1;
        outputTimeStep = dt;
    }
    else
    {
        outputRate = (unsigned int) floor(requestedOutputTimeStep / dt + 0.5);
        outputTimeStep = outputRate * dt;
    }

    if (timelineDuration / outputTimeStep > MAX_OUTPUT_STEPS)
    {
        propFeedback.raiseError(QObject::tr("Propagation steps exceeds %1. Increase the simulation time step.").arg(MAX_OUTPUT_STEPS));
        return false;
    }

    QList<Perturbations*> perturbationsList; // Create the list of perturbations that will influence the propagation

    sta::StateVector stateVector = initialState;

    // deviation, reference, and q will be used only in Encke propagation
    sta::StateVector deviation(Vector3d::Zero(), Vector3d::Zero());
    sta::StateVector reference = initialState;
    double q = 0.0;

    double startTime = sta::JdToMjd(sta::CalendarToJd(timeline->StartTime()));

    sampleTimes << startTime;
    samples << stateVector;

    QFile ciccio("data/PerturbationsData.stae");
    QTextStream cicciostream(&ciccio);
    ciccio.open(QIODevice::WriteOnly);

    //unsigned int steps = 0;
    unsigned int steps = 1; // patched by Ana on 18th June 2010

    QString propagator = loitering->PropagationPosition()->propagator();
    QString integrator = loitering->PropagationPosition()->integrator();

    if (propagator == "TWO BODY")
    {

        double sma            = initialStateKeplerian.SemimajorAxis;
        double e              = initialStateKeplerian.Eccentricity;
        double inclination    = initialStateKeplerian.Inclination;
        double raan           = initialStateKeplerian.AscendingNode;
        double argOfPeriapsis = initialStateKeplerian.ArgumentOfPeriapsis;
        double trueAnomaly    = initialStateKeplerian.TrueAnomaly;
        double meanAnomaly    = trueAnomalyTOmeanAnomaly(trueAnomaly, e);

        // Next lines patched by Guillermo on April 23 2010 to speed up calculations outside the for loop
        double argOfPeriapsisUpdated      = 0.0;
        double meanAnomalyUpdated         = 0.0;
        double raanUpdated                = 0.0;

        double perigee = sma * (1 - e);
        if (perigee < centralBody->meanRadius())
        {
            propFeedback.raiseError(QObject::tr("The perigee distance is smaller than the main body radius."));
            return false;
        }
        else
        {
            for (double t = dt; t < timelineDuration + dt; t += dt)
            {
                JulianDate jd = startTime + sta::secsToDays(t);

                stateVector = propagateTWObody(mu, sma, e, inclination, argOfPeriapsis, raan, meanAnomaly,
                                               dt,
                                               raanUpdated, argOfPeriapsisUpdated, meanAnomalyUpdated);

                argOfPeriapsis = argOfPeriapsisUpdated;
                meanAnomaly    = meanAnomalyUpdated;
                raan           = raanUpdated;

                // Append a trajectory sample every outputRate integration steps (and always at the last step.)
                if (steps % outputRate == 0 || t >= timelineDuration)
                {
                    sampleTimes << jd;
                    samples << stateVector;

                }
                ++steps;
            }
        }
    }
    else if (propagator == "COWELL")
    {
        for (double t = dt; t < timelineDuration + dt; t += dt)
        {
            JulianDate jd = startTime + sta::secsToDays(t);
            stateVector = propagateCOWELL(mu, stateVector, dt, perturbationsList, jd, integrator, propFeedback);

            // Append a trajectory sample every outputRate integration steps (and always at the last step.)
            if (steps % outputRate == 0 || t >= timelineDuration)
            {
                sampleTimes << jd;
                samples << stateVector;

            }
            ++steps;
        }
    }
    else if (propagator == "ENCKE")
    {
        double sma            = initialStateKeplerian.SemimajorAxis;
        double e              = initialStateKeplerian.Eccentricity;
        double inclination    = initialStateKeplerian.Inclination;
        double raan           = initialStateKeplerian.AscendingNode;
        double argOfPeriapsis = initialStateKeplerian.ArgumentOfPeriapsis;
        double meanAnomaly    = initialStateKeplerian.MeanAnomaly;

        for (double t = dt; t < timelineDuration + dt; t += dt)
        {
            JulianDate jd = startTime + sta::secsToDays(t);
            deviation = propagateENCKE(mu, reference, dt, perturbationsList, jd, stateVector, deviation,  q, integrator, propFeedback);

            // PropagateTWObody is used to propagate the reference trajectory
            double argOfPeriapsisUpdated      = 0.0;
            double meanAnomalyUpdated         = 0.0;
            double raanUpdated                = 0.0;
            reference = propagateTWObody(mu, sma, e, inclination, argOfPeriapsis, raan, meanAnomaly,
                                         dt,
                                         raanUpdated, argOfPeriapsisUpdated, meanAnomalyUpdated);

            argOfPeriapsis = argOfPeriapsisUpdated;
            meanAnomaly    = meanAnomalyUpdated;
            raan           = raanUpdated;

            // Calculating the perturbed trajectory
            stateVector = reference + deviation;
            q = deviation.position.dot(reference.position + 0.5 * deviation.position) / pow(reference.position.norm(), 2.0);

#if 0
            // Rectification of the reference trajectory, when the deviation is too large.
            if (q > 0.01)
            {
                sta::KeplerianElements keplerian = cartesianTOorbital(mu, stateVector);

                sma = keplerian.SemimajorAxis;
                e = keplerian.Eccentricity;
                inclination = keplerian.Inclination;
                argOfPeriapsis = keplerian.ArgumentOfPeriapsis;
                raan = keplerian.AscendingNode;
                meanAnomaly = keplerian.MeanAnomaly;

                q = 0;
                reference = stateVector;
                deviation = sta::StateVector(null, null);
            }
#endif

            // Append a trajectory sample every outputRate integration steps (and always at the last step.)
            if (steps % outputRate == 0 || t >= timelineDuration)
            {
                sampleTimes << jd;
                samples << stateVector;

            }
            ++steps;
        }
    }
    else if (propagator == "GAUSS")
    {
        for (double t = dt; t < timelineDuration + dt; t += dt)
        {
            JulianDate jd = startTime + sta::secsToDays(t);
            // Append a trajectory sample every outputRate integration steps (and always at the last step.)
            if (steps % outputRate == 0 || t >= timelineDuration)
            {
                sampleTimes << jd;
                samples << stateVector;

            }
            ++steps;

            stateVector = propagateGAUSS(mu, stateVector, dt, perturbationsList, jd, integrator);

        }
    }
    else
    {
        propFeedback.raiseError(QObject::tr("Unsupported propagator '%1'").arg(propagator));
        return false;
    }

    return true;

}


///////////////////////////////////// PropagateLoiteringAttitude /////////////////////////////
bool PropagateLoiteringAttitude(ScenarioLoiteringType* loitering,
                                QList<double>& sampleTimes,
                                QList<staAttitude::AttitudeVector>& samplesAtt,
                                PropagationFeedback& propFeedback)
{
    qDebug() << "Entering PropagateLoiteringAttitude" << endl;
    //Check the coordinate system of position
    QString loiteringLabel = loitering->ElementIdentifier()->Name();

    // Checking out the attitude coordinate system type
    QString attitudeEulerCoordSysName = loitering->InitialAttitude()->CoordinateSystem();
    QString attitudeQuaternionCoordSysName = loitering->InitialAttitudeUsingQuaternions()->CoordinateSystem();
//    if (attitudeEulerCoordSysName.type() == sta::COORDSYS_INVALID || attitudeQuaternionCoordSysName.type() == sta::COORDSYS_INVALID)
//    {
//        propFeedback.raiseError(QObject::tr("Unrecognized coordinate system '%1'").arg(attitudeCoordSysName));
//        return false;
//    }


    //ScenarioAbstract6DOFPositionType* position = loitering->InitialPosition()->Abstract6DOFPosition().data();
    //sta::StateVector initialState = AbstractPositionToStateVector(position, centralBody);


    // Get the initial state in several forms:
    //   - All Eulers elements the different Euler sequences propagations
    //   - Quaternions for quaternions propagation
    ScenarioAbstract6DOFAttitudeType* eulerAnglesAndRates = loitering->InitialAttitude()->Abstract6DOFAttitude().data();
    ScenarioAbstract8DOFAttitudeType* quaternionsAndRates = loitering->InitialAttitudeUsingQuaternions()->Abstract8DOFAttitude().data();
    staAttitude::AttitudeVector initialStateAttitudeEuler123 = Euler123ToAttitudeVector(eulerAnglesAndRates);
    staAttitude::AttitudeVector initialStateAttitudeEuler321 = Euler321ToAttitudeVector(eulerAnglesAndRates);
    staAttitude::AttitudeVector initialStateAttitudeEuler313 = Euler313ToAttitudeVector(eulerAnglesAndRates);
    staAttitude::AttitudeVector initialStateAttitudeQuaternionsESA = QuaternionESAToAttitudeVector(quaternionsAndRates);
    staAttitude::AttitudeVector initialStateAttitudeQuaternionsJPL = QuaternionJPLToAttitudeVector(quaternionsAndRates);

    // Get the timeline information
    const ScenarioTimeLine* timeline = loitering->TimeLine().data();
    double timelineDuration = timeline->StartTime().secsTo(timeline->EndTime());
    double dt = loitering->PropagationPosition()->timeStep();

    if (dt == 0.0)
    {
        propFeedback.raiseError(QObject::tr("Time step is zero!"));
        return false;
    }

    // We don't output values at every integration step. Instead use the time step
    // from simulation parameters. The actual output step used will not necessarily
    // match the requested output step: the code below sets it to be an integer
    // multiple of the integration step.
    double requestedOutputTimeStep = timeline->StepTime();
    double outputTimeStep;
    unsigned int outputRate;
    if (requestedOutputTimeStep < dt)
    {
        outputRate = 1;
        outputTimeStep = dt;
    }
    else
    {
        outputRate = (unsigned int) floor(requestedOutputTimeStep / dt + 0.5);
        outputTimeStep = outputRate * dt;
    }

    if (timelineDuration / outputTimeStep > MAX_OUTPUT_STEPS)
    {
        propFeedback.raiseError(QObject::tr("Propagation steps exceeds %1. Increase the simulation time step.").arg(MAX_OUTPUT_STEPS));
        return false;
    }


    // In the future place here after the attitude perturbations
    //QList<Perturbations*> perturbationsList; // Create the list of perturbations that will influence the propagation

    QString attitudePropagator = loitering->PropagationAttitude()->propagator();
    QString attitudeIntegrator = loitering->PropagationAttitude()->integrator();    

    ScenarioSC* mySatellite = new ScenarioSC();
    double inertiaMatrixCoeffs[9] =
    {mySatellite->System()->Structure()->MomentsOfInertia()->xAxis(), mySatellite->System()->Structure()->SecondMomentsOfInertia()->xAxis(), mySatellite->System()->Structure()->SecondMomentsOfInertia()->yAxis(),
     mySatellite->System()->Structure()->SecondMomentsOfInertia()->xAxis(), mySatellite->System()->Structure()->MomentsOfInertia()->yAxis(), mySatellite->System()->Structure()->SecondMomentsOfInertia()->zAxis(),
     mySatellite->System()->Structure()->SecondMomentsOfInertia()->yAxis(), mySatellite->System()->Structure()->SecondMomentsOfInertia()->zAxis(), mySatellite->System()->Structure()->MomentsOfInertia()->zAxis()};
    Matrix3d completeInertiaMatrix(inertiaMatrixCoeffs);

    VectorXd inertiaMatrix;
    inertiaMatrix(0,0) = mySatellite->System()->Structure()->MomentsOfInertia()->xAxis();
    inertiaMatrix(0,1) = mySatellite->System()->Structure()->MomentsOfInertia()->yAxis();
    inertiaMatrix(0,2) = mySatellite->System()->Structure()->MomentsOfInertia()->zAxis();
//    inertiaMatrix(0,1) = mySatellite->System()->Structure()->SecondMomentsOfInertia()->xAxis();
//    inertiaMatrix(0,2) = mySatellite->System()->Structure()->SecondMomentsOfInertia()->yAxis();
//    inertiaMatrix(1,2) = mySatellite->System()->Structure()->SecondMomentsOfInertia()->zAxis();

    if (attitudePropagator == "PEuler123")
    {
    }
    else if (attitudePropagator == "PEuler321")
    {
    }
    else if (attitudePropagator == "PEuler313")
    {
    }
    else if (attitudePropagator == "PEuler313")
    {
    }
    else if (attitudePropagator == "PQuaternionsESA")
    {
        staAttitude::AttitudeVector attitudeVector = initialStateAttitudeQuaternionsESA;

        double startTime = sta::JdToMjd(sta::CalendarToJd(timeline->StartTime()));

        sampleTimes << startTime;
        samplesAtt << attitudeVector;

        unsigned int steps = 1; // patched by Ana on 18th June 2010

        for (double t = dt; t < timelineDuration + dt; t += dt)
        {
            JulianDate jd = startTime + sta::secsToDays(t);

            // -- Integrating the attitude -- //

            //Integrating the body rates
            Vector3d bodyRates = propagateEulerEquation(attitudeVector.myBodyRates, startTime, dt, inertiaMatrix);

            //Integrating the quaternions
            Quaterniond integratedQuaternions = propagateQUATERNIONS(attitudeVector.myQuaternion, startTime, dt, bodyRates);

            //Saving integrated values to the new attitude vector
            attitudeVector.myQuaternion.coeffs().coeffRef(0) = integratedQuaternions.coeffs().coeffRef(0);
            attitudeVector.myQuaternion.coeffs().coeffRef(1) = integratedQuaternions.coeffs().coeffRef(1);
            attitudeVector.myQuaternion.coeffs().coeffRef(2) = integratedQuaternions.coeffs().coeffRef(2);
            attitudeVector.myQuaternion.coeffs().coeffRef(3) = integratedQuaternions.coeffs().coeffRef(3);
            attitudeVector.myBodyRates(0) = bodyRates(0);
            attitudeVector.myBodyRates(1) = bodyRates(1);
            attitudeVector.myBodyRates(2) = bodyRates(2);

            // -- End of: Integrating the attitude -- //

            // Append a trajectory sample every outputRate integration steps (and always at the last step.)
            if (steps % outputRate == 0 || t >= timelineDuration)
            {
                sampleTimes << jd;
                samplesAtt << attitudeVector;

            }
            ++steps;
        }
    }
    else if (attitudePropagator == "PQuaternionsJPL")
    {
        staAttitude::AttitudeVector attitudeVector = initialStateAttitudeQuaternionsJPL;

        double startTime = sta::JdToMjd(sta::CalendarToJd(timeline->StartTime()));

        sampleTimes << startTime;
        samplesAtt << attitudeVector;

        unsigned int steps = 1; // patched by Ana on 18th June 2010

        for (double t = dt; t < timelineDuration + dt; t += dt)
        {
            JulianDate jd = startTime + sta::secsToDays(t);

            // -- Integrating the attitude -- //

            //Integrating the body rates
            Vector3d bodyRates = propagateEulerEquation(attitudeVector.myBodyRates, startTime, dt, inertiaMatrix);

            //Integrating the quaternions
            Quaterniond integratedQuaternions = propagateQUATERNIONS(attitudeVector.myQuaternion, startTime, dt, bodyRates);

            //Saving integrated values to the new attitude vector
            attitudeVector.myQuaternion.coeffs().coeffRef(0) = integratedQuaternions.coeffs().coeffRef(0);
            attitudeVector.myQuaternion.coeffs().coeffRef(1) = integratedQuaternions.coeffs().coeffRef(1);
            attitudeVector.myQuaternion.coeffs().coeffRef(2) = integratedQuaternions.coeffs().coeffRef(2);
            attitudeVector.myQuaternion.coeffs().coeffRef(3) = integratedQuaternions.coeffs().coeffRef(3);
            attitudeVector.myBodyRates(0) = bodyRates(0);
            attitudeVector.myBodyRates(1) = bodyRates(1);
            attitudeVector.myBodyRates(2) = bodyRates(2);

            // -- End of: Integrating the attitude -- //

            // Append a trajectory sample every outputRate integration steps (and always at the last step.)
            if (steps % outputRate == 0 || t >= timelineDuration)
            {
                sampleTimes << jd;
                samplesAtt << attitudeVector;

            }
            ++steps;
        }
    }
    else
    {
        propFeedback.raiseError(QObject::tr("Unsupported attitude propagator '%1'").arg(attitudePropagator));
        return false;
    }

    return true;

}




///////////////////////////////////// Activation of puched buttons leading to other window dialogs /////////////////////////////
void LoiteringDialog::on_pushButtonAspect_clicked()
{
    loiteringAspect.exec();
}


void LoiteringDialog::on_perturbationForcesPushButton_clicked()
{
    myPerturbationsForces.exec();
}


void LoiteringDialog::on_perturbationTorquesPushButton_clicked()
{
    //myPerturbationsTorques.exec();
}


void LoiteringDialog::on_trajectoryStopConditionsPushButton_clicked()
{
    myStoppingConditionsTrajetory.exec();
}


void LoiteringDialog::on_attitudeStopConditionsPushButton_clicked()
{
    //myStoppingConditionsAttitude.exec();
}




