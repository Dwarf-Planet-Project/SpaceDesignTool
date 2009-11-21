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
  Modified by Valentino Zuccarelli on 14th June 2009
 Extentive modified by Guillermo to hold TLEs on October 2009
 */

#include <cassert>
#include "scenariotree.h"
#include "scenarioelementbox.h"
#include "Scenario/spacescenario.h"
#include "Scenario/spacescenarioreader.h"
#include "Astro-Core/trajectorypropagation.h"
#include "initialstateeditor.h"
#include "initialstateThreebodyEditor.h"
#include "Locations/locationeditor.h"
#include "RendezVous/rendezvous.h"
#include "Loitering/loitering.h"
#include "Loitering/loiteringTLE.h"
#include "External/external.h"
#include "Lagrangian/lagrangian.h"
#include "Entry/reentry.h"
#include "physicalproperties.h"
#include "aerodynamicproperties.h"
#include "propulsionproperties.h"
#include "payloadproperties.h"
#include <QtGui>
#include <iostream>

int Lagrmode=-1;

/*! Create a new scenario view widget.
 */
ScenarioTree::ScenarioTree(QWidget *parent)
    : QTreeWidget(parent)
{
    QStringList labels;
    labels << tr("Type") << tr("Name");
    
    //header()->setResizeMode(QHeaderView::ResizeToContents);
    header()->setResizeMode(QHeaderView::Interactive);
    header()->setDefaultSectionSize(200);
    header()->setCascadingSectionResizes(true);

    setHeaderLabels(labels);
    
    m_folderIcon.addPixmap(style()->standardPixmap(QStyle::SP_DirClosedIcon),
                           QIcon::Normal, QIcon::Off);
    m_folderIcon.addPixmap(style()->standardPixmap(QStyle::SP_DirOpenIcon),
                           QIcon::Normal, QIcon::On);
    m_scenarioElementIcon.addPixmap(style()->standardPixmap(QStyle::SP_FileIcon));
    
    connect(this, SIGNAL(itemDoubleClicked(QTreeWidgetItem*, int)),
            this, SLOT(editItem(QTreeWidgetItem*, int)));
    connect(this, SIGNAL(itemChanged(QTreeWidgetItem*, int)),
            this, SLOT(editItemInline(QTreeWidgetItem*, int)));
    setExpandsOnDoubleClick(false);
    
    // Set drag and drop behavior
    setDragEnabled(true);
    setAcceptDrops(true);
    setDragDropMode(QAbstractItemView::DragDrop);
    setDropIndicatorShown(true);

    setAlternatingRowColors(true);


}


QStringList ScenarioTree::mimeTypes() const
{
    QStringList types;
    types << ScenarioElementBox::PARTICIPANT_MIME_TYPE
          << ScenarioElementBox::MISSION_ARC_MIME_TYPE
          << ScenarioElementBox::MANEUVER_MIME_TYPE;
    return types;
}


bool ScenarioTree::dropMimeData(QTreeWidgetItem* parent,
                                int /* index */,
                                const QMimeData* data,
                                Qt::DropAction action)
{
    if (action == Qt::IgnoreAction)
        return true;
    
    if (data->formats().empty())
        return false;

    if (!parent)
        return false;
    
    QString dataFormat = data->formats().first();
    QByteArray encodedData = data->data(dataFormat);
    
    // Initialize the reader and scan past the xml header
    SpaceScenarioReader reader(encodedData);
    while (!reader.atEnd() && !reader.isStartElement())
    {
        reader.readNext();
    }
    
    if (dataFormat == ScenarioElementBox::PARTICIPANT_MIME_TYPE)
    {
        ScenarioParticipant* participant = reader.readParticipant();
        if (!participant)
        {
            QMessageBox::information(this,
                                     tr("Bad toolbox element"),
                                     tr("Line %1: %2").arg(reader.lineNumber()).arg(reader.errorString()));
            return false;
        }
        
        ScenarioObject* parentObject = objectForItem(parent);
            
        SpaceScenario* scenario = dynamic_cast<SpaceScenario*>(parentObject);
        if (scenario)
        {
            scenario->addParticipant(participant);
            participant->createItem(parent);        
        }
        
        return true;
    }
    else if (dataFormat == ScenarioElementBox::MISSION_ARC_MIME_TYPE)
    {
        ScenarioTrajectoryPlan* plan = NULL;
        if (parent != NULL)
        {
            ScenarioObject* parentObject = objectForItem(parent);
            plan = dynamic_cast<ScenarioTrajectoryPlan*>(parentObject);
        }
        
        if (!plan)
            return false;
        
        ScenarioAbstractTrajectory* trajectory = reader.readTrajectory();
        if (!trajectory)
        {
            QMessageBox::information(this,
                                     tr("Bad toolbox element"),
                                     tr("Line %1: %2").arg(reader.lineNumber()).arg(reader.errorString()));
            return false;
        }
        else
        {
            plan->addTrajectory(trajectory);
            trajectory->createItem(parent);      
            
            return true;        
        }
    }

    // Report drag and drop not handled (because item was unrecognized or dragged to the
    // wrong place in the tree view.)
    return false;
}


