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
 ------------------ Author:       Guillermo Ortega               -------------------
 ------------------ Affiliation:  European Space Agency (ESA)    -------------------
 -----------------------------------------------------------------------------------
 Updated March 2011
 Update July 2011 by Guillermo

*/

#include "Scenario/staschema.h"
#include "Scenario/missionsDefaults.h"

#include "Astro-Core/constants.h"
#include "Astro-Core/date.h"
#include "Astro-Core/stacoordsys.h"
#include "Astro-Core/stabody.h"
#include "Astro-Core/stamath.h"



MissionsDefaults::~MissionsDefaults()
{

}


/////////////////////////////////////// Generic elements //////////////////////////////////////

QSharedPointer<ScenarioSCSystemType> MissionsDefaults::MissionsDefaults_GenericPlatform()
{
    QSharedPointer<ScenarioSCSystemType> genericPlatform(new ScenarioSCSystemType());

    genericPlatform->SystemBudgets()->MassOfSystem()->setDryMass(1000.0);
    genericPlatform->SystemBudgets()->MassOfSystem()->setWetMass(1500.0);
    genericPlatform->SystemBudgets()->PowerOfSystem()->setTotalPowerBoL(200.0);
    genericPlatform->SystemBudgets()->PowerOfSystem()->setTotalPowerEoL(100.0);
    genericPlatform->SystemBudgets()->Link()->setDownlinkMargin(10);
    genericPlatform->SystemBudgets()->Link()->setUplinkMargin(10);

    genericPlatform->SCAerodynamics()->setSurfaceArea(1.0);
    genericPlatform->SCAerodynamics()->setCd(0.25);

    genericPlatform->Propulsion()->setNumberOfEngines(1);
    genericPlatform->Propulsion()->setPropellantMass(500);
    genericPlatform->Propulsion()->setSpecificImpulse(312);
    genericPlatform->Propulsion()->setThrustPerEngine(100);
    genericPlatform->Propulsion()->setTotalPropulsionDryMass(25);

    genericPlatform->Structure()->setStructuralShape("Cube");			    // Cube, Cylinder or Sphere
    genericPlatform->Structure()->setStructuralMaterial("Aluminium");		    // Steel or Aluminium
    genericPlatform->Structure()->setTotalStructureMass(800);
    genericPlatform->Structure()->Sizing()->setHeight(1);
    genericPlatform->Structure()->Sizing()->setLength(1);
    genericPlatform->Structure()->Sizing()->setWidth(1);
    genericPlatform->Structure()->Sizing()->setVolume(1);
    genericPlatform->Structure()->MomentsOfInertia()->setXAxis(150);
    genericPlatform->Structure()->MomentsOfInertia()->setYAxis(150);
    genericPlatform->Structure()->MomentsOfInertia()->setZAxis(150);
    genericPlatform->Structure()->SecondMomentsOfInertia()->setXAxis(20);
    genericPlatform->Structure()->SecondMomentsOfInertia()->setYAxis(20);
    genericPlatform->Structure()->SecondMomentsOfInertia()->setZAxis(20);
    genericPlatform->Structure()->NaturalFrequency()->setAxialFrequency(50);
    genericPlatform->Structure()->NaturalFrequency()->setLateralFrequency(12);

    genericPlatform->TCS()->Temperature()->setMaximumSCTemperature(85);
    genericPlatform->TCS()->Temperature()->setMinimumSCTemperature(15);
    genericPlatform->TCS()->setHeaterPower(50);
    genericPlatform->TCS()->setRadiatedPower(12);
    genericPlatform->TCS()->setHeaterPower(25);
    genericPlatform->TCS()->setTotalTCSMass(15);
    genericPlatform->TCS()->CoatingArea()->ColdSurface()->ColdCoating()->ElementIdentifier()->setName("White Paint");
    genericPlatform->TCS()->CoatingArea()->ColdSurface()->ColdCoating()->setAbsorptivity(0.2);
    genericPlatform->TCS()->CoatingArea()->ColdSurface()->ColdCoating()->setEmissivity(0.3);
    genericPlatform->TCS()->CoatingArea()->ColdSurface()->setTotalColdArea(1);
    genericPlatform->TCS()->CoatingArea()->HotSurface()->HotCoating()->ElementIdentifier()->setName("White Paint");
    genericPlatform->TCS()->CoatingArea()->HotSurface()->HotCoating()->setAbsorptivity(0.2);
    genericPlatform->TCS()->CoatingArea()->HotSurface()->HotCoating()->setEmissivity(0.3);
    genericPlatform->TCS()->CoatingArea()->HotSurface()->setTotalHotArea(1);

    genericPlatform->EPS()->SolarArray()->setSACellType("Silicon");		    // Silicon, GaAs, Multijunction
    genericPlatform->EPS()->SolarArray()->setAreaOfSolarArray(4);
    genericPlatform->EPS()->SolarArray()->setMassOfSolarArray(30);
    genericPlatform->EPS()->BatteryType()->setBatteryType("Lithium_Ion");	    // Lithium_Ion, Nickel_Cadmium, Lead_Acid
    genericPlatform->EPS()->BatteryType()->setNumberOfBatteries(4);
    genericPlatform->EPS()->BatteryType()->setTotalMassOfBatteries(30);
    genericPlatform->EPS()->BatteryType()->setTotalVolumeOfBatteries(0.01);
    genericPlatform->EPS()->setMassOfPCU(0.1);
    genericPlatform->EPS()->setTotalEPSMass(70);

    genericPlatform->TTC()->setTotalTTCMass(70);
    genericPlatform->TTC()->TTCAntenna()->setDataRate(300);
    genericPlatform->TTC()->TTCAntenna()->setLinkDuration(10);
    genericPlatform->TTC()->TTCAntenna()->setTransmitterEfficiency(90);
    genericPlatform->TTC()->TTCAntenna()->setTransmitterFrequency(25);
    genericPlatform->TTC()->TTCAntenna()->setTransmitterGain(30);
    genericPlatform->TTC()->TTCAntenna()->setTransmitterPower(100);

    genericPlatform->AOCS()->setTotalAOCSMass(0.5);

    genericPlatform->OBDH()->setTotalOBDHMass(0.5);
    genericPlatform->OBDH()->setTotalSizeOfMemory(200);

    return genericPlatform;
}


QSharedPointer<ScenarioTransmitterPayloadType> MissionsDefaults::MissionsDefaults_GenericTransmitter()
{
    QSharedPointer<ScenarioTransmitterPayloadType> genericTransmitter(new ScenarioTransmitterPayloadType());

    genericTransmitter->ElementIdentifier()->setColorName("Yellow");
    genericTransmitter->ElementIdentifier()->setModelName("Default");
    genericTransmitter->ElementIdentifier()->setName("Transmitter #1");

    genericTransmitter->Budget()->setDataRate(100.0);
    genericTransmitter->Budget()->setFrequencyBand(13397312236.56);
    genericTransmitter->Budget()->setMass(30.0);
    genericTransmitter->Budget()->Power()->setPowerConsumptionInDaylight(1000.0);
    genericTransmitter->Budget()->Power()->setPowerConsumptionInEclipse(700.0);
    genericTransmitter->Budget()->Power()->setPowerOnPercentageInDaylight(10.0);
    genericTransmitter->Budget()->Power()->setPowerOnPercentageInEclipse(90.0);
    genericTransmitter->Budget()->Size()->setHeight(0.100);  // in meters
    genericTransmitter->Budget()->Size()->setLength(0.100);
    genericTransmitter->Budget()->Size()->setWidth(0.100);
    genericTransmitter->Budget()->TemperatureRange()->setMaximumTemperature(40.0);
    genericTransmitter->Budget()->TemperatureRange()->setMinimumTemperature(10.0);

    genericTransmitter->Transmitter()->setDepointingLossTx(0.0);
    genericTransmitter->Transmitter()->setTransmittingPower(1000.0);

    genericTransmitter->Transmitter()->Coverage()->setFrustumAngle1(15.0);
    genericTransmitter->Transmitter()->Coverage()->setFrustumAngle2(15.0);
    genericTransmitter->Transmitter()->Coverage()->setFrustumShape("Rectangular");

    genericTransmitter->Transmitter()->PointingDirection()->setAzimuth(0.0);
    genericTransmitter->Transmitter()->PointingDirection()->setAzimuthDot(0.0);
    genericTransmitter->Transmitter()->PointingDirection()->setElevation(90.0*DEG2RAD);
    genericTransmitter->Transmitter()->PointingDirection()->setElevationDot(0.0);
    genericTransmitter->Transmitter()->PointingDirection()->setReferenceSystem("LVLH");  // Not sure about this

    genericTransmitter->Transmitter()->EMproperties()->setAngularBeamWidth(15.0);
    genericTransmitter->Transmitter()->EMproperties()->setAreaEff(30.0);
    genericTransmitter->Transmitter()->EMproperties()->setBandWidth(32346653.32);
    genericTransmitter->Transmitter()->EMproperties()->setBeamType("Parabolic");
    genericTransmitter->Transmitter()->EMproperties()->setDiameter(0.280622);
    genericTransmitter->Transmitter()->EMproperties()->setEfficiency(55.0);
    genericTransmitter->Transmitter()->EMproperties()->setGainMax(30.0);
    genericTransmitter->Transmitter()->EMproperties()->setPolarisation("Linear");
    genericTransmitter->Transmitter()->EMproperties()->setTiltAngle(0.0);

    genericTransmitter->Transmitter()->setFedderLossTx(0.0);

    genericTransmitter->Transmitter()->Modulation()->setDataRate(16654486.34);
    genericTransmitter->Transmitter()->Modulation()->setModulationType("BPSK");

    genericTransmitter->Transmitter()->setObservationChecked(false);

    return genericTransmitter;
}


