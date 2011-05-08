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

 */



#ifndef RENDEZVOUSDIALOG_H
#define RENDEZVOUSDIALOG_H


#include "RVXMLInterface.h"

#include "manoeuvrestreedrag.h"

#include <QDialog>
#include <QTreeWidget>
#include "ui_rendezvousDialog.h"
class MainWindow;




class rendezvousDialog : public QDialog , private Ui_rendezvousDialog
{
Q_OBJECT
public:

        rendezvousDialog(SpaceScenario*,PropagatedScenario*,MainWindow* parent);
	~rendezvousDialog();



private:

    MainWindow* mainwindow;
    RVXMLInterface interfXML;
 protected slots:
        //Manoeuvre tree drag and drop

        void propagate();
        void on_buttonBox_accepted();
	void on_buttonBox_rejected();
        void generateScenario();
        void addDeltaV();
        void addFreeDrift();


};
#endif //RENDEZVOUSDIALOG_H
