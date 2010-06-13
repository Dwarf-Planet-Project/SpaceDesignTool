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
