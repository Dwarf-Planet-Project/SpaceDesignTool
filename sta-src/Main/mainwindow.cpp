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

 ------------------ Author: Guillermo Ortega  -------------------------------------------------
 ------------------ Author: Chris Laurel  -------------------------------------------------
 ------------------ E-mail: (claurel@gmail.com) ----------------------------
  Modified by Guillermo on October 2009 to allow shorcuts on dialogues and change the "calculate" dialogue
  Patched by Guillermo April 2010 to add analyis module
  Patched by Guillermo June 2010 to add constellations module
  Patched by Guillermo on June 2010 to all TLE propagation


 */


#include <QtGui>
#include <QFile>
#include <QColor>
#include <QDir>
#include <QTextStream>
#include <QDesktopServices> // Added by Guillermo for web and mail services
#include <QUrl> // Added by Guillermo to work with URLs
#include <QXmlSchema>
#include <QXmlSchemaValidator>

#include "mainwindow.h"
#include "Main/preferences.h"
#include "scenarioelementbox.h"
#include "scenarioview.h"
#include "scenariotree.h"
#include "timelinewidget.h"
#include "timelineview.h"
#include "ViewActionGroup.h"
#include "Scenario/scenario.h"
#include "Scenario/propagationfeedback.h"
#include "Plotting/ThreeDView.h"
#include "Plotting/groundtrackplottool.h"
#include "Plotting/threedvisualizationtool.h"
#include "Astro-Core/date.h"

#include "Scenario/scenarioPropagator.h"

//****************** /OZGUN
#include "Astro-Core/EclipseDuration.h"
#include <QDebug>
//***************** OZGUN/

#include "propagatedscenario.h"
//#include "RendezVous/rendezvous.h"
#include "Entry/reentry.h"
#include "Loitering/loitering.h"
#include "Loitering/loiteringTLE.h"
#include "SEM/sem.h"
#include "Analysis/analysis.h"
#include "exportdialog.h"

#include "Calculator/STAcalculator.h"

#include "Help/HelpBrowser.h"

#include "about.h"

// Constellations
#include "Constellations/canalysis.h"
#include "Constellations/constellationwizard.h"

#include "ui_mainwindow.h"

#include "Scenario/missionsDefaults.h"

#ifdef TARGET_OS_MAC
#include <Carbon/Carbon.h>
#endif

#include <time.h>


using namespace std;

// Defining the URLs that STA will use for browsing and mailing agents
QUrl STAwebSite("http://sta.estec.esa.int");
QUrl STABugTracker("http://forge.osor.eu/tracker/?group_id=134");

QUrl STAForums("http://forge.osor.eu/forum/?group_id=134");
QUrl STADownloads("http://forge.osor.eu/frs/?group_id=134");
QUrl STAmail("mailto:space.trajectory.analysis@gmail.com?subject=Help&body=Need help with STA...");

QString DEFAULT_CONFIG_FILE = "STA.cfg";
QString BOOKMARKS_FILE = "bookmarks.xbel";
QString SCHEMA_FILE = "schema/spacescenario/2.0/scenario.xsd";

// Line patched by Guillemro as to make the default  window a bit smaller and right to the border
const QSize DEFAULT_MAIN_WINDOW_SIZE(900,600);  // Nice size for Windows, etc
const QPoint DEFAULT_MAIN_WINDOW_POSITION(30,30);

// Used when saving and restoring main window state; increment whenever
// new dockables or toolbars are added.
static const int STA_MAIN_WINDOW_VERSION = 15;

// Number of spaces to indent when writing scenario XML files
static const int SCENARIO_FILE_INDENT_LEVEL = 2;



