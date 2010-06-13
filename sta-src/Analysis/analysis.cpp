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
 ------------------ Author: Ana Raposo and Guillermo Ortega  ------------------------------
*/

#include "analysis.h"
#include "math.h"
#include "Scenario/scenario.h"
#include"Main/propagatedscenario.h"
#include <QDebug>
#include <QComboBox>
#include <QMessageBox>
#include <QInputDialog>
#include"Astro-Core/statevector.h"
#include"Astro-Core/date.h"
#include<QtCore/QFile>
#include<qtextstream.h>
#include"Astro-Core/stacoordsys.h"
#include "Astro-Core/stabody.h"
#include<QDesktopServices>
#include<QUrl>
#include"Astro-Core/calendarTOjulian.h"
#include"Astro-Core/cartesianTOorbital.h"
#include"Astro-Core/date.h"
#include <Coverage/commanalysis.h>
#include <Coverage/coverageanalysis.h>
using namespace std;
//using namespace sta;


analysis::analysis(SpaceScenario*scenario, PropagatedScenario*propagatedScenario,QWidget * parent, Qt::WindowFlags f) : QDialog(parent,f)
{

    setupUi(this);
    m_scenario=new SpaceScenario(*scenario);
    m_propagatedScenario=new PropagatedScenario(*propagatedScenario);

    readScenario();

    connect(treeWidgetReportOptions, SIGNAL(itemDoubleClicked(QTreeWidgetItem*)), this, SLOT(addParameter(QTreeWidgetItem*)));
    connect(AddParameterPushButton, SIGNAL(clicked()), this, SLOT(addParameter()));
    connect(RemoveParameterPushButton, SIGNAL(clicked()), this, SLOT(removeParameter()));
    connect(ComboBoxAnalysisFormat, SIGNAL(activated(int)), this, SLOT(enableReportOption(int)));

}

analysis::~analysis()
{
    for (int i=0;i<treeWidgetTimeSpecifications->topLevelItemCount();i++)
    {
        QTreeWidgetItem*Item=treeWidgetTimeSpecifications->takeTopLevelItem(i);
        delete Item;
        Item=0;
    }
    for(int i=0;i<TreeWidgetMissionArc->topLevelItemCount();i++)
    {
        QTreeWidgetItem*MissionItem=TreeWidgetMissionArc->topLevelItem(i);
        for(int j=0;j<MissionItem->childCount();j++)
        {
        QTreeWidgetItem*Children1=MissionItem->child(j);
        for(int k=0;k<Children1->childCount();k++)
        {
            QTreeWidgetItem*Children2=Children1->child(k);
            for(int l=0;l<Children2->childCount();l++)
            {
                QTreeWidgetItem*Children3=Children2->child(k);
                delete Children3;
            }
            delete Children2;
        }
        delete Children1;
    }
      delete MissionItem;
    }
    for(int i=0;i<treeWidgetShowInReport->topLevelItemCount();i++)
    {
        QTreeWidgetItem* showItem=treeWidgetShowInReport->topLevelItem(i);

            QWidget * boxWidget1;
            QWidget * boxWidget2;
            boxWidget1=treeWidgetShowInReport->itemWidget(showItem,1);
            QComboBox * box1 = dynamic_cast<QComboBox*>(boxWidget1);
            boxWidget2=treeWidgetShowInReport->itemWidget(showItem,2);
            QComboBox * box2 = dynamic_cast<QComboBox*>(boxWidget2);
            delete box1;
            delete box2;
            delete showItem;
     }
    delete m_scenario;
    delete m_propagatedScenario;
}

void analysis::readScenario()
{
    int SpaceObjectInd=0;
    int GroundObjectInd=0;
    int ObjectIndex=0;
    int MissionInd;
    int TxInd;
    int RxInd;
    QStringList AllObjects;


    foreach (QSharedPointer<ScenarioParticipantType> participant, m_scenario->AbstractParticipant())
    {
        int GroundStation=0; //identify the type of Object
        int SpaceCraft=1; //identify the type of Object
        if ((dynamic_cast<ScenarioSC*>(participant.data())!=NULL)) //SC participant
        {
            QTreeWidgetItem * spacecraftItem =new QTreeWidgetItem(TreeWidgetMissionArc);

            spacecraftItem->setText(0,"SpaceCraft");
            QTreeWidgetItem*trajectoryPlan=new QTreeWidgetItem(spacecraftItem);
            trajectoryPlan->setText(0,"TrajectoryPlan");

            QTreeWidgetItem*PayloadSet=new QTreeWidgetItem(spacecraftItem);
            PayloadSet->setText(0,"PayloadSet");

            ScenarioSC* vehicle = dynamic_cast<ScenarioSC*>(participant.data());

            const QList<QSharedPointer<ScenarioAbstractTrajectoryType> >& trajectoryList =
                    vehicle->SCMission()->TrajectoryPlan()->AbstractTrajectory();

            MissionInd=0;
            foreach (QSharedPointer<ScenarioAbstractTrajectoryType> trajectory,trajectoryList) //Loitering
            {
                if (dynamic_cast<ScenarioLoiteringType*>(trajectory.data()))
                {
                    QTreeWidgetItem * loiterItem = new QTreeWidgetItem(trajectoryPlan);
                    loiterItem->setText(0,"Loitering");
                    QTreeWidgetItem * parentType= new QTreeWidgetItem(loiterItem);
                    parentType->setText(0,"space");
                    parentType->setHidden(true);
                    QTreeWidgetItem * parentInd= new QTreeWidgetItem(loiterItem);
                    parentInd->setText(0,QString::number(SpaceObjectInd));
                    parentInd->setHidden(true);
                    QTreeWidgetItem * mindex = new QTreeWidgetItem(loiterItem);
                    mindex->setText(0,QString::number(MissionInd));
                    mindex->setHidden(true);
                    QTreeWidgetItem * mlabel= new QTreeWidgetItem(loiterItem);
                    mlabel->setText(0,"Loiter");
                    mlabel->setHidden(true);
                    QTreeWidgetItem * mission= new QTreeWidgetItem(loiterItem);
                    mission->setText(0,"MArc");
                    mission->setHidden(true);

                    MissionInd++;

                }
            }

            const QList<QSharedPointer<ScenarioAbstractPayloadType> >& payloadList =
                    vehicle->SCMission().data()->PayloadSet().data()->AbstractPayload();

            TxInd=0;
            RxInd=0;

            foreach (QSharedPointer<ScenarioAbstractPayloadType> payloadType, payloadList) //Payload
            {
                if (dynamic_cast<ScenarioTransmitterPayloadType*>(payloadType.data())) //Transmitter
                {
                    QTreeWidgetItem * payloadItem = new QTreeWidgetItem(PayloadSet);
                    payloadItem->setText(0,"Transmitter");

                    QTreeWidgetItem * parentType=new QTreeWidgetItem(payloadItem);
                    parentType->setText(0,QString::number(SpaceCraft));
                    parentType->setHidden(true);

                    QTreeWidgetItem * parentIndex=new QTreeWidgetItem(payloadItem);
                    parentIndex->setText(0,QString::number(SpaceObjectInd));
                    parentIndex->setHidden(true);

                    QTreeWidgetItem * payloadIndex = new QTreeWidgetItem(payloadItem);
                    payloadIndex->setText(0,QString::number(TxInd));
                    payloadIndex->setHidden(true);

                    QTreeWidgetItem * PayloadType = new QTreeWidgetItem(payloadItem);
                    PayloadType->setText(0,"Tx");
                    PayloadType->setHidden(true);

                    TxInd++;

                    TxSC.append(dynamic_cast<ScenarioTransmitterPayloadType*>(payloadType.data()));
                }
                if (dynamic_cast<ScenarioReceiverPayloadType*>(payloadType.data()))
                {
                    QTreeWidgetItem * payloadItem = new QTreeWidgetItem(PayloadSet); //Receiver
                    payloadItem->setText(0,"Receiver");
                    QTreeWidgetItem * parentType=new QTreeWidgetItem(payloadItem);
                    parentType->setText(0,QString::number(SpaceCraft));
                    parentType->setHidden(true);
                    QTreeWidgetItem * parentIndex=new QTreeWidgetItem(payloadItem);
                    parentIndex->setText(0,QString::number(SpaceObjectInd));
                    parentIndex->setHidden(true);
                    QTreeWidgetItem * payloadIndex = new QTreeWidgetItem(payloadItem);
                    payloadIndex->setText(0,QString::number(RxInd));
                    payloadIndex->setHidden(true);

                    QTreeWidgetItem * PayloadType = new QTreeWidgetItem(payloadItem);
                    PayloadType->setText(0,"Rx");
                    PayloadType->setHidden(true);
                    RxInd++;
                    //Receiver=dynamic_cast<ScenarioReceiverPayloadType*>(payloadType.data());
                    //Receiver[0]=dynamic_cast<ScenarioReceiverPayloadType*>(payloadType.data());

                    RxSC.append(dynamic_cast<ScenarioReceiverPayloadType*>(payloadType.data()));
                }

            }
            QTreeWidgetItem*oindex=new QTreeWidgetItem(spacecraftItem);
            oindex->setText(0,QString::number(SpaceObjectInd));
            oindex->setHidden(true);

            QTreeWidgetItem*objectType=new QTreeWidgetItem(spacecraftItem);
            objectType->setText(0,"space");
            objectType->setHidden(true);

            AllObjects.append("SpaceObject");
            //int TotalIndex=ObjectsIndex(AllObjects,SpaceObjectInd,"SC");
            //QTreeWidgetItem*allObjectIndex=new QTreeWidgetItem(spacecraftItem);
            //allObjectIndex->setText(0,QString::number(TotalIndex));
            //allObjectIndex->setHidden(true);
            ////qDebug()<<TotalIndex<<"Indice total do scenario";
            SpaceObjectInd+=1;
        }

        if(dynamic_cast<ScenarioREV*>(participant.data())!=NULL) //Re-entry vehicle
        {
            QTreeWidgetItem * REVitem =new QTreeWidgetItem(TreeWidgetMissionArc);
            REVitem->setText(0,"ReentryVehicle");

            QTreeWidgetItem*trajectoryPlan=new QTreeWidgetItem(REVitem);
            trajectoryPlan->setText(0,"TrajectoryPlan");

            QTreeWidgetItem*PayloadSet=new QTreeWidgetItem(REVitem);
            PayloadSet->setText(0,"PayloadSet");

            ScenarioREV* vehicle = dynamic_cast<ScenarioREV*>(participant.data());

            const QList<QSharedPointer<ScenarioAbstractTrajectoryType> >& trajectoryList =
                    vehicle->REVMission()->REVTrajectoryPlan()->AbstractTrajectory();

            MissionInd=0;

            foreach (QSharedPointer<ScenarioAbstractTrajectoryType> trajectory,trajectoryList)
            {
                if (dynamic_cast<ScenarioEntryArcType*>(trajectory.data())) //re-entry mission arc
                {
                    QTreeWidgetItem * REVmissionItem = new QTreeWidgetItem(trajectoryPlan);
                    REVmissionItem->setText(0,"REV mission arc");
                    QTreeWidgetItem * REVparentType= new QTreeWidgetItem(REVmissionItem);
                    REVparentType->setText(0,"space");
                    REVparentType->setHidden(true);
                    QTreeWidgetItem * REVparentInd= new QTreeWidgetItem(REVmissionItem);
                    REVparentInd->setText(0,QString::number(SpaceObjectInd));
                    REVparentInd->setHidden(true);
                    QTreeWidgetItem * mindex = new QTreeWidgetItem(REVmissionItem);
                    mindex->setText(0,QString::number(MissionInd));
                    mindex->setHidden(true);
                    QTreeWidgetItem * mlabel= new QTreeWidgetItem(REVmissionItem);
                    mlabel->setText(0,"REVmission");
                    mlabel->setHidden(true);
                    QTreeWidgetItem * mission= new QTreeWidgetItem(REVmissionItem);
                    mission->setText(0,"MArc");
                    mission->setHidden(true);
                    MissionInd++;

                }
            }
            QTreeWidgetItem*oindex=new QTreeWidgetItem(REVitem);
            oindex->setText(0,QString::number(SpaceObjectInd));
            oindex->setHidden(true);
            SpaceObjectInd++;
            QTreeWidgetItem*objectType=new QTreeWidgetItem(REVitem);
            objectType->setText(0,"space");
            objectType->setHidden(true);
            AllObjects.append("SpaceObject");
        }
        if(dynamic_cast<ScenarioGroundStation*>(participant.data())) //Ground Station
        {
            QTreeWidgetItem * GSitem =new QTreeWidgetItem(TreeWidgetMissionArc);
            GSitem->setText(0,"GroundStation");
            QTreeWidgetItem*PayloadSet=new QTreeWidgetItem(GSitem);
            PayloadSet->setText(0,"PayloadSet");


            ScenarioGroundStation* station = dynamic_cast<ScenarioGroundStation*>(participant.data());
            const QList<QSharedPointer<ScenarioAbstractPayloadType> >& payloadList=station->PayloadSet().data()->AbstractPayload();

            RxInd=0;
            TxInd=0;

            GSEnvironment.append(dynamic_cast<ScenarioGroundStationEnvironment*>(station->GroundStationEnvironment().data()));

            foreach (QSharedPointer<ScenarioAbstractPayloadType> payloadType, payloadList) //Payload
            {
                if (dynamic_cast<ScenarioTransmitterPayloadType*>(payloadType.data())) //Transmitter
                {
                    QTreeWidgetItem * payloadItem = new QTreeWidgetItem(PayloadSet);
                    payloadItem->setText(0,"Transmitter");
                    QTreeWidgetItem * parentType=new QTreeWidgetItem(payloadItem);
                    parentType->setText(0,QString::number(GroundStation));
                    parentType->setHidden(true);
                    QTreeWidgetItem * parentIndex=new QTreeWidgetItem(payloadItem);
                    parentIndex->setText(0,QString::number(GroundObjectInd));
                    parentIndex->setHidden(true);
                    QTreeWidgetItem * payloadIndex = new QTreeWidgetItem(payloadItem);
                    payloadIndex->setText(0,QString::number(TxInd));
                    payloadIndex->setHidden(true);
                    QTreeWidgetItem * PayloadType = new QTreeWidgetItem(payloadItem);
                    PayloadType->setText(0,"Tx");
                    PayloadType->setHidden(true);
                    TxInd++;

                    TxGS.append(dynamic_cast<ScenarioTransmitterPayloadType*>(payloadType.data()));
                    //Transmitter[0]=dynamic_cast<ScenarioTransmitterPayloadType*>(payloadType.data());
                }
                if (dynamic_cast<ScenarioReceiverPayloadType*>(payloadType.data()))
                {
                    QTreeWidgetItem * payloadItem = new QTreeWidgetItem(PayloadSet); //Receiver
                    payloadItem->setText(0,"Receiver");
                    QTreeWidgetItem * parentType=new QTreeWidgetItem(payloadItem);
                    parentType->setText(0,QString::number(GroundStation));
                    parentType->setHidden(true);
                    QTreeWidgetItem * parentIndex=new QTreeWidgetItem(payloadItem);
                    parentIndex->setText(0,QString::number(GroundObjectInd));
                    parentIndex->setHidden(true);
                    QTreeWidgetItem * payloadIndex = new QTreeWidgetItem(payloadItem);
                    payloadIndex->setText(0,QString::number(RxInd));
                    payloadIndex->setHidden(true);
                    QTreeWidgetItem * PayloadType = new QTreeWidgetItem(payloadItem);
                    PayloadType->setText(0,"Rx");
                    PayloadType->setHidden(true);
                    RxInd++;

                    RxGS.append(dynamic_cast<ScenarioReceiverPayloadType*>(payloadType.data()));
                    ////qDebug()<<RxGS[0];
                    //Receiver=dynamic_cast<ScenarioReceiverPayloadType*>(payloadType.data());//Ricardo&Ana
                }

            }

            QTreeWidgetItem*oindex=new QTreeWidgetItem(GSitem);
            oindex->setText(0,QString::number(GroundObjectInd));
            oindex->setHidden(true);
            GroundObjectInd++;
            QTreeWidgetItem*objectType=new QTreeWidgetItem(GSitem);
            objectType->setText(0,"ground");
            objectType->setHidden(true);
            AllObjects.append("GroundObject");
        }

        //aumentar aqui
    }

}

