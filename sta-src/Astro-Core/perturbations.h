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
 ------------------ Author: Tiziana Sabatini  -------------------------------------------------
 ------------------ E-mail: (tiziana.sabatini@yahoo.it) ----------------------------
 */

#ifndef PERTURBATIONS_H
#define PERTURBATIONS_H

#include "statevector.h"
#include "Scenario/scenario.h"
#include "Astro-Core/stabody.h"
#include <Eigen/Core>

USING_PART_OF_NAMESPACE_EIGEN

/**
 * Abstract base class of all perturbations.
 */
class Perturbations
{
public:
    Perturbations();
    virtual ~Perturbations();

    /**
     * Abstract function calculating perturbing acceleration knowing the position of the body.
     */
    virtual Vector3d calculateAcceleration(sta::StateVector state, double mjd, double dt);
};

/**
 * Class representing planet's gravity field perturbation.
 */
class GravityPerturbations : public Perturbations
{
public:
    GravityPerturbations(const StaBody* centralBody, const ScenarioGravityModel*);
    ~GravityPerturbations();

    /**
     * Return the central celestial body .
     */
    const StaBody* body() const {return m_body;}

    /**
     * Return the number of zonals.
     */
    int zonalCount() const { return m_zonalCount; }

    /**
     * Return the number of tesserals.
     */
    int tesseralCount() const { return m_tesseralCount; }

    /**
     * Return the perturbing acceleration due to the planet's gravity field.
     */
    virtual Vector3d calculateAcceleration(sta::StateVector, double, double);


private:

    /**
     * Function to load gravity constants from the gravity model file;
     * Only the zonal (or tesseral) harmonics whose index is below the maximum zonal (or tesseral)
     * index will be loaded.
     */
    void loadGravityConstants();

    const StaBody* m_body;
    QString m_modelName;
    int m_zonalCount;
    int m_tesseralCount;
    VectorXd J;
    MatrixXd JJ;
    MatrixXd gamma;

};

/**
 * Function to calculate the factorial, used in the conversion from normalized to canonic harmonic coefficients.
 */
double factorial(int);

/**
 * Function to calculate the double factorial, used in the calculation of Legendre polynomials.
 */
double doublefactorial(int);


/**
 * Class representing Atmospheric Drag perturbation.
 */
class AtmosphericDragPerturbations : public Perturbations
{
public:
    AtmosphericDragPerturbations(const QString& atmosphereModel);
    virtual ~AtmosphericDragPerturbations();

    /**
     * Return the string containing the name of the atmospheric model.
     */
    QString atmosphericModel() const {return m_atmosphericModel;}
    /**
     * Return the central celestial body .
     */
    const StaBody* body() const {return m_body;}
    /**
     * Return the body surface area.
     */
    double surface() const {return m_surface;}
    /**
     * Return the mass of the vehicle.
     */
    double mass() const {return m_mass;}
    /**
     * Return the string of the name of the file containig the cd coefficients.
     */
    QString cdCoefficients() const {return m_cdCoefficients;}
    /**
     * Return the perturbing acceleration due to the atmospheric drag.
     */
    virtual Vector3d calculateAcceleration(sta::StateVector, double, double);

private:
    QString m_atmosphericModel;
    const StaBody* m_body;
    double m_surface;
    double m_mass;
    QString m_cdCoefficients;
};

/**
 * Class representing Solar Pressure perturbation.
 */
class SolarPressurePerturbations : public Perturbations
{
public:
    SolarPressurePerturbations(StaBody* centralBody,
                               double reflectivity,
                               double albedo,
                               double ir,
                               double mass,
                               double surfaceArea);
    virtual ~SolarPressurePerturbations();
    /**
     * Return the central celestial body .
     */
    const StaBody* body() const {return m_body;}
    /**
     * Return the reflectivity of the vehicle.
     */
    double reflectivity() const {return m_reflectivity;}
    /**
     * Return true if the albedo is taken into account in the simulation
     */
    bool albedo() const {return m_albedo;}
    /**
     * Return true if the infra-red radiation is taken into account in the simulation
     */
    bool ir() const {return m_ir;}
    /**
     * Return the body surface area.
     */
    double surface() const {return m_surface;}
    /**
     * Return the mass of the vehicle.
     */
    double mass() const {return m_mass;}
    /**
     * Return the perturbing acceleration due to the solar pressure.
     */
    virtual Vector3d calculateAcceleration(sta::StateVector, double, double);

private:
    const StaBody* m_body;
    double m_reflectivity;
    bool m_albedo;
    bool m_ir;
    double m_surface;
    double m_mass;
};

/**
 * Return the albedo reflectivity coefficient given the StaBody.
 */
double albedoReflectivityCoefficient(const StaBody*);

/**
 * Return the infra-red radiation flux given the StaBody.
 */
double irRadiationFlux(const StaBody*);


/**
 * Class representing External Bodies perturbation.
 */
class ExternalBodyPerturbations : public Perturbations
{
public:
    ExternalBodyPerturbations(const StaBody* centralBody,
                              const QList<const StaBody*>& bodies);
    virtual ~ExternalBodyPerturbations();

    /**
     * Return the central celestial body .
     */
    const StaBody* body() const {return m_body;}
    /**
     * Return the list of perturbing celestial bodies.
     */
    QList<const StaBody*> perturbingBodyList() const {return m_perturbingBodyList;}
    /**
     * Return the perturbing acceleration due to third bodies.
     */
    virtual Vector3d calculateAcceleration(sta::StateVector, double, double);

private:
    const StaBody* m_body;
    QList<const StaBody*> m_perturbingBodyList;
};

/**
 * Class representing Space Debris collisions perturbation.
 */
class DebrisPerturbations : public Perturbations
{
public:
    DebrisPerturbations(const StaBody* centralBody,
                        double mass,
                        double surfaceArea);
    virtual ~DebrisPerturbations();

    /**
     * Return the central celestial body .
     */
    const StaBody* body() const {return m_body;}
    /**
     * Return the mass of the vehicle.
     */
    double mass() const {return m_mass;}
    /**
     * Return the surface of the vehicle.
     */
    double surface() const {return m_surface;}
    /**
     * Return the number of debris impacts already counted.
     */
    VectorXd counterDebris() const {return m_counterDebris;}
    /**
     * Return the number of meteoroids impacts already counted.
     */
    VectorXd counterMeteoroids() const {return m_counterMeteoroids;}
    /**
     * Set the number of impacts for a given diameter of debris.
     */
    void setCounterDebris(int i, double counterd)  {m_counterDebris(i) = counterd;}
    /**
     * Set the number of impacts for a given diameter of meteoroids.
     */
    void setCounterMeteoroids(int i, double counterm)  {m_counterMeteoroids(i) = counterm;}
    /**
     * Return the perturbing acceleration due to debris collisions perturbation.
     */
    virtual Vector3d calculateAcceleration(sta::StateVector, double, double);
    /**
     * Load from a file the statistic data about the debris collisions.
     */
    void loadStatistics(QString, VectorXd& , VectorXd&, VectorXd&, VectorXd&, int&);


private:
    const StaBody* m_body;
    double m_mass;
    double m_surface;
    double m_time;
    VectorXd m_counterDebris;
    VectorXd m_counterMeteoroids;
};

/**
 * Generate a random number between inf and sup.
 */
double randomNumber(double inf, double sup);

#endif // PERTURBATIONS_H