QMimeData* ScenarioTree::mimeData(const QList<QTreeWidgetItem*> items) const
{
    QMimeData *mimeData = new QMimeData();
    QByteArray encodedData;
    
    QDataStream stream(&encodedData, QIODevice::WriteOnly);
    
    foreach (QTreeWidgetItem* item, items)
    {
        if (item)
        {
            QString text = "blah blah blah";
            stream << text;
        }
    }
    
    mimeData->setData(ScenarioElementBox::PARTICIPANT_MIME_TYPE, encodedData);
    return mimeData;

//    return QTreeWidget::mimeData(items);
    
#if 0
    // Code to allow dragging scenario objects out of the view
    if (items.isEmpty())
        return NULL;
    
    ScenarioObject* scenarioObject = objectForItem(items.first());
    if (scenarioObject == NULL)
        return NULL;
    else
        return new ScenarioMimeData(scenarioObject);
#endif
}


/*! Update a scenario object to reflect changes made in the
 *  tree widget.
 */
void ScenarioTree::updateScenarioObject(QTreeWidgetItem* /* item */, int /* column */)
{
    QMessageBox::information(this, tr("item changed"), tr("item changed"));
}


void ScenarioTree::removeChildren(QTreeWidgetItem* parentItem)
{
    int childCount = parentItem->childCount();
    QDomElement element = m_domElementForItem[parentItem];
    Q_ASSERT(!element.isNull());
    
    for (int i = childCount - 1; i >= 0; i--)
    {
        QTreeWidgetItem* child = parentItem->takeChild(i);
        if (child->childCount() != 0)
            removeChildren(child);
        
        QDomElement childElement = m_domElementForItem[child];
        Q_ASSERT(!childElement.isNull());
        m_domElementForItem.remove(child);
        element.removeChild(childElement);
        
        delete child;
    }
}
 

void ScenarioTree::updateTreeItems(QTreeWidgetItem* parentItem,
                                   ScenarioObject* scenarioObject)
{
    int childCount = parentItem->childCount();
    for (int i = childCount - 1; i >= 0; i--)
    {
        QTreeWidgetItem* child = parentItem->takeChild(i);
        delete child;
    }
    
    scenarioObject->createItemContents(parentItem);
}


