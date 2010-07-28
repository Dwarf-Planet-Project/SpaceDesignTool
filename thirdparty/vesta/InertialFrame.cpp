/*
 * $Revision: 223 $ $Date: 2010-03-30 05:44:44 -0700 (Tue, 30 Mar 2010) $
 *
 * Copyright by Astos Solutions GmbH, Germany
 *
 * this file is published under the Astos Solutions Free Public License
 * For details on copyright and terms of use see 
 * http://www.astos.de/Astos_Solutions_Free_Public_License.html
 */

#include "InertialFrame.h"
#include "Units.h"

using namespace vesta;
using namespace Eigen;


// Standard frames
// The J2000 equator frame is the native frame of vesta
static InertialFrame* EquatorJ2000Instance = new InertialFrame(Quaterniond::Identity());
static InertialFrame* EclipticJ2000Instance = new InertialFrame(Quaterniond(AngleAxisd(toRadians(J2000_OBLIQUITY), Vector3d::UnitX())));
counted_ptr<InertialFrame> InertialFrame::s_EclipticJ2000(EclipticJ2000Instance);
counted_ptr<InertialFrame> InertialFrame::s_EquatorJ2000(EquatorJ2000Instance);

