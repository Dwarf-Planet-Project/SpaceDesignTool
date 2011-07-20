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
 ------------------ Author: J. Alonso     -------------------------------------------------
 ------------------ ETSIA                 ----------------------------
 Comments:
 Add drop capability to manoeuvres tables.
 */
#ifndef _RENDEZVOUSSCHEDULEDTABLEMANOEUVRES_H_
#define _RENDEZVOUSSCHEDULEDTABLEMANOEUVRES_H_

#include <QtGui>
#include <QTableWidget>


#include "rendezVousManoeuvres.h"

class ScheduledTableManoeuvres : public QTableWidget{
    Q_OBJECT

public:
    QList<RVManoeuvreDescriptor> manoeuvreList;
    ScheduledTableManoeuvres(QWidget *parent);
    ~ScheduledTableManoeuvres();

    void updateManoeuvreList();
    void updateTable(QList <RVManoeuvreDescriptor>);

private:
    void cellDoubleClicked(int,int);
    void dragEnterEvent(QDragEnterEvent *event);
    void dropEvent(QDropEvent *event);

    void mousePressEvent(QMouseEvent *event);



};

#endif // _RENDEZVOUSSCHEDULEDTABLEMANOEUVRES_H_