void ScenarioTree::editScenarioObject(ScenarioObject* scenarioObject,
                                      QTreeWidgetItem* editItem)
{
    if (scenarioObject == NULL || editItem == NULL)
        return;
    
    // TODO: Replace this with an edit method for scenario objects
    if (dynamic_cast<ScenarioTrajectoryPropagation*>(scenarioObject) != NULL)
    {
        ScenarioTrajectoryPropagation* prop = dynamic_cast<ScenarioTrajectoryPropagation*>(scenarioObject);
        TrajectoryPropagationDialog editDialog(this);
        if (!editDialog.loadValues(prop))
        {
            QMessageBox::information(this, tr("Bad TrajectoryPropagation element"), tr("Error in TrajectoryPropagation element"));
        }
        else
        {
            if (editDialog.exec() == QDialog::Accepted)
            {
                editDialog.saveValues(prop);
                updateTreeItems(editItem, scenarioObject);
            }
        }
    }
    
    else if (dynamic_cast<ScenarioInitialStatePosition*>(scenarioObject) != NULL)
    {
        ScenarioInitialStatePosition* initialStatePos = dynamic_cast<ScenarioInitialStatePosition*>(scenarioObject);
        if (Lagrmode==1 || Lagrmode==2 || Lagrmode==3 || Lagrmode==4 || Lagrmode==0)
        {

            InitialStateThreebodyEditorDialog editDialog(this);
            if (!editDialog.loadValues(initialStatePos))
            {
                QMessageBox::information(this, tr("Bad InitialStatePosition element"), tr("Error in InitialStatePosition element"));
            }
            else
            {
                if (editDialog.exec() == QDialog::Accepted)
                {
                    editDialog.saveValues(initialStatePos);
                    updateTreeItems(editItem, scenarioObject);
                }
            }
        }
        else
        {
        InitialStateEditorDialog editDialog(this);
        if (!editDialog.loadValues(initialStatePos))
        {
            QMessageBox::information(this, tr("Bad InitialStatePosition element"), tr("Error in InitialStatePosition element"));
        }
        else
        {
            if (editDialog.exec() == QDialog::Accepted)
            {
                editDialog.saveValues(initialStatePos);
                updateTreeItems(editItem, scenarioObject);
            }
        }
        }
    }
    else if (dynamic_cast<ScenarioLocation*>(scenarioObject) != NULL)
    {
        ScenarioLocation* location = dynamic_cast<ScenarioLocation*>(scenarioObject);
        LocationEditorDialog editDialog(this);
        editDialog.loadValues(location);
        if (editDialog.exec() == QDialog::Accepted)
        {
            editDialog.saveValues(location);
            updateTreeItems(editItem, scenarioObject);
        }
    }
    else if (dynamic_cast<ScenarioRendezvousTrajectory*>(scenarioObject) != NULL)
    {
        ScenarioRendezvousTrajectory* rendezvous = dynamic_cast<ScenarioRendezvousTrajectory*>(scenarioObject);
        RendezvousDialog editDialog(this);
        if (!editDialog.loadValues(rendezvous))
        {
            QMessageBox::information(this, tr("Bad Rendezvous element"), tr("Error in Rendezvous element"));
        }
        else
        {
            if (editDialog.exec() == QDialog::Accepted)
            {
                editDialog.saveValues(rendezvous);
                updateTreeItems(editItem, scenarioObject);
            }
        }
    }
    else if (dynamic_cast<ScenarioLagrangianTrajectory*>(scenarioObject) != NULL)
    {Lagrmode=0;
        ScenarioLagrangianTrajectory* lagrangian = dynamic_cast<ScenarioLagrangianTrajectory*>(scenarioObject);
        LagrangianDialog editDialog(this);
         if (!editDialog.loadValues(lagrangian))
        {
            QMessageBox::information(this, tr("Bad Lagrangian element"), tr("Error in Lagrangian element"));
        }
          else
        {
            if (editDialog.exec() == QDialog::Accepted)
            {
                editDialog.saveValues(lagrangian);
                updateTreeItems(editItem, scenarioObject);
            }
        }
    }

    else if (dynamic_cast<ScenarioLoiteringTrajectory*>(scenarioObject) != NULL)
    {
        Lagrmode=-1;
        ScenarioLoiteringTrajectory* loitering = dynamic_cast<ScenarioLoiteringTrajectory*>(scenarioObject);
        LoiteringDialog editDialog(this);
        if (!editDialog.loadValues(loitering))
        {
            QMessageBox::information(this, tr("Bad Loitering element"), tr("Error in Loitering element"));
        }
        else
        {
            if (editDialog.exec() == QDialog::Accepted)
            {
                editDialog.saveValues(loitering);
                updateTreeItems(editItem, scenarioObject);
            }
        }
    }

    else if (dynamic_cast<ScenarioTleTrajectory*>(scenarioObject) != NULL)
    {
	ScenarioTleTrajectory* loiteringTLE = dynamic_cast<ScenarioTleTrajectory*>(scenarioObject);
	LoiteringTLEDialog editDialog(this);

	if (!editDialog.loadValues(loiteringTLE))
	{
	    QMessageBox::information(this, tr("Bad Loitering TLE element"), tr("Error in Loitering TLE element"));
	}
	else
	{
	    if (editDialog.exec() == QDialog::Accepted)
	    {
		editDialog.saveValues(loiteringTLE);
		// TODO Change the name of the participant using the TLE line 0
		updateTreeItems(editItem, scenarioObject);
	    }
	}
    }


    else if (dynamic_cast<ScenarioExternalTrajectory*>(scenarioObject) != NULL)
    {
	ScenarioExternalTrajectory* externalTrajectory = dynamic_cast<ScenarioExternalTrajectory*>(scenarioObject);
	ExternalDialog editDialog(this);

	if (!editDialog.loadValues(externalTrajectory))
	{
	    QMessageBox::information(this, tr("Bad External element"), tr("Error in external trajectory"));
	}
	else
	{
	    if (editDialog.exec() == QDialog::Accepted)
	    {
		editDialog.saveValues(externalTrajectory);
		updateTreeItems(editItem, scenarioObject);
	    }
	}
    }

    else if (dynamic_cast<ScenarioPhysicalProperties*>(scenarioObject) != NULL)
    {
        ScenarioPhysicalProperties* physicalProperties = dynamic_cast<ScenarioPhysicalProperties*>(scenarioObject);
        PhysicalPropertiesDialog editDialog(this);
        if (!editDialog.loadValues(physicalProperties))
        {
            QMessageBox::information(this, tr("Bad Properties element"), tr("Error in Physical Properties element"));
        }
        else
        {
            if (editDialog.exec() == QDialog::Accepted)
            {
                editDialog.saveValues(physicalProperties);
                updateTreeItems(editItem, scenarioObject);
            }
        }
    }

    else if (dynamic_cast<ScenarioAerodynamicProperties*>(scenarioObject) != NULL)
    {
        ScenarioAerodynamicProperties* aerodynamicProperties = dynamic_cast<ScenarioAerodynamicProperties*>(scenarioObject);
        AerodynamicPropertiesDialog editDialog(this);
        if (!editDialog.loadValues(aerodynamicProperties))
        {
            QMessageBox::information(this, tr("Bad Properties element"), tr("Error in Aerodynamic Properties element"));
        }
        else
        {
            if (editDialog.exec() == QDialog::Accepted)
            {
                editDialog.saveValues(aerodynamicProperties);
                updateTreeItems(editItem, scenarioObject);
            }
        }
    }

    else if (dynamic_cast<ScenarioPropulsionProperties*>(scenarioObject) != NULL)
    {
        ScenarioPropulsionProperties* propulsionProperties = dynamic_cast<ScenarioPropulsionProperties*>(scenarioObject);
        PropulsionPropertiesDialog editDialog(this);
        if (!editDialog.loadValues(propulsionProperties))
        {
            QMessageBox::information(this, tr("Bad Properties element"), tr("Error in Propulsion Properties element"));
        }
        else
        {
            if (editDialog.exec() == QDialog::Accepted)
            {
                editDialog.saveValues(propulsionProperties);
                updateTreeItems(editItem, scenarioObject);
            }
        }
    }

    else if (dynamic_cast<ScenarioPayloadProperties*>(scenarioObject) != NULL)
    {
        ScenarioPayloadProperties* payloadProperties = dynamic_cast<ScenarioPayloadProperties*>(scenarioObject);
        PayloadPropertiesDialog editDialog(this);
        if (!editDialog.loadValues(payloadProperties))
        {
            QMessageBox::information(this, tr("Bad Properties element"), tr("Error in Payload Properties element"));
        }
        else
        {
            if (editDialog.exec() == QDialog::Accepted)
            {
                editDialog.saveValues(payloadProperties);
                updateTreeItems(editItem, scenarioObject);
            }
        }
    }

    else if (dynamic_cast<ScenarioReEntryTrajectory*>(scenarioObject) != NULL)
    {
        Lagrmode=0;
        ScenarioReEntryTrajectory* reentry = dynamic_cast<ScenarioReEntryTrajectory*>(scenarioObject);
        ReEntryDialog editDialog(this);

        if (!editDialog.loadValues(reentry))
        {
             QMessageBox::information(this, tr("Bad ReEntry element"), tr("Error in ReEntry element"));
        }
        else
        {
            if (editDialog.exec() == QDialog::Accepted)
            {
                editDialog.saveValues(reentry);
                updateTreeItems(editItem, scenarioObject);
            }
        }
    }
    else if (dynamic_cast<ScenarioAppearance*>(scenarioObject) != NULL)
    {
        ScenarioAppearance* appearance = dynamic_cast<ScenarioAppearance*>(scenarioObject);
        QString modelFileName = QFileDialog::getOpenFileName(this,
                                                             tr("Select Model File"),
                                                             QString("./models/") + appearance->model(),
                                                             tr("Models (*.3ds *.cmod)"));
        if (!modelFileName.isEmpty())
        {
            // Strip away the path--we just want the filename
            if (modelFileName.contains('/'))
                modelFileName.remove(0, modelFileName.lastIndexOf('/') + 1);
            
            appearance->setModel(modelFileName);
            updateTreeItems(editItem, scenarioObject);
        }
    }
    else
    {
        ScenarioObject* parentObject = NULL;
        editItem = editItem->parent();
        if (editItem != NULL)
        {
            QVariant data = editItem->data(0, ScenarioObjectRole);
            
            if (qVariantCanConvert<void*>(data))
            {
                void* pointer = qVariantValue<void*>(data);
                parentObject = reinterpret_cast<ScenarioObject*>(pointer);
            }
        
            editScenarioObject(parentObject, editItem);
        }
    }    
}


