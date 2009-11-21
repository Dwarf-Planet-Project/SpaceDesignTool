/*
 This program is free software; you can redistribute it and/or modify it under
 the terms of the GNU Lesser General Public License as published by the Free Software
 Foundation; either version 2 of the License, or (at your option) any later
 version.

 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more
 details.

 You should have received a copy of the GNU Lesser General Public License along with
 this program; if not, write to the Free Software Foundation, Inc., 59 Temple
 Place - Suite 330, Boston, MA 02111-1307, USA.
 Further information about the GNU Lesser General Public License can also be found on
 the world wide web at http://www.gnu.org.
 */

/*
 ------ Copyright (C) 2008 European Space Agency (space.trajectory.analysis AT gmail.com) ----
 ------------------ Author: Chris Laurel  -------------------------------------------------
 ------------------ E-mail: (claurel@gmail.com) ----------------------------
 Modified by Tiziana Sabatini on July 2009 to make it consistent with the new perturbations layer
 */

#include "scenarioreentry.h"
#include "scenarioreentrymode.h"
#include "Entry/EntryTrajectory.h"
#include "scenarioreentryproperties.h"
#include "scenariotrajectorypropagation.h"
#include "scenarioenvironment.h"
#include "scenariosimulationparameters.h"
#include "scenariobody.h"
#include "scenarioinitialstate.h"
#include "Astro-Core/cartesianTOspherical.h"
#include "Astro-Core/fixedTOinertial.h"
#include "Astro-Core/getGreenwichHourAngle.h"
#include <QTreeWidgetItem>
#include "Astro-Core/date.h"
#include <QDebug>
#include <QFile>
#include "Eigen/Geometry"

using namespace Eigen;

ScenarioReEntryTrajectory::ScenarioReEntryTrajectory() :
    m_environment(NULL),
    m_simulationMode(NULL),
    //m_windowMode(NULL),
    m_trajectoryPropagation(NULL)
{
    m_environment = new ScenarioEnvironment();
    attachChild(m_environment);

    m_simulationMode = new ScenarioSimulationMode();
    m_simulationMode->setIsActiveTrue();
    attachChild(m_simulationMode);

    //m_windowMode = new ScenarioWindowMode();
    //m_windowMode->setIsActiveFalse();
    //attachChild(m_windowMode);

    m_trajectoryPropagation = new ScenarioTrajectoryPropagation();
    attachChild(m_trajectoryPropagation);
}


void
ScenarioReEntryTrajectory::writeContents(QXmlStreamWriter* out)
{
    if (m_environment)
        m_environment->writeElement(out);
    if (m_simulationMode->isActive())
        m_simulationMode->writeElement(out);
//    if (m_windowMode->isActive())
//        m_windowMode->writeElement(out);
    if (m_trajectoryPropagation)
        m_trajectoryPropagation->writeElement(out);
}


void
ScenarioReEntryTrajectory::createItemContents(QTreeWidgetItem* item)
{

    if (m_environment)
        m_environment->createItem(item);
    if (m_simulationMode)
        m_simulationMode->createItem(item);
//    if (m_windowMode)
//        m_windowMode->createItem(item);
    if (m_trajectoryPropagation)
        m_trajectoryPropagation->createItem(item);

}


sta::StateVector
ScenarioReEntryTrajectory::computeInitialStateVector() const
{
    ScenarioTargettingSimulationParameters *simulationParameters = m_simulationMode->simulationParameters();
    return simulationParameters->initialStatePosition()->computeStateVector(environment()->centralBody()->body());
}


void
ScenarioReEntryTrajectory::setEnvironment(ScenarioEnvironment* environment)
{
    if (environment != m_environment)
    {
        detachChild(m_environment);
        m_environment = environment;
        attachChild(m_environment);
    }
}


void
ScenarioReEntryTrajectory::setSimulationMode(ScenarioSimulationMode* mode)
{
    if (mode != m_simulationMode)
    {
        detachChild(m_simulationMode);
        m_simulationMode = mode;
        attachChild(m_simulationMode);
    }
}


//void
//ScenarioReEntryTrajectory::setWindowMode(ScenarioWindowMode* mode)
//{
//    if (mode != m_windowMode)
//    {
//        detachChild(m_windowMode);
//        m_windowMode = mode;
//        attachChild(m_windowMode);
//    }
//}


void
ScenarioReEntryTrajectory::setTrajectoryPropagation(ScenarioTrajectoryPropagation* trajectoryPropagation)
{
    if (trajectoryPropagation != m_trajectoryPropagation)
    {
        detachChild(m_trajectoryPropagation);
        m_trajectoryPropagation = trajectoryPropagation;
        attachChild(m_trajectoryPropagation);
    }
}

void
ScenarioReEntryTrajectory::writeElement(QXmlStreamWriter* out)
{
    out->writeStartElement(elementName());
    //out->writeAttribute("Name", name());
    writeContents(out);
    out->writeEndElement();
}

