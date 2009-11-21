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

#ifndef _STA_SCENARIO_PROPULSION_H_
#define _STA_SCENARIO_PROPULSION_H_

#include "scenarioobject.h"


class ScenarioPropulsion : public ScenarioObject
{
public:
    ScenarioPropulsion();
    ~ScenarioPropulsion();

    QString elementName() const { return "Propulsion"; }
    QString displayName() const { return QObject::tr("Propulsion"); }

    virtual void writeContents(QXmlStreamWriter* out);
    virtual void createItemContents(QTreeWidgetItem* item);

    unsigned int engineCount() const { return m_engineCount; }
    void setEngineCount(unsigned int engineCount) { m_engineCount = engineCount; }
    double specificImpulse() const { return m_specificImpulse; }
    void setSpecificImpulse(double specificImpulse) { m_specificImpulse = specificImpulse; }
    double thrustPerEngine() const { return m_thrustPerEngine; }
    void setThrustPerEngine(double thrustPerEngine) { m_thrustPerEngine = thrustPerEngine; }
    double propellantMass() const { return m_propellantMass; }
    void setPropellantMass(double propellantMass) { m_propellantMass = propellantMass; }
    QString type() const { return m_type; }
    void setType(QString type) { m_type = type; }

private:
    unsigned int m_engineCount;
    double m_specificImpulse;
    double m_thrustPerEngine;
    double m_propellantMass;
    QString m_type;
};

#endif // _STA_SCENARIO_ASCENT_H_
