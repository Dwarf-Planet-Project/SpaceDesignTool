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

#include "serviceDistanceRateUnit.h"

#include <Eigen/Core>

using namespace Eigen;

#include "QDebug"

DialogServiceDistanceRateUnitFrame::DialogServiceDistanceRateUnitFrame( QWidget * parent, Qt::WindowFlags f) : QFrame(parent,f)
{
	setupUi(this);
    distanceRateUnitWidget = DialogServiceDistanceRateUnitFrame::comboBoxDistanceRateUnitsChoice;
    myPastUnits = 0;
    comboBoxDistanceRateUnitsChoice->setCurrentIndex(myPastUnits);
}

DialogServiceDistanceRateUnitFrame::~DialogServiceDistanceRateUnitFrame()
{
}


// Index meaning is as follows:
// index = 0  is Kilometers/s
// index = 1  is meters/s
// index = 2  is centi-meters/s
// index = 3  is mili-meters/s
// index = 4 is Astronomical Units/s



// Matrix coefficients as follows
//  Km->Km, Km->m, Km->cm, Km->mm, Km->AU,
//  m->Km,   m->m,  m->cm,  m->mm,  m->AU,
//  cm->Km, cm->m, cm->cm, cm->mm, cm-> AU,
//  mm->Km, mm->m, mm->cm, mm->mm, mm->AU,
//  AU->Km, AU->m, AU->cm, AU->mm, AU->AU
// The matrice has to be transposed !!


static double distanceRateConversionMatrixCoeffs[25] =
{1.0,            0.001,           0.00001,        0.000001,       1.495978707e+08,
 1000.0,         1.0,             0.01,           0.001,          1.495978707e+11,
 100000.0,       100.0,           1.0,            0.1,            1.495978707e+13,
 1000000.0,      1000.0,          10.0,           1.0,            1.495978707e+14,
 0.6684587e-19,  0.6684587e-12,   0.6684587e-13,  0.6684587e-16,  1.0};

static const Matrix<double, 5, 5> distanceRateConversionMatrix(distanceRateConversionMatrixCoeffs);



double DialogServiceDistanceRateUnitFrame::convertDistanceRate(int fromDistanceRateUnit, int toDistanceRateUnit, double distance)
{
    double finalDistanceRate = distance * distanceRateConversionMatrix(fromDistanceRateUnit, toDistanceRateUnit);
    return finalDistanceRate;
}


//// Sets the input distance, the output distance and the current index inside the method
void DialogServiceDistanceRateUnitFrame::setInputDistanceRate(double niceInputDistanceRate)
{
    myPastDistanceRate = niceInputDistanceRate;
}



// Index meaning is as follows:
// index = 0  is Kilometers/s
// index = 1  is meters/s
// index = 2  is centi-meters/s
// index = 3  is mili-meters/s
// index = 4 is Astronomical Units/s
void DialogServiceDistanceRateUnitFrame::on_comboBoxDistanceRateUnitsChoice_currentIndexChanged(int myIndex)
{
    myFutureUnits = myIndex;
    myFutureDistanceRate = convertDistanceRate(myPastUnits, myFutureUnits, myPastDistanceRate);
    myPastDistanceRate = myFutureDistanceRate;
    myPastUnits = myFutureUnits;
    myRealDistanceRateForXMLSchema = convertDistanceRate (myPastUnits, 0, myFutureDistanceRate);
}

