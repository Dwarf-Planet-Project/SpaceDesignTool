
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
 ------------------ Author: Valentino Zuccarelli  -------------------------------------------------
 ------------------ E-mail: (Valentino.Zuccarelli@gmail.com) ----------------------------
 */

#ifndef LAGRANGIAN_H
#define LAGRANGIAN_H

#include <QDialog>
#include "ui_lagrangian.h"

class ScenarioTree;
class ScenarioLagrangianTrajectory;
class ScenarioThreebodyenvironment;
class ScenarioSimulationParameters;
class ScenarioHalo;
class ScenarioThreebodyTransfer;

class LagrangianDialog : public QDialog, private Ui_LagrangianDialogClass
{
    Q_OBJECT

public:
    LagrangianDialog(ScenarioTree* parent=NULL);
    ~LagrangianDialog();

    bool loadValues(ScenarioLagrangianTrajectory* lagrangian);
    bool loadValues(ScenarioThreebodyenvironment* threebodyenvironment);
    bool loadValues(ScenarioSimulationParameters* parameters);
    //bool loadValues(ScenarioHalo* halo);
    //bool loadValues(ScenarioThreebodyTransfer* threebodyTransfer);
    bool saveValues(ScenarioLagrangianTrajectory* lagrangian);
    bool saveValues(ScenarioThreebodyenvironment* threebodyenvironment);
    bool saveValues(ScenarioSimulationParameters* parameters);
    //bool saveValues(ScenarioHalo* halo);
    //bool saveValues(ScenarioThreebodyTransfer* ThreebodyTransfer);

public slots:

    void threebody_system();
    void update_coordinate_systems();
    void update_units();
    void lagrangian_advanced();

};

#endif // LAGRANGIAN_H

