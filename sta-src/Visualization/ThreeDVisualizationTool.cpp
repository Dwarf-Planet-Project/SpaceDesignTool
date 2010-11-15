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
 Patched by Guillermo (Nov 09) to un-grid planets by default
 Patched by Guillermo August 2010 to connect the proper equator grid button
 */

#include "ThreeDVisualizationTool.h"
#include "ThreeDView.h"
#include "VisualizationToolBar.h"
#include "Main/ViewActionGroup.h"
#include "Astro-Core/stabody.h"
#include <QBoxLayout>
#include <QFileDialog>
#include <QMessageBox>
#include <QDesktopServices>


ThreeDVisualizationTool::ThreeDVisualizationTool(QWidget* parent, ViewActionGroup* viewActions) :
    QWidget(parent),
    m_view(NULL),
    m_toolBar(NULL)
{
    QGLFormat format;
    format = QGLFormat::defaultFormat();
    format.setDepth(true);
    format.setDepthBufferSize(24);
    format.setSampleBuffers(true);
    format.setSamples(4);
    format.setSwapInterval(1);
    m_view = new ThreeDView(format, this);

    m_toolBar = new VisualizationToolBar(tr("3D View Controls"), this);
    m_toolBar->configureFor3DView(viewActions);
    connect(m_toolBar, SIGNAL(bodyChanged(const StaBody*)),  this,    SLOT(gotoBody(const StaBody*)));
    connect(m_toolBar, SIGNAL(gridToggled(bool)),            m_view,  SLOT(setPlanetGrid(bool)));
    connect(m_toolBar, SIGNAL(equatorToggled(bool)),         m_view,  SLOT(setEquatorialPlane(bool)));
    connect(m_toolBar, SIGNAL(tickIntervalChanged(double)),  this,    SLOT(setTickInterval(double)));
    connect(m_toolBar, SIGNAL(saveImageRequested()),         this,    SLOT(saveImage()));
    connect(m_toolBar, SIGNAL(cameraViewpointChanged(QString)), m_view, SLOT(setCameraViewpoint(const QString&)));
    connect(m_toolBar, SIGNAL(sensorFovsToggled(bool)),      m_view,  SLOT(setSensorFovs(bool)));
    connect(m_toolBar, SIGNAL(inertialObserver()),           m_view,  SLOT(setInertialObserver()));
    connect(m_toolBar, SIGNAL(bodyFixedObserver()),          m_view,  SLOT(setBodyFixedObserver()));

    connect(m_toolBar, SIGNAL(visibleTrajectoryPortionChanged(double)), m_view, SLOT(setVisibleTrajectoryPortion(double)));

    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->addWidget(m_view);
    layout->addWidget(m_toolBar);
    setLayout(layout);

    layout->setSpacing(0);
    layout->setContentsMargins(0, 0, 0, 0);
    setToolBarVisible(true);

    // Sync 3D view state with toolbar settings
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
}


ThreeDVisualizationTool::~ThreeDVisualizationTool()
{
}


void
ThreeDVisualizationTool::gotoBody(const StaBody* body)
{
    m_view->gotoBody(body);
}


void
ThreeDVisualizationTool::showGrid(bool /* enabled */)
{
}


void
ThreeDVisualizationTool::setTickInterval(double /* seconds */)
{
}


void
ThreeDVisualizationTool::saveImage()
{
    QImage image = m_view->grabFrameBuffer(false);
    QString defaultDir = QDesktopServices::storageLocation(QDesktopServices::PicturesLocation);
    QString fileName = QFileDialog::getSaveFileName(this,
                                                    tr("Save Image"),
                                                    defaultDir + "/sta.png",
                                                    tr("Images (*.png *.jpg *.tif)"));
    if (!fileName.isEmpty())
    {
        bool ok = image.save(fileName);
        if (!ok)
        {
            QMessageBox::warning(this, tr("Save error"), tr("Error saving image to %1").arg(fileName));
        }
    }
}


void
ThreeDVisualizationTool::resizeEvent(QResizeEvent* /* ev */)
{
}


void
ThreeDVisualizationTool::setToolBarVisible(bool show)
{
    m_toolBar->setVisible(show);
}
