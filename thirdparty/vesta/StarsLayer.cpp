/*
 * $Revision: 223 $ $Date: 2010-03-30 05:44:44 -0700 (Tue, 30 Mar 2010) $
 *
 * Copyright by Astos Solutions GmbH, Germany
 *
 * this file is published under the Astos Solutions Free Public License
 * For details on copyright and terms of use see
 * http://www.astos.de/Astos_Solutions_Free_Public_License.html
 */

#include "StarsLayer.h"
#include "RenderContext.h"
#include "OGLHeaders.h"
#include "Debug.h"
#include "glhelp/GLVertexBuffer.h"
#include "glhelp/GLShaderProgram.h"
#include <string>

using namespace vesta;
using namespace Eigen;
using namespace std;


// Star shader GLSL source
//
// Stars are drawn as the sum of two Gaussian functions:
//   - The convolution of the pixel function and the point spread function
//   - A glare function that gives a halo effect for bright stars; the physical
//     cause of this effect is scattering of light in the eye or reflections
//     within the optical system of camera. The function is tuned so that it
//     looks right; no attempt is made to physically simulate the light.
//
// The alpha channel of the color is interpreted as a star'ss apparent magnitude
// and translated to brightness by taking a power of 2.512 (the brightness ratio
// between two stars that differ by one magnitude.) The mapping from the [0, 1]
// range of alpha to a magnitude range is controlled by magRange:
//     magnitude = magRange * (alpha - 1)
//
// The point size is computed so that it is just large enough to fit the Gaussian
// disc for the star. This keeps the number of pixels drawn for faint stars very
// low, which saves fill rate and pixel shader cycles.
//
// Before fragment color is generated, it is mapped from linear to sRGB color space.
// This mapping is unecessary if the EXT_framebuffer_sSRGB extension is enabled.

static const char* StarVertexShaderSource =
"uniform vec2 viewportSize;       \n"
"varying vec2 pointCenter;        \n"
"varying vec4 color;              \n"
"varying float brightness;        \n"
"uniform float magRange;          \n"
"uniform float sigma2;            \n"
"uniform float glareSigma2;       \n"
"uniform float glareBrightness;   \n"
"uniform float exposure;          \n"
"const float thresholdBrightness = 1.0 / 512.0;\n"
"void main()                      \n"
"{                                \n"
"    vec4 projectedPosition = gl_ModelViewProjectionMatrix * gl_Vertex;       \n"
"    vec2 devicePosition = projectedPosition.xy / projectedPosition.w;        \n"
"    pointCenter = (devicePosition * 0.5 + vec2(0.5, 0.5)) * viewportSize;    \n"
"    color = gl_Color;                                                        \n"
"    float b = pow(2.512, magRange * (color.a - 1.0));                        \n"

"    float r2 = -log(thresholdBrightness / (exposure * b)) * 2.0 * sigma2;          \n"
"    float rGlare2 = -log(thresholdBrightness / (exposure * glareBrightness * b)) * 2.0 * glareSigma2;      \n"
"    gl_PointSize = 2.0 * sqrt(max(r2, rGlare2));                             \n"

"    brightness = b;                                                          \n"
"    gl_Position = projectedPosition;                                         \n"
"}                                \n"
;

static const char* StarFragmentShaderSource =
"varying vec2 pointCenter;                       \n"
"varying vec4 color;                             \n"
"uniform float sigma2;                           \n"
"uniform float glareSigma2;                      \n"
"uniform float glareBrightness;                  \n"
"uniform float exposure;                         \n"
"varying float brightness;                       \n"

"void main()                                     \n"
"{                                               \n"
"    vec2 offset = gl_FragCoord.xy - pointCenter;                \n"
"    float r2 = dot(offset, offset);                             \n"
"    float b = exp(-r2 / (2.0 * sigma2));                        \n"
"    float spikes = (max(0.0, 1.0 - abs(offset.x + offset.y)) + max(0.0, 1.0 - abs(offset.x - offset.y))) * 3.0 + 0.5;\n"
"    b += glareBrightness * exp(-r2 / (2.0 * glareSigma2)) * spikes;      \n"
"    gl_FragColor = vec4(linearToSRGB(b * exposure * color.rgb * brightness), 1.0);   \n"
"}                                                               \n"
;

static const char* LinearToSRGBSource =
"vec3 linearToSRGB(vec3 c)                       \n"
"{                                               \n"
"    vec3 linear = 12.92 * c;                    \n"
"    vec3 nonlinear = (1.0 + 0.055) * pow(c, vec3(1.0 / 2.4)) - vec3(0.055);\n"
"    return mix(linear, nonlinear, step(vec3(0.0031308), c));\n"
"}                                               \n"
;

static const char* PassthroughSRGBSource =
"vec3 linearToSRGB(vec3 c)                       \n"
"{                                               \n"
"    return c;                                   \n"
"}                                               \n"
;



