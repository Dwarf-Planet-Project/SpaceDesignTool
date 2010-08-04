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
 */

#include "threedvisualizationtool.h"
#include "ThreeDView.h"
#include "visualizationtoolbar.h"
#include "Astro-Core/stabody.h"
#include <QBoxLayout>
#include <QFileDialog>
#include <QMessageBox>


ThreeDVisualizationTool::ThreeDVisualizationTool(QWidget* parent) :
    QWidget(parent),
    m_view(NULL)
{
    QGLFormat format;
    format = QGLFormat::defaultFormat();
    format.setDepth(true);
    format.setDepthBufferSize(24);
    format.setSampleBuffers(true);
    format.setSamples(4);
    format.setSwapInterval(1);
    m_view = new ThreeDView(format, this);

    VisualizationToolBar* toolBar = new VisualizationToolBar(tr("3D View Controls"), this);
    connect(toolBar, SIGNAL(bodyChanged(const StaBody*)),  this, SLOT(gotoBody(const StaBody*)));
    connect(toolBar, SIGNAL(gridToggled(bool)),            m_view, SLOT(setEquatorialPlane(bool)));
    connect(toolBar, SIGNAL(tickIntervalChanged(double)),  this, SLOT(setTickInterval(double)));
    //connect(toolBar, SIGNAL(projectionChanged(bool)),      m_view, SLOT(set2HalfDView(bool)));
    connect(toolBar, SIGNAL(saveImageRequested()),         this, SLOT(saveImage()));

    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->setSpacing(1);
    layout->setContentsMargins(3, 3, 3, 3);
    layout->addWidget(m_view);
    layout->addWidget(toolBar);
    setLayout(layout);

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
    QString fileName = QFileDialog::getSaveFileName(this,
                                                    tr("Save Image"),
                                                    "",
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
