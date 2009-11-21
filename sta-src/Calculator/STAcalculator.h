/*

 This program is free software; you can redistribute it and/or modify it under
 the terms of the GNU General Public License as published by the Free Software
 Foundation; either version 2 of the License, or (at your option) any later
 version.

 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE. See the GNU General Public License for more
 details.

 You should have received a copy of the GNU General Public License along with
 this program; if not, write to the Free Software Foundation, Inc., 59 Temple
 Place - Suite 330, Boston, MA 02111-1307, USA.
 Further information about the GNU General Public License can also be found on
 the world wide web at http://www.gnu.org.
 */


/*
 --- Copyright (C) 2009 STA Steering Board (space.trajectory.analysis AT gmail.com) ---
*/


/*
------------------ Author:       Guillermo Ortega               -------------------
------------------ Affiliation:  European Space Agency          -------------------
-----------------------------------------------------------------------------------
Created October 2009

*/


#ifndef STAcalculator_H
#define STAcalculator_H

#include "ui_STAcalculator.h"
#include <QDialog>

class STAcalculator : public QDialog , private Ui::STACalculatorDialog
{
Q_OBJECT
public:
	STAcalculator( QWidget * parent = 0, Qt::WindowFlags f = 0 );
	~STAcalculator();
protected slots:
	void on_buttonBox_helpRequested();
	void on_buttonBox_clicked(QAbstractButton*);
	void on_tabWidget_currentChanged(int);
	void on_comboBoxPlanets_activated(const QString&);
	void on_AverageRadiusEdit_textChanged(const QString&);
	void on_PeriodEdit_textChanged(const QString&);
	void on_InclinationEdit_textChanged(const QString&);
	void on_DistanceSunEdit_textChanged(const QString&);
	void on_groupBox_toggled(bool);
	void on_semimajorAxisEdit_textChanged(const QString&);
	void on_eccentricityEdit_textChanged(const QString&);
	void on_inclinationEdit_textChanged(const QString&);
	void on_raanEdit_textChanged(const QString&);
	void on_argOfPeriapsisEdit_textChanged(const QString&);
	void on_trueAnomalyEdit_textChanged(const QString&);
	void on_comboBoxSemiMajor_activated(const QString&);
	void on_comboBoxTrueAnomaly_activated(const QString&);
	void on_groupBox_2_toggled(bool);
	void on_comboBoxX_activated(const QString&);
	void on_positionXEdit_textChanged(const QString&);
	void on_comboBoxY_activated(const QString&);
	void on_positionYEdit_textChanged(const QString&);
	void on_positionZEdit_textChanged(const QString&);
	void on_velocityXEdit_textChanged(const QString&);
	void on_velocityYEdit_textChanged(const QString&);
	void on_velocityZEdit_textChanged(const QString&);
	void on_comboBoxZ_activated(const QString&);
	void on_comboBoxVx_activated(const QString&);
	void on_comboBoxVy_activated(const QString&);
	void on_comboBoxVz_activated(const QString&);
	void on_dateTimeEdit_dateTimeChanged(const QDateTime&);
	void on_JDUTCEdit_textChanged(const QString&);
	void on_MJDUTCEdit_textChanged(const QString&);
	void on_JDTDBEdit_textChanged(const QString&);
	void on_MJDTDBEdit_textChanged(const QString&);
	void on_DayFractionEdit_textChanged(const QString&);
	
};
#endif //STAcalculator_H
