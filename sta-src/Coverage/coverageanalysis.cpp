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

       streamReportCov1<<"MJD"<<"       "<<"Range[Km]"<<"    "<<"Elevation[deg]"<<"   "<<endl;


               for(int i=0; i<m_propagatedScenario->spaceObjects().at(m_indexSC)->mission().at(m_indexMA)->trajectorySampleCount(); i++)
               {
                    double mjd=m_propagatedScenario->spaceObjects().at(m_indexSC)->mission().at(m_indexMA)->trajectorySampleTime(i);
                    double elevationAngle=m_propagatedScenario->groundObjects().at(m_indexGS)->elevationAngle(m_propagatedScenario->spaceObjects().at(m_indexSC), mjd);
                    elevationAngle=elevationAngle*RAD2DEG;
                    double range=m_propagatedScenario->groundObjects().at(m_indexGS)->getRange(m_propagatedScenario->spaceObjects().at(m_indexSC), mjd);
                    if(elevationAngle>=5.0)
                        streamReportCov1<<mjd<<"   "<<range<<"   "<<elevationAngle<<endl;
               }
        reportCov1.close();
}
