

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

#include "scenariothreebodyenvironment.h"
#include "scenariobody.h"
#include <QTreeWidgetItem>


ScenarioPartialThreebodyenvironment::ScenarioPartialThreebodyenvironment() :
    m_firstBody(NULL),
    m_secondBody(NULL),
    m_centralBody(NULL)
{
}


ScenarioPartialThreebodyenvironment::~ScenarioPartialThreebodyenvironment()
{
    safeRelease(m_firstBody);
    safeRelease(m_secondBody);
    safeRelease(m_centralBody);
}


void
ScenarioPartialThreebodyenvironment::writeContents(QXmlStreamWriter* out)
{
    m_firstBody->writeElement(out, "FirstBody");
    m_secondBody->writeElement(out, "SecondBody");
}


void
ScenarioPartialThreebodyenvironment::createItemContents(QTreeWidgetItem* item)
{
    m_firstBody->createItem(item, QObject::tr("First Body"));
    m_secondBody->createItem(item, QObject::tr("Second Body"));
}


void
ScenarioPartialThreebodyenvironment::setFirstBody(ScenarioBody* firstBody)
{
    if (firstBody != m_firstBody)
    {
        detachChild(m_firstBody);
        m_firstBody = firstBody;
        attachChild(m_firstBody);
    }
}

void
ScenarioPartialThreebodyenvironment::setSecondBody(ScenarioBody* secondBody)
{
    if (secondBody != m_secondBody)
    {
        detachChild(m_secondBody);
        m_secondBody = secondBody;
        attachChild(m_secondBody);
    }
}


/**** ScenariothreebodyEnvironment ****/

ScenarioThreebodyenvironment::ScenarioThreebodyenvironment() :
    m_solarPressureEnabled(false)
{
        ScenarioPartialThreebodyenvironment::ScenarioPartialThreebodyenvironment();
}


ScenarioThreebodyenvironment::~ScenarioThreebodyenvironment()
{

}


void
ScenarioThreebodyenvironment::writeContents(QXmlStreamWriter* out)
{
    ScenarioPartialThreebodyenvironment::writeContents(out);


}


void
ScenarioThreebodyenvironment::createItemContents(QTreeWidgetItem* item)
{
    ScenarioPartialThreebodyenvironment::createItemContents(item);


}




