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

#ifndef _STA_SCENARIO_TRAJECTORYPROPAGATION_H_
#define _STA_SCENARIO_TRAJECTORYPROPAGATION_H_

#include "scenarioobject.h"


class ScenarioTrajectoryPropagation : public ScenarioObject
{
public:
    ScenarioTrajectoryPropagation();
    virtual ~ScenarioTrajectoryPropagation() {};

    QString elementName() const { return "TrajectoryPropagation"; }
    QString displayName() const { return QObject::tr("Trajectory Propagation"); }
    virtual void writeContents(QXmlStreamWriter* out);
    virtual void createItemContents(QTreeWidgetItem* item);
    
    QString integrator() const { return m_integrator; }
    void setIntegrator(const QString& integrator) { m_integrator = integrator.trimmed().toUpper(); }
    QString propagator() const { return m_propagator; }
    void setPropagator(const QString& propagator) { m_propagator = propagator.trimmed().toUpper(); }
    double timeStep() const { return m_timeStep; }
    void setTimeStep(double timeStep) { m_timeStep = timeStep; }
    
private:
    QString m_integrator;  // TODO: use an enum
    double m_timeStep;
    QString m_propagator;  // TODO: use an enum
};

#endif // _STA_SCENARIO_TRAJECTORYPROPAGATION_H_

