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
*/
/*
------------------ Author: Guillermo Ortega (ESA)  -------------------------------------------------
*/

#ifndef PREFERENCES_H
#define PREFERENCES_H

#include "ui_preferences.h"
#include <QDialog>

class preferences : public QDialog , private Ui::DialogPreferences
{
Q_OBJECT
public:
	preferences( QWidget * parent = 0, Qt::WindowFlags f = 0 );
	~preferences();
protected slots:
	void on_DialogPreferences_accepted();
	void on_DialogPreferences_rejected();
	void on_horizontalSliderAmbientLight_valueChanged(int);
	void on_shadows_clicked(bool);
	void on_atmospheres_clicked(bool);
	void on_clouds_clicked(bool);
	void on_nightLigfhts_clicked(bool);
	void on_reflections_clicked(bool);
	void on_stars_clicked(bool);
	void on_celestialGrid_clicked(bool);
	void on_fieldOfView_clicked(bool);
	void on_coordinateSystems_clicked(bool);
	void on_dataLayers_clicked(bool);
	void on_coverageAreas_clicked(bool);
	void on_satellitesTrajectories_clicked(bool);
	void on_planetsTrajectories_clicked(bool);
	void on_moonsTrajectories_clicked(bool);
	void on_cometsTrajectories_clicked(bool);
	void on_asteroidsTrajectories_clicked(bool);
	void on_reentryTrajectories_clicked(bool);
	void on_spaceVehicleLabels_clicked(bool);
	void on_groundStationsLabels_clicked(bool);
	void on_launchPadLabels_clicked(bool);
	void on_pointsLabels_clicked(bool);
	void on_planetsLabels_clicked(bool);
	void on_moonsLabels_clicked(bool);
	void on_cometsLabels_clicked(bool);
	void on_asteroidsLabels_clicked(bool);
	void on_dateAndTime_clicked(bool);
	void on_altitude_clicked(bool);
	void on_centralBody_clicked();
	
};
#endif //PREFERENCES_H
