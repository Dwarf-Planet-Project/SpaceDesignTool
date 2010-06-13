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
------------------ Author:       Marnix Volckaert               -------------------
------------------ Affiliation:  TUDelft                        -------------------
-----------------------------------------------------------------------------------
 Modified by Tiziana Sabatini on May-July 2009
*/


#ifndef ENTRYTRAJECTORY_H_INCLUDED
#define ENTRYTRAJECTORY_H_INCLUDED
#include <iostream>
#include <iomanip>
#include <fstream>
#include <math.h>
#include "Astro-Core/Atmosphere/AtmosphereModel.h"
#include "BodyREM.h"
#include "capsule.h"
#include "HeatRateClass.h"
#include "Astro-Core/AngleConversion.h"
#include "Astro-Core/cartesianTOorbital.h"
#include "Astro-Core/date.h"
#include "Astro-Core/stamath.h"
#include "Astro-Core/statevector.h"
#include "Astro-Core/stabody.h"
#include "Astro-Core/stacoordsys.h"
#include "Astro-Core/date.h"
#include "Astro-Core/orbitalTOcartesian.h"
#include "Astro-Core/sphericalTOcartesian.h"
#include "Astro-Core/inertialTOfixed.h"
#include "Astro-Core/fixedTOinertial.h"
#include "Astro-Core/cartesianTOspherical.h"
#include "Astro-Core/getGreenwichHourAngle.h"
#include "Astro-Core/perturbations.h"
#include "reentrystructures.h"
#include <Eigen/Core>


USING_PART_OF_NAMESPACE_EIGEN

//class StateVector;
class Perturbations;


class EntryTrajectory
{
public:
    friend class EntryDerivativeCalculator;
    _status status;
    capsule_class capsule;

    /**
     * Constructor for the entry trajectory object
     */
    EntryTrajectory(EntrySettings);
    /**
     * Return the StateVector at the beginnig of the simulation
     */
    sta::StateVector initialise(EntryParameters,sta::StateVector, double);
    /**
     * Integrate the entry trajectory at a generic step
     */
    sta::StateVector integrate(sta::StateVector stateVector, QList<Perturbations*> perturbationsList);
    /**
     * Return the Endstate structure of an entry trajectory
     */
    Endstate getEndstate();
    /**
     * Save the report after integrating the trajectory
     */
    inline void printReport();
    /**
     * Save the trajectory in the output files
     */
    void saveTrajectory(QList<double>&, QList<sta::StateVector>& samples);

private:

    bool save;          //True if trajectory needs to be saved in trajectory.out and misc.out
    EntrySettings settings;
    EntryParameters parameters;

    double time;
    VectorXd state;
    VectorXd state2; //Extra information: 0 = density, 1 = mach number, 2 = load factor, 3 = qdot_c, 4 = qdot_r, 5 = altitude, 6 = velocity, 7 = parachuteDeploy (0 = entry, 1 = descent), 8 = total heat rate, 9 = total heat load

    AtmosphereModel atmosphere;
    BodyREM body;
    HeatRateClass heatrate;

    ofstream trajectory;
    ofstream misc;
    Endstate result;

    /**
     * Calculate the state2 vector
     */
    void calculateState2(Vector3d&, double);
    /**
     * Update the maxima in the endstate structure
     */
    Endstate updateEndstate();
    /**
     * Check the status of the capsule during the trajectory
     */
    inline void checkStatus();
    /**
     * Calculate the endstate structure
     */
    Endstate calculateEndstate();
    /**
     * Saves the trajectory and misc file during integration of the trajectory
     */
    inline void saveOutput(double);
};

#endif // ENTRYTRAJECTORY_H_INCLUDED
