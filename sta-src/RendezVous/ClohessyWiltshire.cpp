//-------------------------------------------------------------------------------------------------
// File name: ClohessyWiltshire.cpp
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


#include <iostream>
using namespace std;
#include <vector>
#include "math.h"

void CWD(double stepsize, double initial_time, double omega, double LHLV_initial_state_vector[8], double LHLV_final_state_vector[7], long double gamma[3])
{
	
	float x1 = LHLV_initial_state_vector[0];			//rewritten so CW equations are legible
	float y1 = LHLV_initial_state_vector[1];
	float z1 = LHLV_initial_state_vector[2];
	float vx1 = LHLV_initial_state_vector[3];
	float vy1 = LHLV_initial_state_vector[4];
	float vz1 = LHLV_initial_state_vector[5];
	double t = initial_time;
	
	LHLV_final_state_vector[0] = x1+z1*(6*omega*t-6*sin(omega*t))+vx1*((4/omega)*sin(omega*t)-3*t)+vz1*((2/omega)*(1-cos(omega*t)))
									+gamma[0]*((4/(omega*omega))*(1-cos(omega*t))-1.5*t*t) +gamma[2]*(2*(t/omega)-(2/(omega*omega))*sin(omega*t));									//x2
	LHLV_final_state_vector[1] = y1*cos(omega*t)+vy1*(sin(omega*t))*(1/omega)
									+gamma[1]*(1-cos(omega*t))/(omega*omega);																											//y2
	LHLV_final_state_vector[2] = z1*(4-3*cos(omega*t))-vx1*((2/omega)*(1-cos(omega*t)))+vz1*(sin(omega*t)/omega)
									+gamma[0]*((2/(omega*omega))*sin(omega*t)-2*(t/omega))+gamma[2]*(1/(omega*omega))*(1-cos(omega*t));													//z2
	LHLV_final_state_vector[3] = z1*6*omega*(1-cos(omega*t))+vx1*(4*cos(omega*t)-3)+vz1*2*sin(omega*t)
									+gamma[0]*((4/omega)*sin(omega*t)-3*t)+gamma[2]*(2/omega)*(1-cos(omega*t));																	//vx2
	LHLV_final_state_vector[4] = y1*-1*omega*sin(omega*t)+vy1*cos(omega*t)
									+gamma[1]*sin(omega*t)/omega;																												//vy2
	LHLV_final_state_vector[5] = z1*3*omega*sin(omega*t)-vx1*2*sin(omega*t)+vz1*cos(omega*t)
									-gamma[0]*(2/omega)*(1-cos(omega*t))+gamma[2]*sin(omega*t)/omega;																			// vz2
}