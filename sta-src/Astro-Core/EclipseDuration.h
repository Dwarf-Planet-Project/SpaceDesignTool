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
 ------------------ Author: Ozgun YILMAZ    ------------------------------------------
 ------------------ email:ozgunus@yahoo.com ------------------------------------------
 */

#ifndef ECLIPSEDURATION_H
#define ECLIPSEDURATION_H

#include <QList>

#include "Astro-Core/statevector.h"
#include "Astro-Core/stabody.h"

#include <Eigen/Core>
using namespace Eigen;

#include <iostream>
using namespace std;


/* You need to give coordinates of the Planet, Star and
  *Spacecraft in Inertial Fixed coordinates
  * it is not important if it is Earth, Star centred
*/

class EclipseDuration
{
public:
    EclipseDuration();

    // Creates the Eclipse State vs Time Function of the mission for any module
    /*
      * StarLightTimeFunction gives 0 in Umbra, 0.5 in Penumbra
      * and 1 under Starlught conditions and fills "EclipseStarLight.stad"
    */

    void StarLightTimeFunction(QList<double>& sampleTimes,
                              QList<sta::StateVector>& SCCoordinates,
                              StaBody* Planet,
                              StaBody* StarRadius);

    // Creates UMBRA-PENUMBRA-DAYLIGHT details for the user
    //(for now it is not doing penumbra calculations, umbra include penumbra)
    void CreateEclipseDetailsFile();


    //Erase after Analysis module integration
    double MjdToFromEpoch(double StartEpoch, double mjd, QString Units);
    //Erase after Analysis module integration

private:
    /*
      Star Coordinates
    */
    bool IsSpacecraftInUmbra(Vector3d StarCoordinates,
                             Vector3d PlanetCoordinate,
                             Vector3d SpacecraftCoordinate);
    bool IsSpacecraftInPenumbra(Vector3d StarCoordinates,
                                Vector3d PlanetCoordinate,
                                Vector3d SpacecraftCoordinate);
    double m_PlanetMeanDiameter;
    double m_StarMeanDiameter;
};

#endif // ECLIPSEDURATION_H
