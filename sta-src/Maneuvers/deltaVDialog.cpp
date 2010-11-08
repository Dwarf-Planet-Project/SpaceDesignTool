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
 ------------------ Author:       Guillermo Ortega               -------------------
 ------------------ Affiliation:  European Space Agency (ESA)    -------------------
 -----------------------------------------------------------------------------------

*/

#include "Maneuvers/deltaVDialog.h"
#include "Main/scenariotree.h"
#include "Scenario/scenario.h"
#include "Astro-Core/date.h"
#include "Astro-Core/stacoordsys.h"
#include "Astro-Core/stabody.h"
#include "Astro-Core/stamath.h"
#include "Astro-Core/trueAnomalyTOmeanAnomaly.h"
#include "Scenario/propagationfeedback.h"
#include "Astro-Core/cartesianTOorbital.h"
#include "Astro-Core/orbitalTOcartesian.h"

#include "Astro-Core/propagateTWObody.h"
#include "Astro-Core/propagateCOWELL.h"
#include "Astro-Core/propagateENCKE.h"
#include "Astro-Core/propagateGAUSS.h"
#include "Scenario/missionAspectDialog.h"

#include "Astro-Core/cartesianTOspherical.h"
#include "Astro-Core/sphericalTOcartesian.h"

#include "ui_missionAspectDialog.h"

#include <QTextStream>
#include <QDebug>
#include <QFile>

#include <math.h>

using namespace Eigen;



DeltaVDialog::DeltaVDialog(ScenarioTree* parent) :
        QDialog(parent)
{
    setupUi(this);

    // Set up the input validators
    QDoubleValidator* doubleValidator = new QDoubleValidator(this);
    QIntValidator* integerValidator = new QIntValidator(this);
    QDoubleValidator* angleValidator = new QDoubleValidator(this);
    angleValidator->setBottom(-360.0);
    angleValidator->setTop(360.0);
    QDoubleValidator* positiveDoubleValidator = new QDoubleValidator(this);
    positiveDoubleValidator->setBottom(0.0);
    QDoubleValidator* zeroToOneValidator = new QDoubleValidator(this);
    zeroToOneValidator->setBottom(0.0);
    zeroToOneValidator->setTop(0.9999);
    QDoubleValidator* minusOneToOneValidator = new QDoubleValidator(this);
    minusOneToOneValidator->setBottom(-1.0);
    minusOneToOneValidator->setTop(1.0);

    durationLineEdit->setValidator(integerValidator);
    azimuthLineEdit->setValidator(angleValidator);
    elevationLineEdit->setValidator(angleValidator);
    magnitudeLineEdit->setValidator(positiveDoubleValidator);
}

DeltaVDialog::~DeltaVDialog()
{

}


bool DeltaVDialog::loadValues(ScenarioDeltaVType* deltaV)
{
    ScenarioElementIdentifierType* arcIdentifier    = deltaV->ElementIdentifier().data();
    ScenarioTimeLine* parameters		            = deltaV->TimeLine().data();
    double myDeltaVx                                = deltaV->DeltaVx();
    double myDeltaVy                                = deltaV->DeltaVy();
    double myDeltaVz                                = deltaV->DeltaVz();
    double myMagnitude                              = deltaV->Magnitude();

    // Delta-V in the file *should* already be normalized, but just in case...
    Vector3d dv = Vector3d(deltaV->DeltaVx(), deltaV->DeltaVy(), deltaV->DeltaVz()).normalized();

    // Cartesian to spherical
    double myElevation = asin(dv.z());
    double myAzimuth;

    // Report an azimuth of zero when dv is aligned with the z-axis
    if (!Vector2d(dv.x(), dv.y()).isZero())
    {
        myAzimuth = atan2(dv.y(), dv.x());
    }
    else
    {
        myAzimuth = 0.0;
    }

    magnitudeLineEdit->setText(QString::number(myMagnitude));
    azimuthLineEdit->setText(QString::number(sta::radToDeg(myAzimuth)));
    elevationLineEdit->setText(QString::number(sta::radToDeg(myElevation)));

    if (loadValues(arcIdentifier) && loadValues(parameters))
    {
        return true;
    }
    else
    {
        return false;
    }
}




