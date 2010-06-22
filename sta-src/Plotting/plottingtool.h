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

#ifndef _PLOTTING_PLOTTINGTOOL_H_
#define _PLOTTING_PLOTTINGTOOL_H_

#include "ui_plottingtool.h"
#include <QComboBox>

namespace Qwt3D
{
    class SurfacePlot;
}
class PropagatedScenario;
class PlotVariable;
class SpaceObject;

class PlottingTool : public QDialog, public Ui_PlottingTool
{
Q_OBJECT

public:
    PlottingTool(QWidget* parent);
    ~PlottingTool();

    // Enum values must match with the page indices in the .ui file
    enum {
        Cartesian2D = 0,
        Cartesian3D = 1,
    };

    void setScenario(PropagatedScenario* scenario);

public slots:
    void setTitle(const QString& text);
    void setXAxisLabel(const QString& text);
    void setYAxisLabel(const QString& text);
    void setZAxisLabel(const QString& text);
    void updatePlotData();
    void setPlotType(int plotType);

private:
    void setPlotDimension(unsigned int dimension);
    void fillValueCombo(QComboBox& combo);
    void fillVehicleCombo(QComboBox& combo);
    void updatePlotData(double startTime, double endTime,
                        const SpaceObject* vehicle,
                        const PlotVariable& x, const PlotVariable& y, const PlotVariable& z);

private:
    Qwt3D::SurfacePlot* m_plot;
    PropagatedScenario* m_scenario;
    int m_plotType;
};

#endif // _PLOTTING_PLOTTINGTOOL_H_
