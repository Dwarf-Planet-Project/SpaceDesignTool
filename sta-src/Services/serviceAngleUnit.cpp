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

#include "serviceAngleUnit.h"

#include <Eigen/Core>

using namespace Eigen;

#include "QDebug"

DialogServiceAngleUnitFrame::DialogServiceAngleUnitFrame( QWidget * parent, Qt::WindowFlags f) : QFrame(parent,f)
{
	setupUi(this);
    angleUnitWidget = DialogServiceAngleUnitFrame::comboBoxAngleUnitsChoice;
    myPastUnits = 0;
    comboBoxAngleUnitsChoice->setCurrentIndex(myPastUnits);
}

DialogServiceAngleUnitFrame::~DialogServiceAngleUnitFrame()
{
}


// Index meaning is as follows:
// index = 0  is Degree
// index = 1  is radians

// Matrix coefficients as follows
// Deg->Deg  Rad->Deg
// Deg->Rad  Rad->Rad

static double angleConversionMatrixCoeffs[4] =
{1.0,            57.295779513,
 0.0174532925,   1.0};

static const Matrix<double, 2, 2> angleConversionMatrix(angleConversionMatrixCoeffs);


double DialogServiceAngleUnitFrame::convertAngle(int fromAngleUnit, int toAngleUnit, double distance)
{
    //qDebug() << distanceConversionMatrix(0,0) << distanceConversionMatrix(0,1)<< distanceConversionMatrix(0,2) << distanceConversionMatrix(0,3) << distanceConversionMatrix(0,4) << endl;
    double finalAngle = distance * angleConversionMatrix(fromAngleUnit, toAngleUnit);
    //qDebug() << fromAngleUnit << toAngleUnit << distanceConversionMatrix(fromAngleUnit, toAngleUnit) << distance << "--->" << finalAngle << endl;
    return finalAngle;
}


//// Sets the input distance, the output distance and the current index inside the method
void DialogServiceAngleUnitFrame::setInputAngle(double niceInputAngle)
{
    myPastAngle = niceInputAngle;
}



// Index meaning is as follows:
// index = 0  is Kilometers
// index = 1  is meters
// index = 2  is centi-meters
// index = 3  is mili-meters
// index = 4 is Astronomical Units
void DialogServiceAngleUnitFrame::on_comboBoxAngleUnitsChoice_currentIndexChanged(int myIndex)
{
    //qDebug() << myPastUnits << "==>" << myFutureUnits << "||||" << myPastAngle << "--->" << myFutureAngle << endl;
    myFutureUnits = myIndex;
    myFutureAngle = convertAngle(myPastUnits, myFutureUnits, myPastAngle);
    myPastAngle = myFutureAngle;
    myPastUnits = myFutureUnits;
    myRealAngleForXMLSchema = convertAngle (myPastUnits, 0, myFutureAngle);
}

