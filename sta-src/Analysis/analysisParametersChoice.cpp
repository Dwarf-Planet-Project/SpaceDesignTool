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
#include <QFormLayout>
#include <QPrinter>
#include <QDesktopServices>

#include "analysisParametersChoice.h"
#include "Analysis/qtiplotmain.h"

#include "Scenario/staschema.h"
#include "Main/propagatedscenario.h"

#include "Astro-Core/stamath.h"
#include "Astro-Core/calculateElements.h"
#include "Services/serviceTimeParameter.h"

using namespace sta;


// Format for date/time strings shown to user when selecting a time range
static const QString TimeRangeSelectionDateFormat = "dd/MM/yyyy hh:mm:ss";


// Mark tree items with children as unselectable. This prevents the user
// from accidentally choosing invalid parameters (e.g. the State Vector category
// instead of the individual state vector components.)
static void SetParentItemsUnselectable(QTreeWidgetItem* item)
{
    if (item->childCount() > 0)
    {
        item->setFlags(item->flags() & ~Qt::ItemIsSelectable);
        for (int i = 0; i < item->childCount(); ++i)
        {
            SetParentItemsUnselectable(item->child(i));
        }
    }
}


analysisParametersChoice::analysisParametersChoice( QWidget * parent) : QWidget(parent)
{
    setupUi(this);

    // Prevent the user from accidentally selecting parameter classes
    for (int i = 0; i < treeWidgetReportOptions->topLevelItemCount(); ++i)
    {
        QTreeWidgetItem* item = treeWidgetReportOptions->topLevelItem(i);
        if (item)
        {
            SetParentItemsUnselectable(item);
        }
    }

    connect(this->AddParameterPushButton, SIGNAL(clicked()), this, SLOT(addSelectedParameters()));
    connect(this->RemoveParameterPushButton, SIGNAL(clicked()), this, SLOT(removeSelectedParameters()));
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


// Convert a state vector to the coordinate system given by the specified name
static StateVector ConvertStateVector(const StateVector& state, const QString& coordSys, const StaBody* center, double mjd)
{
    sta::CoordinateSystemType toSystem = sta::COORDSYS_INVALID;
    if (coordSys == "Fixed")
    {
        toSystem = sta::COORDSYS_BODYFIXED;
    }
    else if (coordSys == "Ecliptic J2000")
    {
        toSystem = sta::COORDSYS_ECLIPTIC_J2000;
    }
    else if (coordSys == "EME J2000")
    {
        toSystem = sta::COORDSYS_EME_J2000;
    }
    else if (coordSys == "EME B1950")
    {
        toSystem = sta::COORDSYS_EME_B1950;
    }
       else if (coordSys == "ICRF")
        {
            toSystem = sta::COORDSYS_ICRF;
        }
        else if (coordSys == "Mean of Date")
        {
            toSystem = sta::COORDSYS_MEAN_OF_DATE;
        }
        else if (coordSys == "True of Date")
        {
            toSystem = sta::COORDSYS_TRUE_OF_DATE;
        }
        else if (coordSys == "Mean of Epoch")
        {
            toSystem = sta::COORDSYS_MEAN_OF_EPOCH;
        }

    //    else if (coordSys == "True of Epoch")
    //    {
    //        toSystem = sta::COORDSYS_TRUE_OF_EPOCH;
    //    }
    //    else if (coordSys == "TEME of Date")
    //    {
    //        toSystem = sta::COORDSYS_TEME_OF_Date;
    //    }
    //    else if (coordSys == "TEME of Epoch")
    //    {
    //        toSystem = sta::COORDSYS_TEME_OF_EPOCH;
    //    }
    //    else if (coordSys == "Alignment at Epoch")
    //    {
    //        toSystem = sta::COORDSYS_ALIGN_AT_EPOCH;
    //    }
    //    else if (coordSys == "Rotating")
    //    {
    //        toSystem = sta::COORDSYS_ROT;
    //    }
    //    else if (coordSys == "Normalized Rotating")
    //    {
    //        toSystem = sta::COORDSYS_ROT_NORM;
    //    }
    Q_ASSERT(toSystem != sta::COORDSYS_INVALID);

    return CoordinateSystem::convert(state,
                                     mjd,
                                     center,
                                     sta::CoordinateSystem(sta::COORDSYS_EME_J2000),
                                     center,
                                     sta::CoordinateSystem(toSystem));
}


// Return true if this output field is the apparent distance to a Solar System body
static bool IsPlanetDistanceField(const QString& fieldName)
{
    return STA_SOLAR_SYSTEM->lookup(fieldName) != NULL;
}

/***** End helper functions *****/


/*
  builds a copy of the scenario tree from the STA main window in the GUI of Analysis Module
 */
void
analysisParametersChoice::loadScenario(SpaceScenario* scenario, PropagatedScenario* propagatedScenario)
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
analysisParametersChoice::addSelectedParameters()
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


void
analysisParametersChoice::addParameter(QTreeWidgetItem* item)
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
        comboBoxOptions(added);
    }
}


void
analysisParametersChoice::removeSelectedParameters()
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
    for (int i = 0; i < NumberOfObjects; i++)
    {
        int numberOfChildren = TreeWidgetMissionArc->topLevelItem(i)->childCount();
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
    QDateTime defaultStartTime = QDateTime(QDate::currentDate()).toUTC();
    QDateTime defaultEndTime = defaultStartTime.addSecs(3600 * 12);

    QDialog timeDialog;
    timeDialog.setWindowTitle(tr("Enter Time Range"));
    QDateTimeEdit* startTimeEdit = new QDateTimeEdit(&timeDialog);
    startTimeEdit->setDateTime(defaultStartTime.toUTC());
    startTimeEdit->setDateTimeRange(QDateTime(QDate(1950, 1, 1)), QDateTime(QDate(2100, 1, 1)));
    startTimeEdit->setDisplayFormat("yyyy-MMM-dd hh:mm:ss UTC");

    QDateTimeEdit* endTimeEdit = new QDateTimeEdit(&timeDialog);
    endTimeEdit->setDateTime(defaultEndTime.toUTC());
    endTimeEdit->setDateTimeRange(QDateTime(QDate(1950, 1, 1)), QDateTime(QDate(2100, 1, 1)));
    endTimeEdit->setDisplayFormat("yyyy-MMM-dd hh:mm:ss UTC");

    QVBoxLayout* vbox = new QVBoxLayout(&timeDialog);
    timeDialog.setLayout(vbox);

    QFormLayout* form = new QFormLayout(&timeDialog);
    QLabel* startLabel = new QLabel("Start time   ", &timeDialog);
    QLabel* endLabel = new QLabel("End time   ", &timeDialog);
    form->addRow(startLabel, startTimeEdit);
    form->addRow(endLabel, endTimeEdit);

    QDialogButtonBox* buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, Qt::Horizontal, &timeDialog);
    vbox->addItem(form);
    vbox->addWidget(buttons);

    connect(buttons, SIGNAL(accepted()), &timeDialog, SLOT(accept()));
    connect(buttons, SIGNAL(rejected()), &timeDialog, SLOT(reject()));

    if (timeDialog.exec() == QDialog::Accepted)
    {
        QDateTime startTime = startTimeEdit->dateTime();
        QDateTime endTime = endTimeEdit->dateTime();
        if (startTime >= endTime)
        {
            QMessageBox::warning(this, "Bad Time Range", "End time must be after start time");
        }
        else
        {
            QTreeWidgetItem*treeTimeN = new QTreeWidgetItem(treeWidgetTimeSpecifications);
            treeTimeN->setText(0, startTimeEdit->dateTime().toString(TimeRangeSelectionDateFormat));
            treeTimeN->setText(1, endTimeEdit->dateTime().toString(TimeRangeSelectionDateFormat));
        }
    }
}


/*  Selects the comboboxes that should be available next to each parameter, according to its type
 *  item- line of the tree that contains the report options
 */