QSharedPointer<ScenarioReceiverPayloadType> MissionsDefaults::MissionsDefaults_GenericReceiver()
{    
    QSharedPointer<ScenarioReceiverPayloadType> receiverPayload(new ScenarioReceiverPayloadType());

    receiverPayload->ElementIdentifier()->setName("receiver");

    double ElInit=(90*DEG2RAD);
    receiverPayload->Receiver()->PointingDirection()->setElevation(ElInit);
    receiverPayload->Receiver()->PointingDirection()->setAzimuth(0);
    receiverPayload->Receiver()->EMproperties()->setEfficiency(55);
    receiverPayload->Receiver()->EMproperties()->setGainMax(30);
    receiverPayload->Receiver()->EMproperties()->setTiltAngle(0);
    receiverPayload->Receiver()->EMproperties()->setPolarisation("Linear");
    receiverPayload->Receiver()->EMproperties()->setBandWidth(32000000.0);//in Hz

    receiverPayload->Receiver()->Coverage()->setFrustumShape("Ellipse");
    receiverPayload->Receiver()->Coverage()->setFrustumAngle1(5.0);
    receiverPayload->Receiver()->Coverage()->setFrustumAngle1(5.0);

    receiverPayload->Receiver()->setDepointingLossRx(0);
    receiverPayload->Receiver()->setFeederLossRx(0);

    receiverPayload->Budget()->setFrequencyBand(14500000000.0);//It's in hertz

    receiverPayload->Receiver()->SystemTemperature()->setRxNoiseFigure(1);
    receiverPayload->Receiver()->SystemTemperature()->setThermoFeeder(290);
    receiverPayload->Receiver()->SystemTemperature()->setThermoReveicer(290);

    receiverPayload->Receiver()->SystemTemperature()->setChoiceTantenna("calculated");

    return receiverPayload;
}


QSharedPointer<ScenarioOpticalPayloadType> MissionsDefaults::MissionsDefaults_GenericOpticalPayload()
{
    QSharedPointer<ScenarioOpticalPayloadType> genericTelescope(new ScenarioOpticalPayloadType());

    genericTelescope->ElementIdentifier()->setName("telescope");
    genericTelescope->Telescope()->OpticalProperties()->setDiameter(0.10);
    genericTelescope->Telescope()->OpticalProperties()->setEfficiency(100.0);

    return genericTelescope;
}


QSharedPointer<ScenarioRadarPayloadType> MissionsDefaults::MissionsDefaults_GenericRadarPayload()
{
    QSharedPointer<ScenarioRadarPayloadType> genericRadar(new ScenarioRadarPayloadType());

    genericRadar->ElementIdentifier()->setName("radar");
    genericRadar->Radar()->RadarProperties()->setAngularBeamWidth(25.0);
    genericRadar->Radar()->RadarProperties()->setDiameter(0.10);
    genericRadar->Radar()->RadarProperties()->setEfficiency(100.0);
    genericRadar->Radar()->RadarProperties()->setGainMax(30.0);
    genericRadar->Radar()->RadarProperties()->setPolarisation("right");
    genericRadar->Radar()->RadarProperties()->setTiltAngle(0.0);

    return genericRadar;
}



//////////////////////////////////////// Initial vectors for position and attitude /////////////////////////////

QSharedPointer<ScenarioInitialPositionType> MissionsDefaults_InitialStateVectorKeplerian()
{
    QSharedPointer<ScenarioInitialPositionType> genericInitialStateVectorKeplerian(new ScenarioInitialPositionType());

    genericInitialStateVectorKeplerian->setCoordinateSystem("INERTIAL J2000");

    QSharedPointer<ScenarioKeplerianElementsType> initPosKeplerian(new ScenarioKeplerianElementsType());
    initPosKeplerian->setSemiMajorAxis(8100.0);
    initPosKeplerian->setInclination(45.0);
    initPosKeplerian->setEccentricity(0.0);
    initPosKeplerian->setRAAN(0.0);
    initPosKeplerian->setArgumentOfPeriapsis(0.0);
    initPosKeplerian->setTrueAnomaly(0.0);

    genericInitialStateVectorKeplerian->setAbstract6DOFPosition(initPosKeplerian);

    return genericInitialStateVectorKeplerian;
}

QSharedPointer<ScenarioInitialPositionType> MissionsDefaults_InitialStateVectorCartesian()
{
    QSharedPointer<ScenarioInitialPositionType> genericInitialStateVectorCartesian(new ScenarioInitialPositionType());

    genericInitialStateVectorCartesian->setCoordinateSystem("INERTIAL J2000");

    QSharedPointer<ScenarioStateVectorType> initPosStateVector(new ScenarioStateVectorType());    
    initPosStateVector->setX(8100.0);
    initPosStateVector->setY(0.0);
    initPosStateVector->setZ(0.0);
    initPosStateVector->setVx(0.0);
    initPosStateVector->setVy(6.52413171);
    initPosStateVector->setVz(2.57791256);

    genericInitialStateVectorCartesian->setAbstract6DOFPosition(initPosStateVector);

    return genericInitialStateVectorCartesian;
}

QSharedPointer<ScenarioInitialAttitudeType> MissionsDefaults_InitialAttitudeVectorEuler()
{
    QSharedPointer<ScenarioInitialAttitudeType> genericInitialAttitudeVectorEuler(new ScenarioInitialAttitudeType());

    genericInitialAttitudeVectorEuler->setCoordinateSystem("Center of Gravity");
    QSharedPointer<ScenarioEulerType> initAttEuler(new ScenarioEulerType());
    initAttEuler->setPhi(0.0);
    initAttEuler->setTheta(0.0);
    initAttEuler->setPsi(0.0);
    initAttEuler->setPhiDot(0.0);
    initAttEuler->setThetaDot(0.0);
    initAttEuler->setPsiDot(0.0);

    genericInitialAttitudeVectorEuler->setAbstract6DOFAttitude(initAttEuler);

    return genericInitialAttitudeVectorEuler;
}

QSharedPointer<ScenarioInitialAttitudeUsingQuaternionsType> MissionsDefaults_InitialAttitudeVectorQuaternion()
{
    QSharedPointer<ScenarioInitialAttitudeUsingQuaternionsType> genericInitialAttitudeVectorQuaternion(new ScenarioInitialAttitudeUsingQuaternionsType());

    genericInitialAttitudeVectorQuaternion->setCoordinateSystem("Center of Gravity");
    QSharedPointer<ScenarioQuaternionType> initAttQuaternion(new ScenarioQuaternionType());
    initAttQuaternion->setQ1(0.0);
    initAttQuaternion->setQ2(0.0);
    initAttQuaternion->setQ3(0.0);
    initAttQuaternion->setQ4(0.0);
    initAttQuaternion->setQ1Dot(0.0);
    initAttQuaternion->setQ2Dot(0.0);
    initAttQuaternion->setQ3Dot(0.0);
    initAttQuaternion->setQ4Dot(0.0);
    genericInitialAttitudeVectorQuaternion->setAbstract8DOFAttitude(initAttQuaternion);
    // The next line should be the last one to make sure the GUI starts on the right tab

    return genericInitialAttitudeVectorQuaternion;
}



//////////////////////////////////////// Generic Loitering /////////////////////////////////////////////////

ScenarioLoiteringType MissionsDefaults::MissionsDefaults_GENERIC()
{

    ScenarioLoiteringType loitering;

    loitering.InitialPosition()->setCoordinateSystem("INERTIAL J2000");
    QSharedPointer<ScenarioKeplerianElementsType> initPosKeplerian(new ScenarioKeplerianElementsType());
    //-- Figues shall be as follows:
    // Distances in Km
    // Distances rates in Km/s
    // Angles in rad
    // Angles rates in rad/s
    initPosKeplerian->setSemiMajorAxis(8100.0);
    initPosKeplerian->setInclination(sta::degToRad(45.0));
    initPosKeplerian->setEccentricity(sta::degToRad(0.0));
    initPosKeplerian->setRAAN(sta::degToRad(0.0));
    initPosKeplerian->setArgumentOfPeriapsis(sta::degToRad(0.0));
    initPosKeplerian->setTrueAnomaly(sta::degToRad(0.0));
    loitering.InitialPosition()->setAbstract6DOFPosition(initPosKeplerian);

    loitering.InitialAttitude()->setCoordinateSystem("Center of Gravity");
    QSharedPointer<ScenarioEulerType> initAttEuler(new ScenarioEulerType());
    //-- Figues shall be as follows:
    // Angles in rad
    // Angles rates in rad/s
    initAttEuler->setPhi(0.0);
    initAttEuler->setTheta(0.0);
    initAttEuler->setPsi(0.0);
    initAttEuler->setPhiDot(0.0);
    initAttEuler->setThetaDot(0.0);
    initAttEuler->setPsiDot(0.0);
    loitering.InitialAttitudeUsingQuaternions()->setCoordinateSystem("Center of Gravity");

    QSharedPointer<ScenarioQuaternionType> initAttQuaternion(new ScenarioQuaternionType());
    initAttQuaternion->setQ1(0.0);
    initAttQuaternion->setQ2(0.0);
    initAttQuaternion->setQ3(0.0);
    initAttQuaternion->setQ4(0.0);
    initAttQuaternion->setQ1Dot(0.0);
    initAttQuaternion->setQ2Dot(0.0);
    initAttQuaternion->setQ3Dot(0.0);
    initAttQuaternion->setQ4Dot(0.0);
    loitering.InitialAttitudeUsingQuaternions()->setAbstract8DOFAttitude(initAttQuaternion);
    loitering.InitialAttitude()->setAbstract6DOFAttitude(initAttEuler);

    QDateTime TheCurrentDateAndTime = QDateTime::currentDateTime(); // Get the current epoch
    loitering.TimeLine()->setStartTime(TheCurrentDateAndTime);
    loitering.TimeLine()->setEndTime(TheCurrentDateAndTime.addDays(1));
    loitering.TimeLine()->setStepTime(60.0);

    loitering.PropagationPosition()->setPropagator("TWO BODY");
    loitering.PropagationPosition()->setIntegrator("Runge-Kutta 3-4");
    loitering.PropagationPosition()->setTimeStep(60.0);

    loitering.PropagationAttitude()->setIntegrator("Runge-Kutta 3-4");
    loitering.PropagationAttitude()->setTimeStep(60.0);

    loitering.Environment()->CentralBody()->setName("Earth");
    loitering.Environment()->CentralBody()->GravityModel()->setModelName("EGM2008");
    loitering.Environment()->CentralBody()->GravityModel()->setNumberOfTesserals(0);
    loitering.Environment()->CentralBody()->GravityModel()->setNumberOfZonals(0);

    loitering.ElementIdentifier()->setName("loitering arc");
    loitering.ElementIdentifier()->setTheOrder(1);
    loitering.ElementIdentifier()->setModelName("Default");
    loitering.ElementIdentifier()->setColorName("Yellow");

    return loitering;
} /////////////////////// end of GENERIC



