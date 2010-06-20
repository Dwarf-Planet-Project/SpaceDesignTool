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

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <Eigen/Core>

USING_PART_OF_NAMESPACE_EIGEN


////////////////////////////////////// Defining maxmimum steps /////////////////////////////////////////
/*!
\brief Maximum order of the multistep method which is implemented.
Maximum order of the multistep method which is implemented.
*/
#define ADAMSBASHFORTH_MAXORDER 5


/*!
\brief Maximum order of the multistep method which is implemented.
Maximum order of the multistep method which is implemented.
*/
#define ADAMSMOULTON_MAXORDER 5

/*!
\brief Maximum order of the single step method which is implemented.
Maximum order of the Nystrom method that has been implemented.
*/
#define NYSTROM_MAXORDER 4

/*!
\brief Minimal order of the single step method which is implemented.
Minimal order of the Nystrom method that has been implemented.
*/
#define NYSTROM_MINORDER 3





////////////////////////////////// Defining the structures of the integrators /////////////////////////////

/*! \brief Integrator Flag types
	  Describes which integrator will be used
*/
typedef enum
{
    /*! Adams-Bashforth multistep */
    ADAMSBASHFORTH,
    /*! Adams-Moulton multistep */
    ADAMSMOULTON,
    /*! Extrapolation */
    EXTRAPOL,
    /*! Nystrom */
    NYSTROM,
    /*! DOPRI5(4), order 4 */
    RK_DOPRI4,
    /*! DOPRI5(4), order 5 */
    RK_DOPRI5,
    /*! RKF2(3), order 2 */
    RK_RK2,
    /*! RKF2(3), order 3 */
    RK_RK3,
    /*! RKF4(5), order 4 */
    RK_RK4,
    /*! RKF4(5), order 5 */
    RK_RK5,
    /*! RKF7(8), order 7 */
    RK_RK7,
    /*! RKF7(8), order 8 */
    RK_RK8,
    /*! DOPRI5(4), order 5 */
    RKSE_DOPRI54,
    /*! RKF2(3), order 2 */
    RKSE_RK23,
    /*! RKF4(5), order 4 */
    RKSE_RK45,
    /*! RKF7(8), order 7 */
    RKSE_RK78
} integrator_flag;



	/*!
\brief internal integrator data
The structure is used to store internal data and to pass them from one call of the integrator to the next call.
The user should never alter the data.
*/
typedef struct
	{
	   /*! is the keep-data initialized? */
	   int initialized;
	   /*! number of stored old steps */
	   int s;
	   /*! index to latest step */
	   int si;
	   /*! current time */
	   double t;
	   /*! holds the function evaluations \f$ f(t,y(t)) \f$ at the last s steps*/
	   double** f;
	   /*! holds the last state vector */
	   double* y;
	   /*! number of equations */
	   int n;
	   /*! step width */
	   double h;
	   /*! order of method */
	   int order;
} adamsbashforth_keep;


/*!
   \brief internal integrator data

   The structure is used to store internal data and to pass them from one call of the integrator to the next call.
   The user should never alter the data.
*/
typedef struct
{
   /*! is the keep-data initialized? */
   int initialized;
   /*! number of stored old steps */
   int s;
   /*! index to latest step */
   int si;
   /*! current time */
   double t;
   /*! holds the function evaluations \f$ f(t,y(t)) \f$ at the last s steps*/
   double** f;
   /*! holds the last state vector */
   double* y;
   /*! tmporary variables */
   double* yold;
   double* yhelp;
   /*! number of equations */
   int n;
   /*! step width */
   double h;
   /*! order of method */
   int order;
} adamsmoulton_keep;



/*!
   \brief internal integrator data

   The structure is used to store internal data and to pass them from one call of the integrator to the next call.
   The user should never alter the data.
*/
typedef struct
{
    /*! is the keep-data initialized? */
    int initialized;
    /*! current time */
    double t;
    /*! holds the last state vector */
    double* y;
    /*! storage for approximations */
    double** S;
    /*! step size factors */
    double* ni;
    /*! temporary storage */
    double** temp;
    /*! number of equations */
    int n;
    /*! step width */
    double h;
    /*! order of method */
    int order;
} extrapol_keep;


