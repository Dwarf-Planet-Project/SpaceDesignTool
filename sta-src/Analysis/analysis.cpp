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
 Patched extensively by Guillermo in July 2010 to add more features like icons, help, etc.
*/

#include "Analysis/analysis.h"
#include "math.h"
#include "Scenario/scenario.h"
#include "Main/propagatedscenario.h"
#include "Main/scenariotree.h"
#include "Astro-Core/statevector.h"
#include "Astro-Core/date.h"
#include "Astro-Core/stacoordsys.h"
#include "Astro-Core/stabody.h"
#include "Astro-Core/stamath.h"
#include "Astro-Core/calendarTOjulian.h"
#include "Astro-Core/cartesianTOorbital.h"
#include "Astro-Core/cartesianTOspherical.h"
#include "Astro-Core/date.h"
#include <Coverage/commanalysis.h>
#include <Coverage/coverageanalysis.h>
#include "Analysis/AnalysisPlot.h"
#include "Plotting/PlotView.h"
#include "Plotting/PlotView3D.h"
#include "Help/HelpBrowser.h"
#include "Analysis/analysisTable.h"

#include <QDesktopServices>
#include <QUrl>
#include <QDebug>
#include <QComboBox>
#include <QMessageBox>
#include <QInputDialog>
#include <QtCore/QFile>
#include <qtextstream.h>

class ScenarioTree;

class HelpBrowser;



analysis::analysis(SpaceScenario*scenario, PropagatedScenario*propagatedScenario,QWidget * parent, Qt::WindowFlags f) : QDialog(parent,f)
{
    setupUi(this);
    m_scenario=new SpaceScenario(*scenario);
    m_propagatedScenario=new PropagatedScenario(*propagatedScenario);

    readScenario();
    DisableUnavailableOptions();
    //connect(treeWidgetReportOptions, SIGNAL(activated(int)), this, SLOT(addParameter(QTreeWidgetItem*)));
    connect(AddParameterPushButton, SIGNAL(clicked()), this, SLOT(addParameter()));
    connect(RemoveParameterPushButton, SIGNAL(clicked()), this, SLOT(removeParameter()));
    connect(ComboBoxAnalysisFormat, SIGNAL(activated(int)), this, SLOT(enableReportOption(int)));

    // Guillermo activating the HELP
    // TotalHelpPushButton->setDisabled(false);  // Not yet
    connect(TotalHelpPushButton, SIGNAL(clicked()), this, SLOT(raiseHelp()));

    PlotComboBox();   

    // Patched by Guillermo to make the columns fixed in width
    treeWidgetTimeSpecifications->setColumnWidth(0, 155);
    treeWidgetTimeSpecifications->setColumnWidth(1, 155);
    treeWidgetTimeSpecifications->setColumnWidth(2, 35);

    treeWidgetShowInReport->setColumnWidth(0, 160);
    treeWidgetShowInReport->setColumnWidth(1, 100);
    treeWidgetShowInReport->setColumnWidth(2, 35);

    treeWidgetXaxis->setColumnWidth(0, 160);
    treeWidgetXaxis->setColumnWidth(1, 100);
    treeWidgetXaxis->setColumnWidth(2, 35);
    treeWidgetYaxis->setColumnWidth(0, 160);
    treeWidgetYaxis->setColumnWidth(1, 100);
    treeWidgetYaxis->setColumnWidth(2, 35);
    treeWidgetZaxis->setColumnWidth(0, 160);
    treeWidgetZaxis->setColumnWidth(1, 100);
    treeWidgetZaxis->setColumnWidth(2, 35);

}

analysis::~analysis()
{
    // Guilelrmo says: this code is not required. It breask STA on MAC
    /*
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
    */
}



//Guillermo
void analysis::raiseHelp()
{
    QString ResourcesPath = QDir::currentPath ();
    QString HelpBrowserPath = ResourcesPath + "/help";
    QString page = "index.html";    // Guillermo says: Ana please change this into the ANALYSIS web page
    HelpBrowser *browser = new HelpBrowser(HelpBrowserPath, page, this, Qt::Tool);
    browser->resize(800, 600);
    browser->setWindowModality(Qt::NonModal);
    browser->activateWindow(); // Required to keep the modeless window alive
    browser->show();
    browser->raise();
}




void analysis::readScenario()

