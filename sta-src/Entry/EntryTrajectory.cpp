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
------------------ Author:       Marnix Volckaert               -------------------
------------------ Affiliation:  TUDelft                        -------------------
-----------------------------------------------------------------------------------
 Modified by Tiziana Sabatini on May-July 2009
*/


#include <QDir>
#include <QTextStream>

#include "EntryTrajectory.h"
#include "Astro-Core/EODE/eode.h"
#include <Eigen/Core>
#include <QProcess>
#include <QDebug>



/*Function containing the equations of motion*/
//=================================================================================================
void derivstate (VectorXd state, double time, VectorXd parameters, VectorXd& derivative)
{
        derivative[0] = state[3];
        derivative[1] = state[4];
        derivative[2] = state[5];
        derivative[3] = parameters[3] + parameters[0] + pow(parameters[6],2)*state[0] + 2.0*parameters[6]*state[4];
        derivative[4] = parameters[4] + parameters[1] + pow(parameters[6],2)*state[1] - 2.0*parameters[6]*state[3];
        derivative[5] = parameters[5] + parameters[2];
        derivative[6] = parameters[7];
        derivative[7] = parameters[8];
}

class EntryDerivativeCalculator : public DerivativeCalculator<8>
{
public:
    EntryDerivativeCalculator(EntryTrajectory *trajectory):
            m_trajectory(trajectory)
            //m_perturbationList(perturbationList)
    {
    }

    virtual void compute(const State& state, double t, State& derivatives) const
    {
        Vector3d position = state.segment<3>(0);
        Vector3d velocity = state.segment<3>(3);
        sta::StateVector stateVector=StateVector(position,velocity);
        Vector3d gravity;                   //Components of gravitational acceleration in appropriate elements
        Vector3d aero;                     	//Components of aerodynamic acceleration in appropriate elements
        VectorXd parameters(9);             //List of parameters to send to integration: 0,1,2 = gravity, 3,4,5 = aero, 6 = omega, 7 = Qdot
        double Cdc;
        double tau = m_trajectory->settings.stepsize;

        m_trajectory->body.updateGravity(stateVector, gravity);
        m_trajectory->body.updateAero(stateVector, m_trajectory->state2, m_trajectory->atmosphere, m_trajectory->capsule, aero, Cdc);
        /*
        Vector3d perturbingAcceleration(0, 0, 0);
        foreach (Perturbations* perturbation, perturbationList)
        {
            perturbingAcceleration += perturbation->calculateAcceleration(stateVector, time, tau);
        }
        gravity += perturbingAcceleration;
        */
        m_trajectory->calculateState2(aero, Cdc);
        m_trajectory->updateEndstate();

        //Test for status
        m_trajectory->checkStatus();
        if ( m_trajectory->status != OK) {
            m_trajectory->result.status = m_trajectory->status;
        }
        parameters << gravity, aero, m_trajectory->body.getOmega(), m_trajectory->state2(3), m_trajectory->state2(4);
        //TO DO : re-insert the heat load integration (now heat flux is always set to zero)
        VectorXd state3(8);
                state3 << stateVector.position, stateVector.velocity, 0, 0;


                derivatives[0] = state3[3];
                derivatives[1] = state3[4];
                derivatives[2] = state3[5];
                derivatives[3] = parameters[3] + parameters[0] + pow(parameters[6],2)*state3[0] + 2.0*parameters[6]*state3[4];
                derivatives[4] = parameters[4] + parameters[1] + pow(parameters[6],2)*state3[1] - 2.0*parameters[6]*state3[3];
                derivatives[5] = parameters[5] + parameters[2];
                derivatives[6] = parameters[7];
                derivatives[7] = parameters[8];
    }

private:
    EntryTrajectory* m_trajectory;

    //const QList<Perturbations*>& m_perturbationList;

};

EntryTrajectory::EntryTrajectory(EntrySettings _settings)
{
    //Set the entry trajectory's settings to the input settings
    //---------------------------------------------------------
    settings = _settings;
    //---------------------------------------------------------

    //Assign body, atmosphere and heat rate, and capsule Cd profile
    //---------------------------------------------------------
    atmosphere.selectModel(settings.modelname);
    body.selectBody(settings.bodyname);
    heatrate.selectBody(settings.bodyname);
    capsule.flag = 0;
    capsule.selectCdCprofile(settings.CdCprofilename);
    //capsule.selectCdPprofile(settings.CdPprofilename);
    //capsule.Sp = settings.parachuteArea;

    state.resize(8);
    state2.resize(10);
}


