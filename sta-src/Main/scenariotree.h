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


#ifndef SCENARIOTREE_H
#define SCENARIOTREE_H

#include <QWidget>
#include <QDomDocument>
#include <QHash>
#include <QIcon>
#include <QTreeWidget>
#include <QMimeData>

class ScenarioObject;

enum { ScenarioObjectRole = Qt::UserRole + 100 };


// ScenarioTree is a TreeWidget for viewing a single space scenario file.
class ScenarioTree : public QTreeWidget
{
Q_OBJECT
        
    public:
        ScenarioTree(QWidget *parent = 0);
        
        ScenarioObject* objectForItem(QTreeWidgetItem* item) const;
        ScenarioObject* selectedObject() const;
        QTreeWidgetItem* selectedItem() const;
        void removeItem(QTreeWidgetItem* item);

        void addScenarioItems(QTreeWidgetItem* item, ScenarioObject* scenarioObject);

    public slots:
        void removeSelection();
        
    protected:
        virtual QStringList mimeTypes() const;
        virtual bool dropMimeData(QTreeWidgetItem* parent, int index, const QMimeData* data,
                                  Qt::DropAction action);
        virtual QMimeData* mimeData(const QList<QTreeWidgetItem*> items) const;
        
    private slots:
        void editItem(QTreeWidgetItem* item, int column);
        void editItemInline(QTreeWidgetItem* item, int column);
        void updateScenarioObject(QTreeWidgetItem* item, int column);
        
    private:
        void removeChildren(QTreeWidgetItem* parentItem);
        void updateTreeItems(QTreeWidgetItem* parentItem, ScenarioObject* scenarioObject);
        void editScenarioObject(ScenarioObject* scenarioObject,
                                QTreeWidgetItem* item);

        QDomDocument m_domDocument;
        QHash<QTreeWidgetItem *, QDomElement> m_domElementForItem;
        QIcon m_folderIcon;
        QIcon m_scenarioElementIcon;
};


class ScenarioMimeData : public QMimeData
{
    Q_OBJECT
    
public:
    ScenarioMimeData(ScenarioObject* scenarioObject) : m_scenarioObject(scenarioObject) {}
    ~ScenarioMimeData() {}
    
    ScenarioObject* scenarioObject() const { return m_scenarioObject; }
    
private:
    ScenarioObject* m_scenarioObject;
};

#endif //SCENARIOTREE_H