void
analysisParametersChoice::comboBoxOptions(QTreeWidgetItem* item)
{
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
        //This group has been patched by Guillermo
        treeWidgetShowInReport->setItemWidget(item,1,TimeFramesBox());
        treeWidgetShowInReport->setItemWidget(item,2,NoUnitsBox());
       // Next lines patched by Guillermo
       /*
       treeWidgetShowInReport->setColumnWidth(0, 100);
       treeWidgetShowInReport->setColumnWidth(1, 200);
       treeWidgetShowInReport->setColumnWidth(2, 150);
       DialogServiceTimeParameterFrame* serviceTimeParameterWidget = new DialogServiceTimeParameterFrame();
       treeWidgetShowInReport->setItemWidget(item,1,serviceTimeParameterWidget);
       serviceTimeParameterWidget->show();
       DialogServiceTimeUnitFrame* serviceTimeUnitWidget = new DialogServiceTimeUnitFrame();
       treeWidgetShowInReport->setItemWidget(item,2,serviceTimeUnitWidget);
       serviceTimeUnitWidget->show();
       */
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
    if(name=="Covered Area" && m_propagatedScenario->coverageTriggered())
    {
        treeWidgetShowInReport->setItemWidget(item,1,NoUnitsBox());
        treeWidgetShowInReport->setItemWidget(item,2,CoveredAreaUnitsBox());
    }
    if((name=="EIRP")||(name=="Received Frequency")||(name=="Doppler Shift")||(name=="Received Power")||(name=="Flux Density")||(name=="Overlap Bandwidth Factor")||
       (name=="Free Space Loss")||(name=="Oxygen Loss")||(name=="Water Vapour Loss")||(name=="Rain Loss")||(name=="Atmospheric Loss")||(name=="Propagation Loss")||
       (name=="G/T")||(name=="C/No")||(name=="C/N")||(name=="Eb/No")||(name=="BER"))
    {
        treeWidgetShowInReport->setItemWidget(item,1,NoUnitsBox());
        treeWidgetShowInReport->setItemWidget(item,2,NoUnitsBox());

    }

}


QComboBox*
analysisParametersChoice::CoordinateBox()
{
    /*
      returns a combobox with all the coordinate systems available
      */
    QComboBox*CoordinateBox2=new QComboBox();
    CoordinateBox2->addItem(tr("EME J2000"),0);
    CoordinateBox2->addItem(tr("EME B1950"),1);
    CoordinateBox2->addItem(tr("Fixed"),2);
    CoordinateBox2->addItem(tr("Ecliptic J2000"),3);
    // CoordinateBox2->addItem(tr("ICRF"),4);
    //    CoordinateBox2->addItem(tr("Rotating"),4);
    //    CoordinateBox2->addItem(tr("Rotating Norm."),5);
    CoordinateBox2->addItem(tr("ICRF"),6);
    CoordinateBox2->addItem(tr("Mean of Date"),7);
    CoordinateBox2->addItem(tr("True of Date"),8);
    CoordinateBox2->addItem(tr("Mean of Epoch"),9);

    //    CoordinateBox2->addItem(tr("True of Epoch"),10);
    //    CoordinateBox2->addItem(tr("TEME of Date"),11);
    //    CoordinateBox2->addItem(tr("TEME of Epoch"),12);
    //    CoordinateBox2->addItem(tr("Alignment at Epoch"),13);

    // Guillermo says: smaller font is better for the row
    //QFont font("Helvetica", 10); CoordinateBox2->setFont(font);
    CoordinateBox2->setMaximumHeight(22);

    return CoordinateBox2;
}


QComboBox*
analysisParametersChoice::TimeFramesBox()
{
    /*
      Description: returns a combobox with all the time frames available
      */
    QComboBox*TimeBox=new QComboBox();
    TimeBox->addItem(tr("Days from Epoch"));
    TimeBox->addItem(tr("Hours from Epoch"));
    TimeBox->addItem(tr("Minutes from Epoch"));
    TimeBox->addItem(tr("Seconds from Epoch"));
    TimeBox->addItem(tr("Earth canonical time"));
    //TimeBox->addItem(tr("GMT"));                  // It's not implemented yet
    TimeBox->addItem(tr("Gregorian GPS"));
    //TimeBox->addItem(tr("Gregorian LCL"));        // It's not implemented yet
    TimeBox->addItem(tr("Gregorian TAI"));
    TimeBox->addItem(tr("Gregorian TDB"));
    TimeBox->addItem(tr("Gregorian TDT"));
    TimeBox->addItem(tr("Gregorian UTC"));
    TimeBox->addItem(tr("Julian Ephemeris Date"));
    // TimeBox->addItem(tr("Julian LCL"));           // It's not implemented yet
    TimeBox->addItem(tr("Julian GPS"));
    TimeBox->addItem(tr("Julian TDB"));
    TimeBox->addItem(tr("Julian UTC"));
    TimeBox->addItem(tr("Julian Date"));
    TimeBox->addItem(tr("Mission Elapsed"));
    TimeBox->addItem(tr("MJD"));
    TimeBox->addItem(tr("YYDDD TDB"));

    // Guillermo says: smaller font is better for the row
    //QFont font("Helvetica", 10); TimeBox->setFont(font);
    TimeBox->setMaximumHeight(22);

    return TimeBox;
}


QComboBox*
analysisParametersChoice::TimeUnitsBox()
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


QComboBox*
analysisParametersChoice::AngleUnitsBox()
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


QComboBox*
analysisParametersChoice::DistanceUnitsBox()
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


QComboBox*
analysisParametersChoice::CoveredAreaUnitsBox()
{
    /*
      Description: returns a combobox with the units "%" and "km^2"
      */
    QComboBox*CoveredAreaBox=new QComboBox();
    CoveredAreaBox->addItem(tr("km^2"),0);
    CoveredAreaBox->addItem(tr("%"),1);

    CoveredAreaBox->setMaximumHeight(22);

    return CoveredAreaBox;
}


QComboBox*
analysisParametersChoice::NoUnitsBox()
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


QComboBox*
analysisParametersChoice::VelocityUnitsBox()
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


