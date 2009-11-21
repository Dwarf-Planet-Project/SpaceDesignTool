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

#include "spacescenario.h"
#include "propagationfeedback.h"
#include <QTreeWidgetItem>


SpaceScenario::~SpaceScenario()
{
    foreach (ScenarioParticipant* participant, m_participantList)
    {
        participant->release();
    }
}


QTreeWidgetItem* SpaceScenario::createItemSelf(QTreeWidgetItem* parent, QString /* itemName */)
{
    QVariant scenarioObjectData = qVariantFromValue(reinterpret_cast<void*>(this));
    parent->setData(0, ScenarioObjectRole, scenarioObjectData);
    
    // Allow the scenario title to be changed
    parent->setFlags(parent->flags() | Qt::ItemIsEditable);

    // Top-level item; don't create anything
    
    return parent;
}


void SpaceScenario::createItemContents(QTreeWidgetItem* item)
{
    foreach (ScenarioParticipant* participant, m_participantList)
    {
        participant->createItem(item);
    }
}


void
SpaceScenario::writeElement(QXmlStreamWriter* out, QString /* xmlElementName */)
{
    out->writeStartDocument();
    //out->writeDTD();
    out->writeStartElement("SpaceScenario");
    out->writeAttribute("Name", name());
    out->writeAttribute("xmlns:xsi", "http://www.w3.org/2001/XMLSchema-instance");
    out->writeAttribute("xsi:noNamespaceSchemaLocation", "SpaceScenario1.0.xsd");
    writeContents(out);
    out->writeEndElement();
    out->writeEndDocument();
}


void
SpaceScenario::writeContents(QXmlStreamWriter* out)
{
    out->writeStartElement("Participants");
    foreach (ScenarioParticipant* participant, m_participantList)
    {
        participant->writeElement(out);
    }
    out->writeEndElement();
}


void 
SpaceScenario::addParticipant(ScenarioParticipant* participant)
{
    Q_ASSERT(participant != NULL);
    Q_ASSERT(!participant->isAttached());
    attachChild(participant);
    m_participantList.append(participant);

    // Hack to set different visual properties for each participant, so
    // that they can be distinguished in the 3D and ground track views.
    // The user should have control over this.
    int colorIndex = m_participantList.size() % 6;
    QColor color;
    switch (colorIndex)
    {
    case 0: color = Qt::red; break;
    case 1: color = Qt::yellow; break;
    case 2: color = Qt::cyan; break;
    case 3: color = Qt::green; break;
    case 4: color = Qt::magenta; break;
    default: color = QColor(128, 255, 64); break;
    }

    participant->visualProperties().trajectoryColor = color;
}


/*! Remove the trajectory at the specified index.
 */
bool
SpaceScenario::removeChild(unsigned int index)
{
    if ((int) index < m_participantList.size())
    {
        ScenarioParticipant* participant = m_participantList.at((int) index);
        m_participantList.removeAt((int) index);
        detachChild(participant);
        return true;
    }
    else
    {
        return false;
    }
}