//MainWindow::MainWindow() :
MainWindow::MainWindow(QWidget *parent)	:
		m_scenario(NULL),
		m_propagatedScenario(NULL),
		m_scenarioView(NULL),
		m_timelineWidget(NULL),
		m_orbitPropagationDialog(NULL),
		m_groundTrackPlotTool(NULL),
		m_scenarioElementBox(NULL),
		m_viewPanel(NULL),

		m_rendezvousDialog(NULL),
		m_reentryDialog(NULL),
		m_loiteringDialog(NULL),
		m_loiteringTLEDialog(NULL),
		m_SEMVehicleDialog(NULL),
		m_AnalysisDialog(NULL),

		m_threeDViewWidget(NULL),

		m_spaceScenarioSchema(NULL),

		m_viewActions(NULL),

		m_lastTime(0.0)
{
    setupUi(this);
    connect(actionQuit, SIGNAL(triggered()), QApplication::instance(), SLOT(closeAllWindows()));

    m_groundTrackPlotTool = new GroundTrackPlotTool(this);
    m_groundTrackPlotTool->setWindowTitle(tr("Ground Track"));

    // Set the dock options. We want the scenario tree view to occupy the entire left side of
    // the main window when docked (i.e. it has precedence over the timeline view), so we'll
    // set the ownership of the lower left corner.
    // Patched by Guillermo: The next line should be the "first" refreshment. Otherwise
    // STA shows a estrange small window 0.5 seconds before the splash screen on Linux and Windows
    setCorner(Qt::TopLeftCorner, Qt::LeftDockWidgetArea);
    setCorner(Qt::BottomLeftCorner, Qt::LeftDockWidgetArea);


    // Set up the dock widgets
    // Scenario view dock item
    QDockWidget* scenarioViewDock = new QDockWidget(tr("Scenario View"), this);
    scenarioViewDock->setObjectName("sta-scenario-view");
    scenarioViewDock->setAllowedAreas(Qt::LeftDockWidgetArea);

    m_scenarioView = new ScenarioView(this);
    scenarioViewDock->setWidget(m_scenarioView);
    addDockWidget(Qt::LeftDockWidgetArea, scenarioViewDock);
    scenarioViewDock->setVisible(true);

    // Scenario element box dock item
    QDockWidget* scenarioElementDock = new QDockWidget(tr("Scenario Element Box"), this);
    scenarioElementDock->setObjectName("sta-scenario-element-box");
    scenarioElementDock->setAllowedAreas(Qt::LeftDockWidgetArea);
    scenarioElementDock->setFloating(false);
    //scenarioElementDock->setAutoFillBackground(true);

    m_scenarioElementBox = new ScenarioElementBox(this);
    scenarioElementDock->setWidget(m_scenarioElementBox);
    addDockWidget(Qt::LeftDockWidgetArea, scenarioElementDock);
    scenarioElementDock->setVisible(true);

    // Timeline widget dock item
    QDockWidget* timelineDock = new QDockWidget(tr("Timeline View"), this);
    timelineDock->setFeatures(timelineDock->features() | QDockWidget::DockWidgetVerticalTitleBar);
    timelineDock->setObjectName("sta-timeline");
    timelineDock->setAllowedAreas(Qt::BottomDockWidgetArea);
    timelineDock->setFloating(false);

    m_timelineWidget = new TimelineWidget(this);
    timelineDock->setWidget(m_timelineWidget);
    addDockWidget(Qt::BottomDockWidgetArea, timelineDock);
    timelineDock->setVisible(true);

    connect(m_timelineWidget, SIGNAL(currentTimeChanged(double)), this, SLOT(setTime(double)));
    connect(m_timelineWidget->timelineView(), SIGNAL(participantClicked(int)),
            this, SLOT(selectParticipant(int)));

    QStringList extrasDirs;

    // Set up the central widget
    m_viewPanel = new QTabWidget(this);
    m_viewPanel->addTab(m_groundTrackPlotTool, tr("Ground Track View"));

    m_threeDViewWidget = new ThreeDVisualizationTool(this);
    m_viewPanel->addTab(m_threeDViewWidget, tr("3D View"));

    setCentralWidget(m_viewPanel);

    // Fixes ground track update problems on Mac OS X; harmless on other platforms.
    connect(m_viewPanel, SIGNAL(currentChanged(int)), this, SLOT(refreshCentralWidget(int)));

    // Add tool window toggle actions to the tools menu
    menuWindow->addAction(scenarioViewDock->toggleViewAction());
    menuWindow->addAction(scenarioElementDock->toggleViewAction());
    menuWindow->addAction(timelineDock->toggleViewAction());
    menuWindow->addSeparator();
    m_dockGroundTrackAction = new QAction(tr("Undock Ground Track View"), menuWindow);
    menuWindow->addAction(m_dockGroundTrackAction);
    connect(m_dockGroundTrackAction, SIGNAL(triggered()), this, SLOT(toggleGroundTrackView()));

    // Initial state has no scenario loaded, so disable the propagate, Engineer, and analise
    // scenario actions. Patched by Guillermo to make it work correctly
    actionPropagate_Scenario->setEnabled(m_scenario != NULL);
    actionPropagateCoverage->setEnabled(m_scenario != NULL);
    actionSat_to_Sat->setEnabled(m_scenario != NULL);
    actionSat_to_Ground->setEnabled(m_scenario != NULL);
    actionSystem_Engineering->setEnabled(m_scenario != NULL);
    actionAnalyse->setEnabled(m_scenario != NULL);

    // Next lines patched by Guillermo to handle the "ABOUT" menu
    aboutAct = new QAction(tr("&About"), this);
    aboutAct->setStatusTip(tr("Shows the About box"));
    connect(aboutAct, SIGNAL(triggered()), this, SLOT(about()));
    helpMenu = menuBar()->addMenu(tr("&About"));
    menuBar()->addSeparator();
    helpMenu->addAction(aboutAct);

    // Next lines patched by Guillermo to handle the "PREFERENCES" menu
    preferencesAct = new QAction(tr("&Preferences"), this);
    preferencesAct->setStatusTip(tr("Shows the Preferences menu"));
    connect(preferencesAct, SIGNAL(triggered()), this, SLOT(preferencesSTA()));
    menuBar()->addSeparator();
    helpMenu->addAction(preferencesAct);

    connect(actionCut, SIGNAL(triggered()), m_scenarioView->m_scenarioTree, SLOT(removeSelection()));

    setUnifiedTitleAndToolBarOnMac(true);
    //setUnifiedTitleAndToolBarOnMac(false);
    //setAttribute(Qt::WA_DeleteOnClose);

    // Next line patched by Guillermo with the 'Cambrian' logo
    setWindowIcon(QIcon(":/icons/STAlogo.png"));
    setWindowTitle("Space Trajectory Analysis");

    // Fixes ground track update problems on Mac OS X; harmless on other platforms.
    connect(m_viewPanel, SIGNAL(currentChanged(int)), this, SLOT(refreshCentralWidget(int)));

    // Next lines patched by Guillermo to handle the "Calculator" menu
    STACalculatorAct = new QAction(tr("&Calculator"), this);
    menuTools->addAction(STACalculatorAct);
    STACalculatorAct->setStatusTip(tr("Activates the STA Calculator"));
    STACalculatorAct->setShortcut(QKeySequence(tr("Ctrl+T", "Tools|Calculator")));
    connect(STACalculatorAct, SIGNAL(triggered()), this, SLOT(ActivateSTACalc()));
    //menuBar()->addMenu(tr("&Tools"));  // Not required sice it is done on QtDesigner already
    //menuTools()->addSeparator();

    // Initialize the time to the current system time
    double now = sta::JdToMjd(sta::CalendarToJd(QDateTime::currentDateTime()));
    setTime(now);
    m_timelineWidget->timelineView()->setTimeRange(now - 1 / 24.0, now + 1.0);
    m_timelineWidget->setZoom("all");

    m_viewActions = new ViewActionGroup();

    if (m_threeDViewWidget)
    {
        ThreeDView* view = m_threeDViewWidget->view();

        // Point the camera in the 3D view at Earth
        view->gotoBody(STA_SOLAR_SYSTEM->earth());

        // Connect actions
        connect(m_viewActions->starsAction(), SIGNAL(toggled(bool)), view, SLOT(setStars(bool)));
        connect(m_viewActions->equatorialGridAction(), SIGNAL(toggled(bool)), view, SLOT(setEquatorialGrid(bool)));
        connect(m_viewActions->cloudsAction(), SIGNAL(toggled(bool)), view, SLOT(setClouds(bool)));
        connect(m_viewActions->shadowsAction(), SIGNAL(toggled(bool)), view, SLOT(setShadows(bool)));
        connect(m_viewActions->atmospheresAction(), SIGNAL(toggled(bool)), view, SLOT(setAtmospheres(bool)));
        connect(m_viewActions->satelliteTrajectoriesAction(), SIGNAL(toggled(bool)), view, SLOT(setSatelliteTrajectories(bool)));
        connect(m_viewActions, SIGNAL(ambientLightChanged(float)), view, SLOT(setAmbientLight(float)));
    }

    // Read saved window preferences
    readSettings();

    // We use a timer with a null timeout value
    m_viewUpdateTimer = new QTimer(this);
    QObject::connect(m_viewUpdateTimer, SIGNAL(timeout()), SLOT(tick()));
    m_viewUpdateTimer->start(5);
}


MainWindow::~MainWindow()
{
    delete m_propagatedScenario;
    delete m_viewActions;
}


/*! Return the currently open scenario. This function will return NULL if
 *  no scenario is loaded.
 */
SpaceScenario* MainWindow::scenario() const
{
    return m_scenario;
}


/*! Make the specified scenario the current scenario and set up all active
 *  views. If a scenario was loaded already, it will be replaced.
 */
void
		MainWindow::setScenario(SpaceScenario* scenario)
{
    clearViews();

    QTreeWidgetItem* rootItem = new QTreeWidgetItem(m_scenarioView->m_scenarioTree);
    rootItem->setExpanded(true);
    // Guillermo says; put a nicer name
    //rootItem->setText(0, scenario->elementName());
    rootItem->setText(0, "Space scenario");
    rootItem->setText(1, scenario->Name());
    rootItem->setFlags(rootItem->flags() & ~Qt::ItemIsDragEnabled);

    m_scenarioView->m_scenarioTree->addScenarioItems(rootItem, scenario);
    m_scenarioView->m_scenarioTree->addTopLevelItem(rootItem);
    replaceCurrentScenario(scenario, "");

    m_scenarioView->m_scenarioTree->update();
    m_scenarioView->m_scenarioTree->setFocus();
}


void MainWindow::on_action2D_Plot_triggered()
{
    showGroundTrackPlotTool();
}


void MainWindow::on_actionNewScenario_triggered()
{
    SpaceScenario* scenario = new SpaceScenario();
    scenario->setName("New scenario");

    setScenario(scenario);

    m_scenarioView->update();
    m_scenarioView->setFocus();

    // Activating now the pull down menus
    actionPropagate_Scenario->setEnabled(m_scenario != NULL);
    actionPropagateCoverage->setEnabled(m_scenario != NULL);
    actionSat_to_Sat->setEnabled(m_scenario != NULL);
    actionSat_to_Ground->setEnabled(m_scenario != NULL);
    actionSystem_Engineering->setEnabled(m_scenario != NULL);
    //actionAnalyse->setEnabled(m_scenario != NULL);
    actionAnalyse->setDisabled(m_scenario != NULL);  // Do not enable analysis here. Do it after propagation

}


