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
 ------------------ Author: Guido Ridolfi  -------------------------------------------------
 ------------------ E-mail: (guido.ridolfi@gmail.com) --------------------------------------
 ------------------ Author: Tiziana Sabatini  ----------------------------------------------
 ------------------ E-mail: (tiziana.sabatini@yahoo.it) ------------------------------------
 */

// Modified by Tiziana Sabatini, May 2009


#include <QtGui>

#include "sem.h"
#include "preliminaryTables.h"
#include "Astro-Core/Atmosphere/AtmosphereModel.h"
#include "Astro-Core/stabody.h"

#include "math.h"
#include "Scenario/scenario.h"
#include "Astro-Core/cartesianTOorbital.h"
#include <QDebug>
#include <QComboBox>

#define c 299792458 // speed of light m/s
#define Pi 3.1415926535
#define Sb 5.67051E-8 //stefan-boltzmann constant w/m^2k^4

using namespace std;


// The tab subsystems has not been implemented : the commented code concern
// this part
SEM::SEM(ScenarioSpaceVehicle* vehicle)
    : QDialog(), m_batterydialog(NULL)
{
        setupUi(this);
        m_vehicle = vehicle;

        connect(MissionComboBox, SIGNAL(activated(int)), this, SLOT(select_table(int)));
        connect(CalculateButton, SIGNAL(clicked()), this, SLOT(sum()));

        ISP->setValue(this->m_vehicle->properties()->propulsionProperties()->specificImpulse());
        DV->setValue(this->m_vehicle->properties()->propulsionProperties()->dV());
        PLMass->setValue(this->m_vehicle->properties()->payloadProperties()->massPayload());
        double masspercentage = (this->m_vehicle->properties()->payloadProperties()->massPayload())/(this->m_vehicle->properties()->physicalProperties()->physicalCharacteristics()->mass())*100;
        PLPerc->setValue(masspercentage);
        PLPower->setValue(this->m_vehicle->properties()->payloadProperties()->powerPayload());

        connect(CalculateMission, SIGNAL(clicked()), this, SLOT(missionCalculations()));
        connect(SemimajorAxis, SIGNAL(valueChanged(double )), this, SLOT(missionCalculations()));
        connect(Inclination, SIGNAL(valueChanged(int )), this, SLOT(missionCalculations()));
        connect(Eccentricity, SIGNAL(valueChanged(double )), this, SLOT(missionCalculations()));
        connect(OrbitElevationAngle, SIGNAL(valueChanged(int )), this, SLOT(missionCalculations()));
        connect(TransferComboBox, SIGNAL(activated(int )), this, SLOT(ManeuverPlan( )));
        connect(calculateManeuvres, SIGNAL(clicked()), this, SLOT(CalculateManeuvres()));
        connect(calculateManeuvres, SIGNAL(clicked()), this, SLOT(sum()));

        //Only the loitering trajectory is considered so far;
        //In further versions of the SEM module it could be maybe possible to analyse a reentry, ascent or
        //interplanetary vehicle; in this case different kinds of trajectory will be considered
        for (int i = 0; i<this->m_vehicle->trajectoryPlan()->trajectories().count(); i++)
        {
            if (dynamic_cast<ScenarioLoiteringTrajectory*>(this->m_vehicle->trajectoryPlan()->trajectories().at(i)))
            {
                ScenarioLoiteringTrajectory* trajectory = dynamic_cast<ScenarioLoiteringTrajectory*>(this->m_vehicle->trajectoryPlan()->trajectories().at(i));
                ScenarioAbstractInitialState* initialState = trajectory->simulationParameters()->initialStatePosition()->initialState();
                ScenarioKeplerianElements* keplerian = dynamic_cast<ScenarioKeplerianElements*>(initialState);
                ScenarioStateVector* stateVector = dynamic_cast<ScenarioStateVector*>(initialState);
                ScenarioSphericalCoordinates* spherical = dynamic_cast<ScenarioSphericalCoordinates*>(initialState);

                double a, e, i, raan, omega, nu;
                Q_ASSERT(keplerian || stateVector || spherical);
                if (keplerian != NULL)
                {
                    a = keplerian->semimajorAxis(); e = keplerian->eccentricity();
                    i = keplerian->inclination(); raan = keplerian->raan();
                    omega = keplerian->argumentOfPeriapsis(); nu = keplerian->trueAnomaly();
                }
                else if (stateVector != NULL)
                {
                    /*
                    cartesianTOorbital(trajectory->centralBody()->mu(), stateVector->position().x(), stateVector->position().y(), stateVector->position().z(),
                                       stateVector->velocity().x(), stateVector->velocity().y(), stateVector->velocity().z(),
                                       a, e, i, raan, omega, nu);
                    */

                    sta::StateVector MyStateVector;
                    MyStateVector.position(0) = stateVector->position().x();
                    MyStateVector.position(1) = stateVector->position().y();
                    MyStateVector.position(2) = stateVector->position().z();
                    MyStateVector.velocity(0) = stateVector->velocity().x();
                    MyStateVector.velocity(1) = stateVector->velocity().y();
                    MyStateVector.velocity(2) = stateVector->velocity().z();

                    sta::KeplerianElements foundKeplerianElements = cartesianTOorbital(trajectory->centralBody()->mu(),  MyStateVector);
                    a     = foundKeplerianElements.SemimajorAxis;
                    e     = foundKeplerianElements.Eccentricity;
                    i     = foundKeplerianElements.Inclination;
                    raan  = foundKeplerianElements.AscendingNode;
                    omega = foundKeplerianElements.ArgumentOfPeriapsis;
                    nu    = foundKeplerianElements.MeanAnomaly;
                }
                //TO DO:
                else if (spherical != NULL){break;}


                SemimajorAxis->setValue(a);
                Eccentricity->setValue(e);
                Inclination->setValue(i);

                break;

            }

        }

        connect(payloadSpatialResolution,SIGNAL(valueChanged(double )), this, SLOT(PayloadDefinition()));
        connect(PayloadNumberBit,SIGNAL(valueChanged(int )), this, SLOT(PayloadDefinition()));
        connect(PayloadWidthSquare,SIGNAL(valueChanged(double )), this, SLOT(PayloadDefinition()));
        connect(PayloadQuality,SIGNAL(valueChanged(double )), this, SLOT(PayloadDefinition()));
        connect(PayloadWavelength,SIGNAL(valueChanged(double )), this, SLOT(PayloadDefinition()));
        connect(PaylodOpticsCalculate, SIGNAL(clicked()), this, SLOT(PayloadViewStore()));
        connect(PayloadDesignCalculate, SIGNAL(clicked()), this, SLOT(PayloadDesign()));
        connect(PayloadDesignCalculate, SIGNAL(clicked()), this, SLOT(sum()));
        connect(PayloadSolarCheckBox, SIGNAL(stateChanged(int )), this, SLOT(PayloadSolarCombo()));
        connect(PayloadEnvironmentalCheckBox, SIGNAL(stateChanged(int )), this, SLOT(PayloadEnvironmentalCombo()));
        connect(PayloadResourcesCheckBox, SIGNAL(stateChanged(int )), this, SLOT(PayloadResourcesCombo()));
        connect(PayloadOtherCheckBox, SIGNAL(stateChanged(int )), this, SLOT(PayloadOther()));
        connect(payloadSolarCombo, SIGNAL(activated(int )), this, SLOT(PayloadSolarCombo()));
        connect(payloadEnvironmentalCombo, SIGNAL(activated(int )), this, SLOT(PayloadEnvironmentalCombo()));
        connect(payloadResourcesCombo, SIGNAL(activated(int )), this, SLOT(PayloadResourcesCombo()));

        connect(LauncherComboBox, SIGNAL(activated(int )), this, SLOT(LauncherSelection()));
        connect(LauncherStoreData, SIGNAL(clicked()), this, SLOT(LauncherStore()));

        connect(overviewCalculate, SIGNAL(clicked()), this , SLOT(OverviewCalculate()));
        connect(overviewUpdate, SIGNAL(clicked()), this , SLOT(OverviewUpdate()));

    }



SEM::~SEM()
{
}



//__________________________________________MISSION DEFINITION_________________________________________________________
//_____________________________________________________________________________________________________________________

