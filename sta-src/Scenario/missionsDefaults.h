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
------------------ Author: Guillermo Ortega  -------------------------------------------
*/

#ifndef _MissionsDefaults_H_
#define _MissionsDefaults_H_

#include "Scenario/staschema.h"

/**
 * Class to create a spacecraft defaults for some ESA and NASA missions and
 * some generic payloads
 */
class MissionsDefaults
{

public:
    ~MissionsDefaults();

     ScenarioSCSystemType MissionsDefaults_GenericPlatform();
     ScenarioTransmitterPayloadType MissionsDefaults_GenericTransmitter();
     ScenarioReceiverPayloadType MissionsDefaults_GenericReceiver();
     ScenarioOpticalPayloadType MissionsDefaults_GenericOpticalPayload();
     ScenarioRadarPayloadType MissionsDefaults_GenericRadarPayload();


    // Science
    ScenarioLoiteringType MissionsDefaults_XMM();
    ScenarioLoiteringType MissionsDefaults_INTEGRAL();
    ScenarioLoiteringType MissionsDefaults_MEX();
    ScenarioLoiteringType MissionsDefaults_Rosetta();
    ScenarioLoiteringType MissionsDefaults_VEX();
    ScenarioLoiteringType MissionsDefaults_Herschel();
    ScenarioLoiteringType MissionsDefaults_Planck();
    ScenarioLoiteringType MissionsDefaults_LISA();
    ScenarioLoiteringType MissionsDefaults_BepiColombo();
    ScenarioLoiteringType MissionsDefaults_ExoMars();

    // Observation of the Earth
    ScenarioLoiteringType MissionsDefaults_Meteosat6();
    ScenarioLoiteringType MissionsDefaults_ERS2();
    ScenarioLoiteringType MissionsDefaults_ENVISAT();
    ScenarioLoiteringType MissionsDefaults_MSG();
    ScenarioLoiteringType MissionsDefaults_MTG();
    ScenarioLoiteringType MissionsDefaults_METOP();
    ScenarioLoiteringType MissionsDefaults_Cryosat();
    ScenarioLoiteringType MissionsDefaults_GOCE();
    ScenarioLoiteringType MissionsDefaults_SMOS();
    ScenarioLoiteringType MissionsDefaults_Aeolus();
    ScenarioLoiteringType MissionsDefaults_EarthCare();
    ScenarioLoiteringType MissionsDefaults_Sentinel1();
    ScenarioLoiteringType MissionsDefaults_Sentinel2();
    ScenarioLoiteringType MissionsDefaults_Sentinel3();
    ScenarioLoiteringType MissionsDefaults_Sentinel5();

    // Tecommunications
    ScenarioLoiteringType MissionsDefaults_Artemis();
    ScenarioLoiteringType MissionsDefaults_Alphasat();
    ScenarioLoiteringType MissionsDefaults_SmallGEO();

    // TEC
    ScenarioLoiteringType MissionsDefaults_PROBA1();
    ScenarioLoiteringType MissionsDefaults_PROBA2();
    ScenarioLoiteringType MissionsDefaults_PROBA3();

    // Human space flight
    ScenarioLoiteringType MissionsDefaults_ISS();
    ScenarioLoiteringType MissionsDefaults_ATV();

    // Launchers
    ScenarioLoiteringType MissionsDefaults_VEGA();
    ScenarioLoiteringType MissionsDefaults_Soyuz();
    ScenarioLoiteringType MissionsDefaults_Ariane5();
    ScenarioLoiteringType MissionsDefaults_IXV();

};

#endif // _MissionsDefaults_H_

