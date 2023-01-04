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

#include "Loitering/loiteringTLE.h"
#include "Main/scenariotree.h"
#include "Scenario/scenario.h"
#include "Astro-Core/date.h"
#include "Astro-Core/stacoordsys.h"
#include "Astro-Core/calendarTOjulian.h"
#include "thirdparty/noradtle/norad.h"

#include <QtGui>
#include <iostream>
#include <QTextStream>

#include "ui_loiteringTLE.h"




// The maximum number of steps allowed for a propagation; the user should be allowed
// to adjust this.
static const int MAX_OUTPUT_STEPS = 1000000;

QT_BEGIN_NAMESPACE
class QMimeData;
QT_END_NAMESPACE
class DropArea;

LoiteringTLEDialog::LoiteringTLEDialog(ScenarioTree* parent) :
		QDialog(parent)
{
    setupUi(this);

    // Set up the input validators
    QDoubleValidator* angleValidator = new QDoubleValidator(this);
    angleValidator->setBottom(0.0);
    angleValidator->setTop(360.0);
    QDoubleValidator* positiveDoubleValidator = new QDoubleValidator(this);
    positiveDoubleValidator->setBottom(0.0);
    QDoubleValidator* zeroToOneValidator = new QDoubleValidator(this);
    zeroToOneValidator->setBottom(0.0);
    zeroToOneValidator->setTop(0.9999);
    QDoubleValidator* minusOneToOneValidator = new QDoubleValidator(this);
    minusOneToOneValidator->setBottom(-1.0);
    minusOneToOneValidator->setTop(1.0);

    TimeStepLineEdit->setValidator(positiveDoubleValidator);

    // This creates the drop area inside a QFormLayout called "DropTLEhere" in the UI file
    dropArea = new DropArea;
    dropArea->setMaximumSize(106, 55); dropArea->setMinimumSize(106, 55); // Set the size
    DropTLEhere->addWidget(dropArea); //Add the widget into the desired area

    // Feeding back what we read as TLE file
    connect(dropArea, SIGNAL(changed(const QMimeData *)), this, SLOT(on_TLE_dropped(const QMimeData *)));
}



LoiteringTLEDialog::~LoiteringTLEDialog()
{

}




/////////////////////////////////// FUNCTIONS ///////////////////////////////////

bool LoiteringTLEDialog::loadValues(ScenarioLoiteringTLEType* loiteringTLE)
{
    // Loading the time line and the time step
    ScenarioTimeLine* timeLine = loiteringTLE->TimeLine().data();

    StartDateTimeEdit->setDateTime(timeLine->StartTime());
    EndDateTimeEdit->setDateTime(timeLine->EndTime());
    TimeStepLineEdit->setText(QString::number(timeLine->StepTime()));

    TLEline0Edit->setText(loiteringTLE->tleLine0());
    TLEline1Edit->setText(loiteringTLE->tleLine1());
    TLEline2Edit->setText(loiteringTLE->tleLine2());

    //loiteringAspectTLE.loadValueCentralBody("Earth");

	ScenarioElementIdentifierType* arcIdentifier = loiteringTLE->ElementIdentifier().data();
	LoiteringTLEDialog::loadValues(arcIdentifier);

    return true;
} // End of LoiteringTLEDialog::loadValues


void LoiteringTLEDialog::on_LoadTLEpushButton_clicked()
{
    QTextStream out (stdout);

    QString TLEFileName;
    QString TLE_line_1;
    QString TLE_line_2;

    // Loading now the TLE file
    QString CompleteTLEFileName = QFileDialog::getOpenFileName(this, tr("Select TLE File"),
															   QString("./TLEs/"),
															   tr("TLEs (*.tle *.TLE)"));

    if (!CompleteTLEFileName.isEmpty())
    {
		// Strip away the path--we just want the filename
		if (CompleteTLEFileName.contains('/'))
		{
			TLEFileName = CompleteTLEFileName.remove(0, TLEFileName.lastIndexOf('/') + 1);
			//out << "TLEFileName: " << TLEFileName << endl;
		};

		// Reading the TLE file
		QFile TLEfile(CompleteTLEFileName);
		TLEfile.open(QIODevice::ReadOnly);
		QTextStream StreamWithTLEs(&TLEfile);
		QString NameOfParticipant = StreamWithTLEs.readLine();
		TLE_line_1 = StreamWithTLEs.readLine();
		TLE_line_2 = StreamWithTLEs.readLine();
		TLEfile.close();

		//Updating the GUI
		LoiteringTLEDialog::TLEline0Edit->setText(NameOfParticipant);
		LoiteringTLEDialog::TLEline1Edit->setText(TLE_line_1);
		LoiteringTLEDialog::TLEline2Edit->setText(TLE_line_2);
	}
}