ScenarioLoiteringTLEType MissionsDefaults::MissionsDefaults_GENERIC_TLE()
{
    ScenarioLoiteringTLEType loiteringTLE;

    QDateTime TheCurrentDateAndTime = QDateTime::currentDateTime(); // Get the current epoch
    loiteringTLE.TimeLine()->setStartTime(TheCurrentDateAndTime);
    loiteringTLE.TimeLine()->setEndTime(TheCurrentDateAndTime.addDays(1));
    loiteringTLE.TimeLine()->setStepTime(60.0);
    loiteringTLE.setTleLine0("ISS (ZARYA)");
    loiteringTLE.setTleLine1("1 25544U 98067A   09282.91732946  .00013034  00000-0  90850-4 0  7559");
    loiteringTLE.setTleLine2("2 25544  51.6398  77.7469 0007731 180.0753 317.5762 15.74717336624025");

    loiteringTLE.ElementIdentifier()->setName("TLE arc");
    loiteringTLE.ElementIdentifier()->setColorName("Red");
    loiteringTLE.ElementIdentifier()->setModelName("ISS");
    loiteringTLE.ElementIdentifier()->setTheOrder(2);

    return loiteringTLE;
}




//////////////////////////////////////// Generic Maneuvers /////////////////////////////////////////////////
ScenarioDeltaVType MissionsDefaults::MissionsDefaults_GENERIC_DELTAV()
{
    ScenarioDeltaVType deltaV;

    QDateTime TheCurrentDateAndTime = QDateTime::currentDateTime(); // Get the current epoch
    deltaV.TimeLine()->setStartTime(TheCurrentDateAndTime);
    deltaV.TimeLine()->setEndTime(TheCurrentDateAndTime.addSecs(1));  // The deltaV duration is 1 second

    deltaV.ElementIdentifier()->setName("Delta V");
    deltaV.ElementIdentifier()->setColorName("Dark Red");
    deltaV.ElementIdentifier()->setModelName("Default");
    deltaV.ElementIdentifier()->setTheOrder(2);

    deltaV.InitialPosition()->setCoordinateSystem("INERTIAL J2000");

    deltaV.Environment()->CentralBody()->setName("Earth");
    deltaV.Environment()->CentralBody()->GravityModel()->setModelName("EGM2008");
    deltaV.Environment()->CentralBody()->GravityModel()->setNumberOfTesserals(0);
    deltaV.Environment()->CentralBody()->GravityModel()->setNumberOfZonals(0);

    deltaV.setDeltaVx(1.0);
    deltaV.setDeltaVy(0.0);
    deltaV.setDeltaVz(0.0);
    deltaV.setMagnitude(200.0);

    return deltaV;
}

//ALONSO.-
// Generic RendezVous Manoeuvres
ScenarioRendezVousManoeuvreType MissionsDefaults::MissionsDefaults_GENERIC_RENDEZVOUS()
{
    ScenarioRendezVousManoeuvreType rvManoeuvre;

//    rvManoeuvre.InitialPosition()->setCoordinateSystem("INERTIAL J2000");

//    QSharedPointer<ScenarioKeplerianElementsType> initPos(new ScenarioKeplerianElementsType());
//    initPos->setSemiMajorAxis(7650.0);
//    initPos->setInclination(45.0);
//    initPos->setEccentricity(0.0);
//    initPos->setRAAN(0.0);
//    initPos->setArgumentOfPeriapsis(0.0);
//    initPos->setTrueAnomaly(0.0);
//    rvManoeuvre.InitialPosition()->setAbstract6DOFPosition(initPos);

//    //rvManoeuvre.InitialAttitude()->setCoordinateSystem("EULER 123");
//    QSharedPointer<ScenarioEulerType> initAtt(new ScenarioEulerType());
//    initAtt->setPhi(0.00000);
//    initAtt->setTheta(0.00000);
//    initAtt->setPsi(0.00000);
//    initAtt->setPhiDot(0.00000);
//    initAtt->setThetaDot(0.00000);
//    initAtt->setPsiDot(0.00000);
//    rvManoeuvre.InitialAttitude()->setAbstract6DOFAttitude(initAtt);

//    QDateTime TheCurrentDateAndTime = QDateTime::currentDateTime(); // Get the current epoch
//    rvManoeuvre.TimeLine()->setStartTime(TheCurrentDateAndTime);
//    rvManoeuvre.TimeLine()->setEndTime(TheCurrentDateAndTime.addDays(1));
//    rvManoeuvre.TimeLine()->setStepTime(60.0);

//    rvManoeuvre.PropagationPosition()->setPropagator("TWO BODY");
//    rvManoeuvre.PropagationPosition()->setIntegrator("Runge-Kutta 3-4");
//    rvManoeuvre.PropagationPosition()->setTimeStep(60.0);

//    rvManoeuvre.PropagationAttitude()->setIntegrator("Runge-Kutta 3-4");
//    rvManoeuvre.PropagationAttitude()->setTimeStep(60.0);

//    rvManoeuvre.Environment()->CentralBody()->setName("Earth");
//    rvManoeuvre.Environment()->CentralBody()->GravityModel()->setModelName("EGM2008");
//    rvManoeuvre.Environment()->CentralBody()->GravityModel()->setNumberOfTesserals(0);
//    rvManoeuvre.Environment()->CentralBody()->GravityModel()->setNumberOfZonals(0);


//    rvManoeuvre.ElementIdentifier()->setColorName("Red");
//    rvManoeuvre.ElementIdentifier()->setModelName("Default");
//    rvManoeuvre.ElementIdentifier()->setTheOrder(1);
//    rvManoeuvre.ElementIdentifier()->setName("RVmanoeuvre");

    return rvManoeuvre;
}





//////////////////////////////////////// Science /////////////////////////////////////////////////

ScenarioLoiteringType MissionsDefaults::MissionsDefaults_XMM()
{
    ScenarioLoiteringType loitering;

    loitering.InitialPosition()->setCoordinateSystem("INERTIAL J2000");
    QSharedPointer<ScenarioKeplerianElementsType> initPos(new ScenarioKeplerianElementsType());
    initPos->setSemiMajorAxis(133930);
    initPos->setInclination(60.6);
    initPos->setEccentricity(0.6);
    initPos->setRAAN(93.6);
    initPos->setArgumentOfPeriapsis(112.6);
    initPos->setTrueAnomaly(0.0);
    loitering.InitialPosition()->setAbstract6DOFPosition(initPos);

    //loitering.InitialAttitude()->setCoordinateSystem("EULER 123");
    QSharedPointer<ScenarioEulerType> initAtt(new ScenarioEulerType());
    initAtt->setPhi(0.00000);
    initAtt->setTheta(0.00000);
    initAtt->setPsi(0.00000);
    initAtt->setPhiDot(0.00000);
    initAtt->setThetaDot(0.00000);
    initAtt->setPsiDot(0.00000);
    loitering.InitialAttitude()->setAbstract6DOFAttitude(initAtt);

    QDateTime TheCurrentDateAndTime = QDateTime::currentDateTime(); // Get the current epoch
    loitering.TimeLine()->setStartTime(TheCurrentDateAndTime);
    loitering.TimeLine()->setEndTime(TheCurrentDateAndTime.addDays(8));
    loitering.TimeLine()->setStepTime(60.0);

    loitering.PropagationPosition()->setPropagator("TWO BODY");
    loitering.PropagationPosition()->setIntegrator("Runge-Kutta 3-4");
    loitering.PropagationPosition()->setTimeStep(60.0);

    loitering.PropagationAttitude()->setIntegrator("Runge-Kutta 3-4");
    loitering.PropagationAttitude()->setTimeStep(60.0);

    loitering.Environment()->CentralBody()->setName("Earth");
    loitering.Environment()->CentralBody()->GravityModel()->setModelName("EGM2008");
    loitering.Environment()->CentralBody()->GravityModel()->setNumberOfTesserals(1);
    loitering.Environment()->CentralBody()->GravityModel()->setNumberOfZonals(1);

    loitering.ElementIdentifier()->setName("XMM loitering");
    loitering.ElementIdentifier()->setTheOrder(1);
    loitering.ElementIdentifier()->setModelName("XMM");
    loitering.ElementIdentifier()->setColorName("Yellow");

    return loitering;
} /////////////////////// end of XMM




