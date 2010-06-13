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

#include<float.h>
#include<math.h>
#include"statevector.h"
#include"calendarTojulian.h"
#include"getGreenwichHourAngle.h"

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Description: 
//		This function converts inertial planet-centered coordinates of position and
//		velocity into plnet-centered, planet-fixed coordinates,
//		using the Greenwich Hour Angle as the rotation angle of the planet
//
// Input:
//		body	identification of the planet/moon
//					Earth=0, Mercury=1, Venus=2, Mars=3, Jupiter=4, Saturn=5, Uranus=6, Nepture=7, Pluto=8, Sun=9
//		GreenwichAngle		Greenwich Hour Angle (rad)
//		x					coordinate X in ECI (km)
//		y					coordinate X in ECI (km)
//		x					coordinate X in ECI (km)
//		xd					speed X in ECI (km)
//		yd					speed X in ECI (km)
//		xd					speed X in ECI (km)
//
// Output:
//		x					coordinate X in ECEF (km)
//		y					coordinate X in ECEF (km)
//		x					coordinate X in ECEF (km)
//		xd					speed X in ECEF (km)
//		yd					speed X in ECEF (km)
//		xd					speed X in ECEF (km)
//	
// Example of call:
//      
//
// Date: October 2006
// Version: 1.0
// Change history:
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////


void inertialTOfixed(int body, double GreenwichAngle, 
					double xInertial, double yInertial, double zInertial, double xdInertial, double ydInertial, double zdInertial,
					double& xFixed, double& yFixed, double& zFixed, double& xdFixed, double& ydFixed, double& zdFixed)
{

	  double rgha, cosgha, singha, term1, term2, omega;

          if(body==0)
              omega = 7.29211585494e-5;  /// Earth rotation rate. Only Earth for the time being
          if(body==3)
              omega = 7.088216383964474e-5;
//      rgha = GreenwichAngle*(3.1415927/180.0);
      rgha = GreenwichAngle;
      cosgha = cos(rgha);
      singha = sin(rgha);
	  
//  Position rotation
      xFixed = xInertial*cosgha + yInertial*singha;
      yFixed = -xInertial*singha + yInertial*cosgha;
      zFixed = zInertial;

//  Velocity rotation
      term1 = xdInertial*cosgha + ydInertial*singha;
      term2 = omega*(-xInertial*singha + yInertial*cosgha);
      xdFixed = term1+term2;
      term1 = -xdInertial*singha + ydInertial*cosgha;
      term2 = omega*(-xInertial*cosgha - yInertial*singha);
      ydFixed = term1+term2;
      zdFixed = zdInertial;

}

sta::StateVector inertialTOfixed(int body, sta::StateVector Vector, double MJDdate) //added by Ana Raposo
{
   /*double year=CurrentTime[0];
   double month=CurrentTime[1];
   double day=CurrentTime[2];
   double hour=CurrentTime[3];
   double minute=CurrentTime[4];
   double second=CurrentTime[5];*/

   //double CurrentJulianDate=calendarTOjulian(year,month,day,hour,minute,second);
   double MJD=MJDdate;
   double GreenwichHourAngle=getGreenwichHourAngle(MJD);

   double rgha, cosgha, singha, term1, term2, omega;

             if(body==0)
                 omega = 7.29211585494e-5;  /// Earth rotation rate. Only Earth for the time being
             if(body==3)
                 omega = 7.088216383964474e-5;
   //      rgha = GreenwichAngle*(3.1415927/180.0);
         rgha = GreenwichHourAngle;
         cosgha = cos(rgha);
         singha = sin(rgha);
         double xInertial=Vector.position.x();
         double yInertial=Vector.position.y();
         double zInertial=Vector.position.z();
         double xdInertial=Vector.velocity.x();
         double ydInertial=Vector.velocity.y();
         double zdInertial=Vector.velocity.z();
         sta::StateVector FixedVector;

   //  Position rotation
         //double xFixed,yFixed,zFixed,xdFixed,ydFixed,zdFixed;
         FixedVector.position.x() = xInertial*cosgha + yInertial*singha;
         FixedVector.position.y()= -xInertial*singha + yInertial*cosgha;
         FixedVector.position.z()= zInertial;

   //  Velocity rotation
         term1 = xdInertial*cosgha + ydInertial*singha;
         term2 = omega*(-xInertial*singha + yInertial*cosgha);
         FixedVector.velocity.x()= term1+term2;
         term1 = -xdInertial*singha + ydInertial*cosgha;
         term2 = omega*(-xInertial*cosgha - yInertial*singha);
         FixedVector.velocity.y()=term1+term2;
         FixedVector.velocity.z()= zdInertial;

         return(FixedVector);
}
