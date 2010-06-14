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


#define Pi 3.1415926535
#include "math.h"
#include <iostream>
using namespace std;
#include "PlanetParameters.h"

//----------------------------------------------------------------------------------------------------------------------------------			 
//The functions of Planet data determine the values of the environment according to the environment (planet) chosen
			 
 double Planet::getMu(int central_body)
{
	if(central_body==1)						
			 { double mu = 22032;return mu;}
	if(central_body==2)							
			 { double mu = 324859;return mu;}
	if(central_body==3)
			 { double mu = 398600;return mu;}
	if(central_body==4)
			 { double mu = 42828;return mu;}
	if(central_body==5)
			 { double mu = 126686534;return mu;}
	if(central_body==6)
			 { double mu = 37931187;return mu;}
	if(central_body==7)
			 { double mu = 5793947;return mu;}
	if(central_body==8)
			 { double mu = 6836529;return mu;}
	if(central_body==9)
			 { double mu = 10012;return mu;}
}
			 
double Planet::getOmega(int central_body)
{
	if(central_body==1)
			 {double omega_planet = 0; return omega_planet;}
	if(central_body==2)
			 {double omega_planet = 0; return omega_planet;}
	if(central_body==3)
			 {double omega_planet = 7.29211564E-05; return omega_planet;}
	if(central_body==4)
			 {double omega_planet = 0; return omega_planet;}
	if(central_body==5)
			 {double omega_planet = 0; return omega_planet;}
	if(central_body==6)
			 {double omega_planet = 0; return omega_planet;}
	if(central_body==7)
			 {double omega_planet = 0; return omega_planet;}
	if(central_body==8)
			 {double omega_planet = 0; return omega_planet;}
	if(central_body==9)
			 {double omega_planet = 0; return omega_planet;}
}

double Planet::getRadius(int central_body)
{
	if(central_body==1)
			 {double radius = 2439.7; return radius;}
	if(central_body==2)
			 {double radius = 6051.9; return radius;}
	if(central_body==3)
			 {double radius = 6378.135; return radius;}
	if(central_body==4)
			 {double radius = 3402.5; return radius;}
	if(central_body==5)
			 {double radius = 71492; return radius;}
	if(central_body==6)
			 {double radius = 60268; return radius;}
	if(central_body==7)
			 {double radius = 24764; return radius;}
	if(central_body==8)
			 {double radius = 25559; return radius;}
	if(central_body==9)
			 {double radius = 1195; return radius;}
}

