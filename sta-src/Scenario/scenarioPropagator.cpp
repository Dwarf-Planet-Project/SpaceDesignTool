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


#include "Scenario/staschema.h"
#include "Scenario/scenario.h"
#include "Scenario/propagationfeedback.h"
#include "Scenario/scenario.h"
#include "Scenario/missionsDefaults.h"
#include "Scenario/scenarioPropagator.h"

#include "Main/scenarioelementbox.h"
#include "Main/mainwindow.h"
#include "Main/scenarioview.h"
#include "Main/scenariotree.h"
#include "Main/propagatedscenario.h"
#include "Main/timelinewidget.h"
#include "Main/timelineview.h"

#include "Astro-Core/orbitalTOcartesian.h"
#include "Astro-Core/cartesianTOorbital.h"
#include "Astro-Core/trueAnomalyTOmeanAnomaly.h"
#include "Astro-Core/stamath.h"
#include "Astro-Core/date.h"
#include "Astro-Core/EclipseDuration.h"

#include "Plotting/plottingtool.h"

#include "Loitering/loitering.h"
#include "Loitering/loiteringTLE.h"

#include "Entry/reentry.h"

#include <QWidget>
#include <QColor>
#include <QDebug>



void scenarioPropagatorSatellite(ScenarioSC* vehicle, PropagationFeedback feedback, PropagatedScenario* propScenario)
{
	const QList<QSharedPointer<ScenarioAbstractTrajectoryType> >& trajectoryList = vehicle->SCMission()->TrajectoryPlan()->AbstractTrajectory();

	MissionsDefaults thisMissionDefaults;

	// Initial state is stored in the first trajectory (for now); so,
	// the empty trajectory plan case has to be treated specially.
	if (!trajectoryList.isEmpty())
	{
		SpaceObject* spaceObject = new SpaceObject();
		spaceObject->setName(vehicle->Name());

		// Propagate all segments of the trajectory plan.
		foreach (QSharedPointer<ScenarioAbstractTrajectoryType> trajectory, trajectoryList)
		{
			QList<double> sampleTimes;
			QList<sta::StateVector> samples;

			if (dynamic_cast<ScenarioLoiteringType*>(trajectory.data()))    // Loitering
			{
				ScenarioLoiteringType* loitering = dynamic_cast<ScenarioLoiteringType*>(trajectory.data());
				PropagateLoiteringTrajectory(loitering, sampleTimes, samples, feedback);

				//******************************************************************** /OZGUN
				// Eclipse function is called and the "data/EclipseStarLight.stad" is generated
				EclipseDuration* Eclipse = new EclipseDuration();

				Eclipse->StarLightTimeFunction(sampleTimes,
											   samples,
											   STA_SOLAR_SYSTEM->lookup("Earth"),
											   STA_SOLAR_SYSTEM->lookup("Sun"));

				//******************************************************************** OZGUN/

				if (feedback.status() != PropagationFeedback::PropagationOk)
				{
					// An error occurred during propagate. Clean up everything and return immediately.
					// The current propagation results will not be replaced.
					if (feedback.status() == PropagationFeedback::PropagationCanceled)
					{
						//QMessageBox::information(this, tr("Canceled"), tr("Propagation was canceled."));
					}
					else
					{
						//QMessageBox::critical(this, tr("Propagation Error"), tr("Error during propagation: %1").arg(feedback.errorString()));
					}

					delete propScenario;
					return;
				}

				QString centralBodyName = loitering->Environment()->CentralBody()->Name();
				StaBody* centralBody = STA_SOLAR_SYSTEM->lookup(centralBodyName);
				if (!centralBody)
				{
					//QMessageBox::warning(this, tr("Propagation Error"), tr("Unrecognized body '%1'").arg(centralBodyName));
					continue;
				}

				QString coordSysName = loitering->InitialPosition()->CoordinateSystem();
				sta::CoordinateSystem coordSys(coordSysName);
				if (coordSys.type() == sta::COORDSYS_INVALID)
				{
					//QMessageBox::warning(this,tr("Propagation Error"),tr("Unrecognized coordinate system '%1'").arg(coordSysName));
					continue;
				}


				if (sampleTimes.size() > 1)
				{
						MissionArc* arc = new MissionArc(centralBody,
														 coordSys,
														 sampleTimes,
														 samples);

						// Loading arc color, name, and model
						arc->setArcName(loitering->ElementIdentifier()->Name());
						QString arcColorName = loitering->ElementIdentifier()->colorName();
                        MissionsDefaults myMissionDefaults;
                        QColor trajectoryColor = myMissionDefaults.missionArcColorFromQt(arcColorName);
						arc->setArcTrajectoryColor(trajectoryColor);
						arc->setModelName(loitering->ElementIdentifier()->modelName());

						spaceObject->addMissionArc(arc);

				}
			} ///////// End of loitering IF
			else if (dynamic_cast<ScenarioLoiteringTLEType*>(trajectory.data()))    //// TLEs
			{
				ScenarioLoiteringTLEType* loiteringTLE = dynamic_cast<ScenarioLoiteringTLEType*>(trajectory.data());
				// Loading arc color, name, and model
				QString arcName = loiteringTLE->ElementIdentifier()->Name();
				QString arcColorName = loiteringTLE->ElementIdentifier()->colorName();
				QColor trajectoryColor = thisMissionDefaults.missionArcColorFromQt(arcColorName);
				QString arcModelName = loiteringTLE->ElementIdentifier()->modelName();
				PropagateLoiteringTLETrajectory(loiteringTLE, sampleTimes, samples, feedback);

				if (feedback.status() != PropagationFeedback::PropagationOk)
				{
					// An error occurred during propagate. Clean up everything and return immediately.
					// The current propagation results will not be replaced.
					if (feedback.status() == PropagationFeedback::PropagationCanceled)
					{
						//QMessageBox::information(this, tr("Canceled"), tr("Propagation was canceled."));
					}
					else
					{
						//QMessageBox::critical(this, tr("Propagation Error"), tr("Error during propagation: %1").arg(feedback.errorString()));
					}

					delete propScenario;
					return;
				}

				QString centralBodyName = "Earth";
				StaBody* centralBody = STA_SOLAR_SYSTEM->lookup(centralBodyName);

				if (!centralBody)
				{
					//QMessageBox::warning(this, tr("Propagation Error"),tr("Unrecognized body '%1'").arg(centralBodyName));
					continue;
				}

				QString coordSysName = "INERTIAL J2000";
				sta::CoordinateSystem coordSys(coordSysName);
				if (coordSys.type() == sta::COORDSYS_INVALID)
				{
					//QMessageBox::warning(this,tr("Propagation Error"),tr("Unrecognized coordinate system '%1'").arg(coordSysName));
					continue;
				}

				if (sampleTimes.size() > 1)
				{
					MissionArc* arc = new MissionArc(centralBody,
													 coordSys,
													 sampleTimes,
													 samples);

					spaceObject->addMissionArc(arc);
				}

			}  /////////////////////////// end of the big IF for all arcs
        }

        propScenario->addSpaceObject(spaceObject);
	}
}