bool LoiteringTLEDialog::saveValues(ScenarioLoiteringTLEType* loiteringTLE)
{
    // Loading the time line and the time step
    ScenarioTimeLine* timeLine = loiteringTLE->TimeLine().data();

    // Saving the time line that constains start date, end date and time step
    timeLine->setStartTime(StartDateTimeEdit->dateTime());
    timeLine->setEndTime(EndDateTimeEdit->dateTime());
    timeLine->setStepTime(TimeStepLineEdit->text().toDouble());

    // Saving now the TLE parameters
    loiteringTLE->setTleLine0(TLEline0Edit->text()); //The name of the vehicle
    loiteringTLE->setTleLine1(TLEline1Edit->text()); //The first TLE line
    loiteringTLE->setTleLine2(TLEline2Edit->text()); //The second TLE line

	ScenarioElementIdentifierType* identifier = loiteringTLE->ElementIdentifier().data();
	LoiteringTLEDialog::saveValues(identifier);

    return true;
}


void LoiteringTLEDialog::on_TLE_dropped(const QMimeData *mimeData)
{
    QTextStream out (stdout);
    QString TLE_line_0;
    QString TLE_line_1;
    QString TLE_line_2;
    QString DroppedTLEFileName;

    QList<QUrl> urlList = mimeData->urls();
    for (int i = 0; i < urlList.size() && i < 32; ++i)
    {
		QString url = urlList.at(i).path();
		DroppedTLEFileName += url;
    }

    if (!DroppedTLEFileName.isEmpty())
    {
		// Reading the TLE file
		QFile TLEfile(DroppedTLEFileName);
		TLEfile.open(QIODevice::ReadOnly);
		QTextStream StreamWithTLEs(&TLEfile);
		TLE_line_0 = StreamWithTLEs.readLine();
		TLE_line_1 = StreamWithTLEs.readLine();
		TLE_line_2 = StreamWithTLEs.readLine();
		TLEfile.close();

		LoiteringTLEDialog::TLEline0Edit->setText(TLE_line_0);
		LoiteringTLEDialog::TLEline1Edit->setText(TLE_line_1);
		LoiteringTLEDialog::TLEline2Edit->setText(TLE_line_2);
	}

}



// This creates a generic drop area
DropArea::DropArea(QWidget *parent)
    : QLabel(parent)
{
    //setFrameStyle(QFrame::Sunken | QFrame::StyledPanel);
    setFrameStyle(QFrame::Sunken);
    setAlignment(Qt::AlignCenter);
    setAcceptDrops(true);
    setAutoFillBackground(true);
    clear();
}



void DropArea::dragEnterEvent(QDragEnterEvent *event)
{
    setText(tr("drop TLE"));
    setBackgroundRole(QPalette::Highlight);

    event->acceptProposedAction();
    emit changed(event->mimeData());
}




void DropArea::dragMoveEvent(QDragMoveEvent *event)
{
    event->acceptProposedAction();
}


// What to do when dropping a file into the drop area
void DropArea::dropEvent(QDropEvent *event)
{

    QTextStream out (stdout);
    QString DroppedTLEFileName, TLEFileName;
    QString TLE_line_0;
    QString TLE_line_1;
    QString TLE_line_2;

    const QMimeData *mimeData = event->mimeData();

    QList<QUrl> urlList = mimeData->urls();
    for (int i = 0; i < urlList.size() && i < 32; ++i)
    {
		QString url = urlList.at(i).path();
		DroppedTLEFileName += url;
    }


    // Strip away the path--we just want the filename
    if (DroppedTLEFileName.contains('/'))
    {
		TLEFileName = DroppedTLEFileName.remove(0, DroppedTLEFileName.lastIndexOf('/') + 1);
		//out << "TLEFileName: " << TLEFileName << endl;
    };
    setText(TLEFileName);

    setBackgroundRole(QPalette::Dark);
    event->acceptProposedAction();
}



void DropArea::dragLeaveEvent(QDragLeaveEvent *event)
{
    clear();
    event->accept();
}



void DropArea::clear()
{
    setText(tr("drop TLE"));
    setBackgroundRole(QPalette::Dark);
    emit changed();
}





