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


//-----------------------------------------------------------------------------------
// File name: arithmetics.cpp
//

//------------------ Author:       Guillermo Ortega               -------------------
//------------------ Affiliation:  European Space Agency (ESA)    -------------------
//-----------------------------------------------------------------------------------

/*
Modified by Guillermo on June 2010 to add  arithmetics to support STA integrators.
These integrators are based on the work of Matthias Gerdts from the University of Wuerzburg

*/



#include "arithmetics.h"
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <cmath>



#define NORM3(A) (sqrt((A)[0]*(A)[0] + (A)[1]*(A)[1] + (A)[2]*(A)[2]))
#define DOT3(A,B) ((A)[0]*(B)[0]+(A)[1]*(B)[1]+(A)[2]*(B)[2])




double epsilon ( void )          // Returns the round off unit for doubleing arithmetic.
/*
epsilon is a number R which is a power of 2 with the property that,
to the precision of the computer's arithmetic satisfies the following:
      1 < 1 + R
    but
      1 = ( 1 + R / 2 )
*/
{
  double r;

  r = 1.0;

  while ( 1.0 < ( double ) ( 1.0 + r )  )
  {
    r = r / 2.0;
  }

  return ( 2.0 * r );
}


double sign ( double x )        // Returns the sign of a double
{
  if ( x < 0.0 )
  {
    return ( -1.0 );
  }
  else
  {
    return ( +1.0 );
  }
}








double* allocDoubleVector(int n, bool setZero)
{
   if (n <= 0) return NULL;

   // allocate one array with n entries
   double* v = (double*)malloc(sizeof(double)*n);
   if ( !v ) return NULL;

   // initialize to 0
   if (setZero)
     for (int i=0; i<n; i++)
      v[i] = 0.0;

   return v;
}





int* allocIntVector(int n, bool setZero)
{
   if (n <= 0) return NULL;

   // allocate one array with n entries
   int* v = (int*)malloc(sizeof(int)*n);
   if ( !v ) return NULL;

   // initialize to 0
   if (setZero)
     for (int i=0; i<n; i++)
      v[i] = 0;

   return v;
}




double** allocDoubleMatrix(int n, int m, bool setZero)
{
   if ((n <= 0) || (m <= 0)) return NULL;

   // allocate one array with n*m entries
   double* help = (double*)malloc(sizeof(double)*n*m);
   if ( !help ) return NULL;

   // initialize to 0
   if (setZero)
     for (int i=0; i<n*m; i++)
      help[i] = 0.0;

   // set row pointers
   double** A = (double**)malloc(sizeof(double*)*n);
   if ( !A )
   {
      free(help);
      return NULL;
   }
   for (int i=0; i<n; i++)
      A[i] = &help[i*m];

   return A;
}





void freeDoubleMatrix(double** A, int n)
{
   if (n > 0)
   {
      free(*A);
      free(A);
   }
}





void findiff1(void f(double t, double z[], double param[], double zp[]),
   double param[], int nx, int np, double t, double z[], double **fzjac)
{
  int i,j;
  double *wa,*fz,temp,h;
  double eps = SQRTEPS;
  // allocate memory
  wa = allocDoubleVector(nx,false);
  fz = allocDoubleVector(nx,false);
  // compute finite difference approximation of Jacobian w.r.t.z and p
  f( t,z,param,fz );
  for (j=0;j<nx;j++) {
    temp = z[j];
    h    = eps*fmax(1.0,fabs(temp));
    z[j] = temp + h;
    f( t,z,param,wa );
    z[j] = temp;
    for (i=0;i<nx;i++) {
      fzjac[i][j] = (wa[i] - fz[i])/h;
    }
  }
  for (j=0;j<np;j++) {
    temp = param[j];
    h = eps*fmax(1.0,fabs(temp));
    param[j] = temp + h;
    f( t,z,param,wa );
    param[j] = temp;
    for (i=0;i<nx;i++) {
      fzjac[i][nx+j] = (wa[i] - fz[i])/h;
    }
  }
  free(fz);
  free(wa);
}






void sens(void f(double t, double z[], double param[], double zp[]),
    void fjac(double t, double z[], double param[], double** jac),
    int ijac, double param[], int neq, int np, double t, double y[],
    double sp[] )
{
  int i,j,k,nx;
  double **jac, sum;
  // allocate memory for Jacobian
  nx = neq/(1+np);
  jac = allocDoubleMatrix(nx, nx+np,false);
  //
  if (ijac == 1)
    fjac( t,y,param,jac );
  else
    findiff1(f,param,nx,np,t,y,jac);
  // righthandside of sensitivity equation
  for (i=0;i<np;i++) {
    for (k=0;k<nx;k++) {
      sum = 0.0;
      for (j=0;j<nx;j++)
	sum = sum + jac[k][j]*y[i*nx+j];
      sum = sum + jac[k][nx+i];
      sp[i*nx+k] = sum;
    }
  }
  // deallocate memory
  freeDoubleMatrix(jac,nx);
}

