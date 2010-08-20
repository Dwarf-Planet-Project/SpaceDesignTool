/*
 * $Revision: 423 $ $Date: 2010-08-11 21:30:49 -0700 (Wed, 11 Aug 2010) $
 *
 * Copyright by Astos Solutions GmbH, Germany
 *
 * this file is published under the Astos Solutions Free Public License
 * For details on copyright and terms of use see 
 * http://www.astos.de/Astos_Solutions_Free_Public_License.html
 */

#include "WorldGeometry.h"
#include "RenderContext.h"
#include "Units.h"
#include "TextureMap.h"
#include "TiledMap.h"
#include "OGLHeaders.h"
#include "ShaderBuilder.h"
#include "Atmosphere.h"
#include "PlanetaryRings.h"
#include "Debug.h"
#include <Eigen/LU>
#include <algorithm>
#include <deque>

using namespace vesta;
using namespace Eigen;
using namespace std;


#define USE_QUADTREE 1
#define DEBUG_QUADTREE 0


static VertexAttribute posNormTexTangentAttributes[] = {
    VertexAttribute(VertexAttribute::Position,     VertexAttribute::Float3),
    VertexAttribute(VertexAttribute::Normal,       VertexAttribute::Float3),
    VertexAttribute(VertexAttribute::TextureCoord, VertexAttribute::Float2),
    VertexAttribute(VertexAttribute::Tangent,      VertexAttribute::Float3),
};

VertexSpec PositionNormalTexTangent(4, posNormTexTangentAttributes);


static const float MaxTileSquareSize = 256.0f;  // size in pixels
static const unsigned int TileSubdivision = 16;
static const float SquareSize = 1.0f / float(TileSubdivision);

// Debugging value for checking the performance of the quadtree drawing algorithm
static unsigned int s_TilesDrawn = 0;


// The QuadtreeTile class is used for level of detail when rendering
// planet geometry. QuadtreeTile implements a 'restricted quadtree':
// adjacent tiles are not allowed to have more differ by more than
// one level of detail. This greatly simplifies the process of stitching
// together tiles to avoid cracks.
//
// The neighbor pointers of a tile will be null when no neighbors exist
// at the current level of detail. When a tile is split into four child
// tiles, missing neighbors must first be created in order to prevent violating
// the level of detail restriction.
//
// Each tile is a patch of the ellipsoid surface, subdivided to TileSubdivision
// squares per side. Batching is essential for performance on modern GPUs, since
// the cost to cull at per-quad granularity is much higher than the cost of
// rendering a few extra primitives.

namespace vesta
{
class QuadtreeTileAllocator;

struct CullingPlaneSet
{
    Hyperplane<float, 3> planes[6];
};


class QuadtreeTile
{
friend class QuadtreeTileAllocator;

public:
    enum Direction
    {
        East  = 0,
        North = 1,
        West  = 2,
        South = 3,
    };

    enum Quadrant
    {
        Northeast = 0,
        Northwest = 1,
        Southwest = 2,
        Southeast = 3,
    };

    enum
    {
        NormalMap = 0x1,
    };

    QuadtreeTile();
    QuadtreeTile(QuadtreeTile* parent, unsigned int quadrant, const Vector3f& semiAxes);

    void setNeighbor(Direction direction, QuadtreeTile* tile)
    {
        m_neighbors[(int) direction] = tile;
    }

    void tessellate(const Vector3f& eyePosition,
                    const CullingPlaneSet& cullFrustum,
                    const Vector3f& globeSemiAxes,
                    float splitThreshold,
                    float pixelSize);
    void split(const CullingPlaneSet& cullFrustum, const Vector3f& semiAxes);
    bool cull(const CullingPlaneSet& cullFrustum) const;
    void render(RenderContext& rc, unsigned int features) const;
    void render(RenderContext& rc, Material& material, TiledMap* tiledMap) const;
    void render(RenderContext& rc, const MapLayer& layer) const;
    void drawPatch(RenderContext& rc, unsigned int features) const;
    void drawPatch(RenderContext& rc, const MapLayer& layer) const;
    void drawPatch(RenderContext& rc, Material& material, TiledMap* baseMap) const;

    bool isRoot() const
    {
        return !m_parent;
    }

    bool hasChildren() const
    {
        // The following test works because a tile has either no children or a full set.
        return m_children[0] != NULL;
    }

    bool isCulled() const
    {
        return m_isCulled;
    }

    Vector2f southwest() const
    {
        return m_southwest;
    }

    float extent() const
    {
        return m_extent;
    }

    void link(Direction direction, QuadtreeTile* neighbor)
    {
        m_neighbors[direction] = neighbor;
        if (neighbor)
        {
            Direction opposing = Direction(((unsigned int) direction + 2) & 0x3);
            neighbor->m_neighbors[opposing] = this;
        }
    }

private:
    void computeCenterAndRadius(const Vector3f& semiAxes);

private:
    QuadtreeTile* m_parent;
    QuadtreeTile* m_neighbors[4];
    QuadtreeTile* m_children[4];
    QuadtreeTileAllocator* m_allocator;
    unsigned int m_level;
    unsigned int m_row;
    unsigned int m_column;
    Vector2f m_southwest;
    float m_extent;
    Vector3f m_center;
    float m_boundingSphereRadius;
    float m_approxPixelSize;
    bool m_isCulled;
};


class QuadtreeTileAllocator
{
public:
    QuadtreeTileAllocator()
    {
    }

    QuadtreeTile* newRootTile(unsigned int row, unsigned int column,
                              const Vector2f& southwest, float extent,
                              const Vector3f& semiAxes)
    {
        QuadtreeTile tile;
        tile.m_allocator = this;
        tile.m_row = row;
        tile.m_column = column;
        tile.m_southwest = southwest;
        tile.m_extent = extent;
        tile.computeCenterAndRadius(semiAxes);

        m_tilePool.push_back(tile);
        return &m_tilePool.back();
    }

    QuadtreeTile* newTile(QuadtreeTile* parent, unsigned int whichChild,
                          const Vector3f& semiAxes)
    {
        QuadtreeTile tile(parent, whichChild, semiAxes);
        m_tilePool.push_back(tile);
        return &m_tilePool.back();
    }

    unsigned int tileCount() const
    {
        return m_tilePool.size();
    }

    void clear()
    {
        m_tilePool.clear();
    }

    typedef deque<QuadtreeTile> TileArray;

    const TileArray& tiles() const
    {
        return m_tilePool;
    }

private:
    TileArray m_tilePool;
};

} // namespace vesta



QuadtreeTile::QuadtreeTile() :
    m_parent(NULL),
    m_level(NULL),
    m_approxPixelSize(0.0f),
    m_isCulled(false)
{
    for (unsigned int i = 0; i < 4; ++i)
    {
        m_neighbors[i] = NULL;
        m_children[i] = NULL;
    }
}


