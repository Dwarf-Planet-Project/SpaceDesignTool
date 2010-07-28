/*
 * $Revision: 369 $ $Date: 2010-07-19 19:13:37 -0700 (Mon, 19 Jul 2010) $
 *
 * Copyright by Astos Solutions GmbH, Germany
 *
 * this file is published under the Astos Solutions Free Public License
 * For details on copyright and terms of use see 
 * http://www.astos.de/Astos_Solutions_Free_Public_License.html
 */

#include "Submesh.h"
#include "Debug.h"
#include <algorithm>
#include <exception>
#include <cmath>
#include <cassert>

using namespace vesta;
using namespace Eigen;
using namespace std;


Submesh::Submesh(VertexArray* vertices) :
    m_vertices(vertices)
{
    m_boundingBox = vertices->computeBoundingBox();
    m_boundingSphereRadius = vertices->computeBoundingSphereRadius();
}


Submesh::~Submesh()
{
    delete m_vertices;
    for (vector<PrimitiveBatch*>::iterator iter = m_primitiveBatches.begin(); iter != m_primitiveBatches.end(); ++iter)
    {
        delete *iter;
    }
}


/** Add a primitive batch to this submesh. The material referenced by the specified
  * material index will be applied to all the primitives in the batch.
  */
void
Submesh::addPrimitiveBatch(PrimitiveBatch* batch, unsigned int materialIndex)
{
    m_primitiveBatches.push_back(batch);
    m_materials.push_back(materialIndex);

#if 0
    // Code to compute the bounding sphere radius based only on
    // vertices referenced in primitive batches. Disabled now because
    // the extra work unnecessary for most meshesh, which have extra
    // vertices stripped out.
    float maxDistSquared = 0.0f;

    if (batch->isIndexed())
    {
        if (batch->indexSize() == PrimitiveBatch::Index16)
        {
            const v_uint16* indices = reinterpret_cast<const v_uint16*>(batch->indexData());
            for (unsigned i = 0; i < batch->indexCount(); ++i)
            {
                Vector3f p = m_vertices->position(indices[i]);
                maxDistSquared = std::max(maxDistSquared, p.squaredNorm());
            }
        }
        else
        {
            const v_uint32* indices = reinterpret_cast<const v_uint32*>(batch->indexData());
            for (unsigned i = 0; i < batch->indexCount(); ++i)
            {
                Vector3f p = m_vertices->position(indices[i]);
                maxDistSquared = std::max(maxDistSquared, p.squaredNorm());
            }
        }
    }
    else
    {
        for (unsigned int i = 0; i < batch->indexCount(); ++i)
        {
            Vector3f p = m_vertices->position(batch->firstVertex() + i);
            maxDistSquared = std::max(maxDistSquared, p.squaredNorm());
        }
    }

    m_boundingSphereRadius = std::max(m_boundingSphereRadius, std::sqrt(maxDistSquared));
#endif
}


/** Merge a list of submeshes to create a single submesh. All submeshes must share
  * the same vertex spec. There must be at least one submesh in the list to merge.
  *
  * \return the new submesh, or null if there was an error creating the submesh.
  */
Submesh*
Submesh::mergeSubmeshes(const std::vector<Submesh*>& submeshes)
{
    if (submeshes.empty())
    {
        return NULL;
    }

    const VertexSpec& vertexSpec = submeshes.front()->vertices()->vertexSpec();
    const unsigned int vertexStride = submeshes.front()->vertices()->stride();

    // Verify that the strides and vertex specs of all submeshes match
    unsigned int vertexCount = 0;
    for (vector<Submesh*>::const_iterator iter = submeshes.begin(); iter != submeshes.end(); ++iter)
    {
        Submesh* s = *iter;
        if (s->vertices()->vertexSpec() != vertexSpec || s->vertices()->stride() != vertexStride)
        {
            VESTA_WARNING("MergeSubmeshes attempted on incompatible submeshes.");
            return false;
        }

        vertexCount += s->vertices()->count();
    }

    // Create a new vertex array large enough to contain all of the submeshes
    unsigned int vertexDataSize = vertexCount * vertexStride;

    Submesh* submesh = NULL;
    char* vertexData = NULL;
    VertexArray* vertexArray = NULL;
    try
    {
        vertexData = new char[vertexDataSize];
        vertexArray = new VertexArray(vertexData, vertexCount, vertexSpec, vertexStride);
        submesh = new Submesh(vertexArray);
    }
    catch (bad_alloc&)
    {
        VESTA_WARNING("Out of memory during submesh merge.");
        if (vertexArray)
        {
            // Deleting the vertex array takes care of the vertexData too
            delete vertexArray;
        }
        else if (vertexData)
        {
            delete[] vertexData;
        }
        return NULL;
    }

    unsigned int vertexDataOffset = 0;

    // Copy vertices from the submeshes in the merge list to the new vertex array
    for (vector<Submesh*>::const_iterator iter = submeshes.begin(); iter != submeshes.end(); ++iter)
    {
        Submesh* s = *iter;
        unsigned int submeshVertexDataSize = vertexStride * s->vertices()->count();
        assert(vertexDataOffset + submeshVertexDataSize <= vertexDataSize);

        copy(reinterpret_cast<const char*>(s->vertices()->data()),
             reinterpret_cast<const char*>(s->vertices()->data()) + submeshVertexDataSize,
             vertexData + vertexDataOffset);
        vertexDataOffset += submeshVertexDataSize;
    }

    // Copy materials and primitive batches from submeshes in the merge list
    try
    {
        unsigned int vertexOffset = 0;
        for (vector<Submesh*>::const_iterator iter = submeshes.begin(); iter != submeshes.end() && submesh != NULL; ++iter)
        {
            Submesh* s = *iter;

            assert(s->materials().size() == s->primitiveBatches().size());
            for (unsigned int i = 0; i < s->primitiveBatchCount(); ++i)
            {
                const PrimitiveBatch* prims = s->primitiveBatches().at(i);
                PrimitiveBatch* newPrims = new PrimitiveBatch(*prims);
                if (vertexOffset != 0)
                {
                    if (!newPrims->offsetIndices(vertexOffset))
                    {
                        delete submesh;
                        submesh = NULL;
                        break;
                    }
                }

                submesh->addPrimitiveBatch(newPrims, s->materials().at(i));
            }

            vertexOffset += s->vertices()->count();
        }
    }
    catch (bad_alloc&)
    {
        VESTA_WARNING("Out of memory during submesh merge.");
        delete submesh;
        submesh = NULL;
    }

    return submesh;
}


