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
