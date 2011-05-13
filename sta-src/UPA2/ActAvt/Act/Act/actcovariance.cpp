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

#include "actcovariance.h"

using namespace act;

ActCovariance::ActCovariance() :
        m_centralbody(STA_SOLAR_SYSTEM->earth()),
        m_mjd(0.0),
        m_coordinatetype(sta::COORDSYS_EME_J2000),
        m_elementtype(ELEM_CARTESIAN),
        m_initialized(false)
{
    srand(time(0));
}

ActCovariance::~ActCovariance()
{

}

int ActCovariance::InitializeCov(MatrixXd meanin, MatrixXd covin, sta::CoordinateSystemType coordType, ActElementType elemType)
{

    // set coordinate system and elements
    setCoordinateType(coordType);
    setElementType(elemType);

    // set mean
    m_mean = meanin;

    // set covariance
    m_covariance = covin;

    // generate samples
    m_samples = NormRnd(meanin, covin, NSAMPLES);

    if (coordType == sta::COORDSYS_EME_J2000 && elemType == ELEM_CARTESIAN)
    {
        m_meanJ2000 = m_mean;
        m_covarianceJ2000 = m_covariance;
        m_samplesJ2000 = m_samples;
    }
    else
    {
        toCartesianJ2000(coordType, elemType);
    }

    m_initialized = true;

    return true;
}

int ActCovariance::InitializeRMS(MatrixXd meanin, MatrixXd rmsin, sta::CoordinateSystemType coordType, ActElementType elemType)
{

    // set coordinate system and elements
    setCoordinateType(coordType);
    setElementType(elemType);

    // set mean
    m_mean = meanin;

    // set covariance
    MatrixXd tmpmat = rmsin.cwise()*rmsin;
    m_covariance.setZero(6,6);
    int i;
    for (i=0; i< m_covariance.cols(); i++) m_covariance(i,i) = tmpmat(0,i);

    // generate samples
    m_samples = NormRnd(m_mean, m_covariance, NSAMPLES);

    if (coordType == sta::COORDSYS_EME_J2000 && elemType == ELEM_CARTESIAN)
    {
        m_meanJ2000 = m_mean;
        m_covarianceJ2000 = m_covariance;
        m_samplesJ2000 = m_samples;
    }
    else
    {
        toCartesianJ2000(coordType, elemType);
    }

    m_initialized = true;

    return true;
}

int ActCovariance::InitializeSamples(MatrixXd samplesin, sta::CoordinateSystemType coordType, ActElementType elemType)
{

    // set coordinate system and elements
    setCoordinateType(coordType);
    setElementType(elemType);

    m_samples = samplesin;
    m_mean = m_samples.colwise().sum()/m_samples.rows();
    m_covariance = MatrixXd::Zero(6,6);


    MatrixXd tmpmat(m_samples.rows(),m_samples.cols());
    int i;
    for (i=0; i< m_samples.cols();i++)
    {
        tmpmat.block(0, i, m_samples.rows(), 1) = m_samples.block(0, i, m_samples.rows(), 1).cwise() - m_mean(0,i);
    }

    for (i=0; i< m_samples.cols(); i++)
    {
        m_covariance.block(i, 0, 1, m_samples.cols()) = tmpmat.block(0,i,m_samples.rows(),1).transpose()*tmpmat;
    }
    m_covariance = m_covariance/(m_samples.rows()-1);


    if (coordType == sta::COORDSYS_EME_J2000 && elemType == ELEM_CARTESIAN)
    {
        m_meanJ2000 = m_mean;
        m_covarianceJ2000 = m_covariance;
        m_samplesJ2000 = m_samples;
    }
    else
    {
        toCartesianJ2000(coordType, elemType);
    }

    m_initialized = true;

    return true;
}

int ActCovariance::InitializeSamplesJ2000(MatrixXd samplesin, sta::CoordinateSystemType coordType, ActElementType elemType)
{

    // set coordinate system and elements
    setCoordinateType(coordType);
    setElementType(elemType);

    m_samplesJ2000 = samplesin;
    m_meanJ2000 = m_samplesJ2000.colwise().sum()/m_samplesJ2000.rows();
    m_covarianceJ2000 = MatrixXd::Zero(6,6);


    MatrixXd tmpmat(m_samplesJ2000.rows(),m_samplesJ2000.cols());
    int i;
    for (i=0; i< m_samplesJ2000.cols();i++)
    {
        tmpmat.block(0, i, m_samplesJ2000.rows(), 1) = m_samplesJ2000.block(0, i, m_samplesJ2000.rows(), 1).cwise() - m_meanJ2000(0,i);
    }

    for (i=0; i< m_samplesJ2000.cols(); i++)
    {
        m_covarianceJ2000.block(i, 0, 1, m_samplesJ2000.cols()) = tmpmat.block(0,i,m_samplesJ2000.rows(),1).transpose()*tmpmat;
    }
    m_covarianceJ2000 = m_covarianceJ2000/(m_samplesJ2000.rows()-1);


    if (coordType == sta::COORDSYS_EME_J2000 && elemType == ELEM_CARTESIAN)
    {
        m_mean = m_meanJ2000;
        m_covariance = m_covarianceJ2000;
        m_samples = m_samplesJ2000;
    }
    else
    {
        fromCartesianJ2000(coordType, elemType);
    }

    m_initialized = true;

    return true;
}

int ActCovariance::InitializeJ2000(MatrixXd meanin, MatrixXd covin, sta::CoordinateSystemType coordType, ActElementType elemType)
{
    // set coordinate system and elements
    setCoordinateType(coordType);
    setElementType(elemType);

    // set mean
    m_meanJ2000 = meanin;

    // set covariance
    m_covarianceJ2000 = covin;

    // generate samples
    m_samplesJ2000 = NormRnd(meanin, covin, NSAMPLES);

    if (coordType == sta::COORDSYS_EME_J2000 && elemType == ELEM_CARTESIAN)
    {
        m_mean = m_meanJ2000;
        m_covariance = m_covarianceJ2000;
        m_samples = m_samplesJ2000;
    }
    else
    {
        fromCartesianJ2000(coordType, elemType);
    }

    m_initialized = true;

    return true;
}


int ActCovariance::InitializeNonLinearCov(MatrixXd meanin, MatrixXd covin, sta::CoordinateSystemType coordType, ActElementType elemType)
{

    // set coordinate system and elements
    setCoordinateType(coordType);
    setElementType(elemType);

    // set mean
    m_mean = meanin;

    // set covariance
    m_covariance = covin;

    // generate samples
    m_samples = NormRnd(meanin, covin, NSAMPLES);

    if (coordType == sta::COORDSYS_EME_J2000 && elemType == ELEM_CARTESIAN)
    {
        m_meanJ2000 = m_mean;
        m_covarianceJ2000 = m_covariance;
        m_samplesJ2000 = m_samples;
    }
    else
    {
        toCartesianJ2000NonLinear(coordType, elemType);
    }

    m_initialized = true;

    return true;
}

int ActCovariance::InitializeNonLinearRMS(MatrixXd meanin, MatrixXd rmsin, sta::CoordinateSystemType coordType, ActElementType elemType)
{

    // set coordinate system and elements
    setCoordinateType(coordType);
    setElementType(elemType);

    // set mean
    m_mean = meanin;

    // set covariance
    MatrixXd tmpmat = rmsin.cwise()*rmsin;
    m_covariance.setZero(6,6);
    int i;
    for (i=0; i< m_covariance.cols(); i++) m_covariance(i,i) = tmpmat(0,i);

    // generate samples
    m_samples = NormRnd(m_mean, m_covariance, NSAMPLES);

    if (coordType == sta::COORDSYS_EME_J2000 && elemType == ELEM_CARTESIAN)
    {
        m_meanJ2000 = m_mean;
        m_covarianceJ2000 = m_covariance;
        m_samplesJ2000 = m_samples;
    }
    else
    {
        toCartesianJ2000NonLinear(coordType, elemType);
    }

    m_initialized = true;

    return true;
}

int ActCovariance::InitializeNonLinearSamples(MatrixXd samplesin, sta::CoordinateSystemType coordType, ActElementType elemType)
{

    // set coordinate system and elements
    setCoordinateType(coordType);
    setElementType(elemType);

    m_samples = samplesin;
    m_mean = m_samples.colwise().sum()/m_samples.rows();
    m_covariance = MatrixXd::Zero(6,6);


    MatrixXd tmpmat(m_samples.rows(),m_samples.cols());
    int i;
    for (i=0; i< m_samples.cols();i++)
    {
        tmpmat.block(0, i, m_samples.rows(), 1) = m_samples.block(0, i, m_samples.rows(), 1).cwise() - m_mean(0,i);
    }

    for (i=0; i< m_samples.cols(); i++)
    {
        m_covariance.block(i, 0, 1, m_samples.cols()) = tmpmat.block(0,i,m_samples.rows(),1).transpose()*tmpmat;
    }
    m_covariance = m_covariance/(m_samples.rows()-1);


    if (coordType == sta::COORDSYS_EME_J2000 && elemType == ELEM_CARTESIAN)
    {
        m_meanJ2000 = m_mean;
        m_covarianceJ2000 = m_covariance;
        m_samplesJ2000 = m_samples;
    }
    else
    {
        toCartesianJ2000NonLinear(coordType, elemType);
    }

    m_initialized = true;

    return true;
}

int ActCovariance::InitializeNonLinearSamplesJ2000(MatrixXd samplesin, sta::CoordinateSystemType coordType, ActElementType elemType)
{

    // set coordinate system and elements
    setCoordinateType(coordType);
    setElementType(elemType);

    m_samplesJ2000 = samplesin;
    m_meanJ2000 = m_samplesJ2000.colwise().sum()/m_samplesJ2000.rows();
    m_covarianceJ2000 = MatrixXd::Zero(6,6);


    MatrixXd tmpmat(m_samplesJ2000.rows(),m_samplesJ2000.cols());
    int i;
    for (i=0; i< m_samplesJ2000.cols();i++)
    {
        tmpmat.block(0, i, m_samplesJ2000.rows(), 1) = m_samplesJ2000.block(0, i, m_samplesJ2000.rows(), 1).cwise() - m_meanJ2000(0,i);
    }

    for (i=0; i< m_samplesJ2000.cols(); i++)
    {
        m_covarianceJ2000.block(i, 0, 1, m_samplesJ2000.cols()) = tmpmat.block(0,i,m_samplesJ2000.rows(),1).transpose()*tmpmat;
    }
    m_covarianceJ2000 = m_covarianceJ2000/(m_samplesJ2000.rows()-1);


    if (coordType == sta::COORDSYS_EME_J2000 && elemType == ELEM_CARTESIAN)
    {
        m_mean = m_meanJ2000;
        m_covariance = m_covarianceJ2000;
        m_samples = m_samplesJ2000;
    }
    else
    {
        fromCartesianJ2000NonLinear(coordType, elemType);
    }

    m_initialized = true;

    return true;
}

int ActCovariance::InitializeNonLinearJ2000(MatrixXd meanin, MatrixXd covin, sta::CoordinateSystemType coordType, ActElementType elemType)
{
    // set coordinate system and elements
    setCoordinateType(coordType);
    setElementType(elemType);

    // set mean
    m_meanJ2000 = meanin;

    // set covariance
    m_covarianceJ2000 = covin;

    // generate samples
    m_samplesJ2000 = NormRnd(meanin, covin, NSAMPLES);

    if (coordType == sta::COORDSYS_EME_J2000 && elemType == ELEM_CARTESIAN)
    {
        m_mean = m_meanJ2000;
        m_covariance = m_covarianceJ2000;
        m_samples = m_samplesJ2000;
    }
    else
    {
        fromCartesianJ2000NonLinear(coordType, elemType);
    }

    m_initialized = true;

    return true;
}

// Normal Matrix computation (inverse of covariance matrix)
MatrixXd ActCovariance::normal()
{
    return matInverse(m_covariance);
}


// eigenvalues of the covariance matrix computation
MatrixXd ActCovariance::eig()
{
    MatrixXd outmat(m_covariance.rows(),m_covariance.cols());
    MatrixXd dummat(m_covariance.rows(),m_covariance.cols());
    Eigenval(m_covariance, outmat, dummat, m_covariance.rows());

    return outmat;

}

// eigenvectors of covariance matrix computation
MatrixXd ActCovariance::eigv()
{
    MatrixXd outmat(m_covariance.rows(),m_covariance.cols());
    MatrixXd dummat(m_covariance.rows(),m_covariance.cols());
    Eigenval(m_covariance, dummat, outmat,  m_covariance.rows());

    return outmat;

}



// RMS computation (sigma vector)
MatrixXd ActCovariance::RMS()
{
    int dim = m_covariance.rows();
    MatrixXd tmpmat(1,dim);
    int i;
    for (i=0; i<dim; i++)
    {
        tmpmat(0,i) = sqrt(m_covariance(i,i));
    }

    return tmpmat;

}

//Weak matrix computation (eigenvector associated to the max eig)
MatrixXd ActCovariance::weak()
{
    int dim = m_covariance.rows();
    MatrixXd tmpeigv(dim, dim);
    tmpeigv = eigv().transpose();

    return tmpeigv.row(dim-1);

}


