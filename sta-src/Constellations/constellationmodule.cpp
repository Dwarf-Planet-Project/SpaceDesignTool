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

 ------------------ Author: Steffen Peter, Claas Grohnfeldt --------------------------------------------------
 ------------------ E-mail: (claasgr@math.uni-bremen.de, spezisteffen@gmx.de) --------------------------------
 Modified by ...
 */

#include "Constellations/constellationmodule.h"
#include <QtDebug>
#include <QMessageBox>
#include <math.h>
#include "Astro-Core/stamath.h"
#include "Astro-Core/nedTOfixed.h"
#include "Astro-Core/rectangularTOpolar.h"
#include <vector>

using namespace sta;
using namespace Eigen;

/** Conversion of the Self-defined Constellation specific parameters to the
  * parameters of the generated satellites
  * @param n number of satellites
  * @param kepler array to store keplerian coordinates
  */
bool selfDefinedConstellation(int n, SatelliteKeplerian* kepler)
{
    for (int i = 0; i < n; i++)
    {
        kepler[i].name = "Satellite " + QString("%1").arg(i+1);
        kepler[i].param[0] = 10000;
        kepler[i].param[1] = 0.0;
        kepler[i].param[2] = 0.0;
        kepler[i].param[3] = 0.0;
        kepler[i].param[4] = 0.0;
        kepler[i].param[5] = 0.0;
    }
    return true;
}

/** Conversion of the Walker Constellation specific parameters to the
  * parameters of the generated satellites
  * @param npl number of equally spaced planes
  * @param nspl number of satellites per orbit plane
  * @param a semimajor axis that is the same for every satellite [km]
  * @param inc inclination that is the same for every satellite [deg]
  * @param f relative spacing between satellites in adjecent planes
  * @param kepler array to store keplerian coordinates
  */
bool walkerConstellation(int npl, int nspl, double a, double inc, int f, SatelliteKeplerian* kepler)
{
    int k = 0;
    for(int i = 1; i <= npl; i++)
    {
        for(int j = 1; j <= nspl; j++)
        {
            // compute the Kelperian values of the j'th satellite on plane number i
            // eccentricity, inclination, RAAN, argument of periapsis, true anomaly
            kepler[k].param[0] = a;
            kepler[k].param[1] = 0.0;
            kepler[k].param[2] = inc;
            kepler[k].param[3] = i*360.0/npl;
            kepler[k].param[4] = 0.0;
            kepler[k].param[5] = int(i*f*360.0/(nspl*npl) + j*360.0/nspl) % 360;

            // set name
            kepler[k].name = "Satellite " + QString("%1-%2").arg(i).arg(j);

            k++;
        }
    }
    return true;
}
/** Conversion of the Flower Constellation specific parameters to the
  * parameters of the generated satellites
  * @param np number of petals [rad]
  * @param nd number of days to repeat ground track [rad]
  * @param ns number of satellites
  * @param om argument of periapsis of each satellite [rad]
  * @param inc inclination of each satellite [rad]
  * @param hp heigt of perigee of each satellite [km]
  * @param schemde true=symmetric, false=asymmetric
  * @param omega1 raan of first satellite [rad]
  * @param psi1 initial true anomaly of first satellite [rad]
  * @param domega limited orbit node range for asymmetric schemes [rad]
  * @param radiusCB central body mean radius [km]
  * @param omegaCB central body rotation velocity [rad/s]
  * @param muCB central body mu
  * @param j2CB central body J2 value
  * @param kepler array to store keplerian coordinates
  * @return status if calculation went right
  */
