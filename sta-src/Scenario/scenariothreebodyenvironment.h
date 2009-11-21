


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

#ifndef SCENARIOTHREEBODYENVIRONMENT_H
#define SCENARIOTHREEBODYENVIRONMENT_H


#include "scenarioobject.h"
#include <Astro-Core/stabody.h>

class ScenarioBody;

class ScenarioPartialThreebodyenvironment : public ScenarioObject
{
public:
    ScenarioPartialThreebodyenvironment();
    virtual ~ScenarioPartialThreebodyenvironment();

    QString elementName() const { return "Threebodyenvironment"; }
    QString displayName() const { return QObject::tr("3-body Environment"); }
    virtual void writeContents(QXmlStreamWriter* out);
    virtual void createItemContents(QTreeWidgetItem* item);

    ScenarioBody* firstBody() const { return m_firstBody; }
    void setFirstBody(ScenarioBody* firstBody);
    ScenarioBody* secondBody() const { return m_secondBody; }
    void setSecondBody(ScenarioBody* secondBody);
    ScenarioBody* centralBody() const { return m_centralBody; }
    void setCentralBody(ScenarioBody* centralBody);

protected:
    ScenarioBody* m_firstBody;
    ScenarioBody* m_secondBody;
    ScenarioBody* m_centralBody;
};


class ScenarioThreebodyenvironment : public ScenarioPartialThreebodyenvironment
{
public:
    ScenarioThreebodyenvironment();
    virtual ~ScenarioThreebodyenvironment();

    virtual void writeContents(QXmlStreamWriter* out);
    virtual void createItemContents(QTreeWidgetItem* item);


    bool solarPressureEnabled() const { return m_solarPressureEnabled; }
    void setSolarPressureEnabled(bool enable) { m_solarPressureEnabled = enable; }

    const QList<ScenarioBody*>& perturbingBodyList() const {return m_perturbingBodyList;}
    void addPerturbingBody(ScenarioBody* body);
    void removePerturbingBody(ScenarioBody* body);

private:
    bool m_solarPressureEnabled;
    QList<ScenarioBody*> m_perturbingBodyList;
};

#endif // SCENARIOTHREEBODYENVIRONMENT_H
