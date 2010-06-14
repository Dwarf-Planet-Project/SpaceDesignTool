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
//------------------ Author:       Irene Huertas                                -------------------
//------------------ Affiliation:  Delft University of Technology (TU Delft)    -------------------
//-------------------------------------------------------------------------------------------------

//#define Pi 3.1415926535
//#include "math.h"
//#include <iostream>
using namespace std;

// ------------------------------------------------------------------------------------------------------------------------------
// This class returns the data depending on the planet chosen
class Planet 
{	
public:
	double getMu(int central_body);				
	double getOmega(int central_body);
	double getRadius(int central_body);
	double getJ2(int central_body);
	double getJ3(int central_body);
	double getJ4(int central_body);
	double getGravity(int central_body, double r, double delta);
	double getGDelta(int central_body, double r, double delta);
	double getTemperature(int central_body, double altitude);
	double getDensity(int central_body, double altitude);
		 
private:
				int central_body;
				double mu;                            
				double omega_planet;                    
				double radius;                          
				double J2, J3, J4;                       
				double gr;
				double gdelta;        
				double temperature;           
				double density;
				double r;
				double delta;
				double altitude;
};
	 
//----------------------------------------------------------------------------------------------------------------------------------			 
//The functions of Planet data determine the values of the environment according to the environment (planet) chosen
			 
//double Planet::getMu(int);
			 
//double Planet::getOmega(int);

//double Planet::getRadius(int);

//double Planet::getJ2(int);

//double Planet::getJ3(int);

//double Planet::getJ4(int);

//double Planet::getGravity(int, double, double);

//double Planet::getGDelta(int, double, double);
			
//double Planet::getTemperature(int, double);
	
//double Planet::getDensity(int, double);
