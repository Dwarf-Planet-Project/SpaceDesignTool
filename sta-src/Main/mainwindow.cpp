/*
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
  Patched by Guillermo on April 2011 to add Interplanetary, Rendezvous and Lagrangian modules on the main window


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
#include <QDockWidget>
#include <QMessageBox>
#include <QFileDialog>
#include <QDesktopWidget>

#include <time.h>

#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "Main/preferences.h"
#include "scenarioelementbox.h"
#include "scenarioview.h"
#include "scenariotree.h"
#include "timelinewidget.h"
#include "timelineview.h"
#include "ViewActionGroup.h"
#include "Scenario/scenario.h"
#include "Scenario/propagationfeedback.h"
#include "Visualization/ThreeDView.h"
#include "Visualization/GroundTrackPlotTool.h"
#include "Visualization/ThreeDVisualizationTool.h"
#include "Astro-Core/date.h"

#include "Scenario/scenarioPropagator.h"

//****************** /OZGUN
#include "Astro-Core/EclipseDuration.h"
#include <QDebug>
//***************** OZGUN/

#include "propagatedscenario.h"
#include "RendezVous/rendezVousDialog.h"
#include "Lagrangian/lagrangianDialog.h"
#include "Interplanetary/interplanetaryDialog.h"
#include "Entry/reentry.h"
#include "Loitering/loitering.h"
#include "Loitering/loiteringTLE.h"
#include "SEM/sem.h"
//#include "Analysis/analysis.h"
#include "staexportdialog.h"

#include "Calculator/STAcalculator.h"
#include "Help/HelpBrowser.h"
#include "OemImporter.h"
#include "about.h"
#include "Constellations/constellationwizard.h"  // Constellations
#include "Scenario/missionsDefaults.h"
#include "Analysis/qtiplotmain.h"


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
        //m_AnalysisDialog(NULL),

        m_threeDViewWidget(NULL),

        m_spaceScenarioSchema(NULL),

        m_viewActions(NULL),

        m_lastTime(0.0)
{
    setupUi(this);
    connect(actionQuit, SIGNAL(triggered()), QApplication::instance(), SLOT(closeAllWindows()));

    m_viewActions = new ViewActionGroup();
    connect(m_viewActions->viewSelectGroup(), SIGNAL(triggered(QAction*)), this, SLOT(selectVisualization(QAction*)));

    m_groundTrackPlotTool = new GroundTrackPlotTool(this, m_viewActions);
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
    //scenarioViewDock->setAllowedAreas(Qt::LeftDockWidgetArea);

    m_scenarioView = new ScenarioView(this);
    scenarioViewDock->setWidget(m_scenarioView);
    addDockWidget(Qt::LeftDockWidgetArea, scenarioViewDock);
    scenarioViewDock->setVisible(true);

    // Scenario element box dock item
    QDockWidget* scenarioElementDock = new QDockWidget(tr("Scenario Element Box"), this);
    scenarioElementDock->setObjectName("sta-scenario-element-box");
    //scenarioElementDock->setAllowedAreas(Qt::LeftDockWidgetArea);
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
    //timelineDock->setAllowedAreas(Qt::BottomDockWidgetArea);
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
    //m_viewPanel = new QTabWidget(this);
    //m_viewPanel->setDocumentMode(true);

    //m_viewPanel->addTab(m_groundTrackPlotTool, tr("Ground Track View"));
    m_viewPanel = new QStackedWidget(this);
    m_viewPanel->setContentsMargins(0, 0, 0, 0);
    m_viewPanel->layout()->setContentsMargins(0, 0, 0, 0);
    m_viewPanel->layout()->setMargin(0);
    m_viewPanel->layout()->setSpacing(0);
    m_viewPanel->addWidget(m_groundTrackPlotTool);

    m_threeDViewWidget = new ThreeDVisualizationTool(this, m_viewActions);
    //m_viewPanel->addTab(m_threeDViewWidget, tr("3D View"));
    m_viewPanel->addWidget(m_threeDViewWidget);

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
    m_fullScreen3DAction = new QAction(tr("Show Full Screen 3D View"), menuWindow);
    m_fullScreen3DAction->setCheckable(true);
    m_fullScreen3DAction->setShortcut(QKeySequence("Ctrl+Shift+F"));
    menuWindow->addAction(m_fullScreen3DAction);
    connect(m_fullScreen3DAction, SIGNAL(triggered(bool)), this, SLOT(setFullScreen3D(bool)));

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

    //setUnifiedTitleAndToolBarOnMac(true);
    setUnifiedTitleAndToolBarOnMac(false);
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

    {
        ThreeDView* view = m_threeDViewWidget->view();

        // Point the camera in the 3D view at Earth
        view->gotoBody(STA_SOLAR_SYSTEM->earth());

        // Connect actions
        connect(m_viewActions->starsAction(), SIGNAL(toggled(bool)), view, SLOT(setStars(bool)));
        connect(m_viewActions->equatorialGridAction(), SIGNAL(toggled(bool)), view, SLOT(setEquatorialGrid(bool)));
        connect(m_viewActions->cloudsAction(), SIGNAL(toggled(bool)), view, SLOT(setClouds(bool)));
        connect(m_viewActions->shadowsAction(), SIGNAL(toggled(bool)), view, SLOT(setShadows(bool)));
        connect(m_viewActions->reflectionsAction(), SIGNAL(toggled(bool)), view, SLOT(setReflections(bool)));
        connect(m_viewActions->atmospheresAction(), SIGNAL(toggled(bool)), view, SLOT(setAtmospheres(bool)));
        connect(m_viewActions->satelliteTrajectoriesAction(), SIGNAL(toggled(bool)), view, SLOT(setSatelliteTrajectories(bool)));
        connect(m_viewActions->planetOrbitsAction(), SIGNAL(toggled(bool)), view, SLOT(setPlanetOrbits(bool)));
        connect(m_viewActions->moonOrbitsAction(), SIGNAL(toggled(bool)), view, SLOT(setMoonOrbits(bool)));
        connect(m_viewActions->planetLabelsAction(), SIGNAL(toggled(bool)), view, SLOT(setPlanetLabels(bool)));
        connect(m_viewActions, SIGNAL(ambientLightChanged(float)), view, SLOT(setAmbientLight(float)));
        connect(m_viewActions, SIGNAL(skyLayerChanged(int)), view, SLOT(setCurrentSkyLayer(int)));
        connect(m_viewActions, SIGNAL(stereoModeChanged(int)), view, SLOT(setStereoMode(int)));
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


// Check to see whether the specified scenario object has central bodies
// that are valid. Returns true if everything is OK, false if a
// central body doesn't exist or doesn't have an ephemeris.
//
bool CheckCentralBodies(QSharedPointer<ScenarioObject> obj)
{
    foreach (QSharedPointer<ScenarioObject> child, obj->children())
    {
        if (!child.dynamicCast<ScenarioCentralBodyType>().isNull())
        {
            QSharedPointer<ScenarioCentralBodyType> centralBody = child.dynamicCast<ScenarioCentralBodyType>();
            const StaBody* body = STA_SOLAR_SYSTEM->lookup(centralBody->Name());
            if (!body)
            {
                // Unknown central body
                QMessageBox::warning(NULL, "Invalid central body", QString("Scenario has an unknown central body '%1'").arg(centralBody->Name()));
                return false;
            }
            else if (body->ephemeris() == NULL)
            {
                // No ephemeris
                QMessageBox::warning(NULL, "Missing ephemeris", QString("Scenario requires ephemeris for body '%1', but this ephemeris is not loaded").arg(centralBody->Name()));
                return false;
            }
        }

        // Recursively check child objects
        if (!CheckCentralBodies(child))
        {
            return false;
        }
    }

    return true;
}


// Check to see whether the specified scenario has central bodies
// that are valid. Returns true if everything is OK, false if a
// central body doesn't exist or doesn't have an ephemeris.
bool CheckScenarioCentralBodies(const SpaceScenario* scenario)
{
    foreach (QSharedPointer<ScenarioParticipantType> participant, scenario->AbstractParticipant())
    {
        if (!CheckCentralBodies(participant))
        {
            return false;
        }
    }

    return true;
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

    SpaceScenario* scenario = NULL;

    QString fileName = QFileDialog::getOpenFileName(this,
                                                    tr("Open Scenario"),
                                                    dir,
                                                    "Space Scenario Files (*.stas);;Orbit Ephemeris Messages (*.oem)");
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

            if (fileName.endsWith(".oem", Qt::CaseInsensitive))
            {
                // Import an OEM (Orbit Ephemeris Message) file
                QFile oemFile(fileName);
                if (!oemFile.open(QIODevice::ReadOnly))
                {
                    QMessageBox::warning(this, tr("Error"), tr("Error opening file %1").arg(fileName));
                    return;
                }

                QTextStream oemStream(&oemFile);
                OemImporter importer(&oemStream);

                scenario = importer.loadScenario();

                if (!scenario)
                {
                    QMessageBox::warning(this, tr("OEM File Error"), tr("%1 (line %2)").arg(importer.errorMessage()).arg(importer.lineNumber()));
                    return;
                }
            }
            else
            {
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
                scenario = SpaceScenario::create(rootElement);
                if (!scenario)
                {
                    QMessageBox::critical(this, tr("Scenario Load Error"), tr("Internal error (parser problem) occurred when loading space scenario."));
                    return;
                }
            }

            // Make sure that the necessary solar system ephemerides are loaded. This check can
            // fail if SPICE solar system kernels are unavailable and the scenario involves a natural
            // satellite other than the Earth's moon.
            if (!CheckScenarioCentralBodies(scenario))
            {
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

            // Don't set the current file name if we've imported from a non-STA format. This
            // forces the user to choose a new file name when saving, which is better than
            // overwriting the source file.
            if (fileName.endsWith(".stas", Qt::CaseInsensitive))
            {
                replaceCurrentScenario(scenario, fileName);
            }
            else
            {
                replaceCurrentScenario(scenario, "");
            }

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

    staExportDialog exportDialog(m_propagatedScenario, this);
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
    staPreferencesWidget->setSkyLayer(m_viewActions->currentSkyLayer());
    staPreferencesWidget->setStereoMode(m_viewActions->stereoMode());
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




///////////////////////////////////////////  Action PROPAGATE ////////////////////////////////////////
void MainWindow::on_actionPropagate_Scenario_triggered()
{
    if (!scenario())
    {
        qDebug() << "No scenario!!";
        return;
    }

    PropagationFeedback feedback;
    PropagatedScenario* propScenario = new PropagatedScenario();

    // Process each participant
    foreach (QSharedPointer<ScenarioParticipantType> participant, scenario()->AbstractParticipant())
    {
        // For space vehicles, we need to propagate trajectories
        if (dynamic_cast<ScenarioSC*>(participant.data()))
        {
            ScenarioSC* vehicle = dynamic_cast<ScenarioSC*>(participant.data());
            scenarioPropagatorSatellite(vehicle, feedback, propScenario, this);
        }
        else if (dynamic_cast<ScenarioREV*>(participant.data()))
        {
            ScenarioREV* entryVehicle = dynamic_cast<ScenarioREV*>(participant.data());
            scenarioPropagatorReEntryVehicle(entryVehicle, feedback, propScenario, this);
        }
        else if (dynamic_cast<ScenarioGroundStation*>(participant.data()))
        {
            ScenarioGroundStation* groundElement = dynamic_cast<ScenarioGroundStation*>(participant.data());
            scenarioPropagatorGroundElement(groundElement, feedback, propScenario, this);
        }
        else if (dynamic_cast<ScenarioPoint*>(participant.data()))
        {
            ScenarioPoint* point = dynamic_cast<ScenarioPoint*>(participant.data());
            scenarioPropagatorPoint(point, Qt::yellow, feedback, propScenario);
        }
        else if (dynamic_cast<ScenarioRegion*>(participant.data()))
        {
            ScenarioRegion* region = dynamic_cast<ScenarioRegion*>(participant.data());
            scenarioPropagatorRegion(region, feedback, propScenario);
        }
    }

    if (feedback.status() != PropagationFeedback::PropagationOk)
    {
        QMessageBox::critical(this, tr("Propagation Error."), tr("Propagation failed: %1").arg(feedback.errorString()));
        delete propScenario;
        setPropagatedScenario(NULL);
    }
    else
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

    m_qtiPlotFrame = new QtiPlotMain(false, this);
    //m_qtiPlotFrame->setName("STA Analysis");
    m_qtiPlotFrame->passTheSTAscenarioToQtiPlotMain(m_scenario, m_propagatedScenario);

    // Call QtiPlot to create a table from the analysis results
    /*
    QtiPlotMain *qtiPlot = findChild<QtiPlotMain*>("STA Analysis");
    if (!qtiPlot)
    {
        qtiPlot = new QtiPlotMain(false, this);
        qtiPlot->setName("STA Analysis");
    }
    */



    //analysis* AnalysisWidget = new analysis(m_scenario,m_propagatedScenario, this, Qt::Window);
    //AnalysisWidget->show();

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
        if (dynamic_cast<ScenarioParticipantType*>(obj) ||
            dynamic_cast<ScenarioAbstractTrajectoryType*>(obj) ||
            dynamic_cast<ScenarioAbstractPayloadType*>(obj))
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
        m_groundTrackPlotTool = new GroundTrackPlotTool(this, m_viewActions);
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
                                                                arc->arcTrajectoryColor(),
                                                                arc->arcName());
        }

        ++objectIndex;
    }
}


