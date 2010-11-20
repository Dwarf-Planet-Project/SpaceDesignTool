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


#ifndef CAPSULE_H_INCLUDED
#define CAPSULE_H_INCLUDED
#include <iostream>
#include <fstream>
#include <QString>

using namespace std;

class capsule_class {
    private:
        double *MachC_array;
        double *MachP_array;
        double *Cdc_array;
        double *Clc_array;
        double *Csc_array;

        double *Cdp_array;
        //------
        int NdatapointsC;                    //Number of datapoints in the interpolation file
        int NdatapointsP;
        int lowbound, highbound;
    public:
        double Sc;                              //Reference area of capsule
        double Sp;                              //Reference area of parachute
        double m;                               //Mass
        double Rn;                              //Nose radius
        //------
        void selectCdCprofile (QString);
        void selectCsCprofile (QString);
        void selectClCprofile (QString);

        void selectCdPprofile (QString);
        double cdc(double);
        double clc(double);
        double csc(double);
        double cdp(double);
        double flag;   //to detect errors in input parameters
};

#endif // CAPSULE_H_INCLUDED
