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

#ifndef _STA_REENTRY_MODE_H_
#define _STA_REENTRY_MODE_H_

#include <QHash>
#include "scenarioobject.h"

class ScenarioTargettingSimulationParameters;
class ScenarioDispersionAnalysis;
class ScenarioFixedVariables;
class ScenarioDeviations;
class ScenarioWindow;
class ScenarioAbstractReentryTrajectoryParam;

class ScenarioSimulationMode : public ScenarioObject
{
    public:
	ScenarioSimulationMode();
	~ScenarioSimulationMode();
	
    QString elementName() const { return "SimulationMode"; }
    QString displayName() const { return QObject::tr("Simulation Mode"); }

    virtual void writeContents(QXmlStreamWriter* out);
    virtual void createItemContents(QTreeWidgetItem* item);
    
    ScenarioTargettingSimulationParameters* simulationParameters() const {return m_simulationParameters;}
    void setSimulationParameters(ScenarioTargettingSimulationParameters* simparam);
    
    ScenarioDispersionAnalysis* dispersionAnalysis() const {return m_dispersionAnalysis;}
    void setDispersionAnalysis(ScenarioDispersionAnalysis* dispersion);

    bool isActive() const { return m_isActive; }
    void setIsActiveTrue() { m_isActive = true; }
    void setIsActiveFalse() { m_isActive = false; }

    private:
        bool m_isActive;
	ScenarioTargettingSimulationParameters* m_simulationParameters;
	ScenarioDispersionAnalysis* m_dispersionAnalysis;
	
};


class ScenarioWindowMode : public ScenarioObject
{
public:
	ScenarioWindowMode();
	~ScenarioWindowMode();
	
    QString elementName() const { return "WindowAnalysisMode"; }
    QString displayName() const { return QObject::tr("Window Analysis Mode"); }
    
    virtual void writeContents(QXmlStreamWriter* out);
    virtual void createItemContents(QTreeWidgetItem* item);
    
    ScenarioTargettingSimulationParameters* simulationParameters() const {return m_simulationParameters;}
    void setSimulationParameters(ScenarioTargettingSimulationParameters* simparam);
    ScenarioFixedVariables* fixedVariables() const {return m_fixedVariables;}
    void setFixedVariables(ScenarioFixedVariables* variables);
    int numSimulations() const{ return m_numSimulations;}
    void setnumSimulations(int num){ m_numSimulations = num;}

    ScenarioWindow* window() const {return m_window;}
    void setWindow(ScenarioWindow* window);

    bool isActive() const { return m_isActive; }
    void setIsActiveTrue() { m_isActive = true; }
    void setIsActiveFalse() { m_isActive = false; }
    

private:
        bool m_isActive;
	ScenarioTargettingSimulationParameters* m_simulationParameters;
	ScenarioFixedVariables* m_fixedVariables;
	int m_numSimulations;
	ScenarioWindow* m_window;
	
	
};

class ScenarioDispersionAnalysis : public ScenarioObject
{

public:
	ScenarioDispersionAnalysis();
	~ScenarioDispersionAnalysis();
	
    QString elementName() const { return "DispersionAnalysis"; }
    QString displayName() const { return QObject::tr("Dispersion Analysis"); }
    virtual void writeContents(QXmlStreamWriter* out);
    virtual void createItemContents(QTreeWidgetItem* item);
    
    int numSimulations() const{ return m_numSimulations;}
    void setnumSimulations(int num){ m_numSimulations = num;}
    
    ScenarioDeviations* deviations() const{ return m_deviations;}
    void setDeviations(ScenarioDeviations* deviations);

    bool isActive() const { return m_isActive; }
    void setIsActiveTrue() { m_isActive = true; }
    void setIsActiveFalse() { m_isActive = false; }

private:
    bool m_isActive;
    int m_numSimulations; 
    ScenarioDeviations* m_deviations;
	
};

class ScenarioDeviations : public ScenarioObject
{

public:
	ScenarioDeviations();
	~ScenarioDeviations();
	
    QString elementName() const { return "Deviations"; }
    QString displayName() const { return QObject::tr("Deviations"); }
    virtual void writeContents(QXmlStreamWriter* out);
    virtual void createItemContents(QTreeWidgetItem* item);
    
    double capsuleBaseRadius() const { return m_capsuleBaseRadius; }
    void setCapsuleBaseRadius(double cbr){m_capsuleBaseRadius = cbr;}
    
    double capsuleMass() const { return m_capsuleMass; }
    void setCapsuleMass(double cm){m_capsuleMass = cm;}
    
    double capsuleCD() const { return m_capsuleCD; }
    void setCapsuleCD(double cCD){m_capsuleCD = cCD;}
    
    double parachuteCD() const { return m_parachuteCD; }
    void setParachuteCD(double pCD){m_parachuteCD = pCD;}
    
