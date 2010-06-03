/*
 This program is free software; you can redistribute it and/or modify it under
 the terms of the GNU Lesser General Public License as published by the Free Software
 Foundation; either version 2 of the License, or (at your option) any later
 version.

 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more
 details.

 You should have received a copy of the GNU Lesser General Public License along with
 this program; if not, write to the Free Software Foundation, Inc., 59 Temple
 Place - Suite 330, Boston, MA 02111-1307, USA.
 Further information about the GNU Lesser General Public License can also be found on
 the world wide web at http://www.gnu.org.
 */

/*
 ------ Copyright (C) 2009 European Space Agency (space.trajectory.analysis AT gmail.com) ----
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
