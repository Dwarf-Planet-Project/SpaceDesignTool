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
                                  EODE
 The European Space Agency Ordinary Differntial Equations library (E-ODE)
 based on the GNU scientific library ODE C-code

 Description of a system of ODEs.

 y' = f(t,y) = dydt(t, y)


 ------------------ Author:       Guillermo Ortega               -------------------
 ------------------ Affiliation:  European Space Agency (ESA)    -------------------
 -----------------------------------------------------------------------------------
 Modified by Tiziana Sabatini on July 2009 (rk4) to convert double[] into Eigen vectors
 Modified by Guillermo on June 2010 to add the following integrators:
  - Adams-Bashforth explicit multistep method with fixed step width
  - Adams-Moulton implicit multistep method with fixed step width
  - Runge-Kutta explicit methods
  - Runge-Kutta explicit methods with step size control and sensitivity analysis
  - Gragg-Bulirsch-Stoer extrapolation method with fixed step width
  - Nystrom method for second order differential equations with fixed step width
 These integrators are based on the work of Matthias Gerdts from the University of Wuerzburg

*/

#include <stdlib.h>
#include <string.h>
#include <cmath>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>

//#include <Eigen/Core>
#include "arithmetics.h"
#include "eode.h"

#include <algorithm>

using namespace std;



/////////////////////////////////////////// Defining the auxiliary functions ////////////////////////////////////////////////

// takes one Fehlberg -fourth-fifth order- step. The routine advances the solution over the fixed step H and returns the
// fifth order (sixth order accurate locally) solution approximation at T+H in array S.
void Step_Integrator
        (
                void f                        // Input, external f, a user-supplied subroutine to evaluate the derivatives y'(t)
                (
                        double t,             // Input, independent variable
                        double y[],           // Input, dependent variable
                        double param[],
                        double yp[] ),        // Input, derivative of the dependent variable
                int neqn,                     // Input, the number of equations to be integrated
                double y[],                   // Input, double Y[NEQN], the current value of the dependent variable
                double param[],
                double t,                     // Input, the current value of the independent variable
                double h,                     // Input, the step size to take
                double yp[],                  // Input, double YP[NEQN], the current value of the derivative of the dependent variable
                double f1[],                  // Output, 1st derivative value needed for the computation
                double f2[],                  // Output, 2nd derivative value needed for the computation
                double f3[],                  // Output, 3rd derivative value needed for the computation
                double f4[],                  // Output, 4th derivative value needed for the computation
                double f5[],                  // Output, 5th derivative value needed for the computation
                double s[]                    // Output, the estimate of the solution at t+h
                )
{

/*  Reference: Erwin Fehlberg, Low-order Classical Runge-Kutta Formulas with Stepsize Control, NASA Technical Report R-315, 1969.
*/

  double ch;
  int i;

  ch = h / 4.0;

  for ( i = 0; i < neqn; i++ )
  {
    f5[i] = y[i] + ch * yp[i];
  }

  f ( t + ch, f5,param, f1 );

  ch = 3.0 * h / 32.0;

  for ( i = 0; i < neqn; i++ )
  {
    f5[i] = y[i] + ch * ( yp[i] + 3.0 * f1[i] );
  }

  f ( t + 3.0 * h / 8.0, f5, param,f2 );

  ch = h / 2197.0;

  for ( i = 0; i < neqn; i++ )
  {
    f5[i] = y[i] + ch *
    ( 1932.0 * yp[i]
    + ( 7296.0 * f2[i] - 7200.0 * f1[i] )
    );
  }

  f ( t + 12.0 * h / 13.0, f5,param, f3 );

  ch = h / 4104.0;

  for ( i = 0; i < neqn; i++ )
  {
    f5[i] = y[i] + ch *
    (
      ( 8341.0 * yp[i] - 845.0 * f3[i] )
    + ( 29440.0 * f2[i] - 32832.0 * f1[i] )
    );
  }

  f ( t + h, f5,param, f4 );

  ch = h / 20520.0;

  for ( i = 0; i < neqn; i++ )
  {
    f1[i] = y[i] + ch *
    (
      ( -6080.0 * yp[i]
      + ( 9295.0 * f3[i] - 5643.0 * f4[i] )
      )
    + ( 41040.0 * f1[i] - 28352.0 * f2[i] )
    );
  }

  f ( t + h / 2.0, f1, param,f5 );
//
//  Ready to compute the approximate solution at T+H.
//
  ch = h / 7618050.0;

  for ( i = 0; i < neqn; i++ )
  {
    s[i] = y[i] + ch *
    (
      ( 902880.0 * yp[i]
      + ( 3855735.0 * f3[i] - 1371249.0 * f4[i] ) )
    + ( 3953664.0 * f2[i] + 277020.0 * f5[i] )
    );
  }

  return;
}





/*!
   \brief Computes one step of the DOPRI5(4) integration procedure.

   Computes one step of the DOPRI5(4) integration procedure.

   \param[in]  f      right handside function
   \param[in]  fjac   Jacobian evaluation function
   \param[in]  ijac   flag
			- ijac == 1 : user supplied Jacobian in fjac
			- ijac != 1 : finite difference Jacobian approximation
   \param[in]  param  parameters
   \param[in]  t      time
   \param[in]  h      step size
   \param[in]  neq    number of equations including sensitivities
   \param[in]  y      state \f$ y(t) \f$
   \param[in]  k1     user must provide \f$ f(t,y(t)) \f$
   \param[out] solp   solution of 5th order method
   \param[out] solq   solution of 4th order method
   \param[in]  np     number of parameters
   \param[in,out] k2  auxiliary array for storing stages of RK
   \param[in,out] k3  auxiliary array for storing stages of RK
   \param[in,out] k4  auxiliary array for storing stages of RK
   \param[in,out] k5  auxiliary array for storing stages of RK
   \param[in,out] k6  auxiliary array for storing stages of RK
   \param[in,out] k7  auxiliary array for storing stages of RK
*/
void dopristep(void f(double t, double y[], double param[], double yp[]),
	 void fjac(double t, double y[], double param[], double** jac),
	 int ijac, double* param, double t, double h, int neq, double* y,
	 double *k1, double *solp, double *solq, int np, double *k2, double *k3,
	 double *k4, double *k5, double *k6,double *k7 )
{
  int i,nx;
  double ch,tch;
  // procedure start
  nx = neq/(np+1);
  // computation of y2=y+a21*h*k1 (a21=1/5) and k2 = f( t+c2*h,y2 ) (c2=1/5)
  ch = h/5.0;
  for (i=0;i<neq;i++) k7[i] = y[i]+ch*k1[i];
  tch = t+ch;
  f( tch,k7,param,k2 );
  if (np > 0) sens(f,fjac,ijac,param,neq,np,tch,k7,&k2[nx]);

  // computation of y3=y+h*(a31*k1+a32*k2) (a31=3/40,a32=9/40) and k3 = f( t+c3*h,y3 ) (c3=3/10)
  ch = 3.0*h/40.0;
  for (i=0;i<neq;i++) k7[i] = y[i]+ch*(k1[i]+3.0*k2[i]);
  tch = t+3.0*h/10.0;
  f( tch,k7,param,k3 );
  if (np > 0) sens(f,fjac,ijac,param,neq,np,tch,k7,&k3[nx]);
  // computation of y4=y+h*(a41*k1+a42*k2+a43*k3) (a41=44/45,a42=-56/15,a43=32/9)
  // and k4 = f( t+c4*h,y4 ) (c4=4/5)
  ch = h/3.0;
  for (i=0;i<neq;i++) k7[i] = y[i]+ch*(44.0*k1[i]/15.0-56.0*k2[i]/5.0+32.0*k3[i]/3.0);
  tch = t+4.0*h/5.0;
  f( tch,k7,param,k4 );
  if (np > 0) sens(f,fjac,ijac,param,neq,np,tch,k7,&k4[nx]);
  // computation of y5=y+h*(a51*k1+a52*k2+a53*k3+a54*k4)
  // (a51=19372/6561,a52=-25360/2187,a53=64448/6561,a54=-212/729) and
  // k5 = f( t+c5*h,y5 ) (c5=8/9)
  ch = h/729.0;
  for (i=0;i<neq;i++) k7[i] = y[i]+ch*(19372.0*k1[i]/9.0-25360.0*k2[i]/3.0+64448.0*k3[i]/9.0-212.0*k4[i]);
  tch = t+8.0*h/9.0;
  f( tch,k7,param,k5 );
  if (np > 0) sens(f,fjac,ijac,param,neq,np,tch,k7,&k5[nx]);
  // computation of y6=y+h*(a61*k1+a62*k2+a63*k3+a64*k4+a65*k5)
  // (a61=9017/3168,a62=-355/33,a63=46732/5247,a64=49/176,a65=-5103/18656)
  // and k6 = f( t+c6*h,y6 ) (c6=1)
  ch = h;
  for (i=0;i<neq;i++) k7[i] = y[i]+ch*(9017.0*k1[i]/3168.0-355.0*k2[i]/33.0+46732.0*k3[i]/5247.0
	       +49.0*k4[i]/176.0-5103.0*k5[i]/18656.0);
  tch = t+h;
  f( tch,k7,param,k6 );
  if (np > 0) sens(f,fjac,ijac,param,neq,np,tch,k7,&k6[nx]);
  // computation of y7=y+h*(a71*k1+a72*k2+a73*k3+a74*k4+a75*k5+a76*k6)
  // (a71=35/384,a72=0,a73=500/1113,a74=125/192,a75=-2187/6784,a76=11/84)
  // and k7 = f( t+c7*h,y7 ) (c7=1)
  ch = h/142464.0;
  for (i=0;i<neq;i++) solp[i] = y[i]+ch*(12985.0*k1[i]+64000.0*k3[i]+92750.0*k4[i]-45927.0*k5[i]
	   +18656.0*k6[i]);
  tch = t+h;
  f( tch,solp,param,k7 );
  if (np > 0) sens(f,fjac,ijac,param,neq,np,tch,solp,&k7[nx]);
  // computation of solution
  // sol = y+h*(b1*k1+b2*k2+b3*k3+b4*k4+b5*k5+b6*k6+b7*k7)
  // (b1=35/384,b2=0,b3=500/1113,b4=125/192,b5=-2187/6784,b6=11/84,b7=0)
  // (b1=5179/57600,b2=0,b3=7571/16695,b4=393/640,b5=-92097/339200,
  // b6=187/2100,b7=1/40)
  for (i=0;i<neq;i++) solq[i] = y[i]+h*(5179.0*k1[i]/57600.0+7571.0*k3[i]/16695.0
	  +393.0*k4[i]/640.0-92097.0*k5[i]/339200.0
	  +187.0*k6[i]/2100.0+k7[i]/40.0);
}




