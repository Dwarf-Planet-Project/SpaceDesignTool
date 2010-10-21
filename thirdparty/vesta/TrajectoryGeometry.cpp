/*
 * $Revision: 541 $ $Date: 2010-10-19 11:56:03 -0700 (Tue, 19 Oct 2010) $
 *
 * Copyright by Astos Solutions GmbH, Germany
 *
 * this file is published under the Astos Solutions Free Public License
 * For details on copyright and terms of use see 
 * http://www.astos.de/Astos_Solutions_Free_Public_License.html
 */

#include "TrajectoryGeometry.h"
#include "Trajectory.h"
#include "RenderContext.h"
#include "Material.h"
#include "Debug.h"
#include <curveplot/curveplot.h>
#include <Eigen/LU>
#include <algorithm>

using namespace vesta;
using namespace Eigen;
using namespace std;


TrajectoryGeometry::TrajectoryGeometry() :
    m_color(Spectrum(1.0f, 1.0f, 1.0f)),
    m_opacity(1.0f),
    m_curvePlot(0),
    m_startTime(0.0),
    m_endTime(0.0),
    m_boundingRadius(0.0),
    m_displayedPortion(Entire),
    m_windowDuration(0.0),
    m_fadeFraction(0.0)
{
    // Make trajectories splittable by default in order to prevent
    // clipping artifacts.
    setClippingPolicy(SplitToPreventClipping);
}


TrajectoryGeometry::~TrajectoryGeometry()
{
    delete m_curvePlot;
}


void
TrajectoryGeometry::render(RenderContext& rc, double clock) const
{
    if (!m_curvePlot)
    {
        return;
    }

    const Frustum& frustum = rc.frustum();

    // Get a high precision modelview matrix; the full transformation is stored at single precision,
    // but the camera space position is stored at double precision.
    Transform3d modelview = rc.modelview().cast<double>();
    Vector3d t = rc.modelTranslation();
    modelview.matrix().col(3) = Vector4d(t.x(), t.y(), t.z(), 1.0);

    if (m_frame.isValid())
    {
        modelview = modelview * m_frame->orientation(clock);
    }

    // Set the model view matrix to identity, as the curveplot module performs all transformations in
    // software using double precision.
    rc.pushModelView();
    rc.identityModelView();

    bool fade = false;
    double startTime = m_startTime;
    double endTime = m_endTime;
    switch (m_displayedPortion)
    {
    case StartToCurrentTime:
        endTime = clock;
        break;
    case CurrentTimeToEnd:
        startTime = clock;
        break;
    case WindowBeforeCurrentTime:
        endTime = clock;
        startTime = clock - m_windowDuration;
        fade = m_fadeFraction > 0.0;
        break;
    default:
        break;
    }

    double subdivisionThreshold = rc.pixelSize() * 30.0;
    if (fade)
    {
        rc.setVertexInfo(VertexSpec::PositionColor);
        Material material;
        material.setDiffuse(Spectrum::White());
        rc.bindMaterial(&material);

        double fadeStartTime = startTime;
        double fadeEndTime = fadeStartTime + m_windowDuration * m_fadeFraction;
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        m_curvePlot->renderFaded(modelview,
                                 -frustum.nearZ, -frustum.farZ, frustum.planeNormals, // viewFrustum
                                 subdivisionThreshold,
                                 startTime, endTime,
                                 Vector4f(m_color.red(), m_color.green(), m_color.blue(), 1.0f),
                                 fadeStartTime, fadeEndTime);
        glDisable(GL_BLEND);
    }
    else
    {
        rc.setVertexInfo(VertexSpec::Position);
        Material material;
        material.setEmission(m_color);
        rc.bindMaterial(&material);

        m_curvePlot->render(modelview,
                            -frustum.nearZ, -frustum.farZ, frustum.planeNormals, // viewFrustum
                            subdivisionThreshold,
                            startTime, endTime);
    }

    rc.popModelView();
}


/** Add a new sample to the trajectory. If this is not the first sample, the time t must
  * be greater than the time of the last added sample; if not, the sample is discarded.
  *
  * \param t time in seconds since J2000.0
  * \param s state vector
  */
void
TrajectoryGeometry::addSample(double t, const StateVector& s)
{
    if (!m_curvePlot)
    {
        m_curvePlot = new CurvePlot();
    }

    if (m_curvePlot->sampleCount() == 0)
    {
        m_startTime = t;
        m_endTime = t;
    }

    if (m_curvePlot->sampleCount() == 0 || t > m_curvePlot->endTime())
    {
        CurvePlotSample sample;
        sample.t = t;
        sample.position = s.position();
        sample.velocity = s.velocity();
        m_curvePlot->addSample(sample);

        m_boundingRadius = std::max(m_boundingRadius, s.position().norm());
        m_endTime = t;
    }
}


/** Remove all trajectory plot samples.
  */
void
TrajectoryGeometry::clearSamples()
{
    // Throw out the previous trajectory
    if (m_curvePlot)
    {
        delete m_curvePlot;
        m_curvePlot = NULL;
    }

    m_boundingRadius = 0.0;
    m_startTime = 0.0;
    m_endTime = 0.0;
}


