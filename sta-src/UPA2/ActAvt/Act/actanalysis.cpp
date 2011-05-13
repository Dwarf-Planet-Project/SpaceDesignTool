//- Copyright (C) 2010 STA Steering Board (space.trajectory.analysis AT gmail.com) -
//
// This program is free software; you can redistribute it and/or modify it under
// the terms of the European Union Public License version 1.1
//
// This program is distributed in the hope that it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
// FOR A PARTICULAR PURPOSE.  See the European Union Public License for more
// details.
//
//
// Further information about the European Union Public License can also be found on
// the world wide web at http://ec.europa.eu/idabc/eupl
//
//------------------ Authors:     Mauro Massari                  -------------------
//------------------              Roberto Armellin               -------------------
//------------------ Affiliation: Dinamica Srl                   -------------------
//------------------ E-mail:      (massari AT dinamicatech.com)  -------------------
//------------------              (armellin AT dinamicatech.com) -------------------

#include "actanalysis.h"

using namespace act;

ActAnalysis::ActAnalysis(SpaceScenario* scenario, PropagatedScenario* propscenario) :
        m_scenario(scenario),
        m_propscenario(propscenario),
        m_initialized(false),
        m_propagated(false),
        m_stmcomputed(false),
        m_coordinatetype(sta::COORDSYS_EME_J2000),
        m_elementtype(ELEM_CARTESIAN),
        m_currentspaceobject(0),
        m_currentarc(0),
        m_variations(1),
        m_SC(false),
        m_REV(false),
        m_linear(true)
{

    ActCovariance cov;
    QList<ActCovariance> container;

    MatrixXd matMC(1,6);
    QList<MatrixXd> containerMC;

    MatrixXd mat(6,6);
    MatrixXd mat2(1,6);
    // initialization of internal data required to access scenario data

    if(dynamic_cast<ScenarioSC*>(m_scenario->AbstractParticipant().at(m_currentspaceobject).data()) != NULL)
    {
        m_vehicle = dynamic_cast<ScenarioSC*>(m_scenario->AbstractParticipant().at(m_currentspaceobject).data());
        m_trajectory = m_vehicle->SCMission()->TrajectoryPlan()->AbstractTrajectory().at(m_currentarc);
        m_propvehicle = m_propscenario->spaceObjects().at(m_currentspaceobject);
        m_proptrajectory = m_propvehicle->mission().at(m_currentarc);
        m_SC=true;

    }

    if(dynamic_cast<ScenarioREV*>(m_scenario->AbstractParticipant().at(m_currentspaceobject).data()) != NULL)
    {
        m_entryvehicle = dynamic_cast<ScenarioREV*>(m_scenario->AbstractParticipant().at(m_currentspaceobject).data());
        m_trajectory = m_entryvehicle->REVMission()->REVTrajectoryPlan()->AbstractTrajectory().at(m_currentarc);
        m_propvehicle = m_propscenario->spaceObjects().at(m_currentspaceobject);
        m_proptrajectory = m_propvehicle->mission().at(m_currentarc);
        m_REV=true;
    }

    // initialize time steps with initial and final time
    m_timesteps.insert(0, m_propscenario->spaceObjects().at(m_currentspaceobject)->mission().at(m_currentarc)->beginning());
    m_timesteps.insert(m_timesteps.size(), m_propscenario->spaceObjects().at(m_currentspaceobject)->mission().at(m_currentarc)->ending());

    // initialize covariances and montecarlo storage at initial and final time
    container.insert(0, cov);
    container.insert(m_covariance.size(), cov);
    containerMC.insert(0, matMC);


    // initialize covariances and montecarlo storage variations (only one as default)
    m_covariance.insert(0, container);
    m_montecarlo.insert(0, containerMC);

    // set centralbody and mjd for initial and final covariance
    m_covariance[0][0].setCentralBody(m_proptrajectory->centralBody());
    m_covariance[0].last().setCentralBody(m_proptrajectory->centralBody());

    m_covariance[0][0].setMJD(m_timesteps[0]);
    m_covariance[0].last().setMJD(m_timesteps.last());

    // initialize STM at initial and final time
    m_STM.insert(0, mat);
    m_STM.insert(m_STM.size(), mat);

    // initialize mean at initial and final time
    m_mean.insert(0, mat2);
    m_mean.insert(m_mean.size(), mat2);



}

ActAnalysis::~ActAnalysis()
{

}

int ActAnalysis::setCurrentSpaceObject(int index)
{
    m_currentspaceobject = index;

    if(dynamic_cast<ScenarioSC*>(m_scenario->AbstractParticipant().at(m_currentspaceobject).data()) != NULL)
    {
        m_vehicle = dynamic_cast<ScenarioSC*>(m_scenario->AbstractParticipant().at(m_currentspaceobject).data());
        m_trajectory = m_vehicle->SCMission()->TrajectoryPlan()->AbstractTrajectory().at(m_currentarc);
        m_propvehicle = m_propscenario->spaceObjects().at(m_currentspaceobject);
        m_proptrajectory = m_propvehicle->mission().at(m_currentarc);
        m_SC=true;

    }

    if(dynamic_cast<ScenarioREV*>(m_scenario->AbstractParticipant().at(m_currentspaceobject).data()) != NULL)
    {
        m_entryvehicle = dynamic_cast<ScenarioREV*>(m_scenario->AbstractParticipant().at(m_currentspaceobject).data());
        m_trajectory = m_entryvehicle->REVMission()->REVTrajectoryPlan()->AbstractTrajectory().at(m_currentarc);
        m_propvehicle = m_propscenario->spaceObjects().at(m_currentspaceobject);
        m_proptrajectory = m_propvehicle->mission().at(m_currentarc);
        m_REV=true;
    }

    int i,j;

    for(j=0;j<m_covariance.size();j++)
    {
        for(i=0; i < m_timesteps.size(); i++)
        {
            m_covariance[j][i].setCentralBody(m_proptrajectory->centralBody());
            m_covariance[j][i].setMJD(m_timesteps[i]);
        }

    }
    m_initialized = false;
    m_stmcomputed = false;

    return true;
}

int ActAnalysis::setCurrentArc(int index)
{
    m_currentarc = index;

    /*if(dynamic_cast<ScenarioSC*>(m_scenario->AbstractParticipant().at(m_currentspaceobject).data()) != NULL)
    {
        m_vehicle = dynamic_cast<ScenarioSC*>(m_scenario->AbstractParticipant().at(m_currentspaceobject).data());
        m_trajectory = m_vehicle->SCMission()->TrajectoryPlan()->AbstractTrajectory();
        m_propvehicle = m_propscenario->spaceObjects().at(m_currentspaceobject);
        m_proptrajectory = m_propvehicle->mission();
        m_SC=true;

    }

    if(dynamic_cast<ScenarioREV*>(m_scenario->AbstractParticipant().at(m_currentspaceobject).data()) != NULL)
    {
        m_entryvehicle = dynamic_cast<ScenarioREV*>(m_scenario->AbstractParticipant().at(m_currentspaceobject).data());
        m_trajectory = m_entryvehicle->REVMission()->REVTrajectoryPlan()->AbstractTrajectory();
        m_propvehicle = m_propscenario->spaceObjects().at(m_currentspaceobject);
        m_proptrajectory = m_propvehicle->mission();
        m_REV=true;
    }*/

    int i,j;

    for(j=0;j<m_covariance.size();j++)
    {
        for(i=0; i < m_timesteps.size(); i++)
        {
            m_covariance[j][i].setCentralBody(m_proptrajectory->centralBody());
            m_covariance[j][i].setMJD(m_timesteps[i]);
        }

    }
    m_initialized = false;
    m_stmcomputed = false;

    return true;
}

int ActAnalysis::setTimeSteps(QList<double> timestepsin)
{
    m_timesteps = timestepsin;
    int j;

    for(j=0;j<m_covariance.size();j++)
    {
        m_covariance[j].clear();
    }


    m_STM.clear();
    m_mean.clear();
    ActCovariance cov;
    MatrixXd mat(6,6);
    MatrixXd mat2(1,6);


    foreach(double step, m_timesteps)
    {
        for(j=0;j<m_covariance.size();j++)
        {
            m_covariance[j].insert(m_covariance[j].size(),cov);
            m_covariance[j].last().setCentralBody(m_proptrajectory->centralBody());
            m_covariance[j].last().setMJD(step);
        }

        m_STM.insert(m_STM.size(), mat);
        m_mean.insert(m_mean.size(), mat2);
    }
    m_initialized = false;
    m_stmcomputed = false;

    return true;
}

int ActAnalysis::setInitialCovarianceNumber(int number)
{
    ActCovariance cov;
    QList<ActCovariance> container;

    MatrixXd matMC(1,6);
    QList<MatrixXd> containerMC;


    foreach(double step, m_timesteps)
    {
        container.insert(container.size(),cov);
        container.last().setCentralBody(m_proptrajectory->centralBody());
        container.last().setMJD(step);
    }
    containerMC.insert(0, matMC);

    if(number < m_covariance.size())
    {
        while(m_covariance.size() > number && m_covariance.size() > 1)
        {
            m_covariance.removeLast();
            m_variations = m_covariance.size();
        }
    }
    else
    {
        while(m_covariance.size() < number)  // if size = number  there is no need of append.
        {
            m_covariance.append(container);
            m_variations = m_covariance.size();
        }
    }

    if(number < m_montecarlo.size())
    {
        while(m_montecarlo.size() > number && m_montecarlo.size() > 1)
        {
            m_montecarlo.removeLast();

        }
    }
    else
    {
        while(m_montecarlo.size() < number)  // if size = number  there is no need of append.
        {
            m_montecarlo.append(containerMC);

        }
    }



    m_initialized = false;

    return 0;
}

int ActAnalysis::InitializeCov(MatrixXd Cov, sta::CoordinateSystemType coordType, ActElementType elemType, int index)
{

    // get mean from scenario at time t0
    int NumSamples = m_proptrajectory->trajectorySampleCount();
    QList<double> TimeSamples;
    QList<int> TimeStepsIdx;

    int i,j;
    for (i=0;i < NumSamples; i++)
    {
        TimeSamples.append(m_proptrajectory->trajectorySampleTime(i));
    }

    sta::StateVector TmpVecJ2000 = m_proptrajectory->trajectorySample(TimeSamples.indexOf(m_timesteps[0]));

    // convert mean reference system

    sta::StateVector TmpVec;
    sta::CoordinateSystem reference(coordType);

    if(reference.isInertial() && (elemType == act::ELEM_CARTESIAN || elemType == act::ELEM_KEPLERIAN))
    {
        TmpVec = reference.fromEmeJ2000(TmpVecJ2000);
    }
    else if(coordType == sta::COORDSYS_BODYFIXED && (elemType == act::ELEM_CARTESIAN || elemType == act::ELEM_SPHERICAL))
    {
        TmpVec = reference.fromEmeJ2000(TmpVecJ2000, m_proptrajectory->centralBody(), m_timesteps[0]);
    }
    else if(coordType == sta::COORDSYS_ROT && elemType == act::ELEM_CARTESIAN)
    {
        // TODO
        QMessageBox msgBox;
        msgBox.setText("The ActAnalysis: In InitializeCov() member function the Coordinate system is incompatible with the element type.");
        msgBox.setWindowTitle("ACT");
        msgBox.setIcon(QMessageBox::Warning);
        msgBox.exec();
        m_initialized = false;
        return false;
    }
    else if(coordType == sta::COORDSYS_ROT_NORM && elemType == act::ELEM_CARTESIAN)
    {
        // TODO
        QMessageBox msgBox;
        msgBox.setText("The ActAnalysis: In InitializeCov() member function the Coordinate system is incompatible with the element type.");
        msgBox.setWindowTitle("ACT");
        msgBox.setIcon(QMessageBox::Warning);
        msgBox.exec();
        m_initialized = false;
        return false;
    }
    else // wrong combination of reference and element type
    {
        QMessageBox msgBox;
        msgBox.setText("The ActAnalysis: In InitializeCov() member function the Coordinate system is incompatible with the element type.");
        msgBox.setWindowTitle("ACT");
        msgBox.setIcon(QMessageBox::Warning);
        msgBox.exec();
        m_initialized = false;
        return false;
    }

    MatrixXd meanin(1,6);
    if(elemType == act::ELEM_KEPLERIAN)
    {
        // convert to keplerian
        sta::KeplerianElements TmpKep = cartesianTOorbital(m_proptrajectory->centralBody()->mu(), TmpVec);
        meanin(0,0) = TmpKep.SemimajorAxis;
        meanin(0,1) = TmpKep.Eccentricity;
        meanin(0,2) = TmpKep.Inclination;
        meanin(0,3) = TmpKep.AscendingNode;
        meanin(0,4) = TmpKep.ArgumentOfPeriapsis;
        meanin(0,5) = TmpKep.MeanAnomaly;

    }
    else if(elemType == act::ELEM_SPHERICAL)
    {

        // convert to spherical
        double r,v,tau,delta,gamma,chi;

        cartesianTOspherical(TmpVec.position(0),TmpVec.position(1),TmpVec.position(2),TmpVec.velocity(0),TmpVec.velocity(1),TmpVec.velocity(2), tau, delta, r, v, gamma, chi);
        meanin(0,0) = r;
        meanin(0,1) = delta;
        meanin(0,2) = tau;
        meanin(0,3) = v;
        meanin(0,4) = gamma;
        meanin(0,5) = chi;


    }
    else // elemType == act::ELEM_CARTESIAN
    {
        for (i=0;i<3;i++)
        {
            meanin(0,i) = TmpVec.position(i);
            meanin(0,i+3) = TmpVec.velocity(i);
        }
    }

    // set coordinatetype and elementtype;
    setCoordinateType(coordType);
    setElementType(elemType);



    // Initialize initial covariance
    if(isLinear())
    {
        m_covariance[index][0].InitializeCov(meanin,Cov,coordType,elemType);
    }
    else
    {
        m_covariance[index][0].InitializeNonLinearCov(meanin,Cov,coordType,elemType);
    }


    m_initialized = true;
    for(j=0;j<m_covariance.size();j++)
    {
        if(!m_covariance[j][0].isInitialized())
        {
            m_initialized = false;
            // return false;
        }
    }


    return true;
}

