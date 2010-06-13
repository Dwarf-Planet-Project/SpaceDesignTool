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
 ------------------ Author: Tiziana Sabatini  ------------------------------------------------
 ------------------ E-mail: (tiziana.sabatini@yahoo.it) --------------------------------------
 Patched by Guillermo to correct the behaviour of the + button on the 3rd body selection panel. Nov 09
 */

#include "perturbations.h"
#include "Atmosphere/AtmosphereModel.h"
#include "cartesianTOorbital.h"
#include "cartesianTOspherical.h"
#include "statevector.h"
#include "stabody.h"
#include "stamath.h"
#include "date.h"
#include "math.h"
#include "inertialTOfixed.h"
#include "getGreenwichHourAngle.h"
#include <QFile>
#include <Eigen/Core>
#include <Eigen/Geometry>
#include "Entry/capsule.h"
#include <QErrorMessage>
#include <QDebug>

const double PI = 3.141592;

Perturbations::Perturbations()
{
}

Perturbations::~Perturbations()
{
}

// TODO: This method should be abstract
Vector3d
Perturbations::calculateAcceleration(sta::StateVector /* state */, double /* time */, double /* dt */)
{
    return Vector3d::Zero();
}

/////////////////////////////// Gravity Field Perturbation ///////////////////////////////
GravityPerturbations::GravityPerturbations(const StaBody* centralBody,
                                           const ScenarioGravityModel* gravityModel)
{
    m_body = centralBody;
    m_modelName = gravityModel->modelName();
    m_zonalCount = gravityModel->numberOfZonals();
    m_tesseralCount = gravityModel->numberOfTesserals();

    //Assigning dimension to matrices of harmonical coefficients
    J.resize(m_zonalCount + 1);
    JJ.resize(m_zonalCount + 1 , m_tesseralCount + 1);
    gamma.resize(m_zonalCount + 1, m_tesseralCount + 1);

    loadGravityConstants();
}


GravityPerturbations::~GravityPerturbations()
{
}

