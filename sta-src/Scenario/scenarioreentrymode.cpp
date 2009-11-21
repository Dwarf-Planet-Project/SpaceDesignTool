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

#include <QTreeWidgetItem>
#include "scenarioreentrymode.h"
#include "scenariosimulationparameters.h"


ScenarioSimulationMode::ScenarioSimulationMode():
m_isActive(false),
m_simulationParameters(NULL),
m_dispersionAnalysis(NULL)
{
    m_simulationParameters = new ScenarioTargettingSimulationParameters();
    attachChild(m_simulationParameters);

    m_dispersionAnalysis = new ScenarioDispersionAnalysis();
    m_dispersionAnalysis->setIsActiveFalse();
    attachChild(m_dispersionAnalysis);
}

ScenarioSimulationMode::~ScenarioSimulationMode(){
}
	
void ScenarioSimulationMode::writeContents(QXmlStreamWriter* out){
	if(m_simulationParameters)
		m_simulationParameters->writeElement(out);
        if(m_dispersionAnalysis->isActive())
		m_dispersionAnalysis->writeElement(out);
}

void ScenarioSimulationMode::createItemContents(QTreeWidgetItem* item){
	if(m_simulationParameters)
		m_simulationParameters->createItem(item);
        if(m_dispersionAnalysis->isActive())
		m_dispersionAnalysis->createItem(item);
}
    
void ScenarioSimulationMode::setSimulationParameters(ScenarioTargettingSimulationParameters* simparam){
    if (simparam != m_simulationParameters)
    {
        detachChild(m_simulationParameters);
        m_simulationParameters = simparam;
        attachChild(m_simulationParameters);
    }
}
    
void ScenarioSimulationMode::setDispersionAnalysis(ScenarioDispersionAnalysis* dispersion){
    if (dispersion != m_dispersionAnalysis)
    {
        detachChild(m_dispersionAnalysis);
        m_dispersionAnalysis = dispersion;
        attachChild(m_dispersionAnalysis);
    }
}

/*** WINDOW MODE***/

ScenarioWindowMode::ScenarioWindowMode():
        m_isActive(false),
	m_simulationParameters(NULL),
	m_numSimulations(0),
	m_fixedVariables(NULL),
	m_window(NULL)
{
	m_simulationParameters = new ScenarioTargettingSimulationParameters();
	m_simulationParameters->setInitialStatePosition(NULL);
	attachChild(m_simulationParameters);
	
	m_fixedVariables = new ScenarioFixedVariables();
	attachChild(m_fixedVariables);
	
	m_window = new ScenarioWindow();
	attachChild(m_window);
}

ScenarioWindowMode::~ScenarioWindowMode(){
}
	
void ScenarioWindowMode::writeContents(QXmlStreamWriter* out){
	if(m_simulationParameters){
            if(m_simulationParameters->initialStatePosition())
                m_simulationParameters->setInitialStatePosition(NULL);
            m_simulationParameters->writeElement(out);
	}
        writeNumericElement(out, "NumberOfSimulations", m_numSimulations);
	if(m_fixedVariables)
		m_fixedVariables->writeElement(out);
	if(m_window)
		m_window->writeElement(out);
}

void ScenarioWindowMode::createItemContents(QTreeWidgetItem* item){
	
    if(m_simulationParameters){
		if(m_simulationParameters->initialStatePosition())
			m_simulationParameters->setInitialStatePosition(NULL);
    	m_simulationParameters->createItem(item);
    }
	
	QTreeWidgetItem* numItem = new QTreeWidgetItem(item);
    numItem->setText(0, "Number of simulations");
    numItem->setText(1, QString::number(m_numSimulations));
    
    if(m_fixedVariables)
    	m_fixedVariables->createItem(item);
    if(m_window)
    	m_window->createItem(item);
}

void ScenarioWindowMode::setSimulationParameters(ScenarioTargettingSimulationParameters* simparam){
    if (simparam != m_simulationParameters)
    {
        detachChild(m_simulationParameters);
        m_simulationParameters = simparam;
		if(m_simulationParameters->initialStatePosition())
			m_simulationParameters->setInitialStatePosition(NULL);
        attachChild(m_simulationParameters);
    }
}

void ScenarioWindowMode::setFixedVariables(ScenarioFixedVariables* variables){
    if (variables != m_fixedVariables)
    {
        detachChild(m_fixedVariables);
        m_fixedVariables = variables;
        attachChild(m_fixedVariables);
    }
}

void ScenarioWindowMode::setWindow(ScenarioWindow* window){
    if (window != m_window)
    {
        detachChild(m_window);
        m_window = window;
        attachChild(m_window);
    }
}

/***Dispersion Analysis***/
ScenarioDispersionAnalysis::ScenarioDispersionAnalysis():
        m_isActive(false),
	m_deviations(NULL),
	m_numSimulations(0)
{
	m_deviations = new ScenarioDeviations();
	attachChild(m_deviations);
}

ScenarioDispersionAnalysis::~ScenarioDispersionAnalysis(){
}

