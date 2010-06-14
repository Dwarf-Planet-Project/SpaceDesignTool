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


#define Pi 3.1415926535

#include <iostream>
using namespace std;
#include "math.h"
#include "Manoeuvres.h"
#include "ClohessyWiltshire.h"

//********************************************************Straight line V-bar approach
void StraightLineVBarApproach(double Isp, double omega, double stepsize, double *InitialStateVector, double xdisplacement, double DeltaVx, double *OutputVector)				
{
	//initialisation of parameters
	double duration = sqrt(xdisplacement*xdisplacement)/DeltaVx;
	InitialStateVector[3]=DeltaVx;
	double deltaVbudget, fuel, range, range_rate, ax, ay, az;
	double OldVelocityX = InitialStateVector[3];
	double OldVelocityY = InitialStateVector[4];
	double OldVelocityZ = InitialStateVector[5];
	
	//calculation of continuous force
	long double gamma[3] = {0,0,0};
	gamma[2] = 2*omega*DeltaVx;
	
	//create output pipe files for data printing
	FILE* fp;
	FILE* fp1;
	fp = fopen("/tmp/currentEphemeris.stae", "a");
	fp1 = fopen("/tmp/currentMiscelaneous.stam", "a");
		
	for (double t=0; t<duration+stepsize; t+=stepsize)
	{
		if (t>duration){t=duration;}																		//corrects for the last step which may not be a full timestep

		CWD( stepsize, t, omega, InitialStateVector, OutputVector, gamma);
		OutputVector[6] = InitialStateVector[6] +t;
		
		if(t<duration)
		{
			deltaVbudget = InitialStateVector[7] + sqrt(pow(DeltaVx,2)) + sqrt(pow(gamma[2]*t,2));			//calculates the DV needed and adds it to the already consumed for plotting
		}
		if(t==duration)
		{	
			OutputVector[3]=OutputVector[3]-DeltaVx;														//apply another delta V to stop manoeuvre
			deltaVbudget = InitialStateVector[7] + sqrt(pow(2*DeltaVx,2)) + sqrt(pow(gamma[2]*t,2));		//add this to the DeltaV budget
		}
		
		ax = (OutputVector[3] - OldVelocityX)/stepsize;
		ay = (OutputVector[4] - OldVelocityY)/stepsize;
		az = (OutputVector[5] - OldVelocityZ)/stepsize;
		OldVelocityX = OutputVector[3];		
		OldVelocityY = OutputVector[4];
		OldVelocityZ = OutputVector[5];
		
		range = sqrt(pow(OutputVector[0],2) + pow(OutputVector[1], 2) + pow(OutputVector[2],2));
		fuel = 
		//range_rate = sqrt(pow(OutputVector[3],2) + pow(OutputVector[4], 2) + pow(OutputVector[5],2));
		
		fprintf(fp, "%f       ", OutputVector[6]);
		fprintf(fp, "%f       ", OutputVector[0]); 
		fprintf(fp, "%f       ",  OutputVector[1]); 
		fprintf(fp, "%f       ", OutputVector[2]); 
		fprintf(fp, "%f       ", OutputVector[3]); 
		fprintf(fp, "%f       ", OutputVector[4]); 
		fprintf(fp, "%f\n", OutputVector[5]);
		
		fprintf(fp1, "%f      ", OutputVector[6]);
		fprintf(fp1, "%f      ", ax);												//prints x-acceleration vs time
		fprintf(fp1, "%f      ", ay);												//prints y-acceleration vs time
		fprintf(fp1, "%f      ", az);												//prints z-acceleration vs time
		fprintf(fp1, "%f      ", deltaVbudget);										//prints deltaV vs time
		//fprintf(fp1, "%f     ", fuel);
		fprintf(fp1, "%f\n", range);												//prints range vs time
		//fprintf(fp1, "%f      ", range_rate);											//prints range-rate vs time
		
	};
	
	OutputVector[7] = deltaVbudget;													//updating the delta V budget for next manoeuvre

	//close the output pipe files
	fclose(fp);
	fclose(fp1);
}

//********************************************************Station keeping

