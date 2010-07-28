/*
 * $Revision: 223 $ $Date: 2010-03-30 05:44:44 -0700 (Tue, 30 Mar 2010) $
 *
 * Copyright by Astos Solutions GmbH, Germany
 *
 * this file is published under the Astos Solutions Free Public License
 * For details on copyright and terms of use see 
 * http://www.astos.de/Astos_Solutions_Free_Public_License.html
 */

#include "StarCatalog.h"
#include <cmath>
#include <algorithm>

using namespace vesta;
using namespace std;


/** Create an empty star catalog.
  */
StarCatalog::StarCatalog()
{
}


StarCatalog::~StarCatalog()
{
}


/** Add a new star to the catalog.
  * @param ra the right ascension (in radians)
  * @param dec the declination (in radians)
  * @param vmag the apparent V magnitude in the Johnson photometric system (mean wavelength 540nm)
  * @param bv the value of B-V color index in the Johnson photometric system
  */
void
StarCatalog::addStar(double ra, double dec, double vmag, double bv)
{
    double cosDec = cos(dec);

    StarRecord star;
    star.x = (float) (cosDec * cos(ra));
    star.y = (float) (cosDec * sin(ra));
    star.z = (float) sin(dec);

    // TODO: Need to compute a star color based on B-V
    const float magThreshold = 7.0f;
    const float magSaturate = 0.0f;
    const float magRange = magThreshold - magSaturate;
    float appMag = (float) vmag;
    float brightness = std::min(1.0f, std::max(0.0f, (magThreshold - appMag) / magRange));
    star.color[3] = (unsigned char) (255.99f * brightness);

    if (bv < -3.0)
    {
        // Type O
        star.color[0] = 155; star.color[1] = 176; star.color[2] = 255;
    }
    else if (bv < 0.0)
    {
        // Type B
        star.color[0] = 170; star.color[1] = 191; star.color[2] = 255;
    }
    else if (bv < 0.3)
    {
        // Type A
        star.color[0] = 202; star.color[1] = 215; star.color[2] = 255;
    }
    else if (bv < 0.58)
    {
        // Type F
        star.color[0] = 248; star.color[1] = 247; star.color[2] = 255;
    }
    else if (bv < 0.81)
    {
        // Type G
        star.color[0] = 255; star.color[1] = 244; star.color[2] = 234;
    }
    else if (bv < 1.4)
    {
        // Type K
        star.color[0] = 255; star.color[1] = 210; star.color[2] = 161;
    }
    else
    {
        // Type M
        star.color[0] = 255; star.color[1] = 204; star.color[2] = 111;
    }

    m_starData.push_back(star);
}
