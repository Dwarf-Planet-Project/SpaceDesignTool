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
August 2011
 */


#ifndef DIALOGSERVICEDISTANCERATEUNITFRAME_H
#define DIALOGSERVICEDISTANCERATEUNITFRAME_H

#include "ui_serviceDistanceRateUnit.h"
#include <QFrame>

class DialogServiceDistanceRateUnitFrame : public QFrame , private Ui::DialogServiceDistanceRateUnitFrame
{
    Q_OBJECT
public:
    DialogServiceDistanceRateUnitFrame( QWidget * parent = 0, Qt::WindowFlags f = 0 );
    ~DialogServiceDistanceRateUnitFrame();

public:
    int myPastUnits;  //  The current index equivalent to the units of the input distance
    int myFutureUnits;

    double myPastDistanceRate; // The input distance
    double myFutureDistanceRate; // The converted distance
    double myRealDistanceRateForXMLSchema;  // This distance is any distance but in Km

    QComboBox *distanceRateUnitWidget;  // This combo is required since it is the external expression of the combobox called "comboBoxDistanceRateUnitsChoice"

    void setInputDistanceRate(double niceInputDistanceRate);

    void on_comboBoxDistanceRateUnitsChoice_currentIndexChanged(int myIndex);

private:
    double convertDistanceRate(int fromDistanceRateUnit, int fromDistanceRateUnit, double distance);

	
};

#endif