int ActAnalysis::InitializeRMS(MatrixXd RMSValues, sta::CoordinateSystemType coordType, ActElementType elemType, int index)
{
    // get mean from scenario at time t0
    int NumSamples = m_proptrajectory->trajectorySampleCount();
    QList<double> TimeSamples;
    QList<int> TimeStepsIdx;

    int i,j;

    for (i=0;i < NumSamples; i++)
    {
        TimeSamples.append(m_proptrajectory->trajectorySampleTime(i));
    }

    sta::StateVector TmpVecJ2000 = m_proptrajectory->trajectorySample(TimeSamples.indexOf(m_timesteps[0]));

    // convert mean reference system

    sta::StateVector TmpVec;
    sta::CoordinateSystem reference(coordType);

    if(reference.isInertial() && (elemType == act::ELEM_CARTESIAN || elemType == act::ELEM_KEPLERIAN))
    {
        TmpVec = reference.fromEmeJ2000(TmpVecJ2000);
    }
    else if(coordType == sta::COORDSYS_BODYFIXED && (elemType == act::ELEM_CARTESIAN || elemType == act::ELEM_SPHERICAL))
    {
        TmpVec = reference.fromEmeJ2000(TmpVecJ2000, m_proptrajectory->centralBody(), m_timesteps[0]);
    }
    else if(coordType == sta::COORDSYS_ROT && elemType == act::ELEM_CARTESIAN)
    {
        // TODO
        QMessageBox msgBox;
        msgBox.setText("The ActAnalysis: In InitializeCov() member function the Coordinate system is incompatible with the element type.");
        msgBox.setWindowTitle("ACT");
        msgBox.setIcon(QMessageBox::Warning);
        msgBox.exec();
        m_initialized = false;
        return false;
    }
    else if(coordType == sta::COORDSYS_ROT_NORM && elemType == act::ELEM_CARTESIAN)
    {
        // TODO
        QMessageBox msgBox;
        msgBox.setText("The ActAnalysis: In InitializeCov() member function the Coordinate system is incompatible with the element type.");
        msgBox.setWindowTitle("ACT");
        msgBox.setIcon(QMessageBox::Warning);
        msgBox.exec();
        m_initialized = false;
        return false;
    }
    else // wrong combination of reference and element type
    {
        QMessageBox msgBox;
        msgBox.setText("The ActAnalysis: In InitializeCov() member function the Coordinate system is incompatible with the element type.");
        msgBox.setWindowTitle("ACT");
        msgBox.setIcon(QMessageBox::Warning);
        msgBox.exec();
        m_initialized = false;
        return false;
    }

    MatrixXd meanin(1,6);
    if(elemType == act::ELEM_KEPLERIAN)
    {
        // convert to keplerian
        sta::KeplerianElements TmpKep = cartesianTOorbital(m_proptrajectory->centralBody()->mu(), TmpVec);
        meanin(0,0) = TmpKep.SemimajorAxis;
        meanin(0,1) = TmpKep.Eccentricity;
        meanin(0,2) = TmpKep.Inclination;
        meanin(0,3) = TmpKep.AscendingNode;
        meanin(0,4) = TmpKep.ArgumentOfPeriapsis;
        meanin(0,5) = TmpKep.MeanAnomaly;

    }
    else if(elemType == act::ELEM_SPHERICAL)
    {

        // convert to spherical
        double r,v,tau,delta,gamma,chi;

        cartesianTOspherical(TmpVec.position(0),TmpVec.position(1),TmpVec.position(2),TmpVec.velocity(0),TmpVec.velocity(1),TmpVec.velocity(2), tau, delta, r, v, gamma, chi);
        meanin(0,0) = r;
        meanin(0,1) = delta;
        meanin(0,2) = tau;
        meanin(0,3) = v;
        meanin(0,4) = gamma;
        meanin(0,5) = chi;


    }
    else // elemType == act::ELEM_CARTESIAN
    {
        for (int i=0;i<3;i++)
        {
            meanin(0,i) = TmpVec.position(i);
            meanin(0,i+3) = TmpVec.velocity(i);
        }
    }

    // set coordinatetype and element type;
    setCoordinateType(coordType);
    setElementType(elemType);


    // Initialize initial covariance
    if(isLinear())
    {
        m_covariance[index][0].InitializeRMS(meanin,RMSValues,coordType,elemType);
    }
    else
    {
        m_covariance[index][0].InitializeNonLinearRMS(meanin,RMSValues,coordType,elemType);
    }


    m_initialized = true;
    for(j=0;j<m_covariance.size();j++)
    {
        if(!m_covariance[j][0].isInitialized())
        {
            m_initialized = false;
            //return false;
        }
    }
    return true;
}

int ActAnalysis::InitializeSamples(MatrixXd Samples, sta::CoordinateSystemType coordType, ActElementType elemType,int index)
{

    // get mean from scenario at time t0
    int NumSamples = m_proptrajectory->trajectorySampleCount();
    QList<double> TimeSamples;
    QList<int> TimeStepsIdx;

    int i,j;

    for (i=0;i < NumSamples; i++)
    {
        TimeSamples.append(m_proptrajectory->trajectorySampleTime(i));
    }

    sta::StateVector TmpVecJ2000 = m_proptrajectory->trajectorySample(TimeSamples.indexOf(m_timesteps[0]));

    // convert mean reference system

    sta::StateVector TmpVec;
    sta::CoordinateSystem reference(coordType);

    if(reference.isInertial() && (elemType == act::ELEM_CARTESIAN || elemType == act::ELEM_KEPLERIAN))
    {
        TmpVec = reference.fromEmeJ2000(TmpVecJ2000);
    }
    else if(coordType == sta::COORDSYS_BODYFIXED && (elemType == act::ELEM_CARTESIAN || elemType == act::ELEM_SPHERICAL))
    {
        TmpVec = reference.fromEmeJ2000(TmpVecJ2000, m_proptrajectory->centralBody(), m_timesteps[0]);
    }
    else if(coordType == sta::COORDSYS_ROT && elemType == act::ELEM_CARTESIAN)
    {
        // TODO
        QMessageBox msgBox;
        msgBox.setText("The ActAnalysis: In InitializeCov() member function the Coordinate system is incompatible with the element type.");
        msgBox.setWindowTitle("ACT");
        msgBox.setIcon(QMessageBox::Warning);
        msgBox.exec();
        m_initialized = false;
        return false;
    }
    else if(coordType == sta::COORDSYS_ROT_NORM && elemType == act::ELEM_CARTESIAN)
    {
        // TODO
        QMessageBox msgBox;
        msgBox.setText("The ActAnalysis: In InitializeCov() member function the Coordinate system is incompatible with the element type.");
        msgBox.setWindowTitle("ACT");
        msgBox.setIcon(QMessageBox::Warning);
        msgBox.exec();
        m_initialized = false;
        return false;
    }
    else // wrong combination of reference and element type
    {
        QMessageBox msgBox;
        msgBox.setText("The ActAnalysis: In InitializeCov() member function the Coordinate system is incompatible with the element type.");
        msgBox.setWindowTitle("ACT");
        msgBox.setIcon(QMessageBox::Warning);
        msgBox.exec();
        m_initialized = false;
        return false;
    }

    MatrixXd meanin(1,6);
    if(elemType == act::ELEM_KEPLERIAN)
    {
        // convert to keplerian
        sta::KeplerianElements TmpKep = cartesianTOorbital(m_proptrajectory->centralBody()->mu(), TmpVec);
        meanin(0,0) = TmpKep.SemimajorAxis;
        meanin(0,1) = TmpKep.Eccentricity;
        meanin(0,2) = TmpKep.Inclination;
        meanin(0,3) = TmpKep.AscendingNode;
        meanin(0,4) = TmpKep.ArgumentOfPeriapsis;
        meanin(0,5) = TmpKep.MeanAnomaly;

    }
    else if(elemType == act::ELEM_SPHERICAL)
    {

        // convert to spherical
        double r,v,tau,delta,gamma,chi;

        cartesianTOspherical(TmpVec.position(0),TmpVec.position(1),TmpVec.position(2),TmpVec.velocity(0),TmpVec.velocity(1),TmpVec.velocity(2), tau, delta, r, v, gamma, chi);
        meanin(0,0) = r;
        meanin(0,1) = delta;
        meanin(0,2) = tau;
        meanin(0,3) = v;
        meanin(0,4) = gamma;
        meanin(0,5) = chi;


    }
    else // elemType == act::ELEM_CARTESIAN
    {
        for (int i=0;i<3;i++)
        {
            meanin(0,i) = TmpVec.position(i);
            meanin(0,i+3) = TmpVec.velocity(i);
        }
    }

    // set coordinatetype and elementtype;
    setCoordinateType(coordType);
    setElementType(elemType);


    // Initialize initial covariance
    if(isLinear())
    {
        m_covariance[index][0].InitializeSamples(Samples, coordType, elemType);
    }
    else
    {
        m_covariance[index][0].InitializeNonLinearSamples(Samples, coordType, elemType);
    }


    m_initialized = true;

    // check that the mean is near enough to the nominal initial condition
//    MatrixXd meanout = m_covariance[index][0].mean();
//    if(sqrt(((meanin-meanout)*(meanin-meanout).transpose())(0,0)) <= MINDIST*sqrt(((meanin)*(meanin).transpose())(0,0)))
//    {
//        m_initialized = true;
//    }
//    else
//    {
//        QMessageBox msgBox;
//        msgBox.setText("The ActAnalysis: In InitializeSamples() member function the samples mean is too distant from the nominal initial conditions.");
//        msgBox.setWindowTitle("ACT");
//        msgBox.setIcon(QMessageBox::Warning);
//        msgBox.exec();
//        m_initialized = false;
//    }

    for(j=0;j<m_covariance.size();j++)
    {
        if(!m_covariance[j][0].isInitialized())
        {
            m_initialized = false;
            //return false;
        }
    }

    return true;

}

int ActAnalysis::InitializeSamples(QString SamplesFileName, sta::CoordinateSystemType coordType, ActElementType elemType, int index)
{

    MatrixXd Samples;
    int j,k;

    // load samples from file
    QFile samplesfile(SamplesFileName);

    if (!samplesfile.open(QIODevice::ReadOnly | QIODevice::Text))
        return 0;

    QTextStream in(&samplesfile);
    QStringList lines;
    while (!in.atEnd())
    {
        lines.append(in.readLine());
    }
    Samples.resize(lines.size(),6);

    for(k=0;k<lines.size();k++)
    {
        QStringList list = lines[k].split(QRegExp("\\s"),QString::SkipEmptyParts);
        for(j=0; j<list.size();j++)
        {
            Samples[k,j] = list[j].toDouble();
        }

    }

    return InitializeSamples(Samples,coordType,elemType, index);

}

