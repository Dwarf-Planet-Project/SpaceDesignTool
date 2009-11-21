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
 ------------------ Author: Annalisa Riccardi   -------------------------------------------------
 ------------------ E-mail: nina1983@gmail.com  ----------------------------
 */

/**
 * This function converts inertial planet-centered coordinates of position into spacecraft local
 * orbit frame
 * Input:
 *               xInertial				coordinate X in ECI (km)
 *               yInertial				coordinate Y in ECI (km)
 *               zInertial				coordinate Z in ECI (km)
 *               i					inclination (degree)
 *               Omega				        RAAN (degree)
 *               t					true anomaly (degree)
 *               w					argument of the perigee (degree)
 *
 * Output:
 *               xlocal					coordinate X in LHLV (km)
 *               ylocal					coordinate Y in LHLV (km)
 *               zlocal					coordinate Z in LHLV (km)
 */
void inertialTOlocal(double xInertial, double yInertial, double zInertial,
                double i, double Omega, double t, double w, double& xlocal, double& ylocal, double& zlocal);