// Median computation
MatrixXd ActCovariance::median()
{
    int dim = m_covariance.rows();
    int n = m_samples.rows();
    double tmpcol[n];
    MatrixXd outvec(1, dim);
    int i,j;
    for (i=0; i<dim; i++)
    {
        for (j=0; j<n; j++)
        {
            tmpcol[j] = m_samples(j,i);
        }

        sort(tmpcol, tmpcol+n);
        if (n%2)
        {
            outvec(0,i) = tmpcol[int(floor(n/2))];
        }
        else
        {
            outvec(0,i) = (tmpcol[int(n/2)] + tmpcol[int((n/2)-1)])/2;
        }
    }

    return outvec;

}

// Min colomwise computation
MatrixXd ActCovariance::min()
{
    int dim = m_covariance.rows();
    MatrixXd outvec(1, dim);
    outvec = m_samples.colwise().minCoeff();

    return outvec;

}


// Max colomwise computation
MatrixXd ActCovariance::max()
{
    int dim = m_covariance.rows();
    MatrixXd outvec(1, dim);
    outvec = m_samples.colwise().maxCoeff();

    return outvec;

}

// Spreds colomwise computation
MatrixXd ActCovariance::range()
{
    int dim = m_covariance.rows();
    MatrixXd outvec(1, dim);
    outvec = m_samples.colwise().maxCoeff() - m_samples.colwise().minCoeff();

    return outvec;

}

MatrixXd ActCovariance::ellipsoid()
{
    MatrixXd eigmat(3,3);
    MatrixXd eigvmat(3,3);
    MatrixXd outmat(3,3);
    Eigenval(m_covariance.block(0,0,3,3), eigmat, eigvmat,  3);
    int i,j;
    for (i=0; i<3; i++)
    {
        for (j=0; j<3; j++)
        {
            outmat(j,i) = eigvmat(j,i)*sqrt(eigmat(i,i));
        }
    }

    return outmat;

}

//  Multivariate normal random distribution
//  Input
//         meanin: mean vector (1xdim)
//         covin: covarance matrix (dimxdim) !must be symmetric!
//         nsamples: number of samples
//  Output
//        samples: Multivariate normal random numbers (nsamplesxdim)
//  Uses stocc library written by Agner Fog

MatrixXd ActCovariance::NormRnd(MatrixXd meanin, MatrixXd covin, int nsamples)
{
    //variables initialization
    int rows = nsamples;
    int cols = covin.cols();

    MatrixXd samples0 (rows, cols);
    MatrixXd samples (rows, cols);
    MatrixXd tmpeig  (cols, cols);
    MatrixXd tmpeigv (cols, cols);

    Eigenval(covin, tmpeig, tmpeigv, cols);

    //initialize random seed

    MotherOfAll urng(((double ) rand()) / RAND_MAX);
    Random::Set(urng);

    Normal Z;

    int i,j;
    // generation of normal distributed numbers scaled by the eigenvalues of the covariance matrix

    // dilatation
    for (i=0; i<cols; i++)
    {
        for (j=0; j<rows; j++)
        {
            samples0(j,i) = Z.Next()*sqrt(tmpeig(i,i));
        }
    }

    //rotation
    samples = samples0*tmpeigv.transpose();

    //translation
    for (i=0; i<cols; i++)
    {
        samples.block(0,i,rows,1) = samples.block(0,i,rows,1).cwise() + meanin(0,i);
    }

    return samples;



}

//  Poisson random distribution
//  Input
//         lambda: mean vector (1xdim)
//         nsamples: number of samples
//  Output
//        samples: Poisson random numbers (nsamplesxdim)
//  Uses stocc library written by Agner Fog

MatrixXd ActCovariance::PoissonRnd(MatrixXd lambda, int nsamples)
{
    int rows = nsamples;
    int cols = lambda.cols();
    MatrixXd samples(rows, cols);
    MotherOfAll urng(((double ) rand()) / RAND_MAX);
    Random::Set(urng);
    int i,j;

    for (j=0; j<cols; j++)
    {
        Poisson P(lambda(0,j));
        for (i=0; i<nsamples; i++)
        {
            samples(i,j) = P.Next();
        }
    }

    return samples;
}

//  Binomial random distribution
//  Input
//         n:  number of trials vector (1xdim) ! must be n>=0 !
//         p:  probability vector (1xdim) ! must be 0<=p<=1 !
//         nsamples: number of samples
//  Output
//        samples: Binomial random numbers (nsamplesxdim)
//  Uses stocc library written by Agner Fog

MatrixXd ActCovariance::BinoRnd(MatrixXi n, MatrixXd p, int nsamples)
{
    int rows = nsamples;
    int cols = n.cols();
    MatrixXd samples(rows, cols);
    MotherOfAll urng(((double ) rand()) / RAND_MAX);
    Random::Set(urng);
    int i,j;
    for (j=0; j<cols; j++)
    {
        Binomial B(n(0,j), p(0,j));
        for (i=0; i<nsamples; i++)
        {
            samples(i,j) = B.Next();
        }
    }

    return samples;

}

//  Negative binomial random distribution
//  Input
//         n:  number of trials vector (1xdim) ! must be n>=0 !
//         p:  probability vector (1xdim) ! must be 0<=p<=1 !
//         nsamples: number of samples
//  Output
//        samples: Negative Binomial random numbers (nsamplesxdim)
//  Uses newran library

MatrixXd ActCovariance::NegBinoRnd(MatrixXi n, MatrixXd p, int nsamples)
{
    int rows = nsamples;
    int cols = n.cols();
    MatrixXd samples(rows, cols);
    MotherOfAll urng(((double ) rand()) / RAND_MAX);
    Random::Set(urng);
    int i,j;

    for (j=0; j<cols; j++)
    {
        NegativeBinomial NB(n(0,j),p(0,j));
        for (i=0; i<nsamples; i++)
        {
            samples(i,j) = NB.Next();
        }
    }

    return samples;

}

//  Chi squared random distribution
//  Input
//         df: degree of freedom vector (1xdim) ! must df >=0 !
//         noncen:  non-centrality parameter vector (1xdim)
//         nsamples: number of samples
//  Output
//        samples: Chi squared random numbers (nsamplesxdim)
//  Uses newran (RandNr folder) library

MatrixXd ActCovariance::ChiSqRnd(MatrixXi df, MatrixXd noncen, int nsamples)
{
    int rows = nsamples;
    int cols = df.cols();
    MatrixXd samples(rows, cols);
    MotherOfAll urng(((double ) rand()) / RAND_MAX);
    Random::Set(urng);
    int i,j;

    for (j=0; j<cols; j++)
    {
        ChiSq CS(df(0,j), noncen(0,j));
        for (i=0; i<nsamples; i++)
        {
            samples(i,j) = CS.Next();
        }
    }

    return samples;

}

//  Dicrete uniform random distribution
//  Input
//         min: vector of minumim admissible values (1xdim)
//         max: vector of maximum admissible values (1xdim) ! must be min<=max columnwise!
//         nsamples: number of samples
//  Output
//        samples: Dicrete uniform random numbers (nsamplesxdim)
//  Uses stocc library written by Agner Fog

MatrixXd ActCovariance::DiscreteUnifRnd(MatrixXi min, MatrixXi max, int nsamples)
{
    int rows = nsamples;
    int cols = min.cols();
    MatrixXd samples(rows, cols);
    MotherOfAll urng(((double ) rand()) / RAND_MAX);
    Random::Set(urng);
    Uniform U;

    int i,j;

    for (j=0; j<cols; j++)
    {
        for (i=0; i<nsamples; i++)
        {
            samples(i,j)   = round(U.Next()*(max(0,j)-min(0,j))) + min(0,j);
        }
    }

    return samples;

}

//   Geometric random distribution
//   Input
//         p: probability vector (1xdim), ! must be 0<=p<=1 !
//         nsamples:  nsamples: number of samples
//   Output
//         samples: Geometric random numbers (nsamplesxdim)
//   Uses stocc library written by Agner Fog

MatrixXd ActCovariance::GeoRnd(MatrixXd p, int nsamples)
{
    int rows = nsamples;
    int cols = p.cols();
    MatrixXd samples(rows, cols);
    MotherOfAll urng(((double ) rand()) / RAND_MAX);
    Random::Set(urng);
    Uniform U;

    int i,j;

    for (j=0; j<cols; j++)
    {
        for (i=0; i<nsamples; i++)
        {
            samples(i,j) = ceil(abs(log(U.Next())/log(1-p(j)))-1);
            if(samples(i,j)-numeric_limits<double>::epsilon( )<0)
            {
                samples(i,j)=abs(samples(i,j)*0);
            }
        }
    }

    return samples;

}

//  Hypergeometric uniform random distribution
//   Input
//         n: the number of balls you take (1xdim), ! must be >=0 !
//         m: the number of red balls in the urn (1xdim), ! must be >=0 !
//         N: the total number of balls in the urn (1xdim), ! must be >=m !
//         nsamples:  nsamples: number of samples
//   Output
//         samples: Hypergeometric random numbers (nsamplesxdim)
//   Uses stocc library written by Agner Fog

MatrixXd ActCovariance::HypGeoRnd(MatrixXi n, MatrixXi m, MatrixXi N, int nsamples)
{
    int rows = nsamples;
    int cols = n.cols();
    MatrixXd samples(rows, cols);


    int i,j;
    for (j=0; j<cols; j++)
    {
        for (i=0; i<nsamples; i++)
        {
            samples(i,j) =  Hypergeometric(n(0,j), m(0,j), N(0,j));
        }
    }

    return samples;
}


// Function for passing form true anomaly to mean anomaly
// Input
//      theta: true anomaly [rad]
//      e: eccentricity
// Output
//      mean anomaly [rad]

double ActCovariance::theta2M(double theta, double e)

{ if (e<1)
  {
  double E = 2*atan(sqrt((1-e)/(1+e))*tan(theta/2));
  return E-e*sin(E);
  }
  else
  {
   double E = 2*atanh(sqrt((e-1)/(1+e))*tan(theta/2));
   return -E+e*sinh(E);
  }

}

// Function for passing form true anomaly to mean anomaly
// Input
//      M: mean anomaly [rad]
//      e: eccentricity
// Output
//      true anomaly [rad]

double ActCovariance::M2theta(double M, double e)

{
  double E = M;
  double ddf;
  int i;

  if (e<1)
  {
     for(i=1; i<8; i++)
     {
         ddf = (e*cos(E)-1);
         E  = E-(M-E + e*sin(E))/ddf;
     }
     return 2*atan(sqrt((1+e)/(1-e))*tan(E/2));
   }
  else
  {

     for(i=1; i<8; i++)
     {
        ddf = (1 - e*cosh(E));
        E  = E-(M + E - e*sinh(E))/ddf;
     }

     return 2*atan(sqrt((1+e)/(e-1))*tanh(E/2));
  }

}

// Function to transform orbital parameters to cartesian coordinates.
// Given the orbital parameters it returns the cartesian coordinates
// Input
//        a: semi major axis [length unit],
//        e: eccentricity [adim],
//        i: inclination [rad],
//        Om: RAAN [rad],
//        om: anomaly of the pericenter [rad],
//        m0: mean anomaly [rad],
//        mu: gravitational parameter [length unit^3/ time unit^2]
// Output
//        xx: cartesian coordinates (6x1)

MatrixXd ActCovariance::po2pv(double mu, double a, double e, double inc, double Om, double om, double m0)

{
MatrixXd A(3,3);
MatrixXd B(3,3);
MatrixXd C(3,3);
MatrixXd r(3,1);
MatrixXd v(3,1);
MatrixXd rr(3,1);
MatrixXd vv(3,1);
MatrixXd xx(6,1);
double p;
double theta = M2theta(m0,e);

A  << cos(om+theta), -sin(om+theta), 0,
      sin(om+theta),  cos(om+theta), 0,
                  0,              0, 1;

B  << 1,      0,       0,
      0, cos(inc),  -sin(inc),
      0, sin(inc),   cos(inc);

C  << cos(Om), -sin(Om),  0,
      sin(Om),  cos(Om),  0,
            0,        0,  1;

p = a*(1-e*e);

r << 1/(1+e*cos(theta))*p, 0, 0;
v << sqrt(mu/p)*e*sin(theta), sqrt(mu/p)*(1+e*cos(theta)), 0;

rr = C*B*A*r;
vv = C*B*A*v;
xx.block(0,0,3,1) = rr;
xx.block(3,0,3,1) = vv;


return xx;

}

// Function to transform the covariance matrix given in orbital parameters to
// cartesian coordinates. Given the orbital parameters it returns the Jacobian
// matrix of the trasformation from orbital parameters to cartesian coordinates
// Given cov the trasformation is: MAT*cov*MAT'
// Input
//        a: semi major axis [length unit],
//        e: eccentricity [adim],
//        i: inclination [rad],
//        Om: RAAN [rad],
//        om: anomaly of the pericenter [rad],
//        m0: mean anomaly [rad],
//        mu: gravitational parameter [length unit^3/ time unit^2]
// Output
//        MAT: output matrix (6x6)

