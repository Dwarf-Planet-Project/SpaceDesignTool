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
 ------------------ Author: Chris Laurel  -------------------------------------------------
 ------------------ E-mail: (claurel@gmail.com) ----------------------------
 */
//#include "Analysis/analysis.h"
#include "stacoordsys.h"
#include "stamath.h"
#include "RotationState.h"
#include "stabody.h"
#include "calendarTOjulian.h"

#include "Scenario/staschema.h"
#include "Main/propagatedscenario.h"

using namespace sta;
using namespace Eigen;

typedef Eigen::Matrix< double, 3, 3 > 	MyMatrix3d;
typedef Eigen::Matrix< double, 3, 1 > 	MyVector3d;

/*
 ICRF Transformation matrix:
 The transformation matrix between ICRF and EMEJ2000 is taken from:
 "The coordinate frames of the US space object catalogs", John H. Seago, David A. Vallado,AIAA 2000-4025
 This transformation is from Ecliptic J2000 to ICRF*/

static double J2000_to_ICRFCoeffs[9] =
{1,             0.000273e-8,         -9.740996e-8,
-0.000273e-8,        1,             -1.324146e-8,
9.740996e-8,    1.324146e-8,                1};

static const MyMatrix3d J2000_to_ICRF_mat(J2000_to_ICRFCoeffs);
static const MyMatrix3d ICRF_to_J2000_mat=J2000_to_ICRF_mat.conjugate();
static Quaterniond J2000_to_ICRF(J2000_to_ICRF_mat);
static Quaterniond ICRF_to_J2000 = J2000_to_ICRF.conjugate();

// ======================= PRECESSION matrix ===================================
//Formulas for precession taken from: "Satellite Orbits: models, methods and applications",
//Montenbruck O., Gill E., Springer, 2005
MyMatrix3d PrecessionMatrix(double mjd)
{
        double julianTDB = sta::MjdToJd(mjd+0.00001);
        double julianSeconds = JdToSecJ2000(julianTDB);
        double julianTT_seconds = convertTDBtoTT(julianSeconds);
        double julianTT = SecJ2000ToJd(julianTT_seconds);
        double T = (julianTT-2451545.0)/36525.0;

    // Precession angles
    double zeta = 2306.2181*T + 0.30188*T*T + 0.017998*T*T*T; //seconds
    double vartheta = 2004.3109*T - 0.42665*T*T - 0.041833*T*T*T;
    double z = zeta + 0.79280*T*T + 0.000205*T*T*T;

    zeta = sta::degToRad(zeta/3600);  //radians
    vartheta = sta::degToRad(vartheta/3600);
    z = sta::degToRad(z/3600);

    static double PrecessionCoeffs[9]=
    {
        -sin(z)*sin(zeta)+cos(z)*cos(vartheta)*cos(zeta), cos(z)*sin(zeta)+sin(z)*cos(vartheta)*cos(zeta),sin(vartheta)*cos(zeta),
        -sin(z)*cos(zeta)-cos(z)*cos(vartheta)*sin(zeta) , cos(z)*cos(zeta)-sin(z)*cos(vartheta)*sin(zeta),-sin(vartheta)*sin(zeta),
        -cos(z)*sin(vartheta),-sin(z)*sin(vartheta),cos(vartheta)
    };

    static const MyMatrix3d Precession_mat(PrecessionCoeffs);     // Convert the array into a matrix
    return Precession_mat;


}

//==================================================================================