{
    /*
      builds a copy of the scenario tree from the STA main window in the GUI of Analysis Module
       */
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

            //spacecraftItem->setText(0,"SpaceCraft");
            spacecraftItem->setText(0,participant.data()->Name());
	    // Guillermo
	    spacecraftItem->setIcon(0, QIcon(":/icons/ParticipantSATELLITE.png"));

            QTreeWidgetItem* trajectoryPlan=new QTreeWidgetItem(spacecraftItem);
	    trajectoryPlan->setText(0,"Trajectory Plan");

            QTreeWidgetItem*PayloadSet=new QTreeWidgetItem(spacecraftItem);
	    PayloadSet->setText(0,"Payload Set");

            ScenarioSC* vehicle = dynamic_cast<ScenarioSC*>(participant.data());

            const QList<QSharedPointer<ScenarioAbstractTrajectoryType> >& trajectoryList =
                    vehicle->SCMission()->TrajectoryPlan()->AbstractTrajectory();

            MissionInd=0;
            foreach (QSharedPointer<ScenarioAbstractTrajectoryType> trajectory,trajectoryList) //Loitering
            {
                if (dynamic_cast<ScenarioLoiteringType*>(trajectory.data()))
                {
                    QTreeWidgetItem* loiterItem = new QTreeWidgetItem(trajectoryPlan);
                    //loiterItem->setText(0,"Loitering");
                    ScenarioLoiteringType* loitering = dynamic_cast<ScenarioLoiteringType*>(trajectory.data());
                    loiterItem->setText(0,loitering->ElementIdentifier()->Name());
		    // Guillermo
		    loiterItem->setIcon(0, QIcon(":/icons/mission-arcs-loitering.png"));

                    QTreeWidgetItem* parentType= new QTreeWidgetItem(loiterItem);
                    parentType->setText(0,"space");
                    parentType->setHidden(true);
                    QTreeWidgetItem* parentInd= new QTreeWidgetItem(loiterItem);
                    parentInd->setText(0,QString::number(SpaceObjectInd));
                    parentInd->setHidden(true);
                    QTreeWidgetItem* mindex = new QTreeWidgetItem(loiterItem);
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
		    // Guillermo
		    payloadItem->setIcon(0, QIcon(":/icons/Payload.png"));

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
		    // Guillermo
		    payloadItem->setIcon(0, QIcon(":/icons/Payload.png"));

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

            SpaceObjectInd+=1;
        }

        if(dynamic_cast<ScenarioREV*>(participant.data())!=NULL) //Re-entry vehicle
        {
            QTreeWidgetItem * REVitem =new QTreeWidgetItem(TreeWidgetMissionArc);
            //REVitem->setText(0,"ReentryVehicle");
            REVitem->setText(0,participant.data()->Name());
	    // Guillermo
	    REVitem->setIcon(0, QIcon(":/icons/ParticipantENTRYVEHICLE.png"));


            QTreeWidgetItem*trajectoryPlan=new QTreeWidgetItem(REVitem);
	    trajectoryPlan->setText(0,"Trajectory Plan");

            QTreeWidgetItem*PayloadSet=new QTreeWidgetItem(REVitem);
	    PayloadSet->setText(0,"Payload Set");

            ScenarioREV* vehicle = dynamic_cast<ScenarioREV*>(participant.data());

            const QList<QSharedPointer<ScenarioAbstractTrajectoryType> >& trajectoryList =
                    vehicle->REVMission()->REVTrajectoryPlan()->AbstractTrajectory();

            MissionInd=0;

            foreach (QSharedPointer<ScenarioAbstractTrajectoryType> trajectory,trajectoryList)
            {
                if (dynamic_cast<ScenarioEntryArcType*>(trajectory.data())) //re-entry mission arc
                {
                    QTreeWidgetItem * REVmissionItem = new QTreeWidgetItem(trajectoryPlan);
                    //REVmissionItem->setText(0,"REV mission arc");
                    ScenarioEntryArcType* Reentry=dynamic_cast<ScenarioEntryArcType*>(trajectory.data());
                    REVmissionItem->setText(0,Reentry->ElementIdentifier()->Name());
		    // Guillermo
		    REVmissionItem->setIcon(0, QIcon(":/icons/mission-arcs-reentry.png"));


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
            //GSitem->setText(0,"GroundStation");
            GSitem->setText(0,participant.data()->Name());
	    // Guillermo
	    GSitem->setIcon(0, QIcon(":/icons/ParticipantSTATION.png"));


            QTreeWidgetItem*PayloadSet=new QTreeWidgetItem(GSitem);
	    PayloadSet->setText(0,"Payload Set");



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
		    // Guillermo
		    payloadItem->setIcon(0, QIcon(":/icons/Payload.png"));

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

                }
                if (dynamic_cast<ScenarioReceiverPayloadType*>(payloadType.data()))
                {
                    QTreeWidgetItem * payloadItem = new QTreeWidgetItem(PayloadSet); //Receiver
                    payloadItem->setText(0,"Receiver");
		    // Guillermo
		    payloadItem->setIcon(0, QIcon(":/icons/Payload.png"));

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


    }

}

int analysis::ObjectsIndex(QStringList AllObjects, int Index, QString ObjectType)
{
    /*
      Inputs: AllObjects-QStringList of the type of objects in the scenario (ground or space), in the same order they appear
              Index-number of order of each participant within the grounp type (number of order of the space object within all the space objects or number of order of the ground object within all the ground objects)
              ObjectType- "space" or "ground"
      Outputs: the number of order of the object within all the objects in the scenario, for example, the second ground station could be the third object in the scenario
      */
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


/*

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

*/

/*

void Analysis::on_groupBoxAnalysisFormat_toggled(bool)
{

}
*/
/*void analysis::on_ComboBoxAnalysisFormat_activated(const QString&)
{

}*/

/*
void analysis::on_groupBoxParameters_toggled(bool)
{

}

void analysis::on_groupBoxTimeSpecifications_toggled(bool)
{

}
*/


bool analysis::CheckIfMissionArc()
{
    /*
  returns true if the all selected items in the scenario tree are mission arcs and returns false otherwise
  */
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
    /*
      adds the start and end epochs af the selected mission arcs;
      the start and end epochs are the times defined in the mission arc properties and correspond to the entire propagation time;
      */
    int NumberOfObjects=TreeWidgetMissionArc->topLevelItemCount();


    QStringList AllObjects;
    QList<int>SelectedObjects;
    for(int i=0;i<NumberOfObjects;i++)
    {
        int numberOfChildren=TreeWidgetMissionArc->topLevelItem(i)->childCount();
        AllObjects.append(TreeWidgetMissionArc->topLevelItem(i)->child(numberOfChildren-1)->text(0));

    }

    QList<QTreeWidgetItem *> selected=TreeWidgetMissionArc->selectedItems();


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


void analysis::ReadTime(int column, double *MJD) //reads the time in the treeWidgetTimeSpecifications
{
    /*
  Inputs: column-column number in the treeWidgetTimeSpecifications (start or end epochs);
          *MJD-pointer to the variable that will save the time in Modified Julian Date
  */
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

            double iHour1=Hour.toDouble();
            double iMinute1=Minute.toDouble();
            double iSecond1=Second.toDouble();

            int iHour=(int)iHour1;
            int iMinute=(int)iMinute1;
            int iSecond=(int)iSecond1;
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
QString analysis::ReadParameter(QTreeWidgetItem*Item)
{
    /*
reads the name of the parameter that will be displayed in the output of the Analysis Module
Inputs: item of the tree that is being considered (report or plot options)
      */
    QTreeWidgetItem*parameter=Item;
    QString name=parameter->text(0);
    return name;
}

QString analysis::ReadUnits(QTreeWidget*Tree,QTreeWidgetItem*Item)
{
    /*
      reads the units the user selected as output units
      Inputs: Tree- widget from where data shall be read, Item- line of the widget that is being considered
      */
    if(Item->childCount()==0)
    {
	QWidget*Box=Tree->itemWidget(Item,2);
	QComboBox*ComboBox=dynamic_cast <QComboBox*>(Box);
	QString Unit=ComboBox->currentText();
	if(Item->text(0)=="Time")
	{
	    QWidget*Box=Tree->itemWidget(Item,1);
	    QComboBox*ComboBox=dynamic_cast <QComboBox*>(Box);
	    QString TimeType=ComboBox->currentText();
	    return TimeType;
	}
	return Unit;
    }
    else
    {
        return " ";
    }
}

QString analysis::ReadCoordinateSys(QTreeWidget*Tree,QTreeWidgetItem*Item)
{
    /*
      reads the coordinate system the user selected as output coordinate system
      Inputs: Tree- widget from where data shall be read, Item- line of the widget that is being considered
      */
    if(Item->childCount()==0)
    {
	QWidget*Box=Tree->itemWidget(Item,1);
	QComboBox*ComboBox=dynamic_cast <QComboBox*>(Box);
	QString Coordinate=ComboBox->currentText();
	return Coordinate;
    }
    else
    {
        return " ";
    }
}

void analysis::on_EditTimePushButton_clicked() //adds new time intervals to the tree
{
    /*
      a new time interval, user-specified, can be inserted in the list of Time Intervals
      */
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
    /*
      the selected time interval, in the Time Intervals area can be deleted
      */
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


int analysis::InputsControl(QList<QTreeWidget*>tree)
{
    /*
      Inputs: tree- list of all the treewidgets that are involved in the analysis

      the function checks if the parameters selected by the user correspond to any parameter from the Coverage Module;
 if yes, it is necessary to verify if the user has selected the right options to perform a coverage analysis

 Types of output:
 return 0-ok
 return 1-invalid number of Tx and Rx
 return 2-invalid Tx and Rx location
 return 3-not enought number of parameters selected
 return 4-not only loiterings belonging to the space object with the payload
      */
    int CovCommCount=0;
    if(tree[0]==treeWidgetShowInReport)
    {
	for(int i=0;i<treeWidgetShowInReport->topLevelItemCount();i++)
	{
	    QTreeWidgetItem*ShowInReport=treeWidgetShowInReport->topLevelItem(i);
	    QString ToReport=ShowInReport->text(0);
	    if((ToReport=="Azimuth")||(ToReport=="Elevation")||(ToReport=="Range")||(ToReport=="Equivalent Isotropical Radiated Power")||(ToReport=="Received Frequency")||(ToReport=="Doppler Shift")||(ToReport=="Received Power")||(ToReport=="Flux Density")||(ToReport=="Overlap Bandwidth Factor")||(ToReport=="Free Space Loss")||(ToReport=="Oxygen Loss")||(ToReport=="Water Vapour Loss")||(ToReport=="Rain Loss")||(ToReport=="Atmospheric Loss")||(ToReport=="Propagation Loss")||(ToReport=="G/T")||(ToReport=="C/No")||(ToReport=="C/N")||(ToReport=="Eb/No")||(ToReport=="BER"))
	    {

		CovCommCount++;
	    }
	}
    }
    else
    {
        for (int j=0;j<tree.size();j++)
        {
            for(int i=0;i<tree[j]->selectedItems().size();i++)
            {
		QTreeWidgetItem*Show=tree[j]->selectedItems().at(i);
		QString ToShow=Show->text(0);
		if((ToShow=="Azimuth")||(ToShow=="Elevation")||(ToShow=="Range")||(ToShow=="Equivalent Isotropical Radiated Power")||(ToShow=="Received Frequency")||(ToShow=="Doppler Shift")||(ToShow=="Received Power")||(ToShow=="Flux Density")||(ToShow=="Overlap Bandwidth Factor")||(ToShow=="Free Space Loss")||(ToShow=="Oxygen Loss")||(ToShow=="Water Vapour Loss")||(ToShow=="Rain Loss")||(ToShow=="Atmospheric Loss")||(ToShow=="Propagation Loss")||(ToShow=="G/T")||(ToShow=="C/No")||(ToShow=="C/N")||(ToShow=="Eb/No")||(ToShow=="BER"))
		{

		    CovCommCount++;
		}
	    }
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

        if(Selected.size()<3)
        {

            return 3; //not enough number of parameters selected
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

                    }
                }

                if(childnumber==4)//Tx and Rx
                {

                    if((Selected.at(j)->child(3)->text(0)=="Tx"))
                    {
                        TxCount++;
                    }
                    if((Selected.at(j)->child(3)->text(0)=="Rx"))
                    {
                        RxCount++;
                    }
                    //
                    if((Selected.at(j)->child(0)->text(0)=="1"))
                    {
                        spaceObjCount++;
                        SpaceIndex.append(Selected.at(j)->child(1)->text(0).toInt());

                    }
                    if((Selected.at(j)->child(0)->text(0)=="0"))
                    {

                        groundObjCount++;

                    }
                }

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
                return 4; //invalid loitering selection: not only loiterings belonging to the space object with the payload
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
    /*
      Displays warning messages according to the result of the InputsControl function
      */
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
    /*
  when the "generate" button is pressed, the plot or report are displayed,
some warning messages can de displayed if:
- there are no parameters selected to analyse
- there are no selected mission arcs
-there are no time intervals selected
-the selected parameters, in the plotting option, are not correct (number or type of parameters selected)
  */
    QString AnalysisFormat=ComboBoxAnalysisFormat->currentText();

    QList<QTreeWidgetItem *> selected=TreeWidgetMissionArc->selectedItems();
    QList<QTreeWidgetItem *> selectedTimes=treeWidgetTimeSpecifications->selectedItems();
    QList<QTreeWidget*>tree;
    int numberOfParameters=0;
    if(AnalysisFormat=="Report")
    {
        numberOfParameters=treeWidgetShowInReport->topLevelItemCount();
        tree.append(treeWidgetShowInReport);
    }
    if(AnalysisFormat=="2D")
    {
        numberOfParameters=treeWidgetXaxis->selectedItems().size()+treeWidgetYaxis->selectedItems().size();
        tree.append(treeWidgetXaxis);
        tree.append(treeWidgetYaxis);

    }
    if(AnalysisFormat=="3D")
    {
	numberOfParameters=treeWidgetXaxis->selectedItems().size()+treeWidgetYaxis->selectedItems().size()+treeWidgetZaxis->selectedItems().size();
    }
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
            if (selectedTimes.size()==0)
            {
                QMessageBox tswarning;
                tswarning.setText("Please select one or more time intervals");
                tswarning.exec();
            }
            else
            {
                int Control=InputsControl(tree); //checks if the selected parameters are valid according to the inputs n other module functions
                if(Control!=0)
                {
                    Warnings(Control);
                }
                else
                {
                    QList<analysis::AnalysisData> DataStructure;
                    if (AnalysisFormat=="Report")
                    {

                        WriteReport(selected,selectedTimes);

                    }
                    if((AnalysisFormat=="2D")||(AnalysisFormat=="3D"))
                    {
                        DataStructure=WriteDataStructure(selected,selectedTimes);
                    }

                    if(AnalysisFormat=="2D")
                    {

                        if(DataStructure.size()==2)
                        {
			    int numberOfLines=0;
			    int numberOfParameters=1; //one parameter per axis

			    for(int i=0;i<DataStructure[0].Data[0].size();i++)
			    {
				numberOfLines++;
			    }

                            // Patched by Guillermo to create a Non-Modal window each time the user wishes to create a 2D plot
                            QWidget *window2D = new QWidget(this, Qt::Tool);

                            QVBoxLayout* layout = new QVBoxLayout(window2D);

                            PlotView* plotView = new PlotView(window2D);
                            layout->addWidget(plotView);

                            AnalysisPlot::AnalysisPlot* Data = new AnalysisPlot();
                            Data->setPoints(DataStructure,numberOfLines,numberOfParameters);
                            plotView->addPlot(Data);

                            QString PlotTitle = "";
                            PlotTitle = PlotTitle + DataStructure[1].ParameterTitles[0];
                            PlotTitle = PlotTitle + " = f (";
                            PlotTitle = PlotTitle + DataStructure[0].ParameterTitles[0];
                            PlotTitle = PlotTitle + ")";

                            plotView->setTitle(PlotTitle);
                            plotView->setLeftLabel(DataStructure[1].ParameterTitles[0]);
                            plotView->setBottomLabel(DataStructure[0].ParameterTitles[0]);
                            plotView->autoScale();
                            plotView->setMinimumSize(600, 400); // Rectangle

                            QHBoxLayout *buttonLayout = new QHBoxLayout;
                            QPushButton *closeButton = new QPushButton(tr("Close"));
                            closeButton->setShortcut(tr("Esc"));
                            closeButton->setAutoDefault(true);
                            buttonLayout->addStretch();
                            buttonLayout->addWidget(closeButton);
                            layout->addLayout(buttonLayout);
                            connect(closeButton, SIGNAL(clicked()), window2D, SLOT(close()));

                            window2D->setLayout(layout);
                            window2D->setWindowModality(Qt::NonModal);
                            window2D->setWindowTitle("STA analysis 2D plot");
                            window2D->setWindowIcon(QIcon(":/icons/CoordinateSystemBody.png"));
                            window2D->show();
                            window2D->raise(); // Required to keep the modeless window alive
                            window2D->activateWindow(); // Required to keep the modeless window alive

			}
                        else
                        {
                            QMessageBox PlotWarning;
                            PlotWarning.setText("Invalid selection of parameters to be plotted");
                            PlotWarning.exec();
                        }
		    }
                    if(AnalysisFormat=="3D")
                    {
                        int numberOfLines=0;
                        int numberOfParameters=1; //one parameter per axis

                        for(int i=0;i<DataStructure[0].Data[0].size();i++)
                        {
                            numberOfLines++;
                        }

                        if(DataStructure.size()==3)
			{

			    // Patched by Guillermo to create a Non-Modal window each time the user wishes to create a 3D plot
                            QWidget *window3D = new QWidget(this, Qt::Tool);

			    QVBoxLayout* layout = new QVBoxLayout(window3D);

			    PlotView3D* plotView = new PlotView3D(window3D);
			    layout->addWidget(plotView);  

			    Analysis3D::Analysis3D *Data = new Analysis3D();
			    Data->setPoints(DataStructure,numberOfLines,numberOfParameters);

			    plotView->addPlot(Data, PlotStyle(PlotStyle::LinePlot, QPen(Qt::red, 2, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin)));

			    QString PlotTitle = "";
			    PlotTitle = PlotTitle + DataStructure[2].ParameterTitles[0];
                            PlotTitle = PlotTitle + " = f (";
			    PlotTitle = PlotTitle + DataStructure[0].ParameterTitles[0];
			    PlotTitle = PlotTitle + ", ";
			    PlotTitle = PlotTitle + DataStructure[1].ParameterTitles[0];
			    PlotTitle = PlotTitle + ")";

			    plotView->setTitle(PlotTitle);
			    plotView->setXLabel(DataStructure[0].ParameterTitles[0]);
			    plotView->setYLabel(DataStructure[1].ParameterTitles[0]);
			    plotView->setZLabel(DataStructure[2].ParameterTitles[0]);
			    plotView->autoScale();
			    plotView->setMinimumSize(500, 500);

			    QHBoxLayout *buttonLayout = new QHBoxLayout;
			    QPushButton *closeButton = new QPushButton(tr("Close"));
			    closeButton->setShortcut(tr("Esc"));
			    closeButton->setAutoDefault(true);
			    buttonLayout->addStretch();
			    buttonLayout->addWidget(closeButton);
			    layout->addLayout(buttonLayout);
			    connect(closeButton, SIGNAL(clicked()), window3D, SLOT(close()));

			    window3D->setLayout(layout);
                            window3D->setWindowModality(Qt::NonModal);
                            window3D->setWindowTitle("STA analysis 3D plot");
                            window3D->setWindowIcon(QIcon(":/icons/CoordinateSystemBody.png"));
			    window3D->show();
                            window3D->raise(); // Required to keep the modeless window alive
                            window3D->activateWindow(); // Required to keep the modeless window alive

                        }
                        else
                        {
                            QMessageBox PlotWarning;
                            PlotWarning.setText("Invalid selection of parameters to be plotted");
                            PlotWarning.exec();
                        }
                    }
                }
            }
        }
    }
}


void analysis::WriteReport(QList<QTreeWidgetItem *> selected,QList<QTreeWidgetItem *> selectedTimes)
{
    /*
  generates and displays the report with the user-specified data
  Inputs: selected- list of the selected lines in the scenario tree of the AM GUI, selectedTimes- list of the selected time intervals
  */
    int numberOfRows = 0; // Guillermo says: keep the number of rows of the report
    int numberOfColumns = 0; // Guillermo says: keep the number of columns of the report

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

	    TxParentIndex=selected.at(t)->child(1)->text(0).toInt();

	    TxArrayIndex=selected.at(t)->child(2)->text(0).toInt();

	    int TxParentTy=selected.at(t)->child(0)->text(0).toInt();



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

	    }
	}

	if(selected.at(t)->child(childnum-1)->text(0)=="Rx")
	{
	    RxParentIndex=selected.at(t)->child(childnum-3)->text(0).toInt();
	    RxArrayIndex=selected.at(t)->child(childnum-2)->text(0).toInt();
	    int RxParentTy=(selected.at(t)->child(0)->text(0)).toInt();
	    if(RxParentTy==0)
	    {

		indGS=RxParentIndex;
		Receiver=RxGS.at(RxArrayIndex);
		Environment=GSEnvironment.at(indGS);
		RxParentType=false;

	    }
	    //indGS=0;
	    if(RxParentTy==1)
	    {

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

    // Patched by Guillermo to allow read of files in MAc and Linux
    QString ResourcesPath = QDir::currentPath ();
    QString analysisFile = ResourcesPath + "/" + "analysisReport.txt";
    QFile file(analysisFile);
    file.open(QIODevice::WriteOnly);
    QTextStream stream(&file);
    stream.setRealNumberPrecision(16);

    //######time of generation of report#####
    QDateTime ReportDateTime=QDateTime::currentDateTime();

    stream<<"Report generated on"<<"\t"<<ReportDateTime.date().day()<<"/"<<ReportDateTime.date().month()<<"/"<<ReportDateTime.date().year()<<" "<<"at"<<" "<<ReportDateTime.time().hour()<<":"<<ReportDateTime.time().minute()<<":"<<ReportDateTime.time().second()<<"\r\n";


    for(int z=0;z<MParentIndex.size();z++)
    {

	SpaceObject*spaceObj=m_propagatedScenario->spaceObjects().at(MParentIndex.at(z));

	{
	    int indMissionArc=MObjectsIndex.at(z);
	    MissionArc*arc=spaceObj->mission().at(MObjectsIndex.at(z));

	    stream<<"Satellite: "<<MParentIndex.at(z)+1<<"\t"<<"Mission Arc: "<<indMissionArc+1<<"\r\n";

	    double StartTime[selectedTimes.size()];
	    double StopTime[selectedTimes.size()];
	    int countStart[selectedTimes.size()];
	    int countStop[selectedTimes.size()];

	    ReadTime(0,StartTime);//in MJD
	    ReadTime(1,StopTime); //in MJD

	    double StartEpoch=arc->trajectorySampleTime(0);
	    int totalTime=(arc->trajectorySampleCount())-1; //position of the last sample time
	    double StopEpoch=arc->trajectorySampleTime(totalTime);

            int CovIndex[4]; //line of Coverage Report for each parameter
            CovIndex[0]=CovIndex[1]=CovIndex[2]=CovIndex[3]=1; //0-Azimuth, 1-Elevation, 2-Range, 3-Access Time
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

		int ControlStart=0;
		int ControlStop=0;
		if ((StopTime[k]>(StopEpoch+2*pow(10,-5)))||(StopTime[k]<=(StartEpoch-2*pow(10,-5))))
		{
		    ControlStart++;
		}
		if ((StopTime[k]>(StopEpoch+2*pow(10,-5)))||(StopTime[k]<=(StartEpoch-2*pow(10,-5))))
		{
		    ControlStop++;
		}

		if((ControlStop==0)&&(ControlStop==0))

		{

		    int i=0;
		    while(( (arc->trajectorySampleTime(i)))<(StartTime[k]))
		    {

			i=i+1;
		    }
		    countStart[k]=i;
		    int m=0;
		    while(((arc->trajectorySampleTime(m)))<(StopTime[k]-pow(10,-5)))
		    {
			m=m+1;
		    }
		    countStop[k]=m;
		    int inumber=countStop[k]-countStart[k]+1;
		    QList<QTreeWidgetItem*>SelTime=treeWidgetTimeSpecifications->selectedItems();
		    QString Start=SelTime[k]->text(0);
		    QString Stop=SelTime[k]->text(1);


		    //stream<<"Satellite:"<<MParentIndex.at(z)+1<<"\t"<<"Mission Arc:"<<indMissionArc+1<<"\r\n";
		    stream<<"#######Beginning of time"<<" "<<(k+1)<<"######"<<"\r\n"<<"######"<<"Start Time"<<" "<<Start<<"\t"<<"Stop Time"<<" "<<Stop<<"######"<<"\r\n";

		    //printing the labels of the displayed parameters
		    for(int i=0;i<treeWidgetShowInReport->topLevelItemCount();i++)
		    {
			QString name=analysis::ReadParameter(treeWidgetShowInReport->topLevelItem(i));
			QString Unit=analysis::ReadUnits(treeWidgetShowInReport,treeWidgetShowInReport->topLevelItem(i));
			// Guillermo patched the following line
			//stream<<"\t"<<name<<"("<<Unit<<")"<<"\t";
			stream << name << "(" << Unit << ")" << "\t";
			numberOfColumns = numberOfColumns +1;

			if(i==((treeWidgetShowInReport->topLevelItemCount())-1))
			{
			    stream<<"\r\n";
			}


		    }

		    double MJDdate[inumber];
		    double JulianDate[inumber];
		    QDateTime GregorianLCL[inumber];
		    double CurrentJulianDate=sta::DateTimeTOjulian(QDateTime::currentDateTime());
		    double JulianLCL[inumber];
		    QDateTime TimeDateVector[inumber];
		    double DayOfYear[inumber];


		    for(int j=countStart[k];j<=countStop[k];j++)
		    {
			int index=j-countStart[k];

			MJDdate[index]=arc->trajectorySampleTime(j);
                        int AccessNumber=0;
                        int AccessStep=0;

			//stream<<MJDdate[index]<<"\t"; //prints MJD by default

			for(int i=0;i<treeWidgetShowInReport->topLevelItemCount();i++)
			{

			    QTreeWidgetItem*parameter=treeWidgetShowInReport->topLevelItem(i);
			    QString name=parameter->text(0);

			    if((name=="x position")||(name=="y position")||(name=="z position")||(name=="x velocity")||(name=="y velocity")||(name=="z velocity"))
			    {

				QString Coordinate=analysis::ReadCoordinateSys(treeWidgetShowInReport,parameter);
				QString Units=analysis::ReadUnits(treeWidgetShowInReport,parameter);

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
					stream<<"\t"<<sta::ConvertUnits(Units,ModifVector[index].position.x(),"km")<<"\t";


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
					stream<<"\t"<<sta::ConvertUnits(Units,ModifVector[index].position.x(),"km")<<"\t";

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

					stream<<"\t"<<sta::ConvertUnits(Units,ModifVector[index].position.x(),"km")<<"\t";


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
					stream<<"\t"<<sta::ConvertUnits(Units,ModifVector[index].position.x(),"km")<<"\t";

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
					stream<<"\t"<<sta::ConvertUnits(Units,ModifVector[index].position.y(),"km")<<"\t";

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
					stream<<"\t"<<sta::ConvertUnits(Units,ModifVector[index].position.y(),"km")<<"\t";

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
					stream<<"\t"<<sta::ConvertUnits(Units,ModifVector[index].position.y(),"km")<<"\t";

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
					stream<<"\t"<<sta::ConvertUnits(Units,ModifVector[index].position.y(),"km")<<"\t";

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

					stream<<"\t"<<sta::ConvertUnits(Units,ModifVector[index].position.z(),"km")<<"\t";

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
					stream<<"\t"<<sta::ConvertUnits(Units,ModifVector[index].position.z(),"km")<<"\t";

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
					stream<<"\t"<<sta::ConvertUnits(Units,ModifVector[index].position.z(),"km")<<"\t";

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
					stream<<"\t"<<sta::ConvertUnits(Units,ModifVector[index].position.z(),"km")<<"\t";

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
					stream<<"\t"<<sta::ConvertUnits(Units,ModifVector[index].velocity.x(),"km/s")<<"\t";



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
					stream<<"\t"<<sta::ConvertUnits(Units,ModifVector[index].velocity.x(),"km/s")<<"\t";



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
					stream<<"\t"<<sta::ConvertUnits(Units,ModifVector[index].velocity.x(),"km/s")<<"\t";

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
					stream<<"\t"<<sta::ConvertUnits(Units,ModifVector[index].velocity.x(),"km/s")<<"\t";



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
					stream<<"\t"<<sta::ConvertUnits(Units,ModifVector[index].velocity.y(),"km/s")<<"\t";



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
					stream<<"\t"<<sta::ConvertUnits(Units,ModifVector[index].velocity.y(),"km/s")<<"\t";



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
					stream<<"\t"<<sta::ConvertUnits(Units,ModifVector[index].velocity.y(),"km/s")<<"\t";

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
					stream<<"\t"<<sta::ConvertUnits(Units,ModifVector[index].velocity.y(),"km/s")<<"\t";



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
					stream<<"\t"<<sta::ConvertUnits(Units,ModifVector[index].velocity.z(),"km/s")<<"\t";

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
					stream<<"\t"<<sta::ConvertUnits(Units,ModifVector[index].velocity.z(),"km/s")<<"\t";
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
					stream<<"\t"<<sta::ConvertUnits(Units,ModifVector[index].velocity.z(),"km/s")<<"\t";

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
					stream<<"\t"<<sta::ConvertUnits(Units,ModifVector[index].velocity.z(),"km/s")<<"\t";



				    }

				}
			    }
			    if(name=="Time")
			    {

				QString TimeCoordinate=analysis::ReadCoordinateSys(treeWidgetShowInReport,parameter);

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
				if(TimeCoordinate=="Julian UTC")
				{
				    //format:DayOfYear/YY UTCTime
				    JulianDate[index]=sta::MjdToJd(MJDdate[index]+0.00001);
				    TimeDateVector[index]=sta::JdToCalendar(JulianDate[index]);
				    int Year=TimeDateVector[index].date().year();
                                    QString YearPreLastDigit=QString::number(Year).at(2);
				    QString YearLastDigit=QString::number(Year).at(3);
                                    QString DDD=sta::calendarToDayOfYear(TimeDateVector[index]);
                                    QString Time = TimeDateVector[index].time().toString(Qt::TextDate);
                                    stream<<DDD<<"/"<<YearPreLastDigit<<YearLastDigit<<" "<<Time;
                                }
				if(TimeCoordinate=="Gregorian LCL")
				{
				    QDateTime CurrentDate=QDateTime::currentDateTime();
				    QDateTime CurrentUTC=CurrentDate.toUTC();
				    double DisplayDate[inumber];
				    QDateTime DisplayDateCalendar[inumber];
				    double CurrentDateInMJD=sta::JdToMjd(sta::CalendarToJd(CurrentDate));
				    double CurrentUtcInMJD=sta::JdToMjd(sta::CalendarToJd(CurrentUTC));

				    if(CurrentUtcInMJD-CurrentDateInMJD<0)

				    {
					DisplayDate[index]=MJDdate[index]+(CurrentDateInMJD-CurrentUtcInMJD)+0.00001;
				    }
				    else
				    {
					DisplayDate[index]=MJDdate[index]-(CurrentUtcInMJD-CurrentDateInMJD)+0.00001;
				    }
				    DisplayDateCalendar[index]=sta::JdToCalendar(sta::MjdToJd(DisplayDate[index]));

                                    QList<QString>Date= sta::TimeLayout(DisplayDateCalendar[index].date().day(),DisplayDateCalendar[index].date().month());
                                    QString Time=DisplayDateCalendar[index].time().toString(Qt::TextDate);
                                    stream<<Date[0]<<"/"<<Date[1]<<"/"<<DisplayDateCalendar[index].date().year()<<" "<<Time<<"\t";
				}
				if(TimeCoordinate=="Gregorian UTC")
				{

				    JulianDate[index]=sta::MjdToJd(MJDdate[index])+0.00001;
				    TimeDateVector[index]=sta::JdToCalendar(JulianDate[index]);
                                    QList<QString>Date=sta::TimeLayout(TimeDateVector[index].date().day(),TimeDateVector[index].date().month());
                                    QString Time=TimeDateVector[index].time().toString(Qt::TextDate);
                                    stream<<Date[0]<<"/"<<Date[1]<<"/"<<TimeDateVector[index].date().year()<<" "<<Time<<"\t";

				}
				if(TimeCoordinate=="Julian LCL")
				{
				    QDateTime CurrentDate=QDateTime::currentDateTime();
				    QDateTime CurrentUTC=CurrentDate.toUTC();
				    double DisplayDate[inumber];
				    QDateTime DisplayDateCalendar[inumber];
				    double CurrentDateInMJD=sta::JdToMjd(sta::CalendarToJd(CurrentDate));
				    double CurrentUtcInMJD=sta::JdToMjd(sta::CalendarToJd(CurrentUTC));

				    if(CurrentUtcInMJD-CurrentDateInMJD<0)

				    {
					DisplayDate[index]=MJDdate[index]+(CurrentDateInMJD-CurrentUtcInMJD)+0.00001;
				    }
				    else
				    {
					DisplayDate[index]=MJDdate[index]-(CurrentUtcInMJD-CurrentDateInMJD)+0.00001;
				    }
				    DisplayDateCalendar[index]=sta::JdToCalendar(sta::MjdToJd(DisplayDate[index]));

                                    QString DDD=sta::calendarToDayOfYear(sta::JdToCalendar(sta::MjdToJd(MJDdate[index]+0.00001)));
				    int Year=(sta::JdToCalendar(sta::MjdToJd(MJDdate[index]))).date().year();
				    QString YearPreLastDigit=QString::number(Year).at(2);
				    QString YearLastDigit=QString::number(Year).at(3);

                                    QString Time=DisplayDateCalendar[index].time().toString(Qt::TextDate);
                                    stream<<DDD<<"/"<<YearPreLastDigit<<YearLastDigit<<" "<<Time<<"\t";

				}

                                if(TimeCoordinate=="Mission Elapsed")
				{
                                    QString Elapsed= sta::MissionElapsedTime(MJDdate[index],StartEpoch);
                                    stream<<Elapsed<<"\t";

                                }
				if(TimeCoordinate=="YYDDD")
				{
				    JulianDate[index]=sta::MjdToJd(MJDdate[index]+0.00001);
				    TimeDateVector[index]=sta::JdToCalendar(JulianDate[index]);
				    int Year=TimeDateVector[index].date().year();
				    QDateTime FirstDayCurrentYear(QDate(Year,1,1),QTime(0,0,0));
				    double StartYearTime=sta::JdToMjd(sta::CalendarToJd(FirstDayCurrentYear));
				    QString YearPreLastDigit=QString::number(Year).at(2);
				    QString YearLastDigit=QString::number(Year).at(3);

				    DayOfYear[index]=sta::MjdToFromEpoch(StartYearTime,MJDdate[index],"Days")+1;
                                    QList<double>DDD=sta::DayOfYearToDDD(DayOfYear[index]);
                                    stream<<YearPreLastDigit<<YearLastDigit;
                                    for(int i=0;i<DDD.size();i++)
                                    {
                                        stream<<DDD[i];
                                    }
                                    stream<<"\t";
				}

                                /*if(TimeCoordinate=="GMT")
				{

                                }*/
			    }

                            if(name=="Epoch")
                            {
                                QWidget*Box=treeWidgetShowInReport->itemWidget(parameter,2);
                                QComboBox*ComboBox=dynamic_cast <QComboBox*>(Box);
                                QString TimeUnits=ComboBox->currentText();

                                double ElapsedTime=MjdToFromEpoch(StartEpoch,MJDdate[index],TimeUnits);

                                if(TimeUnits=="Seconds")
                                {
                                    stream<<ElapsedTime<<"\t";
                                }
                                if(TimeUnits=="Minutes")
                                {
                                    stream<<ElapsedTime<<"\t";
                                }
                                if(TimeUnits=="Hours")
                                {
                                    stream<<ElapsedTime<<"\t";
                                }
                                if(TimeUnits=="Days")
                                {
                                    stream<<ElapsedTime<<"\t";
                                }
                            }

                            if(name=="Access Time")
                            {
                                /*
                                QList<int>AccessData=analysis::calcAccessTime(MJDdate[index],AccessNumber,AccessStep,CovIndex[3],LineOfCoverageReport);
                                if(AccessData.size()==1 && AccessData[0]==0)
                                {
                                    stream<<"No visibility"<<"\t";
                                }
                                if(AccessData.size()==2)
                                {
                                    stream<<AccessNumber<<"."<<AccessStep<<"\t";
                                    */
                                }


                            if((name=="Azimuth")||(name=="Elevation")||(name=="Range"))
                            {
                                QString ToCoord=analysis::ReadCoordinateSys(treeWidgetShowInReport,parameter);
                                QString ToUnit=analysis::ReadUnits(treeWidgetShowInReport,parameter);
				if(name=="Azimuth")
				{

				    if(CovIndex[0]<LineOfCoverageReport.size())
				    {
					QString Line=LineOfCoverageReport.at(CovIndex[0]);

					double TimeCovReport=(Line.section("\t",0,0)).toDouble();
					double Azimuth=(Line.section("\t",3,3)).toDouble();

					if(abs(MJDdate[index]-TimeCovReport)<10e-6)
					{

					    stream<<sta::ConvertUnits(ToUnit,Azimuth,"deg")<<"\t";
					    CovIndex[0]++;
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
				if(name=="Elevation")
				{

				    if(CovIndex[1]<LineOfCoverageReport.length())
				    {
					QString Line=LineOfCoverageReport.at(CovIndex[1]);

					double TimeCovReport=(Line.section("\t",0,0)).toDouble();
					double Elevation=(Line.section("\t",2,2)).toDouble();

					if(abs(MJDdate[index]-TimeCovReport)<10e-6)
					{
					    stream<<sta::ConvertUnits(ToUnit,Elevation,"deg")<<"\t";
					    CovIndex[1]++;
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
				if(name=="Range")
				{
				    if(CovIndex[2]<LineOfCoverageReport.length())
				    {
					QString Line=LineOfCoverageReport.at(CovIndex[2]);

					double TimeCovReport=(Line.section("\t",0,0)).toDouble();
					double Range=(Line.section("\t",1,1)).toDouble();

					if(abs(MJDdate[index]-TimeCovReport)<10e-6)
					{
					    stream<<sta::ConvertUnits(ToUnit,Range,"km")<<"\t";
					    CovIndex[2]++;
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
			    }
			    if(name=="Equivalent Isotropical Radiated Power")
			    {

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
                                        stream<<"No visibility"<<"\t";
				    }

				}
				else
				{
                                    stream<<"No visibility"<<"\t";
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
                                        stream<<"No visibility"<<"\t";
				    }

				}
				else
				{
                                    stream<<"No visibility"<<"\t";
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
                                        stream<<"No visibility"<<"\t";
				    }

				}
				else
				{
                                    stream<<"No visibility"<<"\t";
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
                                        stream<<"No visibility"<<"\t";
				    }

				}
				else
				{
                                    stream<<"No visibility"<<"\t";
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
                                        stream<<"No visibility"<<"\t";
				    }

				}
				else
				{
                                    stream<<"No visibility"<<"\t";
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
				QString ToCoord=analysis::ReadCoordinateSys(treeWidgetShowInReport,parameter);

				sta::StateVector Vector[inumber];
				Vector[index]=arc->trajectorySample(j);
				double Eccentricity=calcKeplerianElements(Vector[index],STA_SOLAR_SYSTEM->lookup("Earth"),"Eccentricity",
									  MJDdate[index],
									  "EME J2000",
									  ToCoord);
				stream<<Eccentricity<<"\t";
			    }
			    if((name=="Inclination")||
			       (name=="RAAN")||
			       (name=="Argument of Periapsis")||
			       (name=="True Anomaly")||
			       (name=="Semimajor Axis"))
			    {
				QString ToCoord=analysis::ReadCoordinateSys(treeWidgetShowInReport,parameter);
				QString ToUnit=analysis::ReadUnits(treeWidgetShowInReport,parameter);

				sta::StateVector Vector[inumber];
				Vector[index]=arc->trajectorySample(j);

				if(name=="Inclination")
				{
				    double Inclination=calcKeplerianElements(Vector[index],STA_SOLAR_SYSTEM->lookup("Earth"),name,
									     MJDdate[index],
									     "EME J2000",
									     ToCoord);
				    stream<<sta::ConvertUnits(ToUnit,Inclination,"rad")<<"\t";
				}
				if(name=="RAAN")
				{
				    double Ascending=calcKeplerianElements(Vector[index],STA_SOLAR_SYSTEM->lookup("Earth"),name,
									   MJDdate[index],
									   "EME J2000",
									   ToCoord);
				    stream<<sta::ConvertUnits(ToUnit,Ascending,"rad")<<"\t";
				}
				if(name=="True Anomaly")
				{
				    double TrueAnom=calcKeplerianElements(Vector[index],STA_SOLAR_SYSTEM->lookup("Earth"),name,
									  MJDdate[index],
									  "EME J2000",
									  ToCoord);
				    stream<<sta::ConvertUnits(ToUnit,TrueAnom,"rad")<<"\t";
				}
				if(name=="Argument of Periapsis")
				{
				    double Periapsis=calcKeplerianElements(Vector[index],STA_SOLAR_SYSTEM->lookup("Earth"),name,
									   MJDdate[index],
									   "EME J2000",
									   ToCoord);
				    stream<<sta::ConvertUnits(ToUnit,Periapsis,"rad")<<"\t";
				}
				if(name=="Semimajor Axis")
				{
				    double SemAxis=calcKeplerianElements(Vector[index],STA_SOLAR_SYSTEM->lookup("Earth"),name,
									 MJDdate[index],
									 "EME J2000",
									 ToCoord);
				    stream<<sta::ConvertUnits(ToUnit,SemAxis,"km")<<"\t";
				}

			    }
			    if((name=="l")||(name=="g")||(name=="h")||(name=="L")||(name=="G")||(name=="H"))
			    {
				QString ToCoord=analysis::ReadCoordinateSys(treeWidgetShowInReport,parameter);
				sta::StateVector Vector[inumber];
				Vector[index]=arc->trajectorySample(j);

				if(name=="l")
				{
				    double Delaunay_l=calcDelaunayElements(Vector[index],STA_SOLAR_SYSTEM->lookup("Earth"),name,
									   MJDdate[index],
									   "EME J2000",
									   ToCoord);
				    stream<<Delaunay_l<<"\t";
				}
				if(name=="g")
				{
				    double Delaunay_g=calcDelaunayElements(Vector[index],STA_SOLAR_SYSTEM->lookup("Earth"),name,
									   MJDdate[index],
									   "EME J2000",
									   ToCoord);
				    stream<<Delaunay_g<<"\t";
				}
				if(name=="h")
				{
				    double Delaunay_h=calcDelaunayElements(Vector[index],STA_SOLAR_SYSTEM->lookup("Earth"),name,
									   MJDdate[index],
									   "EME J2000",
									   ToCoord);
				    stream<<Delaunay_h<<"\t";
				}
				if(name=="L")
				{
				    double Delaunay_L=calcDelaunayElements(Vector[index],STA_SOLAR_SYSTEM->lookup("Earth"),name,
									   MJDdate[index],
									   "EME J2000",
									   ToCoord);
				    stream<<Delaunay_L<<"\t";
				}
				if(name=="G")
				{
				    double Delaunay_G=calcDelaunayElements(Vector[index],STA_SOLAR_SYSTEM->lookup("Earth"),name,
									   MJDdate[index],
									   "EME J2000",
									   ToCoord);
				    stream<<Delaunay_G<<"\t";
				}
				if(name=="H")
				{
				    double Delaunay_H=calcDelaunayElements(Vector[index],STA_SOLAR_SYSTEM->lookup("Earth"),name,
									   MJDdate[index],
									   "EME J2000",
									   ToCoord);
				    stream<<Delaunay_H<<"\t";
				}
			    }
			    if((name=="Latitude")||(name=="Longitude")||(name=="Radial Distance")||(name=="Flight Path Angle")||(name=="Heading Angle")||(name=="Velocity Modulus")||(name=="Altitude"))
			    {
				QString ToCoord=analysis::ReadCoordinateSys(treeWidgetShowInReport,parameter);
				QString Units=analysis::ReadUnits(treeWidgetShowInReport,parameter);

				sta::StateVector Vector[inumber];
				sta::StateVector ModifVector[inumber];
				Vector[index]=arc->trajectorySample(j);
				sta::CoordinateSystem EME2000("INERTIAL J2000");
				ModifVector[index]=CoordinateSystem::convert(Vector[index],
									     MJDdate[index],
									     STA_SOLAR_SYSTEM->lookup("Earth"),
									     EME2000,
									     STA_SOLAR_SYSTEM->lookup("Earth"),
									     analysis::CoordSys(ToCoord));
				double SphericalElements[6]; // tau, delta, r, V, gamma, chi
				cartesianTOspherical(ModifVector[index].position.x(),ModifVector[index].position.y(),ModifVector[index].position.z(),
						     ModifVector[index].velocity.x(),ModifVector[index].velocity.y(),ModifVector[index].velocity.z(),
						     SphericalElements[0],SphericalElements[1],SphericalElements[2],SphericalElements[3],SphericalElements[4],
						     SphericalElements[5]);
				if(name=="Latitude")
				{
				    stream<<sta::ConvertUnits(Units,SphericalElements[1],"rad")<<"\t";
				}
				if(name=="Longitude")
				{
				    stream<<sta::ConvertUnits(Units,SphericalElements[0],"rad")<<"\t";
				}
				if(name=="Radial Distance")
				{
				    stream<<sta::ConvertUnits(Units,SphericalElements[2],"km")<<"\t";
				}
				if(name=="Altitude")
				{
				    stream<<sta::ConvertUnits(Units,SphericalElements[2]-STA_SOLAR_SYSTEM->lookup("Earth")->meanRadius(),"km")<<"\t";
				}
				if(name=="Flight Path Angle")
				{
				    stream<<sta::ConvertUnits(Units,SphericalElements[4],"rad")<<"\t";
				}
				if(name=="Heading Angle")
				{
				    stream<<sta::ConvertUnits(Units,SphericalElements[5],"rad")<<"\t";
				}
				if(name=="Velocity Modulus")
				{

				    stream<<sta::ConvertUnits(Units,SphericalElements[3],"km/s")<<"\t";
				}
			    }

			    if((name=="e*sin(omegaBar)")||(name=="e*cos(omegaBar)")||(name=="tan(i/2)*sin(raan)")||(name=="tan(i/2)*cos(raan)")||(name=="Mean Longitude"))
			    {
				QString ToCoord=analysis::ReadCoordinateSys(treeWidgetShowInReport,parameter);
				QString Units=analysis::ReadCoordinateSys(treeWidgetShowInReport,parameter);

				sta::StateVector Vector[inumber];
				Vector[index]=arc->trajectorySample(j);
				if(name=="e*sin(omegaBar)")
				{
				    double esin=calcEquinoctialElements(Vector[index],STA_SOLAR_SYSTEM->lookup("Earth"),"e*sin(omegaBar)",
									MJDdate[index],
									"EME J2000",
									ToCoord);
				    //stream<<sta::ConvertUnits(Units,esin)<<"\t";
				    stream<<esin<<"\t";
                                }
				if(name=="e*cos(omegaBar)")
				{
				    double ecos=calcEquinoctialElements(Vector[index],STA_SOLAR_SYSTEM->lookup("Earth"),"e*cos(omegaBar)",
									MJDdate[index],
									"EME J2000",
									ToCoord);
				    // stream<<sta::ConvertUnits(Units,ecos)<<"\t";
				    stream<<ecos<<"\t";
				}
				if(name=="tan(i/2)*sin(raan)")
				{
				    double etansin=calcEquinoctialElements(Vector[index],STA_SOLAR_SYSTEM->lookup("Earth"),"tan(i/2)*sin(raan)",
									   MJDdate[index],
									   "EME J2000",
									   ToCoord);
				    // stream<<sta::ConvertUnits(Units,etansin)<<"\t";
				    stream<<etansin<<"\t";
				}
				if(name=="tan(i/2)*cos(raan)")
				{
				    double etancos=calcEquinoctialElements(Vector[index],STA_SOLAR_SYSTEM->lookup("Earth"),"tan(i/2)*cos(raan)",
									   MJDdate[index],
									   "EME J2000",
									   ToCoord);
				    //stream<<sta::ConvertUnits(Units,etancos)<<"\t";
				    stream<<etancos<<"\t";
				}
				if(name=="Mean Longitude")
				{
				    double MeanLon=calcEquinoctialElements(Vector[index],STA_SOLAR_SYSTEM->lookup("Earth"),"Mean Longitude",
									   MJDdate[index],
									   "EME J2000",
									   ToCoord);
				    //stream<<sta::ConvertUnits(Units,MeanLon)<<"\t";
				    stream<<MeanLon<<"\t";
				}
			    }
			}
			stream<<"\r\n";
			numberOfRows = numberOfRows + 1;
		    }
		}
		else
		{
		    stream<<"#######Beginning of time"<<" "<<(k+1)<<"######"<<"\r\n";
		    stream<<"No data available for the chosen time interval, please check the options of the propagation"<<"\r\n";
		}
	    } //

	    stream<<"\r\n";
	}
    }

    file.close();

    // Patched by Guillermo to allow read of files in MAC and Linux
    QString ResourcesPathOutput = QDir::currentPath ();
    QString analysisFileOutput = ResourcesPathOutput + "/" + "analysisReport.txt";
    QString analysisFileOutputURL = "file:///" + analysisFileOutput;

    SpreadSheet* resultsSheet = new SpreadSheet(numberOfRows + 5, numberOfColumns);
    resultsSheet->setupContents(analysisFileOutput);
    resultsSheet->setWindowIcon(QPixmap(":/icons/CoordinateSystemBody.png"));
    resultsSheet->setWindowTitle("Analsyis");
    resultsSheet->resize(640, 420);
    resultsSheet->setWindowModality(Qt::NonModal);
    resultsSheet->activateWindow();
    resultsSheet->show();
    resultsSheet->raise();

    // Guillermo says: next line to used any more since we have now a spreadsheet table
    //QDesktopServices::openUrl(QUrl(analysisFileOutputURL));

}

QList< analysis::AnalysisData> analysis::WriteDataStructure(QList<QTreeWidgetItem*>selected,QList<QTreeWidgetItem*>selectedTimes)
{
    /*
      generates the data structure to send the information to the plotting module
      Inputs: selected- list of the selected lines in the scenario tree of the AM GUI, selectedTimes- list of the selected time intervals
      */
    QList<AnalysisData>DataStructure;
    //QList<QString>Titles;
    //QList<double>LineData;
    //QList<QList<double> >DisplayData;
    //AnalysisData ToStruct;

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

            TxParentIndex=selected.at(t)->child(1)->text(0).toInt();

            TxArrayIndex=selected.at(t)->child(2)->text(0).toInt();

            int TxParentTy=selected.at(t)->child(0)->text(0).toInt();



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

            }
        }

        if(selected.at(t)->child(childnum-1)->text(0)=="Rx")
        {
            RxParentIndex=selected.at(t)->child(childnum-3)->text(0).toInt();
            RxArrayIndex=selected.at(t)->child(childnum-2)->text(0).toInt();
            int RxParentTy=(selected.at(t)->child(0)->text(0)).toInt();
            if(RxParentTy==0)
            {

                indGS=RxParentIndex;
                Receiver=RxGS.at(RxArrayIndex);
                Environment=GSEnvironment.at(indGS);
                RxParentType=false;

            }
            //indGS=0;
            if(RxParentTy==1)
            {

                indSC=RxParentIndex;
                Receiver=RxSC.at(RxArrayIndex);
                RxParentType=true;
            }
        }

    }

    QList<QTreeWidget*>Tree;

    Tree.append(treeWidgetXaxis);
    Tree.append(treeWidgetYaxis);
    Tree.append(treeWidgetZaxis);

    for(int a=0;a<Tree.size();a++)
    {
	QList<double>LineData;
	AnalysisData ToStruct;
	QList<QList<double> >DisplayData;
	QList<QString>Titles;
        QString PlotTitle;
        // checks parameters to be plotted and the module where they come from, for the time being, only for Coverage and Communication data

	QList<QTreeWidgetItem*>PlotParameters=Tree.at(a)->selectedItems();
	for(int i=0;i<PlotParameters.size();i++) //sets true some bool

	{
	    QTreeWidgetItem*parameter=PlotParameters.at(i);
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
	    {
		ReadCommunication3=true;
	    }
	}

        for(int z=0;z<MParentIndex.size();z++) //for each mission arc
	{

	    SpaceObject*spaceObj=m_propagatedScenario->spaceObjects().at(MParentIndex.at(z));

	    {
		int indMissionArc=MObjectsIndex.at(z);
		MissionArc*arc=spaceObj->mission().at(MObjectsIndex.at(z));

		double StartTime[selectedTimes.size()]; // time specifications, for each line in the time tree
		double StopTime[selectedTimes.size()];
		int countStart[selectedTimes.size()]; // time step index from where the data is being collected
		int countStop[selectedTimes.size()];

		ReadTime(0,StartTime);//in MJD, reads the time in the treeWidgetTimeSpecifications
		ReadTime(1,StopTime); //in MJD

		double StartEpoch=arc->trajectorySampleTime(0); //beginning of mission
		int totalTime=(arc->trajectorySampleCount())-1; //position of the last sample time
		double StopEpoch=arc->trajectorySampleTime(totalTime); //end of mission

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
		    int ControlStart=0;
		    int ControlStop=0;
		    if ((StopTime[k]>(StopEpoch+2*pow(10,-5)))||(StopTime[k]<=(StartEpoch-2*pow(10,-5))))
		    {
			ControlStart++;
		    }
		    if ((StopTime[k]>(StopEpoch+2*pow(10,-5)))||(StopTime[k]<=(StartEpoch-2*pow(10,-5))))
		    {
			ControlStop++;
		    }

		    if((ControlStop==0)&&(ControlStop==0)) // there is enought data to continue calculations

		    {

			int i=0;
			while(( (arc->trajectorySampleTime(i)))<(StartTime[k]))
			{

			    i=i+1;
			}
			countStart[k]=i;
			int m=0;
			while(((arc->trajectorySampleTime(m)))<(StopTime[k]-pow(10,-5)))
			{
			    m=m+1;
			}
			countStop[k]=m;
			int inumber=countStop[k]-countStart[k]+1;

			QString Start=selectedTimes[k]->text(0);
			QString Stop=selectedTimes[k]->text(1);

			// WRITE stream<<"Satellite:"<<MParentIndex.at(z)+1<<"\t"<<"Mission Arc:"<<indMissionArc+1<<"\r\n";
			//WRITE stream<<"#######Beginning of time"<<" "<<(k+1)<<"######"<<"\r\n"<<"######"<<"Start Time"<<" "<<Start<<"\t"<<"Stop Time"<<" "<<Stop<<"######"<<"\r\n"<<"MJD"<<"\r\t";

			QList<QTreeWidgetItem*>SelParameters=Tree.at(a)->selectedItems();
			for(int i=0;i<SelParameters.size();i++)
			{
                            //QString name=SelParameters.at(i)->text(0);
                            QString name=analysis::ReadParameter(SelParameters.at(i));
                            QString Unit=analysis::ReadUnits(Tree.at(a),SelParameters.at(i));

                            QString TitleParameter=((name.append("(")).append(Unit)).append(")");

                            Titles.append(TitleParameter);  //labels of the displayed parameters

			}
                        for(int j=0; j<Titles.size();j++)
                        {
                            PlotTitle.append(Titles[j]);
                            if(j!=Titles.size()-1)
                            {
                                PlotTitle.append("versus");
                            }
                        }
			ToStruct.ParameterTitles=Titles;


			double MJDdate[inumber];
			double JulianDate[inumber];
			QDateTime TimeDateVector[inumber];
			double DayOfYear[inumber];

			for(int j=countStart[k];j<=countStop[k];j++)
			{

			    int index=j-countStart[k];

			    MJDdate[index]=arc->trajectorySampleTime(j);

			    //WRITE stream<<MJDdate[index]<<"\t"; //prints MJD by default

			    for(int i=0;i<SelParameters.size();i++)
			    {

                                QTreeWidgetItem*parameter=SelParameters.at(i);
                                //QString name=parameter->text(0);
                                QString name=analysis::ReadParameter(SelParameters.at(i));

				if((name=="x position")||(name=="y position")||(name=="z position")||(name=="x velocity")||(name=="y velocity")||(name=="z velocity"))
				{

                                    QString Coordinate=analysis::ReadCoordinateSys(Tree.at(a),SelParameters.at(i));
                                    QString Units=analysis::ReadUnits(Tree.at(a),SelParameters.at(i));

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
					    // stream<<"\t"<<sta::ConvertUnits(Units,ModifVector[index].position.x())<<"\t";
					    //LineData.append(sta::ConvertUnits(Units,ModifVector[index].position.x()));
					    LineData.append(sta::ConvertUnits(Units,ModifVector[index].position.x(),"km"));

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
					    //stream<<"\t"<<sta::ConvertUnits(Units,ModifVector[index].position.x())<<"\t";
					    LineData.append(sta::ConvertUnits(Units,ModifVector[index].position.x(),"km"));
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

					    //stream<<"\t"<<sta::ConvertUnits(Units,ModifVector[index].position.x())<<"\t";
					    //DataStructure[a].Data[index].append(sta::ConvertUnits(Units,ModifVector[index].position.x()));

					    LineData.append(sta::ConvertUnits(Units,ModifVector[index].position.x(),"km"));

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
					    //stream<<"\t"<<sta::ConvertUnits(Units,ModifVector[index].position.x())<<"\t";
					    LineData.append(sta::ConvertUnits(Units,ModifVector[index].position.x(),"km"));
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
					    //stream<<"\t"<<sta::ConvertUnits(Units,ModifVector[index].position.y())<<"\t";
					    LineData.append(sta::ConvertUnits(Units,ModifVector[index].position.y(),"km"));
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
					    //stream<<"\t"<<sta::ConvertUnits(Units,ModifVector[index].position.y())<<"\t";
					    LineData.append(sta::ConvertUnits(Units,ModifVector[index].position.y(),"km"));
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
					    //stream<<"\t"<<sta::ConvertUnits(Units,ModifVector[index].position.y())<<"\t";
					    LineData.append(sta::ConvertUnits(Units,ModifVector[index].position.y(),"km"));
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
					    //stream<<"\t"<<sta::ConvertUnits(Units,ModifVector[index].position.y())<<"\t";
					    LineData.append(sta::ConvertUnits(Units,ModifVector[index].position.y(),"km"));
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

					    //stream<<"\t"<<sta::ConvertUnits(Units,ModifVector[index].position.z())<<"\t";
					    LineData.append(sta::ConvertUnits(Units,ModifVector[index].position.z(),"km"));
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
					    //stream<<"\t"<<sta::ConvertUnits(Units,ModifVector[index].position.z())<<"\t";
					    LineData.append(sta::ConvertUnits(Units,ModifVector[index].position.z(),"km"));
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
					    //stream<<"\t"<<sta::ConvertUnits(Units,ModifVector[index].position.z())<<"\t";
					    LineData.append(sta::ConvertUnits(Units,ModifVector[index].position.z(),"km"));
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
					    //stream<<"\t"<<sta::ConvertUnits(Units,ModifVector[index].position.z())<<"\t";
					    LineData.append(sta::ConvertUnits(Units,ModifVector[index].position.z(),"km"));
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
					    //stream<<"\t"<<sta::ConvertUnits(Units,ModifVector[index].velocity.x())<<"\t";
					    LineData.append(sta::ConvertUnits(Units,ModifVector[index].velocity.x(),"km/s"));
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
					    //stream<<"\t"<<sta::ConvertUnits(Units,ModifVector[index].velocity.x())<<"\t";
					    LineData.append(sta::ConvertUnits(Units,ModifVector[index].velocity.x(),"km/s"));
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
					    //stream<<"\t"<<sta::ConvertUnits(Units,ModifVector[index].velocity.x())<<"\t";
					    LineData.append(sta::ConvertUnits(Units,ModifVector[index].velocity.x(),"km/s"));
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
					    //stream<<"\t"<<sta::ConvertUnits(Units,ModifVector[index].velocity.x())<<"\t";
					    LineData.append(sta::ConvertUnits(Units,ModifVector[index].velocity.x(),"km/s"));
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
					    //stream<<"\t"<<sta::ConvertUnits(Units,ModifVector[index].velocity.y())<<"\t";
					    LineData.append(sta::ConvertUnits(Units,ModifVector[index].velocity.y(),"km/s"));
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
					    //stream<<"\t"<<sta::ConvertUnits(Units,ModifVector[index].velocity.y())<<"\t";
					    LineData.append(sta::ConvertUnits(Units,ModifVector[index].velocity.y(),"km/s"));
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
					    //stream<<"\t"<<sta::ConvertUnits(Units,ModifVector[index].velocity.y())<<"\t";
					    LineData.append(sta::ConvertUnits(Units,ModifVector[index].velocity.y(),"km/s"));
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
					    //stream<<"\t"<<sta::ConvertUnits(Units,ModifVector[index].velocity.y())<<"\t";
					    LineData.append(sta::ConvertUnits(Units,ModifVector[index].velocity.y(),"km/s"));
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
					    //stream<<"\t"<<sta::ConvertUnits(Units,ModifVector[index].velocity.z())<<"\t";
					    LineData.append(sta::ConvertUnits(Units,ModifVector[index].velocity.z(),"km/s"));
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
					    //stream<<"\t"<<sta::ConvertUnits(Units,ModifVector[index].velocity.z())<<"\t";
					    LineData.append(sta::ConvertUnits(Units,ModifVector[index].velocity.z(),"km/s"));
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
					    //stream<<"\t"<<sta::ConvertUnits(Units,ModifVector[index].velocity.z())<<"\t";
					    LineData.append(sta::ConvertUnits(Units,ModifVector[index].velocity.z(),"km/s"));
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
					    //stream<<"\t"<<sta::ConvertUnits(Units,ModifVector[index].velocity.z())<<"\t";
					    LineData.append(sta::ConvertUnits(Units,ModifVector[index].velocity.z(),"km/s"));
					}
				    }
				}
				if(name=="Time")
				{

                                    QString TimeCoordinate=analysis::ReadUnits(Tree.at(a),SelParameters.at(i));

				    //Options of Time
				    if(TimeCoordinate=="MJD")
				    {
					LineData.append(MJDdate[index]);
				    }
				    if(TimeCoordinate=="Julian Date")
				    {
					JulianDate[index]=sta::MjdToJd(MJDdate[index]);
					LineData.append(JulianDate[index]);
				    }
				    if(TimeCoordinate=="Julian UTC") //not available to plot
				    {
                                        //format:DayOfYear/YY UTCTime
                                        JulianDate[index]=sta::MjdToJd(MJDdate[index]+0.00001);
                                        TimeDateVector[index]=sta::JdToCalendar(JulianDate[index]);
                                        int Year=TimeDateVector[index].date().year();
                                        QString YearPreLastDigit=QString::number(Year).at(2);
                                        QString YearLastDigit=QString::number(Year).at(3);
                                        QString DDD=sta::calendarToDayOfYear(TimeDateVector[index]);
                                        QString Time = TimeDateVector[index].time().toString(Qt::TextDate);
					//stream<<DayOfYear<<"/"<<YearPreLastDigit<<YearLastDigit<<" "<<TimeDateVector[index].time().hour()<<":"<<TimeDateVector[index].time().minute()<<":"<<TimeDateVector[index].time().second()<<"\t";
				    }
				    if(TimeCoordinate=="Gregorian LCL")  //not available to plot
				    {
                                        QDateTime CurrentDate=QDateTime::currentDateTime();
                                        QDateTime CurrentUTC=CurrentDate.toUTC();
                                        double DisplayDate[inumber];
                                        QDateTime DisplayDateCalendar[inumber];
                                        double CurrentDateInMJD=sta::JdToMjd(sta::CalendarToJd(CurrentDate));
                                        double CurrentUtcInMJD=sta::JdToMjd(sta::CalendarToJd(CurrentUTC));

                                        if(CurrentUtcInMJD-CurrentDateInMJD<0)

                                        {
                                            DisplayDate[index]=MJDdate[index]+(CurrentDateInMJD-CurrentUtcInMJD)+0.00001;
                                        }
                                        else
                                        {
                                            DisplayDate[index]=MJDdate[index]-(CurrentUtcInMJD-CurrentDateInMJD)+0.00001;
                                        }
                                        DisplayDateCalendar[index]=sta::JdToCalendar(sta::MjdToJd(DisplayDate[index]));

                                        QList<QString>Date= sta::TimeLayout(DisplayDateCalendar[index].date().day(),DisplayDateCalendar[index].date().month());
                                        QString Time=DisplayDateCalendar[index].time().toString(Qt::TextDate);
				    }
				    if(TimeCoordinate=="Gregorian UTC") // not available to plot
				    {

                                        JulianDate[index]=sta::MjdToJd(MJDdate[index])+0.00001;
                                        TimeDateVector[index]=sta::JdToCalendar(JulianDate[index]);
                                        QList<QString>Date=sta::TimeLayout(TimeDateVector[index].date().day(),TimeDateVector[index].date().month());
                                        QString Time=TimeDateVector[index].time().toString(Qt::TextDate);
				    }
				    if(TimeCoordinate=="Julian LCL") // not available to plot
				    {
                                        QDateTime CurrentDate=QDateTime::currentDateTime();
                                        QDateTime CurrentUTC=CurrentDate.toUTC();
                                        double DisplayDate[inumber];
                                        QDateTime DisplayDateCalendar[inumber];
                                        double CurrentDateInMJD=sta::JdToMjd(sta::CalendarToJd(CurrentDate));
                                        double CurrentUtcInMJD=sta::JdToMjd(sta::CalendarToJd(CurrentUTC));

                                        if(CurrentUtcInMJD-CurrentDateInMJD<0)

                                        {
                                            DisplayDate[index]=MJDdate[index]+(CurrentDateInMJD-CurrentUtcInMJD)+0.00001;
                                        }
                                        else
                                        {
                                            DisplayDate[index]=MJDdate[index]-(CurrentUtcInMJD-CurrentDateInMJD)+0.00001;
                                        }
                                        DisplayDateCalendar[index]=sta::JdToCalendar(sta::MjdToJd(DisplayDate[index]));

                                        QString DDD=sta::calendarToDayOfYear(sta::JdToCalendar(sta::MjdToJd(MJDdate[index]+0.00001)));
                                        int Year=(sta::JdToCalendar(sta::MjdToJd(MJDdate[index]))).date().year();
                                        QString YearPreLastDigit=QString::number(Year).at(2);
                                        QString YearLastDigit=QString::number(Year).at(3);

                                        QString Time=DisplayDateCalendar[index].time().toString(Qt::TextDate);
				    }

                                    if(TimeCoordinate=="Mission Elapsed") //not available to plot
				    {
                                        QString Elapsed= sta::MissionElapsedTime(MJDdate[index],StartEpoch);
				    }
                                    if(TimeCoordinate=="YYDDD") //not available to plot
				    {
                                        JulianDate[index]=sta::MjdToJd(MJDdate[index]+0.00001);
                                        TimeDateVector[index]=sta::JdToCalendar(JulianDate[index]);
                                        int Year=TimeDateVector[index].date().year();
                                        QDateTime FirstDayCurrentYear(QDate(Year,1,1),QTime(0,0,0));
                                        double StartYearTime=sta::JdToMjd(sta::CalendarToJd(FirstDayCurrentYear));
                                        QString YearPreLastDigit=QString::number(Year).at(2);
                                        QString YearLastDigit=QString::number(Year).at(3);

                                        DayOfYear[index]=sta::MjdToFromEpoch(StartYearTime,MJDdate[index],"Days")+1;
                                        QList<double>DDD=sta::DayOfYearToDDD(DayOfYear[index]);

				    }

                                   /* if(TimeCoordinate=="GMT")
				    {

                                    }*/
				}

                                if(name=="Epoch")
                                {
                                    QWidget*Box=Tree.at(a)->itemWidget(parameter,2);
                                    QComboBox*ComboBox=dynamic_cast <QComboBox*>(Box);
                                    QString TimeUnits=ComboBox->currentText();

                                    double ElapsedTime=MjdToFromEpoch(StartEpoch,MJDdate[index],TimeUnits);

                                    if(TimeUnits=="Seconds")
                                    {
                                        LineData.append(ElapsedTime);
                                    }
                                    if(TimeUnits=="Minutes")
                                    {
                                        LineData.append(ElapsedTime);
                                    }
                                    if(TimeUnits=="Hours")
                                    {
                                        LineData.append(ElapsedTime);
                                    }
                                    if(TimeUnits=="Days")
                                    {
                                        LineData.append(ElapsedTime);
                                    }
                                }

				if((name=="Azimuth")||(name=="Elevation")||(name=="Range"))
				{
                                    QString ToCoord=analysis::ReadCoordinateSys(Tree.at(a),SelParameters.at(i));

                                    QString ToUnit=analysis::ReadUnits(Tree.at(a),SelParameters.at(i));
				    if(name=="Azimuth")
				    {

					if(CovIndex[0]<LineOfCoverageReport.size())
					{
					    QString Line=LineOfCoverageReport.at(CovIndex[0]);

					    double TimeCovReport=(Line.section("\t",0,0)).toDouble();
					    double Azimuth=(Line.section("\t",3,3)).toDouble();

					    if(abs(MJDdate[index]-TimeCovReport)<10e-6)
					    {

						LineData.append(sta::ConvertUnits(ToUnit,Azimuth,"deg"));

						CovIndex[0]++;
					    }

					    else
					    {
						//stream<<"No visibility";
						LineData.append(-1);
					    }

					}
					else
					{
					    //stream<<"No visibility";
					    LineData.append(-1);
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
						//stream<<Elevation<<"\t";
						LineData.append(sta::ConvertUnits(ToUnit,Elevation,"deg"));
						CovIndex[1]++;
					    }

					    else
					    {
						//<<"No visibility";
						LineData.append(-1);
					    }

					}
					else
					{
					    //stream<<"No visibility";
					    LineData.append(-1);
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
						//stream<<Range<<"\t";

						LineData.append(sta::ConvertUnits(ToUnit,Range,"km"));
						CovIndex[2]++;
					    }

					    else
					    {
						//stream<<"No visibility";
						LineData.append(-1);
					    }

					    //qDebug()<<"CovIndex"<<CovIndex[2];

					}
					else
					{
					    // stream<<"No visibility";
					    LineData.append(-1);
					}
				    }
				}
				if(name=="Equivalent Isotropical Radiated Power")
				{

				    if(Comm1Index[0]<LineOfComm1Report.length())
				    {
					QString Line=LineOfComm1Report.at(Comm1Index[0]);

					double TimeCommReport=(Line.section("\t",0,0)).toDouble();
					double EIRP=(Line.section("\t",1,1)).toDouble();

					if(abs(MJDdate[index]-TimeCommReport)<10e-6)
					{
					    //stream<<EIRP<<"\t";
					    LineData.append(EIRP);
					    Comm1Index[0]++;
					}

					else
					{
					    //stream<<"No visibility";
					    LineData.append(-1);
					}

				    }
				    else
				    {
					//stream<<"No visibility";
					LineData.append(-1);
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
					    //stream<<RcvFreq<<"\t";
					    LineData.append(RcvFreq);
					    Comm1Index[1]++;
					}

					else
					{
					    //stream<<"No visibility";
					    LineData.append(-1);
					}

				    }
				    else
				    {
					//stream<<"No visibility";
					LineData.append(-1);
				    }

				}
				if(name=="Doppler Shift")
				{

				    if(Comm1Index[2]<LineOfComm1Report.length())
				    {
					QString Line=LineOfComm1Report.at(Comm1Index[2]);

					double TimeCommReport=(Line.section("\t",0,0)).toDouble();
					double Doppler=(Line.section("\t",3,3)).toDouble();

					if(abs(MJDdate[index]-TimeCommReport)<10e-6)
					{
					    //stream<<RcvFreq<<"\t";
					    LineData.append(Doppler);
					    Comm1Index[2]++;
					}

					else
					{
					    //stream<<"No visibility";
					    LineData.append(-1);
					}

				    }
				    else
				    {
					//stream<<"No visibility";
					LineData.append(-1);
				    }
				}
				if(name=="Received Power")
				{

				    if(Comm1Index[3]<LineOfComm1Report.length())
				    {
					QString Line=LineOfComm1Report.at(Comm1Index[3]);

					double TimeCommReport=(Line.section("\t",0,0)).toDouble();
					double RcvPower=(Line.section("\t",4,4)).toDouble();

					if(abs(MJDdate[index]-TimeCommReport)<10e-6)
					{
					    //stream<<RcvpOWER<<"\t";
					    LineData.append(RcvPower);
					    Comm1Index[3]++;
					}

					else
					{
					    //stream<<"No visibility";
					    LineData.append(-1);
					}

				    }
				    else
				    {
					//stream<<"No visibility";
					LineData.append(-1);
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
					    //stream<<FluxDensity<<"\t";
					    LineData.append(FluxDensity);
					    Comm1Index[4]++;
					}

					else
					{
					    //stream<<"No visibility";
					    LineData.append(-1);
					}

				    }
				    else
				    {
					//stream<<"No visibility";
					LineData.append(-1);
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
					    //stream<<OvBWF<<"\t";
					    LineData.append(OvBWF);
					    Comm1Index[5]++;
					}

					else
					{
					    // stream<<"No visibility"<<"\t";
					    LineData.append(-1);
					}
				    }
				    else
				    {
					//stream<<"No visibility"<<"\t";
					LineData.append(-1);
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
					    //stream<<FSL<<"\t";
					    LineData.append(FSL);
					    Comm2Index[0]++;
					}

					else
					{
					    // stream<<"No visibility"<<"\t";
					    LineData.append(-1);
					}

				    }
				    else
				    {
					//stream<<"No visibility"<<"\t";
					LineData.append(-1);
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
					    //stream<<OxLoss<<"\t";
					    LineData.append(OxLoss);
					    Comm2Index[1]++;
					}

					else
					{
					    //stream<<"No visibility"<<"\t";
					    LineData.append(-1);
					}
				    }
				    else
				    {
					//stream<<"No visibility"<<"\t";
					LineData.append(-1);
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
					    //stream<<WVLoss<<"\t";
					    LineData.append(WVLoss);
					    Comm2Index[2]++;
					}

					else
					{
					    //stream<<"No visibility"<<"\t";
					    LineData.append(-1);
					}
				    }
				    else
				    {
					// stream<<"No visibility"<<"\t";
					LineData.append(-1);
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
					    //stream<<RainLoss<<"\t";
					    LineData.append(RainLoss);
					    Comm2Index[3]++;
					}

					else
					{
					    //stream<<"No visibility"<<"\t";
					    LineData.append(-1);
					}
				    }
				    else
				    {
					//stream<<"No visibility"<<"\t";
					LineData.append(-1);
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
					    //stream<<AtmLoss<<"\t";
					    LineData.append(AtmLoss);
					    Comm2Index[4]++;
					}

					else
					{
					    //stream<<"No visibility"<<"\t";
					    LineData.append(-1);
					}
				    }
				    else
				    {
					//stream<<"No visibility"<<"\t";
					LineData.append(-1);
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
					    // stream<<PropLoss<<"\t";
					    LineData.append(PropLoss);
					    Comm2Index[5]++;
					}

					else
					{
					    //stream<<"No visibility"<<"\t";
					    LineData.append(-1);
					}
				    }
				    else
				    {
					//stream<<"No visibility"<<"\t";
					LineData.append(-1);
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
					    //stream<<GT<<"\t";
					    LineData.append(GT);
					    Comm3Index[0]++;
					}

					else
					{
					    //stream<<"No visibility"<<"\t";
					    LineData.append(-1);
					}
				    }
				    else
				    {
					//stream<<"No visibility"<<"\t";
					LineData.append(-1);
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
					    // stream<<CNo<<"\t";
					    LineData.append(CNo);
					    Comm3Index[1]++;
					}

					else
					{
					    //stream<<"No visibility"<<"\t";
					    LineData.append(-1);
					}
				    }
				    else
				    {
					// stream<<"No visibility"<<"\t";
					LineData.append(-1);
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
					    //stream<<CN<<"\t";
					    LineData.append(CN);
					    Comm3Index[2]++;
					}

					else
					{
					    // stream<<"No visibility"<<"\t";
					    LineData.append(-1);
					}
				    }
				    else
				    {
					//stream<<"No visibility"<<"\t";
					LineData.append(-1);
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
					    //stream<<EbNo<<"\t";
					    LineData.append(EbNo);
					    Comm3Index[3]++;
					}

					else
					{
					    //stream<<"No visibility"<<"\t";
					    LineData.append(-1);
					}
				    }
				    else
				    {
					//stream<<"No visibility"<<"\t";
					LineData.append(-1);
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
					    // stream<<BER<<"\t";
					    LineData.append(BER);
					    Comm3Index[4]++;
					}

					else
					{
					    //stream<<"No visibility"<<"\t";
					    LineData.append(-1);
					}
				    }
				    else
				    {
					//stream<<"No visibility"<<"\t";
					LineData.append(-1);
				    }
				}
				if(name=="Eccentricity")
                                {
                                    QString ToCoord=analysis::ReadCoordinateSys(Tree.at(a),SelParameters.at(i));

				    sta::StateVector Vector[inumber];
				    Vector[index]=arc->trajectorySample(j);
				    double Eccentricity=calcKeplerianElements(Vector[index],STA_SOLAR_SYSTEM->lookup("Earth"),"Eccentricity",
									      MJDdate[index],
									      "EME J2000",
									      ToCoord);
				    //stream<<Eccentricity<<"\t";
				    LineData.append(Eccentricity);
				}
				if((name=="Inclination")||
				   (name=="RAAN")||
				   (name=="Argument of Periapsis")||
				   (name=="True Anomaly")||
				   (name=="Semimajor Axis"))
                                {
                                    QString ToCoord=analysis::ReadCoordinateSys(Tree.at(a),SelParameters.at(i));
                                    QString ToUnit=analysis::ReadUnits(Tree.at(a),SelParameters.at(i));

				    sta::StateVector Vector[inumber];
				    Vector[index]=arc->trajectorySample(j);

				    if(name=="Inclination")
				    {
					double Inclination=calcKeplerianElements(Vector[index],STA_SOLAR_SYSTEM->lookup("Earth"),name,
										 MJDdate[index],
										 "EME J2000",
										 ToCoord);
					//stream<<sta::ConvertUnits(ToUnit,Inclination)<<"\t";
					LineData.append(sta::ConvertUnits(ToUnit,Inclination,"rad"));
				    }
				    if(name=="RAAN")
				    {
					double Ascending=calcKeplerianElements(Vector[index],STA_SOLAR_SYSTEM->lookup("Earth"),name,
									       MJDdate[index],
									       "EME J2000",
									       ToCoord);
					//stream<<sta::ConvertUnits(ToUnit,Ascending)<<"\t";
					LineData.append(sta::ConvertUnits(ToUnit,Ascending,"rad"));
				    }
				    if(name=="True Anomaly")
				    {
					double TrueAnom=calcKeplerianElements(Vector[index],STA_SOLAR_SYSTEM->lookup("Earth"),name,
									      MJDdate[index],
									      "EME J2000",
									      ToCoord);
					//stream<<sta::ConvertUnits(ToUnit,TrueAnom)<<"\t";
					LineData.append(sta::ConvertUnits(ToUnit,TrueAnom,"rad"));
				    }
				    if(name=="Argument of Periapsis")
				    {
					double Periapsis=calcKeplerianElements(Vector[index],STA_SOLAR_SYSTEM->lookup("Earth"),name,
									       MJDdate[index],
									       "EME J2000",
									       ToCoord);
					//stream<<sta::ConvertUnits(ToUnit,Periapsis)<<"\t";
					LineData.append(sta::ConvertUnits(ToUnit,Periapsis,"rad"));
				    }
				    if(name=="Semimajor Axis")
				    {
					double SemAxis=calcKeplerianElements(Vector[index],STA_SOLAR_SYSTEM->lookup("Earth"),name,
									     MJDdate[index],
									     "EME J2000",
									     ToCoord);
					//stream<<sta::ConvertUnits(ToUnit,SemAxis)<<"\t";
					LineData.append(sta::ConvertUnits(ToUnit,SemAxis,"km"));
				    }

				}
				if((name=="l")||(name=="g")||(name=="h")||(name=="L")||(name=="G")||(name=="H"))
				{

                                    QString ToCoord=analysis::ReadCoordinateSys(Tree.at(a),SelParameters.at(i));
				    sta::StateVector Vector[inumber];
				    Vector[index]=arc->trajectorySample(j);

				    if(name=="l")
				    {
					double Delaunay_l=calcDelaunayElements(Vector[index],STA_SOLAR_SYSTEM->lookup("Earth"),name,
									       MJDdate[index],
									       "EME J2000",
									       ToCoord);
					//stream<<Delaunay_l<<"\t";
					LineData.append(Delaunay_l);
				    }
				    if(name=="g")
				    {
					double Delaunay_g=calcDelaunayElements(Vector[index],STA_SOLAR_SYSTEM->lookup("Earth"),name,
									       MJDdate[index],
									       "EME J2000",
									       ToCoord);
					//stream<<Delaunay_g<<"\t";
					LineData.append(Delaunay_g);
				    }
				    if(name=="h")
				    {
					double Delaunay_h=calcDelaunayElements(Vector[index],STA_SOLAR_SYSTEM->lookup("Earth"),name,
									       MJDdate[index],
									       "EME J2000",
									       ToCoord);
					//stream<<Delaunay_h<<"\t";
					LineData.append(Delaunay_h);
				    }
				    if(name=="L")
				    {
					double Delaunay_L=calcDelaunayElements(Vector[index],STA_SOLAR_SYSTEM->lookup("Earth"),name,
									       MJDdate[index],
									       "EME J2000",
									       ToCoord);
					//stream<<Delaunay_L<<"\t";
					LineData.append(Delaunay_L);
				    }
				    if(name=="G")
				    {
					double Delaunay_G=calcDelaunayElements(Vector[index],STA_SOLAR_SYSTEM->lookup("Earth"),name,
									       MJDdate[index],
									       "EME J2000",
									       ToCoord);
					//stream<<Delaunay_G<<"\t";
					LineData.append(Delaunay_G);
				    }
				    if(name=="H")
				    {
					double Delaunay_H=calcDelaunayElements(Vector[index],STA_SOLAR_SYSTEM->lookup("Earth"),name,
									       MJDdate[index],
									       "EME J2000",
									       ToCoord);
					// stream<<Delaunay_H<<"\t";
					LineData.append(Delaunay_H);
				    }
                                }
				if((name=="Latitude")||(name=="Longitude")||(name=="Radial Distance")||(name=="Flight Path Angle")||(name=="Heading Angle")||(name=="Velocity Modulus")||(name=="Altitude"))
                                {
                                    QString ToCoord=analysis::ReadCoordinateSys(Tree.at(a),SelParameters.at(i));
                                    QString Units=analysis::ReadUnits(Tree.at(a),SelParameters.at(i));

				    sta::StateVector Vector[inumber];
				    sta::StateVector ModifVector[inumber];
				    Vector[index]=arc->trajectorySample(j);
				    sta::CoordinateSystem EME2000("INERTIAL J2000");
				    ModifVector[index]=CoordinateSystem::convert(Vector[index],
										 MJDdate[index],
										 STA_SOLAR_SYSTEM->lookup("Earth"),
										 EME2000,
										 STA_SOLAR_SYSTEM->lookup("Earth"),
										 analysis::CoordSys(ToCoord));
				    double SphericalElements[6]; // tau, delta, r, V, gamma, chi

				    cartesianTOspherical(ModifVector[index].position.x(),ModifVector[index].position.y(),ModifVector[index].position.z(),
							 ModifVector[index].velocity.x(),ModifVector[index].velocity.y(),ModifVector[index].velocity.z(),
							 SphericalElements[0],SphericalElements[1],SphericalElements[2],SphericalElements[3],SphericalElements[4],
							 SphericalElements[5]);

				    if(name=="Latitude")
				    {
					//stream<<sta::ConvertUnits(Units,SphericalElements[1])<<"\t";
					LineData.append(sta::ConvertUnits(Units,SphericalElements[1],"rad"));
				    }
				    if(name=="Longitude")
				    {
					//stream<<sta::ConvertUnits(Units,SphericalElements[0])<<"\t";
					LineData.append(sta::ConvertUnits(Units,SphericalElements[0],"rad"));
				    }
				    if(name=="Radial Distance")
				    {
					//stream<<sta::ConvertUnits(Units,SphericalElements[2])<<"\t";
					LineData.append(sta::ConvertUnits(Units,SphericalElements[2],"km"));
				    }
				    if (name=="Altitude")
				    {
					double radius=STA_SOLAR_SYSTEM->lookup("Earth")->meanRadius();
					double altitude=sta::ConvertUnits(Units,SphericalElements[2]-radius,"km");
					LineData.append(altitude);
				    }
				    if(name=="Flight Path Angle")
				    {
					// stream<<sta::ConvertUnits(Units,SphericalElements[4])<<"\t";
					LineData.append(sta::ConvertUnits(Units,SphericalElements[4],"rad"));
				    }
				    if(name=="Heading Angle")
				    {
					//stream<<sta::ConvertUnits(Units,SphericalElements[5])<<"\t";
					LineData.append(sta::ConvertUnits(Units,SphericalElements[5],"rad"));
				    }
				    if(name=="Velocity Modulus")
				    {
					//stream<<sta::ConvertUnits(Units,SphericalElements[3])<<"\t";
					LineData.append(sta::ConvertUnits(Units,SphericalElements[3],"km/s"));
				    }
				}
				if((name=="e*sin(omegaBar)")||(name=="e*cos(omegaBar)")||(name=="tan(i/2)*sin(raan)")||(name=="tan(i/2)*cos(raan)")||(name=="Mean Longitude"))
                                {
                                    QString ToCoord=analysis::ReadCoordinateSys(Tree.at(a),SelParameters.at(i));
                                    QString Units=analysis::ReadUnits(Tree.at(a),SelParameters.at(i));

				    sta::StateVector Vector[inumber];
				    Vector[index]=arc->trajectorySample(j);
				    if(name=="e*sin(omegaBar)")
				    {
					double esin=calcEquinoctialElements(Vector[index],STA_SOLAR_SYSTEM->lookup("Earth"),"e*sin(omegaBar)",
									    MJDdate[index],
									    "EME J2000",
									    ToCoord);

					//LineData.append(sta::ConvertUnits(Units,esin));
					LineData.append(esin);
                                    }
				    if(name=="e*cos(omegaBar)")
				    {
					double ecos=calcEquinoctialElements(Vector[index],STA_SOLAR_SYSTEM->lookup("Earth"),"e*cos(omegaBar)",
									    MJDdate[index],
									    "EME J2000",
									    ToCoord);

					// LineData.append(sta::ConvertUnits(Units,ecos));
					LineData.append(ecos);
				    }
				    if(name=="tan(i/2)*sin(raan)")
				    {
					double etansin=calcEquinoctialElements(Vector[index],STA_SOLAR_SYSTEM->lookup("Earth"),"tan(i/2)*sin(raan)",
									       MJDdate[index],
									       "EME J2000",
									       ToCoord);

					// LineData.append(sta::ConvertUnits(Units,etansin));
					LineData.append(etansin);
				    }
				    if(name=="tan(i/2)*cos(raan)")
				    {
					double etancos=calcEquinoctialElements(Vector[index],STA_SOLAR_SYSTEM->lookup("Earth"),"tan(i/2)*cos(raan)",
									       MJDdate[index],
									       "EME J2000",
									       ToCoord);

					//LineData.append(sta::ConvertUnits(Units,etancos));
					LineData.append(etancos);
				    }
				    if(name=="Mean Longitude")
				    {
					double MeanLon=calcEquinoctialElements(Vector[index],STA_SOLAR_SYSTEM->lookup("Earth"),"Mean Longitude",
									       MJDdate[index],
									       "EME J2000",
									       ToCoord);

					// LineData.append(sta::ConvertUnits(Units,MeanLon,"rad"));
					LineData.append(MeanLon);
				    }
				}
			    }
			    //stream<<"\r\n";

			}

		    }

		    else  // the time interval is not included in the propagation time
		    {
			//WRITE stream<<"#######Beginning of time"<<" "<<(k+1)<<"######"<<"\r\n";

			//WRITE stream<<"No data available for the chosen time interval, please check the options of the propagation"<<"\r\n";
		    }

		    //

		    //WRITE stream<<"\r\n";
		    if(LineData.isEmpty())
		    {

		    }
		    else
		    {
			DisplayData.append(LineData);
		    }

		}

	    }
        }
        ToStruct.Data=DisplayData; // AnalysisData structure is now complete, with the data of one axis
        if(DisplayData.isEmpty())
        {

        }
        else
        {
            DataStructure.append(ToStruct); // appending to the list containig all the axes
        }

    }
    return DataStructure;
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
    /*
  returns the name of the output coordinate system that shall be sent to the function that performs the transformation
*/
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
    if(Coordinate==" ")
    {
        sta::CoordinateSystem Coord("INVALID");
        return Coord;
    }
}
double analysis::calcKeplerianElements(sta::StateVector Vector,StaBody*Body,QString OrbElement,
				       double mjd,
				       QString FromCoordinate,
				       QString ToCoordinate)
{
    /*
  Calculates the keplerian elements of a certain orbit
  Inputs:Vector-statevector for each time step,Body-pointer to the central Body (for the time being, only Earth is available),OrbElement- name of the element the user wishes to analyse ,mjd-time in Modified Julian Date,FromCoordinate- coordinate system of the data before transformation, ToCoordinate- coordinate system that shall appear as output)
  Outputs: parameter to be displayed/analysed
  */
    sta::StateVector ModifVector=CoordinateSystem::convert(Vector,
                                                           mjd,
                                                           STA_SOLAR_SYSTEM->lookup("Earth"),
                                                           analysis::CoordSys(FromCoordinate),
                                                           STA_SOLAR_SYSTEM->lookup("Earth"),
                                                           analysis::CoordSys(ToCoordinate));
    //double mu = Earth_GRAVITY_PARAMETER;

    double mu=Body->mu();


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
    if(OrbElement=="RAAN")
    {
        Element=KepElemList.AscendingNode;
    }
    if(OrbElement=="Argument of Periapsis")
    {
        Element=KepElemList.ArgumentOfPeriapsis;
    }
    if(OrbElement=="True Anomaly")
    {
        Element=KepElemList.TrueAnomaly;
    }

    return Element;
}
double analysis::calcDelaunayElements(sta::StateVector Vector,StaBody*Body,QString OrbElement,
				      double mjd,
				      QString FromCoordinate,
				      QString ToCoordinate)
{
    /*
Calculates the Delaunay Elements of a certain orbit
  Inputs:Vector-statevector for each time step,Body-pointer to the central Body (for the time being, only Earth is available),OrbElement- name of the element the user wishes to analyse ,mjd-time in Modified Julian Date,FromCoordinate- coordinate system of the data before transformation, ToCoordinate- coordinate system that shall appear as output)
  Outputs: parameter to be displayed/analysed
  */
    sta::StateVector ModifVector=CoordinateSystem::convert(Vector,
                                                           mjd,
                                                           STA_SOLAR_SYSTEM->lookup("Earth"),
                                                           analysis::CoordSys(FromCoordinate),
                                                           STA_SOLAR_SYSTEM->lookup("Earth"),
                                                           analysis::CoordSys(ToCoordinate));
    double mu = Body->mu();
    //qDebug()<<mu-1000000<<"mu del";
    sta::DelaunayElements DelaunayElem=cartesianTOdelaunay(mu, ModifVector);

    double Element=0;
    if(OrbElement=="l")
    {
        Element=DelaunayElem.l;
    }
    if(OrbElement=="g")
    {
        Element=DelaunayElem.g;
    }
    if(OrbElement=="h")
    {
        Element=DelaunayElem.h;
    }
    if(OrbElement=="L")
    {
        Element=(DelaunayElem.L);
    }
    if(OrbElement=="G")
    {
        Element=DelaunayElem.G;
    }
    if(OrbElement=="H")
    {
        Element=DelaunayElem.H;
    }

    return Element;
}

