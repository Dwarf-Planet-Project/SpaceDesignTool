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