int ActAnalysis::InitializeDist(ActDistributionType Distribution, MatrixXd Param1d, MatrixXi Param2i, MatrixXi Param3i, MatrixXi Param4i, int nsamples, sta::CoordinateSystemType coordType, ActElementType elemType, int index)
{
    // get mean from scenario at time t0
    int NumSamples = m_proptrajectory->trajectorySampleCount();
    QList<double> TimeSamples;
    QList<int> TimeStepsIdx;
    int i;

    for (i=0;i < NumSamples; i++)
    {
        TimeSamples.append(m_proptrajectory->trajectorySampleTime(i));
    }

    sta::StateVector TmpVecJ2000 = m_proptrajectory->trajectorySample(TimeSamples.indexOf(m_timesteps[0]));

    // convert mean reference system

    sta::StateVector TmpVec;
    sta::CoordinateSystem reference(coordType);

    if(reference.isInertial() && (elemType == act::ELEM_CARTESIAN || elemType == act::ELEM_KEPLERIAN))
    {
        TmpVec = reference.fromEmeJ2000(TmpVecJ2000);
    }
    else if(coordType == sta::COORDSYS_BODYFIXED && (elemType == act::ELEM_CARTESIAN || elemType == act::ELEM_SPHERICAL))
    {
        TmpVec = reference.fromEmeJ2000(TmpVecJ2000, m_proptrajectory->centralBody(), m_timesteps[0]);
    }
    else if(coordType == sta::COORDSYS_ROT && elemType == act::ELEM_CARTESIAN)
    {
        // TODO
        QMessageBox msgBox;
        msgBox.setText("The ActAnalysis: In InitializeCov() member function the Coordinate system is incompatible with the element type.");
        msgBox.setWindowTitle("ACT");
        msgBox.setIcon(QMessageBox::Warning);
        msgBox.exec();
        m_initialized = false;
        return false;
    }
    else if(coordType == sta::COORDSYS_ROT_NORM && elemType == act::ELEM_CARTESIAN)
    {
        // TODO
        QMessageBox msgBox;
        msgBox.setText("The ActAnalysis: In InitializeCov() member function the Coordinate system is incompatible with the element type.");
        msgBox.setWindowTitle("ACT");
        msgBox.setIcon(QMessageBox::Warning);
        msgBox.exec();
        m_initialized = false;
        return false;
    }
    else // wrong combination of reference and element type
    {
        QMessageBox msgBox;
        msgBox.setText("The ActAnalysis: In InitializeCov() member function the Coordinate system is incompatible with the element type.");
        msgBox.setWindowTitle("ACT");
        msgBox.setIcon(QMessageBox::Warning);
        msgBox.exec();
        m_initialized = false;
        return false;
    }

    MatrixXd meanin(1,6);
    if(elemType == act::ELEM_KEPLERIAN)
    {
        // convert to keplerian
        sta::KeplerianElements TmpKep = cartesianTOorbital(m_proptrajectory->centralBody()->mu(), TmpVec);
        meanin(0,0) = TmpKep.SemimajorAxis;
        meanin(0,1) = TmpKep.Eccentricity;
        meanin(0,2) = TmpKep.Inclination;
        meanin(0,3) = TmpKep.AscendingNode;
        meanin(0,4) = TmpKep.ArgumentOfPeriapsis;
        meanin(0,5) = TmpKep.MeanAnomaly;

    }
    else if(elemType == act::ELEM_SPHERICAL)
    {

        // convert to spherical
        double r,v,tau,delta,gamma,chi;

        cartesianTOspherical(TmpVec.position(0),TmpVec.position(1),TmpVec.position(2),TmpVec.velocity(0),TmpVec.velocity(1),TmpVec.velocity(2), tau, delta, r, v, gamma, chi);
        meanin(0,0) = r;
        meanin(0,1) = delta;
        meanin(0,2) = tau;
        meanin(0,3) = v;
        meanin(0,4) = gamma;
        meanin(0,5) = chi;


    }
    else // elemType == act::ELEM_CARTESIAN
    {
        for (i=0;i<3;i++)
        {
            meanin(0,i) = TmpVec.position(i);
            meanin(0,i+3) = TmpVec.velocity(i);
        }
    }

    // set coordinatetype and elementtype;
    setCoordinateType(coordType);
    setElementType(elemType);


    MatrixXd Samples;

    // generate samples from distribution

    if(Distribution == DIST_NORMAL)
    {
        for(i=0;i<Param1d.cols();i++)
        {
            if(Param1d(i,i) < 0)
            {
                QMessageBox msgBox;
                msgBox.setText("The ActAnalysis: In InitializeDist() Negative covariance diagonal parameter.");
                msgBox.setWindowTitle("ACT");
                msgBox.setIcon(QMessageBox::Warning);
                msgBox.exec();
                m_initialized = false;
                return false;
            }
        }
        Samples = m_covariance[index][0].NormRnd(meanin,Param1d,nsamples);
    }
    else if(Distribution == DIST_POISSON)
    {
        for(i=0;i<Param1d.cols();i++)
        {
            if(Param1d(0,i) < 0)
            {
                QMessageBox msgBox;
                msgBox.setText("The ActAnalysis: In InitializeDist() Negative Poisson parameter.");
                msgBox.setWindowTitle("ACT");
                msgBox.setIcon(QMessageBox::Warning);
                msgBox.exec();
                m_initialized = false;
                return false;
            }
        }
        Samples = m_covariance[index][0].PoissonRnd(Param1d,nsamples);
    }
    else if(Distribution == DIST_BINOMIAL)
    {
        for(i=0;i<Param1d.cols();i++)
        {
            if(Param1d(0,i) < 0 || Param1d(0,i) > 1)
            {
                QMessageBox msgBox;
                msgBox.setText("The ActAnalysis: In InitializeDist() Binomial parameter out of [0,1] interval.");
                msgBox.setWindowTitle("ACT");
                msgBox.setIcon(QMessageBox::Warning);
                msgBox.exec();
                m_initialized = false;
                return false;
            }
            if(Param2i(0,i) < 0)
            {
                QMessageBox msgBox;
                msgBox.setText("The ActAnalysis: In InitializeDist() Negative Binomial parameter.");
                msgBox.setWindowTitle("ACT");
                msgBox.setIcon(QMessageBox::Warning);
                msgBox.exec();
                m_initialized = false;
                return false;
            }
        }
        Samples = m_covariance[index][0].BinoRnd(Param2i,Param1d,nsamples);
    }
    else if(Distribution == DIST_NEG_BINOMIAL)
    {
        for(i=0;i<Param1d.cols();i++)
        {
            if(Param1d(0,i) <= 0 || Param1d(0,i) > 1)
            {
                QMessageBox msgBox;
                msgBox.setText("The ActAnalysis: In InitializeDist() Binomial parameter out of (0,1] interval.");
                msgBox.setWindowTitle("ACT");
                msgBox.setIcon(QMessageBox::Warning);
                msgBox.exec();
                m_initialized = false;
                return false;
            }
            if(Param2i(0,i) <= 0)
            {
                QMessageBox msgBox;
                msgBox.setText("The ActAnalysis: In InitializeDist() Negative or Null Binomial parameter.");
                msgBox.setWindowTitle("ACT");
                msgBox.setIcon(QMessageBox::Warning);
                msgBox.exec();
                m_initialized = false;
                return false;
            }
        }
        Samples = m_covariance[index][0].NegBinoRnd(Param2i,Param1d,nsamples);
    }
    else if(Distribution == DIST_CHI_SQUARE)
    {
        for(i=0;i<Param2i.cols();i++)
        {
            if(Param2i(0,i) < 1)
            {
                QMessageBox msgBox;
                msgBox.setText("The ActAnalysis: In InitializeDist() Chi-Square parameter < 1.");
                msgBox.setWindowTitle("ACT");
                msgBox.setIcon(QMessageBox::Warning);
                msgBox.exec();
                m_initialized = false;
                return false;
            }
        }
        Samples = m_covariance[index][0].ChiSqRnd(Param2i,Param1d,nsamples);
    }
    else if(Distribution == DIST_DISCRETE_UNIFORM)
    {
        for(i=0;i<Param3i.cols();i++)
        {
            if(Param3i(0,i) < Param2i(0,i))
            {
                QMessageBox msgBox;
                msgBox.setText("The ActAnalysis: In InitializeDist() Discrete Uniform parameter max < min.");
                msgBox.setWindowTitle("ACT");
                msgBox.setIcon(QMessageBox::Warning);
                msgBox.exec();
                m_initialized = false;
                return false;
            }
        }
        Samples = m_covariance[index][0].DiscreteUnifRnd(Param2i,Param3i,nsamples);
    }
    else if(Distribution == DIST_GEOMETRIC)
    {
        for(i=0;i<Param1d.cols();i++)
        {
            if(Param1d(0,i) <= 0 || Param1d(0,i) > 1)
            {
                QMessageBox msgBox;
                msgBox.setText("The ActAnalysis: In InitializeDist() Geometric parameter out of (0,1] interval.");
                msgBox.setWindowTitle("ACT");
                msgBox.setIcon(QMessageBox::Warning);
                msgBox.exec();
                m_initialized = false;
                return false;
            }
        }
        Samples = m_covariance[index][0].GeoRnd(Param1d,nsamples);
    }
    else if(Distribution == DIST_HYPER_GEOMETRIC)
    {
        for(i=0;i<Param2i.cols();i++)
        {
            if(Param2i(0,i) < 0 || Param2i(0,i) > Param4i(0,i) || Param3i(0,i) < 0 || Param3i(0,i) > Param4i(0,i))
            {
                QMessageBox msgBox;
                msgBox.setText("The ActAnalysis: In InitializeDist() Hyper Geometric parameters not consistent.");
                msgBox.setWindowTitle("ACT");
                msgBox.setIcon(QMessageBox::Warning);
                msgBox.exec();
                m_initialized = false;
                return false;
            }
        }
        Samples = m_covariance[index][0].HypGeoRnd(Param2i,Param3i,Param4i,nsamples);
    }
    else
    {
        QMessageBox msgBox;
        msgBox.setText("The ActAnalysis: InitializeDistrib() member function does not support the required Distribution.");
        msgBox.setWindowTitle("ACT");
        msgBox.setIcon(QMessageBox::Critical);
        msgBox.exec();
        m_initialized = false;
        return false;
    }

    return InitializeSamples(Samples,coordType,elemType, index);
}

int ActAnalysis::InitializeCov(MatrixXd Cov, sta::CoordinateSystemType coordType, ActElementType elemType)
{
    return InitializeCov(Cov, coordType, elemType,0);
}

int ActAnalysis::InitializeRMS(MatrixXd RMSValues, sta::CoordinateSystemType coordType, ActElementType elemType)
{
    return InitializeRMS(RMSValues, coordType, elemType, 0);
}

int ActAnalysis::InitializeSamples(MatrixXd Samples, sta::CoordinateSystemType coordType, ActElementType elemType)
{
    return InitializeSamples(Samples, coordType, elemType, 0);
}

int ActAnalysis::InitializeSamples(QString SamplesFileName, sta::CoordinateSystemType coordType, ActElementType elemType)
{
    return InitializeSamples(SamplesFileName, coordType, elemType,0);
}

int ActAnalysis::InitializeDist(ActDistributionType Distribution, MatrixXd Param1d, MatrixXi Param2i, MatrixXi Param3i,  MatrixXi Param4i, int nsamples, sta::CoordinateSystemType coordType, ActElementType elemType)
{
    return InitializeDist(Distribution, Param1d,  Param2i, Param3i, Param4i, nsamples, coordType, elemType,0);
}

int ActAnalysis::propagate()
{
    if(isLinear())
    {
        return propagateCov();
    }
    else
    {
        return propagateMonteCarlo();
    }

}

int ActAnalysis::propagateCov()
{
    int i,j;

    if(!isInitialized())
    {
        QMessageBox msgBox;
        msgBox.setText("The ActAnalysis: propagateCov() member function can be called only after Initialization.");
        msgBox.setWindowTitle("ACT");
        msgBox.setIcon(QMessageBox::Critical);
        msgBox.exec();
        return false;
    }

    if(!isSTMComputed())
    {
        computeSTM();
    }

    for(j=0;j<m_covariance.size();j++)
    {
        if(m_covariance[j][0].isInitialized())
        {
            for (i=1; i < m_covariance[j].size(); i++)
            {
                MatrixXd cov = m_STM[i] * m_covariance[j][0].covarianceJ2000() * m_STM[i].transpose();
                m_covariance[j][i].InitializeJ2000(m_mean[i],cov,coordinateType(),elementType());
            }
        }
    }
    m_propagated = true;
    m_linear = true;
    return true;
}

