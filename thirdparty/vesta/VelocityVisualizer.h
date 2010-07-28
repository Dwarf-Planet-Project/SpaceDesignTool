/*
 * $Revision: 223 $ $Date: 2010-03-30 05:44:44 -0700 (Tue, 30 Mar 2010) $
 *
 * Copyright by Astos Solutions GmbH, Germany
 *
 * this file is published under the Astos Solutions Free Public License
 * For details on copyright and terms of use see 
 * http://www.astos.de/Astos_Solutions_Free_Public_License.html
 */

#ifndef _VESTA_VELOCITY_VISUALIZER_H_
#define _VESTA_VELOCITY_VISUALIZER_H_

#include "ArrowVisualizer.h"


namespace vesta
{

class VelocityVisualizer : public ArrowVisualizer
{
public:
    EIGEN_MAKE_ALIGNED_OPERATOR_NEW

    VelocityVisualizer(double size);
    virtual ~VelocityVisualizer();

    virtual Eigen::Vector3d direction(const Entity* parent, double t) const;
};

}

#endif // _VESTA_VELOCITY_VISUALIZER_H_

