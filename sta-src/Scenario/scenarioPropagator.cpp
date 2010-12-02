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

#include "External/external.h"

#include <QWidget>
#include <QColor>
#include <QDebug>
#include <QMessageBox>


USING_PART_OF_NAMESPACE_EIGEN;



void scenarioPropagatorSatellite(ScenarioSC* vehicle, PropagationFeedback& feedback, PropagatedScenario* propScenario, QWidget* parent)
{
	const QList<QSharedPointer<ScenarioAbstractTrajectoryType> >& trajectoryList = vehicle->SCMission()->TrajectoryPlan()->AbstractTrajectory();
    int numberOfArcs = trajectoryList.size();
    sta::StateVector theLastStateVector = sta::StateVector::zero();
    double theLastSampleTime = 0.0;

	// Initial state is stored in the first trajectory (for now); so,
	// the empty trajectory plan case has to be treated specially.
	if (!trajectoryList.isEmpty())
	{
        SpaceObject* spaceObject = new SpaceObject(vehicle);
		spaceObject->setName(vehicle->Name());

		// Propagate all segments of the trajectory plan.
		foreach (QSharedPointer<ScenarioAbstractTrajectoryType> trajectory, trajectoryList)
		{
			QList<double> sampleTimes;
			QList<sta::StateVector> samples;
            int numberOFsamples;

			if (dynamic_cast<ScenarioLoiteringType*>(trajectory.data()))    // Loitering
			{
                ScenarioLoiteringType* loitering = dynamic_cast<ScenarioLoiteringType*>(trajectory.data());
                QString propagator = loitering->PropagationPosition()->propagator();

                if (theLastSampleTime > 0.0)  // This arc is NOT the first one
                {
                    if (propagator == "TWO BODY")
                    {
                        ScenarioKeplerianElementsType* elements = new ScenarioKeplerianElementsType;
                        QString centralBodyName = loitering->Environment()->CentralBody()->Name();
                        StaBody* centralBody = STA_SOLAR_SYSTEM->lookup(centralBodyName);
                        sta::KeplerianElements initialStateKeplerian = cartesianTOorbital(centralBody->mu(), theLastStateVector);
                        //qDebug() << initialStateKeplerian.Inclination << endl;
                        elements->setSemiMajorAxis(initialStateKeplerian.SemimajorAxis);
                        elements->setEccentricity(initialStateKeplerian.Eccentricity);
                        elements->setInclination(sta::radToDeg(initialStateKeplerian.Inclination));
                        elements->setRAAN(sta::radToDeg(initialStateKeplerian.AscendingNode));
                        elements->setArgumentOfPeriapsis(sta::radToDeg(initialStateKeplerian.ArgumentOfPeriapsis));
                        elements->setTrueAnomaly(sta::radToDeg(initialStateKeplerian.TrueAnomaly));
                        loitering->InitialPosition()->setAbstract6DOFPosition(QSharedPointer<ScenarioAbstract6DOFPositionType>(elements));
                        //QDateTime myStarDateTime = sta::JdToCalendar(sta::MjdToJd(theLastSampleTime));
                        //loitering->TimeLine()->setStartTime(myStarDateTime);
                    }
                    else
                    {
                        ScenarioStateVectorType* stateVector = new ScenarioStateVectorType();
                        stateVector->setX(theLastStateVector.position(0));
                        stateVector->setY(theLastStateVector.position(1));
                        stateVector->setZ(theLastStateVector.position(2));
                        stateVector->setVx(theLastStateVector.velocity(0));
                        stateVector->setVy(theLastStateVector.velocity(1));
                        stateVector->setVz(theLastStateVector.velocity(2));
                        //qDebug() << stateVector->x() << stateVector->y() << stateVector->z() << endl;
                        loitering->InitialPosition()->setAbstract6DOFPosition(QSharedPointer<ScenarioAbstract6DOFPositionType>(stateVector));
                        //QDateTime myStarDateTime = sta::JdToCalendar(sta::MjdToJd(theLastSampleTime));
                        //loitering->TimeLine()->setStartTime(myStarDateTime);
                    }
                }

                PropagateLoiteringTrajectory(loitering, sampleTimes, samples, feedback);

                // Recovering the last state vector
                numberOFsamples = sampleTimes.size();
                theLastStateVector = samples.at(numberOFsamples - 1);
                theLastSampleTime = sampleTimes.at(numberOFsamples - 1);

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
					return;
				}

				QString centralBodyName = loitering->Environment()->CentralBody()->Name();
				StaBody* centralBody = STA_SOLAR_SYSTEM->lookup(centralBodyName);
				if (!centralBody)
				{
                    int ret = QMessageBox::warning(parent, "Propagation Error", "Unrecognized central body");
					continue;
				}

				QString coordSysName = loitering->InitialPosition()->CoordinateSystem();
				sta::CoordinateSystem coordSys(coordSysName);
				if (coordSys.type() == sta::COORDSYS_INVALID)
				{
                    int ret = QMessageBox::warning(parent, "Propagation Error", "Unrecognized coordinate system");
					continue;
				}

                if (numberOFsamples > 1)
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
				PropagateLoiteringTLETrajectory(loiteringTLE, sampleTimes, samples, feedback);

				if (feedback.status() != PropagationFeedback::PropagationOk)
				{
                    return;
				}

				QString centralBodyName = "Earth";
				StaBody* centralBody = STA_SOLAR_SYSTEM->lookup(centralBodyName);

				if (!centralBody)
				{
                    int ret = QMessageBox::warning(parent, "Propagation Error", "Unrecognized central body");
					continue;
				}

				QString coordSysName = "INERTIAL J2000";
				sta::CoordinateSystem coordSys(coordSysName);
				if (coordSys.type() == sta::COORDSYS_INVALID)
				{
                    int ret = QMessageBox::warning(parent, "Propagation Error", "Unrecognized coordinate system");
					continue;
				}

				if (sampleTimes.size() > 1)
				{
					MissionArc* arc = new MissionArc(centralBody,
													 coordSys,
													 sampleTimes,
													 samples);

                    // Loading arc color, name, and model
                    QString arcName = loiteringTLE->ElementIdentifier()->Name();
                    QString arcColorName = loiteringTLE->ElementIdentifier()->colorName();
                    MissionsDefaults myMissionDefaults;
                    QColor trajectoryColor = myMissionDefaults.missionArcColorFromQt(arcColorName);
                    QString arcModelName = loiteringTLE->ElementIdentifier()->modelName();
                    arc->setArcTrajectoryColor(trajectoryColor);
                    arc->setModelName(arcModelName);

					spaceObject->addMissionArc(arc);
				}
            }
            else if (dynamic_cast<ScenarioDeltaVType*>(trajectory.data()))    // DeltaVs
            {
                ScenarioDeltaVType* deltaV = dynamic_cast<ScenarioDeltaVType*>(trajectory.data());
                // Calculating direction and magnitude. The magnitude is
                // stored in m/s, and we need to convert to km/s
                double theDeltaVMagnitude = deltaV->Magnitude() * 0.001;

                // Calculating the duration dt (in seconds)
                theLastSampleTime = sta::JdToMjd(sta::CalendarToJd(deltaV->TimeLine()->StartTime()));
                double endTime = sta::JdToMjd(sta::CalendarToJd(deltaV->TimeLine()->EndTime()));
                double dt = sta::daysToSecs(endTime - theLastSampleTime);

                sampleTimes << theLastSampleTime;
                samples << theLastStateVector;

                // Calculating direction and magnitude. The magnitude is
                // stored in m/s, and we need to convert to km/s
                Vector3d dv = Vector3d(deltaV->DeltaVx(), deltaV->DeltaVy(), deltaV->DeltaVz()) * (deltaV->Magnitude() * 0.001);

                // Compute the transformation from the local satellite coordinate system to the
                // coordinate system of the trajectory.
                Vector3d T = theLastStateVector.velocity.normalized();
                Vector3d R = -theLastStateVector.position.normalized();

                // Compute the rows of the 3x3 transformation matrix
                Vector3d X = T;
                Vector3d Y = (X.cross(R)).normalized();
                Vector3d Z = Y.cross(X);
                Matrix3d M;
                M << X, Y, Z;

                // Transform delta-V
                dv = M * dv;

                // TODO: We should be performing at least two body propagation over the duration
                // of the maneuver.
                theLastStateVector.position += theLastStateVector.velocity * dt;
                theLastStateVector.velocity += dv;

                theLastSampleTime = endTime;

                // Emit the time+state sample
                sampleTimes << theLastSampleTime;
                samples << theLastStateVector;

                QString centralBodyName = deltaV->Environment()->CentralBody()->Name();
                StaBody* centralBody = STA_SOLAR_SYSTEM->lookup(centralBodyName);
                QString coordSysName = deltaV->InitialPosition()->CoordinateSystem();
                sta::CoordinateSystem coordSys(coordSysName);

                MissionArc* arc = new MissionArc(centralBody,
                                                 coordSys,
                                                 sampleTimes,
                                                 samples);

                // Loading arc color, name, and model
                arc->setArcName(deltaV->ElementIdentifier()->Name());
                QString arcColorName = deltaV->ElementIdentifier()->colorName();
                MissionsDefaults myMissionDefaults;
                QColor trajectoryColor = myMissionDefaults.missionArcColorFromQt(arcColorName);
                arc->setArcTrajectoryColor(trajectoryColor);
                arc->setModelName(deltaV->ElementIdentifier()->modelName());
                spaceObject->addMissionArc(arc);
            }
            else if (dynamic_cast<ScenarioExternalType*>(trajectory.data()))    // External trajectory
            {
                ScenarioExternalType* extTrajectory = dynamic_cast<ScenarioExternalType*>(trajectory.data());

                QString centralBodyName = extTrajectory->CentralBody()->Name();
                StaBody* centralBody = STA_SOLAR_SYSTEM->lookup(centralBodyName);
                if (!centralBody)
                {
                    int ret = QMessageBox::warning(parent, "Propagation Error", "Unrecognized central body");
                    continue;
                }

                QString coordSysName = extTrajectory->CoordinateSystem();
                sta::CoordinateSystem coordSys(coordSysName);
                if (coordSys.type() == sta::COORDSYS_INVALID)
                {
                    int ret = QMessageBox::warning(parent, "Propagation Error", "Unrecognized coordinate system");
                    continue;
                }

                PropagateExternalTrajectory(extTrajectory, "", sampleTimes, samples, feedback);

                if (feedback.status() != PropagationFeedback::PropagationOk)
                {
                    return;
                }

                if (sampleTimes.size() > 1)
                {
                    MissionArc* arc = new MissionArc(centralBody,
                                                     coordSys,
                                                     sampleTimes,
                                                     samples);

                    // Loading arc color, name, and model
                    QString arcName = extTrajectory->ElementIdentifier()->Name();
                    QString arcColorName = extTrajectory->ElementIdentifier()->colorName();

                    MissionsDefaults myMissionDefaults;
                    QColor trajectoryColor = myMissionDefaults.missionArcColorFromQt(arcColorName);
                    QString arcModelName = extTrajectory->ElementIdentifier()->modelName();

                    arc->setArcName(arcName);
                    arc->setArcTrajectoryColor(trajectoryColor);
                    arc->setModelName(arcModelName);

                    spaceObject->addMissionArc(arc);
                }

            }
            /////////////////////////// end of the big IF for all arcs
        }

        propScenario->addSpaceObject(spaceObject);
	}
}


