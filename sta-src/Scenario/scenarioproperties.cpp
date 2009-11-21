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

#include <cstdlib>
#include "scenarioproperties.h"
#include "scenarioreentryproperties.h"
//#include "scenariopayload.h"
//#include "scenariodesignoverview.h"
#include <QTreeWidgetItem>
#include <QDebug>


ScenarioProperties::ScenarioProperties() :
    m_physicalproperties(NULL),
    m_aerodynamicproperties(NULL),
    m_propulsionproperties(NULL),
    m_payloadproperties(NULL)
    //m_mainBody(NULL),
    //m_parachute(NULL),
    //m_designOverview(NULL)
{
    m_physicalproperties = new ScenarioPhysicalProperties();
    attachChild(m_physicalproperties);

    m_aerodynamicproperties = new ScenarioAerodynamicProperties();
    attachChild(m_aerodynamicproperties);

    m_propulsionproperties = new ScenarioPropulsionProperties();
    attachChild(m_propulsionproperties);

    m_payloadproperties = new ScenarioPayloadProperties();
    attachChild(m_payloadproperties);
}


ScenarioProperties::~ScenarioProperties()
{
    safeRelease(m_physicalproperties);
    safeRelease(m_aerodynamicproperties);
    safeRelease(m_propulsionproperties);
    safeRelease(m_payloadproperties);
//    safeRelease(m_parachute);
//    safeRelease(m_designOverview);
}


void ScenarioProperties::writeContents(QXmlStreamWriter* out)
{
    m_physicalproperties->writeElement(out);
    if (m_aerodynamicproperties)
        m_aerodynamicproperties->writeElement(out);
    if (m_propulsionproperties)
        m_propulsionproperties->writeElement(out);
    if (m_payloadproperties)
        m_payloadproperties->writeElement(out);
//    if (m_parachute)
//        m_parachute->writeElement(out);
//    if (m_designOverview)
//        m_designOverview->writeElement(out);
}


void ScenarioProperties::createItemContents(QTreeWidgetItem* item)
{
    m_physicalproperties->createItem(item);
    if (m_aerodynamicproperties)
        m_aerodynamicproperties->createItem(item);
    if (m_propulsionproperties)
        m_propulsionproperties->createItem(item);
    if (m_payloadproperties)
        m_payloadproperties->createItem(item);
//    if (m_parachute)
//        m_parachute->createItem(item);
//    if (m_designOverview)
//        m_designOverview->createItem(item);
}


void ScenarioProperties::setPhysicalProperties(ScenarioPhysicalProperties* physicalproperties)
{
    Q_ASSERT(physicalproperties != NULL);
    if (physicalproperties != m_physicalproperties)
    {
        detachChild(m_physicalproperties);
        m_physicalproperties = physicalproperties;

        attachChild(m_physicalproperties);
    }    
}

void ScenarioProperties::setAerodynamicProperties(ScenarioAerodynamicProperties* aerodynamicproperties)
{
    Q_ASSERT(aerodynamicproperties != NULL);
    if (aerodynamicproperties != m_aerodynamicproperties)
    {
        detachChild(m_aerodynamicproperties);
        m_aerodynamicproperties = aerodynamicproperties;

        attachChild(m_aerodynamicproperties);
    }
}

void ScenarioProperties::setPropulsionProperties(ScenarioPropulsionProperties* propulsionproperties)
{
    Q_ASSERT(propulsionproperties != NULL);
    if (propulsionproperties != m_propulsionproperties)
    {
        detachChild(m_propulsionproperties);
        m_propulsionproperties = propulsionproperties;

        attachChild(m_propulsionproperties);
    }
}

void ScenarioProperties::setPayloadProperties(ScenarioPayloadProperties* payloadproperties)
{
    Q_ASSERT(payloadproperties != NULL);
    if (payloadproperties != m_payloadproperties)
    {
        detachChild(m_payloadproperties);
        m_payloadproperties = payloadproperties;

        attachChild(m_payloadproperties);
    }
}

