/*
 Further information about the European Union Public Licence - EUPL v.1.1 can
 also be found on the world wide web at http://ec.europa.eu/idabc/eupl
 */

/*
 ------ Copyright (C) 2010 STA Steering Board (space.trajectory.analysis AT gmail.com) ----
 ------------------ Author: Ricardo Noriega  ----------------------------------------------
 ------------------ email: ricardonor@gmail.com  ------------------------------------------

 */


#include "coverageanalysis.h"

#include <QDebug>


#ifdef Q_WS_MAC
#include <CoreFoundation/CFBundle.h>
#endif


CoverageAnalysis::CoverageAnalysis()
{
}
CoverageAnalysis::CoverageAnalysis(PropagatedScenario * propagatedScenario,
                                   int indexSC,
                                   int indexGS,
                                   int indexMA):m_propagatedScenario(propagatedScenario), m_indexSC(indexSC), m_indexGS(indexGS), m_indexMA(indexMA)
{

    //Constructor with the propagated scenario
}


void CoverageAnalysis::reportAER()
{

    QString staResourcesPath;

#if defined(Q_WS_MAC)
    // On the Mac, load resources from the app bundle
    CFURLRef appUrlRef = CFBundleCopyBundleURL(CFBundleGetMainBundle());
    CFStringRef macPath = CFURLCopyFileSystemPath(appUrlRef,
                                                  kCFURLPOSIXPathStyle);
    const char *pathPtr = CFStringGetCStringPtr(macPath,
                                                CFStringGetSystemEncoding());
    staResourcesPath = QString(pathPtr) + "/Contents/Resources/STAResources";
    CFRelease(appUrlRef);
    CFRelease(macPath);
#elif defined(Q_WS_WIN)
    // On Windows, we have two choices for the resource location. If this
    // is a development version, load them from sta-data. For an installed
    // version of the software, we'll load resources from the app data directory.
    if (QDir("./sta-data").exists())
    {
        staResourcesPath = "./sta-data";
    }
    else if (QDir("../sta-data").exists())
    {
        // QtCreator builds in the debug/ or release/ directory
        staResourcesPath = "../sta-data";
    }
    else
    {
        // staResourcesPath = QDir::home().filePath("STA");
        staResourcesPath = "./resources";
    }
#else
    // Not Windows or Mac
    staResourcesPath = ResourcesPath;
#endif


    QFile reportCov1(staResourcesPath + "/" + "reportCov1.txt");
    reportCov1.open(QIODevice::WriteOnly|QIODevice::ReadWrite);
    QTextStream streamReportCov1(&reportCov1);
    streamReportCov1.setRealNumberPrecision(16);

    streamReportCov1<<"MJD"<<"\t"<<"Range [Km]"<<"\t"<<"Elevation [deg]"<<"\t"<<"Azimuth [deg]"<<endl;


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