/*!
   \brief internal integrator data

   The structure is used to store internal data and to pass them from one call of the integrator to the next call.
   The user should never alter the data.
*/
typedef struct
{
   /*! is the keep-data initialized? */
   int initialized;
   /*! temporary data */
   double* yak;
   /*! temporary data */
   double** k;
   /*! number of equations */
   int n;
   /*! order of method */
   int order;
} nystrom_keep;



/*!
  \brief Integrator options

  The structure is used to store options for the different integrators. Note that you may call the subroutine \ref integrator_defaults to set default values.
*/
typedef struct
{
  /*! order of method (only for Adams-Bashforth, Adams-Moulton, Nystrom and Extrapolation integrators) */
  int order;
  /*! tolerance for nonlinear fixpoint iteration (only for Adams-Moulton integrator) */
  double iterTol;
  /*! maximum number of fixpoint iterations in each step (only for Adams-Moulton integrator) */
  int iterMax;
  /*! step size control: abolute tolerance (only for RKSE integrators) */
  double* atol;
  /*! step size control: relative tolerance (only for RKSE integrators) */
  double* rtol;
  /*! info-array (only for RKSE integrators) */
  int rkse_info[9];
} integrator_options;



/*!
   \brief internal integrator data

   The structure is used to store internal data and to pass them from one call of the integrator to the next call. The user should never alter the data.
*/
typedef struct
{
  /*! is the keep-data initialized? */
  int initialized;
  /*! internal Runge-Kutta stages */
  double** k;
  /*! first dimension of k */
  int ksize;
  /*! temporary storage */
  double* solp;
  /*! temporary storage */
  double* solq;
  /*! rk method */
  integrator_flag method;
  /*! order of rk methd */
  int p;
} rkse_keep;

/*!
   \brief internal integrator data

   The structure is used to store internal data and to pass them from one call of the integrator to the next call. The user should never alter the data.
*/
typedef struct
{
   /*! is the keep-data initialized? */
   int initialized;
   /*! current time */
   double t;
   /*! holds the last state vector */
   double* y;
   /*! holds the evaluation of f */
   double* dy;
   /*! internal Runge-Kutta stages */
   double** k;
   /*! first dimension of k */
   int ksize;
   /*! temporary storage */
   double* tmp;
   /*! number of equations */
   int n;
   /*! rk method */
   integrator_flag method;
} rk_keep;




//////////////////////////////////////////////// Defining auxiliary functions ///////////////////////////////////
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
	 double *k4, double *k5, double *k6,double *k7 );


