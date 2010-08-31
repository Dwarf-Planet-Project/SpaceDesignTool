/*
 * $Revision: 477 $ $Date: 2010-08-31 11:49:37 -0700 (Tue, 31 Aug 2010) $
 *
 * Copyright by Astos Solutions GmbH, Germany
 *
 * this file is published under the Astos Solutions Free Public License
 * For details on copyright and terms of use see 
 * http://www.astos.de/Astos_Solutions_Free_Public_License.html
 */

#include "LabelGeometry.h"
#include "RenderContext.h"
#include "Material.h"
#include <Eigen/Core>

using namespace vesta;
using namespace Eigen;
using namespace std;


LabelGeometry::LabelGeometry() :
    m_iconColor(Spectrum::White())
{
    setFixedApparentSize(true);
}


LabelGeometry::LabelGeometry(const std::string& text, TextureFont* font, const Spectrum& color, float iconSize) :
    m_text(text),
    m_font(counted_ptr<TextureFont>(font)),
    m_color(color),
    m_iconSize(iconSize),
    m_iconColor(Spectrum::White())
{
    setFixedApparentSize(true);
}


LabelGeometry::~LabelGeometry()
{
}


void
LabelGeometry::render(RenderContext& rc, double /* clock */) const
{
    bool hasIcon = !m_icon.isNull();

    Vector3f labelOffset = Vector3f::Zero();
    if (hasIcon)
    {
        labelOffset.x() = std::floor(m_iconSize / 2.0f) + 1.0f;
    }

    // Render during the opaque pass if opaque or during the translucent pass if not.
    if (rc.pass() == RenderContext::TranslucentPass)
    {
        // Keep the screen size of the icon fixed by adding a scale factor equal
        // to the distance from the eye.
        float distanceScale = rc.modelview().translation().norm();

        // Draw the label string as long as it's not empty and we have a valid font
        if (!m_text.empty() && !m_font.isNull())
        {
            rc.drawText(labelOffset, m_text, m_font.ptr(), m_color);
        }

        if (hasIcon)
        {
            Material material;
            material.setEmission(m_iconColor);
            material.setOpacity(0.99f);
            material.setBaseTexture(m_icon.ptr());
            rc.bindMaterial(&material);
            rc.drawBillboard(Vector3f::Zero(), m_iconSize * rc.pixelSize() * distanceScale);
        }
    }
}


float
LabelGeometry::boundingSphereRadius() const
{
    return 0.1f;
}


float
LabelGeometry::apparentSize() const
{
    // TODO: currently, the size of the label is ignored and just the icon is considered
    if (m_icon.isNull())
    {
        return 0.0f;
    }
    else
    {
        return m_iconSize;
    }
}
