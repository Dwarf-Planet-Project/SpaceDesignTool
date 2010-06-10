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
 ------------------ Author: Ozgun YILMAZ    ------------------------------------------
 ------------------ email:ozgunus@yahoo.com ------------------------------------------
 */

#include "EclipseDuration.h"

#include <math.h>

//to Read and write to file
#include <QFile>
#include <QTextStream>
#include <QDir>
#include <QProcess>

#include <QDebug>

//erase -#include "Astro-Core/date.h"- after analysis module integration
#include "Astro-Core/calendarTOjulian.h"

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
    EclipseStarLightStream.setRealNumberPrecision(16);

    int i;
    for (i=0;i<sampleTimes.size();i++)
    {
        //sample Time
//        EclipseStarLightStream<<sta::JdToCalendar
//                (sta::MjdToJd(sampleTimes.at(i))).toString(Qt::ISODate);
        EclipseStarLightStream<<sampleTimes.at(i);
        qDebug()<<"SAMPLE"<<sampleTimes.at(i);
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
    EclipseStarLightStream.setRealNumberPrecision(16);

    Vector3d  PlanetCoordinates;
    PlanetCoordinates.setZero(3);

    int i;
    for (i=0;i<sampleTimes.size();i++)
    {
        //sample Time
//        EclipseStarLightStream<<sta::JdToCalendar
//                (sta::MjdToJd(sampleTimes.at(i))).toString(Qt::ISODate);
        EclipseStarLightStream<<sampleTimes.at(i);
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
    CreateEclipseDetailsFile();

}

void EclipseDuration::CreateEclipseDetailsFile()
{
    //open the eclipse time function to read
    //reach to the Eclipse information file------------
    QString path = QString("data/EclipseStarLight.stad");

    QFile EclipseStarLight(path);
    EclipseStarLight.open(QIODevice::ReadOnly);
//       qDebug()<<"EclipseStarLight.fileName()"<<EclipseStarLight.fileName();
//       qDebug()<<"EclipseStarLight.isOpen()"<<EclipseStarLight.isOpen();
    QTextStream EclipseStarLightStream(&EclipseStarLight);
//    EclipseStarLightStream.setRealNumberPrecision(16);

    //open the file you want to put the results--------
    QString path2 = QString
                   ("data/EclipseDetailedReport.stad");

    QFile DetailedReport(path2);
    DetailedReport.open(QIODevice::ReadWrite);
//           qDebug()<<"GeneratedPowerTime.fileName()"<<DetailedReport.fileName();
//           qDebug()<<"GeneratedPowerTime.isOpen()"<<DetailedReport.isOpen();
    QTextStream DetailedReportStream(&DetailedReport);
    DetailedReportStream.setRealNumberPrecision(16);

    //write a description to the title of the report
    DetailedReportStream << "State" << "\t\t";
    DetailedReportStream << "Start(mjd)" << "\t\t";
    DetailedReportStream << "End(mjd)" << "\t\t";
    DetailedReportStream << "Duration(s)" << "\t";
    DetailedReportStream << "#ofSteps"<<"\t";

    double State;
    double missionStartMjd = 0.0;
    double missionEndMjd = 0.0;
    double tempState;
    double tempMissionEndMjd = 0.0;
    int    numberOfStepsInState = 0;

    numberOfStepsInState = 1;
    EclipseStarLightStream >> missionStartMjd;
    EclipseStarLightStream >> State;

    while (!EclipseStarLightStream.atEnd())
    {
        while (State <= 0.5 )//Penumbra + Umbra
        {
            EclipseStarLightStream >> tempMissionEndMjd;
            EclipseStarLightStream >> tempState;
            numberOfStepsInState++; //increase the number of steps

            if (!EclipseStarLightStream.atEnd())
            {
                missionEndMjd = tempMissionEndMjd;
                State = tempState;
            }
            else
            {
                State = 1.0;
            }

            //if the state is changed or the file is ended
            if (State > 0.5)
            {
                //write the detailed report
                DetailedReportStream << "Eclipse" << "\t\t";
                DetailedReportStream << missionStartMjd << "\t";
                DetailedReportStream << missionEndMjd << "\t";
                //calculate duration from ANas module but it has
                //to be replied after her commit
                DetailedReportStream << MjdToFromEpoch(missionStartMjd,
                                                       missionEndMjd,
                                                       "Seconds") << "\t";
                DetailedReportStream << (numberOfStepsInState -1) << "\t";

                missionStartMjd = missionEndMjd;
                numberOfStepsInState = 1;
                break;
            }
        }

        while (State > 0.5) //Daylight
        {
            EclipseStarLightStream >> tempMissionEndMjd;
            EclipseStarLightStream >> tempState;
            numberOfStepsInState++; //increase the number of steps

            if (!EclipseStarLightStream.atEnd())
            {
                missionEndMjd = tempMissionEndMjd;
                State = tempState;
            }
            else
            {
                State = 0.0;
            }

//            qDebug()<<"******missionStartMjd"<<missionStartMjd;
//            qDebug()<<"missionEndMjd"<<missionEndMjd;
//            qDebug()<<"numberOfStepsInState"<<numberOfStepsInState;

            if (State <= 0.5)
            {
                //write the detailed report
                DetailedReportStream << "Daylight" << "\t";
                DetailedReportStream << missionStartMjd << "\t";
                DetailedReportStream << missionEndMjd << "\t";
                //calculate duration from ANas module but it has
                //to be replied after her commit
                DetailedReportStream << MjdToFromEpoch(missionStartMjd,
                                                       missionEndMjd,
                                                       "Seconds") << "\t";
                DetailedReportStream << (numberOfStepsInState -1) << "\t";

                missionStartMjd = missionEndMjd;
                numberOfStepsInState = 1;

//                qDebug()<<"missionStartMjd"<<missionStartMjd;
//                qDebug()<<"missionEndMjd"<<missionEndMjd;
//                qDebug()<<"numberOfStepsInState"<<numberOfStepsInState;
                break;
            }
        }
    }

//    qDebug()<<"missionStartMjd"<<missionStartMjd;
//    qDebug()<<"missionEndMjd"<<missionEndMjd;
//    qDebug()<<"numberOfStepsInState"<<numberOfStepsInState;
    
    DetailedReportStream << endl;

    DetailedReport.close();
    EclipseStarLight.close();
}


//erase the following after analysis module inegration

double EclipseDuration::MjdToFromEpoch(double StartEpoch,
                                       double mjd,
                                       QString Units)
{
    double ElapsedTime=mjd-StartEpoch;

      if(Units=="Seconds")
      {
        double SecondJD=calendarTOjulian(1858,11,17,0,0,1);//convert 1second into MJD
        double Second=sta::JdToMjd(SecondJD);
        double ElapsedTimeSeconds=ElapsedTime/Second;
     return(ElapsedTimeSeconds);
     }
      if(Units=="Minutes")
      {
         double MinuteJD=calendarTOjulian(1858,11,17,0,1,0); //convert 1minute into MJD
         double MinuteMJD=sta::JdToMjd(MinuteJD);
         double ElapsedTimeMinutes=ElapsedTime/MinuteMJD;
    return(ElapsedTimeMinutes);
  }
      if(Units=="Hours")
      {
          double HourJD=calendarTOjulian(1858,11,17,1,0,0); //convert 1hour into MJD
          double HourMJD=sta::JdToMjd(HourJD);
          double ElapsedTimeHours=ElapsedTime/HourMJD;
    return(ElapsedTimeHours);
      }
      if(Units=="Days")
      {
          double DayJD=calendarTOjulian(1858,11,18,0,0,0); //convert 1day into MJD
          double DayMJD=sta::JdToMjd(DayJD);
          double ElapsedTimeDays=ElapsedTime/DayMJD;
    return(ElapsedTimeDays);
      }
      return 0.0;
}
