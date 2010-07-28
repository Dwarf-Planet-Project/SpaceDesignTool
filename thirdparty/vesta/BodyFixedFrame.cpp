/*
 * $Revision: 223 $ $Date: 2010-03-30 05:44:44 -0700 (Tue, 30 Mar 2010) $
 *
 * Copyright by Astos Solutions GmbH, Germany
 *
 * this file is published under the Astos Solutions Free Public License
 * For details on copyright and terms of use see 
 * http://www.astos.de/Astos_Solutions_Free_Public_License.html
 */

#include "BodyFixedFrame.h"

using namespace vesta;
using namespace Eigen;


BodyFixedFrame::BodyFixedFrame(Entity& body) :
    m_body(&body)
{
}


BodyFixedFrame::~BodyFixedFrame()
{
}


Quaterniond
BodyFixedFrame::orientation(double t) const
{
    return m_body->orientation(t);
}


Vector3d
BodyFixedFrame::angularVelocity(double t) const
{
    return m_body->angularVelocity(t);
}
