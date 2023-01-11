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
------------------ Author: Chris Laurel  -------------------------------------------------
 ------------------ E-mail: (claurel@gmail.com) ----------------------------
 */

namespace sta
{
	#include "exportdialog.h"
	#include "propagatedscenario.h"
	#include "Astro-Core/date.h"
	#include <QTextStream>


	ExportDialog::ExportDialog(PropagatedScenario* scenario, QWidget* parent) :
	    QDialog(parent),
	    m_scenario(scenario)
	{
	    setupUi(this);

	    const QList<SpaceObject*>& spaceObjects = m_scenario->spaceObjects();
	    int index = 0;
	    foreach (SpaceObject* obj, spaceObjects)
	    {
		if (!obj->mission().isEmpty())
		{
		    vehicleCombo->addItem(obj->name(), index);
		}
		index++;
	    }

	    arcCombo->addItem("All");
	}


	ExportDialog::~ExportDialog()
	{
	}


	bool
	ExportDialog::exportTrajectory(QIODevice* out)
	{
	    QTextStream outStream(out);

	    int selectedIndex = vehicleCombo->itemData(vehicleCombo->currentIndex()).toInt();
	    if (selectedIndex >= 0 && selectedIndex < m_scenario->spaceObjects().size())
	    {
		SpaceObject* obj = m_scenario->spaceObjects().at(selectedIndex);
		foreach (MissionArc* arc, obj->mission())
		{
		    int stateVectorCount = arc->trajectorySampleCount();
		    for (int i = 0; i < stateVectorCount; i++)
		    {
			double mjd = arc->trajectorySampleTime(i);
			sta::StateVector state = arc->trajectorySample(i);
			outStream.setRealNumberPrecision(14);
			outStream << sta::MjdToJd(mjd) << ", ";
			outStream.setRealNumberPrecision(12);
			outStream << state.position.x() << ", " << state.position.y() << ", " << state.position.z() << ", ";
			outStream << state.velocity.x() << ", " << state.velocity.y() << ", " << state.velocity.z() << "\n";
		    }
		}
	    }

	    return true;
	}
}
