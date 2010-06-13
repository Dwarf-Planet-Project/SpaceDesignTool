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
 ------------------ Author: Valentino Zuccarelli  -------------------------------------------------
 ------------------ E-mail: (Valentino.Zuccarelli@gmail.com) ----------------------------
 */

#include "propagateTHREEbody.h"
#include <cmath>
#include "EODE/eode.h"
#include <iomanip>
#include <QtDebug>


using namespace std;

extern double Grav_Param;

void threebody_EOM (double time , double sol[], double par[], double deriv[])

{
        double r1, r2;
        radius (Grav_Param, sol[3], sol[4], sol[5], r1, r2);	//function call for radius

        //deriv[0] = (x2p)		sol[0] = xp
        //deriv[1] = (y2p)		sol[1] = yp
        //deriv[2] = (z2p)		sol[2] = zp
        //deriv[3] = (xp)		sol[3] = x
        //deriv[4] = (yp)		sol[4] = y
        //deriv[5] = (zp)		sol[5] = zp

        //equations of motion: CR3BP
        deriv[0]=2*sol[1]+sol[3]-(1-Grav_Param)*(sol[3]+Grav_Param)/(pow(r1,3))+ Grav_Param*(1-Grav_Param-sol[3])/(pow(r2,3));
        deriv[1]=-2*sol[0]+sol[4]-(1-Grav_Param)*sol[4]/(pow(r1,3))-Grav_Param*sol[4]/(pow(r2,3));
        deriv[2]=-(1-Grav_Param)*sol[5]/(pow(r1,3))-Grav_Param*sol[5]/(pow(r2,3));
        deriv[3]=sol[0];
        deriv[4]=sol[1];
        deriv[5]=sol[2];
}



void halorbit_EOM (double time , double sol[], double par[], double deriv[])

{
        double r1, r2;
        radius (Grav_Param, sol[3], sol[4], sol[5], r1, r2);	//function call for radius

        //deriv[0] = (x2p)		sol[0] = xp
        //deriv[1] = (y2p)		sol[1] = yp
        //deriv[2] = (z2p)		sol[2] = zp
        //deriv[3] = (xp)		sol[3] = x
        //deriv[4] = (yp)		sol[4] = y
        //deriv[5] = (zp)		sol[5] = z
        //deriv[6-41]=STMp		sol[6-41]=STM

        //equations of motion: CR3BP
        deriv[0]=2*sol[1]+sol[3]-(1-Grav_Param)*(sol[3]+Grav_Param)/(pow(r1,3))+ Grav_Param*(1-Grav_Param-sol[3])/(pow(r2,3));
        deriv[1]=-2*sol[0]+sol[4]-(1-Grav_Param)*sol[4]/(pow(r1,3))-Grav_Param*sol[4]/(pow(r2,3));
        deriv[2]=-(1-Grav_Param)*sol[5]/(pow(r1,3))-Grav_Param*sol[5]/(pow(r2,3));
        deriv[3]=sol[0];
        deriv[4]=sol[1];
        deriv[5]=sol[2];

        double F[35];

        F[18]=1-(1-Grav_Param)/pow(r1,3)+3*(1-Grav_Param)*pow(sol[3]+Grav_Param,2)/pow(r1,5)-Grav_Param/pow(r2,3)+3*Grav_Param*pow(sol[3]+Grav_Param-1,2)/pow(r2,5);
        F[25]=1-(1-Grav_Param)/pow(r1,3)+3*(1-Grav_Param)*pow(sol[4],2)/pow(r1,5)-Grav_Param/pow(r2,3)+3*Grav_Param*pow(sol[4],2)/pow(r2,5);
        F[32]=(-1+Grav_Param)/pow(r1,3)-Grav_Param/pow(r2,3)+3*pow(sol[5],2)*((1-Grav_Param)/pow(r1,5)+Grav_Param/pow(r2,5));
        F[19]=F[24]=3*sol[4]*((1-Grav_Param)*(sol[3]+Grav_Param)/pow(r1,5)+Grav_Param*(sol[3]+Grav_Param-1)/pow(r2,5));
        F[20]=F[30]=3*sol[5]*((1-Grav_Param)*(sol[3]+Grav_Param)/pow(r1,5)+Grav_Param*(sol[3]+Grav_Param-1)/pow(r2,5));
        F[26]=F[31]=3*sol[4]*sol[5]*((1-Grav_Param)/pow(r1,5)+Grav_Param/pow(r2,5));

        F[3]=F[10]=F[17]=1;
        F[22]=2; F[27]=-2;
        F[0]=F[1]=F[2]=F[4]=F[5]=F[9]=F[6]=F[7]=F[8]=F[12]=F[13]=F[14]=F[11]=F[15]=F[16]=
        F[21]=F[23]=F[28]=F[29]=F[33]=F[34]=F[35]=0;

        int l, j, q;
        double der[42];
                for (l=0; l<36; l++)
                {q=(l+6)/6; //number of line
                 der[l+6]=F[(q-1)*6]*sol[(l+6)%6+6];
                 for (j=1; j<6; j++)
                        {der[l+6]=der[l+6]+F[(q-1)*6+j]*sol[(l+6)%6+(j+1)*6];}
                deriv[l+6]=der[l+6];}
}



void radius (double mi, double x, double y, double z, double &R1, double &R2)

{
        R1 = sqrt(pow(x+mi,2)+pow(y,2)+pow(z,2));	//(distance of mass 3 from mass 1)
        R2 = sqrt(pow(x-1+mi,2)+pow(y,2)+pow(z,2));	//(distance of mass 3 from mass 2)

}
