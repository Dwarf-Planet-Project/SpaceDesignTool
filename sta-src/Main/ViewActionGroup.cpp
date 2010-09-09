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

#include "ViewActionGroup.h"
#include <QSettings>


/** ViewActionGroup contains all actions that control the appearance of the 2D, 2.5D, and 3D
  * views. The actions are aggregated here so that all settings can be kept in sync. Menus and
  * toolbars to control view settings should be constructed using the actions from the
  * same ViewActionGroup (created once, in MainWindow.)
  */
ViewActionGroup::ViewActionGroup() :
    m_ambientLight(0.0f),
    m_currentSkyLayerIndex(0)
{
    m_shadowsAction = new QAction(tr("Shadows"), this);
    m_shadowsAction->setCheckable(true);
    m_atmospheresAction = new QAction(tr("Atmospheres"), this);
    m_atmospheresAction->setCheckable(true);
    m_cloudsAction = new QAction(tr("Clouds"), this);
    m_cloudsAction->setCheckable(true);
    m_reflectionsAction = new QAction(tr("Reflections"), this);
    m_reflectionsAction->setCheckable(true);
    m_starsAction = new QAction(tr("Stars"), this);
    m_starsAction->setCheckable(true);
    m_equatorialGridAction = new QAction(tr("Equatorial Grid"), this);
    m_equatorialGridAction->setCheckable(true);
    m_reentryTrajectoriesAction = new QAction(tr("Re-entry Trajectories"), this);
    m_reentryTrajectoriesAction->setCheckable(true);
    m_satelliteTrajectoriesAction = new QAction(tr("Satellite Trajectories"), this);
    m_satelliteTrajectoriesAction->setCheckable(true);
}


ViewActionGroup::~ViewActionGroup()
{
}


/** Save all action state so that it can be restored later.
  */
void
ViewActionGroup::saveSettings()
{
    QSettings settings;
    settings.beginGroup("ViewSettings");

    settings.setValue("Shadows", m_shadowsAction->isChecked());
    settings.setValue("Atmospheres", m_atmospheresAction->isChecked());
    settings.setValue("Clouds", m_cloudsAction->isChecked());
    settings.setValue("Reflections", m_reflectionsAction->isChecked());
    settings.setValue("Stars", m_starsAction->isChecked());
    settings.setValue("EquatorialGrid", m_equatorialGridAction->isChecked());
    settings.setValue("ReentryTrajectories", m_reentryTrajectoriesAction->isChecked());
    settings.setValue("SatelliteTrajectories", m_satelliteTrajectoriesAction->isChecked());
    settings.setValue("AmbientLight", m_ambientLight);
    settings.setValue("SkyLayer", m_currentSkyLayerIndex);
    settings.endGroup();
}


/** Restore saved action state.
  */
void
ViewActionGroup::restoreSettings()
{
    QSettings settings;
    settings.beginGroup("ViewSettings");

    m_shadowsAction->setChecked(settings.value("Shadows", false).toBool());
    m_atmospheresAction->setChecked(settings.value("Atmospheres", false).toBool());
    m_cloudsAction->setChecked(settings.value("Clouds", false).toBool());
    m_reflectionsAction->setChecked(settings.value("Reflections", false).toBool());
    m_starsAction->setChecked(settings.value("Stars", true).toBool());
    m_equatorialGridAction->setChecked(settings.value("EquatorialGrid", false).toBool());
    m_reentryTrajectoriesAction->setChecked(settings.value("ReentryTrajectories", true).toBool());
    m_satelliteTrajectoriesAction->setChecked(settings.value("SatelliteTrajectories", true).toBool());
    setAmbientLight(settings.value("AmbientLight", 0.2f).toFloat());
    setSkyLayer(settings.value("SkyLayer", 0).toInt());

    settings.endGroup();
}


void
ViewActionGroup::setAmbientLight(float lightLevel)
{
    if (lightLevel != m_ambientLight)
    {
        m_ambientLight = lightLevel;
        emit ambientLightChanged(m_ambientLight);
    }
}


void
ViewActionGroup::setSkyLayer(int layerIndex)
{
    if (layerIndex != m_currentSkyLayerIndex)
    {
        m_currentSkyLayerIndex = layerIndex;
        emit skyLayerChanged(m_currentSkyLayerIndex);
    }
}
