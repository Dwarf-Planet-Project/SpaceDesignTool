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

 ------------------ Author: Ricardo Noriega de Soto  -------------------------------------------------
 ------------------ Email: ricardonor@gmail.com   -------------------------------------------------
  Patched by Guillermo June 2010 to add constellations module

 */


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