sta::StateVector EntryTrajectory::initialise(EntryParameters _parameters,sta::StateVector initialState, double startTime)
{

    parameters = _parameters;           //Save the parameters as a member of the trajectory, to possibly use later in plotting

    capsule.Sc = parameters.Sref;
    capsule.m = parameters.m;
    capsule.Rn = parameters.Rn;

    double theta = getGreenwichHourAngle(startTime);
    int celestialbody;
    if (settings.bodyname == "Earth")
         celestialbody = 0;
    else if (settings.bodyname == "Mars")
         celestialbody = 3;
    //inertialTOfixed(celestialbody, theta, initialState.position.x(), initialState.position.y(), initialState.position.z(), initialState.velocity.x(), initialState.velocity.y(), initialState.velocity.z(),
      //          state(0), state(1), state(2), state(3), state(4), state(5));

    //const Eigen::Vector3d position(initialState.position.x(),initialState.position.y(),initialState.position.z());
    //const Eigen::Vector3d velocity(state(3), state(4), state(5));
    //initialState = sta::StateVector(position,velocity);
    double omega=7.29211585494e-5;
    initialState.velocity.x()+=initialState.position.y()*omega;
    initialState.velocity.y()+=-initialState.position.x()*omega;

    
    //----- Initialise the heat loads to zero
    state(6) = 0.0;
    state(7) = 0.0;
    //---------------------------------------------------------

    //Initialise the rest
    //---------------------------------------------------------
    time = sta::daysToSecs(sta::JdToMjd(startTime));
    for (int i=0; i<10; i++) {
        state2(i) = 0.0;
    }
    state2(5) = parameters.inputstate[0];          //Initial altitude
    status = OK;                                   //Status = OK
    result.Machone_transition_altitude = 0.0;      //Needs to be zero at first so that it only gets set once
    result.parachutedeploy_altitude = 0.0;         //Needs to be zero at first so that it only gets set once
    result.maxconvheatrate.value = 0.0;
    result.maxradheatrate.value = 0.0;
    result.maxtotalheatrate.value = 0.0;
    result.maxloadfactor.value = 0.0;

    trajectory.open("data/REMTrajectory.stae");

    return initialState;
}


Endstate EntryTrajectory::getEndstate()
{
    save = false;
    QList<double> sampleTimesTemp;
    QList<sta::StateVector> samplesTemp;
    //integrate(sampleTimesTemp, samplesTemp);
    return (result);
}


void EntryTrajectory::saveTrajectory (QList<double>& sampleTimes, QList<sta::StateVector>& samples)
{
    save = true;
    trajectory.open("data/REMTrajectory.stae");
    misc.open("data/REMMiscellaneous.stam");
    //integrate(sampleTimes, samples);
    misc.close();
    trajectory.close();
    printReport();
}


sta::StateVector EntryTrajectory::integrate(sta::StateVector stateVector, QList<Perturbations*> perturbationsList)//Modified by Dominic to improve integration stability
{
    double tau = settings.stepsize;

    /*
    Vector3d gravity;                   //Components of gravitational acceleration in appropriate elements
    Vector3d aero;                     	//Components of aerodynamic acceleration in appropriate elements
    VectorXd parameters(9);             //List of parameters to send to integration: 0,1,2 = gravity, 3,4,5 = aero, 6 = omega, 7 = Qdot
    double Cdc;

    //Update gravity, perturbations and aerodynamic accelerations
    body.updateGravity(stateVector, gravity);
    body.updateAero(stateVector, state2, atmosphere, capsule, aero, Cdc);

    //Vector3d perturbingAcceleration(0, 0, 0);
    //foreach (Perturbations* perturbation, perturbationsList)
    //{
    //    perturbingAcceleration += perturbation->calculateAcceleration(stateVector, time, tau);
    //}
    //gravity += perturbingAcceleration;

    calculateState2(aero, Cdc);
    updateEndstate();

    //Test for status
    checkStatus();
    if ( status != OK) {
        result.status = status;
    }

    //Build the parameters vector to send to equations of motion
    parameters << gravity, aero, body.getOmega(), state2(3), state2(4);
    //TO DO : re-insert the heat load integration (now heat flux is always set to zero)
    VectorXd state(8);
            state << stateVector.position, stateVector.velocity, 0, 0;

    //if (settings.integrator == "RK4")
        rk4 (state, 8, time, tau, derivstate, parameters);
    */
    EntryDerivativeCalculator entryCalculator(this);// entryCalculator(this,QList<Perturbations*> perturbationsList);
    Matrix<double, 8, 1> state;
    state<<stateVector.position, stateVector.velocity, 0, 0;
    rk4(state, time, tau, &entryCalculator);



    const Vector3d vec1(state(0), state(1), state(2));
    const Vector3d vec2(state(3), state(4), state(5));
    sta::StateVector vector = sta::StateVector(vec1, vec2);

    stateVector = vector;

    time += tau;

    //------ Calculate the rest of the endstate structure ------
    calculateEndstate();

    return stateVector;
}


