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

#include <QString>

#include "statevector.h"
#include "Astro-Core/stabody.h"

using namespace sta;


/*
  Obtains a Polar state vector (Range, Elevation, Azimuth) of an orbiting participant around
  a body from a given ground station coodinates (Altitude, Latitude, Longitude).
  This rountine uses the centered fixed coordinates of the participant on the body.
*/
sta::PolarVector ParticipantVisibilityFromGroundStation (sta::StateVector      cartesianStateVector,   //[x,y,z,Vx,Vy,Vz in planet fixed
                                                         sta::GeographicVector geographicStateVector,  //[Altitude, Latitude, Longitude]
                                                         QString          Body);                       //Body





#endif // COVERAGE_H