//================================ NUTATION matrix =================================
//Formulas for nutation taken from: "Satellite Orbits: models, methods and applications", Montenbruck O., Gill E., Springer, 2005
MyMatrix3d NutationMatrix(double mjd)
{
    double julianTDB = sta::MjdToJd(mjd+0.00001);
    double julianSeconds = JdToSecJ2000(julianTDB);
    double julianTT_seconds = convertTDBtoTT(julianSeconds);
    double julianTT = SecJ2000ToJd(julianTT_seconds);
    double T = (julianTT-2451545.0)/36525.0;

    struct NutationTable
    {
        int p_l;
        int p_ll;
        int p_D;
        int  p_F;
        int p_Omg;
        double delta_psi_const;
        double delta_psi_T;
        double delta_eps_const;
        double delta_eps_T;
    };
    // p_l, p_ll, p_D, p_F, p_Omg, delta_psi_const, delta_psi_T, delta_eps_const, delta_eps_T
    static NutationTable nutationAngles[106]=
    {
        {0,0,0,0,1,    -171996,    -174.2,    92025,    8.9},
        {0,0,0,0,2,     2062,      0.2,       -895,     0.5},
        {-2,0,2,0,1,    46,         0,          -24,         0},
        {2,0,-2,0,0,    11,         0,            0,         0},
        {-2,0,2,0,2,    -3,         0,            1,         0},
        {1,-1,0,-1,0,   -3,         0,            0,         0},
        {0,-2,2,-2,1,   -2,         0,            1,         0},
        {2,0,-2,0,1,     1,         0,            0,         0},
        {0,0,2,-2,2,    -13187,   -1.6,        5736,    -3.1},
        {0,1,0,0,0,      1426,    -3.4,         54,     -0.1},
        {0,1,2,-2,2,    -517,      1.2,        224,     -0.6},
        {0,-1,2,-2,2,    217,     -0.5,        -95,      0.3},
        {0,0,2,-2,1,     129,      0.1,        -70,         0},
        {2,0,0,-2,0,      48,       0,            1,          0},
        {0,0,2,-2,0,     -22,       0,            0,          0},
        {0,2,0,0,0,       17,     -0.1,         0,          0},
        {0,1,0,0,1,      -15,       0,            9,          0},
        {0,2,2,-2,2,     -16,       0.1,        7,          0},
        {0,-1,0,0,1,     -12,       0,            6,          0},
        {-2,0,0,2,1,     -6,        0,            3,          0},
        {0,-1,2,-2,1,    -5,        0,            3,          0},
        {2,0,0,-2,1,      4,        0,           -2,          0},
        {0,1,2,-2,1,      4,        0,           -2,          0},
        {1,0,0,-1,0,     -4,        0,            0,          0},
        {2,1,0,-2,0,      1,        0,            0,          0},
        {0,0,-2,2,1,      1,        0,            0,          0},
        {0,1,-2,2,0,     -1,        0,            0,          0},
        {0,1,0,0,2,       1,        0,            0,          0},
        {-1,0,0,1,1,      1,        0,            0,          0},
        {0,1,2,-2,0,     -1,        0,            0,          0},
        {0,0,2,0,2,     -2274,   -0.2,         977,       -0.5},
        {1,0,0,0,0,      712,     0.1,          -7,         0},
        {0,0,2,0,1,     -386,    -0.4,         200,         0},
        {1,0,2,0,2,     -301,       0,           129,       -0.1},
        {1,0,0,-2,0,    -158,       0,          -1,           0},
        {-1,0,2,0,2,    123,        0,          -53,          0},
        {0,0,0,2,0,     63,         0,          -2,           0},
        {1,0,0,0,1,     63,     0.1,          -33,          0},
        {-1,0,0,0,1,    -58,    -0.1,         32,           0},
        {-1,0,2,2,2,    -59,        0,          26,           0},
        {1,0,2,0,1,     -51,        0,          27,           0},
        {0,0,2,2,2,     -38,        0,          16,           0},
        {2,0,0,0,0,     29,         0,          -1,           0},
        {1,0,2,-2,2,    29,         0,          -12,          0},
        {2,0,2,0,2,     -31,        0,          13,           0},
        {0,0,2,0,0,     26,         0,          -1,           0},
        {-1,0,2,0,1,    21,         0,          -10,          0},
        {-1,0,0,2,1,    16,         0,          -8,           0},
        {1,0,0,-2,1,    -13,        0,          7,            0},
        {-1,0,2,2,1,    -10,        0,          5,            0},
        {1,1,0,-2,0,    -7,         0,          0,            0},
        {0,1,2,0,2,     7,          0,          -3,           0},
        {0,-1,2,0,2,    -7,         0,          3,            0},
        {1,0,2,2,2,     -8,         0,          3,            0},
        {1,0,0,0,0,     6,          0,          0,            0},
        {2,0,2,-2,2,    6,          0,          -3,             0},
        {0,0,0,2,1,     -6,         0,          3,              0},
        {0,0,2,2,1,     -7,         0,          3,              0},
        {1,0,2,-2,1,     6,         0,          -3,             0},
        {0,0,0,-2,1,     -5,        0,          3,              0},
        {1,-1,0,0,0,     5,         0,          0,              0},
        {2,0,2,0,1,     -5,         0,          3,              0},
        {0,1,0,-2,0,    -4,         0,          0,              0},
        {1,0,-2,0,0,     4,         0,          0,              0},
        {0,0,0,1,0,     -4,         0,          0,              0},
        {1,1,0,0,0,     -3,         0,          0,              0},
        {1,0,2,0,0,     3,          0,          0,              0},
        {1,-1,2,0,2,    -3,         0,          1,              0},
        {-1,-1,2,2,2,   -3,         0,          1,              0},
        {-2,0,0,0,1,    -2,         0,          1,              0},
        {3,0,2,0,2,     -3,         0,          1,              0},
        {0,-1,2,2,2,    -3,         0,          1,              0},
        {1,1,2,0,2,      2,         0,          -1,             0},
        {-1,0,2,-2,1,   -2,         0,          1,              0},
        {2,0,0,0,1,      2,         0,          -1,             0},
        {1,0,0,0,2,     -2,         0,          1,              0},
        {3,0,0,0,0,     2,          0,          0,              0},
        {0,0,2,1,2,     2,          0,          -1,             0},
        {-1,0,0,0,2,    1,          0,          -1,             0},
        {1,0,0,-4,0,    -1,         0,          0,              0},
        {-2,0,2,2,2,    1,          0,          -1,             0},
        {-1,0,2,4,2,    -2,         0,          1,              0},
        {2,0,0,-4,0,    -1,         0,          0,              0},
        {1,1,2,-2,2,    1,          0,          -1,             0},
        {1,0,2,2,1,     -1,         0,          1,              0},
        {-2,0,2,4,2,    -1,         0,          1,              0},
        {-1,0,4,0,2,    1,          0,          0,              0},
        {1,-1,0,-2,0,   1,          0,          0,              0},
        {2,0,2,-2,1,    1,          0,          -1,             0},
        {2,0,2,2,2,     -1,         0,          0,              0},
        {1,0,0,2,1,     -1,         0,          0,              0},
        {0,0,4,-2,2,    1,          0,          0,              0},
        {3,0,2,-2,2,    1,          0,          0,              0},
        {1,0,2,-2,0,    -1,         0,          0,              0},
        {0,1,2,0,1,     1,          0,          0,              0},
        {-1,-1,0,2,1,   1,          0,          0,              0},
        {0,0,-2,0,1,    -1,         0,          0,              0},
        {0,0,2,-1,2,    -1,         0,          0,              0},
        {0,1,0,2,0,     -1,         0,          0,              0},
        {1,0,-2,-2,0,   -1,         0,          0,              0},
        {0,-1,2,0,1,    -1,         0,          0,              0},
        {1,1,0,-2,1,    -1,         0,          0,              0},
        {1,0,-2,2,0,    -1,         0,          0,              0},
        {2,0,0,2,0,     1,          0,          0,              0},
        {0,0,2,4,2,     -1,         0,          0,              0},
        {0,1,0,1,0,     1,          0,          0,              0},
};

    // All the angles are in radians
    double  l =134*3600+57*60+46.733 + (477198*3600+52*60+2.633)*T + (31.310)*T*T + (0.064)*T*T*T;
    double  ll =357*3600+31*60+39.804 +(35999*3600+3*60+1.224)*T -(0.577)*T*T - (0.012)*T*T*T;
    double  F = 93*3600+16*60+18.877 + (483202*3600+1*60+3.137)*T - (13.257)*T*T + (0.011)*T*T*T;
    double  D = 297*3600+51*60+1.307 + (445267*3600+6*60+41.328)*T - (6.891)*T*T + (0.019)*T*T*T;
    double  Omg =125*3600+2*60+40.280 - (1934*3600+8*60+10.539)*T + (7.455)*T*T + (0.008)*T*T*T;

    double delta_eps=0;
    double delta_psi=0;
    for(int i=0; i< 106; i++)
    {
        double phi = nutationAngles[i].p_l*l + nutationAngles[i].p_ll*ll + nutationAngles[i].p_F*F
                     + nutationAngles[i].p_D*D + nutationAngles[i].p_Omg*Omg;
        phi = sta::degToRad(phi/3600);
        delta_eps = delta_eps + (nutationAngles[i].delta_eps_const + nutationAngles[i].delta_eps_T*T)*sta::degToRad(1/3600)*cos(phi);
        delta_psi = delta_psi + (nutationAngles[i].delta_psi_const + nutationAngles[i].delta_psi_T*T)*sta::degToRad(1/3600)*sin(phi);
    }


    double eps = sta::degToRad(23.43929111-(46.8150/3600)*T-(0.00059/3600)*T*T+(0.001813/3600)*T*T*T);

//    delta_eps = sta::degToRad(delta_eps);
//    delta_psi = sta::degToRad(delta_psi);

    double eps_l = eps + delta_eps;


    // Nutation matrix
    static double NutationCoeffs[9]=
    {
        cos(delta_psi),  cos(eps_l)*sin(delta_psi),  sin(eps_l)*sin(delta_psi),
        -cos(eps)*sin(delta_psi),   cos(eps)*cos(eps_l)*cos(delta_psi)+sin(eps)*sin(eps_l),cos(eps)*sin(eps_l)*cos(delta_psi)-sin(eps)*cos(eps_l),
        -sin(eps)*sin(delta_psi),sin(eps)*cos(eps_l)*cos(delta_psi)-cos(eps)*sin(eps_l),sin(eps)*sin(eps_l)*cos(delta_psi)+cos(eps)*cos(eps_l)
    };

//    static double NutationCoeffs[9]=
//    {
//        cos(delta_psi),  -cos(eps)*sin(delta_psi),   -sin(eps)*sin(delta_psi),
//        cos(eps_l)*sin(delta_psi), cos(eps)*cos(eps_l)*cos(delta_psi)+sin(eps)*sin(eps_l), sin(eps)*cos(eps_l)*cos(delta_psi)-cos(eps)*sin(eps_l),
//        sin(eps_l)*sin(delta_psi), cos(eps)*sin(eps_l)*cos(delta_psi)-sin(eps)*cos(eps_l), sin(eps)*sin(eps_l)*cos(delta_psi)+cos(eps)*cos(eps_l)
//    };
    static const MyMatrix3d Nutation_mat(NutationCoeffs);
    return Nutation_mat;
}
//==================================================================================