ScenarioLoiteringType MissionsDefaults::MissionsDefaults_INTEGRAL()
{
    ScenarioLoiteringType loitering;

    loitering.InitialPosition()->setCoordinateSystem("INERTIAL J2000");
    QSharedPointer<ScenarioKeplerianElementsType> initPos(new ScenarioKeplerianElementsType());
    initPos->setSemiMajorAxis(87707.90078484899);
    initPos->setInclination(53.05319831508738);
    initPos->setEccentricity(0.8234577927435308);
    initPos->setRAAN(101.4890638402147);
    initPos->setArgumentOfPeriapsis(302.4416289547550);
    initPos->setTrueAnomaly(85.35622442451043);
    loitering.InitialPosition()->setAbstract6DOFPosition(initPos);

    //loitering.InitialAttitude()->setCoordinateSystem("EULER 123");
    QSharedPointer<ScenarioEulerType> initAtt(new ScenarioEulerType());
    initAtt->setPhi(0.00000);
    initAtt->setTheta(0.00000);
    initAtt->setPsi(0.00000);
    initAtt->setPhiDot(0.00000);
    initAtt->setThetaDot(0.00000);
    initAtt->setPsiDot(0.00000);
    loitering.InitialAttitude()->setAbstract6DOFAttitude(initAtt);

    QDateTime TheCurrentDateAndTime = QDateTime::currentDateTime(); // Get the current epoch
    loitering.TimeLine()->setStartTime(TheCurrentDateAndTime);
    loitering.TimeLine()->setEndTime(TheCurrentDateAndTime.addDays(8));
    loitering.TimeLine()->setStepTime(60.0);

    loitering.PropagationPosition()->setPropagator("TWO BODY");
    loitering.PropagationPosition()->setIntegrator("Runge-Kutta 3-4");
    loitering.PropagationPosition()->setTimeStep(60.0);

    loitering.PropagationAttitude()->setIntegrator("Runge-Kutta 3-4");
    loitering.PropagationAttitude()->setTimeStep(60.0);

    loitering.Environment()->CentralBody()->setName("Earth");
    loitering.Environment()->CentralBody()->GravityModel()->setModelName("EGM2008");
    loitering.Environment()->CentralBody()->GravityModel()->setNumberOfTesserals(1);
    loitering.Environment()->CentralBody()->GravityModel()->setNumberOfZonals(1);

    loitering.ElementIdentifier()->setName("INTEGRAL loitering");
    loitering.ElementIdentifier()->setTheOrder(1);
    loitering.ElementIdentifier()->setModelName("Default");
    loitering.ElementIdentifier()->setColorName("Cyan");

    return loitering;
}



ScenarioLoiteringType MissionsDefaults::MissionsDefaults_MEX()
{
    ScenarioLoiteringType loitering;

    loitering.InitialPosition()->setCoordinateSystem("INERTIAL J2000");
    QSharedPointer<ScenarioKeplerianElementsType> initPos(new ScenarioKeplerianElementsType());
    initPos->setSemiMajorAxis(9354.09);
    initPos->setInclination(86.583);
    initPos->setEccentricity(0.606960);
    initPos->setRAAN(228.774);
    initPos->setArgumentOfPeriapsis(357.981);
    initPos->setTrueAnomaly(0.001);
    loitering.InitialPosition()->setAbstract6DOFPosition(initPos);

    //loitering.InitialAttitude()->setCoordinateSystem("EULER 123");
    QSharedPointer<ScenarioEulerType> initAtt(new ScenarioEulerType());
    initAtt->setPhi(0.00000);
    initAtt->setTheta(0.00000);
    initAtt->setPsi(0.00000);
    initAtt->setPhiDot(0.00000);
    initAtt->setThetaDot(0.00000);
    initAtt->setPsiDot(0.00000);
    loitering.InitialAttitude()->setAbstract6DOFAttitude(initAtt);

    QDateTime TheCurrentDateAndTime = QDateTime::currentDateTime(); // Get the current epoch
    loitering.TimeLine()->setStartTime(TheCurrentDateAndTime);
    loitering.TimeLine()->setEndTime(TheCurrentDateAndTime.addDays(1));
    loitering.TimeLine()->setStepTime(60.0);

    loitering.PropagationPosition()->setPropagator("TWO BODY");
    loitering.PropagationPosition()->setIntegrator("Runge-Kutta 3-4");
    loitering.PropagationPosition()->setTimeStep(60.0);

    loitering.PropagationAttitude()->setIntegrator("Runge-Kutta 3-4");
    loitering.PropagationAttitude()->setTimeStep(60.0);

    loitering.Environment()->CentralBody()->setName("Mars");
    loitering.Environment()->CentralBody()->GravityModel()->setModelName("EGM2008");
    loitering.Environment()->CentralBody()->GravityModel()->setNumberOfTesserals(1);
    loitering.Environment()->CentralBody()->GravityModel()->setNumberOfZonals(1);

    loitering.ElementIdentifier()->setName("MEX loitering");
    loitering.ElementIdentifier()->setTheOrder(1);
    loitering.ElementIdentifier()->setModelName("Default");
    loitering.ElementIdentifier()->setColorName("Red");

    return loitering;
}  ///////////////// End of MEX



ScenarioLoiteringType MissionsDefaults::MissionsDefaults_Rosetta()
{
}

ScenarioLoiteringType MissionsDefaults::MissionsDefaults_VEX()
{
    ScenarioLoiteringType loitering;

    loitering.InitialPosition()->setCoordinateSystem("INERTIAL J2000");
    QSharedPointer<ScenarioKeplerianElementsType> initPos(new ScenarioKeplerianElementsType());
    initPos->setSemiMajorAxis(39468.195);
    initPos->setInclination(89.99);
    initPos->setEccentricity(0.8403);
    initPos->setRAAN(101.4890);
    initPos->setArgumentOfPeriapsis(302.441);
    initPos->setTrueAnomaly(85.356);
    loitering.InitialPosition()->setAbstract6DOFPosition(initPos);

    //loitering.InitialAttitude()->setCoordinateSystem("EULER 123");
    QSharedPointer<ScenarioEulerType> initAtt(new ScenarioEulerType());
    initAtt->setPhi(0.00000);
    initAtt->setTheta(0.00000);
    initAtt->setPsi(0.00000);
    initAtt->setPhiDot(0.00000);
    initAtt->setThetaDot(0.00000);
    initAtt->setPsiDot(0.00000);
    loitering.InitialAttitude()->setAbstract6DOFAttitude(initAtt);

    QDateTime TheCurrentDateAndTime = QDateTime::currentDateTime(); // Get the current epoch
    loitering.TimeLine()->setStartTime(TheCurrentDateAndTime);
    loitering.TimeLine()->setEndTime(TheCurrentDateAndTime.addDays(8));
    loitering.TimeLine()->setStepTime(60.0);

    loitering.PropagationPosition()->setPropagator("TWO BODY");
    loitering.PropagationPosition()->setIntegrator("Runge-Kutta 3-4");
    loitering.PropagationPosition()->setTimeStep(60.0);

    loitering.PropagationAttitude()->setIntegrator("Runge-Kutta 3-4");
    loitering.PropagationAttitude()->setTimeStep(60.0);

    loitering.Environment()->CentralBody()->setName("Venus");
    loitering.Environment()->CentralBody()->GravityModel()->setModelName("EGM2008");
    loitering.Environment()->CentralBody()->GravityModel()->setNumberOfTesserals(1);
    loitering.Environment()->CentralBody()->GravityModel()->setNumberOfZonals(1);

    loitering.ElementIdentifier()->setName("VenusXpress loitering");
    loitering.ElementIdentifier()->setTheOrder(1);
    loitering.ElementIdentifier()->setModelName("Default");
    loitering.ElementIdentifier()->setColorName("White");

    return loitering;
}

ScenarioLoiteringType MissionsDefaults::MissionsDefaults_Herschel()
{
}

ScenarioLoiteringType MissionsDefaults::MissionsDefaults_Planck()
{
}

ScenarioLoiteringType MissionsDefaults::MissionsDefaults_LISA()
{
}

ScenarioLoiteringType MissionsDefaults::MissionsDefaults_BepiColombo()
{
}

ScenarioLoiteringType MissionsDefaults::MissionsDefaults_ExoMars()
{
}


