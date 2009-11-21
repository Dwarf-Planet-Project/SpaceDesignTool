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
 ------ Copyright (C) 2008 European Space Agency (space.trajectory.analysis AT gmail.com) ----
 ------------------ Author: Annalisa Riccardi  -------------------------------------------------
 ------------------ E-mail: (nina1983@gmail.com) ----------------------------
 Modified by Tiziana Sabatini   May 2009
 */

#ifndef _REENTRY_H_
#define _REENTRY_H_

#include <QDialog>
#include "Loitering/loitering.h"
#include "ui_reentry.h"

class ScenarioTree;
class ScenarioEnvironment;
class ScenarioReEntryTrajectory;
class ScenarioProperties;
class ScenarioSimulationMode;
class ScenarioWindowMode;
class ScenarioTrajectoryPropagation;

class ReEntryDialog : public QDialog, private Ui_ReEntryDialogClass
{
   Q_OBJECT

public:
    ReEntryDialog(ScenarioTree* parent = NULL);
    ~ReEntryDialog();

    bool loadValues(ScenarioReEntryTrajectory* reentry);
    bool loadValues(ScenarioEnvironment* environment);
    bool loadValues(ScenarioProperties* vehicleproperties);
    bool loadValues(ScenarioSimulationMode* simulationmode);
    bool loadValues(ScenarioWindowMode* windowmode);
    bool loadValues(ScenarioTrajectoryPropagation* propagation);

    bool saveValues(ScenarioReEntryTrajectory* reentry);
    bool saveValues(ScenarioEnvironment* environment);
    bool saveValues(ScenarioProperties* vehicleproperties);
    bool saveValues(ScenarioSimulationMode* simulationmode);
    bool saveValues(ScenarioWindowMode* windowmode);
    bool saveValues(ScenarioTrajectoryPropagation* propagation);
    TesseralBox* TesseralSpinBox;
    int m_tesserals;

};

#endif // _REENTRY_H_


