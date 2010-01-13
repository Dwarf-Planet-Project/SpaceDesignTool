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
 Patched by Guillermo as to allow smaller resolution PC to display this window. November 2009
 */

#include "plottingtool.h"
#include "Main/propagatedscenario.h"
#include "Astro-Core/cartesianTOorbital.h"
#include <qwt3d_surfaceplot.h>
#include <Eigen/Geometry>
#include <QHash>
#include <QDebug>
#include <algorithm>

using namespace Eigen;
using namespace Qwt3D;


class PlotVariable
{
public:
    enum VariableIndex
    {
        JulianDay,
        ElapsedTime,
        PositionX,
        PositionY,
        PositionZ,
        VelocityX,
        VelocityY,
        VelocityZ,
        SemimajorAxis,
        Eccentricity,
        Inclination,
        RAAN,
        ArgOfPeriapsis,
        MeanAnomaly,
        Altitude,
        Zero
    };

    enum Quantity
    {
        Time,
        Distance,
        Length,
        Velocity,
        Angle,
        Dimensionless,
    };

    PlotVariable() :
        m_name("Unknown"),
        m_index(Zero),
        m_quantity(Dimensionless)
    {
    }

    PlotVariable(const QString name, VariableIndex index, Quantity quantity) :
        m_name(name),
        m_index(index),
        m_quantity(quantity)
    {
    }

    QString name() const
    {
        return m_name;
    }

    VariableIndex index() const
    {
        return m_index;
    }

    Quantity quantity() const
    {
        return m_quantity;
    }

    double value(const SpaceObject* vehicle, double t) const;

private:
    QString m_name;
    VariableIndex m_index;
    Quantity m_quantity;
};

class PlotVariableTable
{
public:
    PlotVariableTable();

    QList<PlotVariable> list;
    QHash<int, PlotVariable> byIndex;
};

PlotVariableTable::PlotVariableTable()
{
    list << PlotVariable("Time", PlotVariable::JulianDay,                  PlotVariable::Time);
    list << PlotVariable("Position x", PlotVariable::PositionX,            PlotVariable::Distance);
    list << PlotVariable("Position y", PlotVariable::PositionY,            PlotVariable::Distance);
    list << PlotVariable("Position z", PlotVariable::PositionZ,            PlotVariable::Distance);
    list << PlotVariable("Velocity x", PlotVariable::VelocityX,            PlotVariable::Velocity);
    list << PlotVariable("Velocity y", PlotVariable::VelocityY,            PlotVariable::Velocity);
    list << PlotVariable("Velocity z", PlotVariable::VelocityZ,            PlotVariable::Velocity);
    list << PlotVariable("Altitude", PlotVariable::Altitude,               PlotVariable::Distance);
    list << PlotVariable("Semi-major axis", PlotVariable::SemimajorAxis,   PlotVariable::Distance);
    list << PlotVariable("Eccentricity", PlotVariable::Eccentricity,       PlotVariable::Dimensionless);
    list << PlotVariable("Inclination", PlotVariable::Inclination,         PlotVariable::Angle);
    list << PlotVariable("RAAN", PlotVariable::RAAN,                       PlotVariable::Angle);
    list << PlotVariable("Arg of periapsis", PlotVariable::ArgOfPeriapsis, PlotVariable::Angle);
    list << PlotVariable("Mean anomaly", PlotVariable::MeanAnomaly,        PlotVariable::Angle);

    foreach (PlotVariable variable, list)
    {
        byIndex[variable.index()] = variable;
    }
}

static PlotVariableTable PlotVariables;


