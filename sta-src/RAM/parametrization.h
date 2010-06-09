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
 ------------------ Author: Dominic Dirkx  ------------------------------------------
 ------------------ E-mail: (d.dirkxATstudent.tudelft.nl) ----------------------------
*/

#ifndef PARAMETRIZATIONDIALOG_H
#define PARAMETRIZATIONDIALOG_H

#include "ui_parametrizedgeometry.h"
#include "vehiclegeometry.h"
#include "Scenario/staschema.h"
#include <QDialog>

class ParametrizationDialog : public QDialog , private Ui::ParametrizationDialog
{
Q_OBJECT
public:
        ParametrizationDialog( QWidget * parent = 0, Qt::WindowFlags f = 0 );
	~ParametrizationDialog();
        QString getFileName();
        void getGlobalCharacteristics(double&Rn, double&Vol, double& Surf);
        int currentType;

        bool loadValues(ScenarioREVGeometryType* geometry);
        bool saveValues(ScenarioREVGeometryType* geometry);


private:
        bool verifyInput();
        VehicleGeometry * vehicle;
        double NoseRadius;
        double Surface;
        double Volume;
protected slots:

	void on_generateGeometryButton_clicked();

	
};
#endif //PARAMETRIZATIONDIALOG_H