double analysis::calcEquinoctialElements(sta::StateVector Vector,StaBody*Body,QString OrbElement,
                                         double mjd,
                                         QString FromCoordinate,
                                         QString ToCoordinate)
{
    /*
Calculates the Equinoctial Elements of a certain orbit
  Inputs:Vector-statevector for each time step,Body-pointer to the central Body (for the time being, only Earth is available),OrbElement- name of the element the user wishes to analyse ,mjd-time in Modified Julian Date,FromCoordinate- coordinate system of the data before transformation, ToCoordinate- coordinate system that shall appear as output)
  Outputs: parameter to be displayed/analysed
      */
    sta::StateVector ModifVector=CoordinateSystem::convert(Vector,
                                                           mjd,
                                                           STA_SOLAR_SYSTEM->lookup("Earth"),
                                                           analysis::CoordSys(FromCoordinate),
                                                           STA_SOLAR_SYSTEM->lookup("Earth"),
                                                           analysis::CoordSys(ToCoordinate));
    double mu = Body->mu();
    sta::EquinoctialElements EquinoctialElem=cartesianTOequinoctial(mu, ModifVector);

    double Element=0;
    if(OrbElement=="Semimajor Axis")
    {
        Element=EquinoctialElem.SemimajorAxis;
    }
    if(OrbElement=="e*cos(omegaBar)")
    {
        Element=EquinoctialElem.ecos;
    }
    if(OrbElement=="e*sin(omegaBar)")
    {
        Element=EquinoctialElem.esin;
    }
    if(OrbElement=="Mean Longitude")
    {
        Element=EquinoctialElem.MeanLon;
    }
    if(OrbElement=="tan(i/2)*sin(raan)")
    {
        Element=EquinoctialElem.tansin;
    }
    if(OrbElement=="tan(i/2)*cos(raan)")
    {
        Element=EquinoctialElem.tancos;
    }

    return Element;
}

