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

#ifndef _PLOTTING_VISUALIZATION_TOOLBAR_H_
#define _PLOTTING_VISUALIZATION_TOOLBAR_H_

#include "Main/ViewActionGroup.h"

#include <QToolBar>
#include <QComboBox>
#include <QStringList>

// Analysis (Claas Grohnfeldt, Steffen Peter)
#include <QMenu>
#include "Constellations/canalysis.h"


class StaBody;
class PropagatedScenario;

// The visualization toolbar contains a list of buttons used for both the ground track
// and 3D views.
class VisualizationToolBar : public QToolBar
{
    Q_OBJECT

public:
    VisualizationToolBar(const QString& title, QWidget* parent = 0);
    ~VisualizationToolBar();

   void configureFor3DView(ViewActionGroup* viewActions);
   void configureFor2DView(ViewActionGroup* viewActions);

   void configureCameraMenu(const PropagatedScenario* scenario);

    // Analysis (Claas Grohnfeldt, Steffen Peter)
   void enableAnalysisTools(ConstellationAnalysis* analysisOfConstellations);
   void disableAnalysisTools();

   QAction* gridAction() const
   {
       return m_gridAction;
   }

   QAction* equatorAction() const
   {
       return m_equatorAction;
   }

private slots:
    void mapSetTickInterval();
    void mapBodyChanged(QString bodyName);
    void setCameraViewpoint(QAction* action);
    void setTrajectoryPortion(QAction* action);

signals:
    void bodyChanged(const StaBody* body);
    void gridToggled(bool enabled);
    void equatorToggled(bool enabled);
    void terminatorToggled(bool enabled);
    void projectionChanged(bool enable25DView);
    void saveImageRequested();
    void tickIntervalChanged(double seconds);
    void sensorFovsToggled(bool enabled);

    // Analysis (Claas Grohnfeldt, Steffen Peter)
    void discretizationToggled(bool enable);
    void coverageCurrentToggled(bool enable);
    void coverageHistoryToggled(bool enable);
    void linkSOToggled(bool enabled);
    void linkGOToggled(bool enabled);

    void inertialObserver();
    void bodyFixedObserver();

    void cameraViewpointChanged(const QString&);
    void visibleTrajectoryPortionChanged(double days);

private:
    QAction* createCameraMenuAction(const QString& label, const QString& iconName, const QString& name);
    void addViewSelectActions(QActionGroup* actionGroup);

private:
    QComboBox* m_bodySelectCombo;
    QAction* m_tickIntervalAction;
    QAction* m_gridAction;
    QAction* m_equatorAction;
    QAction* m_terminatorAction;
    QAction* m_sensorFovsAction;
    QAction* m_saveImageAction;

    QAction* m_enable2DViewAction;
    QAction* m_enable25DViewAction;

    QAction* m_observerFrameAction;

    // Analysis (Claas Grohnfeldt, Steffen Peter)
    QMenu* m_analysisMenu;
    QAction* m_discretizationAction;
    QAction* m_coverageCurrentAction;
    QAction* m_coverageHistoryAction;
    QAction* m_analysisAction;
    QAction* m_linkSOAction;
    QAction* m_linkGOAction;

    QAction* m_cameraAction;
    QAction* m_trajectoryVisiblePortionAction;
};

#endif // _PLOTTING_VISUALIZATION_TOOLBAR_H_