void ScenarioDispersionAnalysis::writeContents(QXmlStreamWriter* out){
        writeNumericElement(out, "NumberOfSimulations", m_numSimulations);
	if(m_deviations)
		m_deviations->writeElement(out);
}

void ScenarioDispersionAnalysis::createItemContents(QTreeWidgetItem* item){
	QTreeWidgetItem* numItem = new QTreeWidgetItem(item);
    numItem->setText(0, "Number of simulations");
    numItem->setText(1, QString::number(m_numSimulations));
    
    if(m_deviations)
    	m_deviations->createItem(item);
}

void ScenarioDispersionAnalysis::setDeviations(ScenarioDeviations* deviations){
    if (deviations != m_deviations)
    {
        detachChild(m_deviations);
        m_deviations = deviations;
        attachChild(m_deviations);
    }
}

/***Deviations***/
ScenarioDeviations::ScenarioDeviations():
m_capsuleBaseRadius(0.0),
m_capsuleMass(0.0),
m_capsuleCD(0.0),
m_parachuteCD(0.0),
m_atmosphereDensity(0.0),
m_entryLong(0.0),
m_entryLat(0.0),
m_velocity(0.0),
m_flightPathAngle(0.0),
m_inertialHeading(0.0)
{
}

ScenarioDeviations::~ScenarioDeviations(){
}

void ScenarioDeviations::writeContents(QXmlStreamWriter* out){
	writeNumericElement(out, "CapsuleBaseRadius", m_capsuleBaseRadius, "m");
	writeNumericElement(out, "CapsuleMass", m_capsuleMass, "Kg");
	writeNumericElement(out, "CapsuleCD", m_capsuleCD);
	writeNumericElement(out, "ParachuteCD", m_parachuteCD);
        writeNumericElement(out, "AtmosphereDensity", m_atmosphereDensity);
	writeNumericElement(out, "EntryLong", m_entryLong, "deg");
	writeNumericElement(out, "EntryLat", m_entryLat, "deg");
	writeNumericElement(out, "Velocity", m_velocity, "Km/s");
	writeNumericElement(out, "FlightPathAngle", m_flightPathAngle, "deg");
	writeNumericElement(out, "InertialHeading", m_inertialHeading, "deg");
}
void ScenarioDeviations::createItemContents(QTreeWidgetItem* item){
    QTreeWidgetItem* item1 = new QTreeWidgetItem(item);
    item1->setText(0, "Capsule Base Radius");
    item1->setText(1, QString::number(m_capsuleBaseRadius));
    
    QTreeWidgetItem* item2 = new QTreeWidgetItem(item);
    item2->setText(0, "Capsule Mass");
    item2->setText(1, QString::number(m_capsuleMass));
    
    QTreeWidgetItem* item3 = new QTreeWidgetItem(item);
    item3->setText(0, "Capsule CD");
    item3->setText(1, QString::number(m_capsuleCD));
    
    QTreeWidgetItem* item4 = new QTreeWidgetItem(item);
    item4->setText(0, "Parachute CD");
    item4->setText(1, QString::number(m_parachuteCD));
    
    QTreeWidgetItem* item5 = new QTreeWidgetItem(item);
    item5->setText(0, "Atmosphere Density");
    item5->setText(1, QString::number(m_atmosphereDensity));
    
    QTreeWidgetItem* item6 = new QTreeWidgetItem(item);
    item6->setText(0, "Entry Longitude");
    item6->setText(1, QString::number(m_entryLong));
    
    QTreeWidgetItem* item7 = new QTreeWidgetItem(item);
    item7->setText(0, "Entry Latitude");
    item7->setText(1, QString::number(m_entryLat));
    
    QTreeWidgetItem* item8 = new QTreeWidgetItem(item);
    item8->setText(0, "Velocity");
    item8->setText(1, QString::number(m_velocity));
    
    QTreeWidgetItem* item9 = new QTreeWidgetItem(item);
    item9->setText(0, "Flight Path Angle");
    item9->setText(1, QString::number(m_flightPathAngle));
    
    QTreeWidgetItem* item10 = new QTreeWidgetItem(item);
    item10->setText(0, "Inertial Heading");
    item10->setText(1, QString::number(m_inertialHeading));
}

/***Fixed Variables***/

ScenarioFixedVariables::ScenarioFixedVariables():
m_ballisticCoeff(0.0),
m_entryAltitude(0.0),
m_inertialVelocity(0.0),
m_inertialFlightPathAngle(0.0),
m_inertialHeading(0.0)
{
}

ScenarioFixedVariables::~ScenarioFixedVariables(){
	
}