void SEM::missionCalculations()
{

            //We consider only missions with Earth as central body for now
                StaBody* stabody = STA_SOLAR_SYSTEM->lookup(STA_EARTH);
                double Radius = stabody->meanRadius(); //km
                double MU = stabody->mu()*pow(10.0,9.0);

                AtmosphereModel atmosphere;

                Mission.Eccentricity = Eccentricity->text().toDouble();
                Mission.SemimajorAxis = SemimajorAxis->text().toDouble();
                Mission.Inclination = Inclination->text().toInt();


                Mission.elevation=OrbitElevationAngle->value();

                calculateParameters(Radius);
                //Mission.CircularAltitude=CircularAltitude->text().toDouble();
                Mission.SemimajorAxis = SemimajorAxis->text().toDouble();
                Mission.PerigeeAltitude = PerigeeAltitude->text();
                Mission.ApogeeAltitude = ApogeeAltitude->text();
                calculateDynamics(Radius, MU);
                calculateGeometry(Radius);

                if(Mission.Eccentricity==0)
                        {Mission.Density=atmosphere.getDensity(3,Mission.CircularAltitude);
                        MaintenanceWidget->setEnabled(true);
                        AltitudeOverview->setText(QString::number(Mission.CircularAltitude));
                        PerigeeOverview->setText("N/A");
                        ApogeeOverview->setText("N/A");
                        Mission.Apogee=Mission.CircularAltitude;
                        Mission.Perigee=Mission.CircularAltitude;
                        }
                        else
                        {
                        Mission.Density=atmosphere.getDensity(3,Mission.PerigeeAltitude.toDouble());
                        MaintenanceWidget->setEnabled(true);
                        AltitudeOverview->setText("N/A");
                        PerigeeOverview->setText(QString::number(Mission.PerigeeAltitude.toDouble()));
                        ApogeeOverview->setText(QString::number(Mission.ApogeeAltitude.toDouble()));
                        Mission.Perigee=Mission.PerigeeAltitude.toDouble();
                        Mission.Apogee=Mission.ApogeeAltitude.toDouble();
                        }

            //in subsystem design:
            //AtmDens->setText(QString::number(Mission.Density));
            double SCradius=Radius+Mission.Perigee;
            PlanetView.AngularRadiusPerigee=asin(pow((SCradius/Radius),-1)); //rad

            PlanetView.EclipseTime=PlanetView.AngularRadiusPerigee/Pi*(Radius+Mission.Perigee)/Mission.SemimajorAxis*Mission.OrbitPeriod; //min
            //in subsystem design:
            //PowerEclipse->setText(QString::number(PlanetView.EclipseTime/60));

                InclinationOverview->setText(QString::number(Mission.Inclination));
                EccentricityOverview->setText(QString::number(Mission.Eccentricity));

}

void SEM::calculateParameters(double Radius)
{
        if(Mission.Eccentricity==0)
        {
                textEdit->setText("");
                Mission.PerigeeAltitude="N/A";
                Mission.ApogeeAltitude="N/A";
                CircularAltitude->setEnabled(true);
                PerigeeAltitude->setText(Mission.PerigeeAltitude);
                ApogeeAltitude->setText(Mission.ApogeeAltitude);

                if(Mission.SemimajorAxis>Radius)
                {
                        textEdit->setText("");
                        if (Mission.SemimajorAxis<(Radius+175))
                        {
                                textEdit->setTextColor(Qt::black);
                                textEdit->setText("You'd better increase the semimajoraxis. Unusual orbit altitude");
                        }
                        Mission.CircularAltitude=(Mission.SemimajorAxis-Radius);
                        CircularAltitude->setText(QString::number(Mission.CircularAltitude));

                }
                else
                {
                        textEdit->setTextColor(Qt::red);
                        textEdit->setText("Semimajor Axis value is lower than planet radius");
                        Mission.CircularAltitude=0;
                        CircularAltitude->setText("0");

                }
        }

        else
        {
                CircularAltitude->setEnabled(false);

                if(Mission.SemimajorAxis>Radius)
                        {
                                CircularAltitude->setText(QString::number(0));
                                Mission.PerigeeAltitude=QString::number(Mission.SemimajorAxis*(1-Mission.Eccentricity)-Radius);
                                Mission.CircularAltitude=Mission.PerigeeAltitude.toDouble();
                                Mission.ApogeeAltitude=QString::number(Mission.SemimajorAxis*(1+Mission.Eccentricity)-Radius);
                                PerigeeAltitude->setText(Mission.PerigeeAltitude);
                                ApogeeAltitude->setText(Mission.ApogeeAltitude);
                                textEdit->setText("");
                                if(Mission.PerigeeAltitude.toDouble()<0)
                                {
                                        textEdit->setTextColor(Qt::red);
                                        textEdit->setText("You cannot use this value for the eccentricity cause the orbit would enter into the Planet.  Decrease the eccentricity or increase the semimajoraxis");

                                }
                                else if(Mission.PerigeeAltitude.toDouble()<175)
                                {
                                        textEdit->setTextColor(Qt::black);
                                        textEdit->setText("You'd better increase the semimajoraxis or decrease the eccentricity. High atmospheric drag at perigee passage");
                                }

                        }
                        else
                        {
                                CircularAltitude->setText(QString::number(0));
                                textEdit->setTextColor(Qt::red);
                                textEdit->setText("Semimajor Axis value is lower than planet radius");
                                Mission.PerigeeAltitude="N/A";
                                Mission.ApogeeAltitude="N/A";
                                PerigeeAltitude->setText(Mission.PerigeeAltitude);
                                ApogeeAltitude->setText(Mission.ApogeeAltitude);
                        }

        }
}

void SEM::calculateDynamics(double Radius, double MU)

{
        Radius=Radius*1000;
        Mission.OrbitPeriod=2*Pi*pow((pow(Mission.SemimajorAxis*1000,3)/MU),0.5); //s
        Mission.AngularVelocity=pow((MU/pow(Mission.SemimajorAxis*1000,3)),0.5);
        Mission.GroundVelocity=2*Pi*(Radius)/Mission.OrbitPeriod/1000;

        OrbitalPeriod->setText(QString::number(Mission.OrbitPeriod/60));
        OrbitalAngVel->setText(QString::number(Mission.AngularVelocity));
        GroundVel->setText(QString::number(Mission.GroundVelocity));

        if(Mission.Eccentricity==0)
        {
                Mission.ApogeeVelocity=0;
                ApogeeVelocity->setText(QString::number(Mission.ApogeeVelocity));
                ApogeeVelocity->setEnabled(false);

                Mission.ApogeeEscVelocity=0;
                ApogeeEscVelocity->setText(QString::number(Mission.ApogeeEscVelocity));
                ApogeeEscVelocity->setEnabled(false);

                Mission.PerigeeVelocity=pow(MU/(Mission.SemimajorAxis*1000),0.5)/1000;
                PerigeeVelocity->setText(QString::number(Mission.PerigeeVelocity));
                PerigeeVelocity->setEnabled(true);

                Mission.PerigeeEscVelocity=pow(2*MU/(Mission.SemimajorAxis*1000),0.5)/1000;
                PerigeeEscVelocity->setText(QString::number(Mission.PerigeeEscVelocity));
                PerigeeEscVelocity->setEnabled(true);
        }
        else
        {

                Mission.ApogeeVelocity=sqrt(2*MU/((Mission.ApogeeAltitude.toDouble()*1000+Radius))-MU/(Mission.SemimajorAxis*1000))/1000;
                ApogeeVelocity->setText(QString::number(Mission.ApogeeVelocity));
                ApogeeVelocity->setEnabled(true);

                Mission.ApogeeEscVelocity=pow(2*MU/((Mission.ApogeeAltitude.toDouble()*1000+Radius)),0.5)/1000;
                ApogeeEscVelocity->setText(QString::number(Mission.ApogeeEscVelocity));
                ApogeeEscVelocity->setEnabled(true);

                Mission.PerigeeVelocity=sqrt(2*MU/((Mission.PerigeeAltitude.toDouble()*1000+Radius))-MU/(1000*Mission.SemimajorAxis))/1000;
                PerigeeVelocity->setText(QString::number(Mission.PerigeeVelocity));
                PerigeeVelocity->setEnabled(true);

                Mission.PerigeeEscVelocity=pow(2*MU/((Mission.PerigeeAltitude.toDouble())*1000+Radius),0.5)/1000;
                PerigeeEscVelocity->setText(QString::number(Mission.PerigeeEscVelocity));
                PerigeeEscVelocity->setEnabled(true);

        }
}
void SEM::calculateGeometry(double Radius)
{

                Mission.angularRadius=PlanetView.AngularRadiusPerigee*180.0/Pi;

                Mission.nadir=asin(cos(Mission.elevation*Pi/180.0)*sin(PlanetView.AngularRadiusPerigee))*180.0/Pi; //deg

                Mission.centralAngle=90-Mission.nadir-Mission.elevation; //deg
                Mission.slantRange=6378.136*sin(Mission.centralAngle*Pi/180.0)/sin(Mission.nadir*Pi/180.0); //km

                //CommPropagation->setText(QString::number(Mission.slantRange));
                //CommElevation->setText(QString::number(Mission.elevation));
                OrbitSlantRange->setText(QString::number(Mission.slantRange));
                OrbitAngularRadius->setText(QString::number(Mission.angularRadius));
                OrbitNadirAngle->setText(QString::number(Mission.nadir));

                double lam0=acos(cos(Mission.elevation*Pi/180.0)*sin(PlanetView.AngularRadiusPerigee))*180.0/Pi; //deg
                Mission.maxHorizon=Radius*tan(lam0*Pi/180.0);
                OrbitDistanceHorizon->setText(QString::number(Mission.maxHorizon));

                Mission.swathWidth=2*Mission.centralAngle;
                OrbitSwathWidth->setText(QString::number(Mission.swathWidth));

                Mission.longitudeShift=Mission.OrbitPeriod/60/1436*360;
                OrbitNodeShift->setText(QString::number(Mission.longitudeShift));
}


