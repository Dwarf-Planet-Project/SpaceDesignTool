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
------------------ Author:       Marnix Volckaert               -------------------
------------------ Affiliation:  TUDelft                        -------------------
-----------------------------------------------------------------------------------
 Modified by Tiziana Sabatini on May-July 2009
*/


#include <math.h>
#include <iostream>
#include <fstream>
#include <string>
#include <QString>
#include "CelestialBody.h"
#include <QFile>
#include <QDebug>
#include <QTextStream>
#include <QDir>
#include <QProcess>

using namespace std;



//------ Function to fetch the right data of a celestial body ------------------------------------------------
void CelestialBody::selectBody (QString name) {



    //QString path = "C:/Users/Tizy/Desktop/Code/trunk/sta-src/Entry/data/bodies/";
    QString path = QString("data/bodies/");
    path.append(name);

    //QTextStream out (stdout); out << "===> Bodies path: " << path << endl;

    QFile body(path);
    body.open(QIODevice::ReadOnly);
    QTextStream bodystream(&body);//------

    bodystream >> mu;
    bodystream >> omega;
    bodystream >> equatorial_radius;
    bodystream >> polar_radius;
    bodystream >> J2;
    bodystream >> J3;
    bodystream >> J4;
    //------
    body.close();                     //close the body's data file
}
//------------------------------------------------------------------------------------------------------------
