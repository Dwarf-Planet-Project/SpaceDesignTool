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
 ------------------ Author: Chris Laurel  -------------------------------------------------
 ------------------ E-mail: (claurel@gmail.com) ----------------------------
 Modified by Guillermo on October 2009 to clean up the interface
 */

#include "locationeditor.h"
#include "Scenario/scenario.h"


/* Dialog box for editing location parameters: central body,
 * latitude, longitude, and altitude.
 */
LocationEditorDialog::LocationEditorDialog(QWidget* parent) :
    QDialog(parent)
{
    setupUi(this);

    // Set up the input validators
    QDoubleValidator* doubleValidator = new QDoubleValidator(this);
    QDoubleValidator* angleValidator = new QDoubleValidator(this);
    angleValidator->setBottom(-360.0);
    angleValidator->setTop(360.0);
    
    latitudeEdit->setValidator(angleValidator);
    longitudeEdit->setValidator(angleValidator);
    altitudeEdit->setValidator(doubleValidator);
}


LocationEditorDialog::~LocationEditorDialog()
{
}


// Load the edit box values from the current node values
bool LocationEditorDialog::loadValues(const ScenarioLocationType* location)
{
#if OLDSCENARIO
    latitudeEdit->setText(QString::number(location->getGroundPosition().latitude));
    longitudeEdit->setText(QString::number(location->getGroundPosition().longitude));
    altitudeEdit->setText(QString::number(location->getGroundPosition().altitude));
#endif

    return true;
}


// Save edit box values to a tree of DOM nodes
bool LocationEditorDialog::saveValues(ScenarioLocationType* location)
{
#if OLDSCENARIO
    GroundPosition pos;
    pos.latitude         = latitudeEdit->text().toDouble();
    pos.longitude        = longitudeEdit->text().toDouble();
    pos.altitude         = altitudeEdit->text().toDouble();
    location->setGroundPosition(pos);
#endif

    return true;
}