AnalysisResult
analysisParametersChoice::generateReport()
{
    QList<QTreeWidgetItem *> selected=TreeWidgetMissionArc->selectedItems();
    QList<QTreeWidgetItem *> selectedTimes=treeWidgetTimeSpecifications->selectedItems();

    QString dataPath = QDesktopServices::storageLocation(QDesktopServices::DataLocation);

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

        if(name=="Azimuth"||name=="Elevation"||name=="Range"||name=="Access Time")
        {

            ReadCoverage=true;
        }
        //if((name=="Equivalent Isotropical Radiated Power")||(name=="Received Frequency")||(name=="Doppler Shift")||(name=="Received Power")||(name=="Flux Density")||(name=="Overlap Bandwidth Factor"))
        if((name=="EIRP")||(name=="Received Frequency")||(name=="Doppler Shift")||(name=="Received Power")||(name=="Flux Density")||(name=="Overlap Bandwidth Factor"))
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

    // Patched by Guillermo to allow read of files in MAC and Linux
    QString analysisFile = dataPath + "/" + "analysisReport.txt";
    qDebug() << analysisFile;
    QFile file(analysisFile);
    file.open(QIODevice::WriteOnly);
    QTextStream stream(&file);
    stream.setRealNumberPrecision(16);

    //######time of generation of report#####
    QDateTime ReportDateTime=QDateTime::currentDateTime();

    stream<<"Report generated on"<<"\t"<<ReportDateTime.date().day()<<"/"<<ReportDateTime.date().month()<<"/"<<ReportDateTime.date().year()<<" "<<"at"<<" "<<ReportDateTime.time().hour()<<":"<<ReportDateTime.time().minute()<<":"<<ReportDateTime.time().second()<<"\r\n";
    int CStart, CStop;

    // Get the column names for the analysis result
    QStringList columnNames;
    for (int itemIndex = 0; itemIndex < treeWidgetShowInReport->topLevelItemCount(); itemIndex++)
    {
        QString name = ReadParameter(treeWidgetShowInReport->topLevelItem(itemIndex));
        if (name != "Access Time")
        {
            QString Unit = ReadUnits(treeWidgetShowInReport,treeWidgetShowInReport->topLevelItem(itemIndex));
            columnNames << QString("%1 (%2)").arg(name, Unit);
        }
        else
        {
            columnNames << "Access Number" << "Start Time (MJD)" << "Stop Time (MJD)" << "Duration of each access (seconds)";
        }
    }

    // Create the analysis result structure; output from the analysis will be dumped
    // here and passed to QtiPlot.
    AnalysisResult analysisResult(columnNames.count());
    analysisResult.setTitle("STA Results");
    for (int i = 0; i < columnNames.count(); ++i)
    {
        analysisResult.setColumnName(i, columnNames.at(i));
    }

    if (selectedTimes.empty())
    {
        QMessageBox::warning(this, "Analysis Error", "No time ranges selected");
        return analysisResult;
    }

    for(int z=0;z<MParentIndex.size();z++)
    {

        SpaceObject*spaceObj=m_propagatedScenario->spaceObjects().at(MParentIndex.at(z));

        {
            int indMissionArc=MObjectsIndex.at(z);
            MissionArc*arc=spaceObj->mission().at(MObjectsIndex.at(z));

            stream<<"Satellite: "<<MParentIndex.at(z)+1<<"\t"<<"Mission Arc: "<<indMissionArc+1<<"\r\n";

            int timeRangeCount = selectedTimes.size();
            QVector<double> StartTime(timeRangeCount);
            QVector<double> StopTime(timeRangeCount);
            QVector<int> countStart(timeRangeCount);
            QVector<int> countStop(timeRangeCount);

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

#if 0
            if(ReadCoverage==true)
            {
                CoverageAnalysis covAna=CoverageAnalysis(m_propagatedScenario, indSC, indGS, indMissionArc);
                covAna.reportAER();
                // Patched by Guillermo to allow read of files in MAC and Linux
                QString CoverageFile = staResourcesPath + "/" + "reportCov1.txt";
                QFile Coverage(CoverageFile);

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

            ReadCommunication1 = true; ReadCommunication2 = false;  ReadCommunication3 = false;
            if(ReadCommunication1==true)
            {
                CommAnalysis commAnalysis=CommAnalysis(Transmitter, Receiver, Environment, m_propagatedScenario, indSC, indGS, indMissionArc, TxParentType, RxParentType);
                commAnalysis.CommReports();
                // Patched by Guillermo to allow read of files in MAC and Linux
                QString Comm1File = staResourcesPath + "/" + "reportComm1.txt";
                QFile Communication1(Comm1File);

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
                // Patched by Guillermo to allow read of files in MAC and Linux
                QString Comm2File = staResourcesPath + "/" + "reportComm2.txt";
                QFile Communication2(Comm2File);

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
                // Patched by Guillermo to allow read of files in MAC and Linux
                QString Comm3File = staResourcesPath + "/" + "reportComm3.txt";
                QFile Communication3(Comm3File);

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
#endif

            for (int k=0;k<selectedTimes.size();k++)
            {
                int ControlStart=0;
                int ControlStop=0;
                if ((StartTime[k]>(StopEpoch+2*pow(10,-5)))||(StartTime[k]<=(StartEpoch-2*pow(10,-5))))
                {
                    ControlStart++;
                }
                if ((StopTime[k]>(StopEpoch+2*pow(10,-5)))||(StopTime[k]<=(StartEpoch-2*pow(10,-5))))
                {

                    ControlStop++;
                }
                CStart=ControlStart;
                CStop=ControlStop;
                int AccessStep=0;
                int AccessNow=0;
                int AccessNumber=0;

                if((ControlStart==0)&&(ControlStop==0))
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
                    stream<<"###Beginning of time"<<" "<<(k+1)<<"###"<<"\r\n"<<"Start Time"<<" "<<Start<<"###"<<"Stop Time"<<" "<<Stop<<"\r\n";

                    //printing the labels of the displayed parameters
                    for(int i=0;i<treeWidgetShowInReport->topLevelItemCount();i++)
                    {
                        QString name = ReadParameter(treeWidgetShowInReport->topLevelItem(i));
                        if(name!="Access Time")
                        {
                            QString Unit = ReadUnits(treeWidgetShowInReport,treeWidgetShowInReport->topLevelItem(i));
                            // Guillermo patched the following line
                            //stream<<"\t"<<name<<"("<<Unit<<")"<<"\t";
                            stream << name << " (" << Unit << ") " << "\t";
                            numberOfColumns++;

                            if(i==((treeWidgetShowInReport->topLevelItemCount())-1))
                            {
                                stream<<"\r\n";
                            }
                        }
                        else
                        {
                            stream<<name<<"\r\n";
                            stream<<"Access Number"<<"\t"<<"Start Time (MJD)"<<"\t"<<"Stop Time (MJD)"<<"\t"<<"Duration of each access (seconds)"<<"\r\n";
                            numberOfColumns=4;

                            if(i==((treeWidgetShowInReport->topLevelItemCount())-1))
                            {
                                stream<<"\r\n";
                            }
                        }
                    }

                    double MJDdate[inumber];
                    //            double JulianDate[inumber];
                    //            double TDBSeconds[inumber];
                    //            double TAIDate[inumber];
                    //            double TDTDate[inumber];
                    //            double JDTDT[inumber];
                    //            double JDGPS[inumber];
                    //            double JDTAI[inumber];
                    //            double GPSDate[inumber];
                    //TimeConversions::GregorianDate gregUTC[inumber];
                    //            double GregSec[inumber];
                    //            double DateSec[inumber];
                    //            double OrbitalPeriod;
                    //            double EarthCanTime[inumber];

                    QDateTime GregorianLCL[inumber];
                    double CurrentJulianDate=sta::DateTimeTOjulian(QDateTime::currentDateTime());
                    double JulianLCL[inumber];
                    QDateTime TimeDateVector[inumber];
                    double DayOfYear[inumber];

                    QVariantList analysisRow;
                    //Access Time calculation
                    for(int i=0;i<treeWidgetShowInReport->topLevelItemCount();i++)
                    {

                        QTreeWidgetItem*parameter=treeWidgetShowInReport->topLevelItem(i);
                        QString name=parameter->text(0);

#if 0
                        if(name=="Access Time")
                        {

                            QList< QList<double> > AccessTime= calcAccessTime(countStop[k],countStart[k], AccessNumber, AccessStep, AccessNow,CovIndex[3],LineOfCoverageReport,arc);

                            for(int l=0;l<AccessTime.size();l++)
                            {
                                QList <double> List=AccessTime.at(l);

                                int j=0;
                                for(int n=0;n<List.size();n++)
                                {
                                    j=j+1;
                                    stream<<List.at(n)<<"\t";
                                    analysisRow << List.at(n);

                                    //if(n==List.size()-1)
                                    if(j%4==0)
                                    {
                                        stream <<"\r\n";
                                    }
                                }
                            }
                            numberOfRows=AccessTime.size();
                        }
#endif
                    }
                    //other parameters
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
                                QString Coordinate = ReadCoordinateSys(treeWidgetShowInReport,parameter);
                                QString Units = ReadUnits(treeWidgetShowInReport,parameter);

                                double outputValue = 0.0;

                                double mjd = MJDdate[index];

                                //Added by Catarina
                                if (Coordinate == "Mean of Epoch")
                                {
                                     mjd = MJDdate[0];
                                }

                                StateVector state = arc->trajectorySample(j);
                                StateVector transformedState = ConvertStateVector(state, Coordinate, arc->centralBody(), mjd);

                                if (name == "x position")
                                {
                                    outputValue = transformedState.position.x();
                                }
                                else if (name == "y position")
                                {
                                    outputValue = transformedState.position.y();
                                }
                                else if (name == "z position")
                                {
                                    outputValue = transformedState.position.z();
                                }
                                else if (name == "x velocity")
                                {
                                    outputValue = transformedState.velocity.x();
                                }
                                else if (name == "y velocity")
                                {
                                    outputValue = transformedState.velocity.y();
                                }
                                else if (name == "z velocity")
                                {
                                    outputValue = transformedState.velocity.z();
                                }

                                stream << sta::ConvertUnits(Units,outputValue, "km") << "\t";
                                analysisRow << sta::ConvertUnits(Units,outputValue, "km");
                            }
                            if(name=="Time")
                            {

                                QString TimeCoordinate = ReadCoordinateSys(treeWidgetShowInReport,parameter);

                                //Options of Time
                                if(TimeCoordinate=="MJD")                               // Added by Ana
                                {
                                    double mjd=convertToMJDUTC(MJDdate,index);
                                    stream<<mjd<<"\t";
                                    analysisRow << mjd;
                                }
                                if(TimeCoordinate=="Julian TDB")                        // Added by Ana
                                {
                                    double julianDate=convertToJulianTDB(MJDdate,index);
                                    stream<<julianDate<<"\t";
                                    analysisRow << julianDate;
                                }
                                if(TimeCoordinate=="Gregorian UTC")                     // Added by Ana. Corrected by Catarina
                                {
                                    QString gregUTC = convertToGregUTC(MJDdate,index);
                                    stream<<gregUTC<<"\t";
                                    analysisRow << gregUTC;
                                }
                                if(TimeCoordinate=="Gregorian LCL")                     // Added by Catarina. Missing: Time zones
                                {
                                    // Convert MJD to Julian Date in TDB
                                    //double julianDate=sta::MjdToJd(MJDdate[index]+0.00001);
                                    // Convert to gregorian UTC
                                    //TimeConversions::GregorianDate gregUTC=TimeConversions::GregorianDate::UTCDateFromTDBJD(julianDate);
                                    // Missing time zones

                                    // This is to patch Ana's code
                                    //QDateTime DisplayDateCalendar[inumber];

                                    //--------------------------------------------------------------------------------
                                    // For now it is shown in UTC

                                    //TimeConversions::GregorianDate gregUTC = TimeConversions::GregorianDate::convertToGregLCL(MJDdate,index);
                                    //stream<<gregUTC.day()<<"/"<<gregUTC.month()<<"/"<<gregUTC.year()<<" "<<gregUTC.hour()<<":"<<gregUTC.minute()<<":"<<gregUTC.second()<<"\t";
                                }
                                if(TimeCoordinate=="Gregorian TDB")                     // Added by Ana. Corrected by Catarina
                                {
                                    QString gregTDB = convertToGregTDB(MJDdate,index);
                                    stream<<gregTDB<<"\t";
                                    analysisRow << gregTDB;
                                }
                                if(TimeCoordinate=="Julian UTC")                        // Added by Ana. Corrected by Catarina
                                {
                                    int Last2Digits;
                                    double hour, minute, second;
                                    QList<double> DDD;
                                    convertToJulianUTC(MJDdate,index, &Last2Digits , &DDD, &hour, &minute,&second);
                                    stream<<Last2Digits<<"/";
                                    for(int i=0;i<DDD.size();i++)
                                    {
                                        stream<<DDD[i];
                                    }
                                    stream<<" "<< hour<<":"<< minute<<":"<<second<<"\t";
//                                    analysisRow << QString("%1/%2 %3:%4:%5").
//                                                      arg(DDD).
//                                                      arg(QString::number(Year).mid(2, 2)).
//                                                      arg(gregUTC.hour(), 2, 10, QChar('0')).
//                                                      arg(gregUTC.minute(), 2, 10, QChar('0')).
//                                                      arg(gregUTC.second(), 2, 10, QChar('0'));

                                }
                                if(TimeCoordinate=="Julian Date")                       // Added by Catarina
                                {
                                    double julian=convertToJulianTDB(MJDdate,index);
                                    stream<<julian<<"\t";
                                    analysisRow << julian;
                                }
                                if(TimeCoordinate=="Julian LCL")                        // Added by Catarina. Missing: time zones
                                {
                                    //double julianDate=sta::MjdToJd(MJDdate[index]+0.00001);
                                    //// Convert to Gregorian UTC
                                    //TimeConversions::GregorianDate gregUTC=TimeConversions::GregorianDate::UTCDateFromTDBJD(julianDate);

                                    //// Convert gregoUTC to JulianUTC
                                    //double julUTC= calendarTOjulian(gregUTC.year(), gregUTC.month(), gregUTC.day(), gregUTC.hour(), gregUTC.minute(),gregUTC.second());

                                    //// Need to add time zones. For now it's in UTC

                                    //stream<<julUTC<<"\t";
                                    //analysisRow << julUTC;
                                }
                                if(TimeCoordinate=="Mission Elapsed")                   // Added by Ana
                                {
                                    QString Elapsed= sta::MissionElapsedTime(MJDdate[index],StartEpoch);

                                    stream<<Elapsed<<"\t";
                                    analysisRow << Elapsed;
                                }
                                if(TimeCoordinate=="Days from Epoch")                   // Added by Catarina
                                {
                                    double ElapsedTimeDays=sta::MjdToFromEpoch(StartEpoch,MJDdate[index],"Days");
                                    stream<<ElapsedTimeDays<<"\t";
                                    analysisRow << ElapsedTimeDays;
                                }
                                if(TimeCoordinate=="Hours from Epoch")                  // Added by Catarina
                                {
                                    double ElapsedTimeHours=sta::MjdToFromEpoch(StartEpoch,MJDdate[index],"Hours");
                                    stream<<ElapsedTimeHours<<"\t";
                                    analysisRow << ElapsedTimeHours;
                                }
                                if(TimeCoordinate=="Minutes from Epoch")                // Added by Catarina
                                {
                                    double ElapsedTimeMinutes=sta::MjdToFromEpoch(StartEpoch,MJDdate[index],"Minutes");
                                    stream<<ElapsedTimeMinutes<<"\t";
                                    analysisRow << ElapsedTimeMinutes;
                                }
                                if(TimeCoordinate=="Seconds from Epoch")                // Added by Catarina
                                {
                                    double ElapsedTimeSeconds=sta::MjdToFromEpoch(StartEpoch,MJDdate[index],"Seconds");
                                    stream<<ElapsedTimeSeconds<<"\t";
                                    analysisRow << ElapsedTimeSeconds;
                                }
                                if(TimeCoordinate=="YYDDD TDB")                         // Added by Ana. Corrected by Catarina.
                                {

                                    int Last2Digits;
                                    QList<double> DDD;
                                    convertToYYDDD(MJDdate,index, &Last2Digits , &DDD);
                                    stream<<Last2Digits<<"/";
                                    for(int i=0;i<DDD.size();i++)
                                    {
                                        stream<<DDD[i];
                                    }
                                    stream<<"\t";
                                    analysisRow << "FIX THIS!";
                                }
                                if(TimeCoordinate=="Gregorian TAI")                     // Added by Catarina
                                {
                                    QString gregTAI = convertToGregTAI(MJDdate,index);
                                    stream<<gregTAI<<"\t";
                                    analysisRow << "FIX THIS!";
                                }
                                if(TimeCoordinate=="Gregorian TDT")                     // Added by Catarina
                                {
                                    QString gregTDT = convertToGregTDT(MJDdate, index);
                                    stream<<gregTDT<<"\t";
                                    analysisRow << "FIX THIS!";
                                }
                                if(TimeCoordinate=="Julian Ephemeris Date")             // Added by Catarina
                                {
                                    double julianEphDate=convertToJulianEphDate(MJDdate,index);
                                    stream<<julianEphDate<<"\t";
                                    analysisRow << julianEphDate;
                                }
                                if(TimeCoordinate=="Gregorian GPS")                     // Added by Catarina
                                {
                                    QString gregGPS = convertToGregGPS(MJDdate,index);
                                    stream<<gregGPS<<"\t";
                                    analysisRow << "FIX THIS!";
                                }
                                if(TimeCoordinate=="Julian GPS")                        // Added by Catarina
                                {
                                    double julianGPS=convertToJulianGPS(MJDdate,index);
                                    stream<<julianGPS<<"\t";
                                    analysisRow << julianGPS;
                                }
                                if(TimeCoordinate=="Earth canonical time")              // Added by Catarina
                                {
                                    double EarthCanTime = convertToEarthCanTime(MJDdate, index, StartEpoch);
                                    stream<<EarthCanTime<<"\t";
                                    analysisRow << EarthCanTime;
                                }
                                if(TimeCoordinate=="GMT")   {}                          // Added by Catarina - not working

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
                                analysisRow << ElapsedTime;
                            }
                            else if (IsPlanetDistanceField(name))
                            {
                                StaBody* body = STA_SOLAR_SYSTEM->lookup(name);
                                Q_ASSERT(body != NULL);

                                QString Units = ReadUnits(treeWidgetShowInReport,parameter);

                                double mjd = MJDdate[index];
                                sta::StateVector state;
                                spaceObj->getStateVector(mjd, *body, CoordinateSystem(COORDSYS_EME_J2000), &state);

                                stream << sta::ConvertUnits(Units, state.position.norm(), "km") << "\t";
                                analysisRow << sta::ConvertUnits(Units, state.position.norm(), "km");
                            }
                            if((name=="Azimuth")||(name=="Elevation")||(name=="Range"))
                            {
                                QString ToCoord = ReadCoordinateSys(treeWidgetShowInReport,parameter);
                                QString ToUnit = ReadUnits(treeWidgetShowInReport,parameter);
                                if(name=="Azimuth")
                                {

                                    if(CovIndex[0]<LineOfCoverageReport.size())
                                    {
                                        QString Line=LineOfCoverageReport.at(CovIndex[0]);

                                        double TimeCovReport=(Line.section("\t",0,0)).toDouble();

                                        double Azimuth=(Line.section("\t",3,3)).toDouble();

                                        if(fabs(MJDdate[index]-TimeCovReport)<10e-6)
                                        {

                                            stream<<sta::ConvertUnits(ToUnit,Azimuth,"deg")<<"\t";
                                            analysisRow <<sta::ConvertUnits(ToUnit,Azimuth,"deg");
                                            CovIndex[0]++;
                                        }

                                        else
                                        {

                                            stream<<"No visibility"<<"\t";
                                            analysisRow << "No visibility";
                                            //CovIndex[0]++;
                                        }

                                    }
                                    else
                                    {
                                        stream<<"No visibility"<<"\t";
                                        analysisRow << "No visibility";
                                    }
                                }
                                if(name=="Elevation")
                                {

                                    if(CovIndex[1]<LineOfCoverageReport.size())
                                    {
                                        QString Line=LineOfCoverageReport.at(CovIndex[1]);

                                        double TimeCovReport=(Line.section("\t",0,0)).toDouble();
                                        double Elevation=(Line.section("\t",2,2)).toDouble();

                                        if(fabs(MJDdate[index]-TimeCovReport)<10e-6)
                                        {
                                            stream<<sta::ConvertUnits(ToUnit,Elevation,"deg")<<"\t";
                                            analysisRow << sta::ConvertUnits(ToUnit, Elevation, "deg");
                                            CovIndex[1]++;
                                        }

                                        else
                                        {
                                            stream<<"No visibility"<<"\t";
                                            analysisRow << "No visibility";
                                        }

                                    }
                                    else
                                    {
                                        stream<<"No visibility"<<"\t";
                                        analysisRow << "No visibility";
                                    }
                                }
                                if(name=="Range")
                                {
                                    if(CovIndex[2]<LineOfCoverageReport.size())
                                    {
                                        QString Line=LineOfCoverageReport.at(CovIndex[2]);

                                        double TimeCovReport=(Line.section("\t",0,0)).toDouble();
                                        double Range=(Line.section("\t",1,1)).toDouble();

                                        if(fabs(MJDdate[index]-TimeCovReport)<10e-6)
                                        {
                                            stream<<sta::ConvertUnits(ToUnit,Range,"km")<<"\t";
                                            analysisRow<<sta::ConvertUnits(ToUnit,Range,"km");
                                            CovIndex[2]++;
                                        }

                                        else
                                        {
                                            stream<<"No visibility"<<"\t";
                                            analysisRow << "No visibility";
                                        }



                                    }
                                    else
                                    {
                                        stream<<"No visibility"<<"\t";
                                        analysisRow << "No visibility";
                                    }
                                }
                            }
                            if(name=="EIRP")
                            {

                                if(Comm1Index[0]<LineOfComm1Report.size())
                                {
                                    QString Line=LineOfComm1Report.at(Comm1Index[0]);

                                    double TimeCommReport=(Line.section("\t",0,0)).toDouble();
                                    double EIRP=(Line.section("\t",1,1)).toDouble();

                                    if(fabs(MJDdate[index]-TimeCommReport)<10e-6)
                                    {
                                        stream<<EIRP<<"\t";
                                        analysisRow << EIRP;
                                        Comm1Index[0]++;
                                    }

                                    else
                                    {
                                        stream<<"No visibility"<<"\t";
                                        analysisRow << "No visibility";
                                    }

                                }
                                else
                                {
                                    stream<<"No visibility"<<"\t";
                                    analysisRow << "No visibility";
                                }
                            }
                            if(name=="Received Frequency")
                            {

                                if(Comm1Index[1]<LineOfComm1Report.size())
                                {
                                    QString Line=LineOfComm1Report.at(Comm1Index[1]);

                                    double TimeCommReport=(Line.section("\t",0,0)).toDouble();
                                    double RcvFreq=(Line.section("\t",2,2)).toDouble();

                                    if(fabs(MJDdate[index]-TimeCommReport)<10e-6)
                                    {
                                        stream<<RcvFreq<<"\t";
                                        analysisRow<<RcvFreq;
                                        Comm1Index[1]++;
                                    }

                                    else
                                    {
                                        stream<<"No visibility"<<"\t";
                                        analysisRow << "No visibility";
                                    }

                                }
                                else
                                {
                                    stream<<"No visibility"<<"\t";
                                    analysisRow << "No visibility";
                                }
                            }
                            if(name=="Doppler Shift")
                            {

                                if(Comm1Index[2]<LineOfComm1Report.size())
                                {
                                    QString Line=LineOfComm1Report.at(Comm1Index[2]);

                                    double TimeCommReport=(Line.section("\t",0,0)).toDouble();
                                    double RcvFreq=(Line.section("\t",3,3)).toDouble();

                                    if(fabs(MJDdate[index]-TimeCommReport)<10e-6)
                                    {
                                        stream<<RcvFreq<<"\t";
                                        analysisRow<<RcvFreq;
                                        Comm1Index[2]++;
                                    }

                                    else
                                    {
                                        stream<<"No visibility"<<"\t";
                                        analysisRow << "No visibility";
                                    }

                                }
                                else
                                {
                                    stream<<"No visibility"<<"\t";
                                    analysisRow << "No visibility";
                                }
                            }
                            if(name=="Received Power")
                            {
                                if(Comm1Index[3]<LineOfComm1Report.size())
                                {
                                    QString Line=LineOfComm1Report.at(Comm1Index[3]);

                                    double TimeCommReport=(Line.section("\t",0,0)).toDouble();
                                    double RcvFreq=(Line.section("\t",4,4)).toDouble();
                                    if(fabs(MJDdate[index]-TimeCommReport)<10e-6)
                                    {
                                        stream<<RcvFreq<<"\t";
                                        analysisRow<<RcvFreq;
                                        Comm1Index[3]++;
                                    }

                                    else
                                    {
                                        stream<<"No visibility"<<"\t";
                                        analysisRow << "No visibility";
                                    }

                                }
                                else
                                {
                                    stream<<"No visibility"<<"\t";
                                    analysisRow << "No visibility";
                                }
                            }
                            if(name=="Flux Density")
                            {
                                if(Comm1Index[4]<LineOfComm1Report.size())
                                {
                                    QString Line=LineOfComm1Report.at(Comm1Index[4]);

                                    double TimeCommReport=(Line.section("\t",0,0)).toDouble();
                                    double FluxDensity=(Line.section("\t",5,5)).toDouble();
                                    if(fabs(MJDdate[index]-TimeCommReport)<10e-6)
                                    {
                                        stream<<FluxDensity<<"\t";
                                        analysisRow<<FluxDensity;
                                        Comm1Index[4]++;
                                    }

                                    else
                                    {
                                        stream<<"No visibility"<<"\t";
                                        analysisRow << "No visibility";
                                    }

                                }
                                else
                                {
                                    stream<<"No visibility"<<"\t";
                                    analysisRow << "No visibility";
                                }
                            }
                            if(name=="Overlap Bandwidth Factor")
                            {

                                if(Comm1Index[5]<LineOfComm1Report.size())
                                {
                                    QString Line=LineOfComm1Report.at(Comm1Index[5]);
                                    double TimeCommReport=(Line.section("\t",0,0)).toDouble();
                                    double OvBWF=(Line.section("\t",6,6)).toDouble();

                                    if(fabs(MJDdate[index]-TimeCommReport)<10e-6)
                                    {
                                        stream<<OvBWF<<"\t";
                                        analysisRow << OvBWF;
                                        Comm1Index[5]++;
                                    }

                                    else
                                    {
                                        stream<<"No visibility"<<"\t";
                                        analysisRow << "No visibility";
                                    }
                                }
                                else
                                {
                                    stream<<"No visibility"<<"\t";
                                    analysisRow << "No visibility";
                                }
                            }
                            if(name=="Free Space Loss")
                            {

                                if(Comm2Index[0]<LineOfComm2Report.size())
                                {
                                    QString Line=LineOfComm2Report.at(Comm2Index[0]);

                                    double TimeCommReport=(Line.section("\t",0,0)).toDouble();

                                    double FSL=(Line.section("\t",1,1)).toDouble();


                                    if(fabs(MJDdate[index]-TimeCommReport)<10e-6)
                                    {
                                        stream<<FSL<<"\t";
                                        analysisRow << FSL;
                                        Comm2Index[0]++;
                                    }

                                    else
                                    {
                                        stream<<"No visibility"<<"\t";
                                        analysisRow << "No visibility";
                                    }

                                }
                                else
                                {
                                    stream<<"No visibility"<<"\t";
                                    analysisRow << "No visibility";
                                }
                            }
                            if(name=="Oxygen Loss")
                            {
                                if(Comm2Index[1]<LineOfComm2Report.size())
                                {
                                    QString Line=LineOfComm2Report.at(Comm2Index[1]);
                                    double TimeCommReport=(Line.section("\t",0,0)).toDouble();
                                    double OxLoss=(Line.section("\t",2,2)).toDouble();

                                    if(fabs(MJDdate[index]-TimeCommReport)<10e-6)
                                    {
                                        stream<<OxLoss<<"\t";
                                        analysisRow << OxLoss;
                                        Comm2Index[1]++;
                                    }

                                    else
                                    {
                                        stream<<"No visibility"<<"\t";
                                        analysisRow << "No visibility";
                                    }
                                }
                                else
                                {
                                    stream<<"No visibility"<<"\t";
                                    analysisRow << "No visibility";
                                }
                            }
                            if(name=="Water Vapour Loss")
                            {
                                if(Comm2Index[2]<LineOfComm2Report.size())
                                {
                                    QString Line=LineOfComm2Report.at(Comm2Index[2]);
                                    double TimeCommReport=(Line.section("\t",0,0)).toDouble();
                                    double WVLoss=(Line.section("\t",3,3)).toDouble();

                                    if(fabs(MJDdate[index]-TimeCommReport)<10e-6)
                                    {
                                        stream<<WVLoss<<"\t";
                                        analysisRow << WVLoss;
                                        Comm2Index[2]++;
                                    }

                                    else
                                    {
                                        stream<<"No visibility"<<"\t";
                                        analysisRow << "No visibility";
                                    }
                                }
                                else
                                {
                                    stream<<"No visibility"<<"\t";
                                    analysisRow << "No visibility";
                                }
                            }
                            if(name=="Rain Loss")
                            {
                                if(Comm2Index[3]<LineOfComm2Report.size())
                                {
                                    QString Line=LineOfComm2Report.at(Comm2Index[3]);
                                    double TimeCommReport=(Line.section("\t",0,0)).toDouble();
                                    double RainLoss=(Line.section("\t",4,4)).toDouble();

                                    if(fabs(MJDdate[index]-TimeCommReport)<10e-6)
                                    {
                                        stream<<RainLoss<<"\t";
                                        analysisRow << RainLoss;
                                        Comm2Index[3]++;
                                    }

                                    else
                                    {
                                        stream<<"No visibility"<<"\t";
                                        analysisRow << "No visibility";
                                    }
                                }
                                else
                                {
                                    stream<<"No visibility"<<"\t";
                                    analysisRow << "No visibility";
                                }
                            }
                            if(name=="Atmospheric Loss")
                            {
                                if(Comm2Index[4]<LineOfComm2Report.size())
                                {
                                    QString Line=LineOfComm2Report.at(Comm2Index[4]);
                                    double TimeCommReport=(Line.section("\t",0,0)).toDouble();
                                    double AtmLoss=(Line.section("\t",5,5)).toDouble();

                                    if(fabs(MJDdate[index]-TimeCommReport)<10e-6)
                                    {
                                        stream<<AtmLoss<<"\t";
                                        analysisRow << AtmLoss;
                                        Comm2Index[4]++;
                                    }

                                    else
                                    {
                                        stream<<"No visibility"<<"\t";
                                        analysisRow << "No visibility";
                                    }
                                }
                                else
                                {
                                    stream<<"No visibility"<<"\t";
                                    analysisRow << "No visibility";
                                }
                            }
                            if(name=="Propagation Loss")
                            {
                                if(Comm2Index[5]<LineOfComm2Report.size())
                                {
                                    QString Line=LineOfComm2Report.at(Comm2Index[5]);
                                    double TimeCommReport=(Line.section("\t",0,0)).toDouble();
                                    double PropLoss=(Line.section("\t",6,6)).toDouble();

                                    if(fabs(MJDdate[index]-TimeCommReport)<10e-6)
                                    {
                                        stream<<PropLoss<<"\t";
                                        analysisRow << PropLoss;
                                        Comm2Index[5]++;
                                    }

                                    else
                                    {
                                        stream<<"No visibility"<<"\t";
                                        analysisRow << "No visibility";
                                    }
                                }
                                else
                                {
                                    stream<<"No visibility"<<"\t";
                                    analysisRow << "No visibility";
                                }
                            }
                            if (name=="G/T")
                            {
                                if(Comm3Index[0]<LineOfComm3Report.size())
                                {
                                    QString Line=LineOfComm3Report.at(Comm3Index[0]);
                                    double TimeCommReport=(Line.section("\t",0,0)).toDouble();
                                    double GT=(Line.section("\t",1,1)).toDouble();

                                    if(fabs(MJDdate[index]-TimeCommReport)<10e-6)
                                    {
                                        stream<<GT<<"\t";
                                        analysisRow << GT;
                                        Comm3Index[0]++;
                                    }

                                    else
                                    {
                                        stream<<"No visibility"<<"\t";
                                        analysisRow << "No visibility";
                                    }
                                }
                                else
                                {
                                    stream<<"No visibility"<<"\t";
                                    analysisRow << "No visibility";
                                }
                            }
                            if(name=="C/No")
                            {
                                if(Comm3Index[1]<LineOfComm3Report.size())
                                {
                                    QString Line=LineOfComm3Report.at(Comm3Index[1]);
                                    double TimeCommReport=(Line.section("\t",0,0)).toDouble();
                                    double CNo=(Line.section("\t",2,2)).toDouble();

                                    if(fabs(MJDdate[index]-TimeCommReport)<10e-6)
                                    {
                                        stream<<CNo<<"\t";
                                        analysisRow << CNo;
                                        Comm3Index[1]++;
                                    }

                                    else
                                    {
                                        stream<<"No visibility"<<"\t";
                                        analysisRow << "No visibility";
                                    }
                                }
                                else
                                {
                                    stream<<"No visibility"<<"\t";
                                    analysisRow << "No visibility";
                                }
                            }
                            if(name=="C/N")
                            {
                                if(Comm3Index[2]<LineOfComm3Report.size())
                                {
                                    QString Line=LineOfComm3Report.at(Comm3Index[2]);
                                    double TimeCommReport=(Line.section("\t",0,0)).toDouble();
                                    double CN=(Line.section("\t",3,3)).toDouble();

                                    if(fabs(MJDdate[index]-TimeCommReport)<10e-6)
                                    {
                                        stream<<CN<<"\t";
                                        analysisRow << CN;
                                        Comm3Index[2]++;
                                    }

                                    else
                                    {
                                        stream<<"No visibility"<<"\t";
                                        analysisRow << "No visibility";
                                    }
                                }
                                else
                                {
                                    stream<<"No visibility"<<"\t";
                                    analysisRow << "No visibility";
                                }
                            }
                            if(name=="Eb/No")
                            {
                                if(Comm3Index[3]<LineOfComm3Report.size())
                                {
                                    QString Line=LineOfComm3Report.at(Comm3Index[3]);
                                    double TimeCommReport=(Line.section("\t",0,0)).toDouble();
                                    double EbNo=(Line.section("\t",4,4)).toDouble();

                                    if(fabs(MJDdate[index]-TimeCommReport)<10e-6)
                                    {
                                        stream<<EbNo<<"\t";
                                        analysisRow << EbNo;
                                        Comm3Index[3]++;
                                    }

                                    else
                                    {
                                        stream<<"No visibility"<<"\t";
                                        analysisRow << "No visibility";
                                    }
                                }
                                else
                                {
                                    stream<<"No visibility"<<"\t";
                                    analysisRow << "No visibility";
                                }
                            }
                            if(name=="BER")
                            {
                                if(Comm3Index[4]<LineOfComm3Report.size())
                                {
                                    QString Line=LineOfComm3Report.at(Comm3Index[4]);
                                    double TimeCommReport=(Line.section("\t",0,0)).toDouble();
                                    double BER=(Line.section("\t",5,5)).toDouble();

                                    if(fabs(MJDdate[index]-TimeCommReport)<10e-6)
                                    {
                                        stream<<BER<<"\t";
                                        analysisRow << BER;
                                        Comm3Index[4]++;
                                    }

                                    else
                                    {
                                        stream<<"No visibility"<<"\t";
                                        analysisRow << "No visibility";
                                    }
                                }
                                else
                                {
                                    stream<<"No visibility"<<"\t";
                                    analysisRow << "No visibility";
                                }
                            }
                            if(name=="Eccentricity")
                            {
                                QString ToCoord = ReadCoordinateSys(treeWidgetShowInReport,parameter);

                                sta::StateVector Vector;
                                Vector=arc->trajectorySample(j);
                                double Eccentricity=calcKeplerianElements(Vector,arc->centralBody(),"Eccentricity",
                                                                          MJDdate[index],
                                                                          "EME J2000",
                                                                          ToCoord);
                                stream<<Eccentricity<<"\t";
                                analysisRow << Eccentricity;
                            }
                            if((name=="Inclination")||
                               (name=="RAAN")||
                               (name=="Argument of Periapsis")||
                               (name=="True Anomaly")||
                               (name=="Semimajor Axis"))
                            {
                                QString ToCoord = ReadCoordinateSys(treeWidgetShowInReport,parameter);
                                QString ToUnit = ReadUnits(treeWidgetShowInReport,parameter);

                                sta::StateVector Vector;
                                Vector=arc->trajectorySample(j);

                                if(name=="Inclination")
                                {
                                    double Inclination=calcKeplerianElements(Vector, arc->centralBody(),name,
                                                                             MJDdate[index],
                                                                             "EME J2000",
                                                                             ToCoord);
                                    stream<<sta::ConvertUnits(ToUnit,Inclination,"rad")<<"\t";
                                    analysisRow << sta::ConvertUnits(ToUnit,Inclination,"rad");
                                }
                                if(name=="RAAN")
                                {
                                    double Ascending=calcKeplerianElements(Vector,arc->centralBody(),name,
                                                                           MJDdate[index],
                                                                           "EME J2000",
                                                                           ToCoord);
                                    stream<<sta::ConvertUnits(ToUnit,Ascending,"rad")<<"\t";
                                    analysisRow << sta::ConvertUnits(ToUnit, Ascending, "rad");
                                }
                                if(name=="True Anomaly")
                                {
                                    double TrueAnom=calcKeplerianElements(Vector,arc->centralBody(),name,
                                                                          MJDdate[index],
                                                                          "EME J2000",
                                                                          ToCoord);
                                    stream<<sta::ConvertUnits(ToUnit,TrueAnom,"rad")<<"\t";
                                    analysisRow << sta::ConvertUnits(ToUnit, TrueAnom, "rad");
                                }
                                if(name=="Argument of Periapsis")
                                {
                                    double Periapsis=calcKeplerianElements(Vector,arc->centralBody(),name,
                                                                           MJDdate[index],
                                                                           "EME J2000",
                                                                           ToCoord);
                                    stream<<sta::ConvertUnits(ToUnit,Periapsis,"rad")<<"\t";
                                    analysisRow << sta::ConvertUnits(ToUnit, Periapsis, "rad");
                                }
                                if(name=="Semimajor Axis")
                                {
                                    double SemAxis=calcKeplerianElements(Vector,arc->centralBody(),name,
                                                                         MJDdate[index],
                                                                         "EME J2000",
                                                                         ToCoord);
                                    stream<<sta::ConvertUnits(ToUnit,SemAxis,"km")<<"\t";
                                    analysisRow << sta::ConvertUnits(ToUnit, SemAxis, "rad");
                                }

                            }
                            if((name=="l")||(name=="g")||(name=="h")||(name=="L")||(name=="G")||(name=="H"))
                            {
                                QString ToCoord = ReadCoordinateSys(treeWidgetShowInReport,parameter);
                                sta::StateVector Vector;
                                Vector=arc->trajectorySample(j);

                                if(name=="l")
                                {
                                    double Delaunay_l=calcDelaunayElements(Vector,arc->centralBody(),name,
                                                                           MJDdate[index],
                                                                           "EME J2000",
                                                                           ToCoord);
                                    stream<<Delaunay_l<<"\t";
                                    analysisRow << Delaunay_l;
                                }
                                if(name=="g")
                                {
                                    double Delaunay_g=calcDelaunayElements(Vector,arc->centralBody(),name,
                                                                           MJDdate[index],
                                                                           "EME J2000",
                                                                           ToCoord);
                                    stream<<Delaunay_g<<"\t";
                                    analysisRow << Delaunay_g;
                                }
                                if(name=="h")
                                {
                                    double Delaunay_h=calcDelaunayElements(Vector,arc->centralBody(),name,
                                                                           MJDdate[index],
                                                                           "EME J2000",
                                                                           ToCoord);
                                    stream<<Delaunay_h<<"\t";
                                    analysisRow << Delaunay_h;
                                }
                                if(name=="L")
                                {
                                    double Delaunay_L=calcDelaunayElements(Vector,arc->centralBody(),name,
                                                                           MJDdate[index],
                                                                           "EME J2000",
                                                                           ToCoord);
                                    stream<<Delaunay_L<<"\t";
                                    analysisRow << Delaunay_L;
                                }
                                if(name=="G")
                                {
                                    double Delaunay_G=calcDelaunayElements(Vector,arc->centralBody(),name,
                                                                           MJDdate[index],
                                                                           "EME J2000",
                                                                           ToCoord);
                                    stream<<Delaunay_G<<"\t";
                                    analysisRow << Delaunay_G;
                                }
                                if(name=="H")
                                {
                                    double Delaunay_H=calcDelaunayElements(Vector,arc->centralBody(),name,
                                                                           MJDdate[index],
                                                                           "EME J2000",
                                                                           ToCoord);
                                    stream<<Delaunay_H<<"\t";
                                    analysisRow << Delaunay_H;
                                }
                            }

                            if (name=="Covered Area" && m_propagatedScenario->coverageTriggered())
                            {
#if 0
                                QString Units = ReadUnits(treeWidgetShowInReport,parameter);
                                if(Units == "km^2")
                                {
                                    stream << m_studyOfConstellations->m_constellationStudySpaceObjectList.at(MParentIndex.at(z)).coveragesample.at(index).coveredArea<<"\t";
                                }else{
                                    stream << m_studyOfConstellations->m_constellationStudySpaceObjectList.at(MParentIndex.at(z)).coveragesample.at(index).coveredAreaInPerCent<<"\t";
                                }
#endif
                            }

                            if ((name=="Latitude")          ||
                                (name=="Longitude")         ||
                                (name=="Radial Distance")   ||
                                (name=="Flight Path Angle") ||
                                (name=="Heading Angle")     ||
                                (name=="Velocity Modulus")  ||
                                (name=="Altitude"))
                            {
                                QString ToCoord = ReadCoordinateSys(treeWidgetShowInReport,parameter);
                                QString Units = ReadUnits(treeWidgetShowInReport,parameter);

                                sta::StateVector Vector[inumber];
                                sta::StateVector ModifVector[inumber];
                                Vector[index]=arc->trajectorySample(j);
                                sta::CoordinateSystem EME2000("INERTIAL J2000");
                                ModifVector[index]=CoordinateSystem::convert(Vector[index],
                                                                             MJDdate[index],
                                                                             arc->centralBody(),
                                                                             EME2000,
                                                                             arc->centralBody(),
                                                                             //analysis::CoordSys(ToCoord));
                                                                             CoordSys(ToCoord));
                                double SphericalElements[6]; // tau, delta, r, V, gamma, chi
                                cartesianTOspherical(ModifVector[index].position.x(),ModifVector[index].position.y(),ModifVector[index].position.z(),
                                                     ModifVector[index].velocity.x(),ModifVector[index].velocity.y(),ModifVector[index].velocity.z(),
                                                     SphericalElements[0],SphericalElements[1],SphericalElements[2],SphericalElements[3],SphericalElements[4],
                                                     SphericalElements[5]);
                                if(name=="Latitude")
                                {
                                    stream<<sta::ConvertUnits(Units,SphericalElements[1],"rad")<<"\t";
                                    analysisRow<<sta::ConvertUnits(Units,SphericalElements[1],"rad");
                                }
                                if(name=="Longitude")
                                {
                                    stream<<sta::ConvertUnits(Units,SphericalElements[0],"rad")<<"\t";
                                    analysisRow<<sta::ConvertUnits(Units,SphericalElements[0],"rad");
                                }
                                if(name=="Radial Distance")
                                {
                                    stream<<sta::ConvertUnits(Units,SphericalElements[2],"km")<<"\t";
                                    analysisRow<<sta::ConvertUnits(Units,SphericalElements[2],"km");
                                }
                                if(name=="Altitude")
                                {
                                    stream<<sta::ConvertUnits(Units,SphericalElements[2]-arc->centralBody()->meanRadius(),"km")<<"\t";
                                    analysisRow<<sta::ConvertUnits(Units,SphericalElements[2]-arc->centralBody()->meanRadius(),"km");
                                }
                                if(name=="Flight Path Angle")
                                {
                                    stream<<sta::ConvertUnits(Units,SphericalElements[4],"rad")<<"\t";
                                    analysisRow<<sta::ConvertUnits(Units,SphericalElements[4],"rad");
                                }
                                if(name=="Heading Angle")
                                {
                                    stream<<sta::ConvertUnits(Units,SphericalElements[5],"rad")<<"\t";
                                    analysisRow<<sta::ConvertUnits(Units,SphericalElements[5],"rad");
                                }
                                if(name=="Velocity Modulus")
                                {

                                    stream<<sta::ConvertUnits(Units,SphericalElements[3],"km/s")<<"\t";
                                    analysisRow<<sta::ConvertUnits(Units,SphericalElements[3],"km/s");
                                }
                            }

                            if((name=="e*sin(omegaBar)")||(name=="e*cos(omegaBar)")||(name=="tan(i/2)*sin(raan)")||(name=="tan(i/2)*cos(raan)")||(name=="Mean Longitude"))
                            {
                                QString ToCoord = ReadCoordinateSys(treeWidgetShowInReport,parameter);
                                QString Units = ReadCoordinateSys(treeWidgetShowInReport,parameter);

                                sta::StateVector Vector[inumber];
                                Vector[index]=arc->trajectorySample(j);
                                if(name=="e*sin(omegaBar)")
                                {
                                    double esin=calcEquinoctialElements(Vector[index],arc->centralBody(),"e*sin(omegaBar)",
                                                                        MJDdate[index],
                                                                        "EME J2000",
                                                                        ToCoord);
                                    //stream<<sta::ConvertUnits(Units,esin)<<"\t";
                                    stream<<esin<<"\t";
                                    analysisRow << esin;
                                }
                                if(name=="e*cos(omegaBar)")
                                {
                                    double ecos=calcEquinoctialElements(Vector[index],arc->centralBody(),"e*cos(omegaBar)",
                                                                        MJDdate[index],
                                                                        "EME J2000",
                                                                        ToCoord);
                                    // stream<<sta::ConvertUnits(Units,ecos)<<"\t";
                                    stream<<ecos<<"\t";
                                    analysisRow << ecos;
                                }
                                if(name=="tan(i/2)*sin(raan)")
                                {
                                    double etansin=calcEquinoctialElements(Vector[index],arc->centralBody(),"tan(i/2)*sin(raan)",
                                                                           MJDdate[index],
                                                                           "EME J2000",
                                                                           ToCoord);
                                    // stream<<sta::ConvertUnits(Units,etansin)<<"\t";
                                    stream<<etansin<<"\t";
                                    analysisRow << etansin;
                                }
                                if(name=="tan(i/2)*cos(raan)")
                                {
                                    double etancos=calcEquinoctialElements(Vector[index],arc->centralBody(),"tan(i/2)*cos(raan)",
                                                                           MJDdate[index],
                                                                           "EME J2000",
                                                                           ToCoord);
                                    //stream<<sta::ConvertUnits(Units,etancos)<<"\t";
                                    stream<<etancos<<"\t";
                                    analysisRow << etancos;
                                }
                                if(name=="Mean Longitude")
                                {
                                    double MeanLon=calcEquinoctialElements(Vector[index],arc->centralBody(),"Mean Longitude",
                                                                           MJDdate[index],
                                                                           "EME J2000",
                                                                           ToCoord);
                                    //stream<<sta::ConvertUnits(Units,MeanLon)<<"\t";
                                    stream<<MeanLon<<"\t";
                                    analysisRow << MeanLon;
                                }
                            }
                        }
                        stream<<"\r\n";
                        analysisResult.appendRow(analysisRow);
                        analysisRow.clear();
                        numberOfRows = numberOfRows + 1;
                    }
                }
                else
                {
                    stream<<"#######Beginning of time"<<" "<<(k+1)<<"######"<<"\r\n";
                    stream<<"No data available for the chosen time interval, please check the options of the propagation"<<"\r\n";
                    if(selectedTimes.size()==1)
                    {
                        QMessageBox Warning;
                        Warning.setText("No data available for the selected time interval");
                        Warning.exec();
                    }
                }
            }

            stream<<"\r\n";
        }
    }

    file.close();

    if (CStart != 0 || CStop != 0)
    {
        QMessageBox::warning(this, "Analysis error", "No data available for the selected time interval");
        analysisResult.clear();
    }

    return analysisResult;
}


