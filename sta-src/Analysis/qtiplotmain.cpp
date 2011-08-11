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



#include <QApplication>
#include <QSignalMapper>
#include <QDebug>
#include <QDockWidget>

#include "qtiplotmain.h"
#include "analysisParametersChoice.h"
#include "Scenario/staschema.h"
#include "Main/propagatedscenario.h"

#include "thirdparty/qtiplot/qtiplot/src/ApplicationWindow.h"


QtiPlotMain::QtiPlotMain(bool factorySettings, QWidget *parent) :
    QMainWindow(parent),
    m_appWindow(NULL),
    m_analysisParameterChooser(NULL)
{
    initQtiPlot();
};


QtiPlotMain::~QtiPlotMain()
{
    delete m_appWindow;
};


/** Generate a QtiPlot table from analysis results.
  */
void
QtiPlotMain::createReport(AnalysisResult *analysis)
{
    if (!m_appWindow)
    {
        initQtiPlot();
    }

    Table* t = m_appWindow->newTable(analysis->title(), analysis->rowCount(), analysis->columnCount());
    for (int i = 0; i < analysis->columnCount(); ++i)
    {
        t->setColName(i, analysis->columnName(i));
    }

    const QVariantList data = analysis->data();
    for (int row = 0; row < analysis->rowCount(); ++row)
    {
        for (int col = 0; col < analysis->columnCount(); ++col)
        {
            t->setText(row, col, data.at(row * analysis->columnCount() + col).toString());
        }
    }
}


void
QtiPlotMain::qtiClosed()
{
    m_appWindow = NULL;
}


void
QtiPlotMain::createTable()
{
    AnalysisResult results = m_analysisParameterChooser->generateReport();
    if (results.isValid())
    {
        createReport(&results);
    }
}


/** Initialize the QtiPlot module.
  */
void
QtiPlotMain::initQtiPlot()
{
    m_appWindow = new ApplicationWindow(false);
    m_appWindow->restoreApplicationGeometry();
    connect(m_appWindow, SIGNAL(destroyed()), this, SLOT(qtiClosed()));

    // We want to change the function of the new table action so that it creates
    // a new table filled with data from STA. In order to do this without modifying
    // the QtiPlot source code, we'll scan the list of actions in the main QtiPlot
    // window and reconnect the one labeled "New Table". This will no longer work if
    // the action text is changed, but it's otherwise a better solution than changing
    // code inside QtiPlot.
    foreach (QObject* child, m_appWindow->children())
    {
        QAction* action = qobject_cast<QAction*>(child);
        if (action)
        {
            if (action->text() == "New &Table")
            {
                action->disconnect();
                connect(action, SIGNAL(activated()), this, SLOT(createTable()));
            }
        }
    }

    m_analysisParameterChooser = new analysisParametersChoice(m_appWindow);
    //QSignalMapper *signalMapper = new QSignalMapper(this);
    //connect(signalMapper, SIGNAL(mapped(int)), this, SIGNAL(digitClicked(int)));

    // Place the analysis parameter chooser window in the dock area of the QtiPlot window
    QDockWidget* analysisParametersChoiceDock = new QDockWidget(tr("Parameters"), m_appWindow);
    analysisParametersChoiceDock->setObjectName("sta-analysis-choice-box");
    analysisParametersChoiceDock->setMinimumWidth(200);
    analysisParametersChoiceDock->setFloating(false);

    analysisParametersChoiceDock->setWidget(m_analysisParameterChooser);
    m_appWindow->addDockWidget(Qt::RightDockWidgetArea, analysisParametersChoiceDock);
    analysisParametersChoiceDock->setVisible(true);
}


/** Set the scenario and propagated scenario that will be analyzed.
  *
  * TODO: the object should hold a reference to the scenario and propagated scenario, or
  * else the main window needs to NULL them out when a new scenario is loaded or repropgated.
  */
void
QtiPlotMain::passTheSTAscenarioToQtiPlotMain(SpaceScenario* scenario, PropagatedScenario* propagatedScenario)
{
    m_analysisParameterChooser->loadScenario(scenario, propagatedScenario);
}
