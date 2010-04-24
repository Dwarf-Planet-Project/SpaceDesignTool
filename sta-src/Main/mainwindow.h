/* 
This program is free software; you can redistribute it and/or modify it under
 the terms of the GNU Lesser General Public License as published by the Free Software
 Foundation; either version 2 of the License, or (at your option) any later
 version.
 
 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more
 details.
 
 You should have received a copy of the GNU Lesser General Public License along with
 this program; if not, write to the Free Software Foundation, Inc., 59 Temple
 Place - Suite 330, Boston, MA 02111-1307, USA.
 Further information about the GNU Lesser General Public License can also be found on
 the world wide web at http://www.gnu.org. 
 */

/*
 ------ Copyright (C) 2008 European Space Agency (space.trajectory.analysis AT gmail.com) ----
 ------------------ Author: Chris Laurel  -------------------------------------------------
 ------------------ E-mail: (claurel@gmail.com) ----------------------------
 Patched extensively by Guillermo to handle about, web sites, TLEs, etc.
 */

#ifndef _MAIN_MAINWINDOW_H_
#define _MAIN_MAINWINDOW_H_

#include <QMainWindow>
#include <QTime>
#include <celestia/celestiacore.h>
#include "timelinewidget.h"
#include "ui_mainwindow.h"

class QCloseEvent;
class QTimer;
class QTabWidget;

class OrbitPropagationDialog;
class GroundTrackPlotTool;
class PlottingTool;
class ScenarioView;
class SpaceScenario;
class ScenarioElementBox;
class TimelineWidget;
class PropagatedScenario;

class RendezvousDialog;
class ReEntryDialog;
class LoiteringDialog;
class LoiteringTLEDialog;
class ExternalDialog;
class AboutDialog;
class SEMVehicleDialog;
class STAcalculator;

class CelestiaGlWidget;
class CelestiaActions;
class CelestiaInterface;

class QXmlSchema;


class MainWindow : public QMainWindow, private Ui_MainWindow
{
    Q_OBJECT
    
public:
    MainWindow();
    ~MainWindow();
    
    SpaceScenario* scenario() const;
    void setScenario(SpaceScenario* scenario);

public:
    void initCelestia(const QString& configFileName,
                      const QStringList& extrasDirectories);
    void initCelestiaState();
    
    void readSettings();
    void writeSettings();
    void loadBookmarks();
    void saveBookmarks();
    
    // Callback methods for Celestia
    void contextMenu(float x, float y, Selection sel);
    void loadingProgressUpdate(const QString& s);
    
public slots:
    void on_action2D_Plot_triggered();
    void on_actionNewScenario_triggered();
    void on_actionOpenScenario_triggered();
    void on_actionSaveScenario_triggered();
    void on_actionSaveScenarioAs_triggered();
    void on_actionExport_triggered();
    void on_actionOpenScript_triggered();
    void on_actionPropagate_Scenario_triggered();
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
    void on_menuEdit_aboutToShow();
    void about();
    void preferencesSTA();
    void ActivateSTACalc();
    
    void celestia_tick();
    void setTime(double jd);
    void setTimeRate(TimelineWidget::TimeRateMode mode, double rate);
    void pauseTime();
    void selectParticipant(int participantIndex);
    
    void refreshCentralWidget(int tabIndex);
    void toggleGroundTrackView();
	
// Next clases added by Guillermo to handle the ABOUT menu
public:
    QMenu   *helpMenu;
    QAction *aboutAct;
    QAction *preferencesAct;
    QAction *STACalculatorAct;
    
signals:
    void progressUpdate(const QString& s, int align, const QColor& c);

private:
    void showGroundTrackPlotTool();
    void replaceCurrentScenario(SpaceScenario* scenario, QString filename);
    void configureTimeline(PropagatedScenario* scenario);
 
    void initAppDataDirectory();
    
    void closeEvent(QCloseEvent* event);
    
    void clearViews();

private:
    SpaceScenario* m_scenario;
    PropagatedScenario* m_propagatedScenario;
    
    QString m_scenarioFileName;
    ScenarioView* m_scenarioView;
    TimelineWidget* m_timelineWidget;
    OrbitPropagationDialog* m_orbitPropagationDialog;
    GroundTrackPlotTool* m_groundTrackPlotTool;
    PlottingTool* m_plottingTool;
    STAcalculator* STAcalculatorWidget;
    ScenarioElementBox* m_scenarioElementBox;
    QTabWidget* m_viewPanel;
    
    RendezvousDialog* m_rendezvousDialog;
    ReEntryDialog* m_reentryDialog;
    LoiteringDialog* m_loiteringDialog;
    LoiteringTLEDialog* m_loiteringTLEDialog;
    SEMVehicleDialog* m_SEMVehicleDialog;

    // Celestia data
    CelestiaGlWidget* m_celestiaViewWidget;
    CelestiaCore* m_celestiaCore;
    CelestiaInterface* m_celestiaInterface;
    CelestiaActions* m_celestiaActions;
    CelestiaCore::Alerter* m_celestiaAlerter;
    QString m_celestiaDataDirPath;
    
    QTimer* m_celestiaUpdateTimer;
    QTime m_intervalChangeTime;
    
    QAction* m_dockGroundTrackAction;

    QXmlSchema* m_spaceScenarioSchema;
};

#endif // _MAIN_MAINWINDOW_H_
