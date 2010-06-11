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
 ------------------ Author: Ozgun YILMAZ      ---------------------------------------------
 ------------------ email: ozgunus@yahoo.com  ---------------------------------------------

 */

#ifndef SEMWIZARD_H
#define SEMWIZARD_H

#include "ui_SEMWizard.h"
#include <QWizard>

#include "SemMain.h"
#include "Scenario/staschema.h"

class SEMWizard : public QWizard , private Ui::Wizard
{
Q_OBJECT
public:
         SEMWizard(ScenarioSC* SCVehicle,QString MissionArc, QWidget * parent = 0, Qt::WindowFlags f = 0 );
	~SEMWizard();
protected slots:
        void showHelp();
        void wizardIsCompleted();
//	void on___qt__passive_wizardbutton0_clicked();
        void on___qt__passive_wizardbutton1_clicked();
//	void on_MissionTypeComboBox_activated(const QString&);
//	void on_PlanetNameComboBox_activated(const QString&);
//      void on_MissionStartTimeDateTimeEdit_dateTimeChanged(const QDateTime &datetime);
//      void on_MissionEndTimeDateTimeEdit_dateTimeChanged(const QDateTime &datetime);
//        void on_SemimajorAxisLineEdit_textEdited(const QString&);
//        void on_EccentricityLineEdit_textEdited(const QString&);
//        void on_InclinationLineEdit_textEdited(const QString&);
//        void on_RAANLineEdit_textEdited(const QString&);
//        void on_ArgumentOfPeriapsisLineEdit_textEdited(const QString&);
//        void on_TrueAnomalyLineEdit_textEdited(const QString&);
        void on_NewPayloadRadioButton_toggled(bool);
        void on_DatabasePayloadRadioButton_toggled(bool);
//	void on_PayloadSetComboBox_activated(const QString&);
//        void on_PayloadNameLineEdit_textEdited(const QString&);
        void on_PayloadResolutionLineEdit_textEdited(const QString&);
        void on_PayloadWavelengthLineEdit_textEdited(const QString&);
//        void on_PayloadWidthLineEdit_textEdited(const QString&);
//        void on_PayladLengthLineEdit_textEdited(const QString&);
//        void on_PayloadHeightLineEdit_textEdited(const QString&);
//        void on_PayloadMassLineEdit_textEdited(const QString&);
//        void on_PayloadDataRateLineEdit_textEdited(const QString&);
        void on_NumberOfPixelLineEdit_textEdited(const QString&);
//        void on_PayloadPowerEclipseLineEdit_textEdited(const QString&);
//        void on_PayloadPowerDaylightLineEdit_textEdited(const QString&);
//        void on_PayloadPowerTimeEclipseLineEdit_textEdited(const QString&);
//        void on_PayloadPowerTimeDaylightLineEdit_textEdited(const QString&);
//        void on_PayloadMinTempLineEdit_textEdited(const QString&);
//        void on_PayloadMaxTempLineEdit_textEdited(const QString&);
	void on_NumberOfPayloadCheckBox_toggled(bool);
	void on_NewPayloadRadioButton_2_toggled(bool);
//	void on_PayloadSetComboBox_2_activated(const QString&);
	void on_DatabasePayloadRadioButton_2_toggled(bool);
//        void on_PayloadNameLineEdit_2_textEdited(const QString&);
        void on_PayloadResolutionLineEdit_2_textEdited(const QString&);
        void on_PayloadWavelengthLineEdit_2_textEdited(const QString&);
//        void on_PayloadWidthLineEdit_2_textEdited(const QString&);
//        void on_PayladLengthLineEdit_2_textEdited(const QString&);
//        void on_PayloadHeightLineEdit_2_textEdited(const QString&);
//        void on_PayloadMassLineEdit_2_textEdited(const QString&);
//        void on_PayloadDataRateLineEdit_2_textEdited(const QString&);
        void on_NumberOfPixelLineEdit_2_textEdited(const QString&);
//        void on_PayloadPowerEclipseLineEdit_2_textEdited(const QString&);
//        void on_PayloadPowerDaylightLineEdit_2_textEdited(const QString&);
//        void on_PayloadPowerTimeEclipseLineEdit_2_textEdited(const QString&);
//        void on_PayloadPowerTimeDaylightLineEdit_2_textEdited(const QString&);
//        void on_PayloadMinTempLineEdit_2_textEdited(const QString&);
//        void on_PayloadMaxTempLineEdit_2_textEdited(const QString&);
	void on_NumberOfPayloadCheckBox_2_toggled(bool);
	void on_NewPayloadRadioButton_3_toggled(bool);
//	void on_PayloadSetComboBox_3_activated(const QString&);
        void on_DatabasePayloadRadioButton_3_toggled(bool);
//        void on_PayloadNameLineEdit_3_textEdited(const QString&);
        void on_PayloadResolutionLineEdit_3_textEdited(const QString&);
        void on_PayloadWavelengthLineEdit_3_textEdited(const QString&);
//        void on_PayloadWidthLineEdit_3_textEdited(const QString&);
//        void on_PayladLengthLineEdit_3_textEdited(const QString&);
//        void on_PayloadHeightLineEdit_3_textEdited(const QString&);
//        void on_PayloadMassLineEdit_3_textEdited(const QString&);
//        void on_PayloadDataRateLineEdit_3_textEdited(const QString&);
        void on_NumberOfPixelLineEdit_3_textEdited(const QString&);
//        void on_PayloadPowerEclipseLineEdit_3_textEdited(const QString&);
//        void on_PayloadPowerDaylightLineEdit_3_textEdited(const QString&);
//        void on_PayloadPowerTimeEclipseLineEdit_3_textEdited(const QString&);
//        void on_PayloadPowerTimeDaylightLineEdit_3_textEdited(const QString&);
//        void on_PayloadMinTempLineEdit_3_textEdited(const QString&);
//        void on_PayloadMaxTempLineEdit_3_textEdited(const QString&);
	void on_NumberOfPayloadCheckBox_3_toggled(bool);
	void on_NewPayloadRadioButton_4_toggled(bool);
//	void on_PayloadSetComboBox_4_activated(const QString&);
        void on_DatabasePayloadRadioButton_4_toggled(bool);
//        void on_PayloadNameLineEdit_4_textEdited(const QString&);
        void on_PayloadResolutionLineEdit_4_textEdited(const QString&);
        void on_PayloadWavelengthLineEdit_4_textEdited(const QString&);
//        void on_PayloadWidthLineEdit_4_textEdited(const QString&);
//        void on_PayladLengthLineEdit_4_textEdited(const QString&);
//        void on_PayloadHeightLineEdit_4_textEdited(const QString&);
//        void on_PayloadMassLineEdit_4_textEdited(const QString&);
//        void on_PayloadDataRateLineEdit_4_textEdited(const QString&);
        void on_NumberOfPixelLineEdit_4_textEdited(const QString&);
//        void on_PayloadPowerEclipseLineEdit_4_textEdited(const QString&);
//        void on_PayloadPowerDaylightLineEdit_4_textEdited(const QString&);
//        void on_PayloadPowerTimeEclipseLineEdit_4_textEdited(const QString&);
//        void on_PayloadPowerTimeDaylightLineEdit_4_textEdited(const QString&);
//        void on_PayloadMinTempLineEdit_4_textEdited(const QString&);
//        void on_PayloadMaxTempLineEdit_4_textEdited(const QString&);
	void on_SolarCellTypeComboBox_activated(const QString&);
	void on_BatteryTypeComboBox_activated(const QString&);
	void on_StructuralMaterialComboBox_activated(const QString&);
	void on_SCShapeComboBox_activated(const QString&);
        void on_ColdFaceEmmissivityLineEdit_textEdited(const QString&);
        void on_ColdFaceAbsorptivityLineEdit_textEdited(const QString&);
        void on_HotFaceEmmissivityLineEdit_textEdited(const QString&);
        void on_HotFaceAbsorptivityLineEdit_textEdited(const QString&);
	void on_ColdFaceCoatingComboBox_activated(const QString&);
	void on_HotFaceCoatingComboBox_activated(const QString&);
        void on_AntennaEfficiencyLineEdit_textEdited(const QString&);
        void on_AntennaGainLineEdit_textEdited(const QString&);
        void on_TXFrequencyLineEdit_textEdited(const QString&);
        void on_TXPowerLineEdit_textEdited(const QString&);
        void on_CommLinDurationPercentageLineEdit_textEdited(const QString&);
        void on_NumberOfNoLinkOrbitLineEdit_textEdited(const QString&);
	void on_CodingTypeComboBox_activated(const QString&);

	
private:
        SemMain SC;
        ScenarioSC* Vehicle;
        ScenarioLoiteringType* SCLoiteringArc;
//        int NumberOfPayloads;
};
#endif //SEMWIZARD_H
