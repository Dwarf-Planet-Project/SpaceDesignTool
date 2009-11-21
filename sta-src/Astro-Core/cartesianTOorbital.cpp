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
------------------ Author:       Guillermo Ortega               -------------------
------------------ Affiliation:  European Space Agency (ESA)    -------------------
-----------------------------------------------------------------------------------
 Patched by Guillermo on July  11th 2009 to comply with STA statevector defintions and treat special cases
 when eccentricity is bigger than zero or when the true anomly or the argument of the perigee are not well
 defined. This routine needs to be converted into equinoctial elements for v2.0

 Added lines 227-230 by Tiziana Sabatini on July 2009 to avoid bugs with circular orbits
 */

#include<float.h>
#include<math.h>
#include<stdio.h>
#include<iostream>
#include <Eigen/Core>
#include <Eigen/Geometry>
#include "statevector.h"

#include <QTextStream>

#define Pi 3.1415926535


using namespace sta;

/* This function converts a state vector given in cartesian coordinates to an state vector given in
   classical orbital elements. For a defintion about the orbital elements, please see "statevector.h"
   /
 Description:
      This function transforms spacecraft cartesian orbital elements in Keplerian
      orbital elements

 Input:
      mu standard gravitational parameter of the planet/moon
      x  x-coordinate in ECI system (km)
      y  y-coordinate in ECI system (km)
      z  z-coordinate in ECI system (km)
      xd vx-coordinate in ECI system (km/s)
      yd vy-coordinate in ECI system (km/s)
      zd vz-coordinate in ECI system (km/s)

 Output:
      a  semi-major axis (km)
      ec eccentricity (-)
      i inclination (rad)
      w0 argument of the perigee (rad)
      o0  right ascention of the ascending node (rad)
      m0 mean anomaly (rad)


 Date: July 2009
 Version: 2.0
*/


/*

void cartesianTOorbital(double mu, double x, double y, double z, double xd, double yd, double zd,
                double& a,double& e,double& i,double& argperi,double& longnode,double& meananom)

{
  Deprecated
}

*/


#ifdef _MSC_VER


// Inverse hyperbolic trig functions are missing from the MSVC math library
double atanh(double x)
{
    return log((1.0 + x) / (1.0 - x)) / 2.0;
}

#endif


/*
   This function converts a state vector given in cartesian coordinates to an state vector given in
   classical orbital elements. For a defintion about the orbital elements, please see "statevector.h"
*/

