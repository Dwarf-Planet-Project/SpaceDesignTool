#ifndef MAINCOVERAGEGUI_H
#define MAINCOVERAGEGUI_H

#include "ui_CoverageModule.h"
#include <QDialog>
#include <QStackedWidget>
#include "sizeoptionsdialog.h"
#include "systemtempdialog.h"
#include "coneshapedialog.h"
#include  "Main/scenariotree.h"
#include "sta-src/Scenario/staschema.h"

class MainCoverageGUI : public QDialog , private Ui::MainCoverageGUI
{
Q_OBJECT
public:

	MainCoverageGUI(ScenarioTree* parent);

	MainCoverageGUI( QWidget * parent = 0, Qt::WindowFlags f = 0 );
	~MainCoverageGUI();

        bool loadValues(ScenarioCommunicationPayloadType*);
        bool saveValues(ScenarioCommunicationPayloadType*);
        //bool loadValues();
        //bool saveValues();
public slots:
     //   void enableConeShape(int);

protected slots:
	void on_tabWidget_2_currentChanged(int);
	void on_PointingGroupBox_2_toggled(bool);
	void on_ElLineEdit_textChanged(const QString&);
	void on_AzLineEdit_textChanged(const QString&);
	void on_ConeShapeComboBox_activated(const QString&);
	void on_ConeShapeToolButton_clicked();
	void on_PolarisationGroupBox_toggled(bool);
	void on_PolarisationComboBox_activated(const QString&);
	void on_TiltLineEdit_textChanged(const QString&);
	void on_AntennaPropGroupBox_toggled(bool);
	void on_EfficiencyLineEdit_textChanged(const QString&);
	void on_FrequencyLineEdit_textChanged(const QString&);
	void on_TypeBeamComboBox_activated(const QString&);
	void on_SizeOptionsButton_clicked();
	void on_Transmitter_toggled(bool);
	void on_PowerLineEdit_textChanged(const QString&);
	void on_TxFeederLossLineEdit_textChanged(const QString&);
	void on_TxDepointingLossLineEdit_textChanged(const QString&);
	void on_ModulationGroupBox_toggled(bool);
	void on_TypeModComboBox_activated(const QString&);
	void on_DataRateLineEdit_textChanged(const QString&);
	void on_Receiver_toggled(bool);
	void on_SystemTempToolButton_clicked();
	void on_SystemTempLineEdit_textChanged(const QString&);
	void on_RxFeederLineEdit_textChanged(const QString&);
	void on_RxDepointingLineEdit_textChanged(const QString&);
	void on_OkButton_clicked();
	void on_CancelButton_clicked();
	
};
#endif //MAINCOVERAGEGUI_H
