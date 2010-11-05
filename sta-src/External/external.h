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
class PropagationFeedback;

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


extern bool
PropagateExternalTrajectory(ScenarioExternalType* extTrajectory,
                            const QString& ephemerisSearchDirectory,
                            QList<double>& sampleTimes,
                            QList<sta::StateVector>& samples,
                            PropagationFeedback& propFeedback);


#endif // _EXTERNAL_H_
