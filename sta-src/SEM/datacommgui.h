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