int ActAnalysis::propagateMonteCarlo()
{
    int i,j,k;

    if(!isInitialized())
    {
        QMessageBox msgBox;
        msgBox.setText("The ActAnalysis: propagateMonteCarlo() member function can be called only after Initialization.");
        msgBox.setWindowTitle("ACT");
        msgBox.setIcon(QMessageBox::Critical);
        msgBox.exec();
        return false;
    }

    for(j=0;j<m_covariance.size();j++)  // non linear propagation of each initial variations needed for AVT (only one for ACT)
    {
        if(m_covariance[j][0].isInitialized())
        {
            // propagate each sample

//            QMessageBox msgBox;
//            msgBox.setText("The ActAnalysis: Getting samples for Montecarlo Propagation.");
//            msgBox.setWindowTitle("ACT");
//            msgBox.setIcon(QMessageBox::Information);
//            msgBox.exec();

            MatrixXd montecarloIC = m_covariance[j][0].samplesJ2000();
            unsigned int NumSamples = m_proptrajectory->trajectorySampleCount();
            PropagationFeedback feedback;
            QList<double> TimeSamples;
            QList<int> TimeStepsIdx;
            QList<sta::StateVector> samples;
            QList<double> sampleTimes;
            sta::StateVector currentIC;
            m_montecarlo[j].clear();

            for(i=0;i<NumSamples;i++)
            {
                TimeSamples << m_proptrajectory->trajectorySampleTime(i);
            }

//            msgBox.setText("The ActAnalysis: Starting Montecarlo Propagations.");
//            msgBox.exec();

            for(k=0;k<montecarloIC.rows();k++)
            {

//                msgBox.setText(QString("The ActAnalysis: Setting Initial condition %1 of %2").arg(k+1).arg(montecarloIC.rows()));
//                msgBox.exec();

                samples.clear();
                sampleTimes.clear();

                currentIC.position(0)= montecarloIC(k,0);
                currentIC.position(1)= montecarloIC(k,1);
                currentIC.position(2)= montecarloIC(k,2);
                currentIC.velocity(0)= montecarloIC(k,3);
                currentIC.velocity(1)= montecarloIC(k,4);
                currentIC.velocity(2)= montecarloIC(k,5);

//                msgBox.setText(QString("The ActAnalysis: Staring propagation %1 of %2").arg(k+1).arg(montecarloIC.rows()));
//                msgBox.exec();

                propagateVehicle(feedback, currentIC, sampleTimes, samples);

//                msgBox.setText(QString("The ActAnalysis: Retriving Results of propagation %1 of %2").arg(k+1).arg(montecarloIC.rows()));
//                msgBox.exec();


                // check that all the time steps are available and fill with last values
                if (samples.size() < NumSamples)
                {
                    sta::StateVector lastSample = samples.last();

                    int sampledim= samples.size();
                    for(i=sampledim;i<NumSamples;i++)
                    {
                        samples << lastSample;
                        sampleTimes << TimeSamples.at(i);
                    }

                }

//                msgBox.setText(QString("The ActAnalysis: Store Results of propagation %1 of %2").arg(k+1).arg(montecarloIC.rows()));
//                msgBox.exec();


                // store results in internal storage m_montecarlo[j][k]
                MatrixXd currentTrajectory;
                currentTrajectory.resize(NumSamples,6);

                for(i=0;i<NumSamples;i++)
                {
                    currentTrajectory(i,0)=samples[i].position(0);
                    currentTrajectory(i,1)=samples[i].position(1);
                    currentTrajectory(i,2)=samples[i].position(2);
                    currentTrajectory(i,3)=samples[i].velocity(0);
                    currentTrajectory(i,4)=samples[i].velocity(1);
                    currentTrajectory(i,5)=samples[i].velocity(2);
                }
                m_montecarlo[j].append(currentTrajectory);



            }

//            msgBox.setText(QString("The ActAnalysis: Recovering results at desired time steps."));
//            msgBox.exec();

            // recover the samples at desired time steps and initialize intermediate covariances
            foreach (double step, m_timesteps)
            {
                // exactly the same value, not <=; time for COV propagation must be in the output times!

                TimeStepsIdx.append(TimeSamples.indexOf(step));
            }


            for (i=1; i < m_covariance[j].size(); i++)
            {
                MatrixXd samplesin;
                samplesin.resize(montecarloIC.rows(),6);



                for(k=0;k<montecarloIC.rows();k++)
                {
//                    msgBox.setText(QString("The ActAnalysis: Initializing Sample %1 of Time Step %2").arg(k+1).arg(TimeStepsIdx[i]));
//                    msgBox.exec();

                    samplesin(k,0)=m_montecarlo[j][k](TimeStepsIdx[i],0);
                    samplesin(k,1)=m_montecarlo[j][k](TimeStepsIdx[i],1);
                    samplesin(k,2)=m_montecarlo[j][k](TimeStepsIdx[i],2);
                    samplesin(k,3)=m_montecarlo[j][k](TimeStepsIdx[i],3);
                    samplesin(k,4)=m_montecarlo[j][k](TimeStepsIdx[i],4);
                    samplesin(k,5)=m_montecarlo[j][k](TimeStepsIdx[i],5);
                }

                m_covariance[j][i].InitializeNonLinearSamplesJ2000(samplesin,coordinateType(),elementType());
            }
        }
    }




    m_propagated = true;
    m_linear = false;
    return true;
}

QList<MatrixXd> ActAnalysis::covariance()
{
    if(!isInitialized())
    {
        QMessageBox msgBox;
        msgBox.setText("The ActAnalysis: covariance() member function can be called only after Initialization.");
        msgBox.setWindowTitle("ACT");
        msgBox.setIcon(QMessageBox::Critical);
        msgBox.exec();
        
    }

    QList<MatrixXd> TmpList;
    foreach (ActCovariance step, m_covariance[0])
    {
        TmpList.append(step.covariance());
    }
    return TmpList;
}

QList<MatrixXd> ActAnalysis::covariance(int index)
{
    if(!isInitialized())
    {
        QMessageBox msgBox;
        msgBox.setText("The ActAnalysis: covariance() member function can be called only after Initialization.");
        msgBox.setWindowTitle("ACT");
        msgBox.setIcon(QMessageBox::Critical);
        msgBox.exec();

    }


    QList<MatrixXd> TmpList;
    if(index < m_covariance.size())
    {
        foreach (ActCovariance step, m_covariance[index])
        {
            TmpList.append(step.covariance());
        }
    }
    return TmpList;
}


MatrixXd ActAnalysis::covariance(int stepnumber, int index)
{
    if(!isInitialized())
    {
        QMessageBox msgBox;
        msgBox.setText("The ActAnalysis: covariance(index) member function can be called only after Initialization.");
        msgBox.setWindowTitle("ACT");
        msgBox.setIcon(QMessageBox::Critical);
        msgBox.exec();
    }

    if(index > m_covariance.size() || stepnumber >= m_covariance[index].size() || (!isPropagated() && stepnumber !=0) )
    {
        QMessageBox msgBox;
        msgBox.setText("ActAnalysis: member function called with index out of scope.");
        msgBox.setWindowTitle("ACT");
        msgBox.setIcon(QMessageBox::Critical);
        msgBox.exec();
        MatrixXd tmp(6,6);
        return tmp;
    }

    return m_covariance[index][stepnumber].covariance();

}

QList<MatrixXd> ActAnalysis::covarianceJ2000()
{
    if(!isInitialized())
    {
        QMessageBox msgBox;
        msgBox.setText("The ActAnalysis: covariance() member function can be called only after Initialization.");
        msgBox.setWindowTitle("ACT");
        msgBox.setIcon(QMessageBox::Critical);
        msgBox.exec();

    }

    QList<MatrixXd> TmpList;
    foreach (ActCovariance step, m_covariance[0])
    {
        TmpList.append(step.covarianceJ2000());
    }
    return TmpList;
}

QList<MatrixXd> ActAnalysis::covarianceJ2000(int index)
{
    if(!isInitialized())
    {
        QMessageBox msgBox;
        msgBox.setText("The ActAnalysis: covariance() member function can be called only after Initialization.");
        msgBox.setWindowTitle("ACT");
        msgBox.setIcon(QMessageBox::Critical);
        msgBox.exec();

    }


    QList<MatrixXd> TmpList;
    if(index < m_covariance.size())
    {

        foreach (ActCovariance step, m_covariance[index])
        {
            TmpList.append(step.covarianceJ2000());
        }
    }
    return TmpList;
}


MatrixXd ActAnalysis::covarianceJ2000(int stepnumber, int index)
{
    if(!isInitialized())
    {
        QMessageBox msgBox;
        msgBox.setText("The ActAnalysis: covariance(index) member function can be called only after Initialization.");
        msgBox.setWindowTitle("ACT");
        msgBox.setIcon(QMessageBox::Critical);
        msgBox.exec();

    }

    if(index > m_covariance.size() || stepnumber >= m_covariance[index].size() || (!isPropagated() && stepnumber !=0) )
    {
        QMessageBox msgBox;
        msgBox.setText("ActAnalysis: member function called with index out of scope.");
        msgBox.setWindowTitle("ACT");
        msgBox.setIcon(QMessageBox::Critical);
        msgBox.exec();
        MatrixXd tmp(6,6);
        return tmp;
    }

    return m_covariance[index][stepnumber].covarianceJ2000();

}

QList<MatrixXd> ActAnalysis::samples()
{
    if(!isInitialized())
    {
        QMessageBox msgBox;
        msgBox.setText("The ActAnalysis: samples() member function can be called only after Initialization.");
        msgBox.setWindowTitle("ACT");
        msgBox.setIcon(QMessageBox::Critical);
        msgBox.exec();

    }

    QList<MatrixXd> TmpList;
    foreach (ActCovariance step, m_covariance[0])
    {
        TmpList.append(step.samples());
    }
    return TmpList;
}

QList<MatrixXd> ActAnalysis::samples(int index)
{
    if(!isInitialized())
    {
        QMessageBox msgBox;
        msgBox.setText("The ActAnalysis: samples() member function can be called only after Initialization.");
        msgBox.setWindowTitle("ACT");
        msgBox.setIcon(QMessageBox::Critical);
        msgBox.exec();

    }

    QList<MatrixXd> TmpList;
    if(index < m_covariance.size())
    {
        foreach (ActCovariance step, m_covariance[index])
        {
            TmpList.append(step.samples());
        }
    }

    return TmpList;
}


MatrixXd ActAnalysis::samples(int stepnumber, int index)
{
    if(!isInitialized())
    {
        QMessageBox msgBox;
        msgBox.setText("The ActAnalysis: samples(index) member function can be called only after Initialization.");
        msgBox.setWindowTitle("ACT");
        msgBox.setIcon(QMessageBox::Critical);
        msgBox.exec();

    }

    if(index > m_covariance.size() || stepnumber >= m_covariance[index].size() || (!isPropagated() && stepnumber !=0))
    {
        QMessageBox msgBox;
        msgBox.setText("ActAnalysis: member function called with index out of scope.");
        msgBox.setWindowTitle("ACT");
        msgBox.setIcon(QMessageBox::Critical);
        msgBox.exec();
        MatrixXd tmp(6,6);
        return tmp;
    }

    return m_covariance[index][stepnumber].samples();
}

QList<MatrixXd> ActAnalysis::samplesJ2000()
{
    if(!isInitialized())
    {
        QMessageBox msgBox;
        msgBox.setText("The ActAnalysis: samples() member function can be called only after Initialization.");
        msgBox.setWindowTitle("ACT");
        msgBox.setIcon(QMessageBox::Critical);
        msgBox.exec();

    }

    QList<MatrixXd> TmpList;
    foreach (ActCovariance step, m_covariance[0])
    {
        TmpList.append(step.samplesJ2000());
    }
    return TmpList;
}

QList<MatrixXd> ActAnalysis::samplesJ2000(int index)
{
    if(!isInitialized())
    {
        QMessageBox msgBox;
        msgBox.setText("The ActAnalysis: samples() member function can be called only after Initialization.");
        msgBox.setWindowTitle("ACT");
        msgBox.setIcon(QMessageBox::Critical);
        msgBox.exec();

    }

    QList<MatrixXd> TmpList;
    if(index < m_covariance.size())
    {
        foreach (ActCovariance step, m_covariance[index])
        {
            TmpList.append(step.samplesJ2000());
        }
    }

    return TmpList;
}


MatrixXd ActAnalysis::samplesJ2000(int stepnumber, int index)
{
    if(!isInitialized())
    {
        QMessageBox msgBox;
        msgBox.setText("The ActAnalysis: samples(index) member function can be called only after Initialization.");
        msgBox.setWindowTitle("ACT");
        msgBox.setIcon(QMessageBox::Critical);
        msgBox.exec();

    }

    if(index > m_covariance.size() || stepnumber >= m_covariance[index].size() || (!isPropagated() && stepnumber !=0))
    {
        QMessageBox msgBox;
        msgBox.setText("ActAnalysis: member function called with index out of scope.");
        msgBox.setWindowTitle("ACT");
        msgBox.setIcon(QMessageBox::Critical);
        msgBox.exec();
        MatrixXd tmp(6,6);
        return tmp;
    }

    return m_covariance[index][stepnumber].samplesJ2000();
}


QList<MatrixXd> ActAnalysis::STM()
{
    if(!isSTMComputed())
    {
        QMessageBox msgBox;
        msgBox.setText("The ActAnalysis: STM() member function can be called only after Propagation.");
        msgBox.setWindowTitle("ACT");
        msgBox.setIcon(QMessageBox::Critical);
        msgBox.exec();

    }

    return m_STM;

}

MatrixXd ActAnalysis::STM(int index)
{
    if(!isPropagated())
    {
        QMessageBox msgBox;
        msgBox.setText("The ActAnalysis: STM(index) member function can be called only after Propagation.");
        msgBox.setWindowTitle("ACT");
        msgBox.setIcon(QMessageBox::Critical);
        msgBox.exec();

    }

    if(index >= m_STM.size())
    {
        QMessageBox msgBox;
        msgBox.setText("ActAnalysis: member function called with index out of scope.");
        msgBox.setWindowTitle("ACT");
        msgBox.setIcon(QMessageBox::Critical);
        msgBox.exec();
        MatrixXd tmp(6,6);
        return tmp;
    }

    return m_STM[index];
}





QList<MatrixXd> ActAnalysis::ellipsoid()
{
    if(!isInitialized())
    {
        QMessageBox msgBox;
        msgBox.setText("The ActAnalysis: ellipsoid() member function can be called only after Initialization.");
        msgBox.setWindowTitle("ACT");
        msgBox.setIcon(QMessageBox::Critical);
        msgBox.exec();

    }

    QList<MatrixXd> TmpList;
    foreach (ActCovariance step, m_covariance[0])
    {
        TmpList.append(step.ellipsoid());
    }
    return TmpList;
}