void ScenarioFixedVariables::writeContents(QXmlStreamWriter* out){
	writeNumericElement(out, "BallisticCoeff", m_ballisticCoeff, "Kg/m^2");
	writeNumericElement(out, "EntryAltitude", m_entryAltitude, "Km");
	writeNumericElement(out, "InertialVelocity", m_inertialVelocity,"Km/s");
        writeNumericElement(out, "InertialFlightPathAngle", m_inertialFlightPathAngle,"Km/s");
	writeNumericElement(out, "InertialHeading", m_inertialHeading,"deg");
}
void ScenarioFixedVariables::createItemContents(QTreeWidgetItem* item){
    QTreeWidgetItem* item1 = new QTreeWidgetItem(item);
    item1->setText(0, "Ballistic Coefficient");
    item1->setText(1, QString::number(m_ballisticCoeff));
    
    QTreeWidgetItem* item2 = new QTreeWidgetItem(item);
    item2->setText(0, "Entry Altitude");
    item2->setText(1, QString::number(m_entryAltitude));
    
    QTreeWidgetItem* item3 = new QTreeWidgetItem(item);
    item3->setText(0, "Inertial Velocity");
    item3->setText(1, QString::number(m_inertialVelocity));

    QTreeWidgetItem* item4 = new QTreeWidgetItem(item);
    item4->setText(0, "Inertial Flight Path Angle");
    item4->setText(1, QString::number(m_inertialFlightPathAngle));
    
    QTreeWidgetItem* item5 = new QTreeWidgetItem(item);
    item5->setText(0, "Inertial Heading");
    item5->setText(1, QString::number(m_inertialHeading));
}

/***ScenarioWindow***/

ScenarioWindow::ScenarioWindow():
	m_xAxisVariable(NULL),
	m_yAxisVariable(NULL)
{	
}

ScenarioWindow::~ScenarioWindow(){
}

void ScenarioWindow::writeContents(QXmlStreamWriter* out){
	if(m_xAxisVariable)
		m_xAxisVariable->writeElement(out);
	if(m_yAxisVariable)
		m_yAxisVariable->writeElement(out);
}

void ScenarioWindow::createItemContents(QTreeWidgetItem* item){
	if(m_xAxisVariable)
		m_xAxisVariable->createItem(item);
	if(m_yAxisVariable)
		m_yAxisVariable->createItem(item);
}

void ScenarioWindow::setXAxisVariable(ScenarioAbstractReentryTrajectoryParam* xvar){
    if (xvar != m_xAxisVariable)
    {
        detachChild(m_xAxisVariable);
        m_xAxisVariable = xvar;
        attachChild(m_xAxisVariable);
    }
}

void ScenarioWindow::setYAxisVariable(ScenarioAbstractReentryTrajectoryParam* yvar){
    if (yvar != m_yAxisVariable)
    {
        detachChild(m_yAxisVariable);
        m_yAxisVariable = yvar;
        attachChild(m_yAxisVariable);
    }
}

/***ScenarioAbstractReentryTrajectoryParam***/



ScenarioAbstractReentryTrajectoryParam::ScenarioAbstractReentryTrajectoryParam(ReentryTrajectoryParamType type){

        QHash<QString,QString>* nameHash = new QHash<QString,QString>;
        nameHash->insert("CapsuleBallisticCoeff","Capsule Ballistic Coefficient");
        nameHash->insert("CapsuleNoseRadius","Capsule Nose Radius");
        nameHash->insert("InertialFlightPathAngle","Inertial Flight Path Angle");
        nameHash->insert("InertialHeading","Inertial Heading");
        nameHash->insert("InertialEntryVelocity","Inertial Entry Velocity");

        QHash<QString,QString>* unitHash = new QHash<QString,QString>;
        unitHash->insert("CapsuleBallisticCoeff","Kg/m^2");
        unitHash->insert("CapsuleNoseRadius","m");
        unitHash->insert("InertialFlightPathAngle","deg");
        unitHash->insert("InertialHeading","deg");
        unitHash->insert("InertialEntryVelocity","Km/s");

        QHash<QString,ReentryTrajectoryParamType>* typeHash = new QHash<QString,ReentryTrajectoryParamType>;
        typeHash->insert("CapsuleBallisticCoeff",CAPSULE_BALLISTIC_COEFFICIENT);
        typeHash->insert("CapsuleNoseRadius",CAPSULE_NOSE_RADIUS);
        typeHash->insert("InertialFlightPathAngle",INERTIAL_FLIGHT_PATH_ANGLE);
        typeHash->insert("InertialHeading",INERTIAL_HEADING);
        typeHash->insert("InertialEntryVelocity",INERTIAL_ENTRY_VELOCITY);

        m_type = type;

        m_name = typeHash->key(m_type);
        m_min = 0;
        m_max = 0;
}

ScenarioAbstractReentryTrajectoryParam::~ScenarioAbstractReentryTrajectoryParam(){
}

QString ScenarioAbstractReentryTrajectoryParam::displayName()
{
   // return nameHash->value(m_name);
   return "";
}

int ScenarioAbstractReentryTrajectoryParam::returnType() const {
        return m_type;
}

void ScenarioAbstractReentryTrajectoryParam::writeContents(QXmlStreamWriter* out){
        writeNumericElement(out, "min", m_min);
        writeNumericElement(out, "max", m_max);
}

void ScenarioAbstractReentryTrajectoryParam::createItemContents(QTreeWidgetItem* item){
	QTreeWidgetItem* min = new QTreeWidgetItem(item);
	QTreeWidgetItem* max = new QTreeWidgetItem(item);
	
    min->setText(0, "min");
    min->setText(1, QString::number(m_min));
    
    max->setText(0, "max");
    max->setText(1, QString::number(m_max));
}
