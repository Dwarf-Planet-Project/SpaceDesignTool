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
#include <QDebug>
#include <QComboBox>


using namespace std;

analysis::analysis( QWidget * parent, Qt::WindowFlags f) : QDialog(parent,f)
{
	setupUi(this);
}

analysis::~analysis()
{
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


// Methods intoduced by Ana that need to be re-worked for the new GUI

/*

void Analysis::on_groupBoxAnalysisFormat_toggled(bool)
{
	qWarning("TODO: %s	%d",__FILE__,__LINE__);
}

//void Analysis::on_ComboBoxAnalysisFormat_activated(const QString&)
//{
//	qWarning("TODO: %s	%d",__FILE__,__LINE__);
//}

void Analysis::on_groupBoxParameters_toggled(bool)
{
	qWarning("TODO: %s	%d",__FILE__,__LINE__);
}

void Analysis::on_groupBoxTimeSpecifications_toggled(bool)
{
	qWarning("TODO: %s	%d",__FILE__,__LINE__);
}

bool Analysis::CheckIfMissionArc() //number of children
{
	QList<QTreeWidgetItem *> selectedMissionArcs=TreeWidgetMissionArc->selectedItems();
	if (selectedMissionArcs.isEmpty())
	{
	    return(false);
	}
	else
	{
	int numberOfchildren=selectedMissionArcs[0]->childCount();
	if (numberOfchildren==0)
	    return (true);
	else
	    return (false);
    }


  }

void Analysis::on_AddDefaultPushButton_clicked()
{
    foreach (QSharedPointer<ScenarioParticipantType> participant, m_scenario->AbstractParticipant())
    {
	if (CheckIfMissionArc())
	{
	    if ((dynamic_cast<ScenarioSC*>(participant.data())!=NULL))
	    {

	    ScenarioSC* vehicle = dynamic_cast<ScenarioSC*>(participant.data());
	    const QList<QSharedPointer<ScenarioAbstractTrajectoryType> >& trajectoryList =
	    vehicle->SCMission()->TrajectoryPlan()->AbstractTrajectory();

	    foreach (QSharedPointer<ScenarioAbstractTrajectoryType> trajectory,trajectoryList)
	    {
		if (dynamic_cast<ScenarioLoiteringType*>(trajectory.data()))
		{
		    qDebug()<<"loitering";
		    ScenarioLoiteringType* loitering = dynamic_cast<ScenarioLoiteringType*>(trajectory.data());
		    QDateTime StartTimeDate=loitering->TimeLine()->StartTime();
		    QDateTime EndTimeDate=loitering->TimeLine()->EndTime();
		    //QDateTime TimeStep=loitering->TimeLine()->StepTime();

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

		    QTreeWidgetItem*treeTime=new QTreeWidgetItem(treeWidgetTimeSpecifications);
		    treeTime->setText(0,StartTime);

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

		  //  QString EndTime = QString::number(EndTimeValue);
		    //QTreeWidgetItem*treeEndTime=new QTreeWidgetItem(treeWidgetTimeSpecifications);
		    treeTime->setText(1,EndTime);

		}
	    }
	}

    }
}
}


void Analysis::on_AddNewPushButton_clicked()
{
	if (CheckIfMissionArc())
    {
	QTreeWidgetItem*treeItem=new QTreeWidgetItem(treeWidgetTimeSpecifications);
	treeItem->setText(0," ");
	treeItem->setText(1," ");
	//treeItem->setText(2," ");
    }
	else
	{
	}
    }

void Analysis::on_EditTimePushButton_clicked()
{
	qWarning("TODO: %s	%d",__FILE__,__LINE__);
}

void Analysis::on_DeleteTimePushButton_clicked()
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

void Analysis::on_groupBoxPlotAxesSettings_toggled(bool)
{
	qWarning("TODO: %s	%d",__FILE__,__LINE__);
}

void Analysis::on_GeneratePushButton_clicked()
{
	qWarning("TODO: %s	%d",__FILE__,__LINE__);
}

void Analysis::on_TotalHelpPushButton_clicked()
{
	qWarning("TODO: %s	%d",__FILE__,__LINE__);
}

void Analysis::on_ClosePushButton_clicked()
{
	this->close();

}


void Analysis::addParameter()

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


void Analysis::addParameter(QTreeWidgetItem* item)
{
	QString text = item->text(0);
	if(listWidgetShowInReport->findItems(text,Qt::MatchExactly).isEmpty())
		listWidgetShowInReport->addItem(text);
}


void Analysis::removeParameter()
{
    QList<QListWidgetItem *> ChosenParameters = listWidgetShowInReport->selectedItems();
    for (int i = 0; i < ChosenParameters.size(); i++)
    {
	delete ChosenParameters.at(i);
    }
}

void Analysis::enableReportOption(int i)
{

    if (i == 0)
    {
    stackedWidget->setCurrentWidget(pageReport);
    //pagePlotAxesSettings->hide();
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
		//QTreeWidgetItem *___qtreewidgetitem487->ItemIsSelectable();
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
QComboBox* Analysis::CoordinateBox()
{
    QComboBox*CoordinateBox2=new QComboBox();
    //QString EMEJ2000="EME J2000";
    //QString Fixed="Fixed";
    CoordinateBox2->addItem(tr("EME J2000"),0);
    CoordinateBox2->addItem(tr("EME B1950"),1);
    CoordinateBox2->addItem(tr("Fixed"),2);
    CoordinateBox2->addItem(tr("Ecliptic J2000"),3);
    CoordinateBox2->addItem(tr("Rotating"),4);
    CoordinateBox2->addItem(tr("Rotating Normalized"),5);
    return CoordinateBox2;
}
//QComboBox*Analysis::TimeFramesBox()
//{
//    TimeBox->addItem(tr("UTC"));
//    TimeBox->addItem(tr("LCL"));
//    TimeBox->addItem(tr("Gregorian UTC"));
//    TimeBox->addItem(tr("Gragorian LCL"));
//   TimeBox->addItem(tr("YYDDD"));
//    TimeBox->addItem(tr("Julian Date"));
//    TimeBox->addItem(tr("Julian LCL"));
//    TimeBox->addItem(tr("Mission elapsed time"));
//    TimeBox->addItem(tr("GMT"));
//    TimeBox->addItem(tr("MJD"));
//    TimeBox->addItem(tr("Days,hours,minutes,and seconds from epoch"));
//    return TimeBox;
}

//QComboBox*Analysis::TimeUnitsBox()
//{
//
//    QComboBox*TimeBox=new QComboBox();
//    TimeBox->addItem(tr("Seconds"),0);
//    TimeBox->addItem(tr("Minutes"),1);
//    TimeBox->addItem(tr("Hours"),2);
//    return TimeBox;
//}
//
//QComboBox*Analysis::AngleUnitsBox()
//{
//    QComboBox*AngleBox=new QComboBox();
//    AngleBox->addItem(tr("Degrees"),0);
//    AngleBox->addItem(tr("Radians"),1);
}

void Analysis::InsertingComboBox()
{
    //Create ComboBoxes for Units and Coordinate Systems Specification
    treeWidgetReportOptions->setColumnWidth(0,125);
    treeWidgetReportOptions->setColumnWidth(1,100);
    treeWidgetReportOptions->setColumnWidth(2,100);
    QStringList Topics;
    Topics<<("Aerodynamics");
    Topics<<("Analysis");
    Topics<<("Coverage");
    Topics<<("Re-entry");
    Topics<<("Systems Engineering");
for (int i=0;i<Topics.size();i++)
    {

    //QList<QTreeWidgetItem*>topicItems=treeWidgetReportOptions->findItems(Topics[i],Qt::MatchExactly,0);
    QTreeWidgetItem*TopicItem=treeWidgetReportOptions->topLevelItem(i);
    //QTreeWidgetItem* TopicItem=topicItems[0];
    int TopicChildren=TopicItem->childCount();
    //qDebug()<<"test1 "<<TopicChildren<<" "<<TopicItem->text(0);
    for (int j=0;j<TopicChildren;j++)
    {
	QTreeWidgetItem*SecondLevelChild=TopicItem->child(j);
	//SecondLevelChild=TopicItem->child(j);
	int Children=SecondLevelChild->childCount();
	//qDebug()<<"test2 "<<Children<<" "<<SecondLevelChild->text(0);

	if (Children!=0) //third level options
	{
	    for (int k=0;k<Children;k++)
	    {


		//qDebug()<<"test3";
	    QTreeWidgetItem*ThirdLevelChild=SecondLevelChild->child(k);
	    //ThirdLevelChild=SecondLevelChild->child(k);
	    QTreeWidgetItem*item=ThirdLevelChild;
	    //AddComboBox(item);
	    QString name=item->text(0);

		if ((name=="x position") ||(name=="y position")||
		    (name=="z position") ||
		    (name=="x velocity") ||
		    (name=="y velocity") ||
		    (name=="z velocity") ||
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
		    (name=="Altitude")||
		    (name=="Latitude")||
		    (name=="Longitude")||
		    (name=="Radius")||
		    (name=="Apogee Planetocentric Altitude")||
		    (name=="Apogee Planetocentric Radius")||
		    (name=="Perigee Planetocentric Altitude")||
		    (name=="Perigee Planetocentric Radius")||
		    (name=="Longitude when passing Ascending Node")||
		    (name=="Longitude when passing Descending Node")||
		    (name=="Maximum Geodetic Latitude")||
		    (name=="Minimum Geodetic Latitude")||
		    (name=="Azimuth")||
		    (name=="Elevation")||
		    (name=="Range"))
		{
		    treeWidgetReportOptions->setItemWidget(item,1,CoordinateBox());
		}

	     }
	 }
	else  // second level options
	{

	    QTreeWidgetItem*item=SecondLevelChild;
	    QString name=item->text(0);

	    if ((name=="x position") ||(name=="y position")||
		(name=="z position") ||
		(name=="x velocity") ||
		(name=="y velocity") ||
		(name=="z velocity") ||
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
		(name=="Altitude")||
		(name=="Latitude")||
		(name=="Longitude")||
		(name=="Radius")||
		(name=="Apogee Planetocentric Altitude")||
		(name=="Apogee Planetocentric Radius")||
		(name=="Perigee Planetocentric Altitude")||
		(name=="Perigee Planetocentric Radius")||
		(name=="Longitude when passing Ascending Node")||
		(name=="Longitude when passing Descending Node")||
		(name=="Maximum Geodetic Latitude")||
		(name=="Minimum Geodetic Latitude")||
		(name=="Azimuth")||
		(name=="Elevation")||
		(name=="Range"))
	    {
		treeWidgetReportOptions->setItemWidget(item,1,CoordinateBox());
	    }

}

}
}
}

*/
