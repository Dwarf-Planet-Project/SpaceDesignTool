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
 ------------------ Author:       Tiziana Sabatini               -------------------
 ------------------ E-Mail: tiziana.sabatini@yahoo.it            -------------------
 -----------------------------------------------------------------------------------
 Created July 2009
*/

#include "reentrystructures.h"
#include <QTextStream>
#include <QtDebug>
#include "Astro-Core/AngleConversion.h"
#include "Astro-Core/Atmosphere/AtmosphereModel.h"
#include "HeatRateClass.h"


EntrySettings createEntrySettings(ScenarioEntryArcType* entry, ScenarioREV* vehicle)
{
    EntrySettings settings;
    //Take the atmospheric file name from the perturbation list
    /*
    QList<Perturbations*> perturbationsList = scenarioReentry->environment()->createListPerturbations(vehicleProperties);
    foreach(Perturbations* perturbation, perturbationsList)
    {
        if(dynamic_cast<AtmosphericDragPerturbations*>(perturbation))
        {
            AtmosphericDragPerturbations* dragperturbation = dynamic_cast<AtmosphericDragPerturbations*>(perturbation);
            settings.modelname = dragperturbation->atmosphericModel();
        }
    }
    */
    settings.modelname="gram99.stad";//entry->Environment()->CentralBody()->atmosphere().append(".stad");

    settings.bodyname = entry->Environment()->CentralBody()->Name();
    
    settings.propagator = entry->PropagationPosition()->propagator();
    settings.integrator = entry->PropagationPosition()->integrator();
    settings.stepsize = entry->PropagationPosition()->timeStep();
    settings.CdCprofilename = vehicle->REVSystem()->AeroThermodynamics()->AeroCoefFile()[0]->FileLocation();
    settings.ClCprofilename = vehicle->REVSystem()->AeroThermodynamics()->AeroCoefFile()[2]->FileLocation();
    settings.CsCprofilename = vehicle->REVSystem()->AeroThermodynamics()->AeroCoefFile()[1]->FileLocation();
    //settings.parachuteArea = vehicleProperties->aerodynamicProperties()->parachuteProperties()->surfaceArea();
    settings.parachuteDeployMach = 0;//vehicleProperties->aerodynamicProperties()->parachuteProperties()->deploymentMach();
    settings.maxloadfactor = entry->Constraints()->maxNormalLoad();
    settings.maxheatrate = entry->Constraints()->maxHeatFlux();
    settings.maxaltitude = entry->Constraints()->maxAltitude();

    return settings;
}

