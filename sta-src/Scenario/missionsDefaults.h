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

    void MissionsDefaults_GenericPlatform(ScenarioSCSystemType* genericPlatform);
    void MissionsDefaults_GenericTransmitter(ScenarioTransmitterPayloadType* genericTransmitter);
    void MissionsDefaults_GenericReceiver(ScenarioReceiverPayloadType* genericReceiver);
    void MissionsDefaults_GenericOpticalPayload(ScenarioOpticalPayloadType* genericOpticalPayload);
    void MissionsDefaults_GenericRadarPayload(ScenarioRadarPayloadType* genericRadarPayload);


    // Science
    void MissionsDefaults_XMM(ScenarioLoiteringType* loiteringXMM);
    void MissionsDefaults_INTEGRAL(ScenarioLoiteringType* loiteringINTEGRAL);
    void MissionsDefaults_MEX(ScenarioLoiteringType* loiteringMEX);
    void MissionsDefaults_Rosetta(ScenarioLoiteringType* loiteringRosetta);
    void MissionsDefaults_VEX(ScenarioLoiteringType* loiteringVEX);
    void MissionsDefaults_Herschel(ScenarioLoiteringType* loiteringHerschel);
    void MissionsDefaults_Planck(ScenarioLoiteringType* loiteringPlanck);
    void MissionsDefaults_LISA(ScenarioLoiteringType* loiteringLISA);
    void MissionsDefaults_BepiColombo(ScenarioLoiteringType* loiteringBepiColombo);
    void MissionsDefaults_ExoMars(ScenarioLoiteringType* loiteringExoMars);

    // Observation of the Earth
    void MissionsDefaults_Meteosat6(ScenarioLoiteringType* loiteringMeteosat6);
    void MissionsDefaults_ERS2(ScenarioLoiteringType* loiteringERS2);
    void MissionsDefaults_ENVISAT(ScenarioLoiteringType* loiteringENVISAT);
    void MissionsDefaults_MSG(ScenarioLoiteringType* loiteringMSG);
    void MissionsDefaults_MTG(ScenarioLoiteringType* loiteringMTG);
    void MissionsDefaults_METOP(ScenarioLoiteringType* loiteringMETOP);
    void MissionsDefaults_Cryosat(ScenarioLoiteringType* loiteringCryosat);
    void MissionsDefaults_GOCE(ScenarioLoiteringType* loiteringGOCE);
    void MissionsDefaults_SMOS(ScenarioLoiteringType* loiteringSMOS);
    void MissionsDefaults_Aeolus(ScenarioLoiteringType* loiteringAeolus);
    void MissionsDefaults_EarthCare(ScenarioLoiteringType* loiteringEarthCare);
    void MissionsDefaults_Sentinel1(ScenarioLoiteringType* loiteringSentine1);
    void MissionsDefaults_Sentinel2(ScenarioLoiteringType* loiteringSentine2);
    void MissionsDefaults_Sentinel3(ScenarioLoiteringType* loiteringSentine3);
    void MissionsDefaults_Sentinel5(ScenarioLoiteringType* loiteringSentine5);

    // Tecommunications
    void MissionsDefaults_Artemis(ScenarioLoiteringType* loiteringArtemis);
    void MissionsDefaults_Alphasat(ScenarioLoiteringType* loiteringAlphasat);
    void MissionsDefaults_SmallGEO(ScenarioLoiteringType* loiteringSmallGEO);

    // TEC
    void MissionsDefaults_PROBA1(ScenarioLoiteringType* loiteringPROBA1);
    void MissionsDefaults_PROBA2(ScenarioLoiteringType* loiteringPROBA2);
    void MissionsDefaults_PROBA3(ScenarioLoiteringType* loiteringPROBA3);

    // Human space flight
    ScenarioLoiteringType MissionsDefaults_ISS();
    void MissionsDefaults_ATV(ScenarioLoiteringType* loiteringATV);

    // Launchers
    void MissionsDefaults_VEGA(ScenarioLoiteringType* loiteringVEGA);
    void MissionsDefaults_Soyuz(ScenarioLoiteringType* loiteringSoyuz);
    void MissionsDefaults_Ariane5(ScenarioLoiteringType* loiteringAriane5);
    void MissionsDefaults_IXV(ScenarioLoiteringType* loiteringIXV);

};

#endif // _MissionsDefaults_H_

