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
 ------ Copyright (C) 2011 STA Steering Board (space.trajectory.analysis AT gmail.com) ----
*/
/*
------------------ Author: Guillermo Ortega  ----------------------------------------
 May 2011

 */

#include <QString>


#ifdef Q_WS_MAC
#include <CoreFoundation/CFBundle.h>
#endif


/** Calculates the path of the STA Data folder
  * This path is essential for STA to find files needed for the compuatation
  * The path is different per operatiing system that STA supports
  *
  * \return a string with the path of the data folder
  *
  */
QString findDataFolder()
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

    return staResourcesPath;

}