double
PlotVariable::value(const SpaceObject* vehicle, double t) const
{
    // TODO: State vector is always computed in the frame of the first
    // mission segment. Need more flexibility than this.
    const StaBody* centralBody = vehicle->mission().at(0)->centralBody();
    sta::CoordinateSystem coordSys = vehicle->mission().at(0)->coordinateSystem();

    sta::StateVector state;
    bool valid = vehicle->getStateVector(t, *centralBody, coordSys, &state);
    if (!valid && index() != JulianDay)
        return 0.0;

    switch (index())
    {
        case JulianDay:
            return t;

        case PositionX:
            return state.position.x();

        case PositionY:
            return state.position.y();

        case PositionZ:
            return state.position.z();

        case VelocityX:
            return state.velocity.x();

        case VelocityY:
            return state.velocity.y();

        case VelocityZ:
            return state.velocity.z();

        case SemimajorAxis:
            return cartesianTOorbital(centralBody->mu(), state).SemimajorAxis;

        case Eccentricity:
            return cartesianTOorbital(centralBody->mu(), state).Eccentricity;

        case Inclination:
            return cartesianTOorbital(centralBody->mu(), state).Inclination;

        case RAAN:
            return cartesianTOorbital(centralBody->mu(), state).AscendingNode;

        case ArgOfPeriapsis:
            return cartesianTOorbital(centralBody->mu(), state).ArgumentOfPeriapsis;

        case MeanAnomaly:
            return cartesianTOorbital(centralBody->mu(), state).MeanAnomaly;

        case Altitude:
            // TODO: Calculate actual distance using distance to ellipsoid function.
            return state.position.norm() - centralBody->meanRadius();

        case Zero:
            return 0.0;

        default:
            return 0.0;
    }
}


PlottingTool::PlottingTool(QWidget* parent) :
    QDialog(parent),
    m_plot(NULL),
    m_scenario(NULL),
    m_plotType(Cartesian2D)
{
    setupUi(this);

    m_plot = new SurfacePlot();
    m_plot->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);

    connect(titleEdit,      SIGNAL(textChanged(QString)), this, SLOT(setTitle(QString)));
    connect(xAxisLabelEdit, SIGNAL(textChanged(QString)), this, SLOT(setXAxisLabel(QString)));
    connect(yAxisLabelEdit, SIGNAL(textChanged(QString)), this, SLOT(setYAxisLabel(QString)));
    connect(zAxisLabelEdit, SIGNAL(textChanged(QString)), this, SLOT(setZAxisLabel(QString)));

    unsigned int columnCount = 2;
    unsigned int rowCount = 250;
    Triple** data = new Triple*[columnCount];
    data[0] = new Triple[rowCount];
    data[1] = new Triple[rowCount];

    // Create some data
    for (unsigned int i = 0; i < rowCount; ++i)
    {
        double u = (double) i / (double) rowCount;
        double theta = u * 30.0;
        data[0][i] = Triple(cos(theta), sin(theta), u * 3);
        data[1][i] = data[0][i];
    }
    m_plot->loadFromData(data, columnCount, rowCount, false, false);

    // Set axis style
    for (unsigned int i = 0; i < m_plot->coordinates()->axes.size(); ++i)
    {
        // Set the major and minor tick intervales
        m_plot->coordinates()->axes[i].setMajors(7);
        m_plot->coordinates()->axes[i].setMinors(4);

        m_plot->coordinates()->axes[i].setLabelFont("Helvetica", 14);
        //m_plot->coordinates()->axes[i].setAutoScale(false);
    }

    m_plot->coordinates()->setGridLines(true, true, FLOOR);
    m_plot->coordinates()->setGridLinesColor(RGBA(0.7, 0.7, 0.7, 1.0));
    m_plot->setTitleFont("Helvetica", 18);
    setPlotDimension(2);
    setPlotType(Cartesian2D);

    titleEdit->setText("STA Example Plot");
    xAxisLabelEdit->setText("x-axis");
    yAxisLabelEdit->setText("y-axis");
    zAxisLabelEdit->setText("z-axis");

    m_plot->setMeshColor(RGBA(0.0, 0.0, 1.0, 1.0));

    m_plot->updateData();
    m_plot->updateGL();

    connect(vehicleCombo, SIGNAL(currentIndexChanged(int)), this, SLOT(updatePlotData()));
    connect(xValueCombo_2D, SIGNAL(currentIndexChanged(int)), this, SLOT(updatePlotData()));
    connect(yValueCombo_2D, SIGNAL(currentIndexChanged(int)), this, SLOT(updatePlotData()));
    connect(xValueCombo_3D, SIGNAL(currentIndexChanged(int)), this, SLOT(updatePlotData()));
    connect(yValueCombo_3D, SIGNAL(currentIndexChanged(int)), this, SLOT(updatePlotData()));
    connect(zValueCombo_3D, SIGNAL(currentIndexChanged(int)), this, SLOT(updatePlotData()));

    connect(variablesStack, SIGNAL(currentChanged(int)), this, SLOT(setPlotType(int)));

    layout()->addWidget(m_plot);
}