///////////////////////////////////////// Defining the integrators //////////////////////////////////////////////
/*!
   \brief Computes one step of the RKF4(5) integration procedure.

   Computes one step of the RKF4(5) integration procedure.

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
   \param[out] solp   solution of 4th order method
   \param[out] solq   solution of 5th order method
   \param[in]  np     number of parameters
   \param[in,out] k2  auxiliary array for storing stages of RK
   \param[in,out] k3  auxiliary array for storing stages of RK
   \param[in,out] k4  auxiliary array for storing stages of RK
   \param[in,out] k5  auxiliary array for storing stages of RK
   \param[in,out] k6  auxiliary array for storing stages of RK
*/
void rkf45step(void f(double t, double y[], double param[], double yp[]),
	 void fjac(double t, double y[], double param[], double** jac),
	 int ijac,double* param,double t,double h,int neq,double *y,
	 double *k1,double *solp,double *solq,int np,
	 double *k2,double *k3,double *k4,double *k5,double *k6 );


 /*!
   \brief Computes one step of the RKF7(8) integration procedure.

   Computes one step of the RKF7(8) integration procedure.

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
   \param[out] solp   solution of 7th order method
   \param[out] solq   solution of 8th order method
   \param[in]  np     number of parameters
   \param[in,out] k2  auxiliary array for storing stages of RK
   \param[in,out] k3  auxiliary array for storing stages of RK
   \param[in,out] k4  auxiliary array for storing stages of RK
   \param[in,out] k5  auxiliary array for storing stages of RK
   \param[in,out] k6  auxiliary array for storing stages of RK
   \param[in,out] k7  auxiliary array for storing stages of RK
   \param[in,out] k8  auxiliary array for storing stages of RK
   \param[in,out] k9  auxiliary array for storing stages of RK
   \param[in,out] k10 auxiliary array for storing stages of RK
   \param[in,out] k11 auxiliary array for storing stages of RK
   \param[in,out] k12 auxiliary array for storing stages of RK
*/
void rkf78step(void f(double t, double y[], double param[], double yp[]),
	 void fjac(double t, double y[], double param[], double** jac),
	 int ijac,double* param,double t,double h,int neq,double *y,
	 double *k1,double *solp,double *solq,int np,
	 double *k2,double *k3,double *k4,double *k5,double *k6,double* k7,
	 double *k8,double *k9,double *k10,double *k11,double *k12 );


 /*!
   \brief Computes one step of the RKF2(3) integration procedure.

   Computes one step of the RKF2(3) integration procedure.

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
   \param[out] solp   solution of 2th order method
   \param[out] solq   solution of 3th order method
   \param[in]  np     number of parameters
   \param[in,out] k2  auxiliary array for storing stages of RK
   \param[in,out] k3  auxiliary array for storing stages of RK
   \param[in,out] k4  auxiliary array for storing stages of RK
   \param[in,out] k5  auxiliary array for storing stages of RK
*/
void rkf23step(void f(double t, double y[], double param[], double yp[]),
	 void fjac(double t, double y[], double param[], double** jac),
	 int ijac,double* param,double t,double h,int neq,double *y,
	 double *k1,double *solp,double *solq,int np,
	 double *k2,double *k3,double *k4,double *k5 );




/*!
   \brief Computes Runge_Kutta_Fehlberg integration procedure.

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



/*!
   \brief Adams-Bashforth multistep method

   \section sec-adams-bashforth Adams-Bashforth multistep method
   Integrate the system of differential equations
	       \f[ y'(t) = f(t,y),\quad y(t) = y_0 \f]
   on the time interval \f$[t, t + h \cdot steps] \f$ using Adams-Bashforth multistep method.

   Reference: J. Stoer, R. Burlisch: "Numerische Mathematik 2", Springer-Verlag Berlin-Heidelberg-New York, pp. 135-136.

   \subsection subsec-adams-bashforth-1 Initialization and first step
   The first call to the integrator must be with parameter \a flag = 1. The method will call a single step method for the first \a order steps which will be stored in the variable \a keep. The user should never alter the variable \a keep and must provide it on subsequent calls.

   \subsection subsec-adams-bashforth-2 Continuation of integration
   Set \a keep = 2 in order to continue the integration. The values of \a n, \a y, \a t, \a order and \a h are ignored.

   \subsection subsec-adams-bashforth-3 Cleanup
   The user may call the subroutine with \a keep = -1 in order to free allocated memory. All values else then \a keep are ignored.

   \param[in]     f      A user-supplied subroutine defining the differential equation \f$ y' = f(t,y) \f$
   \param[in]     param  parameters passed to function f
   \param[in]     n      (if \a flag = 1) the number of equations to be integrated
   \param[in,out] t      (if \a flag = 1) on input: initial time \n
			 on output: final time of integration
   \param[in,out] y      (if \a flag = 1) on input: the initial values \f$ y_0 = y(t) \f$ \n
			 on output: solution, the value of \f$ y(t_{out}) \f$
   \param[out]    yp     solution, the value of \f$ y'(t_{out}) = f(t_{out},y(t_{out})) \f$
   \param[in]     steps  the number of steps with stepwidth \a h
   \param[in]     h      (if \a flag = 1) step width \n
   \param[in]     order  (if \a flag = 1) order of the method, \a order must be between 1 and \ref ADAMSBASHFORTH_MAXORDER \n
   \param[in]     flag   indicator for status of integration: \n
			  - \a flag = 1 on first call \n
			  - \a flag = 2 for continuation of integration \n
			  - \a flag = -1 cleanup (deallocate array in keep)
   \param[in,out] keep   internal integrator data \n
			 the user should never alter \a keep
   \return status flag: \n
	   -      0: success \n
	   -     -1: error: allocation of internal memory failed
	   -     -2: error: \a keep is not initialized, call method with \a flag = 1
	   -    -10: error: \a flag has an invalid value
	   -    -11: error: \a n has an invalid value
	   -    -12: error: \a h has an invalid value, \a h must be non-zero
	   -    -13: error: \a order has an invalid value

   \author Martin Kunkel
   \author interfaced based on EODE (European Space Agency Ordinary Differntial Equations library)
   \date 2009
*/
int adamsbashforth(
	void f(double t, double y[], double param[], double yp[]),
	double param[], int n, double* t, double y[], double yp[], int steps, double h, int order, int flag,
	adamsbashforth_keep* keep);


