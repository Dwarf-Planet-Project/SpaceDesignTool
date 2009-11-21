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
 // Patched by Guillermo as to allow TLEs, June 5th 2009
 // Modified by Valentino Zuccarelli on 11th June 2009
 */

#ifndef _STA_SCENARIO_INITIALSTATE_H_
#define _STA_SCENARIO_INITIALSTATE_H_

#include "scenarioobject.h"
#include "Astro-Core/statevector.h"
#include "Astro-Core/stabody.h"
#include "Astro-Core/stacoordsys.h"

class ScenarioBody;

class ScenarioAbstractInitialState : public ScenarioObject
{
public:
    virtual sta::StateVector computeStateVector(const StaBody* centralBody) const = 0;
};

class ScenarioInitialStatePosition : public ScenarioObject
{
public:
    ScenarioInitialStatePosition();

    QString elementName() const { return "InitialStatePosition"; }
    QString displayName() const { return QObject::tr("Initial State"); }
    virtual void writeContents(QXmlStreamWriter* out);
    virtual void createItemContents(QTreeWidgetItem* item);

    sta::CoordinateSystem coordinateSystem() const { return m_coordinateSystem; }
    void setCoordinateSystem(sta::CoordinateSystem coordinateSystem) { m_coordinateSystem = coordinateSystem; }
    ScenarioAbstractInitialState* initialState() const { return m_initialState; }
    void setInitialState(ScenarioAbstractInitialState* initialState);
    ScenarioBody* centralsystemBody() const { return m_centralsystemBody; }
    void setCentralsystemBody(ScenarioBody* centralsystemBody);
    
    sta::StateVector computeStateVector(const StaBody* centralBody) const;
    
private:
    sta::CoordinateSystem m_coordinateSystem;
    ScenarioAbstractInitialState* m_initialState;
    ScenarioBody* m_centralsystemBody;

};


class ScenarioStateVector : public ScenarioAbstractInitialState
{
public:
    QString elementName() const { return "StateVector"; }
    QString displayName() const { return QObject::tr("State Vector"); }
    virtual void writeContents(QXmlStreamWriter* out);
    virtual void createItemContents(QTreeWidgetItem* item);

    Eigen::Vector3d position() const {return m_stateVector.position;}
    Eigen::Vector3d velocity()  const {return m_stateVector.velocity;}
    void setPosition(const Eigen::Vector3d& position) { m_stateVector.position = position; }
    void setVelocity(const Eigen::Vector3d& velocity) { m_stateVector.velocity = velocity; }

    virtual sta::StateVector computeStateVector(const StaBody* centralBody) const;

public:
    sta::StateVector m_stateVector;
};


class ScenarioKeplerianElements : public ScenarioAbstractInitialState
{
public:
    ScenarioKeplerianElements();
    
    QString elementName() const { return "KeplerianElements"; }
    QString displayName() const { return QObject::tr("Keplerian Elements"); }
    virtual void writeContents(QXmlStreamWriter* out);
    virtual void createItemContents(QTreeWidgetItem* item);
    
    virtual sta::StateVector computeStateVector(const StaBody* centralBody) const;

    double semimajorAxis() const {return m_semimajorAxis;}
    double eccentricity() const {return m_eccentricity;}
    double inclination() const {return m_inclination;}
    double raan() const {return m_raan;}
    double argumentOfPeriapsis() const {return m_argumentOfPeriapsis;}
    double trueAnomaly() const {return m_trueAnomaly;}
        
public:
    double m_semimajorAxis;
    double m_eccentricity;
    double m_inclination;
    double m_raan;
    double m_argumentOfPeriapsis;
    double m_trueAnomaly;
};


class ScenarioTLEs : public ScenarioAbstractInitialState
{
public:
    QString elementName() const { return "TLEs"; }
    QString displayName() const { return QObject::tr("Two-Line Elements"); }
    virtual void writeContents(QXmlStreamWriter* out);
    virtual void createItemContents(QTreeWidgetItem* item);

    Eigen::Vector3d position() const {return m_stateVector.position;}
    Eigen::Vector3d velocity()  const {return m_stateVector.velocity;}
    void setPosition(const Eigen::Vector3d& position) { m_stateVector.position = position; }
    void setVelocity(const Eigen::Vector3d& velocity) { m_stateVector.velocity = velocity; }

    virtual sta::StateVector computeStateVector(const StaBody* centralBody) const;

public:
    sta::StateVector m_stateVector;
};



class ScenarioSphericalCoordinates : public ScenarioAbstractInitialState
{
public:
	ScenarioSphericalCoordinates();
    
    QString elementName() const { return "SphericalCoordinates"; }
    QString displayName() const { return QObject::tr("Spherical Coordinates"); }
    virtual void writeContents(QXmlStreamWriter* out);
    virtual void createItemContents(QTreeWidgetItem* item);
    
    virtual sta::StateVector computeStateVector(const StaBody* centralBody) const;

    double altitude() const {return m_altitude;}
    double longitude() const {return m_longitude;}
    double latitude() const {return m_latitude;}
    double inertialVelocity() const {return m_inertialV;}
    double inertialFlightPathAngle() const {return m_inertialFPA;}
    double inertialHeading() const {return m_inertialH;}
        
public:
    double m_altitude;
    double m_longitude;
    double m_latitude;
    double m_inertialV;
    double m_inertialFPA;
    double m_inertialH;
};


// TODO: Not yet implemented
class ScenarioInitialStateAttitude : public ScenarioObject
{
public:
    ScenarioInitialStateAttitude();
    
    QString elementName() const { return "InitialStateAttitude"; }
    QString displayName() const { return QObject::tr("Initial Attitude"); }
    virtual void writeContents(QXmlStreamWriter* out);
    virtual void createItemContents(QTreeWidgetItem* item);
};

#endif // _STA_SCENARIO_INITIALSTATE_H_