//////////////////////////////////////// Observation of the Earth /////////////////////////////
ScenarioLoiteringType MissionsDefaults::MissionsDefaults_Meteosat6()
{
    ScenarioLoiteringType loitering;

    loitering.InitialPosition()->setCoordinateSystem("INERTIAL J2000");
    QSharedPointer<ScenarioKeplerianElementsType> initPos(new ScenarioKeplerianElementsType());
    initPos->setSemiMajorAxis(42165.45042);
    initPos->setInclination(9.78825);
    initPos->setEccentricity(0.00018184);
    initPos->setRAAN(45.2816);
    initPos->setArgumentOfPeriapsis(316.10267);
    initPos->setTrueAnomaly(237.05669);
    loitering.InitialPosition()->setAbstract6DOFPosition(initPos);

    //loitering.InitialAttitude()->setCoordinateSystem("EULER 123");
    QSharedPointer<ScenarioEulerType> initAtt(new ScenarioEulerType());
    initAtt->setPhi(0.00000);
    initAtt->setTheta(0.00000);
    initAtt->setPsi(0.00000);
    initAtt->setPhiDot(0.00000);
    initAtt->setThetaDot(0.00000);
    initAtt->setPsiDot(0.00000);
    loitering.InitialAttitude()->setAbstract6DOFAttitude(initAtt);

    QDateTime TheCurrentDateAndTime = QDateTime::currentDateTime(); // Get the current epoch
    loitering.TimeLine()->setStartTime(TheCurrentDateAndTime);
    loitering.TimeLine()->setEndTime(TheCurrentDateAndTime.addDays(8));
    loitering.TimeLine()->setStepTime(60.0);

    loitering.PropagationPosition()->setPropagator("TWO BODY");
    loitering.PropagationPosition()->setIntegrator("Runge-Kutta 3-4");
    loitering.PropagationPosition()->setTimeStep(60.0);

    loitering.PropagationAttitude()->setIntegrator("Runge-Kutta 3-4");
    loitering.PropagationAttitude()->setTimeStep(60.0);

    loitering.Environment()->CentralBody()->setName("Earth");
    loitering.Environment()->CentralBody()->GravityModel()->setModelName("EGM2008");
    loitering.Environment()->CentralBody()->GravityModel()->setNumberOfTesserals(1);
    loitering.Environment()->CentralBody()->GravityModel()->setNumberOfZonals(1);

    loitering.ElementIdentifier()->setName("Meteosat-6 loitering");
    loitering.ElementIdentifier()->setTheOrder(1);
    loitering.ElementIdentifier()->setModelName("Default");
    loitering.ElementIdentifier()->setColorName("Yellow");

    return loitering;
}


ScenarioLoiteringType MissionsDefaults::MissionsDefaults_Meteosat7()
{
    ScenarioLoiteringType loitering;

    loitering.InitialPosition()->setCoordinateSystem("INERTIAL J2000");
    QSharedPointer<ScenarioKeplerianElementsType> initPos(new ScenarioKeplerianElementsType());
    initPos->setSemiMajorAxis(42166.95039);
    initPos->setInclination(6.69800);
    initPos->setEccentricity(0.00010777);
    initPos->setRAAN(53.9191);
    initPos->setArgumentOfPeriapsis(10.30276);
    initPos->setTrueAnomaly(163.48669);
    loitering.InitialPosition()->setAbstract6DOFPosition(initPos);

    //loitering.InitialAttitude()->setCoordinateSystem("EULER 123");
    QSharedPointer<ScenarioEulerType> initAtt(new ScenarioEulerType());
    initAtt->setPhi(0.00000);
    initAtt->setTheta(0.00000);
    initAtt->setPsi(0.00000);
    initAtt->setPhiDot(0.00000);
    initAtt->setThetaDot(0.00000);
    initAtt->setPsiDot(0.00000);
    loitering.InitialAttitude()->setAbstract6DOFAttitude(initAtt);

    QDateTime TheCurrentDateAndTime = QDateTime::currentDateTime(); // Get the current epoch
    loitering.TimeLine()->setStartTime(TheCurrentDateAndTime);
    loitering.TimeLine()->setEndTime(TheCurrentDateAndTime.addDays(8));
    loitering.TimeLine()->setStepTime(60.0);

    loitering.PropagationPosition()->setPropagator("TWO BODY");
    loitering.PropagationPosition()->setIntegrator("Runge-Kutta 3-4");
    loitering.PropagationPosition()->setTimeStep(60.0);

    loitering.PropagationAttitude()->setIntegrator("Runge-Kutta 3-4");
    loitering.PropagationAttitude()->setTimeStep(60.0);

    loitering.Environment()->CentralBody()->setName("Earth");
    loitering.Environment()->CentralBody()->GravityModel()->setModelName("EGM2008");
    loitering.Environment()->CentralBody()->GravityModel()->setNumberOfTesserals(1);
    loitering.Environment()->CentralBody()->GravityModel()->setNumberOfZonals(1);

    loitering.ElementIdentifier()->setName("Meteosat-7 loitering");
    loitering.ElementIdentifier()->setTheOrder(1);
    loitering.ElementIdentifier()->setModelName("Default");
    loitering.ElementIdentifier()->setColorName("Yellow");

    return loitering;
}

ScenarioLoiteringType MissionsDefaults::MissionsDefaults_Meteosat8()
{
    ScenarioLoiteringType loitering;

    loitering.InitialPosition()->setCoordinateSystem("INERTIAL J2000");
    QSharedPointer<ScenarioKeplerianElementsType> initPos(new ScenarioKeplerianElementsType());
    initPos->setSemiMajorAxis(42168.7623489);
    initPos->setInclination(0.44495320676);
    initPos->setEccentricity(0.00004342);
    initPos->setRAAN(19.7636);
    initPos->setArgumentOfPeriapsis(116.223918608);
    initPos->setTrueAnomaly(41.8352738537);
    loitering.InitialPosition()->setAbstract6DOFPosition(initPos);

    //loitering.InitialAttitude()->setCoordinateSystem("EULER 123");
    QSharedPointer<ScenarioEulerType> initAtt(new ScenarioEulerType());
    initAtt->setPhi(0.00000);
    initAtt->setTheta(0.00000);
    initAtt->setPsi(0.00000);
    initAtt->setPhiDot(0.00000);
    initAtt->setThetaDot(0.00000);
    initAtt->setPsiDot(0.00000);
    loitering.InitialAttitude()->setAbstract6DOFAttitude(initAtt);

    QDateTime TheCurrentDateAndTime = QDateTime::currentDateTime(); // Get the current epoch
    loitering.TimeLine()->setStartTime(TheCurrentDateAndTime);
    loitering.TimeLine()->setEndTime(TheCurrentDateAndTime.addDays(8));
    loitering.TimeLine()->setStepTime(60.0);

    loitering.PropagationPosition()->setPropagator("TWO BODY");
    loitering.PropagationPosition()->setIntegrator("Runge-Kutta 3-4");
    loitering.PropagationPosition()->setTimeStep(60.0);

    loitering.PropagationAttitude()->setIntegrator("Runge-Kutta 3-4");
    loitering.PropagationAttitude()->setTimeStep(60.0);

    loitering.Environment()->CentralBody()->setName("Earth");
    loitering.Environment()->CentralBody()->GravityModel()->setModelName("EGM2008");
    loitering.Environment()->CentralBody()->GravityModel()->setNumberOfTesserals(1);
    loitering.Environment()->CentralBody()->GravityModel()->setNumberOfZonals(1);

    loitering.ElementIdentifier()->setName("Meteosat-8 loitering");
    loitering.ElementIdentifier()->setTheOrder(1);
    loitering.ElementIdentifier()->setModelName("Default");
    loitering.ElementIdentifier()->setColorName("Yellow");

    return loitering;
}

ScenarioLoiteringType MissionsDefaults::MissionsDefaults_Meteosat9()
{
    ScenarioLoiteringType loitering;

    loitering.InitialPosition()->setCoordinateSystem("INERTIAL J2000");
    QSharedPointer<ScenarioKeplerianElementsType> initPos(new ScenarioKeplerianElementsType());
    initPos->setSemiMajorAxis(42169.4843978);
    initPos->setInclination(0.26696179904);
    initPos->setEccentricity(0.00015129);
    initPos->setRAAN(9.0532);
    initPos->setArgumentOfPeriapsis(209.939388136);
    initPos->setTrueAnomaly(309.256769134);
    loitering.InitialPosition()->setAbstract6DOFPosition(initPos);

    //loitering.InitialAttitude()->setCoordinateSystem("EULER 123");
    QSharedPointer<ScenarioEulerType> initAtt(new ScenarioEulerType());
    initAtt->setPhi(0.00000);
    initAtt->setTheta(0.00000);
    initAtt->setPsi(0.00000);
    initAtt->setPhiDot(0.00000);
    initAtt->setThetaDot(0.00000);
    initAtt->setPsiDot(0.00000);
    loitering.InitialAttitude()->setAbstract6DOFAttitude(initAtt);

    QDateTime TheCurrentDateAndTime = QDateTime::currentDateTime(); // Get the current epoch
    loitering.TimeLine()->setStartTime(TheCurrentDateAndTime);
    loitering.TimeLine()->setEndTime(TheCurrentDateAndTime.addDays(8));
    loitering.TimeLine()->setStepTime(60.0);

    loitering.PropagationPosition()->setPropagator("TWO BODY");
    loitering.PropagationPosition()->setIntegrator("Runge-Kutta 3-4");
    loitering.PropagationPosition()->setTimeStep(60.0);

    loitering.PropagationAttitude()->setIntegrator("Runge-Kutta 3-4");
    loitering.PropagationAttitude()->setTimeStep(60.0);

    loitering.Environment()->CentralBody()->setName("Earth");
    loitering.Environment()->CentralBody()->GravityModel()->setModelName("EGM2008");
    loitering.Environment()->CentralBody()->GravityModel()->setNumberOfTesserals(1);
    loitering.Environment()->CentralBody()->GravityModel()->setNumberOfZonals(1);

    loitering.ElementIdentifier()->setName("Meteosat-9 loitering");
    loitering.ElementIdentifier()->setTheOrder(1);
    loitering.ElementIdentifier()->setModelName("Default");
    loitering.ElementIdentifier()->setColorName("Yellow");

    return loitering;
}

