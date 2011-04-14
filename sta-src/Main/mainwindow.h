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
 ------------------ Author: Chris Laurel  -------------------------------------------------
 ------------------ E-mail: (claurel@gmail.com) ----------------------------
 Patched extensively by Guillermo to handle about, web sites, TLEs, etc.
 Patched again by Guillermo April 2010 to include Analysis and SEM modules
 Patched again by Guillermo June 2010 to include Constellation module
 Patched again by Guillermo April 2011 to include Interplanetary, Rebdezvous and Lagrangian modules
 */

#ifndef _MAIN_MAINWINDOW_H_
#define _MAIN_MAINWINDOW_H_

#include <QMainWindow>
#include <QTime>
#include "timelinewidget.h"
#include "ui_mainwindow.h"

class QCloseEvent;
class QTimer;
class QTabWidget;
class QStackedWidget;

class OrbitPropagationDialog;
class GroundTrackPlotTool;
class ThreeDVisualizationTool;
class ScenarioView;
class SpaceScenario;
class ScenarioElementBox;
class TimelineWidget;
class PropagatedScenario;
class ConstellationStudy;

class RendezvousDialog;
class ReEntryDialog;
class LoiteringDialog;
class LoiteringTLEDialog;
class ExternalDialog;
class AboutDialog;
class SEMVehicleDialog;
class AnalysisDialog;
class STAcalculator;

class ViewActionGroup;

class QXmlSchema;


class MainWindow : public QMainWindow, private Ui_MainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();

    SpaceScenario* scenario() const;
    void setScenario(SpaceScenario* scenario);

public:
    void readSettings();
    void writeSettings();
    void loadBookmarks();
    void saveBookmarks();

public slots:
    void on_action2D_Plot_triggered();
    void on_actionNewScenario_triggered();
    void on_actionOpenScenario_triggered();
    void on_actionSaveScenario_triggered();
    void on_actionSaveScenarioAs_triggered();
    void on_actionExport_triggered();
    void on_actionOpenScript_triggered();
    void on_actionPropagate_Scenario_triggered();
    void on_actionPropagateCoverage_triggered();
    void on_actionSat_to_Sat_triggered();
    void on_actionSat_to_Ground_triggered();
    void on_actionSystem_Engineering_triggered();
    void on_actionScenarioElements_toggled(bool checked);
    void on_action3dViewPreferences_triggered();
    void on_actionSTA_Web_triggered();
    void on_actionSTA_bugtracker_triggered();
    void on_actionSTA_Wiki_triggered();
    void on_actionSTA_User_s_Manual_triggered();
    void on_actionSTA_Forums_triggered();
    void on_actionSTA_Downloads_triggered();
    void on_actionAnalyse_triggered();
    void on_actionCreateConstellation_triggered(); // Constellation (Claas Grohnfeldt, Steffen Peter)
    // Next lines added by Guillermo
    void on_actionCreateInterplanetaryPlan_triggered();
    void on_actionCreateLagrangianPlan_triggered();
    void on_actionCreateRendezvousPlan_triggered();
    //
    void on_menuEdit_aboutToShow();
    void about();
    void preferencesSTA();
    void ActivateSTACalc();
    void selectVisualization(QAction* action);

    void tick();
    void setTime(double mjd);
    void selectParticipant(int participantIndex);

    void refreshCentralWidget(int tabIndex);
    void toggleGroundTrackView();

    void openFileFromAEvent(const QString& fileName);	//Guillermo

    void setFullScreen3D(bool enable);

    // Next clases added by Guillermo to handle the ABOUT menu
public:
    QMenu   *helpMenu;
    QAction *aboutAct;
    QAction *preferencesAct;
    QAction *STACalculatorAct;

signals:
    void progressUpdate(const QString& s, int align, const QColor& c);

private slots:
    void fileNew();

private:
    void showGroundTrackPlotTool();
    void replaceCurrentScenario(SpaceScenario* scenario, QString filename);
    void configureTimeline(PropagatedScenario* scenario);
    void setPropagatedScenario(PropagatedScenario* scenario);

    void initAppDataDirectory();

    void closeEvent(QCloseEvent* event);

    void clearViews();

    SpaceScenario* m_scenario;
    PropagatedScenario* m_propagatedScenario;

    QString m_scenarioFileName;
    ScenarioView* m_scenarioView;
    TimelineWidget* m_timelineWidget;
    OrbitPropagationDialog* m_orbitPropagationDialog;
    GroundTrackPlotTool* m_groundTrackPlotTool;
    ThreeDVisualizationTool* m_threeDViewWidget;
    STAcalculator* STAcalculatorWidget;
    ScenarioElementBox* m_scenarioElementBox;
    //QTabWidget* m_viewPanel;
    QStackedWidget* m_viewPanel;

    RendezvousDialog* m_rendezvousDialog;
    ReEntryDialog* m_reentryDialog;
    LoiteringDialog* m_loiteringDialog;
    LoiteringTLEDialog* m_loiteringTLEDialog;
    SEMVehicleDialog* m_SEMVehicleDialog;
    AnalysisDialog* m_AnalysisDialog;

    QTimer* m_viewUpdateTimer;
    QTime m_intervalChangeTime;

    QAction* m_dockGroundTrackAction;
    QAction* m_fullScreen3DAction;
    QByteArray m_savedWindowState;

    QXmlSchema* m_spaceScenarioSchema;

    ViewActionGroup* m_viewActions;

    double m_lastTime;
};

#endif // _MAIN_MAINWINDOW_H_
