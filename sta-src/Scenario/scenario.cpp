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
 // Patched by Guillermo to convert into mean anomaly April 23 2010
 */

#include "scenario.h"
#include "Astro-Core/orbitalTOcartesian.h"
#include "Astro-Core/cartesianTOorbital.h"
#include "Astro-Core/trueAnomalyTOmeanAnomaly.h"
#include "Astro-Core/stamath.h"


/** Convert an abstract position to a cartesian state vector.
  *
  * \param position an abstract 6 DOF position
  * \param centralBody pointer to an StaBody object
  */
sta::StateVector
AbstractPositionToStateVector(const ScenarioAbstract6DOFPositionType* position,
                              const StaBody* centralBody)
{
    sta::StateVector initialState;

    if (dynamic_cast<const ScenarioStateVectorType*>(position))
    {
        const ScenarioStateVectorType* sv = dynamic_cast<const ScenarioStateVectorType*>(position);
        initialState = sta::StateVector(Eigen::Vector3d(sv->x(), sv->y(), sv->z()),
                                        Eigen::Vector3d(sv->vx(), sv->vy(), sv->vz()));
    }
    else if (dynamic_cast<const ScenarioKeplerianElementsType*>(position))
    {
        const ScenarioKeplerianElementsType* elements = dynamic_cast<const ScenarioKeplerianElementsType*>(position);
        sta::KeplerianElements e;
	e.AscendingNode = elements->RAAN()*Pi()/180.0;
	e.ArgumentOfPeriapsis = elements->argumentOfPeriapsis()*Pi()/180.0;
        e.Eccentricity = elements->eccentricity();
	e.TrueAnomaly = elements->trueAnomaly()*Pi()/180.0;
	double MeanAnomaly =  trueAnomalyTOmeanAnomaly(e.TrueAnomaly,e.Eccentricity);
        e.SemimajorAxis = elements->semiMajorAxis();
	e.Inclination = elements->inclination()*Pi()/180.0;

        initialState = orbitalTOcartesian(centralBody->mu(), e.SemimajorAxis, e.Eccentricity, e.Inclination,
					  e.ArgumentOfPeriapsis, e.AscendingNode, MeanAnomaly);
    }
    else if (dynamic_cast<const ScenarioSphericalCoordinatesType*>(position))
    {
        const ScenarioSphericalCoordinatesType* spherical = dynamic_cast<const ScenarioSphericalCoordinatesType*>(position);

        // TODO: implement this; need to figure out mapping of spherical coordinate properties to
        // parameters for sphericalTOcartesian function -cjl
        // initialState = sphericalTOcartesian();
        // initialStateKeplerial = cartesianToOrbital(initialState);
    }
    else
    {
        qCritical("Unrecognized Abstract6DOFPosition subclass!");
    }

    return initialState;
}


/** Convert an abstract position to Keplerian elements.
  *
  * \param position an abstract 6 DOF position
  * \param centralBody pointer to an StaBody object
  */
sta::KeplerianElements
AbstractPositionToKeplerianElements(const ScenarioAbstract6DOFPositionType* position,
                                    const StaBody* centralBody)
{
    sta::KeplerianElements initialStateKeplerian;

    if (dynamic_cast<const ScenarioStateVectorType*>(position))
    {
        const ScenarioStateVectorType* sv = dynamic_cast<const ScenarioStateVectorType*>(position);
        sta::StateVector initialState = sta::StateVector(Eigen::Vector3d(sv->x(), sv->y(), sv->z()),
                                                         Eigen::Vector3d(sv->vx(), sv->vy(), sv->vz()));
        initialStateKeplerian = cartesianTOorbital(centralBody->mu(), initialState);
    }
    else if (dynamic_cast<const ScenarioKeplerianElementsType*>(position))
    {
        const ScenarioKeplerianElementsType* elements = dynamic_cast<const ScenarioKeplerianElementsType*>(position);
        sta::KeplerianElements e;
        e.AscendingNode = elements->RAAN();
        e.ArgumentOfPeriapsis = elements->argumentOfPeriapsis();
        e.Eccentricity = elements->eccentricity();
        e.TrueAnomaly = elements->trueAnomaly();
        e.SemimajorAxis = elements->semiMajorAxis();
        e.Inclination = elements->inclination();

        initialStateKeplerian = e;
    }
    else if (dynamic_cast<const ScenarioSphericalCoordinatesType*>(position))
    {
        const ScenarioSphericalCoordinatesType* spherical = dynamic_cast<const ScenarioSphericalCoordinatesType*>(position);

        // TODO: implement this; need to figure out mapping of spherical coordinate properties to
        // parameters for sphericalTOcartesian function -cjl
        // initialState = sphericalTOcartesian();
        // initialStateKeplerial = cartesianToOrbital(initialState);
    }
    else
    {
        qCritical("Unrecognized Abstract6DOFPosition subclass!");
    }

    return initialStateKeplerian;
}