// ---------TEST----------
//static double NutationCoeffs[9]=
//{
//    1, 0.000075, 0.000033,
//    -0.000075, 1, -0.000015,
//    -0.000033, 0.000015, 1
//};
//static const MyMatrix3d J2000_to_Nutation_mat(NutationCoeffs);
//static const MyMatrix3d Nutation_to_J2000_mat=J2000_to_Nutation_mat.conjugate();
//static Quaterniond J2000_to_Nutation(J2000_to_Nutation_mat);
//static Quaterniond Nutation_to_J2000 = J2000_to_Nutation.conjugate();

/*
Constants from SPICE's Frames Required Reading document
http://naif.jpl.nasa.gov/pub/naif/toolkit_docs/C/req/frames.html#Inertial%20Frames
B1950:
z     =  1153.04066200330"
theta = -1002.26108439117"
zeta  =  1152.84248596724"

The transformation is given by Rz(-z) * Ry(theta) * Rz(-zeta)

Jay Lieske, ``Expressions for the Precession  Quantities
       Based upon the IAU (1976) System of Astronomical
       Constants'' Astron. Astrophys. 58, 1-16 (1977)
*/

static const Quaterniond B1950_to_J2000 =
    Quaterniond(AngleAxis<double>(arcsecToRad(1153.04066200330), MyVector3d::UnitZ())) *
    Quaterniond(AngleAxis<double>(arcsecToRad(-1002.26108439117), MyVector3d::UnitY())) *
    Quaterniond(AngleAxis<double>(arcsecToRad(1152.84248596724), MyVector3d::UnitZ()));
