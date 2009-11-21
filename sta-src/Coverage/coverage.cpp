//-----------------------------------------------------------------------------------
//
//
//-Copyright (C) 2007 European Space Agency (space.trajectory.analysis AT gmail.com) -
//
// This program is free software; you can redistribute it and/or modify it under
// the terms of the GNU Lesser General Public License as published by the Free Software
// Foundation; either version 2 of the License, or (at your option) any later
// version.
//
// This program is distributed in the hope that it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
// FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more
// details.
//
// You should have received a copy of the GNU Lesser General Public License along with
// this program; if not, write to the Free Software Foundation, Inc., 59 Temple
// Place - Suite 330, Boston, MA  02111-1307, USA.
//
// Further information about the GNU Lesser General Public License can also be found on
// the world wide web at http://www.gnu.org.
//
//
//  Converts TLE elements into Orbital Elements
//
//
//
//
//------------------ Author:       Guillermo Ortega               -------------------
//------------------ Affiliation:  European Space Agency (ESA)    -------------------
//-----------------------------------------------------------------------------------
// Created June 7th 2009

#ifndef COVERAGE_H
#define COVERAGE_H

#include<math.h>

#include <QString>

#include "Eigen/Geometry"

#include "statevector.h"
#include "Astro-Core/stabody.h"

#include "coverage.h"

using namespace sta;


/*
  Creates a 3x3 matrix with a rotation angle of Angle [degres] along X-axis
A = [1      0       0;
     0      cos(x)  sin(x);
     0      -sin(x) cos(x)];
*/
Eigen::Matrix3f 3DRotationMatrixAlongY(double Angle)
{
    //Defining the return type
    Eigen::Matrix3f MyMatrix;
    MyMatrix << 1.0, 0.0, 0.0,
                 0.0, cos(Angle), sin(Angle),
                 0.0, -sin(Angle), cos(Angle);
    return MyMatrix;

};


/*
  Creates a 3x3 matrix with a rotation angle of Angle [degres] along Y-axis
  A = [cos(x)  0      -sin(x);
     0       1      0;
     sin(x)  0      cos(x)];
*/
Eigen::Matrix3f 3DRotationMatrixAlongY(double Angle)
{
    //Defining the return type
    Eigen::Matrix3f MyMatrix;
    MyMatrix << cos(Angle), 0.0 -sin(Angle),
                 0.0, 1.0, 0.0,
                 sin(Angle), 0.0, cos(Angle);
    return MyMatrix;

};



/*
  Creates a 3x3 matrix with a rotation angle of Angle [degres] along Z-axis
A = [cos(x)  sin(x)     0;
     -sin(x) cos(x)     0;
     0       0          1];
*/
Eigen::Matrix3f 3DRotationMatrixAlongZ(double Angle)
{
    //Defining the return type
    Eigen::Matrix3f MyMatrix;
    MyMatrix << cos(Angle), sin(Angle), 0.0,
                 -sin(Angle), cos(Angle), 0.0
                 0.0, 0.0, 1.0;
    return MyMatrix;

};




/*
  Obtains a Polar state vector (Range, Elevation, Azimuth) of an orbiting participant around
  a body from a given ground station coodinates (Altitude, Latitude, Longitude).
  This rountine uses the centered fixed coordinates of the participant on the body.
*/
sta::PolarVector ParticipantVisibilityFromGroundStation (sta::StateVector      cartesianStateVector,   //[x,y,z,Vx,Vy,Vz in planet fixed
                                                         sta::GeographicVector geographicStateVector,  //[Altitude, Latitude, Longitude]
                                                         QString          Body)                       //Body

// output shall be [Range, Elevation, Azimuth, ...]
{

    /* Note that for the time being this routine discards the last 3 elements of the StateVector,
    GeographicVector, and of course the PolarVector
    */

    // Defining intemediate variables
    Eigen::Vector3d RSite, RParticipant, RotationVector;
    Eigen::Vector2d ReducedRotationVector;
    double MyRange, SinAzimuth, CosAzimuth, SinElevation, CosElevation;

    // Defining the output variable
    sta::PolarVector polarStateVector;

    //Quering the body features
    StaBody* MyBody = STA_SOLAR_SYSTEM->lookup(Body);
    double MyBodyRadius = MyBody->meanRadius();

    // Computing range
    MyRange = MyBodyRadius + geographicStateVector.Altitude;

    // Site coordinates
    RSite(0) = MyRange*cos(geographicStateVector.Latitude)*cos(geographicStateVector.Longitude);
    RSite(1) = MyRange*cos(geographicStateVector.Latitude)*sin(geographicStateVector.Longitude);
    RSite(2) = MyRange*sin(geographicStateVector.Latitude);

    RParticipant = cartesianStateVector.position - RSite;


    //Rotation matrices
    RotationVector = 3DRotationMatrixAlongY((3.1415927/2)-geographicStateVector.Latitude)*3DRotationMatrixAlongZ(geographicStateVector.Longitude)*RParticipant;
    ReducedRotationVector <<  RotationVector(0), RotationVector(1);

    // Computing range
    polarStateVector.Range = RotationMatrix.norm();

    // Computing Sin-Cos of AZ-El
    SinElevation = RotationVector(2) / polarStateVector.Range;
    CosElevation = ReducedRotationVector.norm() / polarStateVector.Range;
    SinAzimuth = RotationVector(1) / ReducedRotationVector.norm();
    CosAzimuth = -RotationVector(0) / ReducedRotationVector.norm();


    polarStateVector.Elevation = atan2(SinElevation, CosElevation);
    polarStateVector.Azimuth = atan2(SinAzimuth,CosAzimuth);

    return polarStateVector;



}



#endif // COVERAGE_H




