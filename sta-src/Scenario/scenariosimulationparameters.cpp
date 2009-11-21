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

#include "scenariosimulationparameters.h"
#include "scenarioinitialstate.h"
#include "Astro-Core/date.h"
#include <QTreeWidgetItem>
#include <QDebug>

int module1;
ScenarioSimulationParameters::ScenarioSimulationParameters(int module) :
    m_timeline(NULL),
    m_initialStatePosition(NULL),
    m_initialStateAttitude(NULL),
    m_deltav(NULL)
{
    module1=module;
}


ScenarioSimulationParameters::~ScenarioSimulationParameters()
{
}


void
ScenarioSimulationParameters::writeContents(QXmlStreamWriter* out)
{
    if (m_timeline)
        m_timeline->writeElement(out);
    if (m_initialStatePosition)
        m_initialStatePosition->writeElement(out);

    if (module1==1)
    {
        if (m_deltav)
            m_deltav->writeElement(out);
    }
    else if (module1==0)
    {
        if (m_initialStateAttitude)
            m_initialStateAttitude->writeElement(out);
    }
}


void
ScenarioSimulationParameters::createItemContents(QTreeWidgetItem* item)
{
    if (m_timeline)
        m_timeline->createItem(item);
    if (m_initialStatePosition)
        m_initialStatePosition->createItem(item);
    if (module1==1)
    {
        if (m_deltav)
            m_deltav->createItem(item);
    }
    else if (module1==0)
    {
        if (m_initialStateAttitude)
            m_initialStateAttitude->createItem(item);
    }
}


static void createDoubleItem(QTreeWidgetItem* parent, const QString& name, double value)
{
    QTreeWidgetItem* item = new QTreeWidgetItem(parent);
    item->setText(0, name);
    item->setText(1, QString::number(value));
}


void
ScenarioSimulationParameters::setTimeline(ScenarioExtendedTimeline* timeline)
{
    if (timeline != m_timeline)
    {
        detachChild(m_timeline);
        m_timeline = timeline;
        attachChild(m_timeline);
    }
}


void
ScenarioSimulationParameters::setInitialStatePosition(ScenarioInitialStatePosition* state)
{
    if (state != m_initialStatePosition)
    {
        detachChild(m_initialStatePosition);
        m_initialStatePosition = state;
        attachChild(m_initialStatePosition);
    }
}


void
ScenarioSimulationParameters::setInitialStateAttitude(ScenarioInitialStateAttitude* state)
{
    if (state != m_initialStateAttitude)
    {
        detachChild(m_initialStateAttitude);
        m_initialStateAttitude = state;
        attachChild(m_initialStateAttitude);
    }
}


void
ScenarioSimulationParameters::setDeltav(ScenarioDeltav* deltav)
{
    if (deltav != m_deltav)
    {
        detachChild(m_deltav);
        m_deltav = deltav;
        attachChild(m_deltav);
    }
}

/***ScenarioTargettingSimulationParameter***/

ScenarioTargettingSimulationParameters::ScenarioTargettingSimulationParameters() :
    m_timeline(NULL),
    m_initialStatePosition(NULL),
    m_finalState(NULL)
{
    m_finalState = new ScenarioFinalState();
    m_finalState->setIsActiveFalse();
    attachChild(m_finalState);
}


ScenarioTargettingSimulationParameters::~ScenarioTargettingSimulationParameters()
{
}


void
ScenarioTargettingSimulationParameters::writeContents(QXmlStreamWriter* out)
{
    if (m_timeline)
        m_timeline->writeElement(out);
    if (m_initialStatePosition)
        m_initialStatePosition->writeElement(out);
    if (m_finalState->isActive())
        m_finalState->writeElement(out);
}


void
ScenarioTargettingSimulationParameters::createItemContents(QTreeWidgetItem* item)
{
    if (m_timeline)
        m_timeline->createItem(item);
    if (m_initialStatePosition)
        m_initialStatePosition->createItem(item);
    if (m_finalState)
        m_finalState->createItem(item);
}


void
ScenarioTargettingSimulationParameters::setTimeline(ScenarioTimeline* timeline)
{
    if (timeline != m_timeline)
    {
        detachChild(m_timeline);
        m_timeline = timeline;
        attachChild(m_timeline);
    }
}


void
ScenarioTargettingSimulationParameters::setInitialStatePosition(ScenarioInitialStatePosition* state)
{
    if (state != m_initialStatePosition)
    {
        detachChild(m_initialStatePosition);
        m_initialStatePosition = state;
        attachChild(m_initialStatePosition);
    }
}


void
ScenarioTargettingSimulationParameters::setFinalState(ScenarioFinalState* state)
{
    if (state != m_finalState)
    {
        detachChild(m_finalState);
        m_finalState = state;
        attachChild(m_finalState);
    }
}