QList<MatrixXd> ActAnalysis::ellipsoid(int index)
{
    if(!isInitialized())
    {
        QMessageBox msgBox;
        msgBox.setText("The ActAnalysis: ellipsoid() member function can be called only after Initialization.");
        msgBox.setWindowTitle("ACT");
        msgBox.setIcon(QMessageBox::Critical);
        msgBox.exec();

    }

    QList<MatrixXd> TmpList;
    if(index < m_covariance.size())
    {
        foreach (ActCovariance step, m_covariance[index])
        {
            TmpList.append(step.ellipsoid());
        }
    }
    return TmpList;
}


MatrixXd ActAnalysis::ellipsoid(int stepnumber, int index)
{
    if(!isInitialized())
    {
        QMessageBox msgBox;
        msgBox.setText("The ActAnalysis: ellipsoid(index) member function can be called only after Initialization.");
        msgBox.setWindowTitle("ACT");
        msgBox.setIcon(QMessageBox::Critical);
        msgBox.exec();

    }

    if(index > m_covariance.size() || stepnumber >= m_covariance[index].size() || (!isPropagated() && stepnumber !=0))
    {
        QMessageBox msgBox;
        msgBox.setText("ActAnalysis: member function called with index out of scope.");
        msgBox.setWindowTitle("ACT");
        msgBox.setIcon(QMessageBox::Critical);
        msgBox.exec();
        MatrixXd tmp(3,3);
        return tmp;
    }

    return m_covariance[index][stepnumber].ellipsoid();
}






QList<MatrixXd> ActAnalysis::weak()
{
    if(!isInitialized())
    {
        QMessageBox msgBox;
        msgBox.setText("The ActAnalysis: weak() member function can be called only after Initialization.");
        msgBox.setWindowTitle("ACT");
        msgBox.setIcon(QMessageBox::Critical);
        msgBox.exec();

    }

    QList<MatrixXd> TmpList;
    foreach (ActCovariance step, m_covariance[0])
    {
        TmpList.append(step.weak());
    }
    return TmpList;
}

QList<MatrixXd> ActAnalysis::weak(int index)
{
    if(!isInitialized())
    {
        QMessageBox msgBox;
        msgBox.setText("The ActAnalysis: weak() member function can be called only after Initialization.");
        msgBox.setWindowTitle("ACT");
        msgBox.setIcon(QMessageBox::Critical);
        msgBox.exec();

    }

    QList<MatrixXd> TmpList;
    if(index < m_covariance.size())
    {
        foreach (ActCovariance step, m_covariance[index])
        {
            TmpList.append(step.weak());
        }
    }
    return TmpList;
}


MatrixXd ActAnalysis::weak(int stepnumber, int index)
{
    if(!isInitialized())
    {
        QMessageBox msgBox;
        msgBox.setText("The ActAnalysis: weak(index) member function can be called only after Initialization.");
        msgBox.setWindowTitle("ACT");
        msgBox.setIcon(QMessageBox::Critical);
        msgBox.exec();

    }

    if(index > m_covariance.size() || stepnumber >= m_covariance[index].size() || (!isPropagated() && stepnumber !=0))
    {
        QMessageBox msgBox;
        msgBox.setText("ActAnalysis: member function called with index out of scope.");
        msgBox.setWindowTitle("ACT");
        msgBox.setIcon(QMessageBox::Critical);
        msgBox.exec();
        MatrixXd tmp(6,1);
        return tmp;
    }

    return m_covariance[index][stepnumber].weak();
}



QList<MatrixXd> ActAnalysis::normal()
{
    if(!isInitialized())
    {
        QMessageBox msgBox;
        msgBox.setText("The ActAnalysis: normal() member function can be called only after Initialization.");
        msgBox.setWindowTitle("ACT");
        msgBox.setIcon(QMessageBox::Critical);
        msgBox.exec();

    }

    QList<MatrixXd> TmpList;
    foreach (ActCovariance step, m_covariance[0])
    {
        TmpList.append(step.normal());
    }
    return TmpList;
}

QList<MatrixXd> ActAnalysis::normal(int index)
{
    if(!isInitialized())
    {
        QMessageBox msgBox;
        msgBox.setText("The ActAnalysis: normal() member function can be called only after Initialization.");
        msgBox.setWindowTitle("ACT");
        msgBox.setIcon(QMessageBox::Critical);
        msgBox.exec();

    }

    QList<MatrixXd> TmpList;
    if(index < m_covariance.size())
    {
        foreach (ActCovariance step, m_covariance[index])
        {
            TmpList.append(step.normal());
        }
    }
    return TmpList;
}


MatrixXd ActAnalysis::normal(int stepnumber, int index)
{
    if(!isInitialized())
    {
        QMessageBox msgBox;
        msgBox.setText("The ActAnalysis: normal(index) member function can be called only after Initialization.");
        msgBox.setWindowTitle("ACT");
        msgBox.setIcon(QMessageBox::Critical);
        msgBox.exec();

    }

    if(index > m_covariance.size() || stepnumber >= m_covariance[index].size() || (!isPropagated() && stepnumber !=0))
    {
        QMessageBox msgBox;
        msgBox.setText("ActAnalysis: member function called with index out of scope.");
        msgBox.setWindowTitle("ACT");
        msgBox.setIcon(QMessageBox::Critical);
        msgBox.exec();
        MatrixXd tmp(6,6);
        return tmp;
    }

    return m_covariance[index][stepnumber].normal();
}



QList<MatrixXd> ActAnalysis::eig()
{
    if(!isInitialized())
    {
        QMessageBox msgBox;
        msgBox.setText("The ActAnalysis: eig() member function can be called only after Initialization.");
        msgBox.setWindowTitle("ACT");
        msgBox.setIcon(QMessageBox::Critical);
        msgBox.exec();

    }

    QList<MatrixXd> TmpList;
    foreach (ActCovariance step, m_covariance[0])
    {
        TmpList.append(step.eig());
    }
    return TmpList;
}

QList<MatrixXd> ActAnalysis::eig(int index)
{
    if(!isInitialized())
    {
        QMessageBox msgBox;
        msgBox.setText("The ActAnalysis: eig() member function can be called only after Initialization.");
        msgBox.setWindowTitle("ACT");
        msgBox.setIcon(QMessageBox::Critical);
        msgBox.exec();

    }

    QList<MatrixXd> TmpList;
    if(index < m_covariance.size())
    {
        foreach (ActCovariance step, m_covariance[index])
        {
            TmpList.append(step.eig());
        }
    }
    return TmpList;
}


MatrixXd ActAnalysis::eig(int stepnumber, int index)
{
    if(!isInitialized())
    {
        QMessageBox msgBox;
        msgBox.setText("The ActAnalysis: eig(index) member function can be called only after Initialization.");
        msgBox.setWindowTitle("ACT");
        msgBox.setIcon(QMessageBox::Critical);
        msgBox.exec();

    }

    if(index > m_covariance.size() || stepnumber >= m_covariance[index].size() || (!isPropagated() && stepnumber !=0))
    {
        QMessageBox msgBox;
        msgBox.setText("ActAnalysis: member function called with index out of scope.");
        msgBox.setWindowTitle("ACT");
        msgBox.setIcon(QMessageBox::Critical);
        msgBox.exec();
        MatrixXd tmp(6,6);
        return tmp;
    }

    return m_covariance[index][stepnumber].eig();
}



QList<MatrixXd> ActAnalysis::eigv()
{
    if(!isInitialized())
    {
        QMessageBox msgBox;
        msgBox.setText("The ActAnalysis: eigv() member function can be called only after Initialization.");
        msgBox.setWindowTitle("ACT");
        msgBox.setIcon(QMessageBox::Critical);
        msgBox.exec();

    }

    QList<MatrixXd> TmpList;
    foreach (ActCovariance step, m_covariance[0])
    {
        TmpList.append(step.eigv());
    }
    return TmpList;
}

QList<MatrixXd> ActAnalysis::eigv(int index)
{
    if(!isInitialized())
    {
        QMessageBox msgBox;
        msgBox.setText("The ActAnalysis: eigv() member function can be called only after Initialization.");
        msgBox.setWindowTitle("ACT");
        msgBox.setIcon(QMessageBox::Critical);
        msgBox.exec();

    }

    QList<MatrixXd> TmpList;
    if(index < m_covariance.size())
    {
        foreach (ActCovariance step, m_covariance[index])
        {
            TmpList.append(step.eigv());
        }
    }
    return TmpList;
}


MatrixXd ActAnalysis::eigv(int stepnumber, int index)
{
    if(!isInitialized())
    {
        QMessageBox msgBox;
        msgBox.setText("The ActAnalysis: eigv(index) member function can be called only after Initialization.");
        msgBox.setWindowTitle("ACT");
        msgBox.setIcon(QMessageBox::Critical);
        msgBox.exec();

    }

    if(index > m_covariance.size() || stepnumber >= m_covariance[index].size() || (!isPropagated() && stepnumber !=0))
    {
        QMessageBox msgBox;
        msgBox.setText("ActAnalysis: member function called with index out of scope.");
        msgBox.setWindowTitle("ACT");
        msgBox.setIcon(QMessageBox::Critical);
        msgBox.exec();
        MatrixXd tmp(6,6);
        return tmp;
    }

    return m_covariance[index][stepnumber].eigv();
}


QList<MatrixXd> ActAnalysis::RMS()
{
    if(!isInitialized())
    {
        QMessageBox msgBox;
        msgBox.setText("The ActAnalysis: RMS() member function can be called only after Initialization.");
        msgBox.setWindowTitle("ACT");
        msgBox.setIcon(QMessageBox::Critical);
        msgBox.exec();

    }

    QList<MatrixXd> TmpList;
    foreach (ActCovariance step, m_covariance[0])
    {
        TmpList.append(step.RMS());
    }
    return TmpList;
}

QList<MatrixXd> ActAnalysis::RMS(int index)
{
    if(!isInitialized())
    {
        QMessageBox msgBox;
        msgBox.setText("The ActAnalysis: RMS() member function can be called only after Initialization.");
        msgBox.setWindowTitle("ACT");
        msgBox.setIcon(QMessageBox::Critical);
        msgBox.exec();

    }

    QList<MatrixXd> TmpList;
    if(index < m_covariance.size())
    {
        foreach (ActCovariance step, m_covariance[index])
        {
            TmpList.append(step.RMS());
        }
    }
    return TmpList;
}


MatrixXd ActAnalysis::RMS(int stepnumber, int index)
{
    if(!isInitialized())
    {
        QMessageBox msgBox;
        msgBox.setText("The ActAnalysis: RMS(index) member function can be called only after Initialization.");
        msgBox.setWindowTitle("ACT");
        msgBox.setIcon(QMessageBox::Critical);
        msgBox.exec();

    }

    if(index > m_covariance.size() || stepnumber >= m_covariance[index].size() || (!isPropagated() && stepnumber !=0))
    {
        QMessageBox msgBox;
        msgBox.setText("ActAnalysis: member function called with index out of scope.");
        msgBox.setWindowTitle("ACT");
        msgBox.setIcon(QMessageBox::Critical);
        msgBox.exec();
        MatrixXd tmp(6,1);
        return tmp;
    }

    return m_covariance[index][stepnumber].RMS();
}


QList<MatrixXd> ActAnalysis::mean()
{
    if(!isInitialized())
    {
        QMessageBox msgBox;
        msgBox.setText("The ActAnalysis: mean() member function can be called only after Initialization.");
        msgBox.setWindowTitle("ACT");
        msgBox.setIcon(QMessageBox::Critical);
        msgBox.exec();

    }

    QList<MatrixXd> TmpList;
    foreach (ActCovariance step, m_covariance[0])
    {
        TmpList.append(step.mean());
    }
    return TmpList;
}

QList<MatrixXd> ActAnalysis::mean(int index)
{
    if(!isInitialized())
    {
        QMessageBox msgBox;
        msgBox.setText("The ActAnalysis: mean() member function can be called only after Initialization.");
        msgBox.setWindowTitle("ACT");
        msgBox.setIcon(QMessageBox::Critical);
        msgBox.exec();

    }

    QList<MatrixXd> TmpList;
    if(index < m_covariance.size())
    {
        foreach (ActCovariance step, m_covariance[index])
        {
            TmpList.append(step.mean());
        }
    }
    return TmpList;
}


MatrixXd ActAnalysis::mean(int stepnumber, int index)
{
    if(!isInitialized())
    {
        QMessageBox msgBox;
        msgBox.setText("The ActAnalysis: mean(index) member function can be called only after Initialization.");
        msgBox.setWindowTitle("ACT");
        msgBox.setIcon(QMessageBox::Critical);
        msgBox.exec();

    }

    if(index > m_covariance.size() || stepnumber >= m_covariance[index].size() || (!isPropagated() && stepnumber !=0))
    {
        QMessageBox msgBox;
        msgBox.setText("ActAnalysis: member function called with index out of scope.");
        msgBox.setWindowTitle("ACT");
        msgBox.setIcon(QMessageBox::Critical);
        msgBox.exec();
        MatrixXd tmp(6,1);
        return tmp;
    }

    return m_covariance[index][stepnumber].mean();
}

