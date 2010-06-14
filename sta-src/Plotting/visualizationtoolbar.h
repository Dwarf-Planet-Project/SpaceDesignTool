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

#ifndef _PLOTTING_VISUALIZATION_TOOLBAR_H_
#define _PLOTTING_VISUALIZATION_TOOLBAR_H_

#include <QToolBar>
#include <QComboBox>

// Analysis (Claas Grohnfeldt, Steffen Peter)
#include <QMenu>
#include "Constellations/canalysis.h"


class StaBody;

// The visualization toolbar contains a list of buttons used for both the ground track
// and 3D views.
class VisualizationToolBar : public QToolBar
{
    Q_OBJECT

public:
    VisualizationToolBar(const QString& title, QWidget* parent = 0);
    ~VisualizationToolBar();

    // Analysis (Claas Grohnfeldt, Steffen Peter)
   void enableAnalysisTools(Analysis* analysis);
   void disableAnalysisTools();

private slots:
    void mapSetTickInterval();
    void mapBodyChanged(QString bodyName);

signals:
    void bodyChanged(const StaBody* body);
    void gridToggled(bool enabled);
    void terminatorToggled(bool enabled);
    void projectionChanged(bool enable25DView);
    void saveImageRequested();
    void tickIntervalChanged(double seconds);

    // Analysis (Claas Grohnfeldt, Steffen Peter)
    void discretizationToggled(bool enable);
    void coverageCurrentToggled(bool enable);
    void coverageHistoryToggled(bool enable);
    void linkSOToggled(bool enabled);
    void linkGOToggled(bool enabled);


private:
    QComboBox* m_bodySelectCombo;
    QAction* m_tickIntervalAction;
    QAction* m_gridAction;
    QAction* m_terminatorAction;
    QAction* m_saveImageAction;

    // Possibly move these 2D-specific buttons to a subclass
    QAction* m_enable2DViewAction;
    QAction* m_enable25DViewAction;

    // Analysis (Claas Grohnfeldt, Steffen Peter)
    QMenu* m_analysisMenu;
    QAction* m_discretizationAction;
    QAction* m_coverageCurrentAction;
    QAction* m_coverageHistoryAction;
    QAction* m_analysisAction;
    QAction* m_linkSOAction;
    QAction* m_linkGOAction;

};

#endif // _PLOTTING_VISUALIZATION_TOOLBAR_H_
