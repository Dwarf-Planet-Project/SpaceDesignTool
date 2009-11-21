#ifndef CWIZARD_H
#define CWIZARD_H

#include "ui_cwizard.h"
#include <QWizard>

class cwizard : public QWizard , private Ui::cwizard
{
Q_OBJECT
public:
	cwizard( QWidget * parent = 0, Qt::WindowFlags f = 0 );
	~cwizard();
protected slots:
	void on___qt__passive_wizardbutton0_clicked();
	void on___qt__passive_wizardbutton1_clicked();
	void on_comboBoxConstellationType_activated(const QString&);
	void on_StartingEpochdateTimeEdit_dateTimeChanged(const QDateTime&);
	void on_EndingEpochdateTimeEdit_dateTimeChanged(const QDateTime&);
	void on_lineEdit_textChanged(const QString&);
	void on_centralBodyComboBox_activated(const QString&);
	void on_lineEdit_2_textChanged(const QString&);
	void on_lineEdit_3_textChanged(const QString&);
	void on_lineEdit_4_textChanged(const QString&);
	void on_GravityFieldRadioButton_toggled(bool);
	void on_ZonalsSpinBox_valueChanged(int);
	void on_GravityComboBox_activated(const QString&);
	void on_AtmDragRadioButton_toggled(bool);
	void on_AtmosphereComboBox_activated(const QString&);
	void on_PertBodyRadioButton_toggled(bool);
	void on_AddPushButton_clicked();
	void on_RemovePushButton_clicked();
	void on_PropagatorComboBox_activated(const QString&);
	void on_IntegratorComboBox_activated(const QString&);
	void on_IntStepEdit_textChanged(const QString&);
	void on___qt__passive_wizardbutton5_clicked();
	
};
#endif //CWIZARD_H