// End star shader

StarsLayer::StarsLayer() :
    m_vertexBufferCurrent(false),
    m_starShaderCompiled(false),
    m_style(GaussianStars)
{
}


StarsLayer::StarsLayer(StarCatalog* starCatalog) :
    m_starCatalog(starCatalog),
    m_vertexBufferCurrent(false),
    m_starShaderCompiled(false),
    m_style(GaussianStars)
{
}


StarsLayer::~StarsLayer()
{
}


/** Set the catalog used by this star layer.
  */
void
StarsLayer::setStarCatalog(StarCatalog* starCatalog)
{
    if (m_starCatalog.ptr() != starCatalog)
    {
        m_vertexBufferCurrent = false;
        m_starCatalog = starCatalog;
    }
}


void
StarsLayer::render(RenderContext& rc)
{
    const StarCatalog::StarRecord* starData = m_starCatalog->vertexBuffer();

    if (!m_vertexBufferCurrent)
    {
        if (GLVertexBuffer::supported())
        {
            m_vertexBuffer = new GLVertexBuffer(sizeof(StarCatalog::StarRecord) * m_starCatalog->size(), GL_STATIC_DRAW, starData);
            if (!m_vertexBuffer->isValid())
            {
                m_vertexBuffer = NULL;
            }
        }
        m_vertexBufferCurrent = true;
    }

    if (rc.shaderCapability() != RenderContext::FixedFunction && !m_starShaderCompiled)
    {
        string fragmentShaderSource = string(PassthroughSRGBSource) + StarFragmentShaderSource;
        m_starShader = GLShaderProgram::CreateShaderProgram(StarVertexShaderSource, fragmentShaderSource);

        // Create a version of the shader that does SRGB correction for configurations that don't
        // support the EXT_framebuffer_sRGB extension
        fragmentShaderSource = string(LinearToSRGBSource) + StarFragmentShaderSource;
        m_starShaderSRGB = GLShaderProgram::CreateShaderProgram(StarVertexShaderSource, fragmentShaderSource);

        m_starShaderCompiled = true;
    }

    bool useStarShader = m_style == GaussianStars && m_starShader.isValid() && m_starShaderSRGB.isValid();
    bool enableSRGBExt = GLEW_EXT_framebuffer_sRGB == GL_TRUE;

    if (starData != NULL)
    {
        Material starMaterial;
        starMaterial.setDiffuse(Spectrum(1.0f, 1.0f, 1.0f));
        starMaterial.setBlendMode(Material::AdditiveBlend);
        rc.bindMaterial(&starMaterial);

        if (m_vertexBuffer.isValid())
        {
            rc.bindVertexBuffer(VertexSpec::PositionColor, m_vertexBuffer.ptr(), VertexSpec::PositionColor.size());
        }
        else
        {
            rc.bindVertexArray(VertexSpec::PositionColor, starData, sizeof(StarCatalog::StarRecord));
        }

        if (useStarShader)
        {
            GLShaderProgram* starShader = NULL;
            if (enableSRGBExt)
            {
                starShader = m_starShader.ptr();
            }
            else
            {
                starShader = m_starShaderSRGB.ptr();
            }

            if (enableSRGBExt)
            {
                rc.enableCustomShader(starShader);
                glEnable(GL_FRAMEBUFFER_SRGB_EXT);
            }
            else
            {
                rc.enableCustomShader(starShader);
            }

            starShader->bind();
            Vector2f viewportSize(rc.viewportWidth(), rc.viewportHeight());
            starShader->setConstant("viewportSize", viewportSize);
            starShader->setConstant("magRange", 7.0f);
            starShader->setConstant("sigma2", 0.35f);
            starShader->setConstant("glareSigma2", 15.0f);
            starShader->setConstant("glareBrightness", 0.003f);
            starShader->setConstant("exposure", 3*7.5f);

            glEnable(GL_VERTEX_PROGRAM_POINT_SIZE_ARB);
            if (GLEW_ARB_multisample)
            {
                glDisable(GL_MULTISAMPLE_ARB);
            }
        }
        else
        {
            glPointSize(2.0f);
        }

        rc.drawPrimitives(PrimitiveBatch(PrimitiveBatch::Points, m_starCatalog->size()));

        rc.unbindVertexArray();
        if (m_vertexBuffer.isValid())
        {
            m_vertexBuffer->unbind();
        }

        if (useStarShader)
        {
            rc.disableCustomShader();
            glEnable(GL_VERTEX_PROGRAM_POINT_SIZE_ARB);
            if (GLEW_ARB_multisample)
            {
                glEnable(GL_MULTISAMPLE_ARB);
            }

            if (enableSRGBExt)
            {
                glDisable(GL_FRAMEBUFFER_SRGB_EXT);
            }
        }
    }
}
