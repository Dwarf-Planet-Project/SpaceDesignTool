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

 ------------------ Author: Claas Grohnfeldt, Steffen Peter  -------------------------------------------------
 ------------------ E-mail: (claasgr@math.uni-bremen.de, spezisteffen@gmx.de) --------------------------------
 Modified by...
 */

#ifndef CONSTELLATIONMODULE_H
#define CONSTELLATIONMODULE_H

#include "ui_constellationwizard.h"
//#include "Main/propagatedscenario.h"
#include "Astro-Core/stabody.h"
#include "cstudy.h"
#include "discretization.h"
#include <QList>

class Antenna;

struct SatelliteKeplerian
{
    QString name; // satellite's name
    double param[6]; // eccentricity, inclination, RAAN, argument of periapsis, true anomaly
};

bool walkerConstellation(int numPl, int numSat, double sem, double incl, int f, SatelliteKeplerian* satellitekeplerian);

bool flowerConstellation(int np, int nd, int ns, double om, double inc, double hp, bool scheme, double omega1, double psi1, double domega,
                         double radiusCB, double omegaCB, double muCB, double j2CB, SatelliteKeplerian* satellitekeplerian);

bool selfDefinedConstellation(int n, SatelliteKeplerian* satellitekeplerian);


double getfa(double a, int np, int nd, double inc, double hp, double radiusCB, double omegaCB, double muCB, double j2CB);

double getfmean(double arg, double e, double mean);

// antennas stick always on transmitter
bool visibility(SpaceObject* transmitter, SpaceObject* receiver, const StaBody* body, double currentTime, QList<Antenna*> antennas);
bool visibility(SpaceObject* transmitter, GroundObject* receiver, const StaBody* body, double currentTime, QList<Antenna*> antennas);
bool visibility(GroundObject* transmitter, SpaceObject* receiver, const StaBody* body, double currentTime, QList<Antenna*> antennas);
bool visibility(SpaceObject* transmitter, DiscretizationPoint receiver, const StaBody* body, double currentTime, Antenna* obsAntenna);
QList<DiscretizationPoint> visiblePoints(SpaceObject* spObject, QList<DiscretizationPoint> allMeshPoints,const StaBody* body, double currentTime, Antenna* obsAntenna);

#endif // CONSTELLATIONMODULE_H