QuadtreeTile::QuadtreeTile(QuadtreeTile* parent, unsigned int quadrant, const Vector3f& semiAxes) :
    m_parent(parent),
    m_allocator(parent->m_allocator),
    m_level(parent->m_level + 1),
    m_extent(parent->m_extent * 0.5f),
    m_approxPixelSize(parent->m_approxPixelSize),
    m_isCulled(parent->m_isCulled)
{
    for (unsigned int i = 0; i < 4; ++i)
    {
        m_neighbors[i] = NULL;
        m_children[i] = NULL;
    }

    switch (quadrant)
    {
    case Northeast:
        m_column = parent->m_column * 2 + 1;
        m_row = parent->m_row * 2 + 1;
        m_southwest = parent->m_southwest + Vector2f(m_extent, m_extent);
        break;
    case Northwest:
        m_column = parent->m_column * 2;
        m_row = parent->m_row * 2 + 1;
        m_southwest = parent->m_southwest + Vector2f(0.0f, m_extent);
        break;
    case Southwest:
        m_column = parent->m_column * 2;
        m_row = parent->m_row * 2;
        m_southwest = parent->m_southwest + Vector2f(0.0f, 0.0f);
        break;
    case Southeast:
        m_column = parent->m_column * 2 + 1;
        m_row = parent->m_row * 2;
        m_southwest = parent->m_southwest + Vector2f(m_extent, 0.0f);
        break;
    }

    computeCenterAndRadius(semiAxes);
}


void
QuadtreeTile::tessellate(const Vector3f& eyePosition,
                         const CullingPlaneSet& cullPlanes,
                         const Vector3f& globeSemiAxes,
                         float splitThreshold,
                         float pixelSize)
{
    float tileArc = float(PI) * m_extent;

    // Compute the approximate altitude of the eye point. This is the exact altitude when the
    // world is a sphere, but larger than the actual altitude for other ellipsoids.
    float approxAltitude = eyePosition.norm() - (eyePosition.normalized().cwise() * globeSemiAxes).norm();

    // Compute the approximate projected size of the tile.
    float distanceToTile = max(approxAltitude, (eyePosition - m_center).norm() - m_boundingSphereRadius);
    distanceToTile = max(1.0e-6f, distanceToTile);
    float apparentTileSize = m_boundingSphereRadius / distanceToTile;

    // Compute the approximate projected size of the tile, in pixel
    m_approxPixelSize = apparentTileSize / pixelSize;

    // We may also need to split a tile when the error from approximating a
    // curve as a straight line gets too large. In practice, this is mostly
    // an issue at larger distances.
    //
    // The error expression is derived from formula for the maximum distance
    // from a unit circle to the chord of angle theta: 1 - cos(angle / 2)
    float curveApproxError = globeSemiAxes.maxCoeff() * (1.0f - cos(tileArc * SquareSize * 0.5f));
    float curveErrorPixels = curveApproxError / (distanceToTile * pixelSize);

    // Tessellate when the tile is too large or the curve approximation error is too great
    if (apparentTileSize > splitThreshold || curveErrorPixels > 0.5f)
    {
        // Only split tiles that lie inside the view frustum.
        if (!m_isCulled)
        {
            split(cullPlanes, globeSemiAxes);
            for (unsigned int i = 0; i < 4; ++i)
            {
                m_children[i]->tessellate(eyePosition, cullPlanes, globeSemiAxes, splitThreshold, pixelSize);
            }
        }
    }
}


// Split a tile into four child tiles. If
void
QuadtreeTile::split(const CullingPlaneSet& cullPlanes, const Vector3f& semiAxes)
{
    if (hasChildren())
    {
        return;
    }

    // Split any neighbor tiles are shallower (coarser) than this one. This
    // prevents neighboring tiles from ending up with tessellations that
    // differ by more than one level.
    if (!isRoot())
    {
        for (unsigned int i = 0; i < 4; ++i)
        {
            if (!m_neighbors[i])
            {
                // Parent may not have a neighbor if (and only if!) we're at the edge of the map
                if (m_parent->m_neighbors[i])
                {
                    m_parent->m_neighbors[i]->split(cullPlanes, semiAxes);
                }
            }
        }
    }

    // Create children; mark any that lie outside the view frustum as culled
    for (unsigned int i = 0; i < 4; ++i)
    {
        m_children[i] = m_allocator->newTile(this, Quadrant(i), semiAxes);
        if (!m_isCulled)
        {
            m_children[i]->m_isCulled = m_children[i]->cull(cullPlanes);
        }
    }

    // Link neighbor tiles
    m_children[Northeast]->link(South, m_children[Southeast]);
    m_children[Northwest]->link(South, m_children[Southwest]);
    m_children[Northeast]->link(West, m_children[Northwest]);
    m_children[Southeast]->link(West, m_children[Southwest]);

    // We need to check for the existence of neighbors, as they might be
    // missing for root nodes.
    if (m_neighbors[North])
    {
        m_children[Northeast]->link(North, m_neighbors[North]->m_children[Southeast]);
        m_children[Northwest]->link(North, m_neighbors[North]->m_children[Southwest]);
    }

    if (m_neighbors[West])
    {
        m_children[Northwest]->link(West, m_neighbors[West]->m_children[Northeast]);
        m_children[Southwest]->link(West, m_neighbors[West]->m_children[Southeast]);
    }

    if (m_neighbors[South])
    {
        m_children[Southeast]->link(South, m_neighbors[South]->m_children[Northeast]);
        m_children[Southwest]->link(South, m_neighbors[South]->m_children[Northwest]);
    }

    if (m_neighbors[East])
    {
        m_children[Southeast]->link(East, m_neighbors[East]->m_children[Southwest]);
        m_children[Northeast]->link(East, m_neighbors[East]->m_children[Northwest]);
    }
}


// Return true if this tile lies outside the convex volume given by
// the intersection of half-spaces.
bool
QuadtreeTile::cull(const CullingPlaneSet& cullPlanes) const
{
    // Test the sphere against all of the planes in the cull frustum
    for (unsigned int i = 0; i < 6; ++i)
    {
        if (cullPlanes.planes[i].signedDistance(m_center) < -m_boundingSphereRadius)
        {
            return true;
        }
    }

    return false;
}


void
QuadtreeTile::render(RenderContext& rc, unsigned int features) const
{
    if (!m_isCulled)
    {
        if (hasChildren())
        {
            for (unsigned int i = 0; i < 4; ++i)
            {
                m_children[i]->render(rc, features);
            }
        }
        else
        {
            drawPatch(rc, features);
        }
    }
}


void
QuadtreeTile::render(RenderContext& rc, Material& material, TiledMap* baseTexture) const
{
    if (!m_isCulled)
    {
        if (hasChildren())
        {
            for (unsigned int i = 0; i < 4; ++i)
            {
                m_children[i]->render(rc, material, baseTexture);
            }
        }
        else
        {
            drawPatch(rc, material, baseTexture);
        }
    }
}


void
QuadtreeTile::render(RenderContext& rc, const MapLayer& layer) const
{
    if (!m_isCulled)
    {
        float tileArc = float(PI) * m_extent;
        float lonWest = float(PI) * m_southwest.x();
        float lonEast = lonWest + tileArc;
        float latSouth = float(PI) * m_southwest.y();
        float latNorth = latSouth + tileArc;

        // Check whether map layer lies completely outside this tile
        if (layer.box().west() > lonEast ||
            layer.box().east() < lonWest ||
            layer.box().south() > latNorth ||
            layer.box().north() < latSouth)
        {
            return;
        }

        if (hasChildren())
        {
            for (unsigned int i = 0; i < 4; ++i)
            {
                m_children[i]->render(rc, layer);
            }
        }
        else
        {
            if (layer.box().west() <= lonWest &&
                layer.box().east() >= lonEast &&
                layer.box().south() <= latSouth &&
                layer.box().north() >= latNorth)
            {
                // Draw a complete patch
                drawPatch(rc, layer);
            }
            else
            {
                // Patch isn't completely covered by the map layer; draw
                // only a portion of the patch.
                drawPatch(rc, layer);
            }
        }
    }
}


