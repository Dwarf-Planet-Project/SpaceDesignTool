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
 Modified by Tiziana Sabatini on July 2009 to include the new perturbations layer
 */

#include "scenarioenvironment.h"
#include "scenariobody.h"
#include "Astro-Core/perturbations.h"
#include <QTreeWidgetItem>
#include <QDebug>

ScenarioEnvironment::ScenarioEnvironment():
   m_centralBody(NULL)
{
}


ScenarioEnvironment::~ScenarioEnvironment()
{
    safeRelease(m_centralBody);
    foreach (ScenarioPerturbations* perturbation, m_perturbationsList)
    {
        safeRelease(perturbation);
    }
}


void
ScenarioEnvironment::writeContents(QXmlStreamWriter* out)
{
    m_centralBody->writeElement(out, "CentralBody");

    foreach (ScenarioPerturbations* perturbation, m_perturbationsList)
    {
        perturbation->writeElement(out);
    }
}


void
ScenarioEnvironment::createItemContents(QTreeWidgetItem* item)
{
    m_centralBody->createItem(item, QObject::tr("Central Body"));

    foreach (ScenarioPerturbations* perturbation, m_perturbationsList)
    {
        perturbation->createItem(item);
    }
}


void
ScenarioEnvironment::addPerturbation(ScenarioPerturbations* perturbation)
{
    m_perturbationsList.append(perturbation);
    attachChild(perturbation);
}


void
ScenarioEnvironment::removePerturbation(ScenarioPerturbations* perturbation)
{
    m_perturbationsList.removeOne(perturbation);
    detachChild(perturbation);
}


void
ScenarioEnvironment::setCentralBody(ScenarioBody* centralBody)
{
    if (centralBody != m_centralBody)
    {
        detachChild(m_centralBody);
        m_centralBody = centralBody;
        attachChild(m_centralBody);
    }
}

QList<Perturbations*>
ScenarioEnvironment::createListPerturbations(ScenarioProperties* properties)
{
    QList<Perturbations*> list;
    foreach(ScenarioPerturbations* scenarioperturbation, m_perturbationsList)
    {
        Perturbations* perturbation;

        if (dynamic_cast<ScenarioAtmosphericDragPerturbations*>(scenarioperturbation))
        perturbation = new AtmosphericDragPerturbations(dynamic_cast<ScenarioAtmosphericDragPerturbations*>(scenarioperturbation), properties);

        else if(dynamic_cast<ScenarioGravityPerturbations*>(scenarioperturbation))
        perturbation = new GravityPerturbations(dynamic_cast<ScenarioGravityPerturbations*>(scenarioperturbation));

        else if(dynamic_cast<ScenarioSolarPressurePerturbations*>(scenarioperturbation))
        perturbation = new SolarPressurePerturbations(dynamic_cast<ScenarioSolarPressurePerturbations*>(scenarioperturbation), properties);

        else if(dynamic_cast<ScenarioExternalBodyPerturbations*>(scenarioperturbation))
        perturbation = new ExternalBodyPerturbations(dynamic_cast<ScenarioExternalBodyPerturbations*>(scenarioperturbation));

        else if(dynamic_cast<ScenarioDebrisPerturbations*>(scenarioperturbation))
        perturbation = new DebrisPerturbations(dynamic_cast<ScenarioDebrisPerturbations*>(scenarioperturbation), properties);

        list.append(perturbation);
    }
    return list;
}