///////////////////////////////////////  Defining the Integrators ////////////////////////////////////////
void rkf45step(void f(double t, double y[], double param[], double yp[]),
	 void fjac(double t, double y[], double param[], double** jac),
	 int ijac,double* param,double t,double h,int neq,double *y,
	 double *k1,double *solp,double *solq,int np,
	 double *k2,double *k3,double *k4,double *k5,double *k6 )
{
  int k,nx;
  double ch,tch;
  // procedure start
  nx = neq/(np+1);
  ch=h/4.0;
  for (k=0;k<neq;k++) k6[k]=y[k]+ch*k1[k];
  tch = t+ch;
  f(tch,k6,param,k2 );
  if (np > 0) sens(f,fjac,ijac,param,neq,np,tch,k6,&k2[nx]);
  //
  ch=3.0*h/32.0;
  for (k=0;k<neq;k++) k6[k]=y[k]+ch*(k1[k]+3.0*k2[k]);
  tch = t+3.0*h/8.0;
  f(tch,k6,param,k3 );
  if (np > 0) sens(f,fjac,ijac,param,neq,np,tch,k6,&k3[nx]);
  //
  ch=h/2197.0;
  for (k=0;k<neq;k++) k6[k]=y[k]+ch*(1932.0*k1[k]+(7296.0*k3[k]-7200.0*k2[k]));
  tch = t+12.0*h/13.0;
  f(tch,k6,param,k4 );
  if (np > 0) sens(f,fjac,ijac,param,neq,np,tch,k6,&k4[nx]);
    //
  ch=h/4104.0;
  for (k=0;k<neq;k++) k6[k]=y[k]+ch*((8341.0*k1[k]-845.0*k4[k])+(29440.0*k3[k]-32832.0*k2[k]));
  tch = t+h;
  f(tch,k6,param,k5 );
  if (np > 0) sens(f,fjac,ijac,param,neq,np,tch,k6,&k5[nx]);
    //
  ch=h/20520.0;
  for (k=0;k<neq;k++) k2[k]=y[k]+ch*((-6080.0*k1[k]+(9295.0*k4[k]-5643.0*k5[k]))
	     +(41040.0*k2[k]-28352.0*k3[k]));
  tch = t+h/2.0;
  f(tch,k2,param,k6 );
  if (np > 0) sens(f,fjac,ijac,param,neq,np,tch,k2,&k6[nx]);
  // compute approximate solution at t+h
  for (k=0;k<neq;k++) {
    solp[k] = y[k]+h*(25.0*k1[k]/216.0+1408.0*k3[k]/2565.0+2197.0*k4[k]/4104.0-k5[k]*0.2);
    solq[k] = y[k]+h*(16.0*k1[k]/135.0+6656.0*k3[k]/12825.0+28561.0*k4[k]/56430.0-9.0*k5[k]/50.0
	  +2.0*k6[k]/55.0);
  }
}

void rkf78step(void f(double t, double y[], double param[], double yp[]),
	 void fjac(double t, double y[], double param[], double** jac),
	 int ijac,double* param,double t,double h,int neq,double *y,
	 double *k1,double *solp,double *solq,int np,
	 double *k2,double *k3,double *k4,double *k5,double *k6,double* k7,
	 double *k8,double *k9,double *k10,double *k11,double *k12 )
{
  int k,nx;
  double ch,tch;
  // procedure start
  nx = neq/(np+1);
  // computation of k2
  ch=2.0*h/27.0;
  for (k=0;k<neq;k++) k12[k]=y[k]+ch*k1[k];
  tch = t+ch;
  f(tch,k12,param,k2 );
  if (np > 0) sens(f,fjac,ijac,param,neq,np,tch,k12,&k2[nx]);
  // computation of k3
  ch=h/3.0;
  for (k=0;k<neq;k++) k12[k]=y[k]+ch*(k1[k]/12.0+k2[k]/4.0);
  tch = t+ch/3.0;
  f(tch,k12,param,k3 );
  if (np > 0) sens(f,fjac,ijac,param,neq,np,tch,k12,&k3[nx]);
  // computation of k4
  ch=h/2.0;
  for (k=0;k<neq;k++) k2[k]=y[k]+ch*(k1[k]/12.0+k3[k]/4.0);
  tch = t+ch/3.0;
  f(tch,k2,param,k4 );
  if (np > 0) sens(f,fjac,ijac,param,neq,np,tch,k2,&k4[nx]);
  // computation of k5
  ch=5.0*h/4.0;
  for (k=0;k<neq;k++) k2[k]=y[k]+ch*(k1[k]/3.0+5.0*(-k3[k]+k4[k])/4.0);
  tch = t+ch/3.0;
  f(tch,k2,param,k5 );
  if (np > 0) sens(f,fjac,ijac,param,neq,np,tch,k2,&k5[nx]);
  // computation of k6
  for (k=0;k<neq;k++) k2[k]=y[k]+h*(k1[k]/20.0+k4[k]/4.0+k5[k]/5.0);
  tch = t+h/2.0;
  f(tch,k2,param,k6 );
  if (np > 0) sens(f,fjac,ijac,param,neq,np,tch,k2,&k6[nx]);
  // computation of k7
  ch = h/108.0;
  for (k=0;k<neq;k++) k2[k]=y[k]+ch*(-25.0*k1[k]+125.0*k4[k]-260.0*k5[k]+250.0*k6[k]);
  tch = t+5.0*h/6.0;
  f(tch,k2,param,k7 );
  if (np > 0) sens(f,fjac,ijac,param,neq,np,tch,k2,&k7[nx]);
  // computation of k8
  for (k=0;k<neq;k++) k2[k]=y[k]+h*((93.0*k1[k]-200.0*k6[k]+13.0*k7[k])/900.0+61.0*k5[k]/225.0);
  tch = t+h/6.0;
  f(tch,k2,param,k8 );
  if (np > 0) sens(f,fjac,ijac,param,neq,np,tch,k2,&k8[nx]);
  // computation of k9
  ch = h/90.0;
  for (k=0;k<neq;k++) k2[k]=y[k]+ch*(180.0*k1[k]-795.0*k4[k]+1408.0*k5[k]-1070.0*k6[k]+67.0*k7[k]
	     +270.0*k8[k]);
  tch = t+2.0*h/3.0;
  f(tch,k2,param,k9 );
  if (np > 0) sens(f,fjac,ijac,param,neq,np,tch,k2,&k9[nx]);
  // computation of k10
  for (k=0;k<neq;k++) k2[k]=y[k]+h*((-91.0*k1[k]+23.0*k4[k]+622.0*k6[k]+306.0*k8[k]-9.0*k9[k])/108.0
	    -976.0*k5[k]/135.0-19.0*k7[k]/60.0);
  tch = t+h/3.0;
  f(tch,k2,param,k10 );
  if (np > 0) sens(f,fjac,ijac,param,neq,np,tch,k2,&k10[nx]);
  // computation of k11
  ch = h/4100.0;
  for (k=0;k<neq;k++) k2[k]=y[k]+ch*(2383.0*k1[k]-8525.0*k4[k]+17984.0*k5[k]-15050.0*k6[k]
	     +2133.0*k7[k]+2250.0*k8[k]+1125.0*k9[k]+1800.0*k10[k]);
  tch = t+h;
  f(tch,k2,param,k11 );
  if (np > 0) sens(f,fjac,ijac,param,neq,np,tch,k2,&k11[nx]);
  // computation of k12
  ch = h/205.0;
  for (k=0;k<neq;k++) k2[k]=y[k]+ch*(3.0*k1[k]-30.0*k6[k]-3.0*k7[k]-15.0*k8[k]+15.0*k9[k]+30.0*k10[k]);
  tch = t;
  f(tch,k2,param,k12 );
  if (np > 0) sens(f,fjac,ijac,param,neq,np,tch,k2,&k12[nx]);
  // computation of k13(=k3)
  ch = h/4100.0;
  for (k=0;k<neq;k++) k2[k]=y[k]+ch*(-1777.0*k1[k]-8525.0*k4[k]+17984.0*k5[k]-14450.0*k6[k]
	     +2193.0*k7[k]+2550.0*k8[k]+825.0*k9[k]+1200.0*k10[k]+4100.0*k12[k]);
  tch = t+h;
  f(tch,k2,param,k3 );
  if (np > 0) sens(f,fjac,ijac,param,neq,np,tch,k2,&k3[nx]);
  // computation of solution
  ch = h/840.0;
  for (k=0;k<neq;k++) {
    solp[k] = y[k]+ch*(41.0*(k1[k]+k11[k])+272.0*k6[k]+216.0*(k7[k]+k8[k])+27.0*(k9[k]+k10[k]));
    solq[k] = y[k]+ch*(272.0*k6[k]+216.0*(k7[k]+k8[k])+27.0*(k9[k]+k10[k])+41.0*(k12[k]+k3[k]));
  }
}