QList<MatrixXd> ActAnalysis::meanJ2000()
{
    if(!isInitialized())
    {
        QMessageBox msgBox;
        msgBox.setText("The ActAnalysis: mean() member function can be called only after Initialization.");
        msgBox.setWindowTitle("ACT");
        msgBox.setIcon(QMessageBox::Critical);
        msgBox.exec();

    }

    QList<MatrixXd> TmpList;
    foreach (ActCovariance step, m_covariance[0])
    {
        TmpList.append(step.meanJ2000());
    }
    return TmpList;
}

QList<MatrixXd> ActAnalysis::meanJ2000(int index)
{
    if(!isInitialized())
    {
        QMessageBox msgBox;
        msgBox.setText("The ActAnalysis: mean() member function can be called only after Initialization.");
        msgBox.setWindowTitle("ACT");
        msgBox.setIcon(QMessageBox::Critical);
        msgBox.exec();

    }

    QList<MatrixXd> TmpList;
    if(index < m_covariance.size())
    {
        foreach (ActCovariance step, m_covariance[index])
        {
            TmpList.append(step.meanJ2000());
        }
    }
    return TmpList;
}



MatrixXd ActAnalysis::meanJ2000(int stepnumber, int index)
{
    if(!isInitialized())
    {
        QMessageBox msgBox;
        msgBox.setText("The ActAnalysis: mean(index) member function can be called only after Initialization.");
        msgBox.setWindowTitle("ACT");
        msgBox.setIcon(QMessageBox::Critical);
        msgBox.exec();

    }

    if(index > m_covariance.size() || stepnumber >= m_covariance[index].size() || (!isPropagated() && stepnumber !=0))
    {
        QMessageBox msgBox;
        msgBox.setText("ActAnalysis: member function called with index out of scope.");
        msgBox.setWindowTitle("ACT");
        msgBox.setIcon(QMessageBox::Critical);
        msgBox.exec();
        MatrixXd tmp(6,1);
        return tmp;
    }

    return m_covariance[index][stepnumber].meanJ2000();
}



QList<MatrixXd> ActAnalysis::median()
{
    if(!isInitialized())
    {
        QMessageBox msgBox;
        msgBox.setText("The ActAnalysis: median() member function can be called only after Initialization.");
        msgBox.setWindowTitle("ACT");
        msgBox.setIcon(QMessageBox::Critical);
        msgBox.exec();

    }

    QList<MatrixXd> TmpList;
    foreach (ActCovariance step, m_covariance[0])
    {
        TmpList.append(step.median());
    }
    return TmpList;
}

QList<MatrixXd> ActAnalysis::median(int index)
{
    if(!isInitialized())
    {
        QMessageBox msgBox;
        msgBox.setText("The ActAnalysis: median() member function can be called only after Initialization.");
        msgBox.setWindowTitle("ACT");
        msgBox.setIcon(QMessageBox::Critical);
        msgBox.exec();

    }

    QList<MatrixXd> TmpList;
    if(index < m_covariance.size())
    {
        foreach (ActCovariance step, m_covariance[index])
        {
            TmpList.append(step.median());
        }
    }
    return TmpList;
}



MatrixXd ActAnalysis::median(int stepnumber, int index)
{
    if(!isInitialized())
    {
        QMessageBox msgBox;
        msgBox.setText("The ActAnalysis: median(index) member function can be called only after Initialization.");
        msgBox.setWindowTitle("ACT");
        msgBox.setIcon(QMessageBox::Critical);
        msgBox.exec();

    }

    if(index > m_covariance.size() || stepnumber >= m_covariance[index].size() || (!isPropagated() && stepnumber !=0))
    {
        QMessageBox msgBox;
        msgBox.setText("ActAnalysis: member function called with index out of scope.");
        msgBox.setWindowTitle("ACT");
        msgBox.setIcon(QMessageBox::Critical);
        msgBox.exec();
        MatrixXd tmp(6,1);
        return tmp;
    }

    return m_covariance[index][stepnumber].median();
}


QList<MatrixXd> ActAnalysis::min()
{
    if(!isInitialized())
    {
        QMessageBox msgBox;
        msgBox.setText("The ActAnalysis: min() member function can be called only after Initialization.");
        msgBox.setWindowTitle("ACT");
        msgBox.setIcon(QMessageBox::Critical);
        msgBox.exec();

    }

    QList<MatrixXd> TmpList;
    foreach (ActCovariance step, m_covariance[0])
    {
        TmpList.append(step.min());
    }
    return TmpList;
}

QList<MatrixXd> ActAnalysis::min(int index)
{
    if(!isInitialized())
    {
        QMessageBox msgBox;
        msgBox.setText("The ActAnalysis: min() member function can be called only after Initialization.");
        msgBox.setWindowTitle("ACT");
        msgBox.setIcon(QMessageBox::Critical);
        msgBox.exec();

    }

    QList<MatrixXd> TmpList;
    if (index < m_covariance.size())
    {
        foreach (ActCovariance step, m_covariance[index])
        {
            TmpList.append(step.min());
        }
    }
    return TmpList;
}


MatrixXd ActAnalysis::min(int stepnumber, int index)
{
    if(!isInitialized())
    {
        QMessageBox msgBox;
        msgBox.setText("The ActAnalysis: min(index) member function can be called only after Initialization.");
        msgBox.setWindowTitle("ACT");
        msgBox.setIcon(QMessageBox::Critical);
        msgBox.exec();

    }

    if(index > m_covariance.size() || stepnumber >= m_covariance[index].size() || (!isPropagated() && stepnumber !=0))
    {
        QMessageBox msgBox;
        msgBox.setText("ActAnalysis: member function called with index out of scope.");
        msgBox.setWindowTitle("ACT");
        msgBox.setIcon(QMessageBox::Critical);
        msgBox.exec();
        MatrixXd tmp(6,1);
        return tmp;
    }

    return m_covariance[index][stepnumber].min();
}


QList<MatrixXd> ActAnalysis::max()
{
    if(!isInitialized())
    {
        QMessageBox msgBox;
        msgBox.setText("The ActAnalysis: max() member function can be called only after Initialization.");
        msgBox.setWindowTitle("ACT");
        msgBox.setIcon(QMessageBox::Critical);
        msgBox.exec();

    }

    QList<MatrixXd> TmpList;
    foreach (ActCovariance step, m_covariance[0])
    {
        TmpList.append(step.max());
    }
    return TmpList;
}

QList<MatrixXd> ActAnalysis::max(int index)
{
    if(!isInitialized())
    {
        QMessageBox msgBox;
        msgBox.setText("The ActAnalysis: max() member function can be called only after Initialization.");
        msgBox.setWindowTitle("ACT");
        msgBox.setIcon(QMessageBox::Critical);
        msgBox.exec();

    }

    QList<MatrixXd> TmpList;
    if (index < m_covariance.size())
    {
        foreach (ActCovariance step, m_covariance[index])
        {
            TmpList.append(step.max());
        }
    }
    return TmpList;
}



MatrixXd ActAnalysis::max(int stepnumber, int index)
{
    if(!isInitialized())
    {
        QMessageBox msgBox;
        msgBox.setText("The ActAnalysis: max(index) member function can be called only after Initialization.");
        msgBox.setWindowTitle("ACT");
        msgBox.setIcon(QMessageBox::Critical);
        msgBox.exec();

    }

    if(index > m_covariance.size() || stepnumber >= m_covariance[index].size() || (!isPropagated() && stepnumber !=0))
    {
        QMessageBox msgBox;
        msgBox.setText("ActAnalysis: member function called with index out of scope.");
        msgBox.setWindowTitle("ACT");
        msgBox.setIcon(QMessageBox::Critical);
        msgBox.exec();
        MatrixXd tmp(6,1);
        return tmp;
    }

    return m_covariance[index][stepnumber].max();
}


QList<MatrixXd> ActAnalysis::range()
{
    if(!isInitialized())
    {
        QMessageBox msgBox;
        msgBox.setText("The ActAnalysis: range() member function can be called only after Initialization.");
        msgBox.setWindowTitle("ACT");
        msgBox.setIcon(QMessageBox::Critical);
        msgBox.exec();

    }

    QList<MatrixXd> TmpList;
    foreach (ActCovariance step, m_covariance[0])
    {
        TmpList.append(step.range());
    }
    return TmpList;
}
QList<MatrixXd> ActAnalysis::range(int index)
{
    if(!isInitialized())
    {
        QMessageBox msgBox;
        msgBox.setText("The ActAnalysis: range() member function can be called only after Initialization.");
        msgBox.setWindowTitle("ACT");
        msgBox.setIcon(QMessageBox::Critical);
        msgBox.exec();

    }

    QList<MatrixXd> TmpList;
    if (index < m_covariance.size())
    {
        foreach (ActCovariance step, m_covariance[index])
        {
            TmpList.append(step.range());
        }
    }
    return TmpList;
}



MatrixXd ActAnalysis::range(int stepnumber, int index)
{
    if(!isInitialized())
    {
        QMessageBox msgBox;
        msgBox.setText("The ActAnalysis: range(index) member function can be called only after Initialization.");
        msgBox.setWindowTitle("ACT");
        msgBox.setIcon(QMessageBox::Critical);
        msgBox.exec();

    }

    if(index > m_covariance.size() || stepnumber >= m_covariance[index].size() || (!isPropagated() && stepnumber !=0))
    {
        QMessageBox msgBox;
        msgBox.setText("ActAnalysis: member function called with index out of scope.");
        msgBox.setWindowTitle("ACT");
        msgBox.setIcon(QMessageBox::Critical);
        msgBox.exec();
        MatrixXd tmp(6,1);
        return tmp;
    }

    return m_covariance[index][stepnumber].range();
}

int ActAnalysis::saveOutputFile(QList<ActOutputType> list, QString filename)
{
    return saveOutputFile(list,filename,0);
}