void SEM::ManeuverPlan()
{

                if (TransferComboBox->currentIndex() == 0)
        {
                //PropulsionOrbitTips->setText("");
                //StackedPropulsionInternal->setEnabled(true);
                        //PropulsionExternOnBoardWidget->setEnabled(true);

                FromParkingTotal->setEnabled(true);
                        InclinationWidget->setEnabled(true);
                        //PropulsionRequirementsOrbitGroup->setEnabled(true);
                        //PropulsionExternalPropMass->setText("");
                        //PropulsionOnboardPropMass->setText("");
                        //PropulsionSolidMassBOLMotor->setText("");
                        //PropulsionSolidMassEOLMotor->setText("");
                        //PropulsionSolidMassBOLEngine->setText("");
                        //PropulsionSolidMassEOLEngine->setText("");

                        //PropulsionPresMass1_2->setText("");
                        //PropulsionTankMass1_2->setText("");
                        //PropulsionPresMass1->setText("");
                        //PropulsionTankMass1->setText("");
        }
        else if (TransferComboBox->currentIndex() == 1)
        {
                //PropulsionOrbitTips->setText("");
                //StackedPropulsionInternal->setEnabled(true);
                //        PropulsionExternOnBoardWidget->setEnabled(true);

                FromParkingTotal->setEnabled(true);
                        InclinationWidget->setEnabled(false);
                        ParkingInclination->setValue(Mission.Inclination);
//                        PropulsionRequirementsOrbitGroup->setEnabled(true);
//
//                        PropulsionExternalPropMass->setText("");
//                        PropulsionOnboardPropMass->setText("");
//                        PropulsionSolidMassBOLMotor->setText("");
//                        PropulsionSolidMassEOLMotor->setText("");
//                        PropulsionSolidMassBOLEngine->setText("");
//                        PropulsionSolidMassEOLEngine->setText("");
//
//                        PropulsionPresMass1_2->setText("");
//                        PropulsionTankMass1_2->setText("");
//                        PropulsionPresMass1->setText("");
//                        PropulsionTankMass1->setText("");

                }
        else if (TransferComboBox->currentIndex() == 2)
        {
                FromParkingTotal->setEnabled(false);
                        InclinationWidget->setEnabled(false);
                        HohTime->setText("");
                        TotTransfDv->setText("");
//                        PropulsionRequirementsOrbitGroup->setEnabled(false);
//                        StackedPropulsionInternal->setCurrentIndex(2);
//                        PropulsionExternOnBoardWidget->setCurrentIndex(4);
//
//                        PropulsionExternalPropMass->setText("0");
//                        PropulsionOnboardPropMass->setText("0");
//                        PropulsionSolidMassBOLMotor->setText("0");
//                        PropulsionSolidMassEOLMotor->setText("0");
//                        PropulsionSolidMassBOLEngine->setText("0");
//                        PropulsionSolidMassEOLEngine->setText("0");
//
//                        PropulsionPresMass1_2->setText("0");
//                        PropulsionTankMass1_2->setText("0");
//                        PropulsionPresMass1->setText("0");
//                        PropulsionTankMass1->setText("0");



                        Maneuver.TotalTransferDV=0;
                        Maneuver.HohmannTime=0;
                        //PropulsionOrbitTips->setText("You selected a different transfer option. No transfer motors or engines are considered here.");

                }

}

