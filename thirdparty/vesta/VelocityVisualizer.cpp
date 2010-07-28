/*
 * $Revision: 223 $ $Date: 2010-03-30 05:44:44 -0700 (Tue, 30 Mar 2010) $
 *
 * Copyright by Astos Solutions GmbH, Germany
 *
 * this file is published under the Astos Solutions Free Public License
 * For details on copyright and terms of use see 
 * http://www.astos.de/Astos_Solutions_Free_Public_License.html
 */

#include "VelocityVisualizer.h"
#include "Entity.h"

using namespace vesta;
using namespace Eigen;


VelocityVisualizer::VelocityVisualizer(double size) :
    ArrowVisualizer(size)
{
}


VelocityVisualizer::~VelocityVisualizer()
{
}


Vector3d
VelocityVisualizer::direction(const Entity* parent, double t) const
{
    Vector3d velocity = parent->state(t).velocity();
    if (velocity.isZero())
    {
        // Still need to return a unit vector
        // TODO: Find a better solution here. When the velocity is zero,
        // the velocity vector proably shouldn't be shown.
        return Vector3d::UnitX();
    }
    else
    {
        return velocity.normalized();
    }
}