//////////////////////////////////////////////////  Action to open an existing scenario ///////////////////////////////////
void MainWindow::on_actionOpenScenario_triggered()
{
    // Start the open file dialog in the same directory as last time
    QString dir;
    QSettings settings;
    settings.beginGroup("Preferences");
    if (settings.contains("OpenScenarioDir"))
    {
        dir = settings.value("OpenScenarioDir").toString();
    }

    QString fileName = QFileDialog::getOpenFileName(this,
                                                    tr("Open Scenario"),
                                                    dir,
                                                    "Space Scenario Files (*.stas)");
    if (!fileName.isEmpty())
    {
        QFile scenarioFile(fileName);
        if (!scenarioFile.open(QIODevice::ReadOnly))
        {
            QMessageBox::critical(this, tr("Error"), tr("Error opening file %1").arg(fileName));
        }
        else
        {
            // Save the scenario file directory
            QFileInfo scenarioFileInfo(fileName);
            settings.setValue("OpenScenarioDir", scenarioFileInfo.absolutePath());

            if (!m_spaceScenarioSchema)
            {
                QFile schemaFile(SCHEMA_FILE);
                if (!schemaFile.open(QIODevice::ReadOnly))
                {
                    QMessageBox::critical(this, tr("Critical Error"), tr("Error opening space scenario schema file. Unable to load scenario."));
                    return;
                }

                m_spaceScenarioSchema = new QXmlSchema;
                if (!m_spaceScenarioSchema->load(&schemaFile, QUrl::fromLocalFile(schemaFile.fileName())))
                {
                    QMessageBox::critical(this, tr("Critical Error"), tr("Error in space scenario schema file. Unable to load scenario."));
                    delete m_spaceScenarioSchema;
                    m_spaceScenarioSchema = NULL;
                    return;
                }
            }

            QXmlSchemaValidator validator(*m_spaceScenarioSchema);
            if (!validator.validate(&scenarioFile))
            {

                QMessageBox::critical(this, tr("Scenario Load Error"), tr("Scenario is not a valid space scenario."));
                return;
            }

            scenarioFile.reset();
            QDomDocument scenarioDoc;
            if (!scenarioDoc.setContent(&scenarioFile))
            {
                // This should not fail, since we just got done validating the xml file against the space
                // scenario schema.
                scenarioFile.close();
                QMessageBox::critical(this, tr("Scenario Load Error"), tr("Internal error occurred when loading space scenario."));
                return;
            }

            scenarioFile.close();

            QDomElement rootElement = scenarioDoc.firstChildElement("tns:SpaceScenario");
            SpaceScenario* scenario = SpaceScenario::create(rootElement);
            if (!scenario)
            {
                QMessageBox::critical(this, tr("Scenario Load Error"), tr("Internal error (parser problem) occurred when loading space scenario."));
                return;
            }

            // TODO: Probably should just call setScenario() here.
            clearViews();

            // Prohibit drops to the top level item
            QTreeWidgetItem* invisibleRoot = m_scenarioView->m_scenarioTree->invisibleRootItem();
            invisibleRoot->setFlags(invisibleRoot->flags() & ~Qt::ItemIsDropEnabled);

            QTreeWidgetItem* rootItem = new QTreeWidgetItem(m_scenarioView->m_scenarioTree);
            rootItem->setExpanded(true);
			rootItem->setText(0, "Space scenario");
            rootItem->setText(1, scenario->Name());
            rootItem->setFlags(rootItem->flags() & ~Qt::ItemIsDragEnabled);

            m_scenarioView->m_scenarioTree->addScenarioItems(rootItem, scenario);
            m_scenarioView->m_scenarioTree->addTopLevelItem(rootItem);
            replaceCurrentScenario(scenario, fileName);

            // This sequence seems to be required to force the scenario view
            // widget to update (at least on Mac OS X)
            m_scenarioView->m_scenarioTree->update();
            m_scenarioView->setFocus();
            m_scenarioView->m_scenarioTree->setFocus();
        }
    }

    settings.endGroup();

    // Activating now the pull down menus
    actionPropagate_Scenario->setEnabled(m_scenario != NULL);
    actionPropagateCoverage->setEnabled(m_scenario != NULL);
    actionSat_to_Sat->setEnabled(m_scenario != NULL);
    actionSat_to_Ground->setEnabled(m_scenario != NULL);
    actionSystem_Engineering->setEnabled(m_scenario != NULL);
    //actionAnalyse->setEnabled(m_scenario = NULL);
    actionAnalyse->setDisabled(m_scenario != NULL);  // Do not enable analysis here. Do it after propagation

}

// Add required namespace attributes for the
static void addScenarioNamespaceAttributes(QDomElement& rootElement)
{

}


void MainWindow::on_actionSaveScenario_triggered()
{
    // If the scenario wasn't saved or loaded from a file yet, treat "Save" as "Save As"
    if (m_scenarioFileName.isEmpty())
    {
        on_actionSaveScenarioAs_triggered();
    }
    else
    {
        QFile scenarioFile(m_scenarioFileName);
        if (!scenarioFile.open(QIODevice::WriteOnly))
        {
            QMessageBox::critical(this, tr("Error"), tr("Error opening file %1").arg(m_scenarioFileName));
        }
        else
        {
            QDomDocument newDoc;
            QDomElement scenarioElement = CreateSpaceScenarioElement(m_scenario, newDoc);
            AddScenarioNamespaceAttributes(scenarioElement);
            newDoc.appendChild(scenarioElement);
            QTextStream out(&scenarioFile);
            newDoc.save(out, SCENARIO_FILE_INDENT_LEVEL);
            scenarioFile.close();
        }
    }
}


void MainWindow::on_actionSaveScenarioAs_triggered()
{
    QString fileName = QFileDialog::getSaveFileName(this,
                                                    tr("Save Scenario"),
                                                    "",
                                                    "Space Scenario Files (*.stas)");

    if (!fileName.isEmpty())
    {
        QFile scenarioFile(fileName);
        if (!scenarioFile.open(QIODevice::WriteOnly))
        {
            QMessageBox::critical(this, tr("Error"), tr("Error opening file %1").arg(fileName));
        }
        else
        {
            // WRITE
            // If there's no current scenario file name, set it to the value
            // just chosen by the user.
            if (m_scenarioFileName.isEmpty())
            {
                replaceCurrentScenario(scenario(), fileName);
            }

            QDomDocument saveDoc;
            QDomElement scenarioElement = CreateSpaceScenarioElement(m_scenario, saveDoc);
            AddScenarioNamespaceAttributes(scenarioElement);
            saveDoc.appendChild(scenarioElement);

            QTextStream stream(&scenarioFile);
            saveDoc.save(stream, 1);
        }
    }
}


void MainWindow::on_actionExport_triggered()
{
    if (!m_propagatedScenario)
    {
        // Can't export before we've propagated. The export menu item should be disabled
        // unless the scenario has been propagated.
        QMessageBox::warning(this, tr("Export Error"), "You must propagate the scenario before you can export a trajectory");
        return;
    }

    ExportDialog exportDialog(m_propagatedScenario, this);
    if (exportDialog.exec() == QDialog::Accepted)
    {
        QString fileName = QFileDialog::getSaveFileName(this, tr("Export File"),
                                                        "",
                                                        tr("CSV Files (*.csv)"));
        if (!fileName.isEmpty())
        {
            QFile outFile(fileName);
            if (outFile.open(QIODevice::WriteOnly))
            {
                exportDialog.exportTrajectory(&outFile);
            }
        }
    }
}


void MainWindow::on_actionOpenScript_triggered()
{
}


// Next lines added by Guillermo to handle About and Preferences dialogues
void MainWindow::about()
{
    /*
    QMessageBox::about(this, tr("About Application"),
					   tr("STA 'Archean' v2.0, November 2009\n"
                                               "  \n"
                                               "One of the STA aims is to promote the\n"
                                               "exchange of technical ideas, and raise\n"
                                               "knowledge and competence in the areas\n"
                                               "of applied mathematics, space\n"
                                               "engineering, and informatics.\n"
                                               "  \n"
                                               "STA is Open Source.\n"
                                               "  \n"
                                               "Copyright of the \n"
                                               "STA Steering Board 2005-2009."));

*/
    AboutDialog AboutWidget(this, 0);
    AboutWidget.exec();
}


void MainWindow::ActivateSTACalc()
{

    STAcalculator* STAcalculatorWidget = new STAcalculator(this, Qt::Tool);  // Creating the widget as a tool
    STAcalculatorWidget->show(); // Creating the window modeless. This requires the declaration of the variable on the *.h file
    STAcalculatorWidget->raise(); // Required to keep the modeless window alive
    STAcalculatorWidget->activateWindow(); // Required to keep the modeless window alive

}

void MainWindow::preferencesSTA()
{
    Preferences* staPreferencesWidget = new Preferences(this, Qt::Window);  // Creating the widget as a Window
    staPreferencesWidget->connectViewActions(m_viewActions);
    //staPreferencesWidget->exec();  // Modal window
    staPreferencesWidget->show(); // Creating the window modeless. This requires the declaration of the variable on the *.h file
    staPreferencesWidget->raise(); // Required to keep the modeless window alive
    staPreferencesWidget->activateWindow(); // Required to keep the modeless window alive
    staPreferencesWidget->setAmbientLight(m_viewActions->ambientLight());
}

