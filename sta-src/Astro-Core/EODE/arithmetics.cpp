//-----------------------------------------------------------------------------------
// File name: arithmetics.cpp
//
//-Copyright (C) 2007 European Space Agency (space.trajectory.analysis AT gmail.com) -
//
// This program is free software; you can redistribute it and/or modify it under
// the terms of the GNU Lesser General Public License as published by the Free Software
// Foundation; either version 2 of the License, or (at your option) any later
// version.
//
// This program is distributed in the hope that it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
// FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more
// details.
//
// You should have received a copy of the GNU Lesser General Public License along with
// this program; if not, write to the Free Software Foundation, Inc., 59 Temple
// Place - Suite 330, Boston, MA  02111-1307, USA.
//
// Further information about the GNU Lesser General Public License can also be found on
// the world wide web at http://www.gnu.org.
//
// Simple and handy arithmetics not present in GSL or Eigen libraries
//
//
//------------------ Author:       Guillermo Ortega               -------------------
//------------------ Affiliation:  European Space Agency (ESA)    -------------------
//-----------------------------------------------------------------------------------



#include "arithmetics.h"



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
