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

#include "Scenario/scenario.h"

#include "Astro-Core/stamath.h"
#include "Astro-Core/stacoordsys.h"
#include "Astro-Core/stabody.h"

#include "cmath"

#include "Scenario/propagationfeedback.h"


#include "Main/scenariotree.h"
#include "Scenario/scenario.h"
#include "Astro-Core/date.h"
#include "Astro-Core/stacoordsys.h"
#include "Astro-Core/stabody.h"
#include "Astro-Core/trueAnomalyTOmeanAnomaly.h"
#include "Astro-Core/cartesianTOorbital.h"
#include "Astro-Core/orbitalTOcartesian.h"
#include "Scenario/missionAspectDialog.h"


static const unsigned int MAX_OUTPUT_STEPS = 1000000;


class GenericRendezVousManoeuvreDialog;


/******************************************************************
  Manoeuvres.-
  Each of these objects are groups of RV manoeuvres
  Objective of each of this classes is calculate deltaV, according
  to orbit data, and calculate trajectory in LHLV coordinate system

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
    ScenarioStateVectorType initialState;
    ScenarioStateVectorType finalState;

public:
    //TBTtangential();
    //~TBTtangential();

    void setTangential(int,double,double,ScenarioStateVectorType,double);

    double getDeltaV1();
    double getDeltaV2();
    double getTotalDeltaV();
    double getDurationInSecs();

   ScenarioStateVectorType getFinalState();


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
    ScenarioStateVectorType initialState;
    ScenarioStateVectorType finalState;

public:
    //TBTradial();
    //~TBTradial();

    void setRadial(int,double,double,ScenarioStateVectorType,double);

    double getDeltaV1();
    double getDeltaV2();
    double getTotalDeltaV();
    double getDurationInSecs();

   ScenarioStateVectorType getFinalState();


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
    ScenarioStateVectorType initialState;
    ScenarioStateVectorType finalState;

public:
    //TBTtransfer();
    //~TBTtransfer();

    void setTransfer(int,double,double,ScenarioStateVectorType,double);

    double getDeltaV1();
    double getDeltaV2();
    double getTotalDeltaV();
    double getDurationInSecs();
    double getConstantForce();

    ScenarioStateVectorType getFinalState();

};






/******************************************************************

    RVManoeuvreDescriptor.- this class handles interaction between
    tables and actual manoeuvres in code

  *****************************************************************/


class RVManoeuvreDescriptor {

public:
    QString fullName;
    int descriptor;
    QDateTime initTime;
    QDateTime endTime;
    ScenarioStateVectorType initPosVel;
    ScenarioStateVectorType endPosVel;

    double durationInSecs;
    double distance;
    double inputRV;

   // GenericRendezVousManoeuvreDialog* myDialog;

    //Every descriptor has all kind of manoeuvre ready in order to be set
    TBTtangential* tangential;
    TBTradial*         radial;
    CFTtransfer*     cftTrans;

    //RVManoeuvreDescriptor();
    //~RVManoeuvreDescriptor();
    QString getName();
    void setDescriptor(int);
    int getDescriptor();
    void setAssociatedManoeuvre(double,double,ScenarioStateVectorType,double);
    void setDistance(double);
    double getDistance();

    double getDurationInSecs();
    void setRVinput(double);
    double getRVinput();

    ScenarioStateVectorType getInitPosVel();
    void setInitPosVel(ScenarioStateVectorType*);

    ScenarioStateVectorType getEndPosVel();
    void setEndPosVel(ScenarioStateVectorType*);

};


class RVManoeuvrePlan {

public:
    QList <RVManoeuvreDescriptor> plan;
    //RVManoeuvrePlan();
    //~RVManoeuvrePlan();
};



/******************************************************************
  CWPropagator.
  This class allows propagation of trajectory for RV manoeuvres
  under CW model.

******************************************************************/

class CWpropagator{

public:

    QList<ScenarioStateVectorType> results;
    QList<double> sampleTimes;

    double initialStateVector[9];
    double stepSize;
    double targetOmega;
    double initTime;
    double endTime;

    CWpropagator(double,double*,double,double);
    ~CWpropagator();
    void solverCW(double);

    QList<ScenarioStateVectorType>& getResults();
    QList<double>& getSampleTimes();

};

// In order to get constants accelerations at each moment, needed.
//Possible aplication to perturbed case. STL Rbar needed anyway.

class CWperturbedPropagator{

public:

    QList<ScenarioStateVectorType> results;
    QList<double> sampleTimes;

    //9 places for 3 accelerations. Optimization required.
    QList<Eigen::Vector3d> accelList;

    double initialStateVector[9];
    double stepSize;
    double targetOmega;
    double initTime;
    double endTime;

    CWperturbedPropagator(double,double*,QList<Eigen::Vector3d>,double,double);
    ~CWperturbedPropagator();

    void solverCW(double);

    QList<ScenarioStateVectorType>& getResults();

    QList<double>& getSampleTimes();


};

///Independent procedure. BAD. We are not in the 90's
//If this is STA style, it should be better

extern bool
PropagateRendezVousTrajectory(ScenarioRendezVousManoeuvreType* rvManoeuvre,                             
                             QList<double>& sampleTimes,
                             QList<ScenarioStateVectorType>& samples,
                             PropagationFeedback& propFeedback);




#endif // _RENDEZVOUSMANOEUVRES_H_