static const MyMatrix3d B1950_to_J2000_mat = B1950_to_J2000.toRotationMatrix();
static const MyMatrix3d J2000_to_B1950_mat = B1950_to_J2000.conjugate().toRotationMatrix();

/*
ECLIPTIC J2000:
The value for the obliquity of the ecliptic at J2000 is taken from
"Explanatory Supplement to the Astronomical Almanac", P. Kenneth Seidelmann, 1992.
p. 114, equation 3.222-1:
e0 = 23 26' 21".448 - 46".8150 T - 0".00059 T^2 + 0".001813 T^3
*/
static const Quaterniond Equator_to_Ecliptic =
    Quaterniond(AngleAxis<double>(degToRad(-23.439291111), MyVector3d::UnitX())); //patched by Ana (June 2010)
static const MyMatrix3d Equator_to_Ecliptic_mat = Equator_to_Ecliptic.toRotationMatrix();
static const MyMatrix3d Ecliptic_to_Equator_mat = Equator_to_Ecliptic.conjugate().toRotationMatrix();


/*! Create a coordinate system with the specified name. The name
 *  should be one of the strings allowed in a space scenario file.
 */
CoordinateSystem::CoordinateSystem(QString name) :
    m_type(COORDSYS_INVALID)
{
    if (name == "INERTIAL J2000")
        m_type = COORDSYS_EME_J2000;
    else if (name == "INERTIAL B1950")
        m_type = COORDSYS_EME_B1950;
    else if (name == "ECLIPTIC")
        m_type = COORDSYS_ECLIPTIC_J2000;
    else if (name == "PLANETO FIXED")
        m_type = COORDSYS_BODYFIXED;
    else if (name == "ICRF")
        m_type = COORDSYS_ICRF;
    else if (name == "Mean of Date")
        m_type = COORDSYS_MEAN_OF_DATE;
    else if (name == "True of Date")
        m_type = COORDSYS_TRUE_OF_DATE;
    else if (name == "Mean of Epoch")
        m_type = COORDSYS_MEAN_OF_EPOCH;

}


