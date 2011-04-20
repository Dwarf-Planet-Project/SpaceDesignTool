#include "RVManoeuvres.h"





RVManoeuvreDescriptor::RVManoeuvreDescriptor(){}
RVManoeuvreDescriptor::~RVManoeuvreDescriptor(){}




void RVManoeuvreDescriptor::setAssociatedManoeuvre(){


}

QString RVManoeuvreDescriptor::getName(){
    return fullName;
}

int RVManoeuvreDescriptor::getDescriptor(){
    return descriptor;
}

//ManoeuvrePlan

RVManoeuvrePlan::RVManoeuvrePlan(){}
RVManoeuvrePlan::~RVManoeuvrePlan(){}
