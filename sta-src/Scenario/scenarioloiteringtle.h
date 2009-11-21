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
 */

#ifndef _STA_SCENARIO_LOITERING_TLE_H_
#define _STA_SCENARIO_LOITERING_TLE_H_

#include "scenarioabstracttrajectory.h"

class ScenarioEnvironment;
class ScenarioSimulationParameters;
class ScenarioOptimizationParameters;
class ScenarioTrajectoryPropagation;
class ScenarioExtendedTimeline;


class ScenarioTleTrajectory : public ScenarioAbstractTrajectory
{
public:
    ScenarioTleTrajectory();
    virtual ~ScenarioTleTrajectory();
    
    QString elementName() const { return "LoiteringTLE"; }
    QString displayName() const { return QObject::tr("Loitering (TLE)"); }
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

    QString tleLine0() const { return m_tleLine0; }
    void setTleLine0(const QString& line0) { m_tleLine0 = line0; }

    QString tleLine1() const { return m_tleLine1; }
    void setTleLine1(const QString& line1) { m_tleLine1 = line1; }

    QString tleLine2() const { return m_tleLine2; }
    void setTleLine2(const QString& line2) { m_tleLine2 = line2; }



private:
    ScenarioExtendedTimeline* m_timeline;
    QString m_tleLine0;
    QString m_tleLine1;
    QString m_tleLine2;

#if 0
    double m_startTime;
    double m_endTime;
    double m_timeStep;
#endif
};

#endif // _STA_SCENARIO_LOITERING_H_


