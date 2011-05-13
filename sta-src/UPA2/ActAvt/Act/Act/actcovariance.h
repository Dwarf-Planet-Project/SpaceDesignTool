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

#ifndef ACTCOVARIANCE_H
#define ACTCOVARIANCE_H

#include <iostream>
#include <cmath>
#include <algorithm>
#include <ctime>
#include <cfloat>                           // required for DBL_EPSILON
#include <QMessageBox>

#include <Eigen/Array>
#include <Eigen/LU>
#include <Eigen/Cholesky>
#include <Eigen/QR>
#include <Eigen/SVD>
#include <Eigen/Geometry>
#include <Eigen/LeastSquares>
#include <Eigen/Sparse>
#include "RandNr/newran.h"
//#include "Stocc/randomc.h"                   // define classes for random number generators
//#include "Stocc/stocc.h"                     // define random library classes

#include "Astro-Core/stacoordsys.h"
#include "Astro-Core/stabody.h"
#include "Astro-Core/cartesianTOorbital.h"
#include "Astro-Core/cartesianTOspherical.h"
#include "Astro-Core/orbitalTOcartesian.h"
#include "Astro-Core/sphericalTOcartesian.h"


#include "matrix.h"


#define NSAMPLES 1000
#define SHAT1 2.943035529371538573  // 8/e
#define SHAT2 0.8989161620588987408  // 3-sqrt(12/e)
#define FAK_LEN 1024       // length of factorial table
//#define USEEIGENEIG
//#define USEEIGENINVERSE

// import most common Eigen types
USING_PART_OF_NAMESPACE_EIGEN


namespace act
{
    enum ActElementType
    {
                ELEM_CARTESIAN,             /*! Cartesian */
                ELEM_KEPLERIAN,        /*! Keplerian */
                ELEM_SPHERICAL,       /*! Spherical */
                ELEM_INVALID,
    };

    enum ActDistributionType
    {
                DIST_NORMAL,
                DIST_POISSON,
                DIST_BINOMIAL,
                DIST_NEG_BINOMIAL,
                DIST_CHI_SQUARE,
                DIST_DISCRETE_UNIFORM,
                DIST_GEOMETRIC,
                DIST_HYPER_GEOMETRIC,
                DIST_INVALID,

    };

    template<class T> struct index_cmp
    {
        index_cmp(const T arr) : arr(arr) {}
        bool operator()(const size_t a, const size_t b) const
        {
                return arr[a] < arr[b];
        }
        const T arr;
    };


    class ActCovariance
    {
    public:
        ActCovariance();
        ~ActCovariance();

        void setCentralBody(const StaBody *centralbody) {m_centralbody = centralbody;}
        void setMJD(double mjd){m_mjd=mjd;}

        sta::CoordinateSystemType coordinateType() {return m_coordinatetype;}
        ActElementType elementType() {return m_elementtype;}

        bool isInitialized() {return m_initialized;}


        int InitializeCov(MatrixXd meanin, MatrixXd covin, sta::CoordinateSystemType coordType, ActElementType elemType);
        int InitializeRMS(MatrixXd meanin, MatrixXd rmsin, sta::CoordinateSystemType coordType, ActElementType elemType);
        int InitializeSamples(MatrixXd samplesin, sta::CoordinateSystemType coordType, ActElementType elemType);
        int InitializeJ2000(MatrixXd meanin, MatrixXd covin, sta::CoordinateSystemType coordType, ActElementType elemType);
        int InitializeSamplesJ2000(MatrixXd samplesin, sta::CoordinateSystemType coordType, ActElementType elemType);

        int InitializeNonLinearCov(MatrixXd meanin, MatrixXd covin, sta::CoordinateSystemType coordType, ActElementType elemType);
        int InitializeNonLinearRMS(MatrixXd meanin, MatrixXd rmsin, sta::CoordinateSystemType coordType, ActElementType elemType);
        int InitializeNonLinearSamples(MatrixXd samplesin, sta::CoordinateSystemType coordType, ActElementType elemType);
        int InitializeNonLinearJ2000(MatrixXd meanin, MatrixXd covin, sta::CoordinateSystemType coordType, ActElementType elemType);
        int InitializeNonLinearSamplesJ2000(MatrixXd samplesin, sta::CoordinateSystemType coordType, ActElementType elemType);


