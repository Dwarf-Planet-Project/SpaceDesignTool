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
