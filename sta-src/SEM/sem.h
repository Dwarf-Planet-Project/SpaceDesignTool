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

#ifndef _SEM_H
#define _SEM_H

#include <QtGui/QDialog>
#include "ui_sem.h"
#include <iostream>
#include <fstream>
//#include "semvehicle.h"

class DataBaseDialog;
class ScenarioSpaceVehicle;

// The tab subsystems has not been implemented : the commented parts of the code concern
// this part
class SEM : public QDialog, private Ui_SEMDialogClass
{
    Q_OBJECT

public:
    SEM(ScenarioSpaceVehicle* vehicle);

    ~SEM();

    ScenarioSpaceVehicle* m_vehicle;

     struct PreliminaryDesing
            {
                double Mass;
                double MassPerc;
                double Power;
                double PowerPerc;
                double DV;
                double ISP;
                double MassMargin;
                double PropellantMargin;
                double PowerMargin;
                double PropellantResidual;
                double Volume;
                double Linear;
                double Area;
                double MOI;
                double moi;
                double SAarea;
                double SAoffset;
                double SAMass;
                double SAMoiAboutArrayAxis;
                double SAMoiPerpArrayAxis;
                double SAMoiPerpArrayFace;
                double PCUMass;
                double RegulatorMass;
                double RegulatorPower;
                double WiringMass;
                double WiringPower;
                double TotalDryMass;
            };

     struct Mission
            {
                double SemimajorAxis;
                int Inclination;
                double Eccentricity;
                QString PerigeeAltitude;
                QString ApogeeAltitude;
                double CircularAltitude;

                double angularRadius;
                double maxHorizon;
                double nadir;
                int elevation;
                double centralAngle;
                double slantRange;
                double swathWidth;
                double longitudeShift;

                double OrbitPeriod;
                double AngularVelocity;
                double GroundVelocity;
                double PerigeeVelocity;
                double ApogeeVelocity;
                double PerigeeEscVelocity;
                double ApogeeEscVelocity;
                double Density;
                double Apogee;
                double Perigee;
            };

    struct Payload
    {
        double spatialResolution;
        int bitPerPixel;
        double width;
        double imagingQuality;
        double wavelength;

        double IFOV;
        double alongRes;
        double crossRes;
        double crossPixels;
        double swatsPerSecond;
        double dataRate;
        double focal;
        double aperture;
        double groundMax;
        double mass;
        double power;
        double finalDataRate;

    };

       struct Maneuver
    {
        double ParkOrbitAltitude;
        int 	ParkOrbitInclination;
        double InclChange;
        double DisposalAltitude;
        double EolAltitude;
        double HohmannTime;
        double TotalTransferDV;
        double MissionDuration;
        double TotalMaintDV;
        double TotalDisposalDV;
        double TotalManeuverDV;

    };

           struct PlanetView
    {
        double AngularRadiusPerigee;
        double EclipseTime;
    };

             struct Launcher
    {
        double diameter;
        double length;
        double axialAcc;
        double lateralAcc;
        double axialFreq;
        double lateralFreq;
        double massLEO;
        double massGEO;
    };

            struct PerturbTorques
    {
        double Aerodynamic;
        double Magnetic;
        double Solar;
        double Gravity;
        double TotalDisTorque;
        double DragCoeff;
        double MomArmAtmo;
        double ResidualDipole;
        double MomArmLight;
        double Reflectivity;
        double LightIncidAngle;
        double MaxDevNadir;
    };

//    struct ADCS
//    {
//    	double Accuracy;
//    	double maxSlewAngle;
//    	double minReTargetTime;
//    	double SlewRate;
//    	double SlewTorque;
//    	double ReactionReqAngMom;
//    	double MomentumReqAngMom;
//    	double ReactionTime;
//    	double TotalWheelMass;
//    	double TotalWheelPower;
//    	double ThrustLevel;
//    	double DumpingThrustLevel;
//    	//double PropellantMass;
//    	//double Isp;
//    	//double TotalThrustersMass;
//    	double MagneticDipole;
//    	double TotalTorquerMass;
//    	double TotalTorquerPower;
//
//    };
//    struct Comm
//    {
//    	double mass;
//    	double power;
//    	double margin;
//    	double EbN0;
//    	double simple;
//    	double typical;
//    	double complex;
//    	double DHmass;
//    	double DHpower;
//    	double memoryCapacity;
//
//    };
//
//    struct Power
//    {
//    	double CellEfficiency;
//    	double InherentDegradation;
//    	double reqPower;
//    	double EnergyLossSun;
//    	double EnergyLossEclipse;
//    	double CellDegrYear;
//    	double SAarea;
//    	double SAmass;
//    	double UserMass;
//    	double SAPowerDensity;
//    	double Ncycles;
//    	double BatteryEnergyDensity;
//    	double BatteryCapacity;
//    	double DOD;
//    	double BatteriesNumber;
//    	double BatteryMass;
//    	double TotalBatteryMass;
//    };
//             struct Propulsion
//    {
//    	double OrbitDV;
//    	double OrbitThrust;
//    	double ADCSDV;
//    	double ADCSImpulse;
//    	double ExtIsp;
//    	double ExtPropMass;
//    	double OnBoardIsp;
//    	double OnBoardPropMass;
//    	double Chem2Isp;
//    	double Chem2PropMass;
//    	double Chem3Isp;
//    	double Chem3PropMass;
//    	double SolidDensity;
//    	double ColdDensity;
//    	double MonoDensity;
//    	double Oxidiser1;
//    	double Fuel1;
//    	double Oxidiser2;
//    	double Fuel2;
//    	double LiftOffTotal;
//    	double EOLTotal;
//
//    	double LiftOffHardware;
//    	double BOLHardware;
//    	double LiftOffPropellant;
//
//    	double BOLPropellant;
//    	double BOLPressurant;
//    	double LiftOffPressurant;
//
//    	double power;
//
//    };
//    struct Structure
//    {
//    	double density;
//    	double young;
//    	double ultimate;
//    	double yield;
//    	double poisson;
//    	double ultimateFactor;
//    	double yieldFactor;
//    	double stringers;
//    };
//
//    struct Thermal
//    {
//    	double assorptivity;
//    	double emissivity;
//    	double maxPower;
//    	double minPower;
//    	double mass;
//    	double power;
//
//    };

