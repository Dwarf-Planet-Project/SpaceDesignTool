/*
 * $Revision: 223 $ $Date: 2010-03-30 05:44:44 -0700 (Tue, 30 Mar 2010) $
 *
 * Copyright by Astos Solutions GmbH, Germany
 *
 * this file is published under the Astos Solutions Free Public License
 * For details on copyright and terms of use see 
 * http://www.astos.de/Astos_Solutions_Free_Public_License.html
 */

#include "BillboardGeometry.h"
#include "RenderContext.h"
#include <Eigen/Core>
#include <cassert>

using namespace vesta;
using namespace Eigen;


BillboardGeometry::BillboardGeometry()
{
    m_material.setEmission(Spectrum(1.0f, 1.0f, 1.0f));
}


BillboardGeometry::~BillboardGeometry()
{
}


float
BillboardGeometry::boundingSphereRadius() const
{
    return 0.001f;// m_size;
}


void
BillboardGeometry::render(RenderContext& rc, float cameraDistance, double /* animationTime */) const
{
    // Render during the opaque pass if opaque or during the translucent pass if not.
    if ((rc.pass() == RenderContext::TranslucentPass) ^ isOpaque())
    {
        float scale = m_size * rc.pixelSize() * cameraDistance;
        rc.bindMaterial(&m_material);
        rc.drawBillboard(Vector3f::Zero(), scale);
    }
}


TextureMap*
BillboardGeometry::texture() const
{
    return m_material.baseTexture();
}


void
BillboardGeometry::setTexture(TextureMap* texture)
{
    m_material.setBaseTexture(texture);
}


float
BillboardGeometry::opacity() const
{
    return m_material.opacity();
}


void
BillboardGeometry::setOpacity(float opacity)
{
    m_material.setOpacity(opacity);
}


bool
BillboardGeometry::isOpaque() const
{
    return opacity() == 1.0f;
}
