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
------------------ Author: Annalisa Riccardi  -------------------------------------------------
 ------------------ E-mail: (nina1983@gmail.com) ----------------------------
 Modified by Tiziana Sabatini on July 2009 to support the new perturbations layer
 */

#ifndef _RENDEZVOUS_H_
#define _RENDEZVOUS_H_

#include <QDialog>
#include "Loitering/loitering.h"
#include "ui_rendezvous.h"

class SpaceScenario;
class ScenarioTree;
class ScenarioSpaceVehicle;
class ScenarioRendezvousTrajectory;
class ScenarioEnvironment;
class ScenarioTargettingSimulationParameters;
class ScenarioManoeuvrePlan; 
class ScenarioManoeuvre;

class RendezvousDialog : public QDialog, private Ui_RendezvousDialogClass
{
    Q_OBJECT
    
public:
    //RendezvousDialog(ScenarioTree* parent=NULL);
    RendezvousDialog(QWidget * parent = 0, Qt::WindowFlags f = 0);
    ~RendezvousDialog();

    ScenarioManoeuvre* manoeuvreForItem(QTreeWidgetItem* item) const;
    void writeInputFile();
    void reorderManoeuvrePlanTree();
    void save_changes();
    void displayManoeuvrePlan();
    bool isDragDropHappened();
    bool loadValues(ScenarioRendezvousTrajectory* rendezvous);
    bool loadValues(ScenarioEnvironment* environment);
    bool loadValues(ScenarioTargettingSimulationParameters* parameters);
    bool loadValues(ScenarioManoeuvrePlan* manoeuvreplan);
    bool saveValues(ScenarioRendezvousTrajectory* rendezvous);
    bool saveValues(ScenarioEnvironment* environment);
    bool saveValues(ScenarioTargettingSimulationParameters* parameters);
    bool saveValues(ScenarioManoeuvrePlan* manoeuvreplan);
    TesseralBox* TesseralSpinBox;
    int m_tesserals;
    
public slots:
    void addPerturbingPlanet(QListWidgetItem* item);
    void addPerturbingPlanet();
    void removePerturbingPlanet(); 
    void addManoeuvre(QListWidgetItem* manoeuvreWidget);
    bool manoeuvrePlanChanged(QTreeWidgetItem* item, int column);
    void on_AddButton_clicked();
    void on_DeleteButton_clicked();
    void on_ClearAllButton_clicked();
    void on_OKButton_clicked();
    void on_CancelButton_clicked();
    
private:
	ScenarioTree* m_scenariotree;
	ScenarioManoeuvrePlan* m_manoeuvreplan;
	ScenarioSpaceVehicle* m_chaser;
	ScenarioSpaceVehicle* m_target;
};

#endif // _RENDEZVOUS_H_

