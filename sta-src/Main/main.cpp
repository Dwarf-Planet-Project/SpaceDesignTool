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
------------------ Author: Chris Laurel  -------------------------------------------------
  ------------------ E-mail: (claurel@gmail.com) ----------------------------
 Patched by Guillermo as to change the Splash screen of HADEAN, etc.
 Modified by Guillermo October 2009 to include new ARCHEAN v2.0 Splash Screen

 */

 /* Recalling STA file name convention:
        *.stae  Ephemeris
        *.staa Attitude
        *.stag ground track
        *.stam miscelaneous
        *.stap pipes between GUI and modules
        *.stal locations of ground stations or launch pads, etc.
        *.stat time-lines of mission arcs or maneuvers
        *.sta.txt temporary files
        *.stad databases (atmosphere, CD, CL, etc)
 */

#include <QApplication>
#include <QSplashScreen>
#include <QPixmap>
#include <QBitmap>
#include <QDir>
#include <QMessageBox>
#include <QTextStream>

#ifdef Q_WS_MAC
    #include <CoreFoundation/CFBundle.h>
#endif

#include "Astro-Core/stabody.h"
#include "Astro-Core/jplephemeris.h"
#include "mainwindow.h"


// Celestia command line options
static QString configFileName;

static bool ParseCommandLine();


int main(int argc, char *argv[])
{  
    QApplication app(argc, argv);

    QCoreApplication::setOrganizationName(QObject::tr("STA Steering Board"));
    QCoreApplication::setApplicationName("STA");
    QCoreApplication::setOrganizationName("stasb");    // 'stasb' means STA Steering Board
    QCoreApplication::setOrganizationDomain("org");  // Patched by Guillermo
    QCoreApplication::setApplicationVersion("3.0");
    QCoreApplication::setOrganizationDomain("STASB");  // Patched by Guillermo

    QString ApplicationPath = QDir::currentPath ();
    QString ResourcesPath = ApplicationPath + "/sta-data";

    /*
     #ifdef Q_WS_MAC
        QSettings settings("/Users/go/Desktop/org.stasb.STA.plist", QSettings::NativeFormat);
        settings.setValue("CFBundleDisplayName", "Space Trajectory Analysis");
	settings.setValue("CFBundleDocumentTypes", "STAScenarioFile.icns");
	settings.setValue("CFBundleTypeIconFile", "STAScenarioFile.icns");
	settings.setValue("CFBundleTypeRole", "Editor");
	settings.setValue("NSDocumentClass", "STADocument");
        settings.setValue("CFBundleExecutable", "STA");
	settings.setValue("CFBundleGetInfoString", "3.0 RC1, Copyright 2005-2010 STA Steering Board.");
	settings.setValue("CFBundleIconFile", "STAlogo.icns");
        settings.setValue("CFBundleIdentifier", "org.stasb.STA");
	settings.setValue("LSMinimumSystemVersion", "10.5.0");
	//settings.setValue("QuartzGLEnable", true);
     #else
        //Not implemented yet for Windows or Linux
     #endif   
     */


 ParseCommandLine();
    
 QString staResourcesPath;
 QTextStream out (stdout);

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

    if (!QDir::setCurrent(staResourcesPath))
    {
        QMessageBox::warning(NULL, 
                             QObject::tr("STA Resources Not Found"),
                             QObject::tr("STA resources folder wasn't found. This probably means that STA was not properly installed"));
        exit(0);
    }


    // Initialize the astro core
    SolarSystemBodyDictionary::Create();

    QString ephemerisFilename("ephemerides/de406_1800-2100.dat");
    QFile ephemerisFile(ephemerisFilename);
    if (!ephemerisFile.open(QFile::ReadOnly))
    {
        QMessageBox::critical(NULL,
                              QObject::tr("Ephemeris Data Missing"),
                              QObject::tr("Ephemeris data file %1 not found.").arg(ephemerisFilename));

        exit(0);
    }

    sta::JPLEphemeris* ephemeris = sta::JPLEphemeris::load(&ephemerisFile);
    if (!ephemeris)
    {
        QMessageBox::critical(NULL,
                              QObject::tr("Error Reading Ephemeris"),
                              QObject::tr("Ephemeris file %1 is corrupted.").arg(ephemerisFilename));
        exit(0);
    }

    SolarSystemBodyDictionary::UseEphemeris(ephemeris);

    // end astro core initialization

    // Splash screen
    QPixmap pixmap(":/icons/STAsplash.png");
    QSplashScreen splash(pixmap);
    //splash.resize(700, 415);
    splash.resize(550, 326);

    splash.show();
    app.processEvents();
    
    MainWindow* mainwindow = new MainWindow();

    mainwindow->show();
    
    splash.hide();

    int result = app.exec();
    
    return result;
}


static bool ParseCommandLine()
{
    return true;
}
