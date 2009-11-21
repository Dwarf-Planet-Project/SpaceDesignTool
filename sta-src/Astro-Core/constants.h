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
//                             CONSTANTS and SCCALE FACTORS
// The European Space Agency Constants Lists. The list is divided into several categories:
// Mathematical, Phisical, and String constants.
// All constants are given in the International Unit System MKS.
// The file contains also scale factor between units.
//
// The authoritative sources for physical constants are the 2002 CODATA recommended values,
// published in the web sites:
//   http://www.physics.nist.gov/cuu/Constants/index.html
//   http://physics.nist.gov/Pubs/SP811/appenB9.html
//
//
//
//------------------ Author:       Guillermo Ortega               -------------------
//------------------ Affiliation:  European Space Agency (ESA)    -------------------
// Date: May 21st 2009
// Modification history goes here
//-----------------------------------------------------------------------------------


#ifndef CONSTANTS_H
#define CONSTANTS_H


// Mathematical constants

        const double mypi = 3.141592653589793238462643383279502884197169399375;

        const double TWOPI = 3.141592653589793238462643383279502884197169399375 * 2.0;

        const double HALFPI = 3.141592653589793238462643383279502884197169399375 / 2.0;

        const double RAD2DEG = 180.0 / 3.141592653589793238462643383279502884197169399375;

        const double DEG2RAD = 3.141592653589793238462643383279502884197169399375 / 180.0;

        const double MINUTE = 6e1;                  // s

        const double HOUR = 3.6e3;                  // s

        const double DAY = 8.64e4;                  // s

        const double WEEK = 6.048e5;                // s

        const double INCH = 2.54e-2;                // m

        const double FOOT = 3.048e-1;               // m

        const double YARD = 9.144e-1;               // m

        const double MILE = 1.609344e3;             // m

        const double NAUTICAL_MILE = 1.852e3;       // m

        const double MICRON = 1e-6;                 // m

        const double ANGSTROM = 1e-10;              // m

        const double HECTARE = 1e4;                 // m^2

        const double ACRE = 4.04685642241e3;        // m^2

        const double CALORIE = 4.1868e0;            // kg m^2 / s^2

        const double BTU = 1.05505585262e3;         // kg m^2 / s^2

        const double BAR = 1e5;                     // kg / m s^2

        const double STD_ATMOSPHERE = 1.01325e5;    // kg / m s^2

        const double PSI = 6.89475729317e3;         // kg / m s^2


// Scale factors

        const double YOTTA = 1e24;

        const double ZETTA = 1e21;

        const double EXA = 1e18;

        const double PETA = 1e15;

        const double TERA = 1e12;

        const double GIGA = 1e9;

        const double MEGA = 1e6;

        const double KILO = 1e3;

        const double MILLI = 1e-3;

        const double MICRO = 1e-6;

        const double NANO = 1e-9;

        const double PICO = 1e-12;

        const double FEMTO = 1e-15;

        const double ATTO = 1e-18;

        const double ZEPTO = 1e-21;

        const double YOCTO = 1e-24;



