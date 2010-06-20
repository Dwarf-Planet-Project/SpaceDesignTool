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
// File name: arithmetics.h
///
// Simple and handy arithmetics not present in GSL or Eigen libraries
//
//
//------------------ Author:       Guillermo Ortega               -------------------
//------------------ Affiliation:  European Space Agency (ESA)    -------------------
//-----------------------------------------------------------------------------------

/*
Modified by Guillermo on June 2010 to add  arithmetics to support STA integrators.
These integrators are based on the work of Matthias Gerdts from the University of Wuerzburg

*/



#ifndef ARITHMETICS_H
#define ARITHMETICS_H

double epsilon ( void );              // Returns the round off unit for doubleing arithmetic.
double sign ( double x );             // Returns the sign of a double.

/*!
   \brief machine epsilon

   machine epsilon
*/
#define EPS 2.2204460492503131e-16

/*!
   \brief sqrt of machine epsilon

   sqrt of machine epsilon
*/
#define SQRTEPS 1.490116119384766e-08

/*!
   \brief Allocation of double vector

   \param[in]  n        components
   \param[in]  setZero  all entries of vector are set to 0
   \return              a pointer to a one-dimensional \a double array

   \author Martin Kunkel
   \date 2009
 */
double* allocDoubleVector(int n, bool setZero);

/*!
   \brief Allocation of integer vector

   \param[in]  n        components
   \param[in]  setZero  all entries of vector are set to 0
   \return              a pointer to a one-dimensional \a int array

   \author Martin Kunkel
   \date 2009
 */
int* allocIntVector(int n, bool setZero);

/*!
   \brief Allocation of double matrix

   \param[in]  n        rows
   \param[in]  m        columns
   \param[in]  setZero  all entries of matrix are set to 0
   \return              a pointer to a two-dimensional \a double array which has a guaranteed stride

   \author Martin Kunkel
   \date 2009
 */
double** allocDoubleMatrix(int n, int m, bool setZero);

/*!
   \brief Free double matrix

   \param[in]  A        double matrix allocated by \ref opros_allocDoubleMatrix
   \param[in]  n        rows

   \author Martin Kunkel
   \date 2009
 */
void freeDoubleMatrix(double** A, int n);



/*!
   \brief Jacobian of f by finite differences

   opros_findiff1 computes the Jacobian of the function rhs(t,z,p) w.r.t. (z,p) by finite forward differences.

   \param[in]     f      right hand side function
   \param[in]     param  parameters
   \param[in]     nx     the number of equations
   \param[in]     np     the number of parameters
   \param[in]     t      time
   \param[in]     z      state
   \param[out]    fzjac  jacobian w.r.t. y and param

   \author Matthias Gerdts
   \date 2009
   \author Modified by Guilermo
 */
void findiff1(void f(double t, double z[], double param[], double zp[]),
   double param[], int nx, int np, double t, double z[], double **fzjac);

/*!
   \brief Sensitivity ODE

   Computes right handside of sensitivity ODE and assigns values y(nx+1),...,y(nx(1+np))=y(neq)

   \param[in]     f      right hand side function
   \param[in]     fjac   Jacobian evaluation function
   \param[in]     ijac   flag
			   - ijac == 1 : user supplied Jacobian in fjac
			   - ijac != 1 : use finite difference Jacobian approximation
   \param[in]     param  parameters
   \param[in]     neq    number of equations including sensitivities
   \param[in]     np     the number of parameters
   \param[in]     t      time
   \param[in]     y      state
   \param[out]    sp     sensitivites at t

   \author Matthias Gerdts
   \date 2009
   \author Modified by Guilermo
 */
void sens(void f(double t, double z[], double param[], double zp[]),
    void fjac(double t, double z[], double param[], double** jac),
    int ijac, double param[], int neq, int np, double t, double y[],
    double sp[] );



#endif // ARITHMETICS_H
