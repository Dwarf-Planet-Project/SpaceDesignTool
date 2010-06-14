/*
 This program is free software; you can redistribute it and/or modify it under
 the terms of the European Union Public Licence - EUPL v.1.1 as published by
 the European Commission.

 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE. See the European Union Public Licence - EUPL v.1.1
 for more details.

 You should have received a copy of the European Union Public Licence - EUPL v.1.1
 along with this program.

 Further information about the European Union Public Licence - EUPL v.1.1 can
 also be found on the world wide web at http://ec.europa.eu/idabc/eupl

*/

/*
 ------ Copyright (C) 2010 STA Steering Board (space.trajectory.analysis AT gmail.com) ----
*/
/*
------------------ Author: Chris Laurel  -------------------------------------------------
 ------------------ E-mail: (claurel@gmail.com) ----------------------------
 */

#include "manoeuvreplanwriter.h"
#include "manoeuvreplan.h"

void ManoeuvrePlanWriter::writeManoeuvrePlan(ScenarioManoeuvrePlan* manoeuvreplan){
	m_manoeuvreplan = manoeuvreplan;
	if(m_manoeuvreplan){
		writeStartDocument();
		QString name = m_manoeuvreplan->name();
		writeStartElement("ManoeuvrePlan");
		writeAttribute("Name",name);
		m_manoeuvres = m_manoeuvreplan->manoeuvresList();
		if(m_manoeuvres.size()==0){
			writeEndDocument();
		}
		else{
			foreach(ScenarioManoeuvre* manoeuvre, m_manoeuvres){
			writeManoeuvre(manoeuvre);
			}
			writeEndDocument();
		}
	}
}

void ManoeuvrePlanWriter::writeManoeuvre(ScenarioManoeuvre* manoeuvre){
	if(manoeuvre){
		QString name = manoeuvre->name();
		bool active = manoeuvre->isActive();
		QList<ManoeuvreInput*> inputs = manoeuvre->manoeuvreInputs();
		writeStartElement(name);
		if(active) writeAttribute("status","active");
		else writeAttribute("status", "inactive");
		foreach(ManoeuvreInput* input, inputs){
			writeManoeuvreInput(input);
		}
		writeEndElement();
	}
}

void ManoeuvrePlanWriter::writeManoeuvreInput(ManoeuvreInput* input){
	if(input){
		QString name = input->name();
		double value = input->value();
		writeStartElement(name);
		writeCharacters(QString::number(value));
		writeEndElement();
	}
}

