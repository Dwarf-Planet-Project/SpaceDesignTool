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

#ifndef _STA_SCENARIO_ENVIRONMENT_H_
#define _STA_SCENARIO_ENVIRONMENT_H_

#include "scenarioobject.h"
#include "scenarioperturbations.h"
#include "scenarioproperties.h"
#include <Astro-Core/stabody.h>
#include <Astro-Core/perturbations.h>

class ScenarioBody;
class Perturbations;

class ScenarioEnvironment : public ScenarioObject
{
public:
    ScenarioEnvironment();
    virtual ~ScenarioEnvironment();

    QString elementName() const { return "Environment"; }
    QString displayName() const { return QObject::tr("Environment"); }
    virtual void writeContents(QXmlStreamWriter* out);
    virtual void createItemContents(QTreeWidgetItem* item);

    /*! Return the central body. */
    ScenarioBody* centralBody() const { return m_centralBody; }

    /*! Set the central body. */
    void setCentralBody(ScenarioBody* centralBody);

    /*! Return the list of perturbations scenario. */
    const QList<ScenarioPerturbations*>& perturbationsList() const {return m_perturbationsList;}

    /*! Add a new perturbation scenario to the list. */
    void addPerturbation(ScenarioPerturbations* perturbation);

    /*! Remove a perturbation scenario to the list. */
    void removePerturbation(ScenarioPerturbations* perturbation);

    /*! Create a list of Perturbations items from the list of perturbations scenario. */
    QList<Perturbations*> createListPerturbations(ScenarioProperties*);

protected:
    ScenarioBody* m_centralBody;

private:
    QList<ScenarioPerturbations*> m_perturbationsList;
};


#endif // _STA_SCENARIO_ENVIRONMENT_H_

