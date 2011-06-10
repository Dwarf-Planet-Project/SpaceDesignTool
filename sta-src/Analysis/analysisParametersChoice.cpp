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
 ------ Copyright (C) 2011 STA Steering Board (space.trajectory.analysis AT gmail.com) ----
*/


/*
 ------------------ Author:       Guillermo Ortega               -------------------
 ------------------ Affiliation:  European Space Agency (ESA)    -------------------
 -----------------------------------------------------------------------------------

 */

#include <cmath>

#include <QDesktopServices>
#include <QUrl>
#include <QDebug>
#include <QComboBox>
#include <QMessageBox>
#include <QInputDialog>
#include <QtCore/QFile>
#include <qtextstream.h>
#include <QtGui>
#include <QDebug>
#include <QGLWidget>
#include <QMessageBox>
#include <QPrinter>

#include "analysisParametersChoice.h"
#include "Analysis/qtiplotmain.h"

#include "Scenario/staschema.h"
#include "Main/propagatedscenario.h"


// Format for date/time strings shown to user when selecting a time range
static const QString TimeRangeSelectionDateFormat = "d/M/yyyy hh:mm:ss";



analysisParametersChoice::analysisParametersChoice( QWidget * parent) : QWidget(parent)
{
    setupUi(this);

    connect(this->AddParameterPushButton, SIGNAL(clicked()), this, SLOT(addParameters()));
    connect(this->RemoveParameterPushButton, SIGNAL(clicked()), this, SLOT(removeParameters()));
}

analysisParametersChoice::~analysisParametersChoice()
{
}


// ***** Scenario helper functions *****

QList<QSharedPointer<ScenarioAbstractTrajectoryType> >
        Scenario_GetTrajectoryPlan(ScenarioParticipantType* participant)
{
    if (dynamic_cast<ScenarioSC*>(participant))
    {
        return dynamic_cast<ScenarioSC*>(participant)->SCMission()->TrajectoryPlan()->AbstractTrajectory();
    }
    else if (dynamic_cast<ScenarioREV*>(participant))
    {
        return dynamic_cast<ScenarioREV*>(participant)->REVMission()->REVTrajectoryPlan()->AbstractTrajectory();
    }
    else
    {
        // Return an empty list
        return QList<QSharedPointer<ScenarioAbstractTrajectoryType> >();
    }
}


/** Get the start time, end time, and step time (in seconds) for a mission arc.
  * Return true if the mission arc is valid and has a non-zero duration.
  */
bool
Scenario_GetTimeRange(ScenarioAbstractTrajectoryType* trajectory, QDateTime* startTime, QDateTime* endTime, double* stepTime)
{
    // TODO: This function only exists because TimeLine isn't a property of the base
    // trajectory class. This is probably appropriate, as future trajectory types (e.g. rendezvous)
    // will not have durations that are known until propagation.
    if (dynamic_cast<ScenarioLoiteringType*>(trajectory))
    {
        ScenarioLoiteringType* loitering = dynamic_cast<ScenarioLoiteringType*>(trajectory);
        *startTime = loitering->TimeLine()->StartTime();
        *endTime = loitering->TimeLine()->EndTime();
        *stepTime = loitering->TimeLine()->StepTime();
    }
    else if (dynamic_cast<ScenarioLoiteringTLEType*>(trajectory))
    {
        ScenarioLoiteringTLEType* loiteringTLE = dynamic_cast<ScenarioLoiteringTLEType*>(trajectory);
        *startTime = loiteringTLE->TimeLine()->StartTime();
        *endTime = loiteringTLE->TimeLine()->EndTime();
        *stepTime = loiteringTLE->TimeLine()->StepTime();
    }
    else if (dynamic_cast<ScenarioExternalType*>(trajectory))
    {
        ScenarioExternalType* ext = dynamic_cast<ScenarioExternalType*>(trajectory);
        if (ext->TimeTags().size() >= 2)
        {
            *startTime = ext->TimeTags().first();
            *endTime = ext->TimeTags().last();
            *stepTime = startTime->secsTo(*endTime) / 1000.0; // Default time step is 1/1000 of duration
        }
        else
        {
            *startTime = QDateTime();
            *endTime = QDateTime();
        }
    }
    else if (dynamic_cast<ScenarioDeltaVType*>(trajectory))
    {
        ScenarioDeltaVType* deltaV = dynamic_cast<ScenarioDeltaVType*>(trajectory);
        *startTime = deltaV->TimeLine()->StartTime();
        *endTime = deltaV->TimeLine()->EndTime();
        *stepTime = deltaV->TimeLine()->StartTime().secsTo(deltaV->TimeLine()->EndTime());
    }
    else if (dynamic_cast<ScenarioEntryArcType*>(trajectory))
    {
        ScenarioEntryArcType* entry = dynamic_cast<ScenarioEntryArcType*>(trajectory);
        *startTime = entry->TimeLine()->StartTime();
        *endTime = entry->TimeLine()->EndTime();
        *stepTime = entry->TimeLine()->StepTime();
    }
    else
    {
        // Invalid or unknown trajectory type; set the times to be invalid
        *startTime = QDateTime();
        *endTime = QDateTime();
    }

    return startTime->isValid() && endTime->isValid();
}


