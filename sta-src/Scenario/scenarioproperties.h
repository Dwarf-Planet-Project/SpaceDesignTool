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

#ifndef _STA_SCENARIO_PROPERTIES_H_
#define _STA_SCENARIO_PROPERTIES_H_

#include "scenarioobject.h"

class ScenarioPhysicalProperties;
class ScenarioAerodynamicProperties;
class ScenarioPropulsionProperties;
class ScenarioPhysicalCharacteristics;
class ScenarioGeometricalCharacteristics;
class ScenarioParachuteProperties;
class ScenarioPayloadProperties;
//class ScenarioDesignOverview;

class ScenarioProperties : public ScenarioObject
{
public:
    ScenarioProperties();
    ~ScenarioProperties();
    
    virtual QString elementName() const { return "Properties"; }
    virtual QString displayName() const { return QObject::tr("Properties"); }
    virtual void writeContents(QXmlStreamWriter* out);
    virtual void createItemContents(QTreeWidgetItem* item);
    
    ScenarioPhysicalProperties* physicalProperties() const { return m_physicalproperties; }
    void setPhysicalProperties(ScenarioPhysicalProperties* physicalproperties);
    ScenarioAerodynamicProperties* aerodynamicProperties() const { return m_aerodynamicproperties; }
    void setAerodynamicProperties(ScenarioAerodynamicProperties* aerodynamicproperties);
    ScenarioPropulsionProperties* propulsionProperties() const { return m_propulsionproperties; }
    void setPropulsionProperties(ScenarioPropulsionProperties* propulsionproperties);
    ScenarioPayloadProperties* payloadProperties() const { return m_payloadproperties; }
    void setPayloadProperties(ScenarioPayloadProperties* payloadproperties);
//    ScenarioMainBodyCharacteristics* mainBody() const { return m_mainBody; }
//    void setMainBody(ScenarioMainBodyCharacteristics* mainBody);
//    ScenarioParachuteCharacteristics* parachute() const { return m_parachute; }
//    void setParachute(ScenarioParachuteCharacteristics* parachute);
//    ScenarioDesignOverview* designOverview() const { return m_designOverview; }
//    void setDesignOverview(ScenarioDesignOverview* designOverview);
    
private:
    ScenarioPhysicalProperties* m_physicalproperties;
    ScenarioAerodynamicProperties* m_aerodynamicproperties;
    ScenarioPropulsionProperties* m_propulsionproperties;
    ScenarioPayloadProperties* m_payloadproperties;
//    ScenarioMainBodyCharacteristics* m_mainBody;
//    ScenarioParachuteCharacteristics* m_parachute;
//    ScenarioDesignOverview* m_designOverview;
};


class ScenarioPhysicalProperties : public ScenarioObject
{
public:
    ScenarioPhysicalProperties();
    ~ScenarioPhysicalProperties();

    virtual QString elementName() const { return "PhysicalProperties"; }
    virtual QString displayName() const { return QObject::tr("Physical and Geometrical Properties"); }
    virtual void writeContents(QXmlStreamWriter* out);
    virtual void createItemContents(QTreeWidgetItem* item);

    ScenarioPhysicalCharacteristics* physicalCharacteristics() const { return m_physicalcharacteristics; }
    void setPhysicalCharacteristics(ScenarioPhysicalCharacteristics* physicalcharacteristics);
    ScenarioGeometricalCharacteristics* geometricalCharacteristics() const { return m_geometricalcharacteristics; }
    void setGeometricalCharacteristics(ScenarioGeometricalCharacteristics* geometricalcharacteristics);

private:
    ScenarioPhysicalCharacteristics* m_physicalcharacteristics;
    ScenarioGeometricalCharacteristics* m_geometricalcharacteristics;
};

class ScenarioPhysicalCharacteristics : public ScenarioObject
{
public:
    ScenarioPhysicalCharacteristics();
    ~ScenarioPhysicalCharacteristics();

    virtual QString elementName() const { return "PhysicalCharacteristics"; }
    virtual QString displayName() const { return QObject::tr("Physical Characteristics"); }
    virtual void writeContents(QXmlStreamWriter* out);
    virtual void createItemContents(QTreeWidgetItem* item);

    /*! Return the mass in kilograms. */
    double mass() const { return m_mass; }
    /*! Set the mass in kilograms. */
    void setMass(double kg) { m_mass = kg; }

    /*! Return the surface area in meters^2. */
    double surfaceArea() const { return m_surfaceArea; }
    /*! Set the surface area in meters^2. */
    void setSurfaceArea(double m2) { m_surfaceArea = m2; }

    /*! Return the volume in meters^3. */
    double volume() const { return m_volume; }
    /*! Set the volume in meters^3. */
    void setVolume(double m3) { m_volume = m3; }

private:
    double m_mass;        // mass in kg
    double m_surfaceArea; // surface area in m^2
    double m_volume;      // volume in m^3
};


class ScenarioGeometricalCharacteristics : public ScenarioObject
{
public:
    ScenarioGeometricalCharacteristics();
    ~ScenarioGeometricalCharacteristics();
    
