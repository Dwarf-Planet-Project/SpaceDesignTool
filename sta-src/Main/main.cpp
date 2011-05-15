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
 Patched by Guillermo July 2010 to alow drag files into the app icon
 Patched by Guillermo on May 2011 to find the file path automatically.

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
#include <iostream>
#include <QFileOpenEvent>
#include <QDesktopServices>

#ifdef Q_WS_MAC
#include <CoreFoundation/CFBundle.h>
#endif

#include "Astro-Core/stabody.h"
#include "Astro-Core/jplephemeris.h"
#include "Astro-Core/SpiceEphemeris.h"
#include "findDataFolder.h"

#include "main.h"
#include "mainwindow.h"

using namespace sta;


// Celestia command line options
static QString configFileName;

static bool ParseCommandLine();

// Handling of drag-drop events
bool EventOccurred = true;
bool EventOccurredBlock = false;


int main(int argc, char *argv[])
{
    //QTextStream out (stdout);

    QApplication* app = new QApplication(argc,argv);
    //////////// Handling of drag and drop events ////////////////
    EventListener* eventListener = new EventListener();
    app->installEventFilter(eventListener);
    app->processEvents();
    //////////// End of handling of drag and drop events ////////////////

    // Splash screen
    QPixmap pixmap(":/icons/STAsplash.png");
    QSplashScreen splash(pixmap, Qt::Tool);
    splash.resize(550, 326);
    splash.show();

    QCoreApplication::setOrganizationName(QObject::tr("STA Steering Board"));
    QCoreApplication::setApplicationName("STA");
    QCoreApplication::setOrganizationName("stasb");    // 'stasb' means STA Steering Board
    QCoreApplication::setOrganizationDomain("org");  // Patched by Guillermo
    QCoreApplication::setApplicationVersion("4.0");
    QCoreApplication::setOrganizationDomain("STASB");  // Patched by Guillermo

    //QString ApplicationPath = QDir::currentPath ();
    //QString ResourcesPath = ApplicationPath + "/sta-data";

    //QString ApplicationPath = QApplication::applicationFilePath();
    //        ApplicationPath.truncate(ApplicationPath.lastIndexOf("/"));
    //QString ResourcesPath = ApplicationPath + "/sta-data";

    ParseCommandLine();

    QString staResourcesPath = findDataFolder();


#if defined(Q_WS_MAC)
    if (!QDir::setCurrent(staResourcesPath))
    {
        QMessageBox::warning(NULL,
                             QObject::tr("STA Resources Not Found"),
                             QObject::tr("STA resources folder wasn't found. This probably means that STA was not properly installed"));
        exit(0);
    }
#else
    if (!QDir::setCurrent(staResourcesPath))  // Patched by Guillermo
    {
        QMessageBox::warning(NULL,
                             QObject::tr("STA Resources Not Found"),
                             QObject::tr("STA resources folder wasn't found. This probably means that STA was not properly installed"));
        exit(0);
    }
#endif

    // Initialize the astro core
    SolarSystemBodyDictionary::Create();

    // Initialize SPICE
    SpiceEphemeris* spiceEphem = SpiceEphemeris::InitializeSpice(QDesktopServices::storageLocation(QDesktopServices::DataLocation) + "/spice");
    if (spiceEphem)
    {
        qDebug() << "Using SPICE ephemeris";
        SolarSystemBodyDictionary::UseEphemeris(spiceEphem);
        qDebug() << "OK";
    }
    else
    {
        qDebug() << "SPICE kernels for solar system ephemeris not located or incomplete. JPL DE405 ephemeris";
        qDebug() << "will be instead. This is only a problem when analyzing missions to natural satellites";
        qDebug() << "other than the Moon.";

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
    }

    // end astro core initialization


    MainWindow* mainwindow = new MainWindow();

#if defined (Q_WS_MAC)
    eventListener->setMainWin(mainwindow);
    QString fileToOpen = eventListener->file();
    mainwindow->openFileFromAEvent(fileToOpen);
#else
    QString fileToOpen = ":/untitled.stas";
    const QStringList args = QCoreApplication::arguments();
    if (args.count() == 2)
    {
        fileToOpen = args.at(1);
        mainwindow->openFileFromAEvent(fileToOpen);
    }
#endif


    mainwindow->show();

    splash.hide();

    int result = app->exec();

    delete mainwindow;
    delete eventListener;
    delete app;

    return result;
} ////////////////////////// End of Main.CPP /////////////////////


static bool ParseCommandLine()
{
    return true;
}




/*! Guillermo added this method to handle file drag onto app icon
  */
bool EventListener::eventFilter(QObject*, QEvent* event)
{
    //QTextStream out (stdout);

    if (!EventOccurred)
    {
        int t = event->type();
        if ( (t>=QEvent::MouseButtonPress && t<=QEvent::KeyRelease) || (t>=QEvent::HoverEnter && t<=QEvent::HoverMove) )
            EventOccurred = true;
        //out << "Event occured " << endl;
    }

#ifdef Q_WS_MAC
    if (event->type() == QEvent::FileOpen)
    {
        //out << "QEvent::FileOpen " << endl;
        QString filename = static_cast<QFileOpenEvent*>(event)->file();

        if (pMainWindow)
        {
            if (QApplication::activeModalWidget() == NULL)
            {
                pMainWindow->openFileFromAEvent(filename);
                //out << "File dropped " << endl;
            }
            else
            {
                return true; // ignore file open events while a modal dialog is displayed
            }
        }
        else
        {
            pFile = filename;
        }
    }

#endif

    //out << "Event Listerner called " << endl;
    return false;
}