int analysis::ObjectsIndex(QStringList AllObjects, int Index, QString ObjectType)
{
    int j=-1;
    for (int i=0;i<AllObjects.size();i++)
    {

        if(ObjectType==AllObjects.at(i))
        {
            j=j+1;
            if(j==Index)
            {
                return i;
            }
        }
    }
}

void analysis::on_buttonBox_helpRequested()
{
    qWarning("TODO: %s	%d",__FILE__,__LINE__);
}

void analysis::on_buttonBox_clicked(QAbstractButton*)
{
    qWarning("TODO: %s	%d",__FILE__,__LINE__);
}

void analysis::on_buttonBox_destroyed(QObject*)
{
    qWarning("TODO: %s	%d",__FILE__,__LINE__);
}

void analysis::on_buttonBox_destroyed()
{
    qWarning("TODO: %s	%d",__FILE__,__LINE__);
}

void analysis::on_buttonBox_customContextMenuRequested(const QPoint&)
{
    qWarning("TODO: %s	%d",__FILE__,__LINE__);
}

void analysis::on_buttonBox_accepted()
{
    qWarning("TODO: %s	%d",__FILE__,__LINE__);
}

void analysis::on_buttonBox_rejected()
{
    qWarning("TODO: %s	%d",__FILE__,__LINE__);
}




/*

void Analysis::on_groupBoxAnalysisFormat_toggled(bool)
{
	qWarning("TODO: %s	%d",__FILE__,__LINE__);
}
*/
/*void analysis::on_ComboBoxAnalysisFormat_activated(const QString&)
{
if(ComboBoxAnalysisFormat->currentText()=="Report")
stackedWidget->setCurrentWidget(pageReport);

if((ComboBoxAnalysisFormat->currentText()=="3D plot")||(ComboBoxAnalysisFormat->currentText()=="2D plot")||(ComboBoxAnalysisFormat->currentText()=="2.5D plot"))
    stackedWidget->setCurrentWidget(pagePlotAxesSettings);

}*/

void analysis::on_groupBoxParameters_toggled(bool)
{
    qWarning("TODO: %s	%d",__FILE__,__LINE__);
}

void analysis::on_groupBoxTimeSpecifications_toggled(bool)
{
    qWarning("TODO: %s	%d",__FILE__,__LINE__);
}

bool analysis::CheckIfMissionArc()
{

    int isMissionArc=0;

    QList<QTreeWidgetItem *> selectedMissionArcs=TreeWidgetMissionArc->selectedItems();
    if (selectedMissionArcs.isEmpty())
    {

        return(false);
    }
    else
    {
        for(int i=0;i<selectedMissionArcs.size();i++)
        {
            if (selectedMissionArcs.at(i)->childCount()==5 && selectedMissionArcs.at(i)->child(4)->text(0)=="MArc")
            {
                isMissionArc++;
            }

            else
            {
                return(false);
            }
        }

        if (isMissionArc==selectedMissionArcs.size())
        {
            return (true);
        }
        else
            return(false);
    }

}

void analysis::on_AddDefaultPushButton_clicked()
{
    int NumberOfObjects=TreeWidgetMissionArc->topLevelItemCount();


    QStringList AllObjects;
    QList<int>SelectedObjects;
    for(int i=0;i<NumberOfObjects;i++)
    {
        int numberOfChildren=TreeWidgetMissionArc->topLevelItem(i)->childCount();
        AllObjects.append(TreeWidgetMissionArc->topLevelItem(i)->child(numberOfChildren-1)->text(0));
        ////qDebug()<<"Top Level"<<TreeWidgetMissionArc->topLevelItem(i)->text(0);
        ////qDebug()<<AllObjects.at(i)<<"All objects";
    }

    QList<QTreeWidgetItem *> selected=TreeWidgetMissionArc->selectedItems();

    //int SObjectsSize=0;
    //int* SObjectsIndex=(int*) calloc(1,sizeof(int));
    //int GObjectsSize=0;
    //int* GObjectsIndex=(int*) calloc(1,sizeof(int));
    QList<int>GObjectsIndex;
    QList<int>SObjectsIndex;
    QList<int>MIndex;
    QList<int>MParentIndex;
    QList<int>TxParentIndex;
    QList<int>TxLocIndex;
    QList<int>RxParentIndex;
    QList<int>RxLocIndex;

    if (CheckIfMissionArc())
    {
        for (int t=0;t<selected.size();t++)
        {
            int childnum=selected.at(t)->childCount();
            if (selected.at(t)->child(childnum-1)->text(0)=="ground")
            {

                GObjectsIndex.append(selected.at(t)->child(childnum-2)->text(0).toInt());
            }
            if (selected.at(t)->child(childnum-1)->text(0)=="space")
            {

                SObjectsIndex.append(selected.at(t)->child(childnum-2)->text(0).toInt());
            }
            if (selected.at(t)->child(childnum-1)->text(0)=="MArc")
            {

                MIndex.append(selected.at(t)->child(childnum-3)->text(0).toInt());

                MParentIndex.append(selected.at(t)->child(childnum-4)->text(0).toInt());

            }
            if(selected.at(t)->child(childnum-1)->text(0)=="Tx")
            {

                TxParentIndex.append(selected.at(t)->child(childnum-3)->text(0).toInt());

                TxLocIndex.append(selected.at(t)->child(childnum-4)->text(0).toInt());
            }
            if(selected.at(t)->child(childnum-1)->text(0)=="Rx")
            {

                RxParentIndex.append(selected.at(t)->child(childnum-3)->text(0).toInt());

                RxLocIndex.append(selected.at(t)->child(childnum-4)->text(0).toInt());
            }

        }
        ////qDebug()<<"faz os vectores";

        for(int i=0;i<MParentIndex.size();i++)
        {
            SelectedObjects.append(ObjectsIndex(AllObjects,MParentIndex.at(i),"space"));

        }

        for(int z=0;z<SelectedObjects.size();z++)
        {

            //QSharedPointer<ScenarioParticipantType>participant=m_scenario->AbstractParticipant().at(MParentIndex[z]);
            QSharedPointer<ScenarioParticipantType>participant=m_scenario->AbstractParticipant().at(SelectedObjects.at(z));
            if ((dynamic_cast<ScenarioSC*>(participant.data())!=NULL))
	    {

                ScenarioSC* vehicle = dynamic_cast<ScenarioSC*>(participant.data());
                const QList<QSharedPointer<ScenarioAbstractTrajectoryType> >& trajectoryList =
                        vehicle->SCMission()->TrajectoryPlan()->AbstractTrajectory();

                {

                    //QSharedPointer<ScenarioAbstractTrajectoryType>trajectory=trajectoryList.at(MObjectsIndex[y]);
                    QSharedPointer<ScenarioAbstractTrajectoryType>trajectory=trajectoryList.at(MIndex.at(z));

                    if (dynamic_cast<ScenarioLoiteringType*>(trajectory.data()))
                    {

                        ScenarioLoiteringType* loitering = dynamic_cast<ScenarioLoiteringType*>(trajectory.data());
                        QDateTime StartTimeDate=loitering->TimeLine()->StartTime();
                        QDateTime EndTimeDate=loitering->TimeLine()->EndTime();
                        double StepTime=loitering->TimeLine()->StepTime();
                        QString StepTimeSec= QString::number(StepTime);

                        double StartTimeYear=StartTimeDate.date().year();
                        double EndTimeYear=EndTimeDate.date().year();

                        double StartTimeMonth=StartTimeDate.date().month();
                        double EndTimeMonth=EndTimeDate.date().month();

                        double StartTimeDay=StartTimeDate.date().day();
                        double EndTimeDay=EndTimeDate.date().day();

                        double StartTimeHour=StartTimeDate.time().hour();
                        double EndTimeHour=EndTimeDate.time().hour();

                        double StartTimeMinute=StartTimeDate.time().minute();
                        double EndTimeMinute=EndTimeDate.time().minute();

                        double StartTimeSecond=StartTimeDate.time().second();
                        double EndTimeSecond=EndTimeDate.time().second();

                        QString StartTime = QString::number(StartTimeDay);
                        StartTime.append("/");
                        StartTime.append(QString::number(StartTimeMonth));
                        StartTime.append("/");
                        StartTime.append(QString::number(StartTimeYear));
                        StartTime.append(" ");
                        StartTime.append(QString::number(StartTimeHour));
                        StartTime.append(":");
                        StartTime.append(QString::number(StartTimeMinute));
                        StartTime.append(":");
                        StartTime.append(QString::number(StartTimeSecond));



                        QString EndTime = QString::number(EndTimeDay);
                        EndTime.append("/");
                        EndTime.append(QString::number(EndTimeMonth));
                        EndTime.append("/");
                        EndTime.append(QString::number(EndTimeYear));
                        EndTime.append(" ");
                        EndTime.append(QString::number(EndTimeHour));
                        EndTime.append(":");
                        EndTime.append(QString::number(EndTimeMinute));
                        EndTime.append(":");
                        EndTime.append(QString::number(EndTimeSecond));

                        if((treeWidgetTimeSpecifications->findItems(StartTime,Qt::MatchExactly,0).isEmpty())||(treeWidgetTimeSpecifications->findItems(EndTime,Qt::MatchExactly,1).isEmpty())||(treeWidgetTimeSpecifications->findItems(StepTimeSec,Qt::MatchExactly,2).isEmpty()))
                        {
                            QTreeWidgetItem*treeTime=new QTreeWidgetItem(treeWidgetTimeSpecifications);
                            treeTime->setText(0,StartTime);
                            treeTime->setText(1,EndTime);
                            treeTime->setText(2,StepTimeSec);
                        }

                    }
                }
            }


            if ((dynamic_cast<ScenarioREV*>(participant.data())!=NULL)) //Re-entry mission arc

            {


                ScenarioREV* vehicle = dynamic_cast<ScenarioREV*>(participant.data());
                const QList<QSharedPointer<ScenarioAbstractTrajectoryType> >& trajectoryList =
                        vehicle->REVMission()->REVTrajectoryPlan()->AbstractTrajectory();

                foreach (QSharedPointer<ScenarioAbstractTrajectoryType> trajectory,trajectoryList)
                {
                    if (dynamic_cast<ScenarioEntryArcType*>(trajectory.data()))
                    {
                        //////qDebug()<<"loitering";
                        ScenarioEntryArcType* entry = dynamic_cast<ScenarioEntryArcType*>(trajectory.data());
                        QDateTime StartTimeDate=entry->TimeLine()->StartTime();

                        QDateTime EndTimeDate=entry->TimeLine()->EndTime();

                        double StepTime=entry->PropagationPosition().data()->timeStep();
                        QString StepTimeSec= QString::number(StepTime);

                        double StartTimeYear=StartTimeDate.date().year();
                        double EndTimeYear=EndTimeDate.date().year();

                        double StartTimeMonth=StartTimeDate.date().month();
                        double EndTimeMonth=EndTimeDate.date().month();

                        double StartTimeDay=StartTimeDate.date().day();
                        double EndTimeDay=EndTimeDate.date().day();

                        double StartTimeHour=StartTimeDate.time().hour();
                        double EndTimeHour=EndTimeDate.time().hour();

                        double StartTimeMinute=StartTimeDate.time().minute();
                        double EndTimeMinute=EndTimeDate.time().minute();

                        double StartTimeSecond=StartTimeDate.time().second();
                        double EndTimeSecond=EndTimeDate.time().second();

                        QString StartTime = QString::number(StartTimeDay);
                        StartTime.append("/");
                        StartTime.append(QString::number(StartTimeMonth));
                        StartTime.append("/");
                        StartTime.append(QString::number(StartTimeYear));
                        StartTime.append(" ");
                        StartTime.append(QString::number(StartTimeHour));
                        StartTime.append(":");
                        StartTime.append(QString::number(StartTimeMinute));
                        StartTime.append(":");
                        StartTime.append(QString::number(StartTimeSecond));


                        QString EndTime = QString::number(EndTimeDay);
                        EndTime.append("/");
                        EndTime.append(QString::number(EndTimeMonth));
                        EndTime.append("/");
                        EndTime.append(QString::number(EndTimeYear));
                        EndTime.append(" ");
                        EndTime.append(QString::number(EndTimeHour));
                        EndTime.append(":");
                        EndTime.append(QString::number(EndTimeMinute));
                        EndTime.append(":");
                        EndTime.append(QString::number(EndTimeSecond));


                        if((treeWidgetTimeSpecifications->findItems(EndTime,Qt::MatchExactly,1).isEmpty())||((treeWidgetTimeSpecifications->findItems(StepTimeSec,Qt::MatchExactly,2).isEmpty()))||(treeWidgetTimeSpecifications->findItems(StartTime,Qt::MatchExactly).isEmpty()))
                        {
                            QTreeWidgetItem*treeTime=new QTreeWidgetItem(treeWidgetTimeSpecifications);
                            treeTime->setText(0,StartTime);
                            treeTime->setText(1,EndTime);
                            treeTime->setText(2,StepTimeSec);
                        }

                    }
                }
            }

        }
    }
    else
    {
        QMessageBox MissionArcWarning;
        MissionArcWarning.setText("Please select only valid Mission Arcs");
        MissionArcWarning.exec();
    }
}


