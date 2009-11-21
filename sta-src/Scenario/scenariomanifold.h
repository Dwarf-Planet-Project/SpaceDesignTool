
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

 ------ Copyright (C) 2009 European Space Agency (space.trajectory.analysis AT gmail.com) ----
 ------------------ Author: Valentino Zuccarelli  -------------------------------------------------
 ------------------ E-mail: (Valentino.Zuccarelli@gmail.com) ----------------------------
 */

#ifndef SCENARIOMANIFOLD_H
#define SCENARIOMANIFOLD_H

#include "scenarioobject.h"

class ScenarioBranch;
class ScenarioParameters;
class ScenarioEndCondition;

class ScenarioManifolds: public ScenarioObject
{
public:
    ScenarioManifolds();
    virtual ~ScenarioManifolds(){}

    QString elementName() const { return "Manifolds"; }
    QString displayName() const { return QObject::tr("Manifolds"); }
    virtual void writeContents(QXmlStreamWriter* item);
    virtual void createItemContents(QTreeWidgetItem* item);

    ScenarioBranch* branch() const { return m_branch; }
    void setBranch(ScenarioBranch* branch);
    ScenarioBranch* branch2() const { return m_branch2; }
    void setBranch2(ScenarioBranch* branch2);
    ScenarioBranch* branch3() const { return m_branch3; }
    void setBranch3(ScenarioBranch* branch3);
    ScenarioBranch* branch4() const { return m_branch4; }
    void setBranch4(ScenarioBranch* branch4);

private:
    ScenarioBranch *m_branch;
    ScenarioBranch *m_branch2;
    ScenarioBranch *m_branch3;
    ScenarioBranch *m_branch4;
};


class ScenarioBranch: public ScenarioObject
{
public:
    ScenarioBranch(int numbranch);
    virtual ~ScenarioBranch(){}


    QString elementName() const { return "Branch"; }
    QString displayName() const { return QObject::tr(namebranch); }
    virtual void writeContents(QXmlStreamWriter* item);
    virtual void createItemContents(QTreeWidgetItem* item);

    ScenarioParameters* parameters() const { return m_parameters; }
    void setParameters(ScenarioParameters* parameters);
    QString state() const { return m_state; }
    void setState(QString state) { m_state = state; }
    const char * namebranch;

private:
    QString m_state;   // Default value
    ScenarioParameters *m_parameters;
};

class ScenarioParameters: public ScenarioObject
{
public:
    ScenarioParameters();
    virtual ~ScenarioParameters(){}

    QString elementName() const { return "Parameters"; }
    QString displayName() const { return QObject::tr("Parameters"); }
    virtual void writeContents(QXmlStreamWriter* item);
    virtual void createItemContents(QTreeWidgetItem* item);

    ScenarioEndCondition* endCondition() const { return m_endCondition; }
    void setEndCondition(ScenarioEndCondition* endCondition);
    double deviation() const { return m_deviation; }
    void setDeviation(double epsilon) { m_deviation = epsilon; }

private:
    double m_deviation;
    ScenarioEndCondition* m_endCondition;
};


class ScenarioEndCondition: public ScenarioObject
{
public:
    ScenarioEndCondition();
    virtual ~ScenarioEndCondition(){}

    QString elementName() const { return "EndCondition"; }
    QString displayName() const { return QObject::tr("End Condition"); }
    virtual void writeContents(QXmlStreamWriter* item);
    virtual void createItemContents(QTreeWidgetItem* item);

    bool intersection() const { return m_intersection; }
    void setIntersection(bool intersec) { m_intersection = intersec; }
    double integrationTime() const { return m_integrationTime; }
    void setIntegrationTime(double time) { m_integrationTime = time; }
    unsigned int numPositions() const { return m_numPositions; }
    void setNumPositions(unsigned int number) { m_numPositions = number; }

private:
    bool m_intersection;
    double m_integrationTime;
    unsigned int m_numPositions;
};



#endif // SCENARIOMANIFOLD_H
