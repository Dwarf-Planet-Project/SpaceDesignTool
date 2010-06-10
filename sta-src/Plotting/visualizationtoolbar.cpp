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
 ------ Copyright (C) 2009 European Space Agency (space.trajectory.analysis AT gmail.com) ----
 ------------------ Author: Chris Laurel  -------------------------------------------------
 ------------------ E-mail: (claurel@gmail.com) ----------------------------
 */

#include "visualizationtoolbar.h"
#include "Astro-Core/stabody.h"
#include <QAction>
#include <QMenu>


static double TickIntervals[] =
{
    0.0,
    10.0, 30.0,                        // < 1 minute
    60.0, 120.0, 300.0, 600.0, 1800.0, // < 1 hour
    3600.0, 7200.0
};


VisualizationToolBar::VisualizationToolBar(const QString& title, QWidget* parent) :
    QToolBar(title, parent)
{
    // Create the body selection combo box
    m_bodySelectCombo = new QComboBox(this);
    const QList<StaBody*>& majorBodies = STA_SOLAR_SYSTEM->majorBodies();
    foreach (const StaBody* body, majorBodies)
    {
        if (!body->baseTexture().isEmpty())
        {
            int SolarBody = body->id();
            QString MyIconWithPath;
            if (SolarBody == STA_EARTH) MyIconWithPath = ":/icons/ComboEarth.png";
            else if (SolarBody == STA_MERCURY) MyIconWithPath = ":/icons/ComboMercury.png";
            else if (SolarBody == STA_VENUS) MyIconWithPath = ":/icons/ComboVenus.png";
            else if (SolarBody == STA_MARS) MyIconWithPath = ":/icons/ComboMars.png";
            else if (SolarBody == STA_JUPITER) MyIconWithPath = ":/icons/ComboJupiter.png";
            else if (SolarBody == STA_SATURN) MyIconWithPath = ":/icons/ComboSaturn.png";
            else if (SolarBody == STA_URANUS) MyIconWithPath = ":/icons/ComboUranus.png";
            else if (SolarBody == STA_NEPTUNE) MyIconWithPath = ":/icons/ComboNeptune.png";
            else MyIconWithPath = ":/icons/ComboPluto.png";

            m_bodySelectCombo->addItem(QIcon(MyIconWithPath), body->name(), SolarBody);
        }
    }


    // Create the tick interval action
    m_tickIntervalAction = new QAction(QIcon(":/icons/IconCLOCK.png"), tr("Ticks"), this);
    QActionGroup* tickActionGroup = new QActionGroup(this);
    QMenu* tickMenu = new QMenu(tr("Ticks"), this);

    for (unsigned int i = 0; i < sizeof(TickIntervals) / sizeof(TickIntervals[0]); ++i)
    {
        double seconds = TickIntervals[i];

        QString label;
        if (seconds == 0.0)
        {
            label = tr("Disabled");
        }
        else if (seconds < 60.0)
        {
            label = tr("%1 seconds").arg(seconds);
        }
        else if (seconds < 3600.0)
        {
            label = tr("%1 minutes").arg(seconds / 60.0);
        }
        else
        {
            label = tr("%1 hours").arg(seconds / 3600.0);
        }

        QAction* action = new QAction(label, tickMenu);
        action->setData(seconds);
        action->setCheckable(true);
        tickMenu->addAction(action);
        tickActionGroup->addAction(action);
        connect(action, SIGNAL(triggered()), this, SLOT(mapSetTickInterval()));
    }

    tickActionGroup->actions().at(0)->setChecked(true);
    m_tickIntervalAction->setMenu(tickMenu);
    m_tickIntervalAction->setToolTip(tr("Set tick interval"));

    m_gridAction = new QAction(QIcon(":/icons/IconGRID.png"), tr("Grid"), this);
    m_gridAction->setCheckable(true);
    m_gridAction->setToolTip(tr("Toggle long/lat grid"));

    m_enable25DViewAction = new QAction(QIcon(":/icons/Icon25D.png"), tr("2.5D View"), this);
    m_enable25DViewAction->setCheckable(true);
    m_enable25DViewAction->setToolTip(tr("Toggle 2.5D view"));

    m_terminatorAction = new QAction(QIcon(":/icons/IconSUN.png"), tr("T"), this);
    m_terminatorAction->setCheckable(true);
    m_terminatorAction->setToolTip(tr("Toggle terminator"));

    m_saveImageAction = new QAction(QIcon(":/icons/IconDOWNLOAD.png"), tr("Save"), this);
    m_saveImageAction->setToolTip(tr("Save plot to a file"));

    // Analysis (Claas Grohnfeldt, Steffen Peter)
    m_discretizationAction = new QAction(tr("Show Discretization"), this);
    m_discretizationAction->setCheckable(true);
    m_discretizationAction->setToolTip(tr("Toggle discretization"));
    m_discretizationAction->setVisible(false);

    m_coverageCurrentAction = new QAction(tr("Show Current Coverage"), this);
    m_coverageCurrentAction->setCheckable(true);
    m_coverageCurrentAction->setToolTip(tr("Toggle current coverage"));
    m_coverageCurrentAction->setVisible(false);

    m_coverageHistoryAction = new QAction(tr("Show History Coverage"), this);
    m_coverageHistoryAction->setCheckable(true);
    m_coverageHistoryAction->setToolTip(tr("Toggle history coverage"));
    m_coverageHistoryAction->setVisible(false);

    m_linkSOAction = new QAction(tr("Show Links Sat-Sat"), this);
    m_linkSOAction->setCheckable(true);
    m_linkSOAction->setToolTip(tr("Show links between satellites"));
    m_linkSOAction->setVisible(false);

    m_linkGOAction = new QAction(tr("Show Links Sat-GS"), this);
    m_linkGOAction->setCheckable(true);
    m_linkGOAction->setToolTip(tr("Show links between satellites and ground stations"));
    m_linkGOAction->setVisible(false);

    // create Analysis Menu
    //QMenu* tickMenu = new QMenu(tr("Ticks"), this);
    //QMenu* m_analysisMenu = new QMenu(tr("Analysis Tools"), this);
    m_analysisMenu = new QMenu(this);
    m_analysisMenu->addAction(m_linkSOAction);
    m_analysisMenu->addAction(m_linkGOAction);
    m_analysisMenu->addAction(m_discretizationAction);
    m_analysisMenu->addAction(m_coverageCurrentAction);
    m_analysisMenu->addAction(m_coverageHistoryAction);
    m_analysisAction = new QAction(QIcon(":/icons/IconCONSTELLATION.png"), tr("Analysis Tools"), this);
    m_analysisAction->setMenu(m_analysisMenu);
    //m_analysisAction->setVisible(false); // hide button until scenario is propagated


    // Add all actions and widgets to the toolbar
    addWidget(m_bodySelectCombo);
    addAction(m_tickIntervalAction);
    addAction(m_gridAction);
    addAction(m_terminatorAction);
    addAction(m_enable25DViewAction);
    addAction(m_saveImageAction);

    // Guillermo on widget patching
    addAction(m_analysisAction);



    // Set the initial state of the actions and widgets
    m_enable25DViewAction->setChecked(false);
    // Next line patch by Guillermo to set to false initial grid
    //m_gridAction->setChecked(true);
    m_gridAction->setChecked(false);
    m_terminatorAction->setChecked(false);

    // Guillermo
    m_analysisAction->setVisible(false);



    connect(m_bodySelectCombo,     SIGNAL(currentIndexChanged(QString)), this, SLOT(mapBodyChanged(QString)));
    connect(m_gridAction,          SIGNAL(triggered(bool)),              this, SIGNAL(gridToggled(bool)));
    connect(m_terminatorAction,    SIGNAL(triggered(bool)),              this, SIGNAL(terminatorToggled(bool)));
    connect(m_enable25DViewAction, SIGNAL(toggled(bool)),                this, SIGNAL(projectionChanged(bool)));
    connect(m_saveImageAction,     SIGNAL(triggered()),                  this, SIGNAL(saveImageRequested()));

    // Analysis (Claas Grohnfeldt, Steffen Peter)
    m_discretizationAction->setChecked(false);
    connect(m_discretizationAction,SIGNAL(triggered(bool)),              this, SIGNAL(discretizationToggled(bool)));
    connect(m_coverageCurrentAction,      SIGNAL(triggered(bool)),              this, SIGNAL(coverageCurrentToggled(bool)));
    connect(m_coverageHistoryAction,      SIGNAL(triggered(bool)),              this, SIGNAL(coverageHistoryToggled(bool)));
    connect(m_linkSOAction,        SIGNAL(triggered(bool)),              this, SIGNAL(linkSOToggled(bool)));
    connect(m_linkGOAction,        SIGNAL(triggered(bool)),              this, SIGNAL(linkGOToggled(bool)));




}


