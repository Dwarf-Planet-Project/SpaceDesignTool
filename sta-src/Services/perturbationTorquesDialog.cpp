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
 ------ Copyright (C) 2011 STA Steering Board (space.trajectory.analysis AT gmail.com) ----
*/
/*
------------------ Author: Guillermo Ortega  -------------------------------------------
 */

#include "Services/perturbationTorquesDialog.h"

#include "QDebug"

perturbationTorquesDialog::perturbationTorquesDialog( QWidget * parent, Qt::WindowFlags f) : QDialog(parent,f)
{
    setupUi(this);
}

perturbationTorquesDialog::~perturbationTorquesDialog()
{
}



bool perturbationTorquesDialog::loadValues(const ScenarioParticipantType* myParticipant, const ScenarioPerturbationsTorqueType* perturbationsToParticipant)
{
    DebrisRadioButton->setChecked(false);

    if (perturbationsToParticipant == NULL)
    {
        return true;
    }

    bool theDebris = perturbationsToParticipant->micrometeoroids();

    DebrisRadioButton->setChecked(theDebris);

    return true;
}



bool perturbationTorquesDialog::saveValues(ScenarioParticipantType* myParticipant, ScenarioPerturbationsTorqueType* perturbationsToParticipant)
{
    if (DebrisRadioButton->isChecked())
        perturbationsToParticipant->setMicrometeoroids(true);
    else
        perturbationsToParticipant->setMicrometeoroids(false);

    return true;
}