Vector3d GravityPerturbations::calculateAcceleration(sta::StateVector state, double time, double dt)
{
    double R = body()->equatorialRadius();
    double mu = body()->mu();
    double greenwich = getGreenwichHourAngle (time);

    int n_zonals = zonalCount();
    int m_tesserals = tesseralCount();

    //Operating coordinate conversion from inertial cartesian to earth-fixed spherical
    double longitude, latitude, r, V, g, chi;
    VectorXd state_f(6);

    //TO DO : change the number 0 and put directly the stabody
    inertialTOfixed(0, greenwich,
                state.position.x(), state.position.y(), state.position.z(),
                state.position.x(), state.position.y(), state.position.z(),
                state_f(0), state_f(1), state_f(2), state_f(3), state_f(4), state_f(5));

    cartesianTOspherical(state_f(0), state_f(1), state_f(2), state_f(3), state_f(4), state_f(5),
                         longitude, latitude, r, V, g, chi);

    //Calculating the accelerations
    double derivative_r_zonals(0);
    double derivative_r_tesserals(0);
    double derivative_lat_zonals(0);
    double derivative_lat_tesserals(0);
    double derivative_long_tesserals(0);

    double x = sin(latitude);

    //P Legendre Polynomials, PP associated Legendre functions of the first kind
    VectorXd P(n_zonals + 1);
    MatrixXd PP(n_zonals + 1, m_tesserals + 1);

    //Assign values of P and PP with index below 2
    P(0) = 1;
    PP(0,0) = P(0);
    if (n_zonals != 0)
    {
        P(1) = x;
        PP(1,0) = P(1);
    }
    if (m_tesserals != 0)
        PP(1,1) = - pow((1 - pow(x,2.0)),0.5);

    for (int n = 2; n <= n_zonals ; n++)
    {
        //Legendre polynomials calculation:
        P(n) = ((2*n - 1) * P(n-1) * x - (n-1) * P(n-2)) / n;  //three term recurrence relation
        PP(n,0) = P(n);

        //Spherical coordinates derivative calculation (zonals terms):
        derivative_r_zonals += ((n+1) * J(n) * pow(R,n) * P(n) * pow(r,-(n+2)));
        derivative_lat_zonals += - cos(latitude) * J(n) * pow(r,-(n+2)) * pow(R,n) * (1.0  / (pow(x,2) - 1) * n * (x*P(n) - P(n-1)));

        for (int m = 1; (m <= m_tesserals && m <= n) ; m++)
        {
            //Associated Legendre functions calculation:
            if(n == m)
            {
                PP(n,m) = pow(-1.0,n) * doublefactorial(2*n - 1) * pow((1 - pow(x,2.0)),n/2);
            }
            else if(n == m+1)
            {
                PP(n,m) = x * (2*m + 1) * PP(n-1,m);
            }
            else
            {
                PP(n,m) = 1/(n-m) * ((2*n-1) * x * PP(n-1,m) - (n+m-1) * PP(n-2,m));
            }

            //Spherical coordinates derivative calculation (tesserals terms):
            derivative_r_tesserals += (n+1) * JJ(n,m) * pow(R,n) * PP(n,m) * pow(r,-(n+2)) * cos(m * (longitude - gamma(n,m)));
            derivative_lat_tesserals += - sin(latitude)/pow(r,n+2) * JJ(n,m) * pow(R,n) * (-(n+m) * (n-m+1) * sqrt(1 - (pow(x,2))) / ((pow(x,2)) -1) * PP(n,m-1) - m * x * PP(n,m) / (pow(x,2) - 1)) * cos(m * (longitude - gamma(n,m)));
            derivative_long_tesserals +=  ((1/pow(r,n+2) * JJ(n,m) * pow(R,n) * PP(n,m)) * m * sin(m*(longitude - gamma(n,m))))/cos(latitude) ;

        }
    }

    Vector3d acc_spherical;
    acc_spherical.x() = mu * (derivative_r_zonals + derivative_r_tesserals);
    acc_spherical.y() = -mu * (derivative_lat_zonals + derivative_lat_tesserals);
    acc_spherical.z() = mu * derivative_long_tesserals;

    //TO DO change the following line and use astro-core functions
    //Coordinates transformation from spherical/fixed to cartesian/inertial
    Vector3d acceleration;
    Matrix3d rotation1, rotation2;

    rotation1 << cos(latitude) * cos(longitude), -sin(longitude)*cos(latitude), -sin(latitude),
                sin(longitude), cos(longitude), 0,
                sin(latitude) * cos(longitude), -sin(latitude) * sin(longitude), cos(latitude);
    rotation2 << cos(greenwich), -sin(greenwich), 0,
                sin(greenwich), cos(greenwich), 0,
                0, 0, 1;

    acceleration = rotation1 * rotation2 * acc_spherical;

    return acceleration;
}