/*!
   \brief Adams-Moulton multistep method

   \section sec-adams-moulton Adams-Moulton multistep method
   Integrate the system of differential equations
	       \f[ y'(t) = f(t,y),\quad y(t) = y_0 \f]
   on the time interval \f$[t, t + h \cdot steps] \f$ using Adams-Moulton multistep method.

   Reference: J. Stoer, R. Burlisch: "Numerische Mathematik 2", Springer-Verlag Berlin-Heidelberg-New York, pp. 136-137.

   \subsection subsec-adams-moulton-1 Initialization and first step
   The first call to the integrator must be with parameter \a flag = 1. The method will call a single step method for the first \a order steps which will be stored in the variable \a keep. The user should never alter the variable \a keep and must provide it on subsequent calls.

   \subsection subsec-adams-moulton-2 Continuation of integration
   Set \a keep = 2 in order to continue the integration. The values of \a n, \a y, \a t, \a order and \a h are ignored.

   \subsection subsec-adams-moulton-3 Cleanup
   The user may call the subroutine with \a keep = -1 in order to free allocated memory. All values else then \a keep are ignored.

   \param[in]     f       A user-supplied subroutine defining the differential equation \f$ y' = f(t,y) \f$
   \param[in]     param   parameters passed to function f
   \param[in]     n       (if \a flag = 1) the number of equations to be integrated
   \param[in,out] t       (if \a flag = 1) on input: initial time \n
			  on output: final time of integration
   \param[in,out] y       (if \a flag = 1) on input: the initial values \f$ y_0 = y(t) \f$ \n
			  on output: solution, the value of \f$ y(t_{out}) \f$
   \param[out]    yp      solution, the value of \f$ y'(t_{out}) = f(t_{out},y(t_{out})) \f$
   \param[in]     steps   the number of steps with stepwidth \a h
   \param[in]     h       (if \a flag = 1) step width \n
   \param[in]     order   (if \a flag = 1) order of the method, \a order must be between 1 and \ref ADAMSMOULTON_MAXORDER \n
   \param[in]     iterTol tolerance for nonlinear fixpoint iteration
   \param[in]     iterMax maximum number of fixpoint iterations in each step
   \param[in]     flag    indicator for status of integration: \n
			   - \a flag = 1 on first call \n
			   - \a flag = 2 for continuation of integration \n
			   - \a flag = -1 cleanup (deallocate array in keep)
   \param[in,out] keep    internal integrator data \n
			  the user should never alter \a keep
   \return status flag: \n
	   -      0: success \n
	   -     -1: error: allocation of internal memory failed
	   -     -2: error: \a keep is not initialized, call method with \a flag = 1
	   -    -10: error: \a flag has an invalid value
	   -    -11: error: \a n has an invalid value
	   -    -12: error: \a h has an invalid value, \a h must be non-zero
	   -    -13: error: \a order has an invalid value
	   -    -20: error: solution of nonlinear equation failed,
			    user should call subroutine with \a flag = -1 (cleanup) and restart with \a flag = 1 and smaller \a h
   \author Martin Kunkel
   \author interfaced based on EODE (European Space Agency Ordinary Differntial Equations library)
   \date 2009
*/
int adamsmoulton(
	void f(double t, double y[], double param[], double yp[]),
	double param[], int n, double* t, double y[], double yp[], int steps, double h, int order,
	double iterTol, int iterMax,
	int flag, adamsmoulton_keep* keep);



