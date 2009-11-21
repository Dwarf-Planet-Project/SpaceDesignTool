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

#ifndef _STA_SCENARIO_REENTRY_PROPERTIES_H_
#define _STA_SCENARIO_REENTRY_PROPERTIES_H_

#include "scenarioobject.h"

class ScenarioMainBodyCharacteristics;
class ScenarioParachuteCharacteristics;

class ScenarioReentryProperties : public ScenarioObject
{
    public:
        ScenarioReentryProperties();
        ~ScenarioReentryProperties();

        virtual QString elementName() const { return "Vehicle"; }
        virtual QString displayName() const { return QObject::tr("Main Body Properties"); }
        virtual void writeContents(QXmlStreamWriter* out);
        virtual void createItemContents(QTreeWidgetItem* item);

        ScenarioMainBodyCharacteristics* mainBodyCharacteristics() const {return m_mainbody;}
        void setMainBody(ScenarioMainBodyCharacteristics* mainBodyCharacteristics);

        ScenarioParachuteCharacteristics* parachuteCharacteristics() const {return m_parachute;}
        void setParachute(ScenarioParachuteCharacteristics* parachuteCharacteristics);

    private:
        ScenarioMainBodyCharacteristics* m_mainbody;
        ScenarioParachuteCharacteristics* m_parachute;

};

class ScenarioMainBodyCharacteristics : public ScenarioObject
{
public:
    ScenarioMainBodyCharacteristics();
    ~ScenarioMainBodyCharacteristics(){}
	
    virtual QString elementName() const { return "MainBody"; }
    virtual QString displayName() const { return QObject::tr("Main Body"); }
    virtual void writeContents(QXmlStreamWriter* out);
    virtual void createItemContents(QTreeWidgetItem* item);
  
    /*! Return the mass in kilograms. */
    double mass() const { return m_mass; }
    /*! Set the mass in kilograms. */
    void setMass(double kg) { m_mass = kg; }
    
    /*! Return the diameter in meters. */
    double diameter() const { return m_diameter; }
    /*! Set the diameter in meter. */
    void setDiameter(double m) { m_diameter = m; }
    
    /*! Return the CD coefficients file name. */
    QString CDCoefficients() const { return m_CDcoefficients; }
    /*! Set the the CD coefficients file name. */
    void setCDCoefficients(QString file) { m_CDcoefficients = file; }
    
    /*! Return the GLoadLimit. */
    double GLoadLimit() const { return m_GLoadLimit; }
    /*! Set the GLoadLimit. */
    void setGLoadLimit(double gll) { m_GLoadLimit = gll; }
    
    /*! Return the radius nose in meters. */
    double radiusNose() const { return m_radiusNose; }
    /*! Set the radius nose in meters. */
    void setRadiusNose(double m) { m_radiusNose = m; }
    
    /*! Return the radius base in meters. */
    double radiusBase() const { return m_radiusBase; }
    /*! Set the radius base in meters. */
    void setRadiusBase(double m) { m_radiusBase = m; }
    
    /*! Return the stagnation point heat rate limit in Mj/m^2. */
    double SPHeatRateLimit() const { return m_SPHeatRateLimit; }
    /*! Set the stagnation point heat rate limit in Mj/m^2. */
    void setSPHeatRateLimit(double sp) { m_SPHeatRateLimit = sp; }
    
    /*! Return the maximum altitude in Km. */
    double maxAltitude() const { return m_maxAlt; }
    /*! Set the maximum altitude in Km. */
    void setMaxAltitude(double km) { m_maxAlt = km; }
    
private:
    double m_mass;
    double m_diameter;
    QString m_CDcoefficients;
    double m_GLoadLimit;
    double m_radiusNose;
    double m_radiusBase;
    double m_SPHeatRateLimit;
    double m_maxAlt;
};


class ScenarioParachuteCharacteristics : public ScenarioObject
{
public:
	ScenarioParachuteCharacteristics();
	~ScenarioParachuteCharacteristics(){}
	
    virtual QString elementName() const { return "Parachute"; }
    virtual QString displayName() const { return QObject::tr("Parachute"); }
    virtual void writeContents(QXmlStreamWriter* out);
    virtual void createItemContents(QTreeWidgetItem* item);
  
    /*! Return the mass in kilograms. */
    double mass() const { return m_mass; }
    /*! Set the mass in kilograms. */
    void setMass(double kg) { m_mass = kg; }
    
    /*! Return the surface in square meters. */
    double surface() const { return m_surface; }
    /*! Set the surface in square meters. */
    void setSurface(double m2) { m_surface = m2; }
    
    /*! Return the CD coefficients file name. */
    QString CDCoefficients() const { return m_CDcoefficients; }
    /*! Set the the CD coefficients file name. */
    void setCDCoefficients(QString file) { m_CDcoefficients = file; }
    
    /*! Return the deployment Mach number. */
    double deploymentMach() const { return m_deploymentMach; }
    /*! Set the deployment Mach number. */
    void setDeploymentMach(double dmn) { m_deploymentMach = dmn; }
    
private:
	double m_mass;
        double m_surface;
	QString m_CDcoefficients;
        double m_deploymentMach;
};

#endif //_STA_SCENARIO_REENTRY_PROPERTIES_H_


