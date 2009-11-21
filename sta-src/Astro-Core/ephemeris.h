/*

 This program is free software; you can redistribute it and/or modify it under
 the terms of the GNU General Public License as published by the Free Software
 Foundation; either version 2 of the License, or (at your option) any later
 version.

 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE. See the GNU General Public License for more
 details.

 You should have received a copy of the GNU General Public License along with
 this program; if not, write to the Free Software Foundation, Inc., 59 Temple
 Place - Suite 330, Boston, MA 02111-1307, USA.
 Further information about the GNU General Public License can also be found on
 the world wide web at http://www.gnu.org.
 */


/*
 --- Copyright (C) 2009 STA Steering Board (space.trajectory.analysis AT gmail.com) ---
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
