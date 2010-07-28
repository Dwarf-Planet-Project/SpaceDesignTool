/*
 * $Revision: 223 $ $Date: 2010-03-30 05:44:44 -0700 (Tue, 30 Mar 2010) $
 *
 * Copyright by Astos Solutions GmbH, Germany
 *
 * this file is published under the Astos Solutions Free Public License
 * For details on copyright and terms of use see 
 * http://www.astos.de/Astos_Solutions_Free_Public_License.html
 */

#ifndef _VESTA_TRAJECTORY_GEOMETRY_H_
#define _VESTA_TRAJECTORY_GEOMETRY_H_

#include <Eigen/Core>
#include "Geometry.h"
#include "Spectrum.h"


class CurvePlot;

namespace vesta
{

class Trajectory;

/** TrajectoryGeometry is used for visualizing the paths of bodies through
  * space.
  */
class TrajectoryGeometry : public Geometry
{
public:
    EIGEN_MAKE_ALIGNED_OPERATOR_NEW

    TrajectoryGeometry();
    virtual ~TrajectoryGeometry();

    void render(RenderContext& rc,
                float cameraDistance,
                double animationClock) const;

    float boundingSphereRadius() const
    {
        return m_boundingRadius;
    }

    virtual bool isOpaque() const
    {
        return m_opacity >= 1.0f;
    }

    Spectrum color() const
    {
        return m_color;
    }

    void setColor(const Spectrum& color)
    {
        m_color = color;
    }

    /** Get the opacity (0 = completely transparent, 1 = opaque) of
      * the arrow geometry.
      */
    float opacity() const
    {
        return m_opacity;
    }

    /** Set opacity of the arrows (0 = completely transparent, 1 = opaque).
      */
    void setOpacity(float opacity)
    {
        m_opacity = opacity;
    }

    void computeSamples(const Trajectory* trajectory, double startTime, double endTime, unsigned int steps);

private:
    Spectrum m_color;
    float m_opacity;
    CurvePlot* m_curvePlot;
    double m_startTime;
    double m_endTime;
    double m_boundingRadius;
};

}

#endif // _VESTA_TRAJECTORY_GEOMETRY_H_

