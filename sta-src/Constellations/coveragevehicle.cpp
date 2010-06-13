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

// Authors: Class and Steffen


/*

 ------------------ Author: Claas Grohnfeldt and Steffen Peter -------------------------------
 ------------------ E-mail: (claasgr@math.uni-bremen.de, stepeter@math.uni-bremen.de) --------
 */

#include "coveragevehicle.h"
//#include "Main/mainwindow.h"
#include "Plotting/visualizationtoolbar.h"
//#include "Scenario/scenario.h"
#include "ui_coveragevehicle.h"
//#include <QCheckedButton>
#include <QPushButton>
#include <QButtonGroup>
#include <QDebug>

CoverageVehicleDialog::CoverageVehicleDialog(VisualizationToolBar* parent, Analysis* ana) :
    QDialog(parent)
{
    setupUi(this);
    buttongroup.setExclusive(true);
    for(int k=0; k<ana->m_anaSpaceObjectList.length(); k++)
    {
        const QString name = ana->m_anaSpaceObjectList.at(k).asoname;
        const QPoint point = QPoint(70,(k*40)+10);
        const QSize size = QSize(170,40);
        QPushButton* button = new QPushButton(name,this);
        //QCheckedButton* button = new QPushButton(name,this);
        button->setObjectName(QString::fromUtf8("button"));
        button->setFixedSize(size);
        button->move(point);
        buttongroup.addButton(button, k);
        this->resize(310,(k+1)*50+10);

        QObject::connect(buttongroup.button(k),SIGNAL(clicked()),this,SLOT(close()));
        //SEM* editDialog = new SEM(vehicle);
        //QObject::connect(buttongroup.button(k),SIGNAL(clicked()),editDialog,SLOT(exec()));
    }
}
CoverageVehicleDialog::~CoverageVehicleDialog()
{

}
