/*

 This program is free software; you can redistribute it and/or modify it under
 the terms of the GNU General Public License as published by the Free Software
 Foundation; either version 2 of the License, or (at your option) any later
 version.

 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE. See the GNU General Public License for more
 details.

 You should have received a copy of the GNU General Public License along with
 this program; if not, write to the Free Software Foundation, Inc., 59 Temple
 Place - Suite 330, Boston, MA 02111-1307, USA.
 Further information about the GNU General Public License can also be found on
 the world wide web at http://www.gnu.org.
 */


/*
 --- Copyright (C) 2009 STA Steering Board (space.trajectory.analysis AT gmail.com) ---
*/

//
//------------------ Author:       Tiziana Sabatini               -------------------
//------------------ Affiliation:  Politecnico di Milano          -------------------
//-----------------------------------------------------------------------------------
// Created May 2009

#ifndef ANGLECONVERSION_H
#define ANGLECONVERSION_H

class AngleConversion{


  public:
  void decimalToDegMinSec(double , int& , int& , double& );


  double DegMinSecToDecimal(int , int , double );

 };


#endif // ANGLECONVERSION_H
