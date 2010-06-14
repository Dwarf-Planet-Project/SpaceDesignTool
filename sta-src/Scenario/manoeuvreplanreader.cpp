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
------------------ Author: Annalisa Riccardi  -------------------------------------------------
 ------------------ E-mail: (nina1983@gmail.com) ----------------------------
 */

#include <QStringRef>
#include <iostream>
#include "manoeuvreplan.h"
#include "manoeuvreplanreader.h"

ScenarioManoeuvrePlan* ManoeuvrePlanReader::readManoeuvrePlan()
{    
    while (!atEnd())
    {
        readNext();
        
        if (isStartElement())
        {
            if (name() == "ManoeuvrePlan")
            {
                QString ManoeuvrePlanName = attributes().value("Name").toString();
                readManoeuvresList();
                if (m_manoeuvreplan){
                	m_manoeuvreplan->setName(ManoeuvrePlanName);
                }
            }
            else
            {
                raiseError(QObject::tr("Not a Manoeuvre Plan version 1.0 file."));
            }
        }
    }
    
    return m_manoeuvreplan;
}


void
ManoeuvrePlanReader::readManoeuvresList()
{
	
	m_manoeuvreplan = new ScenarioManoeuvrePlan();

	nextElement();
	
	if (isEndElement()){
		m_manoeuvreplan=NULL;
		raiseError(QObject::tr("Empty ManoeuvrePlan"));
	} 
	while (!atEnd())
	{	    
		if (isEndElement()){
	        break;
	    } 
		else{
		if (isStartElement() && m_manoeuvreplan->typeMap().contains(name().toString()))
	    {
	    	QString status = attributes().value("status").toString();
	    	QString manoeuvreName = name().toString();;
	    	StaManoeuvreType type = m_manoeuvreplan->typeMap().value(manoeuvreName);
	    	ScenarioManoeuvre* manoeuvre = readManoeuvre(type, manoeuvreName);
	    	if (manoeuvre){
	    	     if(manoeuvre->setStatus(status)){
	    	          		manoeuvre->setManoeuvrePlan(m_manoeuvreplan);
	    	           		m_manoeuvreplan->addManoeuvre(manoeuvre);	
	    	     }
	    	     else{	
	    	          raiseError(QObject::tr("Invalid status for Manoeuvre %1").arg(manoeuvreName));
	    	     }
	    	}
	    }
	    else
	    {
	    	raiseError(QObject::tr("%1 is not an accettable manoeuvre.").arg(name().toString()));} 
		}
		nextElement();
	}
	
	if (hasError())
	{
	    delete m_manoeuvreplan;
	    m_manoeuvreplan = NULL;
	}
}


