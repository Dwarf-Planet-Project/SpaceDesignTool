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
------------------ Author: Annalisa Riccardi   -------------------------------------------------
 ------------------ E-mail: (nina1983@gmail.com) ----------------------------
 */

#ifndef _STA_MANOEUVREPLAN_READER_H_
#define _STA_MANOEUVREPLAN_READER_H_

#include <QXmlStreamReader>

class ScenarioManoeuvrePlan;

class ManoeuvrePlanReader : public QXmlStreamReader
{
public:
    ManoeuvrePlanReader(QIODevice* device) :
        QXmlStreamReader(device){
    	
    	m_manoeuvreplan = NULL;
    }
    
    ScenarioManoeuvrePlan* readManoeuvrePlan();
    void readManoeuvresList();
    
    // Manoeuvres
    ScenarioManoeuvre* readManoeuvre(StaManoeuvreType type, QString name);
    ManoeuvreInput* readManoeuvreInput();
    
    void nextElement();
    bool readDouble(double* value);
    bool readDoubleElement(const QString& elementName, double* value);
    bool expectedElement(const QString& elementName);

private:
	ScenarioManoeuvrePlan* m_manoeuvreplan;
};

#endif // _STA_MANOEUVREPLAN_READER_H_