QList<int>  analysis::calcAccessTime(double MissionTime, int AccessNumber, int AccessStep, int CovIndex,QStringList LineOfCoverageReport)
{
    QList<int>AccessTime;  // AccessTime[0]=number of Access, AccessTime[1]=number of step within the Access

    if(CovIndex<LineOfCoverageReport.size())
    {
        QString Line=LineOfCoverageReport.at(CovIndex);
        double TimeCovReport=(Line.section("\t",0,0)).toDouble();

        if(abs(MissionTime-TimeCovReport)<10e-6)
        {
            if(AccessStep==0)
            {
                AccessNumber++;
            }

            CovIndex++;
            AccessTime.append(AccessNumber);
            AccessTime.append(AccessStep);
            AccessStep++;
            return AccessTime;

        }
        else
        {
            AccessStep=0;
            AccessTime.append(0);
            return AccessTime;
        }

    }
    else
    {
        AccessTime.append(0);
        return AccessTime;
    }
}

void analysis::addParameter()

{
    /*
      Selects the items in the list of the parameters that can be sent to the list of the report options/parameters
      */
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
    /*
      allows the user to add the parameters he/she wants to see in the report
      Inputs: item- selected items that are to be sent to the report options list
      */
    QString text = item->text(0);
    if(treeWidgetShowInReport->findItems(text,Qt::MatchExactly).isEmpty())

    {
        QTreeWidgetItem*added=new QTreeWidgetItem(treeWidgetShowInReport);
        added->setText(0,text);
        ComboBoxOptions(added);
    }

}

