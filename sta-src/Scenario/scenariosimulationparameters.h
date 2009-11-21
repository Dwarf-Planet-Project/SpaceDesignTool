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
 // Modified by Valentino Zuccarelli on June 11th 2009
 */

#ifndef _STA_SCENARIO_SIMULATIONPARAMETERS_H_
#define _STA_SCENARIO_SIMULATIONPARAMETERS_H_

#include <QDateTime>
#include <QTime>
#include "scenarioobject.h"
#include "Astro-Core/statevector.h"
#include "Astro-Core/stabody.h"

class ScenarioTimeline;
class ScenarioExtendedTimeline;
class ScenarioInitialStatePosition;
class ScenarioInitialStateAttitude;
class ScenarioFinalState;
class ScenarioDeltav;

class ScenarioSimulationParameters : public ScenarioObject
{
public:
    ScenarioSimulationParameters(int mod);
    virtual ~ScenarioSimulationParameters();

    QString elementName() const { return "SimulationParameters"; }
    QString displayName() const { return QObject::tr("Simulation Parameters"); }
    virtual void writeContents(QXmlStreamWriter* item);
    virtual void createItemContents(QTreeWidgetItem* item);

    ScenarioExtendedTimeline* timeline() const { return m_timeline; }
    void setTimeline(ScenarioExtendedTimeline* timeline);
    ScenarioInitialStatePosition* initialStatePosition() const { return m_initialStatePosition; }
    void setInitialStatePosition(ScenarioInitialStatePosition* initialStatePosition);
    ScenarioInitialStateAttitude* initialStateAttitude() const { return m_initialStateAttitude; }
    void setInitialStateAttitude(ScenarioInitialStateAttitude* initialStateAttitude);
    ScenarioDeltav* deltav() const { return m_deltav; }
    void setDeltav(ScenarioDeltav* deltav);
    //int module;

private:
    ScenarioExtendedTimeline* m_timeline;
    ScenarioInitialStatePosition* m_initialStatePosition;
    ScenarioInitialStateAttitude* m_initialStateAttitude;
    ScenarioDeltav* m_deltav;
};


class ScenarioDeltav : public ScenarioObject
{
public:
    ScenarioDeltav();
    virtual ~ScenarioDeltav() {}

    QString elementName() const { return "Deltav"; }
    QString displayName() const { return QObject::tr("Deltav"); }
    virtual void writeContents(QXmlStreamWriter* item);
    virtual void createItemContents(QTreeWidgetItem* item);

    double Time() const {return m_time;} //time when the manoeuvre is performed
    void setTime(double time) { m_time = time; }
    double Xvelocity() const {return m_xvelocity;} //xdeltaV given
    void setXvelocity(double xdeltav) { m_xvelocity = xdeltav; }
    double Yvelocity() const {return m_yvelocity;} //ydeltaV given
    void setYvelocity(double ydeltav) { m_yvelocity = ydeltav; }
    double Zvelocity() const {return m_zvelocity;} //zdeltaV given
    void setZvelocity(double zdeltav) { m_zvelocity = zdeltav; }

    double m_time;
    double m_xvelocity;
    double m_yvelocity;
    double m_zvelocity;

};


class ScenarioTargettingSimulationParameters : public ScenarioObject
{
public:
    ScenarioTargettingSimulationParameters();
    virtual ~ScenarioTargettingSimulationParameters();

    QString elementName() const { return "SimulationParameters"; }
    QString displayName() const { return QObject::tr("Simulation Parameters"); }
    virtual void writeContents(QXmlStreamWriter* item);
    virtual void createItemContents(QTreeWidgetItem* item);

    ScenarioTimeline* timeline() const { return m_timeline; }
    void setTimeline(ScenarioTimeline* timeline);
    ScenarioInitialStatePosition* initialStatePosition() const { return m_initialStatePosition; }
    void setInitialStatePosition(ScenarioInitialStatePosition* initialStatePosition);
    ScenarioFinalState* finalState() const { return m_finalState; }
    void setFinalState(ScenarioFinalState* finalState);

private:
    ScenarioTimeline* m_timeline;
    ScenarioInitialStatePosition* m_initialStatePosition;
    ScenarioFinalState* m_finalState;
};


// TODO: if the Timeline object is retained, it should be moved to a
// separate file.
class ScenarioTimeline : public ScenarioObject
{
public:
    ScenarioTimeline();
    virtual ~ScenarioTimeline() {}

    QString elementName() const { return "Timeline"; }
    QString displayName() const { return QObject::tr("Timeline"); }
    virtual void writeContents(QXmlStreamWriter* out);
    virtual void createItemContents(QTreeWidgetItem* item);

    /*! Return the start time as a modified Julian date. */
    double startTime() const { return m_startTime; }

    /*! Set the start time to the specified modified Julian date. */
    void setStartTime(double mjd) { m_startTime = mjd; }

    /*! Return the time step in seconds. */
    double timeStep() const { return m_timeStep; }

    /*! Set the time step in seconds */
    void setTimeStep(double seconds) { m_timeStep = seconds; }

private:
    double m_startTime;  // Start time is stored as a modified Julian date
    double m_timeStep;   // TODO: Current unusued
};


class ScenarioExtendedTimeline : public ScenarioTimeline
{
public:
        ScenarioExtendedTimeline(): m_endTime(0.0){
                ScenarioTimeline::ScenarioTimeline();
        }
        ~ScenarioExtendedTimeline(){}

    virtual void writeContents(QXmlStreamWriter* out);
    virtual void createItemContents(QTreeWidgetItem* item);

    /*! Return the end time as a modified Julian date. */
    double endTime() const { return m_endTime; }
    /*! Set the end time to the specified modified Julian date. */
    void setEndTime(double mjd) { m_endTime = mjd; }

private:
    double m_endTime;    // End time is stored as a modified Julian date
};


class ScenarioFinalState : public ScenarioObject
{
public:
        ScenarioFinalState();
        ~ScenarioFinalState(){}

    QString elementName() const { return "FinalState"; }
    QString displayName() const { return QObject::tr("Final State"); }
    virtual void writeContents(QXmlStreamWriter* out);
    virtual void createItemContents(QTreeWidgetItem* item);

    double targetLon() const { return m_targetLongitude; }
    void setTargetLon(double lon) { m_targetLongitude = lon; }

    double targetLat() const { return m_targetLatitude; }
    void setTargetLat(double lat) { m_targetLatitude = lat; }

    bool isActive() const { return m_isActive; }
    void setIsActiveTrue() { m_isActive = true; }
    void setIsActiveFalse() { m_isActive = false; }

private:
        bool m_isActive;
        double m_targetLongitude;
        double m_targetLatitude;
};

#endif // _STA_SCENARIO_SIMULATIONPARAMETERS_H_