// Next lines created by Guillermo to handle the spawning of web browsers and e-mail clients, etc.

void MainWindow::on_actionSTA_Web_triggered()
		//Opens the given url in the appropriate web browser for the user's desktop environment,
		// and returns true if successful; otherwise returns false.
{
    QDesktopServices::openUrl(STAwebSite);
}

void MainWindow::on_actionSTA_bugtracker_triggered()
{
    QDesktopServices::openUrl(STABugTracker);
}



void MainWindow::on_actionSTA_Wiki_triggered()
{
    //QDesktopServices::openUrl(STAWiki);  // Not any longer a web site but a PDF inside the hundle
    /*
    QMessageBox::about(this, tr("User's Manual"),
			     tr("\n"
				"The STA User's maual is a PDF file\n"
				 "STA-ARCHEAN-UM.pdf\n"
				 "installed in your\n"
				 "sta-data resources folder"));
    */

    //HelpBrowser::showPage("index.html");


    QString ResourcesPath = QDir::currentPath ();
    QString HelpBrowserPath = ResourcesPath + "/help";
    QString page = "index.html";

    HelpBrowser *browser = new HelpBrowser(HelpBrowserPath, page, this, Qt::Tool);
    browser->resize(800, 600);
    browser->setWindowModality(Qt::NonModal);
    browser->show();
    browser->raise(); // Required to keep the modeless window alive
    browser->activateWindow(); // Required to keep the modeless window alive 

}


void MainWindow::on_actionSTA_User_s_Manual_triggered()
{
    //QDesktopServices::openUrl(STAUserManual);   // Not any longer a web site but a PDF inside the hundle
    /*
    QMessageBox::about(this, tr("User's Manual"),
			     tr("STA 'Archean' v2.0, November 2009\n"
				"The STA User's maual is a PDF file\n"
				 "STA-ARCHEAN-UM.pdf installed in your\n"
				 "sta-data resources folder"));
     */

}


void MainWindow::on_actionSTA_Downloads_triggered()
{
    QDesktopServices::openUrl(STADownloads);
}

void MainWindow::on_actionSTA_Forums_triggered()
{
    QDesktopServices::openUrl(STAForums);
}




void MainWindow::on_action3dViewPreferences_triggered()
{
    // Set VESTA preferences
}


// TODO:
// Lagrangian trajectory propagation split out from main propagation
// loop during conversion to new scenario object model. Fix this up
// once we've got Lagrangian orbit support in the schema again.
static void
		PropagateLagrangian()
{
#if OLDSCENARIO
    // Guillermo: Why in the name of the Lord is this code HERE?
    if (Lagrmode==2)
    {
        QFile file_manifolds_settings ("3BMmanifolds_settings.stam");
        if (!file_manifolds_settings.open(QIODevice::ReadOnly | QIODevice::Text))
        {
            QMessageBox::warning(NULL,
								 QObject::tr("Manifolds not plotted"),
								 QObject::tr("Manifolds settings data file %1 not found.").arg("3BMmanifolds_settings.stam"));
            delete propScenario;
            return;
        }
        QTextStream settings(&file_manifolds_settings);
        int numberPositions, numberTrajectories, numberPositions2;

        QList<double> singlemanifold_sampleTimes;
        QList<sta::StateVector> singlemanifold_samples;
        int  totalTraj=0;

        while (sampleTimes.isEmpty()!=true)
        {
            int memory=-1; int kind=-1;
            settings>>kind;
            while (kind!=0)
            {
                if (kind==1) //information about the manifolds required
                {
                    memory=kind;
                    settings>>numberTrajectories>>numberPositions>>numberPositions2;  //the number of trajectories for each manifold and the number of positions for each trajectory are read
                    if (totalTraj==0)
                        totalTraj=numberTrajectories;
                    kind=numberPositions2;
                    if (numberPositions2>4)
                    {
                        numberTrajectories=1;
                    }
                }
                else if (kind==2)
                {
                    memory=kind;
                    settings>>numberTrajectories>>numberPositions>>numberPositions2;
                    if (totalTraj==0)
                        totalTraj=numberTrajectories;
                    kind=numberPositions2;
                    if (numberPositions2>4)//the number of trajectories for each manifold and the number of positions for each trajectory are read
                    {
                        numberTrajectories=1;
                    }
                }
                else if (kind==3)
                {
                    memory=kind;
                    settings>>numberTrajectories>>numberPositions>>numberPositions2;
                    if (totalTraj==0)
                        totalTraj=numberTrajectories;
                    kind=numberPositions2;
                    if (numberPositions2>4)//the number of trajectories for each manifold and the number of positions for each trajectory are read
                    {
                        numberTrajectories=1;
                    }
                }
                else if (kind==4)
                {
                    memory=kind;
                    settings>>numberTrajectories>>numberPositions>>numberPositions2;
                    if (totalTraj==0)
                        totalTraj=numberTrajectories;
                    kind=numberPositions2;
                    if (numberPositions2>4)//the number of trajectories for each manifold and the number of positions for each trajectory are read
                    {
                        numberTrajectories=1;
                    }
                }
                else if (kind>4)
                {
                    //the intersection with a main body is the stopping condition
                    settings>>numberPositions2;
                    numberPositions=kind;
                    kind=numberPositions2;
                    if (numberPositions2>4)//the number of trajectories for each manifold and the number of positions for each trajectory are read
                    {
                        numberTrajectories=1;
                    }
                }
                else if (kind==0)
                {
                    break;
                }
                else
                {
                    QMessageBox::warning(NULL,
                                         QObject::tr("Manifolds not plotted"),
                                         QObject::tr("Wrong manifolds settings data file %1.").arg("3BMmanifolds_settings.stam"));
                    delete propScenario;
                    return;
                }

                for (int k=1;k<numberTrajectories+1; k++)
                {
                    if (totalTraj>0) //condition to plot also the halo orbit along with its manifolds
                    {
                        SpaceObject* spaceObject = new SpaceObject();
                        spaceObject->setName("Halo");
                        spaceObject->setModelFile(vehicle->appearance()->model());
                        spaceObject->setTrajectoryColor(Qt::blue);
                        for (int i=1;i<totalTraj+1;i++)
                        {
                            singlemanifold_sampleTimes.append(sampleTimes.takeFirst());
                            singlemanifold_samples.append(samples.takeFirst());
                        }
                        MissionArc* arc = new MissionArc(trajectory->centralBody(),
														 trajectory->coordinateSystem(),
														 singlemanifold_sampleTimes,
														 singlemanifold_samples);
                        spaceObject->addMissionArc(arc);
                        singlemanifold_sampleTimes.clear();
                        singlemanifold_samples.clear();
                        propScenario->addSpaceObject(spaceObject);
                        totalTraj=-1;
                    }

                    SpaceObject* spaceObject = new SpaceObject();
                    spaceObject->setName("");
                    spaceObject->setModelFile(vehicle->appearance()->model());

                    if (memory==1 || memory==3)
                        spaceObject->setTrajectoryColor(Qt::green);
                    else if(memory==2 || memory==4)
                        spaceObject->setTrajectoryColor(Qt::red);

                    for (int i=1;i<numberPositions+1;i++)
                    {
                        singlemanifold_sampleTimes.append(sampleTimes.takeFirst());
                        singlemanifold_samples.append(samples.takeFirst());
                    }

                    MissionArc* arc = new MissionArc(trajectory->centralBody(),
													 trajectory->coordinateSystem(),
													 singlemanifold_sampleTimes,
													 singlemanifold_samples);
                    spaceObject->addMissionArc(arc);
                    singlemanifold_sampleTimes.clear();
                    singlemanifold_samples.clear();
                    propScenario->addSpaceObject(spaceObject);
                }
            }
        }
        file_manifolds_settings.close();
    }
    if (Lagrmode==3||Lagrmode==4)
    {
        luna=1;
		//condition to plot also the halo orbit along with the transfer orbit
        QList<double> trajectoryLegTime;
        QList<sta::StateVector> trajectoryLegSamples;
		SpaceObject* spaceObject = new SpaceObject();
		spaceObject->setName("Halo");
		spaceObject->setModelFile(vehicle->appearance()->model());
		spaceObject->setTrajectoryColor(Qt::blue);
		for (int i=1;i<1001;i++)
		{
			trajectoryLegTime.append(sampleTimes.takeFirst());
			trajectoryLegSamples.append(samples.takeFirst());
		}
		MissionArc* arc = new MissionArc(trajectory->centralBody(),
										 trajectory->coordinateSystem(),
										 trajectoryLegTime,
										 trajectoryLegSamples);

		spaceObject->addMissionArc(arc);
		propScenario->addSpaceObject(spaceObject);
    }
#endif // OLDSCENARIO
}