PlottingTool::~PlottingTool()
{
}


/** Set the scenario. It is legal for the scenario to be null. This
  * just indicates that vehicle combo boxes should be cleared and
  * disabled.
  */
void
PlottingTool::setScenario(PropagatedScenario* scenario)
{
    m_scenario = scenario;

    fillVehicleCombo(*vehicleCombo);

    fillValueCombo(*xValueCombo_2D);
    fillValueCombo(*yValueCombo_2D);
    fillValueCombo(*xValueCombo_3D);
    fillValueCombo(*yValueCombo_3D);
    fillValueCombo(*zValueCombo_3D);
}


void
PlottingTool::setTitle(const QString& text)
{
    m_plot->setTitle(text);
    m_plot->updateGL();
}


void
PlottingTool::setXAxisLabel(const QString& text)
{
    m_plot->coordinates()->axes[X1].setLabelString(text);
    m_plot->coordinates()->axes[X2].setLabelString(text);
    m_plot->coordinates()->axes[X3].setLabelString(text);
    m_plot->coordinates()->axes[X4].setLabelString(text);
    m_plot->updateGL();
}


void
PlottingTool::setYAxisLabel(const QString& text)
{
    m_plot->coordinates()->axes[Y1].setLabelString(text);
    m_plot->coordinates()->axes[Y2].setLabelString(text);
    m_plot->coordinates()->axes[Y3].setLabelString(text);
    m_plot->coordinates()->axes[Y4].setLabelString(text);
    m_plot->updateGL();
}


void
PlottingTool::setZAxisLabel(const QString& text)
{
    m_plot->coordinates()->axes[Z1].setLabelString(text);
    m_plot->coordinates()->axes[Z2].setLabelString(text);
    m_plot->coordinates()->axes[Z3].setLabelString(text);
    m_plot->coordinates()->axes[Z4].setLabelString(text);
    m_plot->updateGL();
}


void
PlottingTool::setPlotType(int plotType)
{
    if (m_plotType == plotType)
    {
        return;
    }

    m_plotType = plotType;
    if (variablesStack->currentIndex() != plotType)
    {
        variablesStack->setCurrentIndex(plotType);
    }

    // Ready for more plot types here: polar, cylindrical, etc.
    switch (plotType)
    {
    case Cartesian2D:
        setPlotDimension(2);
        break;
    case Cartesian3D:
        setPlotDimension(3);
        break;
    default:
        // Unknown plot type. Ignore it.
        break;
    }

    updatePlotData();
}


// Adjust settings particular to 2D or 3D plotting
void
PlottingTool::setPlotDimension(unsigned int dimension)
{
    if (dimension == 2)
    {
        m_plot->coordinates()->setAutoDecoration(false);
        m_plot->coordinates()->setStyle(FRAME_2D, X1, Y1, Z1);
        m_plot->setRotation(90, 0, 0);
        m_plot->disableMouse();
    }
    else if (dimension == 3)
    {
        m_plot->coordinates()->setAutoDecoration(true);
        m_plot->coordinates()->setStyle(FRAME);
        m_plot->setRotation(-60.0, 0.0, 30.0);
        m_plot->enableMouse();
    }
}


