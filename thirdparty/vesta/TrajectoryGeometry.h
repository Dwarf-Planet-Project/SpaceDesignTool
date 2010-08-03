/*
 * $Revision: 400 $ $Date: 2010-08-02 17:39:12 -0700 (Mon, 02 Aug 2010) $
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

    enum TrajectoryPortion
    {
        Entire             = 0,
        StartToCurrentTime = 1,
        CurrentTimeToEnd   = 2,
    };

    /** Return the portion of the trajectory that will be displayed.
      */
    TrajectoryPortion displayedPortion() const
    {
        return m_displayedPortion;
    }

    /** Set the portion of the trajectory that will be displayed. There are three
      * options:
      *    Entire - show the complete trajectory from beginning to end
      *    StartToCurrentTime - show the trajectory from the first point through the current time
      *    CurrenTimeToEnd - show the trajectory from the current time through the end point
      */
    void setDisplayedPortion(TrajectoryPortion portion)
    {
        m_displayedPortion = portion;
    }

private:
    Spectrum m_color;
    float m_opacity;
    CurvePlot* m_curvePlot;
    double m_startTime;
    double m_endTime;
    double m_boundingRadius;
    TrajectoryPortion m_displayedPortion;
};

}

#endif // _VESTA_TRAJECTORY_GEOMETRY_H_

