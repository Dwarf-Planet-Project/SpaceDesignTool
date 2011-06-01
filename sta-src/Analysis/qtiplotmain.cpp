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
QtiPlotMain::initQtiPlot()
{
    m_appWindow = new ApplicationWindow(false);
    m_appWindow->restoreApplicationGeometry();
    connect(m_appWindow, SIGNAL(destroyed()), this, SLOT(qtiClosed()));

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


AnalysisResult::AnalysisResult(unsigned int columnCount)
{
    Q_ASSERT(columnCount > 0);
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
QtiPlotMain::passTheSTAscenarioToQtiPlotMain(SpaceScenario* scenario, PropagatedScenario* propagatedScenario)
{
    m_analysisParameterChooser->loadTheSTAscenario(scenario, propagatedScenario);
}

