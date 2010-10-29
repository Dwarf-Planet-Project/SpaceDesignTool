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


//
// Load JPL's DE200, DE405, and DE406 ephemerides and compute planet
// positions.

#ifndef _STA_ASTROCORE_SPICEEPHEMERIS_H_
#define _STA_ASTROCORE_SPICEEPHEMERIS_H_

#include <QtCore>
#include <vector>
#include "ephemeris.h"

class QIODevice;

namespace sta
{

class SpiceEphemeris : public Ephemeris
{
private:
    SpiceEphemeris();

public:
    virtual ~SpiceEphemeris();

    virtual const QList<StaBodyId>& bodyList() const;
    virtual const StaBody* parentBody(const StaBody* body) const;
    virtual const sta::CoordinateSystemType coordinateSystem(const StaBody* body) const;
    virtual StateVector stateVector(const StaBody* body,
                                    double mjd,
                                    const StaBody* center,
                                    sta::CoordinateSystemType coordSys) const;

    static SpiceEphemeris* load(QIODevice* device);

    double getStartDate() const;
    double getEndDate() const;

    static bool InitializeSpice(const QString& spiceKernelDirectory);

private:
    QList<StaBodyId> m_bodies;
};

} // namespace sta

#endif // _STA_ASTROCORE_SPICEEPHEMERIS_H_

