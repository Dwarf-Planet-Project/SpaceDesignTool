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
 ------ Copyright (C) 2011 STA Steering Board (space.trajectory.analysis AT gmail.com) ----
*/



//------------------ Author:       Guillermo Ortega               -------------------
//------------------ Affiliation:  European Space Agency (ESA)    -------------------
//-----------------------------------------------------------------------------------
// Created May 2011

#include <math.h>

#include "Astro-Core/statevector.h"
#include "Astro-Core/date.h"
#include "Astro-Core/stacoordsys.h"
#include "Astro-Core/stabody.h"
#include "Astro-Core/stamath.h"
#include "Astro-Core/calendarTOjulian.h"
#include "Astro-Core/cartesianTOorbital.h"
#include "Astro-Core/cartesianTOspherical.h"

#include "calculateElements.h"


sta::CoordinateSystem CoordSys(QString Coordinate)
{
    /*
  returns the name of the output coordinate system that shall be sent to the function that performs the transformation
*/
    if(Coordinate=="EME J2000")
    {
        sta::CoordinateSystem Coord("INERTIAL J2000");
        return Coord;
    }
    if(Coordinate=="EME B1950")
    {
        sta::CoordinateSystem Coord("INERTIAL B1950");
        return Coord;
    }
    if(Coordinate=="Fixed")
    {
        sta::CoordinateSystem Coord("PLANETO FIXED");
        return Coord;
    }
    if(Coordinate=="Ecliptic J2000")
    {
        sta::CoordinateSystem Coord("ECLIPTIC");
        return Coord;
    }
        if(Coordinate=="ICRF")
        {
            sta::CoordinateSystem Coord("ICRF");
            return Coord;
        }
        if(Coordinate=="Mean of Date")
        {
            sta::CoordinateSystem Coord("Mean of Date");
            return Coord;
        }
        if(Coordinate=="True of Date")
        {
            sta::CoordinateSystem Coord("True of Date");
            return Coord;
        }



    if(Coordinate==" ")
    {
        sta::CoordinateSystem Coord("INVALID");
        return Coord;
    }
}




/** Calculates the Keplerian Elements of a certain orbit
  * Inputs:
  *     Vector-statevector for each time step
  *     Body-pointer to the central Body
  *     OrbElement- name of the element the user wishes to analyse
  *     mjd-time in Modified Julian Date
  *     FromCoordinate- coordinate system of the data before transformation,
  *     ToCoordinate- coordinate system that shall appear as output)
  *
  * Outputs: parameter to be displayed/analysed
  */
double calcKeplerianElements(const sta::StateVector& Vector,
                                       const StaBody* Body,
                                       const QString& OrbElement,
                                       double mjd,
                                       const QString& FromCoordinate,
                                       const QString& ToCoordinate)
{
    sta::StateVector ModifVector=CoordinateSystem::convert(Vector,
                                                           mjd,
                                                           Body,
                                                           CoordSys(FromCoordinate),
                                                           Body,
                                                           CoordSys(ToCoordinate));
    //double mu = Earth_GRAVITY_PARAMETER;

    double mu=Body->mu();


    sta::KeplerianElements KepElemList=cartesianTOorbital(mu, ModifVector);

    double Element=0;
    if(OrbElement=="Eccentricity")
    {
        Element=KepElemList.Eccentricity;
    }
    if(OrbElement=="Inclination")
    {
        Element=KepElemList.Inclination;
    }
    if(OrbElement=="Semimajor Axis")
    {
        Element=KepElemList.SemimajorAxis;
    }
    if(OrbElement=="RAAN")
    {
        Element=KepElemList.AscendingNode;
    }
    if(OrbElement=="Argument of Periapsis")
    {
        Element=KepElemList.ArgumentOfPeriapsis;
    }
    if(OrbElement=="True Anomaly")
    {
        Element=KepElemList.TrueAnomaly;
    }

    return Element;
}


/** Calculates the Delaunay Elements of a certain orbit
  * Inputs:
  *     Vector-statevector for each time step
  *     Body-pointer to the central Body
  *     OrbElement- name of the element the user wishes to analyse
  *     mjd-time in Modified Julian Date
  *     FromCoordinate- coordinate system of the data before transformation,
  *     ToCoordinate- coordinate system that shall appear as output)
  *
  * Outputs: parameter to be displayed/analysed
  */
double calcDelaunayElements(const sta::StateVector& Vector,
                                      const StaBody* Body,
                                      const QString& OrbElement,
                                      double mjd,
                                      const QString& FromCoordinate,
                                      const QString& ToCoordinate)
{
    sta::StateVector ModifVector=CoordinateSystem::convert(Vector,
                                                           mjd,
                                                           Body,
                                                           CoordSys(FromCoordinate),
                                                           Body,
                                                           CoordSys(ToCoordinate));
    double mu = Body->mu();
    //qDebug()<<mu-1000000<<"mu del";
    sta::DelaunayElements DelaunayElem=cartesianTOdelaunay(mu, ModifVector);

    double Element=0;
    if(OrbElement=="l")
    {
        Element=DelaunayElem.l;
    }
    if(OrbElement=="g")
    {
        Element=DelaunayElem.g;
    }
    if(OrbElement=="h")
    {
        Element=DelaunayElem.h;
    }
    if(OrbElement=="L")
    {
        Element=(DelaunayElem.L);
    }
    if(OrbElement=="G")
    {
        Element=DelaunayElem.G;
    }
    if(OrbElement=="H")
    {
        Element=DelaunayElem.H;
    }

    return Element;
}


/** Calculates the Equinoctial Elements of a certain orbit
  *
  * Inputs:Vector-statevector for each time step,
  *        Body-pointer to the central Body
  *        OrbElement- name of the element the user wishes to analyse
  *        mjd-time in Modified Julian Date,
  *        FromCoordinate- coordinate system of the data before transformation,
  *        ToCoordinate- coordinate system that shall appear as output)
  * Outputs: parameter to be displayed/analysed
  */
double calcEquinoctialElements(const sta::StateVector& Vector,
                                         const StaBody* Body,
                                         const QString& OrbElement,
                                         double mjd,
                                         const QString& FromCoordinate,
                                         const QString& ToCoordinate)
{
    sta::StateVector ModifVector=CoordinateSystem::convert(Vector,
                                                           mjd,
                                                           Body,
                                                           CoordSys(FromCoordinate),
                                                           Body,
                                                           CoordSys(ToCoordinate));
    double mu = Body->mu();
    sta::EquinoctialElements EquinoctialElem=cartesianTOequinoctial(mu, ModifVector);

    double Element=0;
    if(OrbElement=="Semimajor Axis")
    {
        Element=EquinoctialElem.SemimajorAxis;
    }
    if(OrbElement=="e*cos(omegaBar)")
    {
        Element=EquinoctialElem.ecos;
    }
    if(OrbElement=="e*sin(omegaBar)")
    {
        Element=EquinoctialElem.esin;
    }
    if(OrbElement=="Mean Longitude")
    {
        Element=EquinoctialElem.MeanLon;
    }
    if(OrbElement=="tan(i/2)*sin(raan)")
    {
        Element=EquinoctialElem.tansin;
    }
    if(OrbElement=="tan(i/2)*cos(raan)")
    {
        Element=EquinoctialElem.tancos;
    }

    return Element;
}