void analysis::on_AddNewPushButton_clicked()
{
    //if (CheckIfMissionArc())

    //treeItem->setText(2," ");

    //else


}

void analysis::ReadTime(int column, double *MJD) //reads the time in the treeWidgetTimeSpecifications
{

    QList<QTreeWidgetItem*>selectedTimes=treeWidgetTimeSpecifications->selectedItems();

    if (selectedTimes.size() >0)
    {
        int i;
        double JD;

        for (i=0;i<selectedTimes.size();i++)
        {

            QString SelTime=selectedTimes[i]->text(column);

            QString Date=SelTime.section(' ',0,0);
            QString Time=SelTime.section(' ',1,1);

            QString Day=Date.section('/',0,0);
            QString Month=Date.section('/',1,1);
            QString Year=Date.section('/',2,2);

            QString Hour=Time.section(':',0,0);
            QString Minute=Time.section(':',1,1);
            QString Second=Time.section(':',2,2);

            int iDay=Day.toInt();
            int iMonth=Month.toInt();
            int iYear=Year.toInt();

            int iHour=Hour.toInt();
            int iMinute=Minute.toInt();
            int iSecond=Second.toInt();

            //convert to MJD
            JD=calendarTOjulian(iYear,iMonth,iDay,iHour,iMinute,iSecond);
            MJD[i]=sta::JdToMjd(JD);
            //MJD[i]=JD-2400000.5;

        }
    }
    else
    {
        //      return 0;  ;
    }
}

void analysis::on_EditTimePushButton_clicked() //adds new time intervals to the tree
{
    bool ok;
    int check=0;
    //QList<QTreeWidgetItem *> selectedTimes=treeWidgetTimeSpecifications->selectedItems();
    QString InitTime = QInputDialog::getText(this, tr("Insert Start Time"),tr("Start Time"), QLineEdit::Normal,"dd/mm/yyyy hh:mm:ss",&ok);
    QString Date=InitTime.section(' ',0,0);
    QString Time=InitTime.section(' ',1,1);

    QString Day=Date.section('/',0,0);
    QString Month=Date.section('/',1,1);
    QString Year=Date.section('/',2,2);

    QString Hour=Time.section(':',0,0);
    QString Minute=Time.section(':',1,1);
    QString Second=Time.section(':',2,2);

    int iFDay=Day.toInt(&ok);
    int iFYear;
    int iFMonth;
    int iFHour;
    int iFMinute;
    int iFSecond;
    int iEYear;
    int iEMonth;
    int iEDay;
    int iEHour;
    int iEMinute;
    int iESecond;

    if (ok && iFDay>0 && iFDay<32){
        iFMonth=Month.toInt(&ok);
        if (ok && iFMonth>0 && iFMonth<13) {
            iFYear=Year.toInt(&ok);
            if (ok){
                iFHour=Hour.toInt(&ok);
                if (ok && iFHour>=0 && iFHour<24){
                    iFMinute=Minute.toInt(&ok);
                    if(ok && iFMinute>=0 && iFMinute<60){
                        iFSecond=Second.toInt(&ok);
                        if(ok && iFSecond>=0 && iFSecond<60){

                            QString EndTime = QInputDialog::getText(this, tr("Insert End Time"),tr("End Time"), QLineEdit::Normal,"dd/mm/yyyy hh:mm:ss",&ok);

                            QString Date=EndTime.section(' ',0,0);
                            QString Time=EndTime.section(' ',1,1);

                            QString Day=Date.section('/',0,0);
                            QString Month=Date.section('/',1,1);
                            QString Year=Date.section('/',2,2);

                            QString Hour=Time.section(':',0,0);
                            QString Minute=Time.section(':',1,1);
                            QString Second=Time.section(':',2,2);

                            iEDay=Day.toInt(&ok);

                            if (ok && iEDay>0 && iEDay<32){ //check if the numbers introduced by the user are compatible with the calendar
                                iEMonth=Month.toInt(&ok);
                                if (ok && iEMonth>0 && iEMonth<13) {
                                    iEYear=Year.toInt(&ok);
                                    if (ok){
                                        iEHour=Hour.toInt(&ok);
                                        if (ok && iEHour>=0 && iEHour<24){
                                            iEMinute=Minute.toInt(&ok);
                                            if(ok && iEMinute>=0 && iEMinute<60){
                                                iESecond=Second.toInt(&ok);
                                                if(ok && iESecond>=0 && iESecond<60){



                                                    QTreeWidgetItem*treeTimeN=new QTreeWidgetItem(treeWidgetTimeSpecifications);
                                                    treeTimeN->setText(0,InitTime);
                                                    treeTimeN->setText(1,EndTime);
                                                    check=1;
                                                }}}}}}}}}}}}

    if (check==0) {
        QMessageBox parametersWarning;
        parametersWarning.setText("Time Input Error, please verify format"); //the date introduced does not match a calendar date
        parametersWarning.exec();}

    //added new
    double jFirstTime=calendarTOjulian(iFYear,iFMonth,iFDay,iFHour,iFMinute,iFSecond);
    double jEndTime=calendarTOjulian(iEYear,iEMonth,iEDay,iEHour,iEMinute,iESecond);

    double MJDFirstTime=sta::JdToMjd(jFirstTime);
    double MJDEndTime=sta::JdToMjd(jEndTime);


    /*
if (FirstTime<StartEpoch)
{
QMessageBox InvalidStartWarning;
InvalidStartWarning.setText("Invalid Start Time");
InvalidStartWarning.exec();
return;
}

if(EndTime>StopEpoch)
{
QMessageBox InvalidStartWarning;
InvalidStartWarning.setText("Invalid Stop Time");
InvalidStartWarning.exec();
return;
}*/
}

void analysis::on_DeleteTimePushButton_clicked()
{
    QList<QTreeWidgetItem *> selectedTimes=treeWidgetTimeSpecifications->selectedItems();
    for (int i=0;i<selectedTimes.size();i++)
    {
        delete selectedTimes.at(i);
    }
}

//void Analysis::on_groupBoxReport_toggled(bool)
//{
//	qWarning("TODO: %s	%d",__FILE__,__LINE__);
//}

//void Analysis::on_AddParameterPushButton_clicked()
//{
//      qWarning("TODO: %s	%d",__FILE__,__LINE__);
//}
//void Analysis::on_RemoveParameterPushButton_clicked()
//{
//	qWarning("TODO: %s	%d",__FILE__,__LINE__);
//}

/*void analysis::on_groupBoxPlotAxesSettings_toggled(bool)
{
	qWarning("TODO: %s	%d",__FILE__,__LINE__);
}*/
int analysis::InputsControl()
{
    int CovCommCount=0;
    //QList<QTreeWidgetItem*>ShowInReport=treeWidgetShowInReport->topLevelItem();
    for(int i=0;i<treeWidgetShowInReport->topLevelItemCount();i++)
    {
        QTreeWidgetItem*ShowInReport=treeWidgetShowInReport->topLevelItem(i);
        QString ToReport=ShowInReport->text(0);
        if((ToReport=="Azimuth")||(ToReport=="Elevation")||(ToReport=="Range")||(ToReport=="Equivalent Isotropical Radiated Power")||(ToReport=="Received Frequency")||(ToReport=="Doppler Shift")||(ToReport=="Received Power")||(ToReport=="Flux Density")||(ToReport=="Overlap Bandwidth Factor")||(ToReport=="Free Space Loss")||(ToReport=="Oxygen Loss")||(ToReport=="Water Vapour Loss")||(ToReport=="Rain Loss")||(ToReport=="Atmospheric Loss")||(ToReport=="Propagation Loss")||(ToReport=="G/T")||(ToReport=="C/No")||(ToReport=="C/N")||(ToReport=="Eb/No")||(ToReport=="BER"))
        {
            ////qDebug()<<"Cov parameters counting in Inputs Control";
            CovCommCount++;
        }
    }
    QList<QTreeWidgetItem*>Selected=TreeWidgetMissionArc->selectedItems();
    int TxCount=0;
    int RxCount=0;
    int loiterCountCheck=0;
    int spaceObjCount=0;
    int groundObjCount=0;
    //QList<QString>TypeParent;
    QList<int>MissionIndexParent;
    QList<int>SpaceIndex;

    if(CovCommCount!=0)
    {
        ////qDebug()<<"doing the if, so we're calculating coverage or communication parameters";
        if(Selected.size()<3)
        {
            ////qDebug()<<"select one Tx, one Rx and at least one MArc";
            return 3;
        }
        else
        {
            for(int j=0;j<Selected.length();j++)
            {
                int childnumber=Selected.at(j)->childCount();
                if(childnumber==5)
                {
                    if(Selected.at(j)->child(3)->text(0)=="loiter")
                    {
                        MissionIndexParent.append(Selected.at(j)->child(1)->text(0).toInt());
                        ////qDebug()<<"mission arc index being added"<<Selected.at(j)->child(1)->text(0).toInt();
                    }
                }
                ////qDebug()<<childnumber<<j<<"hfjuf";
                if(childnumber==4)//Tx and Rx
                {
                    ////qDebug()<<"there are Rx and Tx selected"<<Selected.at(j)->child(3)->text(0);
                    if((Selected.at(j)->child(3)->text(0)=="Tx"))
                    {
                        TxCount++;
                    }
                    if((Selected.at(j)->child(3)->text(0)=="Rx"))
                    {
                        RxCount++;
                    }
                    ////qDebug()<<Selected.at(j)->child(3)->text(0);"child 3";
                    if((Selected.at(j)->child(0)->text(0)=="1"))
                    {
                        spaceObjCount++;
                        SpaceIndex.append(Selected.at(j)->child(1)->text(0).toInt());
                        ////qDebug()<<spaceObjCount<<"number of parents of Tx/Rx that are space objects";
                    }
                    if((Selected.at(j)->child(0)->text(0)=="0"))
                    {
                        ////qDebug()<<"there are ground objects too";
                        groundObjCount++;
                        ////qDebug()<<groundObjCount<<"number of parents of Tx/Rx that are ground objects";
                    }
                }
                ////qDebug()<<TxCount<<RxCount<<"tx e Rx number";
                ////qDebug()<<spaceObjCount<<groundObjCount<<"space and ground number";
            }

            if((RxCount!=1)||(TxCount!=1))
            {
                return 1; //invalid number of Tx and Rx
            }
            if((spaceObjCount!=1)||(groundObjCount!=1))
            {
                return 2; //invalid Tx and Rx location
            }
            if((TxCount==1) && (RxCount==1)&&(spaceObjCount==1)&&(groundObjCount==1))
            {
                for(int i=0;i<MissionIndexParent.size();i++)
                {
                    ////qDebug()<<"m parent index"<<MissionIndexParent.at(i);
                    ////qDebug()<<"space index"<<SpaceIndex.at(0);
                    if(MissionIndexParent.at(i)!=SpaceIndex.at(0))
                    {
                        loiterCountCheck++;
                    }
                }
            }
            if (loiterCountCheck!=0)
            {
                return 4; //Choose only loiterings belonging to the space object with the payload
            }
            ////qDebug()<<loiterCountCheck<<"loiter count";
            if((RxCount==1)&&(TxCount==1)&&(spaceObjCount==1)&&(groundObjCount==1)&&(loiterCountCheck==0))
            {
                return 0; //correct inputs, proceed
            }
        }

    }
    else
    {
        return 0; //no cov or comm calculations, allow the rest of the program to run
    }

}
void analysis::Warnings(int i)
{
    if(i==1)
    {
        QMessageBox CovCommWarning;
        CovCommWarning.setText("Please select only one Transmitter and one Receiver");
        CovCommWarning.exec();
        return;
    }
    if(i==2)
    {
        QMessageBox CovCommParentWarning;
        CovCommParentWarning.setText("Please select the Transmitter and the Receiver in different types of parent objects");
        CovCommParentWarning.exec();
        return;
    }
    if(i==3)
    {
        QMessageBox CovCommParentWarning;
        CovCommParentWarning.setText("Please select one Transmitter,one Receiver and one or more Mission Arcs");
        CovCommParentWarning.exec();
        return;
    }
    if(i==4)
    {
        QMessageBox CovCommParentWarning;
        CovCommParentWarning.setText("Please choose Loiterings belonging exclusively to the SC with the defined Payload");
        CovCommParentWarning.exec();
        return;
    }
}