MatrixXd ActCovariance::linpo2pv(double mu, double a, double e, double inc, double Om, double om, double m0) // da sostituire
{
    MatrixXd RR(3,6);
    MatrixXd VV(3,6);
    MatrixXd MAT(6,6);
    double term1, term2,term3,term4, term5, term6, term7, term8, term9, term10, term11;
    double si,ci,cO,sO,cot,sot,st,ct;
    double theta = M2theta(m0,e);

    si = sin(inc);
    ci = cos(inc);
    cO = cos(Om);
    sO = sin(Om);
    cot = cos(om+theta);
    sot = sin(om+theta);
    st = sin(theta);
    ct = cos(theta);

    term1 = sqrt((mu/a/(1-(e*e)))); //term1
    term2 = pow(1-(e*e),2); //term2
    term3 = pow((1+e*ct),2); //term3
    term4 = (1+e*ct);
    term5 = (sO*cot+cO*ci*sot);
    term6 = (cO*cot-sO*ci*sot);
    term7 = (-cO*sot-sO*ci*cot);
    term8 = (-sO*cot-cO*ci*sot);
    term9 = (-cO*cot+sO*ci*sot);
    term10 = (-sO*sot+cO*ci*cot);
    term11 = (sO*sot-cO*ci*cot);

    VV << // first line
        -1.0/2*term6/term1*e*st*mu/(a*a)/(1-(e*e))-1.0/2*term7/term1*term4*mu/(a*a)/(1-(e*e)),
        term6/term1*(e*e)*st*mu/a/term2+term6*term1*st+term7/term1*term4*mu/a/term2*e+term7*term1*ct,
        sO*si*sot*term1*e*st+sO*si*cot*term1*term4,
        term8*term1*e*st+term11*term1*term4,
        term7*term1*e*st+term9*term1*term4,
        term6*term1*e*ct+term9*term1*term4,

        // second line
        -1.0/2*term5/term1*e*st*mu/(a*a)/(1-(e*e))-1.0/2*term10/term1*term4*mu/(a*a)/(1-(e*e)),
        term5/term1*(e*e)*st*mu/a/term2+term5*term1*st+term10/term1*term4*mu/a/term2*e+term10*term1*ct,
        -cO*si*sot*term1*e*st-cO*si*cot*term1*term4,
        term6*term1*e*st+term7*term1*term4,
        term10*term1*e*st+term8*term1*term4,
        term5*term1*e*ct+term8*term1*term4,

        // third line
        -1.0/2*si*sot/term1*e*st*mu/(a*a)/(1-(e*e))-1.0/2*si*cot/term1*term4*mu/(a*a)/(1-(e*e)),
        si*sot/term1*(e*e)*st*mu/a/term2+si*sot*term1*st+si*cot/term1*term4*mu/a/term2*e+si*cot*term1*ct,
        ci*sot*term1*e*st+ci*cot*term1*term4,
        0,
        si*cot*term1*e*st-si*sot*term1*term4,
        si*sot*term1*e*ct-si*sot*term1*term4;

    RR << // first line
        term6*(1-(e*e))/term4,
        -2*term6*a*e/term4-term6*a*(1-(e*e))/term3*ct,
        sO*si*sot*a*(1-(e*e))/term4,
        term8*a*(1-(e*e))/term4,
        term7*a*(1-(e*e))/term4,
        term7*a*(1-(e*e))/term4+term6*a*(1-(e*e))/term3*e*st,

        // second line
        term5*(1-(e*e))/term4,
        -2*term5*a*e/term4-term5*a*(1-(e*e))/term3*ct,
        -cO*si*sot*a*(1-(e*e))/term4,
        term6*a*(1-(e*e))/term4,
        term10*a*(1-(e*e))/term4,
        term10*a*(1-(e*e))/term4+term5*a*(1-(e*e))/term3*e*st,

        // third line
        si*sot*(1-(e*e))/term4,
        -2*si*sot*a*e/term4-si*sot*a*(1-(e*e))/term3*ct,
        ci*sot*a*(1-(e*e))/term4,
        0,
        si*cot*a*(1-(e*e))/term4,
        si*cot*a*(1-(e*e))/term4+si*sot*a*(1-(e*e))/term3*e*st;

    MAT.block(0,0,3,6) = RR;
    MAT.block(3,0,3,6) = VV;

    MatrixXd m2t = linM2theta(e, m0);
    return MAT*m2t;
}

// Function to transform the covariance matrix given in cartesian coordinates to
// orbital parameters. Given the cartesian coordinates it returns the Jacobian
// matrix of the trasformation from cartesian coordinates to orbital parameters
// Given cov the trasformation is: MAT*cov*MAT'
// Input
//        mu: gravitational parameter [length unit^3/ time unit^2]
//        TmpVec: cartesian coordinates (6,1) [length unit x3, length/time unit x3]
// Output
//        MAT: output matrix (6x6)

MatrixXd ActCovariance::linpv2po(double mu, sta::StateVector TmpVec)
{

    sta::KeplerianElements TmpKep = cartesianTOorbital(mu, TmpVec);
    double tmpe, tmpi;

    if(TmpKep.Eccentricity==0)
    {
        tmpe=1e-6;
    }
    else
    {
        tmpe=TmpKep.Eccentricity;
    }

    if(TmpKep.Inclination==0)
    {
        tmpi=1e-6;
    }
    else
    {
        tmpi=TmpKep.Inclination;
    }

    MatrixXd MAT (6,6);
    MAT =  linpo2pv(mu,TmpKep.SemimajorAxis,tmpe,tmpi,TmpKep.AscendingNode,TmpKep.ArgumentOfPeriapsis,TmpKep.MeanAnomaly);
    return matInverse(MAT);

}

// Function to transform the covariance matrix given in orbital spherical coordinate to
// cartesian coordinates. Given the spherical coordinates it returns the Jacobian
// matrix of the trasformation from spherical coordinates to cartesian coordinates
// Given cov the trasformation is: MAT*cov*MAT'
/* Input
*        r: position modulud [length unit],
*        delta: latitude [rad],
*        tau: longitude [rad],
*        v: velocity modulus [length/time unit],
*        gamma: flight path angle [rad],
*        chi: heading angle [rad],
*  Output
*        MAT: output matrix (6x6)
*/

MatrixXd ActCovariance::linspherical2cartesian(double r, double delta, double tau, double v, double gamma, double chi) // da sostituire

{

  MatrixXd MAT(6,6);
  double sd,cd,st,ct,sg,cg,sc,cc;

  sd = sin(delta);
  cd = cos(delta);
  st = sin(tau);
  ct = cos(tau);
  sg = sin(gamma);
  cg = cos(gamma);
  sc = sin(chi);
  cc = cos(chi);

  MAT <<
  //first linesgsc
  cd*ct, -r*sd*ct,     -r*cd*st,  0,  0,  0,
  //second line
  cd*st, -r*sd*st,      r*cd*ct,  0,  0,  0,
  //third line
  sd,     r*cd,               0,  0,  0,  0,
  // fourth line
  0, v*(-sg*sd*ct-cg*cc*cd*ct), v*(-sg*cd*st+cg*cc*sd*st-cg*sc*ct),  sg*cd*ct-cg*cc*sd*ct-cg*sc*st,  v*(cg*cd*ct+sg*cc*sd*ct+sg*sc*st), v*(cg*sc*sd*ct-cg*cc*st),
  // fift line
  0, v*(-sg*sd*st-cg*cc*cd*st), v*(sg*cd*ct-cg*cc*sd*ct-cg*sc*st),   sg*cd*st-cg*cc*sd*st+cg*sc*ct,  v*(cg*cd*st+sg*cc*sd*st-sg*sc*ct), v*(cg*sc*sd*st+cg*cc*ct),
  // fouth line
  0, v*(-cg*cc*sd+sg*cd), 0, cg*cc*cd+sg*sd,  v*(-sg*cc*cd+cg*sd), -v*cg*sc*cd;
  return MAT;
}


// Function to transform the covariance matrix given in cartesian coordinates to
// spherical coordinates. Given cartesian coordinates it returns the Jacobian
// matrix of the trasformation from cartesian coordinates to spherical coordinates
// Given cov the trasformation is: MAT*cov*MAT'
/* Input
*        x: position x component [length unit],
*        y: position y component [length unit],
*        z: position z component [length unit],
*        vx: velocity x component [length/time unit],
*        vy: velocity y component [length/time unit],
*        vz: velocity z component [length/time unit],
*  Output
*        MAT: output matrix (6x6)
*/

MatrixXd ActCovariance::lincartesian2spherical(double x, double y, double z, double vx, double vy, double vz) // da sostituire

{
   double x2, y2, z2, z3, vx2, vy2, vz2, v2, v, v32, r2, r, r32, r4, vpr, vpr2;
   double term1, term2, term3, term4, term5, term6, term7;
   MatrixXd MAT (6,6);

    x2 = pow(x,2.0);
    y2 = pow(y,2.0);
    z2 = pow(z,2.0);
    z3 = pow(z,3.0);
    vx2 = pow(vx,2.0);
    vy2 = pow(vy,2.0);
    vz2 = pow(vz,2.0);

    v2  = (vx2 + vy2 + vz2);
    v   = pow(v2, 1.0/2.0);
    v32 = pow(v2, 3.0/2.0);

    r2  = (x2 + y2 + z2);
    r   = pow(r2, 1.0/2.0);
    r32 = pow(r2, 3.0/2.0);

    r4 = pow(r2, 2.0);
    vpr = (vx*x + vy*y + vz*z);
    vpr2 = pow((vx*x + vy*y + vz*z), 2.0);
    term1 = pow((1 - z2/r2), 1.0/2.0);
    term2 = pow((1 - vpr2/(v2*r2)), 1.0/2.0);
    term3 = pow((vy*x - vx*y), 2.0);
    term4 = pow((x*(vz*x - vx*z) + y*(vz*y - vy*z)),2.0);
    term5 = pow(r2, 5.0/2.0);
    term6 = pow((1 - z2/r2), 3.0/2.0);
    term7 = pow((x2/((z2/r2 - 1)*r2) + 1), 1.0/2.0);

    MAT <<  // first line
            x/r, y/r, z/r, 0, 0, 0,
            //second line
          -(x*z)/(term1*r32), -(y*z)/(term1*r32), (1/r - z2/r32)/term1, 0, 0, 0,
           // third line
           (x2/(term1*r32) - 1/(term1*r) + (x2*z2)/(term6*term5))/term7, ((x*y)/(term1*r32) + (x*y*z2)/(term6*term5))/term7, -((x*((2*z)/r2 - (2*z3)/r4))/(2*term6*r) - (x*z)/(term1*r32))/term7, 0, 0, 0,
           // fourth line
           0, 0, 0, vx/v, vy/v, vz/v,
           // fifth line
           (vx/(v*r) - (x*vpr)/(v*r32))/term2, (vy/(v*r) - (y*vpr)/(v*r32))/term2, (vz/(v*r) - (z*vpr)/(v*r32))/term2, (x/(v*r) - (vx*vpr)/(v32*r))/term2, (y/(v*r) - (vy*vpr)/(v32*r))/term2, (z/(v*r) - (vz*vpr)/(v32*r))/term2,
           //sixth line
           ((vy*r)/(x*(vz*x - vx*z) + y*(vz*y - vy*z)) - ((vy*x - vx*y)*(2*vz*x - vx*z)*r)/term4 + (x*(vy*x - vx*y))/((x*(vz*x - vx*z) + y*(vz*y - vy*z))*r))/((term3*r2)/term4 + 1), -((vx*r)/(x*(vz*x - vx*z) + y*(vz*y - vy*z)) + ((vy*x - vx*y)*(2*vz*y - vy*z)*r)/term4 - (y*(vy*x - vx*y))/((x*(vz*x - vx*z) + y*(vz*y - vy*z))*r))/((term3*r2)/term4 + 1),  (((vx*x + vy*y)*(vy*x - vx*y)*r)/term4 + (z*(vy*x - vx*y))/((x*(vz*x - vx*z) + y*(vz*y - vy*z))*r))/((term3*r2)/term4 + 1), -((y*r)/(x*(vz*x - vx*z) + y*(vz*y - vy*z)) - (x*z*(vy*x - vx*y)*r)/term4)/((term3*r2)/term4 + 1), ((x*r)/(x*(vz*x - vx*z) + y*(vz*y - vy*z)) + (y*z*(vy*x - vx*y)*r)/term4)/((term3*r2)/term4 + 1),  -((x2 + y2)*(vy*x - vx*y)*r)/(term4*((term3*r2)/term4 + 1));
 return MAT;
}

// Function to transform the covariance matrix given in planet fixed coordinates to
// inertial coordinates. Given the rotation matrix and the angular velocity of the Earth
//it returns the Jacobian matrix of the trasformation from planet fixed coordinates to inertial coordinates
// Given cov the trasformation is: MAT*cov*MAT'
/* Input
*        rotation: rotation matrix from body fixed to inertial
*        omega: angular velocity of the planet [rad/s]
* Output
*        MAT: output matrix (6x6)
*/

MatrixXd ActCovariance::linfixed2inertial(Matrix3d rotation, Vector3d omega)
{

MatrixXd MAT(6,6);
MatrixXd Om = MatrixXd::Zero(3,3);

Om(0,1) = -omega(2);
Om(0,2) = omega(1);
Om(1,0) = omega(2);
Om(1,2) = -omega(0);
Om(2,0) = -omega(1);
Om(2,1) = omega(0);

MAT.setZero(6,6);

MAT.block(0,0,3,3) = rotation;
MAT.block(3,3,3,3) = rotation;
MAT.block(3,0,3,3) = Om*rotation;

return MAT;

}

