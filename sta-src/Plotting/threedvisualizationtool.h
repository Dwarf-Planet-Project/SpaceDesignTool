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

class CelestiaCore;

class QResizeEvent;

class CelestiaGlWidget;
class CelestiaInterface;
class StaBody;

// Celestia classes
class Star;
class Body;


class ThreeDVisualizationTool : public QWidget
{
Q_OBJECT

public:
    ThreeDVisualizationTool(QWidget* parent,
                            CelestiaGlWidget* celestiaView,
                            CelestiaInterface* celestia,
                            CelestiaCore* celestiaCore);
    ~ThreeDVisualizationTool();

public slots:
    void gotoBody(const StaBody* body);
    void showGrid(bool enabled);
    void setTickInterval(double seconds);
    void saveImage();

protected:
    void resizeEvent(QResizeEvent* event);

private:
    Body* findBody(const StaBody* body);

private:
    CelestiaGlWidget* m_celestiaView;
    CelestiaInterface* m_celestia;

    // TODO: Only a CelestiaInterface object should be required. But first,
    // the CelestiaInterface needs to be expanded with wrappers for more
    // Celestia features.
    CelestiaCore* m_celestiaCore;

    // Celestia objects representing the Sun and Solar System Barycenter
    Star* m_sun;
    Star* m_ssb;
};

#endif // _THREED_VISUALIZATION_TOOL_H_
