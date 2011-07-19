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
 // Patched by Guillermo to convert into mean anomaly April 23 2010
 // Patched by Catarina to add attitude, July 2011
 */

#include "scenario.h"
#include "Astro-Core/orbitalTOcartesian.h"
#include "Astro-Core/cartesianTOorbital.h"
#include "Astro-Core/trueAnomalyTOmeanAnomaly.h"
#include "Astro-Core/stamath.h"
//#include "Astro-Core/attitudevector.h"
#include "Astro-Core/attitudetransformations.h"

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



/** Convert an abstract attitude to an attitude state vector
  *
  * \param position an ????????
  * \param c????????
  */
staAttitude::AttitudeVector
        AbstractAttitudeToAttitudeVector(const ScenarioAbstract6DOFAttitudeType* attitude,
                                         int seq1, int seq2, int seq3)

{
    staAttitude::AttitudeVector initialAttitude;

    if (dynamic_cast<const ScenarioqBIType*>(attitude))
    {
        //Reading the attitude from XML
        const ScenarioqBIType* av = dynamic_cast<const ScenarioqBIType*>(attitude);
        // converting to quaternions and body rates
        Quaterniond quaternions(av->q1(), av->q2(), av->q3(), av->q4());
        Quaterniond quaternionsDot(av->q1Dot(), av->q2Dot(), av->q3Dot(), av->q4Dot());

        Vector3d transformedBodyRates = ToAngularVelocity(quaternions, quaternionsDot);

        // inserting them in the attitude vector
        initialAttitude = staAttitude::AttitudeVector(Eigen::Quaterniond(av->q1(), av->q2(), av->q3(), av->q4()),
                                                      Vector3d(transformedBodyRates));

    }
    else if (dynamic_cast<const ScenarioEulerBIType*>(attitude))
    {
        const ScenarioEulerBIType* euler = dynamic_cast<const ScenarioEulerBIType*>(attitude);
        MyVector3d eulerAngles(euler->phi(), euler->theta(), euler->psi());
        MyVector3d bodyRates(euler->phiDot(), euler->thetaDot(), euler->psiDot());

        //Call object's ToQuaternions function:
        Quaterniond initQuaternion = ToQuaternions(eulerAngles, seq1, seq2, seq3);

        //Call object's ToQuaternionRates function
        Quaterniond initQuaternionRates = ToQuaternionRates(initQuaternion,bodyRates);

        Vector3d transformedBodyRates = ToAngularVelocity(initQuaternion, initQuaternionRates);
        // inserting them in the attitude vector
        initialAttitude = staAttitude::AttitudeVector(Eigen::Quaterniond(initQuaternion),
                                              Eigen::Vector3d(transformedBodyRates));
    }
    else
    {
        qCritical("Unrecognized AbstractAttitude subclass!");
    }

    return initialAttitude;

}


