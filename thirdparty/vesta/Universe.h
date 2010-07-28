/*
 * $Revision: 226 $ $Date: 2010-04-01 22:58:10 -0700 (Thu, 01 Apr 2010) $
 *
 * Copyright by Astos Solutions GmbH, Germany
 *
 * this file is published under the Astos Solutions Free Public License
 * For details on copyright and terms of use see 
 * http://www.astos.de/Astos_Solutions_Free_Public_License.html
 */

#ifndef _VESTA_UNIVERSE_H_
#define _VESTA_UNIVERSE_H_

#include "Entity.h"
#include "StarCatalog.h"
#include "PickResult.h"
#include <vector>

namespace vesta
{

class Universe : public Object
{
public:
    Universe();
    ~Universe();

    const std::vector<Entity*>& entities() const;
    void addEntity(Entity* entity);
    void removeEntity(Entity* entity);
    Entity* findFirst(const std::string& name);

    StarCatalog* starCatalog() const;
    void setStarCatalog(StarCatalog* starCatalog);

    bool pickObject(double t,
                    const Eigen::Vector3d& pickOrigin,
                    const Eigen::Vector3d& pickDirection,
                    double pixelAngle,
                    PickResult* result) const;

private:
    std::vector<Entity*> m_entities;
    counted_ptr<StarCatalog> m_starCatalog;
};

}

#endif // _VESTA_BODY_H_
