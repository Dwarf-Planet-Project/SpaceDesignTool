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

*/

#include "Scenario/staschema.h"
#include "Scenario/missionsDefaults.h"




MissionsDefaults::~MissionsDefaults()
{

}


/////////////////////////////////////// Generic elements //////////////////////////////////////

ScenarioSCSystemType MissionsDefaults::MissionsDefaults_GenericPlatform()
{
    ScenarioSCSystemType genericPlatform;

    genericPlatform.SystemBudgets()->MassOfSystem()->setDryMass(1000.0);
    genericPlatform.SystemBudgets()->MassOfSystem()->setWetMass(1500.0);
    genericPlatform.SystemBudgets()->PowerOfSystem()->setTotalPowerBoL(200.0);
    genericPlatform.SystemBudgets()->PowerOfSystem()->setTotalPowerEoL(100.0);
    genericPlatform.SystemBudgets()->Link()->setDownlinkMargin(10);
    genericPlatform.SystemBudgets()->Link()->setUplinkMargin(10);

    genericPlatform.SCAerodynamics()->setSurfaceArea(1.0);
    genericPlatform.SCAerodynamics()->setCd(0.25);

    genericPlatform.Propulsion()->setNumberOfEngines(1);
    genericPlatform.Propulsion()->setPropellantMass(500);
    genericPlatform.Propulsion()->setSpecificImpulse(312);
    genericPlatform.Propulsion()->setThrustPerEngine(100);
    genericPlatform.Propulsion()->setTotalPropulsionDryMass(25);

    genericPlatform.Structure()->setStructuralShape("Cube");			    // Cube, Cylinder or Sphere
    genericPlatform.Structure()->setStructuralMaterial("Aluminium");		    // Steel or Aluminium
    genericPlatform.Structure()->setTotalStructureMass(800);
    genericPlatform.Structure()->Sizing()->setHeight(1);
    genericPlatform.Structure()->Sizing()->setLength(1);
    genericPlatform.Structure()->Sizing()->setWidth(1);
    genericPlatform.Structure()->Sizing()->setVolume(1);
    genericPlatform.Structure()->MomentsOfInertia()->setXAxis(150);
    genericPlatform.Structure()->MomentsOfInertia()->setYAxis(150);
    genericPlatform.Structure()->MomentsOfInertia()->setZAxis(150);
    genericPlatform.Structure()->SecondMomentsOfArea()->setXAxis(20);
    genericPlatform.Structure()->SecondMomentsOfArea()->setYAxis(20);
    genericPlatform.Structure()->SecondMomentsOfArea()->setZAxis(20);
    genericPlatform.Structure()->NaturalFrequency()->setAxialFrequency(50);
    genericPlatform.Structure()->NaturalFrequency()->setLateralFrequency(12);

    genericPlatform.TCS()->Temperature()->setMaximumSCTemperature(85);
    genericPlatform.TCS()->Temperature()->setMinimumSCTemperature(15);
    genericPlatform.TCS()->setHeaterPower(50);
    genericPlatform.TCS()->setRadiatedPower(12);
    genericPlatform.TCS()->setHeaterPower(25);
    genericPlatform.TCS()->setTotalTCSMass(15);
    genericPlatform.TCS()->CoatingArea()->ColdSurface()->ColdCoating()->setAbsorptivity(0.2);
    genericPlatform.TCS()->CoatingArea()->ColdSurface()->ColdCoating()->setEmissivity(0.3);
    genericPlatform.TCS()->CoatingArea()->ColdSurface()->setTotalColdArea(1);
    genericPlatform.TCS()->CoatingArea()->HotSurface()->HotCoating()->setAbsorptivity(0.2);
    genericPlatform.TCS()->CoatingArea()->HotSurface()->HotCoating()->setEmissivity(0.3);
    genericPlatform.TCS()->CoatingArea()->HotSurface()->setTotalHotArea(1);

    genericPlatform.EPS()->SolarArray()->setSACellType("Silicon");		    // Silicon, GaAs, Multijunction
    genericPlatform.EPS()->SolarArray()->setAreaOfSolarArray(4);
    genericPlatform.EPS()->SolarArray()->setMassOfSolarArray(30);
    genericPlatform.EPS()->BatteryType()->setBatteryType("Lithium_Ion");	    // Lithium_Ion, Nickel_Cadmium, Lead_Acid
    genericPlatform.EPS()->BatteryType()->setNumberOfBatteries(4);
    genericPlatform.EPS()->BatteryType()->setTotalMassOfBatteries(30);
    genericPlatform.EPS()->BatteryType()->setTotalVolumeOfBatteries(0.01);
    genericPlatform.EPS()->setMassOfPCU(0.1);
    genericPlatform.EPS()->setTotalEPSMass(70);

    genericPlatform.TTC()->setTotalTTCMass(70);
    genericPlatform.TTC()->TTCAntenna()->setDataRate(300);
    genericPlatform.TTC()->TTCAntenna()->setLinkDuration(10);
    genericPlatform.TTC()->TTCAntenna()->setTransmitterEfficiency(90);
    genericPlatform.TTC()->TTCAntenna()->setTransmitterFrequency(25);
    genericPlatform.TTC()->TTCAntenna()->setTransmitterGain(30);
    genericPlatform.TTC()->TTCAntenna()->setTransmitterPower(100);

    genericPlatform.AOCS()->setTotalAOCSMass(0.5);

    genericPlatform.OBDH()->setTotalOBDHMass(0.5);
    genericPlatform.OBDH()->setTotalSizeOfMemory(200);

    return genericPlatform;

}