void rkf23step(void f(double t, double y[], double param[], double yp[]),
	 void fjac(double t, double y[], double param[], double** jac),
	 int ijac,double* param,double t,double h,int neq,double *y,
	 double *k1,double *solp,double *solq,int np,
	 double *k2,double *k3,double *k4,double *k5 )
{
  int k,nx;
  double ch,tch;
  // procedure start
  nx = neq/(np+1);
  ch=h/4.0;
  for (k=0;k<neq;k++) k5[k]=y[k]+ch*k1[k];
  tch = t+ch;
  f(tch,k5,param,k2 );
  if (np > 0) sens(f,fjac,ijac,param,neq,np,tch,k5,&k2[nx]);
  //
  ch=h/800.0;
  for (k=0;k<neq;k++) k5[k]=y[k]+ch*(-189.0*k1[k]+729.0*k2[k]);
  tch = t+27.0*h/40.0;
  f(tch,k5,param,k3 );
  if (np > 0) sens(f,fjac,ijac,param,neq,np,tch,k5,&k3[nx]);
  //
  ch=h/891.0;
  for (k=0;k<neq;k++) k5[k]=y[k]+ch*(214.0*k1[k]+27.0*k2[k]+650.0*k3[k]);
  tch = t+h;
  f(tch,k5,param,k4 );
  if (np > 0) sens(f,fjac,ijac,param,neq,np,tch,k5,&k4[nx]);
  // computation of solution
  for (k=0;k<neq;k++) {
    solp[k] = y[k]+h*(214.0*k1[k]/891.0+k2[k]/33.0+650.0*k3[k]/891.0);
/*! \bug Hier könnte noch etwas falsch sein: solq liefert keine besseren Ergebnisse als solp */
    solq[k] = y[k]+h*(533.0*k1[k]/2106.0+800.0*k3[k]/1053.0-k4[k]/78.0);
  }
}



int Runge_Kutta_Fehlberg
        (
        void f (                    // Input, external f, a user-supplied subroutine to evaluate the derivatives y'(t)
                double t,           // Input, time independent variable
                double y[],         // Input, the function to evaluate
                double param[],
                double yp[]         // Input, the derivatiev of the function to calculate
                ),
        int neqn,                   // Input, the number of equations to be integrated
        double y[],                 // Input/output, the current solution vector at time t
        double param[],
        double yp[],                // Input/output, the derivative of the current solution vector at time t.  The user should not set or alter this information!
        double *t,                  // Input/output, the current value of the independent variable.
        double tout,                // Input, the output point at which solution is desired. TOUT = T is allowed on the first call only, in which case the routine returns with FLAG = 2 if continuation is possible.
        double *relerr,             // Input, the relative error tolerances for the local error test.
        double abserr,              // Input, the absolute error tolerances for the local error test.
        int flag                    // Input, indicator for status of integration.
        )

