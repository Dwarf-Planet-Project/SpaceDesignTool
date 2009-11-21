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

#include "scenariopropulsion.h"
#include <QTreeWidgetItem>


ScenarioPropulsion::ScenarioPropulsion() :
    m_engineCount(1),
    m_specificImpulse(0.0),
    m_thrustPerEngine(0.0),
    m_propellantMass(0.0),
    m_type("THERMAL-SOLID")
{
}


ScenarioPropulsion::~ScenarioPropulsion()
{
}


void
ScenarioPropulsion::writeContents(QXmlStreamWriter* out)
{
    writeStringElement(out, "NumberOfEngines", QString::number(m_engineCount));
    writeStringElement(out, "Isp", QString::number(m_specificImpulse));
    writeStringElement(out, "ThrustPerEngine", QString::number(m_thrustPerEngine));
    writeStringElement(out, "PropellantMass", QString::number(m_propellantMass));
    writeStringElement(out, "Type", m_type);
}


void
ScenarioPropulsion::createItemContents(QTreeWidgetItem* item)
{
    item->setText(1, m_type);
    
    QTreeWidgetItem* enginesItem = new QTreeWidgetItem(item);
    enginesItem->setText(0, QObject::tr("Engines"));
    enginesItem->setText(1, QString::number(m_engineCount));

    QTreeWidgetItem* ispItem = new QTreeWidgetItem(item);
    ispItem->setText(0, QObject::tr("Isp"));
    ispItem->setText(1, QString::number(m_specificImpulse));

    QTreeWidgetItem* thrustItem = new QTreeWidgetItem(item);
    thrustItem->setText(0, QObject::tr("Thrust/engine"));
    thrustItem->setText(1, QString::number(m_thrustPerEngine));

    QTreeWidgetItem* massItem = new QTreeWidgetItem(item);
    massItem->setText(0, QObject::tr("Propellant Mass"));
    massItem->setText(1, QString::number(m_propellantMass));
}


