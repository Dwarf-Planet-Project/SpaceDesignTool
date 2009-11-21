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

#include "scenariodesignoverview.h"
#include <QTreeWidgetItem>


ScenarioDesignOverview::ScenarioDesignOverview()
{
}


ScenarioDesignOverview::~ScenarioDesignOverview()
{
    QMapIterator<QString, ScenarioSubsystem*> iter(m_subsystems);
    while (iter.hasNext())
    {
        iter.next();
        safeRelease(iter.value());
    }
}


ScenarioSubsystem*
ScenarioDesignOverview::subsystem(const QString& name) const
{
    if (m_subsystems.contains(name))
        return m_subsystems.value(name);
    else
        return NULL;
}


void
ScenarioDesignOverview::setSubsystem(const QString& name,
                                     ScenarioSubsystem* subsystem)
{
    attachChild(subsystem);
    if (m_subsystems.contains(name))
    {
        detachChild(m_subsystems.value(name));
    }
    m_subsystems[name] = subsystem;
}


void
ScenarioDesignOverview::writeContents(QXmlStreamWriter* out)
{
    QMapIterator<QString, ScenarioSubsystem*> iter(m_subsystems);
    while (iter.hasNext())
    {
        iter.next();
        iter.value()->writeElement(out, iter.key());
    }
}


void
ScenarioDesignOverview::createItemContents(QTreeWidgetItem* item)
{
    QMapIterator<QString, ScenarioSubsystem*> iter(m_subsystems);
    while (iter.hasNext())
    {
        iter.next();
        iter.value()->createItem(item, iter.key());
    }
}



void
ScenarioSubsystem::writeContents(QXmlStreamWriter* out)
{
    writeStringElement(out, "budget", m_budget);
    writeNumericElement(out, "value", m_value);
}


void
ScenarioSubsystem::createItemContents(QTreeWidgetItem* item)
{
    QTreeWidgetItem* budgetItem = new QTreeWidgetItem(item);
    budgetItem->setText(0, QObject::tr("Budget"));
    budgetItem->setText(1, m_budget);   
    
    QTreeWidgetItem* valueItem = new QTreeWidgetItem(item);
    valueItem->setText(0, QObject::tr("Value"));
    valueItem->setText(1, QString::number(m_value));       
}