/*!
   \brief Gragg-Bulirsch-Stoer extrapolation method.

   \section sec-extrapol Gragg-Bulirsch-Stoer extrapolation method
   Integrate the system of differential equations
	       \f[ y'(t) = f(t,y),\quad y(t) = y_0 \f]
   on the time interval \f$[t, t + h \cdot steps] \f$ using Gragg-Bulirsch-Stoer extrapolation method.

   Reference: J. Stoer, R. Burlisch: "Numerische Mathematik 2", Springer-Verlag Berlin-Heidelberg-New York, pp. 166-168.

   \subsection subsec-extrapol-1 Initialization and first step
   The first call to the integrator must be with parameter \a flag = 1. The method will initialize the variable \a keep.
   The user should never alter the variable \a keep and must provide it on subsequent calls.

   \subsection subsec-extrapol-2 Continuation of integration
   Set \a keep = 2 in order to continue the integration. The values of \a n, \a y, \a t, \a order and \a h are ignored.

   \subsection subsec-extrapol-3 Cleanup
   The user may call the subroutine with \a keep = -1 in order to free allocated memory. All values other then \a keep are ignored.

   \param[in]     f      A user-supplied subroutine defining the differential equation \f$ y' = f(t,y) \f$
   \param[in]     param  parameters passed to function f
   \param[in]     n      (if \a flag = 1) the number of equations to be integrated
   \param[in,out] t      (if \a flag = 1) on input: initial time \n
			 on output: final time of integration
   \param[in,out] y      (if \a flag = 1) on input: the initial values \f$ y_0 = y(t) \f$ \n
			 on output: solution, the value of \f$ y(t_{out}) \f$
   \param[out]    yp     solution, the value of \f$ y'(t_{out}) = f(t_{out},y(t_{out})) \f$
   \param[in]     steps  the number of steps with stepwidth \a h
   \param[in]     h      (if \a flag = 1) step width \n
   \param[in]     order  (if \a flag = 1) order of the method, \a order must be greater than or equal to 0 \n
   \param[in]     flag   indicator for status of integration: \n
			  - \a flag = 1 on first call \n
			  - \a flag = 2 for continuation of integration \n
			  - \a flag = -1 cleanup (deallocate array in keep)
   \param[in,out] keep   internal integrator data \n
			 the user should never alter \a keep
   \return status flag: \n
	   -      0: success \n
	   -     -1: error: allocation of internal memory failed
	   -     -2: error: \a keep is not initialized, call method with \a flag = 1
	   -    -10: error: \a flag has an invalid value
	   -    -11: error: \a n has an invalid value
	   -    -12: error: \a h has an invalid value
	   -    -13: error: \a order has an invalid value

   \author Matthias Gerdts
   \author interfaced based on EODE (European Space Agency Ordinary Differntial Equations library)
   \date 2009
*/
int extrapol(
	void f(double t, double y[], double param[], double yp[]),
	double param[], int n, double* t, double y[], double yp[], int steps, double h, int order, int flag,
	extrapol_keep* keep);




/*!
   \brief Nystrom single step method

   \section sec-nystrom Nystrom single step method
   Integrate the system of differential equations
	       \f[ y'(t) = z(t), \quad z'(t) = f(t,y),\quad y(0) = y_0,  z(t) = z_0\f]
   on the time interval \f$[t, t + h \cdot steps] \f$ using Nystrom's single step method.

   Reference: K. Strehmel, R. Weiner: "Numerik gewöhnlicher Differentialgleichungen", Teubner Studienbücher, pp. 72-73.

   \param[in]     f      A user-supplied subroutine defining the differential equation \f$ y' = f(t,y) \f$
   \param[in]     param  parameters passed to function f
   \param[in]     n      (if \a flag = 1) the number of equations to be integrated
   \param[in,out] t      (if \a flag = 1) on input: initial time \n
			 on output: final time of integration
   \param[in,out] y      (if \a flag = 1) on input: the initial values \f$ y_0 = y(t) \f$ \n
			 on output: solution, the value of \f$ y(t_{out}) \f$
   \param[out]    yp     solution, the value of \f$ y'(t_{out}) = f(t_{out},y(t_{out})) \f$
   \param[in]     steps  the number of steps with stepwidth \a h
   \param[in]     h      (if \a flag = 1) step width \n
   \param[in]     order  (if \a flag = 1) order of the method, \a order must be between \ref NYSTROM_MINORDER and \ref NYSTROM_MAXORDER \n
   \param[in]     flag   indicator for status of integration: \n
			  - \a flag = 1 on first call \n
			  - \a flag = 2 for continuation of integration \n
			  - \a flag = -1 cleanup (deallocate array in keep)
   \param[in,out] keep   internal integrator data \n
			 the user should never alter \a keep
   \return status flag: \n
	   -      0: success \n
	   -     -1: error: allocation of internal memory failed
	   -     -2: error: \a keep is not initialized, call method with \a flag = 1
	   -    -10: error: \a flag has an invalid value
	   -    -11: error: \a n has an invalid value
	   -    -12: error: \a h has an invalid value
	   -    -13: error: \a order has an invalid value

   \author Bjoern Huepping
   \author interface and documentation based on adamsbashforth.c by Martin Kunkel
   \date 2009
*/
int nystrom(
	void f(double t, double y[], double param[], double ypp[]),
	double param[], int n, double* t, double y[], double yp[], int steps, double h, int order, int flag,
	nystrom_keep* keep);