double Planet::getJ2(int central_body)
{
	if(central_body==1)
			 {double J2 = 0; return J2;}
	if(central_body==2)
			 {double J2 = 0; return J2;}
	if(central_body==3)
			 {double J2 = 1.08263E-03; return J2;}
	if(central_body==4)
			 {double J2 = 0; return J2;}
	if(central_body==5)
			 {double J2 = 0; return J2;}
 	if(central_body==6)
 			 {double J2 = 0; return J2;}
 	if(central_body==7)
 			 {double J2 = 0; return J2;}
 	if(central_body==8)
 			 {double J2 = 0; return J2;}
 	if(central_body==9)
 			 {double J2 = 0; return J2;}
 }

 double Planet::getJ3(int central_body)
{
 	if(central_body==1)
	{double J3 = 0; return J3;}
 	if(central_body==2)
 			 {double J3 = 0;return J3;}
 	if(central_body==3)
 			 {double J3 = 2.54E-06;return J3;}
 	if(central_body==4)
 			 {double J3 = 0;return J3;}
 	if(central_body==5)
 			 {double J3 = 0;return J3;}
 	if(central_body==6)
 			 {double J3 = 0;return J3;}
 	if(central_body==7)
 			 {double J3 = 0;return J3;}
 	if(central_body==8)
 			 {double J3 = 0;return J3;}
 	if(central_body==9)
 			 {double J3 = 0;return J3;}
 }

 double Planet::getJ4(int central_body)
{
 	if(central_body==1)
 			 {double J4 = 0;return J4;}
 	if(central_body==2)
 			 {double J4 = 0;return J4;}
 	if(central_body==3)
 			 {double J4 = 1.61E-06;return J4;}
 	if(central_body==4)
 			 {double J4 = 0;return J4;}
 	if(central_body==5)
 			 {double J4 = 0;return J4;}
 	if(central_body==6)
 			 {double J4 = 0;return J4;}
 	if(central_body==7)
 			 {double J4 = 0;return J4;}
 	if(central_body==8)
 			 {double J4 = 0;return J4;}
 	if(central_body==9)
 			 {double J4 = 0;return J4;}
 }

 double Planet::getGravity(int central_body, double r, double delta)
{
 	if(central_body==1)
 			{double gr = (Planet::getMu(1)/pow(r,2))*(1.0 - (3.0/2.0)*Planet::getJ2(1)*pow((Planet::getRadius(1)/r),2)*(3.0*pow(sin(delta),2) - 1.0) - 2.0*Planet::getJ3(1)*pow((Planet::getRadius(1)/r),3)*sin(delta)*(5.0*pow(sin(delta),2) - 3.0) - (5.0/8.0)*Planet::getJ4(1)*pow((Planet::getRadius(1)/r),4)*(35.0*pow(sin(delta),4) - 30.0*pow(sin(delta),2) + 3.0)) ; return gr;}
 	if(central_body==2)
 			{double gr = (Planet::getMu(2)/pow(r,2))*(1.0 - (3.0/2.0)*Planet::getJ2(2)*pow((Planet::getRadius(2)/r),2)*(3.0*pow(sin(delta),2) - 1.0) - 2.0*Planet::getJ3(2)*pow((Planet::getRadius(2)/r),3)*sin(delta)*(5.0*pow(sin(delta),2) - 3.0) - (5.0/8.0)*Planet::getJ4(2)*pow((Planet::getRadius(2)/r),4)*(35.0*pow(sin(delta),4) - 30.0*pow(sin(delta),2) + 3.0)) ; return gr;}
 	if(central_body==3)
 			{double gr = (Planet::getMu(3)/pow(r,2))*(1.0 - (3.0/2.0)*Planet::getJ2(3)*pow((Planet::getRadius(3)/r),2)*(3.0*pow(sin(delta),2) - 1.0) - 2.0*Planet::getJ3(3)*pow((Planet::getRadius(3)/r),3)*sin(delta)*(5.0*pow(sin(delta),2) - 3.0) - (5.0/8.0)*Planet::getJ4(3)*pow((Planet::getRadius(3)/r),4)*(35.0*pow(sin(delta),4) - 30.0*pow(sin(delta),2) + 3.0)) ; return gr;}
 	if(central_body==4)
 			{double gr = (Planet::getMu(4)/pow(r,2))*(1.0 - (3.0/2.0)*Planet::getJ2(4)*pow((Planet::getRadius(4)/r),2)*(3.0*pow(sin(delta),2) - 1.0) - 2.0*Planet::getJ3(4)*pow((Planet::getRadius(4)/r),3)*sin(delta)*(5.0*pow(sin(delta),2) - 3.0) - (5.0/8.0)*Planet::getJ4(4)*pow((Planet::getRadius(4)/r),4)*(35.0*pow(sin(delta),4) - 30.0*pow(sin(delta),2) + 3.0)) ; return gr;}
 	if(central_body==5)
 			 {double gr = (Planet::getMu(5)/pow(r,2))*(1.0 - (3.0/2.0)*Planet::getJ2(5)*pow((Planet::getRadius(5)/r),2)*(3.0*pow(sin(delta),2) - 1.0) - 2.0*Planet::getJ3(5)*pow((Planet::getRadius(5)/r),3)*sin(delta)*(5.0*pow(sin(delta),2) - 3.0) - (5.0/8.0)*Planet::getJ4(5)*pow((Planet::getRadius(5)/r),4)*(35.0*pow(sin(delta),4) - 30.0*pow(sin(delta),2) + 3.0)) ; return gr;}
 	if(central_body==6)
 			 {double gr = (Planet::getMu(6)/pow(r,2))*(1.0 - (3.0/2.0)*Planet::getJ2(6)*pow((Planet::getRadius(6)/r),2)*(3.0*pow(sin(delta),2) - 1.0) - 2.0*Planet::getJ3(6)*pow((Planet::getRadius(6)/r),3)*sin(delta)*(5.0*pow(sin(delta),2) - 3.0) - (5.0/8.0)*Planet::getJ4(6)*pow((Planet::getRadius(6)/r),4)*(35.0*pow(sin(delta),4) - 30.0*pow(sin(delta),2) + 3.0)) ; return gr;}
 	if(central_body==7)
 			 {double gr = (Planet::getMu(7)/pow(r,2))*(1.0 - (3.0/2.0)*Planet::getJ2(7)*pow((Planet::getRadius(7)/r),2)*(3.0*pow(sin(delta),2) - 1.0) - 2.0*Planet::getJ3(7)*pow((Planet::getRadius(7)/r),3)*sin(delta)*(5.0*pow(sin(delta),2) - 3.0) - (5.0/8.0)*Planet::getJ4(7)*pow((Planet::getRadius(7)/r),4)*(35.0*pow(sin(delta),4) - 30.0*pow(sin(delta),2) + 3.0)) ; return gr;}
 	if(central_body==8)
 			 {double gr = (Planet::getMu(8)/pow(r,2))*(1.0 - (3.0/2.0)*Planet::getJ2(8)*pow((Planet::getRadius(8)/r),2)*(3.0*pow(sin(delta),2) - 1.0) - 2.0*Planet::getJ3(8)*pow((Planet::getRadius(8)/r),3)*sin(delta)*(5.0*pow(sin(delta),2) - 3.0) - (5.0/8.0)*Planet::getJ4(8)*pow((Planet::getRadius(8)/r),4)*(35.0*pow(sin(delta),4) - 30.0*pow(sin(delta),2) + 3.0)) ; return gr;}
 	if(central_body==9)
 			 {double gr = (Planet::getMu(9)/pow(r,2))*(1.0 - (3.0/2.0)*Planet::getJ2(9)*pow((Planet::getRadius(9)/r),2)*(3.0*pow(sin(delta),2) - 1.0) - 2.0*Planet::getJ3(9)*pow((Planet::getRadius(9)/r),3)*sin(delta)*(5.0*pow(sin(delta),2) - 3.0) - (5.0/8.0)*Planet::getJ4(9)*pow((Planet::getRadius(9)/r),4)*(35.0*pow(sin(delta),4) - 30.0*pow(sin(delta),2) + 3.0)) ; return gr;}
}

