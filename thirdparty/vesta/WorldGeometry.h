/*
 * $Revision: 330 $ $Date: 2010-07-06 17:47:49 -0700 (Tue, 06 Jul 2010) $
 *
 * Copyright by Astos Solutions GmbH, Germany
 *
 * this file is published under the Astos Solutions Free Public License
 * For details on copyright and terms of use see 
 * http://www.astos.de/Astos_Solutions_Free_Public_License.html
 */

#ifndef _VESTA_WORLD_GEOMETRY_H_
#define _VESTA_WORLD_GEOMETRY_H_

#include "Geometry.h"
#include "Material.h"
#include "MapLayer.h"
#include <Eigen/Core>
#include <vector>


namespace vesta
{

class RenderContext;
class TextureMap;
class Atmosphere;
class QuadtreeTileAllocator;
class TiledMap;

/** WorldGeometry is a Geometry object specialized for rendering
  * spherical (or ellipsoidal) worlds. Optionally, a WorldGeometry
  * object may have a cloud layer, an atmosphere, a ring system,
  * and one or more map layers.
  */
class WorldGeometry : public Geometry
{
public:
    EIGEN_MAKE_ALIGNED_OPERATOR_NEW

    WorldGeometry();
    virtual ~WorldGeometry();

    void render(RenderContext& rc,
                float cameraDistance,
                double animationClock) const;
    
    float boundingSphereRadius() const;

    /** Get the lengths of the axes of the globe in kilometers. Note that these are
      * diameters, not radii.
      */
    Eigen::Vector3f ellipsoidAxes() const
    {
        return m_ellipsoidAxes;
    }

    /** Get the maximum radius of the globe in kilometers.
      */
    float maxRadius() const
    {
        return m_ellipsoidAxes.maxCoeff() * 0.5f;
    }

    /** Get the minimum radius of the globe in kilometers.
      */
    float minRadius() const
    {
        return m_ellipsoidAxes.minCoeff() * 0.5f;
    }

    /** Get the polar radius of the globe in kilometers.
      */
    float polarRadius() const
    {
        return m_ellipsoidAxes.z() * 0.5f;
    }

    /** Get the mean radius of the globe in kilometers. This is calculated as the
      * arithmetic mean of the semi-axes of the ellipsoid.
      */
    float meanRadius() const
    {
        return m_ellipsoidAxes.sum() / 6.0f;
    }

    /** Get the mean equatorial radius of the globe in kilometers. This is calculated as the
      * arithmetic mean of the x and y axes of the ellipsoid.
      */
    float meanEquatorialRadius() const
    {
        return (m_ellipsoidAxes.x() + m_ellipsoidAxes.y()) / 4.0f;
    }

    void setSphere(float radius);
    void setSpheroid(float radius, float oblateness);
    void setEllipsoid(const Eigen::Vector3f& axes);

    /** Get the global base texture.
      */
    TextureMap* baseMap() const
    {
        return m_baseMap.ptr();
    }

    void setBaseMap(TextureMap* baseMap);

    TiledMap* tiledMap() const
    {
        return m_baseTiledMap.ptr();
    }

    void setBaseMap(TiledMap* baseMap);

    /** Get the global normal map. The normal map contains normal perturbations
      * derived from a height map.
      */
    TextureMap* normalMap() const
    {
        return m_normalMap.ptr();
    }

    void setNormalMap(TextureMap* normalMap);

    void addLayer(MapLayer* layer);
    void removeLayer(unsigned int index);
    void removeLayer();
    MapLayer* layer(unsigned int index) const;
    unsigned int layerCount() const;

    /** Return true if this globe is self-luminous (e.g. the Sun)
      */
    bool isEmissive() const
    {
        return m_emissive;
    }

    /** Set whether this globe is self-luminous. If true, it
      * will not have any shading applied. Emissive true is the
      * appropriate setting for the Sun. Note that setting emissive
      * to true will *not* make the object a light source.
      */
    void setEmissive(bool emissive)
    {
        m_emissive = emissive;
    }

    Atmosphere* atmosphere() const;

    void setAtmosphere(Atmosphere* atmosphere);

    /** Get the cloud texture map.
      */
    TextureMap* cloudMap() const
    {
        return m_cloudMap.ptr();
    }

    void setCloudMap(TextureMap* cloudMap);

    /** Get the altitude (in km) of the cloud layer above surface of the planet.
      */
    float cloudAltitude() const
    {
        return m_cloudAltitude;
    }

    /** Set the altitude (in km) of the cloud layer above the surface of the planet.
      */
    void setCloudAltitude(float altitude)
    {
        m_cloudAltitude = altitude;
    }

    /** Get the specular reflectance coefficients. These will be zero (black) for
      * globes that aren't glossy.
      */
    Spectrum specularReflectance() const
    {
        return m_specularReflectance;
    }

    /** Set the specular reflectance coefficients. Specular reflectance can be set
      * to non-zero for worlds that have specular reflections from liquids or smooth
      * ice on their surfaces.
      */
    void setSpecularReflectance(const Spectrum& reflectance)
    {
        m_specularReflectance = reflectance;
    }

    /** Get the specular power. Larger values produce a smaller
      * specular highlight on the reflective portions of the globe, making
      * the surface appear smoother.
      */
    float specularPower() const
    {
        return m_specularPower;
    }

    /** Set the specular power. Larger values produce a smaller
      * specular highlight on the reflective portions of the globe, making
      * the surface appear smoother.
      */
    void setSpecularPower(float specularPower)
    {
        m_specularPower = specularPower;
    }

protected:
    virtual bool handleRayPick(const Eigen::Vector3d& pickOrigin,
                               const Eigen::Vector3d& pickDirection,
                               double* distance) const;

private:
    void renderSphere(int subdivisions) const;
    void renderNormalMappedSphere(int subdivisions) const;
    void renderPatch(int subdivisions, const MapLayerBounds& box) const;
    void renderBand(int subdivisions,
                    double latStart,
                    double latEnd,
                    double lonStart,
                    double lonEnd,
                    float tStart,
                    float tEnd) const;

private:
    Eigen::Vector3f m_ellipsoidAxes;
    counted_ptr<TextureMap> m_baseMap;
    counted_ptr<TextureMap> m_normalMap;
    counted_ptr<TiledMap> m_baseTiledMap;
    counted_ptr<Material> m_material;
    counted_ptr<Atmosphere> m_atmosphere;
    std::vector<counted_ptr<MapLayer> > m_layers;
    bool m_emissive;
    Spectrum m_specularReflectance;
    float m_specularPower;

    counted_ptr<TextureMap> m_cloudMap;
    float m_cloudAltitude;

    QuadtreeTileAllocator* m_tileAllocator;
};

}

#endif // _VESTA_WOLRD_GEOMETRY_H_