void scenarioPropagatorSatellite(ScenarioSC* vehicle,  QColor trajectoryColor, PropagationFeedback feedback, PropagatedScenario* propScenario)
	{
		int Lagrmode = 99;
		const QList<QSharedPointer<ScenarioAbstractTrajectoryType> >& trajectoryList = vehicle->SCMission()->TrajectoryPlan()->AbstractTrajectory();

		// Initial state is stored in the first trajectory (for now); so,
		// the empty trajectory plan case has to be treated specially.
		if (!trajectoryList.isEmpty())
		{
			SpaceObject* spaceObject = new SpaceObject();
			spaceObject->setName(vehicle->Name());

			spaceObject->setTrajectoryColor(trajectoryColor);
			/*
#if OLDSCENARIO
        // TODO: Visual properties not added to new space scenario schema yet -cjl
        spaceObject->setModelFile(vehicle->appearance()->model());
        spaceObject->setTrajectoryColor(vehicle->visualProperties().trajectoryColor);
#endif
*/
			// Propagate all segments of the trajectory plan.
			foreach (QSharedPointer<ScenarioAbstractTrajectoryType> trajectory, trajectoryList)
			{
				QList<double> sampleTimes;
				QList<sta::StateVector> samples;

				if (dynamic_cast<ScenarioLoiteringType*>(trajectory.data()))    // Loitering
				{
					ScenarioLoiteringType* loitering = dynamic_cast<ScenarioLoiteringType*>(trajectory.data());
					PropagateLoiteringTrajectory(loitering, sampleTimes, samples, feedback);

					//******************************************************************** /OZGUN
					// Eclipse function is called and the "data/EclipseStarLight.stad" is generated
					EclipseDuration* Eclipse = new EclipseDuration();

					Eclipse->StarLightTimeFunction(sampleTimes,
												   samples,
												   STA_SOLAR_SYSTEM->lookup("Earth"),
												   STA_SOLAR_SYSTEM->lookup("Sun"));

					//******************************************************************** OZGUN/

					if (feedback.status() != PropagationFeedback::PropagationOk)
					{
						// An error occurred during propagate. Clean up everything and return immediately.
						// The current propagation results will not be replaced.
						if (feedback.status() == PropagationFeedback::PropagationCanceled)
						{
							//QMessageBox::information(this, tr("Canceled"), tr("Propagation was canceled."));
						}
						else
						{
							//QMessageBox::critical(this, tr("Propagation Error"), tr("Error during propagation: %1").arg(feedback.errorString()));
						}

						delete propScenario;
						return;
					}

					QString centralBodyName = loitering->Environment()->CentralBody()->Name();
					StaBody* centralBody = STA_SOLAR_SYSTEM->lookup(centralBodyName);
					if (!centralBody)
					{
						//QMessageBox::warning(this, tr("Propagation Error"), tr("Unrecognized body '%1'").arg(centralBodyName));
						continue;
					}

					QString coordSysName = loitering->InitialPosition()->CoordinateSystem();
					sta::CoordinateSystem coordSys(coordSysName);
					if (coordSys.type() == sta::COORDSYS_INVALID)
					{
						//QMessageBox::warning(this,tr("Propagation Error"),tr("Unrecognized coordinate system '%1'").arg(coordSysName));
						continue;
					}


					if (sampleTimes.size() > 1)
					{
						if (Lagrmode != 2)
						{
							MissionArc* arc = new MissionArc(centralBody,
															 coordSys,
															 sampleTimes,
															 samples);
							spaceObject->addMissionArc(arc);
						}
					}
				} ///////// End of loitering IF
				else if (dynamic_cast<ScenarioLoiteringTLEType*>(trajectory.data()))    //// TLEs
				{
					ScenarioLoiteringTLEType* loiteringTLE = dynamic_cast<ScenarioLoiteringTLEType*>(trajectory.data());
					PropagateLoiteringTLETrajectory(loiteringTLE, sampleTimes, samples, feedback);

					if (feedback.status() != PropagationFeedback::PropagationOk)
					{
						// An error occurred during propagate. Clean up everything and return immediately.
						// The current propagation results will not be replaced.
						if (feedback.status() == PropagationFeedback::PropagationCanceled)
						{
							//QMessageBox::information(this, tr("Canceled"), tr("Propagation was canceled."));
						}
						else
						{
							//QMessageBox::critical(this, tr("Propagation Error"), tr("Error during propagation: %1").arg(feedback.errorString()));
						}

						delete propScenario;
						return;
					}

					QString centralBodyName = "Earth";
					StaBody* centralBody = STA_SOLAR_SYSTEM->lookup(centralBodyName);

					if (!centralBody)
					{
						//QMessageBox::warning(this, tr("Propagation Error"),tr("Unrecognized body '%1'").arg(centralBodyName));
						continue;
					}

					QString coordSysName = "INERTIAL J2000";
					sta::CoordinateSystem coordSys(coordSysName);
					if (coordSys.type() == sta::COORDSYS_INVALID)
					{
						//QMessageBox::warning(this,tr("Propagation Error"),tr("Unrecognized coordinate system '%1'").arg(coordSysName));
						continue;
					}

					if (sampleTimes.size() > 1)
					{
						if (Lagrmode != 2)
						{
							MissionArc* arc = new MissionArc(centralBody,
															 coordSys,
															 sampleTimes,
															 samples);
							spaceObject->addMissionArc(arc);
						}
					}

				}  /////////////////////////// end of the big IF for all arcs

#if OLDSCENARIO
				PropagateLagrangian();
#endif
			}

			if (Lagrmode != 2)
			{
				propScenario->addSpaceObject(spaceObject);
			}
		}
	}