void StationKeeping(double Isp, double omega, double duration, double stepsize, double *InitialStateVector, double *OutputVector)
{
	long double gamma[3];
	double deltaVbudget, fuel, range, range_rate, ax, ay, az;
	double OldVelocityX = InitialStateVector[3];
	double OldVelocityY = InitialStateVector[4];
	double OldVelocityZ = InitialStateVector[5];
	
	
	gamma[0] = 0;																						//introduces continuous forces to station-keep
	gamma[1] = omega*omega*InitialStateVector[1];
	gamma[2] = -3*omega*omega*InitialStateVector[2];
	
	//create output pipe files for data printing
	FILE* fp;
	FILE* fp1;
	fp = fopen("/tmp/currentEphemeris.stae", "a");
	fp1 = fopen("/tmp/currentMiscelaneous.stam", "a");	
	
	for (double t=0; t<duration+stepsize; t+=stepsize)
	{
		if (t>duration){t=duration;}																	//corrects for the last step which may not be a full timestep
		
		CWD( stepsize, t, omega, InitialStateVector, OutputVector, gamma);								//propagates with C-W
		OutputVector[6] = t+InitialStateVector[6];														//updates time to total time
		
		deltaVbudget = InitialStateVector[7] +sqrt(pow(gamma[1]*t,2))+sqrt(pow(gamma[2]*t,2));					//calculates the delta V needed for manoeuvre and adds it to the delta V already used
		ax = (OutputVector[3] - OldVelocityX)/stepsize;
		ay = (OutputVector[4] - OldVelocityY)/stepsize;
		az = (OutputVector[5] - OldVelocityZ)/stepsize;
		OldVelocityX = OutputVector[3];		
		OldVelocityY = OutputVector[4];
		OldVelocityZ = OutputVector[5];
		
		range = sqrt(pow(OutputVector[0],2) + pow(OutputVector[1], 2) + pow(OutputVector[2],2));
		//range_rate = sqrt(pow(OutputVector[3],2) + pow(OutputVector[4], 2) + pow(OutputVector[5],2));
		
		fprintf(fp, "%f       ", OutputVector[6]);
		fprintf(fp, "%f       ", OutputVector[0]);								
		fprintf(fp, "%f       ", OutputVector[1]); 
		fprintf(fp, "%f       ", OutputVector[2]); 
		fprintf(fp, "%f       ", OutputVector[3]); 
		fprintf(fp, "%f       ", OutputVector[4]); 
		fprintf(fp, "%f\n", OutputVector[5]);
		
		fprintf(fp1, "%f      ", OutputVector[6]);
		fprintf(fp1, "%f      ", ax);												//prints x-acceleration vs time
		fprintf(fp1, "%f      ", ay);												//prints y-acceleration vs time
		fprintf(fp1, "%f      ", az);												//prints z-acceleration vs time
		fprintf(fp1, "%f      ", deltaVbudget);										//prints deltaV vs time
		//fprintf(fp1, "%f     ", fuel);
		fprintf(fp1, "%f\n", range);												//prints range vs time
		//fprintf(fp1, "%f      ", range_rate);											//prints range-rate vs time
		
	};
	OutputVector[7] = deltaVbudget;
	//cout<<"done with station-keeping";

	//close the output pipe files
	fclose(fp);
	fclose(fp1);
}

//********************************************************Continuous z-thrust transfer
void ContinuousZThrust(double Isp, double omega, double stepsize, double *InitialStateVector, double xdisplacement, double *OutputVector)
{
	//initialisation of parameters
	double T = 2*Pi/omega;
	long double gamma[3]={0,0,0};
	double deltaVbudget, fuel, range, range_rate, ax, ay, az;
	double OldVelocityX = InitialStateVector[3];
	double OldVelocityY = InitialStateVector[4];
	double OldVelocityZ = InitialStateVector[5];	
	
	//calculating continuous thrust
	gamma[2] = omega*omega*xdisplacement/(4*Pi);
	
	//create output pipe files for data printing
	FILE* fp;
	FILE* fp1;
	fp = fopen("/tmp/currentEphemeris.stae", "a");
	fp1 = fopen("/tmp/currentMiscelaneous.stam", "a");
	
	
	for (double t=0; t<T+stepsize; t+=stepsize)
	{
		if (t>T){t=T;}																	//corrects for the last step which may not be a full timestep

		CWD( stepsize, t, omega, InitialStateVector, OutputVector, gamma);
		OutputVector[6] = t+InitialStateVector[6];
		
		deltaVbudget = InitialStateVector[7] + sqrt(pow(gamma[2]*t,2));
		ax = (OutputVector[3] - OldVelocityX)/stepsize;
		ay = (OutputVector[4] - OldVelocityY)/stepsize;
		az = (OutputVector[5] - OldVelocityZ)/stepsize;
		OldVelocityX = OutputVector[3];		
		OldVelocityY = OutputVector[4];
		OldVelocityZ = OutputVector[5];
		
		range = sqrt(pow(OutputVector[0],2) + pow(OutputVector[1], 2) + pow(OutputVector[2],2));
		//range_rate = sqrt(pow(OutputVector[3],2) + pow(OutputVector[4], 2) + pow(OutputVector[5],2));
		
		fprintf(fp, "%f       ", OutputVector[6]);
		fprintf(fp, "%f       ", OutputVector[0]); 
		fprintf(fp, "%f       ",  OutputVector[1]); 
		fprintf(fp, "%f       ", OutputVector[2]); 
		fprintf(fp, "%f       ", OutputVector[3]); 
		fprintf(fp, "%f       ", OutputVector[4]); 
		fprintf(fp, "%f\n", OutputVector[5]);
		
		fprintf(fp1, "%f      ", OutputVector[6]);
		fprintf(fp1, "%f      ", ax);												//prints x-acceleration vs time
		fprintf(fp1, "%f      ", ay);												//prints y-acceleration vs time
		fprintf(fp1, "%f      ", az);												//prints z-acceleration vs time
		fprintf(fp1, "%f      ", deltaVbudget);										//prints deltaV vs time
		//fprintf(fp1, "%f     ", fuel);
		fprintf(fp1, "%f\n", range);												//prints range vs time
		//fprintf(fp1, "%f      ", range_rate);											//prints range-rate vs time
		
	};

	OutputVector[7] = deltaVbudget;

	//close the output pipe files
	fclose(fp);
	fclose(fp1);
}	

