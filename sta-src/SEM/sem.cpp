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
 ------------------ Author: Guillermo Ortega  -------------------------------------------------

 */

#include "sem.h"
#include "Astro-Core/Atmosphere/AtmosphereModel.h"
#include "Astro-Core/stabody.h"
#include "math.h"
#include "Scenario/scenario.h"
#include "Astro-Core/cartesianTOorbital.h"
#include <QDebug>
#include <QComboBox>


using namespace std;

sem::sem( QWidget * parent, Qt::WindowFlags f) : QDialog(parent,f)
{
	setupUi(this);
}

sem::~sem()
{
}

void sem::on_buttonBox_helpRequested()
{
	qWarning("TODO: %s	%d",__FILE__,__LINE__);
}

void sem::on_buttonBox_clicked(QAbstractButton*)
{
	qWarning("TODO: %s	%d",__FILE__,__LINE__);
}

void sem::on_buttonBox_destroyed(QObject*)
{
	qWarning("TODO: %s	%d",__FILE__,__LINE__);
}

void sem::on_buttonBox_destroyed()
{
	qWarning("TODO: %s	%d",__FILE__,__LINE__);
}

void sem::on_buttonBox_customContextMenuRequested(const QPoint&)
{
	qWarning("TODO: %s	%d",__FILE__,__LINE__);
}

void sem::on_buttonBox_accepted()
{
	qWarning("TODO: %s	%d",__FILE__,__LINE__);
}

void sem::on_buttonBox_rejected()
{
	qWarning("TODO: %s	%d",__FILE__,__LINE__);
}

