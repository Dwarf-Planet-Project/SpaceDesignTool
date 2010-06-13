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