{
# define MAXNFE 3000

  static double abserr_save = -1.0;
  double ae;
  double dt;
  double ee;
  double eeoet;
  double eps;
  double esttol;
  double et;
  double *f1;
  double *f2;
  double *f3;
  double *f4;
  double *f5;
  static int flag_save = -1000;
  static double h = -1.0;
  bool hfaild;
  double hmin;
  int i;
  static int init = -1000;
  int k;
  static int kflag = -1000;
  static int kop = -1;
  int mflag;
  static int nfe = -1;
  bool output;
  double relerr_min;
  static double relerr_save = -1.0;
  static double remin = 1.0E-12;
  double s;
  double scale;
  double tol;
  double toln;
  double ypk;
//
//  Check the input parameters.
//
  eps = epsilon ( );

  if ( neqn < 1 )
  {
    return 8;
  }

  if ( (*relerr) < 0.0 )
  {
    return 8;
  }

  if ( abserr < 0.0 )
  {
    return 8;
  }

  if ( flag == 0 || 8 < flag  || flag < -2 )
  {
    return 8;
  }

  mflag = abs ( flag );
//
//  Is this a continuation call?
//
  if ( mflag != 1 )
  {
    if ( *t == tout && kflag != 3 )
    {
      return 8;
    }
//
//  FLAG = -2 or +2:
//
    if ( mflag == 2 )
    {
      if ( kflag == 3 )
      {
        flag = flag_save;
        mflag = abs ( flag );
      }
      else if ( init == 0 )
      {
        flag = flag_save;
      }
      else if ( kflag == 4 )
      {
        nfe = 0;
      }
      else if ( kflag == 5 && abserr == 0.0 )
      {
        exit ( 1 );
      }
      else if ( kflag == 6 && (*relerr) <= relerr_save && abserr <= abserr_save )
      {
        exit ( 1 );
      }
    }
//
//  FLAG = 3, 4, 5, 6, 7 or 8.
//
    else
    {
      if ( flag == 3 )
      {
        flag = flag_save;
        if ( kflag == 3 )
        {
          mflag = abs ( flag );
        }
      }
      else if ( flag == 4 )
      {
        nfe = 0;
        flag = flag_save;
        if ( kflag == 3 )
        {
          mflag = abs ( flag );
        }
      }
      else if ( flag == 5 && 0.0 < abserr )
      {
        flag = flag_save;
        if ( kflag == 3 )
        {
          mflag = abs ( flag );
        }
      }
//
//  Integration cannot be continued because the user did not respond to
//  the instructions pertaining to FLAG = 5, 6, 7 or 8.
//
      else
      {
        exit ( 1 );
      }
    }
  }
//
//  Save the input value of FLAG.
//  Set the continuation flag KFLAG for subsequent input checking.
//
  flag_save = flag;
  kflag = 0;
//
//  Save RELERR and ABSERR for checking input on subsequent calls.
//
  relerr_save = (*relerr);
  abserr_save = abserr;
//
//  Restrict the relative error tolerance to be at least
//
//    2*EPS+REMIN
//
//  to avoid limiting precision difficulties arising from impossible
//  accuracy requests.
//
  relerr_min = 2.0 * epsilon ( ) + remin;
//
//  Is the relative error tolerance too small?
//
  if ( (*relerr) < relerr_min )
  {
    (*relerr) = relerr_min;
    kflag = 3;
    return 3;
  }

  dt = tout - *t;
//
//  Initialization:
//
//  Set the initialization completion indicator, INIT;
//  set the indicator for too many output points, KOP;
//  evaluate the initial derivatives
//  set the counter for function evaluations, NFE;
//  estimate the starting stepsize.
//
  f1 = new double[neqn];
  f2 = new double[neqn];
  f3 = new double[neqn];
  f4 = new double[neqn];
  f5 = new double[neqn];

  if ( mflag == 1 )
  {
    init = 0;
    kop = 0;
    f ( *t, y,param, yp );
    nfe = 1;

    if ( *t == tout )
    {
      return 2;
    }

  }

  if ( init == 0 )
  {
    init = 1;
    h = abs ( dt );
    toln = 0.0;

    for ( k = 0; k < neqn; k++ )
    {
      tol = (*relerr) * abs ( y[k] ) + abserr;
      if ( 0.0 < tol )
      {
        toln = tol;
        ypk = abs ( yp[k] );
        if ( tol < ypk * pow ( h, 5 ) )
        {
          h = ( double ) pow ( ( double ) ( tol / ypk ), 0.2 );
        }
      }
    }

    if ( toln <= 0.0 )
    {
      h = 0.0;
    }

    h = max ( h, 26.0 * eps * max ( abs ( *t ), abs ( dt ) ) );

    if ( flag < 0 )
    {
      flag_save = -2;
    }
    else
    {
      flag_save = 2;
    }
  }
//
//  Set stepsize for integration in the direction from T to TOUT.
//
  h = sign ( dt ) * abs ( h );
//
//  Test to see if too may output points are being requested.
//
  if ( 2.0 * abs ( dt ) <= abs ( h ) )
  {
    kop = kop + 1;
  }
//
//  Unnecessary frequency of output.
//
  if ( kop == 100 )
  {
    kop = 0;
    delete [] f1;
    delete [] f2;
    delete [] f3;
    delete [] f4;
    delete [] f5;
    return 7;
  }
//
//  If we are too close to the output point, then simply extrapolate and return.
//
  if ( abs ( dt ) <= 26.0 * eps * abs ( *t ) )
  {
    *t = tout;
    for ( i = 0; i < neqn; i++ )
    {
      y[i] = y[i] + dt * yp[i];
    }
    f ( *t, y, param,yp );
    nfe = nfe + 1;

    delete [] f1;
    delete [] f2;
    delete [] f3;
    delete [] f4;
    delete [] f5;
    return 2;
  }
//
//  Initialize the output point indicator.
//
  output = false;
//
//  To avoid premature underflow in the error tolerance function,
//  scale the error tolerances.
//
  scale = 2.0 / (*relerr);
  ae = scale * abserr;
//
//  Step by step integration.
//
  for ( ; ; )
  {
    hfaild = false;
//
//  Set the smallest allowable stepsize.
//
    hmin = 26.0 * eps * abs ( *t );
//
//  Adjust the stepsize if necessary to hit the output point.
//
//  Look ahead two steps to avoid drastic changes in the stepsize and
//  thus lessen the impact of output points on the code.
//
    dt = tout - *t;

    if ( 2.0 * abs ( h ) <= abs ( dt ) )
    {
    }
    else
//
//  Will the next successful step complete the integration to the output point?
//
    {
      if ( abs ( dt ) <= abs ( h ) )
      {
        output = true;
        h = dt;
      }
      else
      {
        h = 0.5 * dt;
      }

    }
//
//  Here begins the core integrator for taking a single step.
//
//  The tolerances have been scaled to avoid premature underflow in
//  computing the error tolerance function ET.
//  To avoid problems with zero crossings, relative error is measured
//  using the average of the magnitudes of the solution at the
//  beginning and end of a step.
//  The error estimate formula has been grouped to control loss of
//  significance.
//
//  To distinguish the various arguments, H is not permitted
//  to become smaller than 26 units of roundoff in T.
//  Practical limits on the change in the stepsize are enforced to
//  smooth the stepsize selection process and to avoid excessive
//  chattering on problems having discontinuities.
//  To prevent unnecessary failures, the code uses 9/10 the stepsize
//  it estimates will succeed.
//
//  After a step failure, the stepsize is not allowed to increase for
//  the next attempted step.  This makes the code more efficient on
//  problems having discontinuities and more effective in general
//  since local extrapolation is being used and extra caution seems
//  warranted.
//
//  Test the number of derivative function evaluations.
//  If okay, try to advance the integration from T to T+H.
//
    for ( ; ; )
    {
//
//  Have we done too much work?
//
      if ( MAXNFE < nfe )
      {
        kflag = 4;
        delete [] f1;
        delete [] f2;
        delete [] f3;
        delete [] f4;
        delete [] f5;
        return 4;
      }
//
//  Advance an approximate solution over one step of length H.
//
      Step_Integrator ( f, neqn, y, param,*t, h, yp, f1, f2, f3, f4, f5, f1 );
      nfe = nfe + 5;
//
//  Compute and test allowable tolerances versus local error estimates
//  and remove scaling of tolerances.  The relative error is
//  measured with respect to the average of the magnitudes of the
//  solution at the beginning and end of the step.
//
      eeoet = 0.0;

      for ( k = 0; k < neqn; k++ )
      {
        et = abs ( y[k] ) + abs ( f1[k] ) + ae;

        if ( et <= 0.0 )
        {
          delete [] f1;
          delete [] f2;
          delete [] f3;
          delete [] f4;
          delete [] f5;
          return 5;
        }

        ee = abs
        ( ( -2090.0 * yp[k]
          + ( 21970.0 * f3[k] - 15048.0 * f4[k] )
          )
        + ( 22528.0 * f2[k] - 27360.0 * f5[k] )
        );

        eeoet = max ( eeoet, ee / et );

      }

      esttol = abs ( h ) * eeoet * scale / 752400.0;

      if ( esttol <= 1.0 )
      {
        break;
      }
//
//  Unsuccessful step.  Reduce the stepsize, try again.
//  The decrease is limited to a factor of 1/10.
//
      hfaild = true;
      output = false;

      if ( esttol < 59049.0 )
      {
        s = 0.9 / ( double ) pow ( ( double ) esttol, 0.2 );
      }
      else
      {
        s = 0.1;
      }

      h = s * h;

      if ( abs ( h ) < hmin )
      {
        kflag = 6;
        delete [] f1;
        delete [] f2;
        delete [] f3;
        delete [] f4;
        delete [] f5;
        return 6;
      }

    }
//
//  We exited the loop because we took a successful step.
//  Store the solution for T+H, and evaluate the derivative there.
//
    *t = *t + h;
    for ( i = 0; i < neqn; i++ )
    {
      y[i] = f1[i];
    }
    f ( *t, y,param, yp );
    nfe = nfe + 1;
//
//  Choose the next stepsize.  The increase is limited to a factor of 5.
//  If the step failed, the next stepsize is not allowed to increase.
//
    if ( 0.0001889568 < esttol )
    {
      s = 0.9 / ( double ) pow ( ( double ) esttol, 0.2 );
    }
    else
    {
      s = 5.0;
    }

    if ( hfaild )
    {
      s = min ( s, 1.0 );
    }

    h = sign ( h ) * max ( s * abs ( h ), hmin );
//
//  End of core integrator
//
//  Should we take another step?
//
    if ( output )
    {
      *t = tout;
      delete [] f1;
      delete [] f2;
      delete [] f3;
      delete [] f4;
      delete [] f5;
      return 2;
    }

    if ( flag <= 0 )
    {
      delete [] f1;
      delete [] f2;
      delete [] f3;
      delete [] f4;
      delete [] f5;
      return (-2);
    }

  }
# undef MAXNFE
}



void rk4(VectorXd& state,
         int nX,
         double time,
         double tau,
         void (*derivsRK)(VectorXd state, double time, VectorXd param, VectorXd& deriv),
         VectorXd param)
{
    VectorXd F1(nX);
    VectorXd F2(nX);
    VectorXd F3(nX);
    VectorXd F4(nX);
    VectorXd state_temp(nX);

    //* Evaluate F1 = f(x,t).
    (*derivsRK)( state, time, param, F1 );

    //* Evaluate F2 = f( x+tau*F1/2, t+tau/2 ).
    double half_tau = 0.5*tau;
    double t_half = time + half_tau;

    state_temp = state + half_tau*F1;
    (*derivsRK)( state_temp, t_half, param, F2 );

    //* Evaluate F3 = f( x+tau*F2/2, t+tau/2 ).
    state_temp = state + half_tau*F2;
    (*derivsRK)( state_temp, t_half, param, F3 );

    //* Evaluate F4 = f( x+tau*F3, t+tau ).
    double t_full = time + tau;

    state_temp = state + tau*F3;
    (*derivsRK)( state_temp, t_full, param, F4 );

    //* Return x(t+tau) computed from fourth-order R-K.

    state += tau/6.*(F1 + F4 + 2.*(F2 + F3));

}








/*!
   \brief The Adams-Bashforth defining table.

   The table contains the evalution of the integrals over the interpolation polynoms:
     \f[ \beta_{qi} = \int_0^1 \prod_{l=0, l \neq i}^q \frac{s+l}{-i+l} ds, \quad i=0,1,\ldots,q.  \f]
 */
const double adamsbashforth_beta[5][5] =
     { {    1.0/  1.0,    0.0/  1.0,    0.0/  1.0,    0.0/  1.0,    0.0/  1.0},
       {    3.0/  2.0,   -1.0/  2.0,    0.0/  2.0,    0.0/  2.0,    0.0/  2.0},
       {   23.0/ 12.0,  -16.0/ 12.0,    5.0/ 12.0,    0.0/ 12.0,    0.0/ 12.0},
       {   55.0/ 24.0,  -59.0/ 24.0,   37.0/ 24.0,   -9.0/ 24.0,    0.0/ 24.0},
       { 1901.0/720.0,-2774.0/720.0, 2616.0/720.0,-1274.0/720.0,  251.0/720.0} };