ScenarioManoeuvre*
ManoeuvrePlanReader::readManoeuvre(StaManoeuvreType type, QString name)
{	
    ScenarioManoeuvre* manoeuvre = new ScenarioManoeuvre();
    manoeuvre->setName(name);
    manoeuvre->setType(type);
    ManoeuvreInput* manoeuvreInput = NULL; 
    
    nextElement();
    
	switch(type){
	
	case STA_MANOEUVRE_DURATION:
		if (expectedElement("Duration"))
			    {
			        manoeuvreInput = readManoeuvreInput();
			        manoeuvre->addManoeuvreInput(manoeuvreInput);
			        nextElement();
			    }
		else {
			raiseError(QObject::tr("Invalid input for manoeuvre %1.").arg(manoeuvre->name()));
		}
		break;
	case STA_MANOEUVRE_DELTAV:
		if (expectedElement("DeltaVx"))
		{
		    manoeuvreInput = readManoeuvreInput();
		    manoeuvre->addManoeuvreInput(manoeuvreInput);
		    nextElement();
		    if (expectedElement("DeltaVy"))
		    {
		        manoeuvreInput = readManoeuvreInput();
		        manoeuvre->addManoeuvreInput(manoeuvreInput);
		        nextElement();
		        if (expectedElement("DeltaVz"))
		        {
		        	manoeuvreInput = readManoeuvreInput();
		        	manoeuvre->addManoeuvreInput(manoeuvreInput);
		        	nextElement();
		        	if (expectedElement("Duration"))
		        	{
		        	    manoeuvreInput = readManoeuvreInput();
		        	    manoeuvre->addManoeuvreInput(manoeuvreInput);
		        	    nextElement();
		        	}
		        }
		    }
		}
		else {
			raiseError(QObject::tr("Invalid input for manoeuvre %1.").arg(manoeuvre->name()));
		}
		break;
		
	case STA_MANOEUVRE_V_POSITION:
		if (expectedElement("DisplacementV-bar"))
		{
		     manoeuvreInput = readManoeuvreInput();
		     manoeuvre->addManoeuvreInput(manoeuvreInput);
		     nextElement();
		}
		else {
			raiseError(QObject::tr("Invalid input for manoeuvre %1.").arg(manoeuvre->name()));
		}
		break;
	case STA_MANOEUVRE_R_POSITION:
		if (expectedElement("DisplacementR-bar"))
		{
			manoeuvreInput = readManoeuvreInput();
			manoeuvre->addManoeuvreInput(manoeuvreInput);
			nextElement();
		}
		else raiseError(QObject::tr("Invalid input for manoeuvre %1.").arg(manoeuvre->name()));
		break;
	case STA_MANOEUVRE_V_R_POSITION:
		if (expectedElement("DisplacementV-bar"))
		{
		    manoeuvreInput = readManoeuvreInput();
		    manoeuvre->addManoeuvreInput(manoeuvreInput);
		    nextElement();
		    if (expectedElement("DisplacementR-bar"))
		    {
		       manoeuvreInput = readManoeuvreInput();
		       manoeuvre->addManoeuvreInput(manoeuvreInput);
		       nextElement();
		    }
		}
		else {
			raiseError(QObject::tr("Invalid input for manoeuvre %1.").arg(manoeuvre->name()));
		}
		break;	

	case STA_MANOEUVRE_V_R_DURATION:
		if (expectedElement("DisplacementV-bar"))
		{
		    manoeuvreInput = readManoeuvreInput();
		    manoeuvre->addManoeuvreInput(manoeuvreInput);
		    nextElement();
		    if (expectedElement("DisplacementR-bar"))
		    {
		       manoeuvreInput = readManoeuvreInput();
		       manoeuvre->addManoeuvreInput(manoeuvreInput);
		       nextElement();
		       if (expectedElement("Duration"))
		       {
		           manoeuvreInput = readManoeuvreInput();
		           manoeuvre->addManoeuvreInput(manoeuvreInput);
		           nextElement();
		       }
		    }
		}
		else{
			raiseError(QObject::tr("Invalid input for manoeuvre %1.").arg(manoeuvre->name()));
		}
		break;			  
	}
    
    if (hasError())
    {
        delete manoeuvre;
        manoeuvre = NULL;
    }
    return manoeuvre;
}


ManoeuvreInput* ManoeuvrePlanReader::readManoeuvreInput()
{
	double value;
	QString inputName = name().toString();
	
	if(!readDoubleElement(inputName, &value))
		return NULL;
	
	ManoeuvreInput* manoeuvreInput = new ManoeuvreInput();
	manoeuvreInput->setName(inputName);
	manoeuvreInput->setValue(value);
	
	return manoeuvreInput;
}


bool
ManoeuvrePlanReader::readDouble(double* value)
{
    QString text = readElementText();
    bool convertOk = false;
    double v = text.toDouble(&convertOk);
    if (convertOk){
        *value = v;
    }
    else{
    	raiseError(QObject::tr("Numeric value expected."));
    }
    return convertOk;
}


bool
ManoeuvrePlanReader::readDoubleElement(const QString& elementName,
                                       double* value)
{
    if (!hasError())
    {
        if (expectedElement(elementName))
        {
            if (readDouble(value)){
                return true;
            }
        }
    }
    return false;
}

/*! Check that the current node type is a start element matching the
 *  specified kind. If it isn't raise an error and return false.
 */
bool
ManoeuvrePlanReader::expectedElement(const QString& elementName)
{
    if (!isStartElement())
    {
        // We read an end element; proably means that the expected element
        // was missing from a complex type.
        raiseError(QObject::tr("Missing %1 element in %2").arg(elementName).arg(name().toString()));
        return false;
    }
    else if (name() != elementName)
    {
        raiseError(QObject::tr("Expected %1 element but read %2.").arg(elementName).arg(name().toString()));
        return false;
    }
    else
    {
        return true;
    }
}



void
ManoeuvrePlanReader::nextElement()
{
    while (!atEnd())
    {
        readNext();
        if (isStartElement() || isEndElement())
            break;
    }
}
