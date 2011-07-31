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
 July 2011

 */

#include "serviceAngleRateUnit.h"

#include <Eigen/Core>

using namespace Eigen;

#include "QDebug"

DialogServiceAngleRateUnitFrame::DialogServiceAngleRateUnitFrame( QWidget * parent, Qt::WindowFlags f) : QFrame(parent,f)
{
    setupUi(this);
    angleRateUnitWidget = DialogServiceAngleRateUnitFrame::comboBoxAngleRateUnitsChoice;
    myPastUnits = 0;
    comboBoxAngleRateUnitsChoice->setCurrentIndex(myPastUnits);
}

DialogServiceAngleRateUnitFrame::~DialogServiceAngleRateUnitFrame()
{
}


// Index meaning is as follows:
// index = 0  is Degree
// index = 1  is radians

// Matrix coefficients as follows
// Deg/s->Deg/s  Rad/s->Deg/s
// Deg/s->Rad/s  Rad/s->Rad/s

static double angleRateConversionMatrixCoeffs[4] =
{1.0,            57.295779513,
 0.0174532925,   1.0};

static const Matrix<double, 2, 2> angleRateConversionMatrix(angleRateConversionMatrixCoeffs);


double DialogServiceAngleRateUnitFrame::convertAngleRate(int fromAngleRateUnit, int toAngleRateUnit, double angleRate)
{
    double finalAngleRate = angleRate * angleRateConversionMatrix(fromAngleRateUnit, toAngleRateUnit);
    return finalAngleRate;
}


//// Sets the input distance, the output distance and the current index inside the method
void DialogServiceAngleRateUnitFrame::setInputAngleRate(double niceInputAngleRate)
{
    myPastAngleRate = niceInputAngleRate;
}



// Index meaning is as follows:
// index = 0  is Kilometers
// index = 1  is meters
// index = 2  is centi-meters
// index = 3  is mili-meters
// index = 4 is Astronomical Units
void DialogServiceAngleRateUnitFrame::on_comboBoxAngleRateUnitsChoice_currentIndexChanged(int myIndex)
{
    myFutureUnits = myIndex;
    myFutureAngleRate = convertAngleRate(myPastUnits, myFutureUnits, myPastAngleRate);
    myPastAngleRate = myFutureAngleRate;
    myPastUnits = myFutureUnits;
    myRealAngleRateForXMLSchema = convertAngleRate (myPastUnits, 0, myFutureAngleRate);
}