void EntryTrajectory::calculateState2(Vector3d& aero, double Cdc)
{
    state2(2) = aero.norm() / 9.81; //n = magnitude of resultant vector of accelerations / g
    //------ calculate heat rate at stagnation point ------
    state2(3) = heatrate.ConvectiveHeatRate(state2(6), state2(0), capsule.Rn);
    state2(4) = heatrate.RadiativeHeatRate(state2(6), state2(0), capsule.Rn);
    state2(8) = state2(3) + state2(4);
    state2(9) = state(6) + state(7);
}


inline void EntryTrajectory::checkStatus()
{
    if (state2(5) < 0.0) {status = landed;};
    if (state2(5) > settings.maxaltitude) {status = gone;};     //If higher than x times initial altitude -> gone
    if (state2(8) > settings.maxheatrate) {status = burnt;};
    if (result.maxloadfactor.value > settings.maxloadfactor) {status = crushed;};
}


Endstate EntryTrajectory::updateEndstate()
{
    if (state2(1) < 1.0 && result.Machone_transition_altitude == 0.0) {
        result.Machone_transition_time = time;
        result.Machone_transition_altitude = state2(5);
    }//-------------------------------------
    if (state2(1) < settings.parachuteDeployMach && result.parachutedeploy_altitude == 0.0) {
        result.parachutedeploy_time = time;
        result.parachutedeploy_altitude = state2(5);
        state2(7) = 1.0;
    }//-------------------------------------
    if (state2(2) > result.maxloadfactor.value && state2(7) != 1.0) {         //find the maximum load factor
        result.maxloadfactor.value = state2(2);
        result.maxloadfactor.time = time;
        result.maxloadfactor.altitude = state2(5);
    }//-------------------------------------
    if (state2(3) > result.maxconvheatrate.value) {           //find the maximum heat rate at the stagnation point
        result.maxconvheatrate.value = state2(3);
        result.maxconvheatrate.time = time;
        result.maxconvheatrate.altitude = state2(5);
    }//-------------------------------------
    if (state2(4) > result.maxradheatrate.value) {           //find the maximum heat rate at the stagnation point
        result.maxradheatrate.value = state2(4);
        result.maxradheatrate.time = time;
        result.maxradheatrate.altitude = state2(5);
    }//-------------------------------------
    if (state2(8) > result.maxtotalheatrate.value) {         //find the maximum total heat rate
        result.maxtotalheatrate.value = state2(8);
        result.maxtotalheatrate.time = time;
        result.maxtotalheatrate.altitude = state2(5);
    }
    return(result);
}


