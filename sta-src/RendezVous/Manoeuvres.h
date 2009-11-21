//-------------------------------------------------------------------------------------------------
// File name: Manoeuvres
//
//---- Copyright (C) 2007 European Space Agency (space.trajectory.analysis AT gmail.com) ----------
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
//------------------ Author:       Irene Huertas                                -------------------
//------------------ Affiliation:  Delft University of Technology (TU Delft)    -------------------
//-------------------------------------------------------------------------------------------------


//#define Pi 3.1415926535

//#include <iostream>
//using namespace std;
//#include "math.h"
//#include "ClohessyWiltshire.h"
//#include "Manoeuvres.h"

//******************************************************* Straight line V-bar approach
void StraightLineVBarApproach(double, double, double, double*, double, double, double*);				
//******************************************************* Station keeping
void StationKeeping(double, double, double, double, double*, double*);
//******************************************************* Continuous z-thrust transfer
void ContinuousZThrust(double, double, double, double*, double, double*);
//******************************************************* Impulsive Hopping on V-bar in radial direction
void ImpulsiveRadialHoppingVBar(double, double, double, double*, double, double*);
//******************************************************* Impulsive Hopping on V-bar in tangential direction
void ImpulsiveTangentialHoppingVBar(double, double, double, double*, double, double*);
//******************************************************* Hohmann transfer
void HohmannTransfer(double, double, double, double*, double, double*);
//******************************************************* Delta V
void DeltaV(double, double, double, double, double*, double, double, double, double*);
//******************************************************* Free Drift
void FreeDrift(double, double, double, double, double*, double*);
//********************************************************Straight line R-bar approach	DOES NOT WORK FOR ALL CASES
void StraightLineRBarApproach(double, double, double, double*, double, double, double*);			
//******************************************************** Continuous x-thrust transfer DOES NOT WORK
void ContinuousXThrust(double, double, double, double*, double, double*);
//******************************************************** Impulsive Hopping not on V-bar 
void Rephasing(double, double, double, double*, double, double*);
//******************************************************** Two points transfer NOT TESTED
void TwoPointsTransfer(double, double, double, double, double*, double, double, double*);