bool flowerConstellation(int np, int nd, int ns, double om, double inc, double hp, bool scheme, double omega1, double psi1, double domega,
                         double radiusCB, double omegaCB, double muCB, double j2CB, SatelliteKeplerian* kepler)
{
    // get number of orbit planes
    int nop;
    if (ns%nd == 0) nop = ns/nd; else nop = ns;

    // get semimajor axis
    double a;
    // experimental: find solution for a>0 via bisection (replace it later by a good solver)
    //      initialize
    double l = radiusCB + hp;
    double fl = getfa(l, np, nd, inc, hp, radiusCB, omegaCB, muCB, j2CB);
    double r = 2*radiusCB + hp;
    double fr = getfa(r, np, nd, inc, hp, radiusCB, omegaCB, muCB, j2CB);
    //      find negative value for fr concerning fl
    while (fr * fl > 0 && r < hp + 50 * radiusCB)
    {
        r += radiusCB;
        fr = getfa(r, np, nd, inc, hp, radiusCB, omegaCB, muCB, j2CB);
    }
    if (r > hp + 50*radiusCB){
        return false;
    }
    //      bisection method
    while (r-l > 0.001)
    {
        a = (r+l)/2;
        double fa = getfa(a, np, nd, inc, hp, radiusCB, omegaCB, muCB, j2CB);
        if (fabs(fa) < 0.00001)
        {
            r = a;
            l = a;
        }
        else if (fa*fl < 0.0)
        {
            r = a;
            fr = fa;
        }
        else
        {
            l = a;
            fl = fa;
        }
    }
    // calculate eccentricity
    double e = 1 - (radiusCB + hp)/a;
    // write values and names (not RAAN and true anomaly)
    for (int k = 1; k <= ns; k++)
    {
        kepler[k-1].param[0] = a; //semimajoraxis
        kepler[k-1].param[1] = e; // eccentricity
        kepler[k-1].param[2] = inc/sta::Pi()*180; // inclination
        kepler[k-1].param[4] = om/sta::Pi()*180; // arg. of perigee
        kepler[k-1].name = "Satellite " + QString("%1-%2").arg((k-1)/nd+1).arg((k-1)%nd+1);
    }
    // calculate RAAN
    double* omega = new double[ns];
    omega[0] = omega1;
    kepler[0].param[3] = omega1/sta::Pi()*180;
    if (scheme)
    {
        for (int k = 2; k <= ns; k++)
        {
            omega[k-1] = omega[k-2]-(2*sta::Pi()*nd)/ns;
            kepler[k-1].param[3] = kepler[k-2].param[3]-(360.0*nd)/ns;
        }
    }
    else
    {
        for (int k = 2; k <= ns; k++)
        {
            omega[k-1] = omega[k-2]-domega/(ns-1);
            kepler[k-1].param[3] = kepler[k-2].param[3]-domega*180./(sta::Pi()*(ns-1));
        }
    }
    // calculate true anomaly
    double* ecce = new double[ns];
    double* mean = new double[ns];
    ecce[0] = 2*atan(sqrt((1-e)/(1+e))*tan(psi1/2)); // eccentric anomalie of first satellite
    ecce[0] = fmod(ecce[0],2*sta::Pi());
    mean[0] = ecce[0] - e*sin(ecce[0]); // mean anomalie of first satellite
    mean[0] = fmod(mean[0],2*sta::Pi());
    kepler[0].param[5] = fmod(psi1*180.0/sta::Pi(),360.0);
    double p = 2*(radiusCB + hp) - (radiusCB + hp)*(radiusCB + hp)/a;
    double xi = 0.75 * radiusCB * radiusCB * j2CB / (p*p);
    double dotm = -xi*sqrt(muCB/pow(a,3))*sqrt(1-e*e)*(3*pow(sin(inc),2)-2); // rate of change in mean anomaly
    double dotomega = -2*xi*sqrt(muCB/pow(a,3))*cos(inc); // rate of change in the RAAN
    for (int k = 2; k <= ns; k++)
    {
        mean[k-1] = mean[k-2] + (sqrt(muCB/pow(a,3))+dotm)*(omega[k-2]-omega[k-1])/(omegaCB+dotomega);
        mean[k-1] = fmod(mean[k-1],2*sta::Pi());
        //QMessageBox::warning(NULL,QObject::tr("Error:"),QObject::tr("%1  %2").arg(mean[k-1]).arg(mean[k-2]));
        // bisection to get eccentricity
        double l = 0.0;
        double fl = getfmean(l,e,mean[k-1]); //l-e*sin(l)-mean[k-1];
        double r = 2*sta::Pi();
        double fr = getfmean(r,e,mean[k-1]); //r-e*sin(r)-mean[k-1];
        while (fabs(r-l) > 0.001)
        {
            ecce[k-1] = (r+l)/2;
            double fa = getfmean(ecce[k-1],e,mean[k-1]); //ecce[k-1]-e*sin(ecce[k-1])-mean[k-1];
            if (fabs(fa) < 0.00001)
            {
                r = ecce[k-1];
                l = ecce[k-1];
            }
            else if (fa*fl < 0.0)
            {
                r = ecce[k-1];
                fr = fa;
            }
            else
            {
                l = ecce[k-1];
                fl = fa;
            }
        }
        //QMessageBox::warning(NULL,QObject::tr("Error:"),QObject::tr("%1  %2").arg(ecce[k-1]).arg(ecce[k-2]));
        kepler[k-1].param[5] = 2*atan(sqrt((1+e)/(1-e))*tan(ecce[k-1]/2))*180.0/sta::Pi();
    }
    return true;
}
/** Function used in Flower Constellation to get semimajor axis.
  * @param a semimajor-axis
  * @param np number of petals
  * @param nd number of days to repeat ground track
  * @param inc inclination
  * @param hp height of periapsis
  * @param radiusCB mean radius of central body
  * @param omegaCB rotation velocity of central body
  * @param muCB mu of central body
  * @param j2CB J2 value of central body
  * @return function value
  */
