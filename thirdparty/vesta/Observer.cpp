/*
 * $Revision: 223 $ $Date: 2010-03-30 05:44:44 -0700 (Tue, 30 Mar 2010) $
 *
 * Copyright by Astos Solutions GmbH, Germany
 *
 * this file is published under the Astos Solutions Free Public License
 * For details on copyright and terms of use see 
 * http://www.astos.de/Astos_Solutions_Free_Public_License.html
 */

#include "Observer.h"
#include "InertialFrame.h"

using namespace vesta;
using namespace Eigen;


Observer::Observer(Entity& center) :
    m_center(&center),
    m_positionFrame(InertialFrame::equatorJ2000()),
    m_pointingFrame(InertialFrame::equatorJ2000()),
    m_position(Vector3d::Zero()),
    m_orientation(Quaterniond::Identity())
{
}


Observer::~Observer()
{
}


void
Observer::setCenter(Entity* center)
{
    m_center = center;
}

/** Change the center object and update the position to maintain
  * the same absolute position in space at time t.
  */
void
Observer::updateCenter(Entity* center, double t)
{
    Quaterniond q = m_positionFrame->orientation(t);

    Vector3d absolutePosition = m_center->position(t) + q * m_position;
    m_position = q.conjugate() * (absolutePosition - center->position(t));
    setCenter(center);
}


void
Observer::setPositionFrame(Frame* f)
{
    m_positionFrame = f;
}


/** Change the position frame and update the position to maintain
  * the same absolute position in space at time t.
  */
void
Observer::updatePositionFrame(Frame* f, double t)
{
    m_position = (f->orientation(t).conjugate() * m_positionFrame->orientation(t)) * m_position;
    setPositionFrame(f);
}


void
Observer::setPointingFrame(Frame* f)
{
    m_pointingFrame = f;
}

/** Change the pointing frame and update the orientation to maintain
  * the same orientation position in space at time t.
  */
void
Observer::updatePointingFrame(Frame* f, double t)
{
    m_orientation = (f->orientation(t).conjugate() * m_pointingFrame->orientation(t)) * m_orientation;
    setPointingFrame(f);
}


void
Observer::setPosition(const Vector3d& position)
{
    m_position = position;
}


void
Observer::setOrientation(const Quaterniond& orientation)
{
    m_orientation = orientation;
}


/** Apply the specified rotation to the observer's orientation only.
  */
void
Observer::rotate(const Quaterniond& rotation)
{
    setOrientation(orientation() * rotation);
}


/** Apply the specified rotation to both the observer's orientation
  * and position relative to the center object. The rotation is in the
  * observer's current local coordinate system.
  */
void
Observer::orbit(const Quaterniond& rotation)
{
    // Compute the rotation in the observer's local coordinate system
    Quaterniond q = orientation() * rotation * orientation().conjugate();
    q.normalize(); // Prevent roundoff errors from giving us a non-unit quaternion

    setOrientation(q * orientation());
    setPosition(q * position());
}


/** Change the observer's distance to the center by the specified
  * factor. This has no effect when the observer is positioned
  * exactly at the center.
  */
void
Observer::changeDistance(double factor)
{
    setPosition(position() * factor);
}


/** Get the position of the observer in absolute coordinates (i.e. J2000
  * ecliptic frame, relative to the Solar System barycenter.
  */
Vector3d
Observer::absolutePosition(double t) const
{
    return m_center->position(t) + m_positionFrame->orientation(t) * m_position;
}


/** Get the orientation of the observer with respect to the base (i.e.
  * J2000 ecliptic) frame.
  */
Quaterniond
Observer::absoluteOrientation(double t) const
{
    return m_pointingFrame->orientation(t) * m_orientation;
}
