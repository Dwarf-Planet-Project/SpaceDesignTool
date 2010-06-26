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

#ifndef _LOITERINGTLE_H_
#define _LOITERINGTLE_H_


#include "ui_loiteringTLE.h"

#include "Scenario/scenario.h"
#include "Scenario/propagationfeedback.h"
#include "Astro-Core/statevector.h"

#include <QDialog>


QT_BEGIN_NAMESPACE
class QMimeData;
QT_END_NAMESPACE

class ScenarioTree;
class ScenarioSimulationParameters;
class ScenarioTrajectoryPropagation;

class DropArea;

class LoiteringTLEDialog : public QDialog, private Ui_LoiteringTLEDialogClass
{
    Q_OBJECT

public:
    LoiteringTLEDialog(ScenarioTree* parent=NULL);
    ~LoiteringTLEDialog();

    bool loadValues(ScenarioLoiteringTLEType* loiteringTLE);
    bool saveValues(ScenarioLoiteringTLEType* loiteringTLE);

public slots:
    void on_TLE_dropped(const QMimeData *mimeData);

protected slots:
    void on_LoadTLEpushButton_clicked();

private:
    DropArea *dropArea;

};




// Class to allow drag and drop into a small square a TLE file
class DropArea : public QLabel
{
    Q_OBJECT

public:
    DropArea(QWidget *parent = 0);

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
PropagateLoiteringTLETrajectory(ScenarioLoiteringTLEType* loiteringTLE,
			     QList<double>& sampleTimes,
			     QList<sta::StateVector>& samples,
			     PropagationFeedback& propFeedback);


#endif // _LOITERINGTLE_H_
