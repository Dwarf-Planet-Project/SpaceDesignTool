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

#ifndef _STA_SPACESCENARIO_H_
#define _STA_SPACESCENARIO_H_

#include "scenarioobject.h"
#include "scenarioparticipant.h"

class PropagationFeedback;

class SpaceScenario : public ScenarioObject
{
public:
    SpaceScenario() : ScenarioObject() {}
    ~SpaceScenario();
    
    QString name() const { return m_name; }
    void setName(const QString& name) { m_name = name; }
    
    void writeElement(QXmlStreamWriter* out, QString xmlElementName = "");
    void writeContents(QXmlStreamWriter* out);
    
    QString displayName() const { return QObject::tr("Scenario"); }

    const QList<ScenarioParticipant*>& participants() const { return m_participantList; }
    void addParticipant(ScenarioParticipant* participant);
    virtual bool removeChild(unsigned int index);

    virtual QTreeWidgetItem* createItemSelf(QTreeWidgetItem* parent, QString itemName);
    virtual void createItemContents(QTreeWidgetItem* item);
    
    bool propagate(PropagationFeedback& feedback);
    
private:
    QList<ScenarioParticipant*> m_participantList;
    QString m_name;
};

#endif // _STA_SPACESCENARIO_H_