//*************************************************************************************************************************************************************************** Impulsive Hopping on V-bar in radial direction
void ImpulsiveRadialHoppingVBar(double Isp, double omega, double stepsize, double *InitialStateVector, double xdisplacement, double *OutputVector)
{
	
	//initialisation of parameters
	double T = 2*Pi/omega;
	long double gamma[3] = {0,0,0};
	double deltaVbudget, fuel, range, range_rate, ax, ay, az;
	double OldVelocityX = InitialStateVector[3];
	double OldVelocityY = InitialStateVector[4];
	double OldVelocityZ = InitialStateVector[5];	
	
	//calculation of Delta V needed in z- or y-direction
	double DeltaV = (omega/4)*xdisplacement;
	/*switch(RadialHoppingType){
		case '1':
			InitialStateVector[4] = DeltaV;				//in this case y-impulse
			break;
		default:*/
			InitialStateVector[5] = DeltaV;				//otherwise we just assume it's in-plane
	//}
	
	//create output pipe files for data printing
	FILE* fp;
	FILE* fp1;
	fp = fopen("/tmp/currentEphemeris.stae", "a");
	fp1 = fopen("/tmp/currentMiscelaneous.stam", "a");	
	
	// call Clohessy-Wiltshire equations
	for (double t=0; t<T/2+stepsize; t+=stepsize)																							
	{
		if (t>T/2){t=T/2;}													//corrects for the last step which may not be a full timestep

		CWD(stepsize, t, omega, InitialStateVector, OutputVector, gamma);
		OutputVector[6] = t+InitialStateVector[6];
				
		if (t<T/2)
		{
			deltaVbudget = InitialStateVector[7] + sqrt(pow(DeltaV,2));
		}
		if(t==T/2)
		{	
			OutputVector[5]= OutputVector[5] + DeltaV;																				//apply another delta V to stop manoeuvre
			deltaVbudget = InitialStateVector[7] + sqrt(pow(2*DeltaV,2));
		}
		
		ax = (OutputVector[3] - OldVelocityX)/stepsize;
		ay = (OutputVector[4] - OldVelocityY)/stepsize;
		az = (OutputVector[5] - OldVelocityZ)/stepsize;
		OldVelocityX = OutputVector[3];		
		OldVelocityY = OutputVector[4];
		OldVelocityZ = OutputVector[5];
		
		range = sqrt(pow(OutputVector[0],2) + pow(OutputVector[1], 2) + pow(OutputVector[2],2));
		//range_rate = sqrt(pow(OutputVector[3],2) + pow(OutputVector[4], 2) + pow(OutputVector[5],2));
		
		fprintf(fp, "%f       ", OutputVector[6]);
		fprintf(fp, "%f       ", OutputVector[0]); 
		fprintf(fp, "%f       ",  OutputVector[1]); 
		fprintf(fp, "%f       ", OutputVector[2]); 
		fprintf(fp, "%f       ", OutputVector[3]); 
		fprintf(fp, "%f       ", OutputVector[4]); 
		fprintf(fp, "%f\n", OutputVector[5]);
		
		fprintf(fp1, "%f      ", OutputVector[6]);
		fprintf(fp1, "%f      ", ax);												//prints x-acceleration vs time
		fprintf(fp1, "%f      ", ay);												//prints y-acceleration vs time
		fprintf(fp1, "%f      ", az);												//prints z-acceleration vs time
		fprintf(fp1, "%f      ", deltaVbudget);										//prints deltaV vs time
		//fprintf(fp1, "%f     ", fuel);
		fprintf(fp1, "%f\n", range);												//prints range vs time
     	//fprintf(fp1, "%f      ", range_rate);											//prints range-rate vs time
		
	};
	
	OutputVector[7] = deltaVbudget;
	
	//close the output pipe files
	fclose(fp);
	fclose(fp1);
}

//**************************************************************************************************************************************************************** Impulsive Hopping on V-bar in tangential direction
void ImpulsiveTangentialHoppingVBar(double Isp, double omega, double stepsize, double *InitialStateVector, double xdisplacement, double *OutputVector)
{
	//initialisation of parameters
	double T = 2*Pi/omega;
	long double gamma[3] = {0,0,0};
	double deltaVbudget, fuel, range, range_rate, ax, ay, az;
	double OldVelocityX = InitialStateVector[3];
	double OldVelocityY = InitialStateVector[4];
	double OldVelocityZ = InitialStateVector[5];
	
	//calculation of Delta V needed in x-direction
	double DeltaV = -1*(omega/(6*Pi))*xdisplacement;																		
	InitialStateVector[3] = DeltaV;
	
	//create output pipe files for data printing
	FILE* fp;
	FILE* fp1;
	fp = fopen("/tmp/currentEphemeris.stae", "a");
	fp1 = fopen("/tmp/currentMiscelaneous.stam", "a");	
	
	for (double t=0; t<T+stepsize; t+=stepsize)
	{
		if (t>T){t=T;}																													//corrects for the last step which may not be a full timestep
		
		CWD(stepsize, t, omega, InitialStateVector, OutputVector, gamma);
		OutputVector[6] = t+InitialStateVector[6];
		
		if(t<T)
		{
			deltaVbudget = InitialStateVector[7] + sqrt(pow(DeltaV,2));
		}
		if(t==T)
		{	
			OutputVector[3] = OutputVector[3] - DeltaV;									//apply another delta V to stop manoeuvre
			deltaVbudget = InitialStateVector[7] + sqrt(pow(2*DeltaV,2));
		}
		
		ax = (OutputVector[3] - OldVelocityX)/stepsize;
		ay = (OutputVector[4] - OldVelocityY)/stepsize;
		az = (OutputVector[5] - OldVelocityZ)/stepsize;
		OldVelocityX = OutputVector[3];		
		OldVelocityY = OutputVector[4];
		OldVelocityZ = OutputVector[5];
		
		range = sqrt(pow(OutputVector[0],2) + pow(OutputVector[1], 2) + pow(OutputVector[2],2));
		//range_rate = sqrt(pow(OutputVector[3],2) + pow(OutputVector[4], 2) + pow(OutputVector[5],2));
		
		fprintf(fp, "%f       ", OutputVector[6]);
		fprintf(fp, "%f       ", OutputVector[0]); 
		fprintf(fp, "%f       ",  OutputVector[1]); 
		fprintf(fp, "%f       ", OutputVector[2]); 
		fprintf(fp, "%f       ", OutputVector[3]); 
		fprintf(fp, "%f       ", OutputVector[4]); 
		fprintf(fp, "%f\n", OutputVector[5]);
		
		fprintf(fp1, "%f      ", OutputVector[6]);
		fprintf(fp1, "%f      ", ax);												//prints x-acceleration vs time
		fprintf(fp1, "%f      ", ay);												//prints y-acceleration vs time
		fprintf(fp1, "%f      ", az);												//prints z-acceleration vs time
		fprintf(fp1, "%f      ", deltaVbudget);										//prints deltaV vs time
		//fprintf(fp1, "%f     ", fuel);
		fprintf(fp1, "%f\n", range);												//prints range vs time
		//fprintf(fp1, "%f      ", range_rate);											//prints range-rate vs time
		
	};
	
	OutputVector[7] = deltaVbudget;
	
	//close the output pipe files
	fclose(fp);
	fclose(fp1);
	
}
//********************************************************************************************************************************************************************************************** Hohmann transfer