/*
 * Inputs: column-column number in the treeWidgetTimeSpecifications (start or end epochs);
 *         MJD-pointer to the variable that will save the time in Modified Julian Date
 */
void
analysisParametersChoice::ReadTime(int column, QVector<double>& MJD)
{
    QList<QTreeWidgetItem*>selectedTimes=treeWidgetTimeSpecifications->selectedItems();

    if (selectedTimes.size() >0)
    {
        int i;
        double JD;

        for (i=0;i<selectedTimes.size();i++)
        {
            QDateTime d = QDateTime::fromString(selectedTimes[i]->text(column), TimeRangeSelectionDateFormat);
            if (!d.isValid())
            {
                qDebug() << "Badly formatted date in analysis: " << selectedTimes[i];
            }
            qDebug() << i << ", " << d;

            JD = calendarTOjulian(d.date().year(), d.date().month(), d.date().day(),
                                  d.time().hour(), d.time().minute(), d.time().second());
            MJD[i] = sta::JdToMjd(JD);
        }
    }
    else
    {
        //      return 0;  ;
    }
}


/*
 * reads the name of the parameter that will be displayed in the output of the Analysis Module
 * Inputs: item of the tree that is being considered (report or plot options)
 */
QString
analysisParametersChoice::ReadParameter(QTreeWidgetItem*Item)
{
    QTreeWidgetItem*parameter=Item;
    QString name=parameter->text(0);
    return name;
}


