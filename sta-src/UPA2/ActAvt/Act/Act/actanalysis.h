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

#ifndef ACTANALYSIS_H
#define ACTANALYSIS_H

#include <QList>
#include <QMessageBox>
#include <QString>
#include <QFile>
#include <QTextStream>
#include "Main/propagatedscenario.h"
#include "Scenario/propagationfeedback.h"
#include "Scenario/scenario.h"
#include "Astro-Core/stacoordsys.h"
#include "Astro-Core/cartesianTOorbital.h"
#include "Astro-Core/cartesianTOspherical.h"
#include "Loitering/loitering.h"
#include "Entry/reentry.h"
#include "Astro-Core/statevector.h"
#include "Astro-Core/stamath.h"
#include "actcovariance.h"

#include <iostream>
#include <cmath>
#include <algorithm>
#include <ctime>
#include <cfloat>                           // required for DBL_EPSILON

#include <Eigen/Array>
#include <Eigen/LU>
#include <Eigen/Cholesky>
#include <Eigen/QR>
#include <Eigen/SVD>
#include <Eigen/Geometry>
#include <Eigen/LeastSquares>
#include <Eigen/Sparse>
#include "RandNr/newran.h"
// #include "Stocc/randomc.h"                   // define classes for random number generators
// #include "Stocc/stocc.h"                     // define random library classes

#define MINDIST 1e-3

// import most common Eigen types
USING_PART_OF_NAMESPACE_EIGEN

// class SpaceObject;
// class MissionArc;

namespace act
{

    enum ActOutputType
    {
                OUT_COV,        // 6x6=36
                OUT_COVJ2K,     // 6x6=36
                OUT_SAMPLES,    // Numsamplesx6=variable
                OUT_SAMPLESJ2K, // Numsamplesx6=variable
                OUT_ELLIPSOID,  // 3x3=9
                OUT_WEAK,       // 1x6=6
                OUT_EIG,        // 1x6=6
                OUT_EIGV,       // 6x6=36
                OUT_NORMAL,     // 6x6=36
                OUT_RMS,        // 1x6=6
                OUT_MEAN,       // 1x6=6
                OUT_MEANJ2K,    // 1x6=6
                OUT_MEDIAN,     // 1x6=6
                OUT_MIN,        // 1x6=6
                OUT_MAX,        // 1x6=6
                OUT_RANGE,      // 1x6=6


    };

    class ActAnalysis
    {

    public:

        // Constructor
        ActAnalysis(SpaceScenario* scenario, PropagatedScenario* propscenario);

        // Destructor
        ~ActAnalysis();

        // set parameters (initial and final time, time steps, coordinate and element type, current spaceObject and arc)
        int setInitialTime(double timein) {m_timesteps.first() = timein; return true;}
        int setFinalTime(double timein) {m_timesteps.last() = timein; return true;}
        int setCurrentSpaceObject(int index);
        int setCurrentArc(int index);    //---------- remove to propagate all arcs
        int setTimeSteps(QList<double> timestepsin);
        int setInitialCovarianceNumber(int number);

        // get parameters
        int getInitialCovarianceNumber() {return m_variations;}
        double initialTime() {return m_timesteps.first();}
        double finalTime() {return m_timesteps.last();}
        QList<double> timeSteps() {return m_timesteps;}
        // ------------------ coordinate system and element type conversion should be treated inside now, to allow all arcs propagation
        sta::CoordinateSystemType coordinateType() {return m_coordinatetype;}
        ActElementType elementType() {return m_elementtype;}
        // ----------------------------------------------------------------------------------------------------------------------------

        int currentSpaceObject() {return m_currentspaceobject;}
        // int currentArc() {return m_currentarc;} // remove to propagate all arcs

        bool isPropagated() {return m_propagated;}
        bool isInitialized() {return m_initialized;}
        bool isSTMComputed() {return m_stmcomputed;}
        bool isSC() {return m_SC;}
        bool isREV() {return m_REV;}
        bool isLinear() {return m_linear;}

        // Initialize covariance at initial time considering current spaceobject and arc
        int InitializeCov(MatrixXd Cov, sta::CoordinateSystemType coordType, ActElementType elemType);
        int InitializeRMS(MatrixXd RMSValues, sta::CoordinateSystemType coordType, ActElementType elemType);
        int InitializeSamples(MatrixXd Samples, sta::CoordinateSystemType coordType, ActElementType elemType);
        int InitializeSamples(QString SamplesFileName, sta::CoordinateSystemType coordType, ActElementType elemType);
        int InitializeDist(ActDistributionType Distribution, MatrixXd Param1d, MatrixXi Param2i, MatrixXi Param3i, MatrixXi Param4i, int nsamples, sta::CoordinateSystemType coordType, ActElementType elemType);