// Predicate to define an order on vertices
class VertexOrderingPredicate
{
public:
    VertexOrderingPredicate(const VertexArray* vertexArray) :
        m_vertexArray(vertexArray),
        m_vertexSpec(vertexArray->vertexSpec())
    {
    }

    bool operator()(unsigned int index0, unsigned int index1) const
    {
        const VertexAttribute::Component* vertex0 = m_vertexArray->vertex(index0);
        const VertexAttribute::Component* vertex1 = m_vertexArray->vertex(index1);

        for (unsigned int attributeIndex = 0; attributeIndex < m_vertexSpec.attributeCount(); ++attributeIndex)
        {
            unsigned int attributeOffset = m_vertexSpec.attributeOffset(attributeIndex);
            const VertexAttribute::Component* attr0 = vertex0 + (attributeOffset >> 2);
            const VertexAttribute::Component* attr1 = vertex1 + (attributeOffset >> 2);

            switch (m_vertexSpec.attribute(attributeIndex).format())
            {
            case VertexAttribute::Float4:
                if (attr0[3].f < attr1[3].f)
                {
                    return true;
                }
                else if (attr0[3].f > attr1[3].f)
                {
                    return false;
                }
                // intentional fallthrough

            case VertexAttribute::Float3:
                if (attr0[2].f < attr1[2].f)
                {
                    return true;
                }
                else if (attr0[2].f > attr1[2].f)
                {
                    return false;
                }
                // intentional fallthrough

            case VertexAttribute::Float2:
                if (attr0[1].f < attr1[1].f)
                {
                    return true;
                }
                else if (attr0[1].f > attr1[1].f)
                {
                    return false;
                }
                // intentional fallthrough

            case VertexAttribute::Float1:
                if (attr0[0].f < attr1[0].f)
                {
                    return true;
                }
                else if (attr0[0].f > attr1[0].f)
                {
                    return false;
                }
                break;

            case VertexAttribute::UByte4:
                if (attr0[0].u < attr1[0].u)
                {
                    return true;
                }
                else if (attr0[0].u > attr1[0].u)
                {
                    return false;
                }
                break;

            default:
                break;
            }
        }

        return false;
    }

private:
    const VertexArray* m_vertexArray;
    const VertexSpec& m_vertexSpec;
};


// See if f0 is a distance of tolerance or less from f1. This simple
// test is used instead of a constant precision test because for testing
// vertex equality we want the same 'granularity' over all vertices
// in the mesh.
static inline bool fuzzyEquals(float f0, float f1, float tolerance)
{
    return abs(f0 - f1) <= tolerance;
}


class VertexEqualityPredicate
{
public:
    VertexEqualityPredicate(const VertexArray* vertexArray) :
        m_vertexArray(vertexArray),
        m_vertexSpec(vertexArray->vertexSpec()),
        m_tolerances(NULL)
    {
        m_tolerances = new float[m_vertexSpec.attributeCount()];
        for (unsigned int i = 0; i < m_vertexSpec.attributeCount(); ++i)
        {
            m_tolerances[i] = 0.0f;
        }
    }

    ~VertexEqualityPredicate()
    {
        delete[] m_tolerances;
    }

