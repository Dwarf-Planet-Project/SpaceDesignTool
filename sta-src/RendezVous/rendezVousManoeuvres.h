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
 ------ Copyright (C) 2011 STA Steering Board (space.trajectory.analysis AT gmail.com) ----

 */

/*
 ------------------ Author: J. Alonso     -------------------------------------------------
 ------------------ ETSIA                 ----------------------------
 Comments:
 Manoeuvre auxiliar descriptors. Storage manoeuvres data.
 */

#ifndef _RENDEZVOUSMANOEUVRES_H_
#define _RENDEZVOUSMANOEUVRES_H_

#include "QString"
#include "QDateTime"
#include "Eigen/Eigen"

#include "rendezVousConstants.h"
#include "cmath"

#include "Scenario/scenario.h"
#include "Scenario/propagationfeedback.h"
#include "Scenario/scenario.h"
#include "Scenario/missionAspectDialog.h"


#include "Main/scenariotree.h"
#include "Main/propagatedscenario.h"

#include "Astro-Core/stamath.h"
#include "Astro-Core/stacoordsys.h"
#include "Astro-Core/date.h"
#include "Astro-Core/stacoordsys.h"
#include "Astro-Core/stabody.h"
#include "Astro-Core/trueAnomalyTOmeanAnomaly.h"
#include "Astro-Core/cartesianTOorbital.h"
#include "Astro-Core/orbitalTOcartesian.h"
#include "Astro-Core/eci2lhlv.h"


static const unsigned int MAX_OUTPUT_STEPS = 1000000;




/******************************************************************

Manoeuvres.-
  Each one of these objects are groups of RV manoeuvres.
  Objective of each one of this classes is calculate deltaV, according
  to orbit data. This is done in interfXML, set manoeuvre.

 *****************************************************************/

/*-------------------------------------------------------------
   TBTtangential.- Includes TBT_ALONG_X_AXIS,TBT_HOHMANN_TRANSFER,
   TBT_90_DEGREES_FLY_AROUND
  --------------------------------------------------------------*/

class TBTtangential{
    int designator;
    double deltaV1;
    double deltaV2;
    double totalDeltaV;
    double myDurationInSecs;
    sta::StateVector initialState;
    sta::StateVector finalState;

public:
    void setTangential(int,double,double,sta::StateVector,double);
    double getDeltaV1();
    double getDeltaV2();
    double getTotalDeltaV();
    double getDurationInSecs();

    sta::StateVector getFinalState();
};


/*---------------------------------------------------------------
 TBTradial.- Includes RBT_ALONG_X_AXIS,RBT_90_DEGREES_FLY_AROUND,
 RBT_360_DEGREES_FLY_AROUND
  -------------------------------------------------------------*/

class TBTradial{
    int designator;
    double deltaV1;
    double deltaV2;
    double totalDeltaV;
    double myDurationInSecs;
    sta::StateVector initialState;
    sta::StateVector finalState;

public:

    void setRadial(int,double,double,sta::StateVector,double);

    double getDeltaV1();
    double getDeltaV2();
    double getTotalDeltaV();
    double getDurationInSecs();

   sta::StateVector getFinalState();


};

/*---------------------------------------------------------------
 CFTtransfer.- Includes HOLD_POINTS_VBAR,SLT_VBAR,SLT_RBAR,
    CFT_TANGENTIAL, CFT_RADIAL
  -------------------------------------------------------------*/

class CFTtransfer{
    int designator;
    double deltaV1;
    double deltaV2;
    double totalDeltaV;
    double constantForce;
    double myDurationInSecs;
    sta::StateVector initialState;
    sta::StateVector finalState;
    QList <Vector3d> accelList;

public:
    void setTransfer(int,double,double,sta::StateVector,double,double);

    double getDeltaV1();
    double getDeltaV2();
    double getTotalDeltaV();
    double getDurationInSecs();
    double getConstantForce();

    sta::StateVector getFinalState();
    QList<Vector3d>& getAccelList();


};



/**********************************************************************

    RVmanoeuvreDescriptor.- manPlans are made of this. A descriptor is
            not a manoeuvre. It's something a manoeuvre should complete.
            It's a nutshell, an empty space common for all manoeuvres.
            Previous classes do the math and fill this.

  *********************************************************************/


class RVmanoeuvreDescriptor {

public:
    QString fullName;
    int descriptor;
    QDateTime initTime;
    QDateTime endTime;
    sta::StateVector initPosVel;
    sta::StateVector endPosVel;

    double durationInSecs;
    double distance;
    double inputRV;
    double totalDeltaV;
    double deltaV1;
    double deltaV2;

    QString getName();
    void setName(QString);
    void setDescriptor(int);
    int getDescriptor();
    void setDistance(double);
    double getDistance();
    void setDurationInSecs(double);
    double getDurationInSecs();
    void setRVinput(double);
    double getRVinput();

    sta::StateVector getInitPosVel();
    void setInitPosVel(sta::StateVector*);

    sta::StateVector getEndPosVel();
    void setEndPosVel(sta::StateVector*);

    void setTotalDeltaV(double);
    void setDeltaV1(double);
    void setDeltaV2(double);
    double getTotalDeltaV();
    double getDeltaV1();
    double getDeltaV2();

};


class RVmanoeuvrePlan {

public:
    QList <RVmanoeuvreDescriptor> plan;

};



/******************************************************************
  CWPropagator.
  This class allows propagation of trajectory for RV manoeuvres
  under CW model.

******************************************************************/

class CWpropagator{

public:

    QList<sta::StateVector> results;
    QList<double> sampleTimes;

    double initialStateVector[9];
    double stepSize;
    double targetOmega;
    double initTime;
    double endTime;

    CWpropagator(double,double*,double,double);
    ~CWpropagator();
    void solverCW(double);

    QList<sta::StateVector>& getResults();
    QList<double>& getSampleTimes();

};

// In order to get constants accelerations at each moment, needed.
//Possible aplication to perturbed case. STL Rbar needed anyway.

class CWperturbedPropagator{

public:

    QList<sta::StateVector> results;
    QList<double> sampleTimes;


    QList<Vector3d> accelList;

    double initialStateVector[9];
    double stepSize;
    double targetOmega;
    double initTime;
    double endTime;

    CWperturbedPropagator(double,double*,QList<Vector3d>,double,double);
    ~CWperturbedPropagator();

    void solverCW(double);

    QList<sta::StateVector>& getResults();

    QList<double>& getSampleTimes();


};

///Independent procedure. BAD. We are not in the 90's
//If this is STA style, it should be better

extern bool
PropagateRendezVousTrajectory(ScenarioRendezVousManoeuvreType* rvManoeuvre,                             
                             QList<double>& sampleTimes,
                             QList<sta::StateVector>& samples,
                             PropagationFeedback& propFeedback,
                             PropagatedScenario* propScen);




#endif // _RENDEZVOUSMANOEUVRES_H_
