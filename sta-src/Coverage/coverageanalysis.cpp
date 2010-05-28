#include "coverageanalysis.h"



CoverageAnalysis::CoverageAnalysis()
{
}
CoverageAnalysis::CoverageAnalysis(PropagatedScenario * propagatedScenario):m_propagatedScenario(propagatedScenario)
{

//Constructor with the propagated scenario
}


void CoverageAnalysis::reportAER(){

       QFile reportCov1("reportCov1.txt");
       reportCov1.open(QIODevice::WriteOnly|QIODevice::ReadWrite);
       QTextStream streamReportCov1(&reportCov1);
       streamReportCov1.setRealNumberPrecision(16);

       streamReportCov1<<"MJD"<<"       "<<"Range[Km]"<<"    "<<"Elevation[deg]"<<"   "<<endl;


               for(int i=0; i<m_propagatedScenario->spaceObjects().at(0)->mission().at(0)->trajectorySampleCount(); i++)
               {
                    double mjd=m_propagatedScenario->spaceObjects().at(0)->mission().at(0)->trajectorySampleTime(i);
                    double elevationAngle=m_propagatedScenario->groundObjects().at(0)->elevationAngle(m_propagatedScenario->spaceObjects().at(0), mjd);
                    elevationAngle=elevationAngle*RAD2DEG;
                    double range=m_propagatedScenario->groundObjects().at(0)->getRange(m_propagatedScenario->spaceObjects().at(0), mjd);
                    if(elevationAngle>=5.0)
                        streamReportCov1<<mjd<<"   "<<range<<"   "<<elevationAngle<<endl;
               }
        reportCov1.close();
}
