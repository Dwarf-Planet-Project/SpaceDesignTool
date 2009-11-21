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

#ifndef _STA_SCENARIO_DESIGNOVERVIEW_H_
#define _STA_SCENARIO_DESIGNOVERVIEW_H_

#include <QMap>
#include "scenarioobject.h"


class ScenarioSubsystem;

class ScenarioDesignOverview : public ScenarioObject
{
public:
    ScenarioDesignOverview();
    ~ScenarioDesignOverview();

    QString elementName() const { return "DesignOverview"; }
    QString displayName() const { return QObject::tr("Design"); }

    virtual void writeContents(QXmlStreamWriter* out);
    virtual void createItemContents(QTreeWidgetItem* item);
    
    ScenarioSubsystem* subsystem(const QString& name) const;
    void setSubsystem(const QString& name, ScenarioSubsystem* subsystem);

private:
    QMap<QString, ScenarioSubsystem*> m_subsystems;
};


class ScenarioSubsystem : public ScenarioObject
{
public:
    ScenarioSubsystem(const QString& name) : m_name(name) {}
    ~ScenarioSubsystem() {};

    QString elementName() const { return m_name; }
    QString displayName() const { return m_name; }

    virtual void writeContents(QXmlStreamWriter* out);
    virtual void createItemContents(QTreeWidgetItem* item);

    QString name() const { return m_name; }
    QString budget() const { return m_budget; }
    void setBudget(const QString& budget) { m_budget = budget; }
    double value() const { return m_value; }
    void setValue(const double value) { m_value = value; }

private:
    QString m_name;
    QString m_budget;
    double m_value;
};

#endif // _STA_SCENARIO_DESIGNOVERVIEW_H_
