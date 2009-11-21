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


#ifndef MODEL_H_INCLUDED
#define MODEL_H_INCLUDED

#include <QString>

using namespace std;

class AtmosphereModel {
    private:
        double *altitude_array;
        double *density_array;
        double *pressure_array;
        double *speedofsound_array;
        double *temperature_array;
        //------
        int Ndatapoints;                    //Number of datapoints in the interpolation file
        int lowbound, highbound;
    public:
        void selectModel (QString);
        //------
        double temperature(double);
        double density(double);
        double speedofsound(double);
        double pressure(double);

        //This function is used by the SEM;
        //TO DO : create a common function to get density used by the SEM and Re-entry modules
        double getDensity(int, double);

};


#endif // MODEL_H_INCLUDED
