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

