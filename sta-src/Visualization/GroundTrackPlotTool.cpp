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
 ------------------ Guillermo Ortega (ESA) ---------------------------------------
 Patched by Guillermo on June 2010 to include the code of Claas and Stephen
 Patched by Guillermo August 2010 to add the equqtor line
 */

#include "GroundTrackPlotTool.h"


/*** GroundTrackPlotTool implementation ***/

GroundTrackPlotTool::GroundTrackPlotTool(QWidget* parent, ViewActionGroup* viewActions) :
    QWidget(parent),
    m_view(NULL),
    m_toolBar(NULL)
{
    m_view = new GroundTrackView(this);

    // Create and hook up the tool bar
    m_toolBar = new VisualizationToolBar(tr("View Controls"), this);
    m_toolBar->configureFor2DView(viewActions);

    connect(m_toolBar, SIGNAL(bodyChanged(const StaBody*)),  m_view, SLOT(setBody(const StaBody*)));
    connect(m_toolBar, SIGNAL(gridToggled(bool)),            m_view, SLOT(setShowGrid(bool)));
    connect(m_toolBar, SIGNAL(equatorToggled(bool)),         m_view, SLOT(setShowEquator(bool)));
    connect(m_toolBar, SIGNAL(terminatorToggled(bool)),      m_view, SLOT(setTerminatorVisible(bool)));
    connect(m_toolBar, SIGNAL(tickIntervalChanged(double)),  m_view, SLOT(setTickInterval(double)));
    connect(m_toolBar, SIGNAL(projectionChanged(bool)),      m_view, SLOT(set2HalfDView(bool)));
    connect(m_toolBar, SIGNAL(saveImageRequested()),         m_view, SLOT(saveImage()));

    // Analysis Button signals (Claas Grohnfeldt, Steffen Peter)
    connect(m_toolBar, SIGNAL(discretizationToggled(bool)),  m_view, SLOT(setDiscretizationVisible(bool)));
    connect(m_toolBar, SIGNAL(coverageCurrentToggled(bool)), m_view, SLOT(setCoverageCurrentVisible(bool)));
    connect(m_toolBar, SIGNAL(coverageHistoryToggled(bool)), m_view, SLOT(setCoverageHistoryVisible(bool)));
    connect(m_toolBar, SIGNAL(linkSOToggled(bool)),          m_view, SLOT(setLinkSOVisible(bool)));
    connect(m_toolBar, SIGNAL(linkGOToggled(bool)),          m_view, SLOT(setLinkGOVisible(bool)));

    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->setSpacing(0);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(m_view);
    layout->addWidget(m_toolBar);
    setLayout(layout);

    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
}


GroundTrackPlotTool::~GroundTrackPlotTool()
{
}


// Analysis (Claas Grohnfeldt, Steffen Peter)
void GroundTrackPlotTool::setAnalysis(ConstellationAnalysis* analysisOfConstellations)
{
    m_toolBar->enableAnalysisTools(analysisOfConstellations);
    m_view->setAnalysis(analysisOfConstellations);
}




