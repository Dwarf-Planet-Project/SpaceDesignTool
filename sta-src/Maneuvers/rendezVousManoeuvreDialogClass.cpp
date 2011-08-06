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
------------------ Author: Guillermo Ortega August 2011 -------------------------------------------
*/

#include "rendezVousManoeuvreDialogClass.h"

rendezVousManoeuvreDialogClass::rendezVousManoeuvreDialogClass( QWidget * parent, Qt::WindowFlags f) : QDialog(parent,f)
{
	setupUi(this);
}

rendezVousManoeuvreDialogClass::~rendezVousManoeuvreDialogClass()
{
}



bool rendezVousManoeuvreDialogClass::loadValues(ScenarioRendezVousManoeuvreType* rendezvous)
{

}

bool rendezVousManoeuvreDialogClass::saveValues(ScenarioRendezVousManoeuvreType* rendezvous)
{

}






void rendezVousManoeuvreDialogClass::on_AddRendezVousManoeuvrePushButton_clicked()
{
	qWarning("TODO: %s	%d",__FILE__,__LINE__);
}

void rendezVousManoeuvreDialogClass::on_RemoveRendezVousManoeuvrePushButton_clicked()
{
	qWarning("TODO: %s	%d",__FILE__,__LINE__);
}

void rendezVousManoeuvreDialogClass::on_buttonBoxRendezVousManoeuvreDialog_helpRequested()
{
	qWarning("TODO: %s	%d",__FILE__,__LINE__);
}

void rendezVousManoeuvreDialogClass::on_buttonBoxRendezVousManoeuvreDialog_clicked(QAbstractButton*)
{
	qWarning("TODO: %s	%d",__FILE__,__LINE__);
}

void rendezVousManoeuvreDialogClass::on_treeWidgetRendezVousManoeuvreToChoose_itemDoubleClicked(QTreeWidgetItem*,int)
{
	qWarning("TODO: %s	%d",__FILE__,__LINE__);
}

void rendezVousManoeuvreDialogClass::on_treeWidgetRendezVousManoeuvreToChoose_itemClicked(QTreeWidgetItem*,int)
{
	qWarning("TODO: %s	%d",__FILE__,__LINE__);
}

void rendezVousManoeuvreDialogClass::on_treeWidgetRendezVousManoeuvreSelected_itemClicked(QTreeWidgetItem*,int)
{
	qWarning("TODO: %s	%d",__FILE__,__LINE__);
}

void rendezVousManoeuvreDialogClass::on_treeWidgetRendezVousManoeuvreSelected_itemDoubleClicked(QTreeWidgetItem*,int)
{
	qWarning("TODO: %s	%d",__FILE__,__LINE__);
}

