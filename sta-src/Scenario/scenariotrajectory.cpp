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
 */

#include "scenariotrajectory.h"
#include "scenariotrajectorypropagation.h"
#include "scenariosimulationparameters.h"
#include "scenarioenvironment.h"
#include "propagationfeedback.h"
#include "Astro-Core/cartesianTOorbital.h"
#include "Astro-Core/date.h"
#include "Astro-Core/propagateTWObody.h"
#include "Astro-Core/getGreenwichHourAngle.h"
#include "Astro-Core/inertialTOfixed.h"
#include "Astro-Core/rectangularTOpolar.h"
#include "Astro-Core/stamath.h"

#include <QTreeWidgetItem>
#include <QTemporaryFile>
#include <QTextStream>

using namespace std;


ScenarioTrajectory::ScenarioTrajectory() :
    m_environment(NULL),
    m_simulationParameters(NULL),
    m_optimizationParameters(NULL),
    m_trajectoryPropagation(NULL),
    m_ephemerisFile(NULL),
    m_groundTrackFile(NULL)
{
    m_environment = new ScenarioEnvironment();
    attachChild(m_environment);
    m_simulationParameters = new ScenarioSimulationParameters();
    attachChild(m_simulationParameters);
    m_trajectoryPropagation = new ScenarioTrajectoryPropagation();
    attachChild(m_trajectoryPropagation);
}


ScenarioTrajectory::~ScenarioTrajectory()
{
    // deleting the QTemporaryFile objects will clean up the temp files
    delete m_ephemerisFile;
    delete m_groundTrackFile;
}


void
ScenarioTrajectory::writeElement(QXmlStreamWriter* out)
{
    out->writeStartElement(elementName());
    //out->writeAttribute("Name", name());
    writeContents(out);
    out->writeEndElement();    
}


void
ScenarioTrajectory::writeContents(QXmlStreamWriter* out)
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
ScenarioTrajectory::createItemContents(QTreeWidgetItem* item)
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


void
ScenarioTrajectory::setEnvironment(ScenarioEnvironment* environment)
{
    if (environment != m_environment)
    {
        detachChild(m_environment);
        m_environment = environment;
        attachChild(m_environment);
    }    
}


void
ScenarioTrajectory::setSimulationParameters(ScenarioSimulationParameters* simulationParameters)
{
    if (simulationParameters != m_simulationParameters)
    {
        detachChild(m_simulationParameters);
        m_simulationParameters = simulationParameters;
        attachChild(m_simulationParameters);
    }    
}


void
ScenarioTrajectory::setTrajectoryPropagation(ScenarioTrajectoryPropagation* trajectoryPropagation)
{
    if (trajectoryPropagation != m_trajectoryPropagation)
    {
        detachChild(m_trajectoryPropagation);
        m_trajectoryPropagation = trajectoryPropagation;
        attachChild(m_trajectoryPropagation);
    }    
}


void
ScenarioTrajectory::propagate(PropagationFeedback& propFeedback, sta::StateVector initialState)
{
    const StaBody* centralBody = environment()->centralBody();
    const ScenarioTimeline* timeline = simulationParameters()->timeline();
    
    double sma                 = 0.0;
    double e                   = 0.0;
    double inclination         = 0.0;
    double raan                = 0.0;
    double argOfPeriapsis      = 0.0;
    double trueAnomaly         = 0.0;
    
    cartesianTOorbital(centralBody->mu(),
                       initialState.position.x, initialState.position.y, initialState.position.z,
                       initialState.velocity.x, initialState.velocity.y, initialState.velocity.z,
                       sma, e, inclination, argOfPeriapsis, raan, trueAnomaly);
    
    // Create a new temporary file for the ephemeris
    if (m_ephemerisFile != NULL)
        delete m_ephemerisFile;
    m_ephemerisFile = new QTemporaryFile("ephem-XXXXXX.stae");
    if (!m_ephemerisFile->open())
    {
        delete m_ephemerisFile;
        m_ephemerisFile = NULL;
        propFeedback.raiseError("Error opening ephemeris file");
        return;
    }    
    
    // Create a new temporary file for the ground track
    if (m_groundTrackFile != NULL)
        delete m_groundTrackFile;
    m_groundTrackFile = new QTemporaryFile("track-XXXXXX.stag");
    if (!m_groundTrackFile->open())
    {
        propFeedback.raiseError("Error opening ground track file");
        delete m_groundTrackFile;
        m_groundTrackFile = NULL;
        return;
    }
    
    QTextStream ephemerisStream(m_ephemerisFile);
    QTextStream groundTrackStream(m_groundTrackFile);
    
    // Output values at full double precision
    ephemerisStream.setRealNumberPrecision(16);
    groundTrackStream.setRealNumberPrecision(16);
    
	// Calling the propagation routine iteratively as to find each state vector per step size untill the elapsed time
    double elapsedTime = sta::daysToSecs(timeline->endTime() - timeline->startTime());
    double dt = trajectoryPropagation()->timeStep();
    
    for (double t = 0; t < elapsedTime; t += dt)
    {
        double argOfPeriapsisUpdated      = 0.0;
        double trueAnomalyUpdated         = 0.0;
        double raanUpdated                = 0.0;
        
        JulianDate jd = timeline->startTime() + sta::secsToDays(t);

        sta::StateVector stateVector;
        sta::StateVector fixedStateVector;
        
	    // Finding the state vector at starting time (when valueStep=0)
	    propagateTWObody(centralBody->mu(), 
 					     sma,
                         e,
                         inclination,
                         argOfPeriapsis,
                         raan,
                         trueAnomaly,
                         trajectoryPropagation()->timeStep(),
                         stateVector.position.x, stateVector.position.y, stateVector.position.z,
                         stateVector.velocity.x, stateVector.velocity.y, stateVector.velocity.z,
					     raanUpdated,
                         argOfPeriapsisUpdated,
                         trueAnomalyUpdated);
        
	    // Converting from inertial coordinates to fixed. Only Earth for the time being
	    double greenwichAngle = getGreenwichHourAngle(jd);
        
	    inertialTOfixed(0, greenwichAngle, 
					    stateVector.position.x, stateVector.position.y, stateVector.position.z,
					    stateVector.velocity.x, stateVector.velocity.y, stateVector.velocity.z,
					    fixedStateVector.position.x, fixedStateVector.position.y, fixedStateVector.position.z,
					    fixedStateVector.velocity.x, fixedStateVector.velocity.y, fixedStateVector.velocity.z);
        
	    // Converting to polar
        double radius    = 0.0;
        double longitude = 0.0;
        double latitude  = 0.0;
	    rectangularTOpolar(fixedStateVector.position.x, fixedStateVector.position.y, fixedStateVector.position.z,
                           radius, longitude, latitude);
        
	    // Implementing the Scenario Builder Model
        // Output to ephemeris file
        ephemerisStream << jd << " " << stateVector.position << " " << stateVector.velocity << endl;
        
        // Output to ground track file
        groundTrackStream << sta::radToDeg(longitude) << " " << sta::radToDeg(latitude) << "\n";
        
	    // Exchanging the parameters for the next iteration loop
        argOfPeriapsis = argOfPeriapsisUpdated;
        trueAnomaly         = trueAnomalyUpdated;
        raan                = raanUpdated;
	};
    
    ephemerisStream.flush();
    groundTrackStream.flush();
}


