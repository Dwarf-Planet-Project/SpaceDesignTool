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
 ------------------ Author: Chris Laurel  -------------------------------------------------
 ------------------ E-mail: (claurel@gmail.com) ----------------------------
 ------------------ Author: Guillermo Ortega ESA --------------------------

 */

#include "scenario.h"
#include "propagationfeedback.h"
#include "norad.h"
#include "Astro-Core/date.h"

#include <iostream>
#include <QTextStream>


// The maximum number of steps allowed for a propagation; the user should be allowed
// to adjust this.
static const int MAX_OUTPUT_STEPS = 1000000;


ScenarioTleTrajectory::ScenarioTleTrajectory() :
    m_timeline(NULL)
{
}


ScenarioTleTrajectory::~ScenarioTleTrajectory()
{
    safeRelease(m_timeline);
}


void
ScenarioTleTrajectory::writeContents(QXmlStreamWriter* out)
{
    if (m_timeline)
        m_timeline->writeElement(out);
    writeStringElement(out, "Line0", m_tleLine0);
    writeStringElement(out, "Line1", m_tleLine1);
    writeStringElement(out, "Line2", m_tleLine2);
}


void 
ScenarioTleTrajectory::createItemContents(QTreeWidgetItem* item)
{
    if (m_timeline)
        m_timeline->createItem(item);
}


void
ScenarioTleTrajectory::setTimeline(ScenarioExtendedTimeline* timeline)
{
    if (timeline != m_timeline)
    {
        detachChild(m_timeline);
        m_timeline = timeline;
        attachChild(m_timeline);
    }
}


sta::StateVector
ScenarioTleTrajectory::computeInitialStateVector() const
{
    return sta::StateVector::zero();
}


/*! Propagate a TLE trajectory, filling the lists of sampleTimes
 *  and samples with the results of each step. Errors during propagation
 *  are indicated by setting the error flag in the PropagationFeedback
 *  object.
 *
 *  The return value of propagate is the final state after propagation
 *  is complete (i.e. the last entry in the samples list.)
 *
 *  Note that the accuracy of TLEs is poor outside of a limited window
 *  centered on the epoch time (specified within the TLE.)
 */
sta::StateVector
ScenarioTleTrajectory::propagate(PropagationFeedback& propFeedback,
                                 const sta::StateVector& initialState,
                                 QList<double>& sampleTimes,
                                 QList<sta::StateVector>& samples)
{

    QTextStream out (stdout);

    if (!timeline())
        return sta::StateVector::zero();

    double timelineDuration = sta::daysToSecs(m_timeline->endTime() - m_timeline->startTime());
    double dt = m_timeline->timeStep();

    tle_t tle;
    int tleError = parse_elements(m_tleLine1.toAscii().data(), m_tleLine2.toAscii().data(), &tle);
    if (tleError != 0)
    {
        if (tleError == 3)
            propFeedback.raiseError(QObject::tr("TLE is not parseable."));
        else
            propFeedback.raiseError(QObject::tr("Checksum error in TLE.\n"));
        return initialState;
    }

    if (dt == 0.0)
    {
        propFeedback.raiseError(QObject::tr("Time step is zero!"));
        return initialState;
    }

    if (timelineDuration / dt > MAX_OUTPUT_STEPS)
    {
        propFeedback.raiseError(QObject::tr("Number of propagation steps exceeds %1. Try increasing the simulation time step.").arg(MAX_OUTPUT_STEPS));
        return initialState;
    }

    int ephemeris = TLE_EPHEMERIS_TYPE_SGP4;
    bool isDeep = select_ephemeris(&tle) != 0;
    if (isDeep)
    {
        ephemeris = TLE_EPHEMERIS_TYPE_SDP4;
    }
    else
    {
        ephemeris = TLE_EPHEMERIS_TYPE_SGP4;
    }

    double satelliteParams[N_SAT_PARAMS];
    switch (ephemeris)
    {
    case TLE_EPHEMERIS_TYPE_SGP:
       SGP_init(satelliteParams, &tle);
       break;
    case TLE_EPHEMERIS_TYPE_SGP4:
       SGP4_init(satelliteParams, &tle);
       break;
    case TLE_EPHEMERIS_TYPE_SGP8:
       SGP8_init(satelliteParams, &tle);
       break;
    case TLE_EPHEMERIS_TYPE_SDP4:
       SDP4_init(satelliteParams, &tle);
       break;
    case TLE_EPHEMERIS_TYPE_SDP8:
       SDP8_init(satelliteParams, &tle);
       break;
    }

    // Time variable for SGP is in minutes
    double timeBase = (sta::MjdToJd(m_timeline->startTime()) - tle.epoch) * 1440.0;
    sta::StateVector state;

    // Loop written to ensure that we always sample right to the end of the
    // requested span.
    for (double t = 0.0; t < timelineDuration + dt; t += dt)
    {
        double tclamp = std::min(t, timelineDuration);

        // Time since epoch
        double t1 = timeBase + tclamp / 60.0;

        switch (ephemeris)
        {
        case TLE_EPHEMERIS_TYPE_SGP:
           SGP(t1, &tle, satelliteParams, state.position.data(), state.velocity.data());
           break;
        case TLE_EPHEMERIS_TYPE_SGP4:
           SGP4(t1, &tle, satelliteParams, state.position.data(), state.velocity.data());
           break;
        case TLE_EPHEMERIS_TYPE_SGP8:
           SGP8(t1, &tle, satelliteParams, state.position.data(), state.velocity.data());
           break;
        case TLE_EPHEMERIS_TYPE_SDP4:
           SDP4(t1, &tle, satelliteParams, state.position.data(), state.velocity.data());
           break;
        case TLE_EPHEMERIS_TYPE_SDP8:
           SDP8(t1, &tle, satelliteParams, state.position.data(), state.velocity.data());
           break;
        }

        // SGP output velocities are in km/minute; convert to km/sec.
        state.velocity /= 60.0;

        sampleTimes << m_timeline->startTime() + sta::secsToDays(tclamp);
        samples << state;
    }


    //out << "samples size: " << samples.size() << endl;
    return state;
}
