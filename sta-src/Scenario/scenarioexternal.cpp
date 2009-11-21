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
 ------ Copyright (C) 2009 STA SB (space.trajectory.analysis AT gmail.com) ----
 ------------------ Author: Guillermo Ortega  -------------------------------------------------


 */

#include "scenario.h"
#include "propagationfeedback.h"
#include "Astro-Core/date.h"

#include "scenarioexternal.h"

#include <iostream>
#include <QTextStream>
#include <QFile>

// The maximum number of steps allowed for a propagation; the user should be allowed
// to adjust this.
static const int MAX_OUTPUT_STEPS = 1000000;


ScenarioExternalTrajectory::ScenarioExternalTrajectory() :
    m_timeline(NULL)
{
}


ScenarioExternalTrajectory::~ScenarioExternalTrajectory()
{
    safeRelease(m_timeline);
}


void
ScenarioExternalTrajectory::writeContents(QXmlStreamWriter* out)
{
    if (m_timeline)
        m_timeline->writeElement(out);

    //writeStringElement(out, "StateVector1", m_FirstExternalPropagatedLine);
    //writeStringElement(out, "StateVector2", m_LastExternalPropagatedLine);
    writeStringElement(out, "StateVector1", m_allExternalPropagatedLines);
}


void 
ScenarioExternalTrajectory::createItemContents(QTreeWidgetItem* item)
{
    if (m_timeline)
        m_timeline->createItem(item);
}


void
ScenarioExternalTrajectory::setTimeline(ScenarioExtendedTimeline* timeline)
{
    if (timeline != m_timeline)
    {
        detachChild(m_timeline);
        m_timeline = timeline;
        attachChild(m_timeline);
    }
}


sta::StateVector
ScenarioExternalTrajectory::computeInitialStateVector() const
{
    return sta::StateVector::zero();
}


/*! Propagate an Extrenal trajectory, filling the lists of sampleTimes
 *  and samples with the results of each step. Errors during propagation
 *  are indicated by setting the error flag in the PropagationFeedback
 *  object.
 *
 *  The return value of propagate is the final state after propagation
 *  is complete (i.e. the last entry in the samples list.)
 *
 */
sta::StateVector
ScenarioExternalTrajectory::propagate(PropagationFeedback& propFeedback,
                                 const sta::StateVector& initialState,
                                 QList<double>& sampleTimes,
                                 QList<sta::StateVector>& samples)
{

    sta::StateVector state;
    QTextStream out (stdout);
    QString ExternalFileName;
    QString PropagatedLines = "";
    QStringList ListOfItems;

    if (!timeline())
        return sta::StateVector::zero();

    double timelineDuration = sta::daysToSecs(m_timeline->endTime() - m_timeline->startTime());
    double dt = m_timeline->timeStep();


    if (dt <= 0.0)
    {
        propFeedback.raiseError(QObject::tr("Time step is zero or negative!"));
	return initialState;
    }

    if (timelineDuration / dt > MAX_OUTPUT_STEPS)
    {
	propFeedback.raiseError(QObject::tr("Number of propagation steps exceeds %1. Try increasing the simulation time step.").arg(MAX_OUTPUT_STEPS));
	return initialState;
    }

    // Processing now the state vectors

    ExternalFileName = "ephem-external.stae";   // TODO: make this file generic and not fixed

    QFile ExternalFile(ExternalFileName);
    ExternalFile.open(QIODevice::ReadOnly);
    QTextStream StreamWithExternals(&ExternalFile);
    while (!StreamWithExternals.atEnd())
    {
            PropagatedLines = StreamWithExternals.readLine();      //read in a line
            ListOfItems = PropagatedLines.split(",");
            // Calculating time and storing the time samples
            sampleTimes << ListOfItems.value(0).toDouble();
            // Calculating positions
            state.position.x()  = ListOfItems.value(1).toDouble();
            state.position.y()  = ListOfItems.value(2).toDouble();
            state.position.z()  = ListOfItems.value(3).toDouble();
            // Calculating velocities
            state.velocity.x()  = ListOfItems.value(4).toDouble();
            state.velocity.y()  = ListOfItems.value(5).toDouble();
            state.velocity.z()  = ListOfItems.value(6).toDouble();
            // Storing the samples
            samples << state;
            /*
            out << "x: " << state.position.x() << " ";
            out << "y: " << state.position.y() << " ";
            out << "z: " << state.position.z() << " ";
            out << "vx: " << state.velocity.x() << " ";
            out << "vy: " << state.velocity.y() << " ";
            out << "vz: " << state.velocity.z() << endl;
            */
    }
    ExternalFile.close();

    //out << "samples size: " << samples.size() << endl;
    return state;
}
