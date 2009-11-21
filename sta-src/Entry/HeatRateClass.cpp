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
#include <QDebug>
#include <QTextStream>
#include <QDir>
#include <QProcess>

#include "HeatRateClass.h"

using namespace std;

void HeatRateClass::selectBody (QString name)
{
    QString path = QString("data/heatrates/");
    path.append(name);

    //QTextStream out (stdout); out << "===> heatrates path: " << path << endl;

    QFile heatrate(path);
    heatrate.open(QIODevice::ReadOnly);
    QTextStream heatratestream(&heatrate);
    Ndatapoints = 0;
    //------
        while (!heatratestream.atEnd())
    {
        heatratestream.readLine();      //read in a line just to count it
        Ndatapoints++;                      //increase the counter
    }

    Ndatapoints -= 9;                  //The loop ends after 1 extra count, we need to subtract by 1, and there are 6 parameters before the table
    //------ Allocate the memory for the data arrays ------
    velocity_array = new double[Ndatapoints];
    function_array = new double[Ndatapoints];
    //------ Close and open the file again, workaround to start reading at the beginning of the file (moving the pointer did not work) ------
    heatrate.close();
    heatrate.open(QIODevice::ReadOnly);
    //------ Fill up the data arrays ------
    heatratestream >> k;
    heatratestream >> C;
    heatratestream >> aselector;
    heatratestream >> a;
    heatratestream >> a1; heatratestream >> a2; heatratestream >> a3;
    heatratestream >> b;
    for (int i=0; i<Ndatapoints; i++) {
        heatratestream >> velocity_array[i];
        heatratestream >> function_array[i];
    }
    //------ Close the data file ------
    heatrate.close();
 }


double HeatRateClass::ConvectiveHeatRate (double V, double density, double noseradius)
{
    double qcdot;
    qcdot = k * pow( (density / noseradius), 0.5) * pow(V,3) / 10000.0;  //in W/cm²
    return(qcdot);
}


double HeatRateClass::RadiativeHeatRate (double V, double density, double noseradius)
{
    double qrdot;
    if (aselector == 2) {
        qrdot = C * pow( noseradius, (a1*pow(V,a2)*pow(density,a3)) ) * pow(density,b) * qrdotInterpolation(V);
        if (density==0)
            qrdot = 0;
    }
    else {
        qrdot = C * pow(noseradius,a) * pow(density,b) * qrdotInterpolation(V);
    }

    return(qrdot);
}


double HeatRateClass::qrdotInterpolation (double V)
{

    double func_int;            //Interpolated function value
    //------ if velocity is below lowest data point -----------------------------
    if (velocity_array[0] > V) {
        return(0.0);
    }
    //------ if velocity is above highest data point ------
    else if (V > velocity_array[Ndatapoints-1]) {
        return(0.0);
    }
    else {
        //------ Find the data point higher and lower than the altitude value ------
        for (int i=1; i<Ndatapoints; i++) {
            if (velocity_array[i] > V) {
                highbound = i;
                lowbound = i-1;
                break;
            }
        }
    }
    //------ make the interpolation ------
    func_int = function_array[lowbound] + ( (function_array[highbound] - function_array[lowbound])/(velocity_array[highbound] - velocity_array[lowbound]) ) * (V - velocity_array[lowbound]);
    return (func_int);
}

