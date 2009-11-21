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
Converts true anomaly to Mean anomaly using the eccentric anomaly

------------------ Author:       Guillermo Ortega               -------------------
------------------ Affiliation:  European Space Agency (ESA)    -------------------
-----------------------------------------------------------------------------------

*/

#include <math.h>
#include "trueAnomalyTOmeanAnomaly.h"


double trueAnomalyTOmeanAnomaly (
                                 double trueAnomaly,
                                 double eccentricity
                                 )
{
    double sinE = sin(trueAnomaly) * sqrt(1.0 - eccentricity * eccentricity) / (1.0 + eccentricity * cos(trueAnomaly));
    double cosE = (eccentricity + cos(trueAnomaly)) / (1 + eccentricity * cos(trueAnomaly));
    // We calculate now the eccentric anomaly
    double eccAnomaly = atan2(sinE, cosE);
    double meanAnomaly = eccAnomaly - eccentricity * sin(eccAnomaly);  // Now in rad
    return meanAnomaly;

}

