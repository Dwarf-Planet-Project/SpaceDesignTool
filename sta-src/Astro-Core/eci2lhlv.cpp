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
 ------------------ Author:  Annalisa Riccardi   -----------------------------------------
 ------------------ E-mail: (nina1983@gmail.com) -----------------------------------------
 Modified by Tiziana Sabatini on July 2009
 */
#include<Eigen/Core>
#include<QDebug>

USING_PART_OF_NAMESPACE_EIGEN;


void inertialTOlocal(double xInertial, double yInertial, double zInertial, double i, double Omega, double t, double w,double& xlocal, double& ylocal, double& zlocal)
{
        // phase angle
        double phi = w+t;

        Matrix3d conversion;
        conversion <<   cos(phi)*cos(Omega) - sin(Omega)*cos(i)*sin(phi),   cos(phi)*sin(Omega) + cos(Omega)*cos(i)*sin(phi), sin(i)*sin(phi),
                      - sin(phi)*cos(Omega) - sin(Omega)*cos(i)*cos(phi), - sin(phi)*sin(Omega) + cos(Omega)*cos(i)*cos(phi), sin(i)*cos(phi),
                        sin(i)*sin(Omega),  - sin(i)*cos(Omega),  cos(i);

        Vector3d local, inertial;
        inertial << xInertial, yInertial, zInertial;
        local = conversion * inertial;
        xlocal = local.x();
        ylocal = local.y();
        zlocal = local.z();

}
