/*
 * $Revision: 223 $ $Date: 2010-03-30 05:44:44 -0700 (Tue, 30 Mar 2010) $
 *
 * Copyright by Astos Solutions GmbH, Germany
 *
 * this file is published under the Astos Solutions Free Public License
 * For details on copyright and terms of use see 
 * http://www.astos.de/Astos_Solutions_Free_Public_License.html
 */

#include "Arc.h"
#include "Entity.h"
#include "InertialFrame.h"
#include "Trajectory.h"
#include "RotationModel.h"
#include "FixedPointTrajectory.h"
#include "FixedRotationModel.h"

using namespace vesta;
using namespace Eigen;


/** Create a default arc:
 *    Both the trajectory and body frames are EME J2000
 *    The trajectory is a fixed point at the origin.
 *    The rotation model is a constant rotation matrix (the identity matrix)
 */
Arc::Arc() :
    m_duration(0.0)
{
    m_trajectoryFrame = InertialFrame::equatorJ2000();
    m_bodyFrame = InertialFrame::equatorJ2000();
    m_trajectory = new FixedPointTrajectory(Vector3d::Zero());
    m_rotationModel = new FixedRotationModel(Quaterniond::Identity());
}


Arc::~Arc()
{
}


/** Set the duration of this arc.
 *
 * @param t the duration in units of Julian days (exactly 86400 seconds/JD)
 */
void
Arc::setDuration(double t)
{
    m_duration = t;
}


void
Arc::setCenter(Entity* center)
{
    m_center = center;
}


void
Arc::setTrajectoryFrame(Frame* f)
{
    m_trajectoryFrame = f;
}


void
Arc::setBodyFrame(Frame* f)
{
    m_bodyFrame = f;
}


void
Arc::setTrajectory(Trajectory* trajectory)
{
    m_trajectory = trajectory;
}


void
Arc::setRotationModel(RotationModel* rm)
{
    m_rotationModel = rm;
}

