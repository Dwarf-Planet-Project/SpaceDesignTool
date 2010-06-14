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
//------------------ Author:       Guillermo Ortega               -------------------
//------------------ Affiliation:  European Space Agency (ESA)    -------------------
//-----------------------------------------------------------------------------------


#include "RendezvousCalculation.h"
#include "Manoeuvres.h"
#include "RVDParameters.h"
#include "PlanetParameters.h"
#include "math.h"
#include <iostream>

//load all needed files
#define Pi 3.1415926535
using namespace std;

void RendezvousCalculation()
{
	
	//load parameters from GUI
	
	//RVDData Rendezvous; 
	Chaser chaser1;
	Target target1;
	TimeInput time1;
	Perturbations perturbations;	
	ThirdBody thirdbody;
	int central_body;
	int number_of_manoeuvres;
	double* manoeuvres = NULL;
	
	
	createRVDInput(time1, central_body, target1, chaser1, perturbations, thirdbody, number_of_manoeuvres, &manoeuvres);
	//printf("chaser1.x: ");
	//cout<<chaser1.x<<endl;
	printf("\n");
    if (manoeuvres == NULL)
    {
        cout << "RendezvousCalculation: Error loading manouevres from file; abandoning calculation\n";
        return;
    }
	
	//load planet data
	// Irene, this class is NOT working correctly !!!!!!!
	Planet planet1;
	double mu_planet = planet1.getMu(central_body);					//loads the mu of the chosen planet
																	//double mu_planet = 398600;
																	//cout<<mu_planet<<endl;
	
	//create initial conditions
	double InitialConditions[8] = {chaser1.x, 0, chaser1.z, 0,0,0,0,0};	// initial state vector
	double omega = sqrt(mu_planet/pow(target1.r,3));					// reference frame angular velocity (thus target's)
	double stepsize = time1.timestep;									// stepsize
	double OutputVector[8];												// array to pass on the state vector between two manoeuvres
	int i;																// counter
	
	//initialise change parameters
	double duration, xdisplacement, zdisplacement, DeltaVx, DeltaVy, DeltaVz, approach_speed;
	
	//create output pipe files for data printing
	FILE* fp;
	FILE* fp1;
	fp = fopen("/tmp/currentEphemeris.stae", "w");
	fp1 = fopen("/tmp/currentMiscelaneous.stam", "w");
	//close the output pipe files
	fclose(fp);
	fclose(fp1);
	
	//read manoeuvres and execute them
	printf("Total_number_of_manoeuvres: ");
	cout<<number_of_manoeuvres<<endl;
	printf("\n");
	int position_number = 0;													//read the position of the manoeuvre vector
	int manoeuvre_type = manoeuvres[position_number];							//to determine the manoeuvre type
	printf("manoeuvre_type: ");
	cout<<manoeuvre_type<<endl;
	printf("\n");
	int manoeuvre_number = 1;	// First maneuver
	
	while (manoeuvre_number <= number_of_manoeuvres)
	{
		printf("manoeuvre_number: ");
		cout<<manoeuvre_number<<endl;	
		printf("manoeuvre_type: ");
		cout<<manoeuvre_type<<endl;
		printf("begin time: ");
		cout<<InitialConditions[6]<<endl;
		switch(manoeuvre_type)
		{
			case 0:																												//HOHMANN TRANSFER
				printf("Hohmann transfer: "); printf("\n");
				cout<<"begin position ("<<InitialConditions[0]<<","<<InitialConditions[2]<<")"<<endl;
				zdisplacement = manoeuvres[position_number+4];																		//reads the displacement in Z-direction
				HohmannTransfer(chaser1.chemical_Isp, omega, stepsize, InitialConditions, zdisplacement, OutputVector);									//make the impulsive hopping
				for (i=0; i<7; i++)
				{
					InitialConditions[i] = OutputVector[i];																			//update the Begin vector for next manoeuvre
				}			
					InitialConditions[6] = OutputVector[6];																			//update the time the next manoeuvre starts
				break;	
			case 1:																												//IMPULSIVE HOPPING ON V-BAR, RADIAL IMPULSE
				printf("Impulsive hopping on v-bar, radial: "); printf("\n");
				cout<<"begin position ("<<InitialConditions[0]<<","<<InitialConditions[2]<<")"<<endl;
				xdisplacement = manoeuvres[position_number+2];																		//reads the displacement in x-direction
				ImpulsiveRadialHoppingVBar(chaser1.chemical_Isp, omega, stepsize, InitialConditions, xdisplacement, OutputVector);	//make the impulsive hopping
				for (i=0; i<7; i++)
				{
					InitialConditions[i] = OutputVector[i];																			//update the Begin vector for next manoeuvre
				}
					InitialConditions[6] = OutputVector[6];																			//update the time the next manoeuvre starts
				break;	
			case 2:																												//IMPULSIVE HOPPING ON V-BAR, TANGENTIAL IMPULSE
				printf("Impulsive hopping on v-bar, tangential: "); printf("\n");
				cout<<"begin position ("<<InitialConditions[0]<<","<<InitialConditions[2]<<")"<<endl;
				xdisplacement = manoeuvres[position_number+2];																		//reads the displacement in x-direction
				ImpulsiveTangentialHoppingVBar(chaser1.chemical_Isp, omega, stepsize, InitialConditions, xdisplacement, OutputVector);					//make the impulsive hopping
				for (i=0; i<7; i++)
				{
					InitialConditions[i] = OutputVector[i];																			//update the Begin vector for next manoeuvre
				}
					InitialConditions[6] = OutputVector[6];																			//update the time the next manoeuvre starts
				break;
			case 3:																												//IMPULSIVE HOPPING NOT ON V-BAR
				printf(" Impulsive hopping not on v-bar: "); printf("\n");
				cout<<"begin position ("<<InitialConditions[0]<<","<<InitialConditions[2]<<")"<<endl;
				xdisplacement = manoeuvres[position_number+2];																		//read the displacement in x direction
				Rephasing(chaser1.chemical_Isp, omega, stepsize, InitialConditions, xdisplacement, OutputVector);											// make the rephasing
				for (i=0; i<7; i++)
				{
					InitialConditions[i] = OutputVector[i];																			//update the Begin vector for next manoeuvre
				}
					InitialConditions[6] = OutputVector[6];																			//update the time the next manoeuvre starts
				break;
			case 4:																	//STRAIGHT LINE V-BAR APPROACH
				printf("STRAIGHT LINE V-BAR APPROACH: "); printf("\n");
				cout<<"begin position ("<<InitialConditions[0]<<","<<InitialConditions[2]<<")"<<endl;
				xdisplacement = manoeuvres[position_number+2];																		// read the displacement in x direction
				approach_speed = manoeuvres[position_number+5];																		// read the approach speed
				StraightLineVBarApproach(chaser1.chemical_Isp, omega, stepsize, InitialConditions, xdisplacement, approach_speed, OutputVector);			//make the approach
				for (i=0; i<7; i++)
				{
					InitialConditions[i] = OutputVector[i];																			//update the Begin vector for next manoeuvre
				}
					InitialConditions[6] = OutputVector[6];																			//update the time the next manoeuvre starts
				break;
			case 5:																													//STRAIGHT LINE R-BAR APPROACH//CONTINUOUS HOPPING ON V-BAR
				printf("STRAIGHT LINE R-BAR APPROACH: "); printf("\n");	
				cout<<"begin position ("<<InitialConditions[0]<<","<<InitialConditions[2]<<")"<<endl;
				zdisplacement = manoeuvres[position_number+4];	cout<<"z-displ: "<<zdisplacement<<endl;																	// read the displacement in z direction
				approach_speed = manoeuvres[position_number+7];																		// read the approach speed
				StraightLineRBarApproach(chaser1.chemical_Isp, omega, stepsize, InitialConditions, zdisplacement, approach_speed, OutputVector);			//make the approach
				for (i=0; i<7; i++)
				{
					InitialConditions[i] = OutputVector[i];																			//update the Begin vector for next manoeuvre
				}
					InitialConditions[6] = OutputVector[6];																			//update the time the next manoeuvre starts
				break;
			case 6:																												//CONTINUOUS HOPPING ON V-BAR
				printf("Commencing CONTINUOUS HOPPING ON V-BAR"); printf("\n");
				cout<<"begin position ("<<InitialConditions[0]<<","<<InitialConditions[2]<<")"<<endl;
				xdisplacement = manoeuvres[position_number+2];																		//reads the displacement in x-direction
				ContinuousZThrust(chaser1.chemical_Isp, omega, stepsize, InitialConditions, xdisplacement, OutputVector);									//make the continuous hopping
				{
					InitialConditions[i] = OutputVector[i];																			//update the Begin vector for next manoeuvre
				}
					InitialConditions[6] = OutputVector[6];																				//update the time the next manoeuvre starts
				break;	
			case 7:																												//CONTINOUS X-THRUST TRANSFER
				printf("Commencing X-thrust transfer"); printf("\n");
				cout<<"begin position ("<<InitialConditions[0]<<","<<InitialConditions[2]<<")"<<endl;
				zdisplacement = manoeuvres[position_number+4];																		//read the displacement in z direction
				ContinuousXThrust(chaser1.chemical_Isp, omega, stepsize, InitialConditions, zdisplacement, OutputVector);									//make the z-transfer with continuous x-thrust
				for (i=0; i<7; i++)
				{
					InitialConditions[i] = OutputVector[i];																			//update the Begin vector for next manoeuvre
				}
					InitialConditions[6] = OutputVector[6];																			//update the time the next manoeuvre starts
				break;
			case 8:																												//TWO-POINTS TRANSFER
				printf("Commencing Two points transfer"); printf("\n");
				cout<<"begin position ("<<InitialConditions[0]<<","<<InitialConditions[2]<<")"<<endl;
				duration =  manoeuvres[position_number+1];																			// read the duration of the manoeuvre
				xdisplacement = manoeuvres[position_number+2];																		//read the displacement in x direction
				zdisplacement = manoeuvres[position_number+4];																		//read the displacement in z direction
				TwoPointsTransfer(chaser1.chemical_Isp, omega, stepsize, duration, InitialConditions, xdisplacement, zdisplacement, OutputVector);		//make the two-points transfer
				for (i=0; i<7; i++)
				{
					InitialConditions[i] = OutputVector[i];																			//update the Begin vector for next manoeuvre
				}
					InitialConditions[6] = OutputVector[6];																			//update the time the next manoeuvre starts
				break;
			case 9:																												//DELTA V IMPULSE
				printf("Commencing Delta V"); printf("\n");
				cout<<"begin position ("<<InitialConditions[0]<<","<<InitialConditions[2]<<")"<<endl;
				DeltaVx = manoeuvres[position_number+5];																			// read the Delta V in x direction
				DeltaVy = manoeuvres[position_number+6];																			// read the Delta V in y direction
				DeltaVz = manoeuvres[position_number+7];																			// read the Delta V in z direction
				duration = manoeuvres[position_number+1];																			//read the duration
				DeltaV(chaser1.chemical_Isp, omega, stepsize, duration, InitialConditions, DeltaVx, DeltaVy, DeltaVz, OutputVector);						//make the Delta V
				for (i=0; i<7; i++)
				{
					InitialConditions[i] = OutputVector[i];																			//update the Begin vector for next manoeuvre
				}
					InitialConditions[6] = OutputVector[6];																			//update the time the next manoeuvre starts
				cout<<"End velocity vector: ("<<InitialConditions[3]<<","<<InitialConditions[5]<<")"<<endl;
				break;
			case 10:																				//STATION KEEPING
				printf("Commencing Station Keeping "); printf("\n");
				cout<<"begin position ("<<InitialConditions[0]<<","<<InitialConditions[2]<<")"<<endl;
				duration = manoeuvres[position_number+1];																			//read the duration
				cout<<"duration: "<<duration<<" seconds"<<endl;
				StationKeeping(chaser1.chemical_Isp, omega, duration, stepsize, InitialConditions, OutputVector);											//make the free drift
				for (i=0; i<7; i++)
				{
					InitialConditions[i] = OutputVector[i];																			//update the Begin vector for next manoeuvre
				}
					InitialConditions[6] = OutputVector[6];																			//update the time the next manoeuvre starts
				break;
			case 11:										//FREE DRIFT
				printf("Commencing Free Drift"); printf("\n");
				cout<<"begin position ("<<InitialConditions[0]<<","<<InitialConditions[2]<<")"<<endl;
				duration = manoeuvres[position_number+1];																			//read the duration
				FreeDrift(chaser1.chemical_Isp, omega, duration, stepsize, InitialConditions, OutputVector);												//make the free drift
				for (i=0; i<7; i++)																									//update the Begin vector for next manoeuvre
				{
					InitialConditions[i] = OutputVector[i];																			
				}
					InitialConditions[6] = OutputVector[6];																			//update the time the next manoeuvre starts
				break;
				
		} // End of the switch
		manoeuvre_number = 	manoeuvre_number+1;
		position_number = position_number + 8;				//tell where to start reading the next manoeuvre
		manoeuvre_type = manoeuvres[position_number];
		cout<<"end position ("<<InitialConditions[0]<<","<<InitialConditions[2]<<")"<<endl<<endl;
		
	} // End of the while
	
   // return 0; // 0 is return with no problems
	
}