void analysis::on_GeneratePushButton_clicked()
{

    QList<QTreeWidgetItem *> selected=TreeWidgetMissionArc->selectedItems();
    QList<QTreeWidgetItem *> selectedTimeStep=treeWidgetTimeSpecifications->selectedItems();
    int numberOfParameters=treeWidgetShowInReport->topLevelItemCount();
    if(numberOfParameters==0)
    {
        QMessageBox parametersWarning;
        parametersWarning.setText("Please select the parameters to be calculated");
        parametersWarning.exec();
        return;
    }

    else
    {
        if(selected.size()==0)
        {
            QMessageBox MissionWarning;
            MissionWarning.setText("Please select one or more Mission Arcs");
            MissionWarning.exec();
            return;
        }

        else
        {
            if (selectedTimeStep.size()==0)
            {
                QMessageBox tswarning;
                tswarning.setText("Please select one or more time intervals");
                tswarning.exec();
            }
            else
            {
                int Control=InputsControl(); //checks if the selected parameters are valid according to the inputs n other module functions
                if(Control!=0)
                {
                    Warnings(Control);
                }
                else
                {
                    ////qDebug()<<"ha dados para os calculos, todos os erros foram evitados";
                    QList<int>SObjectsIndex;
                    QList<int>GObjectsIndex;
                    QList<int>MObjectsIndex;
                    QList<int>MParentIndex;

                    int TxParentIndex;
                    bool RxParentType;
                    int TxArrayIndex;
                    int RxArrayIndex;
                    bool TxParentType; //inputCoverage
                    int RxParentIndex; //input Coverage
                    int indSC; //inputCoverage
                    int indGS; //input Coverage
                    ScenarioReceiverPayloadType*Receiver;
                    ScenarioTransmitterPayloadType*Transmitter;
                    ScenarioGroundStationEnvironment*Environment;
                    bool ReadCoverage=false;
                    bool ReadCommunication1=false;
                    bool ReadCommunication2=false;
                    bool ReadCommunication3=false;

                    for (int t=0;t<selected.size();t++) // write the QLists with Parents and Mission indexes

                    {
                        int childnum=selected.at(t)->childCount();
                        if (selected.at(t)->child(childnum-1)->text(0)=="ground")
                        {
                            GObjectsIndex.append(selected.at(t)->child(childnum-2)->text(0).toInt());
                        }
                        if (selected.at(t)->child(childnum-1)->text(0)=="space")
                        {
                            SObjectsIndex.append(selected.at(t)->child(childnum-2)->text(0).toInt());
                        }
                        if (selected.at(t)->child(childnum-1)->text(0)=="MArc")
                        {
                            MObjectsIndex.append(selected.at(t)->child(childnum-3)->text(0).toInt());
                            MParentIndex.append(selected.at(t)->child(childnum-4)->text(0).toInt());
                        }

                        if(selected.at(t)->child(childnum-1)->text(0)=="Tx")
                        {
                            ////qDebug()<<"entra no if do Tx";
                            TxParentIndex=selected.at(t)->child(1)->text(0).toInt();
                            ////qDebug()<<"save tx parent index"<<TxParentIndex;
                            TxArrayIndex=selected.at(t)->child(2)->text(0).toInt();
                            ////qDebug()<<"save the index of the selected tx"<<TxArrayIndex;
                            int TxParentTy=selected.at(t)->child(0)->text(0).toInt();

                            ////qDebug()<<"parent type do tx"<<TxParentTy;

                            if(TxParentTy==0)
                            {
                                indGS=TxParentIndex;
                                Transmitter=TxGS.at(TxArrayIndex);
                                Environment=GSEnvironment.at(indGS);
                                TxParentType=false;
                            }
                            if(TxParentTy==1)
                            {
                                indSC=TxParentIndex;
                                Transmitter=TxSC.at(TxArrayIndex);
                                TxParentType=true;
                                ////qDebug()<<"tx no sc,qual o parent?"<<indSC;
                            }
                        }
                        ////qDebug()<<"em bool, parent do Tx"<<TxParentType;
                        //////qDebug()<<"le o RX?";
                        if(selected.at(t)->child(childnum-1)->text(0)=="Rx")
                        {
                            RxParentIndex=selected.at(t)->child(childnum-3)->text(0).toInt();
                            RxArrayIndex=selected.at(t)->child(childnum-2)->text(0).toInt();
                            int RxParentTy=(selected.at(t)->child(0)->text(0)).toInt();
                            if(RxParentTy==0)
                            {
                                ////qDebug()<<"entrou no if de GS para parent Type de Rx";
                                indGS=RxParentIndex;
                                Receiver=RxGS.at(RxArrayIndex);
                                Environment=GSEnvironment.at(indGS);
                                RxParentType=false;
                                ////qDebug()<<"rx na GS,qual o parent?"<<indGS;
                            }
                            //indGS=0;
                            if(RxParentTy==1)
                            {
                                ////qDebug()<<"entrou no if de SC para parent type de Rx";
                                indSC=RxParentIndex;
                                Receiver=RxSC.at(RxArrayIndex);
                                RxParentType=true;
                            }
                        }

                    }

                    for(int i=0;i<treeWidgetShowInReport->topLevelItemCount();i++) //sets true some bool

                    {
                        QTreeWidgetItem*parameter=treeWidgetShowInReport->topLevelItem(i);
                        QString name=parameter->text(0);

                        if(name=="Azimuth"||name=="Elevation"||name=="Range")
                        {
                            ReadCoverage=true;
                        }
                        if((name=="Equivalent Isotropical Radiated Power")||(name=="Received Frequency")||(name=="Doppler Shift")||(name=="Received Power")||(name=="Flux Density")||(name=="Overlap Bandwidth Factor"))
                        {
                            ReadCommunication1=true;
                        }
                        if((name=="Free Space Loss")||(name=="Oxygen Loss")||(name=="Water Vapour Loss")||(name=="Rain Loss")||(name=="Atmospheric Loss")||(name=="Propagation Loss"))
                        {
                            ReadCommunication2=true;
                        }
                        if((name=="G/T")||(name=="C/No")||(name=="C/N")||(name=="Eb/No")||(name=="BER"))
                            ReadCommunication3=true;
                    }

                    QFile file("analysisReport.txt");
                    file.open(QIODevice::WriteOnly);
                    QTextStream stream(&file);
                    stream.setRealNumberPrecision(16);

                    //######time of generation of report#####
                    QDateTime ReportDateTime=QDateTime::currentDateTime();

                    stream<<"Report generated on"<<"\t"<<ReportDateTime.date().day()<<"/"<<ReportDateTime.date().month()<<"/"<<ReportDateTime.date().year()<<" "<<"at"<<" "<<ReportDateTime.time().hour()<<":"<<ReportDateTime.time().minute()<<":"<<ReportDateTime.time().second()<<"\r\n";


                    for(int z=0;z<MParentIndex.size();z++)
                    {
                        SpaceObject*spaceObj=m_propagatedScenario->spaceObjects().at(MParentIndex.at(z));

                        //for(int y=0;y<MObjectsIndex.size();y++)
                        {
                            int indMissionArc=MObjectsIndex.at(z);
                            MissionArc*arc=spaceObj->mission().at(MObjectsIndex.at(z));

                            QList<QTreeWidgetItem *> selectedTimes=treeWidgetTimeSpecifications->selectedItems();
double StartTime[selectedTimes.size()];
double StopTime[selectedTimes.size()];
                            int countStart[selectedTimes.size()];
                            int countStop[selectedTimes.size()];

                            //double* StartTime=(double*) calloc(selectedTimes.size(),sizeof(double));
                            ReadTime(0,StartTime);//in MJD

                            //double* StopTime=(double*) calloc(selectedTimes.size(),sizeof(double));
                            ReadTime(1,StopTime); //in MJD

                            double StartEpoch=arc->trajectorySampleTime(0);
                            int totalTime=(arc->trajectorySampleCount())-1;
                            double StopEpoch=arc->trajectorySampleTime(totalTime);

                            for (int j=0;j<selectedTimes.size();j++) // Control loop to determine if the start times are valid

                            {

                                if ((StartTime[j]<StartEpoch)||(StartTime[j]>=StopEpoch))
                                {
                                    QMessageBox InvalidStartWarning;
                                    InvalidStartWarning.setText("Invalid Start Time");
                                    InvalidStartWarning.exec();
                                    return;
                                }
                                else // >=
                                {
                                    int i=0;
                                    while(( (arc->trajectorySampleTime(i))/*+MJDoffset*/)<StartTime[j])
                                    {

                                        //////qDebug()<<"startTime"<<StartTime[j];
                                        i=i+1;
                                    }
                                    countStart[j]=i;
                                }
                            }

                            for (int j=0;j<selectedTimes.size();j++) //Control loop to determine if the stop times are valid

                            {
                                if ((StopTime[j]>StopEpoch)||(StopTime[j]<=StartEpoch))
                                {
                                    QMessageBox InvalidStartWarning;
                                    InvalidStartWarning.setText("Invalid Stop Time");
                                    InvalidStartWarning.exec();
                                    return;
                                }
                                else
                                {
                                    int i=0;
                                    while(((arc->trajectorySampleTime(i))/*+MJDoffset*/)<StopTime[j])
                                    {
                                        i=i+1;
                                    }
                                    countStop[j]=i;
                                }
                            }

                            int inumber=0;

                            int CovIndex[3]; //line of Coverage Report for each parameter
                            CovIndex[0]=CovIndex[1]=CovIndex[2]=1; //0-Azimuth, 1-Elevation, 2-Range
                            int Comm1Index[6]; //line of Comminucation Report 1 for each parameter
                            Comm1Index[0]=Comm1Index[1]=Comm1Index[2]=Comm1Index[3]=Comm1Index[4]=Comm1Index[5]=1;
                            int Comm2Index[6];
                            Comm2Index[0]=Comm2Index[1]=Comm2Index[2]=Comm2Index[3]=Comm2Index[4]=Comm2Index[5]=1;
                            int Comm3Index[5];
                            Comm3Index[0]=Comm3Index[1]=Comm3Index[2]=Comm3Index[3]=Comm3Index[4]=1;
                            QString CoverageLine; //each line of the cov report
                            QStringList LineOfCoverageReport; //list of all the lines
                            QString Communication1Line;
                            QString Communication2Line;
                            QString Communication3Line;
                            QStringList LineOfComm1Report;
                            QStringList LineOfComm2Report;
                            QStringList LineOfComm3Report;
                            if(ReadCoverage==true)
                            {
                                CoverageAnalysis covAna=CoverageAnalysis(m_propagatedScenario, indSC, indGS, indMissionArc);
                                covAna.reportAER();
                                ////qDebug()<<"Ricky function2";

                                //file.open(QIODevice::WriteOnly);
                                //QTextStream cov(&file);
                                QFile Coverage("reportCov1.txt");

                                if(Coverage.open(QIODevice::ReadOnly ))
                                {
                                    QTextStream text(&Coverage);
                                    while(!text.atEnd())
                                    {
                                        CoverageLine=text.readLine();
                                        LineOfCoverageReport.append(CoverageLine);
                                    }
                                    Coverage.close();
                                }
                            }
                            if(ReadCommunication1==true)
                            {
                                CommAnalysis commAnalysis=CommAnalysis(Transmitter, Receiver, Environment, m_propagatedScenario, indSC, indGS, indMissionArc,TxParentType,RxParentType);
                                commAnalysis.CommReports();

                                QFile Communication1("reportComm1.txt");

                                if(Communication1.open(QIODevice::ReadOnly ))
                                {
                                    QTextStream Comm1(&Communication1);
                                    while(!Comm1.atEnd())
                                    {
                                        Communication1Line=Comm1.readLine();
                                        LineOfComm1Report.append(Communication1Line);
                                    }
                                    Communication1.close();
                                }
                            }
                            if(ReadCommunication2==true)
                            {
                                CommAnalysis commAnalysis=CommAnalysis(Transmitter, Receiver, Environment, m_propagatedScenario, indSC, indGS, indMissionArc,TxParentType,RxParentType);
                                commAnalysis.CommReports();
                                QFile Communication2("reportComm2.txt");

                                if(Communication2.open(QIODevice::ReadOnly ))
                                {
                                    QTextStream Comm2(&Communication2);
                                    while(!Comm2.atEnd())
                                    {
                                        Communication2Line=Comm2.readLine();
                                        LineOfComm2Report.append(Communication2Line);
                                    }
                                    Communication2.close();
                                }
                            }
                            if(ReadCommunication3==true)
                            {
                                CommAnalysis commAnalysis=CommAnalysis(Transmitter, Receiver, Environment, m_propagatedScenario, indSC, indGS, indMissionArc,TxParentType,RxParentType);
                                commAnalysis.CommReports();
                                QFile Communication3("reportComm3.txt");

                                if(Communication3.open(QIODevice::ReadOnly ))
                                {
                                    QTextStream Comm3(&Communication3);
                                    while(!Comm3.atEnd())
                                    {
                                        Communication3Line=Comm3.readLine();
                                        LineOfComm3Report.append(Communication3Line);
                                    }
                                    Communication3.close();
                                }
                            }
                            for (int k=0;k<selectedTimes.size();k++)
                            {
                                //qDebug()<<"le as opçoes de report-inicio";
                                QList<QTreeWidgetItem*>SelTime=treeWidgetTimeSpecifications->selectedItems();
                                QString Start=SelTime[k]->text(0);
                                QString Stop=SelTime[k]->text(1);

                                stream<<"#######Beginning of time"<<" "<<(k+1)<<"######"<<"\r\n"<<"######"<<"Start Time"<<" "<<Start<<"\t"<<"Stop Time"<<" "<<Stop<<"######"<<"\r\n"<<"MJD"<<"\r\t";

                                //printing the labels of the displayed parameters
                                for(int i=0;i<treeWidgetShowInReport->topLevelItemCount();i++)
                                {
                                    QTreeWidgetItem*parameter=treeWidgetShowInReport->topLevelItem(i);
                                    QString name=parameter->text(0);
                                    //QString parent=parameter->parent()->text(0);
                                    stream<<"\t"<<name<<"\t";
                                    if(i==((treeWidgetShowInReport->topLevelItemCount())-1))
                                    {
                                        stream<<"\r\n";
                                    }
                                    //qDebug()<<"escreve titulos";

                                }

                                inumber=countStop[k]-countStart[k];

                                double MJDdate[inumber];
                                double JulianDate[inumber];
                                QDateTime TimeDateVector[inumber];
                                double ElapsedTime[inumber];

                                for(int j=countStart[k];j<=countStop[k];j++)
                                {
                                    int index=j-countStart[k];
                                    MJDdate[index]=arc->trajectorySampleTime(j);
                                    //stream.setRealNumberPrecision(16);
                                    stream<<MJDdate[index]<<"\t";

                                    for(int i=0;i<treeWidgetShowInReport->topLevelItemCount();i++)
                                    {
                                        QTreeWidgetItem*parameter=treeWidgetShowInReport->topLevelItem(i);
                                        QString name=parameter->text(0);

                                        if((name=="x position")||(name=="y position")||(name=="z position")||(name=="x velocity")||(name=="y velocity")||(name=="z velocity"))
                                        {

                                            QWidget*Box=treeWidgetShowInReport->itemWidget(parameter,1);
                                            QComboBox*ComboBox=dynamic_cast <QComboBox*>(Box);
                                            QString Coordinate=ComboBox->currentText();

                                            QWidget*Box1=treeWidgetShowInReport->itemWidget(parameter,2);
                                            QComboBox*ComboBoxUnit=dynamic_cast <QComboBox*>(Box1);
                                            QString Units=ComboBoxUnit->currentText();

                                            sta::StateVector Vector[inumber];

                                            Vector[index]=arc->trajectorySample(j);

                                            if(name=="x position")

                                            {
                                                if(Coordinate=="Fixed")
                                                {

                                                    sta::StateVector ModifVector[inumber];
                                                    sta::CoordinateSystem Fixed("PLANETO FIXED");
                                                    sta::CoordinateSystem EME2000("INERTIAL J2000");
                                                    ModifVector[index]=CoordinateSystem::convert(Vector[index],
                                                                                                 MJDdate[index],
                                                                                                 STA_SOLAR_SYSTEM->lookup("Earth"),
                                                                                                 EME2000,
                                                                                                 STA_SOLAR_SYSTEM->lookup("Earth"),
                                                                                                 Fixed);
                                                    stream<<"\t"<<ModifVector[index].position.x()<<"\t";


                                                }
                                                if(Coordinate=="Ecliptic J2000")
                                                {

                                                    sta::StateVector ModifVector[inumber];
                                                    sta::CoordinateSystem Ecliptic("ECLIPTIC");
                                                    sta::CoordinateSystem EME2000("INERTIAL J2000");
                                                    ModifVector[index]=CoordinateSystem::convert(Vector[index],
                                                                                                 MJDdate[index],
                                                                                                 STA_SOLAR_SYSTEM->lookup("Earth"),
                                                                                                 EME2000,
                                                                                                 STA_SOLAR_SYSTEM->lookup("Earth"),
                                                                                                 Ecliptic);
                                                    stream<<"\t"<<ModifVector[index].position.x()<<"\t";

                                                }
                                                if(Coordinate=="EME J2000")
                                                {

                                                    ////qDebug()<<"Receiver no state vector x "<<index<<Receiver->Budget()->FrequencyBand();
                                                    ////qDebug()<<"linha seguinte ao ouro debug,funciona?";
                                                    sta::StateVector ModifVector[inumber];
                                                    sta::CoordinateSystem EME2000("INERTIAL J2000");
                                                    ////qDebug()<<"define os vectores";
                                                    ModifVector[index]=CoordinateSystem::convert(Vector[index],
                                                                                                 MJDdate[index],
                                                                                                 STA_SOLAR_SYSTEM->lookup("Earth"),
                                                                                                 EME2000,
                                                                                                 STA_SOLAR_SYSTEM->lookup("Earth"),
                                                                                                 EME2000);
                                                    ////qDebug()<<"a transformaçao foi efectuada com sucesso";
                                                    stream<<"\t"<<ModifVector[index].position.x()<<"\t";

                                                    ////qDebug()<<"escreveu no ficheiro";
                                                }
                                                if(Coordinate=="EME B1950")
                                                {

                                                    sta::StateVector ModifVector[inumber];
                                                    sta::CoordinateSystem B1950("INERTIAL B1950");
                                                    sta::CoordinateSystem EME2000("INERTIAL J2000");
                                                    ModifVector[index]=CoordinateSystem::convert(Vector[index],
                                                                                                 MJDdate[index],
                                                                                                 STA_SOLAR_SYSTEM->lookup("Earth"),
                                                                                                 EME2000,
                                                                                                 STA_SOLAR_SYSTEM->lookup("Earth"),
                                                                                                 B1950);
                                                    stream<<"\t"<<ModifVector[index].position.x()<<"\t";

                                                }
                                            }

                                            if(name=="y position")
                                            {
                                                if(Coordinate=="Fixed")
                                                {

                                                    sta::StateVector ModifVector[inumber];
                                                    sta::CoordinateSystem Fixed("PLANETO FIXED");
                                                    sta::CoordinateSystem EME2000("INERTIAL J2000");
                                                    ModifVector[index]=CoordinateSystem::convert(Vector[index],
                                                                                                 MJDdate[index],
                                                                                                 STA_SOLAR_SYSTEM->lookup("Earth"),
                                                                                                 EME2000,
                                                                                                 STA_SOLAR_SYSTEM->lookup("Earth"),
                                                                                                 Fixed);
                                                    stream<<"\t"<<ModifVector[index].position.y()<<"\t";

                                                }
                                                if(Coordinate=="Ecliptic J2000")
                                                {

                                                    sta::StateVector ModifVector[inumber];
                                                    sta::CoordinateSystem Ecliptic("ECLIPTIC");
                                                    sta::CoordinateSystem EME2000("INERTIAL J2000");
                                                    ModifVector[index]=CoordinateSystem::convert(Vector[index],
                                                                                                 MJDdate[index],
                                                                                                 STA_SOLAR_SYSTEM->lookup("Earth"),
                                                                                                 EME2000,
                                                                                                 STA_SOLAR_SYSTEM->lookup("Earth"),
                                                                                                 Ecliptic);
                                                    stream<<"\t"<<ModifVector[index].position.y()<<"\t";

                                                }
                                                if(Coordinate=="EME J2000")
                                                {
                                                    sta::StateVector ModifVector[inumber];
                                                    sta::CoordinateSystem EME2000("INERTIAL J2000");

                                                    ModifVector[index]=CoordinateSystem::convert(Vector[index],
                                                                                                 MJDdate[index],
                                                                                                 STA_SOLAR_SYSTEM->lookup("Earth"),
                                                                                                 EME2000,
                                                                                                 STA_SOLAR_SYSTEM->lookup("Earth"),
                                                                                                 EME2000);
                                                    stream<<"\t"<<ModifVector[index].position.y()<<"\t";

                                                }
                                                if(Coordinate=="EME B1950")
                                                {

                                                    sta::StateVector ModifVector[inumber];
                                                    sta::CoordinateSystem B1950("INERTIAL B1950");
                                                    sta::CoordinateSystem EME2000("INERTIAL J2000");
                                                    ModifVector[index]=CoordinateSystem::convert(Vector[index],
                                                                                                 MJDdate[index],
                                                                                                 STA_SOLAR_SYSTEM->lookup("Earth"),
                                                                                                 EME2000,
                                                                                                 STA_SOLAR_SYSTEM->lookup("Earth"),
                                                                                                 B1950);
                                                    stream<<"\t"<<ModifVector[index].position.y()<<"\t";

                                                }
                                            }

                                            if(name=="z position")
                                            {

                                                if(Coordinate=="Fixed")
                                                {

                                                    sta::StateVector ModifVector[inumber];
                                                    sta::CoordinateSystem Fixed("PLANETO FIXED");
                                                    sta::CoordinateSystem EME2000("INERTIAL J2000");
                                                    ModifVector[index]=CoordinateSystem::convert(Vector[index],
                                                                                                 MJDdate[index],
                                                                                                 STA_SOLAR_SYSTEM->lookup("Earth"),
                                                                                                 EME2000,
                                                                                                 STA_SOLAR_SYSTEM->lookup("Earth"),
                                                                                                 Fixed);

                                                    stream<<"\t"<<ModifVector[index].position.z()<<"\t";

                                                }
                                                if(Coordinate=="Ecliptic J2000")
                                                {

                                                    sta::StateVector ModifVector[inumber];
                                                    sta::CoordinateSystem Ecliptic("ECLIPTIC");
                                                    sta::CoordinateSystem EME2000("INERTIAL J2000");
                                                    ModifVector[index]=CoordinateSystem::convert(Vector[index],
                                                                                                 MJDdate[index],
                                                                                                 STA_SOLAR_SYSTEM->lookup("Earth"),
                                                                                                 EME2000,
                                                                                                 STA_SOLAR_SYSTEM->lookup("Earth"),
                                                                                                 Ecliptic);
                                                    stream<<"\t"<<ModifVector[index].position.z()<<"\t";

                                                }
                                                if(Coordinate=="EME J2000")
                                                {
                                                    sta::StateVector ModifVector[inumber];
                                                    sta::CoordinateSystem EME2000("INERTIAL J2000");

                                                    ModifVector[index]=CoordinateSystem::convert(Vector[index],
                                                                                                 MJDdate[index],
                                                                                                 STA_SOLAR_SYSTEM->lookup("Earth"),
                                                                                                 EME2000,
                                                                                                 STA_SOLAR_SYSTEM->lookup("Earth"),
                                                                                                 EME2000);
                                                    stream<<"\t"<<ModifVector[index].position.z()<<"\t";

                                                }
                                                if(Coordinate=="EME B1950")
                                                {

                                                    sta::StateVector ModifVector[inumber];
                                                    sta::CoordinateSystem B1950("INERTIAL B1950");
                                                    sta::CoordinateSystem EME2000("INERTIAL J2000");
                                                    ModifVector[index]=CoordinateSystem::convert(Vector[index],
                                                                                                 MJDdate[index],
                                                                                                 STA_SOLAR_SYSTEM->lookup("Earth"),
                                                                                                 EME2000,
                                                                                                 STA_SOLAR_SYSTEM->lookup("Earth"),
                                                                                                 B1950);
                                                    stream<<"\t"<<ModifVector[index].position.z()<<"\t";

                                                }

                                            }

                                            if(name=="x velocity")
                                            {
                                                if(Coordinate=="Fixed")
                                                {

                                                    sta::StateVector ModifVector[inumber];
                                                    sta::CoordinateSystem Fixed("PLANETO FIXED");
                                                    sta::CoordinateSystem EME2000("INERTIAL J2000");
                                                    ModifVector[index]=CoordinateSystem::convert(Vector[index],
                                                                                                 MJDdate[index],
                                                                                                 STA_SOLAR_SYSTEM->lookup("Earth"),
                                                                                                 EME2000,
                                                                                                 STA_SOLAR_SYSTEM->lookup("Earth"),
                                                                                                 Fixed);
                                                    stream<<"\t"<<ModifVector[index].velocity.x()<<"\t";



                                                }
                                                if(Coordinate=="Ecliptic J2000")
                                                {

                                                    sta::StateVector ModifVector[inumber];
                                                    sta::CoordinateSystem Ecliptic("ECLIPTIC");
                                                    sta::CoordinateSystem EME2000("INERTIAL J2000");
                                                    ModifVector[index]=CoordinateSystem::convert(Vector[index],
                                                                                                 MJDdate[index],
                                                                                                 STA_SOLAR_SYSTEM->lookup("Earth"),
                                                                                                 EME2000,
                                                                                                 STA_SOLAR_SYSTEM->lookup("Earth"),
                                                                                                 Ecliptic);
                                                    stream<<"\t"<<ModifVector[index].velocity.x()<<"\t";



                                                }
                                                if(Coordinate=="EME J2000")
                                                {
                                                    sta::StateVector ModifVector[inumber];
                                                    sta::CoordinateSystem EME2000("INERTIAL J2000");

                                                    ModifVector[index]=CoordinateSystem::convert(Vector[index],
                                                                                                 MJDdate[index],
                                                                                                 STA_SOLAR_SYSTEM->lookup("Earth"),
                                                                                                 EME2000,
                                                                                                 STA_SOLAR_SYSTEM->lookup("Earth"),
                                                                                                 EME2000);
                                                    stream<<"\t"<<ModifVector[index].velocity.x()<<"\t";

                                                }
                                                if(Coordinate=="EME B1950")
                                                {

                                                    sta::StateVector ModifVector[inumber];
                                                    sta::CoordinateSystem B1950("INERTIAL B1950");
                                                    sta::CoordinateSystem EME2000("INERTIAL J2000");
                                                    ModifVector[index]=CoordinateSystem::convert(Vector[index],
                                                                                                 MJDdate[index],
                                                                                                 STA_SOLAR_SYSTEM->lookup("Earth"),
                                                                                                 EME2000,
                                                                                                 STA_SOLAR_SYSTEM->lookup("Earth"),
                                                                                                 B1950);
                                                    stream<<"\t"<<ModifVector[index].velocity.x()<<"\t";



                                                }
                                            }
                                            if(name=="y velocity")
                                            {
                                                if(Coordinate=="Fixed")
                                                {

                                                    sta::StateVector ModifVector[inumber];
                                                    sta::CoordinateSystem Fixed("PLANETO FIXED");
                                                    sta::CoordinateSystem EME2000("INERTIAL J2000");
                                                    ModifVector[index]=CoordinateSystem::convert(Vector[index],
                                                                                                 MJDdate[index],
                                                                                                 STA_SOLAR_SYSTEM->lookup("Earth"),
                                                                                                 EME2000,
                                                                                                 STA_SOLAR_SYSTEM->lookup("Earth"),
                                                                                                 Fixed);
                                                    stream<<"\t"<<ModifVector[index].velocity.y()<<"\t";



                                                }
                                                if(Coordinate=="Ecliptic J2000")
                                                {

                                                    sta::StateVector ModifVector[inumber];
                                                    sta::CoordinateSystem Ecliptic("ECLIPTIC");
                                                    sta::CoordinateSystem EME2000("INERTIAL J2000");
                                                    ModifVector[index]=CoordinateSystem::convert(Vector[index],
                                                                                                 MJDdate[index],
                                                                                                 STA_SOLAR_SYSTEM->lookup("Earth"),
                                                                                                 EME2000,
                                                                                                 STA_SOLAR_SYSTEM->lookup("Earth"),
                                                                                                 Ecliptic);
                                                    stream<<"\t"<<ModifVector[index].velocity.y()<<"\t";



                                                }
                                                if(Coordinate=="EME J2000")
                                                {
                                                    sta::StateVector ModifVector[inumber];
                                                    sta::CoordinateSystem EME2000("INERTIAL J2000");

                                                    ModifVector[index]=CoordinateSystem::convert(Vector[index],
                                                                                                 MJDdate[index],
                                                                                                 STA_SOLAR_SYSTEM->lookup("Earth"),
                                                                                                 EME2000,
                                                                                                 STA_SOLAR_SYSTEM->lookup("Earth"),
                                                                                                 EME2000);
                                                    stream<<"\t"<<ModifVector[index].velocity.y()<<"\t";

                                                }
                                                if(Coordinate=="EME B1950")
                                                {

                                                    sta::StateVector ModifVector[inumber];
                                                    sta::CoordinateSystem B1950("INERTIAL B1950");
                                                    sta::CoordinateSystem EME2000("INERTIAL J2000");
                                                    ModifVector[index]=CoordinateSystem::convert(Vector[index],
                                                                                                 MJDdate[index],
                                                                                                 STA_SOLAR_SYSTEM->lookup("Earth"),
                                                                                                 EME2000,
                                                                                                 STA_SOLAR_SYSTEM->lookup("Earth"),
                                                                                                 B1950);
                                                    stream<<"\t"<<ModifVector[index].velocity.z()<<"\t";



                                                }
                                            }
                                            if(name=="z velocity")
                                            {
                                                if(Coordinate=="Fixed")
                                                {

                                                    sta::StateVector ModifVector[inumber];
                                                    sta::CoordinateSystem Fixed("PLANETO FIXED");
                                                    sta::CoordinateSystem EME2000("INERTIAL J2000");
                                                    ModifVector[index]=CoordinateSystem::convert(Vector[index],
                                                                                                 MJDdate[index],
                                                                                                 STA_SOLAR_SYSTEM->lookup("Earth"),
                                                                                                 EME2000,
                                                                                                 STA_SOLAR_SYSTEM->lookup("Earth"),
                                                                                                 Fixed);
                                                    stream<<"\t"<<ModifVector[index].velocity.z()<<"\t";

                                                }
                                                if(Coordinate=="Ecliptic J2000")
                                                {

                                                    sta::StateVector ModifVector[inumber];
                                                    sta::CoordinateSystem Ecliptic("ECLIPTIC");
                                                    sta::CoordinateSystem EME2000("INERTIAL J2000");
                                                    ModifVector[index]=CoordinateSystem::convert(Vector[index],
                                                                                                 MJDdate[index],
                                                                                                 STA_SOLAR_SYSTEM->lookup("Earth"),
                                                                                                 EME2000,
                                                                                                 STA_SOLAR_SYSTEM->lookup("Earth"),
                                                                                                 Ecliptic);
                                                    stream<<"\t"<<ModifVector[index].velocity.z()<<"\t";

                                                    if(Coordinate=="EME J2000")
                                                    {
                                                        sta::StateVector ModifVector[inumber];
                                                        sta::CoordinateSystem EME2000("INERTIAL J2000");

                                                        ModifVector[index]=CoordinateSystem::convert(Vector[index],
                                                                                                     MJDdate[index],
                                                                                                     STA_SOLAR_SYSTEM->lookup("Earth"),
                                                                                                     EME2000,
                                                                                                     STA_SOLAR_SYSTEM->lookup("Earth"),
                                                                                                     EME2000);
                                                        stream<<"\t"<<ModifVector[index].velocity.z()<<"\t";

                                                    }
                                                    if(Coordinate=="EME B1950")
                                                    {

                                                        sta::StateVector ModifVector[inumber];
                                                        sta::CoordinateSystem B1950("INERTIAL B1950");
                                                        sta::CoordinateSystem EME2000("INERTIAL J2000");
                                                        ModifVector[index]=CoordinateSystem::convert(Vector[index],
                                                                                                     MJDdate[index],
                                                                                                     STA_SOLAR_SYSTEM->lookup("Earth"),
                                                                                                     EME2000,
                                                                                                     STA_SOLAR_SYSTEM->lookup("Earth"),
                                                                                                     B1950);
                                                        stream<<"\t"<<ModifVector[index].velocity.z()<<"\t";



                                                    }
                                                }
                                            }
                                        }
                                        if(name=="Time")
                                        {
                                            QWidget*Box=treeWidgetShowInReport->itemWidget(parameter,1);
                                            QComboBox*ComboBox=dynamic_cast <QComboBox*>(Box);
                                            QString TimeCoordinate=ComboBox->currentText();

                                            //Options of Time
                                            if(TimeCoordinate=="MJD")
                                            {
                                                stream<<MJDdate[index]<<"\t";

                                            }

                                            if(TimeCoordinate=="Julian Date")
                                            {
                                                JulianDate[index]=sta::MjdToJd(MJDdate[index]);
                                                stream<<JulianDate[index]<<"\t";
                                            }

                                            if(TimeCoordinate=="Gregorian UTC")
                                            {
                                                JulianDate[index]=sta::MjdToJd(MJDdate[index])+0.00001;
                                                TimeDateVector[index]=sta::JdToCalendar(JulianDate[index]);
                                                stream<<TimeDateVector[index].date().day()<<"/"<<TimeDateVector[index].date().month()<<"/"<<TimeDateVector[index].date().year()<<" "<<TimeDateVector[index].time().hour()<<":"<<TimeDateVector[index].time().minute()<<":"<<TimeDateVector[index].time().second();

                                            }
                                            if(TimeCoordinate=="Mission Elapsed Time")
                                            {
                                            }

                                            if(TimeCoordinate=="Days,hours,minutes,and seconds from epoch")
                                            {
                                                QWidget*Box=treeWidgetShowInReport->itemWidget(parameter,2);
                                                QComboBox*ComboBox=dynamic_cast <QComboBox*>(Box);
                                                QString TimeUnits=ComboBox->currentText();

                                                double ElapsedTime=MjdToFromEpoch(StartEpoch,MJDdate[index],TimeUnits);

                                                if(TimeUnits=="Seconds")
                                                {
                                                    stream<<"Seconds from Epoch"<<" "<<ElapsedTime;
                                                }
                                                if(TimeUnits=="Minutes")
                                                {
                                                    stream<<"Minutes from Epoch"<<" "<<ElapsedTime;
                                                }
                                                if(TimeUnits=="Hours")
                                                {
                                                    stream<<"Hours from Epoch"<<" "<<ElapsedTime;
                                                }
                                                if(TimeUnits=="Days")
                                                {
                                                    stream<<"Days from Epoch"<<" "<<ElapsedTime;
                                                }
                                            }
                                        }
                                        if(name=="Azimuth")
                                        {

                                            if(CovIndex[0]<LineOfCoverageReport.size())
                                            {
                                                QString Line=LineOfCoverageReport.at(CovIndex[0]);

                                                double TimeCovReport=(Line.section("\t",0,0)).toDouble();
                                                double Azimuth=(Line.section("\t",3,3)).toDouble();

                                                if(abs(MJDdate[index]-TimeCovReport)<10e-6)
                                                {
                                                    stream<<Azimuth<<"\t";
                                                    CovIndex[0]++;
                                                }

                                                else
                                                {
                                                    stream<<"No visibility";
                                                }

                                            }
                                            else
                                            {
                                                stream<<"No visibility";
                                            }
                                        }
                                        if(name=="Elevation")
                                        {

                                            if(CovIndex[1]<LineOfCoverageReport.length())
                                            {
                                                QString Line=LineOfCoverageReport.at(CovIndex[1]);

                                                double TimeCovReport=(Line.section("\t",0,0)).toDouble();
                                                double Elevation=(Line.section("\t",2,2)).toDouble();

                                                if(abs(MJDdate[index]-TimeCovReport)<10e-6)
                                                {
                                                    stream<<Elevation<<"\t";
                                                    CovIndex[1]++;
                                                }

                                                else
                                                {
                                                    stream<<"No visibility";
                                                }

                                            }
                                            else
                                            {
                                                stream<<"No visibility";
                                            }
                                        }
                                        if(name=="Range")
                                        {
                                            if(CovIndex[2]<LineOfCoverageReport.length())
                                            {
                                                QString Line=LineOfCoverageReport.at(CovIndex[2]);

                                                double TimeCovReport=(Line.section("\t",0,0)).toDouble();
                                                double Range=(Line.section("\t",1,1)).toDouble();

                                                if(abs(MJDdate[index]-TimeCovReport)<10e-6)
                                                {
                                                    stream<<Range<<"\t";
                                                    CovIndex[2]++;
                                                }

                                                else
                                                {
                                                    stream<<"No visibility";
                                                }

                                                //qDebug()<<"CovIndex"<<CovIndex[2];
                                                //qDebug()<<"index"<<index;
                                            }
                                            else
                                            {
                                                stream<<"No visibility";
                                            }
                                        }
                                        if(name=="Equivalent Isotropical Radiated Power")
                                        {
                                            ////qDebug()<<LineOfCommReport.length()<<Comm1Index[0]<<"blabla";
                                            if(Comm1Index[0]<LineOfComm1Report.length())
                                            {
                                                QString Line=LineOfComm1Report.at(Comm1Index[0]);

                                                double TimeCommReport=(Line.section("\t",0,0)).toDouble();
                                                double EIRP=(Line.section("\t",1,1)).toDouble();

                                                if(abs(MJDdate[index]-TimeCommReport)<10e-6)
                                                {
                                                    stream<<EIRP<<"\t";
                                                    Comm1Index[0]++;
                                                }

                                                else
                                                {
                                                    stream<<"No visibility";
                                                }

                                            }
                                            else
                                            {
                                                stream<<"No visibility";
                                            }
                                        }
                                        if(name=="Received Frequency")
                                        {

                                            if(Comm1Index[1]<LineOfComm1Report.length())
                                            {
                                                QString Line=LineOfComm1Report.at(Comm1Index[1]);

                                                double TimeCommReport=(Line.section("\t",0,0)).toDouble();
                                                double RcvFreq=(Line.section("\t",2,2)).toDouble();

                                                if(abs(MJDdate[index]-TimeCommReport)<10e-6)
                                                {
                                                    stream<<RcvFreq<<"\t";
                                                    Comm1Index[1]++;
                                                }

                                                else
                                                {
                                                    stream<<"No visibility";
                                                }

                                            }
                                            else
                                            {
                                                stream<<"No visibility";
                                            }
                                        }
                                        if(name=="Doppler Shift")
                                        {

                                            if(Comm1Index[2]<LineOfComm1Report.length())
                                            {
                                                QString Line=LineOfComm1Report.at(Comm1Index[2]);

                                                double TimeCommReport=(Line.section("\t",0,0)).toDouble();
                                                double RcvFreq=(Line.section("\t",3,3)).toDouble();

                                                if(abs(MJDdate[index]-TimeCommReport)<10e-6)
                                                {
                                                    stream<<RcvFreq<<"\t";
                                                    Comm1Index[2]++;
                                                }

                                                else
                                                {
                                                    stream<<"No visibility";
                                                }

                                            }
                                            else
                                            {
                                                stream<<"No visibility";
                                            }
                                        }
                                        if(name=="Received Power")
                                        {

                                            if(Comm1Index[3]<LineOfComm1Report.length())
                                            {
                                                QString Line=LineOfComm1Report.at(Comm1Index[3]);

                                                double TimeCommReport=(Line.section("\t",0,0)).toDouble();
                                                double RcvFreq=(Line.section("\t",4,4)).toDouble();

                                                if(abs(MJDdate[index]-TimeCommReport)<10e-6)
                                                {
                                                    stream<<RcvFreq<<"\t";
                                                    Comm1Index[3]++;
                                                }

                                                else
                                                {
                                                    stream<<"No visibility";
                                                }

                                            }
                                            else
                                            {
                                                stream<<"No visibility";
                                            }
                                        }
                                        if(name=="Flux Density")
                                        {

                                            if(Comm1Index[4]<LineOfComm1Report.length())
                                            {
                                                QString Line=LineOfComm1Report.at(Comm1Index[4]);

                                                double TimeCommReport=(Line.section("\t",0,0)).toDouble();
                                                double FluxDensity=(Line.section("\t",5,5)).toDouble();
                                                if(abs(MJDdate[index]-TimeCommReport)<10e-6)
                                                {
                                                    stream<<FluxDensity<<"\t";
                                                    Comm1Index[4]++;
                                                }

                                                else
                                                {
                                                    stream<<"No visibility";
                                                }

                                            }
                                            else
                                            {
                                                stream<<"No visibility";
                                            }
                                        }
                                        if(name=="Overlap Bandwidth Factor")
                                        {

                                            if(Comm1Index[5]<LineOfComm1Report.length())
                                            {
                                                QString Line=LineOfComm1Report.at(Comm1Index[5]);
                                                double TimeCommReport=(Line.section("\t",0,0)).toDouble();
                                                double OvBWF=(Line.section("\t",6,6)).toDouble();

                                                if(abs(MJDdate[index]-TimeCommReport)<10e-6)
                                                {
                                                    stream<<OvBWF<<"\t";
                                                    Comm1Index[5]++;
                                                }

                                                else
                                                {
                                                    stream<<"No visibility"<<"\t";
                                                }
                                            }
                                            else
                                            {
                                                stream<<"No visibility"<<"\t";
                                            }
                                        }
                                        if(name=="Free Space Loss")
                                        {

                                            if(Comm2Index[0]<LineOfComm2Report.length())
                                            {
                                                QString Line=LineOfComm2Report.at(Comm2Index[0]);

                                                double TimeCommReport=(Line.section("\t",0,0)).toDouble();

                                                double FSL=(Line.section("\t",1,1)).toDouble();


                                                if(abs(MJDdate[index]-TimeCommReport)<10e-6)
                                                {
                                                    stream<<FSL<<"\t";
                                                    Comm2Index[0]++;
                                                }

                                                else
                                                {
                                                    stream<<"No visibility"<<"\t";
                                                }

                                            }
                                            else
                                            {
                                                stream<<"No visibility"<<"\t";
                                            }
                                        }
                                        if(name=="Oxygen Loss")
                                        {
                                            if(Comm2Index[1]<LineOfComm2Report.length())
                                            {
                                                QString Line=LineOfComm2Report.at(Comm2Index[1]);
                                                double TimeCommReport=(Line.section("\t",0,0)).toDouble();
                                                double OxLoss=(Line.section("\t",2,2)).toDouble();

                                                if(abs(MJDdate[index]-TimeCommReport)<10e-6)
                                                {
                                                    stream<<OxLoss<<"\t";
                                                    Comm2Index[1]++;
                                                }

                                                else
                                                {
                                                    stream<<"No visibility"<<"\t";
                                                }
                                            }
                                            else
                                            {
                                                stream<<"No visibility"<<"\t";
                                            }
                                        }
                                        if(name=="Water Vapour Loss")
                                        {
                                            if(Comm2Index[2]<LineOfComm2Report.length())
                                            {
                                                QString Line=LineOfComm2Report.at(Comm2Index[2]);
                                                double TimeCommReport=(Line.section("\t",0,0)).toDouble();
                                                double WVLoss=(Line.section("\t",3,3)).toDouble();

                                                if(abs(MJDdate[index]-TimeCommReport)<10e-6)
                                                {
                                                    stream<<WVLoss<<"\t";
                                                    Comm2Index[2]++;
                                                }

                                                else
                                                {
                                                    stream<<"No visibility"<<"\t";
                                                }
                                            }
                                            else
                                            {
                                                stream<<"No visibility"<<"\t";
                                            }
                                        }
                                        if(name=="Rain Loss")
                                        {
                                            if(Comm2Index[3]<LineOfComm2Report.length())
                                            {
                                                QString Line=LineOfComm2Report.at(Comm2Index[3]);
                                                double TimeCommReport=(Line.section("\t",0,0)).toDouble();
                                                double RainLoss=(Line.section("\t",4,4)).toDouble();

                                                if(abs(MJDdate[index]-TimeCommReport)<10e-6)
                                                {
                                                    stream<<RainLoss<<"\t";
                                                    Comm2Index[3]++;
                                                }

                                                else
                                                {
                                                    stream<<"No visibility"<<"\t";
                                                }
                                            }
                                            else
                                            {
                                                stream<<"No visibility"<<"\t";
                                            }
                                        }
                                        if(name=="Atmospheric Loss")
                                        {
                                            if(Comm2Index[4]<LineOfComm2Report.length())
                                            {
                                                QString Line=LineOfComm2Report.at(Comm2Index[4]);
                                                double TimeCommReport=(Line.section("\t",0,0)).toDouble();
                                                double AtmLoss=(Line.section("\t",5,5)).toDouble();

                                                if(abs(MJDdate[index]-TimeCommReport)<10e-6)
                                                {
                                                    stream<<AtmLoss<<"\t";
                                                    Comm2Index[4]++;
                                                }

                                                else
                                                {
                                                    stream<<"No visibility"<<"\t";
                                                }
                                            }
                                            else
                                            {
                                                stream<<"No visibility"<<"\t";
                                            }
                                        }
                                        if(name=="Propagation Loss")
                                        {
                                            if(Comm2Index[5]<LineOfComm2Report.length())
                                            {
                                                QString Line=LineOfComm2Report.at(Comm2Index[5]);
                                                double TimeCommReport=(Line.section("\t",0,0)).toDouble();
                                                double PropLoss=(Line.section("\t",6,6)).toDouble();

                                                if(abs(MJDdate[index]-TimeCommReport)<10e-6)
                                                {
                                                    stream<<PropLoss<<"\t";
                                                    Comm2Index[5]++;
                                                }

                                                else
                                                {
                                                    stream<<"No visibility"<<"\t";
                                                }
                                            }
                                            else
                                            {
                                                stream<<"No visibility"<<"\t";
                                            }
                                        }
                                        if (name=="G/T")
                                        {
                                            if(Comm3Index[0]<LineOfComm3Report.length())
                                            {
                                                QString Line=LineOfComm3Report.at(Comm3Index[0]);
                                                double TimeCommReport=(Line.section("\t",0,0)).toDouble();
                                                double GT=(Line.section("\t",1,1)).toDouble();

                                                if(abs(MJDdate[index]-TimeCommReport)<10e-6)
                                                {
                                                    stream<<GT<<"\t";
                                                    Comm3Index[0]++;
                                                }

                                                else
                                                {
                                                    stream<<"No visibility"<<"\t";
                                                }
                                            }
                                            else
                                            {
                                                stream<<"No visibility"<<"\t";
                                            }
                                        }
                                        if(name=="C/No")
                                        {
                                            if(Comm3Index[1]<LineOfComm3Report.length())
                                            {
                                                QString Line=LineOfComm3Report.at(Comm3Index[1]);
                                                double TimeCommReport=(Line.section("\t",0,0)).toDouble();
                                                double CNo=(Line.section("\t",2,2)).toDouble();

                                                if(abs(MJDdate[index]-TimeCommReport)<10e-6)
                                                {
                                                    stream<<CNo<<"\t";
                                                    Comm3Index[1]++;
                                                }

                                                else
                                                {
                                                    stream<<"No visibility"<<"\t";
                                                }
                                            }
                                            else
                                            {
                                                stream<<"No visibility"<<"\t";
                                            }
                                        }
                                        if(name=="C/N")
                                        {
                                            if(Comm3Index[2]<LineOfComm3Report.length())
                                            {
                                                QString Line=LineOfComm3Report.at(Comm3Index[2]);
                                                double TimeCommReport=(Line.section("\t",0,0)).toDouble();
                                                double CN=(Line.section("\t",3,3)).toDouble();

                                                if(abs(MJDdate[index]-TimeCommReport)<10e-6)
                                                {
                                                    stream<<CN<<"\t";
                                                    Comm3Index[2]++;
                                                }

                                                else
                                                {
                                                    stream<<"No visibility"<<"\t";
                                                }
                                            }
                                            else
                                            {
                                                stream<<"No visibility"<<"\t";
                                            }
                                        }
                                        if(name=="Eb/No")
                                        {
                                            if(Comm3Index[3]<LineOfComm3Report.length())
                                            {
                                                QString Line=LineOfComm3Report.at(Comm3Index[3]);
                                                double TimeCommReport=(Line.section("\t",0,0)).toDouble();
                                                double EbNo=(Line.section("\t",4,4)).toDouble();

                                                if(abs(MJDdate[index]-TimeCommReport)<10e-6)
                                                {
                                                    stream<<EbNo<<"\t";
                                                    Comm3Index[3]++;
                                                }

                                                else
                                                {
                                                    stream<<"No visibility"<<"\t";
                                                }
                                            }
                                            else
                                            {
                                                stream<<"No visibility"<<"\t";
                                            }
                                        }
                                        if(name=="BER")
                                        {
                                            if(Comm3Index[4]<LineOfComm3Report.length())
                                            {
                                                QString Line=LineOfComm3Report.at(Comm3Index[4]);
                                                double TimeCommReport=(Line.section("\t",0,0)).toDouble();
                                                double BER=(Line.section("\t",5,5)).toDouble();

                                                if(abs(MJDdate[index]-TimeCommReport)<10e-6)
                                                {
                                                    stream<<BER<<"\t";
                                                    Comm3Index[4]++;
                                                }

                                                else
                                                {
                                                    stream<<"No visibility"<<"\t";
                                                }
                                            }
                                            else
                                            {
                                                stream<<"No visibility"<<"\t";
                                            }
                                        }
                                        if(name=="Eccentricity")
                                        {
                                            QWidget*Box=treeWidgetShowInReport->itemWidget(parameter,1);
                                            QComboBox*ComboBox=dynamic_cast <QComboBox*>(Box);
                                            QString ToCoord=ComboBox->currentText();

                                          sta::StateVector Vector[inumber];
                                          Vector[index]=arc->trajectorySample(j);
                                          double Eccentricity=calcKeplerianElements(Vector[index],STA_SOLAR_SYSTEM->lookup("Earth"),"Eccentricity",
                                                                                                   MJDdate[index],
                                                                                                   "EME J2000",
                                                                                                   ToCoord);
                                            stream<<Eccentricity<<"\t";
                                        }
                                        if((name=="Inclination")||
                                           (name=="Longitude of the Ascending Node")||
                                           (name=="Argument of Periapsis")||
                                           (name=="Mean Anomaly at Epoch")||
                                           (name=="Semimajor Axis"))
                                        {
                                            QWidget*Box1=treeWidgetShowInReport->itemWidget(parameter,1);
                                            QComboBox*ComboBox1=dynamic_cast <QComboBox*>(Box1);
                                            QString ToCoord=ComboBox1->currentText();
                                            QWidget*Box2=treeWidgetShowInReport->itemWidget(parameter,1);
                                            QComboBox*ComboBox2=dynamic_cast <QComboBox*>(Box2);
                                            QString ToUnit=ComboBox2->currentText();

                                            sta::StateVector Vector[inumber];
                                            Vector[index]=arc->trajectorySample(j);

                                           if(name=="Inclination")
                                            {
                                               double Inclination=calcKeplerianElements(Vector[index],STA_SOLAR_SYSTEM->lookup("Earth"),name,
                                                                                                        MJDdate[index],
                                                                                                        "EME J2000",
                                                                                                        ToCoord);
                                           stream<<Inclination<<"\t";
                                           }
                                           if(name=="Longitude of the Ascending Node")
                                           {
                                               double Ascending=calcKeplerianElements(Vector[index],STA_SOLAR_SYSTEM->lookup("Earth"),name,
                                                                                                        MJDdate[index],
                                                                                                        "EME J2000",
                                                                                                        ToCoord);
                                           stream<<Ascending<<"\t";
                                           }
                                           if(name=="Mean Anomaly at Epoch")
                                           {
                                               double MeanAnom=calcKeplerianElements(Vector[index],STA_SOLAR_SYSTEM->lookup("Earth"),name,
                                                                                                        MJDdate[index],
                                                                                                        "EME J2000",
                                                                                                        ToCoord);
                                           stream<<MeanAnom<<"\t";
                                           }
                                           if(name=="Argument of Periapsis")
                                           {
                                               double Periapsis=calcKeplerianElements(Vector[index],STA_SOLAR_SYSTEM->lookup("Earth"),name,
                                                                                                        MJDdate[index],
                                                                                                        "EME J2000",
                                                                                                        ToCoord);
                                           stream<<Periapsis<<"\t";
                                           }
                                           if(name=="Semimajor Axis")
                                           {
                                               double SemAxis=calcKeplerianElements(Vector[index],STA_SOLAR_SYSTEM->lookup("Earth"),name,
                                                                                                        MJDdate[index],
                                                                                                        "EME J2000",
                                                                                                        ToCoord);
                                           stream<<SemAxis<<"\t";
                                           }
                                        }

                                    }
stream<<"\r\n";
                                }
                            }

                            //CovIndex[0]++;
                            ////qDebug()<<CovIndex[0]<<"linha do ficheiro de coverage que estou a ler";
                            stream<<"\r\n";
                        }
                    }
                    //}
                    //  }
                    //qDebug()<<"file done";
                    file.close();

                    QDesktopServices::openUrl(QUrl(tr("file:///analysisReport.txt")));
                }
            }
        }
    }
}


