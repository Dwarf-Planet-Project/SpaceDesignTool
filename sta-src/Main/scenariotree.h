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
 ------------------ Author: Chris Laurel  -------------------------------------------------
 ------------------ E-mail: (claurel@gmail.com) ----------------------------
 */

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




