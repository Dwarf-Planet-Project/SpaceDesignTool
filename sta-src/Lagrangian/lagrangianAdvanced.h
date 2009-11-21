
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

 ------ Copyright (C) 2009 European Space Agency (space.trajectory.analysis AT gmail.com) ----
 ------------------ Author: Valentino Zuccarelli  -------------------------------------------------
 ------------------ E-mail: (Valentino.Zuccarelli@gmail.com) ----------------------------
 */

#ifndef LAGRANGIANADVANCED_H
#define LAGRANGIANADVANCED_H

#include <QDialog>
#include "ui_lagrangianAdvanced.h"

class ScenarioTree;
class ScenarioLagrangianTrajectory;
class ScenarioThreebodyenvironment;
class ScenarioSimulationParameters;
class ScenarioHalo;
class ScenarioThreebodyTransfer;

class LagrangianAdvancedDialog : public QDialog, private Ui_LagrangianAdvancedDialogClass
{
    Q_OBJECT

public:
    LagrangianAdvancedDialog(ScenarioTree* parent=NULL);
    ~LagrangianAdvancedDialog();

    bool loadValues(ScenarioLagrangianTrajectory* lagrangianAdvanced);
    //bool loadValues(ScenarioThreebodyenvironment* threebodyenvironment);
    //bool loadValues(ScenarioSimulationParameters* parameters);
    bool loadValues(ScenarioHalo* halo);
    bool loadValues(ScenarioThreebodyTransfer* threebodyTransfer);
    bool saveValues(ScenarioLagrangianTrajectory* lagrangianAdvanced);
    //bool saveValues(ScenarioThreebodyenvironment* threebodyenvironment);
    //bool saveValues(ScenarioSimulationParameters* parameters);
    bool saveValues(ScenarioHalo* halo);
    bool saveValues(ScenarioThreebodyTransfer* ThreebodyTransfer);

public slots:
    void research_init_cond();
    void halo_initial_conditions();
    void halo_orbit_computation();
    void manifolds_computation();
    void mode_selection();
    void halo_orbit_initial_conditions_mode3();
    void run_transfer_optimization();
    void optimization_settings();
    void run_EarthMoonTransferLeg1();
    void run_EarthMoonTransferLeg2();
    void run_EarthMoonTransferPatch();

};

#endif // LAGRANGIANADVANCED_H
