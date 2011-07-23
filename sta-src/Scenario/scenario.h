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
 // Patched by Catarina to add attitude, July 2011
 // Extebsively patched by Guillermo to correct errors and change the type of the qauternions in the schema, July 2011
 */

#ifndef _STA_SCENARIO_H_
#define _STA_SCENARIO_H_

#include "staschema.h"

#include "Astro-Core/statevector.h"
#include "Astro-Core/stabody.h"
#include "Astro-Core/attitudevector.h"

// Utility functions for space scenario objects.

extern sta::StateVector
        AbstractPositionToStateVector(const ScenarioAbstract6DOFPositionType* position,
                                      const StaBody* centralBody);
extern sta::KeplerianElements
        AbstractPositionToKeplerianElements(const ScenarioAbstract6DOFPositionType* position,
                                            const StaBody* centralBody);

extern staAttitude::AttitudeVector
        Euler123ToAttitudeVector(const ScenarioAbstract6DOFAttitudeType* attitudeEuler);

extern staAttitude::AttitudeVector
        Euler321ToAttitudeVector(const ScenarioAbstract6DOFAttitudeType* attitudeEuler);

extern staAttitude::AttitudeVector
        Euler313ToAttitudeVector(const ScenarioAbstract6DOFAttitudeType* attitudeEuler);

extern staAttitude::AttitudeVector
        QuaternionJPLToAttitudeVector(const ScenarioAbstract8DOFAttitudeType* attitudeQuaternion);

extern staAttitude::AttitudeVector
        QuaternionESAToAttitudeVector(const ScenarioAbstract8DOFAttitudeType* attitudeQuaternion);


#endif // _STA_SCENARIO_H_