int adamsbashforth(
	void f(double t, double y[], double param[], double yp[]),
	double param[], int n, double* t, double y[], double yp[], int steps, double h, int order, int flag,
	adamsbashforth_keep* keep)
{
   // initialize keep on first call
   if (flag == 1)
   {
      keep->initialized = 0;

      // Check number of equations
      if ( n <= 0 ) return -11;
      keep->n = n;

      // Check step width
      if ( h == 0 ) return -12;
      keep->h = h;

      // Check order
      if ( (order < 1) || (order > ADAMSBASHFORTH_MAXORDER) ) return -13;
      keep->order = order;

      // Initialize keep
      keep->f = allocDoubleMatrix(order, n, false);
      if (!keep->f) return -1;
      keep->y = allocDoubleVector(n, false);
      if (!keep->y)
      {
	 freeDoubleMatrix(keep->f, order);
	 return -1;
      }

      // Store initial values
      keep->s = 1;
      keep->si = 0;
      keep->t = *t;
      for (int i=0; i<n; i++)
	 keep->y[i] = y[i];
      f( keep->t, keep->y, param, keep->f[keep->si] );

      keep->initialized = 1;
   }

   // clean up keep
   else if (flag == -1)
   {
      if (keep->initialized == 1)
      {
	 keep->initialized = 0;
	 free(keep->y);
	 freeDoubleMatrix(keep->f, keep->order);
      }
      return 0;
   }

   // invalid call
   else if (flag != 2)
   {
      return -10;
   }

   // Check status of keep
   if (keep->initialized != 1)
      return -2;

   // Use values of keep
   n = keep->n;
   h = keep->h;
   order = keep->order;

   // Main interation
   for (int k=0; k<steps; k++)
   {
      // Call one step method if not enough steps are stored
      if ( keep->s < order)
      {
	 double** ktmp = allocDoubleMatrix(7, n, false);
	 dopristep(f, NULL, 1, param, keep->t, h, n, keep->y, keep->f[keep->si],
			 keep->y, ktmp[0], 0, ktmp[1], ktmp[2], ktmp[3], ktmp[4], ktmp[5], ktmp[6]);
	 freeDoubleMatrix(ktmp, 7);
	 keep->t += h;
	 keep->s++;
	 keep->si++;
	 f( keep->t, keep->y, param, keep->f[keep->si] );
      }

      // Multi-step method
      else
      {
	 int stmp = keep->si; // Start with latest step
	 for (int p=0; p<order; p++ )
	 {
	    double factor = h * adamsbashforth_beta[order-1][p];
	    for (int i=0; i<n; i++)
	       keep->y[i] += factor * keep->f[stmp][i];
	    stmp--;
	    if (stmp < 0) stmp = order-1;
	 }
	 keep->t += h;
	 keep->si++;
	 if (keep->si >= order) keep->si = 0;
	 f( keep->t, keep->y, param, keep->f[keep->si] );
      }
   }

   // return (copy latest step into output variables)
   *t = keep->t;
   for (int i=0; i<n; i++)
      y[i] = keep->y[i];
   for (int i=0; i<n; i++)
      yp[i] = keep->f[keep->si][i];

   return 0;
}






// The table contains the evalution of the integrals over the interpolation polynoms:
//     \f[ \beta_{qi} = \int_{-1}^0 \prod_{l=0, l \neq i}^q \frac{s+l}{-i+l} ds, \quad i=0,1,\ldots,q.  \f]
const double adamsmoulton_beta[5][5] =
     { {    1.0/  1.0,    0.0/  1.0,    0.0/  1.0,    0.0/  1.0,    0.0/  1.0},
       {    1.0/  2.0,    1.0/  2.0,    0.0/  2.0,    0.0/  2.0,    0.0/  2.0},
       {    5.0/ 12.0,    8.0/ 12.0,   -1.0/ 12.0,    0.0/ 12.0,    0.0/ 12.0},
       {    9.0/ 24.0,   19.0/ 24.0,   -5.0/ 24.0,    1.0/ 24.0,    0.0/ 24.0},
       {  251.0/720.0,  646.0/720.0, -264.0/720.0,  106.0/720.0,  -19.0/720.0} };



int adamsmoulton(
	void f(double t, double y[], double param[], double yp[]),
	double param[], int n, double* t, double y[], double yp[], int steps, double h, int order,
	double iterTol, int iterMax,
	int flag, adamsmoulton_keep* keep)
{
   // initialize keep on first call
   if (flag == 1)
   {
      keep->initialized = 0;

      // Check number of equations
      if ( n <= 0 ) return -11;
      keep->n = n;

      // Check step width
      if ( h == 0 ) return -12;
      keep->h = h;

      // Check order
      if ( (order < 1) || (order > ADAMSMOULTON_MAXORDER) ) return -13;
      keep->order = order;

      // Initialize keep
      keep->f = allocDoubleMatrix(order, n, false);
      if (!keep->f) return -1;
      keep->y = allocDoubleVector(n, false);
      if (!keep->y)
      {
	 freeDoubleMatrix(keep->f, order);
	 return -1;
      }
      keep->yold = allocDoubleVector(n, false);
      if (!keep->yold)
      {
	 freeDoubleMatrix(keep->f, order);
	 free(keep->y);
	 return -1;
      }
      keep->yhelp = allocDoubleVector(n, false);
      if (!keep->yhelp)
      {
	 freeDoubleMatrix(keep->f, order);
	 free(keep->y);
	 free(keep->yold);
	 return -1;
      }

      // Store initial values
      keep->s = 1;
      keep->si = 0;
      keep->t = *t;
      for (int i=0; i<n; i++)
	 keep->y[i] = y[i];
      f( keep->t, keep->y, param, keep->f[keep->si] );

      keep->initialized = 1;
   }

   // clean up keep
   else if (flag == -1)
   {
      if (keep->initialized == 1)
      {
	 keep->initialized = 0;
	 free(keep->yhelp);
	 free(keep->yold);
	 free(keep->y);
	 freeDoubleMatrix(keep->f, keep->order);
      }
      return 0;
   }

   // invalid call
   else if (flag != 2)
   {
      return -10;
   }

   // Check status of keep
   if (keep->initialized != 1)
      return -2;

   int ires = 0;

   // Use values of keep
   n = keep->n;
   h = keep->h;
   order = keep->order;

   // Main interation
   for (int k=0; k<steps; k++)
   {
      // Call one step method if not enough steps are stored (here: order-1 steps)
      if ( keep->s < order-1 )
      {
	 double** ktmp = allocDoubleMatrix(7, n, false);
	 dopristep(f, NULL, 1, param, keep->t, h, n, keep->y, keep->f[keep->si],
			 keep->y, ktmp[0], 0, ktmp[1], ktmp[2], ktmp[3], ktmp[4], ktmp[5], ktmp[6]);
	 freeDoubleMatrix(ktmp, 7);
	 keep->t += h;
	 keep->s++;
	 keep->si++;
	 f( keep->t, keep->y, param, keep->f[keep->si] );
      }

      // implicit Multi-step method
      else
      {
	 /*! \todo implement predictor step, e.g. Adams-Bashforth */
	 for (int i=0; i<n; i++ )
	    keep->yold[i] = keep->y[i];

	 // constant offset
	 for (int i=0; i<n; i++ )
	    keep->yhelp[i] = keep->y[i];
	 int stmp = keep->si;
	 for (int p=1; p<order; p++ )
	 {
	    double factor = h * adamsmoulton_beta[order-1][p];
	    for (int i=0; i<n; i++)
	       keep->yhelp[i] += factor * keep->f[stmp][i];
	    stmp--;
	    if (stmp < 0) stmp = order-1;
	 }

	 // Fixpoint iteration
	 double factor = h * adamsmoulton_beta[order-1][0];
	 int iter = 0;
	 double nrmres = 1e+10;
	 stmp = keep->si+1; // overwrite oldest step#
	 if (stmp >= order) stmp = 0;
	 do
	 {
	    iter++;
	    if (iter > iterMax) break;

	    f( keep->t+h, keep->y, param, keep->f[stmp] );

	    nrmres = 0.0;
	    for (int i=0; i<n; i++ )
	       nrmres = fmax(fabs(keep->yhelp[i] + factor*keep->f[stmp][i] - keep->y[i]), nrmres);
	    if (nrmres <= iterTol) break;

	    for (int i=0; i<n; i++ )
	       keep->y[i] = keep->yhelp[i] + factor*keep->f[stmp][i];
	 }
	 while (1);
	 if (nrmres > iterTol)
	 {
	    // Resore old state
	    for (int i=0; i<n; i++ )
	       keep->y[i] = keep->yold[i];
	    ires = -20;
	 }
	 else
	 {
	    // Propagate
	    keep->si++;
	    if (keep->si >= order) keep->si = 0;
	    keep->t += h;
	 }
      }
   }

   // return (copy latest step into output variables)
   *t = keep->t;
   for (int i=0; i<n; i++)
      y[i] = keep->y[i];
   for (int i=0; i<n; i++)
      yp[i] = keep->f[keep->si][i];
   return ires;
}




