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
        void selectCdPprofile (QString);
        double cdc(double);
        double cdp(double);
        double flag;   //to detect errors in input parameters
};

#endif // CAPSULE_H_INCLUDED
