/*
 * $Revision: 226 $ $Date: 2010-04-01 22:58:10 -0700 (Thu, 01 Apr 2010) $
 *
 * Copyright by Astos Solutions GmbH, Germany
 *
 * this file is published under the Astos Solutions Free Public License
 * For details on copyright and terms of use see 
 * http://www.astos.de/Astos_Solutions_Free_Public_License.html
 */

#ifndef _VESTA_VISUALIZER_H_
#define _VESTA_VISUALIZER_H_

#include "Object.h"
#include "Geometry.h"
#include <Eigen/Geometry>
#include <string>


namespace vesta
{

class Entity;

/** A visualizer is extra geometry that represents something other than
  * the solid body of an object. Visualizers are attached to entities for
  * highlighting, labeling, showing regions of visibility, etc.
  */
class Visualizer : public Object
{
public:
    Visualizer(Geometry* geometry);
    virtual ~Visualizer();

    bool isVisible() const
    {
        return m_visible;
    }

    void setVisibility(bool visible)
    {
        m_visible = visible;
    }

    virtual Eigen::Quaterniond orientation(const Entity* parent, double t) const;

    /** Return the geometry for this visualizer.
      */
    Geometry* geometry() const
    {
        return m_geometry.ptr();
    }

    enum DepthAdjustment
    {
        NoAdjustment   = 0,
        AdjustToFront  = 1,
        AdjustToBack   = 2,
    };

    DepthAdjustment depthAdjustment() const
    {
        return m_depthAdjustment;
    }

    /** Set the depth adjustment for this visualizer. The depth adjustment
      * can be used to ensure that the visualizer will always appear either
      * in front of or behind the object that it is attached to.
      */
    void setDepthAdjustment(DepthAdjustment adjustment)
    {
        m_depthAdjustment = adjustment;
    }

    bool rayPick(const Eigen::Vector3d& pickOrigin,
                 const Eigen::Vector3d& pickDirection,
                 double pixelAngle) const;

protected:
    void setGeometry(Geometry* geometry)
    {
        m_geometry = geometry;
    }

    virtual bool handleRayPick(const Eigen::Vector3d& pickOrigin,
                               const Eigen::Vector3d& pickDirection,
                               double pixelAngle) const;

private:
    bool m_visible;
    counted_ptr<Geometry> m_geometry;
    DepthAdjustment m_depthAdjustment;
};

}

#endif // _VESTA_VISUALIZER_H_
