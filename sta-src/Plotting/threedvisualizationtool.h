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

#ifndef _THREED_VISUALIZATION_TOOL_H_
#define _THREED_VISUALIZATION_TOOL_H_

#include <QWidget>

class ThreeDView;
class StaBody;


/** ThreeDVisualizationTool is a widget containing a 3D view along with
  * controls specific for navigating and controlling the presentation of
  * the 3D view.
  */
class ThreeDVisualizationTool : public QWidget
{
Q_OBJECT

public:
    ThreeDVisualizationTool(QWidget* parent);
    ~ThreeDVisualizationTool();

public slots:
    void gotoBody(const StaBody* body);
    void showGrid(bool enabled);
    void setTickInterval(double seconds);
    void saveImage();

    ThreeDView* view() const
    {
        return m_view;
    }

protected:
    void resizeEvent(QResizeEvent* event);

private:
    ThreeDView* m_view;
};

#endif // _THREED_VISUALIZATION_TOOL_H_
