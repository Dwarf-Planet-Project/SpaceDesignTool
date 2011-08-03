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
------------------ Author: Guillermo Ortega  -------------------------------------------
July 2011
 */


#ifndef DIALOGSERVICEANGLEUNITFRAME_H
#define DIALOGSERVICEANGLEUNITFRAME_H

#include "ui_serviceAngleUnit.h"
#include <QFrame>

class DialogServiceAngleUnitFrame : public QFrame , private Ui::DialogServiceAngleUnitFrame
{
Q_OBJECT
public:
	DialogServiceAngleUnitFrame( QWidget * parent = 0, Qt::WindowFlags f = 0 );
	~DialogServiceAngleUnitFrame();

public:
    int myPastUnits;  //  The current index equivalent to the units of the input distance
    int myFutureUnits;

    double myPastAngle; // The input angle
    double myFutureAngle; // The converted angle
    double myRealAngleForXMLSchema;  // This angle is any angle but in radians

    QComboBox *angleUnitWidget;  // This combo is required since it is the external expression of the combobox called "comboBoxAngleUnitsChoice"

    void setInputAngle(double niceInputAngle);

    void on_comboBoxAngleUnitsChoice_currentIndexChanged(int myIndex);

private:
    double convertAngle(int fromAngleUnit, int toAngleUnit, double angle);

	
};
#endif //DIALOGSERVICEAngleUNITFRAME_H
