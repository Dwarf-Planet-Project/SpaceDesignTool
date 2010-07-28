/*
 * $Revision: 389 $ $Date: 2010-07-26 19:34:58 -0700 (Mon, 26 Jul 2010) $
 *
 * Copyright by Astos Solutions GmbH, Germany
 *
 * this file is published under the Astos Solutions Free Public License
 * For details on copyright and terms of use see 
 * http://www.astos.de/Astos_Solutions_Free_Public_License.html
 */

#ifndef _VESTA_UNIVERSE_RENDERER_H_
#define _VESTA_UNIVERSE_RENDERER_H_

#include "Universe.h"
#include "SkyLayer.h"
#include "Spectrum.h"
#include "Viewport.h"
#include "LightingEnvironment.h"
#include <Eigen/StdVector>
#include <vector>

namespace vesta
{

class Observer;
class RenderContext;
class Framebuffer;
class CubeMapFramebuffer;

class UniverseRenderer
{
public:
    EIGEN_MAKE_ALIGNED_OPERATOR_NEW

    UniverseRenderer();
    ~UniverseRenderer();

    enum RenderStatus
    {
        RenderOk,
        RenderNoViewSet,
        RenderViewSetAlreadyStarted
    };

    RenderStatus beginViewSet(const Universe& universe, double t);
    RenderStatus endViewSet();

    RenderStatus renderView(const Observer* observer,
                            double fieldOfView,
                            int viewportWidth,
                            int viewportHeight);
    RenderStatus renderView(const LightingEnvironment* lighting,
                            const Observer* observer,
                            double fieldOfView,
                            const Viewport& viewport,
                            Framebuffer* renderSurface = 0);
    RenderStatus renderView(const LightingEnvironment* lighting,
                            const Eigen::Vector3d& cameraPosition,
                            const Eigen::Quaterniond& cameraOrientation,
                            double fieldOfView,
                            const Viewport& viewport,
                            Framebuffer* renderSurface = 0);
    RenderStatus renderCubeMap(const LightingEnvironment* lighting,
                               const Eigen::Vector3d& cameraPosition,
                               CubeMapFramebuffer* cubeMap);

    Spectrum ambientLight() const
    {
        return m_ambientLight;
    }

    void setAmbientLight(const Spectrum& spectrum);

    void addSkyLayer(SkyLayer* layer);
    void removeSkyLayer(unsigned int index);
    SkyLayer* skyLayer(unsigned int index) const;
    unsigned int skyLayerCount() const;

    bool initializeShadowMaps(unsigned int shadowMapSize = 1024,
                              unsigned int maxShadowMaps = 1);

    /** Return true if this renderer has shadows enabled.
      */
    bool shadowsEnabled() const
    {
        return m_shadowsEnabled;
    }
    void setShadowsEnabled(bool enable);
    bool shadowsSupported() const;

    /** Return true if visualizers will be drawn. Visualizers are on by default.
     */
    bool visualizersEnabled() const
    {
        return m_visualizersEnabled;
    }
    void setVisualizersEnabled(bool enable);

    /** Return true if sky layers will be drawn. Sky layers are on by default.
      */
    bool skyLayersEnabled() const
    {
        return m_skyLayersEnabled;
    }
    void setSkyLayersEnabled(bool enable);

public:
    struct VisibleItem
    {
        EIGEN_MAKE_ALIGNED_OPERATOR_NEW

        const Entity* entity;
        const Geometry* geometry;
        Eigen::Vector3d position;
        Eigen::Vector3d cameraRelativePosition;
        Eigen::Quaternionf orientation;
        float nearDistance;    // signed distance to the camera plane
        float farDistance;     // signed distance to the camera plane
        float boundingRadius;
    };

    struct LightSourceItem
    {
        const LightSource* lightSource;
        Eigen::Vector3d position;
        Eigen::Vector3d cameraRelativePosition;
    };

    typedef std::vector<VisibleItem, Eigen::aligned_allocator<VisibleItem> > VisibleItemVector;

    struct DepthBufferSpan
    {
        float nearDistance;
        float farDistance;
        unsigned int backItemIndex;
        unsigned int itemCount;
    };

private:
    void splitDepthBuffer();
    void coalesceDepthBuffer();
    void renderDepthBufferSpan(const DepthBufferSpan& span,
                               float fieldOfView,
                               float aspectRatio);
    bool renderDepthBufferSpanShadows(const DepthBufferSpan& span,
                                      const Eigen::Vector3d& lightPosition);
    void addVisibleItem(const Entity* entity,
                        const Geometry* geometry,
                        const Eigen::Vector3d& position,
                        const Eigen::Vector3d& cameraRelativePosition,
                        const Eigen::Vector3f& cameraSpacePosition,
                        const Eigen::Quaternionf& orientation,
                        float nearAdjust);
    void drawItem(const VisibleItem& item);
    Eigen::Matrix4f setupShadowRendering(const Framebuffer* shadowMap,
                                         const Eigen::Vector3f& lightDirection,
                                         float shadowGroupSize);

    void setDepthRange(float front, float back);

private:
    RenderContext* m_renderContext;

    const Universe* m_universe;
    double m_currentTime;

    VisibleItemVector m_visibleItems;
    VisibleItemVector m_splittableItems;
    std::vector<DepthBufferSpan> m_depthBufferSpans;
    std::vector<DepthBufferSpan> m_mergedDepthBufferSpans;
    std::vector<LightSourceItem> m_lightSources;

    Spectrum m_ambientLight;
    std::vector<counted_ptr<SkyLayer> > m_skyLayers;

    counted_ptr<Framebuffer> m_shadowMap;

    bool m_shadowsEnabled;
    bool m_visualizersEnabled;
    bool m_skyLayersEnabled;
    float m_depthRangeFront;
    float m_depthRangeBack;

    counted_ptr<Framebuffer> m_renderSurface;
    Viewport m_renderViewport;

    const LightingEnvironment* m_lighting;
};

}

#endif // _VESTA_UNIVERSE_RENDERER_H_