void analysis::removeParameter()
{
    /*
      deletes the selected parameters from the report options list
      */
    QList<QTreeWidgetItem *> ChosenParameters = treeWidgetShowInReport->selectedItems();
    for (int i = 0; i < ChosenParameters.size(); i++)
    {
        delete ChosenParameters.at(i);
    }
}

void analysis::enableReportOption(int i)
{
    /*
      changes the layout of the GUI according to the selected analysis format
      Inputs: i=0  report
          i=1, 2D
          i=2, 2.5D
          i=3, 3D
          i=4, histogram
      */
    QList<QTreeWidgetItem*>MissionArcs=TreeWidgetMissionArc->selectedItems();
    for (int k=0;k<MissionArcs.size();k++)
    {
        MissionArcs.at(k)->setSelected(false);
    }

    QList<QTreeWidgetItem*>TimeSelections=treeWidgetTimeSpecifications->selectedItems();
    for (int k=0;k<TimeSelections.size();k++)
    {
        TimeSelections.at(k)->setSelected(false);
    }

    QList<QTreeWidgetItem*>ReportOptions=treeWidgetReportOptions->selectedItems();
    for (int k=0;k<ReportOptions.size();k++)
    {
        ReportOptions.at(k)->setSelected(false);
    }

    QList<QTreeWidgetItem*>ShowInReport=treeWidgetShowInReport->selectedItems();
    for (int k=0;k<ShowInReport.size();k++)
    {
        ShowInReport.at(k)->setSelected(false);
    }

    QList<QTreeWidgetItem*>Xaxis=treeWidgetXaxis->selectedItems();
    for (int k=0;k<Xaxis.size();k++)
    {
        Xaxis.at(k)->setSelected(false);
    }

    QList<QTreeWidgetItem*>Yaxis=treeWidgetYaxis->selectedItems();
    for (int k=0;k<Yaxis.size();k++)
    {
        Yaxis.at(k)->setSelected(false);
    }

    QList<QTreeWidgetItem*>Zaxis=treeWidgetZaxis->selectedItems();
    for (int k=0;k<Zaxis.size();k++)
    {
        Zaxis.at(k)->setSelected(false);
    }

    if (i == 0)
    {
        stackedWidget->setCurrentWidget(pageReport);
    }
    else
    {
        //connect(TimeBox,SIGNAL(activated(int)),this,SLOT(SetTimeUnits()));
        stackedWidget->setCurrentWidget(pagePlotAxesSettings);

        //PlotComboBox();
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
            QMessageBox HistogramWarning;
            HistogramWarning.setText("No histogram functionality is implemented yet");
            HistogramWarning.exec();
            return;
	}
    }
}