        int InitializeCov(MatrixXd Cov, sta::CoordinateSystemType coordType, ActElementType elemType, int index);
        int InitializeRMS(MatrixXd RMSValues, sta::CoordinateSystemType coordType, ActElementType elemType, int index);
        int InitializeSamples(MatrixXd Samples, sta::CoordinateSystemType coordType, ActElementType elemType, int index);
        int InitializeSamples(QString SamplesFileName, sta::CoordinateSystemType coordType, ActElementType elemType, int index);
        int InitializeDist(ActDistributionType Distribution, MatrixXd Param1d, MatrixXi Param2i, MatrixXi Param3i, MatrixXi Param4i, int nsamples, sta::CoordinateSystemType coordType, ActElementType elemType, int index);

        void setLinear() {m_linear=true;}
        void setNonLinear() {m_linear=false;}


        int propagate();

        QList<MatrixXd> covariance();
        QList<MatrixXd> covariance(int index);

        MatrixXd covariance(int stepnumber, int index);

        QList<MatrixXd> covarianceJ2000();
        QList<MatrixXd> covarianceJ2000(int index);

        MatrixXd covarianceJ2000(int stepnumber, int index);

        QList<MatrixXd> samples();
        QList<MatrixXd> samples(int index);
        MatrixXd samples(int stepnumber, int index);


        QList<MatrixXd> samplesJ2000();
        QList<MatrixXd> samplesJ2000(int index);
        MatrixXd samplesJ2000(int stepnumber, int index);

        QList<MatrixXd> STM();
        MatrixXd STM(int index);

        QList<MatrixXd> ellipsoid();
        QList<MatrixXd> ellipsoid(int index);
        MatrixXd ellipsoid(int stepnumber, int index);

        QList<MatrixXd> weak();
        QList<MatrixXd> weak(int index);
        MatrixXd weak(int stepnumber, int index);

        QList<MatrixXd> normal();
        QList<MatrixXd> normal(int index);
        MatrixXd normal(int stepnumber, int index);

        QList<MatrixXd> eig();
        QList<MatrixXd> eig(int index);
        MatrixXd eig(int stepnumber, int index);

        QList<MatrixXd> eigv();
        QList<MatrixXd> eigv(int index);
        MatrixXd eigv(int stepnumber, int index);

        QList<MatrixXd> RMS();
        QList<MatrixXd> RMS(int index);
        MatrixXd RMS(int stepnumber, int index);

        QList<MatrixXd> mean();
        QList<MatrixXd> mean(int index);
        MatrixXd mean(int stepnumber, int index);

        QList<MatrixXd> meanJ2000();
        QList<MatrixXd> meanJ2000(int index);
        MatrixXd meanJ2000(int stepnumber, int index);


        QList<MatrixXd> median();
        QList<MatrixXd> median(int index);
        MatrixXd median(int stepnumber, int index);

        QList<MatrixXd> min();
        QList<MatrixXd> min(int index);
        MatrixXd min(int stepnumber, int index);

        QList<MatrixXd> max();
        QList<MatrixXd> max(int index);
        MatrixXd max(int stepnumber, int index);

        QList<MatrixXd> range();
        QList<MatrixXd> range(int index);
        MatrixXd range(int stepnumber, int index);

        int saveOutputFile(QList<ActOutputType> list, QString filename);
        int saveOutputFile(QList<ActOutputType> list, QString filename, int index);
        int saveSampleFile(QString filename, int stepnumber, int index);
        int saveSampleFileJ2K(QString filename, int stepnumber, int index);


    private:
        SpaceScenario* m_scenario;
        PropagatedScenario* m_propscenario;
        bool m_initialized;
        bool m_propagated;
        bool m_stmcomputed;
        sta::CoordinateSystemType m_coordinatetype;
        ActElementType m_elementtype;
        int m_currentspaceobject;
        int m_currentarc;
        int m_variations;
        bool m_SC;
        bool m_REV;
        bool m_linear;

       // ScenarioSpaceVehicle* m_vehicle;
        ScenarioSC* m_vehicle;
        ScenarioREV* m_entryvehicle;
        QSharedPointer<ScenarioAbstractTrajectoryType> m_trajectory;
        //ScenarioAbstractTrajectoryType* m_trajectory;
        SpaceObject* m_propvehicle;

        MissionArc* m_proptrajectory;
        //MissionArc* m_proptrajectory;


        QList<double> m_timesteps;
        QList<MatrixXd> m_STM;
        QList<MatrixXd> m_mean;
        QList< QList<ActCovariance> > m_covariance;
        QList< QList<MatrixXd> > m_montecarlo;


        int setCoordinateType(sta::CoordinateSystemType coordType) {m_coordinatetype = coordType; return true;}
        int setElementType(ActElementType elemType) {m_elementtype = elemType; return true;}

        int propagateCov();
        // ----------------------- added method to propagate MonteCarlo Simulations ---------------------------------------------------
        int propagateMonteCarlo();
        // ----------------------------------------------------------------------------------------------------------------------------

        void computeSTM();
        void propagateVehicle(PropagationFeedback& feedback, const sta::StateVector& perturbedIC, QList<double>& sampleTimes, QList<sta::StateVector>& samples);

    };

}

#endif // ACTANALYSIS_H
