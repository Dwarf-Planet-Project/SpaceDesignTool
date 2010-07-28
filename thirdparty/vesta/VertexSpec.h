/*
 * $Revision: 365 $ $Date: 2010-07-19 11:07:24 -0700 (Mon, 19 Jul 2010) $
 *
 * Copyright by Astos Solutions GmbH, Germany
 *
 * this file is published under the Astos Solutions Free Public License
 * For details on copyright and terms of use see 
 * http://www.astos.de/Astos_Solutions_Free_Public_License.html
 */

#ifndef _VESTA_VERTEX_SPEC_H_
#define _VESTA_VERTEX_SPEC_H_

#include "VertexAttribute.h"

namespace vesta
{

class VertexSpec
{
public:
    enum { InvalidAttribute = 1000 };

    VertexSpec(unsigned int attributeCount,
               VertexAttribute* attributes,
               unsigned int* attributeOffsets = 0);
    VertexSpec(const VertexSpec& other);
    ~VertexSpec();

    VertexSpec& operator=(const VertexSpec& other);

    bool operator==(const VertexSpec& other) const;
    bool operator!=(const VertexSpec& other) const;

    unsigned int attributeCount() const
    {
        return m_attributeCount;
    }

    VertexAttribute attribute(unsigned int index) const
    {
        return m_attributes[index];
    }

    unsigned int attributeOffset(unsigned int index) const
    {
        return m_attributeOffsets[index];
    }

    unsigned int attributeIndex(VertexAttribute::Semantic semantic) const
    {
        for (unsigned int i = 0; i < m_attributeCount; ++i)
        {
            if (m_attributes[i].semantic() == semantic)
            {
                return i;
            }
        }

        return InvalidAttribute;
    }

    unsigned int size() const
    {
        return m_size;
    }

public:
    static VertexSpec PositionNormal;
    static VertexSpec PositionNormalTex;
    static VertexSpec PositionTex;
    static VertexSpec PositionColor;
    static VertexSpec PositionColorTex;
    static VertexSpec Position;

private:
    void initAttributes(unsigned int attributeCount,
                        VertexAttribute* attributes);
    void copyAttributeOffsets(unsigned int* attributeOffsets);
    void computeAttributeOffsets();

private:
    unsigned int m_attributeCount;
    VertexAttribute* m_attributes;
    unsigned int* m_attributeOffsets;
    unsigned int m_size;
};

}

#endif // _VESTA_VERTEX_ARRAY_H_
