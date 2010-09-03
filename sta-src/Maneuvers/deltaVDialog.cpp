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

*/

#include "Maneuvers/deltaVDialog.h"
#include "Main/scenariotree.h"
#include "Scenario/scenario.h"
#include "Astro-Core/date.h"
#include "Astro-Core/stacoordsys.h"
#include "Astro-Core/stabody.h"
#include "Astro-Core/stamath.h"
#include "Astro-Core/trueAnomalyTOmeanAnomaly.h"
#include "Scenario/propagationfeedback.h"
#include "Astro-Core/cartesianTOorbital.h"
#include "Astro-Core/orbitalTOcartesian.h"

#include "Astro-Core/propagateTWObody.h"
#include "Astro-Core/propagateCOWELL.h"
#include "Astro-Core/propagateENCKE.h"
#include "Astro-Core/propagateGAUSS.h"
#include "Scenario/missionAspectDialog.h"

#include "Astro-Core/cartesianTOspherical.h"
#include "Astro-Core/sphericalTOcartesian.h"

#include "ui_missionAspectDialog.h"

#include <QTextStream>
#include <QDebug>
#include <QFile>

#include <math.h>

using namespace Eigen;



deltaVDialog::deltaVDialog(ScenarioTree* parent) :
        QDialog(parent)
{
    setupUi(this);

    // Set up the input validators
    QDoubleValidator* doubleValidator = new QDoubleValidator(this);
    QIntValidator* integerValidator = new QIntValidator(this);
    QDoubleValidator* angleValidator = new QDoubleValidator(this);
    angleValidator->setBottom(-360.0);
    angleValidator->setTop(360.0);
    QDoubleValidator* positiveDoubleValidator = new QDoubleValidator(this);
    positiveDoubleValidator->setBottom(0.0);
    QDoubleValidator* zeroToOneValidator = new QDoubleValidator(this);
    zeroToOneValidator->setBottom(0.0);
    zeroToOneValidator->setTop(0.9999);
    QDoubleValidator* minusOneToOneValidator = new QDoubleValidator(this);
    minusOneToOneValidator->setBottom(-1.0);
    minusOneToOneValidator->setTop(1.0);

    durationLineEdit->setValidator(integerValidator);
    azimuthLineEdit->setValidator(angleValidator);
    elevationLineEdit->setValidator(angleValidator);
    magnitudeLineEdit->setValidator(positiveDoubleValidator);
}

deltaVDialog::~deltaVDialog()
{

}





bool deltaVDialog::loadValues(ScenarioDeltaVType* deltaV)
{
    ScenarioElementIdentifierType* arcIdentifier    = deltaV->ElementIdentifier().data();
    ScenarioTimeLine* parameters		            = deltaV->TimeLine().data();
    double myDeltaVx                                = deltaV->DeltaVx();
    double myDeltaVy                                = deltaV->DeltaVy();
    double myDeltaVz                                = deltaV->DeltaVz();
    double myMagnitude                              = deltaV->Magnitude();

    double myAzimuth = 0.0;
    double myElevation = 0.0;
    double range = 0.0;

    // Conversion of the direction of the impulse from cartesian into polar
    myElevation = atan2(myDeltaVz, myDeltaVx);
    myAzimuth = atan2(myDeltaVy, myDeltaVx);

    // Conversion from radians to deg
    myAzimuth = sta::radToDeg(myAzimuth);
    myElevation = sta::radToDeg(myElevation);

    magnitudeLineEdit->setText(QString::number(myMagnitude));
    azimuthLineEdit->setText(QString::number(myAzimuth));
    elevationLineEdit->setText(QString::number(myElevation));

    if (loadValues(arcIdentifier) && loadValues(parameters))
    {
        return true;
    }
    else
    {
        return false;
    }
}




bool deltaVDialog::loadValues(ScenarioElementIdentifierType* arcIdentifier)
{
    QString theArcName = arcIdentifier->Name();
    deltaVAspect.loadValueArcName(theArcName);

    QString theArcColor = arcIdentifier->colorName();
    deltaVAspect.loadValueArcColor(theArcColor);

    QString theArcModel = arcIdentifier->modelName();
    deltaVAspect.loadValueArcModel(theArcModel);

    return true;
}



