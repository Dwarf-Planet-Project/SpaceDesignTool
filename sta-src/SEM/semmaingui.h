#ifndef SEMMAINGUI_H
#define SEMMAINGUI_H

#include "ui_SemMainGUI.h"
#include <QDialog>

#include "SemMain.h"
#include <QDebug>

#include "Scenario/staschema.h"

class SemMainGUI : public QDialog , private Ui::SemMainGUI
{
Q_OBJECT
public:
        SemMainGUI(ScenarioSC* SCVehicle, QWidget * parent = 0, Qt::WindowFlags f = 0 );
        SemMainGUI(SemMain * SCWizard, ScenarioSC* SCVehicle, QWidget * parent = 0, Qt::WindowFlags f = 0 );
        ~SemMainGUI();

       SemMain SC;

protected slots:
        void on_MainSemTabWidget_currentChanged(int Index);
//        void on_groupBox_17_toggled(bool);
        void on_Payload1WidthLineEdit_textEdited(const QString&);
        void on_Payload1LengthLineEdit_textEdited(const QString&);
        void on_Payload1HeightLineEdit_textEdited(const QString&);
//        void on_Payload1VolumeLineEdit_textChanged(const QString&);
        void on_Payload1MassLineEdit_textChanged(const QString&);
        void on_Payload1PowerConsumptionInEclipseLineEdit_textChanged(const QString&);
        void on_Payload1PowerConsumptionInDaylightLineEdit_textChanged(const QString&);
        void on_Payload1PowerOnTimeEclipseLineEdit_textChanged(const QString&);
        void on_Payload1PowerOnTimeDaylightLineEdit_textChanged(const QString&);
        void on_Payload1MaxTempLimitLineEdit_textChanged(const QString&);
        void on_Payload1MinTempLimitLineEdit_textChanged(const QString&);
        void on_Payload1DataRateLineEdit_textChanged(const QString&);
//        void on_groupBox_18_toggled(bool);
        void on_Payload2WidthLineEdit_textChanged(const QString&);
        void on_Payload2LengthLineEdit_textChanged(const QString&);
        void on_Payload2HeightLineEdit_textChanged(const QString&);
//        void on_Payload2VolumeLineEdit_textChanged(const QString&);
        void on_Payload2MassLineEdit_textChanged(const QString&);
        void on_Payload2PowerConsumptionInEclipseLineEdit_textChanged(const QString&);
        void on_Payload2PowerConsumptionInDaylightLineEdit_textChanged(const QString&);
        void on_Payload2PowerOnTimeEclipseLineEdit_textChanged(const QString&);
        void on_Payload2PowerOnTimeDaylightLineEdit_textChanged(const QString&);
        void on_Payload2MaxTempLimitLineEdit_textChanged(const QString&);
        void on_Payload2MinTempLimitLineEdit_textChanged(const QString&);
        void on_Payload2DataRateLineEdit_textChanged(const QString&);
//        void on_groupBox_19_toggled(bool);
        void on_Payload3WidthLineEdit_textChanged(const QString&);
        void on_Payload3LengthLineEdit_textChanged(const QString&);
        void on_Payload3HeightLineEdit_textChanged(const QString&);
//        void on_Payload3VolumeLineEdit_textChanged(const QString&);
        void on_Payload3MassLineEdit_textChanged(const QString&);
        void on_Payload3PowerConsumptionInEclipseLineEdit_textChanged(const QString&);
        void on_Payload3PowerConsumptionInDaylightLineEdit_textChanged(const QString&);
        void on_Payload3PowerOnTimeEclipseLineEdit_textChanged(const QString&);
        void on_Payload3PowerOnTimeDaylightLineEdit_textChanged(const QString&);
        void on_Payload3MaxTempLimitLineEdit_textChanged(const QString&);
        void on_Payload3MinTempLimitLineEdit_textChanged(const QString&);
        void on_Payload3DataRateLineEdit_textChanged(const QString&);
//        void on_groupBox_20_toggled(bool);
        void on_Payload4WidthLineEdit_textChanged(const QString&);
        void on_Payload4LengthLineEdit_textChanged(const QString&);
        void on_Payload4HeightLineEdit_textChanged(const QString&);
//        void on_Payload4VolumeLineEdit_textChanged(const QString&);
        void on_Payload4MassLineEdit_textChanged(const QString&);
        void on_Payload4PowerConsumptionInEclipseLineEdit_textChanged(const QString&);
        void on_Payload4PowerConsumptionInDaylightLineEdit_textChanged(const QString&);
        void on_Payload4PowerOnTimeEclipseLineEdit_textChanged(const QString&);
        void on_Payload4PowerOnTimeDaylightLineEdit_textChanged(const QString&);
        void on_Payload4MaxTempLimitLineEdit_textChanged(const QString&);
        void on_Payload4MinTempLimitLineEdit_textChanged(const QString&);
        void on_Payload4DataRateLineEdit_textChanged(const QString&);
//        void on_groupBox_2_toggled(bool);