void SEM::CalculateManeuvres()
{

        //ManeuverPlan();
        //We consider only missions with Earth as central body for now
        StaBody* stabody = STA_SOLAR_SYSTEM->lookup(STA_EARTH);
        double radius = stabody->meanRadius(); //km
        double MU = stabody->mu()*pow(10.0,9.0);

        if (TransferComboBox->currentIndex() != 2)
    {
        Maneuver.ParkOrbitAltitude = ParkingOrbit->text().toDouble();
                Maneuver.ParkOrbitInclination = ParkingInclination->text().toInt();
                Maneuver.InclChange = PerigeeInclChange->text().toDouble();
                ApogeeInclChange->setText(QString::number(100-Maneuver.InclChange));

                double Di1=abs(Maneuver.ParkOrbitInclination-Mission.Inclination)*Maneuver.InclChange/100;
                double Di2=abs(Maneuver.ParkOrbitInclination-Mission.Inclination)-Di1;
                double parkingRadius=Maneuver.ParkOrbitAltitude+radius; //km
                double operationalRadius=Mission.Apogee+radius; //km
                double semimajorTransfer=(parkingRadius+operationalRadius)/2;

                Maneuver.HohmannTime=Pi/60*pow((pow((semimajorTransfer*1000),3)/MU),0.5)/2;
                HohTime->setText(QString::number(Maneuver.HohmannTime));


                double VcircPark=sqrt(MU/(parkingRadius*1000));
                double VcircOperational=sqrt(MU/(operationalRadius*1000));
                double eTransfer=(operationalRadius-parkingRadius)/(operationalRadius+parkingRadius);
                double VperigeeTransfer=VcircPark*sqrt(1+eTransfer);
                double VapogeeTransfer=VcircOperational*sqrt(1-eTransfer);

                double DVperigee=sqrt(pow(VcircPark,2)+pow(VperigeeTransfer,2)-2*VcircPark*VperigeeTransfer*cos(Di1*Pi/180));
                double DVapogee=sqrt(pow(VapogeeTransfer,2)+pow(Mission.PerigeeVelocity*1000,2)-2*VapogeeTransfer*Mission.PerigeeVelocity*1000*cos(Di2*Pi/180));

                Maneuver.TotalTransferDV=DVperigee+DVapogee;
                TotTransfDv->setText(QString::number(Maneuver.TotalTransferDV));
    }
        else
        {
                Maneuver.HohmannTime=0;
                                HohTime->setText(QString::number(Maneuver.HohmannTime));
                                Maneuver.TotalTransferDV=0;
                                                TotTransfDv->setText(QString::number(Maneuver.TotalTransferDV));
        }


        //DragCoeff->setValue(DragCoeffManeuvers->text().toDouble());
        //PerturbTorques.DragCoeff=DragCoeffManeuvers->text().toDouble();

        //if(Mission.Eccentricity==0)
        //{
                WarningManeuvers->setText(" ");
                if(!(SAarea->text()!=NULL && TotalMass->text()!=NULL))
                {
                        WarningManeuvers->setTextColor(Qt::red);
                        WarningManeuvers->setText("For the calculation of the maintenance dV a section area of the S/C and the Mass are required. Perform the preliminary estimation first.");
                        Maneuver.TotalMaintDV=0;
                }
                else
                {
                        WarningManeuvers->setTextColor(Qt::black);
                        double semimajorLoss=2*Pi*(PerturbTorques.DragCoeff*(PL.Area+PL.SAarea )/TotalMass_2->text().toDouble())*Mission.Density*pow(Mission.SemimajorAxis*1000,2);
                        Maneuver.MissionDuration=MissionDuration->text().toDouble();

                        double KmLoss=semimajorLoss/1000*(365*60*60*24/(Mission.OrbitPeriod))*Maneuver.MissionDuration;
                        double DVperKM=-sqrt(MU/(Mission.SemimajorAxis*1000))+sqrt(MU/((Mission.SemimajorAxis-1)*1000));

                        Maneuver.TotalMaintDV=DVperKM*KmLoss;
                        TotalMaintDV->setText(QString::number(Maneuver.TotalMaintDV));
                }

                Maneuver.DisposalAltitude=DisposalOrbit->text().toDouble();

                double operationalRadius=Mission.Apogee+radius; //km
                double disposalRadius=Maneuver.DisposalAltitude+radius;//km

                //double VcircDisposal=sqrt(MU/(disposalRadius*1000));
                double VcircOperational=sqrt(MU/(operationalRadius*1000)); //m/s

                //double VcircOperational=sqrt(MU/(operationalRadius*1000));
                double eDisposal=(operationalRadius-disposalRadius)/(operationalRadius+disposalRadius);

                //double VperigeeDisposal=VcircDisposal*sqrt(1+eDisposal);
                double VapogeeDisposal=VcircOperational*sqrt(1-eDisposal); //m/s

                double DVdisposal=VapogeeDisposal*(1-sqrt(2*(radius*1000+Maneuver.DisposalAltitude*1000)/(2*radius*1000+Maneuver.DisposalAltitude*1000+Mission.Apogee*1000))); //m/s

                //double DVapogee2=VcircOperational-VapogeeDisposal;


                Maneuver.TotalDisposalDV=DVdisposal;
                TotalDisposalDV->setText(QString::number(Maneuver.TotalDisposalDV));
//	}
        //else
        //{
                //Maneuver.TotalMaintDV=0;
                //TotalMaintDV->setText(QString::number(Maneuver.TotalMaintDV));
                //Maneuver.TotalDisposalDV=0;
                //TotalDisposalDV->setText(QString::number(Maneuver.TotalDisposalDV));
        //}

        Maneuver.TotalManeuverDV=Maneuver.TotalDisposalDV+Maneuver.TotalMaintDV+Maneuver.TotalTransferDV+UserDV->text().toDouble();
        TotalManeuversDV->setText(QString::number(Maneuver.TotalManeuverDV));
        //PropulsionOrbitChangeDV->setText(QString::number(Maneuver.TotalTransferDV+UserDV->text().toDouble()));
        //PropulsionOrbitMaintenanceDV->setText(QString::number(Maneuver.TotalDisposalDV+Maneuver.TotalMaintDV));

        //to include when the other tabs of the SEM gui will be included:
//        if(Maneuver.TotalManeuverDV >=0 && Maneuver.TotalManeuverDV<=100)
//        {
//                EquipmentAdvices->setTextColor(Qt::black);
//                EquipmentAdvices->setText("One set of thrusters should be sufficient. The ADCS should be provided with a thrusting control law and an Onboard attitude reference.");
//                EquipmentAdvices->append(" ");
//            EquipmentAdvices->append("[Wertz and Larson, SMAD 3th edition] ");
//        }
//        else if(Maneuver.TotalManeuverDV >100 && Maneuver.TotalManeuverDV<=1000)
//        {
//                EquipmentAdvices->setTextColor(Qt::black);
//                EquipmentAdvices->setText("More thrusters, but may be enough if coasting phase uses thrusters.");
//                EquipmentAdvices->append("A separate control law for the thrusting phases should be foreseen. The ADCS actuators must be sized for thrusting disturbances. Onboard attitude reference for thrusting phases.");
//                EquipmentAdvices->append(" ");
//            EquipmentAdvices->append("[Wertz and Larson, SMAD 3th edition] ");
//        }
//        else if(Maneuver.TotalManeuverDV >1000 && Maneuver.TotalManeuverDV<=5000)
//        {
//                EquipmentAdvices->setTextColor(Qt::black);
//                EquipmentAdvices->setText("Solid motor or large bipropellant stage.");
//                EquipmentAdvices->append("Large thrusters or a gimbaled engine or spin stabilization for attitude control during burns");
//                EquipmentAdvices->append(" ");
//                EquipmentAdvices->setText("IMU for accurate reference and velocity measurements. Different actuators, sensors, and control laws for the burn or coasting phases are required. Need for navigation or guidance.");
//                EquipmentAdvices->append(" ");
//                EquipmentAdvices->append("[Wertz and Larson, SMAD 3th edition] ");
//        }
//        else
//        {
//                EquipmentAdvices->setTextColor(Qt::red);
//                EquipmentAdvices->setText("The total dV you require is very high to be achieved by the spacecraft alone. Think on different solutions if possible");
//        }
//

        if (DisposalOrbit->value()<50)
                {
                        WarningManeuvers->setTextColor(Qt::red);
                        WarningManeuvers->append("Unusual Disposal orbit altitude, too low. Consider changing it.");
                }
                else
                {
                        WarningManeuvers->setTextColor(Qt::black);
                }

        DurationOverview->setText(MissionDuration->text());
        //PowerMissionDuration->setText(MissionDuration->text());
        //Power.Ncycles=Maneuver.MissionDuration*365*24*60*60/Mission.OrbitPeriod;
        //PowerCycles->setText(QString::number(Power.Ncycles));
        DV->setValue(TotalManeuversDV->text().toDouble());



}

//______________________________________Payload Definition____________________________________________________________
//_____________________________________________________________________________________________________________________

void SEM::PayloadDefinition()
{
        //if (PayloadSpatialResolution->value()==0 || PayloadFrequency->value()==0)
        //{
        //	PayloadViewTips->setText("Insert the needed values. Insert Required resolution and Frequency.");
        //	PayloadViewTips->append("Data are not stored in SEM. Press the StoreData button to share data with the other subsystems.");
        //}
        //else
        //	PayloadViewTips->append("Data are not stored in SEM. Press the StoreData button to share data with the other subsystems.");

        PayloadViewTips->setText("Insert the needed data. Data are not stored in SEM");
        Payload.spatialResolution = payloadSpatialResolution->value();//m
        Payload.bitPerPixel=PayloadNumberBit->value();
        Payload.width=PayloadWidthSquare->value(); // m^-6
        Payload.imagingQuality=PayloadQuality->value();
        Payload.wavelength=PayloadWavelength->value(); //m^-6
        double altitude=Mission.Perigee;

        double IFOV=Payload.spatialResolution/(Mission.slantRange*1000)*180.0/Pi;//deg
        PayloadFOV->setText(QString::number(IFOV));

        double alongRes=IFOV*altitude*1000*(Pi/180.0);
        PayloadAlongPixel->setText(QString::number(alongRes));

        double crossRes=alongRes;
        PayloadCrossPixel->setText(QString::number(crossRes));

        double crossPixels=2*Mission.nadir/IFOV;
        PayloadNumberCross->setText(QString::number(crossPixels));

        double swatsPerSecond=Mission.GroundVelocity*1000/Payload.spatialResolution;
        PayloadNumberSwats->setText(QString::number(swatsPerSecond));

        double pixelPerSec=crossPixels*swatsPerSecond;
        double dataRate=pixelPerSec*Payload.bitPerPixel/pow(10.0,3.0);
        PayloadDataRate->setText(QString::number(dataRate));

        double focal=altitude*1000*Payload.width*pow(10.0,-6.0)/crossRes; //m
        PayloadFocalLength->setText(QString::number(focal));

        double aperture=2.44*Payload.wavelength*pow(10.0,-6.0)*focal*Payload.imagingQuality/(Payload.width*pow(10.0,-6.0));
        PayloadDiameter->setText(QString::number(aperture));

        double groundMax=2.44*Payload.wavelength*pow(10.0,-6.0)*Mission.slantRange*1000/aperture/sin(Mission.elevation*Pi/180.0); //m
        PayloadGroundMaxRange->setText(QString::number(groundMax));

}