int extrapol(
	void f(double t, double y[], double param[], double yp[]),
	double param[], int n, double* t, double y[], double yp[], int steps, double h, int order, int flag,
	extrapol_keep* keep)
{
  int ietajm1,ietaj,ietajp1;
  double hp,tj,tmp;
  // initialize keep on first call
  if (flag == 1)
    {
      keep->initialized = 0;

      // Check number of equations
      if ( n <= 0 ) return -11;
      keep->n = n;

      // Check step width
      if ( h == 0 ) return -12;
      keep->h = h;

      // Check order
      if ( order < 0 ) return -13;
      keep->order = order;

      // Initialize y
      keep->y = allocDoubleVector(n, false);
      if (!keep->y) return -1;

      // Initialize S
      keep->S = allocDoubleMatrix(order+1, n, false);
      if (!keep->S)
	{
	  free(keep->y);
	  return -1;
	}

      // Initialize ni with Bulirsch sequence {2,4,6,8,12,16,...}, n_i = 2*n_i-2, i>=3
      keep->ni = allocDoubleVector(order+1, false);
      if (!keep->ni)
	{
	  free(keep->y);
	  freeDoubleMatrix(keep->S, order+1);
	  return -1;
	}
      keep->ni[0] = 2.0;
      if (order > 0) keep->ni[1] = 4.0;
      if (order > 1) keep->ni[2] = 6.0;
      for (int i=3; i<=order; i++)
	keep->ni[i] = 2.0*keep->ni[i-2];

      // Initialize temp
      keep->temp = allocDoubleMatrix(3, n, false);
      if (!keep->temp)
	{
	  free(keep->y);
	  freeDoubleMatrix(keep->S, order+1);
	  free(keep->ni);
	  return -1;
	}

      // Store initial values
      keep->t = *t;
      for (int i=0; i<n; i++)
	 keep->y[i] = y[i];

      keep->initialized = 1;
   }

   // clean up keep
   else if (flag == -1)
   {
      if (keep->initialized == 1)
      {
	 keep->initialized = 0;
	 free(keep->y);
	 freeDoubleMatrix(keep->S, keep->order+1);
	 free(keep->ni);
	 freeDoubleMatrix(keep->temp, 3);
      }
      return 0;
   }

   // invalid call
   else if (flag != 2)
   {
      return -10;
   }

   // Check status of keep
   if (keep->initialized != 1)
      return -2;

   // Use values of keep
   n     = keep->n;
   h     = keep->h;
   order = keep->order;

   // Main interation: perform steps integration steps
   for (int m=0; m<steps; m++)
     {

       // Extrapolation method
       for (int p=0; p <= order; p++)
	 {
	   // define extrapolation step size and time
	   hp = h/keep->ni[p];
	   tj = keep->t;

	   // Compute Euler step
	   f(tj,keep->y,param,yp);
	   for (int i=0; i<n; i++)
	     {
	       keep->temp[0][i] = keep->y[i];
	       keep->temp[1][i] = keep->y[i] + hp*yp[i];
	     }
	   ietajm1 = 0;
	   ietaj   = 1;
	   ietajp1 = 2;

	   // Compute approximations with midpoint-rule
	   for (int j=1; j<keep->ni[p]; j++)
	     {
	       tj += hp;
	       f(tj,keep->temp[ietaj],param,yp);
	       for (int i=0; i<n; i++)
		 keep->temp[ietajp1][i] = keep->temp[ietajm1][i] + 2.0*hp*yp[i];
	       ietajm1++;
	       if (ietajm1 > 2) ietajm1 = 0;
	       ietaj++;
	       if (ietaj > 2) ietaj = 0;
	       ietajp1++;
	       if (ietajp1 > 2) ietajp1 = 0;
	     }
	   // Compute Gragg's function
	   tj = keep->t + h;
	   f(tj,keep->temp[ietaj],param,yp);
	   for (int i=0; i<n; i++)
	     keep->S[p][i] = (keep->temp[ietaj][i] + keep->temp[ietajm1][i] + hp*yp[i])/2.0;
	 }

       // Start extrapolation steps
       for (int k=1; k<=order; k++)
	 {
	   for (int p=order; p>=k; p--)
	     {
	       tmp = keep->ni[p]/keep->ni[p-k];
	       for (int i=0; i<n; i++)
		 keep->S[p][i] = keep->S[p][i] + (keep->S[p][i] - keep->S[p-1][i])/(tmp*tmp-1.0);
	     }
	 }
       for (int i=0; i<n; i++)
	 keep->y[i] = keep->S[order][i];
       keep->t += h;
     }

   // return (copy latest step into output variables and evaluate derivative)
   *t = keep->t;
   for (int i=0; i<n; i++)
     y[i] = keep->y[i];
   f(keep->t,keep->y,param,yp);

   return 0;
}



// The defining tables are indexed in the form of a[order][i][j].
const double nystrom_a[4][4][4] =
   { { {  0.0/ 1.0,  0.0/ 1.0,  0.0/ 1.0,  0.0/ 1.0},
       {  0.0/ 1.0,  0.0/ 1.0,  0.0/ 1.0,  0.0/ 1.0},
       {  0.0/ 1.0,  0.0/ 1.0,  0.0/ 1.0,  0.0/ 1.0},
       {  0.0/ 1.0,  0.0/ 1.0,  0.0/ 1.0,  0.0/ 1.0} },
     { {  0.0/ 1.0,  0.0/ 1.0,  0.0/ 1.0,  0.0/ 1.0},
       {  0.0/ 1.0,  0.0/ 1.0,  0.0/ 1.0,  0.0/ 1.0},
       {  0.0/ 1.0,  0.0/ 1.0,  0.0/ 1.0,  0.0/ 1.0},
       {  0.0/ 1.0,  0.0/ 1.0,  0.0/ 1.0,  0.0/ 1.0} },
     { {  0.0/ 1.0,  0.0/ 1.0,  0.0/ 1.0,  0.0/ 1.0},
       {  1.0/ 8.0,  0.0/ 1.0,  0.0/ 1.0,  0.0/ 1.0},
       {  0.0/ 1.0,  1.0/ 2.0,  0.0/ 1.0,  0.0/ 1.0},
       {  0.0/ 1.0,  0.0/ 1.0,  0.0/ 1.0,  0.0/ 1.0} },
     { {  0.0/ 1.0,  0.0/ 1.0,  0.0/ 1.0,  0.0/ 1.0},
       {  1.0/50.0,  0.0/ 1.0,  0.0/ 1.0,  0.0/ 1.0},
       { -1.0/27.0,  7.0/27.0,  0.0/ 1.0,  0.0/ 1.0},
       {  3.0/10.0, -2.0/35.0,  9.0/35.0,  0.0/ 1.0} } };

const double nystrom_c[4][4] =
     { {  0.0/ 1.0,  0.0/ 1.0,  0.0/ 1.0,  0.0/ 1.0},
       {  0.0/ 1.0,  0.0/ 1.0,  0.0/ 1.0,  0.0/ 1.0},
       {  0.0/ 1.0,  1.0/ 2.0,  1.0/ 1.0,  0.0/ 1.0},
       {  0.0/ 1.0,  1.0/ 5.0,  2.0/ 3.0,  1.0/ 1.0} };

const double nystrom_bp[4][4] =
     { {   0.0/  1.0,   0.0/  1.0,   0.0/  1.0,   0.0/  1.0},
       {   0.0/  1.0,   0.0/  1.0,   0.0/  1.0,   0.0/  1.0},
       {   1.0/  6.0,   1.0/  3.0,   0.0/  1.0,   0.0/  1.0},
       {  14.0/336.0, 100.0/336.0,  54.0/336.0,   0.0/  1.0} };

const double nystrom_b[4][4] =
     { {   0.0/  1.0,   0.0/  1.0,   0.0/  1.0,   0.0/  1.0},
       {   0.0/  1.0,   0.0/  1.0,   0.0/  1.0,   0.0/  1.0},
       {   1.0/  6.0,   4.0/  6.0,   1.0/  6.0,   0.0/  1.0},
       {  14.0/336.0, 125.0/336.0, 162.0/336.0,  35.0/336.0} };


int nystrom(
	void f(double t, double y[], double param[], double ypp[]),
	double param[], int n, double* t, double y[], double yp[], int steps, double h, int order, int flag,
	nystrom_keep* keep)
{
   // initialize keep on first call
   if (flag == 1)
   {
      keep->initialized = 0;

      // Check number of equations
      if ( n <= 0 ) return -11;
      keep->n = n;

      // Check order
      if ( (order < NYSTROM_MINORDER) || (order > NYSTROM_MAXORDER) ) return -13;
      keep->order = order;

      // Initialize keep
      keep->k = allocDoubleMatrix(order, n, false);
      if (!keep->k) return -1;
      keep->yak = allocDoubleVector(n, false);
      if (!keep->yak)
      {
	 freeDoubleMatrix(keep->k, order);
	 return -1;
      }

      keep->initialized = 1;
   }
   // clean up keep
   else if (flag == -1)
   {
      if (keep->initialized == 1)
      {
	 keep->initialized = 0;
	 free(keep->yak);
	 freeDoubleMatrix(keep->k, keep->order);

      }
      return 0;
   }
   // invalid call
   else if (flag != 2)
   {
      return -10;
   }

   // Check status of keep
   if (keep->initialized != 1)
      return -2;


   // Use values of keep
   n = keep->n;
   order = keep->order-1;

   // Check h
   if (h == 0) return -12;

   // Main interation
   for (int m=0; m<steps; m++)
   {
		for (int i=0; i<=order; i++)
		{
			for (int yakn=0; yakn<n; yakn++)
			{
				keep->yak[yakn] = 0;
				for (int j=0; j<i; j++)
				{
					keep->yak[yakn] += nystrom_a[order][i][j] * keep->k[j][yakn];
				}
				keep->yak[yakn] *= h*h;
				keep->yak[yakn] += y[yakn] + nystrom_c[order][i]*h*yp[yakn];
			}
			f(*t + nystrom_c[order][i]*h, keep->yak, param, keep->k[i]);
		}
		// Calculate y[i], yp[i], i=0,...,s-1
		for (int i=0; i<n; i++)
		{
			double bpksum = 0.0;
			double bksum = 0.0;
			for (int j=0; j<=order; j++)
			{
				bpksum += nystrom_bp[order][j]*keep->k[j][i];
				bksum += nystrom_b[order][j]*keep->k[j][i];
			}
			y[i] += h*yp[i] + h*h*bpksum;
			yp[i] += h*bksum;
		}
	*t += h;
   }

   return 0;
}



