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
 ------------------ Author: Claas Grohnfeldt, Steffen Peter  -------------------------------------------------
 ------------------ E-mail: (claasgr@math.uni-bremen.de, spezisteffen@gmx.de) ----------------------------
 Patched by Guillermo June 2010 to make it compatible with Cambrian

*/



#ifndef CONSTELLATIONWIZARD_H
#define CONSTELLATIONWIZARD_H


#include "Main/scenariotree.h"
#include "Main/mainwindow.h"
#include "Scenario/scenario.h"

#include <QDialog>

#include "ui_constellationwizard.h"

class MainWindow;
//class ScenarioTree;
//class ScenarioView;

class ConstellationWizardDialog : public QDialog, private Ui_constellationWizardDialog
{
    Q_OBJECT

public:
    ConstellationWizardDialog(MainWindow* parent);
    ~ConstellationWizardDialog();
private:
    MainWindow* mainwindow;
    void accept();


public slots:
    void disableflower();
};

#endif  //CONSTELLATIONWIZARD_H
