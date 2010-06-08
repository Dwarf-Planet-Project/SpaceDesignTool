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

    // Creates the Eclipse Function for the mission
    /*
      * StarLightTimeFunction gives 0 in Umbra, 0.5 in Penumbra
      * and 1 under Starlught conditions and fills "EclipseStarLight.stad"
    */

    void StarLightTimeFunction(QList<double>& sampleTimes,
                              QList<sta::StateVector>& PlanetCoordinates,
                              QList<sta::StateVector>& SCCoordinates,
                              StaBody* Planet,
                              StaBody* StarRadius);

    void StarLightTimeFunction(QList<double>& sampleTimes,
                              QList<sta::StateVector>& SCCoordinates,
                              StaBody* Planet,
                              StaBody* StarRadius);

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