        MatrixXd samples() {return m_samples;}
        MatrixXd samplesJ2000() {return m_samplesJ2000;}

        MatrixXd mean() {return m_mean;}
        MatrixXd meanJ2000() {return m_meanJ2000;}

        MatrixXd covariance() {return m_covariance;}
        MatrixXd covarianceJ2000() {return m_covarianceJ2000;}

        MatrixXd normal();
        MatrixXd eig();
        MatrixXd eigv();
        MatrixXd RMS();
        MatrixXd weak();
        MatrixXd median();
        MatrixXd min();
        MatrixXd max();
        MatrixXd range();
        MatrixXd ellipsoid();

        MatrixXd NormRnd(MatrixXd meanin, MatrixXd covin, int nsamples);
        MatrixXd PoissonRnd(MatrixXd lambda, int nsamples);
        MatrixXd BinoRnd(MatrixXi n, MatrixXd p, int nsamples);
        MatrixXd NegBinoRnd(MatrixXi n, MatrixXd p, int nsamples);
        MatrixXd ChiSqRnd(MatrixXi df, MatrixXd noncen, int nsamples);
        MatrixXd DiscreteUnifRnd(MatrixXi min, MatrixXi max, int nsamples);
        MatrixXd GeoRnd(MatrixXd p, int nsamples);
        MatrixXd HypGeoRnd(MatrixXi n, MatrixXi m, MatrixXi N, int nsamples);
        MatrixXd matInverse(MatrixXd dir);

    private:

        const StaBody *m_centralbody;
        double m_mjd;
        sta::CoordinateSystemType m_coordinatetype;
        ActElementType m_elementtype;

        bool m_initialized;


        MatrixXd m_samples;
        MatrixXd m_samplesJ2000;
        MatrixXd m_covariance;
        MatrixXd m_covarianceJ2000;
        MatrixXd m_mean;
        MatrixXd m_meanJ2000;

        int setCoordinateType(sta::CoordinateSystemType coordType) {m_coordinatetype = coordType; return true;}
        int setElementType(ActElementType elemType) {m_elementtype = elemType; return true;}

        void toCartesianJ2000(sta::CoordinateSystemType coordType, ActElementType elemType);
        void fromCartesianJ2000(sta::CoordinateSystemType coordType, ActElementType elemType);

        void toCartesianJ2000NonLinear(sta::CoordinateSystemType coordType, ActElementType elemType);
        void fromCartesianJ2000NonLinear(sta::CoordinateSystemType coordType, ActElementType elemType);

        MatrixXd po2pv(double mu, double a, double e, double inc, double Om, double om, double m0);
        MatrixXd linpo2pv(double mu, double a, double e, double inc, double Om, double om, double m0);
        MatrixXd linpv2po(double mu, sta::StateVector TmpVec);

        MatrixXd linspherical2cartesian(double r, double delta, double tau, double v, double gamma, double chi);
        MatrixXd lincartesian2spherical(double x, double y, double z, double vx, double vy, double vz);

        MatrixXd linfixed2inertial(Matrix3d rotation, Vector3d omega);
        MatrixXd lininertial2fixed(Matrix3d rotation, Vector3d omega);

        MatrixXd linrotating2inertial(double alfa, double beta, double i, double rnorm, double vnorm);
        MatrixXd lininertial2rotating(double alfa, double beta, double i, double rnorm, double vnorm);

        double theta2M(double theta, double e);
        double M2theta(double M, double e);
        MatrixXd lintheta2M(double e, double theta);
        MatrixXd linM2theta(double e, double M);     

        MatrixXd linrot(MatrixXd R);

        void Eigenval(MatrixXd mat, MatrixXd &eigenval, MatrixXd &eigenvec, int dim);
        void Jacobi_Cyclic_Method(double eigenvalues[], double *eigenvectors, double *A, int n);

        double LnFac(int32_t n);
        double fc_lnpk(int32_t k, int32_t L, int32_t m, int32_t n);
        int32_t HypRatioOfUnifoms (int32_t n, int32_t m, int32_t N);
        int32_t HypInversionMod (int32_t n, int32_t m, int32_t N);
        int32_t Hypergeometric (int32_t n, int32_t m, int32_t N);
    };

};


#endif // ACTCOVARIANCE_H