/*
  builds a copy of the scenario tree from the STA main window in the GUI of Analysis Module
 */
void analysisParametersChoice::loadTheSTAscenario(SpaceScenario* scenario, PropagatedScenario* propagatedScenario)

{
    m_scenario = new SpaceScenario(*scenario);
    m_propagatedScenario = new PropagatedScenario(*propagatedScenario);

    int SpaceObjectInd = 0;
    int GroundObjectInd = 0;
    //int ObjectIndex=0;
    int MissionInd;
    int TxInd;
    int RxInd;
    QStringList AllObjects;

    qDebug() << "--------> loadTheSTAscenario got the data" << endl;


    foreach (QSharedPointer<ScenarioParticipantType> participant, m_scenario->AbstractParticipant())
    {
        int GroundStation=0; //identify the type of Object
        int SpaceCraft=1; //identify the type of Object

        if ((dynamic_cast<ScenarioSC*>(participant.data())!=NULL)) //SC participant
        {
            QTreeWidgetItem* spacecraftItem = new QTreeWidgetItem(TreeWidgetMissionArc);
            spacecraftItem->setExpanded(true);
            spacecraftItem->setText(0,participant.data()->Name());
            spacecraftItem->setIcon(0, QIcon(":/icons/ParticipantSATELLITE.png"));

            QTreeWidgetItem* trajectoryPlan=new QTreeWidgetItem(spacecraftItem);
            trajectoryPlan->setText(0,"Trajectory Plan");
            trajectoryPlan->setExpanded(true);

            QTreeWidgetItem* PayloadSet = new QTreeWidgetItem(spacecraftItem);
            PayloadSet->setText(0,"Payload Set");
            PayloadSet->setExpanded(true);

            ScenarioSC* vehicle = dynamic_cast<ScenarioSC*>(participant.data());

            const QList<QSharedPointer<ScenarioAbstractTrajectoryType> >& trajectoryList =
                    vehicle->SCMission()->TrajectoryPlan()->AbstractTrajectory();

            MissionInd=0;

            foreach (QSharedPointer<ScenarioAbstractTrajectoryType> trajectory,trajectoryList) //Loitering
            {
                QTreeWidgetItem* loiterItem = new QTreeWidgetItem(trajectoryPlan);
                loiterItem->setExpanded(true);
                loiterItem->setText(0, trajectory->ElementIdentifier()->Name());
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
                if (dynamic_cast<ScenarioLoiteringType*>(trajectory.data()))
                {
                    mlabel->setText(0, "Loiter");
                }
                else if (dynamic_cast<ScenarioLoiteringTLEType*>(trajectory.data()))
                {
                    mlabel->setText(0, "TLE");
                }
                else if (dynamic_cast<ScenarioDeltaVType*>(trajectory.data()))
                {
                    mlabel->setText(0, "Delta-V");
                }
                else if (dynamic_cast<ScenarioExternalType*>(trajectory.data()))
                {
                    mlabel->setText(0, "External");
                }

                mlabel->setHidden(true);
                QTreeWidgetItem * mission= new QTreeWidgetItem(loiterItem);
                mission->setText(0,"MArc");
                mission->setHidden(true);

                MissionInd++;
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
                    payloadItem->setExpanded(true);
                    payloadItem->setText(0,"Transmitter");
                    payloadItem->setIcon(0, QIcon(":/icons/Payload.png"));

                    QTreeWidgetItem * parentType =new QTreeWidgetItem(payloadItem);
                    parentType->setText(0,QString::number(SpaceCraft));
                    parentType->setHidden(true);

                    QTreeWidgetItem * parentIndex =new QTreeWidgetItem(payloadItem);
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
                    payloadItem->setExpanded(true);
                    payloadItem->setText(0,"Receiver");
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
            QTreeWidgetItem* oindex = new QTreeWidgetItem(spacecraftItem);
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
            QTreeWidgetItem * REVitem = new QTreeWidgetItem(TreeWidgetMissionArc);
            REVitem->setExpanded(true);
            REVitem->setText(0,participant.data()->Name());
            REVitem->setIcon(0, QIcon(":/icons/ParticipantENTRYVEHICLE.png"));

            QTreeWidgetItem* trajectoryPlan = new QTreeWidgetItem(REVitem);
            trajectoryPlan->setExpanded(true);
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
                    REVmissionItem->setExpanded(true);
                    ScenarioEntryArcType* Reentry=dynamic_cast<ScenarioEntryArcType*>(trajectory.data());
                    REVmissionItem->setText(0,Reentry->ElementIdentifier()->Name());
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
            QTreeWidgetItem * GSitem = new QTreeWidgetItem(TreeWidgetMissionArc);
            GSitem->setExpanded(true);
            GSitem->setText(0,participant.data()->Name());
            GSitem->setIcon(0, QIcon(":/icons/ParticipantSTATION.png"));

            QTreeWidgetItem* PayloadSet = new QTreeWidgetItem(GSitem);
            PayloadSet->setExpanded(true);
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
                    payloadItem->setExpanded(true);
                    payloadItem->setText(0,"Transmitter");
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
                    payloadItem->setExpanded(true);
                    payloadItem->setText(0,"Receiver");
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


void
analysisParametersChoice::addParameters()
{
    qDebug() << "add parameters";
}


void
analysisParametersChoice::removeParameters()
{
    qDebug() << "remove parameters";
}


int
analysisParametersChoice::objectsIndex(QStringList AllObjects, int Index, QString ObjectType)
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


bool
analysisParametersChoice::checkIfMissionArc()
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


void
analysisParametersChoice::on_AddDefaultPushButton_clicked()
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

    if (checkIfMissionArc())
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
            SelectedObjects.append(objectsIndex(AllObjects,MParentIndex.at(i),"space"));
        }

        for(int z=0;z<SelectedObjects.size();z++)
        {
            QSharedPointer<ScenarioParticipantType>participant=m_scenario->AbstractParticipant().at(SelectedObjects.at(z));

            QList<QSharedPointer<ScenarioAbstractTrajectoryType> > trajectoryList = Scenario_GetTrajectoryPlan(participant.data());

            int index = MIndex.at(z);
            if (index < trajectoryList.size())
            {
                QSharedPointer<ScenarioAbstractTrajectoryType> trajectory = trajectoryList.at(MIndex.at(z));

                QDateTime StartTimeDate;
                QDateTime EndTimeDate;
                double stepTime;

                if (Scenario_GetTimeRange(trajectory.data(), &StartTimeDate, &EndTimeDate, &stepTime))
                {
                    QString StepTimeSec= QString::number(stepTime);
                    QString StartTime = StartTimeDate.toString(TimeRangeSelectionDateFormat);
                    QString EndTime   = EndTimeDate.toString(TimeRangeSelectionDateFormat);

                    if ((treeWidgetTimeSpecifications->findItems(StartTime, Qt::MatchExactly,0).isEmpty()) ||
                        (treeWidgetTimeSpecifications->findItems(EndTime, Qt::MatchExactly,1).isEmpty())   ||
                        (treeWidgetTimeSpecifications->findItems(StepTimeSec, Qt::MatchExactly,2).isEmpty()))
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
    else
    {
        QMessageBox MissionArcWarning;
        MissionArcWarning.setText("Please select only valid Mission Arcs");
        MissionArcWarning.exec();
    }
}


void
analysisParametersChoice::on_DeleteTimePushButton_clicked()
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


void
analysisParametersChoice::on_EditTimePushButton_clicked() //adds new time intervals to the tree
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

    if (check==0)
    {
        QMessageBox parametersWarning;
        parametersWarning.setText("Time Input Error, please verify format"); //the date introduced does not match a calendar date
        parametersWarning.exec();
    }
}

