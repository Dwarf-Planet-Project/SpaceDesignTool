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

#include "WMSTiledMap.h"

using namespace vesta;
using namespace std;


WMSTiledMap::WMSTiledMap(TextureMapLoader* loader,
                         const QString& layerName,
                         unsigned int tileSize,
                         unsigned int levelCount) :
    HierarchicalTiledMap(loader, tileSize),
    m_levelCount(levelCount)
{
    m_tileNamePattern = QString("wms:") + layerName + ",%1,%2,%3";
}


string
WMSTiledMap::tileResourceIdentifier(unsigned int level, unsigned int column, unsigned int row)
{
    QString s = m_tileNamePattern.arg(level).arg(column).arg(row);
    return string(s.toUtf8().data());
}


bool
WMSTiledMap::isValidTileAddress(unsigned int level, unsigned int column, unsigned int row)
{
    return level < m_levelCount && column < (1u << (level + 1)) && row < (1u << level);
}


bool WMSTiledMap::tileResourceExists(const std::string& /* resourceId */)
{
    return true;//level < levelCount;
}
