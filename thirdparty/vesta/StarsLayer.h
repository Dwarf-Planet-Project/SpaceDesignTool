/*
 * $Revision: 375 $ $Date: 2010-07-20 12:25:37 -0700 (Tue, 20 Jul 2010) $
 *
 * Copyright by Astos Solutions GmbH, Germany
 *
 * this file is published under the Astos Solutions Free Public License
 * For details on copyright and terms of use see
 * http://www.astos.de/Astos_Solutions_Free_Public_License.html
 */

#ifndef _VESTA_STARS_LAYER_H_
#define _VESTA_STARS_LAYER_H_

#include "SkyLayer.h"
#include "StarCatalog.h"


namespace vesta
{

class GLVertexBuffer;
class GLShaderProgram;

class StarsLayer : public SkyLayer
{
public:
    StarsLayer();
    explicit StarsLayer(StarCatalog* starCatalog);
    ~StarsLayer();

    StarCatalog* starCatalog() const
    {
        return m_starCatalog.ptr();
    }

    void setStarCatalog(StarCatalog* starCatalog);

    virtual void render(RenderContext& rc);

    enum StarStyle
    {
        PointStars    = 0,
        GaussianStars = 1,
    };

    StarStyle style() const
    {
        return m_style;
    }

    void setStyle(StarStyle style);

    /** Return the apparent magnitude of the faintest stars that will be visible.
      */
    float limitingMagnitude() const
    {
        return m_limitingMagnitude;
    }

    /** Set the magnitude of the faintest stars that will be visible. A value of
      * 6.5 is approximately right for a human observer under clear, dark skies.
      */
    void setLimitingMagnitude(float limitingMagnitude);

private:
    void updateVertexBuffer();

private:
    counted_ptr<StarCatalog> m_starCatalog;
    char* m_vertexArray;
    counted_ptr<GLVertexBuffer> m_vertexBuffer;
    counted_ptr<GLShaderProgram> m_starShader;
    counted_ptr<GLShaderProgram> m_starShaderSRGB;
    bool m_vertexBufferCurrent;
    bool m_starShaderCompiled;
    StarStyle m_style;
    float m_limitingMagnitude;
};

}
#endif // _VESTA_STARS_LAYER_H_
