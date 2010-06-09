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
 ------------------ Author: Dominic Dirkx  ------------------------------------------
 ------------------ E-mail: (d.dirkxATstudent.tudelft.nl) ----------------------------
*/

#include "math.h"

void bodyTOwind(double alpha, double beta, double* vector)
{

    double convMat[3][3]=
    {
        {cos(alpha)*cos(beta),-sin(beta),sin(alpha)*cos(beta)},
        {cos(alpha)*sin(beta),cos(beta),sin(alpha)*sin(beta)},
        {-sin(alpha),0,cos(alpha)}
    };

    double comp0,comp1,comp2;
    comp0=convMat[0][0]*vector[0]+convMat[0][1]*vector[1]+convMat[0][2]*vector[2];
    comp1=convMat[1][0]*vector[0]+convMat[1][1]*vector[1]+convMat[1][2]*vector[2];
    comp2=convMat[2][0]*vector[0]+convMat[2][1]*vector[1]+convMat[2][2]*vector[2];
    vector[0]=comp0;
    vector[1]=comp1;
    vector[2]=comp2;


}