class TrajectorySampleGenerator : public TrajectoryPlotGenerator
{
public:
    TrajectorySampleGenerator(const Trajectory* trajectory) :
        m_trajectory(trajectory)
    {
    }

    StateVector state(double t) const
    {
        return m_trajectory->state(t);
    }

    double startTime() const
    {
        return m_trajectory->startTime();
    }

    double endTime() const
    {
        return m_trajectory->endTime();
    }

private:
    const Trajectory* m_trajectory;
};


/** Automatically add samples to the trajectory plot. Samples of the specified trajectory
  * are calculated at regular intervals between the startTime and endTime. Any existing samples
  * in the trajectory plot are replaced.
  */
void
TrajectoryGeometry::computeSamples(const Trajectory* trajectory, double startTime, double endTime, unsigned int steps)
{
    // Abort if we're asked to plot a null trajectory
    if (!trajectory)
    {
        return;
    }

    TrajectorySampleGenerator gen(trajectory);
    computeSamples(&gen, startTime, endTime, steps);
}


/** Automatically add samples to the trajectory plot. Samples of the specified trajectory
  * are calculated at regular intervals between the startTime and endTime.
  */
void
TrajectoryGeometry::updateSamples(const Trajectory* trajectory, double startTime, double endTime, unsigned int steps)
{
    // Abort if we're asked to plot a null trajectory
    if (!trajectory)
    {
        return;
    }

    TrajectorySampleGenerator gen(trajectory);
    computeSamples(&gen, startTime, endTime, steps);
}


/** Automatically add samples to the trajectory plot. States from the specified generator
  * are calculated at regular intervals between the startTime and endTime. Any existing samples
  * in the trajectory plot are replaced.
  */
void
TrajectoryGeometry::computeSamples(const TrajectoryPlotGenerator* generator, double startTime, double endTime, unsigned int steps)
{
    // Abort if we're asked to use a null generator
    if (!generator)
    {
        return;
    }

    clearSamples();

    startTime = max(generator->startTime(), startTime);
    endTime = min(generator->endTime(), endTime);

    // Nothing to plot if end is before start
    if (endTime <= startTime)
    {
        return;
    }

    m_curvePlot = new CurvePlot();

    m_startTime = startTime;
    m_endTime = endTime;
    double dt = (endTime - startTime) / steps;

    for (unsigned int i = 0; i <= steps; ++i)
    {
        double t = m_startTime + i * dt;
        StateVector state = generator->state(t);
        addSample(t, state);
    }

    // Adjust the bounding radius slightly to prevent culling when the
    // trajectory lies barely inside the view frustum.
    m_boundingRadius *= 1.1;
}


/** Automatically add samples to the trajectory plot. Samples of the specified
  * generator are calculated at regular intervals between the startTime and endTime.
  */
void
TrajectoryGeometry::updateSamples(const TrajectoryPlotGenerator* generator, double startTime, double endTime, unsigned int steps)
{
    // Abort if we're asked to use a null generator
    if (!generator)
    {
        return;
    }

    if (!m_curvePlot)
    {
        // Trajectory hasn't been created yet; initialize it for the specified time range
        computeSamples(generator, startTime, endTime, steps);
        return;
    }

    double dt = (endTime - startTime) / (steps - 1);
    double windowStartTime = max(generator->startTime(), startTime - dt);
    double windowEndTime = min(generator->endTime(), endTime + dt);

    if (endTime <= m_curvePlot->startTime() || startTime >= m_curvePlot->endTime())
    {
        computeSamples(generator, windowStartTime, windowEndTime, steps);
    }
    else
    {
        if (startTime < m_curvePlot->startTime())
        {
            // Add samples at the beginning
            for (double t = m_curvePlot->startTime() - dt; t > windowStartTime; t -= dt)
            {
                t = max(t, windowStartTime);
                StateVector sv = generator->state(t);

                CurvePlotSample sample;
                sample.t = t;
                sample.position = sv.position();
                sample.velocity = sv.velocity();
                m_curvePlot->addSample(sample);
                m_boundingRadius = std::max(m_boundingRadius, sv.position().norm());
            }
        }

        if (endTime > m_curvePlot->endTime())
        {
            unsigned int sampCount = 0;
            // Add samples at the end
            for (double t = m_curvePlot->endTime() + dt; t < windowEndTime; t += dt)
            {
                t = min(t, windowEndTime);
                StateVector sv = generator->state(t);

                CurvePlotSample sample;
                sample.t = t;
                sample.position = sv.position();
                sample.velocity = sv.velocity();
                m_curvePlot->addSample(sample);
                m_boundingRadius = std::max(m_boundingRadius, sv.position().norm());
                ++sampCount;
            }
        }

        // Remove samples
        m_curvePlot->removeSamplesAfter(windowEndTime);
        m_curvePlot->removeSamplesBefore(windowStartTime);
    }

    m_startTime = windowStartTime;
    m_endTime = windowEndTime;
}
