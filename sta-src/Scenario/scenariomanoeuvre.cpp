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

#include <QString>
#include <iostream>
#include <QTreeWidgetItem>
#include "scenariomanoeuvre.h"

ScenarioManoeuvre::ScenarioManoeuvre()

{
	
	hash.insert("FreeDrift", STA_MANOEUVRE_DURATION);
	hash.insert("StationKeeping", STA_MANOEUVRE_DURATION);
	hash.insert("DeltaV",STA_MANOEUVRE_DELTAV);
	hash.insert("HomannTransfer",STA_MANOEUVRE_R_POSITION);
	hash.insert("Imp2PointTransfer", STA_MANOEUVRE_V_R_DURATION);
	hash.insert("ImpHoppVbarRadial",STA_MANOEUVRE_V_POSITION );
	hash.insert("ImpHoppVbarTg", STA_MANOEUVRE_V_POSITION);
	hash.insert("ImpHoppNoVbar", STA_MANOEUVRE_V_POSITION);
	hash.insert("XThrustTransfer", STA_MANOEUVRE_R_POSITION);
	hash.insert("ForcedVbar", STA_MANOEUVRE_V_POSITION);
	hash.insert("ForcedRbar", STA_MANOEUVRE_R_POSITION);
	hash.insert("ContHoppVbar", STA_MANOEUVRE_V_POSITION);
	
	hashName.insert("FreeDrift", "Free Drift");
	hashName.insert("StationKeeping", "Station Keeping");
	hashName.insert("DeltaV","Delta V");
	hashName.insert("HomannTransfer","Homann Transfer");
	hashName.insert("Imp2PointTransfer", "Impulsive Two-point Transfer");
	hashName.insert("ImpHoppVbarRadial","Impulsive Hopping on V-bar in z-direction by radial impulse");
	hashName.insert("ImpHoppVbarTg", "Impulsive Hopping on V-bar in z-direction by tangential impulse");
	hashName.insert("ImpHoppNoVbar", "Impulsive Hopping not on V-Bar");
	hashName.insert("XThrustTransfer", "Continuous tangential transfer (X-thrust transfer)");
	hashName.insert("ForcedVbar", "Forced motion along V-bar");
	hashName.insert("ForcedRbar", "Forced motion along R-bar");
	hashName.insert("ContHoppVbar", "Continuous Hopping on V-Bar");
}

ScenarioManoeuvre::~ScenarioManoeuvre(){
}

void ScenarioManoeuvre::writeContents(QXmlStreamWriter* out){
	if(!m_manoeuvreInputs.isEmpty()){
		foreach(ManoeuvreInput* input, m_manoeuvreInputs)
			writeNumericElement(out, input->name(), input->value());
	}
}

void ScenarioManoeuvre::createItemContents(QTreeWidgetItem* item){
	if(!m_manoeuvreInputs.isEmpty()){
		foreach(ManoeuvreInput* input, m_manoeuvreInputs){
			QTreeWidgetItem* inputItem = new QTreeWidgetItem(item);
			inputItem->setText(0,input->getNameMap().value(input->name()));
			inputItem->setText(1,QString::number(input->value()));
		}
	}
}

void ScenarioManoeuvre::setManoeuvreInputs(){
	ManoeuvreInput* input1 = new ManoeuvreInput();
	input1->setName("Duration");
	input1->setValue(0.0);
	ManoeuvreInput* input2 = new ManoeuvreInput();
	input2->setName("DeltaVx");
	input2->setValue(0.0);
	ManoeuvreInput* input3 = new ManoeuvreInput();
	input3->setName("DeltaVy");
	input3->setValue(0.0);
	ManoeuvreInput* input4 = new ManoeuvreInput();
	input4->setName("DeltaVz");
	input4->setValue(0.0);
	ManoeuvreInput* input5 = new ManoeuvreInput();
	input5->setName("DisplacementV-bar");
	input5->setValue(0.0);
	ManoeuvreInput* input6 = new ManoeuvreInput();
	input6->setName("DisplacementR-bar");
	input6->setValue(0.0);	
	
	switch(hash.value(m_name)){
	case STA_MANOEUVRE_DURATION:
		m_manoeuvreInputs.append(input1);
		break;
	case STA_MANOEUVRE_DELTAV:
		m_manoeuvreInputs.append(input2);
		m_manoeuvreInputs.append(input3);
		m_manoeuvreInputs.append(input4);
		m_manoeuvreInputs.append(input1);
		break;
	case STA_MANOEUVRE_V_POSITION:
		m_manoeuvreInputs.append(input5);
		break;
	case STA_MANOEUVRE_R_POSITION:
		m_manoeuvreInputs.append(input6);
		break;
	case STA_MANOEUVRE_V_R_POSITION:
		m_manoeuvreInputs.append(input5);
		m_manoeuvreInputs.append(input6);
		break;
	case STA_MANOEUVRE_V_R_DURATION:
		m_manoeuvreInputs.append(input5);
		m_manoeuvreInputs.append(input6);
		m_manoeuvreInputs.append(input1);
		break;
	}
}

void ScenarioManoeuvre::setName(QString name){
	if(hashName.contains(name))
		m_name = name;
}



ManoeuvreInput::ManoeuvreInput():
	m_name(NULL),
	m_value(0.0)
{
    	nameMap.insert("Duration", "Duration [s]");
    	nameMap.insert("DeltaVx", "Delta Vx [km/s]");
    	nameMap.insert("DeltaVy", "Delta Vy [km/s]");
    	nameMap.insert("DeltaVz", "Delta Vz [km/s]");
    	nameMap.insert("DisplacementV-bar", "Displacement in V-bar direction [Km]");
    	nameMap.insert("DisplacementR-bar", "Displacement in R-bar direction [Km]");
}


void ManoeuvreInput::setName(QString name){
	if(nameMap.contains(name))
		m_name = name;
}