void
QuadtreeTile::drawPatch(RenderContext& rc, unsigned int features) const
{
    unsigned int vertexStride = 8;
    if ((features & NormalMap) != 0)
    {
        vertexStride = 11;
    }

    const unsigned int vertexCount = (TileSubdivision + 1) * (TileSubdivision + 1);
    const unsigned int triangleCount = TileSubdivision * TileSubdivision * 2;
	
    float *vertexData;
	vertexData = (float *) malloc(sizeof(float)*vertexCount*vertexStride);

    v_uint16 indexData[triangleCount * 3];

    unsigned int vertexIndex = 0;

    float tileArc = float(PI) * m_extent;
    float lonWest = float(PI) * m_southwest.x();
    float latSouth = float(PI) * m_southwest.y();
    float dlon = tileArc / float(TileSubdivision);
    float dlat = tileArc / float(TileSubdivision);
    float du = m_extent / float(TileSubdivision);
    float dv = m_extent / float(TileSubdivision);

    for (unsigned int i = 0; i <= TileSubdivision; ++i)
    {
        float v = m_southwest.y() + i * dv;
        float lat = latSouth + i * dlat;
        float cosLat = cos(lat);
        float sinLat = sin(lat);

        if ((features & NormalMap) != 0)
        {
            for (unsigned int j = 0; j <= TileSubdivision; ++j)
            {
                unsigned int vertexStart = vertexStride * vertexIndex;

                float lon = lonWest + j * dlon;
                float u = m_southwest.x() + j * du;

                float cosLon = cos(lon);
                float sinLon = sin(lon);

                Vector3f p(cosLat * cos(lon), cosLat * sin(lon), sinLat);

                // Position
                vertexData[vertexStart + 0]  = p.x();
                vertexData[vertexStart + 1]  = p.y();
                vertexData[vertexStart + 2]  = p.z();

                // Vertex normal
                vertexData[vertexStart + 3]  = p.x();
                vertexData[vertexStart + 4]  = p.y();
                vertexData[vertexStart + 5]  = p.z();

                // Texture coordinate
                vertexData[vertexStart + 6]  = u * 0.5f + 0.5f;
                vertexData[vertexStart + 7]  = 0.5f - v;

                // Tangent (we use dP/du), where P(u,v) is the sphere parametrization
                vertexData[vertexStart + 8]  = -sinLon;
                vertexData[vertexStart + 9]  = cosLon;
                vertexData[vertexStart + 10] = 0.0f;

                ++vertexIndex;
            }
        }
        else
        {
            for (unsigned int j = 0; j <= TileSubdivision; ++j)
            {
                unsigned int vertexStart = vertexStride * vertexIndex;

                float lon = lonWest + j * dlon;
                float u = m_southwest.x() + j * du;

                Vector3f p(cosLat * cos(lon), cosLat * sin(lon), sinLat);
                vertexData[vertexStart + 0] = p.x();
                vertexData[vertexStart + 1] = p.y();
                vertexData[vertexStart + 2] = p.z();
                vertexData[vertexStart + 3] = p.x();
                vertexData[vertexStart + 4] = p.y();
                vertexData[vertexStart + 5] = p.z();
                vertexData[vertexStart + 6] = u * 0.5f + 0.5f;
                vertexData[vertexStart + 7] = 0.5f - v;

                ++vertexIndex;
            }
        }
    }

    unsigned int triangleIndex = 0;
    for (unsigned int i = 0; i < TileSubdivision; ++i)
    {
        for (unsigned int j = 0; j < TileSubdivision; ++j)
        {
            v_uint16 i00 = v_uint16(i * (TileSubdivision + 1) + j);
            v_uint16 i01 = i00 + 1;
            v_uint16 i10 = i00 + v_uint16(TileSubdivision + 1);
            v_uint16 i11 = i10 + 1;

            indexData[triangleIndex * 3 + 0] = i00;
            indexData[triangleIndex * 3 + 1] = i01;
            indexData[triangleIndex * 3 + 2] = i11;
            ++triangleIndex;

            indexData[triangleIndex * 3 + 0] = i00;
            indexData[triangleIndex * 3 + 1] = i11;
            indexData[triangleIndex * 3 + 2] = i10;
            ++triangleIndex;
        }
    }

    if ((features & NormalMap) != 0)
    {
        rc.bindVertexArray(PositionNormalTexTangent, vertexData, vertexStride * 4);
    }
    else
    {
        rc.bindVertexArray(VertexSpec::PositionNormalTex, vertexData, vertexStride * 4);
    }
    glDrawElements(GL_TRIANGLES, triangleCount * 3, GL_UNSIGNED_SHORT, indexData);

    s_TilesDrawn++;
	
	free(vertexData);
}


void
QuadtreeTile::drawPatch(RenderContext& rc, Material& material, TiledMap* baseMap) const
{
    const unsigned int MaxVertexSize = 11;
    unsigned int vertexStride = 8;

    const unsigned int vertexCount = (TileSubdivision + 1) * (TileSubdivision + 1);
    const unsigned int triangleCount = TileSubdivision * TileSubdivision * 2;

    float vertexData[vertexCount * MaxVertexSize];
    v_uint16 indexData[triangleCount * 3];

    unsigned int vertexIndex = 0;

    float tileArc = float(PI) * m_extent;
    float lonWest = float(PI) * m_southwest.x();
    float latSouth = float(PI) * m_southwest.y();
    float dlon = tileArc / float(TileSubdivision);
    float dlat = tileArc / float(TileSubdivision);

    unsigned int tileSize = 1024;

    unsigned int mapLevel = m_level;
    unsigned int mapColumn = m_column;
    unsigned int mapRow = m_row;

    if (m_approxPixelSize < tileSize)
    {
        unsigned int n = tileSize / m_approxPixelSize;
        while (n > 0 && mapLevel > 0)
        {
            n >>= 1;
            mapColumn >>= 1;
            mapRow >>= 1;
            mapLevel--;
        }
    }

    float u0;
    float v0;
    float du;
    float dv;

    TiledMap::TextureSubrect r = baseMap->tile(mapLevel, mapColumn, mapRow);
    if (mapLevel >= m_level)
    {
        u0 = r.u0;
        v0 = r.v0;
        du = (r.u1 - r.u0) / float(TileSubdivision);
        dv = (r.v1 - r.v0) / float(TileSubdivision);
    }
    else
    {
        unsigned int log2Scale = m_level - mapLevel;
        float scale = 1.0f / float(1 << log2Scale);
        unsigned int mask = (1 << log2Scale) - 1;

        float uExt = (r.u1 - r.u0) * scale;
        float vExt = (r.v1 - r.v0) * scale;

        u0 = r.u0 + uExt * (m_column & mask);
        v0 = r.v0 + vExt * (m_row & mask);
        du = uExt / float(TileSubdivision);
        dv = vExt / float(TileSubdivision);
    }

    for (unsigned int i = 0; i <= TileSubdivision; ++i)
    {
        float v = v0 + i * dv;
        float lat = latSouth + i * dlat;
        float cosLat = cos(lat);
        float sinLat = sin(lat);

        for (unsigned int j = 0; j <= TileSubdivision; ++j)
        {
            unsigned int vertexStart = vertexStride * vertexIndex;

            float lon = lonWest + j * dlon;
            float u = u0 + j * du;

            Vector3f p(cosLat * cos(lon), cosLat * sin(lon), sinLat);
            vertexData[vertexStart + 0] = p.x();
            vertexData[vertexStart + 1] = p.y();
            vertexData[vertexStart + 2] = p.z();
            vertexData[vertexStart + 3] = p.x();
            vertexData[vertexStart + 4] = p.y();
            vertexData[vertexStart + 5] = p.z();
            vertexData[vertexStart + 6] = u;
            vertexData[vertexStart + 7] = 1.0f - v;

            ++vertexIndex;
        }
    }

    unsigned int triangleIndex = 0;
    for (unsigned int i = 0; i < TileSubdivision; ++i)
    {
        for (unsigned int j = 0; j < TileSubdivision; ++j)
        {
            v_uint16 i00 = v_uint16(i * (TileSubdivision + 1) + j);
            v_uint16 i01 = i00 + 1;
            v_uint16 i10 = i00 + v_uint16(TileSubdivision + 1);
            v_uint16 i11 = i10 + 1;

            indexData[triangleIndex * 3 + 0] = i00;
            indexData[triangleIndex * 3 + 1] = i01;
            indexData[triangleIndex * 3 + 2] = i11;
            ++triangleIndex;

            indexData[triangleIndex * 3 + 0] = i00;
            indexData[triangleIndex * 3 + 1] = i11;
            indexData[triangleIndex * 3 + 2] = i10;
            ++triangleIndex;
        }
    }

    material.setBaseTexture(r.texture);
    rc.bindMaterial(&material);
    rc.bindVertexArray(VertexSpec::PositionNormalTex, vertexData, vertexStride * 4);
    glDrawElements(GL_TRIANGLES, triangleCount * 3, GL_UNSIGNED_SHORT, indexData);

    s_TilesDrawn++;
}