void SEM::PayloadViewStore()
{
        PayloadDefinition();
        PayloadViewTips->setText("");

        if (PayloadAlongPixel->text().toDouble() > Payload.spatialResolution)
        {
                PayloadViewTips->setText("The resolution higher than the required one. Reduce the requirement or decrease the altitude of the orbit or increase the minimum elevation angle.");
                PayloadViewTips->append("Data are stored in SEM");
        }
        if (payloadSpatialResolution->value() == 0.0 || PayloadWavelength->value() == 0.0)
        {
                PayloadViewTips->setText("Insert the needed data. Data are not stored in SEM");
        }
        else
        {
                PayloadViewTips->setText("Data are stored in SEM");

                Payload.IFOV=PayloadFOV->text().toDouble();
                Payload.alongRes=PayloadAlongPixel->text().toDouble();
                Payload.crossRes=PayloadCrossPixel->text().toDouble();
                Payload.crossPixels=PayloadNumberCross->text().toDouble();
                Payload.swatsPerSecond=PayloadNumberSwats->text().toDouble();
                Payload.dataRate=PayloadDataRate->text().toDouble();
                Payload.wavelength=PayloadWavelength->text().toDouble();
                Payload.focal=PayloadFocalLength->text().toDouble();
                Payload.aperture=PayloadDiameter->text().toDouble();
                Payload.groundMax=PayloadGroundMaxRange->text().toDouble();


                PayloadDataRateOther->setValue(Payload.dataRate);

        }
}


void SEM::PayloadSolarCombo()
{
        if(PayloadSolarCheckBox->isChecked())
        {
                payloadSolarCombo->setEnabled(true);
                PayloadMassSolar->setEnabled(true);
                PayloadPowerSolar->setEnabled(true);
                PayloadDataRateSolar->setEnabled(true);
                PayloadSolarSizingRatio->setEnabled(true);

                if(payloadSolarCombo->currentIndex()==0)
                {
                        PayloadMassSolar->setText("465");
                        PayloadPowerSolar->setText("30");
                        PayloadDataRateSolar->setText("400");

                }
                if(payloadSolarCombo->currentIndex()==1)
                {
                        PayloadMassSolar->setText("128");
                        PayloadPowerSolar->setText("164");
                        PayloadDataRateSolar->setText("1280");

                }

        }
        else
        {
                payloadSolarCombo->setEnabled(false);
                PayloadMassSolar->setText("");
                PayloadPowerSolar->setText("");
                PayloadDataRateSolar->setText("");
                payloadSolarCombo->setEnabled(false);
                PayloadMassSolar->setEnabled(false);
                PayloadPowerSolar->setEnabled(false);
                PayloadDataRateSolar->setEnabled(false);
                PayloadSolarSizingRatio->setEnabled(false);

        }

}


void SEM::PayloadEnvironmentalCombo()
        {
        if(PayloadEnvironmentalCheckBox->isChecked())
                {
                        payloadEnvironmentalCombo->setEnabled(true);
                        PayloadMassEnvironmental->setEnabled(true);
                        PayloadPowerEnvironmental->setEnabled(true);
                        PayloadDataRateEnvironmental->setEnabled(true);
                        PayloadEnvironmentalSizingRatio->setEnabled(true);

                        if(payloadEnvironmentalCombo->currentIndex()==0)
                        {
                                PayloadMassEnvironmental->setText("16");
                                PayloadPowerEnvironmental->setText("60");
                                PayloadDataRateEnvironmental->setText("100");

                        }
                        if(payloadEnvironmentalCombo->currentIndex()==1)
                        {
                                PayloadMassEnvironmental->setText("191");
                                PayloadPowerEnvironmental->setText("165");
                                PayloadDataRateEnvironmental->setText("20000");

                        }
                        if(payloadEnvironmentalCombo->currentIndex()==2)
                        {
                                PayloadMassEnvironmental->setText("147");
                                PayloadPowerEnvironmental->setText("330");
                                PayloadDataRateEnvironmental->setText("10");

                        }

                }
                else
                {
                        payloadEnvironmentalCombo->setEnabled(false);

                        PayloadMassEnvironmental->setText("");
                        PayloadPowerEnvironmental->setText("");
                        PayloadDataRateEnvironmental->setText("");
                        PayloadMassEnvironmental->setEnabled(false);
                        PayloadPowerEnvironmental->setEnabled(false);
                        PayloadDataRateEnvironmental->setEnabled(false);
                        PayloadEnvironmentalSizingRatio->setEnabled(false);

                }
        }


        void SEM::PayloadResourcesCombo()
        {
                if(PayloadResourcesCheckBox->isChecked())
                                {
                                        payloadResourcesCombo->setEnabled(true);

                                        PayloadMassResources->setEnabled(true);
                                        PayloadPowerResources->setEnabled(true);
                                        PayloadDataRateResources->setEnabled(true);
                                        PayloadApertureResources->setEnabled(true);

                                        if(payloadResourcesCombo->currentIndex()==0)
                                        {
                                                PayloadMassResources->setText("10");
                                                PayloadPowerResources->setText("1");
                                                PayloadDataRateResources->setText("");
                                                PayloadApertureResources->setText("0.5");
                                        }
                                        if(payloadResourcesCombo->currentIndex()==1)
                                        {
                                                PayloadMassResources->setText("808");
                                                PayloadPowerResources->setText("3000");
                                                PayloadDataRateResources->setText("120000");
                                                PayloadApertureResources->setText("4");
                                        }
                                        if(payloadResourcesCombo->currentIndex()==2)
                                        {
                                                PayloadMassResources->setText("800");
                                                PayloadPowerResources->setText("900");
                                                PayloadDataRateResources->setText("30000");
                                                PayloadApertureResources->setText("1");
                                        }
                                        if(payloadResourcesCombo->currentIndex()==3)
                                        {
                                                PayloadMassResources->setText("239");
                                                PayloadPowerResources->setText("280");
                                                PayloadDataRateResources->setText("85000");
                                                PayloadApertureResources->setText("0.4");
                                        }

                                }
                                else
                                {
                                        payloadResourcesCombo->setEnabled(false);

                                        PayloadMassResources->setText("");
                                        PayloadPowerResources->setText("");
                                        PayloadDataRateResources->setText("");
                                        PayloadApertureResources->setText("");

                                        PayloadMassResources->setEnabled(false);
                                        PayloadPowerResources->setEnabled(false);
                                        PayloadDataRateResources->setEnabled(false);
                                        PayloadApertureResources->setEnabled(false);
                                }
        }

void SEM::PayloadOther()
{
        if(PayloadOtherCheckBox->isChecked())
        {
                PayloadWidgetOptic->setEnabled(true);
        }
        else
        {
                PayloadWidgetOptic->setEnabled(false);
                PayloadMassOther->setValue(0.0);
                PayloadPowerOther->setValue(0.0);
                PayloadDataRateOther->setValue(0.0);
                PayloadApertureOther->setValue(0.0);
        }
}