void scenarioPropagatorReEntryVehicle(ScenarioREV* entryVehicle,  QColor trajectoryColor, PropagationFeedback feedback, PropagatedScenario* propScenario)
	{
		const QList<QSharedPointer<ScenarioAbstractTrajectoryType> >& trajectoryList = entryVehicle->REVMission()->REVTrajectoryPlan()->AbstractTrajectory();

		if(!trajectoryList.isEmpty())
		{
			SpaceObject* spaceObject = new SpaceObject();
			spaceObject->setName(entryVehicle->Name());
			spaceObject->setTrajectoryColor(trajectoryColor);
			foreach (QSharedPointer<ScenarioAbstractTrajectoryType> trajectory, trajectoryList)
			{
				QList<double> sampleTimes;
				QList<sta::StateVector> samples;

				if (dynamic_cast<ScenarioEntryArcType*>(trajectory.data()))
				{
					ScenarioEntryArcType* entry = dynamic_cast<ScenarioEntryArcType*>(trajectory.data());
					PropagateEntryTrajectory(entryVehicle, entry, sampleTimes, samples, feedback);
					if (feedback.status() != PropagationFeedback::PropagationOk)
					{
						// An error occurred during propagate. Clean up everything and return immediately.
						// The current propagation results will not be replaced.
						if (feedback.status() == PropagationFeedback::PropagationCanceled)
						{
							//QMessageBox::information(this, tr("Canceled"), tr("Propagation was canceled."));
						}
						else
						{
							//QMessageBox::critical(this, tr("Propagation Error"), tr("Error during propagation: %1").arg(feedback.errorString()));
						}

						delete propScenario;
						return;
					}

					QString centralBodyName = entry->Environment()->CentralBody()->Name();
					StaBody* centralBody = STA_SOLAR_SYSTEM->lookup(centralBodyName);
					if (!centralBody)
					{
						//QMessageBox::warning(this, tr("Propagation Error"), tr("Unrecognized body '%1'").arg(centralBodyName));
						continue;
					}

					QString coordSysName = entry->InitialPosition()->CoordinateSystem();
					sta::CoordinateSystem coordSys(coordSysName);
					if (coordSys.type() == sta::COORDSYS_INVALID)
					{
						//QMessageBox::warning(this, tr("Propagation Error"), tr("Unrecognized coordinate system '%1'").arg(coordSysName));
						continue;
					}
					MissionArc* arc = new MissionArc(centralBody,
													 coordSys,
													 sampleTimes,
													 samples);
					spaceObject->addMissionArc(arc);

				}
			}
            propScenario->addSpaceObject(spaceObject);
		}


	}




