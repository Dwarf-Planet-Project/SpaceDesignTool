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
--------------------------------- Author: Guillermo Ortega  ----------------------------------
----------------------------------------------------------------------------------------------
 */

#ifndef _STA_SCENARIOPROPAGATOR_H_
#define _STA_SCENARIOPROPAGATOR_H_

#include "Scenario/staschema.h"
#include "Main/mainwindow.h"
#include "Scenario/propagationfeedback.h"

#include <QWidget>
#include <QColor>


void scenarioPropagatorSatellite(ScenarioSC* vehicle, PropagationFeedback feedback, PropagatedScenario* propScenario, QWidget* parent);
void scenarioPropagatorReEntryVehicle(ScenarioREV* vehicle, PropagationFeedback feedback, PropagatedScenario* propScenario, QWidget* parent);
void scenarioPropagatorGroundElement(ScenarioGroundStation* groundElement, PropagationFeedback feedback, PropagatedScenario* propScenario, QWidget* parent);



void scenarioPropagatorReEntryVehicle(ScenarioREV* vehicle,  QColor trajectoryColor, PropagationFeedback feedback, PropagatedScenario* propScenario);

void scenarioPropagatorLaunchVehicle(ScenarioLV* entryVehicle,  QColor trajectoryColor, PropagationFeedback feedback, PropagatedScenario* propScenario);

void scenarioPropagatorGroundElement(ScenarioGroundStation* groundElement,  QColor trajectoryColor, PropagationFeedback feedback, PropagatedScenario* propScenario);

void scenarioPropagatorPoint(ScenarioPoint* aPoint,  QColor trajectoryColor, PropagationFeedback feedback, PropagatedScenario* propScenario);


#endif // _STA_SCENARIO_H_