sta::KeplerianElements cartesianTOorbital(double mu, sta::StateVector cartesianStateVector)
{

      double h, r, v, rdotv, rdot, trueAnomaly;
      double EccentricSINTrueAnomaly, EccentricCOSTrueAnomaly;
      double SINofTheNode, COSofTheNode;
      double ArgumentOfLatitudeSIN, ArgumentOfLatitudeCOS;
      double pseudoTrueAnomaly, pseudoEccentricity;
      double eccentricAnomaly;

      Eigen::Vector3d OrbitalMomentum;

      KeplerianElements foundKeplerianElements;

      // First find the direction of angular momentum vector
      // Compute orbital momentum vector, Eigen style
      OrbitalMomentum = cartesianStateVector.position.cross(cartesianStateVector.velocity);

      // Computing the vector norms
      h = OrbitalMomentum.norm();
      r = cartesianStateVector.position.norm();
      v = cartesianStateVector.velocity.norm();

      rdotv = cartesianStateVector.position.dot(cartesianStateVector.velocity);
      rdot  = rdotv / r;


      // Caculating INCLINATION
      foundKeplerianElements.Inclination = acos(OrbitalMomentum(2) / h );


      // Calculating the Ascending Node
      if (OrbitalMomentum(0) != 0.0 || OrbitalMomentum(1) != 0.0)
      {
          foundKeplerianElements.AscendingNode = atan2( OrbitalMomentum(0), -OrbitalMomentum(1) );

      }
      else
      {
          foundKeplerianElements.AscendingNode = 0.0;
      }


      // Calculating the Semimajor Axis
      foundKeplerianElements.SemimajorAxis = 1.0 / (2.0 / r - ( v * v) / mu);


      // Calculating the Eccentricity
      EccentricSINTrueAnomaly = rdot * h / mu;
      EccentricCOSTrueAnomaly = (( h * h ) / ( mu * r )) - 1.0;

      foundKeplerianElements.Eccentricity = sqrt( EccentricSINTrueAnomaly * EccentricSINTrueAnomaly + EccentricCOSTrueAnomaly * EccentricCOSTrueAnomaly );


      // Calulating the Argument of the Periapsis
      if (EccentricSINTrueAnomaly != 0.0 || EccentricCOSTrueAnomaly != 0.0)
      {
          trueAnomaly = atan2(EccentricSINTrueAnomaly, EccentricCOSTrueAnomaly);
      }
      else
      {
          trueAnomaly = 0.0;
      }

      SINofTheNode = sin(foundKeplerianElements.AscendingNode);
      COSofTheNode = cos(foundKeplerianElements.AscendingNode);

      ArgumentOfLatitudeSIN = (cartesianStateVector.position(1) * COSofTheNode - cartesianStateVector.position(0) * SINofTheNode ) / cos(foundKeplerianElements.Inclination);
      ArgumentOfLatitudeCOS = cartesianStateVector.position(0) * COSofTheNode + cartesianStateVector.position(1) * SINofTheNode;

      if (ArgumentOfLatitudeSIN != 0.0 || ArgumentOfLatitudeCOS != 0.0)
      {
          pseudoTrueAnomaly = atan2(ArgumentOfLatitudeSIN, ArgumentOfLatitudeCOS);
      }
      else
      {
          pseudoTrueAnomaly = 0.0;
      }

      foundKeplerianElements.ArgumentOfPeriapsis = pseudoTrueAnomaly - trueAnomaly;


      if (foundKeplerianElements.ArgumentOfPeriapsis > M_PI)
      {
          foundKeplerianElements.ArgumentOfPeriapsis -= 2.0 * M_PI;
      }

      if (foundKeplerianElements.ArgumentOfPeriapsis < -M_PI)
      {
          foundKeplerianElements.ArgumentOfPeriapsis += 2.0 * M_PI;
      }


      // Calculating now the Mean Anomaly
      pseudoEccentricity = sqrt( fabs(1.0 - foundKeplerianElements.Eccentricity) / (1.0 + foundKeplerianElements.Eccentricity) );

      if (foundKeplerianElements.Eccentricity < 1.0 )  // circular of elliptical case
      {
          eccentricAnomaly = 2.0 * atan (pseudoEccentricity * tan (trueAnomaly / 2.0 ) );
          foundKeplerianElements.MeanAnomaly = eccentricAnomaly - foundKeplerianElements.Eccentricity * sin (eccentricAnomaly);
          if (foundKeplerianElements.MeanAnomaly > M_PI) //Special case in which the mean anomly passed pi radians (elliptic orbit)
          {
              foundKeplerianElements.MeanAnomaly -= 2.0 * M_PI;
          }
          if (foundKeplerianElements.MeanAnomaly < - M_PI) //Special case in which the mean anomly passed -pi radians (elliptic orbit)
          {
              foundKeplerianElements.MeanAnomaly += 2.0 * M_PI;
          }
      }
      else
      {
          eccentricAnomaly = 2.0 * atanh (pseudoEccentricity * tan (trueAnomaly / 2.0 ) );
          foundKeplerianElements.MeanAnomaly = foundKeplerianElements.Eccentricity * sinh (eccentricAnomaly) - eccentricAnomaly;
      }

      foundKeplerianElements.TrueAnomaly = trueAnomaly;
      if (foundKeplerianElements.Eccentricity < 1e-15)
          foundKeplerianElements.TrueAnomaly = foundKeplerianElements.MeanAnomaly;
      foundKeplerianElements.EccentricAnomaly = eccentricAnomaly;

      // Finally putting all together and returning control
      return foundKeplerianElements;

}