/////////////////////////////////////// PropagateLoiteringTLETrajectory /////////////////////////////
bool
		PropagateLoiteringTLETrajectory(ScenarioLoiteringTLEType* loiteringTLE,
										QList<double>& sampleTimes,
										QList<sta::StateVector>& samples,
										PropagationFeedback& propFeedback)
{
    double timelineDuration = loiteringTLE->TimeLine()->StartTime().secsTo(loiteringTLE->TimeLine()->EndTime());

    if (timelineDuration < 0)
    {
		propFeedback.raiseError(QObject::tr("End time before initial time"));
		return true;
    }

    double dt = loiteringTLE->TimeLine()->StepTime();
    tle_t tle;

    int tleError = parse_elements(loiteringTLE->tleLine1().toLatin1().data(), loiteringTLE->tleLine2().toLatin1().data(), &tle);

    if (tleError != 0)
    {
		if (tleError == 3)
			propFeedback.raiseError(QObject::tr("TLE is not parseable."));
		else
			propFeedback.raiseError(QObject::tr("Checksum error in TLE.\n"));
		//return initialState;
		return true;
    }

    if (dt == 0.0)
    {
		propFeedback.raiseError(QObject::tr("Time step is zero!"));
		//return initialState;
		return true;
    }

    if (timelineDuration / dt > MAX_OUTPUT_STEPS)
    {
		propFeedback.raiseError(QObject::tr("Number of propagation steps exceeds %1. Try increasing the simulation time step.").arg(MAX_OUTPUT_STEPS));
		//return initialState;
		return true;
    }

    int ephemeris = TLE_EPHEMERIS_TYPE_SGP4;
    bool isDeep = select_ephemeris(&tle) != 0;
    if (isDeep)
    {
		ephemeris = TLE_EPHEMERIS_TYPE_SDP4;
    }
    else
    {
		ephemeris = TLE_EPHEMERIS_TYPE_SGP4;
    }

    double satelliteParams[N_SAT_PARAMS];

    switch (ephemeris)
    {
    case TLE_EPHEMERIS_TYPE_SGP:
		SGP_init(satelliteParams, &tle);
		break;
    case TLE_EPHEMERIS_TYPE_SGP4:
		SGP4_init(satelliteParams, &tle);
		break;
    case TLE_EPHEMERIS_TYPE_SGP8:
		SGP8_init(satelliteParams, &tle);
		break;
    case TLE_EPHEMERIS_TYPE_SDP4:
		SDP4_init(satelliteParams, &tle);
		break;
    case TLE_EPHEMERIS_TYPE_SDP8:
		SDP8_init(satelliteParams, &tle);
		break;
    }

    double startTimeJd = sta::CalendarToJd(loiteringTLE->TimeLine()->StartTime());
    double timeBase = (startTimeJd - tle.epoch) * 1440.0;

    sta::StateVector state;

    // Loop written to ensure that we always sample right to the end of the
    // requested span.
    for (double t = 0.0; t < timelineDuration + dt; t += dt)
    {
		double tclamp = std::min(t, timelineDuration);

		// Time since epoch
		double t1 = timeBase + tclamp / 60.0;

		switch (ephemeris)
		{
		case TLE_EPHEMERIS_TYPE_SGP:
			SGP(t1, &tle, satelliteParams, state.position.data(), state.velocity.data());
			break;
		case TLE_EPHEMERIS_TYPE_SGP4:
			SGP4(t1, &tle, satelliteParams, state.position.data(), state.velocity.data());
			break;
		case TLE_EPHEMERIS_TYPE_SGP8:
			SGP8(t1, &tle, satelliteParams, state.position.data(), state.velocity.data());
			break;
		case TLE_EPHEMERIS_TYPE_SDP4:
			SDP4(t1, &tle, satelliteParams, state.position.data(), state.velocity.data());
			break;
		case TLE_EPHEMERIS_TYPE_SDP8:
			SDP8(t1, &tle, satelliteParams, state.position.data(), state.velocity.data());
			break;
		}

		// SGP output velocities are in km/minute; convert to km/sec.
		state.velocity /= 60.0;

		//sampleTimes << m_timeline->startTime() + sta::secsToDays(tclamp);
        sampleTimes << sta::JdToMjd(startTimeJd) + sta::secsToDays(tclamp);
		samples << state;
    }


    return true;
}



bool LoiteringTLEDialog::loadValues(ScenarioElementIdentifierType* arcIdentifier)
{
	QString theArcName = arcIdentifier->Name();
	loiteringAspectTLE.loadValueArcName(theArcName);

	QString theArcColor = arcIdentifier->colorName();
	loiteringAspectTLE.loadValueArcColor(theArcColor);

	QString theArcModel = arcIdentifier->modelName();
	loiteringAspectTLE.loadValueArcModel(theArcModel);
}




bool LoiteringTLEDialog::saveValues(ScenarioElementIdentifierType* arcIdentifier)
{
	// The arc name
	QString theArcName = loiteringAspectTLE.saveValueArcName();
	arcIdentifier->setName(theArcName);

	// The color
	QString theColorName = loiteringAspectTLE.saveValueArcColor();
	arcIdentifier->setColorName(theColorName);

	// The model
	QString theModelName = loiteringAspectTLE.saveValueArcModel();
	arcIdentifier->setModelName(theModelName);
}




void LoiteringTLEDialog::on_pushButtonAspectTLE_clicked()
{
//	loiteringAspectTLE.removePlanetsFromComboBoxForTLEs();
//	loiteringAspectTLE.removePlanetsFromComboBoxForTLEs();
//	loiteringAspectTLE.removePlanetsFromComboBoxForTLEs();
//	loiteringAspectTLE.removePlanetsFromComboBoxForTLEs();
	loiteringAspectTLE.exec();
}
