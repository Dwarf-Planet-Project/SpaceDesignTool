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

#ifndef INTERPOLATORS_H
#define INTERPOLATORS_H

#include <Astro-Core/statevector.h>

using namespace sta;

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
                                         double interval);                // Time distance between x1 and x1




// Routine programmed C. Laurel
// Cubic interpolation between two state vectors v0 and v1.
// Interval is the length of time (in seconds) between the two state vectors.
// t is a value between 0 and 1 that specifies the time within the interval.
sta::StateVector cubicInterpolate(const sta::StateVector& v0,
                                         const sta::StateVector& v1,
                                         double t,
                                         double interval);


#endif // INTERPOLATORS_H
