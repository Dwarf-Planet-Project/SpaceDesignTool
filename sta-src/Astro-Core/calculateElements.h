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
 ------ Copyright (C) 2011 STA Steering Board (space.trajectory.analysis AT gmail.com) ----
*/



//------------------ Author:       Guillermo Ortega               -------------------
//------------------ Affiliation:  European Space Agency (ESA)    -------------------
//-----------------------------------------------------------------------------------
// Created May 2011


#include "Astro-Core/statevector.h"
#include "Astro-Core/date.h"
#include "Astro-Core/stacoordsys.h"
#include "Astro-Core/stabody.h"
#include "Astro-Core/stamath.h"
#include "Astro-Core/calendarTOjulian.h"
#include "Astro-Core/cartesianTOorbital.h"
#include "Astro-Core/cartesianTOspherical.h"


sta::CoordinateSystem CoordSys(QString Coordinate);

double calcKeplerianElements(
            const sta::StateVector& Vector,
            const StaBody* Body,
            const QString& OrbElement,
             double mjd,
             const QString& FromCoordinate,
             const QString& ToCoordinate);


double calcDelaunayElements(const sta::StateVector& Vector,
                            const StaBody* Body,
                            const QString& OrbElement,
                            double mjd,
                            const QString& FromCoordinate,
                            const QString& ToCoordinate);

double calcEquinoctialElements(const sta::StateVector& Vector,
                               const StaBody* Body,
                               const QString& OrbElement,
                               double mjd,
                               const QString& FromCoordinate,
                               const QString& ToCoordinate);
