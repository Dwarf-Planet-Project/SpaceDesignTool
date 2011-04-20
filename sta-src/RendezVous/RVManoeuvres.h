#ifndef RVMANOEUVRES_H
#define RVMANOEUVRES_H

#include "QString"
#include "QDateTime"
#include "Astro-Core/stamath.h"
#include "RVconstants.h"
#include "Scenario/scenarioPropagator.h"
#include "Scenario/missionsDefaults.h"

#include "Loitering/loitering.h"

#include "Astro-Core/stamath.h"

#include "Main/mainwindow.h"
#include "Main/scenariotree.h"

#include "Scenario/scenario.h"

#include "Scenario/propagationfeedback.h"
#include "Scenario/scenarioPropagator.h"
#include "Main/propagatedscenario.h"


class RVManoeuvreDescriptor {

public:
    QString fullName;
    int descriptor;
    QDateTime initTime;
    QDateTime endTime;
    sta::StateVector* initPos;
    sta::StateVector* endPos;
    double deltaV;


    RVManoeuvreDescriptor();
    ~RVManoeuvreDescriptor();
    QString getName();
    void setDescriptor(int);
    int getDescriptor();
    void setAssociatedManoeuvre();


};

class RVManoeuvrePlan {

public:
    QList <RVManoeuvreDescriptor> plan;
    RVManoeuvrePlan();
    ~RVManoeuvrePlan();
};



#endif // RVMANOEUVRES_H
