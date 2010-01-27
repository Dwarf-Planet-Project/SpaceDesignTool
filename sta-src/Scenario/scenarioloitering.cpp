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
 ------ Copyright (C) 2008 European Space Agency (space.trajectory.analysis AT gmail.com) ----
 ------------------ Author: Chris Laurel  -------------------------------------------------
 ------------------ E-mail: (claurel@gmail.com) ----------------------------
 ------------------ Author: Guillermo Ortega ESA  May 14th 2009 --------------------------

Modified by Valentino Zuccarelli on 14th June 2009 at line 69
Mofidied by Guillermo to pathc the bug of the anomaly. Guillermo July 8th 2009
Modified by Tiziana Sabatini on July 2009 (propagate function, added perturbations)
 */

#include "scenario.h"
#include "propagationfeedback.h"

// For trajectory propagation
#include "Astro-Core/perturbations.h"
#include "Astro-Core/cartesianTOorbital.h"
#include "Astro-Core/date.h"
#include "Astro-Core/propagateTWObody.h"
#include "Astro-Core/propagateCOWELL.h"
#include "Astro-Core/propagateENCKE.h"
#include "Astro-Core/propagateGAUSS.h"
#include "Astro-Core/stamath.h"

#include "Plotting/groundtrackplottool.h"

#include "Astro-Core/trueAnomalyTOmeanAnomaly.h"

#include <QTreeWidgetItem>
#include <QtDebug>
#include <QDir>
#include <QMessageBox>

#include <iostream>
#include <QTextStream>

// The maximum number of steps allowed for a propagation; the user should be allowed
// to adjust this.
static const int MAX_OUTPUT_STEPS = 1000000;


ScenarioLoiteringTrajectory::ScenarioLoiteringTrajectory() :
    m_environment(NULL),
    m_simulationParameters(NULL),
    m_optimizationParameters(NULL),
    m_trajectoryPropagation(NULL)
{
    m_environment = new ScenarioEnvironment();
    attachChild(m_environment);
    m_simulationParameters = new ScenarioSimulationParameters(0);
    attachChild(m_simulationParameters);
    m_trajectoryPropagation = new ScenarioTrajectoryPropagation();
    attachChild(m_trajectoryPropagation);
}


void
ScenarioLoiteringTrajectory::writeElement(QXmlStreamWriter* out)
{
    out->writeStartElement(elementName());
    //out->writeAttribute("Name", name());
    writeContents(out);
    out->writeEndElement();    
}


void
ScenarioLoiteringTrajectory::writeContents(QXmlStreamWriter* out)
{
    if (m_environment)
        m_environment->writeElement(out);
    if (m_simulationParameters)
        m_simulationParameters->writeElement(out);
    // TODO: if (m_optimizationParameters)
    //      m_optimizationParameters->writeElement(out);
    if (m_trajectoryPropagation)
        m_trajectoryPropagation->writeElement(out);
}


void 
ScenarioLoiteringTrajectory::createItemContents(QTreeWidgetItem* item)
{
    if (m_environment)
        m_environment->createItem(item);
    if (m_simulationParameters)
        m_simulationParameters->createItem(item);
    // TODO: if (m_optimizationParameters)
    //           m_optimizationParameters->createItem(item);
    if (m_trajectoryPropagation)
        m_trajectoryPropagation->createItem(item);
}


sta::StateVector
ScenarioLoiteringTrajectory::computeInitialStateVector() const
{
    return m_simulationParameters->initialStatePosition()->computeStateVector(environment()->centralBody()->body());
}


/*! Propagate a loitering trajectory, filling the lists of sampleTimes
 *  and samples with the results of each step. Errors during propagation
 *  are indicated by setting the error flag in the PropagationFeedback
 *  object.
 *
 *  The return value of propagate is the final state after propagation
 *  is complete (i.e. the last entry in the samples list.)
 */
