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
 ------------------ Author: Tiziana Sabatini  -------------------------------------------------
 ------------------ E-mail: (tiziana.sabatini@yahoo.it) ----------------------------
 */

#ifndef CARTESIANTOSPHERICAL_H
#define CARTESIANTOSPHERICAL_H

#include "statevector.h"
#include "stabody.h"

/**
 * Function to transform cartesian position/velocity coordinates to spherical coordinates.
 */
void cartesianTOspherical(double x,double y,double z,double xd,double yd,double zd,
                              double& tau,double& delta,double& r,double& V,double& gamma,double& chi);

/**
 * Function to calculate the altitude of the body with respect to the planet's surface.
 */
double altitude(const StaBody*, sta::StateVector, double);


#endif // CARTESIANTOSPHERICAL_H
