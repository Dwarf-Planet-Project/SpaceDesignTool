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
  Patched by Guillermo on June 2010 to include the constellations module
 */

#ifndef _STA_VISUALIZATION_GROUND_TRACK_PLOT_TOOL_H_
#define _STA_VISUALIZATION_GROUND_TRACK_PLOT_TOOL_H_

#include "GroundTrackView.h"
#include "Astro-Core/stabody.h"

#include <QWidget>
#include <QAbstractScrollArea>
#include <QGraphicsView>
#include <Eigen/Geometry>

#include "VisualizationToolBar.h"


/** GroundTrackPlotTool contains a GroundTrackView widget and a VisualizationToolBar.
  */
class GroundTrackPlotTool : public QWidget
{
Q_OBJECT
public:
    GroundTrackPlotTool(QWidget* parent, ViewActionGroup* viewActions);
    ~GroundTrackPlotTool();

    GroundTrackView* view() const { return m_view; }

    void setAnalysis(ConstellationStudy* studyOfConstellations);

    VisualizationToolBar* toolBar() const
    {
        return m_toolBar;
    }
    
private:
    GroundTrackView* m_view;
    VisualizationToolBar* m_toolBar;
};

#endif // _STA_VISUALIZATION_GROUND_TRACK_PLOT_TOOL_H_
