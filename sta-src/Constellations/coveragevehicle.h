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

// Authors: Class and Steffen


/*
 ------------------ Author: Claas Grohnfeldt and Steffen Peter -------------------------------
 ------------------ E-mail: (claasgr@math.uni-bremen.de, stepeter@math.uni-bremen.de) --------
 */

#ifndef _COVERAGEVEHICLE_H
#define _COVERAGEVEHICLE_H

#include <QDialog>
#include "ui_coveragevehicle.h"
#include "Plotting/visualizationtoolbar.h"
#include "analysis.h"

class VisualizationToolBar;

class CoverageVehicleDialog : public QDialog, private Ui_CoverageVehicleDialogClass
{
    Q_OBJECT

public:
   CoverageVehicleDialog(VisualizationToolBar* parent = NULL, Analysis* ana = NULL);
   ~CoverageVehicleDialog();

    QButtonGroup buttongroup;

};
#endif // _COVERAGEVEHICLE_H