/*void analysis::on_TotalHelpPushButton_clicked()
{
	qWarning("TODO: %s	%d",__FILE__,__LINE__);
}
*/
/*void analysis::on_ClosePushButton_clicked()
{
	this->close();

}*/
sta::CoordinateSystem analysis::CoordSys(QString Coordinate)
{


    if(Coordinate=="EME J2000")
    {
        sta::CoordinateSystem Coord("INERTIAL J2000");
        return Coord;
    }
    if(Coordinate=="EME B1950")
    {
       sta::CoordinateSystem Coord("INERTIAL B1950");
        return Coord;
    }
    if(Coordinate=="Fixed")
    {
        sta::CoordinateSystem Coord("PLANETO FIXED");
        return Coord;
    }
    if(Coordinate=="Ecliptic J2000")
    {
        sta::CoordinateSystem Coord("ECLIPTIC");
        return Coord;
    }
}
double analysis::calcKeplerianElements(sta::StateVector Vector,StaBody*Body,QString OrbElement,
                                                       double mjd,
                                                       QString FromCoordinate,
                                                       QString ToCoordinate)
{

    sta::StateVector ModifVector=CoordinateSystem::convert(Vector,
                                                           mjd,
                                                           STA_SOLAR_SYSTEM->lookup("Earth"),
                                                           analysis::CoordSys(FromCoordinate),
                                                           STA_SOLAR_SYSTEM->lookup("Earth"),
                                                           analysis::CoordSys(ToCoordinate));
    double mu = Body->mu();
    sta::KeplerianElements KepElemList=cartesianTOorbital(mu, ModifVector);

    double Element=0;
    if(OrbElement=="Eccentricity")
    {
        Element=KepElemList.Eccentricity;
    }
    if(OrbElement=="Inclination")
    {
        Element=KepElemList.Inclination;
    }
    if(OrbElement=="Semimajor Axis")
    {
        Element=KepElemList.SemimajorAxis;
    }
    if(OrbElement=="Longitude of the Ascending Node")
    {
        Element=KepElemList.AscendingNode;
    }
    if(OrbElement=="Argument of Periapsis")
    {
        Element=KepElemList.ArgumentOfPeriapsis;
    }
    if(OrbElement=="Mean Anomaly at Epoch")
    {
        Element=KepElemList.MeanAnomaly;
    }

    return Element;
}
void analysis::addParameter()

