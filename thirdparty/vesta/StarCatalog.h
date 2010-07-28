/*
 * $Revision: 223 $ $Date: 2010-03-30 05:44:44 -0700 (Tue, 30 Mar 2010) $
 *
 * Copyright by Astos Solutions GmbH, Germany
 *
 * this file is published under the Astos Solutions Free Public License
 * For details on copyright and terms of use see 
 * http://www.astos.de/Astos_Solutions_Free_Public_License.html
 */

#ifndef _VESTA_STAR_CATALOG_H_
#define _VESTA_STAR_CATALOG_H_

#include "Entity.h"
#include <vector>

namespace vesta
{

class StarCatalog : public Object
{
public:
    StarCatalog();
    ~StarCatalog();

    unsigned int size() const
    {
        return m_starData.size();
    }

    void addStar(double ra, double dec, double vmag, double bv);

public:
    struct StarRecord
    {
        float x;
        float y;
        float z;
        unsigned char color[4];
    };

    const StarRecord* vertexBuffer() const
    {
        if (m_starData.empty())
            return 0;
        else
            return &m_starData[0];
    }

private:
    std::vector<StarRecord> m_starData;
};

}

#endif // _VESTA_STAR_CATALOG_H_