ScenarioLoiteringType MissionsDefaults::MissionsDefaults_ERS2()
{
    ScenarioLoiteringType loitering;

    loitering.InitialPosition()->setCoordinateSystem("INERTIAL J2000");
    QSharedPointer<ScenarioKeplerianElementsType> initPos(new ScenarioKeplerianElementsType());
    initPos->setSemiMajorAxis(7159.5);
    initPos->setInclination(98.543);
    initPos->setEccentricity(0.0001042);
    initPos->setRAAN(0.0);
    initPos->setArgumentOfPeriapsis(92.4836);
    initPos->setTrueAnomaly(0.0);
    loitering.InitialPosition()->setAbstract6DOFPosition(initPos);

    //loitering.InitialAttitude()->setCoordinateSystem("EULER 123");
    QSharedPointer<ScenarioEulerType> initAtt(new ScenarioEulerType());
    initAtt->setPhi(0.00000);
    initAtt->setTheta(0.00000);
    initAtt->setPsi(0.00000);
    initAtt->setPhiDot(0.00000);
    initAtt->setThetaDot(0.00000);
    initAtt->setPsiDot(0.00000);
    loitering.InitialAttitude()->setAbstract6DOFAttitude(initAtt);

    QDateTime TheCurrentDateAndTime = QDateTime::currentDateTime(); // Get the current epoch
    loitering.TimeLine()->setStartTime(TheCurrentDateAndTime);
    loitering.TimeLine()->setEndTime(TheCurrentDateAndTime.addDays(8));
    loitering.TimeLine()->setStepTime(60.0);

    loitering.PropagationPosition()->setPropagator("TWO BODY");
    loitering.PropagationPosition()->setIntegrator("Runge-Kutta 3-4");
    loitering.PropagationPosition()->setTimeStep(60.0);

    loitering.PropagationAttitude()->setIntegrator("Runge-Kutta 3-4");
    loitering.PropagationAttitude()->setTimeStep(60.0);

    loitering.Environment()->CentralBody()->setName("Earth");
    loitering.Environment()->CentralBody()->GravityModel()->setModelName("EGM2008");
    loitering.Environment()->CentralBody()->GravityModel()->setNumberOfTesserals(1);
    loitering.Environment()->CentralBody()->GravityModel()->setNumberOfZonals(1);

    loitering.ElementIdentifier()->setName("ERS-2 loitering");
    loitering.ElementIdentifier()->setTheOrder(1);
    loitering.ElementIdentifier()->setModelName("Default");
    loitering.ElementIdentifier()->setColorName("Yellow");

    return loitering;
}

ScenarioLoiteringType MissionsDefaults::MissionsDefaults_ENVISAT()
{
    ScenarioLoiteringType loitering;

    loitering.InitialPosition()->setCoordinateSystem("INERTIAL J2000");
    QSharedPointer<ScenarioKeplerianElementsType> initPos(new ScenarioKeplerianElementsType());
    initPos->setSemiMajorAxis(7159.4927);
    initPos->setInclination(98.549387);
    initPos->setEccentricity(0.001165);
    initPos->setRAAN(0.0);
    initPos->setArgumentOfPeriapsis(90.0);
    initPos->setTrueAnomaly(0.0);
    loitering.InitialPosition()->setAbstract6DOFPosition(initPos);

    //loitering.InitialAttitude()->setCoordinateSystem("EULER 123");
    QSharedPointer<ScenarioEulerType> initAtt(new ScenarioEulerType());
    initAtt->setPhi(0.00000);
    initAtt->setTheta(0.00000);
    initAtt->setPsi(0.00000);
    initAtt->setPhiDot(0.00000);
    initAtt->setThetaDot(0.00000);
    initAtt->setPsiDot(0.00000);
    loitering.InitialAttitude()->setAbstract6DOFAttitude(initAtt);

    QDateTime TheCurrentDateAndTime = QDateTime::currentDateTime(); // Get the current epoch
    loitering.TimeLine()->setStartTime(TheCurrentDateAndTime);
    loitering.TimeLine()->setEndTime(TheCurrentDateAndTime.addDays(8));
    loitering.TimeLine()->setStepTime(60.0);

    loitering.PropagationPosition()->setPropagator("TWO BODY");
    loitering.PropagationPosition()->setIntegrator("Runge-Kutta 3-4");
    loitering.PropagationPosition()->setTimeStep(60.0);

    loitering.PropagationAttitude()->setIntegrator("Runge-Kutta 3-4");
    loitering.PropagationAttitude()->setTimeStep(60.0);

    loitering.Environment()->CentralBody()->setName("Earth");
    loitering.Environment()->CentralBody()->GravityModel()->setModelName("EGM2008");
    loitering.Environment()->CentralBody()->GravityModel()->setNumberOfTesserals(1);
    loitering.Environment()->CentralBody()->GravityModel()->setNumberOfZonals(1);

    loitering.ElementIdentifier()->setName("ENVISAT loitering");
    loitering.ElementIdentifier()->setTheOrder(1);
    loitering.ElementIdentifier()->setModelName("Default");
    loitering.ElementIdentifier()->setColorName("Yellow");

    return loitering;
}

ScenarioLoiteringType MissionsDefaults::MissionsDefaults_MSG()
{
}

ScenarioLoiteringType MissionsDefaults::MissionsDefaults_MTG()
{
}

ScenarioLoiteringType MissionsDefaults::MissionsDefaults_METOP()
{
}

ScenarioLoiteringType MissionsDefaults::MissionsDefaults_Cryosat()
{
    ScenarioLoiteringType loitering;

    loitering.InitialPosition()->setCoordinateSystem("INERTIAL J2000");
    QSharedPointer<ScenarioKeplerianElementsType> initPos(new ScenarioKeplerianElementsType());
    initPos->setSemiMajorAxis(7095.348557673);
    initPos->setInclination(92.000678420);
    initPos->setEccentricity(0.001406846);
    initPos->setRAAN(129.997076727);
    initPos->setArgumentOfPeriapsis(115.619512345);
    initPos->setTrueAnomaly(283.899507188);
    loitering.InitialPosition()->setAbstract6DOFPosition(initPos);

    QDateTime TheCurrentDateAndTime = QDateTime::currentDateTime(); // Get the current epoch
    loitering.TimeLine()->setStartTime(TheCurrentDateAndTime);
    loitering.TimeLine()->setEndTime(TheCurrentDateAndTime.addDays(8));
    loitering.TimeLine()->setStepTime(60.0);

    loitering.PropagationPosition()->setPropagator("TWO BODY");
    loitering.PropagationPosition()->setIntegrator("Runge-Kutta 3-4");
    loitering.PropagationPosition()->setTimeStep(60.0);

    loitering.PropagationAttitude()->setIntegrator("Runge-Kutta 3-4");
    loitering.PropagationAttitude()->setTimeStep(60.0);

    loitering.Environment()->CentralBody()->setName("Earth");
    loitering.Environment()->CentralBody()->GravityModel()->setModelName("EGM2008");
    loitering.Environment()->CentralBody()->GravityModel()->setNumberOfTesserals(1);
    loitering.Environment()->CentralBody()->GravityModel()->setNumberOfZonals(1);

    loitering.ElementIdentifier()->setName("CryoSAT loitering");
    loitering.ElementIdentifier()->setTheOrder(1);
    loitering.ElementIdentifier()->setModelName("Default");
    loitering.ElementIdentifier()->setColorName("Yellow");

    return loitering;
}

ScenarioLoiteringType MissionsDefaults::MissionsDefaults_GOCE()
{
}

ScenarioLoiteringType MissionsDefaults::MissionsDefaults_SMOS()
{
}



ScenarioLoiteringType MissionsDefaults::MissionsDefaults_Aeolus()
{
    ScenarioLoiteringType loitering;

    loitering.InitialPosition()->setCoordinateSystem("INERTIAL J2000");
    QSharedPointer<ScenarioKeplerianElementsType> initPos(new ScenarioKeplerianElementsType());
    initPos->setSemiMajorAxis(6786.13);
    initPos->setInclination(97.0638);
    initPos->setEccentricity(0.0006);
    initPos->setRAAN(100.651);
    initPos->setArgumentOfPeriapsis(0.0);
    initPos->setTrueAnomaly(0.0);
    loitering.InitialPosition()->setAbstract6DOFPosition(initPos);

    QDateTime TheCurrentDateAndTime = QDateTime::currentDateTime(); // Get the current epoch
    loitering.TimeLine()->setStartTime(TheCurrentDateAndTime);
    loitering.TimeLine()->setEndTime(TheCurrentDateAndTime.addDays(1));
    loitering.TimeLine()->setStepTime(60.0);

    loitering.PropagationPosition()->setPropagator("TWO BODY");
    loitering.PropagationPosition()->setIntegrator("Runge-Kutta 3-4");
    loitering.PropagationPosition()->setTimeStep(60.0);

    loitering.PropagationAttitude()->setIntegrator("Runge-Kutta 3-4");
    loitering.PropagationAttitude()->setTimeStep(60.0);

    loitering.Environment()->CentralBody()->setName("Earth");
    loitering.Environment()->CentralBody()->GravityModel()->setModelName("EGM2008");
    loitering.Environment()->CentralBody()->GravityModel()->setNumberOfTesserals(1);
    loitering.Environment()->CentralBody()->GravityModel()->setNumberOfZonals(1);

    loitering.ElementIdentifier()->setName("Aeolus loitering");
    loitering.ElementIdentifier()->setTheOrder(1);
    loitering.ElementIdentifier()->setModelName("Default");
    loitering.ElementIdentifier()->setColorName("Green");

    return loitering;
} /////////// End of Aeolus



ScenarioLoiteringType MissionsDefaults::MissionsDefaults_EarthCare()
{

}