void HohmannTransfer(double Isp, double omega, double stepsize, double *InitialStateVector, double zdisplacement, double *OutputVector)
{
	//initialisation of parameters
	double T = 2*Pi/omega;
	long double gamma[3] = {0,0,0};
	double deltaVbudget, fuel, range, range_rate, ax, ay, az;
	double OldVelocityX = InitialStateVector[3];
	double OldVelocityY = InitialStateVector[4];
	double OldVelocityZ = InitialStateVector[5];
	
	double DeltaV = -1*(omega/4)*(zdisplacement);									//calculation of Delta Vx needed for displacement in z-direction
	double DeltaV0=3*omega*InitialStateVector[2]/2;									//initial speed due to different orbitl height
	InitialStateVector[3] = DeltaV+DeltaV0;

	//create output pipe files for data printing
	FILE* fp;
	FILE* fp1;
	fp = fopen("/tmp/currentEphemeris.stae", "a");
	fp1 = fopen("/tmp/currentMiscelaneous.stam", "a");
	
	for (double t=0; t<T/2+stepsize; t+=stepsize)
	{
		if (t>T/2){t=T/2;}													//corrects for the last step which may not be a full timestep

		CWD(stepsize, t, omega, InitialStateVector, OutputVector, gamma);
		OutputVector[6] = t+InitialStateVector[6];
				
		if(t<T/2)
		{
			deltaVbudget = InitialStateVector[7] + sqrt(pow(DeltaV,2));
		}
		if(t==T/2)
		{	
			OutputVector[3] = OutputVector[3] +DeltaV;																				//apply another delta V to stop manoeuvre
			deltaVbudget = InitialStateVector[7] + sqrt(pow(2*DeltaV,2));
		}
		
		ax = (OutputVector[3] - OldVelocityX)/stepsize;
		ay = (OutputVector[4] - OldVelocityY)/stepsize;
		az = (OutputVector[5] - OldVelocityZ)/stepsize;
		OldVelocityX = OutputVector[3];		
		OldVelocityY = OutputVector[4];
		OldVelocityZ = OutputVector[5];
		
		range = sqrt(pow(OutputVector[0],2) + pow(OutputVector[1], 2) + pow(OutputVector[2],2));
		//range_rate = sqrt(pow(OutputVector[3],2) + pow(OutputVector[4], 2) + pow(OutputVector[5],2));
		
		fprintf(fp, "%f       ", OutputVector[6]);
		fprintf(fp, "%f       ", OutputVector[0]); 
		fprintf(fp, "%f       ",  OutputVector[1]); 
		fprintf(fp, "%f       ", OutputVector[2]); 
		fprintf(fp, "%f       ", OutputVector[3]); 
		fprintf(fp, "%f       ", OutputVector[4]); 
		fprintf(fp, "%f\n", OutputVector[5]); 
		
		fprintf(fp1, "%f      ", OutputVector[6]);
		fprintf(fp1, "%f      ", ax);												//prints x-acceleration vs time
		fprintf(fp1, "%f      ", ay);												//prints y-acceleration vs time
		fprintf(fp1, "%f      ", az);												//prints z-acceleration vs time
		fprintf(fp1, "%f      ", deltaVbudget);										//prints deltaV vs time
		//fprintf(fp1, "%f     ", fuel);
		fprintf(fp1, "%f\n", range);												//prints range vs time
		//fprintf(fp1, "%f      ", range_rate);											//prints range-rate vs time
		
	};

	OutputVector[7] = deltaVbudget;

	//close the output pipe files
	fclose(fp);
	fclose(fp1);
}

