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
STA uses interpolators based on the State Vector structures defined in "Astro-Core/statevector.h"
file. Vector and matrices operations are performed using the Eigen library.

Given a set of data points (x1 , y1 ) . . . (xn , yn ), the STA interpolators compute a
continuous interpolating function y(x) such that y(xi ) = yi

The interpolation is piecewise smooth, and its behavior at the end-points is determined by the
type of interpolation used.

*/

/*
 ------------------ Author: Guillermo Ortega ESA  -------------------------------------------
 */

#include <Eigen/Core>
#include <Eigen/Geometry>

#include <Astro-Core/statevector.h>

#include "Interpolators.h"

using namespace sta;
using namespace Eigen;

/*
 Linear interpolation between two state vectors x0 and x1.
interval is the length of time (in seconds) between the two state vectors.
t is a value between 0 and 1 that specifies the time within the interval.

If the two known points are given by the coordinates (x_0,y_0)
and (x_1,y_1), the linear interpolant is the straight line
between these points. For a value x in the interval (x_0, x_1),
the value y along the straight line is given from the equation

    \frac{y - y_0}{x - x_0} = \frac{y_1 - y_0}{x_1 - x_0}

Solving this equation for y, which is the unknown value at x, gives

    y = y_0 + (x-x_0)\frac{y_1 - y_0}{x_1-x_0}

which is the formula for linear interpolation in the interval
(x_0,x_1). Outside this interval, the formula is identical
to linear extrapolation.

*/
sta::StateVector linearInterpolate(const sta::StateVector& x0,    // First state vector
                                         const sta::StateVector& x1,     // Second state vector
                                         double t,                       // Time within x0 an x1
                                         double interval)                // Time distance between x1 and x1
{
    sta::StateVector result;
    result.position = x0.position + t * ((x1.position - x0.position) * (1.0 / interval));
    result.velocity = x0.velocity + t * ((x1.velocity - x0.velocity) * (1.0 / interval));
    return result;
}



// Routine programmed C. Laurel
// Cubic interpolation between two state vectors v0 and v1.
// Interval is the length of time (in seconds) between the two state vectors.
// t is a value between 0 and 1 that specifies the time within the interval.
sta::StateVector cubicInterpolate(const sta::StateVector& v0,
                                         const sta::StateVector& v1,
                                         double t,
                                         double interval)
{
    double t2 = t * t;
    double t3 = t2 * t;
    Vector3d a = 2.0 * (v0.position - v1.position) + interval * (v1.velocity + v0.velocity);
    Vector3d b = 3.0 * (v1.position - v0.position) - interval * (2.0 * v0.velocity + v1.velocity);
    Vector3d c = v0.velocity * interval;
    Vector3d d = v0.position;

    sta::StateVector result;
    result.position = a * t3 + b * t2 + c * t + d;
    result.velocity = (a * (3.0 * t2) + b * (2.0 * t) + c) * (1.0 / interval);

    return result;
}



