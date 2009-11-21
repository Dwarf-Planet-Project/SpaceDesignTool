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

#include "scenariolocation.h"
#include <QTreeWidgetItem>


ScenarioLocation::ScenarioLocation() :
    m_centralBody(STA_SOLAR_SYSTEM->lookup("Earth"))
{
}


ScenarioLocation::~ScenarioLocation()
{
}


void ScenarioLocation::writeContents(QXmlStreamWriter* out)
{
    out->writeStartElement("GroundPosition");
    writeNumericElement(out, "Latitude", m_groundPosition.latitude, "deg");
    writeNumericElement(out, "Longitude", m_groundPosition.longitude, "deg");
    writeNumericElement(out, "Altitude", m_groundPosition.altitude, "m");
    out->writeEndElement();
    
    out->writeStartElement("CentralBody");
    out->writeCharacters(m_centralBody->name());
    out->writeEndElement();
}


void ScenarioLocation::createItemContents(QTreeWidgetItem* item)
{
    QTreeWidgetItem* centralBodyItem = new QTreeWidgetItem(item);
    centralBodyItem->setText(0, QObject::tr("Central Body"));
    centralBodyItem->setText(1, m_centralBody->name());
    
    QTreeWidgetItem* latitudeItem = new QTreeWidgetItem(item);
    latitudeItem->setText(0, QObject::tr("Latitude"));
    latitudeItem->setText(1, QString::number(m_groundPosition.latitude));

    QTreeWidgetItem* longitudeItem = new QTreeWidgetItem(item);
    longitudeItem->setText(0, QObject::tr("Longitude"));
    longitudeItem->setText(1, QString::number(m_groundPosition.longitude));

    QTreeWidgetItem* altitudeItem = new QTreeWidgetItem(item);
    altitudeItem->setText(0, QObject::tr("Altitude"));
    altitudeItem->setText(1, QString::number(m_groundPosition.altitude));
}
