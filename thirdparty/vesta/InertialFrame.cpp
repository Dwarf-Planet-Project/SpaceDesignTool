/*
 * $Revision: 490 $ $Date: 2010-09-07 13:34:23 -0700 (Tue, 07 Sep 2010) $
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

static double secsToRadians(double secs)
{
    return toRadians(secs / 3600.0);
}

// Matrix coefficients from Seidelman, "Explanatory Supplement to the Astronomical Almanac" (1992), p. 312
static double B1950ToJ2000Coeffs[9] =
{
     0.9999256794956877,  0.0111814832391717,   0.0048590037723143,
    -0.0111814832204662,  0.9999374848933135,  -0.0000271702937440,
    -0.0048590038153592, -0.0000271625947142,   0.9999881946023742
};

static Matrix3d B1950ToJ2000Mat(B1950ToJ2000Coeffs);
static Quaterniond B1950ToJ2000(B1950ToJ2000Mat);
static Quaterniond J2000ToB1950 = B1950ToJ2000.conjugate();


// Fricke offset; this is already included in the B1950 to J2000 matrix. Not needed unless
// we want to create a B1950 frame without the offset. SPICE distinguishes between the B1950
// reference frame and the FK4/B1950 frame. Seidelman (Explanatory Supplement to the Astronomical
// Almanac, chapter 5) uses B1950 to mean the FK4/B1950.0 system.
// static Quaterniond FK4ToB1950 = Quaterniond(AngleAxisd(secsToRadians(0.525), Vector3d::UnitZ()));

// Rotations to convert from galactic coordinates to FK4/B1950.0
static Quaterniond GalacticToB1950 = Quaterniond(AngleAxisd(toRadians(282.25), Vector3d::UnitZ())) *
                                     Quaterniond(AngleAxisd(toRadians(62.6),   Vector3d::UnitX())) *
                                     Quaterniond(AngleAxisd(toRadians(327.0),  Vector3d::UnitZ()));

// Standard frames
// The J2000 equator frame is the native frame of vesta
static InertialFrame* EquatorJ2000Instance = new InertialFrame(Quaterniond::Identity());

static InertialFrame* EclipticJ2000Instance = new InertialFrame(Quaterniond(AngleAxisd(toRadians(J2000_OBLIQUITY), Vector3d::UnitX())));
static InertialFrame* EquatorB1950Instance = new InertialFrame(B1950ToJ2000);
//static InertialFrame* FK4Instance = new InertialFrame(B1950ToJ2000 * FK4ToB1950);
static InertialFrame* GalacticInstance = new InertialFrame(B1950ToJ2000 * GalacticToB1950);

counted_ptr<InertialFrame> InertialFrame::s_EclipticJ2000(EclipticJ2000Instance);
counted_ptr<InertialFrame> InertialFrame::s_EquatorJ2000(EquatorJ2000Instance);
counted_ptr<InertialFrame> InertialFrame::s_EquatorB1950(EquatorB1950Instance);
counted_ptr<InertialFrame> InertialFrame::s_Galactic(GalacticInstance);