inline void setVertex(float* vertexData, const Vector3f& p, float u, float v)
{
    vertexData[0] = p.x();
    vertexData[1] = p.y();
    vertexData[2] = p.z();
    vertexData[3] = p.x();
    vertexData[4] = p.y();
    vertexData[5] = p.z();
    vertexData[6] = u;
    vertexData[7] = 1.0f - v;
}


// Helper function to generate one row of vertices for a map
// layer patch.
static unsigned int
mapLayerRow(const MapLayer& layer,
            float* vertexData,
            unsigned int vertexIndex,
            unsigned int vertexStride,
            float lat, float v,
            float lonWest, float dlon,
            float u0, float du,
            unsigned int startColumn, unsigned int endColumn,
            bool westEdge, bool eastEdge)
{
    float cosLat = cos(lat);
    float sinLat = sin(lat);

    if (westEdge)
    {
        float lon = layer.box().west();
        Vector3f p(cosLat * cos(lon), cosLat * sin(lon), sinLat);
        setVertex(vertexData + vertexStride * vertexIndex, p, 0.0f, v);

        ++vertexIndex;
    }

    for (unsigned int j = startColumn; j <= endColumn; ++j)
    {
        float lon = lonWest + j * dlon;
        float u = u0 + j * du;
        Vector3f p(cosLat * cos(lon), cosLat * sin(lon), sinLat);

        setVertex(vertexData + vertexStride * vertexIndex, p, u, v);

        ++vertexIndex;
    }

    if (eastEdge)
    {
        float lon = layer.box().east();
        Vector3f p(cosLat * cos(lon), cosLat * sin(lon), sinLat);
        setVertex(vertexData + vertexStride * vertexIndex, p, 1.0f, v);

        ++vertexIndex;
    }

    return vertexIndex;
}


// Draw a patch for a map layer. Layers are rendered with exactly the same
// vertices as the base planet surface when possible in order to eliminate
// depth buffer artifacts. Using identical vertex coordinates may not be
// possible at the very edges of map layers.
void
QuadtreeTile::drawPatch(RenderContext& rc, const MapLayer& layer) const
{
    const unsigned int vertexStride = 8;

    const unsigned int maxVertexCount = (TileSubdivision + 1) * (TileSubdivision + 1);
    const unsigned int maxTriangleCount = TileSubdivision * TileSubdivision * 2;
    float vertexData[maxVertexCount * vertexStride];
    v_uint16 indexData[maxTriangleCount * 3];

    float tileArc = float(PI) * m_extent;
    float lonWest = float(PI) * m_southwest.x();
    float latSouth = float(PI) * m_southwest.y();
    float dlon = tileArc / float(TileSubdivision);
    float dlat = tileArc / float(TileSubdivision);

    float layerLonExtent = layer.box().east() - layer.box().west();
    float layerLatExtent = layer.box().north() - layer.box().south();

    // Compute texture coordinate starting points and steps
    float u0 = (lonWest - layer.box().west()) / layerLonExtent;
    float v0 = (latSouth - layer.box().south()) / layerLatExtent;
    float du = tileArc / layerLonExtent / TileSubdivision;
    float dv = tileArc / layerLatExtent / TileSubdivision;

    // Handle map layers that only partially cover the current world patch. westEdge, etc.
    // flags will be set for any edge that crosses the world patch.
    bool westEdge = false;
    bool eastEdge = false;
    bool southEdge = false;
    bool northEdge = false;

    unsigned int startColumn = 0;
    if (lonWest < layer.box().west())
    {
        startColumn = ceil((layer.box().west() - lonWest) / tileArc * TileSubdivision);
        westEdge = true;
    }

    unsigned int endColumn = TileSubdivision;
    if (lonWest + tileArc > layer.box().east())
    {
        endColumn = floor((layer.box().east() - lonWest) / tileArc * TileSubdivision);
        eastEdge = true;
    }

    unsigned int startRow = 0;
    if (latSouth < layer.box().south())
    {
        startRow = ceil((layer.box().south() - latSouth) / tileArc * TileSubdivision);
        southEdge = true;
    }

    unsigned int endRow = TileSubdivision;
    if (latSouth + tileArc > layer.box().north())
    {
        endRow = floor((layer.box().north() - latSouth) / tileArc * TileSubdivision);
        northEdge = true;
    }

    // Generate vertices for the patch
    unsigned int vertexIndex = 0;
    if (southEdge)
    {
        vertexIndex = mapLayerRow(layer,
                                  vertexData, vertexIndex, vertexStride,
                                  layer.box().south(),
                                  0.0f,
                                  lonWest, dlon,
                                  u0, du,
                                  startColumn, endColumn, westEdge, eastEdge);
    }

    for (unsigned int i = startRow; i <= endRow; ++i)
    {
        vertexIndex = mapLayerRow(layer,
                                  vertexData, vertexIndex, vertexStride,
                                  latSouth + i * dlat,
                                  v0 + i * dv,
                                  lonWest, dlon,
                                  u0, du,
                                  startColumn, endColumn, westEdge, eastEdge);
    }

    if (northEdge)
    {
        vertexIndex = mapLayerRow(layer,
                                  vertexData, vertexIndex, vertexStride,
                                  layer.box().north(),
                                  1.0f,
                                  lonWest, dlon,
                                  u0, du,
                                  startColumn, endColumn, westEdge, eastEdge);
    }

    // Compute the total number of vertices and triangles in this patch.
    unsigned int columnCount = endColumn - startColumn;
    unsigned int rowCount = endRow - startRow;
    if (northEdge)
    {
        rowCount++;
    }
    if (southEdge)
    {
        rowCount++;
    }
    if (westEdge)
    {
        columnCount++;
    }
    if (eastEdge)
    {
        columnCount++;
    }

    // Generate indices for the patch
    unsigned int triangleIndex = 0;
    for (unsigned int i = 0; i < rowCount; ++i)
    {
        for (unsigned int j = 0; j < columnCount; ++j)
        {
            v_uint16 i00 = v_uint16(i * (columnCount + 1) + j);
            v_uint16 i01 = i00 + 1;
            v_uint16 i10 = i00 + v_uint16(columnCount + 1);
            v_uint16 i11 = i10 + 1;

            indexData[triangleIndex * 3 + 0] = i00;
            indexData[triangleIndex * 3 + 1] = i01;
            indexData[triangleIndex * 3 + 2] = i11;
            ++triangleIndex;

            indexData[triangleIndex * 3 + 0] = i00;
            indexData[triangleIndex * 3 + 1] = i11;
            indexData[triangleIndex * 3 + 2] = i10;
            ++triangleIndex;
        }
    }

    // Draw the triangles
    unsigned int triangleCount = rowCount * columnCount * 2;
    if (triangleCount > 0)
    {
        rc.bindVertexArray(VertexSpec::PositionNormalTex, vertexData, vertexStride * sizeof(float));
        glDrawElements(GL_TRIANGLES, triangleCount * 3, GL_UNSIGNED_SHORT, indexData);
    }
}