//void ScenarioProperties::setTechnical(ScenarioTechnicalCharacteristics* technical)
//{
//    if (technical != m_technical)
//    {
//        detachChild(m_technical);
//        m_technical = technical;
//        attachChild(m_technical);
//    }
//}
//
//
//void ScenarioProperties::setMainBody(ScenarioMainBodyCharacteristics* mainBody)
//{
//    if (mainBody != m_mainBody)
//    {
//        detachChild(m_mainBody);
//        m_mainBody = mainBody;
//        attachChild(m_mainBody);
//    }
//}
//
//
//void ScenarioProperties::setParachute(ScenarioParachuteCharacteristics* parachute)
//{
//    if (parachute != m_parachute)
//    {
//        detachChild(m_parachute);
//        m_parachute = parachute;
//        attachChild(m_parachute);
//    }
//}
//
//
//void ScenarioProperties::setDesignOverview(ScenarioDesignOverview* designOverview)
//{
//    if (designOverview != m_designOverview)
//    {
//        detachChild(m_designOverview);
//        m_designOverview = designOverview;
//        attachChild(m_designOverview);
//    }
//}

/*** ScenarioPhysicalProperties ***/
ScenarioPhysicalProperties::ScenarioPhysicalProperties() :
    m_physicalcharacteristics(NULL),
    m_geometricalcharacteristics(NULL)
{
    m_physicalcharacteristics = new ScenarioPhysicalCharacteristics();
    attachChild(m_physicalcharacteristics);
}

ScenarioPhysicalProperties::~ScenarioPhysicalProperties()
{
}

void ScenarioPhysicalProperties::writeContents(QXmlStreamWriter* out)
{
    m_physicalcharacteristics->writeElement(out);
    if (m_geometricalcharacteristics)
        m_geometricalcharacteristics->writeElement(out);
}

void ScenarioPhysicalProperties::createItemContents(QTreeWidgetItem* item)
{
    m_physicalcharacteristics->createItem(item);
    if (m_geometricalcharacteristics)
        m_geometricalcharacteristics->createItem(item);
}

void ScenarioPhysicalProperties::setPhysicalCharacteristics(ScenarioPhysicalCharacteristics* physicalcharacteristics)
{
    Q_ASSERT(physicalcharacteristics != NULL);
    if (physicalcharacteristics != m_physicalcharacteristics)
    {
        detachChild(m_physicalcharacteristics);
        m_physicalcharacteristics = physicalcharacteristics;
        attachChild(m_physicalcharacteristics);
    }
}

void ScenarioPhysicalProperties::setGeometricalCharacteristics(ScenarioGeometricalCharacteristics* geometricalcharacteristics)
{
    if (geometricalcharacteristics != m_geometricalcharacteristics)
    {
        detachChild(m_geometricalcharacteristics);
        m_geometricalcharacteristics = geometricalcharacteristics;
        attachChild(m_geometricalcharacteristics);
    }
}

/*** ScenarioPhysicalCharacteristics ***/

ScenarioPhysicalCharacteristics::ScenarioPhysicalCharacteristics() :
    m_mass(0.0),
    m_surfaceArea(0.0),
    m_volume(0.0)
{
}

ScenarioPhysicalCharacteristics::~ScenarioPhysicalCharacteristics()
{
}

void ScenarioPhysicalCharacteristics::writeContents(QXmlStreamWriter* out)
{
    writeNumericElement(out, "volume", m_volume, "m^3");
    writeNumericElement(out, "mass", m_mass, "Kg");
    writeNumericElement(out, "surface", m_surfaceArea, "m^2");
}

void ScenarioPhysicalCharacteristics::createItemContents(QTreeWidgetItem* item)
{
    QTreeWidgetItem* massItem = new QTreeWidgetItem(item);
    massItem->setText(0, QObject::tr("Mass"));
    massItem->setText(1, QString::number(m_mass));    
    
    QTreeWidgetItem* surfaceAreaItem = new QTreeWidgetItem(item);
    surfaceAreaItem->setText(0, QObject::tr("Surface Area"));
    surfaceAreaItem->setText(1, QString::number(m_surfaceArea));    
    
    QTreeWidgetItem* volumeItem = new QTreeWidgetItem(item);
    volumeItem->setText(0, QObject::tr("Volume"));
    volumeItem->setText(1, QString::number(m_volume));
}

