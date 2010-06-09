
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
 ------------------ Author: Tiziana Sabatini  -------------------------------------------------
 ------------------ E-mail: (tiziana.sabatini@yahoo.it) ----------------------------
 */

#include "entrymass.h"
#include "scenariotree.h"
#include "Scenario/scenario.h"


EntryMassDialog::EntryMassDialog(ScenarioTree* parent):QDialog(parent)
{
    setupUi(this);

    QDoubleValidator* positiveDoubleValidator = new QDoubleValidator(this);
    positiveDoubleValidator->setBottom(0.0);

    dryMassLineEdit->setValidator(positiveDoubleValidator);

    bool loadValues(ScenarioREVGeometryType* geometry);
    bool saveValues(ScenarioREVGeometryType* geometry);
}


bool EntryMassDialog::loadValues(ScenarioREVWeights* weights)
{
    dryMassLineEdit->setText(QString::number(weights->totalDryMass()));
    return true;
}

bool EntryMassDialog::saveValues(ScenarioREVWeights* weights)
{
    weights->setTotalDryMass(dryMassLineEdit->text().toDouble());
    return true;
}
