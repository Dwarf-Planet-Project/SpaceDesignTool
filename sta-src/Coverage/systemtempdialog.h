#ifndef SYSTEMTEMPDIALOG_H
#define SYSTEMTEMPDIALOG_H

#include "ui_SystemTemp.h"
#include <QDialog>
#include "maincoveragegui.h"
#include "Scenario/staschema.h"

class SystemTempDialog : public QDialog , private Ui::SystemTemperature
{
Q_OBJECT
public:
	SystemTempDialog( QWidget * parent = 0, Qt::WindowFlags f = 0 );
	~SystemTempDialog();
        bool loadValues(ScenarioReceiver*);
        bool saveValues(ScenarioReceiver*);

protected slots:
	void on_SystemTempGroupBox_toggled(bool);
	void on_RxNoiseFigureLineEdit_textChanged(const QString&);
	void on_RxFeederTmpLineEdit_textChanged(const QString&);
	void on_RxTempLineEdit_textChanged(const QString&);
	void on_ConstantTempRadioButton_toggled(bool);
	void on_AntennaTempLineEdit_textChanged(const QString&);
	void on_CalculatedTempRadioButton_toggled(bool);
	void on_OkButton_clicked();
	void on_CancelButton_clicked();
	
};
#endif //SYSTEMTEMPDIALOG_H
