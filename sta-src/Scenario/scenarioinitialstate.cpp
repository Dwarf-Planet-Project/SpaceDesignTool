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

Modified by Valentino Zuccarelli on 11th June 2009
 Mofidied by Guillermo to pathc the bug of the anomaly. Guillermo July 8th 2009

 */

#include "scenariobody.h"
#include "scenarioinitialstate.h"
#include "Astro-Core/orbitalTOcartesian.h"
#include "Astro-Core/sphericalTOcartesian.h"
#include "Astro-Core/stamath.h"
#include <QTreeWidgetItem>
#include <Entry/BodyREM.h>
#include <QDebug>

#include "Astro-Core/trueAnomalyTOmeanAnomaly.h"


using namespace Eigen;

/*** ScenarioInitialStatePosition ***/

ScenarioInitialStatePosition::ScenarioInitialStatePosition() :
    m_coordinateSystem(sta::COORDSYS_EME_J2000),
    m_initialState(NULL),
    m_centralsystemBody(NULL)
{

}


void
ScenarioInitialStatePosition::writeContents(QXmlStreamWriter* out)
{
    out->writeTextElement("CoordinateSystem", m_coordinateSystem.name());
    if (m_initialState)
        m_initialState->writeElement(out);
    m_centralsystemBody->writeElement(out, "CentralsystemBody");
    //out->writeTextElement("CentralsystemBody", m_centralsystemBody->body()->name());
}


void
ScenarioInitialStatePosition::createItemContents(QTreeWidgetItem* item)
{
    QTreeWidgetItem* coordSysItem = new QTreeWidgetItem(item);
    coordSysItem->setText(0, QObject::tr("Coordinate System"));
    coordSysItem->setText(1, m_coordinateSystem.name());
    coordSysItem->setText(0, QObject::tr("Central Body"));
    coordSysItem->setText(1, m_centralsystemBody->body()->name());
    if (m_initialState)
        m_initialState->createItem(item);
}


void
ScenarioInitialStatePosition::setCentralsystemBody(ScenarioBody* centralsystemBody)
{
    if (centralsystemBody != m_centralsystemBody)
    {
        detachChild(m_centralsystemBody);
        m_centralsystemBody = centralsystemBody;
        attachChild(m_centralsystemBody);
    }
}


void
ScenarioInitialStatePosition::setInitialState(ScenarioAbstractInitialState* initialState)
{
    if (initialState != m_initialState)
    {
        detachChild(m_initialState);
        m_initialState = initialState;
        attachChild(m_initialState);
    }    
}


sta::StateVector
ScenarioInitialStatePosition::computeStateVector(const StaBody* centralBody) const
{
    return m_initialState->computeStateVector(centralBody);
}



/*** ScenarioKeplerianElements ***/

ScenarioKeplerianElements::ScenarioKeplerianElements() :
    m_semimajorAxis(0.0),
    m_eccentricity(0.0),
    m_inclination(0.0),
    m_raan(0.0),
    m_argumentOfPeriapsis(0.0),
    m_trueAnomaly(0.0)
{
}


void ScenarioKeplerianElements::writeContents(QXmlStreamWriter* out)
{
    writeNumericElement(out, "SemiMajorAxis", m_semimajorAxis, "km");
    writeNumericElement(out, "Eccentricity", m_eccentricity);
    writeNumericElement(out, "Inclination", m_inclination, "deg");
    writeNumericElement(out, "RAAN", m_raan, "deg");
    writeNumericElement(out, "ArgumentOfPeriapsis", m_argumentOfPeriapsis, "deg");
    writeNumericElement(out, "TrueAnomaly", m_trueAnomaly, "deg");
}


static void createDoubleItem(QTreeWidgetItem* parent, const QString& name, double value)
{
    QTreeWidgetItem* item = new QTreeWidgetItem(parent);
    item->setText(0, name);
    item->setText(1, QString::number(value));
}


void
ScenarioKeplerianElements::createItemContents(QTreeWidgetItem* item)
{
    createDoubleItem(item, QObject::tr("Semimajor Axis"),   m_semimajorAxis);
    createDoubleItem(item, QObject::tr("Eccentricity"),     m_eccentricity);
    createDoubleItem(item, QObject::tr("Inclination"),      m_inclination);
    createDoubleItem(item, QObject::tr("RAAN"),             m_raan);
    createDoubleItem(item, QObject::tr("Arg of Periapsis"), m_argumentOfPeriapsis);
    createDoubleItem(item, QObject::tr("True Anomaly"),     m_trueAnomaly);
}