void scenarioPropagatorReEntryVehicle(ScenarioREV* entryVehicle, PropagationFeedback& feedback, PropagatedScenario* propScenario, QWidget* parent)
{
    const QList<QSharedPointer<ScenarioAbstractTrajectoryType> >& trajectoryList = entryVehicle->REVMission()->REVTrajectoryPlan()->AbstractTrajectory();

    if(!trajectoryList.isEmpty())
    {
        SpaceObject* spaceObject = new SpaceObject(entryVehicle);
        spaceObject->setName(entryVehicle->Name());

        // Propagate all segments of the trajectory plan.
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
                        int ret = QMessageBox::warning(parent, "Canceled", "Propagation was canceled.");
                    }
                    else
                    {
                        int ret = QMessageBox::critical(parent, "Propagation Error", "Error during propagation");
                    }

                    delete propScenario;
                    return;
                }

                QString centralBodyName = entry->Environment()->CentralBody()->Name();
                StaBody* centralBody = STA_SOLAR_SYSTEM->lookup(centralBodyName);
                if (!centralBody)
                {
                    int ret = QMessageBox::warning(parent, "Propagation Error", "Unrecognized central body");
                    continue;
                }

                QString coordSysName = entry->InitialPosition()->CoordinateSystem();
                sta::CoordinateSystem coordSys(coordSysName);
                if (coordSys.type() == sta::COORDSYS_INVALID)
                {
                    int ret = QMessageBox::warning(parent, "Propagation Error", "Unrecognized coordinate system");
                    continue;
                }
                MissionArc* arc = new MissionArc(centralBody,
                                                 coordSys,
                                                 sampleTimes,
                                                 samples);

                // Loading arc color, name, and model
                arc->setArcName(entry->ElementIdentifier()->Name());
                QString arcColorName = entry->ElementIdentifier()->colorName();
                MissionsDefaults myMissionDefaults;
                QColor trajectoryColor = myMissionDefaults.missionArcColorFromQt(arcColorName);
                // Yellow for the time being
                arc->setArcTrajectoryColor("Yellow");
                //arc->setArcTrajectoryColor(trajectoryColor);
                arc->setModelName(entry->ElementIdentifier()->modelName());
                spaceObject->addMissionArc(arc);
            }
        }
        propScenario->addSpaceObject(spaceObject);
    }
}