double Planet::getGDelta(int central_body, double r, double delta)
 {
 		if(central_body==1)
 		{double gdelta = -3.0*(Planet::getMu(1)/pow(r,2))*pow((radius/r),2)*sin(delta)*cos(delta) * (Planet::getJ2(1) + (1.0/2.0*sin(delta))*Planet::getJ3(1)*(radius/r)*(5.0*pow(sin(delta),2) - 1.0) + (5.0/6.0)*Planet::getJ4(1)*pow((radius/r),2)*(7.0*pow(sin(delta),2) - 3.0)) ; return gdelta;}
 		if(central_body==2)
 		{double gdelta = -3.0*(Planet::getMu(2)/pow(r,2))*pow((radius/r),2)*sin(delta)*cos(delta) * (Planet::getJ2(2) + (1.0/2.0*sin(delta))*Planet::getJ3(2)*(radius/r)*(5.0*pow(sin(delta),2) - 1.0) + (5.0/6.0)*Planet::getJ4(2)*pow((radius/r),2)*(7.0*pow(sin(delta),2) - 3.0)) ; return gdelta;}
 		if(central_body==3)
 		{double gdelta = -3.0*(Planet::getMu(3)/pow(r,2))*pow((radius/r),2)*sin(delta)*cos(delta) * (Planet::getJ2(3) + (1.0/2.0*sin(delta))*Planet::getJ3(3)*(radius/r)*(5.0*pow(sin(delta),2) - 1.0) + (5.0/6.0)*Planet::getJ4(3)*pow((radius/r),2)*(7.0*pow(sin(delta),2) - 3.0)) ; return gdelta;}
 		if(central_body==4)
 		{double gdelta = -3.0*(Planet::getMu(4)/pow(r,2))*pow((radius/r),2)*sin(delta)*cos(delta) * (Planet::getJ2(4) + (1.0/2.0*sin(delta))*Planet::getJ3(4)*(radius/r)*(5.0*pow(sin(delta),2) - 1.0) + (5.0/6.0)*Planet::getJ4(4)*pow((radius/r),2)*(7.0*pow(sin(delta),2) - 3.0)) ; return gdelta;}
 		if(central_body==5)
 		{double gdelta = -3.0*(Planet::getMu(5)/pow(r,2))*pow((radius/r),2)*sin(delta)*cos(delta) * (Planet::getJ2(5) + (1.0/2.0*sin(delta))*Planet::getJ3(5)*(radius/r)*(5.0*pow(sin(delta),2) - 1.0) + (5.0/6.0)*Planet::getJ4(5)*pow((radius/r),2)*(7.0*pow(sin(delta),2) - 3.0)) ; return gdelta;}
 		if(central_body==6)
 		{double gdelta =-3.0*(Planet::getMu(6)/pow(r,2))*pow((radius/r),2)*sin(delta)*cos(delta) * (Planet::getJ2(6) + (1.0/2.0*sin(delta))*Planet::getJ3(6)*(radius/r)*(5.0*pow(sin(delta),2) - 1.0) + (5.0/6.0)*Planet::getJ4(6)*pow((radius/r),2)*(7.0*pow(sin(delta),2) - 3.0)) ; return gdelta;} 		
		if(central_body==7)
		{double gdelta =-3.0*(Planet::getMu(7)/pow(r,2))*pow((radius/r),2)*sin(delta)*cos(delta) * (Planet::getJ2(7) + (1.0/2.0*sin(delta))*Planet::getJ3(7)*(radius/r)*(5.0*pow(sin(delta),2) - 1.0) + (5.0/6.0)*Planet::getJ4(7)*pow((radius/r),2)*(7.0*pow(sin(delta),2) - 3.0)) ; return gdelta;} 			
		if(central_body==8)
 		{double gdelta = -3.0*(Planet::getMu(8)/pow(r,2))*pow((radius/r),2)*sin(delta)*cos(delta) * (Planet::getJ2(8) + (1.0/2.0*sin(delta))*Planet::getJ3(8)*(radius/r)*(5.0*pow(sin(delta),2) - 1.0) + (5.0/6.0)*Planet::getJ4(8)*pow((radius/r),2)*(7.0*pow(sin(delta),2) - 3.0)) ; return gdelta;}
 		if(central_body==9)
 		{double gdelta = -3.0*(Planet::getMu(9)/pow(r,2))*pow((radius/r),2)*sin(delta)*cos(delta) * (Planet::getJ2(9) + (1.0/2.0*sin(delta))*Planet::getJ3(9)*(radius/r)*(5.0*pow(sin(delta),2) - 1.0) + (5.0/6.0)*Planet::getJ4(9)*pow((radius/r),2)*(7.0*pow(sin(delta),2) - 3.0)) ; return gdelta;}
 }
			
 double Planet::getTemperature(int central_body, double altitude)
 {
 	if(central_body==1)							 
			{double temperature = 1.0; return temperature;}
 	if(central_body==2)							
 			 {double temperature = 1.0; return temperature;}
 	if(central_body==3)
 			 {double temperature = 1.0; return temperature;}
 	if(central_body==4)
 			 {double temperature = 1.0; return temperature;}
 	if(central_body==5)
 			 {double temperature = 1.0; return temperature;}
 	if(central_body==6)
 			 {double temperature = 1.0; return temperature;}
 	if(central_body==7)
 			 {double temperature = 1.0; return temperature;}
 	if(central_body==8)
 			 {double temperature = 1.0; return temperature;}
 	if(central_body==9)
 			 {double temperature = 1.0; return temperature;}
 }
	
 double Planet::getDensity(int central_body, double altitude)
 {
 	if(central_body==1)							 
			{double density = 1.0; return density;}
 	if(central_body==2)							
 			 {double density = 1.0;return density;}
 	if(central_body==3)
 			 {double density = 1.0;return density;}
 	if(central_body==4)
 			 {double density = 1.0;return density;}
 	if(central_body==5)
 			 {double density = 1.0;return density;}
 	if(central_body==6)
 			 {double density = 1.0;return density;}
 	if(central_body==7)
 			 {double density = 1.0;return density;}
 	if(central_body==8)
 			 {double density = 1.0;return density;}
 	if(central_body==9)
 			 {double density = 1.0;return density;}
 }