/*** ScenarioGeometricalCharacteristics ***/

ScenarioGeometricalCharacteristics::ScenarioGeometricalCharacteristics() :
    m_radiusBase(0.0),
    m_radiusNose(0.0)
{
}

ScenarioGeometricalCharacteristics::~ScenarioGeometricalCharacteristics()
{
}

void ScenarioGeometricalCharacteristics::writeContents(QXmlStreamWriter* out)
{
    writeNumericElement(out, "radiusBase", m_radiusBase, "m");
    writeNumericElement(out, "radiusNose", m_radiusNose, "m");
}

void ScenarioGeometricalCharacteristics::createItemContents(QTreeWidgetItem* item)
{
    QTreeWidgetItem* radiusBaseItem = new QTreeWidgetItem(item);
    radiusBaseItem->setText(0, QObject::tr("Radius Base"));
    radiusBaseItem->setText(1, QString::number(m_radiusBase));

    QTreeWidgetItem* radiusNoseItem = new QTreeWidgetItem(item);
    radiusNoseItem->setText(0, QObject::tr("Radius Nose"));
    radiusNoseItem->setText(1, QString::number(m_radiusNose));
}

/*** ScenarioAerodynamicProperties ***/
ScenarioAerodynamicProperties::ScenarioAerodynamicProperties() :
    m_CDCoefficients(NULL),
    m_GLoadLimit(0.0),
    m_SPHeatRateLimit(0.0),
    m_parachuteproperties(NULL)
{
}

ScenarioAerodynamicProperties::~ScenarioAerodynamicProperties()
{
}

void ScenarioAerodynamicProperties::writeContents(QXmlStreamWriter* out)
{
    writeStringElement(out, "CDCoefficients", m_CDCoefficients);
    writeNumericElement(out, "GLoadLimit", m_GLoadLimit, "-");
    writeNumericElement(out, "SPHeatRateLimit", m_SPHeatRateLimit, "MJ/m2");
    if (m_parachuteproperties)
        m_parachuteproperties->writeElement(out);
}

void ScenarioAerodynamicProperties::createItemContents(QTreeWidgetItem* item)
{
    QTreeWidgetItem* CDCoefficientsItem = new QTreeWidgetItem(item);
    CDCoefficientsItem->setText(0, QObject::tr("CD coefficients"));
    CDCoefficientsItem->setText(1, m_CDCoefficients);

    QTreeWidgetItem* GLoadLimitItem = new QTreeWidgetItem(item);
    GLoadLimitItem->setText(0, QObject::tr("G Load Limit"));
    GLoadLimitItem->setText(1, QString::number(m_GLoadLimit));

    QTreeWidgetItem* SPHeatRateLimitItem = new QTreeWidgetItem(item);
    SPHeatRateLimitItem->setText(0, QObject::tr("SP Heat Rate Limit"));
    SPHeatRateLimitItem->setText(1, QString::number(m_SPHeatRateLimit));

    if (m_parachuteproperties)
        m_parachuteproperties->createItem(item);
}

void ScenarioAerodynamicProperties::setParachuteProperties(ScenarioParachuteProperties* parachuteproperties)
{
    if (parachuteproperties != m_parachuteproperties)
    {
        detachChild(m_parachuteproperties);
        m_parachuteproperties = parachuteproperties;
        attachChild(m_parachuteproperties);
    }
}

/*** ScenarioParachuteProperties ***/

ScenarioParachuteProperties::ScenarioParachuteProperties() :
    m_surfaceArea(0.0),
    m_CDCoefficients(NULL),
    m_deploymentMach(0.0)
{
}

ScenarioParachuteProperties::~ScenarioParachuteProperties()
{
}

void ScenarioParachuteProperties::writeContents(QXmlStreamWriter* out)
{
    writeNumericElement(out, "SurfaceArea", m_surfaceArea, "m2");
    writeStringElement(out, "CDCoefficients", m_CDCoefficients);
    writeNumericElement(out, "DeploymentMach", m_deploymentMach, "-");
}