    virtual QString elementName() const { return "GeometricalCharacteristics"; }
    virtual QString displayName() const { return QObject::tr("Geometrical Characteristics"); }
    virtual void writeContents(QXmlStreamWriter* out);
    virtual void createItemContents(QTreeWidgetItem* item);
    
    /*! Return the base radius of the capsule. */
    double radiusBase() const { return m_radiusBase; }
    /*! Set the base radius of the capsule. */
    void setRadiusBase(double radiusbase) { m_radiusBase = radiusbase; }

    /*! Return the nose radius of the capsule. */
    double radiusNose() const { return m_radiusNose; }
    /*! Set the nose radius of the capsule. */
    void setRadiusNose(double radiusnose) { m_radiusNose = radiusnose; }

private:
    double m_radiusBase;
    double m_radiusNose;
};

class ScenarioAerodynamicProperties : public ScenarioObject
{
public:
    ScenarioAerodynamicProperties();
    ~ScenarioAerodynamicProperties();

    virtual QString elementName() const { return "AerodynamicProperties"; }
    virtual QString displayName() const { return QObject::tr("Aerodynamic and Thermal Properties"); }
    virtual void writeContents(QXmlStreamWriter* out);
    virtual void createItemContents(QTreeWidgetItem* item);

    /*! Return the CD coefficients file name. */
    QString CDCoefficients() const { return m_CDCoefficients; }
    /*! Set the the CD coefficients file name. */
    void setCDCoefficients(QString file) { m_CDCoefficients = file; }

    /*! Return the GLoadLimit. */
    double GLoadLimit() const { return m_GLoadLimit; }
    /*! Set the GLoadLimit. */
    void setGLoadLimit(double gll) { m_GLoadLimit = gll; }

    /*! Return the stagnation point heat rate limit in Mj/m^2. */
    double SPHeatRateLimit() const { return m_SPHeatRateLimit; }
    /*! Set the stagnation point heat rate limit in Mj/m^2. */
    void setSPHeatRateLimit(double sp) { m_SPHeatRateLimit = sp; }

    ScenarioParachuteProperties* parachuteProperties() const { return m_parachuteproperties; }
    void setParachuteProperties(ScenarioParachuteProperties* parachuteproperties);

private:
    QString m_CDCoefficients;
    double m_GLoadLimit;
    double m_SPHeatRateLimit;
    ScenarioParachuteProperties* m_parachuteproperties;
};

class ScenarioParachuteProperties : public ScenarioObject
{
public:
    ScenarioParachuteProperties();
    ~ScenarioParachuteProperties();

    virtual QString elementName() const { return "ParachuteProperties"; }
    virtual QString displayName() const { return QObject::tr("Parachute Properties"); }
    virtual void writeContents(QXmlStreamWriter* out);
    virtual void createItemContents(QTreeWidgetItem* item);

    /*! Return the base radius of the capsule. */
    double surfaceArea() const { return m_surfaceArea; }
    /*! Set the base radius of the capsule. */
    void setSurfaceArea(double surfaceArea) { m_surfaceArea = surfaceArea; }

    /*! Return the CD coefficients file name. */
    QString CDCoefficients() const { return m_CDCoefficients; }
    /*! Set the the CD coefficients file name. */
    void setCDCoefficients(QString file) { m_CDCoefficients = file; }

    /*! Return the deployment Mach number. */
    double deploymentMach() const { return m_deploymentMach; }
    /*! Set the deployment Mach number. */
    void setDeploymentMach(double dmn) { m_deploymentMach = dmn; }

private:
    double m_surfaceArea;
    QString m_CDCoefficients;
    double  m_deploymentMach;
};

class ScenarioPropulsionProperties : public ScenarioObject
{
public:
    ScenarioPropulsionProperties();
    ~ScenarioPropulsionProperties();

    QString elementName() const { return "PropulsionProperties"; }
    QString displayName() const { return QObject::tr("Propulsion Properties"); }

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
    double dV() const { return m_dV; }
    void setDV(double dV) { m_dV = dV; }
    QString type() const { return m_type; }
    void setType(QString type) { m_type = type; }

private:
    unsigned int m_engineCount;
    double m_specificImpulse;
    double m_thrustPerEngine;
    double m_propellantMass;
    double m_dV;
    QString m_type;
};

class ScenarioPayloadProperties : public ScenarioObject
{
public:
    ScenarioPayloadProperties();
    ~ScenarioPayloadProperties();

    QString elementName() const { return "PayloadProperties"; }
    QString displayName() const { return QObject::tr("Payload Properties"); }

    virtual void writeContents(QXmlStreamWriter* out);
    virtual void createItemContents(QTreeWidgetItem* item);

    double massPayload() const { return m_massPayload; }
    void setMassPayload(unsigned int massPayload) { m_massPayload = massPayload; }
    double powerPayload() const { return m_powerPayload; }
    void setPowerPayload(double powerPayload) { m_powerPayload = powerPayload; }

private:
    double m_massPayload;
    double m_powerPayload;
};

#endif // _STA_SCENARIO_PROPERTIES_H_