bool DeltaVDialog::loadValues(ScenarioElementIdentifierType* arcIdentifier)
{
    QString theArcName = arcIdentifier->Name();
    deltaVAspect.loadValueArcName(theArcName);

    QString theArcColor = arcIdentifier->colorName();
    deltaVAspect.loadValueArcColor(theArcColor);

    QString theArcModel = arcIdentifier->modelName();
    deltaVAspect.loadValueArcModel(theArcModel);

    return true;
}



bool DeltaVDialog::loadValues(ScenarioTimeLine* timeLine)
{
    QDateTime myStartTime = timeLine->StartTime();
    QDateTime myEndTime   = timeLine->EndTime();
    startDateTimeEdit->setDateTime(myStartTime);
    endDateTimeEdit->setDateTime(myEndTime);
    durationLineEdit->setText(QString::number(myStartTime.secsTo(myEndTime)));

    return true;
}




bool DeltaVDialog::saveValues(ScenarioDeltaVType* deltaV)
{
    ScenarioElementIdentifierType* arcIdentifier    = deltaV->ElementIdentifier().data();
    ScenarioTimeLine* parameters		             = deltaV->TimeLine().data();

    double myMagnitude  = magnitudeLineEdit->text().toDouble();
    double myAzimuth    = azimuthLineEdit->text().toDouble();
    double myElevation  = elevationLineEdit->text().toDouble();

    // From degrees to radians
    myAzimuth   = sta::degToRad(myAzimuth);
    myElevation = sta::degToRad(myElevation);

    // Conversion of the direction of the impulse from polar into cartesian
    double myDeltaVx = cos(myAzimuth) * cos(myElevation);
    double myDeltaVy = sin(myAzimuth) * cos(myElevation);
    double myDeltaVz = sin(myElevation);

    deltaV->setDeltaVx(myDeltaVx);
    deltaV->setDeltaVy(myDeltaVy);
    deltaV->setDeltaVz(myDeltaVz);
    deltaV->setMagnitude(myMagnitude);

    if (saveValues(arcIdentifier) && saveValues(parameters))
    {
        return true;
    }
    else
    {
        return false;
    }
}




bool DeltaVDialog::saveValues(ScenarioElementIdentifierType* arcIdentifier)
{
    // The arc name
    QString theArcName = deltaVAspect.saveValueArcName();
    arcIdentifier->setName(theArcName);

    // The color
    QString theColorName = deltaVAspect.saveValueArcColor();
    arcIdentifier->setColorName(theColorName);

    // The model
    QString theModelName = deltaVAspect.saveValueArcModel();
	arcIdentifier->setModelName(theModelName);

    return true;
}



bool DeltaVDialog::saveValues(ScenarioTimeLine* timeline)
{
    QDateTime myStartTime = startDateTimeEdit->dateTime();
    QDateTime myEndTime   = endDateTimeEdit->dateTime();
    timeline->setStartTime(myStartTime);
    timeline->setEndTime(myEndTime);
    double duration = myStartTime.secsTo(myEndTime);
    duration = durationLineEdit->text().toDouble();

    return true;
}



void DeltaVDialog::on_endDateTimeEdit_dateTimeChanged(const QDateTime&)
{
    QDateTime myStartTime = startDateTimeEdit->dateTime();
    QDateTime myEndTime   = endDateTimeEdit->dateTime();
    durationLineEdit->setText(QString::number(myStartTime.secsTo(myEndTime)));
}

void DeltaVDialog::on_durationLineEdit_textChanged(const QString&)
{
    QDateTime myStartTime = startDateTimeEdit->dateTime();
    double myDuration = durationLineEdit->text().toDouble();
    QDateTime myEndTime = myStartTime.addSecs(myDuration);
    endDateTimeEdit->setDateTime(myEndTime);
 }






void DeltaVDialog::on_pushButtonAspect_clicked()
{
    deltaVAspect.exec();
}
