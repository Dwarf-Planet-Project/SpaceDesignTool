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
Updated March 2011
*/

#ifndef _MissionsDefaults_H_
#define _MissionsDefaults_H_

#include "Scenario/staschema.h"

#include <QColor>

/**
 * Class to create a spacecraft defaults for some ESA and NASA missions and
 * some generic payloads
 */
class MissionsDefaults
{

public:
    ~MissionsDefaults();

    // Default platform and payloads for a generic mission
    QSharedPointer<ScenarioSCSystemType> MissionsDefaults_GenericPlatform();
    QSharedPointer<ScenarioTransmitterPayloadType> MissionsDefaults_GenericTransmitter();
    QSharedPointer<ScenarioReceiverPayloadType> MissionsDefaults_GenericReceiver();
    QSharedPointer<ScenarioOpticalPayloadType> MissionsDefaults_GenericOpticalPayload();
    QSharedPointer<ScenarioRadarPayloadType> MissionsDefaults_GenericRadarPayload();

    // Generic loitering satellite
    ScenarioLoiteringType MissionsDefaults_GENERIC();

    // Generic loitering satellite
    ScenarioLoiteringTLEType MissionsDefaults_GENERIC_TLE();

    // Generic deltaV maneuver
    ScenarioDeltaVType MissionsDefaults_GENERIC_DELTAV();

    //ALONSO.-
    // Generic RendezVous Manoeuvres
    ScenarioRendezVousManoeuvreType MissionsDefaults_GENERIC_RENDEZVOUS();

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
    ScenarioLoiteringType MissionsDefaults_Meteosat7();
    ScenarioLoiteringType MissionsDefaults_Meteosat8();
    ScenarioLoiteringType MissionsDefaults_Meteosat9();
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
    ScenarioLoiteringType MissionsDefaults_Sentinel1A();
     ScenarioLoiteringType MissionsDefaults_Sentinel1B();
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
    ScenarioEntryArcType MissionsDefaults_ATV();

    // Launchers
    //ScenarioLoiteringType MissionsDefaults_VEGA();
    //ScenarioLoiteringType MissionsDefaults_Soyuz();
    //ScenarioLoiteringType MissionsDefaults_Ariane5();
    ScenarioEntryArcType MissionsDefaults_IXV();


    // List of allowable missions
    QList<QString> esaDefaultLoiteringMissions();
    QList<QString> nasaDefaultLoiteringMissions();
    // ... and so on with CNES, DLR, JAXA

    // List of allowable colors for mission arcs
    // Notice that those are already provided by Qt
    /*
    Qt::white;		// 3	 White (#ffffff)
    Qt::black;		//2	 Black (#000000)
    Qt::red;		//7	 Red (#ff0000)
    Qt::darkRed;	//13	 Dark red (#800000)
    Qt::green;		//8	 Green (#00ff00)
    Qt::darkGreen;	//14	 Dark green (#008000)
    Qt::blue;		//9	 Blue (#0000ff)
    Qt::darkBlue;	//15	 Dark blue (#000080)
    Qt::cyan;		//10	 Cyan (#00ffff)
    Qt::darkCyan;	//16	 Dark cyan (#008080)
    Qt::magenta;	//11	 Magenta (#ff00ff)
    Qt::darkMagenta;	//17	 Dark magenta (#800080)
    Qt::yellow;		//12	 Yellow (#ffff00)
    Qt::darkYellow;;	//18	 Dark yellow (#808000)
    Qt::gray;		//5	 Gray (#a0a0a4)
    Qt::darkGray;	//4	 Dark gray (#808080)
    Qt::lightGray;	//6	 Light gray (#c0c0c0)
    */

    QColor missionArcColorFromQt(QString colorLabel);

};

#endif // _MissionsDefaults_H_