int ActAnalysis::saveOutputFile(QList<ActOutputType> list, QString filename, int index)
{
    QList<QList <MatrixXd> > results;

    if(!isPropagated())
    {
        return false;
    }

    QFile outfile(filename);
    if (!outfile.open(QIODevice::WriteOnly | QIODevice::Text))
             return false;

    QTextStream out(&outfile);

    out.setFieldWidth(16);
    out.setFieldAlignment(QTextStream::AlignRight);
    out.setPadChar(' ');

    out.setRealNumberPrecision(8);
    out.setRealNumberNotation(QTextStream::ScientificNotation);

    out << "#            MJD";

    int i,j,k,p;
    foreach(ActOutputType step, list)
    {
        if(step == act::OUT_COV)
        {
            for(i=0;i<6;i++) // rows
            {
                for(j=0;j<6;j++)  // cols
                {
                    QString label;
                    QTextStream outlabel(&label);
                    outlabel << "Cov(" << i+1 << "," << j+1 << ")";
                    out << label;
                }
            }
            results.append(covariance(index));
        }
        else if(step == act::OUT_COVJ2K)
        {
            for(i=0;i<6;i++) // rows
            {
                for(j=0;j<6;j++)  // cols
                {
                    QString label;
                    QTextStream outlabel(&label);
                    outlabel << "CovJ2K(" << i+1 << "," << j+1 << ")";
                    out << label;
                }
            }
            results.append(covarianceJ2000(index));
        }
        else if(step == act::OUT_EIG)
        {
            for(i=0;i<6;i++) // rows
            {
                QString label;
                QTextStream outlabel(&label);
                outlabel << "Eig(" << i+1 << ")";
                out << label;
            }
            results.append(eig(index));
        }
        else if(step == act::OUT_EIGV)
        {
            for(i=0;i<6;i++) // rows
            {
                for(j=0;j<6;j++)  // cols
                {
                    QString label;
                    QTextStream outlabel(&label);
                    outlabel << "EigV(" << i+1 << "," << j+1 << ")";
                    out << label;
                }
            }
            results.append(eigv(index));
        }
        else if(step == act::OUT_ELLIPSOID)
        {
            for(i=0;i<3;i++) // rows
            {
                for(j=0;j<3;j++)  // cols
                {
                    QString label;
                    QTextStream outlabel(&label);
                    outlabel << "Ellipsoid(" << i+1 << "," << j+1 << ")";
                    out << label;
                }
            }
            results.append(ellipsoid(index));
        }
        else if(step == act::OUT_NORMAL)
        {
            for(i=0;i<6;i++) // rows
            {
                for(j=0;j<6;j++)  // cols
                {
                    QString label;
                    QTextStream outlabel(&label);
                    outlabel << "Normal(" << i+1 << "," << j+1 << ")";
                    out << label;
                }
            }
            results.append(normal(index));
        }
        else if(step == act::OUT_WEAK)
        {
            for(i=0;i<6;i++) // rows
            {
                QString label;
                QTextStream outlabel(&label);
                outlabel << "Weak(" << i+1 << ")";
                out << label;
            }
            results.append(weak(index));
        }
        else if(step == act::OUT_MEAN)
        {
            for(i=0;i<6;i++) // rows
            {
                QString label;
                QTextStream outlabel(&label);
                outlabel << "Mean(" << i+1 << ")";
                out << label;
            }
            results.append(mean(index));
        }
        else if(step == act::OUT_MEANJ2K)
        {
            for(i=0;i<6;i++) // rows
            {
                QString label;
                QTextStream outlabel(&label);
                outlabel << "MeanJ2K(" << i+1 << ")";
                out << label;
            }
            results.append(meanJ2000(index));
        }
        else if(step == act::OUT_MEDIAN)
        {
            for(i=0;i<6;i++) // rows
            {
                QString label;
                QTextStream outlabel(&label);
                outlabel << "Median(" << i+1 << ")";
                out << label;
            }
            results.append(median(index));
        }
        else if(step == act::OUT_RMS)
        {
            for(i=0;i<6;i++) // rows
            {
                QString label;
                QTextStream outlabel(&label);
                outlabel << "RMS(" << i+1 << ")";
                out << label;
            }
            results.append(RMS(index));
        }
        else if(step == act::OUT_MIN)
        {
            for(i=0;i<6;i++) // rows
            {
                QString label;
                QTextStream outlabel(&label);
                outlabel << "Min(" << i+1 << ")";
                out << label;
            }
            results.append(min(index));
        }
        else if(step == act::OUT_MAX)
        {
            for(i=0;i<6;i++) // rows
            {
                QString label;
                QTextStream outlabel(&label);
                outlabel << "Max(" << i+1 << ")";
                out << label;
            }
            results.append(max(index));
        }
        else if(step == act::OUT_RANGE)
        {
            for(i=0;i<6;i++) // rows
            {
                QString label;
                QTextStream outlabel(&label);
                outlabel << "Range(" << i+1 << ")";
                out << label;

            }
            results.append(range(index));
        }

    }
    out.setFieldWidth(0);
    out << endl;
    out.setFieldWidth(16);


    for(k=0;k<m_timesteps.size();k++)
    {
        out << m_timesteps[k];

        for(p=0;p<list.size();p++)
        {
            ActOutputType step = list.at(p);
            if(step == act::OUT_COV)
            {
                for(i=0;i<6;i++) // rows
                {
                    for(j=0;j<6;j++)  // cols
                    {
                        out << results[p][k](i,j);
                    }
                }

            }
            else if(step == act::OUT_COVJ2K)
            {
                for(i=0;i<6;i++) // rows
                {
                    for(j=0;j<6;j++)  // cols
                    {
                        out << results[p][k](i,j);
                    }
                }

            }
            else if(step == act::OUT_EIG)
            {
                for(i=0;i<6;i++) // rows
                {
                    out << results[p][k](i,i);
                }

            }
            else if(step == act::OUT_EIGV)
            {
                for(i=0;i<6;i++) // rows
                {
                    for(j=0;j<6;j++)  // cols
                    {
                        out << results[p][k](i,j);
                    }
                }


            }
            else if(step == act::OUT_ELLIPSOID)
            {
                for(i=0;i<3;i++) // rows
                {
                    for(j=0;j<3;j++)  // cols
                    {
                        out << results[p][k](i,j);
                    }
                }


            }
            else if(step == act::OUT_NORMAL)
            {
                for(i=0;i<6;i++) // rows
                {
                    for(j=0;j<6;j++)  // cols
                    {
                        out << results[p][k](i,j);
                    }
                }

            }
            else if(step == act::OUT_WEAK)
            {
                for(i=0;i<6;i++) // rows
                {
                    out << results[p][k](0,i);
                }

            }
            else if(step == act::OUT_MEAN)
            {
                for(i=0;i<6;i++) // rows
                {
                    out << results[p][k](0,i);
                }

            }
            else if(step == act::OUT_MEANJ2K)
            {
                for(i=0;i<6;i++) // rows
                {
                    out << results[p][k](0,i);
                }

            }
            else if(step == act::OUT_MEDIAN)
            {
                for(i=0;i<6;i++) // rows
                {
                    out << results[p][k](0,i);
                }

            }
            else if(step == act::OUT_RMS)
            {
                for(i=0;i<6;i++) // rows
                {
                    out << results[p][k](0,i);
                }

            }
            else if(step == act::OUT_MIN)
            {
                for(i=0;i<6;i++) // rows
                {
                    out << results[p][k](0,i);
                }

            }
            else if(step == act::OUT_MAX)
            {
                for(i=0;i<6;i++) // rows
                {
                    out << results[p][k](0,i);
                }

            }
            else if(step == act::OUT_RANGE)
            {
                for(i=0;i<6;i++) // rows
                {
                    out << results[p][k](0,i);
                }

            }

        }



        out.setFieldWidth(0);
        out << endl;
        out.setFieldWidth(16);
    }

    outfile.close();
    return true;

}

int ActAnalysis::saveSampleFile(QString filename, int stepnumber, int index)
{
    if(!isPropagated())
    {
        return false;
    }

    QFile outfile(filename);
    if (!outfile.open(QIODevice::WriteOnly | QIODevice::Text))
             return false;

    QTextStream out(&outfile);

    out.setFieldWidth(16);
    out.setFieldAlignment(QTextStream::AlignRight);
    out.setPadChar(' ');

    out.setRealNumberPrecision(8);
    out.setRealNumberNotation(QTextStream::ScientificNotation);

    MatrixXd results=samples(stepnumber, index);
    int i,j;
    for(i=0;i<results.rows();i++)
    {
        for(j=0;j<results.cols();j++)
        {
            out << results(i,j);
        }
        out.setFieldWidth(0);
        out << endl;
        out.setFieldWidth(16);
    }
    outfile.close();
    return true;
}

int ActAnalysis::saveSampleFileJ2K(QString filename, int stepnumber, int index)
{
    if(!isPropagated())
    {
        return false;
    }

    QFile outfile(filename);
    if (!outfile.open(QIODevice::WriteOnly | QIODevice::Text))
             return false;

    QTextStream out(&outfile);

    out.setFieldWidth(16);
    out.setFieldAlignment(QTextStream::AlignRight);
    out.setPadChar(' ');

    out.setRealNumberPrecision(8);
    out.setRealNumberNotation(QTextStream::ScientificNotation);

    MatrixXd results=samplesJ2000(stepnumber, index);
    int i,j;
    for(i=0;i<results.rows();i++)
    {
        for(j=0;j<results.cols();j++)
        {
            out << results(i,j);
        }
        out.setFieldWidth(0);
        out << endl;
        out.setFieldWidth(16);
    }

    outfile.close();
    return true;

}


void ActAnalysis::computeSTM()
{
    unsigned int NumSamples;
    // BE CAREFULL: Hard coded 6 states!
    MatrixXd MaxStates(6,1);
    MatrixXd Perturbations(6,1);
    QList<double> TimeSamples;
    QList<int> TimeStepsIdx;


    double eps = sqrt(2.22044604925031e-016); // setting the entity of the perturbation
    int i,j,index;

    for (i=0; i<6; i++)
    {
        MaxStates(i,0)= 1;
    }


    // evaluate perturbation entity
    NumSamples = m_proptrajectory->trajectorySampleCount();

    //int idx1=NumSamples-1; // will store position of tf
    //int idx2=0; // will store position of ti
    double normp;
    double normv;

    for (index=0;index < NumSamples; index++)
    {
        sta::StateVector CurrentSample = m_proptrajectory->trajectorySample(index);
        TimeSamples.append(m_proptrajectory->trajectorySampleTime(index));

        normp = sqrt(pow(CurrentSample.position(0),2)+pow(CurrentSample.position(1),2)+pow(CurrentSample.position(2),2));
        normv = sqrt(pow(CurrentSample.velocity(0),2)+pow(CurrentSample.velocity(1),2)+pow(CurrentSample.velocity(2),2));

        for (i=0; i<3; i++)
        {
            if( normp > MaxStates(i,0))
            {
                MaxStates(i,0) = normp;
            }
            if( normv > MaxStates(i+3,0))
            {
                MaxStates(i+3,0) = normv;
            }
        }

    }


    foreach (double step, m_timesteps)
    {
        // exactly the same value, not <=; time for COV propagation must be in the output times!

        TimeStepsIdx.append(TimeSamples.indexOf(step));
    }


    for (i=0; i<6; i++)
    {
        Perturbations(i,0) = MaxStates(i,0)*eps;
    }

    // recover Initial Conditions from first state of propagated trajectory
    sta::StateVector InitialConditions = m_proptrajectory->trajectorySample(0);
    QList<sta::StateVector> FinalStates;

    foreach (int step, TimeStepsIdx)
    {
        FinalStates.append(m_proptrajectory->trajectorySample(step));
    }

    for(index=0; index < FinalStates.size(); index++)
    {
        MatrixXd mat(1,6);
        for (i=0;i<3;i++)
        {
            mat(0,i)   = FinalStates[index].position(i);
            mat(0,i+3) = FinalStates[index].velocity(i);
        }
        m_mean[index] = mat;
    }

    QList<sta::StateVector> FWPerturbedInitialConditions;
    QList< QList<sta::StateVector> > FWPerturbedFinalStates;


    QList<sta::StateVector> BWPerturbedInitialConditions;
    QList< QList<sta::StateVector> > BWPerturbedFinalStates;


    QList<double> sampleTimes;
    QList<sta::StateVector> samples;

    PropagationFeedback feedback;

    for (i=0; i< 6 ; i++)
    {
        QList<sta::StateVector> List;
        FWPerturbedFinalStates.append(List);
        BWPerturbedFinalStates.append(List);
    }

    // forward propagation of central finite differences
    // position
    for (i=0; i<3 ; i++)
    {
        sta::StateVector tmp(InitialConditions);

        // perturb initial condition
        tmp.position(i) += Perturbations(i,0);
        FWPerturbedInitialConditions.append(tmp);

        // propagate perturbed trajectories
        sampleTimes.clear();
        samples.clear();

        propagateVehicle(feedback, FWPerturbedInitialConditions.at(i), sampleTimes, samples);

        // check that all the time steps are available and fill with last values
        if (samples.size() < NumSamples)
        {
            sta::StateVector lastSample = samples.last();

            int sampledim= samples.size();
            for(j=sampledim;j<NumSamples;j++)
            {
                samples << lastSample;
                sampleTimes << TimeSamples.at(j);
            }

        }

        foreach (int step, TimeStepsIdx)
        {
            FWPerturbedFinalStates[i].append(samples.at(step));
        }

    }

    //velocity
    for (i=3; i<6 ; i++)
    {
        sta::StateVector tmp(InitialConditions);

        // perturb initial condition
        tmp.velocity(i-3) += Perturbations(i,0);
        FWPerturbedInitialConditions.append(tmp);

        // propagate perturbed trajectories
        sampleTimes.clear();
        samples.clear();

        propagateVehicle(feedback, FWPerturbedInitialConditions.at(i), sampleTimes, samples);

        // check that all the time steps are available and fill with last values
        if (samples.size() < NumSamples)
        {
            sta::StateVector lastSample = samples.last();

            int sampledim= samples.size();
            for(j=sampledim;j<NumSamples;j++)
            {
                samples << lastSample;
                sampleTimes << TimeSamples.at(j);
            }

        }

        foreach (int step, TimeStepsIdx)
        {
            FWPerturbedFinalStates[i].append(samples.at(step));
        }

    }

    // backward propagation of central finite differences
    // position
    for (i=0; i<3 ; i++)
    {
        sta::StateVector tmp(InitialConditions);

        // perturb initial condition
        tmp.position(i) -= Perturbations(i,0);
        BWPerturbedInitialConditions.append(tmp);

        // propagate perturbed trajectories
        sampleTimes.clear();
        samples.clear();

        propagateVehicle(feedback, BWPerturbedInitialConditions.at(i), sampleTimes, samples);

        // check that all the time steps are available and fill with last values
        if (samples.size() < NumSamples)
        {
            sta::StateVector lastSample = samples.last();

            int sampledim= samples.size();
            for(j=sampledim;j<NumSamples;j++)
            {
                samples << lastSample;
                sampleTimes << TimeSamples.at(j);
            }

        }

        foreach (int step, TimeStepsIdx)
        {
            BWPerturbedFinalStates[i].append(samples.at(step));
        }
    }

    //velocity
    for (i=3; i<6 ; i++)
    {
        sta::StateVector tmp(InitialConditions);

        // perturb initial condition
        tmp.velocity(i-3) -= Perturbations(i,0);
        BWPerturbedInitialConditions.append(tmp);

        // propagate perturbed trajectories
        sampleTimes.clear();
        samples.clear();

        propagateVehicle(feedback, BWPerturbedInitialConditions.at(i), sampleTimes, samples);

        // check that all the time steps are available and fill with last values
        if (samples.size() < NumSamples)
        {
            sta::StateVector lastSample = samples.last();


            int sampledim= samples.size();
            for(j=sampledim;j<NumSamples;j++)
            {
                samples << lastSample;
                sampleTimes << TimeSamples.at(j);
            }

        }

        foreach (int step, TimeStepsIdx)
        {
            BWPerturbedFinalStates[i].append(samples.at(step));
        }

    }
    //cout << FWPerturbedInitialConditions[0].position;
    //cout << FWPerturbedFinalStates[perturbation number][da 0 a numero di timestep-1 = TimeStepsIdx[i] ].position;

    //         STM2              STM3
    // |----------------|---------------------|
    //                 STM1
    // |--------------------------------------|
    // t0               ti                    tf

    // STM = STM3 = STM1 inv(STM2)

    // compute STM1
    MatrixXd STM1(6,6);
    MatrixXd STM2(6,6);
    MatrixXd STM2inv(6,6);

    for (i=0;i<3;i++) // rows
    {
        for(j=0;j<6;j++)  //cols
        {
            // centered finite differences
            STM2(i,j) = (FWPerturbedFinalStates[j].at(0).position(i)-BWPerturbedFinalStates[j].at(0).position(i))/(2*Perturbations(j,0));
            STM2(i+3,j) = (FWPerturbedFinalStates[j].at(0).velocity(i)-BWPerturbedFinalStates[j].at(0).velocity(i))/(2*Perturbations(j,0));
        }
    }
    STM2inv = m_covariance[0][0].matInverse(STM2);

    for(int step=0; step < TimeStepsIdx.size(); step++)
    {
        for (i=0;i<3;i++) // rows
        {
            for(j=0;j<6;j++)  //cols
            {
                // centered finite differences
                STM1(i,j) = (FWPerturbedFinalStates[j].at(step).position(i)-BWPerturbedFinalStates[j].at(step).position(i))/(2*Perturbations(j,0));
                STM1(i+3,j) = (FWPerturbedFinalStates[j].at(step).velocity(i)-BWPerturbedFinalStates[j].at(step).velocity(i))/(2*Perturbations(j,0));

            }
        }

        m_STM[step] = STM1 * STM2inv;
        //std::cout << "In actanalysis computeSTM" << std::endl;
        //std::cout <<std::endl << STM1 * STM2inv <<std::endl; // *****************************//

    }
    m_stmcomputed=true;
}

