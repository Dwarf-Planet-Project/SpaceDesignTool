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
------------------ Author: Chris Laurel  -------------------------------------------------
 ------------------ E-mail: (claurel@gmail.com) ----------------------------
 */

#include "initialstateeditor.h"
#include "Scenario/scenario.h"
#include "Astro-Core/stacoordsys.h"


/* Dialog box for editing initial state parameters. The dialog
 * has a separate page for each method of entering the initial
 * state. Currently just two methods are supported: state vector
 * and Keplerian elements.
 */
InitialStateEditorDialog::InitialStateEditorDialog(QWidget* parent) :
    QDialog(parent)
{
    setupUi(this);

    // Set up the coordinate system combo box
    coordinateSystemCombo->addItem(tr("Planet fixed"), (int) sta::COORDSYS_BODYFIXED);
    coordinateSystemCombo->addItem(tr("Inertial (J2000)"), (int) sta::COORDSYS_EME_J2000);
    coordinateSystemCombo->addItem(tr("Inertial (B1950)"), (int) sta::COORDSYS_EME_B1950);
    coordinateSystemCombo->addItem(tr("Ecliptic (J2000)"), (int) sta::COORDSYS_ECLIPTIC_J2000);
    
    // Set up the input validators
    QDoubleValidator* doubleValidator = new QDoubleValidator(this);
    QDoubleValidator* angleValidator = new QDoubleValidator(this);
    angleValidator->setBottom(-360.0);
    angleValidator->setTop(360.0);
    QDoubleValidator* positiveAngleValidator = new QDoubleValidator(this);
    positiveAngleValidator->setBottom(0.0);
    positiveAngleValidator->setTop(360.0);
    QDoubleValidator* positiveDoubleValidator = new QDoubleValidator(this);
    positiveDoubleValidator->setBottom(0.0);
    QDoubleValidator* zeroToOneValidator = new QDoubleValidator(this);
    zeroToOneValidator->setBottom(0.0);
    zeroToOneValidator->setTop(0.9999);
    
    positionXEdit->setValidator(doubleValidator);
    positionYEdit->setValidator(doubleValidator);
    positionZEdit->setValidator(doubleValidator);
    velocityXEdit->setValidator(doubleValidator);
    velocityYEdit->setValidator(doubleValidator);
    velocityZEdit->setValidator(doubleValidator);
 
    semimajorAxisEdit->setValidator(positiveDoubleValidator);
    eccentricityEdit->setValidator(zeroToOneValidator);
    inclinationEdit->setValidator(angleValidator);
    raanEdit->setValidator(positiveAngleValidator);
    argOfPeriapsisEdit->setValidator(positiveAngleValidator);
    trueAnomalyEdit->setValidator(positiveAngleValidator);
}


InitialStateEditorDialog::~InitialStateEditorDialog()
{
}


// Load the edit box values from the current node values
bool InitialStateEditorDialog::loadValues(const ScenarioInitialStatePosition* initialStatePos)
{
#if OLDSCENARIO
    // Set the coordinate system combo box value
    for (int i = 0; i < coordinateSystemCombo->count(); i++)
    {
        if (coordinateSystemCombo->itemData(i) == initialStatePos->coordinateSystem().type())
        {
            coordinateSystemCombo->setCurrentIndex(i);
            break;
        }
    }
    
    ScenarioAbstractInitialState* initialState = initialStatePos->initialState();
    ScenarioKeplerianElementsType* elements = dynamic_cast<ScenarioKeplerianElementsType*>(initialState);
    ScenarioStateVector* stateVector = dynamic_cast<ScenarioStateVector*>(initialState);
    Q_ASSERT(elements || stateVector);
    if (elements != NULL)
    {
        initialStateTypeCombo->setCurrentIndex(1);
        initialStateStack->setCurrentWidget(keplerianElementsPage);
        
        semimajorAxisEdit->setText(QString::number(elements->semiMajorAxis()));
        eccentricityEdit->setText(QString::number(elements->eccentricity()));
        inclinationEdit->setText(QString::number(elements->inclination()));
        raanEdit->setText(QString::number(elements->RAAN()));
        argOfPeriapsisEdit->setText(QString::number(elements->argumentOfPeriapsis()));
        trueAnomalyEdit->setText(QString::number(elements->trueAnomaly()));
    }
    else if (stateVector != NULL)
    {
        initialStateTypeCombo->setCurrentIndex(0);
        initialStateStack->setCurrentWidget(stateVectorPage);

        positionXEdit->setText(QString::number(stateVector->position().x()));
        positionYEdit->setText(QString::number(stateVector->position().y()));
        positionZEdit->setText(QString::number(stateVector->position().z()));
        velocityXEdit->setText(QString::number(stateVector->velocity().x()));
        velocityYEdit->setText(QString::number(stateVector->velocity().y()));
        velocityZEdit->setText(QString::number(stateVector->velocity().z()));
    }
    else
    {
        // Unknown initial state type
        return false;
    }
#endif
    return true;
}


// Save edit box values to a tree of DOM nodes
bool InitialStateEditorDialog::saveValues(ScenarioInitialStatePosition* initialStatePos)
{
#if OLDSCENARIO
    int coordSysIndex = coordinateSystemCombo->currentIndex();
    initialStatePos->setCoordinateSystem((sta::CoordinateSystemType) coordinateSystemCombo->itemData(coordSysIndex).toInt());
    
    if (initialStateTypeCombo->currentIndex() == 0)
    {
        ScenarioStateVector* stateVector = new ScenarioStateVector();
        Eigen::Vector3d position(positionXEdit->text().toDouble(),
                                 positionYEdit->text().toDouble(),
                                 positionZEdit->text().toDouble());
        Eigen::Vector3d velocity(velocityXEdit->text().toDouble(),
                                 velocityYEdit->text().toDouble(),
                                 velocityZEdit->text().toDouble());

        stateVector->setPosition(position);
        stateVector->setVelocity(velocity);
        
        initialStatePos->setInitialState(stateVector);
    }
    else
    {
        ScenarioKeplerianElementsType* elements = new ScenarioKeplerianElementsType();
        elements->setSemiMajorAxis(semimajorAxisEdit->text().toDouble());
        elements->setEccentricity(eccentricityEdit->text().toDouble());
        elements->setInclination(inclinationEdit->text().toDouble());
        elements->setRAAN(raanEdit->text().toDouble());
        elements->setArgumentOfPeriapsis(argOfPeriapsisEdit->text().toDouble());
        elements->setTrueAnomaly(trueAnomalyEdit->text().toDouble());
        
        initialStatePos->setInitialState(elements);
    }
#endif

    return true;
}