void SEM::PayloadDesign()
{
        double solarSizing=PayloadSolarSizingRatio->value();
        double environmentSizing=PayloadEnvironmentalSizingRatio->value();
        double apertureNew=PayloadApertureRatio->value();
        double scaling=PayloadScalingFactor->value();

        double mass=0.0;
        double power=0.0;
        double dataRate=0.0;

        if(PayloadSolarCheckBox->isChecked())
        {
                mass=mass+scaling*pow(solarSizing,3)*PayloadMassSolar->text().toDouble();
                power=power+scaling*pow(solarSizing,3)*PayloadPowerSolar->text().toDouble();
                dataRate=dataRate+scaling*solarSizing*PayloadDataRateSolar->text().toDouble();
        }
        if(PayloadEnvironmentalCheckBox->isChecked())
        {
                mass=mass+scaling*pow(environmentSizing,3)*PayloadMassEnvironmental->text().toDouble();
                power=power+scaling*pow(environmentSizing,3)*PayloadPowerEnvironmental->text().toDouble();
                dataRate=dataRate+scaling*environmentSizing*PayloadDataRateEnvironmental->text().toDouble();
        }
        if(PayloadResourcesCheckBox->isChecked())
        {
                double sizingResources=apertureNew/PayloadApertureResources->text().toDouble();
                mass=mass+scaling*pow(sizingResources,3)*PayloadMassResources->text().toDouble();
                power=power+scaling*pow(sizingResources,3)*PayloadPowerResources->text().toDouble();
                dataRate=dataRate+scaling*sizingResources*PayloadDataRateResources->text().toDouble();
        }
        if(PayloadOtherCheckBox->isChecked())
                {
                        double massOther = PayloadMassOther->value();
                        double powerOther = PayloadPowerOther->value();
                        double dataRateOther = PayloadDataRateOther->value();
                        double apertureOther = PayloadApertureOther->value();
                        double sizingOther = apertureNew/apertureOther;
                        mass = mass+scaling*pow(sizingOther,3)*massOther;
                        power = power+scaling*pow(sizingOther,3)*powerOther;
                        dataRate = dataRate+scaling*sizingOther*dataRateOther;
                }
        Payload.mass=mass;
        Payload.power=power;
        Payload.finalDataRate=dataRate;
        PayloadNewMass->setText(QString::number(Payload.mass));
        PayloadNewPower->setText(QString::number(Payload.power));
        PayloadNewDataRate->setText(QString::number(Payload.finalDataRate));

        PayloadMassOverview->setText(QString::number(Payload.mass));
        PayloadPowerOverview->setText(QString::number(Payload.power));
        PLMass->setValue(Payload.mass);
        PLPower->setValue(Payload.power);
        //CommTelemetryDR->setValue(Payload.finalDataRate); //kbps
        DataRateBudgetOverview->setText(QString::number(Payload.finalDataRate)); //kbps
        //sum();
        CalculateManeuvres();
}


//__________________________________________PRELIMINARY DESIGN__________________________________________________
//____________________________________________________________________________________________________________________

int SEM::select_table(int index)
{
    int type;
    if(index==0)
    {
        type=1;
    }
    if(index==1)
    {
        type=2;
    }
    if(index==2)
    {
        type=3;
    }
    if(index==3)
    {
        type=4;
    }
    return type;
}

void SEM::sum(){
        //m_a = lineEdit->text().toDouble();
        //m_b = lineEdit_2->text().toDouble();
        //PL.Mass = PLMass->text().toDouble();

        //We consider only missions with Earth as central body for now
        StaBody* stabody = STA_SOLAR_SYSTEM->lookup(STA_EARTH);
        double Radius = stabody->meanRadius(); //km
        double GravConst = stabody->mu()*pow(10.0,9.0);
        //TO DO : the solar flux should be integrated in the Astro-Core:
        double SolarFlux = 1367;

        PL.Mass = PLMass->text().toDouble();
        PL.MassPerc = PLPerc->text().toDouble();
        PL.Power = PLPower->text().toDouble();
        PL.PowerPerc = PLPowPerc->text().toDouble();

        //if(TotalManeuversDV->text()!="")
        //{
        //	PL.DV=TotalManeuversDV->text().toDouble();
        //	TotalManeuversDV->setText(TotalManeuversDV->text());
        //}
        //else
        //{
                PL.DV = DV->text().toDouble();
                //}

        PL.ISP=ISP->text().toDouble();
        PL.PowerMargin=PowerMargin->text().toDouble();
        PL.MassMargin=MassMargin->text().toDouble();
        PL.PropellantMargin=PropellantMargin->text().toDouble();
        PL.PropellantResidual=PropellantResidual->text().toDouble();

        int type=select_table(MissionComboBox->currentIndex());
        Breackdown Mass = createTablesMassBreackdown(createTables(type,PL.MassPerc),PL.Mass);
        Breackdown Power = createTablesPowerBreackdown(createTablesPower(PL.Power,PL.PowerPerc),PL.Power);

        PayloadMass->setText(QString::number(Mass.Payload));
        StructureMass->setText(QString::number(Mass.Structure));
        ThermalMass->setText(QString::number(Mass.Thermal));
        PowerMass->setText(QString::number(Mass.Power));
        TTCMass->setText(QString::number(Mass.TTC));
        ADCSMass->setText(QString::number(Mass.ADCS));
        PropulsionMass->setText(QString::number(Mass.Propulsion));

        double masstotal=Mass.Total+Mass.Total*PL.MassMargin/100;

        TotalMass->setText(QString::number(masstotal));
        PL.TotalDryMass=masstotal;

        PayloadPower->setText(QString::number(Power.Payload));
        StructurePower->setText(QString::number(Power.Structure));
        ThermalPower->setText(QString::number(Power.Thermal));
        PowerPower->setText(QString::number(Power.Power));
        TTCPower->setText(QString::number(Power.TTC));
        ADCSPower->setText(QString::number(Power.ADCS));
        PropulsionPower->setText(QString::number(Power.Propulsion));
        TotalPower->setText(QString::number(Power.Total+Power.Total*PL.PowerMargin/100));
        TotalPower_2->setText(QString::number(Power.Total+Power.Total*PL.PowerMargin/100));

        //PowerRequiredPower->setValue(Power.Total+Power.Total*PL.PowerMargin/100);
        //ThermalMaxPower->setValue(Power.Total+Power.Total*PL.PowerMargin/100);
        //ThermalMinPower->setValue(Power.Total+Power.Total*PL.PowerMargin/100);

        double PropMass=(masstotal)*(exp(PL.DV/(PL.ISP*GravConst/pow(Radius*1000,2)))-1);
        double PropMassMargin=PropMass+(PropMass*PL.PropellantMargin/100);
        double PropMassTotal=PropMassMargin+PropMassMargin*PL.PropellantResidual/100;
        PropellantMass->setText(QString::number(PropMassTotal));

        TotalMass_2->setText(QString::number(PropMassTotal+masstotal));

        PL.Volume=0.01*(PropMassTotal+masstotal);
        Volume->setText(QString::number(PL.Volume));

        PL.Linear=0.25*pow((PropMassTotal+masstotal),0.3334);
        Linear->setText(QString::number(PL.Linear));

        PL.Area=PL.Volume/PL.Linear;
        Area->setText(QString::number(PL.Area));

        PL.MOI=1.0/12.0*(PropMassTotal+masstotal)*(3*pow((sqrt(PL.Volume/(Pi*PL.Linear))/2),2)+pow(PL.Linear,2));// considering a cylinder
        MoI->setText(QString::number(PL.MOI));

        PL.moi=0.5*(PropMassTotal+masstotal)*(pow((sqrt(PL.Volume/(Pi*PL.Linear))/2),2));// considering a cylinder
        MoI_2->setText(QString::number(PL.moi));

        PL.SAarea=Power.Total/(0.07*SolarFlux);
        SAarea->setText(QString::number(PL.SAarea));

        PL.SAoffset=1.5*PL.Linear+0.5*pow((PL.SAarea/2),0.5);
        SAoffset->setText(QString::number(PL.SAoffset));

        PL.SAMass=0.04*Power.Total;
        SAMass->setText(QString::number(PL.SAMass));

        PL.SAMoiAboutArrayAxis=(PL.SAarea/24)*PL.SAMass;
        SAMoiAboutArrayAxis->setText(QString::number(PL.SAMoiAboutArrayAxis));

        PL.SAMoiPerpArrayAxis=(pow(PL.SAoffset,2)+PL.SAarea/24)*PL.SAMass;
        SAMoiPerpArrayAxis->setText(QString::number(PL.SAMoiPerpArrayAxis));

        PL.SAMoiPerpArrayFace=(pow(PL.SAoffset,2)+PL.SAarea/12)*PL.SAMass;
        SAMoiPerpArrayFace->setText(QString::number(PL.SAMoiPerpArrayFace));

        PL.PCUMass=0.02*Power.Total;
        PCUMass->setText(QString::number(PL.PCUMass));

        PL.RegulatorMass=0.025*Power.Total;
        RegulatorMass->setText(QString::number(PL.RegulatorMass));

        PL.RegulatorPower=0.2*Power.Total;
        RegulatorPower->setText(QString::number(PL.RegulatorPower));

        PL.WiringMass=0.03*masstotal;
        WiringMass->setText(QString::number(PL.WiringMass));

        PL.WiringPower=0.03*Power.Total;
        WiringPower->setText(QString::number(PL.WiringPower));

        //StructureSCMass->setText(QString::number(PropMassTotal+masstotal));
        //StructureDiameter->setText(QString::number(2*pow((PL.Volume/PL.Linear/Pi),0.5)));
        //StructureLength->setText(QString::number(PL.Linear));

        //ThermalSurface->setText(QString::number(PL.Area));
}

