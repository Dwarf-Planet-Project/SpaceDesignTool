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

#ifndef VIEW_ACTION_GROUP_H_
#define VIEW_ACTION_GROUP_H_

#include <QAction>

class ViewActionGroup : public QObject
{
Q_OBJECT
public:
    ViewActionGroup();
    ~ViewActionGroup();

    void restoreSettings();
    void saveSettings();

public:
    QAction* shadowsAction() const
    {
        return m_shadowsAction;
    }

    QAction* atmospheresAction() const
    {
        return m_atmospheresAction;
    }

    QAction* cloudsAction() const
    {
        return m_cloudsAction;
    }

    QAction* reflectionsAction() const
    {
        return m_reflectionsAction;
    }

    QAction* starsAction() const
    {
        return m_starsAction;
    }

    QAction* equatorialGridAction() const
    {
        return m_equatorialGridAction;
    }

    QAction* reentryTrajectoriesAction() const
    {
        return m_reentryTrajectoriesAction;
    }

    QAction* satelliteTrajectoriesAction() const
    {
        return m_satelliteTrajectoriesAction;
    }

    QAction* planetOrbitsAction() const
    {
        return m_planetOrbitsAction;
    }

    QAction* moonOrbitsAction() const
    {
        return m_moonOrbitsAction;
    }

    float ambientLight() const
    {
        return m_ambientLight;
    }

    int currentSkyLayer() const
    {
        return m_currentSkyLayerIndex;
    }

public slots:
    void setAmbientLight(float lightLevel);
    void setSkyLayer(int layerIndex);

signals:
    void ambientLightChanged(float lightLevel);
    void skyLayerChanged(int layerIndex);

private:
    QAction* m_shadowsAction;
    QAction* m_atmospheresAction;
    QAction* m_cloudsAction;
    QAction* m_reflectionsAction;
    QAction* m_starsAction;
    QAction* m_equatorialGridAction;
    QAction* m_reentryTrajectoriesAction;
    QAction* m_satelliteTrajectoriesAction;
    QAction* m_planetOrbitsAction;
    QAction* m_moonOrbitsAction;
    float m_ambientLight;
    int m_currentSkyLayerIndex;
};
#endif // VIEW_ACTION_GROUP_H_
