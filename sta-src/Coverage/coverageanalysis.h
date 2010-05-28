#ifndef COVERAGEANALYSIS_H
#define COVERAGEANALYSIS_H

#include <Eigen/Core>
#include <Eigen/Geometry>
#include "Scenario/staschema.h"
#include <QFile>
#include <QDebug>
#include <QTextStream>
#include <QDir>
#include "math.h"
#include <Main/propagatedscenario.h>
#include <Astro-Core/constants.h>


class CoverageAnalysis
{
public:
    CoverageAnalysis();
    CoverageAnalysis(PropagatedScenario*);

    void reportAER();


private:

    PropagatedScenario* m_propagatedScenario;



};

#endif // COVERAGEANALYSIS_H