int luna;


///////////////////////////////////////////  Action PROPAGATE ////////////////////////////////////////
void MainWindow::on_actionPropagate_Scenario_triggered()
{
    if (!scenario())
    {
        return;
    }

    PropagationFeedback feedback;
    PropagatedScenario* propScenario = new PropagatedScenario();

    //extern int Lagrmode;

    int Lagrmode = 99;	    //Guillermo says: take this thong out of here

    int colorIndex = 0;

    // Process each participant
    foreach (QSharedPointer<ScenarioParticipantType> participant, scenario()->AbstractParticipant())
    {
        MissionsDefaults thisMissionDefaults;
        // Hack to set different visual properties for each participant, so
        // that they can be distinguished in the 3D and ground track views.
        // The user should have control over this.
        QColor trajectoryColor;
        switch (colorIndex % 6)
        {
        case 0: trajectoryColor = Qt::yellow; break;
        case 1: trajectoryColor = Qt::cyan; break;
        case 2: trajectoryColor = Qt::green; break;
        case 3: trajectoryColor = Qt::magenta; break;
        case 4: trajectoryColor = QColor(128, 255, 64); break;
        default: trajectoryColor = Qt::red; break;
        }
        ++colorIndex;

        // For space vehicles, we need to propagate trajectories
        if (dynamic_cast<ScenarioSC*>(participant.data()))
        {
			ScenarioSC* vehicle = dynamic_cast<ScenarioSC*>(participant.data());
			// Guillermo says: this part has been modified to accomodate mission arcs colors
			const QList<QSharedPointer<ScenarioAbstractTrajectoryType> >& trajectoryList = vehicle->SCMission()->TrajectoryPlan()->AbstractTrajectory();
			int arcIterator=0;
			foreach (QSharedPointer<ScenarioAbstractTrajectoryType> trajectory, trajectoryList)
			{
				ScenarioAbstractTrajectoryType* myAbstractTrajectory = dynamic_cast<ScenarioAbstractTrajectoryType*>(trajectory.data());
				QString trajectoryType = myAbstractTrajectory->elementName();
				if (trajectoryType == "LoiteringType")
				{
					ScenarioLoiteringType* loitering = dynamic_cast<ScenarioLoiteringType*>(trajectory.data());
					QString arcColorName = loitering->ElementIdentifier()->colorName();
					trajectoryColor = thisMissionDefaults.missionArcColorFromQt(arcColorName);
				}
				else if (trajectoryType == "LoiteringTLEType")
				{
					ScenarioLoiteringTLEType* loiteringTLE = dynamic_cast<ScenarioLoiteringTLEType*>(trajectory.data());
					QString arcColorName = loiteringTLE->ElementIdentifier()->colorName();
					trajectoryColor = thisMissionDefaults.missionArcColorFromQt(arcColorName);
				}
                scenarioPropagatorSatellite(vehicle,  trajectoryColor, feedback, propScenario);
				arcIterator++;
			}
        }
        else if (dynamic_cast<ScenarioREV*>(participant.data()))//Added by Dominic to allow propagation of re-entry vehicle trajectories
        {
            ScenarioREV* entryVehicle = dynamic_cast<ScenarioREV*>(participant.data());
            scenarioPropagatorReEntryVehicle(entryVehicle,  trajectoryColor, feedback, propScenario);
        }
        else if (dynamic_cast<ScenarioGroundStation*>(participant.data()))
        {
            ScenarioGroundStation* groundElement = dynamic_cast<ScenarioGroundStation*>(participant.data());
            scenarioPropagatorGroundElement(groundElement, trajectoryColor, feedback, propScenario);
        }
        else if (dynamic_cast<ScenarioPoint*>(participant.data()))
        {
            ScenarioPoint* point = dynamic_cast<ScenarioPoint*>(participant.data());
            scenarioPropagatorPoint(point, trajectoryColor, feedback, propScenario);
        }
    }

    if (propScenario)
    {
        setPropagatedScenario(propScenario);
    }
}   ////////////////////////// End of action PROPAGATE /////////////////////////////


///////////////////////////////////////////  Action ACTIVATE SEM  ////////////////////////////////////////
void MainWindow::on_actionSystem_Engineering_triggered()
{
    if (!scenario())
		return;

    //************************************************************ /OZGUN
    sem* SEMWidget = new sem(scenario(), this);  // Creating the widget as a tool and transferring the scenerio
    //************************************************************ OZGUN/

    SEMWidget->show(); // Creating the window modeless. This requires the declaration of the variable on the *.h file
    SEMWidget->raise(); // Required to keep the modeless window alive
    SEMWidget->activateWindow(); // Required to keep the modeless window alive

    // Activating now the pull down menus
    actionPropagate_Scenario->setEnabled(m_scenario != NULL);
    actionPropagateCoverage->setEnabled(m_scenario != NULL);
    actionSat_to_Sat->setEnabled(m_scenario != NULL);
    actionSat_to_Ground->setEnabled(m_scenario != NULL);
    actionSystem_Engineering->setEnabled(m_scenario != NULL);
    //actionAnalyse->setEnabled(m_scenario != NULL);
}   ///////////////////////////////////////////  End of Action ACTIVATE SEM ////////////////////////////////////////



///////////////////////////////////////////  Action ANALYZE ////////////////////////////////////////
void MainWindow::on_actionAnalyse_triggered()
{
    if (!scenario())
    {
        return;
    }

    analysis* AnalysisWidget = new analysis(m_scenario,m_propagatedScenario, this, Qt::Window);
    AnalysisWidget->show();

    //AnalysisWidget->raise(); // Required to keep the modeless window alive
    //AnalysisWidget->activateWindow(); // Required to keep the modeless window alive
}	///////////////////////////////////////////  End of Action ANALYZE ////////////////////////////////////////




void MainWindow::on_actionScenarioElements_toggled(bool checked)
{
    m_scenarioElementBox->setVisible(checked);
}


// Enable/disable the edit menu items based on the current tree view
// selection.
void MainWindow::on_menuEdit_aboutToShow()
{
    actionCut->setEnabled(false);
    actionCopy->setEnabled(false);
    actionPaste->setEnabled(false);

    if (m_scenarioView->m_scenarioTree)
    {
        ScenarioObject* obj = m_scenarioView->m_scenarioTree->selectedObject();
        if (dynamic_cast<ScenarioParticipantType*>(obj) || dynamic_cast<ScenarioAbstractTrajectoryType*>(obj))
        {
            actionCut->setEnabled(true);
        }
    }
}


// Set the current scenario file name and update the window
// title accordingly.
// IMPORTANT: This is an internal method only. If you're calling from another
// class, you probably want to call setScenario(), which takes care of
// additional setup.
void MainWindow::replaceCurrentScenario(SpaceScenario* scenario, QString filename)
{
    m_scenario = scenario;

    //actionPropagate_Scenario->setEnabled(m_scenario != NULL);
    //actionSystem_Engineering->setEnabled(m_scenario != NULL);

    m_scenarioFileName = filename;
    QString displayFilename = filename.section('/', -1);
    if (displayFilename.isEmpty())
        displayFilename = tr("Untitled");

    setWindowTitle(tr("STA - %1").arg(displayFilename));

    //   MainWindow::clearViews();  // Line patched by Guillermo to refresh the scenario view after this operation

}


void MainWindow::showGroundTrackPlotTool()
{
    if (m_groundTrackPlotTool == NULL)
    {
        m_groundTrackPlotTool = new GroundTrackPlotTool(this);
        m_groundTrackPlotTool->resize(1001, 500);
    }
    m_groundTrackPlotTool->show();
    m_groundTrackPlotTool->raise();
    m_groundTrackPlotTool->activateWindow();
}


