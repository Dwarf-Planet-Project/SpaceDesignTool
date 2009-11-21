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
 ------------------ Author: Annalisa Riccardi  -------------------------------------------------
 ------------------ E-mail: (nina1983@gmail.com) ----------------------------
 */

#include <cstdlib>
#include "scenarioreentryproperties.h"
#include <QTreeWidgetItem>



ScenarioReentryProperties::ScenarioReentryProperties() :
    m_mainbody(NULL),
    m_parachute(NULL)
{
    m_mainbody = new ScenarioMainBodyCharacteristics();
    attachChild(m_mainbody);
    m_parachute = new ScenarioParachuteCharacteristics();
    attachChild(m_parachute);
}


ScenarioReentryProperties::~ScenarioReentryProperties()
{
    safeRelease(m_mainbody);
    safeRelease(m_parachute);
}


void ScenarioReentryProperties::writeContents(QXmlStreamWriter* out)
{
	m_mainbody->writeElement(out);
	m_parachute->writeElement(out);
}


void ScenarioReentryProperties::createItemContents(QTreeWidgetItem* item)
{
    m_mainbody->createItem(item);
    m_parachute->createItem(item);
}


void ScenarioReentryProperties::setMainBody(ScenarioMainBodyCharacteristics* mainbody)
{
    Q_ASSERT(mainbody != NULL);
    if (mainbody != m_mainbody)
    {
        detachChild(m_mainbody);
        m_mainbody = mainbody;
        attachChild(m_mainbody);
    }    
}


void ScenarioReentryProperties::setParachute(ScenarioParachuteCharacteristics* parachute)
{
	Q_ASSERT(parachute != NULL);
    if (parachute != m_parachute)
    {
        detachChild(m_parachute);
        m_parachute = parachute;
        attachChild(m_parachute);
    }    
}



/*** ScenarioMainBodyCharacteristics ***/

ScenarioMainBodyCharacteristics::ScenarioMainBodyCharacteristics() :
    m_mass(0.0),
    m_diameter(0.0),
	m_CDcoefficients(NULL),
	m_GLoadLimit(0.0),
	m_radiusNose(0.0),
	m_radiusBase(0.0),
	m_SPHeatRateLimit(0.0),
	m_maxAlt(0.0)
{
}


void ScenarioMainBodyCharacteristics::writeContents(QXmlStreamWriter* out)
{
    writeNumericElement(out, "Mass", m_mass, "Kg");
    writeNumericElement(out, "Diameter", m_diameter, "m");
    writeStringElement(out, "CDCoefficients", m_CDcoefficients);
    writeNumericElement(out, "GLoadLimit", m_GLoadLimit);
    writeNumericElement(out, "RadiusNose",m_radiusNose, "m");
    writeNumericElement(out, "RadiusBase", m_radiusBase, "m");
    writeNumericElement(out, "SPHeatRateLimit", m_SPHeatRateLimit, "Mj/m^2");
    writeNumericElement(out, "MaximumAltitude", m_maxAlt, "Km");
}


void ScenarioMainBodyCharacteristics::createItemContents(QTreeWidgetItem* item)
{
    QTreeWidgetItem* massItem = new QTreeWidgetItem(item);
    massItem->setText(0, QObject::tr("Mass"));
    massItem->setText(1, QString::number(m_mass));    
    
    QTreeWidgetItem* diameterItem = new QTreeWidgetItem(item);
    diameterItem->setText(0, QObject::tr("Diameter"));
    diameterItem->setText(1, QString::number(m_diameter));   
    
    QTreeWidgetItem* CDCoeffItem = new QTreeWidgetItem(item);
    CDCoeffItem->setText(0, QObject::tr("CD coefficients"));
    CDCoeffItem->setText(1, m_CDcoefficients);   
    
    QTreeWidgetItem* GLoadLimitItem = new QTreeWidgetItem(item);
    GLoadLimitItem->setText(0, QObject::tr("G Load Limit"));
    GLoadLimitItem->setText(1, QString::number(m_GLoadLimit));   
    
    QTreeWidgetItem* radiusNoseItem = new QTreeWidgetItem(item);
    radiusNoseItem->setText(0, QObject::tr("Radius Nose"));
    radiusNoseItem->setText(1, QString::number(m_radiusNose));   
    
    QTreeWidgetItem* radiusBaseItem = new QTreeWidgetItem(item);
    radiusBaseItem->setText(0, QObject::tr("Radius Base"));
    radiusBaseItem->setText(1, QString::number(m_radiusBase));   
    
    QTreeWidgetItem* SPHeatRateLimitItem = new QTreeWidgetItem(item);
    SPHeatRateLimitItem->setText(0, QObject::tr("Stagnation Point Heat Rate Limit"));
    SPHeatRateLimitItem->setText(1, QString::number(m_SPHeatRateLimit));   
    
    QTreeWidgetItem* maxAltItem = new QTreeWidgetItem(item);
    maxAltItem->setText(0, QObject::tr("Maximum Altitude"));
    maxAltItem->setText(1, QString::number(m_maxAlt));   

}

/*** ScenarioParachuteCharacteristics ***/

ScenarioParachuteCharacteristics::ScenarioParachuteCharacteristics() :
	m_mass(0.0),
        m_surface(0.0),
	m_CDcoefficients(NULL),
        m_deploymentMach(0.0)
{
}


void ScenarioParachuteCharacteristics::writeContents(QXmlStreamWriter* out)
{
    writeNumericElement(out, "Mass", m_mass, "Kg");
    writeNumericElement(out, "Surface", m_surface, "m");
    writeStringElement(out, "CDCoefficients", m_CDcoefficients);
    writeNumericElement(out, "DeploymentMachNumber", m_deploymentMach, "-");
}


void ScenarioParachuteCharacteristics::createItemContents(QTreeWidgetItem* item)
{
    QTreeWidgetItem* massItem = new QTreeWidgetItem(item);
    massItem->setText(0, QObject::tr("Mass"));
    massItem->setText(1, QString::number(m_mass));    
    
    QTreeWidgetItem* surfaceItem = new QTreeWidgetItem(item);
    surfaceItem->setText(0, QObject::tr("Surface Area"));
    surfaceItem->setText(1, QString::number(m_surface));
    
    QTreeWidgetItem* CDCoeffItem = new QTreeWidgetItem(item);
    CDCoeffItem->setText(0, QObject::tr("CD coefficients"));
    CDCoeffItem->setText(1, m_CDcoefficients);   
    
    QTreeWidgetItem* deploymentMachItem = new QTreeWidgetItem(item);
    deploymentMachItem->setText(0, QObject::tr("Deployment Mach Number"));
    deploymentMachItem->setText(1, QString::number(m_deploymentMach));
}
