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



analysisParametersChoice::analysisParametersChoice( QWidget * parent) : QTabWidget(parent)
{
	setupUi(this);
}

analysisParametersChoice::~analysisParametersChoice()
{
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

/*

    foreach (QSharedPointer<ScenarioParticipantType> participant, m_scenario->AbstractParticipant())
    {
        int GroundStation=0; //identify the type of Object
        int SpaceCraft=1; //identify the type of Object

        if ((dynamic_cast<ScenarioSC*>(participant.data())!=NULL)) //SC participant
        {
            QTreeWidgetItem* spacecraftItem = new QTreeWidgetItem(scenarioTabAnalysisChoiceStreeView);
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
            QTreeWidgetItem * REVitem = new QTreeWidgetItem(scenarioTabAnalysisChoiceStreeView);
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
            QTreeWidgetItem * GSitem = new QTreeWidgetItem(scenarioTabAnalysisChoiceStreeView);
            GSitem->setExpanded(true);
            GSitem->setText(0,participant.data()->Name());
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

    */

}