//******************************************************************************************************************************************************************************************************* Delta V
void DeltaV(double Isp, double omega, double stepsize, double duration, double *InitialStateVector, double DeltaVx, double DeltaVy, double DeltaVz, double *OutputVector)
{
	//initialisation of parameters
	long double gamma[3] = {0,0,0};
	double deltaVbudget, fuel, range, range_rate, ax, ay, az;
	double OldVelocityX = InitialStateVector[3];
	double OldVelocityY = InitialStateVector[4];
	double OldVelocityZ = InitialStateVector[5];
	
	double DeltaVx0=3*omega*InitialStateVector[2]/2;
	double DeltaVy0=3*omega*InitialStateVector[1]/2;
	InitialStateVector[3] = DeltaVx+DeltaVx0;
	InitialStateVector[4] = DeltaVy+DeltaVy0;
	InitialStateVector[5] = DeltaVz;
	
	//create output pipe files for data printing
	FILE* fp;
	FILE* fp1;
	fp = fopen("/tmp/currentEphemeris.stae", "a");
	fp1 = fopen("/tmp/currentMiscelaneous.stam", "a");	
	
	for (double t=0; t<duration+stepsize; t+=stepsize)
	{
		if (t>duration){t=duration;}													//corrects for the last step which may not be a full timestep

		CWD(stepsize, t, omega, InitialStateVector, OutputVector, gamma);
		OutputVector[6] = t+InitialStateVector[6];
		
		ax = (OutputVector[3] - OldVelocityX)/stepsize;
		ay = (OutputVector[4] - OldVelocityY)/stepsize;
		az = (OutputVector[5] - OldVelocityZ)/stepsize;
		OldVelocityX = OutputVector[3];		
		OldVelocityY = OutputVector[4];
		OldVelocityZ = OutputVector[5];
		
		range = sqrt(pow(OutputVector[0],2) + pow(OutputVector[1], 2) + pow(OutputVector[2],2));
		//range_rate = sqrt(pow(OutputVector[3],2) + pow(OutputVector[4], 2) + pow(OutputVector[5],2));
		deltaVbudget = InitialStateVector[7] + sqrt(pow(DeltaVx,2)) + sqrt(pow(DeltaVy,2)) + sqrt(pow(DeltaVz,2));
		
		fprintf(fp, "%f       ", OutputVector[6]);
		fprintf(fp, "%f       ", OutputVector[0]); 
		fprintf(fp, "%f       ",  OutputVector[1]); 
		fprintf(fp, "%f       ", OutputVector[2]); 
		fprintf(fp, "%f       ", OutputVector[3]); 
		fprintf(fp, "%f       ", OutputVector[4]); 
		fprintf(fp, "%f\n", OutputVector[5]);
		
		fprintf(fp1, "%f      ", OutputVector[6]);
		fprintf(fp1, "%f      ", ax);												//prints x-acceleration vs time
		fprintf(fp1, "%f      ", ay);												//prints y-acceleration vs time
		fprintf(fp1, "%f      ", az);												//prints z-acceleration vs time
		fprintf(fp1, "%f      ", deltaVbudget);										//prints deltaV vs time
		//fprintf(fp1, "%f     ", fuel);
		fprintf(fp1, "%f\n", range);												//prints range vs time
	    //fprintf(fp1, "%f      ", range_rate);											//prints range-rate vs time
		
		
	};

	OutputVector[7] = deltaVbudget;

	//close the output pipe files
	fclose(fp);
	fclose(fp1);
}

//****************************************************************************************************************************************** Free Drift
void FreeDrift(double Isp, double omega, double duration, double stepsize, double *InitialStateVector, double *OutputVector)
{
	double Z0 = InitialStateVector[2];
	double initVx0 = 3*omega*Z0/2;
	InitialStateVector[3] = initVx0;
	long double gamma[3] = {0,0,0};
	double deltaVbudget, fuel, range, range_rate, ax, ay, az;
	double OldVelocityX = InitialStateVector[3];
	double OldVelocityY = InitialStateVector[4];
	double OldVelocityZ = InitialStateVector[5];
	
	//create output pipe files for data printing
	FILE* fp;
	FILE* fp1;
	fp = fopen("/tmp/currentEphemeris.stae", "a");
	fp1 = fopen("/tmp/currentMiscelaneous.stam", "a");	
	
	for (double t=0; t<duration+stepsize; t+=stepsize)
	{
		if (t>duration){t=duration;}													//corrects for the last step which may not be a full timestep

		CWD(stepsize, t, omega, InitialStateVector, OutputVector, gamma);
		OutputVector[6] = t+InitialStateVector[6];
		deltaVbudget = InitialStateVector[7];
		
		ax = (OutputVector[3] - OldVelocityX)/stepsize;
		ay = (OutputVector[4] - OldVelocityY)/stepsize;
		az = (OutputVector[5] - OldVelocityZ)/stepsize;
		OldVelocityX = OutputVector[3];		
		OldVelocityY = OutputVector[4];
		OldVelocityZ = OutputVector[5];
		
		range = sqrt(pow(OutputVector[0],2) + pow(OutputVector[1], 2) + pow(OutputVector[2],2));
		//range_rate = sqrt(pow(OutputVector[3],2) + pow(OutputVector[4], 2) + pow(OutputVector[5],2));
		
		fprintf(fp, "%f       ", OutputVector[6]);
		fprintf(fp, "%f       ", OutputVector[0]); 
		fprintf(fp, "%f       ", OutputVector[1]); 
		fprintf(fp, "%f       ", OutputVector[2]); 
		fprintf(fp, "%f       ", OutputVector[3]); 
		fprintf(fp, "%f       ", OutputVector[4]); 
		fprintf(fp, "%f\n", OutputVector[5]);
		
		fprintf(fp1, "%f      ", OutputVector[6]);
		fprintf(fp1, "%f      ", ax);												//prints x-acceleration vs time
		fprintf(fp1, "%f      ", ay);												//prints y-acceleration vs time
		fprintf(fp1, "%f      ", az);												//prints z-acceleration vs time
		fprintf(fp1, "%f      ", deltaVbudget);										//prints deltaV vs time
		//fprintf(fp1, "%f     ", fuel);
		fprintf(fp1, "%f \n", range);												//prints range vs time
		//fprintf(fp1, "%f      ", range_rate);											//prints range-rate vs time
	};
	
	OutputVector[7] = deltaVbudget;
	
	//close the output pipe files
	fclose(fp);
	fclose(fp1);
}

//********************************************************Straight line R-bar approach	

