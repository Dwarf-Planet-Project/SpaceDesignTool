#include "Launcher.h"

#include <QFile>
#include <QTextStream>
#include <QDebug>

Launcher::Launcher()
{
}

void Launcher::setLauncher(QString Name)
{
    QString path = QString("data/Estimations/LauncherDetails.stad");

    QFile LauncherDetails(path);
    LauncherDetails.open(QIODevice::ReadOnly);
//           qDebug()<<LauncherDetails.fileName();
//           qDebug()<<LauncherDetails.isOpen();
    QTextStream LauncherDetailsStream(&LauncherDetails);

    //to skip the first descriptive line of the "LauncherDetails.stad"
    QString InfosOfStadFile;
    int NumberOfColumns = 9;
    int i;
    for (i=0;i<NumberOfColumns;i++)
    {
        LauncherDetailsStream >> InfosOfStadFile;
//                   qDebug() << InfosOfStadFile;
    }

    //find the values from the database - http://nssdc.gsfc.nasa.gov/planetary/factsheet/
    QString LName = "";
    double LCargoBayDiameter = 0.0;
    double LCargoBayLength= 0.0;
    double LAxialAcc= 0.0;
    double LLaterialAcc= 0.0;
    double LAxialFreq= 0.0;
    double LLateralFreq= 0.0;
    double LMassLEO= 0.0;
    double LMassGEO= 0.0;

    do
    {
        LauncherDetailsStream >> LName;
//        qDebug() <<LName;
        LauncherDetailsStream >> LCargoBayDiameter;
//        qDebug() <<LCargoBayDiameter;
        LauncherDetailsStream >> LCargoBayLength;
//        qDebug() <<LCargoBayDiameter;
        LauncherDetailsStream >> LAxialAcc;
//        qDebug() <<LCargoBayDiameter;
        LauncherDetailsStream >> LLaterialAcc;
//        qDebug() <<LCargoBayDiameter;
        LauncherDetailsStream >> LAxialFreq;
//        qDebug() <<LCargoBayDiameter;
        LauncherDetailsStream >> LLateralFreq;
//        qDebug() <<LCargoBayDiameter;
        LauncherDetailsStream >> LMassLEO;
//        qDebug() <<LCargoBayDiameter;
        LauncherDetailsStream >> LMassGEO;
    }
    while ((!(LName==Name))&&(!(LName=="UNDEFINED")));

    LauncherDetails.close();

    CargoBayDiameter = LCargoBayDiameter;
    CargoBayLength = LCargoBayLength;
    AxialFreq = LAxialFreq;
    LateralFreq = LLateralFreq;
    MassLEO = LMassLEO;
    MassGEO = LMassGEO;
}

double Launcher::getCargoBayDiameter()
{
    return CargoBayDiameter;
}

double Launcher::getCargoBayLength()
{
    return CargoBayLength;
}

double Launcher::getAxialFreq()
{
    return AxialFreq;
}

double Launcher::getLateralFreq()
{
    return LateralFreq;
}

double Launcher::getMass(double AltitudeMeter)
{
    if (AltitudeMeter > 35786000)//altitude of GEO
        return MassGEO;
    else
        return MassLEO;
}


