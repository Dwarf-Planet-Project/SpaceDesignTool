#include "EclipseDuration.h"

#include <math.h>

//to Read and write to file
#include <QFile>
#include <QTextStream>
#include <QDir>
#include <QProcess>

#include <QDebug>

#include <Astro-Core/date.h>

using namespace sta;

EclipseDuration::EclipseDuration()
{
}

bool EclipseDuration::IsSpacecraftInUmbra(Vector3d StarCoordinates,
                                          Vector3d PlanetCoordinates,
                                          Vector3d SpacecraftCoordinates)
{
//    //Planet-Star Information has to be gathered from Stabody.h
//    double m_PlanetMeanDiameter = 2;
//    double m_StarMeanDiameter = 5;

    double PlanetStarDistance = (StarCoordinates - PlanetCoordinates).norm();
    double UmbralApexToPlanetCenterDistance = (m_PlanetMeanDiameter*PlanetStarDistance)
                                              /(m_StarMeanDiameter-m_PlanetMeanDiameter);
    double UmbralSubtendedAngle = asin(m_PlanetMeanDiameter
                                       /(2*UmbralApexToPlanetCenterDistance));

    Vector3d PlanetSpacecraftVector
            = SpacecraftCoordinates - PlanetCoordinates;
    Vector3d UnitPlanetStarVector
            = (StarCoordinates - PlanetCoordinates).normalized();   // r_s

    // Check if Spacecraft is on the Star Side of the planet
    if(PlanetSpacecraftVector.dot(UnitPlanetStarVector)>=0)
    {
        return false;
    }

    Vector3d SCProjectionOnPlanetStarCenterPlaneVector
            = PlanetSpacecraftVector.dot(UnitPlanetStarVector) * UnitPlanetStarVector;
    Vector3d PlanetStarCenterPlaneToSCVector
            = PlanetSpacecraftVector - SCProjectionOnPlanetStarCenterPlaneVector;

    double PlanetStarPlaneToUmbraTerminatorDistance
            = (UmbralApexToPlanetCenterDistance - SCProjectionOnPlanetStarCenterPlaneVector.norm())
              * atan(UmbralSubtendedAngle);

    if (PlanetStarCenterPlaneToSCVector.norm() <= PlanetStarPlaneToUmbraTerminatorDistance)
    {
        return true;
    }else
        return false;
}

bool EclipseDuration::IsSpacecraftInPenumbra(Vector3d StarCoordinates,
                                             Vector3d PlanetCoordinates,
                                             Vector3d SpacecraftCoordinates)
{
//    //Planet-Star Information has to be gathered from Stabody.h
//    double m_PlanetMeanDiameter = 2 * m_Planet;
//    double m_StarMeanDiameter = 5;

    double PlanetStarDistance = (StarCoordinates - PlanetCoordinates).norm();
    double PenumbralApexToPlanetCenterDistance
            = (m_PlanetMeanDiameter*PlanetStarDistance)/(m_StarMeanDiameter+m_PlanetMeanDiameter);
    double PenumbralSubtendedAngle
            = asin(m_PlanetMeanDiameter/(2*PenumbralApexToPlanetCenterDistance));

    Vector3d PlanetSpacecraftVector
            = SpacecraftCoordinates - PlanetCoordinates;
    Vector3d UnitPlanetStarVector
            = (StarCoordinates - PlanetCoordinates).normalized();   // r_s

    // Check if Spacecraft is on the Star Side of the planet
    if(PlanetSpacecraftVector.dot(UnitPlanetStarVector)>=0)
    {
        return false;
    }

    Vector3d SCProjectionOnPlanetStarCenterPlaneVector
            = PlanetSpacecraftVector.dot(UnitPlanetStarVector) * UnitPlanetStarVector;
    Vector3d PlanetStarCenterPlaneToSCVector
            = PlanetSpacecraftVector - SCProjectionOnPlanetStarCenterPlaneVector;

    double PlanetStarPlaneToPenumbraTerminatorDistance
            = (PenumbralApexToPlanetCenterDistance + SCProjectionOnPlanetStarCenterPlaneVector.norm())
              * atan(PenumbralSubtendedAngle);

    if (PlanetStarCenterPlaneToSCVector.norm() <= PlanetStarPlaneToPenumbraTerminatorDistance)
    {
        if (EclipseDuration::IsSpacecraftInUmbra(StarCoordinates,
                                                 PlanetCoordinates,
                                                 SpacecraftCoordinates))
            return false;
        else
            return true;
    }else
        return false;
}

