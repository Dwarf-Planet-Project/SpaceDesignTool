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
 ------------------ Author: Ana Raposo and Guillermo Ortega  -------------------------------
*/


#ifndef analysis_H
#define analysis_H

#include "ui_analysis.h"
#include <QDialog>
#include "Scenario/staschema.h"
#include"Main/propagatedscenario.h"

class analysis : public QDialog , private Ui::analysisDialogClass
{
Q_OBJECT
public:
        analysis(SpaceScenario * scenario, PropagatedScenario*propagatedScenario,QWidget * parent = 0, Qt::WindowFlags f = 0 );
	~analysis();
        void readScenario();
int ObjectsIndex(QStringList AllObjects, int Index, QString ObjectType);
QString CovCommCalc();
int InputsControl(QList<QTreeWidget*>tree);
void Warnings(int i);

struct AnalysisData
{
QString GenerationTime;
QList<double>MissionSat; //mission arc, satellite numbers
QList<QString>ParameterTitles;
QList<QList<double> >Data; // QList of lines, each line a QList of data

};

QList<AnalysisData> WriteDataStructure(QList<QTreeWidgetItem *> selected,QList<QTreeWidgetItem *> selectedTimes);
void WriteReport(QList<QTreeWidgetItem *> selected,QList<QTreeWidgetItem *> selectedTimes);
void DisableUnavailableOptions();

protected slots:
	// Classical action written by Guillermo
	/*
	void on_buttonBox_helpRequested();
	void on_buttonBox_clicked(QAbstractButton*);
	void on_buttonBox_destroyed(QObject*);
	void on_buttonBox_destroyed();
	void on_buttonBox_customContextMenuRequested(const QPoint&);
	void on_buttonBox_accepted();
	void on_buttonBox_rejected();
	*/


	// Actions written by Ana

        //void on_groupBoxAnalysisFormat_toggled(bool);
        //void on_ComboBoxAnalysisFormat_activated(const QString&);
	void on_groupBoxParameters_toggled(bool);
	void on_groupBoxTimeSpecifications_toggled(bool);
	void on_AddDefaultPushButton_clicked();
	//void on_AddNewPushButton_clicked();
        void on_EditTimePushButton_clicked();
	void on_DeleteTimePushButton_clicked();
	//void on_groupBoxReport_toggled(bool);
	//void on_AddParameterPushButton_clicked();
	//void on_RemoveParameterPushButton_clicked();
        //void on_groupBoxPlotAxesSettings_toggled(bool);
	void on_GeneratePushButton_clicked();
        //void on_TotalHelpPushButton_clicked();  // Guillermo says: Why is this required? Can you used my action?
	// void on_ClosePushButton_clicked();   // Guillermo says: not needed any more

    public slots:  // Added by Ana
	void addParameter(QTreeWidgetItem* item); // clicked
	void addParameter(); //clicked
	void removeParameter();//clicked
	void enableReportOption(int i);
        //void SetTimeUnits(int i);
        //void PlotComboBox(QList<QTreeWidgetItem*>List, int i);
       // void InsertingComboBox(int);
       // void AddDefaultTime();

private:
        SpaceScenario* m_scenario;
        PropagatedScenario*m_propagatedScenario;
        //PropagatedScenario*m_propagatedScenario;
        bool CheckIfMissionArc();
        void InsertComboBox();
        void ComboBoxOptions();
        void PlotComboBox();
        QComboBox * TimeFramesBox();
        QComboBox * TimeUnitsBox();
        QComboBox * AngleUnitsBox();
        QComboBox * DistanceUnitsBox();
        QComboBox * CoordinateBox();
        QComboBox * VelocityUnitsBox();
        QComboBox * NoUnitsBox();
        QList<MissionArc*> m_missionArcs;
        QList<SpaceObject*> m_spaceObjectList;
        //double* ReadTime(int column);
        void ReadTime(int column, double *MJD);
        double ReadStopTime();
        QList<ScenarioTransmitterPayloadType*>TxSC;
        QList<ScenarioTransmitterPayloadType*>TxGS;
        QList<ScenarioReceiverPayloadType*>RxSC;
        QList<ScenarioReceiverPayloadType*>RxGS;
        QList<ScenarioGroundStationEnvironment*>GSEnvironment;
        double calcKeplerianElements(sta::StateVector Vector,StaBody*Body,QString OrbElement,
                                                               double mjd,
                                                               QString FromCoordinate,
                                                               QString ToCoordinate);
        sta::CoordinateSystem CoordSys(QString Coordinate);
        double calcDelaunayElements(sta::StateVector Vector,StaBody*Body,QString OrbElement,
                                                                       double mjd,
                                                                       QString FromCoordinate,
                                                                       QString ToCoordinate);
        double calcEquinoctialElements(sta::StateVector Vector,StaBody*Body,QString OrbElement,
                                                 double mjd,
                                                 QString FromCoordinate,
                                                 QString ToCoordinate);




    };
#endif //analysis_H