/*!
   \brief Runge-Kutta single step methods

   \section sec-rk Runge-Kutta single step methods
   Integrate the system of differential equations
	       \f[ y'(t) = f(t,y),\quad y(t) = y_0 \f]
   on the time interval \f$[t, t + h \cdot steps] \f$ using a Runge-Kutta single step method.

   \subsection subsec-rk-1 Initialization and first step
   The first call to the integrator must be with parameter \a flag = 1. The method will initialize the variables in \a keep. The user should never alter \a keep and must provide it on subsequent calls.

   \subsection subsec-rk-2 Continuation of integration
   Set \a keep = 2 in order to continue the integration. The values of \a n, \a y, \a t and \a method are ignored.

   \subsection subsec-rk-3 Cleanup
   The user may call the subroutine with \a flag = -1 in order to free allocated memory. All values else then \a keep are ignored.

   \param[in]     f       A user-supplied subroutine defining the differential equation \f$ y' = f(t,y) \f$
   \param[in]     param   parameters passed to function f
   \param[in]     n       (if \a flag = 1) the number of equations to be integrated
   \param[in,out] t       (if \a flag = 1) on input: initial time \n
			  on output: final time of integration
   \param[in,out] y       (if \a flag = 1) on input: the initial values \f$ y_0 = y(t) \f$ \n
			  on output: solution, the value of \f$ y(t_{out}) \f$
   \param[out]    yp      solution, the value of \f$ y'(t_{out}) = f(t_{out},y(t_{out})) \f$
   \param[in]     steps   the number of steps with stepwidth \a h
   \param[in]     h       step width \n
   \param[in]     method  (if \a flag = 1) Runge-Kutta method see \ref integrator_flag for possible values
   \param[in]     flag    indicator for status of integration: \n
			   - \a flag = 1 on first call \n
			   - \a flag = 2 for continuation of integration \n
			   - \a flag = -1 cleanup (deallocate array in keep)
   \param[in,out] keep    internal integrator data \n
			  the user should never alter \a keep
   \return status flag: \n
	   -      0: success \n
	   -     -1: error: allocation of internal memory failed
	   -     -2: error: \a keep is not initialized, call method with \a flag = 1
	   -     -9: error: \a method has an invalid value
	   -    -10: error: \a flag has an invalid value
	   -    -11: error: \a n has an invalid value
	   -    -12: error: \a h has an invalid value, \a h must be non-zero

   \author Martin Kunkel
   \author interfaced based on EODE (European Space Agency Ordinary Differntial Equations library)
   \date 2009
*/
int rk(
	void f(double t, double y[], double param[], double yp[]),
	double param[], int n, double* t, double y[], double yp[], int steps, double h,
	integrator_flag method, int flag, rk_keep* keep);