sta::StateVector
ScenarioLoiteringTrajectory::propagate(PropagationFeedback& propFeedback,
                                       const sta::StateVector& initialState,
                                       QList<double>& sampleTimes,
                                       QList<sta::StateVector>& samples)
{

    QTextStream out (stdout);

    double mu = centralBody()->mu();

    const ScenarioExtendedTimeline* timeline = simulationParameters()->timeline();

    // Creating the list of perturbations that will influence the propagation
    ScenarioSpaceVehicle* spacevehicle = dynamic_cast<ScenarioSpaceVehicle*>(this->parent()->parent());
    ScenarioProperties* vehicleproperties = spacevehicle->properties();

    QList<Perturbations*> perturbationsList = environment()->createListPerturbations(vehicleproperties);

    double timelineDuration = sta::daysToSecs(timeline->endTime() - timeline->startTime());
    double dt = trajectoryPropagation()->timeStep();

    if (dt == 0.0)
    {
        propFeedback.raiseError(QObject::tr("Time step is zero!"));
        return initialState;
    }

    // We don't output values at every integration step. Instead use the time step
    // from simulation parameters. The actual output step used will not necessarily
    // match the requested output step: the code below sets it to be an integer
    // multiple of the integration step.
    double requestedOutputTimeStep = simulationParameters()->timeline()->timeStep();
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
        return initialState;
    }

    // Calculate initial keplerian elements in case the propagator Two Body will be used
    sta::KeplerianElements foundKeplerianElements = cartesianTOorbital(mu, initialState);

    double sma            = foundKeplerianElements.SemimajorAxis;
    double e              = foundKeplerianElements.Eccentricity;
    double inclination    = foundKeplerianElements.Inclination;
    double raan           = foundKeplerianElements.AscendingNode;
    double argOfPeriapsis = foundKeplerianElements.ArgumentOfPeriapsis;
    double meanAnomaly    = foundKeplerianElements.MeanAnomaly;

    double perigee = sma * (1-e);
#if 0
    if (perigee<centralBody()->meanRadius())
    {
        QMessageBox::warning(NULL, QObject::tr("The trajectory has been not propagated"),
                                   QObject::tr("The perigee distance is smaller than the main body radius."));
        return initialState.zero();
    }
