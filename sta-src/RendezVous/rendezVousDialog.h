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
 ------------------ Author: Guillermo Ortega  -------------------------------------------------
 ------------------ European Space Agency ----------------------------
 ------------------ Modified: J. Alonso     -------------------------------------------------
 ------------------ ETSIA                   ----------------------------
 Comments:
 Header for dialog.
 */


#ifndef _RENDEZVOUSDIALOG_H_
#define _RENDEZVOUSDIALOG_H_

#include <QDialog>
#include <QTreeWidget>
#include "Scenario/missionAspectDialog.h"

#include "rendezVousXMLInterface.h"

#include "rendezVousManoeuvresTreeDrag.h"
#include "rendezVousScheduledManoeuvresTable.h"


#include "ui_rendezVousDialog.h"
#include "ui_missionAspectDialog.h"

class MainWindow;


class RendezVousDialog : public QDialog , private Ui_RendezVousDialogClass
{
Q_OBJECT
public:

        RendezVousDialog(SpaceScenario*,PropagatedScenario*,MainWindow* parent);
        ~RendezVousDialog();
        void setupTargetChaserInitialData(int);

        double userOrbitalInput[13];
        void updateUserOrbitalInput();

private:

    MainWindow* mainwindow;
    RVdiagXMLinterface interfXML;

protected slots:
        void generateScenario();
        void okActivated();



};
#endif //_RENDEZVOUSDIALOG_H_