/*! Return the coordinate system name. This will be a string that may be
 *  to specify the coordinate system in an STA space scenario file.
 */
QString
CoordinateSystem::name() const
{
    switch (m_type)
    {
    case COORDSYS_EME_J2000:
        return "INERTIAL J2000";
    case COORDSYS_EME_B1950:
        return "INERTIAL B1950";
    case COORDSYS_ECLIPTIC_J2000:
        return "ECLIPTIC";
    case COORDSYS_BODYFIXED:
        return "PLANETO FIXED";
    case COORDSYS_ICRF:
        return "ICRF";
    case  COORDSYS_MEAN_OF_DATE:
        return "Mean of Date";
    case COORDSYS_TRUE_OF_DATE:
      return "True of Date";
    case COORDSYS_MEAN_OF_EPOCH:
      return "Mean of Epoch";

    case COORDSYS_INVALID:
    default:
        return "INVALID";
    }
}


/*! Convert a state vector from this coordinate system to
 *  the Earth Mean Equator J2000.0 system.
 *  (Only works for inertial systems.)
 */
sta::StateVector
CoordinateSystem::toEmeJ2000(const StateVector& v) const
{
    Q_ASSERT(isInertial());

//    if (m_type == COORDSYS_EME_J2000)
//        return v;

    MyMatrix3d rotation;
    switch (m_type)
    {
    case COORDSYS_EME_J2000:
        rotation = MyMatrix3d::Identity();
        break;
    case COORDSYS_EME_B1950:
        rotation = B1950_to_J2000_mat;
        break;
    case COORDSYS_ECLIPTIC_J2000:
        rotation = Ecliptic_to_Equator_mat;
        break;
    case COORDSYS_ICRF:
        rotation = ICRF_to_J2000_mat;
        break;
    case COORDSYS_BODYFIXED:
    default:
        rotation = MyMatrix3d::Identity();
        break;
    }

    return StateVector(rotation * v.position, rotation * v.velocity);
}


/*! Convert a state vector to this coordinate system from
 *  the Earth Mean Equator J2000.0 system.
 *  (Only works for inertial systems.)
 */
sta::StateVector
CoordinateSystem::fromEmeJ2000(const StateVector& v) const
{
    Q_ASSERT(isInertial());

//    if (m_type == COORDSYS_EME_J2000)
//        return v;

    MyMatrix3d rotation;
    switch (m_type)
    {
    case COORDSYS_EME_J2000:
        rotation = MyMatrix3d::Identity();
        break;
    case COORDSYS_EME_B1950:
        rotation = J2000_to_B1950_mat;
        break;
    case COORDSYS_ECLIPTIC_J2000:
        rotation = Equator_to_Ecliptic_mat;
        break;

    case COORDSYS_BODYFIXED:
    case COORDSYS_ICRF:
        rotation = J2000_to_ICRF_mat;
        break;
    default:
        rotation = MyMatrix3d::Identity();
        break;
    }

    return StateVector(rotation * v.position, rotation * v.velocity);
}


