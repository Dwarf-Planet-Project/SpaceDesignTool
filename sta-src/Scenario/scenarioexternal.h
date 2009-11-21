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

#ifndef _SCENARIOEXTERNAL_H_
#define _SCENARIOEXTERNAL_H_

#include "scenarioabstracttrajectory.h"

#include <QPlainTextEdit>

class ScenarioEnvironment;
class ScenarioSimulationParameters;
class ScenarioOptimizationParameters;
class ScenarioTrajectoryPropagation;
class ScenarioExtendedTimeline;


class ScenarioExternalTrajectory : public ScenarioAbstractTrajectory
{
public:
    ScenarioExternalTrajectory();
    virtual ~ScenarioExternalTrajectory();
    
    QString elementName() const { return "External"; }
    QString displayName() const { return QObject::tr("External"); }
    virtual void writeContents(QXmlStreamWriter* out);
    virtual void createItemContents(QTreeWidgetItem* item);

    // ScenarioAbstractTrajectory methods
    virtual sta::StateVector computeInitialStateVector() const;
    virtual sta::StateVector propagate(PropagationFeedback& feedback,
                                       const sta::StateVector& initialState,
                                       QList<double>& sampleTimes,
                                       QList<sta::StateVector>& samples);

#if 0
    /*! Return the start time as a modified Julian date. */
    double startTime() const { return m_startTime; }

    /*! Set the start time to the specified modified Julian date. */
    void setStartTime(double mjd) { m_startTime = mjd; }

    /*! Return the end time as a modified Julian date. */
    double endTime() const { return m_startTime; }

    /*! Set the end time to the specified modified Julian date. */
    void setEndTime(double mjd) { m_endTime = mjd; }

    /*! Return the time step in seconds. */
    double timeStep() const { return m_timeStep; }

    /*! Set the time step in seconds */
    void setTimeStep(double seconds) { m_timeStep = seconds; }
#endif

    ScenarioExtendedTimeline* timeline() const { return m_timeline; }
    void setTimeline(ScenarioExtendedTimeline* timeline);

    QString firstExternalPropagatedLine() const { return m_FirstExternalPropagatedLine; }
    void setFirstExternalPropagatedLine(const QString& firstLine) { m_FirstExternalPropagatedLine = firstLine; }
    QString lastExternalPropagatedLine() const { return m_LastExternalPropagatedLine; }
    void setLastExternalPropagatedLine(const QString& lastLine) { m_LastExternalPropagatedLine = lastLine; }
    QString allExternalPropagatedLines() const { return m_allExternalPropagatedLines; }
    void setAllExternalPropagatedLines(const QString& allLines) { m_allExternalPropagatedLines = allLines; }

private:
    ScenarioExtendedTimeline* m_timeline;
    QString m_FirstExternalPropagatedLine;
    QString m_LastExternalPropagatedLine;
    QString m_allExternalPropagatedLines;

#if 0
    double m_startTime;
    double m_endTime;
    double m_timeStep;
#endif
};

#endif // _SCENARIOEXTERNAL_H_


