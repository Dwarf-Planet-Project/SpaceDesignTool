//-------------------------------------------------------------------------------------------------
// File name: RVDParameters
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

#ifndef _RVDPARAMETERS_H_
#define _RVDPARAMETERS_H_

//#include <iostream>
#include "math.h"
#include <fstream>
#include <vector>

using namespace std;

//create objects chaser, target, perturbations and time
struct Chaser
{
	double x;
	double z ;
	double mass;
	double chemical_Isp;
	double aero_drag_coeff;
	double aero_wet_area;
	double reflection_coefficient;
};

struct Target
{
	double r;
	double i;
	double RAAN;
	double arg_of_periapsis;
	double true_anomaly;
	double eccentricity;
	double mass;
	double aero_drag_coeff;
	double aero_wet_area;
	double reflection_coefficient;
};

struct TimeInput
{
	double start_day;
	double start_month;
	double start_year;
	double start_hour;
	double start_minute;
	double start_second;
	double timestep;
};

struct Perturbations
{
	bool atmospheric;		
	bool solar;
	bool sphericity;
};

struct ThirdBody
{
	bool Venus_triaxiality;
	bool Mercury_triaxiality;
	bool Earth_triaxiality;
	bool Mars_triaxiality;
	bool Jupiter_triaxiality;
	bool Saturn_triaxiality;
	bool Neptune_triaxiality;
	bool Uranus_triaxiality;
	bool Pluto_triaxiality; 
	bool Sun_triaxiality;
	bool Moon_triaxiality;
	bool Deimos_triaxiality;
	bool Enceladus_triaxiality;
	bool Europa_triaxiality;
	bool Titan_triaxiality;
	bool Triton_triaxiality;
};


void createRVDInput(TimeInput& time, int& central_body, Target& target, Chaser& chaser, 
					Perturbations& perturbations, ThirdBody& thirdbody, 
					int& number_of_manoeuvres, double** manoeuvres);


#endif //_RVDPARAMETERS_H_

//this is a line that makes homage to Lara Croft