static StateVector bodyFixedToEmeJ2000(const StateVector& state,
                                       const StaBody* center,
                                       double mjd)
{
    const RotationState* rotation = center->rotationState();
    MyMatrix3d r = MyMatrix3d::Identity();
    MyVector3d w = MyVector3d::Zero();
    if (rotation)
    {
        r = rotation->orientation(mjd).toRotationMatrix();
        w = rotation->angularVelocity(mjd);
    }

    return StateVector(r * state.position, r * state.velocity + w.cross(r*state.position));
}


static StateVector emeJ2000ToBodyFixed(const StateVector& state,
                                       const StaBody* center,
                                       double mjd)
{
    const RotationState* rotation = center->rotationState();
    MyMatrix3d r = MyMatrix3d::Identity();
    MyVector3d w = MyVector3d::Zero();
    if (rotation)
    {
        r = rotation->orientation(mjd).conjugate().toRotationMatrix();
        w = -rotation->angularVelocity(mjd);
    }

    return StateVector(r * state.position, r * state.velocity + w.cross(r*state.position));
}

/*! Convert a state vector from this coordinate system to
 *  the Earth Mean Equator J2000.0 system.
 *  (Only works for inertial and body fixed systems.)
 */
sta::StateVector
CoordinateSystem::toEmeJ2000(const StateVector& v, const StaBody* center, double mjd) const
{
    Q_ASSERT(isInertial() || m_type == COORDSYS_BODYFIXED);
    if (m_type == COORDSYS_BODYFIXED)
        return bodyFixedToEmeJ2000(v, center,mjd);

    else if (m_type == COORDSYS_MEAN_OF_DATE)
    {
        //calculates the precession matrix to transform from ICRF to Mean of Date
        MyMatrix3d rotation = PrecessionMatrix(mjd);
        // calculates the precession matrix to transform from Mean of Date to ICRF
       rotation = rotation.conjugate();
        // r (MoD) = P(MoD to ICRF) * T (ICRF to J2000)
        rotation = ICRF_to_J2000_mat*rotation;
        return StateVector(rotation * v.position, rotation * v.velocity);
    }

    else if (m_type == COORDSYS_MEAN_OF_EPOCH)
    {
        //calculates the precession matrix to transform from ICRF to Mean of Date
        MyMatrix3d rotation = PrecessionMatrix(mjd);
        // calculates the precession matrix to transform from Mean of Date to ICRF
       rotation = rotation.conjugate();
        // r (MoD) = P(MoD to ICRF) * T (ICRF to J2000)
        rotation = ICRF_to_J2000_mat*rotation;
        return StateVector(rotation * v.position, rotation * v.velocity);
    }

    //This transformation does not have the correct results, because it depends on Mean of date. And this RF
    //is still giving wrong results... Still don't know why...
    else if (m_type == COORDSYS_TRUE_OF_DATE)
    {
       // calculates de nutation matrix
       MyMatrix3d nutation = NutationMatrix(mjd);
       nutation = nutation.conjugate();
       // calculates the precession matrix
      MyMatrix3d precession = PrecessionMatrix(mjd);
      precession = precession.conjugate();
       // N = transformation from True of Date to Mean of Date
       // P = transformation from Mean of Date to ICRF
       // T = transformation from ICRF to EME J2000
       // r (ToD) = N * P * T    -> the rotations are made from right to left
       MyMatrix3d rotation = ICRF_to_J2000_mat*precession*nutation*Ecliptic_to_Equator_mat;

       return StateVector(rotation * v.position, rotation * v.velocity);
    }

//    else if (m_type == COORDSYS_TRUE_OF_DATE)
//    {
//        MyMatrix3d nutation = Nutation_to_J2000_mat;
//        MyMatrix3d precession = PrecessionMatrix(mjd);
//        MyMatrix3d rotation = precession*nutation*Ecliptic_to_Equator_mat;
//        return StateVector(rotation * v.position, rotation * v.velocity);
//    }
    else
        return toEmeJ2000(v);
}


/*! Convert a state vector to this coordinate system from
 *  the Earth Mean Equator J2000.0 system.
 *  (Only works for inertial and body fixed systems.)
 */