bool deltaVDialog::loadValues(ScenarioTimeLine* timeLine)
{
    QDateTime myStartTime = timeLine->StartTime();
    QDateTime myEndTime   = timeLine->EndTime();
    startDateTimeEdit->setDateTime(myStartTime);
    endDateTimeEdit->setDateTime(myEndTime);
    durationLineEdit->setText(QString::number(myStartTime.secsTo(myEndTime)));

    return true;
}




bool deltaVDialog::saveValues(ScenarioDeltaVType* deltaV)
{
    ScenarioElementIdentifierType* arcIdentifier    = deltaV->ElementIdentifier().data();
    ScenarioTimeLine* parameters		            = deltaV->TimeLine().data();

    double myMagnitude = magnitudeLineEdit->text().toDouble();
    double myAzimuth    = azimuthLineEdit->text().toDouble();
    double myElevation  = elevationLineEdit->text().toDouble();

    // From degrees to radians
    myAzimuth = sta::degToRad(myAzimuth);
    myElevation = sta::degToRad(myElevation);

    // Conversion of the direction of the impulse from polar into cartesian
    double r = 1;
    double myDeltaVx = r * cos(myAzimuth);
    double myDeltaVy = r * sin(myAzimuth);
    double myDeltaVz = r * sin(myElevation);

    deltaV->setDeltaVx(myDeltaVx);
    deltaV->setDeltaVy(myDeltaVy);
    deltaV->setDeltaVz(myDeltaVz);
    deltaV->setMagnitude(myMagnitude);

    if (saveValues(arcIdentifier) && saveValues(parameters))
    {
        return true;
    }
    else
        return false;

}




bool deltaVDialog::saveValues(ScenarioElementIdentifierType* arcIdentifier)
{
    // The arc name
    QString theArcName = deltaVAspect.saveValueArcName();
    arcIdentifier->setName(theArcName);

    // The color
    QString theColorName = deltaVAspect.saveValueArcColor();
    arcIdentifier->setColorName(theColorName);

    // The model
    QString theModelName = deltaVAspect.saveValueArcModel();
	arcIdentifier->setModelName(theModelName);

    return true;
}



bool deltaVDialog::saveValues(ScenarioTimeLine* timeline)
{
    QDateTime myStartTime = startDateTimeEdit->dateTime();
    QDateTime myEndTime   = endDateTimeEdit->dateTime();
    timeline->setStartTime(myStartTime);
    timeline->setEndTime(myEndTime);
    double duration = myStartTime.secsTo(myEndTime);
    duration = durationLineEdit->text().toDouble();

    return true;
}





/*
/////////////////////////////////////// PropagateLoiteringTrajectory /////////////////////////////
bool PropagateLoiteringTrajectory(ScenarioLoiteringType* loitering,
                                     QList<double>& sampleTimes,
                                     QList<sta::StateVector>& samples,
                                     PropagationFeedback& propFeedback)
{
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
    sta::StateVector initialState = AbstractPositionToStateVector(position, centralBody);
    sta::KeplerianElements initialStateKeplerian = AbstractPositionToKeplerianElements(position, centralBody);

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
        propFeedback.raiseError(QObject::tr("Number of propagation steps exceeds %1. Try increasing the simulation time step.").arg(MAX_OUTPUT_STEPS));
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
        double inclination    = initialStateKeplerian.Inclination*Pi()/180.0;
        double raan           = initialStateKeplerian.AscendingNode*Pi()/180.0;
        double argOfPeriapsis = initialStateKeplerian.ArgumentOfPeriapsis*Pi()/180.0;
        double trueAnomaly    = initialStateKeplerian.TrueAnomaly*Pi()/180.0;
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
} // ------------------------------- End o the propagation method -----------------------

*/




void deltaVDialog::on_endDateTimeEdit_dateTimeChanged(const QDateTime&)
{
    QDateTime myStartTime = startDateTimeEdit->dateTime();
    QDateTime myEndTime   = endDateTimeEdit->dateTime();
    durationLineEdit->setText(QString::number(myStartTime.secsTo(myEndTime)));
}

void deltaVDialog::on_durationLineEdit_textChanged(const QString&)
{
    QDateTime myStartTime = startDateTimeEdit->dateTime();
    double myDuration = durationLineEdit->text().toDouble();
    QDateTime myEndTime = myStartTime.addSecs(myDuration);
    endDateTimeEdit->setDateTime(myEndTime);
 }






void deltaVDialog::on_pushButtonAspect_clicked()
{
    deltaVAspect.exec();
}
