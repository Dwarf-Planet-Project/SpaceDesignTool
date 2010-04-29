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
#include <istream>
#include <fstream>
#include <QFile>
#include <QDebug>
#include <QTextStream>
#include <QDir>
#include <QProcess>
#include <string.h>
#include <stdlib.h>// for MAX_PATH
#include <iostream> // for cout and cin
#include <math.h>


class CommAnalysis
{
public:

    //CONSTRUCTORS

    CommAnalysis();

    CommAnalysis(ScenarioTransmitterPayloadType* ,ScenarioReceiverPayloadType* );

    //Default Destructor
    ~CommAnalysis();


    //FUNCTIONS

    void AntennaCalculation(); //This function shall be called in order to calculate either maximum gain, diameter or beamwidth and load it in the gui

private:
    ScenarioTransmitterPayloadType* m_transmitter;
    ScenarioReceiverPayloadType* m_receiver;


};

#endif // COMMANALYSIS_H