void MainWindow::configureTimeline(PropagatedScenario* scenario)
{
    double duration = scenario->endTime() - scenario->startTime();

    if (duration > 0.0)
    {
        double extraTime = duration / 20.0;

        m_timelineWidget->timelineView()->setTimeRange(scenario->startTime() - extraTime,
                                                       scenario->endTime() + extraTime);
        m_timelineWidget->timelineView()->setVisibleSpan(duration + extraTime * 2.0);
    }

    m_timelineWidget->timelineView()->clearMission();

    int objectIndex = 0;
    foreach (SpaceObject* spaceObject, scenario->spaceObjects())
    {
        foreach (MissionArc* arc, spaceObject->mission())
        {
            m_timelineWidget->timelineView()->addMissionSegment(objectIndex,
                                                                arc->beginning(),
                                                                arc->ending(),
                                                                spaceObject->trajectoryColor());
        }

        ++objectIndex;
    }
}


// Set the propagated scenario and notify all view widgets so they can show
// the results of the new propagation.
void
		MainWindow::setPropagatedScenario(PropagatedScenario* scenario)
{
    if (scenario != m_propagatedScenario)
    {
        // Update all of the propagated scenario views:
        //   Timeline, ground track, 3D view
        if (m_groundTrackPlotTool)
        {
            m_groundTrackPlotTool->view()->setScenario(scenario);
        }

        if (m_threeDViewWidget)
        {
            m_threeDViewWidget->view()->setScenario(scenario);
        }

        configureTimeline(scenario);
        setTime(scenario->startTime());

        delete m_propagatedScenario;
        m_propagatedScenario = scenario;

        // Activate now the modules that are allowed to be used after propagation
        actionAnalyse->setEnabled(m_propagatedScenario);
    }
}


// Handle timer signals and notify animated widgets.
void MainWindow::tick()
{
    QDateTime currentTime = QDateTime::currentDateTime();

    // Get the current time as the number of seconds since J2000
    double now = 86400.0 * QDate(2000, 1, 1).daysTo(currentTime.date()) +
                 (1.0 / 1000.0) * QTime(0, 0).msecsTo(currentTime.time());

    if (m_lastTime == 0.0)
    {
        m_lastTime = now;
    }

    double dt = now - m_lastTime;
    m_lastTime = now;

    // Update the timeline widget (necessary whenever the timeline isn't paused)
    m_timelineWidget->tick(dt);

    // Update the 3D widget. The 3D widget needs special handling for smooth camera
    // motions. Other view widgets (such as the ground track view) should be updated
    // in the setTime() method.
    if (m_threeDViewWidget)
    {
        m_threeDViewWidget->view()->tick(dt);
    }

    // Old code kept around in case we need to implement another workaround to
    // avoid using CPU time when no updates are required.
#if USE_CELESTIA
    const int SlowUpdateInterval = 30;         // millisec
    const int TimerIntervalChangeDelay = 1000; // millisec

    // If nothing is moving in the view, reduce the timer interval so that
    // we're not tight looping on this function. We want to avoid immediately
    // dropping the timer update rate in order to avoid jerky interaction
    // with the 3D window.
    if (m_celestiaCore && m_celestiaViewWidget)
    {
        int newInterval = SlowUpdateInterval;

        if (m_celestiaCore->viewUpdateRequired())
        {
            newInterval = 0;
            m_intervalChangeTime.restart();
        }

        if (newInterval != m_celestiaUpdateTimer->interval())
        {
            if (newInterval == 0 || m_intervalChangeTime.elapsed() > TimerIntervalChangeDelay)
            {
                m_celestiaUpdateTimer->setInterval(newInterval);
                m_intervalChangeTime.restart();
            }
        }
    }
#endif
}



void MainWindow::setTime(double mjd)
{
    // Notify all view widgets that care about time updates.
    if (m_timelineWidget)
    {
        m_timelineWidget->timelineView()->setCurrentTime(mjd);
    }

    if (m_groundTrackPlotTool)
    {
        m_groundTrackPlotTool->view()->setCurrentTime(mjd);
    }

    if (m_threeDViewWidget)
    {
        m_threeDViewWidget->view()->setCurrentTime(mjd);
    }
}


void MainWindow::selectParticipant(int participantIndex)
{
    if (m_threeDViewWidget)
    {
        if (participantIndex >= 0 && participantIndex < m_propagatedScenario->spaceObjects().size())
        {
            m_threeDViewWidget->view()->selectParticipant(m_propagatedScenario->spaceObjects().at(participantIndex));
        }
        else
        {
            m_threeDViewWidget->view()->selectParticipant(NULL);
        }
    }
}


// Update the central widget when the visible tab is changed. This probably
// shouldn't be necessary, but redraws are missed on Mac OS X without it
void MainWindow::refreshCentralWidget(int tabIndex)
{
    if (tabIndex == 0)
    {
        if (m_groundTrackPlotTool)
        {
            m_groundTrackPlotTool->view()->viewport()->update();
        }
    }
    else if (tabIndex == 1)
    {
        // TODO: Update VESTA widget
    }
}


void MainWindow::toggleGroundTrackView()
{
    if (m_groundTrackPlotTool->parentWidget())
    {
        m_viewPanel->removeTab(0);
        m_groundTrackPlotTool->setParent(NULL);
        m_groundTrackPlotTool->move(100, 100);
        m_groundTrackPlotTool->show();
        m_dockGroundTrackAction->setText(tr("Dock Ground Track View"));
    }
    else
    {
        m_viewPanel->insertTab(0, m_groundTrackPlotTool, tr("Ground Track View"));
        m_dockGroundTrackAction->setText(tr("Undock Ground Track View"));
    }
}


/*! Set up the application data directory, creating it if necessary. The
 *  directory contains user-specific, persistent information for STA
 *  (such as bookmarks) which aren't stored in settings. The location
 *  of the data directory depends on the platform:
 *
 *  Win32: %APPDATA%\STA
 *  Mac OS X: $HOME/Library/Application Support/STA
 *  Unix and Mac OS X: $HOME/.config/STA
 */
void MainWindow::initAppDataDirectory()
{
#ifdef _WIN32
    // On Windows, the STA data directory is %APPDATA%\STA
    // First, get the value of the APPDATA environment variable
    QStringList envVars = QProcess::systemEnvironment();
    QString appDataPath;
    foreach (QString envVariable, envVars)
    {
        if (envVariable.startsWith("APPDATA"))
        {
            QStringList nameValue = envVariable.split("=");
            if (nameValue.size() == 2)
                appDataPath = nameValue[1];
            break;
        }
    }
#else
    // UNIX or Mac OS X
#ifdef TARGET_OS_MAC
    QString appDataPath = QDir::home().filePath("Library/Application Support");
#else
    QString appDataPath = QDir::home().filePath(".config");
#endif

#endif

    if (appDataPath != "")
    {
        // Create a STA subdirectory of APPDATA if it doesn't already exist
        QDir appDataDir(appDataPath);
        if (appDataDir.exists())
        {
            // TODO: Set path for loading VESTA info
        }
    }
#ifdef _DEBUG
    else
    {
        QMessageBox::warning(this, "APPDIR missing", "APPDIR environment variable not found!");
    }
#endif
}


void MainWindow::readSettings()
{
    QDesktopWidget desktop;

    QSettings settings;

    settings.beginGroup("MainWindow");

    QSize windowSize = settings.value("Size", DEFAULT_MAIN_WINDOW_SIZE).toSize();
    QPoint windowPosition = settings.value("Pos", DEFAULT_MAIN_WINDOW_POSITION).toPoint();

    // Make sure that the saved size fits on screen; it's possible for the previous saved
    // position to be off-screen if the monitor settings have changed.
    bool onScreen = false;
    for (int screenIndex = 0; screenIndex < desktop.numScreens(); screenIndex++)
    {
        QRect screenGeometry = desktop.screenGeometry(screenIndex);
        if (screenGeometry.contains(windowPosition))
        {
            onScreen = true;
        }
    }

    if (!onScreen)
    {
        windowPosition = DEFAULT_MAIN_WINDOW_POSITION;
        windowSize = DEFAULT_MAIN_WINDOW_SIZE;
    }

    resize(windowSize);
    move(windowPosition);
    if (settings.contains("State"))
    {
        restoreState(settings.value("State").toByteArray(), STA_MAIN_WINDOW_VERSION);
    }
    if (settings.value("Fullscreen", false).toBool())
    {
        showFullScreen();
    }

    QPoint winpos = settings.value("Pos", DEFAULT_MAIN_WINDOW_SIZE).toPoint();
    settings.endGroup();

    m_viewActions->restoreSettings();

    // Render settings read in qtglwidget
}


