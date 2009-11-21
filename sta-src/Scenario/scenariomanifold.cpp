
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


#include <QTreeWidgetItem>
#include "scenariomanifold.h"
#include "scenarioinitialstate.h"

ScenarioManifolds::ScenarioManifolds() :
    m_branch(NULL),
    m_branch2(NULL),
    m_branch3(NULL),
    m_branch4(NULL)
{
}


void
ScenarioManifolds::writeContents(QXmlStreamWriter* out)
{
//    if (branch_one==true)
    {
    if (m_branch)
        m_branch->writeElement(out);
    }
//    if (branch_two==true)
    {
    if (m_branch2)
        m_branch2->writeElement(out);
    }
//    if (branch_three==true)
    {
    if (m_branch3)
        m_branch3->writeElement(out);
    }
//    if (branch_four==true)
    {
    if (m_branch4)
        m_branch4->writeElement(out);
    }
}


void
ScenarioManifolds::createItemContents(QTreeWidgetItem* item)
{
 //   if (branch_one==true)
    {
    if (m_branch)
        m_branch->createItem(item);
    }
//    if (branch_two==true)
    {
    if (m_branch2)
        m_branch2->createItem(item);
    }
//     if (branch_three==true)
    {
    if (m_branch3)
        m_branch3->createItem(item);
    }
//    if (branch_four==true)
    {
    if (m_branch4)
        m_branch4->createItem(item);
    }

}





ScenarioBranch::ScenarioBranch(int numbranch) :
    m_state(),   // Default value
    m_parameters (NULL)

{
    if (numbranch==1)
       namebranch = "Stable-Right";
    else if (numbranch==2)
       namebranch = "Unstable-Right";
    else if (numbranch==3)
       namebranch = "Stable-Left";
    else if (numbranch==4)
       namebranch = "Unstable-Left";
}

void
ScenarioBranch::writeContents(QXmlStreamWriter* out)
{
    writeStringElement(out, "State", m_state);
    if (m_parameters)
        m_parameters->writeElement(out);

}


void
ScenarioBranch::createItemContents(QTreeWidgetItem* item)
{
    item->setText(1, m_state);


    if (m_parameters)
        m_parameters->createItem(item);
}


void
ScenarioManifolds::setBranch(ScenarioBranch* branch)
{
    if (branch != m_branch)
    {
        detachChild(m_branch);
        m_branch = branch;
        attachChild(m_branch);
    }
}


void
ScenarioManifolds::setBranch2(ScenarioBranch* branch2)
{
    if (branch2 != m_branch2)
    {
        detachChild(m_branch2);
        m_branch2 = branch2;
        attachChild(m_branch2);
    }
}

void
ScenarioManifolds::setBranch3(ScenarioBranch* branch3)
{
    if (branch3 != m_branch3)
    {
        detachChild(m_branch3);
        m_branch3 = branch3;
        attachChild(m_branch3);
    }
}

void
ScenarioManifolds::setBranch4(ScenarioBranch* branch4)
{
    if (branch4 != m_branch4)
    {
        detachChild(m_branch4);
        m_branch4 = branch4;
        attachChild(m_branch4);
    }
}


void
ScenarioBranch::setParameters(ScenarioParameters* parameters)
{
    if (parameters != m_parameters)
    {
        detachChild(m_parameters);
        m_parameters = parameters;
        attachChild(m_parameters);
    }
}


void
ScenarioParameters::setEndCondition(ScenarioEndCondition* endCondition)
{
    if (endCondition != m_endCondition)
    {
        detachChild(m_endCondition);
        m_endCondition = endCondition;
        attachChild(m_endCondition);
    }
}



/*** ScenarioParameters ***/

ScenarioParameters::ScenarioParameters() :
    m_deviation(),
    m_endCondition(NULL)

{
}


void
ScenarioParameters::writeContents(QXmlStreamWriter* out)
{
    writeNumericElement(out, "Deviation", m_deviation, "");
    if (m_endCondition)
        m_endCondition->writeElement(out);

}


void
ScenarioParameters::createItemContents(QTreeWidgetItem* item)
{
    QTreeWidgetItem* deviationItem = new QTreeWidgetItem(item);
    deviationItem->setText(0, QObject::tr("Deviation"));
    deviationItem->setText(1, QString::number(m_deviation));

    if (m_endCondition)
        m_endCondition->createItem(item);
}




/*** ScenarioEndCondition ***/

ScenarioEndCondition::ScenarioEndCondition() :
    m_intersection(),
    m_integrationTime(),
    m_numPositions()

{
}


void
ScenarioEndCondition::writeContents(QXmlStreamWriter* out)
{
    writeBooleanElement(out, "intersection", m_intersection);
    writeNumericElement(out, "integrationTime", m_integrationTime, "");
    writeNumericElement(out, "numPositions", m_numPositions, "");
}


void ScenarioEndCondition::createItemContents(QTreeWidgetItem* item)
{
    QTreeWidgetItem* intersectionItem = new QTreeWidgetItem(item);
    QTreeWidgetItem* integrationTimeItem = new QTreeWidgetItem(item);
    QTreeWidgetItem* numPositionsItem = new QTreeWidgetItem(item);

    intersectionItem->setText(0, QObject::tr("Main body intersection"));
    intersectionItem->setText(1, QObject::tr(m_intersection ? "True" : "False"));
    integrationTimeItem->setText(0, QObject::tr("Integration Time"));
    integrationTimeItem->setText(1, QString::number(m_integrationTime));
    numPositionsItem->setText(0, QObject::tr("Number of positions"));
    numPositionsItem->setText(1, QString::number(m_numPositions));
}