int rk(
	void f(double t, double y[], double param[], double yp[]),
	double param[], int n, double* t, double y[], double yp[], int steps, double h,
	integrator_flag method, int flag, rk_keep* keep)
{
   // initialize keep on first call
   if (flag == 1)
   {
      keep->initialized = 0;

      // Check number of equations
      if ( n <= 0 ) return -11;
      keep->n = n;

      // Save method
      keep->method = method;
      if (    (keep->method != RK_DOPRI4)
	   && (keep->method != RK_DOPRI5)
	   && (keep->method != RK_RK2)
	   && (keep->method != RK_RK3)
	   && (keep->method != RK_RK4)
	   && (keep->method != RK_RK5)
	   && (keep->method != RK_RK7)
	   && (keep->method != RK_RK8) )
	return -9;

      // Initialize keep
      keep->y = allocDoubleVector(n, false);
      if (!keep->y) return -1;
      keep->dy = allocDoubleVector(n, false);
      if (!keep->dy)
      {
	 free(keep->y);
	 return -1;
      }
      keep->tmp = allocDoubleVector(n, false);
      if (!keep->tmp)
      {
	 free(keep->y);
	 free(keep->dy);
	 return -1;
      }
      switch (method)
      {
	 case RK_DOPRI4:
	 case RK_DOPRI5:
	    keep->ksize = 6;
	    break;
	 case RK_RK2:
	 case RK_RK3:
	    keep->ksize = 4;
	    break;
	 case RK_RK4:
	 case RK_RK5:
	    keep->ksize = 5;
	    break;
	 case RK_RK7:
	 case RK_RK8:
	    keep->ksize = 11;
	    break;
	 default:
	    return -9;
	    break;
      }
      keep->k = allocDoubleMatrix(keep->ksize, n, false);
      if (!keep->k)
      {
	 free(keep->y);
	 free(keep->dy);
	 free(keep->tmp);
	 return -1;
      }

      // Store initial values and evaluate f
      keep->t = *t;
      for (int i=0; i<n; i++)
	 keep->y[i] = y[i];
      f( keep->t, keep->y, param, keep->dy );
      keep->initialized = 1;
   }

   // clean up keep
   else if (flag == -1)
   {
      if (keep->initialized == 1)
      {
	 keep->initialized = 0;
	 free(keep->y);
	 free(keep->dy);
	 free(keep->tmp);
	 freeDoubleMatrix(keep->k, keep->ksize);
      }
      return 0;
   }

   // invalid call
   else if (flag != 2)
   {
      return -10;
   }

   // Check status of keep
   if (keep->initialized != 1)
      return -2;

   // Check step width
   if ( h == 0 ) return -12;

   // Use values of keep
   n = keep->n;
   method = keep->method;

   // Main interation
   for (int k=0; k<steps; k++)
   {
      switch (method)
      {
	 case RK_DOPRI4:
	    dopristep(f, NULL, 1, param, keep->t, h, n,
			    keep->y, keep->dy, keep->tmp, keep->y, 0,
			    keep->k[0], keep->k[1], keep->k[2], keep->k[3],
			    keep->k[4], keep->k[5]);
	    break;
	 case RK_DOPRI5:
	    dopristep(f, NULL, 1, param, keep->t, h, n,
			    keep->y, keep->dy, keep->y, keep->tmp, 0,
			    keep->k[0], keep->k[1], keep->k[2], keep->k[3],
			    keep->k[4], keep->k[5]);
	    break;
	 case RK_RK2:
	    rkf23step(f, NULL, 1, param, keep->t, h, n,
			    keep->y, keep->dy, keep->y, keep->tmp, 0,
			    keep->k[0], keep->k[1], keep->k[2], keep->k[3]);
	    break;
	 case RK_RK3:
	    rkf23step(f, NULL, 1, param, keep->t, h, n,
			    keep->y, keep->dy, keep->tmp, keep->y, 0,
			    keep->k[0], keep->k[1], keep->k[2], keep->k[3]);
	    break;
	 case RK_RK4:
	    rkf45step(f, NULL, 1, param, keep->t, h, n,
			    keep->y, keep->dy, keep->y, keep->tmp, 0,
			    keep->k[0], keep->k[1], keep->k[2], keep->k[3],
			    keep->k[4]);
	    break;
	 case RK_RK5:
	    rkf45step(f, NULL, 1, param, keep->t, h, n,
			    keep->y, keep->dy, keep->tmp, keep->y, 0,
			    keep->k[0], keep->k[1], keep->k[2], keep->k[3],
			    keep->k[4]);
	    break;
	 case RK_RK7:
	    rkf78step(f, NULL, 1, param, keep->t, h, n,
			    keep->y, keep->dy, keep->y, keep->tmp, 0,
			    keep->k[0], keep->k[1], keep->k[2], keep->k[3],
			    keep->k[4], keep->k[5], keep->k[6], keep->k[7],
			    keep->k[8], keep->k[9], keep->k[10]);
	    break;
	 case RK_RK8:
	    rkf78step(f, NULL, 1, param, keep->t, h, n,
			    keep->y, keep->dy, keep->tmp, keep->y, 0,
			    keep->k[0], keep->k[1], keep->k[2], keep->k[3],
			    keep->k[4], keep->k[5], keep->k[6], keep->k[7],
			    keep->k[8], keep->k[9], keep->k[10]);
	    break;
	 default:
	    return -9;
	    break;
      }
      keep->t += h;
      f( keep->t, keep->y, param, keep->dy );
   }

   // return (copy latest step into output variables)
   *t = keep->t;
   for (int i=0; i<n; i++)
      y[i] = keep->y[i];
   for (int i=0; i<n; i++)
      yp[i] = keep->dy[i];
   return 0;
}



