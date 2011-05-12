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
#include "Analysis/analysisTable.h"
#include "Analysis/AnalysisPlot.h"
#include "Scenario/scenario.h"
#include "Main/propagatedscenario.h"
#include "Main/scenariotree.h"
#include "Main/findDataFolder.h"
#include "Astro-Core/statevector.h"
#include "Astro-Core/date.h"
#include "Astro-Core/stacoordsys.h"
#include "Astro-Core/stabody.h"
#include "Astro-Core/stamath.h"
#include "Astro-Core/calendarTOjulian.h"
#include "Astro-Core/cartesianTOorbital.h"
#include "Astro-Core/cartesianTOspherical.h"
#include "Astro-Core/date.h"
#include "Coverage/commanalysis.h"
#include "Coverage/coverageanalysis.h"

#include "Plotting/PlotView.h"
#include "Plotting/PlotView3D.h"
#include "Plotting/TimePlotScale.h"
#include "Help/HelpBrowser.h"

#include "Services/serviceTimeParameter.h"
#include "Services/serviceTimeUnit.h"

#include "Constellations/cstudy.h"

#include "Analysis/qtiplotmain.h"

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

#ifdef Q_WS_MAC
#include <CoreFoundation/CFBundle.h>
#endif


class ScenarioTree;

class HelpBrowser;


// Format for date/time strings shown to user when selecting a time range
static const QString TimeRangeSelectionDateFormat = "d/M/yyyy hh:mm:ss";


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