EntryParameters createEntryParametersSimulation(ScenarioEntryArcType* entry, ScenarioREV* vehicle)
{

    EntryParameters parameters;

    parameters.Sref = vehicle->REVSystem()->AeroThermodynamics()->referenceArea();
    parameters.cref= vehicle->REVSystem()->AeroThermodynamics()->referenceLength();
    parameters.m = vehicle->REVSystem()->Weights()->totalDryMass();
    parameters.Rn = vehicle->REVSystem()->Geometry()->noseRadius();

    QString coordinatesystem = entry->InitialPosition()->CoordinateSystem();
    if (dynamic_cast<ScenarioSphericalCoordinatesType*>(entry->InitialPosition()->Abstract6DOFPosition().data()))
    {
        parameters.coordselector = 1;
        ScenarioSphericalCoordinatesType* sphericalCoord = dynamic_cast<ScenarioSphericalCoordinatesType*>(entry->InitialPosition()->Abstract6DOFPosition().data());
        parameters.inputstate[0] = 1000*sphericalCoord->radialDistance();
        parameters.inputstate[1] = sphericalCoord->longitude();
        parameters.inputstate[2] = sphericalCoord->latitude();
        parameters.inputstate[3] = 1000*sphericalCoord->flightPathVelocity();
        parameters.inputstate[4] = sphericalCoord->flightPathAngle();
        parameters.inputstate[5] = sphericalCoord->headingAngle();
    }
    else if (dynamic_cast<ScenarioKeplerianElementsType*>(entry->InitialPosition()->Abstract6DOFPosition().data()))
    {
        parameters.coordselector = 2;
        ScenarioKeplerianElementsType* elements = dynamic_cast<ScenarioKeplerianElementsType*>(entry->InitialPosition()->Abstract6DOFPosition().data());
        parameters.inputstate[0] = 1000 * elements->semiMajorAxis();
        parameters.inputstate[1] = elements->eccentricity();
        parameters.inputstate[2] = elements->inclination();
        parameters.inputstate[3] = elements->RAAN();
        parameters.inputstate[4] = elements->argumentOfPeriapsis();
        parameters.inputstate[5] = elements->trueAnomaly();
    }
    /*
    else if (coordinatesystem=="StateVector")
    {
        parameters.coordselector = 3;
        ScenarioStateVector* vector = dynamic_cast<ScenarioStateVector*>(scenarioReentry->simulationMode()->simulationParameters()->initialStatePosition()->initialState());
        parameters.inputstate[0] = 1000 * vector->position().x();
        parameters.inputstate[1] = 1000 * vector->position().y();
        parameters.inputstate[2] = 1000 * vector->position().z();
        parameters.inputstate[3] = 1000 * vector->velocity().x();
        parameters.inputstate[4] = 1000 * vector->velocity().y();
        parameters.inputstate[5] = 1000 * vector->velocity().z();
    }
    */
    else  qFatal("Error in the initial state position coordinate systems");

    return parameters;
}



//EntryParameterIntervals ReentryValues::createEntryParameterIntervals(ScenarioReEntryTrajectory* scenarioReentry)
//{
//
//        int xparameter, yparameter;
//        EntryParameterIntervals parameterIntervals;
//
//        xparameter = scenarioReentry->windowMode()->window()->xAxisVariable()->returnType();//;2;//
//        yparameter = scenarioReentry->windowMode()->window()->yAxisVariable()->returnType();//4;//
//
//        parameterIntervals.K.low = scenarioReentry->windowMode()->fixedVariables()->ballisticCoeff();//38.2;//
//        parameterIntervals.K.high = scenarioReentry->windowMode()->fixedVariables()->ballisticCoeff();//38.2;//
//        parameterIntervals.Rn.low = scenarioReentry->vehicle()->mainBodyCharacteristics()->radiusNose();//0.1;//
//        parameterIntervals.Rn.high = scenarioReentry->vehicle()->mainBodyCharacteristics()->radiusNose();//0.4;//
//        parameterIntervals.velocity.low = scenarioReentry->windowMode()->fixedVariables()->inertialVelocity();//12400;//
//        parameterIntervals.velocity.high = scenarioReentry->windowMode()->fixedVariables()->inertialVelocity();//12400;//
//        parameterIntervals.gamma.low = scenarioReentry->windowMode()->fixedVariables()->inertialFlightPathAngle();//-12;//
//        parameterIntervals.gamma.high = scenarioReentry->windowMode()->fixedVariables()->inertialFlightPathAngle();//-4;//
//        parameterIntervals.chi.low = scenarioReentry->windowMode()->fixedVariables()->inertialHeading();//90;//
//        parameterIntervals.chi.high = scenarioReentry->windowMode()->fixedVariables()->inertialHeading();//90;//
//
//        switch (xparameter)
//         {
//             case(0):
//
//                 parameterIntervals.K.low = scenarioReentry->windowMode()->window()->xAxisVariable()->minRange();
//                 parameterIntervals.K.high = scenarioReentry->windowMode()->window()->xAxisVariable()->maxRange();
//                 break;
//             case(1):
//
//                 parameterIntervals.Rn.low = scenarioReentry->windowMode()->window()->xAxisVariable()->minRange();
//                 parameterIntervals.Rn.high = scenarioReentry->windowMode()->window()->xAxisVariable()->maxRange();
//                 break;
//             case(2):
//
//                parameterIntervals.velocity.low = scenarioReentry->windowMode()->window()->xAxisVariable()->minRange();
//                parameterIntervals.velocity.high = scenarioReentry->windowMode()->window()->xAxisVariable()->maxRange();
//                break;
//             case(3):
//
//                 parameterIntervals.gamma.low = scenarioReentry->windowMode()->window()->xAxisVariable()->minRange();
//                 parameterIntervals.gamma.high = scenarioReentry->windowMode()->window()->xAxisVariable()->maxRange();
//                 break;
//             case(4):
//
//                 parameterIntervals.chi.low = scenarioReentry->windowMode()->window()->xAxisVariable()->minRange();
//                 parameterIntervals.chi.high = scenarioReentry->windowMode()->window()->xAxisVariable()->maxRange();
//                 break;
//         }
//
//        switch (yparameter)
//         {
//             case(0):
//
//                 parameterIntervals.K.low = scenarioReentry->windowMode()->window()->yAxisVariable()->minRange();
//                 parameterIntervals.K.high = scenarioReentry->windowMode()->window()->yAxisVariable()->maxRange();
//                 break;
//             case(1):
//
//                 parameterIntervals.Rn.low = scenarioReentry->windowMode()->window()->yAxisVariable()->minRange();
//                 parameterIntervals.Rn.high = scenarioReentry->windowMode()->window()->yAxisVariable()->maxRange();
//                 break;
//             case(2):
//
//                 parameterIntervals.velocity.low = scenarioReentry->windowMode()->window()->yAxisVariable()->minRange();
//                 parameterIntervals.velocity.high = scenarioReentry->windowMode()->window()->yAxisVariable()->maxRange();
//                 break;
//             case(3):
//
//                 parameterIntervals.gamma.low = scenarioReentry->windowMode()->window()->yAxisVariable()->minRange();
//                 parameterIntervals.gamma.high = scenarioReentry->windowMode()->window()->yAxisVariable()->maxRange();
//                 break;
//             case(4):
//
//                 parameterIntervals.chi.low = scenarioReentry->windowMode()->window()->yAxisVariable()->minRange();
//                 parameterIntervals.chi.high = scenarioReentry->windowMode()->window()->yAxisVariable()->maxRange();
//                 break;
//         }
//        return parameterIntervals;
//}