    double atmosphereDensity() const { return m_atmosphereDensity; }
    void setAtmosphereDensity(double ad){m_atmosphereDensity = ad;}
    
    double entryLong() const { return m_entryLong; }
    void setEntryLong(double elo){m_entryLong = elo;}
    
    double entryLat() const { return m_entryLat; }
    void setEntryLat(double ela){m_entryLat = ela;}
    
    double velocity() const { return m_velocity; }
    void setVelocity(double v){m_velocity = v;}
    
    double flightPathAngle() const { return m_flightPathAngle; }
    void setFlightPathAngle(double fpa){m_flightPathAngle = fpa;}
    
    double inertialHeading() const { return m_inertialHeading; }
    void setInertialHeading(double ih){m_inertialHeading = ih;}

private:
	double m_capsuleBaseRadius;
	double m_capsuleMass;
	double m_capsuleCD;
	double m_parachuteCD;
        double m_atmosphereDensity;
	double m_entryLong;
	double m_entryLat;
	double m_velocity;
	double m_flightPathAngle;
	double m_inertialHeading;
	
	
};

class ScenarioFixedVariables : public ScenarioObject
{

public:
	ScenarioFixedVariables();
	~ScenarioFixedVariables();
	
    QString elementName() const { return "FixedVariables"; }
    QString displayName() const { return QObject::tr("Fixed Variables"); }
    virtual void writeContents(QXmlStreamWriter* out);
    virtual void createItemContents(QTreeWidgetItem* item);
    
    double ballisticCoeff() const { return m_ballisticCoeff; }
    void setBallisticCoeff(double bc){m_ballisticCoeff = bc;}
    
    double entryAltitude() const { return m_entryAltitude; }
    void setEntryAltitude(double ea){m_entryAltitude = ea;}
    
    double inertialVelocity() const { return m_inertialVelocity; }
    void setInertialVelocity(double iv){m_inertialVelocity = iv;}

    double inertialFlightPathAngle() const { return m_inertialFlightPathAngle; }
    void setInertialFlightPathAngle(double ifpa){m_inertialFlightPathAngle = ifpa;}
    
    double inertialHeading() const { return m_inertialHeading; }
    void setInertialHeading(double ih){m_inertialHeading = ih;}

private:
	double m_ballisticCoeff;
	double m_entryAltitude;
	double m_inertialVelocity;
        double m_inertialFlightPathAngle;
	double m_inertialHeading;
	
};

class ScenarioWindow : public ScenarioObject
{
public:
	ScenarioWindow();
	~ScenarioWindow();
	
	QString elementName() const { return "Window"; }
	QString displayName() const { return QObject::tr("Window"); }
    virtual void writeContents(QXmlStreamWriter* out);
    virtual void createItemContents(QTreeWidgetItem* item);
    
    ScenarioAbstractReentryTrajectoryParam* xAxisVariable() const{return m_xAxisVariable;}
    ScenarioAbstractReentryTrajectoryParam* yAxisVariable() const{return m_yAxisVariable;}
    
    void setXAxisVariable(ScenarioAbstractReentryTrajectoryParam* xvar);
    void setYAxisVariable(ScenarioAbstractReentryTrajectoryParam* yvar);
    
	
private:
	ScenarioAbstractReentryTrajectoryParam* m_xAxisVariable;
	ScenarioAbstractReentryTrajectoryParam* m_yAxisVariable;
};


enum ReentryTrajectoryParamType
{
    CAPSULE_NOSE_RADIUS,
    CAPSULE_BALLISTIC_COEFFICIENT,
    INERTIAL_ENTRY_VELOCITY,
    INERTIAL_FLIGHT_PATH_ANGLE,
    INERTIAL_HEADING,
};

class ScenarioAbstractReentryTrajectoryParam : public ScenarioObject
{
public:
	ScenarioAbstractReentryTrajectoryParam(ReentryTrajectoryParamType type);
	~ScenarioAbstractReentryTrajectoryParam();
	
	QString elementName() const { return m_name; }
	QString displayName();
    virtual void writeContents(QXmlStreamWriter* out);
    virtual void createItemContents(QTreeWidgetItem* item);
    
    ReentryTrajectoryParamType type() const{return m_type;}
    int returnType() const;
    double minRange() const{ return m_min;}
    double maxRange() const{ return m_max;}
    
    void setType(ReentryTrajectoryParamType type){ m_type = type;}
    void setMin(double min){ m_min = min;}
    void setMax(double max){ m_max = max;}
	
private:
//        QHash<QString,QString>* nameHash;
//        QHash<QString,QString>* unitHash;
//        QHash<QString,ReentryTrajectoryParamType>* typeHash;
	QString m_name;
	ReentryTrajectoryParamType m_type;
	double m_min;
	double m_max;
};

#endif // _STA_REENTRY_MODE_H_

