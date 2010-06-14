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
#include "visualizationtoolbar.h"
#include "Astro-Core/stabody.h"
#include "celestia/qt/qtglwidget.h"
#include "celengine/planetgrid.h"
#include <QBoxLayout>
#include <QFileDialog>
#include <QMessageBox>

// TODO: CelestiaInterface should be moved from Main to Plotting module
#include "Main/celestiainterface.h"

ThreeDVisualizationTool::ThreeDVisualizationTool(QWidget* parent,
                                                 CelestiaGlWidget* celestiaView,
                                                 CelestiaInterface* celestia,
                                                 CelestiaCore* celestiaCore) :
    QWidget(parent),
    m_celestiaView(celestiaView),
    m_celestia(celestia),
    m_celestiaCore(celestiaCore),
    m_sun(NULL),
    m_ssb(NULL)
{
    // Create and hook up the tool bar
    VisualizationToolBar* toolBar = new VisualizationToolBar(tr("3D View Controls"), this);
    connect(toolBar, SIGNAL(bodyChanged(const StaBody*)),  this, SLOT(gotoBody(const StaBody*)));
    connect(toolBar, SIGNAL(gridToggled(bool)),            this, SLOT(showGrid(bool)));
    connect(toolBar, SIGNAL(tickIntervalChanged(double)),  this, SLOT(setTickInterval(double)));
    //connect(toolBar, SIGNAL(projectionChanged(bool)),      m_view, SLOT(set2HalfDView(bool)));
    connect(toolBar, SIGNAL(saveImageRequested()),         this, SLOT(saveImage()));

    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->setSpacing(1);
    layout->setContentsMargins(3, 3, 3, 3);
    layout->addWidget(m_celestiaView);
    layout->addWidget(toolBar);
    setLayout(layout);

    Selection sun = celestiaCore->getSimulation()->getUniverse()->find("Sol");
    m_sun = sun.star();
    Selection ssb = celestiaCore->getSimulation()->getUniverse()->find("SSB");
    m_ssb = ssb.star();

    // Sync 3D view state with toolbar settings
    //showGrid(true);
    showGrid(false);

    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
}


ThreeDVisualizationTool::~ThreeDVisualizationTool()
{
}


double kmToCelUnits(double km)
{
    return astro::kilometersToLightYears(km);
}


void
ThreeDVisualizationTool::gotoBody(const StaBody* body)
{
    if (!body)
        return;

    Body* celBody = findBody(body);
    if (!celBody)
        return;

    Simulation* simulation = m_celestiaCore->getSimulation();
    simulation->setSelection(Selection(celBody));
    simulation->follow();
    simulation->gotoSelection(0.0,
                              kmToCelUnits(celBody->getRadius() * 7.0),
                              Vec3f(0.0f, 1.0f, 0.0f),
                              ObserverFrame::Equatorial);
    m_celestiaCore->setViewChanged();
}


void
ThreeDVisualizationTool::showGrid(bool enabled)
{
    foreach (const StaBody* body, STA_SOLAR_SYSTEM->majorBodies())
    {
        Body* celBody = findBody(body);
        if (celBody)
        {
            if (enabled)
            {
                PlanetographicGrid* grid = new PlanetographicGrid(*celBody);
                grid->setTag("longlatgrid");
                celBody->addReferenceMark(grid);
            }
            else
            {
                celBody->removeReferenceMark("longlatgrid");
            }
        }
    }
    m_celestiaCore->setViewChanged();
}


void
ThreeDVisualizationTool::setTickInterval(double /* seconds */)
{
}


void
ThreeDVisualizationTool::saveImage()
{
    QImage image = m_celestiaView->grabFrameBuffer();
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


Body*
ThreeDVisualizationTool::findBody(const StaBody* body)
{
    string cname = body->name().toUtf8().data();

    Selection searchContexts[2] = { Selection(m_sun), Selection(m_ssb) };
    Selection object = m_celestiaCore->getSimulation()->getUniverse()->find(cname, searchContexts, 2);

    return object.body();
}


void
ThreeDVisualizationTool::resizeEvent(QResizeEvent* /* ev */)
{
    this->m_celestiaCore->setViewChanged();
}
