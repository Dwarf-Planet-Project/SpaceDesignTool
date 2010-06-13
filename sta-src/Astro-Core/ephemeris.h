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

#ifndef _STA_EPHEMERIS_H_
#define _STA_EPHEMERIS_H_

#include "stabody.h"
#include "stacoordsys.h"
#include "statevector.h"
#include <QList>

namespace sta
{

/** Interface for an ephemeris. The interface has a single function to calculate the
  * state vector of a body in a given coordinate system.
  */
class Ephemeris
{
public:
    virtual ~Ephemeris() {}

    virtual const QList<StaBodyId>& bodyList() const = 0;
    virtual const StaBody* parentBody(const StaBody* body) const = 0;
    virtual const sta::CoordinateSystemType coordinateSystem(const StaBody* body) const = 0;
    virtual StateVector stateVector(const StaBody* body,
                                    double mjd,
                                    const StaBody* center,
                                    sta::CoordinateSystemType coordSys) const = 0;

};

}

#endif // _STA_EPHEMERIS_H_