    // Test two vertices for equality using a fuzzy comparison for all floating point
    // attributes.
    bool operator()(unsigned int index0, unsigned int index1) const
    {
        const VertexAttribute::Component* vertex0 = m_vertexArray->vertex(index0);
        const VertexAttribute::Component* vertex1 = m_vertexArray->vertex(index1);

        for (unsigned int attributeIndex = 0; attributeIndex < m_vertexSpec.attributeCount(); ++attributeIndex)
        {
            unsigned int attributeOffset = m_vertexSpec.attributeOffset(attributeIndex);
            const VertexAttribute::Component* attr0 = vertex0 + (attributeOffset >> 2);
            const VertexAttribute::Component* attr1 = vertex1 + (attributeOffset >> 2);

            float tolerance = m_tolerances[attributeIndex];

            switch (m_vertexSpec.attribute(attributeIndex).format())
            {
            case VertexAttribute::Float4:
                if (!fuzzyEquals(attr0[3].f, attr1[3].f, tolerance))
                {
                    return false;
                }
                // intentional fallthrough

            case VertexAttribute::Float3:
                if (!fuzzyEquals(attr0[2].f, attr1[2].f, tolerance))
                {
                    return false;
                }
                // intentional fallthrough

            case VertexAttribute::Float2:
                if (!fuzzyEquals(attr0[1].f, attr1[1].f, tolerance))
                {
                    return false;
                }
                // intentional fallthrough

            case VertexAttribute::Float1:
                if (!fuzzyEquals(attr0[0].f, attr1[0].f, tolerance))
                {
                    return false;
                }
                break;

            case VertexAttribute::UByte4:
                if (attr0[0].u != attr1[0].u)
                {
                    return false;
                }
                break;

            default:
                break;
            }
        }

        return true;
    }

    // Set the per-component tolerance for an attribute
    void setTolerance(VertexAttribute::Semantic semantic, float tolerance)
    {
        for (unsigned int i = 0; i < m_vertexSpec.attributeCount(); ++i)
        {
            if (m_vertexSpec.attribute(i).semantic() == semantic)
            {
                m_tolerances[i] = tolerance;
            }
        }
    }

private:
    const VertexArray* m_vertexArray;
    const VertexSpec& m_vertexSpec;
    float* m_tolerances;
};


/** Remove duplicate vertices in this submesh.
  *
  * \return true if unquification was successful, false if an error occurred (should only happen
  * in a low memory situation.)
  */
bool
Submesh::uniquifyVertices(float positionTolerance, float normalTolerance, float texCoordTolerance)
{
    vector<unsigned int> vertexIndices;
    vertexIndices.resize(m_vertices->count());
    for (unsigned int i = 0; i < m_vertices->count(); ++i)
    {
        vertexIndices[i] = i;
    }

    // Order the vertices so that identical ones will be close to each other.
    sort(vertexIndices.begin(), vertexIndices.end(), VertexOrderingPredicate(m_vertices));

    VertexEqualityPredicate equal(m_vertices);
    equal.setTolerance(VertexAttribute::Position,     positionTolerance);
    equal.setTolerance(VertexAttribute::Normal,       normalTolerance);
    equal.setTolerance(VertexAttribute::TextureCoord, texCoordTolerance);
    equal.setTolerance(VertexAttribute::Tangent,      normalTolerance);

    // Count the number of unique vertices so that we know how much space to allocate
    unsigned int uniqueVertexCount = 0;
    for (unsigned int i = 0; i < m_vertices->count(); ++i)
    {
        if (i == 0 || !equal(vertexIndices[i - 1], vertexIndices[i]))
        {
            uniqueVertexCount++;
        }
    }

    // Don't continue if we can't shrink the amount of vertex data
    if (uniqueVertexCount == m_vertices->count())
    {
        return true;
    }

    // Build the map that associates vertices in the old vertex array with unique indices.
    // In the same loop, copy the unique vertex data from the old vertex array to the
    // new one.
    vector<v_uint32> vertexMap;
    vertexMap.resize(m_vertices->count());

    unsigned int vertexStride = m_vertices->stride();
    char* newVertexData = new char[uniqueVertexCount * m_vertices->stride()];
    const char* currentVertexData = reinterpret_cast<const char*>(m_vertices->data());

    v_uint32 newVertexIndex = 0;
    for (unsigned int i = 0; i < m_vertices->count(); ++i)
    {
        if (i == 0 || !equal(vertexIndices[i - 1], vertexIndices[i]))
        {
            if (i > 0)
            {
                newVertexIndex++;
            }

            assert(newVertexIndex < uniqueVertexCount);
            const char* vertexStart = currentVertexData + vertexIndices[i] * vertexStride;
            copy(vertexStart, vertexStart + vertexStride, newVertexData + newVertexIndex * vertexStride);
        }

        vertexMap[vertexIndices[i]] = newVertexIndex;
    }

    VertexArray* newVertexArray = new VertexArray(newVertexData, uniqueVertexCount, m_vertices->vertexSpec(), m_vertices->stride());

    // Remap all vertex indices
    for (vector<PrimitiveBatch*>::const_iterator iter = m_primitiveBatches.begin(); iter != m_primitiveBatches.end(); ++iter)
    {
        if (!(*iter)->remapIndices(vertexMap))
        {
            VESTA_WARNING("Problem remapping vertex indices.");
            delete newVertexArray;
            return false;
        }
    }

    delete m_vertices;
    m_vertices = newVertexArray;

    //VESTA_LOG("%d of %d vertices unique.", uniqueVertexCount, vertexIndices.size());

    return true;
}
