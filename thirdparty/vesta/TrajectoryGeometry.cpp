/*
 * $Revision: 400 $ $Date: 2010-08-02 17:39:12 -0700 (Mon, 02 Aug 2010) $
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
#include <curveplot/curveplot.h>
#include <Eigen/LU>
#include <algorithm>

using namespace vesta;
using namespace Eigen;


TrajectoryGeometry::TrajectoryGeometry() :
    m_color(Spectrum(1.0f, 1.0f, 1.0f)),
    m_opacity(1.0f),
    m_curvePlot(0),
    m_startTime(0.0),
    m_endTime(0.0),
    m_boundingRadius(0.0),
    m_displayedPortion(Entire)
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
TrajectoryGeometry::render(RenderContext& rc, float /* cameraDistance */, double animationTime) const
{
    if (!m_curvePlot)
    {
        return;
    }

    rc.setVertexInfo(VertexSpec::Position);

    const RenderContext::Frustum& frustum = rc.frustum();

    // Get a high precision modelview matrix; the full transformation is stored at single precision,
    // but the camera space position is stored at double precision.
    Transform3d modelview = rc.modelview().cast<double>();
    Vector3d t = rc.modelTranslation();
    modelview.matrix().col(3) = Vector4d(t.x(), t.y(), t.z(), 1.0);

    Material material;
    material.setEmission(m_color);
    rc.bindMaterial(&material);

    // Set the model view matrix to identity, as the curveplot module performs all transformations in
    // software using double precision.
    rc.pushModelView();
    rc.identityModelView();

    double startTime = m_startTime;
    double endTime = m_endTime;
    switch (m_displayedPortion)
    {
    case StartToCurrentTime:
        endTime = animationTime;
        break;
    case CurrentTimeToEnd:
        startTime = animationTime;
        break;
    default:
        break;
    }

    m_curvePlot->render(modelview,
                        -frustum.nearZ, -frustum.farZ, frustum.planeNormals, // viewFrustum
                        rc.pixelSize() * 30.0,                               // subdivisionThreshold
                        startTime, endTime);

    rc.popModelView();
}


void
TrajectoryGeometry::computeSamples(const Trajectory* trajectory, double startTime, double endTime, unsigned int steps)
{
    // Abort if we're asked to plot a null trajectory
    if (!trajectory)
    {
        return;
    }

    // Throw out the previous trajectory
    if (m_curvePlot)
    {
        delete m_curvePlot;
    }

    m_boundingRadius = 0.0;

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
        StateVector state = trajectory->state(t);

        CurvePlotSample sample;
        sample.t = t;
        sample.position = state.position();
        sample.velocity = state.velocity();
        m_curvePlot->addSample(sample);

        m_boundingRadius = std::max(m_boundingRadius, state.position().norm());
    }

    // Adjust the bounding radius slightly to prevent culling when the
    // trajectory lies barely inside the view frustum.
    m_boundingRadius *= 1.1;
}