int rkse(
    void f(double t, double y[], double param[], double yp[]),
    void fjac(double t, double y[], double param[], double **jac),
    int np, double param[], int n, double* t, double tout, double y[], double yp[],
    double rtol[],double atol[], int info[],
    integrator_flag method, int flag, rkse_keep* keep)
{
    int dir,maxit,i,icount,nx,iend,ijac;
    double alpha,amax,amin,hmin,h,hneu,sk,ex,hilf,err;


    // initialize keep on first call
    if (flag == 1)
    {
	keep->initialized = 0;

	// Save method
	keep->method = method;
   if (    (keep->method != RKSE_DOPRI54)
	&& (keep->method != RKSE_RK23)
	&& (keep->method != RKSE_RK45)
	&& (keep->method != RKSE_RK78) )
	return -9;

	// Initialize keep
	keep->solq = allocDoubleVector(n, false);
	if (!keep->solq) return -1;
	keep->solp = allocDoubleVector(n, false);
	if (!keep->solp)
	{
	    free(keep->solq);
	    return -1;
	}
	switch (method)
	{
	    case RKSE_DOPRI54:
		keep->ksize = 6;
		keep->p=5;
		break;
	    case RKSE_RK23:
		keep->ksize = 4;
		keep->p=2;
		break;
	    case RKSE_RK45:
		keep->ksize = 5;
		keep->p=4;
		break;
	    case RKSE_RK78:
		keep->ksize = 11;
		keep->p=7;
       break;
      default:
       return -9;
       break;
	}
	keep->k = allocDoubleMatrix(keep->ksize, n, false);
	if (!keep->k)
	{
	    free(keep->solq);
	    free(keep->solp);
	    return -1;
	}

	// Store initial values
	keep->initialized = 1;
   return 0;
    }

    // clean up keep
    else if (flag == -1)
    {
	if (keep->initialized == 1)
	{
	    keep->initialized = 0;
	    free(keep->solq);
	    free(keep->solp);
	    freeDoubleMatrix(keep->k, keep->ksize);
	}
	return 0;
    }

    // invalid call
    else if (flag != 2)
    {
	return -10;
    }

    // Check status of keep
    if (keep->initialized != 1)
	return -7;

    // Check step width ! Warning: h is not initialized here
    // if ( h == 0 ) return -12;

    // Main interation

    // Check input data
    if (n <= 0)
    {
	printf("Error in dimensions: n <= 0!");
	return -3;
    }
    if (np < 0)
    {
	printf("Error in dimensions: np < 0!");
	return -3;
    }

    icount=0;
    for (int i=0; i<9; i++)
    {
	if ( i != 1 && (info[i] < 0 || info[i] > 1))
	{
	    printf("Error in input data: info(%d)<0 or info(%d)>1!",i,i);
	    icount=1;
	}
	if ( i == 1 && info[i] < 0)
	{
	    printf("Error in input data: info(%d)<0 or info(%d)>1!",i,i);
	    icount=1;
	}
    }
    if (icount == 1) return -3;

    // maximum number of iteration steps
    if ( info[1] == 0 )
	maxit = 10000;
    else
	maxit = info[1];

    // security factor (usually: alpha=0.8,0.9,0.25^1/(p+1))
    if ( info[2] == 1 )
	alpha = param[np];
    else
	alpha = 0.8;

    // minimal and maximal step size factor (usually: 0.2<=amin<=0.5,1.5<=amax<=5)
    if ( info[3] == 1 )
	amin = param[np+1];
    else
	amin = 0.2;
    if ( info[4] == 1 )
	amax = param[np+2];
    else
	amax = 1.5;

    // machine precision, minimal step length and initial step length
    if ( info[5] == 1 )
	hmin = param[np+3];
    else
	hmin = SQRTEPS;
    if ( info[6] == 1 )
	h = param[np+4];
    else
	h = fmin( 1.0e-2, fabs(tout-*t) );

    // include sensitivities in error control?
    nx = n/(np+1);
    if ( info[7] == 1 )
	iend = n;
    else
	iend = nx;

    // t and tout too close?
    if (fabs(*t-tout) <= hmin) {
	printf("Error: abs(t-tout)<=hmin!");
	return -6;
    }

    // direction of integration
    if (tout > *t)
	dir=1;
    else {
	dir=0;
	h = -h;
    }

    // calculation of jacobi matrix
    ijac = info[8];

    // derivative of x at t
    f ( *t, y, param, yp );
    if ( np > 0 ) sens(f,fjac,ijac,param,n,np,*t,y,&yp[nx]);

    // main loop
    while (icount <= maxit)
    {
	icount++;
	if ( *t+h >= tout && dir)
	    h = tout-*t;
	else if (*t+h <= tout && (!dir))
	    h = tout-*t;
	if ( fabs(h) <= hmin) return 1;

	// RK step from t to t+h
	switch (keep->method)
	{
	    case RKSE_DOPRI54:
		dopristep(f, fjac, ijac, param, *t, h, n,
				y, yp, keep->solp, keep->solq, np,
				keep->k[0], keep->k[1], keep->k[2], keep->k[3],
				keep->k[4], keep->k[5]);
		break;
	    case RKSE_RK23:
		rkf23step(f, fjac, ijac, param, *t, h, n,
				y, yp, keep->solp, keep->solq, np,
				keep->k[0], keep->k[1], keep->k[2], keep->k[3]);
		break;
	    case RKSE_RK45:
		rkf45step(f, fjac, ijac, param, *t, h, n,
				y, yp, keep->solp, keep->solq, np,
				keep->k[0], keep->k[1], keep->k[2], keep->k[3],
				keep->k[4]);
		break;
	    case RKSE_RK78:
		rkf78step(f, fjac, ijac, param, *t, h, n,
				y, yp, keep->solp, keep->solq, np,
				keep->k[0], keep->k[1], keep->k[2], keep->k[3],
				keep->k[4], keep->k[5], keep->k[6], keep->k[7],
				keep->k[8], keep->k[9], keep->k[10]);
		break;
       default:
	 return -9;
	 break;
	}

	// error estimation err=max_i |up_i-uq_i |/sk_i
	sk = atol[0]+fmax( fabs(keep->solp[0]),fabs(y[0]))*rtol[0];
	err = fabs(keep->solp[0]-keep->solq[0]);
	err = err/sk;
	for (i=1;i<iend;i++)
	{
	    if ( info[0] == 1 )
		sk = atol[i]+fmax( fabs(keep->solp[i]),fabs(y[i]))*rtol[i];
	    else
		sk = atol[0]+fmax( fabs(keep->solp[i]),fabs(y[i]))*rtol[0];
	    hilf = fabs(keep->solp[i]-keep->solq[i]);
	    hilf = hilf/sk;
	    if ( hilf > err ) err = hilf;
	}

	// exponent
	ex = 1.0/(1.0+(double)(keep->p));

	// new step size
	if (err == 0.0)
	    hneu = amax*h;
	else
	    hneu = fmin(amax,fmax(amin,alpha*pow((1.0/err),ex)))*h;

	// step size too small
	if ( fabs(hneu) < hmin )
	{
	    printf("New stepsize hneu less than minimal stepsize! Integration stopped!");
	    return -4;
	}
	if ( err <= 1.0 )
	{
	    // accept integration step
	    *t = *t + h;
	    for (i=0;i<n;i++)
		y[i] = keep->solp[i];
	    if ( fabs(*t-tout) <= EPS ) return 0;
	    h = hneu;
	    f ( *t, y, param, yp );
	    if ( np > 0 ) sens(f,fjac,ijac,param,n,np,*t,y,&yp[nx]);
	} else
	    // do not accept integration step
	    h = hneu;
    }
    if ( icount > maxit )
    {
	printf("maximum number of integration steps reached!\n");
	printf("T=%lf11.4\n",*t);
	printf("Y=\n");
	for (i=0;i<n;i++)
	    printf("%lf30.16\n",y[i]);
	printf("Integration stopped!");
	return -5;
    }

    return 0;
}









//////////////////////////////// Defining the control of the ingetrators //////////////////////////
int integrator_defaults( integrator_options* options, int n )
{
   options->order = 4;
   options->iterTol = 1e-10;
   options->iterMax = 20;
   options->atol = allocDoubleVector(n, false);
   if (!options->atol) return -1;
   for (int i=0; i<n; i++)
      options->atol[i] = 1e-3;
   options->rtol = allocDoubleVector(n, false);
   if (!options->rtol)
   {
      free(options->atol);
      return -1;
   }
   for (int i=0; i<n; i++)
      options->rtol[i] = 1e-3;
   options->rkse_info[0] = 1;
   options->rkse_info[1] = 0;
   options->rkse_info[2] = 0;
   options->rkse_info[3] = 0;
   options->rkse_info[4] = 0;
   options->rkse_info[5] = 0;
   options->rkse_info[6] = 0;
   options->rkse_info[7] = 0;
   options->rkse_info[8] = 0;
   return 0;
}

void integrator_options_cleanup( integrator_options* options )
{
   free(options->atol);
   free(options->rtol);
}

int integrate(
	integrator_flag iflag,
	void f(double t, double y[], double param[], double yp[]),
	double param[], int n, double* t, double y[], double yp[], int steps, double h, int flag,
	integrator_options* options, void** keep)
{
   int returnvalue;
   switch (iflag)
   {
   case ADAMSBASHFORTH:
      if (flag == 1)
      {
	 *keep = malloc(sizeof(adamsbashforth_keep));
	 if (!*keep) return -1;
      }
      returnvalue = 10*adamsbashforth(f, param, n, t, y, yp, steps, h, options->order, flag, (adamsbashforth_keep*) *keep);
      break;

   case ADAMSMOULTON:
      if (flag == 1)
      {
	 *keep = malloc(sizeof(adamsmoulton_keep));
	 if (!*keep) return -1;
      }
      returnvalue = 10*adamsmoulton(f, param, n, t, y, yp, steps, h, options->order, options->iterTol, options->iterMax, flag, (adamsmoulton_keep*) *keep);
      break;

   case EXTRAPOL:
      if (flag == 1)
      {
	 *keep = malloc(sizeof(extrapol_keep));
	 if (!*keep) return -1;
      }
      returnvalue = 10*extrapol(f, param, n, t, y, yp, steps, h, options->order, flag, (extrapol_keep*) *keep);
      break;

   case NYSTROM:
      if (flag == 1)
      {
	 *keep = malloc(sizeof(nystrom_keep));
	 if (!*keep) return -1;
      }
      returnvalue = 10*nystrom(f, param, n, t, y, yp, steps, h, options->order, flag, (nystrom_keep*) *keep);
      break;

   case RK_DOPRI4:
   case RK_DOPRI5:
   case RK_RK2:
   case RK_RK3:
   case RK_RK4:
   case RK_RK5:
   case RK_RK7:
   case RK_RK8:
      if (flag == 1)
      {
	 *keep = malloc(sizeof(rk_keep));
	 if (!*keep) return -1;
      }
      returnvalue = 10*rk(f, param, n, t, y, yp, steps, h, iflag, flag, (rk_keep*) *keep);
      break;

   case RKSE_DOPRI54:
   case RKSE_RK23:
   case RKSE_RK45:
   case RKSE_RK78:
      if (flag == 1)
      {
	 *keep = malloc(sizeof(rkse_keep));
	 if (!*keep) return -1;
      }
      double tout = 0.0;
      if (t) tout = *t+steps*h;
      returnvalue = 10*rkse(f, NULL, 0, param, n, t, tout, y, yp, options->rtol, options->atol, options->rkse_info, iflag, flag, (rkse_keep*) *keep);
      if (returnvalue > 0) returnvalue = 0;

      break;
   }

   if (flag == -1) free(*keep);
   return returnvalue;
}

