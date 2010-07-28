/*
 * $Revision: 223 $ $Date: 2010-03-30 05:44:44 -0700 (Tue, 30 Mar 2010) $
 *
 * Copyright by Astos Solutions GmbH, Germany
 *
 * this file is published under the Astos Solutions Free Public License
 * For details on copyright and terms of use see 
 * http://www.astos.de/Astos_Solutions_Free_Public_License.html
 */

#ifndef _VESTA_TRAJECTORY_H_
#define _VESTA_TRAJECTORY_H_

#include "Object.h"
#include "StateVector.h"
#include <Eigen/Core>


namespace vesta
{

class Trajectory : public Object
{
public:
    Trajectory() {}
    virtual ~Trajectory() {}

    /*! Compute the state vector at the specified time. Time is
     *  given as the number of seconds since 1 Jan 2000 12:00:00 UTC.
     */
    virtual StateVector state(double t) const = 0;

    /*! Return the radius of a sphere centered at the origin that can
     *  contain the entire orbit. This sphere used to avoid calculating
     *  positions of objects that can't possible be visible.
     */
    virtual double boundingSphereRadius() const = 0;

    /*! The default implementation computes the complete state vector
     *  and discards velocity. Subclasses may override this method to
     *  provide a cheaper function for when just the position is requird.
     */
    virtual Eigen::Vector3d position(double t) const
    {
        return state(t).position();
    }

    /*! The default implementation computes the complete state vector
     *  and discards position. Subclasses may override this method to
     *  provide a cheaper function for when just the velocity is requird.
     */
    virtual Eigen::Vector3d velocity(double t) const
    {
        return state(t).velocity();
    }

    /*! Return true if the trajectory is periodic.
     */
    virtual bool isPeriodic() const
    {
        return false;
    }

    /*! Return the period of the trajectory in seconds. If the trajectory
     *  is aperiodic, this method returns zero.
     */
    virtual double period() const
    {
        return 0.0;
    }
};

}

#endif // _VESTA_TRAJECTORY_H_