void GravityPerturbations::loadGravityConstants()
{
    //Assign a null value to constant harmonics below index 2
    J(0) = 0;
    J(1) = 0;
    JJ(0,0) = 0;    gamma(0,0) = 0;
    JJ(1,0) = 0;    gamma(1,0) = 0;
    if (m_tesseralCount != 0)
    {
        JJ(1,1) = 0;
        gamma(1,1) = 0;
    }

    //Open the file .stad containing the normalized gravity constants.
    //The number of loaded constants is consistent with the accuracy order the user selected.
    QString path = QString("data/bodies/");
    path.append(m_modelName);

    //QTextStream out (stdout); out << "===> bodies path: " << path << endl;

    QFile gravity(path);

    if (!gravity.open(QIODevice::ReadOnly))
    {
        // TODO: It's an error if the gravity model doesn't exist! Need a mechanism
        // for reporting this to the user.
        return;
    }

    QTextStream gravitystream(&gravity);

    MatrixXd C(m_zonalCount + 1, m_zonalCount + 1), S(m_zonalCount + 1, m_zonalCount + 1);
    int n = 0, m = 0;

    while (gravitystream.status() == QTextStream::Ok)
    {
        gravitystream >> n;
        if (n > m_zonalCount) break;
        gravitystream >> m;
        gravitystream >> C(n,m);
        gravitystream >> S(n,m);
    }
    gravity.close();

    //Converting the gravity constants.
    for (int i = 2 ; i <= m_zonalCount ; i++)
    {
        J(i) = - sqrt(2*i + 1.0) * C(i,0);
        if (m_tesseralCount != 0)
        {
            for (int j = 1 ; (j <= m_tesseralCount && j <= i) ; j++)
            {
                gamma(i,j) = atan(S(i,j)/C(i,j)) / j;
                JJ(i,j) = - fabs(sqrt(factorial(i-j) * (2*i + 1) * 2 / factorial(i+j)) * C(i,j) / cos(j * gamma(i,j)));
            }
        }
    }
}

double factorial(int num)
{
    if (num==0 || num==1)
    return 1.0;
    return factorial(num-1)*num;
}

double doublefactorial(int num)
{
    if (num==0 || num==1)
    return 1.0;
    return doublefactorial(num-2)*num;
}

/////////////////////////////// Atmospheric Drag Perturbation ///////////////////////////////
AtmosphericDragPerturbations::AtmosphericDragPerturbations(const QString& atmosphereModel)
{
#if OLDSCENARIO
    m_atmosphericModel = perturbation->atmosphericModel();
    m_body = perturbation->centralBody();
    m_surface = properties->physicalProperties()->physicalCharacteristics()->surfaceArea();
    m_cdCoefficients = properties->aerodynamicProperties()->CDCoefficients();
    m_mass = properties->physicalProperties()->physicalCharacteristics()->mass();
#endif
}

AtmosphericDragPerturbations::~AtmosphericDragPerturbations()
{
}

Vector3d
AtmosphericDragPerturbations::calculateAcceleration(sta::StateVector state, double time, double dt)
{
    //Conversion from kilometers to meters has been made.
    //Calculating the density from the altitude:
    AtmosphereModel atmosphere;
    atmosphere.selectModel(atmosphericModel());
    double h = altitude(body(), state, time);
    double rho = atmosphere.density(h * pow(10,3.0)) * pow(10,9.0);

    //Calculating the CD coefficient from the altitude (calling re-entry module functions)
    capsule_class vehicle;
    vehicle.selectCdCprofile(cdCoefficients());
    double cd = vehicle.cdc(h * pow(10,3.0));

    //Considering the atmosphere rotating with the Earth
    //TO DO put omega as a property of StaBody; only earth rotation has been considered
    double omega = 7.29211585494e-5;
    state.velocity.x() += omega * state.position.y();
    state.velocity.y() -= omega * state.position.x();

    //Calculating the accelerations vector
    Vector3d acceleration = - 0.5 * rho * state.velocity.norm() * state.velocity * (cd * surface()*pow(10,-6.0))/mass();

    return acceleration;
}

/////////////////////////////// Solar Pressure Perturbation ///////////////////////////////
SolarPressurePerturbations::SolarPressurePerturbations(StaBody* centralBody,
                                                       double reflectivity,
                                                       double albedo,
                                                       double ir,
                                                       double mass,
                                                       double surfaceArea) :
    m_body(centralBody),
    m_reflectivity(reflectivity),
    m_albedo(albedo),
    m_ir(ir),
    m_mass(mass),
    m_surface(surfaceArea)
{
}

SolarPressurePerturbations::~SolarPressurePerturbations()
{
}