//_____________________________LAUNCHER___________________________________________________________________________
//________________________________________________________________________________________________________________

void SEM::LauncherSelection()
{
        if (LauncherComboBox->currentIndex()==0)
        {
                LauncherWidget->setCurrentIndex(0);
                LauncherDiameter->setText("4.2");
                LauncherLength->setText("12");
                LauncherAxialAcc->setText("5.5");
                LauncherLateralAcc->setText("1.2");
                LauncherAxialFreq->setText("15");
                LauncherLateralFreq->setText("10");
                LauncherMassLEO->setText("6580");
                LauncherMassGEO->setText("570");
                LauncherTips->setText("Click on store data to allow the other modules to use those data.");
        }
        else if (LauncherComboBox->currentIndex()==1)
        {
                LauncherWidget->setCurrentIndex(0);
                LauncherDiameter->setText("4.2");
                LauncherLength->setText("12");
                LauncherAxialAcc->setText("5.5");
                LauncherLateralAcc->setText("1.2");
                LauncherAxialFreq->setText("15");
                LauncherLateralFreq->setText("10");
                LauncherMassLEO->setText("8640");
                LauncherMassGEO->setText("1050");
                LauncherTips->setText("Click on store data to allow the other modules to use those data.");
        }
        else if (LauncherComboBox->currentIndex()==2)
        {
                LauncherWidget->setCurrentIndex(0);
                LauncherDiameter->setText("2.9");
                LauncherLength->setText("8.5");
                LauncherAxialAcc->setText("2.4");
                LauncherLateralAcc->setText("6");
                LauncherAxialFreq->setText("35");
                LauncherLateralFreq->setText("15");
                LauncherMassLEO->setText("3990");
                LauncherMassGEO->setText("730");
                LauncherTips->setText("Click on store data to allow the other modules to use those data.");
        }
        else if (LauncherComboBox->currentIndex()==3)
        {
                LauncherWidget->setCurrentIndex(0);
                LauncherDiameter->setText("2.9");
                LauncherLength->setText("8.5");
                LauncherAxialAcc->setText("2.4");
                LauncherLateralAcc->setText("6");
                LauncherAxialFreq->setText("35");
                LauncherLateralFreq->setText("15");
                LauncherMassLEO->setText("5089");
                LauncherMassGEO->setText("910");
                LauncherTips->setText("Click on store data to allow the other modules to use those data.");
        }
        else if (LauncherComboBox->currentIndex()==4) //vega
                {
                        LauncherWidget->setCurrentIndex(0);
                        LauncherDiameter->setText("2.6");
                        LauncherLength->setText("3.5");
                        LauncherAxialAcc->setText("5.5");
                        LauncherLateralAcc->setText("0.9");
                        LauncherAxialFreq->setText("30");
                        LauncherLateralFreq->setText("15");
                        LauncherMassLEO->setText("1500");
                        LauncherMassGEO->setText("");
                        LauncherTips->setText("Click on store data to allow the other modules to use those data.");
                }
        else if (LauncherComboBox->currentIndex()==5) //ariane 5
                        {
                                LauncherWidget->setCurrentIndex(0);
                                LauncherDiameter->setText("5.4");
                                LauncherLength->setText("12.72");
                                LauncherAxialAcc->setText("4.5");
                                LauncherLateralAcc->setText("2");
                                LauncherAxialFreq->setText("30");
                                LauncherLateralFreq->setText("10");
                                LauncherMassLEO->setText("22000");
                                LauncherMassGEO->setText("");
                                LauncherTips->setText("Click on store data to allow the other modules to use those data.");
                        }
        else if (LauncherComboBox->currentIndex()==6)
        {
                LauncherWidget->setCurrentIndex(1);
                LauncherTips->setText("Insert the required parameters.");
        }
}

void SEM::LauncherStore()
{
        LauncherSelection();
        if (LauncherComboBox->currentIndex()==0||LauncherComboBox->currentIndex()==1||LauncherComboBox->currentIndex()==2||LauncherComboBox->currentIndex()==3||LauncherComboBox->currentIndex()==4||LauncherComboBox->currentIndex()==5)
                {
                Launcher.diameter=LauncherDiameter->text().toDouble();
                Launcher.length=LauncherLength->text().toDouble();
                Launcher.axialAcc=LauncherAxialAcc->text().toDouble();
                Launcher.lateralAcc=LauncherLateralAcc->text().toDouble();
                Launcher.axialFreq=LauncherAxialFreq->text().toDouble();
                Launcher.lateralFreq=LauncherLateralFreq->text().toDouble();
                Launcher.massLEO=LauncherMassLEO->text().toDouble();
                Launcher.massGEO=LauncherMassGEO->text().toDouble();
                LauncherTips->setText("Data stored in SEM");

                //StructureAxialLoadFactor->setText(QString::number(Launcher.axialAcc));
                //StructureLateralLoadFactor->setText(QString::number(Launcher.lateralAcc));
                //StructureAxialFreq->setText(QString::number(Launcher.axialFreq));
                //StructureLateralFreq->setText(QString::number(Launcher.lateralFreq));

                }
        else
        {
                if(LauncherDiameter_2->value()==0.0 || LauncherLength_2->value()==0.0 || LauncherAxialAcc_2->value()==0.0 || LauncherLateralAcc_2->value()==0.0 || LauncherAxialFreq_2->value()==0.0 || LauncherLateralFreq_2->value()==0.0 || LauncherMassLEO_2->value()==0.0 || LauncherMassGEO_2->value()==0.0)
                {
                        LauncherTips->setText("Insert proper values for the parameters. No value stored in SEM.");
                }
                else
                {
                        LauncherTips->setText("");
                        Launcher.diameter=LauncherDiameter_2->text().toDouble();
                        Launcher.length=LauncherLength_2->text().toDouble();
                        Launcher.axialAcc=LauncherAxialAcc_2->text().toDouble();
                        Launcher.lateralAcc=LauncherLateralAcc_2->text().toDouble();
                        Launcher.axialFreq=LauncherAxialFreq_2->text().toDouble();
                        Launcher.lateralFreq=LauncherLateralFreq_2->text().toDouble();
                        Launcher.massLEO=LauncherMassLEO_2->text().toDouble();
                        Launcher.massGEO=LauncherMassGEO_2->text().toDouble();
                        LauncherTips->setText("Data stored in SEM");

                        //StructureAxialLoadFactor->setText(QString::number(Launcher.axialAcc));
                        //StructureLateralLoadFactor->setText(QString::number(Launcher.lateralAcc));
                        //StructureAxialFreq->setText(QString::number(Launcher.axialFreq));
                        //StructureLateralFreq->setText(QString::number(Launcher.lateralFreq));
                }
        }

        LauncherOverview->setText(LauncherComboBox->currentText());

}


//____________________________________________OVERVIEW__________________________________________________________
//______________________________________________________________________________________________________________

