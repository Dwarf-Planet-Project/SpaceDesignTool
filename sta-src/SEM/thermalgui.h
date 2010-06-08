#ifndef THERMALGUI_H
#define THERMALGUI_H

#include "ui_ThermalGUI.h"
#include <QDialog>

#include "SemMain.h"

class ThermalGUI : public QDialog , private Ui::ThermalGUI
{
Q_OBJECT
public:
        ThermalGUI(SemMain * SC, QWidget * parent = 0, Qt::WindowFlags f = 0 );
	~ThermalGUI();
protected slots:
//	void on_ThermalGUISCMinTemperatureLineEdit_textChanged(const QString&);
//	void on_ThermalGUISCMaxTemperatureLineEdit_textChanged(const QString&);
//	void on_ThermalGUIPayload1NameLineEdit_textChanged(const QString&);
//	void on_ThermalGUIPayload1MinTempLineEdit_textChanged(const QString&);
//	void on_ThermalGUIPayload1MaxTempLineEdit_textChanged(const QString&);
//	void on_ThermalGUIPayload2NameLineEdit_textChanged(const QString&);
//	void on_ThermalGUIPayload2MinTempLineEdit_textChanged(const QString&);
//	void on_ThermalGUIPayload2MaxTempLineEdit_textChanged(const QString&);
//	void on_ThermalGUIPayload3NameLineEdit_textChanged(const QString&);
//	void on_ThermalGUIPayload3MinTempLineEdit_textChanged(const QString&);
//	void on_ThermalGUIPayload3MaxTempLineEdit_textChanged(const QString&);
//	void on_ThermalGUIPayload4NameLineEdit_textChanged(const QString&);
//	void on_ThermalGUIPayload4MinTempLineEdit_textChanged(const QString&);
//	void on_ThermalGUIPayload4MaxTempLineEdit_textChanged(const QString&);
	void on_ThermalGUIColdCoatingComboBox_activated(const QString&);
        void on_ThermalGUIColdEmmissivityLineEdit_textEdited(const QString&);
        void on_ThermalGUIColdAbsorptivityLineEdit_textEdited(const QString&);
	void on_ThermalGUIHotCoatingComboBox_activated(const QString&);
        void on_ThermalGUIHotEmmissivityLineEdit_textEdited(const QString&);
        void on_ThermalGUIHotAbsorptivityLineEdit_textEdited(const QString&);
        void on_ThermalGUIHeaterPowerLineEdit_textChanged(const QString&);
        void on_ThermalGUIRadiatorLineEdit_textChanged(const QString&);
        void on_ThermalGUICalculatePushButton_clicked();
	void on_ThermalGUISavePushButton_clicked();
	
private:
        SemMain * SCVirtual;
};
#endif //THERMALGUI_H