void
QuadtreeTile::computeCenterAndRadius(const Vector3f& semiAxes)
{
    float tileArc = float(PI) * m_extent;

    float lonWest = float(PI) * m_southwest.x();
    float latSouth = float(PI) * m_southwest.y();
    float lonCenter = lonWest + tileArc * 0.5f;
    float latCenter = latSouth + tileArc * 0.5f;

    // Compute the center of the tile.
    float cosCenterLat = cos(latCenter);
    Vector3f unitCenter(cosCenterLat * cos(lonCenter), cosCenterLat * sin(lonCenter), sin(latCenter));
    m_center = unitCenter.cwise() * semiAxes;

    // Compute the radius of a sphere large enough to contain this tile
    float cornerLat = latSouth >= 0.0f ? latSouth : latSouth + tileArc;
    float cosCornerLat = cos(cornerLat);
    Vector3f corner(cosCornerLat * cos(lonWest), cosCornerLat * sin(lonWest), sin(cornerLat));
    corner = corner.cwise() * semiAxes;
    m_boundingSphereRadius = (corner - m_center).norm();
}


WorldGeometry::WorldGeometry() :
    m_emissive(false),
    m_specularReflectance(Spectrum(0.0f, 0.0f, 0.0f)),
    m_specularPower(20.0f),
    m_cloudAltitude(0.0f),
    m_tileAllocator(NULL)
{
    setClippingPolicy(Geometry::PreventClipping);
    m_material = new Material();
    m_material->setDiffuse(Spectrum(1.0f, 1.0f, 1.0f));

    m_tileAllocator = new QuadtreeTileAllocator;
}


WorldGeometry::~WorldGeometry()
{
    delete m_tileAllocator;
}