Endstate EntryTrajectory::calculateEndstate() {

    //------ Update the maximum values one more time ------
    if (state2(2) > result.maxloadfactor.value) {         //find the maximum heat rate
        result.maxloadfactor.value = state2(2);
        result.maxloadfactor.time = time;
        result.maxloadfactor.altitude = state2(5);
    }//-------------------------------------
    if (state2(3) > result.maxconvheatrate.value) {           //find the maximum heat rate at the stagnation point
        result.maxconvheatrate.value = state2(3);
        result.maxconvheatrate.time = time;
        result.maxconvheatrate.altitude = state2(5);
    }//-------------------------------------
    if (state2(4) > result.maxradheatrate.value) {           //find the maximum heat rate at the stagnation point
        result.maxradheatrate.value = state2(3);
        result.maxradheatrate.time = time;
        result.maxradheatrate.altitude = state2(5);
    }//-------------------------------------
    if (state2(8) > result.maxtotalheatrate.value) {         //find the maximum total heat rate

        result.maxtotalheatrate.value = state2(4);
        result.maxtotalheatrate.time = time;
        result.maxtotalheatrate.altitude = state2(5);
    }
    //------ Compute time of flight, total heat load and impact velocity ------
    result.timeofflight = time;
    result.totalheatload = state2(9);
    result.tpsmass = (0.0009*pow(state(6),0.5306)) * parameters.m;
    result.impactvelocity = state2(6);

    //The following is only applicable for a successful landing
    //---------------------------------------------------------
    if (status == 0) {
        //------ Compute the landing longitude ------
        result.longitude = atan2(state(1), state(0)) / acos(-1.0) * 180;
        //------ Compute the landing latitude ------
        //lambda = asin( state(2) / sqrt(pow(state(0),2) + pow(state(1),2) + pow(state(2),2)) );
        //result.latitude = body.geocentric2geodetic_latitude(lambda) / acos(-1) * 180;
        result.latitude = asin( state(2) / sqrt(pow(state(0),2) + pow(state(1),2) + pow(state(2),2)) ) / acos(-1.0) * 180;
    }
    //---------------------------------------------------------
    return(result);
}


inline void EntryTrajectory::saveOutput (double time)
{

    double stateSpherical[6];
    cartesianTOspherical(state(0), state(1), state(2), state(3), state(4), state(5),
                         stateSpherical[0], stateSpherical[1], stateSpherical[2], stateSpherical[3], stateSpherical[4], stateSpherical[5]);
    stateSpherical[2] = body.altitude(stateSpherical[2] , stateSpherical[1] );
    double theta = getGreenwichHourAngle(sta::MjdToJd(sta::secsToDays(time)));
    double a,b,c,d,e,f;
    int celestialbody;
    if (settings.bodyname == "Earth")
         celestialbody = 0;
    else if (settings.bodyname == "Mars")
         celestialbody = 3;
    fixedTOinertial(celestialbody, theta, state(0), state(1), state(2), state(3), state(4), state(5),
                    a,b,c,d,e,f);
    double v = sqrt(d*d + e*e + f*f);
    double g,h,i;
    cartesianTOspherical(a, b, c, d, e, f,
                         g, h, i, stateSpherical[3], stateSpherical[4], stateSpherical[5]);

    trajectory << setfill('0');
        trajectory << setiosflags(ios::fixed) << setprecision(2) << setw(10) << state(0) << "     ";
        trajectory << setiosflags(ios::fixed) << setprecision(2) << setw(10) << state(1) << "     ";
        trajectory << setiosflags(ios::fixed) << setprecision(2) << setw(10) << state(2) << "     ";
        trajectory << setiosflags(ios::fixed) << setprecision(2) << setw(8) << state(3) << "     ";
        trajectory << setiosflags(ios::fixed) << setprecision(2) << setw(8) << state(4) << "     ";
        trajectory << setiosflags(ios::fixed) << setprecision(2) << setw(8) << state(5) << "     ";
        trajectory << stateSpherical[0]*1000 << "     ";
        trajectory << stateSpherical[1]*1000 << "     ";
        trajectory << setiosflags(ios::fixed) << setprecision(2) << setw(10) << stateSpherical[2]*1000 << "     ";
        trajectory << setiosflags(ios::fixed) << setprecision(2) << setw(10) << v << "     ";
        trajectory << stateSpherical[4]*1000 << "     ";
        trajectory << setiosflags(ios::fixed) << setprecision(2) << setw(10) << stateSpherical[5]*1000 << "     ";
        trajectory << state2[0]*1000 << "     ";//density
        trajectory << state2[1] << "     ";//Ma
        trajectory << state2[2]*1000 << "     ";//load factor
        trajectory << state2[3] << "     ";//qdot_c
        trajectory << state2[4] << "     ";//qdot_r
        trajectory << state[7] << "     ";//parachute
        trajectory << state2[8]*1000 << "     ";//totalheatrate
        trajectory << time << endl;
}