void StraightLineRBarApproach(double Isp, double omega, double stepsize, double *InitialStateVector, double zdisplacement, double ApproachSpeed, double *OutputVector)				//DeltaVz is positive downwards (drifting from target)
{
	//initialisation of parameters
	double duration = sqrt(zdisplacement*zdisplacement)/ApproachSpeed;							//determines the duration of the manoeuvre with the given approach speed
	double DeltaVz;
	double Z0 = InitialStateVector[2];
	double initVx0 = 3*omega*Z0/2;
	InitialStateVector[3] = initVx0;															//initial x-velocity due to different orbital height
	double deltaVbudget, fuel, range, range_rate, ax, ay, az;
	double OldVelocityX = InitialStateVector[3];
	double OldVelocityY = InitialStateVector[4];
	double OldVelocityZ = InitialStateVector[5];
	
	if (zdisplacement>0)
	{DeltaVz = ApproachSpeed;}
	else
	{DeltaVz = -ApproachSpeed;}
	
	//create output pipe files for data printing
	FILE* fp;
	FILE* fp1;
	fp = fopen("/tmp/currentEphemeris.stae", "a");
	fp1 = fopen("/tmp/currentMiscelaneous.stam", "a");
	
	//calculation of continuous force
	InitialStateVector[5] = DeltaVz;
	long double gamma[3] = {0,0,0};
	gamma[0] = -2*omega*DeltaVz;
	
	for (double t=0; t<duration+stepsize; t+=stepsize)
	{
		if (t>duration){t=duration;}													//corrects for the last step which may not be a full timestep

		gamma[2] = -3*omega*omega*(DeltaVz*t+InitialStateVector[2]);
		
		double x1 = InitialStateVector[0];			//rewritten so CW equations are legible
		double y1 = InitialStateVector[1];
		double z1 = InitialStateVector[2];
		double vx1 = InitialStateVector[3];
		double vy1 = InitialStateVector[4];
		double vz1 = InitialStateVector[5];
		
		OutputVector[0] = (gamma[0]+4*omega*omega*x1+2*vz1*omega)/(4*omega*omega) - (cos(2*t*omega)*(gamma[0]+2*vz1*omega))/(4*omega*omega)+(vx1*sin(2*omega*t))/(2*omega);
		OutputVector[1] = cos(omega*t)*y1+(vy1*sin(omega*t))/omega;
		OutputVector[2] = (sin(2*omega*t)*(gamma[0]+2*vz1*omega))/(4*omega*omega) - t*gamma[0]/(2*omega) + (2*omega*z1 - vx1)/(2*omega) + (vx1*cos(2*t*omega))/(2*omega);
		OutputVector[3] = cos(2*omega*t)*(gamma[0]+2*omega*vz1)/(2*omega)+vx1*cos(omega*t);
		OutputVector[4] = -1*y1*omega*sin(omega*t)+vy1*cos(omega*t);
		OutputVector[5] = cos(2*omega*t)*(gamma[0]+vz1*omega)/(2*omega) - gamma[0]/(2*omega) - vx1*sin(2*omega*t);
		OutputVector[6] = t+InitialStateVector[6];
		
				
		if(t<duration)
		{
			deltaVbudget = InitialStateVector[7] + sqrt(pow(DeltaVz,2)) + sqrt(pow(gamma[0]*t,2)) + sqrt(pow(gamma[2]*t,2));
		}
		if(t==duration)
		{	
			OutputVector[3]=OutputVector[3] - DeltaVz;																				//apply another delta V to stop manoeuvre
			deltaVbudget = InitialStateVector[7] + sqrt(pow(2*DeltaVz,2)) + sqrt(pow(gamma[0]*t,2)) + sqrt(pow(gamma[2]*t,2));
		}
		
		ax = (OutputVector[3] - OldVelocityX)/stepsize;
		ay = (OutputVector[4] - OldVelocityY)/stepsize;
		az = (OutputVector[5] - OldVelocityZ)/stepsize;
		OldVelocityX = OutputVector[3];		
		OldVelocityY = OutputVector[4];
		OldVelocityZ = OutputVector[5];
		
		range = sqrt(pow(OutputVector[0],2) + pow(OutputVector[1], 2) + pow(OutputVector[2],2));
		//range_rate = sqrt(pow(OutputVector[3],2) + pow(OutputVector[4], 2) + pow(OutputVector[5],2));
		
		fprintf(fp, " %f      ", OutputVector[6]);
		fprintf(fp, "%f       ", OutputVector[0]);
		fprintf(fp, "%f       ", OutputVector[1]); 
		fprintf(fp, "%f       ", OutputVector[2]); 
		fprintf(fp, "%f       ", OutputVector[3]); 
		fprintf(fp, " %f      ", OutputVector[4]); 
		fprintf(fp, " %f\n", OutputVector[5]);
		
		fprintf(fp1, "%f      ", OutputVector[6]);
		fprintf(fp1, "%f      ", ax);												//prints x-acceleration vs time
		fprintf(fp1, "%f      ", ay);												//prints y-acceleration vs time
		fprintf(fp1, "%f      ", az);												//prints z-acceleration vs time
		fprintf(fp1, "%f      ", deltaVbudget);										//prints deltaV vs time
		//fprintf(fp1, "%f     ", fuel);
		fprintf(fp1, "%f    \n", range);												//prints range vs time
		//fprintf(fp1, "%f      ", range_rate);											//prints range-rate vs time
		
	};

	OutputVector[7] = deltaVbudget;
	
	//close the output pipe files
	fclose(fp);
	fclose(fp1);
}

//******************************************************** Continuous x-thrust transfer