// Function to transform the covariance matrix given in inertial coordinates
// to planet fixed. Given the rotation matrix and the angular velocity of the Earth
// it returns the Jacobian matrix of the trasformation from inertial coordinates
// to planet fixed coordinates
// Given cov the trasformation is: MAT*cov*MAT'
/* Input
*        rotation: rotation matrix from inertial to fixed
*        omega: angular velocity of the planet [rad/s]
* Output
*        MAT: output matrix (6x6)
*/

MatrixXd ActCovariance::lininertial2fixed(Matrix3d rotation, Vector3d omega)
{

    MatrixXd MAT(6,6);
    MatrixXd Om = MatrixXd::Zero(3,3);

    Om(0,1) = -omega(2);
    Om(0,2) = omega(1);
    Om(1,0) = omega(2);
    Om(1,2) = -omega(0);
    Om(2,0) = -omega(1);
    Om(2,1) = omega(0);

    MAT.setZero(6,6);

    MAT.block(0,0,3,3) = rotation;
    MAT.block(3,3,3,3) = rotation;
    MAT.block(3,0,3,3) = Om*rotation;

    return MAT;

}


// Function to transform the covariance matrix given in rotating reference frame to
// inertial coordinates. Given the relative RAAN, longitute of the pericenter, inclination and
// angular velocity  it returns the Jacobian matrix of the trasformation from rotational coordinates
// to inertial coordinates (WITH SCALING!!!!!!!!!!)
// Given cov the trasformation is: MAT*cov*MAT'
/* Input
*        alfa: relative RAAN [rad]
*        beta: relative longitude of ascending node [rad]
*        i: inclination [rad]
*        omega: modulus of angular velocity [rad/time unit]
*        rnorm: scaling factor for positions
*        vnorm: scaling factor for velocities
* Output
*        MAT: output matrix (6x6)
*/

MatrixXd ActCovariance::linrotating2inertial(double alfa, double beta, double i, double rnorm, double vnorm)
{

  MatrixXd MAT(6,6);
  double sO,cO,sb,cb,si,ci;
  double omega = 1;

  sO = sin(alfa);
  cO = cos(alfa);
  sb = sin(beta);
  cb = cos(beta);
  si = sin(i);
  ci = cos(i);

MAT <<
// first row
cO*cb-sO*ci*sb, -cO*sb-sO*ci*cb, sO*si,  0,  0,  0,
// second row
sO*cb+cO*ci*sb, -sO*sb+cO*ci*cb, -cO*si, 0,  0,  0,
//third row
si*sb, si*cb, ci, 0, 0, 0,
//fourth row
-cO*omega*sb-sO*ci*omega*cb, -cO*omega*cb+sO*ci*omega*sb, 0, cO*cb-sO*ci*sb, -cO*sb-sO*ci*cb, sO*si,
//fifth row
-sO*omega*sb+cO*ci*omega*cb, -sO*omega*cb-cO*ci*omega*sb, 0, sO*cb+cO*ci*sb, -sO*sb+cO*ci*cb, -cO*si,
//sixth row
si*omega*cb, -si*omega*sb, 0, si*sb, si*cb, ci;

MAT.block(0,0,3,6) = MAT.block(0,0,3,6)*rnorm;
MAT.block(3,0,3,6) = MAT.block(3,0,3,6)*vnorm;

return MAT;

}

// Function to transform the covariance matrix given in inertial reference frame to
// rotating coordinates. Given the relative RAAN, longitute of the pericenter, inclination and
// angular velocity  it returns the Jacobian matrix of the trasformation from inertial coordinates
// to rotating coordinates (WITH SCALING,!!!!!!!)
// Given cov the trasformation is: MAT*cov*MAT'
/* Input
*        alfa: relative RAAN [rad]
*        beta: relative longitude of ascending node [rad]
*        i: relative inclination [rad]
*        omega: modulus of angular velocity [rad/time unit]
*        rnorm: scaling factor for positions
*        vnorm: scaling factor for velocities
* Output
*        MAT: output matrix (6x6)
*/

MatrixXd ActCovariance::lininertial2rotating(double alfa, double beta, double i, double rnorm, double vnorm)
{

  MatrixXd MAT(6,6);
  double sO,cO,sb,cb,si,ci;
  double omega=1;

  sO = sin(alfa);
  cO = cos(alfa);
  sb = sin(beta);
  cb = cos(beta);
  si = sin(i);
  ci = cos(i);

MAT <<
// first row
 cb*cO-sb*ci*sO,  cb*sO+sb*ci*cO, sb*si,  0, 0, 0,
// second row
-sb*cO-cb*ci*sO, -sb*sO+cb*ci*cO, cb*si,  0, 0, 0,
//third row
si*sO,  -si*cO,  ci, 0, 0, 0,
//fourth row
 omega*(-sb*cO-cb*ci*sO), omega*(-sb*sO+cb*ci*cO),  omega*cb*si,  cb*cO-sb*ci*sO,  cb*sO+sb*ci*cO, sb*si,
//fifth row
-omega*(cb*cO-sb*ci*sO), -omega*(cb*sO+sb*ci*cO), -omega*sb*si, -sb*cO-cb*ci*sO, -sb*sO+cb*ci*cO, cb*si,
//sixth row
0, 0, 0, si*sO, -si*cO, ci;

MAT.block(0,0,3,6) = MAT.block(0,0,3,6)/rnorm;
MAT.block(3,0,3,6) = MAT.block(3,0,3,6)/vnorm;

return MAT;
}

// Function to transform the covariance matrix given in orbital parameters with true anomaly to
// the covariance matrix in orbital parameters with mean anomaly. Given the true anomaly and the eccentricity
// it returns the Jacobian matrix of the trasformation
// Given cov the trasformation is: MAT*cov*MAT'
// Input
//       e: eccentricity
//       theta: true anomaly
// Output
//        MAT: output matrix (6x6)

MatrixXd ActCovariance::lintheta2M(double e, double theta)
{

MatrixXd MAT(6,6);
MAT << 1, 0, 0, 0, 0, 0,
       0, 1, 0, 0, 0, 0,
       0, 0, 1, 0, 0, 0,
       0, 0, 0, 1, 0, 0,
       0, 0, 0, 0, 1, 0,
       0, 0, 0, 0, 0, 1;

double c1 = sqrt((1-e)/(1+e));
double c2 = pow(tan(1.0/2*theta),2);
double c3 = pow((1+e),2);
double c4 = sqrt((e-1)/(1+e));

if (e<1)
{

MAT(5,1) = 1/c1*tan(1.0/2*theta)*(-1/(1+e)-(1-e)/c3)/(1+(1-e)/(1+e)*c2)-sin(2*atan(c1*tan(1.0/2*theta)))-e*cos(2*atan(c1*tan(1.0/2*theta)))/c1*tan(1.0/2*theta)*(-1/(1+e)-(1-e)/c3)/(1+(1-e)/(1+e)*c2);
MAT(5,5) = 2*c1*(1.0/2+1.0/2*c2)/(1+(1-e)/(1+e)*c2)-2*e*cos(2*atan(c1*tan(1.0/2*theta)))*c1*(1.0/2+1.0/2*c2)/(1+(1-e)/(1+e)*c2);

}
else

{
MAT(5,1) = -1/c4*tan(1.0/2*theta)*(1/(1+e)-(e-1)/c3)/(1-(e-1)/(1+e)*c2)+sinh(2*atanh(c4*tan(1.0/2*theta)))+e*cosh(2*atanh(c4*tan(1.0/2*theta)))/c4*tan(1.0/2*theta)*(1/(1+e)-(e-1)/c3)/(1-(e-1)/(1+e)*c2);
MAT(5,5) =-2*c4*(1.0/2+1.0/2*c2)/(1-(e-1)/(1+e)*c2)+2*e*cosh(2*atanh(c4*tan(1.0/2*theta)))*c4*(1.0/2+1.0/2*c2)/(1-(e-1)/(1+e)*c2);
}

return MAT;

}


// Function to transform the covariance matrix given in orbital parameters with mean anomaly to
// the covariance matrix in orbital parameters with true anomaly. Given the mean anomaly and the eccentricity
// it returns the Jacobian matrix of the trasformation
// Given cov the trasformation is: MAT*cov*MAT'
// Input
//       e: eccentricity
//       M: mean anomaly
// Output
//        MAT: output matrix (6x6)

MatrixXd ActCovariance::linM2theta(double e, double M)
{

MatrixXd MAT(6,6);
double theta;

theta = M2theta(M, e);
MAT = lintheta2M(e, theta);

return matInverse(MAT);

}


// Function to transform the covariance matrix when a rotation of the coordinates is performed.
// Given the rotation matrix it returns the Jacobian matrix of the trasformation
// Given cov the trasformation is: MAT*cov*MAT'
// Input
//       R: rotation matrix (3x3)
// Output
//       MAT: output matrix (6x6)

MatrixXd ActCovariance::linrot(MatrixXd R)
{
    MatrixXd MAT(6,6);
    MAT.setZero(6,6);
    MAT.block(0,0,3,3) = R;
    MAT.block(3,3,3,3) = R;
    return MAT;
}

void ActCovariance::toCartesianJ2000(sta::CoordinateSystemType coordType, ActElementType elemType)
{
    sta::StateVector TmpVecJ2000;
    sta::StateVector TmpVec;
    sta::CoordinateSystem reference(coordType);

    m_meanJ2000.resize(m_mean.rows(),m_mean.cols());
    m_samplesJ2000.resize(m_samples.rows(),m_samples.cols());
    m_covarianceJ2000.resize(m_covariance.rows(),m_covariance.cols());
    int i;
    if(reference.isInertial() && (elemType == act::ELEM_CARTESIAN || elemType == act::ELEM_KEPLERIAN))
    {

        if(elemType == act::ELEM_KEPLERIAN)
        {
            // mean conversion

            // convert from keplerian
            TmpVec = orbitalTOcartesian(m_centralbody->mu(), m_mean(0,0), m_mean(0,1), m_mean(0,2), m_mean(0,4), m_mean(0,3), m_mean(0,5));
            TmpVecJ2000 = reference.toEmeJ2000(TmpVec);

            for (i=0;i<3;i++)
            {
                m_meanJ2000(0,i) = TmpVecJ2000.position(i);
                m_meanJ2000(0,i+3) = TmpVecJ2000.velocity(i);
            }

            // covariance conversion
            MatrixXd JacKep = linpo2pv(m_centralbody->mu(), m_mean(0,0), m_mean(0,1), m_mean(0,2), m_mean(0,3), m_mean(0,4), m_mean(0,5));
            MatrixXd JacRot;

            JacRot = linrot(reference.rotToEmeJ2000());
            m_covarianceJ2000 =  JacRot * JacKep * m_covariance * JacKep.transpose() * JacRot.transpose();

            // samples generation
            m_samplesJ2000 = NormRnd(m_meanJ2000, m_covarianceJ2000, m_samples.rows());

        }
        else // elemType == act::ELEM_CARTESIAN
        {
            // mean conversion
            for (i=0;i<3;i++)
            {
                TmpVec.position(i) = m_mean(0,i);
                TmpVec.velocity(i) = m_mean(0,i+3);
            }

            TmpVecJ2000 = reference.toEmeJ2000(TmpVec);

            for (i=0;i<3;i++)
            {
                m_meanJ2000(0,i) = TmpVecJ2000.position(i);
                m_meanJ2000(0,i+3) = TmpVecJ2000.velocity(i);
            }


            // covariance conversion
            MatrixXd JacRot;

            JacRot = linrot(reference.rotToEmeJ2000());
            m_covarianceJ2000 =  JacRot * m_covariance * JacRot.transpose();

            // samples generation
            m_samplesJ2000 = NormRnd(m_meanJ2000, m_covarianceJ2000, m_samples.rows());

        }


    }
    else if(coordType == sta::COORDSYS_BODYFIXED && (elemType == act::ELEM_CARTESIAN || elemType == act::ELEM_SPHERICAL))
    {

        if(elemType == act::ELEM_SPHERICAL)
        {
            // mean conversion
            double x,y,z,vx,vy,vz;

            // convert from spherical
            sphericalTOcartesian(m_mean(0,2),m_mean(0,1),m_mean(0,0),m_mean(0,3),m_mean(0,4),m_mean(0,5),x,y,z,vx,vy,vz);

            TmpVec.position(0) = x;
            TmpVec.position(1) = y;
            TmpVec.position(2) = z;
            TmpVec.velocity(0) = vx;
            TmpVec.velocity(1) = vy;
            TmpVec.velocity(2) = vz;

            TmpVecJ2000 = reference.toEmeJ2000(TmpVec,m_centralbody,m_mjd);

            for (int i=0;i<3;i++)
            {
                m_meanJ2000(0,i) = TmpVecJ2000.position(i);
                m_meanJ2000(0,i+3) = TmpVecJ2000.velocity(i);
            }

            // covariance conversion
            MatrixXd JacSph = linspherical2cartesian(m_mean(0,0), m_mean(0,1), m_mean(0,2), m_mean(0,3), m_mean(0,4), m_mean(0,5));

            MatrixXd JacRot = linfixed2inertial(reference.rotToEmeJ2000(m_centralbody,m_mjd), reference.omegaToEmeJ2000(m_centralbody, m_mjd));

            m_covarianceJ2000 =  JacRot * JacSph * m_covariance * JacSph.transpose() * JacRot.transpose();

            std::cout << "actcovariance toCartesianJ2000" << std::endl;
            std::cout << "PHI1"<<  std::endl << JacSph << std::endl;
            std::cout << "PHI2"<<  std::endl << JacRot << std::endl;
            std::cout << "omega"<< std::endl << reference.omegaToEmeJ2000(m_centralbody, m_mjd) << std::endl;

            // samples generation
            m_samplesJ2000 = NormRnd(m_meanJ2000, m_covarianceJ2000, m_samples.rows());


        }
        else // elemType == act::ELEM_CARTESIAN
        {
            // mean conversion
            for (i=0;i<3;i++)
            {
                TmpVec.position(i) = m_mean(0,i);
                TmpVec.velocity(i) = m_mean(0,i+3);
            }

            TmpVecJ2000 = reference.toEmeJ2000(TmpVec,m_centralbody,m_mjd);

            for (int i=0;i<3;i++)
            {
                m_meanJ2000(0,i) = TmpVecJ2000.position(i);
                m_meanJ2000(0,i+3) = TmpVecJ2000.velocity(i);
            }

            // covariance conversion
            MatrixXd JacRot = linfixed2inertial(reference.rotToEmeJ2000(m_centralbody,m_mjd), reference.omegaToEmeJ2000(m_centralbody, m_mjd));

            std::cout << "PHI2"<<  std::endl << JacRot << std::endl;
            std::cout << "omega"<< std::endl << reference.omegaToEmeJ2000(m_centralbody, m_mjd) << std::endl ;

            m_covarianceJ2000 =  JacRot * m_covariance * JacRot.transpose();

            // samples generation
            m_samplesJ2000 = NormRnd(m_meanJ2000, m_covarianceJ2000, m_samples.rows());

        }

    }

    else if(coordType == sta::COORDSYS_ROT && elemType == act::ELEM_CARTESIAN)
    {
        // TODO
    }
    else if(coordType == sta::COORDSYS_ROT_NORM && elemType == act::ELEM_CARTESIAN)
    {
        // TODO
    }
    else // wrong combination of reference and element type
    {
        QMessageBox msgBox;
        msgBox.setText("The ActCovariance: In toCartesianJ2000() member function the Coordinate system is incompatible with the element type.");
        msgBox.setWindowTitle("ACT");
        msgBox.setIcon(QMessageBox::Warning);
        msgBox.exec();
        m_initialized = false;
        return;
    }

}