void MainWindow::writeSettings()
{
    QSettings settings;

    settings.beginGroup("MainWindow");
    if (isFullScreen())
    {
        // Save the normal size, not the fullscreen size; fullscreen will
        // be restored automatically.
        settings.setValue("Size", normalGeometry().size());
        settings.setValue("Pos", normalGeometry().topLeft());
    }
    else
    {
        settings.setValue("Size", size());
        settings.setValue("Pos", pos());
    }
    settings.setValue("State", saveState(STA_MAIN_WINDOW_VERSION));
    settings.setValue("Fullscreen", isFullScreen());
    settings.endGroup();

    m_viewActions->saveSettings();
}


void MainWindow::closeEvent(QCloseEvent* event)
{
    writeSettings();
    // saveBookmarks();

    event->accept();
}


/*! Reset all information in the timeline, ground track, and tree views. This should
 *  be called after closing the current scenario.
 */
void MainWindow::clearViews()
{
    m_scenarioView->m_scenarioTree->clear();
    if (m_groundTrackPlotTool)
    {
        m_groundTrackPlotTool->view()->setScenario(NULL);
    }
    if (m_threeDViewWidget)
    {
        m_threeDViewWidget->view()->setScenario(NULL);
    }
    if (m_timelineWidget)
    {
        m_timelineWidget->timelineView()->clearMission();
    }

    m_scenarioView->update();  // Lined added by Guillermo as suggested by Chris to update the view of STA
    m_scenarioView->m_scenarioTree->update();
}


// Constellation (Claas Grohnfeldt, Steffen Peter)
// Call constellation Wizard
///////////////////////////////////////////  Action CREATE CONSTELLATION ////////////////////////////////////////
void MainWindow::on_actionCreateConstellation_triggered()
{
    ConstellationWizardDialog* constellationwizard = new ConstellationWizardDialog(this);
    constellationwizard->exec();
    constellationwizard->setFocus();

    // Activating now the pull down menus
    actionPropagate_Scenario->setEnabled(m_scenario != NULL);
    actionPropagateCoverage->setEnabled(m_scenario != NULL);
    actionSat_to_Sat->setEnabled(m_scenario != NULL);
    actionSat_to_Ground->setEnabled(m_scenario != NULL);
    actionSystem_Engineering->setEnabled(m_scenario != NULL);
    actionAnalyse->setEnabled(m_scenario != NULL);
}   ///////////////////////////////////////////  End of Action CREATE CONSTELLATION ////////////////////////////////////////





///////////////////////////////////////////  Action PROPAGATE WITH COVERAGE ////////////////////////////////////////
void MainWindow::on_actionPropagateCoverage_triggered()
{

    if (!scenario())
    {
		return;
    }

    PropagationFeedback feedback;
    PropagatedScenario* propScenario = new PropagatedScenario();

    //extern int Lagrmode;

    int Lagrmode = 99;	    //Guillermo says: take this thong out of here

    int colorIndex = 0;

    // Process each participant
    foreach (QSharedPointer<ScenarioParticipantType> participant, scenario()->AbstractParticipant())
    {
		// Hack to set different visual properties for each participant, so
		// that they can be distinguished in the 3D and ground track views.
		// The user should have control over this.
		QColor trajectoryColor;
		switch (colorIndex % 6)
		{
		case 0: trajectoryColor = Qt::yellow; break;
		case 1: trajectoryColor = Qt::cyan; break;
		case 2: trajectoryColor = Qt::green; break;
		case 3: trajectoryColor = Qt::magenta; break;
		case 4: trajectoryColor = QColor(128, 255, 64); break;
		default: trajectoryColor = Qt::red; break;
		}
		++colorIndex;

		// For space vehicles, we need to propagate trajectories
		if (dynamic_cast<ScenarioSC*>(participant.data()))
		{
			ScenarioSC* vehicle = dynamic_cast<ScenarioSC*>(participant.data());

			scenarioPropagatorSatellite(vehicle,  trajectoryColor, feedback, propScenario);
		}

		else if (dynamic_cast<ScenarioREV*>(participant.data()))//Added by Dominic to allow propagation of re-entry vehicle trajectories
		{
			ScenarioREV* entryVehicle = dynamic_cast<ScenarioREV*>(participant.data());

			scenarioPropagatorReEntryVehicle(entryVehicle,  trajectoryColor, feedback, propScenario);
		}
		else if (dynamic_cast<ScenarioGroundStation*>(participant.data()))
		{
			ScenarioGroundStation* groundElement = dynamic_cast<ScenarioGroundStation*>(participant.data());

			scenarioPropagatorGroundElement(groundElement, trajectoryColor, feedback, propScenario);
		}
		else if (dynamic_cast<ScenarioPoint*>(participant.data()))
		{
			ScenarioPoint* point = dynamic_cast<ScenarioPoint*>(participant.data());

			scenarioPropagatorPoint(point, trajectoryColor, feedback, propScenario);
		}
    }


    // Guillermo says; for the time being we call a estrange ANALYSIS method on the constellations module
    ConstellationAnalysis* analysisOfConstellations = new ConstellationAnalysis(propScenario, true, false, false);

    if (propScenario)
    {
        setPropagatedScenario(propScenario);
        if (m_groundTrackPlotTool)
        {
            m_groundTrackPlotTool->setAnalysis(analysisOfConstellations);
        }
    }

}   ///////////////////////////////////////////  End of Action PROPAGATE WITH COVERAGE ////////////////////////////////////////


///////////////////////////////////////////  Action PROPAGATE WITH COVERAGE BETWEE SATELLITES ////////////////////////////////////////
void MainWindow::on_actionSat_to_Sat_triggered()
{

    if (!scenario())
    {
		return;
    }

    PropagationFeedback feedback;
    PropagatedScenario* propScenario = new PropagatedScenario();

    //extern int Lagrmode;

    int Lagrmode = 99;	    //Guillermo says: take this thong out of here

    int colorIndex = 0;

    // Process each participant
    foreach (QSharedPointer<ScenarioParticipantType> participant, scenario()->AbstractParticipant())
    {
		// Hack to set different visual properties for each participant, so
		// that they can be distinguished in the 3D and ground track views.
		// The user should have control over this.
		QColor trajectoryColor;
		switch (colorIndex % 6)
		{
		case 0: trajectoryColor = Qt::yellow; break;
		case 1: trajectoryColor = Qt::cyan; break;
		case 2: trajectoryColor = Qt::green; break;
		case 3: trajectoryColor = Qt::magenta; break;
		case 4: trajectoryColor = QColor(128, 255, 64); break;
		default: trajectoryColor = Qt::red; break;
		}
		++colorIndex;

		// For space vehicles, we need to propagate trajectories
		if (dynamic_cast<ScenarioSC*>(participant.data()))
		{
			ScenarioSC* vehicle = dynamic_cast<ScenarioSC*>(participant.data());

			scenarioPropagatorSatellite(vehicle,  trajectoryColor, feedback, propScenario);
		}

		else if (dynamic_cast<ScenarioREV*>(participant.data()))//Added by Dominic to allow propagation of re-entry vehicle trajectories
		{
			ScenarioREV* entryVehicle = dynamic_cast<ScenarioREV*>(participant.data());

			scenarioPropagatorReEntryVehicle(entryVehicle,  trajectoryColor, feedback, propScenario);
		}
		else if (dynamic_cast<ScenarioGroundStation*>(participant.data()))
		{
			ScenarioGroundStation* groundElement = dynamic_cast<ScenarioGroundStation*>(participant.data());

			scenarioPropagatorGroundElement(groundElement, trajectoryColor, feedback, propScenario);
		}
		else if (dynamic_cast<ScenarioPoint*>(participant.data()))
		{
			ScenarioPoint* point = dynamic_cast<ScenarioPoint*>(participant.data());

			scenarioPropagatorPoint(point, trajectoryColor, feedback, propScenario);
		}
    }


    ConstellationAnalysis* analysisOfConstellations = new ConstellationAnalysis(propScenario, false, true, false);

    if (propScenario)
    {
        setPropagatedScenario(propScenario);
        if (m_groundTrackPlotTool)
        {
            m_groundTrackPlotTool->setAnalysis(analysisOfConstellations);
        }
    }

}  ///////////////////////////////////////////  End of Action PROPAGATE WITH COVERAGE BETWEEN SATELLITES ////////////////////////////////////////