{
    QList<QTreeWidgetItem *> selectedParameters=treeWidgetReportOptions->selectedItems();
    int numberOfchildren = selectedParameters[0]->childCount();
    if (numberOfchildren == 0)
    {
        QList<QTreeWidgetItem *> parameters = treeWidgetReportOptions->selectedItems();

        for (int i = 0; i < parameters.size(); i++)
        {
            addParameter(parameters.at(i));

        }
    }
}


void analysis::addParameter(QTreeWidgetItem* item)
{
    QString text = item->text(0);
    if(treeWidgetShowInReport->findItems(text,Qt::MatchExactly).isEmpty())

    {
        QTreeWidgetItem*added=new QTreeWidgetItem(treeWidgetShowInReport);
        added->setText(0,text);
        ComboBoxOptions();
    }

}


void analysis::removeParameter()
{
    QList<QTreeWidgetItem *> ChosenParameters = treeWidgetShowInReport->selectedItems();
    for (int i = 0; i < ChosenParameters.size(); i++)
    {
        delete ChosenParameters.at(i);
    }
}

void analysis::enableReportOption(int i)
{

    if (i == 0)
    {
        stackedWidget->setCurrentWidget(pageReport);

    }
    else
    {
        stackedWidget->setCurrentWidget(pagePlotAxesSettings);

        if(i == 1) //2D plot
        {
            treeWidgetXaxis->show();
            treeWidgetYaxis->show();
            treeWidgetZaxis->hide();
        }
        if (i ==2 ) //2.5D plot
        {
            treeWidgetXaxis->show();
            treeWidgetYaxis->show();

        }
        if (i == 3) //3D plot
        {
            treeWidgetXaxis->show();
            treeWidgetYaxis->show();
            treeWidgetZaxis->show();
        }
        if ( i== 4) //histogram
        {
	    pageReport->hide();
	    pagePlotAxesSettings->hide();
	}
    }
}
QComboBox* analysis::CoordinateBox()
{
    QComboBox*CoordinateBox2=new QComboBox();
    CoordinateBox2->addItem(tr("EME J2000"),0);
    CoordinateBox2->addItem(tr("EME B1950"),1);
    CoordinateBox2->addItem(tr("Fixed"),2);
    CoordinateBox2->addItem(tr("Ecliptic J2000"),3);
    CoordinateBox2->addItem(tr("Rotating"),4);
    CoordinateBox2->addItem(tr("Rotating Normalized"),5);
    return CoordinateBox2;
}
QComboBox*analysis::TimeFramesBox()
{
    QComboBox*TimeBox=new QComboBox();
    TimeBox->addItem(tr("Gregorian UTC"));
    TimeBox->addItem(tr("Gregorian LCL"));
    TimeBox->addItem(tr("YYDDD"));
    TimeBox->addItem(tr("Julian Date"));
    TimeBox->addItem(tr("Julian LCL"));
    TimeBox->addItem(tr("Mission Elapsed Time"));
    TimeBox->addItem(tr("GMT"));
    TimeBox->addItem(tr("MJD"));
    TimeBox->addItem(tr("Days,hours,minutes,and seconds from epoch"));
    return TimeBox;
}