void ContinuousXThrust(double Isp, double omega, double stepsize, double *InitialStateVector, double zdisplacement, double *OutputVector)
{
	double T = 2*Pi/omega;
	long double gamma[3]={0,0,0};
	double deltaVbudget, fuel, range, range_rate, ax, ay, az;
	double OldVelocityX = InitialStateVector[3];
	double OldVelocityY = InitialStateVector[4];
	double OldVelocityZ = InitialStateVector[5];
	
	gamma[0] = -0.25*omega*omega*zdisplacement/Pi;	
	double Z0 = InitialStateVector[2];
	double initVx0 = 3*omega*Z0/2;
	InitialStateVector[3] = initVx0;				
	
	//create output pipe files for data printing
	FILE* fp;
	FILE* fp1;
	fp = fopen("/tmp/currentEphemeris.stae", "a");
	fp1 = fopen("/tmp/currentMiscelaneous.stam", "a");
	
	for (double t=0; t<T+stepsize; t+=stepsize)
	{
		if (t>T){t=T;}													//corrects for the last step which may not be a full timestep

		CWD( stepsize, t, omega, InitialStateVector, OutputVector, gamma);
		OutputVector[6] = t + InitialStateVector[6];
		
		ax = (OutputVector[3] - OldVelocityX)/stepsize;
		ay = (OutputVector[4] - OldVelocityY)/stepsize;
		az = (OutputVector[5] - OldVelocityZ)/stepsize;
		OldVelocityX = OutputVector[3];		
		OldVelocityY = OutputVector[4];
		OldVelocityZ = OutputVector[5];
		
		range = sqrt(pow(OutputVector[0],2) + pow(OutputVector[1], 2) + pow(OutputVector[2],2));
		//range_rate = sqrt(pow(OutputVector[3],2) + pow(OutputVector[4], 2) + pow(OutputVector[5],2));
		deltaVbudget = InitialStateVector[7] + sqrt(pow(gamma[0]*t,2));		
		
		fprintf(fp, "%f       ", OutputVector[6]);
		fprintf(fp, "%f       ", OutputVector[0]); 
		fprintf(fp, "%f       ",  OutputVector[1]); 
		fprintf(fp, "%f       ", OutputVector[2]); 
		fprintf(fp, "%f       ", OutputVector[3]); 
		fprintf(fp, "%f       ", OutputVector[4]); 
		fprintf(fp, "%f\n", OutputVector[5]);
		
		fprintf(fp1, "%f      ", OutputVector[6]);
		fprintf(fp1, "%f      ", ax);												//prints x-acceleration vs time
		fprintf(fp1, "%f      ", ay);												//prints y-acceleration vs time
		fprintf(fp1, "%f      ", az);												//prints z-acceleration vs time
		fprintf(fp1, "%f      ", deltaVbudget);										//prints deltaV vs time
		//fprintf(fp1, "%f     ", fuel);
		fprintf(fp1, "%f\n", range);												//prints range vs time
		//fprintf(fp1, "%f      ", range_rate);											//prints range-rate vs time
		
	};
	
	OutputVector[7] = deltaVbudget;
	
	//close the output pipe files
	fclose(fp);
	fclose(fp1);
}

//************************************************************************************************************************************************************************ Impulsive Hopping not on V-bar 
void Rephasing(double Isp, double omega, double stepsize, double *InitialStateVector, double xdisplacement, double *OutputVector)
{
	
	//initialisation of parameters
	double T = 2*Pi/omega;
	long double gamma[3] = {0,0,0};
	InitialStateVector[3] = 3*omega*((InitialStateVector[2])/2);
	double deltaVbudget, fuel, range, range_rate, ax, ay, az;
	double OldVelocityX = InitialStateVector[3];
	double OldVelocityY = InitialStateVector[4];
	double OldVelocityZ = InitialStateVector[5];
	
	//calculation of Delta V needed in z-direction
	double DeltaV = (omega/4)*xdisplacement-3*omega*omega*InitialStateVector[2]*(T/16);
	InitialStateVector[5] = DeltaV;	
	
	//create output pipe files for data printing
	FILE* fp;
	FILE* fp1;
	fp = fopen("/tmp/currentEphemeris.stae", "a");
	fp1 = fopen("/tmp/currentMiscelaneous.stam", "a");
	
	// call Clohessy-Wiltshire equations
	for (double t=0; t<T/2+stepsize; t+=stepsize)																							
	{
		if (t>T/2){t=T/2;}													//corrects for the last step which may not be a full timestep

		CWD(stepsize, t, omega, InitialStateVector, OutputVector, gamma);
		OutputVector[6] = t+InitialStateVector[6];
				
		if(t<T/2)
		{
			deltaVbudget = InitialStateVector[7] + sqrt(pow(DeltaV,2));
		}
		if(t==T/2)
		{	
			OutputVector[5]=OutputVector[5] + DeltaV;																				//apply another delta V to stop manoeuvre
			deltaVbudget = InitialStateVector[7] + sqrt(pow(2*DeltaV,2));
		}
		
		ax = (OutputVector[3] - OldVelocityX)/stepsize;
		ay = (OutputVector[4] - OldVelocityY)/stepsize;
		az = (OutputVector[5] - OldVelocityZ)/stepsize;
		OldVelocityX = OutputVector[3];		
		OldVelocityY = OutputVector[4];
		OldVelocityZ = OutputVector[5];
		
		range = sqrt(pow(OutputVector[0],2) + pow(OutputVector[1], 2) + pow(OutputVector[2],2));
		//range_rate = sqrt(pow(OutputVector[3],2) + pow(OutputVector[4], 2) + pow(OutputVector[5],2));
		
		fprintf(fp, "%f       ", OutputVector[6]);
		fprintf(fp, "%f       ", OutputVector[0]); 
		fprintf(fp, "%f       ",  OutputVector[1]); 
		fprintf(fp, "%f       ", OutputVector[2]); 
		fprintf(fp, "%f       ", OutputVector[3]); 
		fprintf(fp, "%f       ", OutputVector[4]); 
		fprintf(fp, "%f\n", OutputVector[5]);
		
		fprintf(fp1, "%f      ", OutputVector[6]);
		fprintf(fp1, "%f      ", ax);												//prints x-acceleration vs time
		fprintf(fp1, "%f      ", ay);												//prints y-acceleration vs time
		fprintf(fp1, "%f      ", az);												//prints z-acceleration vs time
		fprintf(fp1, "%f      ", deltaVbudget);										//prints deltaV vs time
		//fprintf(fp1, "%f     ", fuel);
		fprintf(fp1, "%f\n", range);												//prints range vs time
		//fprintf(fp1, "%f      ", range_rate);											//prints range-rate vs time
		
	};
	
	OutputVector[7] = deltaVbudget;	
	
	//close the output pipe files
	fclose(fp);
	fclose(fp1);
}

