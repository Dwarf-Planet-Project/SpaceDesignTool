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
 --- Copyright (C) 2008 European Space Agency (space.trajectory.analysis AT gmail.com) ---
 ------------------ Author:Guillermo Ortega (ESA)  -------------------------------------------
 */

#ifndef _EXTERNAL_H_
#define _EXTERNAL_H_

#include <QDialog>
#include "Scenario/scenario.h"

#include "ui_external.h"

QT_BEGIN_NAMESPACE
class QMimeData;
QT_END_NAMESPACE

class ScenarioTree;
class ScenarioSimulationParameters;
class ScenarioTrajectoryPropagation;

class ExternalDropArea;

class ExternalDialog : public QDialog, private Ui_ExternalDialogClass
{
    Q_OBJECT

public:
    ExternalDialog(ScenarioTree* parent=NULL);
    ~ExternalDialog();

#if OLDSCENARIO
    bool loadValues(ScenarioExternalTrajectory* externalTrajectory);
    bool saveValues(ScenarioExternalTrajectory* externalTrajectory);
#endif

public slots:
    void on_ExternalFile_dropped(const QMimeData *mimeData);

protected slots:
    void on_LoadExternalpushButton_clicked();

private:
    ExternalDropArea *ExternaldropArea;

};




// Class to allow drag and drop into a small square a TLE file
class ExternalDropArea : public QLabel
{
    Q_OBJECT

public:
    ExternalDropArea(QWidget *parent = 0);

public slots:
    void clear();

signals:
    void changed(const QMimeData *mimeData = 0);

protected:
    void dragEnterEvent(QDragEnterEvent *event);
    void dragMoveEvent(QDragMoveEvent *event);
    void dragLeaveEvent(QDragLeaveEvent *event);
    void dropEvent(QDropEvent *event);
};

#endif // _EXTERNAL_H_