QComboBox*analysis::TimeUnitsBox()
{

    QComboBox*TimeBox=new QComboBox();
    TimeBox->addItem(tr("Seconds"),0);
    TimeBox->addItem(tr("Minutes"),1);
    TimeBox->addItem(tr("Hours"),2);
    TimeBox->addItem(tr("Days"),3);
    return TimeBox;
}

QComboBox*analysis::AngleUnitsBox()
{
    QComboBox*AngleBox=new QComboBox();
    AngleBox->addItem(tr("Rad"),0);
    AngleBox->addItem(tr("Deg"),1);
    return AngleBox;
}

QComboBox*analysis::DistanceUnitsBox()
{
    QComboBox*DistanceBox=new QComboBox();
    DistanceBox->addItem(tr("Km"),0);
    DistanceBox->addItem(tr("m"),1);
    DistanceBox->addItem(tr("AU"),2);
    DistanceBox->addItem(tr("NM"),3);
    return DistanceBox;
}

QComboBox*analysis::VelocityUnitsBox()
{
    QComboBox*VelocityBox=new QComboBox();
    VelocityBox->addItem(tr("Km/s"),0);
    VelocityBox->addItem(tr("m/s"),0);
    return VelocityBox;
}
void analysis::ComboBoxOptions()
{
    treeWidgetShowInReport->setColumnWidth(0,138);
    treeWidgetShowInReport->setColumnWidth(1,120);
    treeWidgetShowInReport->setColumnWidth(2,50);
    //qDebug()<<treeWidgetShowInReport->topLevelItemCount();
    for(int i=0;i<treeWidgetShowInReport->topLevelItemCount();i++)
    {
        QTreeWidgetItem*item=treeWidgetShowInReport->topLevelItem(i);
        QString name=item->text(0);
        if ((name=="x position") ||
            (name=="y position")||
            (name=="z position") ||
            (name=="Sun")||
            (name=="Mercury")||
            (name=="Venus")||
            (name=="Earth")||
            (name=="Moon")||
            (name=="Mars")||
            (name=="Jupiter")||
            (name=="Saturn")||
            (name=="Uranus")||
            (name=="Neptune")||
            (name=="Pluto") ||
            (name=="Radius")||
            (name=="Apogee Planetocentric Radius")||
            (name=="Perigee Planetocentric Radius")||
            (name=="Range")||
            (name=="Semimajor Axis"))
        {
            treeWidgetShowInReport->setItemWidget(item,1,CoordinateBox());
            treeWidgetShowInReport->setItemWidget(item,2,DistanceUnitsBox());
        }
        if((name=="Eccentricity"))

        {
            treeWidgetShowInReport->setItemWidget(item,1,CoordinateBox());

        }
        if((name=="x velocity")||
           (name=="y velocity")||
           (name=="z velocity"))
        {
            treeWidgetShowInReport->setItemWidget(item,1,CoordinateBox());
            treeWidgetShowInReport->setItemWidget(item,2,VelocityUnitsBox());
        }
        if((name=="Access Time")||
           (name=="Time")||
           (name=="Time crossing the J2000 epoch")||
           (name=="Time crossing the Mean Ecliptic Plane of Date")||
           (name=="Time crossing the True Ecliptic Plane of Date")||
           (name=="Local Time of Ascending Node")||
           (name=="Local Time of Descending Node")||
           (name=="Time of Ascending Node")||
           (name=="Time of Descending Node")||
           (name=="Time of Apogee")||
           (name=="Time of Perigee")||
           (name=="Time of Minimum Latitude")||
           (name=="Time of Maximum Altitude")||
           (name=="Time of Maximum Sun Elevation")||
           (name=="Time of Apparent Noon")||
           (name=="Time of Apparent Midnight")||
           (name=="Time of Apparent Dawn")||
           (name=="Time of Apparent Dusk"))
        {
            treeWidgetShowInReport->setItemWidget(item,1,TimeFramesBox());
            treeWidgetShowInReport->setItemWidget(item,2,TimeUnitsBox());
        }
        if((name=="Inclination")||
           (name=="Longitude of the Ascending Node")||
           (name=="Argument of Periapsis")||
           (name=="Mean Anomaly at Epoch")||
           (name=="Apogee Planetocentric Altitude")||
           (name=="Perigee Planetocentric Altitude")||
           (name=="Longitude when passing Ascending Node")||
           (name=="Longitude when passing Descending Node")||
           (name=="Maximum Geodetic Latitude")||
           (name=="Minimum Geodetic Latitude")||
           (name=="Heading Angle")||
           (name=="Altitude")||
           (name=="Latitude")||
           (name=="Longitude")||
           (name=="Azimuth")||
           (name=="Elevation")||
           (name=="Flight Path Angle")
           )
        {
            treeWidgetShowInReport->setItemWidget(item,1,CoordinateBox());
            treeWidgetShowInReport->setItemWidget(item,2,AngleUnitsBox());
        }
    }
}