//********************************************************Two points transfer

void TwoPointsTransfer(double Isp, double omega, double stepsize, double duration, double *InitialStateVector, double xdisplacement, double zdisplacement, double *OutputVector)
{
	long double gamma[3]={0,0,0};
	double deltaVbudget, fuel, range, range_rate, ax, ay, az;
	double OldVelocityX = InitialStateVector[3];
	double OldVelocityY = InitialStateVector[4];
	double OldVelocityZ = InitialStateVector[5];
	
	//determining the initial velocity
	double Z0 = InitialStateVector[2];
	double initVx0 = 3*omega*Z0/2;
	double xdot0 = (-1*omega*sin(omega*duration)*xdisplacement + omega*(6*omega*duration*sin(omega*duration) - 14*(1-cos(omega*duration)))*Z0+2*omega*(1-cos(omega*duration))*(Z0+zdisplacement))/(3*omega*duration*sin(omega*duration)-8*(1-cos(omega*duration)));
	InitialStateVector[3] = xdot0;				cout<<InitialStateVector[3];
	double initVz0 = InitialStateVector[5];
	double zdot0 = omega*(-2*xdisplacement*(1-cos(omega*duration))+(4*sin(omega*duration)-3*omega*duration*cos(omega*duration))*Z0 + (3*omega*duration-4*sin(omega*duration))*(Z0+zdisplacement))/(3*omega*duration*sin(omega*duration)-8*(1-cos(omega*duration)));
	InitialStateVector[5] = zdot0;
	//determining the initial Delta Vs
	double DeltaVx0 = xdot0 - initVx0;
	double DeltaVz0 = zdot0 - initVz0;
	
	//create output pipe files for data printing
	FILE* fp;
	FILE* fp1;
	fp = fopen("/tmp/currentEphemeris.stae", "a");
	fp1 = fopen("/tmp/currentMiscelaneous.stam", "a");
	
	for (double t=0; t<duration+stepsize; t+=stepsize)
	{
		if (t>duration){t=duration;}													//corrects for the last step which may not be a full timestep
		
		CWD( stepsize, t, omega, InitialStateVector, OutputVector, gamma);
		OutputVector[6] = t + InitialStateVector[6];
		
		if(t<duration)
		{
			deltaVbudget = InitialStateVector[7] + sqrt(pow(DeltaVx0, 2)) + sqrt(pow(DeltaVz0,2));
		}
		if(t==duration)
		{
			double StopDeltaVx = 3*omega*OutputVector[3]/2 - OutputVector[3];
			double StopDeltaVz = -OutputVector[5];
			deltaVbudget = InitialStateVector[7] + sqrt(pow(DeltaVx0,2)) + sqrt(pow(DeltaVz0,2)) + sqrt(pow(StopDeltaVx,2)) + sqrt(pow(StopDeltaVz,2)); 
			OutputVector[3] = OutputVector[3] + StopDeltaVx;															
			OutputVector[5] = OutputVector[5] + StopDeltaVz;
			
		}
		
		ax = (OutputVector[3] - OldVelocityX)/stepsize;
		ay = (OutputVector[4] - OldVelocityY)/stepsize;
		az = (OutputVector[5] - OldVelocityZ)/stepsize;
		OldVelocityX = OutputVector[3];		
		OldVelocityY = OutputVector[4];
		OldVelocityZ = OutputVector[5];
		
		range = sqrt(pow(OutputVector[0],2) + pow(OutputVector[1], 2) + pow(OutputVector[2],2));
		//range_rate = sqrt(pow(OutputVector[3],2) + pow(OutputVector[4], 2) + pow(OutputVector[5],2));
		
		fprintf(fp, " %f      ", OutputVector[6]);
		fprintf(fp, "%f       ", OutputVector[0]); 
		fprintf(fp, "%f       ", OutputVector[1]); 
		fprintf(fp, "%f       ", OutputVector[2]); 
		fprintf(fp, "%f       ", OutputVector[3]); 
		fprintf(fp, " %f      ", OutputVector[4]); 
		fprintf(fp, " %f\n", OutputVector[5]);
		
		fprintf(fp1, "%f      ", OutputVector[6]);
		fprintf(fp1, "%f      ", ax);												//prints x-acceleration vs time
		fprintf(fp1, "%f      ", ay);												//prints y-acceleration vs time
		fprintf(fp1, "%f      ", az);												//prints z-acceleration vs time
		fprintf(fp1, "%f      ", deltaVbudget);										//prints deltaV vs time
		//fprintf(fp1, "%f     ", fuel);
		fprintf(fp1, "%f\n", range);												//prints range vs time
		//fprintf(fp1, "%f      ", range_rate);											//prints range-rate vs time
		
	};
	
	OutputVector[7] = deltaVbudget;	
	
	//close the output pipe files
	fclose(fp);
	fclose(fp1);
}
