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
 // Patched by Guillermo June 14th 2009
 */

#include "scenariogroundelement.h"
#include "scenariolocation.h"
#include <QTreeWidgetItem>


/*** ScenarioGroundElement ***/

ScenarioGroundElement::ScenarioGroundElement() :
    m_location(NULL)
{
}


ScenarioGroundElement::~ScenarioGroundElement()
{
}


void
ScenarioGroundElement::writeContents(QXmlStreamWriter* out)
{
    if (m_location)
        m_location->writeElement(out);
}


void
ScenarioGroundElement::createItemContents(QTreeWidgetItem* item)
{
    if (elementName() == "GroundStation")
        item->setIcon(0, QIcon(":/icons/antena-pallete.png"));
    else if (elementName() == "LaunchPad")
        item->setIcon(0, QIcon(":/icons/LaunchPad.png"));
    //else if (elementName() == "Target")
    else if (elementName() == "Point")
        item->setIcon(0, QIcon(":/icons/Target.png"));
    
    if (m_location)
        m_location->createItem(item);

   // Expanding now the item that just being dropped into the Scenario Box
    item->setExpanded(true);
}


void
ScenarioGroundElement::setLocation(ScenarioLocation* location)
{
    if (location != m_location)
    {
        detachChild(m_location);
        m_location = location;
        attachChild(m_location);
    }    
}



/*** ScenarioLaunchPad ***/

void ScenarioLaunchPad::writeContents(QXmlStreamWriter* out)
{
    ScenarioGroundElement::writeContents(out);
    writeNumericElement(out, "LaunchPadClearingAltitude", m_clearingAltitude, "m");
}


void ScenarioLaunchPad::createItemContents(QTreeWidgetItem* item)
{
    ScenarioGroundElement::createItemContents(item);
    
    QTreeWidgetItem* clearingAltitudeItem = new QTreeWidgetItem(item);
    clearingAltitudeItem->setText(0, QObject::tr("Clearing Altitude"));
    clearingAltitudeItem->setText(1, QString::number(m_clearingAltitude));
}