// Physical constants

        const double Mol_Weight_SL_Earth = 28.9644e-3;        // (kg/mole)

        const double RADIATION_CONSTANT = 5.67032e-08;        // (W/m^2 K^4)

        const double ZERO_KELVIN = 273.16;                    // (K)

        const double SPEED_OF_LIGHT = 299792458.0;            // (+/- 1 m/s)

        const double PLANCK_CONSTANT = 6.62606896e-34;        // kg m^2 / s   Planck’s constant, h

        const double ASTRONOMICAL_UNIT = 1.49597870691e11;     // m

        const double LIGHT_YEAR = 9.46053620707e15;            // m

        const double NUM_AVOGADRO = 6.02214199e23;             // 1 / mol

        const double PARSEC = 3.08567758135e16;                // m

        const double ELECTRON_VOLT =1.602176487e-19;           // kg m^2 / s^2

        const double MASS_ELECTRON = 9.10938188e-31;           // kg

        const double BOLTZMANN_CONSTANT = 1.3806504e-23;       // kg m^2 / K s^2  The Boltzmann constant, k.

        const double STEFAN_BOLTZMANN_CONSTANT = 5.67040047374e-8; // kg / K^4 s^3

        const double MOLAR_GAS_CONSTANT = 8.314472e0;          // kg m^2 / K mol s^2     The molar gas constant, R0

        const double STANDARD_GAS_VOLUME = 2.2710981e-2;       // m^3 / mol   The standard gas volume, V

        const double ELECTRON_CHARGE = 1.602176487e-19;        // A s

        const double EARTH_GRAVITY_ZERO = 9.80665;             // (m/s**2) earth sea level and 45 deg northern latitude

        const double GRAVITATIONAL_CONSTANT = 6.67428e-11;    // m^3 / kg s^2



// Masses of the Solar System bodies

       const double Sun_MASS = 1.98892e30;                     // kg
       const double Mercury_MASS = 3.30e23;                     // kg
       const double Venus_MASS = 4.87e24;                     // kg
       const double Earth_MASS = 5.97e24;                     // kg
       const double Mars_MASS = 6.42e23;                     // kg
       const double Jupiter_MASS = 1.90e27;                     // kg
       const double Saturn_MASS = 5.68e26;                     // kg
       const double Uranus_MASS = 8.68e25;                     // kg
       const double Neptune_MASS = 1.02e26;                     // kg
       const double Pluto_MASS = 1.27e22;                     // kg
       const double Moon_MASS = 7.35e22;                     // kg
       const double Phobos_MASS = 1.08e16;                     // kg
       const double Deimos_MASS = 1.80e15;                     // kg
       const double Metis_MASS = 9.56e16;                     // kg
       const double Adrastea_MASS = 1.91e16;                     // kg
       const double Amalthea_MASS = 3.50e18;                     // kg
       const double Thebe_MASS = 7.77e17;                     // kg
       const double Io_MASS = 8.93e22;                     // kg
       const double Europa_MASS = 4.80e22;                     // kg
       const double Ganymede_MASS = 1.48e23;                     // kg
       const double Callisto_MASS = 1.08e23;                     // kg
       const double Leda_MASS = 5.68e15;                     // kg
       const double Himalia_MASS = 9.56e18;                     // kg
       const double Lysithea_MASS = 7.77e16;                     // kg
       const double Elara_MASS = 7.77e17;                     // kg
       const double Ananke_MASS = 3.82e16;                     // kg
       const double Carme_MASS = 9.56e16;                     // kg
       const double Pasiphae_MASS = 1.91e17;                     // kg
       const double Sinope_MASS = 7.77e16;                     // kg

// Radius of the Solar System bodies

       const double Sun_RADIUS = 695000000;                     // m
       const double Mercury_RADIUS = 2440000;                     // m
       const double Venus_RADIUS = 6052000;                     // m
       const double Earth_RADIUS = 6378000;                     // m
       const double Mars_RADIUS = 3397000;                     // m
       const double Jupiter_RADIUS = 71492000;                     // m
       const double Saturn_RADIUS = 60268000;                     // m
       const double Uranus_RADIUS = 25559000;                     // m
       const double Neptune_RADIUS = 24766000;                     // m
       const double Pluto_RADIUS = 1150000;                     // m
       const double Moon_RADIUS = 1738000;                     // m
       const double Phobos_RADIUS = 11000;                     // m
       const double Deimos_RADIUS = 6000;                     // m
       const double Metis_RADIUS = 20000;                     // m
       const double Adrastea_RADIUS = 10000;                     // m
       const double Amalthea_RADIUS = 94000;                     // m
       const double Thebe_RADIUS = 50000;                     // m
       const double Io_RADIUS = 1821000;                     // m
       const double Europa_RADIUS = 1565000;                     // m
       const double Ganymede_RADIUS = 2634000;                     // m
       const double Callisto_RADIUS = 2403000;                     // m
       const double Leda_RADIUS = 8000;                     // m
       const double Himalia_RADIUS = 93000;                     // m
       const double Lysithea_RADIUS = 18000;                     // m
       const double Elara_RADIUS = 38000;                     // m
       const double Ananke_RADIUS = 15000;                     // m
       const double Carme_RADIUS = 20000;                     // m
       const double Pasiphae_RADIUS = 25000;                     // m
       const double Sinope_RADIUS = 18000;                     // m