double getfa(double a, int np, int nd, double inc, double hp, double radiusCB, double omegaCB, double muCB, double j2CB)
{
    double p = 2*(radiusCB + hp) - (radiusCB + hp)*(radiusCB + hp)/a;
    double e = 1 - (radiusCB + hp)/a;
    double xi = 0.75 * radiusCB * radiusCB * j2CB / (p * p);
    double n = sqrt(muCB/pow(a,3));
    return omegaCB/n - nd*(1 + xi*((2 - 3*pow(sin(inc),2))*sqrt(1 - e*e)))
            /(np*(1 + 2*n*cos(inc)*xi/omegaCB));
}
/** Function used in Flower Constellation to get eccentric anomaly.
  * @param arg argument (eccentric anomaly)
  * @param e parameter: eccentricity
  * @param mean parameter: mean anomaly
  * @return difference to @param mean
  */
double getfmean(double arg, double e, double mean){
    return arg - e*sin(arg) - mean;
}

bool visibility(SpaceObject* transmitter, SpaceObject* receiver, const StaBody* body, double currentTime, QList<Antenna*> antennas)
{
    sta::StateVector vTransmitter;
    sta::StateVector vReceiver;
    transmitter->getStateVector(currentTime, *body, sta::CoordinateSystem(sta::COORDSYS_EME_J2000), &vTransmitter);
    receiver->getStateVector(currentTime, *body, sta::CoordinateSystem(sta::COORDSYS_EME_J2000), &vReceiver);
    // check, if line of sight is not interrupted by central body
    Vector3d distRT = vReceiver.position-vTransmitter.position;
    double k = - vTransmitter.position.dot(distRT)/pow(distRT.norm(),2);
    double dist = (vTransmitter.position + k*(distRT)).norm();
    if (!(k > 0 & k < 1 & dist < body->meanRadius()))
    {
        // first step: get theta (angle between NED and VDG)
        double azimuth, elevation;
        double r; // radius, we don't need this
        // get elevation and azimuth
        rectangularTOpolar(vTransmitter.position,r,azimuth,elevation);
        // calculate NED e_x and e_z and e_x' = e_x1
        Eigen::Vector3d e_x;
        nedTOfixed(elevation, azimuth, Vector3d::UnitX(), e_x);
        Eigen::Vector3d e_z;
        nedTOfixed(elevation, azimuth, Vector3d::UnitZ(), e_z);
        Eigen::Vector3d e_x1 = (vTransmitter.velocity - vTransmitter.velocity.dot(e_z)*e_z).normalized();
        // calculate theta'
        double theta1 = acos(e_x.dot(e_x1));
        // signum of determinant
        double sigdet = e_x.dot(e_x1.cross(e_z));
        if (sigdet != 0)
        {
            sigdet = sigdet/fabs(sigdet);
        }
        double theta;
        if (theta1 >= 0)
        {
            theta = sigdet*theta1;
        }
        else
        {
            theta = sigdet*(sta::Pi() - theta1);
        }
        // second step: get CBCF coordinates for d (per Antenna)
        Matrix3d rotation;
        double sinaz = sin(azimuth);
        double cosaz = cos(azimuth);
        double sinth = sin(theta);
        double costh = cos(theta);
        double sinel = sin(elevation);
        double cosel = cos(elevation);
        rotation << Vector3d(-costh*cosaz*sinel-sinth*sinaz,-costh*sinaz*sinel+sinth*cosaz,costh*cosel),
                    Vector3d(sinth*cosaz*sinel-costh*sinaz,sinth*sinaz*sinel+costh*cosaz,-sinth*cosel),
                    Vector3d(-cosaz*cosel,-cosel*sinaz,-sinel);
        // check for each antenna
        foreach(Antenna* antenna, antennas)
        {
            if (antenna->hasInCone(vReceiver.position, vTransmitter.position, rotation))
            {
                return true;
            }
        }
        return false;
    } else {
        return false;
    }
}
bool visibility(SpaceObject* transmitter, GroundObject* receiver, const StaBody* body, double currentTime, QList<Antenna*> antennas)
{
    sta::StateVector vTransmitter;
    Eigen::Vector3d vReceiverPos;
    transmitter->getStateVector(currentTime, *body, CoordinateSystem(COORDSYS_EME_J2000), &vTransmitter);
    // get the 3D coordinates for discretization point (altitude on surface is set to 0.0) in J2000 (CBCI) (longitude + greenwich hour angle)
    vReceiverPos = body->planetographicToCartesian(receiver->latitude, receiver->longitude + radToDeg(getGreenwichHourAngle(MjdToJd(currentTime))), receiver->altitude);
    // check, if line of sight is not interrupted by central body
    Vector3d distRT = vReceiverPos - vTransmitter.position;
    double k = - vTransmitter.position.dot(distRT)/pow(distRT.norm(),2);
    double dist = (vTransmitter.position + k*(distRT)).norm();
    if (!(k > 0 & k < 1 & dist < body->meanRadius()))
    {
        // first step: get theta (angle between NED and VDG)
        double azimuth, elevation;
        double r; // radius, we don't need this
        // get elevation and azimuth
        rectangularTOpolar(vTransmitter.position,r,azimuth, elevation);
        // calculate NED e_x and e_z and e_x' = e_x1
        Eigen::Vector3d e_x;
        nedTOfixed(elevation, azimuth, Vector3d::UnitX(), e_x);
        Eigen::Vector3d e_z;
        nedTOfixed(elevation, azimuth, Vector3d::UnitZ(), e_z);
        Eigen::Vector3d e_x1 = (vTransmitter.velocity - vTransmitter.velocity.dot(e_z)*e_z).normalized();
        // calculate theta'
        double theta1 = acos(e_x.dot(e_x1));
        // signum of determinant
        double sigdet = e_x.dot(e_x1.cross(e_z));
        if (sigdet != 0)
        {
            sigdet = sigdet/fabs(sigdet);
        }
        double theta;
        if (theta1 >= 0)
        {
            theta = sigdet*theta1;
        }
        else
        {
            theta = sigdet*(sta::Pi() - theta1);
        }
        // second step: get CBCI coordinates for d (per Antenna)
        Matrix3d rotation;
        double sinaz = sin(azimuth);
        double cosaz = cos(azimuth);
        double sinth = sin(theta);
        double costh = cos(theta);
        double sinel = sin(elevation);
        double cosel = cos(elevation);
        rotation << Vector3d(-costh*cosaz*sinel-sinth*sinaz,-costh*sinaz*sinel+sinth*cosaz,costh*cosel),
                    Vector3d(sinth*cosaz*sinel-costh*sinaz,sinth*sinaz*sinel+costh*cosaz,-sinth*cosel),
                    Vector3d(-cosaz*cosel,-cosel*sinaz,-sinel);
        // check for each antenna
        foreach(Antenna* antenna, antennas)
        {
            if (antenna->hasInCone(vReceiverPos, vTransmitter.position, rotation))
            {
                return true;
            }
        }
        return false;
    } else {
        return false;
    }
}
bool visibility(GroundObject* transmitter, SpaceObject* receiver, const StaBody* body, double currentTime, Antenna* gAntenna)
{
    sta::StateVector vReceiver;
    receiver->getStateVector(currentTime, *body, sta::CoordinateSystem(sta::COORDSYS_EME_J2000), &vReceiver);
    Eigen::Vector3d vTransmitterPos;
    double greenwAng = radToDeg(getGreenwichHourAngle(MjdToJd(currentTime)));
    vTransmitterPos = body->planetographicToCartesian(transmitter->latitude, transmitter->longitude + greenwAng, transmitter->altitude);
    // check, if line of sight is not interrupted by central body
    Vector3d distRT = vReceiver.position - vTransmitterPos;
    double k = - vTransmitterPos.dot(distRT)/pow(distRT.norm(),2);
    double dist = (vTransmitterPos + k*(distRT)).norm();
    if (!(k > 0 & k < 1 & dist < body->meanRadius()))
    {
        // second step: get CBCI coordinates for d (per Antenna)
        Matrix3d rotation;
        double sinel = sin(degToRad(transmitter->latitude));
        double cosel = cos(degToRad(transmitter->latitude));
        double sinaz = sin(degToRad(transmitter->longitude + greenwAng));
        double cosaz = cos(degToRad(transmitter->longitude + greenwAng));
        rotation << Vector3d(-cosaz*sinel,-sinaz*sinel,cosel),
                    Vector3d(-sinaz,cosaz,0.0),
                    Vector3d(-cosaz*cosel,-cosel*sinaz,-sinel);
        // check for Groundstation antenna
        return (gAntenna->hasInCone(vReceiver.position, vTransmitterPos, rotation));
    } else {
        return false;
    }
}

