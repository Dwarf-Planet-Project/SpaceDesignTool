#ifndef AERODYNAMICMETHODS_H
#define AERODYNAMICMETHODS_H

#include "ui_aerodynamicmethods.h"
#include "Scenario/staschema.h"
#include <QDialog>

class AerodynamicMethodDialog : public QDialog , private Ui::AerodynamicMethodDialog
{
Q_OBJECT
public:
        AerodynamicMethodDialog(ScenarioREVAeroThermodynamicsType* aero, QWidget * parent = 0, Qt::WindowFlags f = 0 );
	~AerodynamicMethodDialog();

protected slots:
	void on_buttonBox_helpRequested();
	void on_buttonBox_clicked(QAbstractButton*);
	
};
#endif //AERODYNAMICMETHODDIALOG_H