        void on_MissionStartDateTimeEdit_dateTimeChanged ( const QDateTime & datetime);
        void on_MissionEndDateTimeEdit_dateTimeChanged ( const QDateTime & datetime);

        void on_SemiMajorAxisLineEdit_textChanged(const QString&);
        void on_EccentricityLineEdit_textChanged(const QString&);
        void on_InclinationLineEdit_textChanged(const QString&);
        void on_ArgumentOfPerigeeLineEdit_textChanged(const QString&);
        void on_RAANLineEdit_textChanged(const QString&);
//        void on_StructureBox_2_toggled(bool);
//        void on_SCWidthLineEdit_2_textChanged(const QString&);
//        void on_SCLengthLineEdit_2_textChanged(const QString&);
//        void on_SCHeightLineEdit_2_textChanged(const QString&);
//        void on_SCMassLineEdit_2_textChanged(const QString&);
//        void on_SCLateralFreqLineEdit_2_textChanged(const QString&);
//        void on_SCAxialFreqLineEdit_2_textChanged(const QString&);
        void on_StructureDetailsButton_clicked();
//        void on_groupBox_12_toggled(bool);
//        void on_TotalPayloadDataRateLineEdit_2_textChanged(const QString&);
//        void on_MemorySizeLineEdit_2_textChanged(const QString&);
//        void on_TXPowerLineEdit_2_textChanged(const QString&);
//        void on_TotalLinkDurationLineEdit_2_textChanged(const QString&);
        void on_DataCommDetailsButton_clicked();
//        void on_groupBox_13_toggled(bool);
//        void on_groupBox_14_toggled(bool);
//        void on_HeaterPowerLineEdit_2_textChanged(const QString&);
//        void on_RadiatorLineEdit_2_textChanged(const QString&);
        void on_ThermalSubsystemGraphComboBox_2_activated(const QString&);
        void on_ThermalGraphPushButton_2_clicked();
        void on_ThermalDetailsButton_clicked();
//        void on_groupBox_15_toggled(bool);
//        void on_PowerConsumptionInEclipseLineEdit_2_textChanged(const QString&);
//        void on_PowerConsumptionInDaylightLineEdit_2_textChanged(const QString&);
//        void on_BOLPowerLineEdit_2_textChanged(const QString&);
//        void on_EOLPowerLineEdit_2_textChanged(const QString&);
        void on_PowerGraphComboBox_2_activated(const QString&);
        void on_PowerGraphPushButton_clicked();
        void on_PowerDetailsButton_clicked();
//        void on_groupBox_16_toggled(bool);
        void on_LauncherSelectionComboBox_2_activated(const QString&);
//        void on_LauncherPayloadMassLineEdit_2_textChanged(const QString&);
//        void on_LauncherInclinationLineEdit_2_textChanged(const QString&);
//        void on_LauncherLateralFreqCapabilityLineEdit_2_textChanged(const QString&);
//        void on_LauncherAxialFreqCapabilityLineEdit_2_textChanged(const QString&);
        void on_PlanetNameComboBox_activated(const QString&);

        void on_LauncherDetailsButton_clicked();
        void on_SemMainGUICalculatePushButton_clicked();
        void on_SemMainGUISavePushButton_clicked();
        void on_SemMainGUICancelPushButton_clicked();

private:
        void RefreshSemMainGUI();
        void RefreshSemMainGUIPayload();
        void RetrieveScenarioSC();

        ScenarioSC* Vehicle;
};
#endif //SEMMAINGUI_H
