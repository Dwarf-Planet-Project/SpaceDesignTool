/*
 File name: eode.h
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

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <Eigen/Core>

USING_PART_OF_NAMESPACE_EIGEN


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
        );

/*

  Purpose:

    R4_RKF45 carries out the Runge-Kutta-Fehlberg method.

  Discussion:

    This version of the routine uses double real arithmetic.

    This routine is primarily designed to solve non-stiff and mildly stiff
    differential equations when derivative evaluations are inexpensive.
    It should generally not be used when the user is demanding
    high accuracy.

    This routine integrates a system of NEQN first-order ordinary differential
    equations of the form:

      dY(i)/dT = F(T,Y(1),Y(2),...,Y(NEQN))

    where the Y(1:NEQN) are given at T.

    Typically the subroutine is used to integrate from T to TOUT but it
    can be used as a one-step integrator to advance the solution a
    single step in the direction of TOUT.  On return, the parameters in
    the call list are set for continuing the integration.  The user has
    only to call again (and perhaps define a new value for TOUT).

    Before the first call, the user must

    * supply the subroutine F(T,Y,YP) to evaluate the right hand side;
      and declare F in an EXTERNAL statement;

    * initialize the parameters:
      NEQN, Y(1:NEQN), T, TOUT, RELERR, ABSERR, FLAG.
      In particular, T should initially be the starting point for integration,
      Y should be the value of the initial conditions, and FLAG should
      normally be +1.

    Normally, the user only sets the value of FLAG before the first call, and
    thereafter, the program manages the value.  On the first call, FLAG should
    normally be +1 (or -1 for single step mode.)  On normal return, FLAG will
    have been reset by the program to the value of 2 (or -2 in single
    step mode), and the user can continue to call the routine with that
    value of FLAG.

    (When the input magnitude of FLAG is 1, this indicates to the program
    that it is necessary to do some initialization work.  An input magnitude
    of 2 lets the program know that that initialization can be skipped,
    and that useful information was computed earlier.)

    The routine returns with all the information needed to continue
    the integration.  If the integration reached TOUT, the user need only
    define a new TOUT and call again.  In the one-step integrator
    mode, returning with FLAG = -2, the user must keep in mind that
    each step taken is in the direction of the current TOUT.  Upon
    reaching TOUT, indicated by the output value of FLAG switching to 2,
    the user must define a new TOUT and reset FLAG to -2 to continue
    in the one-step integrator mode.

    In some cases, an error or difficulty occurs during a call.  In that case,
    the output value of FLAG is used to indicate that there is a problem
    that the user must address.  These values include:

    * 3, integration was not completed because the input value of RELERR, the
      relative error tolerance, was too small.  RELERR has been increased
      appropriately for continuing.  If the user accepts the output value of
      RELERR, then simply reset FLAG to 2 and continue.

    * 4, integration was not completed because more than MAXNFE derivative
      evaluations were needed.  This is approximately (MAXNFE/6) steps.
      The user may continue by simply calling again.  The function counter
      will be reset to 0, and another MAXNFE function evaluations are allowed.

    * 5, integration was not completed because the solution vanished,
      making a pure relative error test impossible.  The user must use
      a non-zero ABSERR to continue.  Using the one-step integration mode
      for one step is a good way to proceed.

    * 6, integration was not completed because the requested accuracy
      could not be achieved, even using the smallest allowable stepsize.
      The user must increase the error tolerances ABSERR or RELERR before
      continuing.  It is also necessary to reset FLAG to 2 (or -2 when
      the one-step integration mode is being used).  The occurrence of
      FLAG = 6 indicates a trouble spot.  The solution is changing
      rapidly, or a singularity may be present.  It often is inadvisable
      to continue.

    * 7, it is likely that this routine is inefficient for solving
      this problem.  Too much output is restricting the natural stepsize
      choice.  The user should use the one-step integration mode with
      the stepsize determined by the code.  If the user insists upon
      continuing the integration, reset FLAG to 2 before calling
      again.  Otherwise, execution will be terminated.

    * 8, invalid input parameters, indicates one of the following:
      NEQN <= 0;
      T = TOUT and |FLAG| /= 1;
      RELERR < 0 or ABSERR < 0;
      FLAG == 0  or FLAG < -2 or 8 < FLAG.

  Modified:

    27 March 2004

  Author:

    Herman Watts, Lawrence Shampine,
    Sandia Laboratories,
    Albuquerque, New Mexico.

    C++ version by John Burkardt

  Reference:

    Erwin Fehlberg,
    Low-order Classical Runge-Kutta Formulas with Stepsize Control,
    NASA Technical Report R-315, 1969.

    Lawrence Shampine, Herman Watts, S Davenport,
    Solving Non-stiff Ordinary Differential Equations - The State of the Art,
    SIAM Review,
    Volume 18, pages 376-411, 1976.

  Parameters:

    Input, external F, a user-supplied subroutine to evaluate the
    derivatives Y'(T), of the form:

      void f ( double t, double y[], double yp[] )

    Input, int NEQN, the number of equations to be integrated.

    Input/output, double Y[NEQN], the current solution vector at T.

    Input/output, double YP[NEQN], the derivative of the current solution
    vector at T.  The user should not set or alter this information!

    Input/output, double *T, the current value of the independent variable.

    Input, double TOUT, the output point at which solution is desired.
    TOUT = T is allowed on the first call only, in which case the routine
    returns with FLAG = 2 if continuation is possible.

    Input, double *RELERR, ABSERR, the relative and absolute error tolerances
    for the local error test.  At each step the code requires:
      abs ( local error ) <= RELERR * abs ( Y ) + ABSERR
    for each component of the local error and the solution vector Y.
    RELERR cannot be "too small".  If the routine believes RELERR has been
    set too small, it will reset RELERR to an acceptable value and return
    immediately for user action.

    Input, int FLAG, indicator for status of integration. On the first call,
    set FLAG to +1 for normal use, or to -1 for single step mode.  On
    subsequent continuation steps, FLAG should be +2, or -2 for single
    step mode.

    Output, int RKF45_S, indicator for status of integration.  A value of 2
    or -2 indicates normal progress, while any other value indicates a
    problem that should be addressed.

    */