void ActCovariance::fromCartesianJ2000(sta::CoordinateSystemType coordType, ActElementType elemType)
{

    sta::StateVector TmpVecJ2000;
    sta::StateVector TmpVec;
    sta::KeplerianElements TmpKep;
    sta::CoordinateSystem reference(coordType);

    m_mean.resize(m_meanJ2000.rows(),m_meanJ2000.cols());
    m_samples.resize(m_samplesJ2000.rows(),m_samplesJ2000.cols());
    m_covariance.resize(m_covarianceJ2000.rows(),m_covarianceJ2000.cols());
    int i;
    if(reference.isInertial() && (elemType == act::ELEM_CARTESIAN || elemType == act::ELEM_KEPLERIAN))
    {

        if(elemType == act::ELEM_KEPLERIAN)
        {
            // mean conversion
            for (i=0;i<3;i++)
            {
                TmpVecJ2000.position(i) = m_meanJ2000(0,i);
                TmpVecJ2000.velocity(i) = m_meanJ2000(0,i+3);
            }

            // convert to keplerian
            TmpKep = cartesianTOorbital(m_centralbody->mu(), reference.fromEmeJ2000(TmpVecJ2000));

            m_mean(0,0) = TmpKep.SemimajorAxis;
            m_mean(0,1) = TmpKep.Eccentricity;
            m_mean(0,2) = TmpKep.Inclination;
            m_mean(0,3) = TmpKep.AscendingNode;
            m_mean(0,4) = TmpKep.ArgumentOfPeriapsis;
            m_mean(0,5) = TmpKep.MeanAnomaly;

            // covariance conversion
            MatrixXd JacKep = linpv2po(m_centralbody->mu(), reference.fromEmeJ2000(TmpVecJ2000));
            MatrixXd JacRot;

            JacRot = linrot(reference.rotFromEmeJ2000());
            m_covariance =  JacRot * JacKep * m_covarianceJ2000 * JacKep.transpose() * JacRot.transpose();

            // samples conversion
            m_samples = NormRnd(m_mean, m_covariance, m_samplesJ2000.rows());

        }
        else // elemType == act::ELEM_CARTESIAN
        {
            // mean conversion
            for (i=0;i<3;i++)
            {
                TmpVecJ2000.position(i) = m_meanJ2000(0,i);
                TmpVecJ2000.velocity(i) = m_meanJ2000(0,i+3);
            }

            TmpVec = reference.fromEmeJ2000(TmpVecJ2000);
            for (int i=0;i<3;i++)
            {
                m_mean(0,i) = TmpVec.position(i);
                m_mean(0,i+3) = TmpVec.velocity(i);
            }

            // covariance conversion
            MatrixXd JacRot;

            JacRot = linrot(reference.rotFromEmeJ2000());
            m_covariance =  JacRot * m_covarianceJ2000 * JacRot.transpose();

            // samples generation
            m_samples = NormRnd(m_mean, m_covariance, m_samplesJ2000.rows());

        }


    }
    else if(coordType == sta::COORDSYS_BODYFIXED && (elemType == act::ELEM_CARTESIAN || elemType == act::ELEM_SPHERICAL))
    {

        if(elemType == act::ELEM_SPHERICAL)
        {
            // mean conversion
            for (i=0;i<3;i++)
            {
                TmpVecJ2000.position(i) = m_meanJ2000(0,i);
                TmpVecJ2000.velocity(i) = m_meanJ2000(0,i+3);
            }

            TmpVec = reference.fromEmeJ2000(TmpVecJ2000, m_centralbody, m_mjd);

            // convert to spherical
            double r,v,tau,delta,gamma,chi;

            cartesianTOspherical(TmpVec.position(0),TmpVec.position(1),TmpVec.position(2),TmpVec.velocity(0),TmpVec.velocity(1),TmpVec.velocity(2), tau, delta, r, v, gamma, chi);
            m_mean(0,0) = r;
            m_mean(0,1) = delta;
            m_mean(0,2) = tau;
            m_mean(0,3) = v;
            m_mean(0,4) = gamma;
            m_mean(0,5) = chi;


            // covariance conversion
            MatrixXd JacSph = lincartesian2spherical(TmpVec.position(0),TmpVec.position(1),TmpVec.position(2),TmpVec.velocity(0),TmpVec.velocity(1),TmpVec.velocity(2));

            MatrixXd JacRot = lininertial2fixed(reference.rotFromEmeJ2000(m_centralbody,m_mjd), reference.omegaFromEmeJ2000(m_centralbody, m_mjd));

            m_covariance =  JacRot * JacSph * m_covarianceJ2000 * JacSph.transpose() * JacRot.transpose();

            // samples generation
            m_samples = NormRnd(m_mean, m_covariance, m_samplesJ2000.rows());

        }
        else // elemType == act::ELEM_CARTESIAN
        {


            // mean conversion
            for (i=0;i<3;i++)
            {
                TmpVecJ2000.position(i) = m_meanJ2000(0,i);
                TmpVecJ2000.velocity(i) = m_meanJ2000(0,i+3);
            }

            TmpVec = reference.fromEmeJ2000(TmpVecJ2000, m_centralbody, m_mjd);

            for (i=0;i<3;i++)
            {
                m_mean(0,i) = TmpVec.position(i);
                m_mean(0,i+3) = TmpVec.velocity(i);
            }


            // covariance conversion
            MatrixXd JacRot = lininertial2fixed(reference.rotFromEmeJ2000(m_centralbody,m_mjd), reference.omegaFromEmeJ2000(m_centralbody, m_mjd));

            m_covariance =  JacRot * m_covarianceJ2000 * JacRot.transpose();

            // samples generation
            m_samples = NormRnd(m_mean, m_covariance, m_samplesJ2000.rows());

        }

    }
    else if(coordType == sta::COORDSYS_ROT && elemType == act::ELEM_CARTESIAN)
    {
        // TODO
    }
    else if(coordType == sta::COORDSYS_ROT_NORM && elemType == act::ELEM_CARTESIAN)
    {
        // TODO
    }
    else // wrong combination of reference and element type
    {
        QMessageBox msgBox;
        msgBox.setText("The ActCovariance: In fromCartesianJ2000() member function the Coordinate system is incompatible with the element type.");
        msgBox.setWindowTitle("ACT");
        msgBox.setIcon(QMessageBox::Warning);
        msgBox.exec();
        m_initialized = false;
        return;
    }

}

