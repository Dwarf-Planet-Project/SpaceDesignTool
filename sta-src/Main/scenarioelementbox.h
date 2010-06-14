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


//#ifndef SCENARIOELEMENTBOX_H
//#define SCENARIOELEMENTBOX_H
#include <QWidget>
#include <QTreeWidget>
#include "ui_scenarioelementbox.h"



class ScenarioElementWidget : public QTreeWidget
{
    Q_OBJECT
    
public:
    ScenarioElementWidget(QWidget* parent = NULL);
    
protected:
    virtual QStringList mimeTypes() const;
    virtual QMimeData* mimeData(const QList<QTreeWidgetItem*> items) const;    
};


class ScenarioElementBox : public QWidget, private Ui_ScenarioElementBox
{
    Q_OBJECT
    
public:
    ScenarioElementBox(QWidget* parent = NULL);
    ~ScenarioElementBox();
    
    static const QString PARTICIPANT_MIME_TYPE;
    static const QString MISSION_ARC_MIME_TYPE;
    static const QString PAYLOAD_MIME_TYPE; //Line added by Ricardo to create a new MIME type for different payloads
    static const QString MANEUVER_MIME_TYPE;
    
private:
    ScenarioElementWidget* m_elementTreeWidget;
};


//#endif //SCENARIOELEMENTBOX_H
