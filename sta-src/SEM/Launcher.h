#ifndef LAUNCHER_H
#define LAUNCHER_H

#include "QString"

class Launcher
{
public:
    Launcher();

    void setLauncher(QString Name);

    double getCargoBayDiameter();
    double getCargoBayLength();
    double getAxialFreq();
    double getLateralFreq();
    double getMass(double AltitudeMeter);

private:
    double CargoBayDiameter;
    double CargoBayLength;
    double AxialFreq;
    double LateralFreq;
    double MassLEO;
    double MassGEO;
};

#endif // LAUNCHER_H