void ActCovariance::toCartesianJ2000NonLinear(sta::CoordinateSystemType coordType, ActElementType elemType)
{
    sta::StateVector TmpVecJ2000;
    sta::StateVector TmpVec;
    sta::CoordinateSystem reference(coordType);

    m_meanJ2000.resize(m_mean.rows(),m_mean.cols());
    m_samplesJ2000.resize(m_samples.rows(),m_samples.cols());
    m_covarianceJ2000.resize(m_covariance.rows(),m_covariance.cols());
    int i,j;
    if(reference.isInertial() && (elemType == act::ELEM_CARTESIAN || elemType == act::ELEM_KEPLERIAN))
    {

        if(elemType == act::ELEM_KEPLERIAN)
        {
            // mean conversion

            // convert from keplerian
            TmpVec = orbitalTOcartesian(m_centralbody->mu(), m_mean(0,0), m_mean(0,1), m_mean(0,2), m_mean(0,4), m_mean(0,3), m_mean(0,5));
            TmpVecJ2000 = reference.toEmeJ2000(TmpVec);

            for (i=0;i<3;i++)
            {
                m_meanJ2000(0,i) = TmpVecJ2000.position(i);
                m_meanJ2000(0,i+3) = TmpVecJ2000.velocity(i);
            }

            // sample conversion
            for(i=0;i<m_samples.rows();i++)
            {
                TmpVec = orbitalTOcartesian(m_centralbody->mu(), m_samples(i,0), m_samples(i,1), m_samples(i,2), m_samples(i,4), m_samples(i,3), m_samples(i,5));
                TmpVecJ2000 = reference.toEmeJ2000(TmpVec);
                for(j=0;j<3;j++)
                {
                    m_samplesJ2000(i,j) = TmpVecJ2000.position(j);
                    m_samplesJ2000(i,j+3) = TmpVecJ2000.velocity(j);

                }
            }

            // covariance generation
            MatrixXd tmpmat(m_samplesJ2000.rows(),m_samplesJ2000.cols());
            for (i=0; i< m_samplesJ2000.cols();i++)
            {
                tmpmat.block(0, i, m_samplesJ2000.rows(), 1) = m_samplesJ2000.block(0, i, m_samplesJ2000.rows(), 1).cwise() - m_meanJ2000(0,i);
            }

            for (i=0; i< m_samplesJ2000.cols(); i++)
            {
                m_covarianceJ2000.block(i, 0, 1, m_samplesJ2000.cols()) = tmpmat.block(0,i,m_samplesJ2000.rows(),1).transpose()*tmpmat;
            }
            m_covarianceJ2000 = m_covarianceJ2000/(m_samplesJ2000.rows()-1);

        }
        else // elemType == act::ELEM_CARTESIAN
        {
            // mean conversion
            for (i=0;i<3;i++)
            {
                TmpVec.position(i) = m_mean(0,i);
                TmpVec.velocity(i) = m_mean(0,i+3);
            }

            TmpVecJ2000 = reference.toEmeJ2000(TmpVec);

            for (i=0;i<3;i++)
            {
                m_meanJ2000(0,i) = TmpVecJ2000.position(i);
                m_meanJ2000(0,i+3) = TmpVecJ2000.velocity(i);
            }


            // sample conversion
            for(i=0;i<m_samples.rows();i++)
            {
                for(j=0;j<3;j++)
                {
                    TmpVec.position(j) = m_samples(i,j);
                    TmpVec.velocity(j) = m_samples(i,j+3);

                }

                TmpVecJ2000 = reference.toEmeJ2000(TmpVec);
                for(j=0;j<3;j++)
                {
                    m_samplesJ2000(i,j) = TmpVecJ2000.position(j);
                    m_samplesJ2000(i,j+3) = TmpVecJ2000.velocity(j);

                }
            }

            // covariance generation
            MatrixXd tmpmat(m_samplesJ2000.rows(),m_samplesJ2000.cols());
            for (i=0; i< m_samplesJ2000.cols();i++)
            {
                tmpmat.block(0, i, m_samplesJ2000.rows(), 1) = m_samplesJ2000.block(0, i, m_samplesJ2000.rows(), 1).cwise() - m_meanJ2000(0,i);
            }

            for (i=0; i< m_samplesJ2000.cols(); i++)
            {
                m_covarianceJ2000.block(i, 0, 1, m_samplesJ2000.cols()) = tmpmat.block(0,i,m_samplesJ2000.rows(),1).transpose()*tmpmat;
            }
            m_covarianceJ2000 = m_covarianceJ2000/(m_samplesJ2000.rows()-1);



        }


    }
    else if(coordType == sta::COORDSYS_BODYFIXED && (elemType == act::ELEM_CARTESIAN || elemType == act::ELEM_SPHERICAL))
    {

        if(elemType == act::ELEM_SPHERICAL)
        {
            // mean conversion
            double x,y,z,vx,vy,vz;

            // convert from spherical
            sphericalTOcartesian(m_mean(0,2),m_mean(0,1),m_mean(0,0),m_mean(0,3),m_mean(0,4),m_mean(0,5),x,y,z,vx,vy,vz);

            TmpVec.position(0) = x;
            TmpVec.position(1) = y;
            TmpVec.position(2) = z;
            TmpVec.velocity(0) = vx;
            TmpVec.velocity(1) = vy;
            TmpVec.velocity(2) = vz;

            TmpVecJ2000 = reference.toEmeJ2000(TmpVec,m_centralbody,m_mjd);

            for (int i=0;i<3;i++)
            {
                m_meanJ2000(0,i) = TmpVecJ2000.position(i);
                m_meanJ2000(0,i+3) = TmpVecJ2000.velocity(i);
            }

            // sample conversion
            for(i=0;i<m_samples.rows();i++)
            {
                sphericalTOcartesian(m_samples(i,2),m_samples(i,1),m_samples(i,0),m_samples(i,3),m_samples(i,4),m_samples(i,5),x,y,z,vx,vy,vz);

                TmpVec.position(0) = x;
                TmpVec.position(1) = y;
                TmpVec.position(2) = z;
                TmpVec.velocity(0) = vx;
                TmpVec.velocity(1) = vy;
                TmpVec.velocity(2) = vz;

                TmpVecJ2000 = reference.toEmeJ2000(TmpVec,m_centralbody,m_mjd);

                for(j=0;j<3;j++)
                {
                    m_samplesJ2000(i,j) = TmpVecJ2000.position(j);
                    m_samplesJ2000(i,j+3) = TmpVecJ2000.velocity(j);

                }
            }

            // covariance generation
            MatrixXd tmpmat(m_samplesJ2000.rows(),m_samplesJ2000.cols());
            for (i=0; i< m_samplesJ2000.cols();i++)
            {
                tmpmat.block(0, i, m_samplesJ2000.rows(), 1) = m_samplesJ2000.block(0, i, m_samplesJ2000.rows(), 1).cwise() - m_meanJ2000(0,i);
            }

            for (i=0; i< m_samplesJ2000.cols(); i++)
            {
                m_covarianceJ2000.block(i, 0, 1, m_samplesJ2000.cols()) = tmpmat.block(0,i,m_samplesJ2000.rows(),1).transpose()*tmpmat;
            }
            m_covarianceJ2000 = m_covarianceJ2000/(m_samplesJ2000.rows()-1);


        }
        else // elemType == act::ELEM_CARTESIAN
        {
            // mean conversion
            for (i=0;i<3;i++)
            {
                TmpVec.position(i) = m_mean(0,i);
                TmpVec.velocity(i) = m_mean(0,i+3);
            }

            TmpVecJ2000 = reference.toEmeJ2000(TmpVec,m_centralbody,m_mjd);

            for (int i=0;i<3;i++)
            {
                m_meanJ2000(0,i) = TmpVecJ2000.position(i);
                m_meanJ2000(0,i+3) = TmpVecJ2000.velocity(i);
            }

            // sample conversion
            for(i=0;i<m_samples.rows();i++)
            {

                for (j=0;j<3;j++)
                {
                    TmpVec.position(j) = m_samples(i,j);
                    TmpVec.velocity(j) = m_samples(i,j+3);
                }

                TmpVecJ2000 = reference.toEmeJ2000(TmpVec,m_centralbody,m_mjd);

                for(j=0;j<3;j++)
                {
                    m_samplesJ2000(i,j) = TmpVecJ2000.position(j);
                    m_samplesJ2000(i,j+3) = TmpVecJ2000.velocity(j);

                }
            }

            // covariance generation
            MatrixXd tmpmat(m_samplesJ2000.rows(),m_samplesJ2000.cols());
            for (i=0; i< m_samplesJ2000.cols();i++)
            {
                tmpmat.block(0, i, m_samplesJ2000.rows(), 1) = m_samplesJ2000.block(0, i, m_samplesJ2000.rows(), 1).cwise() - m_meanJ2000(0,i);
            }

            for (i=0; i< m_samplesJ2000.cols(); i++)
            {
                m_covarianceJ2000.block(i, 0, 1, m_samplesJ2000.cols()) = tmpmat.block(0,i,m_samplesJ2000.rows(),1).transpose()*tmpmat;
            }
            m_covarianceJ2000 = m_covarianceJ2000/(m_samplesJ2000.rows()-1);

        }

    }

    else if(coordType == sta::COORDSYS_ROT && elemType == act::ELEM_CARTESIAN)
    {
        // TODO
    }
    else if(coordType == sta::COORDSYS_ROT_NORM && elemType == act::ELEM_CARTESIAN)
    {
        // TODO
    }
    else // wrong combination of reference and element type
    {
        QMessageBox msgBox;
        msgBox.setText("The ActCovariance: In toCartesianJ2000() member function the Coordinate system is incompatible with the element type.");
        msgBox.setWindowTitle("ACT");
        msgBox.setIcon(QMessageBox::Warning);
        msgBox.exec();
        m_initialized = false;
        return;
    }

}