void rk4(VectorXd& x, int nX, double t, double tau, void (*derivsRK)(VectorXd x, double time, VectorXd param, VectorXd& deriv), VectorXd param);

/*
 Runge-Kutta integrator (4th order)
 Inputs
   x          Current value of dependent variable
   nX         Number of elements in dependent variable x
   t          Independent variable (usually time)
   tau        Step size (usually time step)
   derivsRK   Right hand side of the ODE; derivsRK is the
              name of the function which returns dx/dt
              Calling format derivsRK(x,t,param,dxdt).
   param      Extra parameters passed to derivsRK
 Output
   x          New value of x after a step of size tau
*/

#if 0
class DerivativeCalculator
{
public:
    virtual void compute(const Eigen::VectorXd& state, double t, Eigen::VectorXd& derivatives) const = 0;
};

void rk4(VectorXd& x, double t, double tau, const DerivativeCalculator* derivativeCalculator);
#endif

/** Base class for a functor that evaluates the derivates for an integrator. In order
  * to avoid memory allocation, this version is designed to be able to work with Eigen's
  * fixed size vectors.
  */
template<int STATE_SIZE> class DerivativeCalculator
{
public:
    typedef Eigen::Matrix<double, STATE_SIZE, 1> State;

    virtual void compute(const State& state, double t, State& derivatives) const = 0;
};

/** Templatized version of the 4th order Runge-Kutta integrator that is designed to
  * work with a subclass of DerivativeCalculator. Using fixed sized objects will
  * avoid potentially expensive memory allocations.
  */
template<class DERIVCALC> void
rk4(typename DERIVCALC::State& state,
    double t,
    double tau,
    const DERIVCALC* derivativeCalculator)
{
    typename DERIVCALC::State F1;
    typename DERIVCALC::State F2;
    typename DERIVCALC::State F3;
    typename DERIVCALC::State F4;
    typename DERIVCALC::State state_temp;

    // Evaluate F1 = f(x,t).
    derivativeCalculator->compute(state, t, F1);

    // Evaluate F2 = f( x+tau*F1/2, t+tau/2 ).
    double half_tau = 0.5*tau;
    double t_half = t + half_tau;

    state_temp = state + half_tau*F1;
    derivativeCalculator->compute(state_temp, t_half, F2);

    // Evaluate F3 = f( x+tau*F2/2, t+tau/2 ).
    state_temp = state + half_tau*F2;
    derivativeCalculator->compute(state_temp, t_half, F3);

    // Evaluate F4 = f( x+tau*F3, t+tau ).
    double t_full = t + tau;

    state_temp = state + tau*F3;
    derivativeCalculator->compute(state_temp, t_full, F4);

    // Return x(t+tau) computed from fourth-order R-K.
    state += (tau / 6.0) * (F1 + F4 + 2.0 * (F2 + F3));
}


