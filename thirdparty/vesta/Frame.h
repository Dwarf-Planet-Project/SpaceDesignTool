/*
 * $Revision: 475 $ $Date: 2010-08-31 08:09:34 -0700 (Tue, 31 Aug 2010) $
 *
 * Copyright by Astos Solutions GmbH, Germany
 *
 * this file is published under the Astos Solutions Free Public License
 * For details on copyright and terms of use see 
 * http://www.astos.de/Astos_Solutions_Free_Public_License.html
 */

#ifndef _VESTA_FRAME_H_
#define _VESTA_FRAME_H_

#include "Object.h"
#include <Eigen/Core>
#include <Eigen/Geometry>


namespace vesta
{

/** Abstract base class for reference frames. A reference frame in vesta is a
  * simply set of three orthogonal axes. Subclasses must override the orientation()
  * and angularVelocity() methods.
  */
class Frame : public Object
{
public:
    virtual ~Frame() {}

    /** Compute the orientation of the frame at the specified time.
      * @param t The time given as the number of seconds since 1 Jan 2000 12:00:00 TDB.
      */
    virtual Eigen::Quaterniond orientation(double t) const = 0;

    /** Compute the angular of the frame at the specified time. The units
      * of angular velocity are radians per second.
      * @param t The time given as the number of seconds since 1 Jan 2000 12:00:00 TDB.
      */
    virtual Eigen::Vector3d angularVelocity(double t) const = 0;
};

}

#endif // _VESTA_FRAME_H_