QComboBox* analysis::CoordinateBox()
{
    /*
      returns a combobox with all the coordinate systems available
      */
    QComboBox*CoordinateBox2=new QComboBox();
    CoordinateBox2->addItem(tr("EME J2000"),0);
    CoordinateBox2->addItem(tr("EME B1950"),1);
    CoordinateBox2->addItem(tr("Fixed"),2);
    CoordinateBox2->addItem(tr("Ecliptic J2000"),3);
    CoordinateBox2->addItem(tr("Rotating"),4);
    CoordinateBox2->addItem(tr("Rotating Norm."),5);

    // Guillermo says: smaller font is better for the row
    //QFont font("Helvetica", 10); CoordinateBox2->setFont(font);
    CoordinateBox2->setMaximumHeight(22);

    return CoordinateBox2;
}


QComboBox*analysis::TimeFramesBox()
{
    /*
      Description: returns a combobox with all the time frames available
      */
    QComboBox*TimeBox=new QComboBox();
    TimeBox->addItem(tr("MJD"));
    TimeBox->addItem(tr("Julian Date"));
    TimeBox->addItem(tr("Julian UTC"));
    TimeBox->addItem(tr("Julian LCL"));
    TimeBox->addItem(tr("Gregorian UTC"));
    TimeBox->addItem(tr("Gregorian LCL"));
    TimeBox->addItem(tr("YYDDD"));
    TimeBox->addItem(tr("Mission Elapsed"));
    //TimeBox->addItem(tr("GMT"));


    // Guillermo says: smaller font is better for the row
    //QFont font("Helvetica", 10); TimeBox->setFont(font);
    TimeBox->setMaximumHeight(22);

    return TimeBox;
}
QComboBox*analysis::TimeUnitsBox()
{
    /*
  Description: returns a combobox with all the time units available
  */
    QComboBox*TimeBox=new QComboBox();
    TimeBox->addItem(tr("Seconds"),0);
    TimeBox->addItem(tr("Minutes"),1);
    TimeBox->addItem(tr("Hours"),2);
    TimeBox->addItem(tr("Days"),3);

    // Guillermo says: smaller font is better for the row
    //QFont font("Helvetica", 10);     TimeBox->setFont(font);
    TimeBox->setMaximumHeight(22);

    return TimeBox;
}