analysis::analysis(SpaceScenario*scenario,
                   PropagatedScenario* propagatedScenario,
                   QWidget* parent,
                   Qt::WindowFlags f) :
    QDialog(parent,f),
    m_scenario(NULL),
    m_propagatedScenario(NULL),
    m_studyOfConstellations(NULL),
    m_qtiPlotFrame(NULL)
{
    setupUi(this);
    m_scenario = new SpaceScenario(*scenario);
    m_propagatedScenario = new PropagatedScenario(*propagatedScenario);
    if(m_propagatedScenario->coverageTriggered())
    {
        m_studyOfConstellations=new ConstellationStudy(*propagatedScenario->studyOfConstellations());
    }

    readScenario();
    DisableUnavailableOptions(m_propagatedScenario->coverageTriggered());
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
                QTreeWidgetItem* loiterItem = new QTreeWidgetItem(trajectoryPlan);

                loiterItem->setText(0, trajectory->ElementIdentifier()->Name());
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


void analysis::ReadTime(int column, QVector<double>& MJD) //reads the time in the treeWidgetTimeSpecifications
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
 return 5-the access time is not available in a multi parameter report
      */
    int CovCommCount=0;
    int AccessControl=0;
    if(tree[0]==treeWidgetShowInReport)
    {
        for(int i=0;i<treeWidgetShowInReport->topLevelItemCount();i++)
        {
            QTreeWidgetItem*ShowInReport=treeWidgetShowInReport->topLevelItem(i);
            QString ToReport=ShowInReport->text(0);

            if((ToReport=="Azimuth")||(ToReport=="Elevation")||(ToReport=="Range")||(ToReport=="EIRP")||(ToReport=="Received Frequency")||(ToReport=="Doppler Shift")||(ToReport=="Received Power")||(ToReport=="Flux Density")||(ToReport=="Overlap Bandwidth Factor")||(ToReport=="Free Space Loss")||(ToReport=="Oxygen Loss")||(ToReport=="Water Vapour Loss")||(ToReport=="Rain Loss")||(ToReport=="Atmospheric Loss")||(ToReport=="Propagation Loss")||(ToReport=="G/T")||(ToReport=="C/No")||(ToReport=="C/N")||(ToReport=="Eb/No")||(ToReport=="BER"))
            {

                CovCommCount++;
            }
            if((ToReport=="Access Time")&&treeWidgetShowInReport->topLevelItemCount()!=1)
            {

                CovCommCount++;
                AccessControl++;
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
                if((ToShow=="Azimuth")||(ToShow=="Elevation")||(ToShow=="Range")||(ToShow=="EIRP")||(ToShow=="Received Frequency")||(ToShow=="Doppler Shift")||(ToShow=="Received Power")||(ToShow=="Flux Density")||(ToShow=="Overlap Bandwidth Factor")||(ToShow=="Free Space Loss")||(ToShow=="Oxygen Loss")||(ToShow=="Water Vapour Loss")||(ToShow=="Rain Loss")||(ToShow=="Atmospheric Loss")||(ToShow=="Propagation Loss")||(ToShow=="G/T")||(ToShow=="C/No")||(ToShow=="C/N")||(ToShow=="Eb/No")||(ToShow=="BER"))
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

    if(CovCommCount!=0&&AccessControl==0)
    {
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

    }
    if (tree[0]==treeWidgetShowInReport)
    {
        if( AccessControl!=0)
        {

            return 5;
        }
    }
    //else
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
    if(i==5)
    {
        QMessageBox AccessWarning;
        AccessWarning.setText("The Access Time is not available in a multi parameter report");
        AccessWarning.exec();
        return;
    }
}


// Create an appropriate plot scale based on the parameter and units requested.
// Right now, this function does special handling of time/data parameters and
// assigns a simple linear scale for everything else.
static
        PlotScale* CreatePlotScale(const QString& parameterName, const QString& units, double minValue, double maxValue)
{
    PlotScale* scale = NULL;

    if (parameterName == "Time")
    {
        if (units == "Gregorian UTC")
        {
            scale = new TimePlotScale(minValue, maxValue);
            DateLabelFormatter formatter(DateLabelFormatter::GregorianUTC);
            scale->setLabelFormatter(&formatter);
        }
        else if (units == "Gregorian LCL")
        {
            scale = new TimePlotScale(minValue, maxValue);
            DateLabelFormatter formatter(DateLabelFormatter::GregorianLocal);
            scale->setLabelFormatter(&formatter);
        }
        else if (units == "Mission Elapsed")
        {
            scale = new TimePlotScale(minValue, maxValue);
            DateLabelFormatter formatter(DateLabelFormatter::Elapsed);
            scale->setLabelFormatter(&formatter);
        }
        else if (units == "Julian UTC")
        {
            scale = new TimePlotScale(minValue, maxValue);
            DateLabelFormatter formatter(DateLabelFormatter::JulianUTC);
            scale->setLabelFormatter(&formatter);
        }
        else if (units == "Julian LCL")
        {
            scale = new TimePlotScale(minValue, maxValue);
            DateLabelFormatter formatter(DateLabelFormatter::JulianLocal);
            scale->setLabelFormatter(&formatter);
        }
        else if (units == "YYDDD")
        {
            scale = new TimePlotScale(minValue, maxValue);
            DateLabelFormatter formatter(DateLabelFormatter::YearAndDayNumber);
            scale->setLabelFormatter(&formatter);
        }
    }

    // If we didn't create a special scale, just use the default
    if (!scale)
    {
        scale = new LinearPlotScale(minValue, maxValue);
    }

    return scale;
}


void analysis::setPlotScales()
{
    double minX = 0.0;
    double minY = 0.0;
    double maxX = 0.0;
    double maxY = 0.0;
    plotView2D->extrema(minX, minY, maxX, maxY);


    QList<QTreeWidgetItem*> xSelection = treeWidgetXaxis->selectedItems();
    QList<QTreeWidgetItem*> ySelection = treeWidgetYaxis->selectedItems();

    QString xParamName = analysis::ReadParameter(xSelection.at(0));
    QString yParamName = analysis::ReadParameter(ySelection.at(0));
    QString xUnits = analysis::ReadUnits(treeWidgetXaxis, xSelection.at(0));
    QString yUnits = analysis::ReadUnits(treeWidgetYaxis, ySelection.at(0));

    PlotScale* xAxisScale = CreatePlotScale(xParamName, xUnits, minX, maxX);
    PlotScale* yAxisScale = CreatePlotScale(yParamName, yUnits, minY, maxY);
    plotView2D->setHorizontalScale(*xAxisScale);
    plotView2D->setVerticalScale(*yAxisScale);

    // Clean up; the PlotView keeps its own copies of the scales
    delete xAxisScale;
    delete yAxisScale;
    //plotView2D->setHorizontalScale();
    //plotView2D->autoScale();
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

    // Abort if there are no parameters
    if(numberOfParameters == 0)
    {
        QMessageBox parametersWarning;
        parametersWarning.setText("Please select the parameters to be calculated");
        parametersWarning.exec();
        return;
    }

    // Abort if there are no mission arcs selected
    if (selected.isEmpty())
    {
        QMessageBox MissionWarning;
        MissionWarning.setText("Please select one or more Mission Arcs");
        MissionWarning.exec();
        return;
    }

    // Abort if there are no time intervals selected
    if (selectedTimes.size()==0)
    {
        QMessageBox tswarning;
        tswarning.setText("Please select one or more time intervals");
        tswarning.exec();
        return;
    }

    int Control=InputsControl(tree); //checks if the selected parameters are valid according to the inputs n other module functions
    if(Control!=0)
    {
        Warnings(Control);
        return;
    }

    // Write the ouput
    QList<analysis::AnalysisData> DataStructure;
    if (AnalysisFormat=="Report")
    {
        WriteReport(selected,selectedTimes);
    }
    else if((AnalysisFormat=="2D")||(AnalysisFormat=="3D"))
    {
        DataStructure=WriteDataStructure(selected,selectedTimes);
    }

    // Show the report: spreadsheet, 2D, or 3D
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
            QWidget *window2D = new QWidget(this, Qt::Window);
            QVBoxLayout* layout = new QVBoxLayout(window2D);

            plotView2D = new PlotView(window2D);
            layout->addWidget(plotView2D);

            AnalysisPlot::AnalysisPlot* Data = new AnalysisPlot();
            Data->setPoints(DataStructure,numberOfLines,numberOfParameters);
            plotView2D->addPlot(Data);

            QString PlotTitle = "";
            PlotTitle = PlotTitle + DataStructure[1].ParameterTitles[0];
            PlotTitle = PlotTitle + " = f ( ";
            PlotTitle = PlotTitle + DataStructure[0].ParameterTitles[0];
            PlotTitle = PlotTitle + " )";

            plotView2D->setTitle(PlotTitle);
            plotView2D->setLeftLabel(DataStructure[1].ParameterTitles[0]);
            plotView2D->setBottomLabel(DataStructure[0].ParameterTitles[0]);
            setPlotScales();
            plotView2D->setMinimumSize(640, 480); // Rectangle

            QHBoxLayout *buttonLayout = new QHBoxLayout;
            buttonLayout->addStretch();
            QPushButton *saveButton = new QPushButton(tr("Save"));
            buttonLayout->addWidget(saveButton);
            connect(saveButton, SIGNAL(clicked()), this, SLOT(saveImage2D()));
            QPushButton *closeButton = new QPushButton(tr("Close"));
            closeButton->setShortcut(tr("Esc"));
            closeButton->setAutoDefault(true);
            connect(closeButton, SIGNAL(clicked()), window2D, SLOT(close()));
            buttonLayout->addWidget(closeButton);
            layout->addLayout(buttonLayout);

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
    else if(AnalysisFormat=="3D")
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
            QWidget *window3D = new QWidget(this, Qt::Window);

            QVBoxLayout* layout = new QVBoxLayout(window3D);

            plotView3D = new PlotView3D(window3D);
            layout->addWidget(plotView3D);

            Analysis3D::Analysis3D *Data = new Analysis3D();
            Data->setPoints(DataStructure,numberOfLines,numberOfParameters);

            plotView3D->addPlot(Data, PlotStyle(PlotStyle::LinePlot, QPen(Qt::red, 2, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin)));

            QString PlotTitle = "";
            PlotTitle = PlotTitle + DataStructure[2].ParameterTitles[0];
            PlotTitle = PlotTitle + " = f (";
            PlotTitle = PlotTitle + DataStructure[0].ParameterTitles[0];
            PlotTitle = PlotTitle + ", ";
            PlotTitle = PlotTitle + DataStructure[1].ParameterTitles[0];
            PlotTitle = PlotTitle + ")";

            plotView3D->setTitle(PlotTitle);
            plotView3D->setXLabel(DataStructure[0].ParameterTitles[0]);
            plotView3D->setYLabel(DataStructure[1].ParameterTitles[0]);
            plotView3D->setZLabel(DataStructure[2].ParameterTitles[0]);
            plotView3D->autoScale();
            plotView3D->setMinimumSize(600, 600);

            QHBoxLayout *buttonLayout = new QHBoxLayout;
            buttonLayout->addStretch();
            QPushButton *saveButton = new QPushButton(tr("Save"));
            buttonLayout->addWidget(saveButton);
            connect(saveButton, SIGNAL(clicked()), this, SLOT(saveImage3D()));
            QPushButton *closeButton = new QPushButton(tr("Close"));
            closeButton->setShortcut(tr("Esc"));
            closeButton->setAutoDefault(true);
            connect(closeButton, SIGNAL(clicked()), window3D, SLOT(close()));
            buttonLayout->addWidget(closeButton);
            layout->addLayout(buttonLayout);

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


void analysis::WriteReport(QList<QTreeWidgetItem *> selected,QList<QTreeWidgetItem *> selectedTimes)
{
    // Identifying the resources path
    QString staResourcesPath = findDataFolder();

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
    QString analysisFile = staResourcesPath + "/" + "analysisReport.txt";
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
        QString name = analysis::ReadParameter(treeWidgetShowInReport->topLevelItem(itemIndex));
        if (name != "Access Time")
        {
            QString Unit = analysis::ReadUnits(treeWidgetShowInReport,treeWidgetShowInReport->topLevelItem(itemIndex));
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
                        QString name=analysis::ReadParameter(treeWidgetShowInReport->topLevelItem(i));
                        if(name!="Access Time")
                        {
                            QString Unit=analysis::ReadUnits(treeWidgetShowInReport,treeWidgetShowInReport->topLevelItem(i));
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

                        if(name=="Access Time")
                        {

                            QList< QList<double> > AccessTime= analysis::calcAccessTime(countStop[k],countStart[k], AccessNumber, AccessStep, AccessNow,CovIndex[3],LineOfCoverageReport,arc);

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
                                QString Coordinate = analysis::ReadCoordinateSys(treeWidgetShowInReport,parameter);
                                QString Units = analysis::ReadUnits(treeWidgetShowInReport,parameter);

                                double outputValue = 0.0;

                                double mjd = MJDdate[index];

                                //Added by Catarina
                                if (Coordinate == "Mean of Epoch")
                                {
                                     mjd = MJDdate[0];
                                }

                                StateVector state = arc->trajectorySample(0);
                                        state = arc->trajectorySample(j);

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

                                QString TimeCoordinate=analysis::ReadCoordinateSys(treeWidgetShowInReport,parameter);

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

                                QString Units = analysis::ReadUnits(treeWidgetShowInReport,parameter);

                                double mjd = MJDdate[index];
                                sta::StateVector state;
                                spaceObj->getStateVector(mjd, *body, CoordinateSystem(COORDSYS_EME_J2000), &state);

                                stream << sta::ConvertUnits(Units, state.position.norm(), "km") << "\t";
                                analysisRow << sta::ConvertUnits(Units, state.position.norm(), "km");
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
                                QString ToCoord=analysis::ReadCoordinateSys(treeWidgetShowInReport,parameter);

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
                                QString ToCoord=analysis::ReadCoordinateSys(treeWidgetShowInReport,parameter);
                                QString ToUnit=analysis::ReadUnits(treeWidgetShowInReport,parameter);

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
                                QString ToCoord=analysis::ReadCoordinateSys(treeWidgetShowInReport,parameter);
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
                            if(name=="Covered Area" && m_propagatedScenario->coverageTriggered())
                            {
                                QString Units=analysis::ReadUnits(treeWidgetShowInReport,parameter);
                                if(Units == "km^2")
                                {
                                    stream<<m_studyOfConstellations->m_constellationStudySpaceObjectList.at(MParentIndex.at(z)).coveragesample.at(index).coveredArea<<"\t";
                                }else{
                                    stream<<m_studyOfConstellations->m_constellationStudySpaceObjectList.at(MParentIndex.at(z)).coveragesample.at(index).coveredAreaInPerCent<<"\t";
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
                                                                             arc->centralBody(),
                                                                             EME2000,
                                                                             arc->centralBody(),
                                                                             analysis::CoordSys(ToCoord));
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
                                QString ToCoord=analysis::ReadCoordinateSys(treeWidgetShowInReport,parameter);
                                QString Units=analysis::ReadCoordinateSys(treeWidgetShowInReport,parameter);

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
        QMessageBox Warning;
        Warning.setText("No data available for the selected time interval");
        Warning.exec();
    }
    else
    {
        // Call QtiPlot to create a table from the analysis results
        QtiPlotMain* qtiPlot = findChild<QtiPlotMain*>("QTIPLOT");
        if (!qtiPlot)
        {
            qtiPlot = new QtiPlotMain(false, this);
            qtiPlot->setName("QTIPLOT");
        }

        qtiPlot->createReport(&analysisResult);
    }
}


QList< analysis::AnalysisData> analysis::WriteDataStructure(QList<QTreeWidgetItem*>selected,QList<QTreeWidgetItem*>selectedTimes)
{
    /*
      generates the data structure to send the information to the plotting module
      Inputs: selected- list of the selected lines in the scenario tree of the AM GUI, selectedTimes- list of the selected time intervals
      */

    // Identifying the resources path
    QString staResourcesPath = findDataFolder();

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

                int timeRangeCount = selectedTimes.size();
                QVector<double> StartTime(timeRangeCount); // time specifications, for each line in the time tree
                QVector<double> StopTime(timeRangeCount);
                QVector<int> countStart(timeRangeCount); // time step index from where the data is being collected
                QVector<int> countStop(timeRangeCount);

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
                if(ReadCommunication1==true)
                {
                    CommAnalysis commAnalysis=CommAnalysis(Transmitter, Receiver, Environment, m_propagatedScenario, indSC, indGS, indMissionArc,TxParentType,RxParentType);
                    commAnalysis.CommReports();

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

                        for (int j=countStart[k];j<=countStop[k];j++)
                        {
                            int index = j - countStart[k];

                            MJDdate[index]=arc->trajectorySampleTime(j);

                            //WRITE stream<<MJDdate[index]<<"\t"; //prints MJD by default

                            for(int i=0;i<SelParameters.size();i++)
                            {
                                QTreeWidgetItem*parameter=SelParameters.at(i);
                                QString name=analysis::ReadParameter(SelParameters.at(i));

                                if((name=="x position")||(name=="y position")||(name=="z position")||(name=="x velocity")||(name=="y velocity")||(name=="z velocity"))
                                {
                                    QString Coordinate = analysis::ReadCoordinateSys(Tree.at(a),SelParameters.at(i));
                                    QString Units = analysis::ReadUnits(Tree.at(a),SelParameters.at(i));

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

                                    LineData.append(sta::ConvertUnits(Units, outputValue, "km"));
                                }
                                else if(name=="Time")
                                {
                                    QString TimeCoordinate=analysis::ReadUnits(Tree.at(a),SelParameters.at(i));

                                    // Options of Time
                                    if (TimeCoordinate == "MJD")
                                    {
                                        LineData.append(MJDdate[index]);
                                    }
                                    else if (TimeCoordinate == "Julian Date")
                                    {
                                        LineData.append(sta::MjdToJd(MJDdate[index]));
                                    }
                                    else if (TimeCoordinate == "Mission Elapsed")
                                    {
                                        // Generate times relative to the start epoch
                                        LineData.append(MJDdate[index] - StartEpoch);
                                    }
                                    else if (TimeCoordinate == "Julian UTC"    ||
                                             TimeCoordinate == "Gregorian LCL" ||
                                             TimeCoordinate == "Gregorian UTC" ||
                                             TimeCoordinate == "Julian LCL"    ||
                                             TimeCoordinate == "YYDDD")
                                    {
                                        // For all of these date types, we do nothing special here. The PlotView
                                        // class handles the formatting of dates
                                        LineData.append(MJDdate[index]);
                                    }
                                }

                                else if(name=="Epoch")
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
                                else if (IsPlanetDistanceField(name))
                                {
                                    StaBody* body = STA_SOLAR_SYSTEM->lookup(name);
                                    Q_ASSERT(body != NULL);

                                    QString Units = analysis::ReadUnits(Tree.at(a),SelParameters.at(i));

                                    double mjd = MJDdate[index];
                                    sta::StateVector state;
                                    spaceObj->getStateVector(mjd, *body, CoordinateSystem(COORDSYS_EME_J2000), &state);

                                    LineData.append(sta::ConvertUnits(Units, state.position.norm(), "km"));
                                }
                                else if((name=="Azimuth")||(name=="Elevation")||(name=="Range"))
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
                                else if(name=="EIRP")
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
                                else if(name=="Received Frequency")
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
                                else if(name=="Doppler Shift")
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
                                else if(name=="Received Power")
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
                                else if(name=="Flux Density")
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
                                else if(name=="Overlap Bandwidth Factor")
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
                                else if(name=="Free Space Loss")
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
                                else if(name=="Oxygen Loss")
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
                                else if(name=="Water Vapour Loss")
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
                                else if(name=="Rain Loss")
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
                                else if(name=="Atmospheric Loss")
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
                                else if(name=="Propagation Loss")
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
                                else if (name=="G/T")
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
                                else if(name=="C/No")
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
                                else if(name=="C/N")
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
                                else if(name=="Eb/No")
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
                                else if(name=="BER")
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
                                else if(name=="Eccentricity")
                                {
                                    QString ToCoord=analysis::ReadCoordinateSys(Tree.at(a),SelParameters.at(i));

                                    sta::StateVector Vector=arc->trajectorySample(j);
                                    double Eccentricity=calcKeplerianElements(Vector,arc->centralBody(),"Eccentricity",
                                                                              MJDdate[index],
                                                                              "EME J2000",
                                                                              ToCoord);
                                    //stream<<Eccentricity<<"\t";
                                    LineData.append(Eccentricity);
                                }
                                else if((name=="Inclination")||
                                        (name=="RAAN")||
                                        (name=="Argument of Periapsis")||
                                        (name=="True Anomaly")||
                                        (name=="Semimajor Axis"))
                                {
                                    QString ToCoord=analysis::ReadCoordinateSys(Tree.at(a),SelParameters.at(i));
                                    QString ToUnit=analysis::ReadUnits(Tree.at(a),SelParameters.at(i));

                                    sta::StateVector Vector = arc->trajectorySample(j);

                                    if(name=="Inclination")
                                    {
                                        double Inclination=calcKeplerianElements(Vector,arc->centralBody(),name,
                                                                                 MJDdate[index],
                                                                                 "EME J2000",
                                                                                 ToCoord);
                                        //stream<<sta::ConvertUnits(ToUnit,Inclination)<<"\t";
                                        LineData.append(sta::ConvertUnits(ToUnit,Inclination,"rad"));
                                    }
                                    if(name=="RAAN")
                                    {
                                        double Ascending=calcKeplerianElements(Vector,arc->centralBody(),name,
                                                                               MJDdate[index],
                                                                               "EME J2000",
                                                                               ToCoord);
                                        //stream<<sta::ConvertUnits(ToUnit,Ascending)<<"\t";
                                        LineData.append(sta::ConvertUnits(ToUnit,Ascending,"rad"));
                                    }
                                    if(name=="True Anomaly")
                                    {
                                        double TrueAnom=calcKeplerianElements(Vector,arc->centralBody(),name,
                                                                              MJDdate[index],
                                                                              "EME J2000",
                                                                              ToCoord);
                                        //stream<<sta::ConvertUnits(ToUnit,TrueAnom)<<"\t";
                                        LineData.append(sta::ConvertUnits(ToUnit,TrueAnom,"rad"));
                                    }
                                    if(name=="Argument of Periapsis")
                                    {
                                        double Periapsis=calcKeplerianElements(Vector,arc->centralBody(),name,
                                                                               MJDdate[index],
                                                                               "EME J2000",
                                                                               ToCoord);
                                        //stream<<sta::ConvertUnits(ToUnit,Periapsis)<<"\t";
                                        LineData.append(sta::ConvertUnits(ToUnit,Periapsis,"rad"));
                                    }
                                    if(name=="Semimajor Axis")
                                    {
                                        double SemAxis=calcKeplerianElements(Vector,arc->centralBody(),name,
                                                                             MJDdate[index],
                                                                             "EME J2000",
                                                                             ToCoord);
                                        //stream<<sta::ConvertUnits(ToUnit,SemAxis)<<"\t";
                                        LineData.append(sta::ConvertUnits(ToUnit,SemAxis,"km"));
                                    }

                                }
                                else if((name=="l")||
                                        (name=="g")||
                                        (name=="h")||
                                        (name=="L")||
                                        (name=="G")||
                                        (name=="H"))
                                {
                                    QString ToCoord=analysis::ReadCoordinateSys(Tree.at(a),SelParameters.at(i));
                                    sta::StateVector Vector[inumber];
                                    Vector[index]=arc->trajectorySample(j);

                                    if(name=="l")
                                    {
                                        double Delaunay_l=calcDelaunayElements(Vector[index],arc->centralBody(),name,
                                                                               MJDdate[index],
                                                                               "EME J2000",
                                                                               ToCoord);
                                        //stream<<Delaunay_l<<"\t";
                                        LineData.append(Delaunay_l);
                                    }
                                    if(name=="g")
                                    {
                                        double Delaunay_g=calcDelaunayElements(Vector[index],arc->centralBody(),name,
                                                                               MJDdate[index],
                                                                               "EME J2000",
                                                                               ToCoord);
                                        //stream<<Delaunay_g<<"\t";
                                        LineData.append(Delaunay_g);
                                    }
                                    if(name=="h")
                                    {
                                        double Delaunay_h=calcDelaunayElements(Vector[index],arc->centralBody(),name,
                                                                               MJDdate[index],
                                                                               "EME J2000",
                                                                               ToCoord);
                                        //stream<<Delaunay_h<<"\t";
                                        LineData.append(Delaunay_h);
                                    }
                                    if(name=="L")
                                    {
                                        double Delaunay_L=calcDelaunayElements(Vector[index],arc->centralBody(),name,
                                                                               MJDdate[index],
                                                                               "EME J2000",
                                                                               ToCoord);
                                        //stream<<Delaunay_L<<"\t";
                                        LineData.append(Delaunay_L);
                                    }
                                    if(name=="G")
                                    {
                                        double Delaunay_G=calcDelaunayElements(Vector[index],arc->centralBody(),name,
                                                                               MJDdate[index],
                                                                               "EME J2000",
                                                                               ToCoord);
                                        //stream<<Delaunay_G<<"\t";
                                        LineData.append(Delaunay_G);
                                    }
                                    if(name=="H")
                                    {
                                        double Delaunay_H=calcDelaunayElements(Vector[index],arc->centralBody(),name,
                                                                               MJDdate[index],
                                                                               "EME J2000",
                                                                               ToCoord);
                                        // stream<<Delaunay_H<<"\t";
                                        LineData.append(Delaunay_H);
                                    }
                                }
                                if(name=="Covered Area" && m_propagatedScenario->coverageTriggered())
                                {
                                    QString Units=analysis::ReadUnits(Tree.at(a),SelParameters.at(i));
                                    if(Units == "km^2")
                                    {
                                        LineData.append(m_studyOfConstellations->m_constellationStudySpaceObjectList.at(MParentIndex.at(z)).coveragesample.at(index).coveredArea);
                                    }else{
                                        LineData.append(m_studyOfConstellations->m_constellationStudySpaceObjectList.at(MParentIndex.at(z)).coveragesample.at(index).coveredAreaInPerCent);
                                    }
                                }
                                else if((name=="Latitude")||
                                        (name=="Longitude")||
                                        (name=="Radial Distance")||
                                        (name=="Flight Path Angle")||
                                        (name=="Heading Angle")||
                                        (name=="Velocity Modulus")||
                                        (name=="Altitude"))
                                {
                                    QString ToCoord=analysis::ReadCoordinateSys(Tree.at(a),SelParameters.at(i));
                                    QString Units=analysis::ReadUnits(Tree.at(a),SelParameters.at(i));

                                    sta::StateVector Vector[inumber];
                                    sta::StateVector ModifVector[inumber];
                                    Vector[index]=arc->trajectorySample(j);
                                    sta::CoordinateSystem EME2000("INERTIAL J2000");
                                    ModifVector[index]=CoordinateSystem::convert(Vector[index],
                                                                                 MJDdate[index],
                                                                                 arc->centralBody(),
                                                                                 EME2000,
                                                                                 arc->centralBody(),
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
                                        double radius=arc->centralBody()->meanRadius();
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
                                else if((name=="e*sin(omegaBar)")||
                                        (name=="e*cos(omegaBar)")||
                                        (name=="tan(i/2)*sin(raan)")||
                                        (name=="tan(i/2)*cos(raan)")||
                                        (name=="Mean Longitude"))
                                {
                                    QString ToCoord=analysis::ReadCoordinateSys(Tree.at(a),SelParameters.at(i));
                                    QString Units=analysis::ReadUnits(Tree.at(a),SelParameters.at(i));

                                    sta::StateVector Vector[inumber];
                                    Vector[index]=arc->trajectorySample(j);
                                    if(name=="e*sin(omegaBar)")
                                    {
                                        double esin=calcEquinoctialElements(Vector[index],arc->centralBody(),"e*sin(omegaBar)",
                                                                            MJDdate[index],
                                                                            "EME J2000",
                                                                            ToCoord);

                                        //LineData.append(sta::ConvertUnits(Units,esin));
                                        LineData.append(esin);
                                    }
                                    if(name=="e*cos(omegaBar)")
                                    {
                                        double ecos=calcEquinoctialElements(Vector[index],arc->centralBody(),"e*cos(omegaBar)",
                                                                            MJDdate[index],
                                                                            "EME J2000",
                                                                            ToCoord);

                                        // LineData.append(sta::ConvertUnits(Units,ecos));
                                        LineData.append(ecos);
                                    }
                                    if(name=="tan(i/2)*sin(raan)")
                                    {
                                        double etansin=calcEquinoctialElements(Vector[index],arc->centralBody(),"tan(i/2)*sin(raan)",
                                                                               MJDdate[index],
                                                                               "EME J2000",
                                                                               ToCoord);

                                        // LineData.append(sta::ConvertUnits(Units,etansin));
                                        LineData.append(etansin);
                                    }
                                    if(name=="tan(i/2)*cos(raan)")
                                    {
                                        double etancos=calcEquinoctialElements(Vector[index],arc->centralBody(),"tan(i/2)*cos(raan)",
                                                                               MJDdate[index],
                                                                               "EME J2000",
                                                                               ToCoord);

                                        //LineData.append(sta::ConvertUnits(Units,etancos));
                                        LineData.append(etancos);
                                    }
                                    if(name=="Mean Longitude")
                                    {
                                        double MeanLon=calcEquinoctialElements(Vector[index],arc->centralBody(),"Mean Longitude",
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
        if(Coordinate=="ICRF")
        {
            sta::CoordinateSystem Coord("ICRF");
            return Coord;
        }
        if(Coordinate=="Mean of Date")
        {
            sta::CoordinateSystem Coord("Mean of Date");
            return Coord;
        }
        if(Coordinate=="True of Date")
        {
            sta::CoordinateSystem Coord("True of Date");
            return Coord;
        }



    if(Coordinate==" ")
    {
        sta::CoordinateSystem Coord("INVALID");
        return Coord;
    }
}


/** Calculates the Keplerian Elements of a certain orbit
  * Inputs:
  *     Vector-statevector for each time step
  *     Body-pointer to the central Body
  *     OrbElement- name of the element the user wishes to analyse
  *     mjd-time in Modified Julian Date
  *     FromCoordinate- coordinate system of the data before transformation,
  *     ToCoordinate- coordinate system that shall appear as output)
  *
  * Outputs: parameter to be displayed/analysed
  */
double analysis::calcKeplerianElements(const sta::StateVector& Vector,
                                       const StaBody* Body,
                                       const QString& OrbElement,
                                       double mjd,
                                       const QString& FromCoordinate,
                                       const QString& ToCoordinate)
{
    sta::StateVector ModifVector=CoordinateSystem::convert(Vector,
                                                           mjd,
                                                           Body,
                                                           analysis::CoordSys(FromCoordinate),
                                                           Body,
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


/** Calculates the Delaunay Elements of a certain orbit
  * Inputs:
  *     Vector-statevector for each time step
  *     Body-pointer to the central Body
  *     OrbElement- name of the element the user wishes to analyse
  *     mjd-time in Modified Julian Date
  *     FromCoordinate- coordinate system of the data before transformation,
  *     ToCoordinate- coordinate system that shall appear as output)
  *
  * Outputs: parameter to be displayed/analysed
  */
double analysis::calcDelaunayElements(const sta::StateVector& Vector,
                                      const StaBody* Body,
                                      const QString& OrbElement,
                                      double mjd,
                                      const QString& FromCoordinate,
                                      const QString& ToCoordinate)
{
    sta::StateVector ModifVector=CoordinateSystem::convert(Vector,
                                                           mjd,
                                                           Body,
                                                           analysis::CoordSys(FromCoordinate),
                                                           Body,
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


/** Calculates the Equinoctial Elements of a certain orbit
  *
  * Inputs:Vector-statevector for each time step,
  *        Body-pointer to the central Body
  *        OrbElement- name of the element the user wishes to analyse
  *        mjd-time in Modified Julian Date,
  *        FromCoordinate- coordinate system of the data before transformation,
  *        ToCoordinate- coordinate system that shall appear as output)
  * Outputs: parameter to be displayed/analysed
  */
double analysis::calcEquinoctialElements(const sta::StateVector& Vector,
                                         const StaBody* Body,
                                         const QString& OrbElement,
                                         double mjd,
                                         const QString& FromCoordinate,
                                         const QString& ToCoordinate)
{
    sta::StateVector ModifVector=CoordinateSystem::convert(Vector,
                                                           mjd,
                                                           Body,
                                                           analysis::CoordSys(FromCoordinate),
                                                           Body,
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

QList< QList<double> >  analysis::calcAccessTime(int countStop,int countStart, int AccessNumber, int AccessStep, int AccessNow,int CovIndex,QStringList LineOfCoverageReport,MissionArc*arc)
{
    QList< QList<double> >AccessTime;
    QList <double> AccessList;
    // for each list, AccessTime[0]=number of Access, AccessTime[1]=start time(mjd), AccessTime[2]=stop time(MJD), AccessTime[3]=duration (seconds)

    int totalStepsIndex=countStop-countStart;
    QList<double> MJDaccess;
    double StartTime,StopTime;
    for(int j=countStart;j<=countStop;j++)
    {
        int index=j-countStart;

        MJDaccess.append(arc->trajectorySampleTime(j));

        int readTime=CovIndex;
        if(CovIndex<LineOfCoverageReport.size())
        {

            QString Line=LineOfCoverageReport.at(readTime);
            double TimeCovReport=(Line.section("\t",0,0)).toDouble();

            if(fabs(MJDaccess[index]-TimeCovReport)<10e-6)
            {

                if(AccessStep==0)
                {
                    AccessNumber++;
                }
            }
        }
        readTime++;
    }


    int Size=AccessNumber;
    AccessNumber=0;

    //for(int i=0;i<Size;i++)
    {

        for(int j=countStart;j<=countStop;j++)
        {

            int index=j-countStart;

            MJDaccess.append(arc->trajectorySampleTime(j));


            if(CovIndex<LineOfCoverageReport.size())
            {

                QString Line=LineOfCoverageReport.at(CovIndex);
                double TimeCovReport=(Line.section("\t",0,0)).toDouble();

                if(fabs(MJDaccess[index]-TimeCovReport)<10e-6)
                {

                    if(AccessStep==0)
                    {
                        AccessNumber++;
                        AccessList.append(AccessNumber);
                        StartTime=MJDaccess[index];
                        AccessList.append(StartTime); //start time of the access
                    }

                    //CovIndex++;
                    AccessStep++;
                    AccessNow=1;
                    if(index==totalStepsIndex)
                    {
                        AccessList.append(StopTime);; //access time stop time
                        double duration=sta::MjdToFromEpoch(StartTime, StopTime, "Seconds");
                        AccessList.append(duration);
                    }
                    CovIndex++;
                }
                else
                {

                    AccessStep=0;
                    if(AccessNow==1)
                    {
                        StopTime=MJDaccess[index-1];

                        AccessList.append(StopTime);; //access time stop time
                        double duration=sta::MjdToFromEpoch(StartTime, StopTime, "Seconds");
                        AccessList.append(duration);
                    }
                    AccessNow=0;
                }
            }
            else
            {

            }

            //double duration=sta::MjdToFromEpoch(StartTime, StopTime, "Seconds");
            // AccessList.append(duration);
        }
        AccessTime.append(AccessList);

    }
    return AccessTime;
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


QComboBox*analysis::TimeFramesBox()
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
QComboBox* analysis::CoveredAreaUnitsBox()
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
        //This group has been patched by Guillermo
        treeWidgetShowInReport->setItemWidget(item,1,TimeFramesBox());
        treeWidgetShowInReport->setItemWidget(item,2,NoUnitsBox());
       // Next lines oatched by Guillermo
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


void
        analysis::SetPlotComboBox(const QString& name, QTreeWidget* treeWidget, QTreeWidgetItem* item)
{
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
        treeWidget->setItemWidget(item,1,CoordinateBox());
        treeWidget->setItemWidget(item,2,DistanceUnitsBox());
    }
    else if(name=="Time")
    {
        treeWidget->setItemWidget(item,1,TimeFramesBox());
        treeWidget->setItemWidget(item,2,NoUnitsBox());
    }
    else if(name=="Epoch")
    {
        treeWidget->setItemWidget(item,1,NoUnitsBox());
        treeWidget->setItemWidget(item,2,TimeUnitsBox());
    }
    else if((name=="Eccentricity")||
            (name=="L")||
            (name=="G")||
            (name=="l")||
            (name=="g")||
            (name=="h")||
            (name=="H"))
    {
        treeWidget->setItemWidget(item,1,CoordinateBox());
        treeWidget->setItemWidget(item,2,NoUnitsBox());
    }
    else if((name=="e*sin(omegaBar)")||(name=="e*cos(omegaBar)")||(name=="tan(i/2)*sin(raan)")||(name=="tan(i/2)*cos(raan)"))
    {
        treeWidget->setItemWidget(item,1,CoordinateBox());
        treeWidget->setItemWidget(item,2,NoUnitsBox());
    }
    else if((name=="x velocity")||
            (name=="y velocity")||
            (name=="z velocity"))
    {
        treeWidget->setItemWidget(item,1,CoordinateBox());
        treeWidget->setItemWidget(item,2,VelocityUnitsBox());
    }
    else if ((name=="Access Time")||
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
        treeWidget->setItemWidget(item,1,TimeFramesBox());
        treeWidget->setItemWidget(item,2,TimeUnitsBox());
    }
    else if((name=="Inclination")||
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
        treeWidget->setItemWidget(item,1,CoordinateBox());
        treeWidget->setItemWidget(item,2,AngleUnitsBox());
    }
    else if(name=="Covered Area" && m_propagatedScenario->coverageTriggered())
    {
        treeWidget->setItemWidget(item,1,NoUnitsBox());
        treeWidget->setItemWidget(item,2,CoveredAreaUnitsBox());
    }
}


void analysis::PlotComboBox()
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
            for(int k=0;k<topItem->childCount();k++)
            {
                if(topItem->child(k)->childCount()==0)
                {
                    QTreeWidgetItem*item=topItem->child(k);
                    QString name=item->text(0);
                    SetPlotComboBox(name, Tree.at(a), item);
                }
                else
                {
                    for (int l=0;l<topItem->child(k)->childCount();l++)
                    {
                        QTreeWidgetItem*item=topItem->child(k)->child(l);
                        QString name=item->text(0);
                        SetPlotComboBox(name, Tree.at(a), item);
                    }
                }
            }
        }
    }
}


void analysis::DisableUnavailableOptions(bool coverageTrigg)
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
                    if(topItem->child(k)->text(0)=="Covered Area" && !coverageTrigg)
                    {
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



void analysis::saveImage2D()
{
    QPixmap image = QPixmap::grabWindow(plotView2D->winId());
    QString fileName = QFileDialog::getSaveFileName(plotView2D,
                                                    tr("Save Image"),
                                                    "",
                                                    tr("Images (*.png  *.jpg *.tif *.pdf)"));
    if (!fileName.isEmpty())
    {
        if (fileName.toLower().endsWith(".pdf"))
        {
            // Save as a PDF file by printing to a file
            QPrinter pdfPrinter;
            pdfPrinter.setOrientation(QPrinter::Landscape);
            pdfPrinter.setOutputFileName(fileName);
            pdfPrinter.setOutputFormat(QPrinter::PdfFormat);

            QPainter pdfPainter(&pdfPrinter);
            this->plotView2D->paint(pdfPainter);
        }
        else
        {
            bool ok = image.save(fileName);
            if (!ok)
            {
                QMessageBox::warning(this, tr("Save error"), tr("Error saving image to %1").arg(fileName));
            }
        }
    }
}



void analysis::saveImage3D()
{

    QPixmap image = QPixmap::grabWindow(plotView3D->winId());
    QString fileName = QFileDialog::getSaveFileName(plotView3D,
                                                    tr("Save Image"),
                                                    "",
                                                    tr("Images (*.png  *.jpg *.tif)"));
    if (!fileName.isEmpty())
    {
        bool ok = image.save(fileName);
        if (!ok)
        {
            QMessageBox::warning(this, tr("Save error"), tr("Error saving image to %1").arg(fileName));
        }
    }
}