sta::StateVector
CoordinateSystem::fromEmeJ2000(const StateVector& v, const StaBody* center, double mjd) const
{
    Q_ASSERT(isInertial() || m_type == COORDSYS_BODYFIXED);
    if (m_type == COORDSYS_BODYFIXED)
        return emeJ2000ToBodyFixed(v, center, mjd);

    else if (m_type == COORDSYS_MEAN_OF_DATE)
    {
        // calculates the precession matrix to transform from ICRF to Mean of Date
        MyMatrix3d rotation = PrecessionMatrix(mjd);
        // r (MoD) = P(ICRF to MoD) * T (J2000 to ICRF)
        rotation = rotation*J2000_to_ICRF_mat;
        return StateVector(rotation * v.position, rotation * v.velocity);

    }
    else if (m_type == COORDSYS_MEAN_OF_EPOCH)
    {
        // calculates the precession matrix to transform from ICRF to Mean of Date
        MyMatrix3d rotation = PrecessionMatrix(mjd);
        // r (MoD) = P(ICRF to MoD) * T (J2000 to ICRF)
        rotation = rotation*J2000_to_ICRF_mat*Ecliptic_to_Equator_mat;
        return StateVector(rotation * v.position, rotation * v.velocity);

    }

//    This transformation does not have the correct results, because it depends on Mean of date. And this RF
//    is still giving wrong results... Still don't know why...
    else if (m_type == COORDSYS_TRUE_OF_DATE)
    {
       // Calculates the nutation matrix
       MyMatrix3d nutation = NutationMatrix(mjd);
       // Calculates the precession matrix
       MyMatrix3d precession = PrecessionMatrix(mjd);
       // N = transformation from Mean of Date to True of Date
       // P = transformation from ICRF to Mean of Date
       // T = transformation from EME J2000 to ICRF
       // r (ToD) = N * P * T    -> the rotations are made from right to left
        MyMatrix3d rotation =nutation*precession *J2000_to_ICRF_mat*Ecliptic_to_Equator_mat;
       return StateVector(rotation * v.position, rotation * v.velocity);
    }
//    else if (m_type == COORDSYS_TRUE_OF_DATE)
//    {
//        MyMatrix3d nutation = Nutation_to_J2000_mat;
//        MyMatrix3d precession = PrecessionMatrix(mjd);
//        MyMatrix3d rotation = nutation*precession;
//        return StateVector(rotation * v.position, rotation * v.velocity);
//    }
    else
        return fromEmeJ2000(v);
}


/*! General coordinate system conversion function: convert
 *  a state vector between any two coordinate system (though
 *  only inertial and body-fixed systems are supported right
 *  now.)
 */
sta::StateVector
CoordinateSystem::convert(const StateVector& state,
                          double mjd,
                          const StaBody* fromCenter,
                          const CoordinateSystem& fromSys,
                          const StaBody* toCenter,
                          const CoordinateSystem& toSys)
{
    // First, check for simple cases...

    // No conversion necessary.
    if (fromCenter == toCenter && fromSys == toSys)
    {
        return state;
    }

    // Simple case when coordinate systems are identical, but centers are
    // different.
    // Different centers may imply different body fixed coordinate systems,
    // so non-inertial systems are non handled here.
    if (fromSys == toSys && fromSys.isInertial())
    {
        return state + fromCenter->stateVector(mjd, toCenter, fromSys.type());
    }

    // Same center.
    if (fromCenter == toCenter)
    {
        return toSys.fromEmeJ2000(fromSys.toEmeJ2000(state, fromCenter, mjd), toCenter, mjd);
    }

    // Handle the general case:
    //    Convert the input state vector to an EME J2000 coordinate system centered at the Solar
    //    System Barycenter, then convert it to the target coordinate system.
    //    TODO: Reducing to SSB-centered coordinates is overkill in some situations
    StateVector emeState = fromSys.toEmeJ2000(state, fromCenter, mjd) +
        fromCenter->stateVector(mjd, STA_SOLAR_SYSTEM->ssb(), COORDSYS_EME_J2000);
    emeState = emeState - toCenter->stateVector(mjd, STA_SOLAR_SYSTEM->ssb(), COORDSYS_EME_J2000);
    return toSys.fromEmeJ2000(emeState, toCenter, mjd);
}
// ACT AVT

MyMatrix3d CoordinateSystem::rotToEmeJ2000()
{
    Q_ASSERT(isInertial());


    MyMatrix3d rotation;
    switch (m_type)
    {
    case COORDSYS_EME_J2000:
        rotation = MyMatrix3d::Identity();
        break;
    case COORDSYS_EME_B1950:
        rotation = B1950_to_J2000_mat;
        break;
    case COORDSYS_ECLIPTIC_J2000:
        rotation = Ecliptic_to_Equator_mat;
        break;

    case COORDSYS_BODYFIXED:
    case COORDSYS_ICRF:
        rotation = ICRF_to_J2000_mat;
        break;
    default:
        rotation = MyMatrix3d::Identity();
        break;
    }

    return rotation;
}