void ActAnalysis::propagateVehicle(PropagationFeedback& feedback, const sta::StateVector& perturbedIC, QList<double>& sampleTimes, QList<sta::StateVector>& samples)
{
    if (isSC()) //SC
    {
        if (dynamic_cast<ScenarioLoiteringType*>(m_trajectory.data()))    // Loitering
        {
            ScenarioLoiteringType* loitering = dynamic_cast<ScenarioLoiteringType*>(m_trajectory.data());
            QString propagator = loitering->PropagationPosition()->propagator();
            ScenarioKeplerianElementsType* oldElements = new ScenarioKeplerianElementsType;
            ScenarioStateVectorType* oldStateVector = new ScenarioStateVectorType;

            QSharedPointer<ScenarioAbstract6DOFPositionType> oldInitialCondition = loitering->InitialPosition()->Abstract6DOFPosition();

            if (propagator == "TWO BODY")
            {
                oldElements->setSemiMajorAxis(dynamic_cast<ScenarioKeplerianElementsType*>(oldInitialCondition.data())->semiMajorAxis());
                oldElements->setEccentricity(dynamic_cast<ScenarioKeplerianElementsType*>(oldInitialCondition.data())->eccentricity());
                oldElements->setInclination(dynamic_cast<ScenarioKeplerianElementsType*>(oldInitialCondition.data())->inclination());
                oldElements->setRAAN(dynamic_cast<ScenarioKeplerianElementsType*>(oldInitialCondition.data())->RAAN());
                oldElements->setArgumentOfPeriapsis(dynamic_cast<ScenarioKeplerianElementsType*>(oldInitialCondition.data())->argumentOfPeriapsis());
                oldElements->setTrueAnomaly(dynamic_cast<ScenarioKeplerianElementsType*>(oldInitialCondition.data())->trueAnomaly());

                ScenarioKeplerianElementsType* elements = new ScenarioKeplerianElementsType;

                QString centralBodyName = loitering->Environment()->CentralBody()->Name();
                StaBody* centralBody = STA_SOLAR_SYSTEM->lookup(centralBodyName);
                sta::KeplerianElements initialStateKeplerian = cartesianTOorbital(centralBody->mu(), perturbedIC);
                //qDebug() << initialStateKeplerian.Inclination << endl;
                elements->setSemiMajorAxis(initialStateKeplerian.SemimajorAxis);
                elements->setEccentricity(initialStateKeplerian.Eccentricity);
                elements->setInclination(sta::radToDeg(initialStateKeplerian.Inclination));
                elements->setRAAN(sta::radToDeg(initialStateKeplerian.AscendingNode));
                elements->setArgumentOfPeriapsis(sta::radToDeg(initialStateKeplerian.ArgumentOfPeriapsis));
                elements->setTrueAnomaly(sta::radToDeg(initialStateKeplerian.TrueAnomaly));
                loitering->InitialPosition()->setAbstract6DOFPosition(QSharedPointer<ScenarioAbstract6DOFPositionType>(elements));
                //QDateTime myStarDateTime = sta::JdToCalendar(sta::MjdToJd(theLastSampleTime));
                //loitering->TimeLine()->setStartTime(myStarDateTime);
            }
            else
            {
                oldStateVector->setX(dynamic_cast<ScenarioStateVectorType*>(oldInitialCondition.data())->x());
                oldStateVector->setY(dynamic_cast<ScenarioStateVectorType*>(oldInitialCondition.data())->y());
                oldStateVector->setZ(dynamic_cast<ScenarioStateVectorType*>(oldInitialCondition.data())->z());
                oldStateVector->setVx(dynamic_cast<ScenarioStateVectorType*>(oldInitialCondition.data())->vx());
                oldStateVector->setVy(dynamic_cast<ScenarioStateVectorType*>(oldInitialCondition.data())->vy());
                oldStateVector->setVz(dynamic_cast<ScenarioStateVectorType*>(oldInitialCondition.data())->vz());

                ScenarioStateVectorType* stateVector = new ScenarioStateVectorType();

                stateVector->setX(perturbedIC.position(0));
                stateVector->setY(perturbedIC.position(1));
                stateVector->setZ(perturbedIC.position(2));
                stateVector->setVx(perturbedIC.velocity(0));
                stateVector->setVy(perturbedIC.velocity(1));
                stateVector->setVz(perturbedIC.velocity(2));
                //qDebug() << stateVector->x() << stateVector->y() << stateVector->z() << endl;
                loitering->InitialPosition()->setAbstract6DOFPosition(QSharedPointer<ScenarioAbstract6DOFPositionType>(stateVector));
                //QDateTime myStarDateTime = sta::JdToCalendar(sta::MjdToJd(theLastSampleTime));
                //loitering->TimeLine()->setStartTime(myStarDateTime);
            }


            PropagateLoiteringTrajectory(loitering, sampleTimes, samples, feedback);

            if (propagator == "TWO BODY")
            {
                loitering->InitialPosition()->setAbstract6DOFPosition(QSharedPointer<ScenarioAbstract6DOFPositionType>(oldElements));
            }
            else
            {
                loitering->InitialPosition()->setAbstract6DOFPosition(QSharedPointer<ScenarioAbstract6DOFPositionType>(oldStateVector));
            }

        } ///////// End of loitering IF
        else if (dynamic_cast<ScenarioDeltaVType*>(m_trajectory.data()))    // DeltaVs
        {
            ScenarioDeltaVType* deltaV = dynamic_cast<ScenarioDeltaVType*>(m_trajectory.data());
            // Calculating direction and magnitude. The magnitude is
            // stored in m/s, and we need to convert to km/s
            double theDeltaVMagnitude = deltaV->Magnitude() * 0.001;

            // Calculating the duration dt (in seconds)
            double startTime = sta::JdToMjd(sta::CalendarToJd(deltaV->TimeLine()->StartTime()));
            double endTime = sta::JdToMjd(sta::CalendarToJd(deltaV->TimeLine()->EndTime()));
            double dt = sta::daysToSecs(endTime - startTime);

            sampleTimes << startTime;
            samples << perturbedIC;

            // Calculating direction and magnitude. The magnitude is
            // stored in m/s, and we need to convert to km/s
            Vector3d dv = Vector3d(deltaV->DeltaVx(), deltaV->DeltaVy(), deltaV->DeltaVz()) * (deltaV->Magnitude() * 0.001);

            // Compute the transformation from the local satellite coordinate system to the
            // coordinate system of the trajectory.
            Vector3d T = perturbedIC.velocity.normalized();
            Vector3d R = -perturbedIC.position.normalized();

            // Compute the rows of the 3x3 transformation matrix
            Vector3d X = T;
            Vector3d Y = (X.cross(R)).normalized();
            Vector3d Z = Y.cross(X);
            Matrix3d M;
            M << X, Y, Z;

            // Transform delta-V
            dv = M * dv;

            // TODO: We should be performing at least two body propagation over the duration
            // of the maneuver.
            sta::StateVector theLastStateVector = perturbedIC;
            theLastStateVector.position += theLastStateVector.velocity * dt;
            theLastStateVector.velocity += dv;



            // Emit the time+state sample
            sampleTimes << endTime;
            samples << theLastStateVector;
        }
    }
    else if (isREV())  // REV
    {
        if (dynamic_cast<ScenarioEntryArcType*>(m_trajectory.data()))
        {
            ScenarioEntryArcType* entry = dynamic_cast<ScenarioEntryArcType*>(m_trajectory.data());

            QSharedPointer<ScenarioAbstract6DOFPositionType> oldInitialCondition = entry->InitialPosition()->Abstract6DOFPosition();

            ScenarioStateVectorType* oldStateVector = new ScenarioStateVectorType;
            oldStateVector->setX(dynamic_cast<ScenarioStateVectorType*>(oldInitialCondition.data())->x());
            oldStateVector->setY(dynamic_cast<ScenarioStateVectorType*>(oldInitialCondition.data())->y());
            oldStateVector->setZ(dynamic_cast<ScenarioStateVectorType*>(oldInitialCondition.data())->z());
            oldStateVector->setVx(dynamic_cast<ScenarioStateVectorType*>(oldInitialCondition.data())->vx());
            oldStateVector->setVy(dynamic_cast<ScenarioStateVectorType*>(oldInitialCondition.data())->vy());
            oldStateVector->setVz(dynamic_cast<ScenarioStateVectorType*>(oldInitialCondition.data())->vz());


            sta::StateVector TmpFixed;
            sta::CoordinateSystem reference(entry->InitialPosition()->CoordinateSystem());

            // ----------------   check coordinate type
            // ----------------   convert from inertial to fixed
            if(reference.type() == sta::COORDSYS_BODYFIXED)
            {
                TmpFixed = reference.fromEmeJ2000(perturbedIC, STA_SOLAR_SYSTEM->lookup(entry->Environment()->CentralBody()->Name()), sta::JdToMjd(sta::CalendarToJd(entry->TimeLine()->StartTime())));
            }
            else
            {
                TmpFixed = perturbedIC;
            }



            // ----------------


            // ---------------- convert from cartesian to spherical according to entry settings

            if (dynamic_cast<ScenarioSphericalCoordinatesType*>(entry->InitialPosition()->Abstract6DOFPosition().data()))
            {
                ScenarioSphericalCoordinatesType* sphericalCoord = new ScenarioSphericalCoordinatesType;

                // convert to spherical
                double r,v,tau,delta,gamma,chi;

                cartesianTOspherical(TmpFixed.position(0),TmpFixed.position(1),TmpFixed.position(2),TmpFixed.velocity(0),TmpFixed.velocity(1),TmpFixed.velocity(2), tau, delta, r, v, gamma, chi);
                sphericalCoord->setRadialDistance(r);
                sphericalCoord->setFlightPathVelocity(v);
                sphericalCoord->setHeadingAngle(chi);
                sphericalCoord->setFlightPathAngle(gamma);
                sphericalCoord->setLatitude(delta);
                sphericalCoord->setLongitude(tau);

                entry->InitialPosition()->setAbstract6DOFPosition(QSharedPointer<ScenarioAbstract6DOFPositionType>(sphericalCoord));

            }
            else
            {
                ScenarioStateVectorType* stateVector = new ScenarioStateVectorType();
                stateVector->setX(TmpFixed.position(0));
                stateVector->setY(TmpFixed.position(1));
                stateVector->setZ(TmpFixed.position(2));
                stateVector->setVx(TmpFixed.velocity(0));
                stateVector->setVy(TmpFixed.velocity(1));
                stateVector->setVz(TmpFixed.velocity(2));
                entry->InitialPosition()->setAbstract6DOFPosition(QSharedPointer<ScenarioAbstract6DOFPositionType>(stateVector));
            }
            // ----------------



            PropagateEntryTrajectory(m_entryvehicle, entry, sampleTimes, samples, feedback);

            entry->InitialPosition()->setAbstract6DOFPosition(QSharedPointer<ScenarioAbstract6DOFPositionType>(oldStateVector));


        }
    }
}