void
WorldGeometry::render(RenderContext& rc,
                      float cameraDistance,
                      double t) const
{
    // Determine the level of detail
    float radius = maxRadius();
    float cameraAltitude = cameraDistance;
    float projectedSizeInPixels = (radius / rc.pixelSize()) / max(cameraAltitude, radius * 0.001f);
    int subdivisions = (int) max(8.0f, min(50.0f, projectedSizeInPixels / 20));

    // Get the position of the eye in model coordinates *before* scaling
    Transform3f invModelView = Transform3f(rc.modelview().inverse());
    Vector3f eyePosition = invModelView * Vector3f::Zero();

    // Calculate the horizon distance; we'll just approximate this for non-spherical
    // bodies, using an estimate that will always be greater than or equal to the
    // actual horizon distance (thus ensuring that we don't clip anything that should
    // be visible.
    float approxAltitude = eyePosition.norm() - m_ellipsoidAxes.minCoeff() * 0.5f;
    float horizonDistance = 0.0f;
    if (approxAltitude > 0.0f)
    {
        float r = m_ellipsoidAxes.maxCoeff() * 0.5f;
        horizonDistance = sqrt((2 * r + approxAltitude) * approxAltitude);
    }

    // Compute the culling planes. Use the horizon distance for the far plane in order
    // to cull as many surface patches as possible.
    Frustum viewFrustum = rc.frustum();
    float farDistance = max(viewFrustum.nearZ, min(horizonDistance, viewFrustum.farZ));
    Matrix4f modelviewTranspose = rc.modelview().matrix().transpose();
    CullingPlaneSet cullingPlanes;
    for (unsigned int i = 0; i < 4; ++i)
    {
        cullingPlanes.planes[i] = Hyperplane<float, 3>(viewFrustum.planeNormals[i].cast<float>(), 0.0f);
        cullingPlanes.planes[i].coeffs() = modelviewTranspose * cullingPlanes.planes[i].coeffs();
    }
    cullingPlanes.planes[4].coeffs() = modelviewTranspose * Vector4f(0.0f, 0.0f, -1.0f, -viewFrustum.nearZ);
    cullingPlanes.planes[5].coeffs() = modelviewTranspose * Vector4f(0.0f, 0.0f,  1.0f, farDistance);

    rc.pushModelView();
    rc.scaleModelView(m_ellipsoidAxes * 0.5f);

    // Enable normal maps when a normal texture has been set and the
    // render context supports shaders.
    bool useNormalTexture = false;
    if (!m_normalMap.isNull() && rc.shaderCapability() != RenderContext::FixedFunction)
    {
        useNormalTexture = true;
    }

    unsigned int tileFeatures = 0;

    // Set the vertex information directly; we should change this so that
    // we're using a vertex array instead of immediate mode rendering.
    if (m_emissive)
    {
        // Switch to unlit rendering by disabling surface normals
        // required for lighting.
        rc.setVertexInfo(VertexSpec::PositionTex);
    }
    else if (useNormalTexture)
    {
        rc.setVertexInfo(PositionNormalTexTangent);
        tileFeatures |= QuadtreeTile::NormalMap;
    }
    else
    {
        rc.setVertexInfo(VertexSpec::PositionNormalTex);
    }

    RenderContext::ScatteringParameters scatteringParams;

    float atmosphereHeight = 0.0f;

    if (!m_atmosphere.isNull())
    {
        float r = maxRadius();

        atmosphereHeight = m_atmosphere->transparentHeight() / r;

        scatteringParams.planetRadius = 1.0f;
        scatteringParams.atmosphereRadius = 1.0f + atmosphereHeight;
        scatteringParams.rayleighScaleHeight = m_atmosphere->rayleighScaleHeight() / r;
        scatteringParams.rayleighCoeff = m_atmosphere->rayleighScatteringCoeff() * r * 1000.0f;
        scatteringParams.color = m_atmosphere->color(100000.0f);
        scatteringParams.mieAsymmetry = m_atmosphere->mieAsymmetry();
        scatteringParams.transmittanceTexture = m_atmosphere->transmittanceTexture();
        scatteringParams.scatterTexture = m_atmosphere->scatterTexture();
        rc.setScatteringParameters(scatteringParams);
        rc.setScattering(true);
    }

    rc.setSphericalGeometryHint(true);

    Material material = *m_material;
    material.setBaseTexture(m_baseMap.ptr());
    material.setSpecular(m_specularReflectance);
    material.setPhongExponent(m_specularPower);
    material.setSpecularModifier(Material::DiffuseTextureAlpha);

    if (useNormalTexture)
    {
        material.setNormalTexture(m_normalMap.ptr());
        rc.bindMaterial(&material);
#if !USE_QUADTREE
        renderNormalMappedSphere(subdivisions);
#endif
    }
    else
    {
        rc.bindMaterial(&material);
#if !USE_QUADTREE
        renderSphere(subdivisions);
#endif
    }

#if USE_QUADTREE
    // Create the root quadtree nodes. Presently, we always start with two root
    // tiles: one for the western hemisphere and one for the eastern hemisphere.
    // But, depending on what sort of tiles we have, a different set of root
    // tiles might be more appropriate.
    Vector3f semiAxes = m_ellipsoidAxes * 0.5f;

    m_tileAllocator->clear();
    QuadtreeTile* westHemi = m_tileAllocator->newRootTile(0, 0, Vector2f(-1.0f, -0.5f), 1.0f, semiAxes);
    QuadtreeTile* eastHemi = m_tileAllocator->newRootTile(0, 1, Vector2f( 0.0f, -0.5f), 1.0f, semiAxes);

    // Set up the neighbor connections for the root nodes. Since the map wraps,
    // the eastern hemisphere is both the east and west neighbor of the western
    // hemisphere (and vice versa.) There are no north and south neighbors.
    westHemi->setNeighbor(QuadtreeTile::West, eastHemi);
    westHemi->setNeighbor(QuadtreeTile::East, eastHemi);
    eastHemi->setNeighbor(QuadtreeTile::West, westHemi);
    eastHemi->setNeighbor(QuadtreeTile::East, westHemi);

    // TODO: Consider map tile resolution when setting the split threshold
    float splitThreshold = rc.pixelSize() * MaxTileSquareSize * TileSubdivision;
    westHemi->tessellate(eyePosition, cullingPlanes, semiAxes, splitThreshold, rc.pixelSize());
    eastHemi->tessellate(eyePosition, cullingPlanes, semiAxes, splitThreshold, rc.pixelSize());

    s_TilesDrawn = 0;
    if (m_baseTiledMap.isNull())
    {
        westHemi->render(rc, tileFeatures);
        eastHemi->render(rc, tileFeatures);
    }
    else
    {
        westHemi->render(rc, material, m_baseTiledMap.ptr());
        eastHemi->render(rc, material, m_baseTiledMap.ptr());
    }
#endif

    if (!m_layers.empty())
    {
        // Enable polygon offset to ensure that layers are rendered on top of the
        // base planet geometry.
        // TODO: Investigate replacing this with a projection matrix adjustment;
        // glPolygonOffset can interfere with the performance of GPUs that have
        // hiearchical z-buffer optimizations.
        glEnable(GL_POLYGON_OFFSET_FILL);
        glPolygonOffset(-3.0f, 0.0f);

        // Add a scale factor to prevent depth buffer artifacts. The scale factor
        // is dependent on the projected size of the planet sphere.
        rc.pushModelView();

#if !USE_QUADTREE
        float patchScale = std::max(1.001f, (projectedSizeInPixels + 1) / projectedSizeInPixels);
        rc.scaleModelView(Vector3f::Constant(patchScale));
#endif

        Material simpleMaterial;
        simpleMaterial.setDiffuse(Spectrum(1.0f, 1.0f, 1.0f));
        for (unsigned int layerIndex = 0; layerIndex < m_layers.size(); ++layerIndex)
        {
            const MapLayer* layer = m_layers[layerIndex].ptr();
            if (layer && layer->opacity() > 0.0f)
            {
                TextureMap* texture = layer->texture();
                if (texture)
                {
                    simpleMaterial.setOpacity(layer->opacity());
                    simpleMaterial.setBaseTexture(texture);
                    rc.bindMaterial(&simpleMaterial);

#if USE_QUADTREE
                    westHemi->render(rc, *layer);
                    eastHemi->render(rc, *layer);
#else
                    renderPatch(subdivisions, layer->box());
#endif
                }
            }
        }
        rc.popModelView();

        glDisable(GL_POLYGON_OFFSET_FILL);
    }

    if (!m_cloudMap.isNull())
    {
        float scale = 1.0f + m_cloudAltitude / maxRadius();

        rc.pushModelView();
        rc.scaleModelView(Vector3f::Constant(scale));

        Material cloudMaterial;
        cloudMaterial.setOpacity(1.0f);
        cloudMaterial.setBlendMode(Material::AlphaBlend);
        cloudMaterial.setDiffuse(Spectrum(1.0f, 1.0f, 1.0f));
        cloudMaterial.setBaseTexture(m_cloudMap.ptr());
        rc.bindMaterial(&cloudMaterial);

        // Draw the inside of the cloud layer if the viewer is below the clouds.
        // Instead of the actual viewer height above the surface, use the distance
        // along a line through the center of the planet ellipsoid. This is correct
        // since the cloud geometry is drawn as a scaled ellipsoid rather than at
        // constant height above the planet.
        float ellipDistance = (rc.modelTranslation().cast<float>().cwise() * (m_ellipsoidAxes * (0.5f / radius))).norm();
        if (ellipDistance < m_cloudAltitude + radius)
        {
            glCullFace(GL_FRONT);
        }

        // Only draw the cloud layer if the cloud texture is resident; otherwise, the cloud
        // layer is drawn as an opaque shell until texture loading is complete.
        m_cloudMap->makeResident();
        if (m_cloudMap->isResident())
        {
            renderSphere(max(int(TileSubdivision * 2), subdivisions));
        }
        glCullFace(GL_BACK);

        rc.popModelView();
    }

    // Draw the atmosphere as a pixel shaded 'shell'
    if (!m_atmosphere.isNull())
    {
        // Scale the scattering parameters as well as the geometry
        float scale = 1.0f + atmosphereHeight;
        scatteringParams.planetRadius /= scale;
        scatteringParams.atmosphereRadius /= scale;
        scatteringParams.rayleighScaleHeight /= scale;
        scatteringParams.rayleighCoeff *= scale;
        rc.setScatteringParameters(scatteringParams);

        glCullFace(GL_FRONT);
        rc.pushModelView();
        rc.scaleModelView(Vector3f::Constant(scale));

        // Atmosphere rendering benefits greatly from sRGB gamma correction; enable this
        // setting eventually:
        // if (GLEW_EXT_framebuffer_sRGB)
        //     glEnable(GL_FRAMEBUFFER_SRGB_EXT);
        Material atmosphereMaterial;
        atmosphereMaterial.setOpacity(0.0f);
        atmosphereMaterial.setBlendMode(Material::PremultipliedAlphaBlend);
        rc.bindMaterial(&atmosphereMaterial);
        renderSphere(subdivisions);

        rc.popModelView();
        glCullFace(GL_BACK);
        // if (GLEW_EXT_framebuffer_sRGB)
        //     glDisable(GL_FRAMEBUFFER_SRGB_EXT);

        rc.setScattering(false);
    }

    rc.setSphericalGeometryHint(false);

#if USE_QUADTREE && DEBUG_QUADTREE
    VESTA_LOG("Tiles drawn: %d (of %d allocated)", s_TilesDrawn, m_tileAllocator->tileCount());

    rc.setVertexInfo(VertexSpec::Position);
    rc.pushModelView();

    Material highlight;
    highlight.setDiffuse(Spectrum(0.0f, 1.0f, 0.0f));
    highlight.setOpacity(0.3f);
    rc.bindMaterial(&highlight);

    rc.setModelView(Matrix4f::Identity());
    rc.pushProjection();
    rc.orthographicProjection2D(-1.0f, 1.0f, -1.0f, 1.0f);
    glDisable(GL_DEPTH_TEST);

    float minExtent = 1.0f;
    for (QuadtreeTileAllocator::TileArray::const_iterator iter = m_tileAllocator->tiles().begin();
         iter != m_tileAllocator->tiles().end(); ++iter)
    {
        if (!iter->hasChildren() && !iter->isCulled())
        {
            glBegin(GL_LINE_LOOP);

            Vector3f p0(iter->southwest().x(), iter->southwest().y(), 0.0f);
            Vector3f p1(iter->southwest().x() + iter->extent(), iter->southwest().y(), 0.0f);
            Vector3f p2(iter->southwest().x() + iter->extent(), iter->southwest().y() + iter->extent(), 0.0f);
            Vector3f p3(iter->southwest().x(), iter->southwest().y() + iter->extent(), 0.0f);

            glVertex3fv(p0.data());
            glVertex3fv(p1.data());
            glVertex3fv(p2.data());
            glVertex3fv(p3.data());

            glEnd();

            minExtent = min(minExtent, iter->extent());
        }
    }

    glEnable(GL_DEPTH_TEST);
    rc.popProjection();

    rc.popModelView();
#endif // DEBUG_QUADTREE

    rc.popModelView();

    if (m_ringSystem.isValid())
    {
        m_ringSystem->render(rc, cameraDistance, t);
    }
}


