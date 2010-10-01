// Copyright (C) 2010 Chris Laurel <claurel@gmail.com>
//
// This file is a part of qtvesta, a set of classes for using
// the VESTA library with the Qt framework.
//
// qtvesta is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2 of the License, or (at your option) any later version.
//
// qtvesta is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with qtvesta. If not, see <http://www.gnu.org/licenses/>.

#ifndef _QTVESTA_WMS_TILED_MAP_H_
#define _QTVESTA_WMS_TILED_MAP_H_

#include <vesta/HierarchicalTiledMap.h>
#include <vesta/TextureMapLoader.h>
#include <QString>

class WMSTiledMap : public vesta::HierarchicalTiledMap
{
public:
    WMSTiledMap(vesta::TextureMapLoader* loader, const QString& layerName, unsigned int tileSize, unsigned int levelCount);

    virtual std::string tileResourceIdentifier(unsigned int level, unsigned int column, unsigned int row);
    virtual bool isValidTileAddress(unsigned int level, unsigned int column, unsigned int row);
    virtual bool tileResourceExists(const std::string& resourceId);

private:
    QString m_tileNamePattern;
    unsigned int m_levelCount;
};

#endif // _QTVESTA_WMS_TILED_MAP_H_