void ScenarioParachuteProperties::createItemContents(QTreeWidgetItem* item)
{
    QTreeWidgetItem* CDCoefficientsItem = new QTreeWidgetItem(item);
    CDCoefficientsItem->setText(0, QObject::tr("CD coefficients"));
    CDCoefficientsItem->setText(1, m_CDCoefficients);

    QTreeWidgetItem* surfaceAreaItem = new QTreeWidgetItem(item);
    surfaceAreaItem->setText(0, QObject::tr("Surface Area"));
    surfaceAreaItem->setText(1, QString::number(m_surfaceArea));

    QTreeWidgetItem* deploymentMachItem = new QTreeWidgetItem(item);
    deploymentMachItem->setText(0, QObject::tr("Deployment Mach Number"));
    deploymentMachItem->setText(1, QString::number(m_deploymentMach));
}

/*** ScenarioPropulsionProperties ***/
ScenarioPropulsionProperties::ScenarioPropulsionProperties() :
    m_engineCount(1),
    m_specificImpulse(0.0),
    m_thrustPerEngine(0.0),
    m_propellantMass(0.0),
    m_dV(0.0),
    m_type("THERMAL-SOLID")
{
}

ScenarioPropulsionProperties::~ScenarioPropulsionProperties()
{
}

void ScenarioPropulsionProperties::writeContents(QXmlStreamWriter* out)
{
    writeStringElement(out, "NumberOfEngines", QString::number(m_engineCount));
    writeStringElement(out, "Isp", QString::number(m_specificImpulse));
    writeStringElement(out, "ThrustPerEngine", QString::number(m_thrustPerEngine));
    writeStringElement(out, "PropellantMass", QString::number(m_propellantMass));
    writeStringElement(out, "Dv", QString::number(m_dV));
    writeStringElement(out, "Type", m_type);
}

void ScenarioPropulsionProperties::createItemContents(QTreeWidgetItem* item)
{
   item->setText(1, m_type);

    QTreeWidgetItem* enginesItem = new QTreeWidgetItem(item);
    enginesItem->setText(0, QObject::tr("Engines"));
    enginesItem->setText(1, QString::number(m_engineCount));

    QTreeWidgetItem* thrustItem = new QTreeWidgetItem(item);
    thrustItem->setText(0, QObject::tr("Thrust/engine"));
    thrustItem->setText(1, QString::number(m_thrustPerEngine));

    QTreeWidgetItem* ispItem = new QTreeWidgetItem(item);
    ispItem->setText(0, QObject::tr("Isp"));
    ispItem->setText(1, QString::number(m_specificImpulse));

    QTreeWidgetItem* massItem = new QTreeWidgetItem(item);
    massItem->setText(0, QObject::tr("Propellant Mass"));
    massItem->setText(1, QString::number(m_propellantMass));

    QTreeWidgetItem* dvItem = new QTreeWidgetItem(item);
    dvItem->setText(0, QObject::tr("DV"));
    dvItem->setText(1, QString::number(m_dV));
}

/*** ScenarioPayloadProperties ***/
ScenarioPayloadProperties::ScenarioPayloadProperties() :
    m_massPayload(0.0),
    m_powerPayload(0.0)
{
}

ScenarioPayloadProperties::~ScenarioPayloadProperties()
{
}

void ScenarioPayloadProperties::writeContents(QXmlStreamWriter* out)
{
    writeStringElement(out, "PayloadMass", QString::number(m_massPayload));
    writeStringElement(out, "PayloadPower", QString::number(m_powerPayload));
}

void ScenarioPayloadProperties::createItemContents(QTreeWidgetItem* item)
{
    QTreeWidgetItem* massPayloadItem = new QTreeWidgetItem(item);
    massPayloadItem->setText(0, QObject::tr("Mass"));
    massPayloadItem->setText(1, QString::number(m_massPayload));

    QTreeWidgetItem* powerPayloadItem = new QTreeWidgetItem(item);
    powerPayloadItem->setText(0, QObject::tr("Power"));
    powerPayloadItem->setText(1, QString::number(m_powerPayload));
}