Vector3d
SolarPressurePerturbations::calculateAcceleration(sta::StateVector state, double time, double dt)
{
    //calculation of the Sun position via the ephemeris
    sta::StateVector sunVector = STA_SOLAR_SYSTEM->sun()->stateVector(time, body(), sta::COORDSYS_EME_J2000);

    double sunAscension = atan(sunVector.position.y()/sunVector.position.x());
    double sunDeclination = atan(sunVector.position.z() / sqrt(pow(sunVector.position.y(),2.0) + pow(sunVector.position.x(),2.0)));

    double distancePlanetSun = sunVector.position.norm();
    double distancePlanetSat = state.position.norm();
    double distanceSunSat = (sunVector.position - state.position).norm();

    //Modeling the solar radiation (using Km)
    double sigma = 5.6704 * pow(10,-8.0); //Boltzmann constant
    double Tsun = 5778; //Sun temperature
    double Rsun = 6.955 * pow(10,5.0); // Sun radius

    double W = sigma * pow(Tsun, 4.0) * pow((Rsun / distanceSunSat), 2.0);

    //Calculation of the acceleration module
    double c = 300000;
    double fp = (1 + reflectivity()) * W * (surface() * pow(10,-6.0)) / c / mass();

    Vector3d accelerationAlbedo(0,0,0);
    Vector3d accelerationIR(0,0,0);
    //Calculation of the albedo and infra-red radiations
    if (albedo())
    {
        double Walbedo = albedoReflectivityCoefficient(body()) * W * pow((body()->meanRadius() / distancePlanetSat), 2.0);
        double fpalbedo = (1 + reflectivity()) * Walbedo * (surface() * pow(10,-6.0)) / c / mass();
        accelerationAlbedo = fpalbedo * state.position/distancePlanetSat;
    }

    if (ir())
    {
        double Wir = irRadiationFlux(body()) * pow((body()->meanRadius() / distancePlanetSat), 2.0);
        double fpir = (1 + reflectivity()) * Wir * (surface() * pow(10,-6.0)) / c / mass();
        accelerationIR = fpir * state.position/distancePlanetSat;
    }

    Vector3d acceleration(-fp * cos(sunAscension) * cos(sunDeclination),
                          -fp * sin(sunAscension) * cos(sunDeclination),
                          -fp * sin(sunDeclination));

    //calculation of the Earth shadow; if the spacecraft is overshadowed by the Earth the solar pressure will be ignored
    double gamma = acos((pow(distanceSunSat,2.0) + pow(distancePlanetSun,2.0) - pow(distancePlanetSat,2.0)) / (2 * distancePlanetSun * distanceSunSat));
    double gamma_c = atan(body()->meanRadius() / distancePlanetSun);
//
//    if (gamma < gamma_c)
//    {
//        if (distanceSunSat > distancePlanetSun)
//            acceleration << 0, 0, 0;
//    }

    acceleration += accelerationAlbedo + accelerationIR;

    return acceleration;
}

double albedoReflectivityCoefficient(const StaBody* body)
{
    switch(body->id())
    {
    case STA_MERCURY:
        return 0.53;
    case STA_VENUS:
        return 0.76;
    case STA_EARTH:
        return 0.35;
    case STA_MARS:
        return 0.16;
    case STA_JUPITER:
        return 0.73;
    case STA_SATURN:
        return 0.76;
    case STA_URANUS:
        return 0.93;
    case STA_NEPTUNE:
        return 0.84;
    case STA_PLUTO:
        return 0.14;
    case STA_SUN:
        return 0;
    case STA_MOON:
        return 0.067;
    default:
        return 0;
    }
}

double irRadiationFlux(const StaBody* body)
{
    switch(body->id())
    {
    case STA_MERCURY:
        return 2139;
    case STA_VENUS:
        return 155;
    case STA_EARTH:
        return 240;
    case STA_MARS:
        return 123;
    case STA_JUPITER:
        return 3.4;
    case STA_SATURN:
        return 0.9;
    case STA_URANUS:
        return 0.063;
    case STA_NEPTUNE:
        return 0.06;
    case STA_PLUTO:
        return 0.191;
    case STA_SUN:
        return 0;
    case STA_MOON:
        return 316;
    default:
        return 0;
    }
}


