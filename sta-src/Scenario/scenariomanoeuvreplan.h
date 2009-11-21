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
 ------------------ Author: Annalisa Riccardi  -------------------------------------------------
 ------------------ E-mail: (nina1983@gmail.com) ----------------------------
 */

#ifndef _STA_SCENARIO_MANOEUVREPLAN_H_
#define _STA_SCENARIO_MANOEUVREPLAN_H_

#include "scenarioobject.h"
#include <QHash>
#include <QStringList>

class ScenarioManoeuvre;

class ScenarioManoeuvrePlan : public ScenarioObject
{
public:
	ScenarioManoeuvrePlan();
    ~ScenarioManoeuvrePlan();
    
    QString elementName() const { return "ManoeuvrePlan"; }
    QString displayName() const { return QObject::tr("Manoeuvre Plan"); }
    virtual void writeContents(QXmlStreamWriter* out);
    virtual void createItemContents(QTreeWidgetItem* item);
    
    QList<ScenarioManoeuvre*>& manoeuvresList() {return m_manoeuvres;}
    void addManoeuvre(ScenarioManoeuvre* manoeuvre);
    void removeManoeuvre(ScenarioManoeuvre* manoeuvre);
    
private:
	QList<ScenarioManoeuvre*> m_manoeuvres;
};

#endif // _STA_SCENARIO_MANOEUVREPLAN_H_