void
WorldGeometry::renderSphere(int subdivisions) const
{
    float lastSinPhi = -1.0f;
    float lastCosPhi = 0.0f;
    float lastTexT = 1.0f;

    // Render latitudinal bands
    for (int band = -subdivisions + 1; band <= subdivisions; band++)
    {
        float t = (float) (band) / (float) subdivisions;
        float phi = t * (float) PI / 2;
        float sinPhi = std::sin(phi);
        float cosPhi = std::cos(phi);

        float texT = (1.0f - t) * 0.5f;

        glBegin(GL_QUAD_STRIP);

        for (int slice = 0; slice < subdivisions * 4; slice++)
        {
            float u = (float) slice / (float) (subdivisions * 4);
            float theta = u * 2.0f * (float) PI;
            float sinTheta = std::sin(theta);
            float cosTheta = std::cos(theta);
            float texS = u;

            Vector3f v0(lastCosPhi * cosTheta, lastCosPhi * sinTheta, lastSinPhi);
            Vector3f v1(cosPhi * cosTheta,     cosPhi * sinTheta, sinPhi);

            glTexCoord2f(texS, texT);
            glNormal3fv(v1.data());
            glVertex3fv(v1.data());
            glTexCoord2f(texS, lastTexT);
            glNormal3fv(v0.data());
            glVertex3fv(v0.data());
        }

        Vector3f v0(lastCosPhi, 0.0f, lastSinPhi);
        Vector3f v1(cosPhi,     0.0f, sinPhi);
        glTexCoord2f(1.0f, texT);
        glNormal3fv(v1.data());
        glVertex3fv(v1.data());
        glTexCoord2f(1.0f, lastTexT);
        glNormal3fv(v0.data());
        glVertex3fv(v0.data());

        glEnd();

        lastSinPhi = sinPhi;
        lastCosPhi = cosPhi;
        lastTexT = texT;
    }
}


void
WorldGeometry::renderNormalMappedSphere(int subdivisions) const
{
    float lastSinPhi = -1.0f;
    float lastCosPhi = 0.0f;
    float lastTexT = 1.0f;
    int tangentLocation = ShaderBuilder::TangentAttributeLocation;

    // Render latitudinal bands
    for (int band = -subdivisions + 1; band <= subdivisions; band++)
    {
        float t = (float) (band) / (float) subdivisions;
        float phi = t * (float) PI / 2;
        float sinPhi = std::sin(phi);
        float cosPhi = std::cos(phi);

        float texT = (1.0f - t) * 0.5f;

        glBegin(GL_QUAD_STRIP);

        for (int slice = 0; slice < subdivisions * 4; slice++)
        {
            float u = (float) slice / (float) (subdivisions * 4);
            float theta = u * 2.0f * (float) PI;
            float sinTheta = std::sin(theta);
            float cosTheta = std::cos(theta);
            float texS = u;

            Vector3f v0(lastCosPhi * cosTheta, lastCosPhi * sinTheta, lastSinPhi);
            Vector3f v1(cosPhi * cosTheta,     cosPhi * sinTheta, sinPhi);

            glTexCoord2f(texS, texT);
            glNormal3fv(v1.data());
            glVertexAttrib3f(tangentLocation, -sinTheta, cosTheta, 0.0f);
            glVertex3fv(v1.data());
            glTexCoord2f(texS, lastTexT);
            glNormal3fv(v0.data());
            glVertexAttrib3f(tangentLocation, -sinTheta, cosTheta, 0.0f);
            glVertex3fv(v0.data());
        }

        Vector3f v0(lastCosPhi, 0.0f, lastSinPhi);
        Vector3f v1(cosPhi,     0.0f, sinPhi);
        glTexCoord2f(1.0f, texT);
        glNormal3fv(v1.data());
        glVertexAttrib3f(tangentLocation, 0.0f, 1.0f, 0.0f);
        glVertex3fv(v1.data());
        glTexCoord2f(1.0f, lastTexT);
        glNormal3fv(v0.data());
        glVertexAttrib3f(tangentLocation, 0.0f, 1.0f, 0.0f);
        glVertex3fv(v0.data());

        glEnd();

        lastSinPhi = sinPhi;
        lastCosPhi = cosPhi;
        lastTexT = texT;
    }
}


void
WorldGeometry::renderBand(int subdivisions,
                          double latStart,
                          double latEnd,
                          double lonStart,
                          double lonEnd,
                          float tStart,
                          float tEnd) const
{
    double lonStep = PI / (subdivisions * 2);
    float invLonRange = (float) (1.0 / (lonEnd - lonStart));
    int startLonStep = (int) floor(lonStart / lonStep) + 1;
    int endLonStep = (int) floor(lonEnd / lonStep);
    float sinLatStart = (float) std::sin(latStart);
    float cosLatStart = (float) std::cos(latStart);
    float sinLatEnd = (float) std::sin(latEnd);
    float cosLatEnd = (float) std::cos(latEnd);

    glBegin(GL_QUAD_STRIP);

    float sinLon = float(std::sin(lonStart));
    float cosLon = float(std::cos(lonStart));
    Vector3f v0(cosLatStart * cosLon, cosLatStart * sinLon, sinLatStart);
    Vector3f v1(cosLatEnd * cosLon,   cosLatEnd * sinLon,   sinLatEnd);
    glTexCoord2f(0.0f, 1.0f - tEnd);
    glNormal3fv(v1.data());
    glVertex3fv(v1.data());
    glTexCoord2f(0.0f, 1.0f - tStart);
    glNormal3fv(v0.data());
    glVertex3fv(v0.data());

    for (int i = startLonStep; i < endLonStep; ++i)
    {
        float lon = (float) (i * lonStep);

        sinLon = std::sin(lon);
        cosLon = std::cos(lon);
        float texS = (lon - (float) lonStart) * invLonRange;

        v0 = Vector3f(cosLatStart * cosLon, cosLatStart * sinLon, sinLatStart);
        v1 = Vector3f(cosLatEnd * cosLon,   cosLatEnd * sinLon,   sinLatEnd);

        glTexCoord2f(texS, 1.0f - tEnd);
        glNormal3fv(v1.data());
        glVertex3fv(v1.data());
        glTexCoord2f(texS, 1.0f - tStart);
        glNormal3fv(v0.data());
        glVertex3fv(v0.data());
    }

    sinLon = float(std::sin(lonEnd));
    cosLon = float(std::cos(lonEnd));
    v0 = Vector3f(cosLatStart * cosLon, cosLatStart * sinLon, sinLatStart);
    v1 = Vector3f(cosLatEnd * cosLon,   cosLatEnd * sinLon,   sinLatEnd);
    glTexCoord2f(1.0f, 1.0f - tEnd);
    glNormal3fv(v1.data());
    glVertex3fv(v1.data());
    glTexCoord2f(1.0f, 1.0f - tStart);
    glNormal3fv(v0.data());
    glVertex3fv(v0.data());

    glEnd();
}