/////////////////////////////// Third Body Perturbation ///////////////////////////////

ExternalBodyPerturbations::ExternalBodyPerturbations(const StaBody* centralBody,
                                                     const QList<const StaBody*>& bodies) :
    m_body(centralBody),
    m_perturbingBodyList(bodies)
{
}


ExternalBodyPerturbations::~ExternalBodyPerturbations()
{
}


Vector3d
ExternalBodyPerturbations::calculateAcceleration(sta::StateVector state, double time, double dt)
{
    Vector3d acceleration(0.0, 0.0, 0.0);

    foreach(const StaBody* thirdbody,  perturbingBodyList())
    {
        double mu = thirdbody->mu();
        sta::StateVector thirdbodystate = thirdbody->stateVector(time, body(), sta::COORDSYS_EME_J2000);

        acceleration += mu * ((thirdbodystate.position - state.position) / pow((thirdbodystate.position - state.position).norm(),3.0) - thirdbodystate.position / pow(thirdbodystate.position.norm(), 3.0));
    }

    return acceleration;
}

/////////////////////////////// Space Debris Perturbation ///////////////////////////////

DebrisPerturbations::DebrisPerturbations(const StaBody* centralBody,
                                         double mass,
                                         double surfaceArea) :
    m_body(centralBody),
    m_mass(mass),
    m_surface(surfaceArea),
    m_time(0)
{
    m_counterDebris.resize(25);
    m_counterDebris.setZero();
    m_counterMeteoroids.resize(25);
    m_counterMeteoroids.setZero();
}


DebrisPerturbations::~DebrisPerturbations()
{
}