/** Update the graph data based on the settings of the variables. */
void
PlottingTool::updatePlotData()
{
    // If the vehicle combo boxes isn't set up yet, we can't plot.
    if (vehicleCombo->count() == 0)
    {
        return;
    }

    int vehicleIndex = vehicleCombo->itemData(vehicleCombo->currentIndex()).toInt();

    // All indexes must refer to a value vehicle
    if (vehicleIndex < 0 || vehicleIndex >= m_scenario->spaceObjects().size())
    {
        return;
    }

    // TODO: Allow user to specify a time range
    const SpaceObject* vehicle = m_scenario->spaceObjects().at(vehicleIndex);
    double startTime = vehicle->missionStartTime();
    double endTime   = vehicle->missionEndTime();

    if (variablesStack->currentIndex() == 0)
    {
        PlotVariable x = PlotVariables.byIndex[xValueCombo_2D->itemData(xValueCombo_2D->currentIndex()).toInt()];
        PlotVariable y = PlotVariables.byIndex[yValueCombo_2D->itemData(yValueCombo_2D->currentIndex()).toInt()];
        updatePlotData(startTime, endTime, vehicle, x, y, PlotVariable());
    }
    else if (variablesStack->currentIndex() == 1)
    {
        PlotVariable x = PlotVariables.byIndex[xValueCombo_3D->itemData(xValueCombo_3D->currentIndex()).toInt()];
        PlotVariable y = PlotVariables.byIndex[yValueCombo_3D->itemData(yValueCombo_3D->currentIndex()).toInt()];
        PlotVariable z = PlotVariables.byIndex[zValueCombo_3D->itemData(zValueCombo_3D->currentIndex()).toInt()];
        updatePlotData(startTime, endTime, vehicle, x, y, z);
    }
}


/** Helper function for updatePlotData() (the version with no arguments) that
  * actually does the work of capturing values from the propagated space
  * scenario.
  */