void scenarioPropagatorReEntryVehicle(ScenarioREV* entryVehicle,  QColor trajectoryColor, PropagationFeedback& feedback, PropagatedScenario* propScenario)
{
    const QList<QSharedPointer<ScenarioAbstractTrajectoryType> >& trajectoryList = entryVehicle->REVMission()->REVTrajectoryPlan()->AbstractTrajectory();

    if(!trajectoryList.isEmpty())
    {
        SpaceObject* spaceObject = new SpaceObject(entryVehicle);
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
                // Loading arc color, name, and model
                arc->setArcName(entry->ElementIdentifier()->Name());
                QString arcColorName = entry->ElementIdentifier()->colorName();
                MissionsDefaults myMissionDefaults;
                QColor trajectoryColor = myMissionDefaults.missionArcColorFromQt(arcColorName);
                arc->setArcTrajectoryColor(trajectoryColor);
                arc->setModelName(entry->ElementIdentifier()->modelName());
                spaceObject->addMissionArc(arc);
            }
        }
        propScenario->addSpaceObject(spaceObject);
    }
}




void scenarioPropagatorLaunchVehicle(ScenarioLV* vehicle,  QColor trajectoryColor, PropagationFeedback& feedback, PropagatedScenario* propScenario)
{
    // Nothing for STA
}




