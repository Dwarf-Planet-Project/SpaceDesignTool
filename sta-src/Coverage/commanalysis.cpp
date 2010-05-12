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

#include "commanalysis.h"
#include <QDebug>


//Definition of constructors, the default one contains a warning.

CommAnalysis::CommAnalysis()
{
qDebug()<< "Warning: CommAnalysis object created without any participant to make the calculations";
}


CommAnalysis::CommAnalysis(ScenarioTransmitterPayloadType *transmitter, ScenarioReceiverPayloadType *receiver, PropagatedScenario* propagatedScenario):m_transmitter(transmitter), m_receiver(receiver), m_propagatedScenario(propagatedScenario)
{
qDebug()<<"Now this function is working, let's try this and then try to see the values";
//This constructor probably should input ScenarioSC and ScenarioGE, then check if this participants have transmitters/receivers.... then we could overload another constructor for 2 SpaceCrafts...
}

//Default destructor

CommAnalysis::~CommAnalysis(){

}

double frequency=14.5E9;
double lightSpeed=SPEED_OF_LIGHT;
double Pi=3.141592;

////////////////////////////////////////////// FUNCTIONS ////////////////////////////////////////////////////////

void CommAnalysis::getRange(){ //This function is not woking... I don't know how to read the scenario...yet!

    foreach(SpaceObject*spaceObj,m_propagatedScenario->spaceObjects())
    {

        foreach(MissionArc*arc,spaceObj->mission())
        {
           sta::StateVector Vector[arc->trajectorySampleCount()];

           double Date[arc->trajectorySampleCount()];
           double MJDdate[arc->trajectorySampleCount()];

           sta::StateVector result;

           for(int i=0;i<arc->trajectorySampleCount();i++)
           {
               Vector[i]=arc->trajectorySample(i);
               Date[i]=arc->trajectorySampleTime(i);
               MJDdate[i]=Date[i]-2400000.5;

           }
           foreach(GroundObject*groundObj,m_propagatedScenario->groundObjects())
           {
               for(int i=0;i<arc->trajectorySampleCount();i++)
               {
                   qDebug()<<"The elevation angle in every time step for these participants is: "<<groundObj->elevationAngle(spaceObj,MJDdate[i]);
               }
           }

         }
    }




}


double CommAnalysis::FreeSpaceLoss(){

    //The inputs for this function will be the frequency and the range between both participants

    double freeSpaceLoss=0, freeSpaceLossDb;
    double range=2251496.627; //This need to be calculated...

    freeSpaceLoss=pow((4*Pi*range*frequency/lightSpeed), 2);  //This formula is defined to be in the denominator of the C/No when it is expressed in natural units
    freeSpaceLossDb=10*log10(freeSpaceLoss); //This value shall be preceded by a minus when it is in decibels

    qDebug()<< "The FSL is: -" << freeSpaceLossDb;

    return freeSpaceLossDb;
}

double CommAnalysis::OxygenSpecificAttenuation(){ //THE FREQUENCY HAS TO BE IN GHZ!!!!!!!

    double h=6000;//equivalent height of a uniform medium for oxygen calculations
    double oxSpecAtt;

    oxSpecAtt=((7.1/(pow(frequency,2)+0.36))+(4.5/(pow((frequency-57),2)+0.98)))*pow(frequency,2)/1000;  //dB/Km


    double oxSpecAttDb=10*log10(oxSpecAtt);

    qDebug() << "the oxygen specific attenuation is: " << oxSpecAttDb;

    return oxSpecAttDb;
}

