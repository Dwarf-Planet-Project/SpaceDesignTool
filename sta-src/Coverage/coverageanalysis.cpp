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
 ------------------ Author: Ricardo Noriega  ----------------------------------------------
 ------------------ email: ricardonor@gmail.com  ------------------------------------------

 */

#include "coverageanalysis.h"



CoverageAnalysis::CoverageAnalysis()
{
}
CoverageAnalysis::CoverageAnalysis(PropagatedScenario * propagatedScenario, int indexSC, int indexGS, int indexMA):m_propagatedScenario(propagatedScenario), m_indexSC(indexSC), m_indexGS(indexGS), m_indexMA(indexMA)
{

//Constructor with the propagated scenario
}


void CoverageAnalysis::reportAER(){

       QFile reportCov1("reportCov1.txt");
       reportCov1.open(QIODevice::WriteOnly|QIODevice::ReadWrite);
       QTextStream streamReportCov1(&reportCov1);
       streamReportCov1.setRealNumberPrecision(16);

       streamReportCov1<<"MJD"<<"\t"<<"Range[Km]"<<"\t"<<"Elevation[deg]"<<"\t"<<"Azimuth[deg]"<<endl;


               for(int i=0; i<m_propagatedScenario->spaceObjects().at(m_indexSC)->mission().at(m_indexMA)->trajectorySampleCount(); i++)
               {
                    double mjd=m_propagatedScenario->spaceObjects().at(m_indexSC)->mission().at(m_indexMA)->trajectorySampleTime(i);
                    double elevationAngle=m_propagatedScenario->groundObjects().at(m_indexGS)->elevationAngle(m_propagatedScenario->spaceObjects().at(m_indexSC), mjd);
                    elevationAngle=elevationAngle*RAD2DEG;
                    double range=m_propagatedScenario->groundObjects().at(m_indexGS)->getRange(m_propagatedScenario->spaceObjects().at(m_indexSC), mjd);
                    double azimuth=m_propagatedScenario->groundObjects().at(m_indexGS)->azimuthAngle(m_propagatedScenario->spaceObjects().at(m_indexSC), mjd);
                    if(elevationAngle>=0.0)
                        streamReportCov1<<mjd<<"\t"<<range<<"\t"<<elevationAngle<<"\t"<<azimuth<<endl;
               }
        reportCov1.close();
}
