
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

#include "scenariothreebodytransfer.h"
#include "scenarioinitialstate.h"
#include "Astro-Core/date.h"
#include <QTreeWidgetItem>

ScenarioThreebodyTransfer::ScenarioThreebodyTransfer() :
    m_startTime(),
    m_parkingOrbit(NULL),
    m_haloOrbit(NULL),
    m_optimizationParameters(NULL)
{
}


ScenarioThreebodyTransfer::~ScenarioThreebodyTransfer()
{
}


void
ScenarioThreebodyTransfer::writeContents(QXmlStreamWriter* out)
{
    QDateTime startTime = sta::JdToCalendar(sta::MjdToJd(m_startTime));
     out->writeTextElement("StartingEpoch", startTime.toString(Qt::ISODate));
    if (m_parkingOrbit)
        m_parkingOrbit->writeElement(out);
    if (m_haloOrbit)
        m_haloOrbit->writeElement(out);
    if (m_optimizationParameters)
        m_optimizationParameters->writeElement(out);
}


void
ScenarioThreebodyTransfer::createItemContents(QTreeWidgetItem* item)
{

    QDateTime startTime = sta::JdToCalendar(sta::MjdToJd(m_startTime));

    QTreeWidgetItem* startItem = new QTreeWidgetItem(item);
    startItem->setText(0, QObject::tr("Starting Epoch"));
    startItem->setText(1, startTime.toString(Qt::ISODate));

    if (m_parkingOrbit)
        m_parkingOrbit->createItem(item);
    if (m_haloOrbit)
        m_haloOrbit->createItem(item);
    if (m_optimizationParameters)
        m_optimizationParameters->createItem(item);
}


void
ScenarioThreebodyTransfer::setParkingOrbit(ScenarioParkingOrbit* parkingOrbit)
{
    if (parkingOrbit != m_parkingOrbit)
    {
        detachChild(m_parkingOrbit);
        m_parkingOrbit = parkingOrbit;
        attachChild(m_parkingOrbit);
    }
}

void
ScenarioThreebodyTransfer::setHaloOrbit(ScenarioHaloOrbit* haloOrbit)
{
    if (haloOrbit != m_haloOrbit)
    {
        detachChild(m_haloOrbit);
        m_haloOrbit = haloOrbit;
        attachChild(m_haloOrbit);
    }
}


void
ScenarioThreebodyTransfer::setOptimizationParameters(ScenarioOptimizationParameters* optimizationParameters)
{
    if (optimizationParameters != m_optimizationParameters)
    {
        detachChild(m_optimizationParameters);
        m_optimizationParameters = optimizationParameters;
        attachChild(m_optimizationParameters);
    }
}



/***ScenarioParkingOrbit***/

ScenarioParkingOrbit::ScenarioParkingOrbit() :
    m_pOrbit(NULL)
{
}


ScenarioParkingOrbit::~ScenarioParkingOrbit()
{
}


void
ScenarioParkingOrbit::writeContents(QXmlStreamWriter* out)
{
    if (m_pOrbit)
        m_pOrbit->writeElement(out);
}


void
ScenarioParkingOrbit::createItemContents(QTreeWidgetItem* item)
{
    if (m_pOrbit)
        m_pOrbit->createItem(item);
}


void
ScenarioParkingOrbit::setInitialStatePosition(ScenarioInitialStatePosition* pOrbit)
{
    if (pOrbit != m_pOrbit)
    {
        detachChild(m_pOrbit);
        m_pOrbit = pOrbit;
        attachChild(m_pOrbit);
    }
}



/***ScenarioHaloOrbit***/

ScenarioHaloOrbit::ScenarioHaloOrbit() :
    m_hOrbit(NULL)
{
}


ScenarioHaloOrbit::~ScenarioHaloOrbit()
{
}


void
ScenarioHaloOrbit::writeContents(QXmlStreamWriter* out)
{
    if (m_hOrbit)
        m_hOrbit->writeElement(out);
}


void
ScenarioHaloOrbit::createItemContents(QTreeWidgetItem* item)
{
    if (m_hOrbit)
        m_hOrbit->createItem(item);
}


void
ScenarioHaloOrbit::setInitialStatePosition(ScenarioInitialStatePosition* hOrbit)
{
    if (hOrbit != m_hOrbit)
    {
        detachChild(m_hOrbit);
        m_hOrbit = hOrbit;
        attachChild(m_hOrbit);
    }
}



/***ScenarioOptimizationParameters***/

ScenarioOptimizationParameters::ScenarioOptimizationParameters() :
    m_timeofFlight(),
    m_maximumToF(),
    m_propellant()
{
}


ScenarioOptimizationParameters::~ScenarioOptimizationParameters()
{
}


void
ScenarioOptimizationParameters::writeContents(QXmlStreamWriter* out)
{
    writeBooleanElement(out, "timeofFlight", m_timeofFlight);
    writeNumericElement(out, "maximumToF", m_maximumToF, "days");
    writeBooleanElement(out, "propellant", m_propellant);
}


void
ScenarioOptimizationParameters::createItemContents(QTreeWidgetItem* item)
{
    QTreeWidgetItem* timeofFlightItem = new QTreeWidgetItem(item);
    QTreeWidgetItem* maximumToFItem = new QTreeWidgetItem(item);
    QTreeWidgetItem* propellantItem = new QTreeWidgetItem(item);

    timeofFlightItem->setText(0, QObject::tr("TOF"));
    timeofFlightItem->setText(1, QObject::tr(m_timeofFlight ? "True" : "False"));
    maximumToFItem->setText(0, QObject::tr("Maximum TOF"));
    maximumToFItem->setText(1, QString::number(m_maximumToF));
    propellantItem->setText(0, QObject::tr("Total DeltaV"));
    propellantItem->setText(1, QObject::tr(m_propellant ? "True" : "False"));

}

#if 0
void
ScenarioOptimizationParameters::setTimeofFlight(bool tof)
{
    if (tof != m_timeofFlight)
    {
        detachChild(m_timeofFlight);
        m_timeofFlight = tof;;
        attachChild(m_timeofFlight);
    }
}

#endif
