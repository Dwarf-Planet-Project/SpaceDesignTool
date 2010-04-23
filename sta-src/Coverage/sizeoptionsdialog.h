#ifndef SIZEOPTIONSDIALOG_H
#define SIZEOPTIONSDIALOG_H

#include "ui_AntennaSize.h"
#include <QDialog>
#include "maincoveragegui.h"
#include "Scenario/staschema.h"

class SizeOptionsDialog : public QDialog , private Ui::SizeOptions
{
Q_OBJECT
public:
	SizeOptionsDialog( QWidget * parent = 0, Qt::WindowFlags f = 0 );
	~SizeOptionsDialog();
        bool loadValues(ScenarioAntennaType*);
        bool saveValues(ScenarioAntennaType*);

protected slots:
	void on_AntennaSizeGroupBox_toggled(bool);
	void on_GainMaxRadioButton_toggled(bool);
	void on_DiameterRadioButton_toggled(bool);
	void on_BeamWidthRadioButton_toggled(bool);
	void on_GainLineEdit_textChanged(const QString&);
	void on_DiameterLineEdit_textChanged(const QString&);
	void on_BeamLineEdit_textChanged(const QString&);
	void on_OkButton_clicked();
	void on_CancelButton_clicked();
	
};
#endif //SIZEOPTIONSDIALOG_H
