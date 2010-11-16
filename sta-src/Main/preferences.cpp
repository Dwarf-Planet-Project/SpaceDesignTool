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
------------------ Author: Guillermo Ortega (ESA)  -------------------------------------------------
*/



#include "preferences.h"
#include "ViewActionGroup.h"
#include "Visualization/ThreeDView.h"


Preferences::Preferences( QWidget * parent, Qt::WindowFlags f) : QDialog(parent,f)
{
	setupUi(this);
}

Preferences::~Preferences()
{
}


// Connect widgets to the view actions
void Preferences::connectViewActions(const ViewActionGroup* viewActions)
{
    // Synchronize state from the view actions group
    shadows->setChecked(viewActions->shadowsAction()->isChecked());
    atmospheres->setChecked(viewActions->atmospheresAction()->isChecked());
    clouds->setChecked(viewActions->cloudsAction()->isChecked());
    reflections->setChecked(viewActions->reflectionsAction()->isChecked());
    stars->setChecked(viewActions->starsAction()->isChecked());
    celestialGrid->setChecked(viewActions->equatorialGridAction()->isChecked());
    satellitesTrajectories->setChecked(viewActions->satelliteTrajectoriesAction()->isChecked());
    reentryTrajectories->setChecked(viewActions->reentryTrajectoriesAction()->isChecked());
    planetsTrajectories->setChecked(viewActions->planetOrbitsAction()->isChecked());
    moonsTrajectories->setChecked(viewActions->moonOrbitsAction()->isChecked());
    planetsLabels->setChecked(viewActions->planetLabelsAction()->isChecked());

    // Connect dialog widgets to view actions
    // Note that we connect from the clicked() signal of the checkbox widgets. This prevents circular
    // connection, since the clicked() signal is only emitted when the user clicks the checkbox, not
    // whenever the state changes.
    connect(shadows,                SIGNAL(clicked(bool)), viewActions->shadowsAction(),               SLOT(setChecked(bool)));
    connect(atmospheres,            SIGNAL(clicked(bool)), viewActions->atmospheresAction(),           SLOT(setChecked(bool)));
    connect(clouds,                 SIGNAL(clicked(bool)), viewActions->cloudsAction(),                SLOT(setChecked(bool)));
    connect(reflections,            SIGNAL(clicked(bool)), viewActions->reflectionsAction(),           SLOT(setChecked(bool)));
    connect(stars,                  SIGNAL(clicked(bool)), viewActions->starsAction(),                 SLOT(setChecked(bool)));
    connect(celestialGrid,          SIGNAL(clicked(bool)), viewActions->equatorialGridAction(),        SLOT(setChecked(bool)));
    connect(satellitesTrajectories, SIGNAL(clicked(bool)), viewActions->satelliteTrajectoriesAction(), SLOT(setChecked(bool)));
    connect(reentryTrajectories,    SIGNAL(clicked(bool)), viewActions->reentryTrajectoriesAction(),   SLOT(setChecked(bool)));
    connect(planetsTrajectories,    SIGNAL(clicked(bool)), viewActions->planetOrbitsAction(),          SLOT(setChecked(bool)));
    connect(moonsTrajectories,      SIGNAL(clicked(bool)), viewActions->moonOrbitsAction(),            SLOT(setChecked(bool)));
    connect(planetsLabels,          SIGNAL(clicked(bool)), viewActions->planetLabelsAction(),          SLOT(setChecked(bool)));
    connect(skyLayerCombo,          SIGNAL(currentIndexChanged(int)), viewActions,                     SLOT(setSkyLayer(int)));
    connect(comboBox3DVision,       SIGNAL(currentIndexChanged(int)), viewActions,                     SLOT(setStereoMode(int)));  // Guillermo
    connect(this, SIGNAL(ambientLightChanged(float)), viewActions, SLOT(setAmbientLight(float)));

    // Connect from view actions to dialog widgets
    connect(viewActions->shadowsAction(),               SIGNAL(toggled(bool)), shadows,                SLOT(setChecked(bool)));
    connect(viewActions->atmospheresAction(),           SIGNAL(toggled(bool)), atmospheres,            SLOT(setChecked(bool)));
    connect(viewActions->cloudsAction(),                SIGNAL(toggled(bool)), clouds,                 SLOT(setChecked(bool)));
    connect(viewActions->reflectionsAction(),           SIGNAL(toggled(bool)), reflections,            SLOT(setChecked(bool)));
    connect(viewActions->starsAction(),                 SIGNAL(toggled(bool)), stars,                  SLOT(setChecked(bool)));
    connect(viewActions->equatorialGridAction(),        SIGNAL(toggled(bool)), celestialGrid,          SLOT(setChecked(bool)));
    connect(viewActions->satelliteTrajectoriesAction(), SIGNAL(toggled(bool)), satellitesTrajectories, SLOT(setChecked(bool)));
    connect(viewActions->reentryTrajectoriesAction(),   SIGNAL(toggled(bool)), reentryTrajectories,    SLOT(setChecked(bool)));
    connect(viewActions->planetOrbitsAction(),          SIGNAL(toggled(bool)), planetsTrajectories,    SLOT(setChecked(bool)));
    connect(viewActions->planetLabelsAction(),          SIGNAL(toggled(bool)), planetsLabels,          SLOT(setChecked(bool)));
    connect(viewActions->moonOrbitsAction(),            SIGNAL(toggled(bool)), moonsTrajectories,      SLOT(setChecked(bool)));
    connect(viewActions, SIGNAL(skyLayerChanged(int)),  this, SLOT(setSkyLayer(int)));
    connect(viewActions, SIGNAL(stereoModeChanged(int)), this, SLOT(setStereoMode(int))); // Guillermo
    connect(viewActions, SIGNAL(ambientLightChanged(float)), this, SLOT(setAmbientLight(float)));
}