// Set the propagated scenario and notify all view widgets so they can show
// the results of the new propagation.
void MainWindow::setPropagatedScenario(PropagatedScenario* scenario)
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
            m_threeDViewWidget->toolBar()->configureCameraMenu(scenario);
            m_threeDViewWidget->view()->setScenario(scenario);
        }

        setTime(scenario->startTime());
        configureTimeline(scenario);

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


void
        MainWindow::selectVisualization(QAction* action)
{
    QString visName = action->text();

    if (m_viewPanel->count() == 1)
    {
        // The ground track view is undocked; if that's the visualization that was selected,
        // move it to the front. Otherwise, bring the 3D window to the front.
        if (visName == "2D")
        {
            m_groundTrackPlotTool->raise();
        }
        else
        {
            this->raise();
        }
    }
    else
    {
        if (visName == "2D")
        {
            m_viewPanel->setCurrentIndex(0);
        }
        else if (visName == "3D")
        {
            m_viewPanel->setCurrentIndex(1);
        }
    }
}


void
        MainWindow::toggleGroundTrackView()
{
    if (m_groundTrackPlotTool->parentWidget())
    {
        m_viewPanel->removeWidget(m_groundTrackPlotTool);
        m_groundTrackPlotTool->setParent(NULL);
        m_groundTrackPlotTool->move(100, 100);
        m_groundTrackPlotTool->show();
        m_dockGroundTrackAction->setText(tr("Dock Ground Track View"));
    }
    else
    {
        m_viewPanel->setCurrentIndex(0);
        m_viewPanel->insertWidget(0, m_groundTrackPlotTool);
        m_dockGroundTrackAction->setText(tr("Undock Ground Track View"));
    }
}