void EclipseDuration::StarLightTimeFunction(QList<double> &sampleTimes,
                                            QList<sta::StateVector> &PlanetCoordinates,
                                            QList<sta::StateVector> &SCCoordinates,
                                            StaBody* Planet,
                                            StaBody* Star)
{
    m_PlanetMeanDiameter = 2 * Planet->meanRadius();
    m_StarMeanDiameter = 2 * Star->meanRadius();

    QString path = QString("data/EclipseStarLight.stad");

    QFile EclipseStarLight(path);
    EclipseStarLight.open(QIODevice::ReadWrite);
//       qDebug()<<"EclipseStarLight.fileName()"<<EclipseStarLight.fileName();
//       qDebug()<<"EclipseStarLight.isOpen()"<<EclipseStarLight.isOpen();
    QTextStream EclipseStarLightStream(&EclipseStarLight);

    int i;
    for (i=0;i<sampleTimes.size();i++)
    {
        //sample Time
        EclipseStarLightStream<<sta::JdToCalendar
                (sta::MjdToJd(sampleTimes.at(i))).toString(Qt::ISODate);
        EclipseStarLightStream<<"          ";

        double tempEclipse = 0.0;

        //check if it is in Penumbra or Umbra
        if (EclipseDuration::IsSpacecraftInUmbra
            (Star->stateVector(sampleTimes.at(i),
                               Planet,
                               sta::COORDSYS_BODYFIXED).position,
             PlanetCoordinates.at(i).position,
             SCCoordinates.at(i).position))
            tempEclipse = 0.0;
        else
        {
            if (EclipseDuration::IsSpacecraftInPenumbra
                (Star->stateVector(sampleTimes.at(i),
                                   Planet,
                                   sta::COORDSYS_BODYFIXED).position,
                 PlanetCoordinates.at(i).position,
                 SCCoordinates.at(i).position))
                tempEclipse = 0.5;
            else
                tempEclipse = 1.0;
        }

        EclipseStarLightStream<<tempEclipse;
        EclipseStarLightStream<<"        ";
    }

    EclipseStarLightStream<<endl;

    EclipseStarLight.close();
}

void EclipseDuration::StarLightTimeFunction(QList<double> &sampleTimes,
                                            QList<sta::StateVector> &SCCoordinates,
                                            StaBody* Planet,
                                            StaBody* Star)
{
    m_PlanetMeanDiameter = 2 * Planet->meanRadius();
    m_StarMeanDiameter = 2 * Star->meanRadius();

    QString path = QString("data/EclipseStarLight.stad");

    QFile EclipseStarLight(path);
    EclipseStarLight.open(QIODevice::ReadWrite);
//       qDebug()<<"EclipseStarLight.fileName()"<<EclipseStarLight.fileName();
//       qDebug()<<"EclipseStarLight.isOpen()"<<EclipseStarLight.isOpen();
    QTextStream EclipseStarLightStream(&EclipseStarLight);

    Vector3d  PlanetCoordinates;
    PlanetCoordinates.setZero(3);

    int i;
    for (i=0;i<sampleTimes.size();i++)
    {
        //sample Time
        EclipseStarLightStream<<sta::JdToCalendar
                (sta::MjdToJd(sampleTimes.at(i))).toString(Qt::ISODate);
        EclipseStarLightStream<<"          ";

        double tempEclipse = 0.0;

        //check if it is in Penumbra or Umbra
        if (EclipseDuration::IsSpacecraftInUmbra
            (Star->stateVector(sampleTimes.at(i),
                               Planet,
                               sta::COORDSYS_BODYFIXED).position,
             PlanetCoordinates,
             SCCoordinates.at(i).position))
            tempEclipse = 0.0;
        else
        {
            if (EclipseDuration::IsSpacecraftInPenumbra
                (Star->stateVector(sampleTimes.at(i),
                                   Planet,
                                   sta::COORDSYS_BODYFIXED).position,
                 PlanetCoordinates,
                 SCCoordinates.at(i).position))
                tempEclipse = 0.5;
            else
                tempEclipse = 1.0;
        }

        EclipseStarLightStream<<tempEclipse;
        EclipseStarLightStream<<"        ";
    }

    EclipseStarLightStream<<endl;

    EclipseStarLight.close();

}