void ScenarioTree::editItem(QTreeWidgetItem* item, int /* column */)
{
    if (item == NULL)
        return;

	ScenarioObject* scenarioObject = NULL;

    QVariant data = item->data(0, ScenarioObjectRole);
    if (qVariantCanConvert<void*>(data))
    {
        void* pointer = qVariantValue<void*>(data);
        scenarioObject = reinterpret_cast<ScenarioObject*>(pointer);
    }
      
    // Some items in the tree view are just data fields; if the user has
    // double clicked on one, we'll invoke the editor on the item's
    // parent. This makes things a bit more convenient for the user.    
    if (scenarioObject == NULL)
    {
        item = item->parent();
        
        if (item != NULL)
        {
            QVariant data = item->data(0, ScenarioObjectRole);
            if (qVariantCanConvert<void*>(data))
            {
                void* pointer = qVariantValue<void*>(data);
                scenarioObject = reinterpret_cast<ScenarioObject*>(pointer);
            }
        }
    }
    
    if (scenarioObject != NULL && item != NULL)
    {
        editScenarioObject(scenarioObject, item);
    }
}


/* This slot is signalled when an inline edit is performed on an item
 * in the scenario tree.
 */
void ScenarioTree::editItemInline(QTreeWidgetItem* item, int column)
{
    ScenarioObject* object = objectForItem(item);
    if (object == NULL)
        return;
    
    // TODO: If we allow more objects to be editing in the scenario
    // tree view, we need to add an edit method.
    if (dynamic_cast<ScenarioParticipant*>(object) && column == 1)
    {
        // Change the name of a participant
        ScenarioParticipant* participant = dynamic_cast<ScenarioParticipant*>(object);
        participant->setName(item->text(1));
    }
    else if (dynamic_cast<SpaceScenario*>(object) && column == 1)
    {
        SpaceScenario* scenario = dynamic_cast<SpaceScenario*>(object);
        scenario->setName(item->text(1));
    }
}