#endif
    sta::StateVector stateVector = initialState;

    // deviation, reference, and q will be used only in Encke propagation
    sta::StateVector deviation(Vector3d::Zero(), Vector3d::Zero());
    sta::StateVector reference = initialState;
    double q = 0.0;

    sampleTimes << timeline->startTime();
    samples << stateVector;

    double time = timeline->startTime(); //mjd
    QFile ciccio("data/PerturbationsData.stae");
    QTextStream cicciostream(&ciccio);
    ciccio.open(QIODevice::WriteOnly);

    unsigned int steps = 0;
    for (double t = dt; t < timelineDuration + dt; t += dt)
    {
        JulianDate jd = timeline->startTime() + sta::secsToDays(t);

        // Choosing the propagator and propagating the trajectory
        if (trajectoryPropagation()->propagator() == "TWO BODY")
        {
            double perigee=sma*(1-e);
            if (perigee<centralBody()->meanRadius())
            {
                QMessageBox::warning(NULL,
                              QObject::tr("The trajectory has been not propagated"),
                              QObject::tr("The perigee distance is smaller than the main body radius."));
                return stateVector.zero();
            }

            double argOfPeriapsisUpdated      = 0.0;
            double meanAnomalyUpdated         = 0.0;
            double raanUpdated                = 0.0;
            stateVector = propagateTWObody(mu, sma, e, inclination, argOfPeriapsis, raan, meanAnomaly,
                                      trajectoryPropagation()->timeStep(), raanUpdated, argOfPeriapsisUpdated,
                                      meanAnomalyUpdated);

            argOfPeriapsis = argOfPeriapsisUpdated;
            meanAnomaly    = meanAnomalyUpdated;
            raan           = raanUpdated;
        }

        else if (trajectoryPropagation()->propagator() == "COWELL")
        {
            stateVector = propagateCOWELL(mu, stateVector, trajectoryPropagation()->timeStep(), perturbationsList, time, trajectoryPropagation()->integrator(), propFeedback);
        }

        else if (trajectoryPropagation()->propagator() == "ENCKE")
        {
            deviation = propagateENCKE(mu, reference, trajectoryPropagation()->timeStep(),perturbationsList, time, stateVector, deviation,  q, trajectoryPropagation()->integrator(), propFeedback);

            // PropagateTWObody is used to propagate the reference trajectory
            double argOfPeriapsisUpdated      = 0.0;
            double meanAnomalyUpdated         = 0.0;
            double raanUpdated                = 0.0;
            reference = propagateTWObody(mu, sma, e, inclination, argOfPeriapsis, raan, meanAnomaly,
                                      trajectoryPropagation()->timeStep(), raanUpdated, argOfPeriapsisUpdated,
                                      meanAnomalyUpdated);

            argOfPeriapsis = argOfPeriapsisUpdated;
            meanAnomaly    = meanAnomalyUpdated;
            raan           = raanUpdated;

            // Calculating the perturbed trajectory
            stateVector = reference + deviation;
            q = deviation.position.dot(reference.position + 0.5 * deviation.position) / pow(reference.position.norm(), 2.0);

//            // Rectification of the reference trajectory, when the deviation is too large.
//            if (q > 0.01)
//            {
//               sta::KeplerianElements keplerian = cartesianTOorbital(mu, stateVector);
//
//               sma = keplerian.SemimajorAxis;
//               e = keplerian.Eccentricity;
//               inclination = keplerian.Inclination;
//               argOfPeriapsis = keplerian.ArgumentOfPeriapsis;
//               raan = keplerian.AscendingNode;
//               meanAnomaly = keplerian.MeanAnomaly;
//
//               q = 0;
//               reference = stateVector;
//               deviation = sta::StateVector(null, null);
//            }
        }

        else if (trajectoryPropagation()->propagator() == "GAUSS")
        {
            stateVector = propagateGAUSS(mu, stateVector, trajectoryPropagation()->timeStep(), perturbationsList, time, trajectoryPropagation()->integrator());
        }

        KeplerianElements kep = cartesianTOorbital(mu, stateVector);

        cicciostream.setRealNumberPrecision(8);
        //cicciostream << kep.SemimajorAxis << "      ";
        //cicciostream << kep.Eccentricity << "      ";
        cicciostream << kep.Inclination << "      ";
        //cicciostream << kep.ArgumentOfPeriapsis << "      ";
        //cicciostream << kep.AscendingNode << "      ";
        //cicciostream << kep.TrueAnomaly << "      ";

//        //Treat the debris perturbation if selected by the user
//        foreach (Perturbations* perturbation, perturbationsList)
//        if (dynamic_cast<DebrisPerturbations*>(perturbation))
//        {
//            DebrisPerturbations* debris = dynamic_cast<DebrisPerturbations*>(perturbation);
//            double gravityAcceleration = (-pow(initialState.position.norm(),-3.0) * mu * initialState.position).norm();
//            double perturbedAcceleration = gravityAcceleration + debris->calculatePerturbingEffect(initialState, sta::daysToSecs(jd));
//        }

        // Append a trajectory sample every outputRate integration steps (and
        // always at the last step.)
        if (steps % outputRate == 0 || t >= timelineDuration)
        {
            sampleTimes << jd;
            samples << stateVector;
        }
        ++steps;

        time += sta::secsToDays(dt);
    };

    //out << "samples size: " << samples.size() << endl;
    return stateVector;
}


/*! Get the origin of the trajectory.
 */
const StaBody* 
ScenarioLoiteringTrajectory::centralBody() const
{
    return environment()->centralBody()->body();
}


/*! Get the coordinate system in which this trajectory is defined.
 */
const sta::CoordinateSystem
ScenarioLoiteringTrajectory::coordinateSystem() const
{
    // TODO: should this be the initial state coord sys?
    // return sta::CoordinateSystem(sta::COORDSYS_EME_J2000);
    return this->simulationParameters()->initialStatePosition()->coordinateSystem();
}


void
ScenarioLoiteringTrajectory::setEnvironment(ScenarioEnvironment* environment)
{
    if (environment != m_environment)
    {
        detachChild(m_environment);
        m_environment = environment;
        attachChild(m_environment);
    }    
}

 
void
ScenarioLoiteringTrajectory::setSimulationParameters(ScenarioSimulationParameters* simulationParameters)
{
    if (simulationParameters != m_simulationParameters)
    {
        detachChild(m_simulationParameters);
        m_simulationParameters = simulationParameters;
        attachChild(m_simulationParameters);
    }    
}


void
ScenarioLoiteringTrajectory::setTrajectoryPropagation(ScenarioTrajectoryPropagation* trajectoryPropagation)
{
    if (trajectoryPropagation != m_trajectoryPropagation)
    {
        detachChild(m_trajectoryPropagation);
        m_trajectoryPropagation = trajectoryPropagation;
        attachChild(m_trajectoryPropagation);
    }    
}

