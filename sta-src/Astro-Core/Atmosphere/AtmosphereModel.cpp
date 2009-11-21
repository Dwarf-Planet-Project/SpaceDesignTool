/*
 Copyright (C) 2007 European Space Agency (space.trajectory.analysis AT gmail.com) -

 This program is free software; you can redistribute it and/or modify it under
 the terms of the GNU Lesser General Public License as published by the Free Software
 Foundation; either version 2 of the License, or (at your option) any later
 version.

 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more
 details.

 You should have received a copy of the GNU Lesser General Public License along with
 this program; if not, write to the Free Software Foundation, Inc., 59 Temple
 Place - Suite 330, Boston, MA  02111-1307, USA.

 Further information about the GNU Lesser General Public License can also be found on
 the world wide web at http://www.gnu.org.
*/
/*
------------------ Author:       Marnix Volckaert               -------------------
------------------ Affiliation:  TUDelft                        -------------------
-----------------------------------------------------------------------------------
 Modified by Tiziana Sabatini on May-July 2009
*/


#include <iostream>
#include <fstream>
#include <QString>
#include <math.h>
#include <QFile>
#include <QTextStream>
#include <QDir>
#include <QProcess>

#include "AtmosphereModel.h"
#include <QDebug>

using namespace std;

//------------------------------------------------------------------------------------------------------------
void AtmosphereModel::selectModel (QString name)
{
    Ndatapoints = 0;

    QString path = QString("data/atmospheres/") + name;
    QFile model(path);
    model.open(QIODevice::ReadOnly);
	
    QTextStream modelstream(&model);//------
	
    while (!modelstream.atEnd())
    {
        modelstream.readLine();      //read in a line just to count it
        Ndatapoints++;                      //increase the counter
    }
    //Ndatapoints--;                  //The loop ends after 1 extra count, we need to subtract by 1
	
    //------ Allocate the memory for the data arrays ------
    altitude_array = new double[Ndatapoints];
    density_array = new double[Ndatapoints];
    pressure_array = new double[Ndatapoints];
    speedofsound_array = new double[Ndatapoints];
    temperature_array = new double[Ndatapoints];
    //------ Close and open the file again, workaround to start reading at the beginning of the file (moving the pointer did not work) ------
    model.close();
	
    model.open(QIODevice::ReadOnly);
	
    //------ Fill up the data arrays, set in correct SI units ------
    for (int i=0; i<Ndatapoints; i++) {
        modelstream >> altitude_array[i]; altitude_array[i] *= 1000.0;
        modelstream >> density_array[i];
        modelstream >> pressure_array[i];
        modelstream >> speedofsound_array[i];
        modelstream >> temperature_array[i];
		
    }
    //------ Close the data file ------
	
    model.close();
	
}

double AtmosphereModel::temperature(double altitude) {
	
    double temp_int;            //Interpolated temperature
    //------ if altitude is below lowest data point -----------------------------
    if (altitude_array[0] > altitude) {
        return(temperature_array[0]);
    }
    //------ if altitude is above highest data point ------
    else if (altitude > altitude_array[Ndatapoints-1]) {
        return(temperature_array[Ndatapoints-1]);
    }
    else {
        //------ Find the data point higher and lower than the altitude value ------
        for (int i=1; i<Ndatapoints; i++) {
            if (altitude_array[i] > altitude) {
                highbound = i;
                lowbound = i-1;
                break;
            }
        }
    }
    //------ make the interpolation ------
    temp_int = temperature_array[lowbound] + ( (temperature_array[highbound] - temperature_array[lowbound])/(altitude_array[highbound] - altitude_array[lowbound]) ) * (altitude - altitude_array[lowbound]);
    return (temp_int);
}

double AtmosphereModel::density(double altitude) {
    double dens_int;            //Interpolated density
    //------ if altitude is below lowest data point -----------------------------
    if (altitude_array[0] > altitude) {
        return(density_array[0]);
    }
    //------ if altitude is above highest data point ------
    else if (altitude > altitude_array[Ndatapoints-1]) {
        //return(density_array[Ndatapoints-1]);
        return(0.0);
    }
    else {
        //------ Find the data point higher and lower than the altitude value ------
        for (int i=1; i<Ndatapoints; i++) {
            if (altitude_array[i] > altitude) {
                highbound = i;
                lowbound = i-1;
                break;
            }
        }
    }
    //------ make the interpolation ------
    dens_int = density_array[lowbound] + ( (density_array[highbound] - density_array[lowbound])/(altitude_array[highbound] - altitude_array[lowbound]) ) * (altitude - altitude_array[lowbound]);
    //dens_int = 1.225 * exp(-altitude / 7050.0);
    return (dens_int);
}