void
PlottingTool::updatePlotData(double startTime,
                             double endTime,
                             const SpaceObject* vehicle,
                             const PlotVariable& x,
                             const PlotVariable& y,
                             const PlotVariable& z)
{
    // Sample times are taken from the ephemeris of the primary object; for now,
    // we'll just use the vehicle defined for the x-axis.
    QVector<Triple> triples;

    AlignedBox<double, 3> bounds;

    foreach (MissionArc* arc, vehicle->mission())
    {
        if (endTime > arc->beginning() && startTime < arc->ending())
        {
            for (int i = 0; i < arc->trajectorySampleCount(); ++i)
            {
                double t = arc->trajectorySampleTime(i);
                if (t >= startTime && t <= endTime)
                {
                    Vector3d p(x.value(vehicle, t), y.value(vehicle, t), z.value(vehicle, t));
                    bounds.extend(p);
                    triples << Triple(p.x(), p.y(), p.z());
                }
            }
        }
    }

    // No data in the graph. It's likely that a bad time range was provided.
    if (triples.empty())
    {
        return;
    }

    Vector3d extents = bounds.max() - bounds.min();
    Vector3d axisMin = bounds.min();
    Vector3d axisMax = bounds.max();

    // Prevent divide-by-zero errors when one variable is constant
    Vector3d safeExtents = extents.cwise().max(Vector3d::Constant(precision<double>()));

    // Smart auto-scaling
    //
    //   The idea is to scale values into similar size ranges so that they can be meaningfully
    //   visualized. But, we don't want to distort the relationship between two values of the
    //   the same quantity. For example, if we rescaled position x and position y by different
    //   values, true geometry of an orbit would be distorted.
    //
    //   The decision to normalize an axis is based on what quantity (i.e. distance, angle, etc.)
    //   the variable measures. There are three cases:
    //
    //   * All three quantities different: normalize all axes
    //   * All three quantities the same: normalize no axes
    //   * Two variables of one quantity, third variable a different quantity: use the same
    //       factor to rescale the two variables of the same quantity into a domain
    //       of about the same size as the third variable.

    Vector3d translate;
    Vector3d scale;

    if (x.quantity() == y.quantity() && x.quantity() == z.quantity())
    {
        // All quantities identical: no rescaling
        translate = Vector3d::Zero();
        scale = Vector3d::Ones();
    }
    else if (x.quantity() != y.quantity() && x.quantity() != z.quantity() && y.quantity() != z.quantity())
    {
        // All quantities different: rescale everything to fit in a unit cube
        translate = -bounds.min();
        scale = safeExtents.cwise().inverse();
    }
    else
    {
        // Two quantities match. Scale the two identical variables by the same
        // factor.
        unsigned int unmatched;
        unsigned int matched1;
        unsigned int matched0;
        if (x.quantity() == y.quantity())
        {
            matched0 = 0;
            matched1 = 1;
            unmatched = 2;
        }
        else if (x.quantity() == z.quantity())
        {
            matched0 = 0;
            matched1 = 2;
            unmatched = 1;
        }
        else
        {
            matched0 = 1;
            matched1 = 2;
            unmatched = 0;
        }

        translate(unmatched) = -bounds.min()(unmatched);
        scale(unmatched) = 1.0 / safeExtents(unmatched);

        double minValue = std::min(bounds.min()(matched0), bounds.min()(matched1));
        double maxValue = std::max(bounds.max()(matched0), bounds.max()(matched1));
        double range = maxValue - minValue;
        translate(matched0) = translate(matched1) = -minValue;
        scale(matched0) = scale(matched1) = 1.0 / std::max(range, precision<double>());

        axisMin(matched0) = axisMin(matched1) = minValue;
        axisMax(matched0) = axisMax(matched1) = maxValue;
    }

    // Normalize the data to fit inside a unit cube
    for (unsigned int i = 0; i < triples.size(); ++i)
    {
        triples[i] = Triple((triples[i].x + translate.x()) * scale.x(),
                            (triples[i].y + translate.y()) * scale.y(),
                            (triples[i].z + translate.z()) * scale.z());
    }

    const Triple* data[2];
    data[0] = triples.constData();
    data[1] = triples.constData();

    // TODO: Ugly const cast here. Need to patch QwtPlot3D to take const data instead.
    m_plot->loadFromData(const_cast<Triple**>(data), 2, triples.size(), false, false);
    m_plot->updateData();

    // Set the scale on all the axes
    m_plot->coordinates()->axes[X1].setLimits(axisMin.x(), axisMax.x());
    m_plot->coordinates()->axes[X2].setLimits(axisMin.x(), axisMax.x());
    m_plot->coordinates()->axes[X3].setLimits(axisMin.x(), axisMax.x());
    m_plot->coordinates()->axes[X4].setLimits(axisMin.x(), axisMax.x());

    m_plot->coordinates()->axes[Y1].setLimits(axisMin.y(), axisMax.y());
    m_plot->coordinates()->axes[Y2].setLimits(axisMin.y(), axisMax.y());
    m_plot->coordinates()->axes[Y3].setLimits(axisMin.y(), axisMax.y());
    m_plot->coordinates()->axes[Y4].setLimits(axisMin.y(), axisMax.y());

    m_plot->coordinates()->axes[Z1].setLimits(axisMin.z(), axisMax.z());
    m_plot->coordinates()->axes[Z2].setLimits(axisMin.z(), axisMax.z());
    m_plot->coordinates()->axes[Z3].setLimits(axisMin.z(), axisMax.z());
    m_plot->coordinates()->axes[Z4].setLimits(axisMin.z(), axisMax.z());

    m_plot->updateGL();
}


void
PlottingTool::fillValueCombo(QComboBox& combo)
{
    combo.clear();
    foreach (PlotVariable variable, PlotVariables.list)
    {
        combo.addItem(variable.name(), (int) variable.index());
    }
}


void
PlottingTool::fillVehicleCombo(QComboBox& combo)
{
    combo.clear();
    if (m_scenario != NULL)
    {
        int vehicleIndex = 0;
        foreach (SpaceObject* obj, m_scenario->spaceObjects())
        {
            if (!obj->name().isEmpty())
            {
                combo.addItem(obj->name(), vehicleIndex);
            }
            vehicleIndex++;
        }
    }
}
