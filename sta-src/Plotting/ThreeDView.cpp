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
 */


#include "ThreeDView.h"
#include <vesta/Universe.h>
#include <vesta/UniverseRenderer.h>
#include <QWheelEvent>

using namespace vesta;
using namespace Eigen;


ThreeDView::ThreeDView(QWidget* parent) :
    m_universe(NULL),
    m_renderer(NULL)
{
    m_universe = new Universe();
    m_universe->addRef();
}


ThreeDView::~ThreeDView()
{
    if (m_universe)
    {
        m_universe->release();
    }

    delete m_renderer;
}


QSize
ThreeDView::minimumSizeHint() const
{
    return QSize(100, 100);
}


QSize
ThreeDView::sizeHint() const
{
    return QSize(800, 500);
}


void
ThreeDView::initializeGL()
{
    m_renderer = new UniverseRenderer();

    glClearColor(0.0f, 0.0f, 1.0f, 0.0f);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glEnable(GL_CULL_FACE);
}


void
ThreeDView::paintGL()
{
    glDepthMask(GL_TRUE);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}


void
ThreeDView::resizeGL(int width, int height)
{
    glViewport(0, 0, width, height);
}


void
ThreeDView::mousePressEvent(QMouseEvent *event)
{
}


void
ThreeDView::mouseReleaseEvent(QMouseEvent* event)
{
}


void
ThreeDView::mouseMoveEvent(QMouseEvent *event)
{
}


void
ThreeDView::wheelEvent(QWheelEvent* event)
{
    if (event->orientation() == Qt::Vertical)
    {
    }
}


void
ThreeDView::keyPressEvent(QKeyEvent* event)
{
}


void
ThreeDView::keyReleaseEvent(QKeyEvent* event)
{
}