VisualizationToolBar::~VisualizationToolBar()
{
}


// Private slot to translate a tick interval menu event into
// a signal with a double parameter.
void
VisualizationToolBar::mapSetTickInterval()
{
    QAction* action = qobject_cast<QAction*>(sender());
    if (action)
    {
        double interval = action->data().toDouble();
        emit tickIntervalChanged(interval);
    }
}


// Private slot to translate a body name into an STA Body
void
VisualizationToolBar::mapBodyChanged(QString bodyName)
{
    const StaBody* body = STA_SOLAR_SYSTEM->lookup(bodyName);
    if (body)
    {
        emit bodyChanged(body);
    }
}



// Analysis (Claas Grohnfeldt, Steffen Peter)
// Procedure to create and show the Analysis Toolbar
void
VisualizationToolBar::enableAnalysisTools(Analysis* analysis)
{
    // reset
    m_analysisAction->setVisible(false);
    m_linkSOAction->setVisible(false);
    m_linkGOAction->setVisible(false);
    m_discretizationAction->setVisible(false);
    m_coverageCurrentAction->setVisible(false);
    m_coverageHistoryAction->setVisible(false);
    m_analysisAction->setChecked(false);
    m_linkSOAction->setChecked(false);
    m_linkGOAction->setChecked(false);
    m_discretizationAction->setChecked(false);
    m_coverageCurrentAction->setChecked(false);
    m_coverageHistoryAction->setChecked(false);
    // set
    if (analysis != NULL)
    {
	if (!analysis->m_anaSpaceObjectList.at(0).linksamples.isEmpty()) // link SO
	{
	    m_linkSOAction->setVisible(true);
	    m_analysisAction->setVisible(true);
	}
	if (!analysis->m_anaSpaceObjectList.at(0).groundlinksamples.isEmpty()) // link GO
	{
	    m_linkGOAction->setVisible(true);
	    m_analysisAction->setVisible(true);
	}
	if(!analysis->m_discreteMesh->meshAsList.isEmpty()) // discretization
	{
	    m_discretizationAction->setVisible(true);
	    m_analysisAction->setVisible(true);
	}
	if(!analysis->m_anaSpaceObjectList.at(0).coveragesample.isEmpty()) // coverage
	{
	    m_coverageCurrentAction->setVisible(true);
	    m_coverageHistoryAction->setVisible(true);
	    m_analysisAction->setVisible(true);
	}
    }
}

void
VisualizationToolBar::disableAnalysisTools()
{
    m_analysisAction->setVisible(false);
}