void Preferences::on_DialogPreferences_accepted()
{
	qWarning("TODO: %s	%d",__FILE__,__LINE__);
}

void Preferences::on_DialogPreferences_rejected()
{
	qWarning("TODO: %s	%d",__FILE__,__LINE__);
}

void Preferences::on_horizontalSliderAmbientLight_valueChanged(int value)
{
    float minValue = float(horizontalSliderAmbientLight->minimum());
    float maxValue = float(horizontalSliderAmbientLight->maximum());
    float lightLevel = (float(value) - minValue) / (maxValue - minValue);

    emit ambientLightChanged(lightLevel);
}

void Preferences::setAmbientLight(float lightLevel)
{
    float minValue = float(horizontalSliderAmbientLight->minimum());
    float maxValue = float(horizontalSliderAmbientLight->maximum());
    int value = minValue + int((maxValue - minValue) * lightLevel);

    if (value != horizontalSliderAmbientLight->value())
    {
        horizontalSliderAmbientLight->setValue(value);
    }
}

void Preferences::setSkyLayer(int skyLayerIndex)
{
    if (skyLayerIndex >= 0 && skyLayerIndex < skyLayerCombo->count())
    {
        skyLayerCombo->setCurrentIndex(skyLayerIndex);
    }
}

void Preferences::setStereoMode(int stereoMode)
{
    if (stereoMode >= 0 && stereoMode < comboBox3DVision->count())
    {
        comboBox3DVision->setCurrentIndex(stereoMode);
    }
}

void Preferences::on_fieldOfView_clicked(bool)
{
	qWarning("TODO: %s	%d",__FILE__,__LINE__);
}

void Preferences::on_coordinateSystems_clicked(bool)
{
	qWarning("TODO: %s	%d",__FILE__,__LINE__);
}

void Preferences::on_dataLayers_clicked(bool)
{
	qWarning("TODO: %s	%d",__FILE__,__LINE__);
}

void Preferences::on_coverageAreas_clicked(bool)
{
	qWarning("TODO: %s	%d",__FILE__,__LINE__);
}

void Preferences::on_satellitesTrajectories_clicked(bool)
{
	qWarning("TODO: %s	%d",__FILE__,__LINE__);
}

void Preferences::on_cometsTrajectories_clicked(bool)
{
	qWarning("TODO: %s	%d",__FILE__,__LINE__);
}

void Preferences::on_asteroidsTrajectories_clicked(bool)
{
	qWarning("TODO: %s	%d",__FILE__,__LINE__);
}

void Preferences::on_reentryTrajectories_clicked(bool)
{
	qWarning("TODO: %s	%d",__FILE__,__LINE__);
}

void Preferences::on_spaceVehicleLabels_clicked(bool)
{
	qWarning("TODO: %s	%d",__FILE__,__LINE__);
}

void Preferences::on_groundStationsLabels_clicked(bool)
{
	qWarning("TODO: %s	%d",__FILE__,__LINE__);
}

void Preferences::on_launchPadLabels_clicked(bool)
{
	qWarning("TODO: %s	%d",__FILE__,__LINE__);
}

void Preferences::on_pointsLabels_clicked(bool)
{
	qWarning("TODO: %s	%d",__FILE__,__LINE__);
}

void Preferences::on_planetsLabels_clicked(bool)
{
	qWarning("TODO: %s	%d",__FILE__,__LINE__);
}

void Preferences::on_moonsLabels_clicked(bool)
{
	qWarning("TODO: %s	%d",__FILE__,__LINE__);
}

void Preferences::on_cometsLabels_clicked(bool)
{
	qWarning("TODO: %s	%d",__FILE__,__LINE__);
}

void Preferences::on_asteroidsLabels_clicked(bool)
{
	qWarning("TODO: %s	%d",__FILE__,__LINE__);
}

void Preferences::on_dateAndTime_clicked(bool)
{
	qWarning("TODO: %s	%d",__FILE__,__LINE__);
}

void Preferences::on_altitude_clicked(bool)
{
	qWarning("TODO: %s	%d",__FILE__,__LINE__);
}

void Preferences::on_centralBody_clicked()
{
	qWarning("TODO: %s	%d",__FILE__,__LINE__);
}