/*** ScenarioTimeline ***/

ScenarioTimeline::ScenarioTimeline() :
    m_startTime(0.0),   // Default value is J2000.0
    m_timeStep(1.0)     // Default value is 1 second
{
}


void
ScenarioTimeline::writeContents(QXmlStreamWriter* out)
{
    QDateTime startTime = sta::JdToCalendar(sta::MjdToJd(m_startTime));
    out->writeTextElement("StartTime", startTime.toString(Qt::ISODate));
    writeNumericElement(out, "StepTime", m_timeStep, "s");
}


void
ScenarioTimeline::createItemContents(QTreeWidgetItem* item)
{
    QDateTime startTime = sta::JdToCalendar(sta::MjdToJd(m_startTime));

    QTreeWidgetItem* startItem = new QTreeWidgetItem(item);
    startItem->setText(0, QObject::tr("Start Time"));
    startItem->setText(1, startTime.toString(Qt::ISODate));

    QTreeWidgetItem* timeStepItem = new QTreeWidgetItem(item);
    timeStepItem->setText(0, QObject::tr("Time Step"));
    timeStepItem->setText(1, QString::number(m_timeStep));
}

/*** ScenarioExtendedTimeline ***/

void ScenarioExtendedTimeline::writeContents(QXmlStreamWriter* out){
        ScenarioTimeline::writeContents(out);
        QDateTime endTime = sta::JdToCalendar(sta::MjdToJd(m_endTime));
    out->writeTextElement("EndTime", endTime.toString(Qt::ISODate));
}
void ScenarioExtendedTimeline::createItemContents(QTreeWidgetItem* item){
        ScenarioTimeline::createItemContents(item);
    QDateTime endTime = sta::JdToCalendar(sta::MjdToJd(m_endTime));

    QTreeWidgetItem* endItem = new QTreeWidgetItem(item);
    endItem->setText(0, QObject::tr("End Time"));
    endItem->setText(1, endTime.toString(Qt::ISODate));
}

/***ScenarioFinalState***/

ScenarioFinalState::ScenarioFinalState() :
        m_isActive(false),
        m_targetLongitude(0.0),
        m_targetLatitude(0.0)
{
}

void ScenarioFinalState::writeContents(QXmlStreamWriter* out){
        writeNumericElement(out, "TargetLongitude", m_targetLongitude, "deg");
        writeNumericElement(out, "TargetLatitude", m_targetLatitude, "deg");
}
void ScenarioFinalState::createItemContents(QTreeWidgetItem* item){
    QTreeWidgetItem* lonItem = new QTreeWidgetItem(item);
    QTreeWidgetItem* latItem = new QTreeWidgetItem(item);

    //lonItem->setText(0, QObject::tr("Target longitude"));
    lonItem->setText(0, QObject::tr("Point longitude"));
    lonItem->setText(1, QString::number(m_targetLongitude));
    //latItem->setText(0, QObject::tr("Target latitude"));
    latItem->setText(0, QObject::tr("Point latitude"));
    latItem->setText(1, QString::number(m_targetLatitude));
}


/***ScenarioDeltav***/

ScenarioDeltav::ScenarioDeltav() :
    m_time(),
    //m_xposition(),
    //m_yposition(),
    //m_zposition(),
    m_xvelocity(),
    m_yvelocity(),
    m_zvelocity()
{
}


void
ScenarioDeltav::writeContents(QXmlStreamWriter* out)
{
        writeNumericElement(out, "Time", m_time, "h");
        //writeNumericElement(out, "X-position", m_xposition, "km");
        //writeNumericElement(out, "Y-position", m_yposition, "km");
        //writeNumericElement(out, "Z-position", m_zposition, "km");
        writeNumericElement(out, "X-Deltav", m_xvelocity, "km/s");
        writeNumericElement(out, "Y-Deltav", m_yvelocity, "km/s");
        writeNumericElement(out, "Z-Deltav", m_zvelocity, "km/s");
}


void
ScenarioDeltav::createItemContents(QTreeWidgetItem* item)
{
        createDoubleItem(item, QObject::tr("Time"), m_time);
        //createDoubleItem(item, QObject::tr("X-position"), m_xposition);
        //createDoubleItem(item, QObject::tr("Y-position"), m_yposition);
        //createDoubleItem(item, QObject::tr("Z-position"), m_zposition);
        createDoubleItem(item, QObject::tr("X-DeltaV"), m_xvelocity);
        createDoubleItem(item, QObject::tr("Y-DeltaV"), m_yvelocity);
        createDoubleItem(item, QObject::tr("Z-DeltaV"), m_zvelocity);
}
