/*
 This program is free software; you can redistribute it and/or modify it under
 the terms of the GNU Lesser General Public License as published by the Free Software
 Foundation; either version 2 of the License, or (at your option) any later
 version.

 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more
 details.

 You should have received a copy of the GNU Lesser General Public License along with
 this program; if not, write to the Free Software Foundation, Inc., 59 Temple
 Place - Suite 330, Boston, MA 02111-1307, USA.
 Further information about the GNU Lesser General Public License can also be found on
 the world wide web at http://www.gnu.org.
 */

/*
 ------ Copyright (C) 2009 European Space Agency (space.trajectory.analysis AT gmail.com) ----
 ------------------ Author: Chris Laurel  -------------------------------------------------
 ------------------ E-mail: (claurel@gmail.com) ----------------------------
 */

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

