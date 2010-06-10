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
 ------------------ Author: Guillermo Ortega  -------------------------------------------------
 ------------------ Author: Ozgun YILMAZ      ---------------------------------------------
 ------------------ email: ozgunus@yahoo.com  ---------------------------------------------

 */

#include "sem.h"
#include "Astro-Core/Atmosphere/AtmosphereModel.h"
#include "Astro-Core/stabody.h"
#include "math.h"
#include "Scenario/scenario.h"
#include "Astro-Core/cartesianTOorbital.h"
#include <QDebug>
#include <QComboBox>

#include "semmaingui.h"
#include "semwizard.h"

#include <QSharedPointer>
#include <QList>

using namespace std;

sem::sem(SpaceScenario* SCscenario,  QWidget * parent, Qt::WindowFlags f) : QDialog(parent,f)
{
	setupUi(this);

        scenario = SCscenario;

        foreach (QSharedPointer<ScenarioParticipantType> participant, scenario->AbstractParticipant())
        {
            // For space vehicles, we need to show in the drop down menu
            if (dynamic_cast<ScenarioSC*>(participant.data()))
            {
                ScenarioSC* vehicle = dynamic_cast<ScenarioSC*>(participant.data());
//                const QList<QSharedPointer<ScenarioAbstractTrajectoryType> >& trajectoryList
//                        = vehicle->SCMission()
//                          ->SCMission()->TrajectoryPlan()->AbstractTrajectory();
//
//                // Initial state is stored in the first trajectory (for now); so,
//                // the empty trajectory plan case has to be treated specially.
//                if (!trajectoryList.isEmpty())
                qDebug()<<"VEHICLE NAME "<<vehicle->ElementIdentifier()->Name();

                //add the sc to the combobox list
                SelectSpacecraftComboBox->addItem(vehicle->ElementIdentifier()->Name());

                //look at the existing payloads of the each SC-------------/*
                foreach (QSharedPointer<ScenarioAbstractPayloadType> Payload,
                         vehicle->SCMission()->PayloadSet()->AbstractPayload())
                {
                    Payload->Budget()->DataRate();
                }

                //look at the existing payloads of the each SC-------------*/


            }
        }
}

sem::~sem()
{
}

void sem::on_buttonBox_helpRequested()
{
	qWarning("TODO: %s	%d",__FILE__,__LINE__);
}

void sem::on_buttonBox_clicked(QAbstractButton*)
{
	qWarning("TODO: %s	%d",__FILE__,__LINE__);
}

void sem::on_buttonBox_destroyed(QObject*)
{
	qWarning("TODO: %s	%d",__FILE__,__LINE__);
}

void sem::on_buttonBox_destroyed()
{
	qWarning("TODO: %s	%d",__FILE__,__LINE__);
}

void sem::on_buttonBox_customContextMenuRequested(const QPoint&)
{
	qWarning("TODO: %s	%d",__FILE__,__LINE__);
}

void sem::on_buttonBox_accepted()
{
    // Process each participant
    foreach (QSharedPointer<ScenarioParticipantType> participant, scenario->AbstractParticipant())
    {
        // For space vehicles, we need to find which one we are interested in
        if (dynamic_cast<ScenarioSC*>(participant.data()))
        {
            ScenarioSC* vehicle = dynamic_cast<ScenarioSC*>(participant.data());

            if (SelectSpacecraftComboBox->currentText()== vehicle->ElementIdentifier()->Name())
            {
                //------------------------------------------------------------------
                if (WizardSelectionRadioButton->isChecked())
                {//Wizard is selected
                    if (SelectSpacecraftComboBox->currentIndex() != 0)
                    {
                        //open the wizard
                        SEMWizard NewWizard(vehicle, this);
                        NewWizard.exec();
                        break;
                    }
                }
                else
                {
                    if (GUISelectionRadioButton->isChecked())
                    {//GUI is selected
                        if (SelectSpacecraftComboBox->currentIndex() != 0)
                        {
                            //open the GUI
                            SemMainGUI NewDialog(vehicle, this);
                            NewDialog.exec();
                            break;
                        }
                    }
                }
                //--------------------------------------------
            }
        }
    }

	qWarning("TODO: %s	%d",__FILE__,__LINE__);
}

void sem::on_buttonBox_rejected()
{
	qWarning("TODO: %s	%d",__FILE__,__LINE__);
}