ScenarioTransmitterPayloadType MissionsDefaults::MissionsDefaults_GenericTransmitter()
{

}

ScenarioReceiverPayloadType MissionsDefaults::MissionsDefaults_GenericReceiver()
{

}

ScenarioOpticalPayloadType MissionsDefaults::MissionsDefaults_GenericOpticalPayload()
{

}

ScenarioRadarPayloadType MissionsDefaults::MissionsDefaults_GenericRadarPayload()
{

}



//////////////////////////////////////// Generic Loitering /////////////////////////////////////////////////

ScenarioLoiteringType MissionsDefaults::MissionsDefaults_GENERIC()
{
    ScenarioLoiteringType loitering;

    loitering.InitialPosition()->setCoordinateSystem("INERTIAL J2000");
    QSharedPointer<ScenarioKeplerianElementsType> initPos(new ScenarioKeplerianElementsType());
    initPos->setSemiMajorAxis(7100.0);
    initPos->setInclination(31.2);
    initPos->setEccentricity(0.1);
    initPos->setRAAN(123.45);
    initPos->setArgumentOfPeriapsis(10.23);
    initPos->setTrueAnomaly(0.0);
    loitering.InitialPosition()->setAbstract6DOFPosition(initPos);

    loitering.InitialAttitude()->setCoordinateSystem("EULER 123");
    QSharedPointer<ScenarioEulerBIType> initAtt(new ScenarioEulerBIType());
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

    loitering.ElementIdentifier()->setName("loitering arc");
    loitering.ElementIdentifier()->setTheOrder(1);
    loitering.ElementIdentifier()->setModelName("Default");
    loitering.ElementIdentifier()->setColorName("Yellow");

    return loitering;
} /////////////////////// end of GENERIC




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

    loitering.InitialAttitude()->setCoordinateSystem("EULER 123");
    QSharedPointer<ScenarioEulerBIType> initAtt(new ScenarioEulerBIType());
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

    loitering.InitialAttitude()->setCoordinateSystem("EULER 123");
    QSharedPointer<ScenarioEulerBIType> initAtt(new ScenarioEulerBIType());
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
}

ScenarioLoiteringType MissionsDefaults::MissionsDefaults_ERS2()
{
}

ScenarioLoiteringType MissionsDefaults::MissionsDefaults_ENVISAT()
{
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

    loitering.InitialAttitude()->setCoordinateSystem("EULER 123");
    QSharedPointer<ScenarioEulerBIType> initAtt(new ScenarioEulerBIType());
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

    loitering.ElementIdentifier()->setName("Aeolus loitering");
    loitering.ElementIdentifier()->setTheOrder(1);
    loitering.ElementIdentifier()->setModelName("Default");
    loitering.ElementIdentifier()->setColorName("Green");

    return loitering;
} /////////// End of Aeolus



ScenarioLoiteringType MissionsDefaults::MissionsDefaults_EarthCare()
{
}

ScenarioLoiteringType MissionsDefaults::MissionsDefaults_Sentinel1()
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
    initPos->setRAAN(100.35);
    initPos->setArgumentOfPeriapsis(201.76);
    initPos->setTrueAnomaly(0.0);
    loitering.InitialPosition()->setAbstract6DOFPosition(initPos);

    loitering.InitialAttitude()->setCoordinateSystem("EULER 123");
    QSharedPointer<ScenarioEulerBIType> initAtt(new ScenarioEulerBIType());
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
}


// TEC
ScenarioLoiteringType MissionsDefaults::MissionsDefaults_PROBA1()
{
}

ScenarioLoiteringType MissionsDefaults::MissionsDefaults_PROBA2()
{
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

    loitering.InitialAttitude()->setCoordinateSystem("EULER 123");
    QSharedPointer<ScenarioEulerBIType> initAtt(new ScenarioEulerBIType());
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
	    "Aeolus" <<
	    "Artemis" <<
	    "ISS";
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
}




