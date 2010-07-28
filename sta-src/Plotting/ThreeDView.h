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

#ifndef _THREED_VIEW_H_
#define _THREED_VIEW_H_

#include <QGLWidget>

namespace vesta
{
    class Universe;
    class UniverseRenderer;
}

/** ThreeDView is a widget with a rendered 3D view of the current space scenario.
  */
class ThreeDView : public QGLWidget
{
public:
    Q_OBJECT

public:
    ThreeDView(QWidget *parent = 0);
    ~ThreeDView();

   QSize minimumSizeHint() const;
   QSize sizeHint() const;

protected:
    void initializeGL();
    void paintGL();
    void resizeGL(int width, int height);
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent* event);
    void mouseMoveEvent(QMouseEvent *event);
    void wheelEvent(QWheelEvent* event);
    void keyPressEvent(QKeyEvent* event);
    void keyReleaseEvent(QKeyEvent* event);

private:
    vesta::Universe* m_universe;
    vesta::UniverseRenderer* m_renderer;
    double m_currentTime;
};

#endif // _THREED_VIEW_H_
