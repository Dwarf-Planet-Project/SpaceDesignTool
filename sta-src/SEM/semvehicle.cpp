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
 ------------------ Author: Guido Ridolfi  -------------------------------------------------
 ------------------ E-mail: (guido.ridolfi@gmail.com) --------------------------------------
 ------------------ Author: Tiziana Sabatini  -------------------------------------------------
 ------------------ E-mail: (tiziana.sabatini@yahoo.it) ----------------------------
  Modified by Guillermo on October 2009 to crate standard buttons from participants and add a close dialogue
 */

#include "semvehicle.h"
#include "Main/mainwindow.h"
#include "Scenario/scenario.h"
#include "ui_semvehicle.h"
#include <QPushButton>
#include <QButtonGroup>
#include "sem.h"
#include <QDebug>

SEMVehicleDialog::SEMVehicleDialog(MainWindow* parent) :
    QDialog(parent)

{
    setupUi(this);

    if (!parent->scenario())
    return;

    int i = 0;
    int count = 0;
    QList<ScenarioSpaceVehicle*> spacevehicles;
    buttongroup.setExclusive(true);

    foreach (ScenarioParticipant* participant, parent->scenario()->participants())
    {
        if (dynamic_cast<ScenarioSpaceVehicle*>(participant))
        {
            ScenarioSpaceVehicle* vehicle = dynamic_cast<ScenarioSpaceVehicle*>(participant);

            //So far only satellites will be accepted for the SEM analysis
            if (vehicle->type().toLower() == "satellite")
            {
                count++;
                spacevehicles.append(vehicle);
            }
        }
    }

    int mycount = 0;
    foreach (ScenarioSpaceVehicle* vehicle, spacevehicles)
    {
        const QString name = vehicle->name();
	//const QPoint point = QPoint(70,(i*40)+10);
        const QPoint point = QPoint(10,(i*35)+10);
	//const QSize size = QSize(170,40);
	const QSize size = QSize(170,30);
        QPushButton* button = new QPushButton(name,this);
        button->setObjectName(QString::fromUtf8("button"));
	button->setFixedSize(size);
        button->move(point);
        buttongroup.addButton(button, i);
        this->resize(200,(i+1)*40+10);

        QObject::connect(buttongroup.button(i),SIGNAL(clicked()),this,SLOT(close()));
        SEM* editDialog = new SEM(vehicle);
        QObject::connect(buttongroup.button(i),SIGNAL(clicked()),editDialog,SLOT(exec()));

        i++;
        mycount = i;
    }
    const QPoint closepoint = QPoint(110,(mycount*35)+10);
    const QSize closesize = QSize(70,30);
    QPushButton* buttonClose = new QPushButton("Close",this);
    buttonClose->setObjectName(QString::fromUtf8("buttonClose"));
    buttonClose->setFixedSize(closesize);
    buttonClose->move(closepoint);
    buttongroup.addButton(buttonClose, mycount);
    this->resize(190,(mycount+2)*25+30);
    QObject::connect(buttongroup.button(i),SIGNAL(clicked()),this,SLOT(close()));
}


SEMVehicleDialog::~SEMVehicleDialog()
{

}

