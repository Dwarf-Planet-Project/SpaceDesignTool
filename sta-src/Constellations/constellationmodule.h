/*
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
 ------ Copyright (C) 2009 European Space Agency (space.trajectory.analysis AT gmail.com) ----
 ------------------ Author: Claas Grohnfeldt, Steffen Peter  -------------------------------------------------
 ------------------ E-mail: (claasgr@math.uni-bremen.de, spezisteffen@gmx.de) --------------------------------
 Modified by...
 */

#ifndef CONSTELLATIONMODULE_H
#define CONSTELLATIONMODULE_H

#include "ui_constellationwizard.h"
//#include "Main/propagatedscenario.h"
#include "Astro-Core/stabody.h"
#include "canalysis.h"
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
bool visibility(GroundObject* transmitter, SpaceObject* receiver, const StaBody* body, double currentTime, Antenna* gAntenna);
bool visibility(SpaceObject* transmitter, DiscretizationPoint receiver, const StaBody* body, double currentTime, Antenna* obsAntenna);
QList<DiscretizationPoint> visiblePoints(SpaceObject* spObject, QList<DiscretizationPoint> allMeshPoints,const StaBody* body, double currentTime, Antenna* obsAntenna);

#endif // CONSTELLATIONMODULE_H