/*!
   \brief Runge-Kutta single step methods with step-size selection

   \section sec-rk-ss Runge-Kutta single step methods with step-size selection
   Integrate the system of differential equations
	       \f[ y'(t) = f(t,y),\quad y(t) = y_0 \f]
   on the time interval \f$[t, t + h \cdot steps] \f$ using a Runge-Kutta single step method with step-size selection.

   \subsection subsec-rk-1 Initialization and first step
   The first call to the integrator must be with parameter \a flag = 1. The method will initialize the variables in \a keep. The user should never alter \a keep and must provide it on subsequent calls.

   \subsection subsec-rk-2 Continuation of integration
   Set \a keep = 2 in order to continue the integration. The values of \a n, \a y, \a t and \a method are ignored.

   \subsection subsec-rk-3 Cleanup
   The user may call the subroutine with \a flag = -1 in order to free allocated memory. All values else then \a keep are ignored.

   \param[in]     f       A user-supplied subroutine defining the differential equation \f$ y' = f(t,y) \f$
   \param[in]     fjac    A user-supplied subroutine defining the Jacobian \f$f'_y(t,y)\f$
   \param[in]     np      number of parameters for sensitivity analysis (np >= 0)
   \param[in]     param   contains parameters
   \param[in]     n       (if \a flag = 1) the number of equations to be integrated
   \param[in,out] t       (if \a flag = 1) on input: initial time \n
			  on output: final time of integration
   \param[in]     tout    final time \n
   \param[in,out] y       (if \a flag = 1) on input: the initial values \f$ y_0 = y(t) \f$ \n
			  on output: solution, the value of \f$ y(t_{out}) \f$
   \param[out]    yp      solution, the value of \f$ y'(t_{out}) = f(t_{out},y(t_{out})) \f$
   \param[in]     rtol    array of relative tolerances
   \param[in]     atol    array of absolute tolerances
   \param[in]     info    array of dimension 9 \n
			  info(0)=0:    rtol and atol are scalars \n
				  1:    rtol and atol are arrays  \n
			  info(1)=0:    default value for maximum number of \n
					integration steps \n
				 >0:    user defined maximum number of \n
					integration steps
			  info(2)=0:    default value for security factor
					needed for calculation of new step size \n
				  1:    user defined value of security factor
					in rpar(np) \n
			  info(3)=0:    default value for minimal step size
					multiplication factor \n
				  1:    minimal step size multiplication factor
					in rpar(np+1) \n
			  info(4)=0:    default value for maximal step size
					multiplication factor \n
				  1:    maximal step size multiplication factor
					in rpar(np+2) \n
			  info(5)=0:    default value for minimal step size \n
				  1:    minimal step size in rpar(np+3) \n
			  info(6)=0:    default value for initial step size \n
				  1:    initial step size in rpar(np+4) \n
			  info(7)=0:    sensitivities are not included in the
					error control \n
				  1:    sensitivities are included in the
					error control \n
			  info(8)=0:    numerical approximation of jacobian
					of f by use of divided differences \n
				  1:    jacobian of f is provided in fjac
   \param[in]     method  (if \a flag = 1) Runge-Kutta method see \ref integrator_flag for possible values
   \param[in]     flag    indicator for status of integration: \n
			   - \a flag = 1 on first call \n
			   - \a flag = 2 for continuation of integration \n
			   - \a flag = -1 cleanup (deallocate array in keep)
   \param[in,out] keep    internal integrator data \n
			  the user should never alter \a keep
   \return status flag: \n
	   -      1: success, tout has not been reached exactly \n
	   -      0: success \n
	   -     -1: error: allocation of internal memory failed
	   -     -2: error: \a keep is not initialized, call method with \a flag = 1
	   -     -3: input error in info
	   -     -4: step size too small
	   -     -5: maximum number of integration steps
	   -     -6: t and tout closer than hmin
	   -     -7: invalid status of initialization
	   -     -9: \a method has an invalid value
	   -    -10: error: \a flag has an invalid value
	   -    -11: error: \a n has an invalid value
	   -    -12: error: \a h has an invalid value

   \author Matthias Gerdts
   \author interfaced based on EODE (European Space Agency Ordinary Differntial Equations library)
   \date 2009
*/
int rkse(
    void f(double t, double y[], double param[], double yp[]),
    void fjac(double t, double y[], double param[], double **jac),
    int np, double param[], int n, double* t, double tout, double y[], double yp[],
    double rtol[],double atol[], int info[],
    integrator_flag method, int flag, rkse_keep* keep);