QComboBox*analysis::AngleUnitsBox()
{
    /*
      Description: returns a combobox with all the angle units available
      */
    QComboBox*AngleBox=new QComboBox();
    AngleBox->addItem(tr("deg"),1);
    AngleBox->addItem(tr("rad"),0);


    // Guillermo says: smaller font is better for the row
    //QFont font("Helvetica", 10);     AngleBox->setFont(font);
    AngleBox->setMaximumHeight(22);

    return AngleBox;
}

QComboBox* analysis::DistanceUnitsBox()
{
    /*
      Description: returns a combobox with all the distance units available
      */
    QComboBox*DistanceBox=new QComboBox();
    DistanceBox->addItem(tr("km"),0);
    DistanceBox->addItem(tr("m"),1);
    DistanceBox->addItem(tr("AU"),2);
    DistanceBox->addItem(tr("nm"),3);

    // Guillermo says: smaller font is better for the row
    //QFont font("Helvetica", 10);     DistanceBox->setFont(font);
    DistanceBox->setMaximumHeight(22);

    return DistanceBox;
}
QComboBox* analysis::NoUnitsBox()
{
    /*
    Description: returns a combobox with no text, for parameters with no units
        */
    QComboBox* NoUnits=new QComboBox();
    NoUnits->addItem(tr("-"));

    // Guillermo says: smaller font is better for the row
    //QFont font("Helvetica", 10);     NoUnits->setFont(font);
    NoUnits->setMaximumHeight(22);

    return NoUnits;
}

