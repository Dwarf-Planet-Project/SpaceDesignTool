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
 */

#ifndef DIALOGSERVICETIMEPARAMETERFRAME_H
#define DIALOGSERVICETIMEPARAMETERFRAME_H

#include "ui_serviceTimeParameter.h"
#include <QFrame>
#include "Astro-Core/date.h"
class DialogServiceTimeParameterFrame : public QFrame , private Ui::DialogServiceTimeParameterFrame
{
Q_OBJECT
public:
	DialogServiceTimeParameterFrame( QWidget * parent = 0, Qt::WindowFlags f = 0 );
	~DialogServiceTimeParameterFrame();
protected slots:
	void on_comboBoxTimeParameterChoice_currentIndexChanged(int);
	
};

QString convertToGregUTC(double mjd[], int num);
QString convertToGregTDB(double mjd[], int num);
QString convertToGregTAI(double mjd[], int num);
QString convertToGregTDT(double mjd[], int num);
QString convertToGregGPS(double mjd[], int num);

double convertToMJDUTC(double mjd[], int num);
double convertToJulianTDB(double mjd[], int num);
double convertToJulianDate(double mjd[], int num);
double convertToJulianEphDate(double mjd[], int num);
double convertToJulianGPS(double mjd[], int num);
double convertToEarthCanTime(double mjd[], int num, double StartEpoch);
void convertToYYDDD(double MJDdate[],int index, int * Last2Digits , QList<double> *DDD);
void convertToJulianUTC(double mjd[],int num, int * Last2Digits , QList<double> *DDD, double *hour, double *minute, double *second);
#endif //DIALOGSERVICETIMEPARAMETERFRAME_H
