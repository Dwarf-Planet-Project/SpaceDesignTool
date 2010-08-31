/*
 * $Revision: 475 $ $Date: 2010-08-31 08:09:34 -0700 (Tue, 31 Aug 2010) $
 *
 * Copyright by Astos Solutions GmbH, Germany
 *
 * this file is published under the Astos Solutions Free Public License
 * For details on copyright and terms of use see 
 * http://www.astos.de/Astos_Solutions_Free_Public_License.html
 */

#ifndef _VESTA_ARC_H_
#define _VESTA_ARC_H_

#include "Object.h"
#include <Eigen/Core>

namespace vesta
{

class Entity;
class Frame;
class Trajectory;
class RotationModel;

/** An Arc is one segment of a Chronology. Within an Arc,
 *  a single trajectory expresses translational motion relative
 *  to the center object and in the trajectory frame. Similarly,
 *  rotational motion is described by a rotation model object.
 *  The frame for rotational motion is given by the Arc's bodyFrame,
 *  which can be disctinct from the trajectory frame.
 */
class Arc : public Object
{
public:
    Arc();
    ~Arc();

    Entity* center() const
    {
        return m_center.ptr();
    }

    void setCenter(Entity* center);

    Frame* trajectoryFrame() const
    {
        return m_trajectoryFrame.ptr();
    }

    void setTrajectoryFrame(Frame* f);

    Frame* bodyFrame() const
    {
        return m_bodyFrame.ptr();
    }

    void setBodyFrame(Frame* f);

    Trajectory* trajectory() const
    {
        return m_trajectory.ptr();
    }

    void setTrajectory(Trajectory* trajectory);

    RotationModel* rotationModel() const
    {
        return m_rotationModel.ptr();
    }

    void setRotationModel(RotationModel* rm);

    double duration() const
    {
        return m_duration;
    }

    void setDuration(double t);

private:
    counted_ptr<Entity> m_center;
    counted_ptr<Frame> m_trajectoryFrame;
    counted_ptr<Frame> m_bodyFrame;
    counted_ptr<Trajectory> m_trajectory;
    counted_ptr<RotationModel> m_rotationModel;
    double m_duration;
};

} // namespace

#endif // _VESTA_ARC_H_
