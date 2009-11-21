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

#ifndef _STA_SCENARIO_MANOEUVRE_H_
#define _STA_SCENARIO_MANOEUVRE_H_

#include <QHash>
#include "scenarioobject.h"
#include "scenariomanoeuvreplan.h"

enum StaManoeuvreType
{
	STA_MANOEUVRE_DURATION							= 1000,
	STA_MANOEUVRE_DELTAV							= 1001,
	STA_MANOEUVRE_V_POSITION						= 1002,
	STA_MANOEUVRE_R_POSITION						= 1003,
	STA_MANOEUVRE_V_R_POSITION						= 1004,
	STA_MANOEUVRE_V_R_DURATION						= 1005
};

class ManoeuvreInput;

class ScenarioManoeuvre : public ScenarioObject
{
public:
    ScenarioManoeuvre();
    ~ScenarioManoeuvre();
    
    QString elementName() const { return m_name; }
    QString displayName() const { return hashName.value(m_name); }
    virtual void writeContents(QXmlStreamWriter* out);
    virtual void createItemContents(QTreeWidgetItem* item);
    
    QList<ManoeuvreInput*>& manoeuvreInputs() { return m_manoeuvreInputs; }
    void setManoeuvreInputs();
    
    void setName(QString name);
    void setActive(bool status){m_active = status;}
    bool isActive() const {return m_active;}

    QHash<QString, QString> nameMap() const{ return hashName;}
    //QTreeWidgetItem* item() const{ return m_item;}
    //void setItem(QTreeWidgetItem* item) {m_item = item;}
    
private:
	QString m_name;
	bool m_active;
	QList<ManoeuvreInput*> m_manoeuvreInputs; 
	QHash<QString, StaManoeuvreType > hash;
	QHash<QString, QString> hashName;
	//QTreeWidgetItem* m_item;
};

class ManoeuvreInput 
{
public:
    ManoeuvreInput();
    ~ManoeuvreInput(){}
    
    double value() const {return m_value;}
    QString name() const {return m_name;}
    QHash<QString, QString> getNameMap() const {return nameMap;}
    void setName(QString name);
    void setValue(double value){m_value=value;}
    
private:
	QString m_name;
	double m_value;    
	QHash<QString, QString> nameMap;
};

#endif // _STA_SCENARIO_MANOEUVRE_H_