void scenarioPropagatorLaunchVehicle(ScenarioLV* vehicle,  QColor trajectoryColor, PropagationFeedback feedback, PropagatedScenario* propScenario)
	{
		// Nothing for STA
	}




void scenarioPropagatorGroundElement(ScenarioGroundStation* groundElement,  QColor trajectoryColor, PropagationFeedback feedback, PropagatedScenario* propScenario)
	{
		QSharedPointer<ScenarioAbstract3DOFPositionType> position = groundElement->Location()->Abstract3DOFPosition();
		QSharedPointer<ScenarioGroundPositionType> groundPosition = qSharedPointerDynamicCast<ScenarioGroundPositionType>(position);

		if (!groundPosition.isNull())
		{
			StaBody* centralBody = STA_SOLAR_SYSTEM->lookup(groundElement->Location()->CentralBody());
			if (centralBody)
			{
				GroundObject* groundObject = new GroundObject();

				groundObject->name = groundElement->Name();
				groundObject->centralBody = centralBody;
				groundObject->longitude = groundPosition->longitude();
				groundObject->latitude = groundPosition->latitude();
				groundObject->altitude = groundPosition->altitude();

				propScenario->addGroundObject(groundObject);
			}
		}
	}



void scenarioPropagatorPoint(ScenarioPoint* aPoint,  QColor trajectoryColor, PropagationFeedback feedback, PropagatedScenario* propScenario)
	{
		QSharedPointer<ScenarioAbstract3DOFPositionType> position = aPoint->Location()->Abstract3DOFPosition();
		QSharedPointer<ScenarioGroundPositionType> groundPosition = qSharedPointerDynamicCast<ScenarioGroundPositionType>(position);
		if (!groundPosition.isNull())
		{
			StaBody* centralBody = STA_SOLAR_SYSTEM->lookup(aPoint->Location()->CentralBody());

			if (centralBody)
			{
				GroundObject* groundObject = new GroundObject();

				groundObject->name = aPoint->Name();
				groundObject->centralBody = centralBody;
				groundObject->longitude = groundPosition->longitude();
				groundObject->latitude = groundPosition->latitude();
				groundObject->altitude = groundPosition->altitude();

				propScenario->addGroundObject(groundObject);
			}
		}

	}
