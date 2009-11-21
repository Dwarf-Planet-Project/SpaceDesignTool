/*
 File name: eode.cpp
 This program is free software; you can redistribute it and/or modify it under
 the terms of the GNU Lesser General Public License as published by the Free Software
 Foundation; either version 2 of the License, or (at your option) any later
 version.

 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more
 details.

 You should have received a copy of the GNU Lesser General Public License along with
 this program; if not, write to the Free Software Foundation, Inc., 59 Temple
 Place - Suite 330, Boston, MA 02111-1307, USA.
 Further information about the GNU Lesser General Public License can also be found on
 the world wide web at http://www.gnu.org.
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
*/

#include <stdlib.h>
#include <string.h>
#include <cmath>

//#include <Eigen/Core>
#include "arithmetics.h"
#include "eode.h"

#include <algorithm>

using namespace std;
// import most common Eigen types
//USING_PART_OF_NAMESPACE_EIGEN



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





///////////////////////////////////////  Defining the Integrator ////////////////////////////////////////

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