/** Return the scenario object associated with the specified tree item; may
 *  return NULL, since some tree items refer to scenario object properties
 *  and not scenario objects themselves.
 */
ScenarioObject* ScenarioTree::objectForItem(QTreeWidgetItem* item) const
{
    ScenarioObject* scenarioObject = NULL;
    QVariant data = item->data(0, ScenarioObjectRole);
    
    if (qVariantCanConvert<void*>(data))
    {
        void* pointer = qVariantValue<void*>(data);
        scenarioObject = reinterpret_cast<ScenarioObject*>(pointer);
    }
    
    return scenarioObject;
}


/** Return the currently selected scenario object, or NULL if nothing is currently
  * selected.
  */
ScenarioObject* ScenarioTree::selectedObject() const
{
    QTreeWidgetItem* item = selectedItem();
    if (item != NULL)
    {
        return objectForItem(item);
    }
    else
    {
        return NULL;
    }
}


/** Return the currently selected tree item, or NULL if nothing is currently
  * selected.
  */
QTreeWidgetItem* ScenarioTree::selectedItem() const
{
    QModelIndexList selected = selectionModel()->selectedRows();
    if (!selected.isEmpty())
    {
        return static_cast<QTreeWidgetItem*>(selected.first().internalPointer());
    }

    return NULL;
}


void ScenarioTree::removeItem(QTreeWidgetItem* item)
{
    removeItemWidget(item, 0);
}


/** Remove the selected item. This has no effect unless the selected
  * item represents a removeable object, e.g. a participant or
  * trajectory segment.
  */
void ScenarioTree::removeSelection()
{
    QModelIndexList selected = selectionModel()->selectedRows();
    if (!selected.isEmpty())
    {
        QModelIndex index = selected.first();
        QTreeWidgetItem* item = static_cast<QTreeWidgetItem*>(selected.first().internalPointer());
        QTreeWidgetItem* parentItem = static_cast<QTreeWidgetItem*>(index.parent().internalPointer());
        if (!item || !parentItem)
            return;

        ScenarioObject* parentObject = objectForItem(parentItem);
        if (parentObject->removeChild(index.row()))
        {
            model()->removeRow(index.row(), index.parent());
        }
    }
}