// Gravity parameters of the Solar System bodies (i.e. mu = GRAVITATIONAL_CONSTANT * Planet_MASS)

       const double Sun_GRAVITY_PARAMETER = 1.328182e20;                    // m^3 s^-2
       const double Mercury_GRAVITY_PARAMETER = 2.202512e13;                   // m^3 s^-2
       const double Venus_GRAVITY_PARAMETER = 3.250374e14;                    // m^3 s^-2
       const double Earth_GRAVITY_PARAMETER = 3.984545e14;                  // m^3 s^-2
       const double Mars_GRAVITY_PARAMETER = 4.284888e13;                     // m^3 s^-2
       const double Jupiter_GRAVITY_PARAMETER = 1.268113e17;                    // m^3 s^-2
       const double Saturn_GRAVITY_PARAMETER = 3.790991e16;                    // m^3 s^-2
       const double Uranus_GRAVITY_PARAMETER = 5.793275e15;                    // m^3 s^-2
       const double Neptune_GRAVITY_PARAMETER = 6.807766e15;                    // m^3 s^-2
       const double Pluto_GRAVITY_PARAMETER = 8.476336e11;                    // m^3 s^-2
       const double Moon_GRAVITY_PARAMETER = 4.905596e12;                    // m^3 s^-2
       const double Phobos_GRAVITY_PARAMETER = 7.208222e05;                    // m^3 s^-2
       const double Deimos_GRAVITY_PARAMETER = 1.201370e05;                    // m^3 s^-2
       const double Metis_GRAVITY_PARAMETER = 6.380612e06;                    // m^3 s^-2
       const double Adrastea_GRAVITY_PARAMETER = 1.274787e06;                    // m^3 s^-2
       const double Amalthea_GRAVITY_PARAMETER = 2.335998e08;                    // m^3 s^-2
       const double Thebe_GRAVITY_PARAMETER = 5.185916e07;                    // m^3 s^-2
       const double Io_GRAVITY_PARAMETER = 5.960132e12;                    // m^3 s^-2
       const double Europa_GRAVITY_PARAMETER = 3.203654e12;                    // m^3 s^-2
       const double Ganymede_GRAVITY_PARAMETER = 9.877934e12;                    // m^3 s^-2
       const double Callisto_GRAVITY_PARAMETER = 7.208222e12;                    // m^3 s^-2
       const double Leda_GRAVITY_PARAMETER = 3.790991e05;                    // m^3 s^-2
       const double Himalia_GRAVITY_PARAMETER = 6.380612e08;                    // m^3 s^-2
       const double Lysithea_GRAVITY_PARAMETER = 5.185916e06;                    // m^3 s^-2
       const double Elara_GRAVITY_PARAMETER = 5.185916e07;                    // m^3 s^-2
       const double Ananke_GRAVITY_PARAMETER = 2.549575e06;                    // m^3 s^-2
       const double Carme_GRAVITY_PARAMETER = 6.380612e06;                    // m^3 s^-2
       const double Pasiphae_GRAVITY_PARAMETER = 1.274787e07;                    // m^3 s^-2
       const double Sinope_GRAVITY_PARAMETER = 5.185916e06;                    // m^3 s^-2


// String constants

        const char QUOTE[] = "''";


#endif // CONSTANTS_H
