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

// Authors: Class and Steffen


#ifndef CONSTSTUDY_H
#define CONSTSTUDY_H

#include "Main/propagatedscenario.h"
#include "Constellations/constellationmodule.h"
#include "Constellations/discretization.h"
#include <QMessageBox>
#include <QObject>

//using namespace Eigen;

class SpaceObject;
class GroundObject;
// just a working version, to implement the features, later we use STA internal data
// ---------------------------------------------------------------------------------


struct ConeType
{
    int shape; // 1 circular, 2 rectangular, ...
    double coneangle;
};

class Antenna
{
    public:
    Antenna(double az, double el, double ang, int shape):
            m_elevation(el),
            m_azimuth(az)
    {
        m_cone.coneangle = ang;
        m_cone.shape = shape;
        //m_cone.shape = 1; //for the moment
        // calculate direction of Antenna in VDG coordinate system (velocity dependend)
        m_VDGdirection = Eigen::Vector3d(cos(m_elevation)*cos(m_azimuth),
                                         cos(m_elevation)*sin(m_azimuth),
                                         sin(m_elevation));
    }

    ~Antenna()
    {
    }

    int getShape()
    {
        return m_cone.shape;
    }

    Eigen::Vector3d VDGdirection()
    {
        return m_VDGdirection;
    }
    double azimuth()
    {
        return m_azimuth;
    }
    double elevation()
    {
        return m_elevation;
    }
    /**
      * the antenna needs its current Position (CBCF frame), the position of the Object (CBCF frame) that is visible or not
      * and the rotation matrix (rotation between VDG and CBCF frame) to decide, dependent on the
      * cone shape and size, if the object is visible for it or not.
      * Attention: This function is not supposed to check if there are barriers between antenna
      * and visible object
      **/
    bool hasInCone(Eigen::Vector3d visibleObject, Eigen::Vector3d ownPosition, Eigen::Matrix3d rotation)
    {
        // rotate antenna position into CBCF frame
        Eigen::Vector3d d_c = rotation*m_VDGdirection;
        // choose cone shape
        if (m_cone.shape == 1)
        {
            double alpha = acos((d_c.normalized()).dot((visibleObject - ownPosition).normalized()));
            // attention!!! not correct yet, we still have to consider the two acos cases
            return (alpha < m_cone.coneangle);
        }
        else
        {
            return false;
        }
    }
    private:
    double m_elevation;
    double m_azimuth;
    Eigen::Vector3d m_VDGdirection;
    ConeType m_cone;
};
// ---------------------------------------------------------------------------------

// Constellation Study structure

struct CoverageSample
{
    double curtime;
    QList<DiscretizationPoint> histpoints;
    QList<DiscretizationPoint> curpoints;
    double coveredArea;
    double coveredAreaInPerCent;

    int *firstPoints;
    int highestRow;
    int lowestRow;
    bool anyPointSeen;
};
struct LinkSample
{
    public:
        double curtime;
        QList<SpaceObject*> connection;
};
struct GroundLinkSample
{
    public:
        double curtime;
        QList<GroundObject*> connection;
};
class ConstellationStudySpaceObject
{
    public:
        ConstellationStudySpaceObject(SpaceObject* spaceObject);
        ~ConstellationStudySpaceObject();

        void choose(bool c);

        SpaceObject* m_spaceObject;
        QList<LinkSample> linksamples;
        QList<GroundLinkSample> groundlinksamples;
        QList<CoverageSample> coveragesample;
        QList<Antenna*> observation;
        QList<Antenna*> visibilityR;
        QList<Antenna*> visibilityT;
        QColor asocolor;
        QString asoname;
    private:
        bool m_chosen;

};


class ConstellationStudy
{
    public:
        ConstellationStudy(PropagatedScenario* scenario, bool m_calcCoverage, bool m_calcSOLink, bool m_calcGOLink);
        ~ConstellationStudy();

        QList<ConstellationStudySpaceObject> m_constellationStudySpaceObjectList;
        DiscreteMesh* m_discreteMesh;
    private:

        PropagatedScenario* m_scenario;
        //QList<double> m_samples;
        QList<double> m_sampleTimes;
        //QList<double> m_samplesLinkGroundObjects;
        double m_startTime;
        double m_endTime;
        double m_timeStep;
        const StaBody* m_body;
        bool m_calcCoverage;
        bool m_calcSOLink;
        bool m_calcGOLink;

};

// compare functions
bool linkSampleLessThan(const LinkSample &s1, const LinkSample &s2);
bool groundlinkSampleLessThan(const GroundLinkSample &s1, const GroundLinkSample &s2);
bool coverageSampleLessThan(const CoverageSample &s1, const CoverageSample &s2);


#endif // CONSTSTUDY_H