sta::StateVector
ScenarioKeplerianElements::computeStateVector(const StaBody* centralBody) const
{

    // Convert the true anomaly in radians
    double trueAnomaly = sta::degToRad(m_trueAnomaly);
    // Convert the true anomaly to mean anomaly
    double meanAnomaly = trueAnomalyTOmeanAnomaly(trueAnomaly, m_eccentricity);

    return orbitalTOcartesian(centralBody->mu(),
                              m_semimajorAxis,
                              m_eccentricity,
			      //sta::degToRad(m_inclination),
			      // Poor's man repair of the inclination = 90 singularity. This patch need to be properly addresed
			      sta::degToRad(m_inclination+0.000000001),
                              sta::degToRad(m_argumentOfPeriapsis),
                              sta::degToRad(m_raan),
                              meanAnomaly);
}


sta::StateVector
ScenarioStateVector::computeStateVector(const StaBody* /* centralBody */) const
{
    return m_stateVector;
}


void
ScenarioStateVector::writeContents(QXmlStreamWriter* out)
{
    //writeVectorElement(out, "Position", position(), "km");
    //writeVectorElement(out, "Velocity", velocity(), "km");
    writeNumericElement(out, "x",  position().x());
    writeNumericElement(out, "y",  position().y());
    writeNumericElement(out, "z",  position().z());
    writeNumericElement(out, "vx", velocity().x());
    writeNumericElement(out, "vy", velocity().y());
    writeNumericElement(out, "vz", velocity().z());
}


void
ScenarioStateVector::createItemContents(QTreeWidgetItem* item)
{

    createDoubleItem(item, QObject::tr("x"),  position().x());
    createDoubleItem(item, QObject::tr("y"),  position().y());
    createDoubleItem(item, QObject::tr("z"),  position().z());
    createDoubleItem(item, QObject::tr("vx"), velocity().x());
    createDoubleItem(item, QObject::tr("vy"), velocity().y());
    createDoubleItem(item, QObject::tr("vz"), velocity().z());
}

/*** Scenario Spherical Coordinates***/

ScenarioSphericalCoordinates::ScenarioSphericalCoordinates() :
    m_altitude(0.0),
    m_longitude(0.0),
    m_latitude(0.0),
    m_inertialV(0.0),
    m_inertialFPA(0.0),
    m_inertialH(0.0)
{
}


void
ScenarioSphericalCoordinates::writeContents(QXmlStreamWriter* out)
{
    writeNumericElement(out, "Altitude", m_altitude, "km");
    writeNumericElement(out, "Longitude", m_longitude, "deg" );
    writeNumericElement(out, "Latitude", m_latitude, "deg");
    writeNumericElement(out, "InertialVelocity", m_inertialV, "Km/s");
    writeNumericElement(out, "InertialFlightPathAngle", m_inertialFPA, "deg");
    writeNumericElement(out, "InertialHeading", m_inertialH, "deg");
}




void
ScenarioSphericalCoordinates::createItemContents(QTreeWidgetItem* item)
{
    createDoubleItem(item, QObject::tr("Entry altitude"),  m_altitude);
    createDoubleItem(item, QObject::tr("Entry longitude"), m_longitude);
    createDoubleItem(item, QObject::tr("Entry latitude"),  m_latitude);
    createDoubleItem(item, QObject::tr("Inertial velocity"), m_inertialV);
    createDoubleItem(item, QObject::tr("Inertial Flight Path Angle"), m_inertialFPA);
    createDoubleItem(item, QObject::tr("Inertial Heading"), m_inertialH);
}


sta::StateVector 
ScenarioSphericalCoordinates::computeStateVector(const StaBody* centralBody) const
{
    double tau = sta::degToRad(m_longitude)+4.8949;
    double delta = sta::degToRad(m_latitude);
    double f, rs;
    f = (centralBody->radii().x() - centralBody->radii().z()) / centralBody->radii().x();
    rs = centralBody->radii().x() * 1000 * sqrt( pow((1-f),2) / (1- f*(2-f) * pow(cos(delta),2)) );
    double r = (m_altitude*1000 + rs);
    double V = m_inertialV*1000;
    double gamma = sta::degToRad(m_inertialFPA);
    double chi = sta::degToRad(m_inertialH);

    Vector3d position;
    Vector3d velocity;

    //------ Transform to Carthesian coordinates
    sphericalTOcartesian(tau, delta, r, V, gamma, chi,
                       position.x(), position.y(), position.z(),
                       velocity.x(), velocity.y(), velocity.z());


    return sta::StateVector(position, velocity);
}



/*** ScenarioInitialStateAttitude ***/

ScenarioInitialStateAttitude::ScenarioInitialStateAttitude()
{
}


void
ScenarioInitialStateAttitude::writeContents(QXmlStreamWriter* /* out */)
{
}


void
ScenarioInitialStateAttitude::createItemContents(QTreeWidgetItem* /* item */)
{
}
