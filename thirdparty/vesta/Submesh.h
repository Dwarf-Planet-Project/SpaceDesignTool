/*
 * $Revision: 369 $ $Date: 2010-07-19 19:13:37 -0700 (Mon, 19 Jul 2010) $
 *
 * Copyright by Astos Solutions GmbH, Germany
 *
 * this file is published under the Astos Solutions Free Public License
 * For details on copyright and terms of use see 
 * http://www.astos.de/Astos_Solutions_Free_Public_License.html
 */

#ifndef _VESTA_SUBMESH_H_
#define _VESTA_SUBMESH_H_

#include "Object.h"
#include "VertexArray.h"
#include "PrimitiveBatch.h"
#include "BoundingBox.h"
#include <vector>

namespace vesta
{

class Submesh : public Object
{
public:
    Submesh(VertexArray* vertices);
    ~Submesh();

    void addPrimitiveBatch(PrimitiveBatch* batch, unsigned int materialIndex = DefaultMaterialIndex);

    const VertexArray* vertices() const
    {
        return m_vertices;
    }

    const std::vector<PrimitiveBatch*>& primitiveBatches() const
    {
        return m_primitiveBatches;
    }

    unsigned int primitiveBatchCount() const
    {
        return m_primitiveBatches.size();
    }

    const std::vector<unsigned int>& materials() const
    {
        return m_materials;
    }

    BoundingBox boundingBox() const
    {
        return m_boundingBox;
    }

    /** Get the radius of an origin centered sphere large enough
      * to contain the entire submesh.
      */
    float boundingSphereRadius() const
    {
        return m_boundingSphereRadius;
    }

    static Submesh* mergeSubmeshes(const std::vector<Submesh*>& submeshes);
    bool uniquifyVertices(float positionTolerance = 0.0f, float normalTolerance = 0.0f, float texCoordTolerance = 0.0f);

    static const unsigned int DefaultMaterialIndex = 0xffffffff;

private:
    VertexArray* m_vertices;
    std::vector<PrimitiveBatch*> m_primitiveBatches;
    std::vector<unsigned int> m_materials;
    BoundingBox m_boundingBox;
    float m_boundingSphereRadius;
};

}

#endif // _VESTA_SUBMESH_H_