MyMatrix3d CoordinateSystem::rotFromEmeJ2000()
{
    Q_ASSERT(isInertial());

    MyMatrix3d rotation;
    switch (m_type)
    {
    case COORDSYS_EME_J2000:
        rotation = MyMatrix3d::Identity();
        break;
    case COORDSYS_EME_B1950:
        rotation = J2000_to_B1950_mat;
        break;
    case COORDSYS_ECLIPTIC_J2000:
        rotation = Equator_to_Ecliptic_mat;
        break;

    case COORDSYS_BODYFIXED:
    case COORDSYS_ICRF:
        rotation = J2000_to_ICRF_mat;
        break;

    default:
        rotation = MyMatrix3d::Identity();
        break;
    }
    return rotation;

}



static MyMatrix3d rotBodyFixedToEmeJ2000(const StaBody* center, double mjd)
{
    const RotationState* rotation = center->rotationState();
    MyMatrix3d r = MyMatrix3d::Identity();
    MyVector3d w = MyVector3d::Zero();
    if (rotation)
    {
        r = rotation->orientation(mjd).toRotationMatrix();
        w = rotation->angularVelocity(mjd);
    }

    return r;
}


static MyMatrix3d rotEmeJ2000ToBodyFixed(const StaBody* center, double mjd)
{
    const RotationState* rotation = center->rotationState();
    MyMatrix3d r = MyMatrix3d::Identity();
    MyVector3d w = MyVector3d::Zero();
    if (rotation)
    {
        r = rotation->orientation(mjd).conjugate().toRotationMatrix();
        w = -rotation->angularVelocity(mjd);
    }

    return r;
}

static MyVector3d omegaBodyFixedToEmeJ2000(const StaBody* center, double mjd)
{
    const RotationState* rotation = center->rotationState();
    MyMatrix3d r = MyMatrix3d::Identity();
    MyVector3d w = MyVector3d::Zero();
    if (rotation)
    {
        r = rotation->orientation(mjd).toRotationMatrix();
        w = rotation->angularVelocity(mjd);
    }

    return w;
}


static MyVector3d omegaEmeJ2000ToBodyFixed(const StaBody* center, double mjd)
{
    const RotationState* rotation = center->rotationState();
    MyMatrix3d r = MyMatrix3d::Identity();
    MyVector3d w = MyVector3d::Zero();
    if (rotation)
    {
        r = rotation->orientation(mjd).conjugate().toRotationMatrix();
        w = -rotation->angularVelocity(mjd);
    }

    return w;
}


MyMatrix3d CoordinateSystem::rotToEmeJ2000(const StaBody* center, double mjd)
{
    Q_ASSERT(isInertial() || m_type == COORDSYS_BODYFIXED);
    if (m_type == COORDSYS_BODYFIXED)
        return rotBodyFixedToEmeJ2000(center, mjd);
    else
        return rotToEmeJ2000();

}

MyMatrix3d CoordinateSystem::rotFromEmeJ2000(const StaBody* center, double mjd)
{
    Q_ASSERT(isInertial() || m_type == COORDSYS_BODYFIXED);
    if (m_type == COORDSYS_BODYFIXED)
        return rotEmeJ2000ToBodyFixed(center, mjd);
    else
        return rotFromEmeJ2000();
}

MyVector3d CoordinateSystem::omegaToEmeJ2000(const StaBody* center, double mjd)
{
    Q_ASSERT(isInertial() || m_type == COORDSYS_BODYFIXED);
    if (m_type == COORDSYS_BODYFIXED)
        return omegaBodyFixedToEmeJ2000(center, mjd);
    else
        return MyVector3d::Zero();

}

MyVector3d CoordinateSystem::omegaFromEmeJ2000(const StaBody* center, double mjd)
{
    Q_ASSERT(isInertial() || m_type == COORDSYS_BODYFIXED);
    if (m_type == COORDSYS_BODYFIXED)
        return omegaEmeJ2000ToBodyFixed(center, mjd);
    else
        return MyVector3d::Zero();
}

