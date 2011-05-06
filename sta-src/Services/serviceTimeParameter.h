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