/*
 * reads the units the user selected as output units
 * Inputs: Tree- widget from where data shall be read, Item- line of the widget that is being considered
 */
QString
analysisParametersChoice::ReadUnits(QTreeWidget* Tree, QTreeWidgetItem* Item)
{
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


/*
 * reads the coordinate system the user selected as output coordinate system
 * Inputs: Tree- widget from where data shall be read, Item- line of the widget that is being considered
 */
QString
analysisParametersChoice::ReadCoordinateSys(QTreeWidget* Tree, QTreeWidgetItem* Item)
{
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




AnalysisResult::AnalysisResult(unsigned int columnCount)
{
    for (unsigned int i = 0; i < columnCount; ++i)
    {
        m_columnNames << "";
    }
}


void
AnalysisResult::appendRow(const QVariantList& values)
{
    if (values.count() == columnCount())
    {
        m_data << values;
    }
}

int
AnalysisResult::columnCount() const
{
    return m_columnNames.count();
}


int
AnalysisResult::rowCount() const
{
    return m_data.count() / columnCount();
}


void
AnalysisResult::setTitle(const QString& title)
{
    m_title = title;
}


QString
AnalysisResult::columnName(int column) const
{
    if (column >= 0 && column < columnCount())
    {
        return m_columnNames.at(column);
    }
    else
    {
        return "";
    }
}


void
AnalysisResult::setColumnName(int column, const QString& name)
{
    if (column >= 0 && column < columnCount())
    {
        m_columnNames[column] = name;
    }
}


void
AnalysisResult::clear()
{
    m_data.clear();
}
