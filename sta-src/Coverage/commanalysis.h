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
 ------------------ Author: Ricardo Noriega  ----------------------------------------------
 ------------------ email: ricardonor@gmail.com  ------------------------------------------

 */

#ifndef COMMANALYSIS_H
#define COMMANALYSIS_H

#include <Eigen/Core>
#include <Eigen/Geometry>
#include "Scenario/staschema.h"
#include <QFile>
#include <QDebug>
#include <QTextStream>
#include <QDir>
#include <QProcess>
#include <string.h>
#include <stdlib.h>// for MAX_PATH
#include <math.h>
#include <Main/propagatedscenario.h>
#include <Astro-Core/constants.h>
#include <Astro-Core/stacoordsys.h>
#include <Astro-Core/stabody.h>
#include <Locations/environmentdialog.h>

using namespace sta;

class CommAnalysis
{
public:

    //CONSTRUCTORS

    CommAnalysis();

    CommAnalysis(ScenarioTransmitterPayloadType* ,ScenarioReceiverPayloadType*, ScenarioGroundStationEnvironment* ,PropagatedScenario*, int, int, int, bool, bool);

    //Default Destructor
    ~CommAnalysis();
    //Public function to create the output of this module. It is the only public function in order to preserve the input data
    void CommReports();


private:

    ///////////////////////////// PRIVATE FUNCTIONS NOT TO BE USED OUTSIDE THIS CLASS /////////////////////////////////////
    
    QList<double> getDopplerShiftList(int numberOfRows);
    QList<double> getFreeSpaceLossList(int numberOfRows);
    double OxygenSpecificAttenuation(double frequency); //Here I pass the frequency as argument coz It has to be the groundStation frequency...I don't know how to get the parent object yet!
    double WaterVapourSpecificAttenuation(double frequency, double latitude, double longitude);
    QList<double> getAtmosphericAttenuation(double frequency, double wpSpecAtt, double oxSpecAtt, int numberOfRows);
    QList<double> getRainAttenuation(double latitude, double longitude, int numberOfRows);
    double SystemTempCalculations();
    double Modulations(double EbNo);

    ///////////////////////////// PRIVATE MEMBERS NOT TO BE USED OUTSIDE THIS CLASS /////////////////////////////////////
    ScenarioTransmitterPayloadType* m_transmitter;
    ScenarioReceiverPayloadType* m_receiver;
    PropagatedScenario* m_propagatedScenario;
    ScenarioGroundStationEnvironment* m_environment;

    int m_indexSC;
    int m_indexGS;
    int m_indexMA;
    bool m_flagTX; //If this flag is 1 means that the transmitter is on board a spacecraft, if it is 0, on a ground station
    bool m_flagRX; //If this flag is 1 means that the receiver is on board a spacecraft, if it is 0, on a ground station

    QList<double> elevationAngleList;//in radians
    QList<double> rangeList;//in metres
    QList<double> dopplerList;
    QList<double> freeSpaceLossList;//in dB
    QList<double> oxAttList;//in dB
    QList<double> wvAttList;//in dB
    QList<double> mjdList;//in mjd
    QList<double> atmosphericAttList;//in dB
    QList<double> rainAttenuationList;//in dB
    QList<double> antennaTempList;//in K
    QList<double> systemTempList;//in K
    QList<double> carrierToNoiseDensityList;//in dB

    QString antennaTempChoice;
    QString oxcheck;
    QString wvcheck;
    QString raincheck;

};

#endif // COMMANALYSIS_H
