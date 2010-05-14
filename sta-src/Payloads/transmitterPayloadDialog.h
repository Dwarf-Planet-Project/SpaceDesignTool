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
 ------ Copyright (C) 2010 STA Steering Board (space.trajectory.analysis AT gmail.com) ----
 ------------------ Author: Ricardo Noriega  ----------------------------------------------
 ------------------ email: ricardonor@gmail.com  ------------------------------------------

 */


#ifndef TRANSMITTERPAYLOADDIALOG_H
#define TRANSMITTERPAYLOADDIALOG_H

#include "ui_transmitterPayloadDialog.h"
#include "sta-src/Scenario/staschema.h"
#include <QDialog>
#include <QObject>
#include <QStackedWidget>
#include <QtGui>


class transmitterPayloadDialog : public QDialog , private Ui::transmitterPayloadDialog
{
Q_OBJECT
public:
	transmitterPayloadDialog( QWidget * parent = 0, Qt::WindowFlags f = 0 );
	~transmitterPayloadDialog();
	
	bool loadValues(ScenarioTransmitterPayloadType*);
	bool saveValues(ScenarioTransmitterPayloadType*);

        void antennaCalculations(ScenarioTransmitterPayloadType*);

        ScenarioTransmitterPayloadType* m_transmitterPayload;

protected slots:
	void on_buttonBox_helpRequested();
	void on_buttonBox_clicked(QAbstractButton*);
	void on_tabWidget_currentChanged(int);
	void on_MassLineEdit_textChanged(const QString&);
	void on_LengthLineEdit_textChanged(const QString&);
	void on_WidthLineEdit_textChanged(const QString&);
	void on_HightLineEdit_textChanged(const QString&);
	void on_VertAngleLineEdit_textChanged(const QString&);
	void on_HorAngleLineEdit_textChanged(const QString&);
	void on_ConeAngleLineEdit_textChanged(const QString&);
	void on_ElLineEdit_textChanged(const QString&);
	void on_AzLineEdit_textChanged(const QString&);
	void on_ConeShapeComboBox_activated(const QString&);
	void on_PolarisationGroupBox_toggled(bool);
	void on_PolarisationComboBox_activated(const QString&);
	void on_TiltLineEdit_textChanged(const QString&);
	void on_ModulationGroupBox_toggled(bool);
	void on_TypeModComboBox_activated(const QString&);
	void on_DataRateLineEdit_textChanged(const QString&);
	void on_EquipmentGroupBox_toggled(bool);
	void on_PowerLineEdit_textChanged(const QString&);
	void on_TxFeederLossLineEdit_textChanged(const QString&);
	void on_TxDepointingLossLineEdit_textChanged(const QString&);
	void on_AntennaSizeGroupBox_toggled(bool);
	void on_GainMaxRadioButton_toggled(bool);
	void on_DiameterRadioButton_toggled(bool);
	void on_BeamWidthRadioButton_toggled(bool);
	void on_GainLineEdit_textChanged(const QString&);
	void on_DiameterLineEdit_textChanged(const QString&);
	void on_BeamLineEdit_textChanged(const QString&);
	void on_groupBox_toggled(bool);
	void on_EfficiencyLineEdit_textChanged(const QString&);
	void on_FrequencyLineEdit_textChanged(const QString&);
	void on_TypeBeamComboBox_activated(const QString&);
	
};
#endif //TRANSMITTERPAYLOADDIALOG_H