//EntryParameters ReentryValues::createEntryParametersWindow(ScenarioReEntryTrajectory* scenarioReentry, EntryParameterIntervals parameterIntervals)
//{
//        EntryParameters parameters;
//        int InputNsimulations, Npoints, Nsimulations;
//
//        double cd = 1.5;
//        bool failed_target = false;
//
//        InputNsimulations = scenarioReentry->windowMode()->numSimulations();//800;//
//        Npoints = int ( round (pow(InputNsimulations,0.5)) );
//        Nsimulations = Npoints * Npoints;
//        parameters.coordselector = 1;
//
//        parameters.inputstate[0] = scenarioReentry->windowMode()->fixedVariables()->entryAltitude();//125000;//   //The initial altitude is always fixed
//
//        parameters.m = scenarioReentry->vehicle()->mainBodyCharacteristics()->mass();//45;
//        double averageK = (parameterIntervals.K.high + parameterIntervals.K.low)/2.0;
//        parameters.R = sqrt( parameters.m / (averageK * acos(-1) * cd) );
//        parameters.Rn = (parameterIntervals.Rn.high + parameterIntervals.Rn.low)/2.0;
//        parameters.inputstate[1] = 0.0;
//        parameters.inputstate[2] = 0.0;
//        parameters.inputstate[3] = (parameterIntervals.velocity.high + parameterIntervals.velocity.low)/2.0;
//        parameters.inputstate[4] = (parameterIntervals.gamma.high + parameterIntervals.gamma.low)/2.0;
//        parameters.inputstate[5] = (parameterIntervals.chi.high + parameterIntervals.chi.low)/2.0;
//
//        return parameters;
//    }
//
//
//angle ReentryValues::createTargetAngleLatitude(ScenarioReEntryTrajectory* scenarioReentry)
//{
//    angle TargetLatitude;
//    AngleConversion angleconversion;
//
//    double latitudeDecimal = scenarioReentry->simulationMode()->simulationParameters()->finalState()->targetLat();//-28.56;//
//    angleconversion.decimalToDegMinSec(latitudeDecimal, TargetLatitude.degrees, TargetLatitude.minutes, TargetLatitude.seconds);
//
//    return TargetLatitude;
//}
//
//angle ReentryValues::createTargetAngleLongitude(ScenarioReEntryTrajectory* scenarioReentry)
//{
//    angle TargetLongitude;
//    AngleConversion angleconversion;
//
//    double longitudeDecimal = scenarioReentry->simulationMode()->simulationParameters()->finalState()->targetLon();//134.45;//
//    angleconversion.decimalToDegMinSec(longitudeDecimal, TargetLongitude.degrees, TargetLongitude.minutes, TargetLongitude.seconds);
//
//    return TargetLongitude;
//}
//
//angle ReentryValues::createTargetAngleLatitudeWindow(ScenarioReEntryTrajectory* scenarioReentry)
//{
//    angle TargetLatitude;
//    AngleConversion angleconversion;
//
//    double latitudeDecimal = scenarioReentry->windowMode()->simulationParameters()->finalState()->targetLat();//-28.56;//
//    angleconversion.decimalToDegMinSec(latitudeDecimal, TargetLatitude.degrees, TargetLatitude.minutes, TargetLatitude.seconds);
//
//    return TargetLatitude;
//}
//
//angle ReentryValues::createTargetAngleLongitudeWindow(ScenarioReEntryTrajectory* scenarioReentry)
//{
//    angle TargetLongitude;
//    AngleConversion angleconversion;
//
//    double longitudeDecimal = scenarioReentry->windowMode()->simulationParameters()->finalState()->targetLon();//134.45;//
//    angleconversion.decimalToDegMinSec(longitudeDecimal, TargetLongitude.degrees, TargetLongitude.minutes, TargetLongitude.seconds);
//
//    return TargetLongitude;
//}
//
//Uncertainties_struct ReentryValues::createUncertainties_struct(ScenarioReEntryTrajectory* scenarioReentry)
//{
//    Uncertainties_struct inputuncertainties;
//
//    inputuncertainties.mass = scenarioReentry->simulationMode()->dispersionAnalysis()->deviations()->capsuleMass();
//    //inputuncertainties.altitude = scenarioReentry->simulationMode()->dispersionAnalysis()->deviations()->;
//    inputuncertainties.tau = scenarioReentry->simulationMode()->dispersionAnalysis()->deviations()->capsuleBaseRadius();
//    //inputuncertainties.delta = scenarioReentry->simulationMode()->dispersionAnalysis();
//    inputuncertainties.velocity = scenarioReentry->simulationMode()->dispersionAnalysis()->deviations()->velocity();
//    inputuncertainties.gamma = scenarioReentry->simulationMode()->dispersionAnalysis()->deviations()->flightPathAngle();
//    inputuncertainties.chi = scenarioReentry->simulationMode()->dispersionAnalysis()->deviations()->inertialHeading();
//    inputuncertainties.density = scenarioReentry->simulationMode()->dispersionAnalysis()->deviations()->atmosphereDensity();
//    //inputuncertainties.temperature = scenarioReentry->simulationMode()->dispersionAnalysis()->deviations()->;
//    inputuncertainties.pressure;
//    //inputuncertainties.speedofsound = scenarioReentry->simulationMode()->dispersionAnalysis()->deviations()->;
//    inputuncertainties.cdcapsule = scenarioReentry->simulationMode()->dispersionAnalysis()->deviations()->capsuleCD();
//    inputuncertainties.cdparachute = scenarioReentry->simulationMode()->dispersionAnalysis()->deviations()->parachuteCD();
//
//    return inputuncertainties;
//
//}
//