void scenarioPropagatorGroundElement(ScenarioGroundStation* groundElement, PropagationFeedback& feedback, PropagatedScenario* propScenario, QWidget* parent)
{
    QSharedPointer<ScenarioAbstract3DOFPositionType> position = groundElement->Location()->Abstract3DOFPosition();
    QSharedPointer<ScenarioGroundPositionType> groundPosition = qSharedPointerDynamicCast<ScenarioGroundPositionType>(position);

    if (!groundPosition.isNull())
    {
        StaBody* centralBody = STA_SOLAR_SYSTEM->lookup(groundElement->Location()->CentralBody());
        if (centralBody)
        {
            GroundObject* groundObject = new GroundObject();

            groundObject->centralBody = centralBody;
            groundObject->longitude = groundPosition->longitude();
            groundObject->latitude = groundPosition->latitude();
            groundObject->altitude = groundPosition->altitude();

            // Loading station color, name, and model
            QString stationName = groundElement->ElementIdentifier()->Name();
            //groundObject->name = groundElement->Name();
            groundObject->name = stationName;
            QString stationColorName = groundElement->ElementIdentifier()->colorName();
            MissionsDefaults myMissionDefaults;
            QColor trajectoryColor = myMissionDefaults.missionArcColorFromQt(stationColorName);
            QString arcModelName = groundElement->ElementIdentifier()->modelName();

            propScenario->addGroundObject(groundObject);
        }
        else
        {
            int ret = QMessageBox::warning(parent, "Propagation Error", "Unrecognized central body");
        }
    }
}




void scenarioPropagatorGroundElement(ScenarioGroundStation* groundElement,  QColor trajectoryColor, PropagationFeedback& feedback, PropagatedScenario* propScenario)
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



void scenarioPropagatorPoint(ScenarioPoint* aPoint,  QColor trajectoryColor, PropagationFeedback& feedback, PropagatedScenario* propScenario)
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


void scenarioPropagatorPoint(ScenarioPoint* aPoint,  PropagationFeedback& feedback, PropagatedScenario* propScenario)
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
