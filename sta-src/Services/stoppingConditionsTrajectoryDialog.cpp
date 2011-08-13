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
 ------------------ Author:       Guillermo Ortega               -------------------
 ------------------ Affiliation:  European Space Agency (ESA)    -------------------
 -----------------------------------------------------------------------------------
*/

#include "stoppingConditionsTrajectoryDialog.h"

#include <QDebug>

stoppingConditionsTrajectoryDialog::stoppingConditionsTrajectoryDialog( QWidget * parent, Qt::WindowFlags f) : QDialog(parent,f)
{
	setupUi(this);

    connect(this->AddStoppingConditionsTrajectoryPushButton, SIGNAL(clicked()), this, SLOT(addSelectedParameters()));
    connect(this->RemoveStoppingConditionsTrajectoryPushButton, SIGNAL(clicked()), this, SLOT(removeSelectedParameters()));
}

stoppingConditionsTrajectoryDialog::~stoppingConditionsTrajectoryDialog()
{
}



bool stoppingConditionsTrajectoryDialog::loadValues(const ScenarioTrajectoryStoppingConditionType* listOfStopConditions)
{
    if (listOfStopConditions == NULL)
    {
        return true;
    }
    else
    {
//        QList<QString> stopConditionsIDList = listOfStopConditions->ConditionIdentifier();
//        QString conditionID;
//        for (int j = 0; j < stopConditionsIDList.size(); j++)
//        {
//            conditionID = stopConditionsIDList.at(j);
//            QList<QListWidgetItem*> items = treeWidgetStoppingConditionsTrajectoryToChoose->findItems(conditionID, Qt::MatchFixedString);
//            if (nameBody != centralBody->Name())
//            {
//                PertBodyListWidget->addItem(nameBody);
//            }
//        }

        return true;
    }

}

bool stoppingConditionsTrajectoryDialog::saveValues(ScenarioTrajectoryStoppingConditionType* listOfStopConditions)
{

}

void stoppingConditionsTrajectoryDialog::on_treeWidgetStoppingConditionsTrajectoryToChoose_itemDoubleClicked(QTreeWidgetItem* myItem, int theNumber)
{
    addParameter(myItem);
}

void stoppingConditionsTrajectoryDialog::on_treeWidgetStoppingConditionsTrajectorySelected_itemDoubleClicked(QTreeWidgetItem* myItem, int theNumber)
{
    QList<QTreeWidgetItem *> ChosenParameters = treeWidgetStoppingConditionsTrajectorySelected->selectedItems();
    for (int i = 0; i < ChosenParameters.size(); i++)
    {
        delete ChosenParameters.at(i);
    }
}

void stoppingConditionsTrajectoryDialog::addSelectedParameters()
{
    /*
      Selects the items in the list of the parameters that can be sent to the list of the report options/parameters
      */
    QList<QTreeWidgetItem *> selectedParameters=treeWidgetStoppingConditionsTrajectoryToChoose->selectedItems();
    int numberOfchildren = selectedParameters[0]->childCount();
    if (numberOfchildren == 0)
    {
        QList<QTreeWidgetItem *> parameters = treeWidgetStoppingConditionsTrajectoryToChoose->selectedItems();

        for (int i = 0; i < parameters.size(); i++)
        {
            addParameter(parameters.at(i));
        }
    }
}

void stoppingConditionsTrajectoryDialog::addParameter(QTreeWidgetItem* item)
{
    /*
      allows the user to add the parameters he/she wants to see in the report
      Inputs: item- selected items that are to be sent to the report options list
      */
    QString text = item->text(0);
    if(treeWidgetStoppingConditionsTrajectorySelected->findItems(text,Qt::MatchExactly).isEmpty())

    {
        QTreeWidgetItem*added=new QTreeWidgetItem(treeWidgetStoppingConditionsTrajectorySelected);
        added->setText(0,text);
        outputTableOptions(added);
    }
}

void stoppingConditionsTrajectoryDialog::removeSelectedParameters()
{
    /*
      deletes the selected parameters from the report options list
      */
    QList<QTreeWidgetItem *> ChosenParameters = treeWidgetStoppingConditionsTrajectorySelected->selectedItems();
    for (int i = 0; i < ChosenParameters.size(); i++)
    {
        delete ChosenParameters.at(i);
    }
}


/*  Selects the comboboxes that should be available next to each parameter, according to its type
 *  item- line of the tree that contains the report options
 */
void stoppingConditionsTrajectoryDialog::outputTableOptions(QTreeWidgetItem* item)
{
    QString name=item->text(0);

    if (name=="Epoch")
    {
        serviceEpoch* myEpoch = new serviceEpoch();
        QDateTime TheCurrentDateAndTime = QDateTime::currentDateTime(); // Get the current epoch
        myEpoch->setDateTimeEdit(TheCurrentDateAndTime.addDays(2));
        treeWidgetStoppingConditionsTrajectorySelected->setItemWidget(item,1, myEpoch);
        serviceGenericParameter* myToleranceZero = new serviceGenericParameter();
        myToleranceZero->setGenericParameterLineEditWidget("0.0");
        myToleranceZero->genericParameterLineEditWidget->setReadOnly(true);
        treeWidgetStoppingConditionsTrajectorySelected->setItemWidget(item,2, myToleranceZero);
    }
    else if (name=="1-day")
    {
        serviceEpoch* myEpoch = new serviceEpoch();
        QDateTime TheCurrentDateAndTime = QDateTime::currentDateTime(); // Get the current epoch
        myEpoch->setDateTimeEdit(TheCurrentDateAndTime.addDays(1));
        treeWidgetStoppingConditionsTrajectorySelected->setItemWidget(item,1, myEpoch);
        serviceGenericParameter* myToleranceZero = new serviceGenericParameter();
        myToleranceZero->setGenericParameterLineEditWidget("0.0");
        myToleranceZero->genericParameterLineEditWidget->setReadOnly(true);
        treeWidgetStoppingConditionsTrajectorySelected->setItemWidget(item,2, myToleranceZero);
    }
    else if (name=="1-week")
    {
        serviceEpoch* myEpoch = new serviceEpoch();
        QDateTime TheCurrentDateAndTime = QDateTime::currentDateTime(); // Get the current epoch
        myEpoch->setDateTimeEdit(TheCurrentDateAndTime.addDays(7));
        treeWidgetStoppingConditionsTrajectorySelected->setItemWidget(item,1, myEpoch);
        serviceGenericParameter* myToleranceZero = new serviceGenericParameter();
        myToleranceZero->setGenericParameterLineEditWidget("0.0");
        myToleranceZero->genericParameterLineEditWidget->setReadOnly(true);
        treeWidgetStoppingConditionsTrajectorySelected->setItemWidget(item,2, myToleranceZero);
    }
}


void stoppingConditionsTrajectoryDialog::on_buttonBoxStoppingConditionsTrajectoryDialog_accepted()
{
    //qWarning("TODO: %s	%d",__FILE__,__LINE__);
}

void stoppingConditionsTrajectoryDialog::on_buttonBoxStoppingConditionsTrajectoryDialog_rejected()
{
    //qWarning("TODO: %s	%d",__FILE__,__LINE__);
}

