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

Converts true anomaly to Mean anomaly using the eccentric anomaly.

The true anomaly is an angular parameter that defines the position of a body moving
along a Keplerian orbit. It is the angle between the direction of periapsis and the
current position of the body, as seen from the main focus of the ellipse (the point
around which the object orbits).

Similarly, the  mean anomaly is a parameter that also defines the position of a
body that is moving in a Kepler orbit.
It is defined as the time since the last periapsis (closest approach
to the central body) times 2π / T, where T is the duration of a full orbit.
The mean anomaly increases uniformly from 0 to 2π radians during each orbit.

The mean anomaly M can be computed from the eccentric anomaly E by the formula

                        M = E - e \cdot \sin E

To find the position of the object in an elliptic Kepler orbit at a given time t,
the corresponding mean anomaly is determined and then the corresponding
eccentric anomaly is found by solving this equation numerically.



------------------ Author:       Guillermo Ortega               -------------------
------------------ Affiliation:  European Space Agency (ESA)    -------------------
-----------------------------------------------------------------------------------

*/



#ifndef TRUEANOMALYTOMEANANOMALY_H
#define TRUEANOMALYTOMEANANOMALY_H


double trueAnomalyTOmeanAnomaly (
                                 double trueAnomaly,        // The true anomaly [rad]
                                 double eccentricity        // The eccentricity of the orbit [-]
                                 );

#endif // TRUEANOMALYTOMEANANOMALY_H