sta::StateVector
ScenarioReEntryTrajectory::propagate(PropagationFeedback& propFeedback,
                                     const sta::StateVector& initialState,
                                     QList<double>& sampleTimes,
                                     QList<sta::StateVector>& samples)
{
    ScenarioSpaceVehicle* spacevehicle = dynamic_cast<ScenarioSpaceVehicle*>(this->parent()->parent());
    ScenarioProperties* vehicleproperties= spacevehicle->properties();
    EntrySettings inputSettings = createEntrySettings(this, vehicleproperties);
    EntryTrajectory trajectory(inputSettings);

    //Removing atmosphericDrag from the list of perturbations, as it is considered as a major force.
    QList<Perturbations*> perturbationsList = environment()->createListPerturbations(vehicleproperties);
    foreach (Perturbations* perturbation, perturbationsList)
    {
        if (dynamic_cast<AtmosphericDragPerturbations*>(perturbation))
            perturbationsList.removeOne(perturbation);
    }

    if (simulationMode()->isActive())
    {
        const double startTime = simulationMode()->simulationParameters()->timeline()->startTime();
        double time_jd = sta::MjdToJd(startTime);
        double dt = trajectoryPropagation()->timeStep();

        double time_s = sta::daysToSecs(startTime);
        EntryParameters parameters = createEntryParametersSimulation(this, vehicleproperties);

        //Message errors on input parameters :
        if (dt == 0.0)
        {
            propFeedback.raiseError(QObject::tr("Time step is zero!"));
            return initialState;
        }
        if (parameters.m == 0.0)
        {
            propFeedback.raiseError(QObject::tr("Vehicle mass is zero!"));
            return initialState;
        }
        if (parameters.R == 0.0)
        {
            propFeedback.raiseError(QObject::tr("Vehicle base radius is zero!"));
            return initialState;
        }
        if (inputSettings.maxheatrate == 0)
        {
            propFeedback.raiseError(QObject::tr("The maximum heat rate is zero!"));
            return initialState;
        }
        if (inputSettings.maxloadfactor == 0)
        {
            propFeedback.raiseError(QObject::tr("The maximum load factor is zero!"));
            return initialState;
        }
        if (trajectory.capsule.flag == 1)
        {
            propFeedback.raiseError(QObject::tr("Cd coefficient file name is not valid!"));
            return initialState;
        }
        if (trajectory.capsule.flag == 2)
        {
            propFeedback.raiseError(QObject::tr("Cd parachute coefficient file name is not valid!"));
            return initialState;
        }
        sta::StateVector stateVector;
        stateVector = trajectory.initialise(parameters, initialState, time_jd);
        _status status = OK;

        while (status == OK)
        {
            stateVector = trajectory.integrate(stateVector, perturbationsList);
            status = trajectory.status;

            time_jd += sta::secsToDays(dt);
            double theta = getGreenwichHourAngle(time_jd);
            time_s += dt;
            JulianDate jd = sta::secsToDays(time_s);
            sampleTimes << jd;

            double posx, posy, posz, velx, vely, velz;
            int celestialbody;
            if (inputSettings.bodyname == "Earth")
                celestialbody = 0;
            else if (inputSettings.bodyname == "Mars")
                celestialbody = 3;
            fixedTOinertial(celestialbody, theta , stateVector.position.x(), stateVector.position.y(), stateVector.position.z(), stateVector.velocity.x(), stateVector.velocity.y(), stateVector.velocity.z(),
                posx, posy, posz, velx, vely, velz);

            Eigen::Vector3d pos = Vector3d(posx/1000, posy/1000, posz/1000);
            Eigen::Vector3d vel = Vector3d(velx/1000, vely/1000, velz/1000);
            sta::StateVector statevector = sta::StateVector(pos, vel);
            samples << statevector;
        }
        //trajectory.printReport();
        return stateVector;
    }

//    else if (windowMode()->isActive())
//    {
//        qDebug() << "I'm propagating the window mode";
//        EntryParameterIntervals parameterIntervals = reentryValues.createEntryParameterIntervals(this);
//        EntryParameters parameters = reentryValues.createEntryParametersWindow(this, parameterIntervals);
//        const ScenarioTimeline* timeline = windowMode()->simulationParameters()->timeline();
//        double startTime = sta::daysToSecs(timeline->startTime());
//        trajectory.initialise(parameters, startTime);
//
//        int InputNsimulations, Npoints, Nsimulations;
//        double cd = 1.5;
//        bool failed_target = false;
//
//        InputNsimulations = windowMode()->numSimulations();//800
//        Npoints = int ( round (pow(InputNsimulations,0.5)) );
//        Nsimulations = Npoints * Npoints;
//        angle TargetLatitude = reentryValues.createTargetAngleLatitudeWindow(this);
//        angle TargetLongitude = reentryValues.createTargetAngleLongitudeWindow(this);
//
//        Endstate endresult = trajectory.getEndstate();
//
//        qDebug() << endresult.status;
//
//        if (endresult.status == landed) {
//        double difflong, difflat;
//                do {
//                    //Initialise the entry trajectory
//                    trajectory.initialise(parameters, startTime);
//
//
//                    //Get the landing location
//                    Endstate endresult = trajectory.getEndstate();
//                    if (endresult.status != landed) {
//                        cout << "Target not reached with middle point trajectory, entering at (0, 0)" << endl;
//                        parameters.inputstate[1] = 0.0;
//                        parameters.inputstate[2] = 0.0;
//                        failed_target = true;
//                        qDebug() << failed_target;
//                        break;
//                    }
//                    //Update the entry location
//                    difflong = endresult.longitude - windowMode()->simulationParameters()->finalState()->targetLon();
//                    difflat = endresult.latitude - windowMode()->simulationParameters()->finalState()->targetLat();;
//                    parameters.inputstate[1] -= difflong;
//                    parameters.inputstate[2] -= difflat;
//
//                } while (abs(difflong) > 0.0001 || abs(difflat) > 0.0001);
//                if (!failed_target) {
//                    qDebug() << "Target reached with middle point trajectory, entering at (" << parameters.inputstate[1] << "," << parameters.inputstate[2] << ")";
//                }
//    }
//    else {
//        qDebug() << "Target not reached with middle point trajectory, entering at (0, 0)";
//        parameters.inputstate[1] = 0.0;
//        parameters.inputstate[2] = 0.0;
//    }
//        //Open the output file
//    //--------------------
//    ofstream window_output ("C:/Users/Tizy/Desktop/Code/trunk/sta-src/Entry/REMWindowOutput.stap");
//    //--------------------
//    qDebug() << "window1";
//    //Create the matrices to store the results
//    //----------------------------------------
//    double * xArray = new double [Npoints];
//    double * yArray = new double [Npoints];
//    TNT::Array2D< double > MaxCqdotMatrix(Npoints, Npoints);
//    TNT::Array2D< double > MaxCqdotAltMatrix(Npoints, Npoints);
//    TNT::Array2D< double > MaxCqdotTimeMatrix(Npoints, Npoints);
//    TNT::Array2D< double > MaxRqdotMatrix(Npoints, Npoints);
//    TNT::Array2D< double > MaxRqdotAltMatrix(Npoints, Npoints);
//    TNT::Array2D< double > MaxRqdotTimeMatrix(Npoints, Npoints);
//    TNT::Array2D< double > MaxTqdotMatrix(Npoints, Npoints);
//    TNT::Array2D< double > MaxTqdotAltMatrix(Npoints, Npoints);
//    TNT::Array2D< double > MaxTqdotTimeMatrix(Npoints, Npoints);
//    TNT::Array2D< double > MaxGMatrix(Npoints, Npoints) ;
//    TNT::Array2D< double > MaxGAltMatrix(Npoints, Npoints);
//    TNT::Array2D< double > MaxGTimeMatrix(Npoints, Npoints);
//    TNT::Array2D< double > Mach1AltMatrix(Npoints, Npoints);
//    TNT::Array2D< double > Mach1TimeMatrix(Npoints, Npoints);
//    TNT::Array2D< double > DeployAltMatrix(Npoints, Npoints);
//    TNT::Array2D< double > DeployTimeMatrix(Npoints, Npoints);
//    TNT::Array2D< double > TotalQMatrix(Npoints, Npoints);
//    TNT::Array2D< double > TPSMassMatrix(Npoints, Npoints);
//    TNT::Array2D< double > TofMatrix(Npoints, Npoints);
//    TNT::Array2D< double > ImpactVMatrix(Npoints, Npoints);
//    TNT::Array2D< double > StatusMatrix(Npoints, Npoints);
//    //----------------------------------------
//
//    //Create the parameter arrays and fill them
//    //-----------------------------------------
//    double * KArray = new double[Nsimulations];
//    double * RnArray = new double[Nsimulations];
//    double * VArray = new double[Nsimulations];
//    double * gammaArray = new double[Nsimulations];
//    double * chiArray = new double[Nsimulations];
//    int xparameter = 2;//windowMode()->window()->xAxisVariable()->returnType();
//    int yparameter = 4;//windowMode()->window()->yAxisVariable()->returnType();
//    //------
//    switch (xparameter) {
//        case 1: for (int i = 0; i < Npoints; i++) { xArray[i] = parameterIntervals.K.low + ( i * (parameterIntervals.K.high-parameterIntervals.K.low) / (Npoints-1) );} break;
//        case 2: for (int i = 0; i < Npoints; i++) { xArray[i] = parameterIntervals.Rn.low + ( i * (parameterIntervals.Rn.high-parameterIntervals.Rn.low) / (Npoints-1) );}; break;
//        case 3: for (int i = 0; i < Npoints; i++) { xArray[i] = parameterIntervals.velocity.low + ( i * (parameterIntervals.velocity.high-parameterIntervals.velocity.low) / (Npoints-1) );}; break;
//        case 4: for (int i = 0; i < Npoints; i++) { xArray[i] = parameterIntervals.gamma.low + ( i * (parameterIntervals.gamma.high-parameterIntervals.gamma.low) / (Npoints-1) );}; break;
//        case 5: for (int i = 0; i < Npoints; i++) { xArray[i] = parameterIntervals.chi.low + ( i * (parameterIntervals.chi.high-parameterIntervals.chi.low) / (Npoints-1) );}; break;
//    }
//    switch (yparameter) {
//        case 1: for (int i = 0; i < Npoints; i++) { yArray[i] = parameterIntervals.K.high - ( i * (parameterIntervals.K.high-parameterIntervals.K.low) / (Npoints-1) );}; break;
//        case 2: for (int i = 0; i < Npoints; i++) { yArray[i] = parameterIntervals.Rn.high - ( i * (parameterIntervals.Rn.high-parameterIntervals.Rn.low) / (Npoints-1) );}; break;
//        case 3: for (int i = 0; i < Npoints; i++) { yArray[i] = parameterIntervals.velocity.high - ( i * (parameterIntervals.velocity.high-parameterIntervals.velocity.low) / (Npoints-1) );}; break;
//        case 4: for (int i = 0; i < Npoints; i++) { yArray[i] = parameterIntervals.gamma.high - ( i * (parameterIntervals.gamma.high-parameterIntervals.gamma.low) / (Npoints-1) );}; break;
//        case 5: for (int i = 0; i < Npoints; i++) { yArray[i] = parameterIntervals.chi.high - ( i * (parameterIntervals.chi.high-parameterIntervals.chi.low) / (Npoints-1) );}; break;
//    }
//    qDebug() << "window2";
//    //------
//    if (xparameter == 1) { for (int i = 0; i < Npoints; i++){ for (int j = 0; j < Npoints; j++) KArray[i*Npoints + j] = xArray[i];}}
//    else if (yparameter == 1) { for (int i = 0; i < Npoints; i++){ for (int j = 0; j < Npoints; j++) KArray[i*Npoints + j] = yArray[j];}}
//    else for (int i = 0; i < Nsimulations; i++) KArray[i] = parameterIntervals.K.low;
//    //------
//    if (xparameter == 2) { for (int i = 0; i < Npoints; i++){ for (int j = 0; j < Npoints; j++) RnArray[i*Npoints + j] = xArray[i];}}
//    else if (yparameter == 2) { for (int i = 0; i < Npoints; i++){ for (int j = 0; j < Npoints; j++) RnArray[i*Npoints + j] = yArray[j];}}
//    else for (int i = 0; i < Nsimulations; i++) RnArray[i] = parameterIntervals.Rn.low;
//    //------
//    if (xparameter == 3) { for (int i = 0; i < Npoints; i++){ for (int j = 0; j < Npoints; j++) VArray[i*Npoints + j] = xArray[i];}}
//    else if (yparameter == 3) { for (int i = 0; i < Npoints; i++){ for (int j = 0; j < Npoints; j++) VArray[i*Npoints + j] = yArray[j];}}
//    else for (int i = 0; i < Nsimulations; i++) VArray[i] = parameterIntervals.velocity.low;
//    //------
//    if (xparameter == 4) { for (int i = 0; i < Npoints; i++){ for (int j = 0; j < Npoints; j++) gammaArray[i*Npoints + j] = xArray[i];}}
//    else if (yparameter == 4) { for (int i = 0; i < Npoints; i++){ for (int j = 0; j < Npoints; j++) gammaArray[i*Npoints + j] = yArray[j];}}
//    else for (int i = 0; i < Nsimulations; i++) gammaArray[i] = parameterIntervals.gamma.low;
//    //------
//    if (xparameter == 5) { for (int i = 0; i < Npoints; i++){ for (int j = 0; j < Npoints; j++) chiArray[i*Npoints + j] = xArray[i];}}
//    else if (yparameter == 5) { for (int i = 0; i < Npoints; i++){ for (int j = 0; j < Npoints; j++) chiArray[i*Npoints + j] = yArray[j];}}
//    else for (int i = 0; i < Nsimulations; i++) chiArray[i] = parameterIntervals.chi.low;
//    //-------------------------------------------------------------------------
//qDebug() << "window3";
//
//        //Main loop
//        //=====================
//        qDebug() << "Number of steps: " << Npoints;
//        for (int xstep = 0; xstep < Npoints; xstep++) {
//
//            for (int ystep = 0; ystep < Npoints; ystep++) {
//
//                //Make the parameter structure
//                //----------------------------
//                parameters.m = 100.0;
//                double localK = KArray[xstep * Npoints + ystep];
//                parameters.R = sqrt( parameters.m/ (localK * acos(-1) * cd) );
//                parameters.Rn = RnArray [xstep * Npoints + ystep];
//                parameters.inputstate[3] = VArray [xstep * Npoints + ystep];
//                parameters.inputstate[4] = gammaArray [xstep * Npoints + ystep];
//                parameters.inputstate[5] = chiArray [xstep * Npoints + ystep];
//                //----------------------------
//
//                //Initialise the entry trajectory
//                //-----------------------------------------
//                trajectory.initialise(parameters, startTime);
//                //-----------------------------------------
//
//                //Get the endstate of the trajectory
//                //----------------------------------
//                Endstate endresult = trajectory.getEndstate();
//                //----------------------------------
//
//                //Fill the output matrices
//                //------------------------
//                MaxCqdotMatrix[xstep][ystep] = endresult.maxconvheatrate.value * (1 - endresult.status);
//                MaxCqdotAltMatrix[xstep][ystep] = endresult.maxconvheatrate.altitude * (1 - endresult.status);
//                MaxCqdotTimeMatrix[xstep][ystep] = endresult.maxconvheatrate.time * (1 - endresult.status);
//                MaxRqdotMatrix[xstep][ystep] = endresult.maxradheatrate.value * (1 - endresult.status);
//                MaxRqdotAltMatrix[xstep][ystep] = endresult.maxradheatrate.altitude * (1 - endresult.status);
//                MaxRqdotTimeMatrix[xstep][ystep] = endresult.maxradheatrate.time * (1 - endresult.status);
//                MaxTqdotMatrix[xstep][ystep] = endresult.maxtotalheatrate.value * (1 - endresult.status);
//                MaxTqdotAltMatrix[xstep][ystep] = endresult.maxtotalheatrate.altitude * (1 - endresult.status);
//                MaxTqdotTimeMatrix[xstep][ystep] = endresult.maxtotalheatrate.time * (1 - endresult.status);
//                MaxGMatrix[xstep][ystep] = endresult.maxloadfactor.value * (1 - endresult.status);
//                MaxGAltMatrix[xstep][ystep] = endresult.maxloadfactor.value * (1 - endresult.status);
//                MaxGTimeMatrix[xstep][ystep] = endresult.maxloadfactor.value * (1 - endresult.status);
//                Mach1AltMatrix[xstep][ystep] = endresult.Machone_transition_altitude * (1 - endresult.status);
//                Mach1TimeMatrix[xstep][ystep] = endresult.Machone_transition_time * (1 - endresult.status);
//                DeployAltMatrix[xstep][ystep] = endresult.parachutedeploy_altitude * (1 - endresult.status);
//                DeployTimeMatrix[xstep][ystep] = endresult.parachutedeploy_time * (1 - endresult.status);
//                TotalQMatrix[xstep][ystep] = endresult.totalheatload * (1 - endresult.status);
//                TPSMassMatrix[xstep][ystep] = endresult.tpsmass * (1 - endresult.status);
//                TofMatrix[xstep][ystep] = endresult.timeofflight * (1 - endresult.status);
//                ImpactVMatrix[xstep][ystep] = endresult.impactvelocity * (1 - endresult.status);
//                StatusMatrix[xstep][ystep] = endresult.status;
//                //-----------------
//            }
//qDebug() << "window4";
//            //Give feedback about progress
//            //----------------------------
//            qDebug() << xstep+1;
//            //----------------------------
//        }
//        //=====================
//
//
//        //Write the output file
//        //---------------------
//        for (int i=0; i<Npoints; i++) {
//            window_output << xArray[i] << "     ";
//        }
//        window_output << "\n";
//        //------
//        for (int i=0; i<Npoints; i++) {
//            window_output << yArray[i] << "     ";
//        }
//        window_output << "\n";
//        //-----------------------------------------------------
//        for (int j=0; j<Npoints; j++) {for (int i=0; i<Npoints; i++) {window_output << MaxCqdotMatrix[i][j] << "     ";} window_output << "\n";}
//        for (int j=0; j<Npoints; j++) {for (int i=0; i<Npoints; i++) {window_output << MaxCqdotAltMatrix[i][j] << "     ";} window_output << "\n";}
//        for (int j=0; j<Npoints; j++) {for (int i=0; i<Npoints; i++) {window_output << MaxCqdotTimeMatrix[i][j] << "     ";} window_output << "\n";}
//        for (int j=0; j<Npoints; j++) {for (int i=0; i<Npoints; i++) {window_output << MaxRqdotMatrix[i][j] << "     ";} window_output << "\n";}
//        for (int j=0; j<Npoints; j++) {for (int i=0; i<Npoints; i++) {window_output << MaxRqdotAltMatrix[i][j] << "     ";} window_output << "\n";}
//        for (int j=0; j<Npoints; j++) {for (int i=0; i<Npoints; i++) {window_output << MaxRqdotTimeMatrix[i][j] << "     ";} window_output << "\n";}
//        for (int j=0; j<Npoints; j++) {for (int i=0; i<Npoints; i++) {window_output << MaxTqdotMatrix[i][j] << "     ";} window_output << "\n";}
//        for (int j=0; j<Npoints; j++) {for (int i=0; i<Npoints; i++) {window_output << MaxTqdotAltMatrix[i][j] << "     ";} window_output << "\n";}
//        for (int j=0; j<Npoints; j++) {for (int i=0; i<Npoints; i++) {window_output << MaxTqdotTimeMatrix[i][j] << "     ";} window_output << "\n";}
//        for (int j=0; j<Npoints; j++) {for (int i=0; i<Npoints; i++) {window_output << MaxGMatrix[i][j] << "     ";} window_output << "\n";}
//        for (int j=0; j<Npoints; j++) {for (int i=0; i<Npoints; i++) {window_output << MaxGAltMatrix[i][j] << "     ";} window_output << "\n";}
//        for (int j=0; j<Npoints; j++) {for (int i=0; i<Npoints; i++) {window_output << MaxGTimeMatrix[i][j] << "     ";} window_output << "\n";}
//        for (int j=0; j<Npoints; j++) {for (int i=0; i<Npoints; i++) {window_output << Mach1AltMatrix[i][j] << "     ";} window_output << "\n";}
//        for (int j=0; j<Npoints; j++) {for (int i=0; i<Npoints; i++) {window_output << Mach1TimeMatrix[i][j] << "     ";} window_output << "\n";}
//        for (int j=0; j<Npoints; j++) {for (int i=0; i<Npoints; i++) {window_output << DeployAltMatrix[i][j] << "     ";} window_output << "\n";}
//        for (int j=0; j<Npoints; j++) {for (int i=0; i<Npoints; i++) {window_output << DeployTimeMatrix[i][j] << "     ";} window_output << "\n";}
//        for (int j=0; j<Npoints; j++) {for (int i=0; i<Npoints; i++) {window_output << TotalQMatrix[i][j] << "     ";} window_output << "\n";}
//        for (int j=0; j<Npoints; j++) {for (int i=0; i<Npoints; i++) {window_output << TPSMassMatrix[i][j] << "     ";} window_output << "\n";}
//        for (int j=0; j<Npoints; j++) {for (int i=0; i<Npoints; i++) {window_output << TofMatrix[i][j] << "     ";} window_output << "\n";}
//        for (int j=0; j<Npoints; j++) {for (int i=0; i<Npoints; i++) {window_output << ImpactVMatrix[i][j] << "     ";} window_output << "\n";}
//        for (int j=0; j<Npoints; j++) {for (int i=0; i<Npoints; i++) {window_output << StatusMatrix[i][j] << "     ";} window_output << "\n";}
//        qDebug() << "window5";//-----------------------------------------------------
//        window_output.close();
//        //----------------------
//    }
//    //    //Dispersion analysis mode ++++++++++++++++++++++++++++
//    if (simulationMode()->dispersionAnalysis()->isActive())
//    {
//        //Set the seed of the random number generator
//        srand ( time(NULL) );
//
//        Uncertainties_struct inputuncertainties = reentryValues.createUncertainties_struct(this);
//        EntryParameters parameters;
//        //ifstream dispersion_input ("REMDispersionInput.stap");
//        int Nsimulations;
//        double inputstate_temp[6];
//        double mass_temp;
//        Nsimulations = simulationMode()->dispersionAnalysis()->numSimulations();
//        parameters.R = vehicle()->mainBodyCharacteristics()->radiusBase();
//        mass_temp = vehicle()->mainBodyCharacteristics()->mass();
//        parameters.Rn = vehicle()->mainBodyCharacteristics()->radiusNose();
//
//        const ScenarioTimeline* timeline = simulationMode()->simulationParameters()->timeline();
//        double startTime = sta::daysToSecs(timeline->startTime());
//
//        QString coordinatesystem = simulationMode()->simulationParameters()->initialStatePosition()->initialState()->elementName();
//
//        if (coordinatesystem=="SphericalCoordinates")
//        {
//            parameters.coordselector = 1;
//            ScenarioSphericalCoordinates* sphericalCoord = dynamic_cast<ScenarioSphericalCoordinates*>(simulationMode()->simulationParameters()->initialStatePosition()->initialState());
//            inputstate_temp[1] = sphericalCoord->altitude();
//            inputstate_temp[2] = sphericalCoord->longitude();
//            inputstate_temp[3] = sphericalCoord->latitude();
//            inputstate_temp[4] = sphericalCoord->inertialVelocity();
//            inputstate_temp[5] = sphericalCoord->inertialFlightPathAngle();
//            inputstate_temp[6] = sphericalCoord->inertialHeading();
//
//        }
//        else if (coordinatesystem=="KeplerianElements")
//        {
//            parameters.coordselector = 2;
//            ScenarioKeplerianElements* elements = dynamic_cast<ScenarioKeplerianElements*>(simulationMode()->simulationParameters()->initialStatePosition()->initialState());
//            inputstate_temp[1] = elements->semimajorAxis();
//            inputstate_temp[2] = elements->eccentricity();
//            inputstate_temp[3] = elements->inclination();
//            inputstate_temp[4] = elements->raan();
//            inputstate_temp[5] = elements->argumentOfPeriapsis();
//            inputstate_temp[6] = elements->trueAnomaly();
//        }
//        else  qFatal("Error in the initial state position coordinate systems");
//
//    angle TargetLongitude, TargetLatitude;
//    AngleConversion angleconversion;
//    double latitudeDecimal = simulationMode()->dispersionAnalysis()->deviations()->entryLat();
//    double longitudeDecimal = simulationMode()->dispersionAnalysis()->deviations()->entryLong();
//    angleconversion.decimalToDegMinSec(latitudeDecimal, TargetLatitude.degrees, TargetLatitude.minutes, TargetLatitude.seconds);
//    angleconversion.decimalToDegMinSec(longitudeDecimal, TargetLongitude.degrees, TargetLongitude.minutes, TargetLongitude.seconds);
//
//
//    //-------------------------------------------
//
//    //Open the output file
//    //--------------------
//    ofstream dispersion_output ("REMDispersionOutput.stap");
//    dispersion_output << longitudeDecimal << "\t" << latitudeDecimal << "\t" << 0 << "\t" << 0 << endl;
//    //--------------------
//
//    //Creat the dynamic arrays
//    //------------------------
//    double * dispersion_longitudes = new double [Nsimulations];
//    double * dispersion_latitudes = new double [Nsimulations];
//    double * dispersion_crosstracks = new double [Nsimulations];
//    double * dispersion_alongtracks = new double [Nsimulations];
//    double maxalongtrack = 0.0, maxcrosstrack = 0.0;
//    //------------------------
//
//    //Main loop
//    //========================
//    for (int i=0; i<Nsimulations; i++) {
//
//        //Create the uncertainties based on the variances
//        //-------------------------------------------
//        parameters.uncertainties.mass = (2 * ((double)rand() / (double)RAND_MAX) * inputuncertainties.mass) - inputuncertainties.mass;
//        parameters.uncertainties.altitude = (2 * ((double)rand() / (double)RAND_MAX) * inputuncertainties.altitude) - inputuncertainties.altitude;
//        parameters.uncertainties.tau = (2 * ((double)rand() / (double)RAND_MAX) * inputuncertainties.tau) - inputuncertainties.tau;
//        parameters.uncertainties.delta = (2 * ((double)rand() / (double)RAND_MAX) * inputuncertainties.delta) - inputuncertainties.delta;
//        parameters.uncertainties.velocity = (2 * ((double)rand() / (double)RAND_MAX) * inputuncertainties.velocity) - inputuncertainties.velocity;
//        parameters.uncertainties.gamma = (2 * ((double)rand() / (double)RAND_MAX) * inputuncertainties.gamma) - inputuncertainties.gamma;
//        parameters.uncertainties.chi = (2 * ((double)rand() / (double)RAND_MAX) * inputuncertainties.chi) - inputuncertainties.chi;
//        parameters.uncertainties.density = (2 * ((double)rand() / (double)RAND_MAX) * inputuncertainties.density) - inputuncertainties.density;
//        parameters.uncertainties.temperature = (2 * ((double)rand() / (double)RAND_MAX) * inputuncertainties.temperature) - inputuncertainties.temperature;
//        parameters.uncertainties.pressure = (2 * ((double)rand() / (double)RAND_MAX) * inputuncertainties.pressure) - inputuncertainties.pressure;
//        parameters.uncertainties.speedofsound = (2 * ((double)rand() / (double)RAND_MAX) * inputuncertainties.speedofsound) - inputuncertainties.speedofsound;
//        parameters.uncertainties.cdcapsule = (2 * ((double)rand() / (double)RAND_MAX) * inputuncertainties.cdcapsule) - inputuncertainties.cdcapsule;
//        parameters.uncertainties.cdparachute = (2 * ((double)rand() / (double)RAND_MAX) * inputuncertainties.cdparachute) - inputuncertainties.cdparachute;
//        //-------------------------------------------
//
//        //Construct the parameter structure
//        //-------------------------------------------
//        parameters.m = mass_temp + parameters.uncertainties.mass;
//        parameters.inputstate[0] = inputstate_temp[0] + parameters.uncertainties.altitude;
//        parameters.inputstate[1] = inputstate_temp[1] + parameters.uncertainties.tau;
//        parameters.inputstate[2] = inputstate_temp[2] + parameters.uncertainties.delta;
//        parameters.inputstate[3] = inputstate_temp[3] + parameters.uncertainties.velocity;
//        parameters.inputstate[4] = inputstate_temp[4] + parameters.uncertainties.gamma;
//        parameters.inputstate[5] = inputstate_temp[5] + parameters.uncertainties.chi;
//        //-------------------------------------------
//
//        //Initialise the entry trajectory
//        //-----------------------------------------
//        trajectory.initialise(parameters, startTime);
//        //-----------------------------------------
//
//        //Get the endstate of the trajectory
//        //----------------------------------
//        Endstate endresult = trajectory.getEndstate();
//        //----------------------------------
//
//        //Write the output file
//        //----------------------------------
//        dispersion_longitudes[i] = endresult.longitude;
//        dispersion_latitudes[i] = endresult.latitude;
//        AngleConversion angleconversion;
//        dispersion_alongtracks[i] = (dispersion_longitudes[i]-angleconversion.DegMinSecToDecimal(TargetLongitude.degrees, TargetLongitude.minutes, TargetLongitude.seconds))*cos((inputstate_temp[5]-90.0)) - (dispersion_latitudes[i]-angleconversion.DegMinSecToDecimal(TargetLatitude.degrees, TargetLatitude.minutes, TargetLatitude.seconds))*sin((inputstate_temp[5]-90.0)) ;//- angle2double(TargetLongitude);
//        dispersion_crosstracks[i] = (dispersion_longitudes[i]-angleconversion.DegMinSecToDecimal(TargetLongitude.degrees, TargetLongitude.minutes, TargetLongitude.seconds))*sin((inputstate_temp[5]-90.0)) + (dispersion_latitudes[i]-angleconversion.DegMinSecToDecimal(TargetLatitude.degrees, TargetLatitude.minutes, TargetLatitude.seconds))*cos((inputstate_temp[5]-90.0)) ;//- angle2double(TargetLatitude);
//        dispersion_output << dispersion_longitudes[i] << "\t" << dispersion_latitudes[i] << "\t" << dispersion_alongtracks[i] << "\t" << dispersion_crosstracks[i] << endl;
//        if (dispersion_alongtracks[i] > maxalongtrack) maxalongtrack = dispersion_alongtracks[i];
//        if (dispersion_crosstracks[i] > maxcrosstrack) maxcrosstrack = dispersion_crosstracks[i];
//        //----------------------------------
//
//        //Give feedback about progress
//        //----------------------------
//        cout << i << endl;
//        //----------------------------
//        }
//        //========================
//    dispersion_output.close();
//
//    //Write out a report
//    //------------------
//    ofstream dispersion_report ("REMDispersionReport.stap");
//    dispersion_report << "Dispersion analysis: report" << endl;
//    dispersion_report << "===========================" << endl << " " << endl;
//    dispersion_report << "Nominal mission parameters" << endl;
//    dispersion_report << "--------------------------" << endl;
//    dispersion_report << "Capsule radius:\t\t   " << parameters.R << " m" << endl;
//    dispersion_report << "Capsule mass:\t\t   " << mass_temp << " kg" << endl;
//    dispersion_report << "Capsule nose radius:\t   " << parameters.Rn << " m" << endl;
//    dispersion_report << "Entry altitude:\t\t   " << inputstate_temp[0] << " m" << endl;
//    dispersion_report << "Entry longitude:\t   " << inputstate_temp[1] << " °" << endl;
//    dispersion_report << "Entry latitude:\t\t   " << inputstate_temp[2] << " °" << endl;
//    dispersion_report << "Entry velocity:\t\t   " << inputstate_temp[3] << " m/s" << endl;
//    dispersion_report << "Entry flight path angle:   " << inputstate_temp[4] << " °" << endl;
//    dispersion_report << "Entry heading:\t\t   " << inputstate_temp[5] << " °" << endl;
//    dispersion_report << "--------------------------" << endl << " " << endl;
//    dispersion_report << "Variances of uncertainties" << endl;
//    dispersion_report << "--------------------------" << endl;
//    dispersion_report << "Capsule mass:\t "  << "+-" << inputuncertainties.mass << " kg" << endl;
//    dispersion_report << "Capsule Cd profile:\t " << "+-" << inputuncertainties.cdcapsule*100 << "%" << endl;
//    dispersion_report << "Parachute Cd profile:\t " << "+-" << inputuncertainties.cdparachute*100 << "%" << endl;
//    dispersion_report << "Entry altitude:\t " << "+-" << inputuncertainties.altitude << " m" << endl;
//    dispersion_report << "Entry longitude:\t " << "+-" << inputuncertainties.tau << " deg" << endl;
//    dispersion_report << "Entry latitude:\t " << "+-" << inputuncertainties.delta << " deg" << endl;
//    dispersion_report << "Entry velocity:\t\t " << "+-" << inputuncertainties.velocity << " m/s" << endl;
//    dispersion_report << "Entry flight path angle:\t " << "+-" << inputuncertainties.gamma << " deg" << endl;
//    dispersion_report << "Entry heading:\t " << "+-" << inputuncertainties.chi << " deg" << endl;
//    dispersion_report << "--------------------------" << endl << " " << endl;
//    dispersion_report << "Dispersion results" << endl;
//    dispersion_report << "--------------------------" << endl;
//    dispersion_report << "Maximum along track dispersion: " << maxalongtrack*111111.11 << "m" << endl;
//    dispersion_report << "Maximum cross track dispersion: " << maxcrosstrack*111111.11 << "m" << endl;
//    dispersion_report << "--------------------------" << endl;
//    //------------------
//    }
//    //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    //return 0;

}


const StaBody*
ScenarioReEntryTrajectory::centralBody() const
{
    return environment()->centralBody()->body();
}

const sta::CoordinateSystem
ScenarioReEntryTrajectory::coordinateSystem() const
{
    // TODO: should this be the initial state coord sys?
    return sta::CoordinateSystem(sta::COORDSYS_EME_J2000);
}


