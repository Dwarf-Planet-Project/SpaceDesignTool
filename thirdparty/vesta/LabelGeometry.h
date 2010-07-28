/*
 * $Revision: 314 $ $Date: 2010-06-29 17:32:55 -0700 (Tue, 29 Jun 2010) $
 *
 * Copyright by Astos Solutions GmbH, Germany
 *
 * this file is published under the Astos Solutions Free Public License
 * For details on copyright and terms of use see 
 * http://www.astos.de/Astos_Solutions_Free_Public_License.html
 */

#ifndef _VESTA_LABEL_GEOMETRY_H_
#define _VESTA_LABEL_GEOMETRY_H_

#include "Geometry.h"
#include "Spectrum.h"
#include "TextureFont.h"
#include "TextureMap.h"
#include <string>


namespace vesta
{

/** LabelGeometry is a geometry type used for single-line screen aligned text
  * and an icon. Both the icon and the label may be omitted.
  */
class LabelGeometry : public Geometry
{
public:
    EIGEN_MAKE_ALIGNED_OPERATOR_NEW

    LabelGeometry(const std::string& text, TextureFont* font, const Spectrum& color = Spectrum(1.0f, 1.0f, 1.0f), float iconSize = 20.0f);
    LabelGeometry();

    virtual ~LabelGeometry();

    // Implementations of abstract methods for Geometry
    void render(RenderContext& rc,
                float cameraDistance,
                double animationClock) const;

    float boundingSphereRadius() const;

    virtual bool isOpaque() const
    {
        return false;
    }

    virtual float apparentSize() const;
    // end geometry methods


    std::string text() const
    {
        return m_text;
    }

    void setText(const std::string& text)
    {
        m_text = text;
    }

    const TextureFont* font() const
    {
        return m_font.ptr();
    }

    void setFont(TextureFont* font)
    {
        m_font = font;
    }

    Spectrum color() const
    {
        return m_color;
    }

    void setColor(const Spectrum& color)
    {
        m_color = color;
    }

    /** Get the icon texture map; this method returns null
      * if no icon is set.
      */
    TextureMap* icon() const
    {
        return m_icon.ptr();
    }

    /** Set the icon texture map. Setting it to null means
      * that no icon will be shown.
      */
    void setIcon(TextureMap* icon)
    {
        m_icon = icon;
    }

    /** Get the icon size in pixels. */
    float iconSize() const
    {
        return m_iconSize;
    }

    /** Set the icon size. The default size of the icon is 20 pixels.
      *
      * \param pixels the size of the icon in pixels.
      */
    void setIconSize(float pixels)
    {
        m_iconSize = pixels;
    }

    Spectrum iconColor() const
    {
        return m_iconColor;
    }

    void setIconColor(const Spectrum& color)
    {
        m_iconColor = color;
    }

private:
    std::string m_text;
    counted_ptr<TextureFont> m_font;
    counted_ptr<TextureMap> m_icon;
    Spectrum m_color;
    float m_iconSize;
    Spectrum m_iconColor;
};

}

#endif // _VESTA_LABEL_GEOMETRY_H_

