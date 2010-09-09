/*
 * $Revision: 477 $ $Date: 2010-08-31 11:49:37 -0700 (Tue, 31 Aug 2010) $
 *
 * Copyright by Astos Solutions GmbH, Germany
 *
 * this file is published under the Astos Solutions Free Public License
 * For details on copyright and terms of use see
 * http://www.astos.de/Astos_Solutions_Free_Public_License.html
 */

#ifndef _VESTA_VECTOR_MAP_LAYER_H_
#define _VESTA_VECTOR_MAP_LAYER_H_

#include "WorldLayer.h"
#include "Spectrum.h"
#include <Eigen/Core>
#include <vector>


namespace vesta
{

class MapLineString
{
public:
    EIGEN_MAKE_ALIGNED_OPERATOR_NEW

    MapLineString();

    void addPoint(const Eigen::Vector3f& p);
    const std::vector<Eigen::Vector3f>& points() const;

    Spectrum color() const
    {
        return m_color;
    }

    void setColor(const Spectrum& color)
    {
        m_color = color;
    }

    float opacity() const
    {
        return m_opacity;
    }

    void setOpacity(float opacity)
    {
        m_opacity = opacity;
    }

private:
    std::vector<Eigen::Vector3f> m_points;
    Spectrum m_color;
    float m_opacity;
};


/** VectorMapLayer is a world layer that contains a collection of vector shape elements: points,
  * lines, and polygons.
  */
class VectorMapLayer : public WorldLayer
{
public:
    EIGEN_MAKE_ALIGNED_OPERATOR_NEW

    /** Construct a new planet grid with white grid lines and an opacity of 50%.
      */
    VectorMapLayer();
    ~VectorMapLayer();

    virtual void renderTile(RenderContext& rc, const WorldGeometry* world, const QuadtreeTile* tile) const;

private:
    std::vector<MapLineString*> m_lineStrings;
};

}

#endif // _VESTA_TILE_BORDER_LAYER_H_
