//-------------------------------------------------------------------------------------------------
// File name: RVDParameters.cpp
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
#include "stdlib.h"
#include <vector>
#include "math.h"
#include <fstream>
#include "RVDParameters.h"

using namespace std;


//this function inside the class RVDData is the one that actually does the reading job
void createRVDInput(TimeInput& time, int& central_body, Target& target, Chaser& chaser, 
							 Perturbations& perturbations, ThirdBody& thirdbody, 
							 int& number_of_manoeuvres, double** manoeuvres)
{
	//opens the pipe it is going to read from
	std::ifstream myfile("/tmp/rendezvouspipe.stap");
	
	//declares the parameters to be filled in
//	Chaser chaser;
//	Target target;
//	TimeInput time;
//	Perturbations perturbations;	
//	ThirdBody thirdbody;
//	int central_body;
//	vector<double> manoeuvres;
	
	//time parameters
	myfile >> time.start_day;	//cout<<time.start_day<<endl;
	myfile >> time.start_month;
	myfile >> time.start_year;
	myfile >> time.start_hour;			
	myfile >> time.start_minute;			
	myfile >> time.start_second;			
	myfile >> time.timestep;				
	
	//central body
	myfile >> central_body;		
	
	//target spacecraft parameters
	myfile >> target.r;   
	myfile >> target.i;
	myfile >> target.RAAN;
	myfile >> target.true_anomaly;
	myfile >> target.arg_of_periapsis;
	myfile >> target.eccentricity;
	myfile >> target.mass;
	myfile >> target.aero_drag_coeff;
	myfile >> target.aero_wet_area;
	myfile >> target.reflection_coefficient;
	
	//chaser spacecraft parameters
	myfile >> chaser.x; cout<<chaser.x<<endl;
	myfile >> chaser.z;
	myfile >> chaser.mass;
	myfile >> chaser.chemical_Isp;
	myfile >> chaser.aero_drag_coeff;
	myfile >> chaser.aero_wet_area;
	myfile >> chaser.reflection_coefficient; cout<<"chaser ref coeff: "<<chaser.reflection_coefficient<<endl;
		
	//selection of perturbations
	myfile >> perturbations.solar;
	myfile >> perturbations.atmospheric;		
	myfile >> perturbations.sphericity;
	//selection of triaxiality perturbations
	myfile >> thirdbody.Mercury_triaxiality;
	myfile >> thirdbody.Venus_triaxiality;
	myfile >> thirdbody.Earth_triaxiality;
	myfile >> thirdbody.Mars_triaxiality;
	myfile >> thirdbody.Jupiter_triaxiality;
	myfile >> thirdbody.Saturn_triaxiality;
	myfile >> thirdbody.Uranus_triaxiality;
	myfile >> thirdbody.Neptune_triaxiality;
	myfile >> thirdbody.Pluto_triaxiality; 
	myfile >> thirdbody.Sun_triaxiality;
	myfile >> thirdbody.Moon_triaxiality;
	myfile >> thirdbody.Deimos_triaxiality;
	myfile >> thirdbody.Europa_triaxiality;
	myfile >> thirdbody.Titan_triaxiality;
	myfile >> thirdbody.Enceladus_triaxiality;
	myfile >> thirdbody.Triton_triaxiality;
	
	//manoeuvres parameters
	//double data;
	
	myfile >> number_of_manoeuvres; cout<<"number of manoeuvres: "<<number_of_manoeuvres<<endl;
    if (!myfile.good())
    {
        cout << "Error reading from rendezvous pipe file\n";
        return;
    }
    
	int manoeuvre_lines = 8*number_of_manoeuvres;
    
    *manoeuvres = new double[manoeuvre_lines];
    if (manoeuvres == NULL)
    {
        cout << "createdRVDInput(): Failed to allocate memory for manoeuvres" << endl;
        return;
    }
			
	//stack the manoeuvre parameters in the vector
	for(int count=0; count<manoeuvre_lines; count++)
	{
		//myfile>>data;
		//manoeuvres.push_back( data );  Irene: this does not work
		//manoeuvres[count] = data;	// this works
		myfile >> (*manoeuvres)[count];
		//cout<<manoeuvres[count]<<endl;
		
	}
	myfile.close(); cout << "I closed the input pipe file"<<endl;
	cout<<"number of manoeuvres: "<<number_of_manoeuvres<<endl;
	
}

//this is a line that makes homage to Lara Croft