void SEM::OverviewCalculate()
{
        double totalDryMassLiftOff;
        double totalDryMassBOL;
        double massMargin;
        double powerMargin;
        double totalPower;
        double totalLiftOffMass;
        double totalBOLMass;

        totalDryMassLiftOff=PayloadMassOverview->text().toDouble()+StructureMassOverview->text().toDouble()+ThermalMassOverview->text().toDouble()+PowerMassOverview->text().toDouble()+TTCMassOverview->text().toDouble()+ADCSMassOverview->text().toDouble()+PropulsionMassLiftOffOverview->text().toDouble();
        massMargin=MassMarginOverview->value();
        totalDryMassBOL=PayloadMassOverview->text().toDouble()+StructureMassOverview->text().toDouble()+ThermalMassOverview->text().toDouble()+PowerMassOverview->text().toDouble()+TTCMassOverview->text().toDouble()+ADCSMassOverview->text().toDouble()+PropulsionMassBOLOverview->text().toDouble();
        TotalMassDryOverview->setText(QString::number(totalDryMassLiftOff*(1+massMargin/100.0)));
        TotalMassDryOverviewBOL->setText(QString::number(totalDryMassBOL*(1+massMargin/100.0)));

        totalLiftOffMass=totalDryMassLiftOff*(1+massMargin/100.0)+PropellantBudgetLiftOffOverview->text().toDouble();
        totalBOLMass=totalDryMassBOL*(1+massMargin/100.0)+PropellantBudgetBOLOverview->text().toDouble();

        TotalMassLiftOffOverview->setText(QString::number(totalLiftOffMass));
        TotalMassBOLOverview->setText(QString::number(totalBOLMass));

        powerMargin=PowerMarginOverview->value();
        totalPower=(PayloadPowerOverview->text().toDouble()+StructurePowerOverview->text().toDouble()+ThermalPowerOverview->text().toDouble()+PowerPowerOverview->text().toDouble()+TTCPowerOverview->text().toDouble()+ADCSPowerOverview->text().toDouble()+PropulsionPowerOverview->text().toDouble())*(1+powerMargin/100.0);
        TotalPowerOverview->setText(QString::number(totalPower));
        if (totalLiftOffMass>LauncherMassLEO->text().toDouble() && totalLiftOffMass>LauncherMassLEO_2->value())
        {
                OverviewLauncherTip->setText("The satellite mass exceed the launcher capabilities for LEO");
        }

        if (totalLiftOffMass>LauncherMassGEO->text().toDouble() && totalLiftOffMass>LauncherMassGEO_2->value())
        {
                OverviewLauncherTip->append("The satellite mass exceed the launcher capabilities for GEO");
        }
        overviewUpdate->setEnabled(true);


}
void SEM::OverviewUpdate()
{
        disconnect(PayloadDesignCalculate, SIGNAL(clicked()), this, SLOT(sum()));
        disconnect(calculateManeuvres, SIGNAL(clicked()), this, SLOT(sum()));


                MissionGroupBox->setEnabled(false);
                PreliminaryPayloadGroupBox->setEnabled(false);
                DV->setValue(0.0);
                PLMass->setValue(0.0);
                PLPerc->setValue(0.0);
                PLPower->setValue(0.0);
                PLPowPerc->setValue(0.0);
                PreliminaryPayloadGroupBox->setEnabled(false);
                MassMargin->setValue(0);
                MassMargin->setEnabled(false);
                PowerMargin->setValue(0);
                PowerMargin->setEnabled(false);

                ISP->setValue(0);
                ISP->setEnabled(false);
                PropellantMargin->setValue(0);
                PropellantMargin->setEnabled(false);
                PropellantResidual->setValue(0);
                PropellantResidual->setEnabled(false);
                CalculateButton->setEnabled(false);


                PL.Mass = PayloadMassOverview->text().toDouble();
                PL.Power = PayloadPowerOverview->text().toDouble();


                PayloadMass->setText(QString::number(PL.Mass));
                StructureMass->setText(QString::number(PL.Power));

                ThermalMass->setText(ThermalMassOverview->text());
                PowerMass->setText(PowerMassOverview->text());
                TTCMass->setText(TTCMassOverview->text());
                ADCSMass->setText(ADCSMassOverview->text());
                PropulsionMass->setText(PropulsionMassLiftOffOverview->text());

                double masstotal=TotalMassDryOverview->text().toDouble();

                TotalMass->setText(TotalMassDryOverview->text());

                PL.TotalDryMass=TotalMassDryOverview->text().toDouble();

                PayloadPower->setText(PayloadPowerOverview->text());
                StructurePower->setText(StructurePowerOverview->text());
                ThermalPower->setText(ThermalPowerOverview->text());
                PowerPower->setText(PowerPowerOverview->text());
                TTCPower->setText(TTCPowerOverview->text());
                ADCSPower->setText(ADCSPowerOverview->text());
                PropulsionPower->setText(PropulsionPowerOverview->text());

                TotalPower->setText(TotalPowerOverview->text());
                TotalPower_2->setText(TotalPowerOverview->text());

                //PowerRequiredPower->setValue(TotalPowerOverview->text().toDouble());
                //ThermalMaxPower->setValue(TotalPowerOverview->text().toDouble());
                //ThermalMinPower->setValue(TotalPowerOverview->text().toDouble());

                double PropMassTotal=PropellantBudgetLiftOffOverview->text().toDouble();
                PropellantMass->setText(PropellantBudgetLiftOffOverview->text());


                TotalMass_2->setText(QString::number(PropMassTotal+masstotal));

                PL.Volume=0.01*(PropMassTotal+masstotal);
                Volume->setText(QString::number(PL.Volume));

                PL.Linear=0.25*pow((PropMassTotal+masstotal),0.3334);
                Linear->setText(QString::number(PL.Linear));

                PL.Area=PL.Volume/PL.Linear;
                Area->setText(QString::number(PL.Area));

                PL.MOI=1.0/12.0*(PropMassTotal+masstotal)*(3*pow((sqrt(PL.Volume/(Pi*PL.Linear))/2),2)+pow(PL.Linear,2));// considering a cylinder
                MoI->setText(QString::number(PL.MOI));

                PL.moi=0.5*(PropMassTotal+masstotal)*(pow((sqrt(PL.Volume/(Pi*PL.Linear))/2),2));// considering a cylinder
                MoI_2->setText(QString::number(PL.moi));


                //PL.SAarea=PowerSAarea->text().toDouble();
                SAarea->setText(QString::number(PL.SAarea));

                PL.SAoffset=1.5*PL.Linear+0.5*pow((PL.SAarea/2),0.5);
                SAoffset->setText(QString::number(PL.SAoffset));

                //PL.SAMass=PowerSAmass->text().toDouble();
                SAMass->setText(QString::number(PL.SAMass));

                PL.SAMoiAboutArrayAxis=(PL.SAarea/24)*PL.SAMass;
                SAMoiAboutArrayAxis->setText(QString::number(PL.SAMoiAboutArrayAxis));

                PL.SAMoiPerpArrayAxis=(pow(PL.SAoffset,2)+PL.SAarea/24)*PL.SAMass;
                SAMoiPerpArrayAxis->setText(QString::number(PL.SAMoiPerpArrayAxis));

                PL.SAMoiPerpArrayFace=(pow(PL.SAoffset,2)+PL.SAarea/12)*PL.SAMass;
                SAMoiPerpArrayFace->setText(QString::number(PL.SAMoiPerpArrayFace));

                PL.PCUMass=0.02*TotalPowerOverview->text().toDouble();
                PCUMass->setText(QString::number(PL.PCUMass));

                PL.RegulatorMass=0.025*TotalPowerOverview->text().toDouble();
                RegulatorMass->setText(QString::number(PL.RegulatorMass));

                PL.RegulatorPower=0.2*TotalPowerOverview->text().toDouble();
                RegulatorPower->setText(QString::number(PL.RegulatorPower));

                PL.WiringMass=0.03*masstotal;
                WiringMass->setText(QString::number(PL.WiringMass));

                PL.WiringPower=0.03*TotalPowerOverview->text().toDouble();
                WiringPower->setText(QString::number(PL.WiringPower));

                //StructureSCMass->setText(QString::number(PropMassTotal+masstotal));
                //StructureDiameter->setText(QString::number(2*pow((PL.Volume/PL.Linear/Pi),0.5)));
                //StructureLength->setText(QString::number(PL.Linear));

                //ThermalSurface->setText(QString::number(PL.Area));


                missionCalculations();
                CalculateManeuvres();
                PayloadDefinition();
                PayloadViewStore();
                PayloadDesign();
                //perturbingTorques();
                //ActuatorsDesign();
                //ADCSmassAndPower();
                //ADCSmassAndPower();
                //PowerRequirements();
                //PowerSources();
                //PowerBatteries();
                //PropulsionRequirements();
                //PropulsionCalculatePropMass();
                //PropulsionCalculateStorage();
                //PropulsionMassAndPower();
                //CommTelemetryIterate();
                //CommCommandIterate();
                //CommMassAndPower();
                //CommDHCalculate();
                //StructureStore();
                //StructureIterations();
                //StructureDesignStore();
                //ThermalCalculations();
                OverviewCalculate();

                OverviewUpdateTips->setText("Check the design windows again, there might be some data that have been updated.");
}


