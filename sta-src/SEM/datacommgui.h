#ifndef DATACOMMGUI_H
#define DATACOMMGUI_H

#include "ui_DataCommGUI.h"
#include <QDialog>

#include "SemMain.h"

class DataCommGUI : public QDialog , private Ui::DataComm
{
Q_OBJECT
public:
        DataCommGUI(SemMain * SC, QWidget * parent = 0, Qt::WindowFlags f = 0 );
	~DataCommGUI();
protected slots:
//	void on_groupBox_toggled(bool);
//	void on_DataHandlingPayload1NameLineEdit_textChanged(const QString&);
//	void on_DataHandlingPayload1DataRateLineEdit_textChanged(const QString&);
//	void on_DataHandlingPayload1PowerOnPerOrbitLineEdit_textChanged(const QString&);
//	void on_DataHandlingPayload2NameLineEdit_textChanged(const QString&);
//	void on_DataHandlingPayload2DataRateLineEdit_textChanged(const QString&);
//	void on_DataHandlingPayload2PowerOnPerOrbitLineEdit_textChanged(const QString&);
//	void on_DataHandlingPayload3NameLineEdit_textChanged(const QString&);
//	void on_DataHandlingPayload3DataRateLineEdit_textChanged(const QString&);
//	void on_DataHandlingPayload3PowerOnPerOrbitLineEdit_textChanged(const QString&);
//	void on_DataHandlingPayload4NameLineEdit_textChanged(const QString&);
//	void on_DataHandlingPayload4DataRateLineEdit_textChanged(const QString&);
//	void on_DataHandlingPayload4PowerOnPerOrbitLineEdit_textChanged(const QString&);
//	void on_DataHandlingMemorySizeLineEdit_textChanged(const QString&);
        void on_CodingTypeComboBox_activated(const QString&);
        void on_NumberOfOrbitWithNoLinkLineEdit_textChanged(const QString&);

//	void on_groupBox_2_toggled(bool);
//	void on_TTCAntennaDiameterLineEdit_textChanged(const QString&);
	void on_TTC_TXPowerLineEdit_textChanged(const QString&);
        void on_TTCLinkDurationLineEdit_textEdited(const QString&);
        void on_TTCLinkDataRateLineEdit_textEdited(const QString&);

        void on_TTCAntennaEfficiencyLineEdit_textChanged(const QString&);
        void on_TTCAntennaGainLineEdit_textChanged(const QString&);
        void on_TTCAntennaFrequencyLineEdit_textChanged(const QString&);

	void on_DataCommCalculatePushButton_clicked();
	void on_DataCommSavePushButton_clicked();
	void on_DataCommClosePushButton_clicked();
	
private:
        SemMain * SCVirtual;
};
#endif //DATACOMMGUI_H