///////////////////////////////////////////  Action PROPAGATE WITH COVERAGE BETWEEN STATIONS AND SATELLITES ////////////////////////////////////////
void MainWindow::on_actionSat_to_Ground_triggered()
{

    if (!scenario())
    {
		return;
    }

    PropagationFeedback feedback;
    PropagatedScenario* propScenario = new PropagatedScenario();

    //extern int Lagrmode;

    int Lagrmode = 99;	    //Guillermo says: take this thong out of here

    int colorIndex = 0;

    // Process each participant
    foreach (QSharedPointer<ScenarioParticipantType> participant, scenario()->AbstractParticipant())
    {
        // Hack to set different visual properties for each participant, so
        // that they can be distinguished in the 3D and ground track views.
        // The user should have control over this.
        QColor trajectoryColor;
        switch (colorIndex % 6)
        {
        case 0: trajectoryColor = Qt::yellow; break;
        case 1: trajectoryColor = Qt::cyan; break;
        case 2: trajectoryColor = Qt::green; break;
        case 3: trajectoryColor = Qt::magenta; break;
        case 4: trajectoryColor = QColor(128, 255, 64); break;
        default: trajectoryColor = Qt::red; break;
        }
        ++colorIndex;

        // For space vehicles, we need to propagate trajectories
        if (dynamic_cast<ScenarioSC*>(participant.data()))
        {
            ScenarioSC* vehicle = dynamic_cast<ScenarioSC*>(participant.data());
            scenarioPropagatorSatellite(vehicle,  trajectoryColor, feedback, propScenario);
        }

        else if (dynamic_cast<ScenarioREV*>(participant.data()))//Added by Dominic to allow propagation of re-entry vehicle trajectories
        {
            ScenarioREV* entryVehicle = dynamic_cast<ScenarioREV*>(participant.data());
            scenarioPropagatorReEntryVehicle(entryVehicle,  trajectoryColor, feedback, propScenario);
        }
        else if (dynamic_cast<ScenarioGroundStation*>(participant.data()))
        {
            ScenarioGroundStation* groundElement = dynamic_cast<ScenarioGroundStation*>(participant.data());
            scenarioPropagatorGroundElement(groundElement, trajectoryColor, feedback, propScenario);
        }
        else if (dynamic_cast<ScenarioPoint*>(participant.data()))
        {
            ScenarioPoint* point = dynamic_cast<ScenarioPoint*>(participant.data());
            scenarioPropagatorPoint(point, trajectoryColor, feedback, propScenario);
        }
    }

    ConstellationAnalysis* analysisOfConstellations = new ConstellationAnalysis(propScenario, false, false, true);

    if (propScenario)
    {
        setPropagatedScenario(propScenario);
        if (m_groundTrackPlotTool)
        {
            m_groundTrackPlotTool->setAnalysis(analysisOfConstellations);
        }
    }

}  ///////////////////////////////////////////  End of Action PROPAGATE WITH COVERAGE BETWEEN STATIONS AND SATELLITES ////////////////////////////////////////


///////////////////////////////////////////  Method to handle change of events (drop, drag, etc) ////////////////////////////////////////
/*! This method handles what happen when the user drops a file into the app icon
  */
void MainWindow::openFileFromAEvent(const QString& fileName)
{
    // Start the open file dialog in the same directory as last time
    QString dir;
    QSettings settings;
    settings.beginGroup("Preferences");
    if (settings.contains("OpenScenarioDir"))
    {
		dir = settings.value("OpenScenarioDir").toString();
    }

    if (!fileName.isEmpty())
    {
		QFile scenarioFile(fileName);
		if (!scenarioFile.open(QIODevice::ReadOnly))
		{
			QMessageBox::critical(this, tr("Error"), tr("Error opening file %1").arg(fileName));
		}
		else
		{
			// Save the scenario file directory
			QFileInfo scenarioFileInfo(fileName);
			settings.setValue("OpenScenarioDir", scenarioFileInfo.absolutePath());

			if (!m_spaceScenarioSchema)
			{
				QFile schemaFile(SCHEMA_FILE);
				if (!schemaFile.open(QIODevice::ReadOnly))
				{
					QMessageBox::critical(this, tr("Critical Error"), tr("Error opening space scenario schema file. Unable to load scenario."));
					return;
				}

				m_spaceScenarioSchema = new QXmlSchema;
				if (!m_spaceScenarioSchema->load(&schemaFile, QUrl::fromLocalFile(schemaFile.fileName())))
				{
					QMessageBox::critical(this, tr("Critical Error"), tr("Error in space scenario schema file. Unable to load scenario."));
					delete m_spaceScenarioSchema;
					m_spaceScenarioSchema = NULL;
					return;
				}
			}

			QXmlSchemaValidator validator(*m_spaceScenarioSchema);
			if (!validator.validate(&scenarioFile))
			{

				QMessageBox::critical(this, tr("Scenario Load Error"), tr("Scenario is not a valid space scenario."));
				return;
			}

			scenarioFile.reset();
			QDomDocument scenarioDoc;
			if (!scenarioDoc.setContent(&scenarioFile))
			{
				// This should not fail, since we just got done validating the xml file against the space
				// scenario schema.
				scenarioFile.close();
				QMessageBox::critical(this, tr("Scenario Load Error"), tr("Internal error occurred when loading space scenario."));
				return;
			}

			scenarioFile.close();

			QDomElement rootElement = scenarioDoc.firstChildElement("tns:SpaceScenario");
			SpaceScenario* scenario = SpaceScenario::create(rootElement);
			if (!scenario)
			{
				QMessageBox::critical(this, tr("Scenario Load Error"), tr("Internal error (parser problem) occurred when loading space scenario."));
				return;
			}

			// TODO: Probably should just call setScenario() here.
			clearViews();

			// Prohibit drops to the top level item
			QTreeWidgetItem* invisibleRoot = m_scenarioView->m_scenarioTree->invisibleRootItem();
			invisibleRoot->setFlags(invisibleRoot->flags() & ~Qt::ItemIsDropEnabled);

			QTreeWidgetItem* rootItem = new QTreeWidgetItem(m_scenarioView->m_scenarioTree);
			rootItem->setExpanded(true);
			rootItem->setText(0, "Space scenario");
			rootItem->setText(1, scenario->Name());
			rootItem->setFlags(rootItem->flags() & ~Qt::ItemIsDragEnabled);

			m_scenarioView->m_scenarioTree->addScenarioItems(rootItem, scenario);
			m_scenarioView->m_scenarioTree->addTopLevelItem(rootItem);
			replaceCurrentScenario(scenario, fileName);

			// This sequence seems to be required to force the scenario view
			// widget to update (at least on Mac OS X)
			m_scenarioView->m_scenarioTree->update();
			m_scenarioView->setFocus();
			m_scenarioView->m_scenarioTree->setFocus();
		}
    }

    settings.endGroup();

    // Activating now the pull down menus
    actionPropagate_Scenario->setEnabled(m_scenario != NULL);
    actionPropagateCoverage->setEnabled(m_scenario != NULL);
    actionSat_to_Sat->setEnabled(m_scenario != NULL);
    actionSat_to_Ground->setEnabled(m_scenario != NULL);
    actionSystem_Engineering->setEnabled(m_scenario != NULL);
    actionAnalyse->setDisabled(m_scenario != NULL);  // Do not enable analysis here. Do it after propagation

}
