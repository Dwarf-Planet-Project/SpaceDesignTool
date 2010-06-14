/*
 This program is free software; you can redistribute it and/or modify it under
 the terms of the European Union Public Licence - EUPL v.1.1 as published by
 the European Commission.

 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE. See the European Union Public Licence - EUPL v.1.1
 for more details.

 You should have received a copy of the European Union Public Licence - EUPL v.1.1
 along with this program.

 Further information about the European Union Public Licence - EUPL v.1.1 can
 also be found on the world wide web at http://ec.europa.eu/idabc/eupl

*/

/*
 ------ Copyright (C) 2010 STA Steering Board (space.trajectory.analysis AT gmail.com) ----
*/

/*
------------------ Author: Chris Laurel  -------------------------------------------------
 ------------------ E-mail: (claurel@gmail.com) ----------------------------
 */

/*
 * Celestia interface for STA Space Scenario
 * 
 * This header contains the interface to Celestia for STA scenario files.
 * Scenario objects should use the wrapper classes defined here rather than
 * calling Celestia functions directly. Constraining the usage of Celestia
 * makes it easier to port to new versions of Celestia.
 *
 * Any missing Celestia functionality can be added to the wrapper classes.
 */

#ifndef _STA_SCENARIO_CELESTIAINTERFACE_H_
#define _STA_SCENARIO_CELESTIAINTERFACE_H_

#include <QList>
#include <QColor>
#include "Astro-Core/stabody.h"
#include "Astro-Core/stacoordsys.h"


// Forward declarations of Celestia classes; do not include the files
// here: Celestia objects should remain opaque to modules that include
// celestiainterface.h
class CelestiaCore;
class Universe;
class Body;
class Star;
class SolarSystem;
class MissionArc;
class Location;


class CelestiaInterface;

class CelestiaBody
{
 friend class CelestiaInterface;

 public:
    // TODO: make this private; should only be called by
    // CelestiaInterface factory methods.
    CelestiaBody(CelestiaInterface* iface, Body* body);
    ~CelestiaBody();

    bool setTimeline(const QList<MissionArc*>& missionArcs);
    
    void setGeometry(const QString& geometryFile);
    
    bool isVisible() const;
    void setVisible(bool visible);
    
    void setOrbitAlwaysVisible(bool visible);
    void setOrbitColor(QColor color);
    void setOrbitColorOverridden(bool override);
    
    void mark(const QString& symbolName, QColor color, float size, const QString& label);
    void unmark();
    
 private:
    Body* body() const { return m_body; }

 private:
    CelestiaInterface* m_interface;
    Body* m_body;  // Celestia body
    bool m_marked;
};


class CelestiaLocation
{
 friend class CelestiaInterface;
public:
    // TODO: make this private; should only be called by
    // CelestiaInterface factory methods.
    CelestiaLocation(Location* location);
    ~CelestiaLocation();
    
    void setColor(QColor color);
    
private:
    Location* m_location;
};


class CelestiaInterface
{
 friend class CelestiaBody;

 private:
    CelestiaInterface();
    ~CelestiaInterface();

 public:
    CelestiaBody* createBody(const QString& name, const StaBody* parent);
    CelestiaBody* createSpacecraft(const QString& name, const StaBody* parent);
    CelestiaBody* createSurfaceObject(const QString& name,
                                      const StaBody* centralBody,
                                      double latitude, double longitude, double altitude);
    CelestiaLocation* createLocation(const QString& name,
                                     const StaBody* centralBody,
                                     double latitude, double longitude, double altitude);

    static CelestiaInterface* Create(CelestiaCore* appCore);
    
    void select(CelestiaBody* body);
    
 private:
    Universe* universe() const { return m_universe; }
    Star* sun() const { return m_sun; }
    Star* ssb() const { return m_ssb; }
    Body* findBody(const StaBody* body);

 private:
    CelestiaCore* m_appCore;
    Universe* m_universe;
    Star* m_sun;
    Star* m_ssb; // Solar system barycenter
    SolarSystem* m_solarSystem;
};



#endif // _STA_SCENARIO_CELESTIAINTERFACE_H_

