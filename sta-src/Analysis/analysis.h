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
 ------------------ Author: Ana Raposo and Guillermo Ortega  -------------------------------

 */


#ifndef analysis_H
#define analysis_H

#include "ui_analysis.h"
#include <QDialog>

class analysis : public QDialog , private Ui::analysisDialogClass
{
Q_OBJECT
public:
	analysis( QWidget * parent = 0, Qt::WindowFlags f = 0 );
	~analysis();
protected slots:
	// Classical action written by Guillermo
	void on_buttonBox_helpRequested();
	void on_buttonBox_clicked(QAbstractButton*);
	void on_buttonBox_destroyed(QObject*);
	void on_buttonBox_destroyed();
	void on_buttonBox_customContextMenuRequested(const QPoint&);
	void on_buttonBox_accepted();
	void on_buttonBox_rejected();

	/*
	// Actions written by Ana
	void on_groupBoxAnalysisFormat_toggled(bool);
	//void on_ComboBoxAnalysisFormat_activated(const QString&);
	void on_groupBoxParameters_toggled(bool);
	void on_groupBoxTimeSpecifications_toggled(bool);
	void on_AddDefaultPushButton_clicked();
	void on_AddNewPushButton_clicked();
	void on_EditTimePushButton_clicked();
	void on_DeleteTimePushButton_clicked();
	//void on_groupBoxReport_toggled(bool);
	//void on_AddParameterPushButton_clicked();
	//void on_RemoveParameterPushButton_clicked();
	void on_groupBoxPlotAxesSettings_toggled(bool);
	void on_GeneratePushButton_clicked();
	void on_TotalHelpPushButton_clicked();  // Guillermo says: Why is this required? Can you used my action?
	// void on_ClosePushButton_clicked();   // Guillermo says: not needed any more

    public slots:  // Added by Ana
	void addParameter(QTreeWidgetItem* item); // clicked
	void addParameter(); //clicked
	void removeParameter();//clicked
	void enableReportOption(int i);
       // void AddDefaultTime();

	*/


};
#endif //analysis_H