/////////////////////////////// Defining the control of the integrators ///////////////////////////////////
/*!
   \brief Common integrator subroutine

   \section integrator Common integrator.
   Integrate the system of differential equations
	       \f[ y'(t) = f(t,y),\quad y(t) = y_0 \f]
   on the time interval \f$[t, t + h \cdot steps] \f$ using different one-step and multi-step methods with default options. Order for extrapolation, Nystrom and Adams-Integrator is 4.

   \subsection subsec-integrator-1 Initialization and first step
   The first call to the integrator must be with parameter \a flag = 1. The method will initialize the integrator and the variable \a keep. The user should never alter the variable \a keep and must provide it on subsequent calls.

   \subsection subsec-integrator-2 Continuation of integration
   Set \a keep = 2 in order to continue the integration. The values of \a n, \a y, \a t, \a order and \a h are ignored.

   \subsection subsec-integrator-3 Cleanup
   The user may call the subroutine with \a keep = -1 in order to free allocated memory. All values else than \a keep are ignored.

   \param[in]     iflag  indicator for integrator \n
			 Valid integrators: \n
			 - ADAMSBASHFORTH
			 - ADAMSMOULTON
			 - EXTRAPOL
			 - NYSTROM
			 - RK_DOPRI4
			 - RK_DOPRI5
			 - RK_RK2
			 - RK_RK3
			 - RK_RK4
			 - RK_RK5
			 - RK_RK7
			 - RK_RK8
			 - RKSE_DOPRI54
			 - RKSE_RK23
			 - RKSE_RK45
			 - RKSE_RK78
   \param[in]     f      A user-supplied subroutine defining the differential equation \f$ y' = f(t,y) \f$
   \param[in]     param  parameters passed to function f
   \param[in]     n      (if \a flag = 1) the number of equations to be integrated
   \param[in,out] t      (if \a flag = 1) on input: initial time \n
			 on output: final time of integration \f$ t_{out} = t + h\cdot steps \f$
   \param[in,out] y      (if \a flag = 1) on input: the initial values \f$ y_0 = y(t) \f$ \n
			 on output: solution, the value of \f$ y(t_{out}) \f$
   \param[out]    yp     solution, the value of \f$ y'(t_{out}) = f(t_{out},y(t_{out})) \f$
   \param[in]     steps  the number of steps with stepwidth \a h
   \param[in]     h      (if \a flag = 1) step width \n
   \param[in]     flag   indicator for status of integration: \n
			  - \a flag = 1 on first call \n
			  - \a flag = 2 for continuation of integration \n
			  - \a flag = -1 cleanup (deallocate array in keep)
   \param[in]     options integrator options
   \param[in,out] keep   internal integrator data \n
			 the user should never alter \a keep
			 \note you need to pass a reference to a keep-variable since the integrator allocates the required memory
   \return status flag: \n
	   -      0: success \n
	   -     -1: error in memory allocation \n
	   -   <-10: error code of integrator times 10. For a detailed description refer to Sections \ref sec-nystrom,
		     \ref sec-adams-bashforth, \ref sec-adams-moulton, \ref sec-rk, \ref sec-rk-ss

   \author Martin Kunkel
   \author interfaced based on EODE (European Space Agency Ordinary Differntial Equations library)
   \date 2010
*/
int integrate(
	integrator_flag iflag,
	void f(double t, double y[], double param[], double yp[]),
	double param[], int n, double* t, double y[], double yp[], int steps, double h, int flag,
	integrator_options* options, void** keep);


/*!
   \brief Sets default integrator options
   \param[out]     options  integrator options
   \param[in]      n        number of equations

   \return status flag: \n
	   -      0: success \n
	   -     -1: error in memory allocation

   \author Martin Kunkel
   \date 2010
*/
int integrator_defaults( integrator_options* options, int n );

/*!
   \brief Cleanup of integrator options
   \param[out]     options  integrator options

   \author Martin Kunkel
   \date 2010
*/
void integrator_options_cleanup( integrator_options* options );