QComboBox*analysis::VelocityUnitsBox()
{
    /*
      Description: returns a combobox with all the velocity units available
      */
    QComboBox*VelocityBox=new QComboBox();
    VelocityBox->addItem(tr("km/s"),0);
    VelocityBox->addItem(tr("m/s"),0);

    // Guillermo says: smaller font is better for the row
    //QFont font("Helvetica", 10);     VelocityBox->setFont(font);
    VelocityBox->setMaximumHeight(22);

    return VelocityBox;
}


void analysis::ComboBoxOptions(QTreeWidgetItem*item)
{
    /*Description: selects the comboboxes that should be available next to each parameter, according to its type
      item- line of the tree that contains the report options
      */
    //treeWidgetShowInReport->setColumnWidth(0,138);
    //treeWidgetShowInReport->setColumnWidth(1,120);
    //treeWidgetShowInReport->setColumnWidth(2,50);

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
	(name=="Radial Distance")||
	(name=="Apogee Planetocentric Radius")||
	(name=="Perigee Planetocentric Radius")||
	(name=="Apogee Planetocentric Altitude")||
	(name=="Perigee Planetocentric Altitude")||
	(name=="Altitude")||
	(name=="Range")||
	(name=="Velocity Modulus")||
	(name=="Semimajor Axis"))
    {
	treeWidgetShowInReport->setItemWidget(item,1,CoordinateBox());
	treeWidgetShowInReport->setItemWidget(item,2,DistanceUnitsBox());
    }
    if(name=="Time")
    {
	treeWidgetShowInReport->setItemWidget(item,1,TimeFramesBox());
	treeWidgetShowInReport->setItemWidget(item,2,NoUnitsBox());
    }
    if(name=="Epoch")
    {
	treeWidgetShowInReport->setItemWidget(item,1,NoUnitsBox());
	treeWidgetShowInReport->setItemWidget(item,2,TimeUnitsBox());
    }

    if((name=="Eccentricity")||
       (name=="L")||
       (name=="G")||
       (name=="l")||
       (name=="g")||
       (name=="h")||
       (name=="H"))

    {
	treeWidgetShowInReport->setItemWidget(item,1,CoordinateBox());
	treeWidgetShowInReport->setItemWidget(item,2,NoUnitsBox());

    }
    if((name=="e*sin(omegaBar)")||(name=="e*cos(omegaBar)")||(name=="tan(i/2)*sin(raan)")||(name=="tan(i/2)*cos(raan)"))
    {
	treeWidgetShowInReport->setItemWidget(item,1,CoordinateBox());
	treeWidgetShowInReport->setItemWidget(item,2,NoUnitsBox());
    }

    if((name=="x velocity")||
       (name=="y velocity")||
       (name=="z velocity"))
    {
	treeWidgetShowInReport->setItemWidget(item,1,CoordinateBox());
	treeWidgetShowInReport->setItemWidget(item,2,VelocityUnitsBox());
    }
    if((name=="Access Time")||

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
       (name=="RAAN")||
       (name=="Argument of Periapsis")||
       (name=="True Anomaly")||
       (name=="Mean Longitude")||

       (name=="Longitude when passing Ascending Node")||
       (name=="Longitude when passing Descending Node")||
       (name=="Maximum Geodetic Latitude")||
       (name=="Minimum Geodetic Latitude")||
       (name=="Heading Angle")||

       (name=="Latitude")||
       (name=="Longitude")||
       (name=="Azimuth")||
       (name=="Elevation")||
       (name=="Flight Path Angle")||
       (name=="l")||(name=="g")||(name=="h")

	)
        {
	treeWidgetShowInReport->setItemWidget(item,1,CoordinateBox());
	treeWidgetShowInReport->setItemWidget(item,2,AngleUnitsBox());
    }
    if((name=="Equivalent Isotropical Radiated Power")||(name=="Received Frequency")||(name=="Doppler Shift")||(name=="Received Power")||(name=="Flux Density")||(name=="Overlap Bandwidth Factor")||
       (name=="Free Space Loss")||(name=="Oxygen Loss")||(name=="Water Vapour Loss")||(name=="Rain Loss")||(name=="Atmospheric Loss")||(name=="Propagation Loss")||
       (name=="G/T")||(name=="C/No")||(name=="C/N")||(name=="Eb/No")||(name=="BER"))
    {
	treeWidgetShowInReport->setItemWidget(item,1,NoUnitsBox());
	treeWidgetShowInReport->setItemWidget(item,2,NoUnitsBox());

    }

}

void analysis::PlotComboBox()
{

    {

	QList<QTreeWidget*>Tree;
	Tree.append(treeWidgetXaxis);
	Tree.append(treeWidgetYaxis);
	Tree.append(treeWidgetZaxis);
	for(int a=0;a<Tree.size();a++)
	{

	    for(int j=0;j<Tree.at(a)->topLevelItemCount();j++)
	    {

		QTreeWidgetItem*topItem=Tree.at(a)->topLevelItem(j);
		if(topItem->childCount()==0)
		{

		}
		else
		{

		    for(int k=0;k<topItem->childCount();k++)
		    {

			if(topItem->child(k)->childCount()==0)
			{
			    QTreeWidgetItem*item=topItem->child(k);
			    QString name=item->text(0);
			    //qDebug()<<"name of first children"<<name;
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
				(name=="Radial Distance")||
				(name=="Apogee Planetocentric Altitude")||
				(name=="Perigee Planetocentric Altitude")||
				(name=="Altitude")||
				(name=="Apogee Planetocentric Radius")||
				(name=="Perigee Planetocentric Radius")||
				(name=="Range")||
				(name=="Velocity Modulus")||
				(name=="Semimajor Axis"))
			    {
				Tree.at(a)->setItemWidget(item,1,CoordinateBox());
				Tree.at(a)->setItemWidget(item,2,DistanceUnitsBox());
			    }
			    if(name=="Time")
			    {
				Tree.at(a)->setItemWidget(item,1,TimeFramesBox());
				Tree.at(a)->setItemWidget(item,2,NoUnitsBox());


			    }
                            if(name=="Epoch")
			    {
				Tree.at(a)->setItemWidget(item,1,NoUnitsBox());
				Tree.at(a)->setItemWidget(item,2,TimeUnitsBox());
			    }

			    if((name=="Eccentricity")||
			       (name=="L")||
			       (name=="G")||
			       (name=="l")||
			       (name=="g")||
			       (name=="h")||
			       (name=="H"))

			    {
				Tree.at(a)->setItemWidget(item,1,CoordinateBox());
				Tree.at(a)->setItemWidget(item,2,NoUnitsBox());

			    }
			    if((name=="e*sin(omegaBar)")||(name=="e*cos(omegaBar)")||(name=="tan(i/2)*sin(raan)")||(name=="tan(i/2)*cos(raan)"))
			    {
				Tree.at(a)->setItemWidget(item,1,CoordinateBox());
				Tree.at(a)->setItemWidget(item,2,NoUnitsBox());
			    }

			    if((name=="x velocity")||
			       (name=="y velocity")||
			       (name=="z velocity"))
			    {
				Tree.at(a)->setItemWidget(item,1,CoordinateBox());
				Tree.at(a)->setItemWidget(item,2,VelocityUnitsBox());
			    }
			    if((name=="Access Time")||

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
				Tree.at(a)->setItemWidget(item,1,TimeFramesBox());
				Tree.at(a)->setItemWidget(item,2,TimeUnitsBox());
			    }
			    if((name=="Inclination")||
			       (name=="RAAN")||
			       (name=="Argument of Periapsis")||
			       (name=="True Anomaly")||
			       (name=="Mean Longitude")||

			       (name=="Longitude when passing Ascending Node")||
			       (name=="Longitude when passing Descending Node")||
			       (name=="Maximum Geodetic Latitude")||
			       (name=="Minimum Geodetic Latitude")||
			       (name=="Heading Angle")||

			       (name=="Latitude")||
			       (name=="Longitude")||
			       (name=="Azimuth")||
			       (name=="Elevation")||
			       (name=="Flight Path Angle")||
			       (name=="l")||(name=="g")||(name=="h")

				)
				{
				Tree.at(a)->setItemWidget(item,1,CoordinateBox());
				Tree.at(a)->setItemWidget(item,2,AngleUnitsBox());
			    }
			}
			else
			{

			    for (int l=0;l<topItem->child(k)->childCount();l++)
			    {
				QTreeWidgetItem*item=topItem->child(k)->child(l);
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
				    (name=="Radial Distance")||
				    (name=="Apogee Planetocentric Altitude")||
				    (name=="Perigee Planetocentric Altitude")||
				    (name=="Altitude")||
				    (name=="Apogee Planetocentric Radius")||
				    (name=="Perigee Planetocentric Radius")||
				    (name=="Range")||
				    (name=="Velocity Modulus")||
				    (name=="Semimajor Axis"))
				{
				    Tree.at(a)->setItemWidget(item,1,CoordinateBox());
				    Tree.at(a)->setItemWidget(item,2,DistanceUnitsBox());
				}
				if(name=="Time")
				{
				    Tree.at(a)->setItemWidget(item,1,CoordinateBox());
				    Tree.at(a)->setItemWidget(item,2,NoUnitsBox());
				}
                                if(name=="Epoch")
				{
				    Tree.at(a)->setItemWidget(item,2,TimeUnitsBox());
				}

				if((name=="Eccentricity")||
				   (name=="L")||
				   (name=="G")||
				   (name=="l")||
				   (name=="g")||
				   (name=="h")||
				   (name=="H"))

				{
				    Tree.at(a)->setItemWidget(item,1,CoordinateBox());
				    Tree.at(a)->setItemWidget(item,2,NoUnitsBox());

				}
				if((name=="e*sin(omegaBar)")||(name=="e*cos(omegaBar)")||(name=="tan(i/2)*sin(raan)")||(name=="tan(i/2)*cos(raan)"))
				{
				    Tree.at(a)->setItemWidget(item,1,CoordinateBox());
				    Tree.at(a)->setItemWidget(item,2,NoUnitsBox());
				}

				if((name=="x velocity")||
				   (name=="y velocity")||
				   (name=="z velocity"))
				{
				    Tree.at(a)->setItemWidget(item,1,CoordinateBox());
				    Tree.at(a)->setItemWidget(item,2,VelocityUnitsBox());
				}
				if((name=="Access Time")||

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
				    Tree.at(a)->setItemWidget(item,1,TimeFramesBox());
				    Tree.at(a)->setItemWidget(item,2,TimeUnitsBox());
				}
				if((name=="Inclination")||
				   (name=="RAAN")||
				   (name=="Argument of Periapsis")||
				   (name=="True Anomaly")||
				   (name=="Mean Longitude")||

				   (name=="Longitude when passing Ascending Node")||
				   (name=="Longitude when passing Descending Node")||
				   (name=="Maximum Geodetic Latitude")||
				   (name=="Minimum Geodetic Latitude")||
				   (name=="Heading Angle")||

				   (name=="Latitude")||
				   (name=="Longitude")||
				   (name=="Azimuth")||
				   (name=="Elevation")||
				   (name=="Flight Path Angle")||
				   (name=="l")||(name=="g")||(name=="h")

				    )
				    {
				    Tree.at(a)->setItemWidget(item,1,CoordinateBox());
				    Tree.at(a)->setItemWidget(item,2,AngleUnitsBox());
				}
			    }
			}

		    }
		}

	    }

	}
    }
}

void analysis::DisableUnavailableOptions()
{
    QList<QTreeWidget*>Tree;
    Tree.append(treeWidgetReportOptions);
    Tree.append(treeWidgetXaxis);
    Tree.append(treeWidgetYaxis);
    Tree.append(treeWidgetZaxis);
    for(int a=0;a<Tree.size();a++)
    {

        for(int j=0;j<Tree.at(a)->topLevelItemCount();j++)
        {

            QTreeWidgetItem*topItem=Tree.at(a)->topLevelItem(j);
            if(topItem->childCount()==0)
            {

            }
            else
            {
                for(int k=0;k<topItem->childCount();k++)
                {
                    if(topItem->text(0)=="SEM Data"||topItem->text(0)=="Aerodynamics"||topItem->text(0)=="Re-entry")
                    {
                        topItem->setDisabled(true);
                        topItem->child(k)->setDisabled(true);
                    }

                    for (int l=0;l<topItem->child(k)->childCount();l++)
                    {
                        QTreeWidgetItem*item=topItem->child(k)->child(l);
			// QString name=item->text(0);

                        if(topItem->child(k)->text(0)=="Access Time")
                        {
                            topItem->child(k)->setDisabled(true);
                        }
                        if(topItem->child(k)->text(0)=="Apparent Position Vector between the participant and")
                        {
                            Tree.at(a)->setItemWidget(item,1,CoordinateBox());
                            Tree.at(a)->setItemWidget(item,2,DistanceUnitsBox());
                            topItem->child(k)->setDisabled(true);
                            item->setDisabled(true);
                        }

                        if(
				(topItem->child(k)->text(0)=="Pass Times")||(topItem->child(k)->text(0)=="Ecliptic Crossing Times")||(topItem->child(k)->text(0)=="Covariance"))

			{
                            Tree.at(a)->setItemWidget(item,1,TimeFramesBox());
                            Tree.at(a)->setItemWidget(item,2,TimeUnitsBox());
                            topItem->child(k)->setDisabled(true);
                            item->setDisabled(true);
                        }
                        if(topItem->child(k)->text(0)=="Pass Data")
                        {
                            topItem->child(k)->setDisabled(true);
                            item->setDisabled(true);
                        }
                    }


		}
	    }

        }

    }
}
