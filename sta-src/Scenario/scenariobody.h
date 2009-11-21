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
 Modified by Tiziana Sabatini July 2009 (deleted gravityModel and atmosphericModel, they will
 be included in perturbations classes)
*/

#ifndef _STA_SCENARIO_BODY_H_
#define _STA_SCENARIO_BODY_H_

#include "Astro-Core/stabody.h"
#include "scenarioobject.h"

class ScenarioBody : public ScenarioObject
{
public:
    ScenarioBody(const StaBody* body);
    ~ScenarioBody();

    virtual QString elementName() const { return "Body"; }
    virtual QString displayName() const { return "Body"; }
    virtual void writeElement(QXmlStreamWriter* out, QString xmlElementName);
    virtual void writeContents(QXmlStreamWriter* out);
    virtual void createItemContents(QTreeWidgetItem* item);

    const StaBody* body() const { return m_body; }
    void setBody(const StaBody* body) { m_body = body; }

private:
    const StaBody* m_body;
};


#endif // _STA_SCENARIO_BODY_H_

