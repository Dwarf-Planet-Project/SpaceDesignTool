/* 
 This program is free software; you can redistribute it and/or modify it under
 the terms of the GNU Lesser General Public License as published by the Free Software
 Foundation; either version 2 of the License, or (at your option) any later
 version.
 
 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more
 details.
 
 You should have received a copy of the GNU Lesser General Public License along with
 this program; if not, write to the Free Software Foundation, Inc., 59 Temple
 Place - Suite 330, Boston, MA 02111-1307, USA.
 Further information about the GNU Lesser General Public License can also be found on
 the world wide web at http://www.gnu.org. 
 */

/*
 ------ Copyright (C) 2008 European Space Agency (space.trajectory.analysis AT gmail.com) ----
 ------------------ Author: Chris Laurel  -------------------------------------------------
 ------------------ E-mail: (claurel@gmail.com) ----------------------------
 */

#ifndef _STA_SCENARIO_LOCATION_H_
#define _STA_SCENARIO_LOCATION_H_

#include "scenarioobject.h"
#include "Astro-Core/stabody.h"


// TODO: Consider moving this to a separate file, perhaps in the astro
// core.
struct GroundPosition
{
public:
    GroundPosition() :
        latitude(0.0), longitude(0.0), altitude(0.0) 
    {
    }
    
    GroundPosition(double _latitude, double _longitude, double _altitude = 0.0) :
        latitude(_latitude), longitude(_longitude), altitude(_altitude)
    {
    }
    
public:
    double latitude;
    double longitude;
    double altitude;
};


class ScenarioLocation : public ScenarioObject
{
public:
    ScenarioLocation();
    ~ScenarioLocation();
    
    virtual void writeContents(QXmlStreamWriter* out);
    virtual void createItemContents(QTreeWidgetItem* item);
    
    const StaBody* centralBody() const { return m_centralBody; }
    void setCentralBody(const StaBody* centralBody) { m_centralBody = centralBody; }
    GroundPosition getGroundPosition() const { return m_groundPosition; }
    void setGroundPosition(const GroundPosition& groundPosition) { m_groundPosition = groundPosition; }

    QString elementName() const { return "Location"; }
    QString displayName() const { return QObject::tr("Location"); }
    
private:
    const StaBody* m_centralBody;
    GroundPosition m_groundPosition;
};

#endif // _STA_SCENARIO_LOCATION_H_