void ActCovariance::fromCartesianJ2000NonLinear(sta::CoordinateSystemType coordType, ActElementType elemType)
{

    sta::StateVector TmpVecJ2000;
    sta::StateVector TmpVec;
    sta::KeplerianElements TmpKep;
    sta::CoordinateSystem reference(coordType);

    m_mean.resize(m_meanJ2000.rows(),m_meanJ2000.cols());
    m_samples.resize(m_samplesJ2000.rows(),m_samplesJ2000.cols());
    m_covariance.resize(m_covarianceJ2000.rows(),m_covarianceJ2000.cols());
    int i,j;
    if(reference.isInertial() && (elemType == act::ELEM_CARTESIAN || elemType == act::ELEM_KEPLERIAN))
    {

        if(elemType == act::ELEM_KEPLERIAN)
        {
            // mean conversion
            for (i=0;i<3;i++)
            {
                TmpVecJ2000.position(i) = m_meanJ2000(0,i);
                TmpVecJ2000.velocity(i) = m_meanJ2000(0,i+3);
            }

            // convert to keplerian
            TmpKep = cartesianTOorbital(m_centralbody->mu(), reference.fromEmeJ2000(TmpVecJ2000));

            m_mean(0,0) = TmpKep.SemimajorAxis;
            m_mean(0,1) = TmpKep.Eccentricity;
            m_mean(0,2) = TmpKep.Inclination;
            m_mean(0,3) = TmpKep.AscendingNode;
            m_mean(0,4) = TmpKep.ArgumentOfPeriapsis;
            m_mean(0,5) = TmpKep.MeanAnomaly;

            // sample conversion
            for(i=0;i<m_samplesJ2000.rows();i++)
            {
                for (j=0;j<3;j++)
                {
                    TmpVecJ2000.position(j) = m_samplesJ2000(i,j);
                    TmpVecJ2000.velocity(j) = m_samplesJ2000(i,j+3);
                }

                TmpKep = cartesianTOorbital(m_centralbody->mu(), reference.fromEmeJ2000(TmpVecJ2000));

                m_samples(i,0) = TmpKep.SemimajorAxis;
                m_samples(i,1) = TmpKep.Eccentricity;
                m_samples(i,2) = TmpKep.Inclination;
                m_samples(i,3) = TmpKep.AscendingNode;
                m_samples(i,4) = TmpKep.ArgumentOfPeriapsis;
                m_samples(i,5) = TmpKep.MeanAnomaly;


            }

            // covariance generation
            MatrixXd tmpmat(m_samples.rows(),m_samples.cols());
            for (i=0; i< m_samples.cols();i++)
            {
                tmpmat.block(0, i, m_samples.rows(), 1) = m_samples.block(0, i, m_samples.rows(), 1).cwise() - m_mean(0,i);
            }

            for (i=0; i< m_samples.cols(); i++)
            {
                m_covariance.block(i, 0, 1, m_samples.cols()) = tmpmat.block(0,i,m_samples.rows(),1).transpose()*tmpmat;
            }
            m_covariance = m_covariance/(m_samples.rows()-1);

        }
        else // elemType == act::ELEM_CARTESIAN
        {
            // mean conversion
            for (i=0;i<3;i++)
            {
                TmpVecJ2000.position(i) = m_meanJ2000(0,i);
                TmpVecJ2000.velocity(i) = m_meanJ2000(0,i+3);
            }

            TmpVec = reference.fromEmeJ2000(TmpVecJ2000);
            for (int i=0;i<3;i++)
            {
                m_mean(0,i) = TmpVec.position(i);
                m_mean(0,i+3) = TmpVec.velocity(i);
            }

            // sample conversion
            for(i=0;i<m_samplesJ2000.rows();i++)
            {
                for (j=0;j<3;j++)
                {
                    TmpVecJ2000.position(j) = m_samplesJ2000(i,j);
                    TmpVecJ2000.velocity(j) = m_samplesJ2000(i,j+3);
                }

                TmpVec = reference.fromEmeJ2000(TmpVecJ2000);

                for (int j=0;j<3;j++)
                {
                    m_samples(i,j) = TmpVec.position(j);
                    m_samples(i,j+3) = TmpVec.velocity(j);
                }


            }

            // covariance generation
            MatrixXd tmpmat(m_samples.rows(),m_samples.cols());
            for (i=0; i< m_samples.cols();i++)
            {
                tmpmat.block(0, i, m_samples.rows(), 1) = m_samples.block(0, i, m_samples.rows(), 1).cwise() - m_mean(0,i);
            }

            for (i=0; i< m_samples.cols(); i++)
            {
                m_covariance.block(i, 0, 1, m_samples.cols()) = tmpmat.block(0,i,m_samples.rows(),1).transpose()*tmpmat;
            }
            m_covariance = m_covariance/(m_samples.rows()-1);

        }


    }
    else if(coordType == sta::COORDSYS_BODYFIXED && (elemType == act::ELEM_CARTESIAN || elemType == act::ELEM_SPHERICAL))
    {

        if(elemType == act::ELEM_SPHERICAL)
        {
            // mean conversion
            for (i=0;i<3;i++)
            {
                TmpVecJ2000.position(i) = m_meanJ2000(0,i);
                TmpVecJ2000.velocity(i) = m_meanJ2000(0,i+3);
            }

            TmpVec = reference.fromEmeJ2000(TmpVecJ2000, m_centralbody, m_mjd);

            // convert to spherical
            double r,v,tau,delta,gamma,chi;

            cartesianTOspherical(TmpVec.position(0),TmpVec.position(1),TmpVec.position(2),TmpVec.velocity(0),TmpVec.velocity(1),TmpVec.velocity(2), tau, delta, r, v, gamma, chi);
            m_mean(0,0) = r;
            m_mean(0,1) = delta;
            m_mean(0,2) = tau;
            m_mean(0,3) = v;
            m_mean(0,4) = gamma;
            m_mean(0,5) = chi;


            // sample conversion
            for(i=0;i<m_samplesJ2000.rows();i++)
            {
                for (j=0;j<3;j++)
                {
                    TmpVecJ2000.position(j) = m_samplesJ2000(i,j);
                    TmpVecJ2000.velocity(j) = m_samplesJ2000(i,j+3);
                }

                TmpVec = reference.fromEmeJ2000(TmpVecJ2000, m_centralbody, m_mjd);

                cartesianTOspherical(TmpVec.position(0),TmpVec.position(1),TmpVec.position(2),TmpVec.velocity(0),TmpVec.velocity(1),TmpVec.velocity(2), tau, delta, r, v, gamma, chi);

                m_samples(i,0) = r;
                m_samples(i,1) = delta;
                m_samples(i,2) = tau;
                m_samples(i,3) = v;
                m_samples(i,4) = gamma;
                m_samples(i,5) = chi;

            }

            // covariance generation
            MatrixXd tmpmat(m_samples.rows(),m_samples.cols());
            for (i=0; i< m_samples.cols();i++)
            {
                tmpmat.block(0, i, m_samples.rows(), 1) = m_samples.block(0, i, m_samples.rows(), 1).cwise() - m_mean(0,i);
            }

            for (i=0; i< m_samples.cols(); i++)
            {
                m_covariance.block(i, 0, 1, m_samples.cols()) = tmpmat.block(0,i,m_samples.rows(),1).transpose()*tmpmat;
            }
            m_covariance = m_covariance/(m_samples.rows()-1);

        }
        else // elemType == act::ELEM_CARTESIAN
        {


            // mean conversion
            for (i=0;i<3;i++)
            {
                TmpVecJ2000.position(i) = m_meanJ2000(0,i);
                TmpVecJ2000.velocity(i) = m_meanJ2000(0,i+3);
            }

            TmpVec = reference.fromEmeJ2000(TmpVecJ2000, m_centralbody, m_mjd);

            for (i=0;i<3;i++)
            {
                m_mean(0,i) = TmpVec.position(i);
                m_mean(0,i+3) = TmpVec.velocity(i);
            }


            // sample conversion
            for(i=0;i<m_samplesJ2000.rows();i++)
            {
                for (j=0;j<3;j++)
                {
                    TmpVecJ2000.position(j) = m_samplesJ2000(i,j);
                    TmpVecJ2000.velocity(j) = m_samplesJ2000(i,j+3);
                }

                TmpVec = reference.fromEmeJ2000(TmpVecJ2000, m_centralbody, m_mjd);

                for (int j=0;j<3;j++)
                {
                    m_samples(i,j) = TmpVec.position(j);
                    m_samples(i,j+3) = TmpVec.velocity(j);
                }


            }

            // covariance generation
            MatrixXd tmpmat(m_samples.rows(),m_samples.cols());
            for (i=0; i< m_samples.cols();i++)
            {
                tmpmat.block(0, i, m_samples.rows(), 1) = m_samples.block(0, i, m_samples.rows(), 1).cwise() - m_mean(0,i);
            }

            for (i=0; i< m_samples.cols(); i++)
            {
                m_covariance.block(i, 0, 1, m_samples.cols()) = tmpmat.block(0,i,m_samples.rows(),1).transpose()*tmpmat;
            }
            m_covariance = m_covariance/(m_samples.rows()-1);


        }

    }
    else if(coordType == sta::COORDSYS_ROT && elemType == act::ELEM_CARTESIAN)
    {
        // TODO
    }
    else if(coordType == sta::COORDSYS_ROT_NORM && elemType == act::ELEM_CARTESIAN)
    {
        // TODO
    }
    else // wrong combination of reference and element type
    {
        QMessageBox msgBox;
        msgBox.setText("The ActCovariance: In fromCartesianJ2000() member function the Coordinate system is incompatible with the element type.");
        msgBox.setWindowTitle("ACT");
        msgBox.setIcon(QMessageBox::Warning);
        msgBox.exec();
        m_initialized = false;
        return;
    }

}
////////////////////////////////////////////////////////////////////////////////
// File: jacobi_cyclic_method.c                                               //
// Routines:                                                                  //
//    Jacobi_Cyclic_Method                                                    //
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//  void Jacobi_Cyclic_Method                                                 //
//            (double eigenvalues[], double *eigenvectors, double *A, int n)  //
//                                                                            //
//  Description:                                                              //
//     Find the eigenvalues and eigenvectors of a symmetric n x n matrix A    //
//     using the Jacobi method. Upon return, the input matrix A will have     //
//     been modified.                                                         //
//     The Jacobi procedure for finding the eigenvalues and eigenvectors of a //
//     symmetric matrix A is based on finding a similarity transformation     //
//     which diagonalizes A.  The similarity transformation is given by a     //
//     product of a sequence of orthogonal (rotation) matrices each of which  //
//     annihilates an off-diagonal element and its transpose.  The rotation   //
//     effects only the rows and columns containing the off-diagonal element  //
//     and its transpose, i.e. if a[i][j] is an off-diagonal element, then    //
//     the orthogonal transformation rotates rows a[i][] and a[j][], and      //
//     equivalently it rotates columns a[][i] and a[][j], so that a[i][j] = 0 //
//     and a[j][i] = 0.                                                       //
//     The cyclic Jacobi method considers the off-diagonal elements in the    //
//     following order: (0,1),(0,2),...,(0,n-1),(1,2),...,(n-2,n-1).  If the  //
//     the magnitude of the off-diagonal element is greater than a treshold,  //
//     then a rotation is performed to annihilate that off-diagnonal element. //
//     The process described above is called a sweep.  After a sweep has been //
//     completed, the threshold is lowered and another sweep is performed     //
//     with the new threshold. This process is completed until the final      //
//     sweep is performed with the final threshold.                           //
//     The orthogonal transformation which annihilates the matrix element     //
//     a[k][m], k != m, is Q = q[i][j], where q[i][j] = 0 if i != j, i,j != k //
//     i,j != m and q[i][j] = 1 if i = j, i,j != k, i,j != m, q[k][k] =       //
//     q[m][m] = cos(phi), q[k][m] = -sin(phi), and q[m][k] = sin(phi), where //
//     the angle phi is determined by requiring a[k][m] -> 0.  This condition //
//     on the angle phi is equivalent to                                      //
//               cot(2 phi) = 0.5 * (a[k][k] - a[m][m]) / a[k][m]             //
//     Since tan(2 phi) = 2 tan(phi) / (1.0 - tan(phi)^2),                    //
//               tan(phi)^2 + 2cot(2 phi) * tan(phi) - 1 = 0.                 //
//     Solving for tan(phi), choosing the solution with smallest magnitude,   //
//       tan(phi) = - cot(2 phi) + sgn(cot(2 phi)) sqrt(cot(2phi)^2 + 1).     //
//     Then cos(phi)^2 = 1 / (1 + tan(phi)^2) and sin(phi)^2 = 1 - cos(phi)^2 //
//     Finally by taking the sqrts and assigning the sign to the sin the same //
//     as that of the tan, the orthogonal transformation Q is determined.     //
//     Let A" be the matrix obtained from the matrix A by applying the        //
//     similarity transformation Q, since Q is orthogonal, A" = Q'AQ, where Q'//
//     is the transpose of Q (which is the same as the inverse of Q).  Then   //
//         a"[i][j] = Q'[i][p] a[p][q] Q[q][j] = Q[p][i] a[p][q] Q[q][j],     //
//     where repeated indices are summed over.                                //
//     If i is not equal to either k or m, then Q[i][j] is the Kronecker      //
//     delta.   So if both i and j are not equal to either k or m,            //
//                                a"[i][j] = a[i][j].                         //
//     If i = k, j = k,                                                       //
//        a"[k][k] =                                                          //
//           a[k][k]*cos(phi)^2 + a[k][m]*sin(2 phi) + a[m][m]*sin(phi)^2     //
//     If i = k, j = m,                                                       //
//        a"[k][m] = a"[m][k] = 0 =                                           //
//           a[k][m]*cos(2 phi) + 0.5 * (a[m][m] - a[k][k])*sin(2 phi)        //
//     If i = k, j != k or m,                                                 //
//        a"[k][j] = a"[j][k] = a[k][j] * cos(phi) + a[m][j] * sin(phi)       //
//     If i = m, j = k, a"[m][k] = 0                                          //
//     If i = m, j = m,                                                       //
//        a"[m][m] =                                                          //
//           a[m][m]*cos(phi)^2 - a[k][m]*sin(2 phi) + a[k][k]*sin(phi)^2     //
//     If i= m, j != k or m,                                                  //
//        a"[m][j] = a"[j][m] = a[m][j] * cos(phi) - a[k][j] * sin(phi)       //
//                                                                            //
//     If X is the matrix of normalized eigenvectors stored so that the ith   //
//     column corresponds to the ith eigenvalue, then AX = X Lamda, where     //
//     Lambda is the diagonal matrix with the ith eigenvalue stored at        //
//     Lambda[i][i], i.e. X'AX = Lambda and X is orthogonal, the eigenvectors //
//     are normalized and orthogonal.  So, X = Q1 Q2 ... Qs, where Qi is      //
//     the ith orthogonal matrix,  i.e. X can be recursively approximated by  //
//     the recursion relation X" = X Q, where Q is the orthogonal matrix and  //
//     the initial estimate for X is the identity matrix.                     //
//     If j = k, then x"[i][k] = x[i][k] * cos(phi) + x[i][m] * sin(phi),     //
//     if j = m, then x"[i][m] = x[i][m] * cos(phi) - x[i][k] * sin(phi), and //
//     if j != k and j != m, then x"[i][j] = x[i][j].                         //
//                                                                            //
//  Arguments:                                                                //
//     double  eigenvalues                                                    //
//        Array of dimension n, which upon return contains the eigenvalues of //
//        the matrix A.                                                       //
//     double* eigenvectors                                                   //
//        Matrix of eigenvectors, the ith column of which contains an         //
//        eigenvector corresponding to the ith eigenvalue in the array        //
//        eigenvalues.                                                        //
//     double* A                                                              //
//        Pointer to the first element of the symmetric n x n matrix A. The   //
//        input matrix A is modified during the process.                      //
//     int     n                                                              //
//        The dimension of the array eigenvalues, number of columns and rows  //
//        of the matrices eigenvectors and A.                                 //
//                                                                            //
//  Return Values:                                                            //
//     Function is of type void.                                              //
//                                                                            //
//  Example:                                                                  //
//     #define N                                                              //
//     double A[N][N], double eigenvalues[N], double eigenvectors[N][N]       //
//                                                                            //
//     (your code to initialize the matrix A )                                //
//                                                                            //
//     Jacobi_Cyclic_Method(eigenvalues, (double*)eigenvectors,               //
//                                                          (double *) A, N); //
////////////////////////////////////////////////////////////////////////////////
//                                                                            //

void ActCovariance::Jacobi_Cyclic_Method(double eigenvalues[], double *eigenvectors,
                                                              double *A, int n)
{
   int  i, j, k, m;
   double *pAk, *pAm, *p_r, *p_e;
   double threshold_norm;
   double threshold;
   double tan_phi, sin_phi, cos_phi, tan2_phi, sin2_phi, cos2_phi;
   double sin_2phi, cos_2phi, cot_2phi;
   double dum1;
   double dum2;
   double dum3;
   double max;

                  // Take care of trivial cases

   if ( n < 1) return;
   if ( n == 1) {
      eigenvalues[0] = *A;
      *eigenvectors = 1.0;
      return;
   }

          // Initialize the eigenvalues to the identity matrix.

   for (p_e = eigenvectors, i = 0; i < n; i++)
      for (j = 0; j < n; p_e++, j++)
         if (i == j) *p_e = 1.0; else *p_e = 0.0;

            // Calculate the threshold and threshold_norm.

   for (threshold = 0.0, pAk = A, i = 0; i < ( n - 1 ); pAk += n, i++)
      for (j = i + 1; j < n; j++) threshold += *(pAk + j) * *(pAk + j);
   threshold = sqrt(threshold + threshold);
   threshold_norm = threshold * DBL_EPSILON;
   max = threshold + 1.0;
   while (threshold > threshold_norm) {
      threshold /= 10.0;
      if (max < threshold) continue;
      max = 0.0;
      for (pAk = A, k = 0; k < (n-1); pAk += n, k++) {
         for (pAm = pAk + n, m = k + 1; m < n; pAm += n, m++) {
            if ( fabs(*(pAk + m)) < threshold ) continue;

                 // Calculate the sin and cos of the rotation angle which
                 // annihilates A[k][m].

            cot_2phi = 0.5 * ( *(pAk + k) - *(pAm + m) ) / *(pAk + m);
            dum1 = sqrt( cot_2phi * cot_2phi + 1.0);
            if (cot_2phi < 0.0) dum1 = -dum1;
            tan_phi = -cot_2phi + dum1;
            tan2_phi = tan_phi * tan_phi;
            sin2_phi = tan2_phi / (1.0 + tan2_phi);
            cos2_phi = 1.0 - sin2_phi;
            sin_phi = sqrt(sin2_phi);
            if (tan_phi < 0.0) sin_phi = - sin_phi;
            cos_phi = sqrt(cos2_phi);
            sin_2phi = 2.0 * sin_phi * cos_phi;
            cos_2phi = cos2_phi - sin2_phi;

                     // Rotate columns k and m for both the matrix A
                     //     and the matrix of eigenvectors.

            p_r = A;
            dum1 = *(pAk + k);
            dum2 = *(pAm + m);
            dum3 = *(pAk + m);
            *(pAk + k) = dum1 * cos2_phi + dum2 * sin2_phi + dum3 * sin_2phi;
            *(pAm + m) = dum1 * sin2_phi + dum2 * cos2_phi - dum3 * sin_2phi;
            *(pAk + m) = 0.0;
            *(pAm + k) = 0.0;
            for (i = 0; i < n; p_r += n, i++) {
               if ( (i == k) || (i == m) ) continue;
               if ( i < k ) dum1 = *(p_r + k); else dum1 = *(pAk + i);
               if ( i < m ) dum2 = *(p_r + m); else dum2 = *(pAm + i);
               dum3 = dum1 * cos_phi + dum2 * sin_phi;
               if ( i < k ) *(p_r + k) = dum3; else *(pAk + i) = dum3;
               dum3 = - dum1 * sin_phi + dum2 * cos_phi;
               if ( i < m ) *(p_r + m) = dum3; else *(pAm + i) = dum3;
            }
            for (p_e = eigenvectors, i = 0; i < n; p_e += n, i++) {
               dum1 = *(p_e + k);
               dum2 = *(p_e + m);
               *(p_e + k) = dum1 * cos_phi + dum2 * sin_phi;
               *(p_e + m) = - dum1 * sin_phi + dum2 * cos_phi;
            }
         }
         for (i = 0; i < n; i++)
            if ( i == k ) continue;
            else if ( max < fabs(*(pAk + i))) max = fabs(*(pAk + i));
      }
   }
   for (pAk = A, k = 0; k < n; pAk += n, k++) eigenvalues[k] = *(pAk + k);
}