void
        MainWindow::setFullScreen3D(bool enable)
{
    if (enable)
    {
        // Keep track of the window state before switching to fullscreen
        // mode.
        m_savedWindowState = saveState(STA_MAIN_WINDOW_VERSION);

        foreach (QObject* obj, this->children())
        {
            QDockWidget* dockWidget = qobject_cast<QDockWidget*>(obj);
            if (dockWidget)
            {
                dockWidget->hide();
            }
        }

        m_viewPanel->setCurrentIndex(1);
        m_threeDViewWidget->view()->setViewChanged();
        m_threeDViewWidget->setToolBarVisible(false);
        showFullScreen();
    }
    else
    {
        m_threeDViewWidget->setToolBarVisible(true);
        showNormal();
        restoreState(m_savedWindowState, STA_MAIN_WINDOW_VERSION);
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
        //showFullScreen();
    }

    QPoint winpos = settings.value("Pos", DEFAULT_MAIN_WINDOW_SIZE).toPoint();
    settings.endGroup();

    m_viewActions->restoreSettings();

    // Render settings read in qtglwidget
}


/** Save the position of the windows and dock items before exiting. They
 *  will be restored the next time the user runs STA.
 */
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

    // If the user exists during fullscreen mode, save the window state
    // that was active *before* switching to fullscreen
    if (isFullScreen())
    {
        settings.setValue("State", m_savedWindowState);
    }
    else
    {
        settings.setValue("State", saveState(STA_MAIN_WINDOW_VERSION));
    }
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

    // Process each participant
    foreach (QSharedPointer<ScenarioParticipantType> participant, scenario()->AbstractParticipant())
    {

        // For space vehicles, we need to propagate trajectories
        if (dynamic_cast<ScenarioSC*>(participant.data()))
        {
            ScenarioSC* vehicle = dynamic_cast<ScenarioSC*>(participant.data());
            scenarioPropagatorSatellite(vehicle, feedback, propScenario, this);
        }
        else if (dynamic_cast<ScenarioREV*>(participant.data()))
        {
            ScenarioREV* entryVehicle = dynamic_cast<ScenarioREV*>(participant.data());
            scenarioPropagatorReEntryVehicle(entryVehicle, feedback, propScenario, this);
        }
        else if (dynamic_cast<ScenarioGroundStation*>(participant.data()))
        {
            ScenarioGroundStation* groundElement = dynamic_cast<ScenarioGroundStation*>(participant.data());
            scenarioPropagatorGroundElement(groundElement, feedback, propScenario, this);
        }
        else if (dynamic_cast<ScenarioPoint*>(participant.data()))
        {
            ScenarioPoint* point = dynamic_cast<ScenarioPoint*>(participant.data());
            scenarioPropagatorPoint(point, feedback, propScenario);
        }

    }


    // Guillermo says; for the time being we call a estrange ANALYSIS method on the constellations module
    propScenario->setCoverageTriggered();
    propScenario->setStudyOfConstellations();
    ConstellationStudy* studyOfConstellations = new ConstellationStudy(propScenario, true, false, false);

    if (propScenario)
    {
        setPropagatedScenario(propScenario);
        if (m_groundTrackPlotTool)
        {
            m_groundTrackPlotTool->setAnalysis(studyOfConstellations);
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

    // Process each participant
    foreach (QSharedPointer<ScenarioParticipantType> participant, scenario()->AbstractParticipant())
    {

        // For space vehicles, we need to propagate trajectories
        if (dynamic_cast<ScenarioSC*>(participant.data()))
        {
            ScenarioSC* vehicle = dynamic_cast<ScenarioSC*>(participant.data());
            scenarioPropagatorSatellite(vehicle, feedback, propScenario, this);
        }
        else if (dynamic_cast<ScenarioREV*>(participant.data()))
        {
            ScenarioREV* entryVehicle = dynamic_cast<ScenarioREV*>(participant.data());
            scenarioPropagatorReEntryVehicle(entryVehicle, feedback, propScenario, this);
        }
        else if (dynamic_cast<ScenarioGroundStation*>(participant.data()))
        {
            ScenarioGroundStation* groundElement = dynamic_cast<ScenarioGroundStation*>(participant.data());
            scenarioPropagatorGroundElement(groundElement, feedback, propScenario, this);
        }
        else if (dynamic_cast<ScenarioPoint*>(participant.data()))
        {
            ScenarioPoint* point = dynamic_cast<ScenarioPoint*>(participant.data());
            scenarioPropagatorPoint(point, feedback, propScenario);
        }

    }


    ConstellationStudy* studyOfConstellations = new ConstellationStudy(propScenario, false, true, false);

    if (propScenario)
    {
        setPropagatedScenario(propScenario);
        if (m_groundTrackPlotTool)
        {
            m_groundTrackPlotTool->setAnalysis(studyOfConstellations);
        }
    }

}  ///////////////////////////////////////////  End of Action PROPAGATE WITH COVERAGE BETWEEN SATELLITES ////////////////////////////////////////


void MainWindow::on_actionCreateInterplanetaryPlan_triggered()
{
    interplanetaryDialog* myInterplanetaryPlanDialoge = new interplanetaryDialog(this);
    myInterplanetaryPlanDialoge->exec();
    myInterplanetaryPlanDialoge->setFocus();

    // Activating now the pull down menus
    actionPropagate_Scenario->setEnabled(m_scenario != NULL);
    actionPropagateCoverage->setEnabled(m_scenario != NULL);
    actionSat_to_Sat->setEnabled(m_scenario != NULL);
    actionSat_to_Ground->setEnabled(m_scenario != NULL);
    actionSystem_Engineering->setEnabled(m_scenario != NULL);
    actionAnalyse->setEnabled(m_scenario != NULL);
}



void MainWindow::on_actionCreateLagrangianPlan_triggered()
{
    lagrangianDialog* myLagrangianPlanDialoge = new lagrangianDialog(this);
    myLagrangianPlanDialoge->exec();
    myLagrangianPlanDialoge->setFocus();

    // Activating now the pull down menus
    actionPropagate_Scenario->setEnabled(m_scenario != NULL);
    actionPropagateCoverage->setEnabled(m_scenario != NULL);
    actionSat_to_Sat->setEnabled(m_scenario != NULL);
    actionSat_to_Ground->setEnabled(m_scenario != NULL);
    actionSystem_Engineering->setEnabled(m_scenario != NULL);
    actionAnalyse->setEnabled(m_scenario != NULL);
}



void MainWindow::on_actionCreateRendezvousPlan_triggered()
{
    //ALONSO.- Changes in definition of RVDialog. scenario and propagated scenario are needed
    RendezVousDialog* myRendezvousPlanDialoge = new RendezVousDialog(m_scenario,m_propagatedScenario,this);
    myRendezvousPlanDialoge->exec();
    myRendezvousPlanDialoge->setFocus();

    // Activating now the pull down menus
    actionPropagate_Scenario->setEnabled(m_scenario != NULL);
    actionPropagateCoverage->setEnabled(m_scenario != NULL);
    actionSat_to_Sat->setEnabled(m_scenario != NULL);
    actionSat_to_Ground->setEnabled(m_scenario != NULL);
    actionSystem_Engineering->setEnabled(m_scenario != NULL);
    actionAnalyse->setEnabled(m_scenario != NULL);
}




///////////////////////////////////////////  Action PROPAGATE WITH COVERAGE BETWEEN STATIONS AND SATELLITES ////////////////////////////////////////
void MainWindow::on_actionSat_to_Ground_triggered()
{

    if (!scenario())
    {
        return;
    }

    PropagationFeedback feedback;
    PropagatedScenario* propScenario = new PropagatedScenario();


    // Process each participant
    foreach (QSharedPointer<ScenarioParticipantType> participant, scenario()->AbstractParticipant())
    {

        // For space vehicles, we need to propagate trajectories
        if (dynamic_cast<ScenarioSC*>(participant.data()))
        {
            ScenarioSC* vehicle = dynamic_cast<ScenarioSC*>(participant.data());
            scenarioPropagatorSatellite(vehicle, feedback, propScenario, this);
        }
        else if (dynamic_cast<ScenarioREV*>(participant.data()))
        {
            ScenarioREV* entryVehicle = dynamic_cast<ScenarioREV*>(participant.data());
            scenarioPropagatorReEntryVehicle(entryVehicle, feedback, propScenario, this);
        }
        else if (dynamic_cast<ScenarioGroundStation*>(participant.data()))
        {
            ScenarioGroundStation* groundElement = dynamic_cast<ScenarioGroundStation*>(participant.data());
            scenarioPropagatorGroundElement(groundElement, feedback, propScenario, this);
        }
        else if (dynamic_cast<ScenarioPoint*>(participant.data()))
        {
            ScenarioPoint* point = dynamic_cast<ScenarioPoint*>(participant.data());
            scenarioPropagatorPoint(point, feedback, propScenario);
        }

    }

    ConstellationStudy* studyOfConstellations = new ConstellationStudy(propScenario, false, false, true);

    if (propScenario)
    {
        setPropagatedScenario(propScenario);
        if (m_groundTrackPlotTool)
        {
            m_groundTrackPlotTool->setAnalysis(studyOfConstellations);
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
            QMessageBox::critical(this, tr("Error"), tr("opening file %1").arg(fileName));
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

            // This sequence seems to be required to force the scenario view
            // widget to update (at least on Mac OS X)
            m_scenarioView->m_scenarioTree->update();
            m_scenarioView->setFocus();
            m_scenarioView->m_scenarioTree->setFocus();

            setScenario(scenario);
        }
    }

    settings.endGroup();


    //qDebug() << "Activating menus" << actionPropagate_Scenario->isEnabled();

    // Activating now the pull down menus
    actionPropagate_Scenario->setEnabled(m_scenario != NULL);
    actionPropagateCoverage->setEnabled(m_scenario != NULL);
    actionSat_to_Sat->setEnabled(m_scenario != NULL);
    actionSat_to_Ground->setEnabled(m_scenario != NULL);
    actionSystem_Engineering->setEnabled(m_scenario != NULL);
    actionAnalyse->setDisabled(m_scenario != NULL);  // Do not enable analysis here. Do it after propagation

}

///////////////////////////////////  Create a new scenario ////////////////////////////////////////////
void MainWindow::fileNew()
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
