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
 ------ Copyright (C) 2009 European Space Agency (space.trajectory.analysis AT gmail.com) ----
 ------------------ Author: Chris Laurel  -------------------------------------------------
 ------------------ E-mail: (claurel@gmail.com) ----------------------------
 */

#ifndef _PLOTTING_VISUALIZATION_TOOLBAR_H_
#define _PLOTTING_VISUALIZATION_TOOLBAR_H_

#include <QToolBar>
#include <QComboBox>

class StaBody;

// The visualization toolbar contains a list of buttons used for both the ground track
// and 3D views.
class VisualizationToolBar : public QToolBar
{
    Q_OBJECT

public:
    VisualizationToolBar(const QString& title, QWidget* parent = 0);
    ~VisualizationToolBar();

private slots:
    void mapSetTickInterval();
    void mapBodyChanged(QString bodyName);

signals:
    void bodyChanged(const StaBody* body);
    void gridToggled(bool enabled);
    void terminatorToggled(bool enabled);
    void projectionChanged(bool enable25DView);
    void saveImageRequested();
    void tickIntervalChanged(double seconds);

private:
    QComboBox* m_bodySelectCombo;
    QAction* m_tickIntervalAction;
    QAction* m_gridAction;
    QAction* m_terminatorAction;
    QAction* m_saveImageAction;

    // Possibly move these 2D-specific buttons to a subclass
    QAction* m_enable2DViewAction;
    QAction* m_enable25DViewAction;
};

#endif // _PLOTTING_VISUALIZATION_TOOLBAR_H_