inline void EntryTrajectory::printReport ()
{

    angle longitude_angle;
    angle latitude_angle;
    angle entrylong_angle;
    angle entrylat_angle;
    AngleConversion angleconversion;
    //angleconversion.decimalToDegMinSec(result.longitude, longitude_angle.degrees, longitude_angle.minutes, longitude_angle.seconds);
    //angleconversion.decimalToDegMinSec(result.latitude, latitude_angle.degrees, latitude_angle.minutes, latitude_angle.seconds);
    angleconversion.decimalToDegMinSec(parameters.inputstate[1], entrylong_angle.degrees, entrylong_angle.minutes, entrylong_angle.seconds);
    angleconversion.decimalToDegMinSec(parameters.inputstate[2], entrylat_angle.degrees, entrylat_angle.minutes, entrylat_angle.seconds);

//    angle longitude_angle = double2angle (result.longitude);
//    angle latitude_angle = double2angle (result.latitude);
//    angle entrylong_angle = double2angle (parameters.inputstate[1]);
//    angle entrylat_angle = double2angle (parameters.inputstate[2]);
    //ofstream report ("C:/Users/Tizy/Desktop/Code/trunk/sta-src/Entry/REMReport.txt");
    ofstream report("data/REMReport.txt");

    //ofstream summary ("data/REMSummary.txt");

    switch (status)
       {
       case landed:
            cout << "Landed" << endl;
            report << "Status report: succesful landing" << endl;
            report << "================================" << endl;
            report << " " << endl;
            report << "Capsule data and initial conditions" << endl;
            report << "--------------------------------" << endl;
            //report << "Capsule base radius:\t" << parameters.R << " m" << endl; Make consistent with enw schema
            report << "Capsule mass:\t\t" << parameters.m << " kg" << endl;
            report << "Capsule nose radius:\t" << parameters.Rn << " m" << endl;
            report << "Entry altitude:\t\t" << parameters.inputstate[0] / 1000.0 << " km" << endl;
            report << "Entry longitude:\t" << entrylong_angle.degrees << "° " << entrylong_angle.minutes << "' " << entrylong_angle.seconds << "'' " << "(" << parameters.inputstate[1] << "°)" << endl;
            report << "Entry latitude:\t\t" << entrylat_angle.degrees << "° " << entrylat_angle.minutes << "' " << entrylat_angle.seconds << "'' " << "(" << parameters.inputstate[2] << "°)"<< endl;
            report << "Velocity:\t\t" << parameters.inputstate[3] << " m/s" << endl;
            report << "Flight path angle:\t" << parameters.inputstate[4] << " deg" << endl;
            report << "Heading:\t\t" << parameters.inputstate[5] << " deg" << endl;
            report << "--------------------------------" << endl;
            report << " " << endl;
            report << "Trajectory data" << endl;
            report << "--------------------------------" << endl;
            //report << "Impact longitude:\t" << longitude_angle.degrees << "° " << longitude_angle.minutes << "' " << longitude_angle.seconds << "'' " << "(" << result.longitude << "°)" << endl;
            //report << "Impact latitude:\t" << latitude_angle.degrees << "° " << latitude_angle.minutes << "' " << latitude_angle.seconds << "'' " << "(" << result.latitude << "°)" << endl;
            report << setprecision (3);
            report << "Impact velocity:\t" << result.impactvelocity << " m/s" << endl;
            report << fixed;
            report << "Total time of flight:\t" << result.timeofflight << " s" << endl;
            report << "--------------------------------" << endl;
            report << " " << endl;
            report << "Load data" << endl;
            report << "--------------------------------" << endl;
            report << "Maximum G load:\t\t\t" << result.maxloadfactor.value << " G (limit: " << settings.maxloadfactor << " G)" << endl;
            report << "Maximum convective heat rate:\t" << result.maxconvheatrate.value << " W/cm²" << endl;
            report << "Maximum radiative heat rate:\t" << result.maxradheatrate.value << " W/cm²" << endl;
            report << "Maximum total heat rate:\t" << result.maxtotalheatrate.value << " W/cm² (limit: " << settings.maxheatrate << " W/cm²)" << endl;
            report << "Total heat load:\t\t" << result.totalheatload << " J/cm²" << endl;
            report << setprecision (3);
            report << "TPS mass estimate:\t\t" << result.tpsmass << " kg (" << (0.0009 * pow(result.totalheatload,0.5306))*100.0 << " % of capsule mass)" << endl;
            report << fixed;
            report << "--------------------------------" << endl;
            report << " " << endl;
            report << "Entry profile: altitude" << endl;
            report << "--------------------------------" << endl;
            report << "Maximum radiative heat rate:\t" << result.maxradheatrate.altitude / 1000.0 << " km" << endl;
            report << "Maximum total heat rate:\t" << result.maxtotalheatrate.altitude / 1000.0 << " km" << endl;
            report << "Maximum convective heat rate:\t" << result.maxconvheatrate.altitude / 1000.0 << " km" << endl;
            report << "Maximum G:\t\t\t" << result.maxloadfactor.altitude / 1000.0 << " km" << endl;
            report << "Mach 1 transition:\t\t" << result.Machone_transition_altitude / 1000.0 << " km" << endl;
            report << "Parachute deployment:\t\t" << result.parachutedeploy_altitude / 1000.0 << " km (M " << settings.parachuteDeployMach << ")" << endl;
            report << "--------------------------------" << endl;
            report << " " << endl;
            report << "Entry profile: time (TE = time of entry, TI = time of impact)" << endl;
            report << "--------------------------------" << endl;
            report << "Maximum radiative heat rate:\tTE + " << result.maxradheatrate.time << " s, TI - " << (result.timeofflight - result.maxradheatrate.time) << " s" << endl;
            report << "Maximum total heat rate:\tTE + " << result.maxtotalheatrate.time << " s, TI - " << (result.timeofflight - result.maxtotalheatrate.time) << " s" << endl;
            report << "Maximum convective heat rate:\tTE + " << result.maxconvheatrate.time << " s, TI - " << (result.timeofflight - result.maxconvheatrate.time) << " s" << endl;
            report << "Maximum G:\t\t\tTE + " << result.maxloadfactor.time << " s, TI - " << (result.timeofflight - result.maxloadfactor.time) << " s" << endl;
            report << "Mach 1 transition:\t\tTE + " << result.Machone_transition_time << " s, TI - " << (result.timeofflight - result.Machone_transition_time) << " s" << endl;
            report << "Parachute deployment:\t\tTE + " << result.parachutedeploy_time << " s, TI - " << (result.timeofflight - result.parachutedeploy_time) << " s (M " << settings.parachuteDeployMach << ")" << endl;
            report << "================================";
            //    summary << longitude_angle.degrees << endl; summary << longitude_angle.minutes << endl; summary << longitude_angle.seconds << endl;
            //    summary << latitude_angle.degrees << endl; summary << latitude_angle.minutes << endl; summary << latitude_angle.seconds << endl;
            //    summary << result.maxconvheatrate.value << endl; summary << result.maxconvheatrate.altitude << endl; summary << result.maxconvheatrate.time << endl;
            //    summary << result.maxradheatrate.value << endl; summary << result.maxradheatrate.altitude << endl; summary << result.maxradheatrate.time << endl;
            //    summary << result.maxtotalheatrate.value << endl; summary << result.maxtotalheatrate.altitude << endl; summary << result.maxtotalheatrate.time << endl;
            //    summary << result.maxloadfactor.value << endl; summary << result.maxloadfactor.altitude << endl; summary << result.maxloadfactor.time << endl;
            //    summary << result.Machone_transition_altitude << endl; summary << result.Machone_transition_time << endl;
            //    summary << result.parachutedeploy_altitude << endl;  summary << result.parachutedeploy_time << endl;
            //    summary << result.totalheatload << endl; summary << result.timeofflight << endl; summary << result.impactvelocity << endl;
            break;
        case gone:
            cout << "Gone" << endl;
            report << "Status report: unsuccesful landing" << endl;
            report << "==================================" << endl;
            report << "The capsule reached maximum altitude after " << result.timeofflight << " s" << endl;
            break;
        case crushed:
            cout << "Crushed" << endl;
            break;
        case burnt:
            cout << "Burnt" << endl;
            break;
        case OK:
            cout << "In air" << endl;
            break;


        }

    report.close();
    //summary.close();
}