ScenarioLoiteringType MissionsDefaults::MissionsDefaults_Sentinel1A()
{
    ScenarioLoiteringType loitering;

    loitering.InitialPosition()->setCoordinateSystem("INERTIAL J2000");
    QSharedPointer<ScenarioKeplerianElementsType> initPos(new ScenarioKeplerianElementsType());
    initPos->setSemiMajorAxis(7064.23);
    initPos->setInclination(98.1638);
    initPos->setEccentricity(0.0006);
    initPos->setRAAN(4.651);
    initPos->setArgumentOfPeriapsis(0.0324);
    initPos->setTrueAnomaly(2.56);
    loitering.InitialPosition()->setAbstract6DOFPosition(initPos);

    //loitering.InitialAttitude()->setCoordinateSystem("EULER 123");
    QSharedPointer<ScenarioEulerType> initAtt(new ScenarioEulerType());
    initAtt->setPhi(0.00000);
    initAtt->setTheta(0.00000);
    initAtt->setPsi(0.00000);
    initAtt->setPhiDot(0.00000);
    initAtt->setThetaDot(0.00000);
    initAtt->setPsiDot(0.00000);
    loitering.InitialAttitude()->setAbstract6DOFAttitude(initAtt);

    QDateTime TheCurrentDateAndTime = QDateTime::currentDateTime(); // Get the current epoch
    loitering.TimeLine()->setStartTime(TheCurrentDateAndTime);
    loitering.TimeLine()->setEndTime(TheCurrentDateAndTime.addDays(1));
    loitering.TimeLine()->setStepTime(60.0);

    loitering.PropagationPosition()->setPropagator("TWO BODY");
    loitering.PropagationPosition()->setIntegrator("Runge-Kutta 3-4");
    loitering.PropagationPosition()->setTimeStep(60.0);

    loitering.PropagationAttitude()->setIntegrator("Runge-Kutta 3-4");
    loitering.PropagationAttitude()->setTimeStep(60.0);

    loitering.Environment()->CentralBody()->setName("Earth");
    loitering.Environment()->CentralBody()->GravityModel()->setModelName("EGM2008");
    loitering.Environment()->CentralBody()->GravityModel()->setNumberOfTesserals(1);
    loitering.Environment()->CentralBody()->GravityModel()->setNumberOfZonals(1);

    loitering.ElementIdentifier()->setName("Sentinel-1A loitering");
    loitering.ElementIdentifier()->setTheOrder(1);
    loitering.ElementIdentifier()->setModelName("Default");
    loitering.ElementIdentifier()->setColorName("Green");

    return loitering;
}

ScenarioLoiteringType MissionsDefaults::MissionsDefaults_Sentinel1B()
{
}

ScenarioLoiteringType MissionsDefaults::MissionsDefaults_Sentinel2()
{
}

ScenarioLoiteringType MissionsDefaults::MissionsDefaults_Sentinel3()
{
}

ScenarioLoiteringType MissionsDefaults::MissionsDefaults_Sentinel5()
{
}


////////////////////////////////////// Tecommunications //////////////////////////////
ScenarioLoiteringType MissionsDefaults::MissionsDefaults_Artemis()
{
    ScenarioLoiteringType loitering;

    loitering.InitialPosition()->setCoordinateSystem("INERTIAL J2000");
    QSharedPointer<ScenarioKeplerianElementsType> initPos(new ScenarioKeplerianElementsType());
    initPos->setSemiMajorAxis(42164.1);
    initPos->setInclination(8.0);
    initPos->setEccentricity(0.0);
    initPos->setRAAN(21.5);
    initPos->setArgumentOfPeriapsis(0.0);
    initPos->setTrueAnomaly(0.0);
    loitering.InitialPosition()->setAbstract6DOFPosition(initPos);

    //loitering.InitialAttitude()->setCoordinateSystem("EULER 123");
    QSharedPointer<ScenarioEulerType> initAtt(new ScenarioEulerType());
    initAtt->setPhi(0.00000);
    initAtt->setTheta(0.00000);
    initAtt->setPsi(0.00000);
    initAtt->setPhiDot(0.00000);
    initAtt->setThetaDot(0.00000);
    initAtt->setPsiDot(0.00000);
    loitering.InitialAttitude()->setAbstract6DOFAttitude(initAtt);

    QDateTime TheCurrentDateAndTime = QDateTime::currentDateTime(); // Get the current epoch
    loitering.TimeLine()->setStartTime(TheCurrentDateAndTime);
    loitering.TimeLine()->setEndTime(TheCurrentDateAndTime.addDays(1));
    loitering.TimeLine()->setStepTime(60.0);

    loitering.PropagationPosition()->setPropagator("TWO BODY");
    loitering.PropagationPosition()->setIntegrator("Runge-Kutta 3-4");
    loitering.PropagationPosition()->setTimeStep(60.0);

    loitering.PropagationAttitude()->setIntegrator("Runge-Kutta 3-4");
    loitering.PropagationAttitude()->setTimeStep(60.0);

    loitering.Environment()->CentralBody()->setName("Earth");
    loitering.Environment()->CentralBody()->GravityModel()->setModelName("EGM2008");
    loitering.Environment()->CentralBody()->GravityModel()->setNumberOfTesserals(1);
    loitering.Environment()->CentralBody()->GravityModel()->setNumberOfZonals(1);

    /*
    // Perturbations not required
    loitering.Environment()->PerturbationsToCentralBody()->setAtmosphereDrag(false);
    loitering.Environment()->PerturbationsToCentralBody()->setAlbedo(false);
    loitering.Environment()->PerturbationsToCentralBody()->setSolarPressure(false);
    loitering.Environment()->PerturbationsToCentralBody()->setCr(1.0);
    loitering.Environment()->PerturbationsToCentralBody()->setIR(false);
    loitering.Environment()->PerturbationsToCentralBody()->setMicrometeoroids(false);
    loitering.Environment()->PerturbationsToCentralBody()->setThirdBody(false);
    loitering.Environment()->PerturbationsToCentralBody()->setUserDefined(false);
    loitering.Environment()->PerturbationsToCentralBody()->setThirdBody(false);
    */

    loitering.ElementIdentifier()->setName("Artemis loitering");
    loitering.ElementIdentifier()->setTheOrder(1);
    loitering.ElementIdentifier()->setModelName("Default");
    loitering.ElementIdentifier()->setColorName("Red");

    return loitering;
}  ////////// Edn of Artemis

ScenarioLoiteringType MissionsDefaults::MissionsDefaults_Alphasat()
{
}

ScenarioLoiteringType MissionsDefaults::MissionsDefaults_SmallGEO()
{
    ScenarioLoiteringType loitering;

    loitering.InitialPosition()->setCoordinateSystem("INERTIAL J2000");
    QSharedPointer<ScenarioKeplerianElementsType> initPos(new ScenarioKeplerianElementsType());
    initPos->setSemiMajorAxis(42169.48);
    initPos->setInclination(1.0);
    initPos->setEccentricity(0.0002);
    initPos->setRAAN(9.0532);
    initPos->setArgumentOfPeriapsis(121.74);
    initPos->setTrueAnomaly(0.0);
    loitering.InitialPosition()->setAbstract6DOFPosition(initPos);

    //loitering.InitialAttitude()->setCoordinateSystem("EULER 123");
    QSharedPointer<ScenarioEulerType> initAtt(new ScenarioEulerType());
    initAtt->setPhi(0.00000);
    initAtt->setTheta(0.00000);
    initAtt->setPsi(0.00000);
    initAtt->setPhiDot(0.00000);
    initAtt->setThetaDot(0.00000);
    initAtt->setPsiDot(0.00000);
    loitering.InitialAttitude()->setAbstract6DOFAttitude(initAtt);

    QDateTime TheCurrentDateAndTime = QDateTime::currentDateTime(); // Get the current epoch
    loitering.TimeLine()->setStartTime(TheCurrentDateAndTime);
    loitering.TimeLine()->setEndTime(TheCurrentDateAndTime.addDays(8));
    loitering.TimeLine()->setStepTime(60.0);

    loitering.PropagationPosition()->setPropagator("TWO BODY");
    loitering.PropagationPosition()->setIntegrator("Runge-Kutta 3-4");
    loitering.PropagationPosition()->setTimeStep(60.0);

    loitering.PropagationAttitude()->setIntegrator("Runge-Kutta 3-4");
    loitering.PropagationAttitude()->setTimeStep(60.0);

    loitering.Environment()->CentralBody()->setName("Earth");
    loitering.Environment()->CentralBody()->GravityModel()->setModelName("EGM2008");
    loitering.Environment()->CentralBody()->GravityModel()->setNumberOfTesserals(1);
    loitering.Environment()->CentralBody()->GravityModel()->setNumberOfZonals(1);

    loitering.ElementIdentifier()->setName("SmallGEO loitering");
    loitering.ElementIdentifier()->setTheOrder(1);
    loitering.ElementIdentifier()->setModelName("Default");
    loitering.ElementIdentifier()->setColorName("Yellow");

    return loitering;
}


