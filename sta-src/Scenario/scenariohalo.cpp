
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

 ------ Copyright (C) 2009 European Space Agency (space.trajectory.analysis AT gmail.com) ----
 ------------------ Author: Valentino Zuccarelli  -------------------------------------------------
 ------------------ E-mail: (Valentino.Zuccarelli@gmail.com) ----------------------------
 */

#include "scenariohalo.h"
#include "scenariomanifold.h"
#include "scenarioinitialstate.h"
#include <QTreeWidgetItem>

ScenarioHalo::ScenarioHalo() :
        m_lpoint(),
        m_initialStatePosition(NULL),
        m_amplitudes(NULL),
        m_manifolds(NULL)
{
}


ScenarioHalo::~ScenarioHalo()
{
}


void
ScenarioHalo::writeContents(QXmlStreamWriter* out)
{
    writeStringElement(out, "Lpoint", m_lpoint);
    if (m_initialStatePosition)
        m_initialStatePosition->writeElement(out);
    if (m_amplitudes)
        m_amplitudes->writeElement(out);
    if (m_manifolds)
        m_manifolds->writeElement(out);
}


void
ScenarioHalo::createItemContents(QTreeWidgetItem* item)
{
    //item->setText(1, m_lpoint);

    QTreeWidgetItem* lpointItem = new QTreeWidgetItem(item);
    lpointItem->setText(0, QObject::tr("L point"));
    lpointItem->setText(1, m_lpoint);

    if (m_initialStatePosition)
        m_initialStatePosition->createItem(item);
    if (m_amplitudes)
        m_amplitudes->createItem(item);
    if (m_manifolds)
        m_manifolds->createItem(item);
}


void
ScenarioHalo::setInitialStatePosition(ScenarioInitialStatePosition* state)
{
    if (state != m_initialStatePosition)
    {
        detachChild(m_initialStatePosition);
        m_initialStatePosition = state;
        attachChild(m_initialStatePosition);
    }
}

void
ScenarioHalo::setAmplitudes(ScenarioAmplitudes* ampli)
{
    if (ampli != m_amplitudes)
    {
        detachChild(m_amplitudes);
        m_amplitudes = ampli;
        attachChild(m_amplitudes);
    }
}

void
ScenarioHalo::setManifolds(ScenarioManifolds* mani)
{
    if (mani != m_manifolds)
    {
        detachChild(m_manifolds);
        m_manifolds = mani;
        attachChild(m_manifolds);
    }
}

/*** ScenarioAmplitudes ***/

ScenarioAmplitudes::ScenarioAmplitudes() :
    m_ax(),   // Default value
    m_az()    // Default value
{
}


void
ScenarioAmplitudes::writeContents(QXmlStreamWriter* out)
{
    writeNumericElement(out, "xAmplitude", m_ax, "km");
    writeNumericElement(out, "zAmplitude", m_az, "km");
}


void
ScenarioAmplitudes::createItemContents(QTreeWidgetItem* item)
{
    QTreeWidgetItem* xAmplitudeItem = new QTreeWidgetItem(item);
    xAmplitudeItem->setText(0, QObject::tr("x-Amplitude"));
    xAmplitudeItem->setText(1, QString::number(m_ax));

    QTreeWidgetItem* zAmplitudeItem = new QTreeWidgetItem(item);
    zAmplitudeItem->setText(0, QObject::tr("z-Amplitude"));
    zAmplitudeItem->setText(1, QString::number(m_az));
}