Vector3d
DebrisPerturbations::calculateAcceleration(sta::StateVector state, double time, double dt)
{
    double mu = body()->mu();
    m_time += dt;
    sta::KeplerianElements keplerian = cartesianTOorbital(mu, state);

    //Trajectory analysis:
    double h = keplerian.SemimajorAxis * (1 - pow(keplerian.Eccentricity, 2)) / (1 + keplerian.Eccentricity * cos(keplerian.TrueAnomaly)) - body()->meanRadius();

    int Ndatapoints = 0;
    VectorXd diameter, Ndebris, Nmet, Ntot;

    // TODO: Shouldn't we use the absolute value of inclination here???
    double inclinationDeg = sta::radToDeg(keplerian.Inclination);

    if ( 300 <= h && h <= 500 && 46.6 <= inclinationDeg && inclinationDeg <= 57.6 )
        loadStatistics("debris_impact_low_altitude.stad", diameter, Ndebris, Nmet, Ntot, Ndatapoints);

    else if ( 700 <= h && h <= 900 && 70 <= inclinationDeg && inclinationDeg <= 90 )
        loadStatistics("debris_impact_medium_altitude.stad", diameter, Ndebris, Nmet, Ntot, Ndatapoints);

    else if ( 35600 <= h && h <= 35900 && 0 <= inclinationDeg && inclinationDeg <= 5.0 )
        loadStatistics("debris_impact_geo_altitude.stad", diameter, Ndebris, Nmet, Ntot, Ndatapoints);

    else return Vector3d(0.0, 0.0, 0.0);

    double density_deb = 0.0028; //kg/cm^3
    double density_met = 0.0025; //kg/cm^3
    double deltaT = 0.0001; //10^-4 s
    double accelerationDebris = 0;
    /* double accelerationMeteoroids = 0; */
    VectorXd nDebris(Ndatapoints), diffDebris(Ndatapoints);
    VectorXd nMeteoroids(Ndatapoints), diffMeteoroids(Ndatapoints);
    Vector3d acceleration(0.0, 0.0, 0.0);

    nDebris = Ndebris/365/24/60/60 * m_time * surface();
    nMeteoroids = Nmet/365/24/60/60 * m_time * surface();

    for (int i = 0; i < Ndatapoints; i++)
    {
        if (i != Ndatapoints-1)
        {
            diffDebris(i) = nDebris(i) - nDebris(i + 1);
            diffMeteoroids(i) = nMeteoroids(i) - nMeteoroids(i + 1);
        }
        else
        {
            diffDebris(i) = nDebris(i);
            diffMeteoroids(i) = nMeteoroids(i);
        }

        // Debris collisions calculation
        if (floor(diffDebris(i)) > counterDebris()(i))
        {
            setCounterDebris(i, nDebris(i));

            double accDebrisModule;
            if (i == 0)
                accDebrisModule = 3 * (density_deb * 4/3 * PI * pow(0.5 * randomNumber(0, diameter(i)),3)) / mass() * randomNumber(4.5,5.5) / deltaT;
            else
                accDebrisModule = 3 * (density_deb * 4/3 * PI * pow(0.5 * randomNumber(diameter(i-1),diameter(i)),3)) / mass() * randomNumber(4.5,5.5) / deltaT;

            //generate a random direction
            double alpha = randomNumber(0, 2*PI);
            double beta = randomNumber(-PI, PI);
            acceleration.x() += accDebrisModule * cos(alpha) * cos(beta);
            acceleration.y() += accDebrisModule * sin(alpha) * cos(beta);
            acceleration.z() += accDebrisModule * sin(beta);
        }

        // Meteoroids collisions calculation
        if (floor(diffMeteoroids(i)) > counterMeteoroids()(i))
        {
            setCounterMeteoroids(i, nMeteoroids(i));

            double accMeteoroidsModule;
            if (i == 0)
                accMeteoroidsModule = 3 * (density_met * 4/3 * PI * pow(0.5 * randomNumber(0, diameter(i)),3)) / mass() * randomNumber(4.5,5.5) / deltaT;
            else
                accMeteoroidsModule = 3 * (density_met * 4/3 * PI * pow(0.5 * randomNumber(diameter(i-1),diameter(i)),3)) / mass() * randomNumber(4.5,5.5) / deltaT;

            //generate a random direction
            double alpha = randomNumber(0, 2*PI);
            double beta = randomNumber(-PI, PI);
            acceleration.x() += accMeteoroidsModule * cos(alpha) * cos(beta);
            acceleration.y() += accMeteoroidsModule * sin(alpha) * cos(beta);
            acceleration.z() += accMeteoroidsModule * sin(beta);
        }
    }
    return acceleration;
}

void
DebrisPerturbations::loadStatistics(QString filename, VectorXd& diameter, VectorXd& Ndebris, VectorXd& Nmet, VectorXd& Ntot, int& Ndatapoints)
{
    QString path("data/atmospheres/");
    path.append(filename);
    QFile model("data/atmospheres/debris_impact_low_altitude.stad");

    model.open(QIODevice::ReadOnly);
    QTextStream modelstream(&model);

    while (!modelstream.atEnd())
    {
        modelstream.readLine();
        Ndatapoints ++;
    }
    model.close();
    diameter.resize(Ndatapoints); Ndebris.resize(Ndatapoints); Nmet.resize(Ndatapoints); Ntot.resize(Ndatapoints);

    model.open(QIODevice::ReadOnly);
    for (int i = 0; i < Ndatapoints; i++)
    {
        modelstream >> diameter(i);
        modelstream >> Ndebris(i);
        modelstream >> Nmet(i);
        modelstream >> Ntot(i);
    }
    model.close();
}

double randomNumber(double inf, double sup)
{
    const float scale = rand()/float(RAND_MAX);
    return inf + scale * (sup - inf);
}