// TEC
ScenarioLoiteringType MissionsDefaults::MissionsDefaults_PROBA1()
{
    ScenarioLoiteringType loitering;

    loitering.InitialPosition()->setCoordinateSystem("INERTIAL J2000");
    QSharedPointer<ScenarioKeplerianElementsType> initPos(new ScenarioKeplerianElementsType());
    initPos->setSemiMajorAxis(6371.0+551.0);
    initPos->setInclination(97.9);
    initPos->setEccentricity(0.0002);
    initPos->setRAAN(9.0532);
    initPos->setArgumentOfPeriapsis(121.74);
    initPos->setTrueAnomaly(0.0);
    loitering.InitialPosition()->setAbstract6DOFPosition(initPos);

    //loitering.InitialAttitude()->setCoordinateSystem("EULER 123");
    QSharedPointer<ScenarioEulerType> initAtt(new ScenarioEulerType());
    initAtt->setPhi(0.00000);
    initAtt->setTheta(0.00000);
    initAtt->setPsi(0.00000);
    initAtt->setPhiDot(0.00000);
    initAtt->setThetaDot(0.00000);
    initAtt->setPsiDot(0.00000);
    loitering.InitialAttitude()->setAbstract6DOFAttitude(initAtt);

    QDateTime TheCurrentDateAndTime = QDateTime::currentDateTime(); // Get the current epoch
    loitering.TimeLine()->setStartTime(TheCurrentDateAndTime);
    loitering.TimeLine()->setEndTime(TheCurrentDateAndTime.addDays(8));
    loitering.TimeLine()->setStepTime(60.0);

    loitering.PropagationPosition()->setPropagator("TWO BODY");
    loitering.PropagationPosition()->setIntegrator("Runge-Kutta 3-4");
    loitering.PropagationPosition()->setTimeStep(60.0);

    loitering.PropagationAttitude()->setIntegrator("Runge-Kutta 3-4");
    loitering.PropagationAttitude()->setTimeStep(60.0);

    loitering.Environment()->CentralBody()->setName("Earth");
    loitering.Environment()->CentralBody()->GravityModel()->setModelName("EGM2008");
    loitering.Environment()->CentralBody()->GravityModel()->setNumberOfTesserals(1);
    loitering.Environment()->CentralBody()->GravityModel()->setNumberOfZonals(1);

    loitering.ElementIdentifier()->setName("PROBA-1 loitering");
    loitering.ElementIdentifier()->setTheOrder(1);
    loitering.ElementIdentifier()->setModelName("Default");
    loitering.ElementIdentifier()->setColorName("Yellow");

    return loitering;
}

ScenarioLoiteringType MissionsDefaults::MissionsDefaults_PROBA2()
{
    ScenarioLoiteringType loitering;

    loitering.InitialPosition()->setCoordinateSystem("INERTIAL J2000");
    QSharedPointer<ScenarioKeplerianElementsType> initPos(new ScenarioKeplerianElementsType());
    initPos->setSemiMajorAxis(6371.0+728.0);
    initPos->setInclination(98.2);
    initPos->setEccentricity(0.0002);
    initPos->setRAAN(9.0532);
    initPos->setArgumentOfPeriapsis(121.74);
    initPos->setTrueAnomaly(0.0);
    loitering.InitialPosition()->setAbstract6DOFPosition(initPos);

    //loitering.InitialAttitude()->setCoordinateSystem("EULER 123");
    QSharedPointer<ScenarioEulerType> initAtt(new ScenarioEulerType());
    initAtt->setPhi(0.00000);
    initAtt->setTheta(0.00000);
    initAtt->setPsi(0.00000);
    initAtt->setPhiDot(0.00000);
    initAtt->setThetaDot(0.00000);
    initAtt->setPsiDot(0.00000);
    loitering.InitialAttitude()->setAbstract6DOFAttitude(initAtt);

    QDateTime TheCurrentDateAndTime = QDateTime::currentDateTime(); // Get the current epoch
    loitering.TimeLine()->setStartTime(TheCurrentDateAndTime);
    loitering.TimeLine()->setEndTime(TheCurrentDateAndTime.addDays(8));
    loitering.TimeLine()->setStepTime(60.0);

    loitering.PropagationPosition()->setPropagator("TWO BODY");
    loitering.PropagationPosition()->setIntegrator("Runge-Kutta 3-4");
    loitering.PropagationPosition()->setTimeStep(60.0);

    loitering.PropagationAttitude()->setIntegrator("Runge-Kutta 3-4");
    loitering.PropagationAttitude()->setTimeStep(60.0);

    loitering.Environment()->CentralBody()->setName("Earth");
    loitering.Environment()->CentralBody()->GravityModel()->setModelName("EGM2008");
    loitering.Environment()->CentralBody()->GravityModel()->setNumberOfTesserals(1);
    loitering.Environment()->CentralBody()->GravityModel()->setNumberOfZonals(1);

    loitering.ElementIdentifier()->setName("PROBA-2 loitering");
    loitering.ElementIdentifier()->setTheOrder(1);
    loitering.ElementIdentifier()->setModelName("Default");
    loitering.ElementIdentifier()->setColorName("Yellow");

    return loitering;
}

ScenarioLoiteringType MissionsDefaults::MissionsDefaults_PROBA3()
{
}


///////////////////////////////////////////// Human space flight ////////////////////////////////////////
ScenarioLoiteringType MissionsDefaults::MissionsDefaults_ISS()
{
    ScenarioLoiteringType loitering;

    loitering.InitialPosition()->setCoordinateSystem("INERTIAL J2000");
    QSharedPointer<ScenarioKeplerianElementsType> initPos(new ScenarioKeplerianElementsType());
    initPos->setSemiMajorAxis(6378.0 + 450.1365);
    initPos->setInclination(51.6651);
    initPos->setEccentricity(0.0006);
    initPos->setRAAN(12.4829);
    initPos->setArgumentOfPeriapsis(45.0278);
    initPos->setTrueAnomaly(87.3523);
    loitering.InitialPosition()->setAbstract6DOFPosition(initPos);

    //loitering.InitialAttitude()->setCoordinateSystem("EULER 123");
    QSharedPointer<ScenarioEulerType> initAtt(new ScenarioEulerType());
    initAtt->setPhi(0.00000);
    initAtt->setTheta(0.00000);
    initAtt->setPsi(0.00000);
    initAtt->setPhiDot(0.00000);
    initAtt->setThetaDot(0.00000);
    initAtt->setPsiDot(0.00000);
    loitering.InitialAttitude()->setAbstract6DOFAttitude(initAtt);

    QDateTime TheCurrentDateAndTime = QDateTime::currentDateTime(); // Get the current epoch
    loitering.TimeLine()->setStartTime(TheCurrentDateAndTime);
    loitering.TimeLine()->setEndTime(TheCurrentDateAndTime.addDays(1));
    loitering.TimeLine()->setStepTime(60.0);

    loitering.PropagationPosition()->setPropagator("TWO BODY");
    loitering.PropagationPosition()->setIntegrator("Runge-Kutta 3-4");
    loitering.PropagationPosition()->setTimeStep(60.0);

    loitering.PropagationAttitude()->setIntegrator("Runge-Kutta 3-4");
    loitering.PropagationAttitude()->setTimeStep(60.0);

    loitering.Environment()->CentralBody()->setName("Earth");
    loitering.Environment()->CentralBody()->GravityModel()->setModelName("EGM2008");
    loitering.Environment()->CentralBody()->GravityModel()->setNumberOfTesserals(1);
    loitering.Environment()->CentralBody()->GravityModel()->setNumberOfZonals(1);

    loitering.ElementIdentifier()->setName("ISS loitering");
    loitering.ElementIdentifier()->setTheOrder(1);
    loitering.ElementIdentifier()->setModelName("ISS");
    loitering.ElementIdentifier()->setColorName("Yellow");

    return loitering;
}




ScenarioEntryArcType MissionsDefaults::MissionsDefaults_ATV()
{
}



// Launchers
/*
ScenarioLoiteringType MissionsDefaults_VEGA()
{
}

ScenarioLoiteringType MissionsDefaults_Soyuz()
{
}

ScenarioLoiteringType MissionsDefaults_Ariane5()
{
}
*/


ScenarioEntryArcType MissionsDefaults::MissionsDefaults_IXV()
{
}




// List of allowable missions
QList<QString> MissionsDefaults::esaDefaultLoiteringMissions()
{
    QList<QString> myMissionsList;
    myMissionsList <<
            "XMM" <<
            "MEX" <<
            "INTEGRAL" <<
            "VEX" <<
            "Aeolus" <<
            "Artemis" <<
            "ISS" <<
            "ENVISAT" <<
            "ERS2" <<
            "METEOSAT6" <<
            "METEOSAT7" <<
            "METEOSAT8" <<
            "METEOSAT9" <<
            "CRYOSAT" <<
            "SENTINEL1A" <<
            "SMALLGEO" <<
            "PROBA1" <<
            "PROBA2";

    return myMissionsList;
}


QList<QString> MissionsDefaults::nasaDefaultLoiteringMissions()
{
    QList<QString> myMissionsList;
    myMissionsList << "";
    return myMissionsList;

}




QColor MissionsDefaults::missionArcColorFromQt(QString colorLabel)
{
    if (colorLabel == "Yellow")
        return Qt::yellow;
    else if (colorLabel == "Red")
        return Qt::red;
    else if (colorLabel == "Green")
        return Qt::green;
    else if (colorLabel == "Blue")
        return Qt::blue;
    else if (colorLabel == "Magenta")
        return Qt::magenta;
    else if (colorLabel == "White")
        return Qt::white;
    else if (colorLabel == "Cyan")
        return Qt::cyan;
    else if (colorLabel == "Gray")
        return Qt::gray;
    else if (colorLabel == "Dark Red")
        return Qt::darkRed;
    else if (colorLabel == "Dark Yellow")
        return Qt::darkYellow;
    else if (colorLabel == "Dark Blue")
        return Qt::darkBlue;
    else if (colorLabel == "Dark Magenta")
        return Qt::darkMagenta;
    else if (colorLabel == "Light Gray")
        return Qt::lightGray;
    else if (colorLabel == "Drak Gray")
        return Qt::darkGray;
    else if (colorLabel == "Dark Cyan")
        return Qt::darkCyan;
    else if (colorLabel == "Black")
        return Qt::black;
    else if (colorLabel == "Hide")
        return Qt::transparent;
}




