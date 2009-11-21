
/*
 This program is free software; you can redistribute it and/or modify it under
 the terms of the GNU Lesser General Public License as published by the Free Software
 Foundation; either version 2 of the License, or (at your option) any later
 version.

 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more
 details.

 You should have received a copy of the GNU Lesser General Public License along with
 this program; if not, write to the Free Software Foundation, Inc., 59 Temple
 Place - Suite 330, Boston, MA 02111-1307, USA.
 Further information about the GNU Lesser General Public License can also be found on
 the world wide web at http://www.gnu.org.
 */

/*
 ------ Copyright (C) 2009 European Space Agency (space.trajectory.analysis AT gmail.com) ----
 ------------------ Author: Valentino Zuccarelli  -------------------------------------------------
 ------------------ E-mail: (Valentino.Zuccarelli@gmail.com) ----------------------------
 */


#include "initialstateThreebodyEditor.h"
#include "Scenario/scenarioinitialstate.h"


/* Dialog box for editing initial state parameters. The dialog
 * has a separate page for each method of entering the initial
 * state. Currently just two methods are supported: state vector
 * and Keplerian elements.
 */
InitialStateThreebodyEditorDialog::InitialStateThreebodyEditorDialog(QWidget* parent) :
    QDialog(parent)
{
    setupUi(this);

    // Set up the coordinate system combo box
    //coordinateSystemCombo->addItem(tr("Planet fixed"), (int) sta::COORDSYS_BODYFIXED);
    coordinateSystemCombo->addItem(tr("Co-Rotating normalized"), (int) sta::COORDSYS_ROT_NORM);
    coordinateSystemCombo->addItem(tr("Co-Rotating"), (int) sta::COORDSYS_ROT);
    coordinateSystemCombo->addItem(tr("Inertial body-centred"), (int) sta::COORDSYS_BODYFIXED);
    //coordinateSystemCombo->addItem(tr("Ecliptic (J2000)"), (int) sta::COORDSYS_ECLIPTIC_J2000);

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


InitialStateThreebodyEditorDialog::~InitialStateThreebodyEditorDialog()
{
}


// Load the edit box values from the current node values
bool InitialStateThreebodyEditorDialog::loadValues(const ScenarioInitialStatePosition* initialState_threebody_Pos)
{
    // Set the coordinate system combo box value
    for (int i = 0; i < coordinateSystemCombo->count(); i++)
    {
        if (coordinateSystemCombo->itemData(i) == initialState_threebody_Pos->coordinateSystem().type())
        {
            coordinateSystemCombo->setCurrentIndex(i);
            break;
        }
    }

    ScenarioAbstractInitialState* initialState = initialState_threebody_Pos->initialState();
    ScenarioKeplerianElements* elements = dynamic_cast<ScenarioKeplerianElements*>(initialState);
    ScenarioStateVector* stateVector = dynamic_cast<ScenarioStateVector*>(initialState);
    Q_ASSERT(elements || stateVector);
    if (elements != NULL)
    {
        initialStateTypeCombo->setCurrentIndex(1);
        initialStateStack->setCurrentWidget(keplerianElementsPage);

        semimajorAxisEdit->setText(QString::number(elements->m_semimajorAxis));
        eccentricityEdit->setText(QString::number(elements->m_eccentricity));
        inclinationEdit->setText(QString::number(elements->m_inclination));
        raanEdit->setText(QString::number(elements->m_raan));
        argOfPeriapsisEdit->setText(QString::number(elements->m_argumentOfPeriapsis));
        trueAnomalyEdit->setText(QString::number(elements->m_trueAnomaly));
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

    return true;
}


// Save edit box values to a tree of DOM nodes
bool InitialStateThreebodyEditorDialog::saveValues(ScenarioInitialStatePosition* initialState_threebody_Pos)
{
    int coordSysIndex = coordinateSystemCombo->currentIndex();
    initialState_threebody_Pos->setCoordinateSystem((sta::CoordinateSystemType) coordinateSystemCombo->itemData(coordSysIndex).toInt());

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

        initialState_threebody_Pos->setInitialState(stateVector);
    }
    else
    {
        ScenarioKeplerianElements* elements = new ScenarioKeplerianElements();
        elements->m_semimajorAxis       = semimajorAxisEdit->text().toDouble();
        elements->m_eccentricity        = eccentricityEdit->text().toDouble();
        elements->m_inclination         = inclinationEdit->text().toDouble();
        elements->m_raan                = raanEdit->text().toDouble();
        elements->m_argumentOfPeriapsis = argOfPeriapsisEdit->text().toDouble();
        elements->m_trueAnomaly         = trueAnomalyEdit->text().toDouble();

        initialState_threebody_Pos->setInitialState(elements);
    }

    return true;
}

