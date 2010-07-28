/*
 * $Revision: 223 $ $Date: 2010-03-30 05:44:44 -0700 (Tue, 30 Mar 2010) $
 *
 * Copyright by Astos Solutions GmbH, Germany
 *
 * this file is published under the Astos Solutions Free Public License
 * For details on copyright and terms of use see 
 * http://www.astos.de/Astos_Solutions_Free_Public_License.html
 */

#ifndef _VESTA_INERTIAL_FRAME_H_
#define _VESTA_INERTIAL_FRAME_H_

#include "Frame.h"


namespace vesta
{

class InertialFrame : public Frame
{
public:
    EIGEN_MAKE_ALIGNED_OPERATOR_NEW

    InertialFrame(const Eigen::Quaterniond& q) : m_orientation(q) {}
    virtual ~InertialFrame() {}

    /** Return the orientation of the frame. For a inertial frames,
     *  this value is not time dependent.
     */
    virtual Eigen::Quaterniond orientation(double /* t */) const
    {
        return m_orientation;
    }

    /** Return the orientation of the inertial frame.
     */
    Eigen::Quaterniond orientation() const
    {
        return m_orientation;
    }

    Eigen::Vector3d angularVelocity(double /* t */) const
    {
        return Eigen::Vector3d::Zero();
    }

    static InertialFrame* eclipticJ2000()
    {
        return &*s_EclipticJ2000;
    }

    static InertialFrame* equatorJ2000()
    {
        return &*s_EquatorJ2000;
    }

    static counted_ptr<InertialFrame> s_EclipticJ2000;
    static counted_ptr<InertialFrame> s_EquatorJ2000;

private:
    Eigen::Quaterniond m_orientation;
};

}

#endif // _VESTA_INERTIAL_FRAME_H_
