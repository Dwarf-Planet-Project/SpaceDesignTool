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
    CoverageAnalysis(PropagatedScenario*, int, int, int);

    void reportAER();


private:

    PropagatedScenario* m_propagatedScenario;
    int m_indexSC;
    int m_indexGS;
    int m_indexMA;


};

#endif // COVERAGEANALYSIS_H