    int type;
        Ui::SEMDialogClass ui;

        void set_PerigeeAltitude(QString text);
        void set_ApogeeAltitude(QString text);
        void calculateParameters(double Radius);
        void calculateDynamics(double Radius,double MU);
        void calculateGeometry(double Radius);

public slots:

        void sum();
        void PayloadDefinition();
        void PayloadViewStore();
        void PayloadDesign();
        void PayloadSolarCombo();
        void PayloadEnvironmentalCombo();
        void PayloadResourcesCombo();
        void PayloadOther();

        int select_table(int i);
        //void selectADCS();
        //void ADCSactivate();
        //void ADCSmassAndPower();
        //void OpenDataBase();
        //void openBatteryDatabase();
        //void openSolarCellDatabase();
        //void addToBatteryDatabase();
        //void addToSolarCellDatabase();
        //void saveDataBase();

        void missionCalculations();
        void ManeuverPlan();
        void CalculateManeuvres();

        //void perturbingTorques();
        //void typeADCS();
        //void ActuatorsDesign();

//        void PowerRequirements();
//        void PowerSourcesCombo();
//        void PowerSources();
//        void PowerBatteriesCombo();
//        void PowerBatteries();
//
//        void PropulsionExternalCombo();
//        void PropulsionOrbitPropellantSelection();
//        void PropulsionADCSPropellantSelection();
//        void PropulsionRequirements();
//        void PropulsionOnBoard();
//        void PropulsionChemical2();
//        void PropulsionExternal();
//        void PropulsionChemical3();
//        void PropulsionCalculatePropMass();
//        void PropulsionCalculateStorage();
//        void PropulsionMassAndPower();
//
//        void CommSelectFreq();
//        //void CommElevationAngle();
//        void CommTelemetryIterate();
//        void CommCommandIterate();
//        void CommBeamwidthAperture(double beamwidth);
//        void CommApertureBeamwidth(double diameter);
//        void CommBeamwidthAperture_2(double beamwidth);
//        void CommApertureBeamwidth_2(double diameter);
//        void CommBeamwidthAperture_command(double beamwidth);
//        void CommApertureBeamwidth_command(double diameter);
//        void CommBeamwidthAperture_command_2(double beamwidth);
//        void CommApertureBeamwidth_command_2(double diameter);
//        void CommTransmitter();
//        void CommMassAndPower();
//
//        void CommDHProcessingComboBox();
//        void CommDHTelemetryComboBox();
//        void CommDHOtherComboBox();
//        void CommDHBusComboBox();
//        void CommDHRadiationComboBox();
//        void CommDHTypeComboBox();
//        void CommDHCalculate();

        void LauncherSelection();
        void LauncherStore();

//        void StructureMaterialSelection();
//        void StructureStore();
//        void StructureStringersSelection();
//        void StructureIterations();
//        void StructureDesignStore();
//
//        void ThermalCalculations();
//        void ThermalSelectMaterial();

        void OverviewCalculate();
        void OverviewUpdate();




private:
    PreliminaryDesing PL;
    Mission Mission;
    Maneuver Maneuver;
    Payload Payload;
    Launcher Launcher;
    PlanetView PlanetView;
    PerturbTorques PerturbTorques;
    DataBaseDialog* m_batterydialog;
    DataBaseDialog* m_solardialog;
    //PerturbTorques PerturbTorques;
    //ADCS ADCS;
    //Power Power;
    //Propulsion Propulsion;
    //Structure Structure;
    //Thermal Thermal;
    //Comm Comm;
    //bool DBmodified;


};

#endif // _SEM_H