// wrapper for eigenvalues and eigenvector computation
void ActCovariance::Eigenval(MatrixXd mat, MatrixXd &eigenval, MatrixXd &eigenvec, int dim)
{
    double eigenvalues[dim];
    double eigenvectors[dim][dim];
    double A[dim][dim];

    int indi, indj, ind, i,j;



#ifdef USEEIGENEIG

    Eigen::EigenSolver<MatrixXd> solver(mat);

    MatrixXd autoval = solver.eigenvalues().real().transpose();
    MatrixXd autovec = solver.eigenvectors().real();

    for(ind=0;ind<dim;ind++)
    {
      eigenvalues[ind]=autoval(0,ind);
      if(eigenvalues[ind]< 0)
      {
          eigenvalues[ind] = 0;
      }
      for(indj=0;indj<dim;indj++)
      {
        eigenvectors[ind][indj]=autovec(ind,indj);
      }
    }

#else

    for(indi=0; indi<dim; indi++)
    {
        for(indj=0; indj<dim; indj++)
        {
            A[indi][indj] = mat(indi,indj);
            eigenval(indi, indj) =0;
        }
    }

  Jacobi_Cyclic_Method(eigenvalues, (double*) eigenvectors, (double*) A, dim);


  for(ind=0;ind<dim;ind++)
  {
    if(eigenvalues[ind]< 0)
    {
        eigenvalues[ind] = 0;
    }
  }

#endif

  vector<double> tmpeig;


  vector<size_t> b;
  for (i = 0; i < dim; i++)
  {
    b.push_back(i);
    tmpeig.push_back(eigenvalues[i]);
  }


  std::sort(b.begin(), b.end(), index_cmp<vector<double>&>(tmpeig));

  for (i = 0; i < b.size(); i++)
  {
      eigenval(i,i)=tmpeig[b.at(i)];
      for(j=0; j<dim; j++)
      {
         eigenvec(j,i) = eigenvectors[j][b.at(i)];
      }
  }


}


//Wrapper for the inverse function
MatrixXd ActCovariance::matInverse(MatrixXd dir)

{
#ifdef USEEIGENINVERSE

    return dir.inverse();

#else

  int dim;
  dim = dir.cols();
  MatrixXd minv(dim,dim);
  matrix <double> mat(dim,dim);  // for test we create & invert this matrix

  double val;
  bool ok;
  int i,j;

  for (i=0; i < dim; i++)  // define random values for initial matrix
  {
    for (j=0; j< dim; j++)
      {
        mat.setvalue(i,j, dir(i,j));
      }
   }
   mat.invert();

   for (i=0; i < dim; i++)  // define random values for initial matrix
   {
     for (j=0; j< dim; j++)
       {
       mat.getvalue(i, j, val, ok);
       minv(i,j) = val;
      }
    }

return minv;
#endif
}

/***********************************************************************
Hypergeometric distribution
***********************************************************************/
int32_t ActCovariance::Hypergeometric (int32_t n, int32_t m, int32_t N)
{
   /*
   This function generates a random variate with the hypergeometric
   distribution. This is the distribution you get when drawing balls without
   replacement from an urn with two colors. n is the number of balls you take,
   m is the number of red balls in the urn, N is the total number of balls in
   the urn, and the return value is the number of red balls you get.

   This function uses inversion by chop-down search from the mode when
   parameters are small, and the ratio-of-uniforms method when the former
   method would be too slow or would give overflow.
   */

   int32_t fak, addd;                    // used for undoing transformations
   int32_t x;                            // result

   // symmetry transformations
   fak = 1;
   addd = 0;

   if (m > N/2)
   {
      // invert m
      m = N - m;
      fak = -1;
      addd = n;
   }

   if (n > N/2)
   {
      // invert n
      n = N - n;
      addd += fak * m;
      fak = - fak;
   }

   if (n > m)
   {
      // swap n and m
      x = n;
      n = m;
      m = x;
   }

   // cases with only one possible result end here
   if (n == 0)
       return addd;

   //------------------------------------------------------------------
   //                 choose method
   //------------------------------------------------------------------
   if (N > 680 || n > 70)
   {
      // use ratio-of-uniforms method
      x = HypRatioOfUnifoms (n, m, N);
   }
   else
   {
      // inversion method, using chop-down search from mode
      x = HypInversionMod (n, m, N);
   }
   // undo symmetry transformations
   return x * fak + addd;
}


/***********************************************************************
Subfunctions used by hypergeometric
***********************************************************************/

int32_t ActCovariance::HypInversionMod (int32_t n, int32_t m, int32_t N)
{
   /*
   Subfunction for Hypergeometric distribution. Assumes 0 <= n <= m <= N/2.
   Overflow protection is needed when N > 680 or n > 75.

   Hypergeometric distribution by inversion method, using down-up
   search starting at the mode using the chop-down technique.

   This method is faster than the rejection method when the variance is low.
   */

   // Sampling
   int32_t       I;                    // Loop counter
   int32_t       L = N - m - n;        // Parameter
   double        modef;                // mode, float
   double        Mp, np;               // m + 1, n + 1
   double        p;                    // temporary
   double        U;                    // uniform random
   double        c, d;                 // factors in iteration
   double        divisor;              // divisor, eliminated by scaling
   double        k1, k2;               // float version of loop counter
   double        L1 = L;               // float version of L
   int32_t  hyp_n_last, hyp_m_last, hyp_N_last;            // Last values of parameters
   int32_t  hyp_mode, hyp_mp;                              // Mode, mode+1
   int32_t  hyp_bound;                                     // Safety upper bound
   double hyp_fm;
   MotherOfAll urng(((double ) rand()) / RAND_MAX);
   Random::Set(urng);
   Uniform Un;
   U = Un.Next();                    // uniform random number to be converted

   // Last values of hypergeometric parameters
   hyp_n_last = hyp_m_last = hyp_N_last = -1;

   Mp = (double)(m + 1);
   np = (double)(n + 1);

   if (N != hyp_N_last || m != hyp_m_last || n != hyp_n_last)
   {
        // set-up when parameters have changed
        hyp_N_last = N;
        hyp_m_last = m;
        hyp_n_last = n;

        p  = Mp / (N + 2.);
        modef = np * p;                       // mode, real
        hyp_mode = (int)modef;            // mode, integer
        if (hyp_mode == modef && p == 0.5)
        {
         hyp_mp = hyp_mode--;
        }
        else
        {
         hyp_mp = hyp_mode + 1;
        }
        // mode probability, using log factorial function
        // (may read directly from fac_table if N < FAK_LEN)
        hyp_fm = exp(LnFac(N-m) - LnFac(L+hyp_mode) - LnFac(n-hyp_mode)
         + LnFac(m)   - LnFac(m-hyp_mode) - LnFac(hyp_mode)
         - LnFac(N)   + LnFac(N-n)      + LnFac(n)        );

        // safety bound - guarantees at least 17 significant decimal digits
        // bound = min(n, (int)(modef + k*c'))
        hyp_bound = (int)(modef + 11. * sqrt(modef * (1.-p) * (1.-n/(double)N)+1.));
        if (hyp_bound > n) hyp_bound = n;
   }

   // loop until accepted
   while(1) {

      // start chop-down search at mode
      if ((U -= hyp_fm) <= 0.)
          return(hyp_mode);

      c = d = hyp_fm;

      // alternating down- and upward search from the mode
      k1 = hyp_mp - 1;  k2 = hyp_mode + 1;
      for (I = 1; I <= hyp_mode; I++, k1--, k2++)
      {
         // Downward search from k1 = hyp_mp - 1
         divisor = (np - k1)*(Mp - k1);
         // Instead of dividing c with divisor, we multiply U and d because
         // multiplication is faster. This will give overflow if N > 800
         U *= divisor;  d *= divisor;
         c *= k1 * (L1 + k1);
         if ((U -= c) <= 0.)
             return(hyp_mp - I - 1); // = k1 - 1

         // Upward search from k2 = hyp_mode + 1
         divisor = k2 * (L1 + k2);
         // re-scale parameters to avoid time-consuming division
         U *= divisor;  c *= divisor;
         d *= (np - k2) * (Mp - k2);
         if ((U -= d) <= 0.)
             return(hyp_mode + I);  // = k2
         // Values of n > 75 or N > 680 may give overflow if you leave out this..
         // overflow protection
         // if (U > 1.E100) {U *= 1.E-100; c *= 1.E-100; d *= 1.E-100;}
      }

      // Upward search from k2 = 2*mode + 1 to bound
      for (k2 = I = hyp_mp + hyp_mode; I <= hyp_bound; I++, k2++)
      {
         divisor = k2 * (L1 + k2);
         U *= divisor;
         d *= (np - k2) * (Mp - k2);
         if ((U -= d) <= 0.)  return(I);
         // more overflow protection
         // if (U > 1.E100) {U *= 1.E-100; d *= 1.E-100;}
      }
   }
}


int32_t ActCovariance::HypRatioOfUnifoms (int32_t n, int32_t m, int32_t N)
{
   /*
   Subfunction for Hypergeometric distribution using the ratio-of-uniforms
   rejection method.

   This code is valid for 0 < n <= m <= N/2.

   The computation time hardly depends on the parameters, except that it matters
   a lot whether parameters are within the range where the LnFac function is
   tabulated.

   Reference: E. Stadlober: "The ratio of uniforms approach for generating
   discrete random variates". Journal of Computational and Applied Mathematics,
   vol. 31, no. 1, 1990, pp. 181-189.
   */
   int32_t L;                          // N-m-n
   int32_t mode;                       // mode
   int32_t k;                          // integer sample
   double x;                           // real sample
   double rNN;                         // 1/(N*(N+2))
   double my;                          // mean
   double var;                         // variance
   double u;                           // uniform random
   double lf;                          // ln(f(x))
   int32_t  hyp_n_last, hyp_m_last, hyp_N_last;            // Last values of parameters
   int32_t  hyp_bound;                                     // Safety upper bound
   double hyp_a;                                           // hat center
   double hyp_h;                                           // hat width
   double hyp_fm;                                          // Value at mode
   MotherOfAll urng(((double ) rand()) / RAND_MAX);
   Random::Set(urng);
   // Last values of hypergeometric parameters
   hyp_n_last = hyp_m_last = hyp_N_last = -1;
   Uniform Un;
   u = Un.Next();

   L = N - m - n;
   if (hyp_N_last != N || hyp_m_last != m || hyp_n_last != n)
   {
      hyp_N_last = N;  hyp_m_last = m;  hyp_n_last = n;         // Set-up
      rNN = 1. / ((double)N*(N+2));                             // make two divisions in one
      my = (double)n * m * rNN * (N+2);                         // mean = n*m/N
      mode = (int)(double(n+1) * double(m+1) * rNN * N);    // mode = floor((n+1)*(m+1)/(N+2))
      var = (double)n * m * (N-m) * (N-n) / ((double)N*N*(N-1));// variance
      hyp_h = sqrt(SHAT1 * (var+0.5)) + SHAT2 ;                  // hat width
      hyp_a = my + 0.5;                                         // hat center
      hyp_fm = fc_lnpk(mode, L, m, n);                          // maximum
      hyp_bound = (int)(hyp_a + 4.0 * hyp_h);               // safety-bound
      if (hyp_bound > n) hyp_bound = n;
   }

   while(1)
   {
                                                // uniform random number
      if (u == 0)
          continue;                      // avoid division by 0

      x = hyp_a + hyp_h * (Un.Next()-0.5) / u;    // generate hat distribution

      if (x < 0. || x > 2E9)
          continue;           // reject, avoid overflow

      k = (int)x;

      if (k > hyp_bound)
          continue;               // reject if outside range

      lf = hyp_fm - fc_lnpk(k,L,m,n);            // ln(f(k))

      if (u * (4.0 - u) - 3.0 <= lf)
          break;      // lower squeeze accept

      if (u * (u-lf) > 1.0)
          continue;            // upper squeeze reject

      if (2.0 * log(u) <= lf)
          break;             // final acceptance
   }

   return k;
}

//***********************************************************************
//Log factorial function
//***********************************************************************/
double ActCovariance::LnFac(int32_t n)
{
   // log factorial function. gives natural logarithm of n!

   // define constants
   static const double                 // coefficients in Stirling approximation
      C0 =  0.918938533204672722,      // ln(sqrt(2*pi))
      C1 =  1./12.,
      C3 = -1./360.;
   // C5 =  1./1260.,                  // use r^5 term if FAK_LEN < 50
   // C7 = -1./1680.;                  // use r^7 term if FAK_LEN < 20
   // static variables
   static double fac_table[FAK_LEN];   // table of ln(n!):
   static int initialized = 0;         // remember if fac_table has been initialized

   if (n < FAK_LEN)
   {
      if (!initialized)
      {              // first time. Must initialize table
         // make table of ln(n!)
         double sum = fac_table[0] = 0.;
         for (int i=1; i<FAK_LEN; i++)
         {
            sum += log(double(i));
            fac_table[i] = sum;
         }
         initialized = 1;
      }
      return fac_table[n];
   }
   // not found in table. use Stirling approximation
   double  n1, r;
   n1 = n;  r  = 1. / n1;
   return (n1 + 0.5)*log(n1) - n1 + C0 + r*(C1 + r*r*C3);
}

double ActCovariance::fc_lnpk(int32_t k, int32_t L, int32_t m, int32_t n)
{
   // subfunction used by hypergeometric and Fisher's noncentral hypergeometric distribution
   return(LnFac(k) + LnFac(m - k) + LnFac(n - k) + LnFac(L + k));
}