double AtmosphereModel::pressure(double altitude) {
	
    double press_int;            //Interpolated pressure
    //------ if altitude is below lowest data point -----------------------------
    if (altitude_array[0] > altitude) {
        return(pressure_array[0]);
    }
    //------ if altitude is above highest data point ------
    else if (altitude > altitude_array[Ndatapoints-1]) {
        return(pressure_array[Ndatapoints-1]);
    }
    else {
        //------ Find the data point higher and lower than the altitude value ------
        for (int i=1; i<Ndatapoints; i++) {
            if (altitude_array[i] > altitude) {
                highbound = i;
                lowbound = i-1;
                break;
            }
        }
    }
    //------ make the interpolation ------
    press_int = pressure_array[lowbound] + ( (pressure_array[highbound] - pressure_array[lowbound])/(altitude_array[highbound] - altitude_array[lowbound]) ) * (altitude - altitude_array[lowbound]);
    return (press_int);
}

double AtmosphereModel::speedofsound(double altitude) {
	
    double speed_int;            //Interpolated speedofsound
    //------ if altitude is below lowest data point -----------------------------
    if (altitude_array[0] > altitude) {
        return(speedofsound_array[0]);
    }
    //------ if altitude is above highest data point ------
    else if (altitude > altitude_array[Ndatapoints-1]) {
        return(speedofsound_array[Ndatapoints-1]);
    }
    else {
        //------ Find the data point higher and lower than the altitude value ------
        for (int i=1; i<Ndatapoints; i++) {
            if (altitude_array[i] > altitude) {
                highbound = i;
                lowbound = i-1;
                break;
            }
        }
    }
    //------ make the interpolation ------
    speed_int = speedofsound_array[lowbound] + ( (speedofsound_array[highbound] - speedofsound_array[lowbound])/(altitude_array[highbound] - altitude_array[lowbound]) ) * (altitude - altitude_array[lowbound]);
    return (speed_int);
}

//This function is used by the SEM;
//TO DO : create a common function to get density used by the SEM and Re-entry modules
double AtmosphereModel::getDensity(int central_body, double altitude)
{
    double density = 0.0;

     if (central_body == 3)
     {
        if(altitude>=0 && altitude<100)
            density = 1.225*exp(-altitude/7);
        else if(altitude>=100 && altitude<150)
            density=4.79e-7  - ((altitude-100)*1.81e-9/(50));
        else if(altitude>=150 && altitude<200)
            density=1.81e-9  - ((altitude-150)*2.53e-10/(50));
        else if(altitude>=200 && altitude<250)
            density=2.53e-10 - ((altitude-200)*6.24e-11/(50));
        else if(altitude>=250 && altitude<300)
            density=6.24e-11 - ((altitude-250)*1.95e-11/(50));
        else if(altitude>=300 && altitude<350)
            density=1.95e-11 - ((altitude-300)*6.98e-12/(50));
        else if(altitude>=350 && altitude<400)
            density=6.98e-12 - ((altitude-350)*2.72e-12/(50));
        else if(altitude>=400 && altitude<450)
            density=2.72e-12 - ((altitude-400)*1.13e-12/(50));
        else if(altitude>=450 && altitude<500)
            density=1.13e-12 - ((altitude-450)*4.89e-13/(50));
        else if(altitude>=500 && altitude<550)
            density=4.89e-13 - ((altitude-500)*2.21e-13/(50));
        else if(altitude>=550 && altitude<600)
            density=2.21e-13 - ((altitude-550)*1.04e-13/(50));
        else if(altitude>=600 && altitude<650)
            density=1.04e-13 - ((altitude-600)*5.15e-14/(50));
        else if(altitude>=650 && altitude<700)
            density=5.15e-14 - ((altitude-650)*2.72e-14/(50));
        else if(altitude>=700 && altitude<750)
            density=2.72e-14 - ((altitude-700)*1.55e-14/(50));
        else if(altitude>=750 && altitude<800)
            density=1.55e-14 - ((altitude-750)*9.63e-15/(50));
        else if(altitude>=800 && altitude<850)
            density=9.63e-15 - ((altitude-800)*6.47e-15/(50));
        else if(altitude>=850 && altitude<900)
            density=6.47e-15 - ((altitude-850)*4.66e-15/(50));
        else if(altitude>=900 && altitude<950)
            density=4.66e-15 - ((altitude-900)*3.54e-15/(50));
        else if(altitude>=950 && altitude<1000)
            density=3.54e-15 - ((altitude-950)*2.79e-15/(50));
        else if(altitude>=1000 && altitude<1250)
            density=2.79e-15 - ((altitude-1000)*1.11e-15/(250));
        else if(altitude>=1250 && altitude<=1500)
            density=1.11e-15 - ((altitude-1250)*5.21e-16/(250));
        else
            density=0;
    }

    return density;
}