void
WorldGeometry::renderPatch(int subdivisions,
                           const MapLayerBounds& box) const
{
    double latStep = PI / (subdivisions * 2);
    float invLatRange = (float) (1.0 / (box.north() - box.south()));
    int startLatStep = (int) floor(box.south() / latStep) + 1;
    int endLatStep   = (int) floor(box.north() / latStep);
    float west = (float) (box.west() + PI);
    float east = (float) (box.east() + PI);

    // Render latitudinal bands
    if (startLatStep > endLatStep)
    {
    }
    else
    {
        renderBand(subdivisions,
                   (float) box.south(), (float) (startLatStep * latStep), west, east,
                   0.0f, float(startLatStep * latStep - (float) box.south()) * invLatRange);
        for (int band = startLatStep; band < endLatStep; ++band)
        {
            float latStart = (float) (band * latStep);
            float latEnd = (float) ((band + 1) * latStep);
            renderBand(subdivisions,
                       latStart, latEnd, west, east,
                       (latStart - (float) box.south()) * invLatRange, (latEnd - (float) box.south()) * invLatRange);
        }
        renderBand(subdivisions,
                   (float) (endLatStep * latStep), (float) box.north(), west, east,
                   float(endLatStep * latStep - (float) box.south()) * invLatRange, 1.0f);
    }

}


float
WorldGeometry::boundingSphereRadius() const
{
    float r = maxRadius();

    float atmosphereHeight = 0.0f;
    if (m_atmosphere.isValid())
    {
        atmosphereHeight = m_atmosphere->transparentHeight();
    }

    if (m_cloudMap.isValid())
    {
        atmosphereHeight = max(atmosphereHeight, m_cloudAltitude);
    }

    float boundingRadius = r + atmosphereHeight;
    if (m_ringSystem.isValid())
    {
        boundingRadius = max(boundingRadius, m_ringSystem->outerRadius());
    }

    return boundingRadius;
}


float
WorldGeometry::nearPlaneDistance(const Eigen::Vector3f& cameraPosition) const
{
    // Use a custom calculation for the near plane distance. We're concerned about
    // clipping the main planet geometry and not as worried about the rings,
    // atmosphere, and cloud layer.
    // TODO: We should compute the distance to the planet ellipsoid (and eventually
    // the terrain model), not just the bounding sphere.
    float nearDistance = cameraPosition.norm() - maxRadius();
    if (m_ringSystem.isValid())
    {
        // Avoid near clipping of the rings; calculate the distance from the viewer
        // to the ring geometry. CameraPosition is in local coordinates, so
        // so |cameraPosition.z| is the distance to the ring plane.
        float ringPlaneDistance = abs(cameraPosition.z());

        // Calculate the distance between the rings and the projection of the camera
        // position onto the ring plane.
        Vector2f ringPlanePos(cameraPosition.x(), cameraPosition.y());
        float r = ringPlanePos.norm();
        float inPlaneDistance = 0.0f;
        if (r > m_ringSystem->outerRadius())
        {
            inPlaneDistance = r - m_ringSystem->outerRadius();
        }
        else
        {
            inPlaneDistance = m_ringSystem->innerRadius() - r;
        }

        float distanceToRings = max(1.0f, max(ringPlaneDistance, inPlaneDistance));
        nearDistance = min(nearDistance, distanceToRings);
    }

    return nearDistance;
}


/** Set the shape of the world to be a perfect sphere with the specified
  * radius.
  */
void
WorldGeometry::setSphere(float radius)
{
    m_ellipsoidAxes.fill(radius * 2.0f);
}


/** Set the shape to be an spheroid with distinct polar and equatorial
  * radii. The equatorial radius is given by radius, and the polar radius
  * is radius * (1 - oblateness). When oblateness is zero, the body will
  * be perfectly spherical; for oblateness > 0, it will be an oblate spheroid.
  */
void
WorldGeometry::setSpheroid(float radius, float oblateness)
{
    m_ellipsoidAxes = Vector3f(radius * 2.0f, radius * 2.0f, radius * (1.0f - oblateness) * 2.0f);
}


/** Set the shape to be triaxial ellipsoid with the specified
  * axis lengths.
  */
void
WorldGeometry::setEllipsoid(const Vector3f& axes)
{
    m_ellipsoidAxes = axes;
}


/** Set the global base texture.
  */
void
WorldGeometry::setBaseMap(TextureMap* baseMap)
{
    m_baseMap = baseMap;
}


/** Set a tiled map as the global base texture.
  */
void
WorldGeometry::setBaseMap(TiledMap* baseMap)
{
    m_baseTiledMap = baseMap;
}


/** Set the global normal map for this world.
  */
void
WorldGeometry::setNormalMap(TextureMap* normalMap)
{
    m_normalMap = normalMap;
}


/** Add a new map layer. The new layer is appended to the list layers and will
  * be drawn on top of previously added layers.
  */
void
WorldGeometry::addLayer(MapLayer* layer)
{
    m_layers.push_back(counted_ptr<MapLayer>(layer));
}


/** Remove the topmost map layer. The method has no effect if there are no
  * map layers.
  */
void
WorldGeometry::removeLayer()
{
    if (!m_layers.empty())
    {
        m_layers.pop_back();
    }
}


/** Remove the map layer at the specified index. The method has no effect if
  * the index is invalid.
  */
void
WorldGeometry::removeLayer(unsigned int index)
{
    if (index < m_layers.size())
    {
        m_layers.erase(m_layers.begin() + index);
    }
}


/** Get the map layer at the specified index. If the index is out
  * of range, the method returns null.
  */
MapLayer*
WorldGeometry::layer(unsigned int index) const
{
    if (index >= m_layers.size())
    {
        return 0;
    }
    else
    {
        return m_layers[index].ptr();
    }
}


/** Get the number of map layers (not including the base.)
  */
unsigned int
WorldGeometry::layerCount() const
{
    return m_layers.size();
}


Atmosphere*
WorldGeometry::atmosphere() const
{
    return m_atmosphere.ptr();
}


/** Set the atmosphere for this world. The atmosphere may be null
  * for worlds without an atmosphere. Note that older graphics hardware
  * may not be capable of rendering atmospheres.
  */
void
WorldGeometry::setAtmosphere(Atmosphere* atmosphere)
{
    m_atmosphere = atmosphere;
}


/** Set the texture map used for the cloud layer. Clouds are only drawn
  * when a cloud map has been assigned.
  */
void
WorldGeometry::setCloudMap(TextureMap* cloudMap)
{
    m_cloudMap = cloudMap;
}


/** Set the ring system. Setting it to null indicates that the planet has no
  * ring system (the default state.)
  */
void
WorldGeometry::setRingSystem(PlanetaryRings* rings)
{
    m_ringSystem = rings;
}


bool
WorldGeometry::handleRayPick(const Eigen::Vector3d& /* pickOrigin */,
                            const Eigen::Vector3d& /* pickDirection */,
                            double* /* distance */) const
{
    // TODO: handle ellipsoidal bodies

    return true;
}
