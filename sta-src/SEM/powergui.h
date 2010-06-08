#ifndef POWERGUI_H
#define POWERGUI_H

#include "ui_PowerGUI.h"
#include <QDialog>

#include "SemMain.h"

class PowerGUI : public QDialog , private Ui::PowerGUI
{
Q_OBJECT
public:
        PowerGUI( SemMain * SC, QWidget * parent = 0, Qt::WindowFlags f = 0 );
	~PowerGUI();
protected slots:
        //void on_PowerGUIPayload1NameLineEdit_textChanged(const QString&);
        //void on_PowerGUIPayload1PowerLineEdit_textChanged(const QString&);
	void on_PowerGUIPayload1PowerOnEclipseLineEdit_textChanged(const QString&);
	void on_PowerGUIPayload1PowerOnDaylightLineEdit_textChanged(const QString&);
        //void on_PowerGUIPayload2NameLineEdit_textChanged(const QString&);
        //void on_PowerGUIPayload2PowerLineEdit_textChanged(const QString&);
	void on_PowerGUIPayload2PowerOnEclipseLineEdit_textChanged(const QString&);
	void on_PowerGUIPayload2PowerOnDaylightLineEdit_textChanged(const QString&);
        //void on_PowerGUIPayload3NameLineEdit_textChanged(const QString&);
        //void on_PowerGUIPayload3PowerLineEdit_textChanged(const QString&);
	void on_PowerGUIPayload3PowerOnEclipseLineEdit_textChanged(const QString&);
	void on_PowerGUIPayload3PowerOnDaylightLineEdit_textChanged(const QString&);
        //void on_PowerGUIPayload4NameLineEdit_textChanged(const QString&);
        //void on_PowerGUIPayload4PowerLineEdit_textChanged(const QString&);
	void on_PowerGUIPayload4PowerOnEclipseLineEdit_textChanged(const QString&);
	void on_PowerGUIPayload4PowerOnDaylightLineEdit_textChanged(const QString&);
        //void on_PowerGUITotalPowerInEclipseLineEdit_textChanged(const QString&);
	void on_PowerGUISolarCellTypeComboBox_activated(const QString&);
        //void on_PowerGUISolarArrayAreaLineEdit_textChanged(const QString&);
        void on_PowerGUIBatteryTypeComboBox_activated(const QString&);
        //void on_PowerGUIBatteryNumberLineEdit_textChanged(const QString&);
        //void on_PowerGUITotalPowerInDaylightLineEdit_textChanged(const QString&);
        //void on_PowerGUIBOLPowerLineEdit_textChanged(const QString&);
        //void on_PowerGUIEOLPowerLineEdit_textChanged(const QString&);
	void on_PowerGUISavePushButton_clicked();

private:
        SemMain * SCVirtual;
};
#endif //POWERGUI_H