/** function calculates if a space object can see this point on the surface
  *
  **/
bool visibility(SpaceObject* transmitter, DiscretizationPoint receiver, const StaBody* body, double currentTime, Antenna* obsAntenna)
{
    sta::StateVector vTransmitter;
    Eigen::Vector3d vReceiverPos;
    transmitter->getStateVector(currentTime, *body, CoordinateSystem(COORDSYS_EME_J2000), &vTransmitter);
    // get the 3D coordinates for discretization point (altitude on surface is set to 0.0) in J2000 (CBCI) (longitude + greenwich hour angle)
    vReceiverPos = body->planetographicToCartesian(receiver.latitude, receiver.longitude + radToDeg(getGreenwichHourAngle(MjdToJd(currentTime))), 0.0);
    // check, if line of sight is not interrupted by central body
    Vector3d distRT = vReceiverPos - vTransmitter.position;
    double k = - vTransmitter.position.dot(distRT)/pow(distRT.norm(),2);
    double dist = (vTransmitter.position + k*(distRT)).norm();
    if (!(k > 0 & k < 1 & dist < body->meanRadius()))
    {
        // first step: get theta (angle between NED and VDG)
        double azimuth, elevation;
        double r; // radius, we don't need this
        // get elevation and azimuth
        rectangularTOpolar(vTransmitter.position,r,azimuth,elevation);
        // calculate NED e_x and e_z and e_x' = e_x1
        Eigen::Vector3d e_x;
        nedTOfixed(elevation, azimuth, Vector3d::UnitX(), e_x);
        Eigen::Vector3d e_z;
        nedTOfixed(elevation, azimuth, Vector3d::UnitZ(), e_z);
        Eigen::Vector3d e_x1 = (vTransmitter.velocity - vTransmitter.velocity.dot(e_z)*e_z).normalized();
        // calculate theta'
        double theta1 = acos(e_x.dot(e_x1));
        // signum of determinant
        double sigdet = e_x.dot(e_x1.cross(e_z));
        if (sigdet != 0)
        {
            sigdet = sigdet/fabs(sigdet);
        }
        double theta;
        if (theta1 >= 0)
        {
            theta = sigdet*theta1;
        }
        else
        {
            theta = sigdet*(sta::Pi() - theta1);
        }
        // second step: get CBCF coordinates for d (per Antenna)
        Matrix3d rotation;
        double sinaz = sin(azimuth);
        double cosaz = cos(azimuth);
        double sinth = sin(theta);
        double costh = cos(theta);
        double sinel = sin(elevation);
        double cosel = cos(elevation);
        rotation << Vector3d(-costh*cosaz*sinel-sinth*sinaz,-costh*sinaz*sinel+sinth*cosaz,costh*cosel),
                    Vector3d(sinth*cosaz*sinel-costh*sinaz,sinth*sinaz*sinel+costh*cosaz,-sinth*cosel),
                    Vector3d(-cosaz*cosel,-cosel*sinaz,-sinel);
        // check for observation antenna
        return obsAntenna->hasInCone(vReceiverPos, vTransmitter.position, rotation);
    } else {
        return false;
    }
}


