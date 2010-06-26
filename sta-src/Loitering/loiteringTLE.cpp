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

#include <QtGui>
#include <iostream>
#include <QTextStream>

#include "ui_loiteringTLE.h"


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
    //DropArea *dropArea;
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
	//out << "NameOfParticipant: " << NameOfParticipant << endl;
	TLE_line_1 = StreamWithTLEs.readLine();
	//out << "TLE_line_1: " << TLE_line_1 << endl;
	TLE_line_2 = StreamWithTLEs.readLine();
	//out << "TLE_line_2: " << TLE_line_2 << endl;
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
	//out << "TLE_line_0: " << TLE_line_0 << endl;
	TLE_line_1 = StreamWithTLEs.readLine();
	//out << "TLE_line_1: " << TLE_line_1 << endl;
	TLE_line_2 = StreamWithTLEs.readLine();
	//out << "TLE_line_2: " << TLE_line_2 << endl;
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

    /*
    if (mimeData->hasText())
    {
	//setText(mimeData->text());
	//setTextFormat(Qt::PlainText);
	QMessageBox::information(this, tr("Good TLE file"), tr("This is a TLE file"));
    }
    else if (mimeData->hasUrls())
    {
	QList<QUrl> urlList = mimeData->urls();
	QString text;
	for (int i = 0; i < urlList.size() && i < 32; ++i) {
	    QString url = urlList.at(i).path();
	    text += url + QString("\n");
	}
	setText(text);
    }
    else
    {
	QMessageBox::information(this, tr("Bad TLE file"), tr("This is not a TLE file: \nError loading TLEs"));
    }
    */

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


    /*

    //QTextStream out (stdout);

    QString loiteringLabel = loitering->ElementIdentifier()->Name();
    //out << "PropagateLoiteringTrajectory called with ARC name: " << loiteringLabel << endl;

    QString centralBodyName = loitering->Environment()->CentralBody()->Name();
    StaBody* centralBody = STA_SOLAR_SYSTEM->lookup(centralBodyName);
    if (!centralBody)
    {
	propFeedback.raiseError(QObject::tr("Unrecognized central body '%1'").arg(centralBodyName));
	return false;
    }

    QString coordSysName = loitering->InitialPosition()->CoordinateSystem();
    sta::CoordinateSystem coordSys(coordSysName);
    if (coordSys.type() == sta::COORDSYS_INVALID)
    {
	propFeedback.raiseError(QObject::tr("Unrecognized coordinate system '%1'").arg(coordSysName));
	return false;
    }

    // Get the initial state in two forms:
    //   - Keplerian elements for simple two-body propagation
    //   - A state vector for any other sort of propagation
    ScenarioAbstract6DOFPositionType* position = loitering->InitialPosition()->Abstract6DOFPosition().data();
    sta::StateVector initialState = AbstractPositionToStateVector(position, centralBody);
    sta::KeplerianElements initialStateKeplerian = AbstractPositionToKeplerianElements(position, centralBody);

    double mu = centralBody->mu();

    // Get the timeline information
    const ScenarioTimeLine* timeline = loitering->TimeLine().data();
    double timelineDuration = timeline->StartTime().secsTo(timeline->EndTime());
    double dt = loitering->PropagationPosition()->timeStep();

    if (dt == 0.0)
    {
	propFeedback.raiseError(QObject::tr("Time step is zero!"));
	return false;
    }

    // We don't output values at every integration step. Instead use the time step
    // from simulation parameters. The actual output step used will not necessarily
    // match the requested output step: the code below sets it to be an integer
    // multiple of the integration step.
    double requestedOutputTimeStep = timeline->StepTime();
    double outputTimeStep;
    unsigned int outputRate;
    if (requestedOutputTimeStep < dt)
    {
	outputRate = 1;
	outputTimeStep = dt;
    }
    else
    {
	outputRate = (unsigned int) floor(requestedOutputTimeStep / dt + 0.5);
	outputTimeStep = outputRate * dt;
    }

    if (timelineDuration / outputTimeStep > MAX_OUTPUT_STEPS)
    {
	propFeedback.raiseError(QObject::tr("Number of propagation steps exceeds %1. Try increasing the simulation time step.").arg(MAX_OUTPUT_STEPS));
	return false;
    }

#if OLDSCENARIO
    // Create the list of perturbations that will influence the propagation
    ScenarioSpaceVehicle* spacevehicle = dynamic_cast<ScenarioSpaceVehicle*>(this->parent()->parent());
    ScenarioProperties* vehicleproperties = spacevehicle->properties();

    QList<Perturbations*> perturbationsList = environment()->createListPerturbations(vehicleproperties);
#endif
    QList<Perturbations*> perturbationsList;

    sta::StateVector stateVector = initialState;

    // deviation, reference, and q will be used only in Encke propagation
    sta::StateVector deviation(Vector3d::Zero(), Vector3d::Zero());
    sta::StateVector reference = initialState;
    double q = 0.0;

    // Next lines cretaed by Guillermo for trace purposes. To delete in future
    //double MyJulianDate = sta::CalendarToJd(timeline->StartTime());
    //QTextStream out (stdout);
    //out << "MyJulianDate: " << MyJulianDate << endl;

    double startTime = sta::JdToMjd(sta::CalendarToJd(timeline->StartTime()));
    //out << "MyModifiedJulianDate: " << startTime << endl;


    sampleTimes << startTime;
    samples << stateVector;

    QFile ciccio("data/PerturbationsData.stae");
    QTextStream cicciostream(&ciccio);
    ciccio.open(QIODevice::WriteOnly);

    unsigned int steps = 0;

    QString propagator = loitering->PropagationPosition()->propagator();
    QString integrator = loitering->PropagationPosition()->integrator();

    if (propagator == "TWO BODY")
    {

	double sma            = initialStateKeplerian.SemimajorAxis;
	double e              = initialStateKeplerian.Eccentricity;
	double inclination    = initialStateKeplerian.Inclination*Pi()/180.0;
	double raan           = initialStateKeplerian.AscendingNode*Pi()/180.0;
	double argOfPeriapsis = initialStateKeplerian.ArgumentOfPeriapsis*Pi()/180.0;
	double trueAnomaly    = initialStateKeplerian.TrueAnomaly*Pi()/180.0;
	double meanAnomaly    = trueAnomalyTOmeanAnomaly(trueAnomaly, e);

	QTextStream out (stdout);
	out << "sma: " << sma << endl;
	out << "e: " << e << endl;
	out << "inclination: " << inclination << endl;
	out << "raan: " << raan << endl;
	out << "argOfPeriapsis: " << argOfPeriapsis << endl;
	out << "trueAnomaly: " << trueAnomaly << endl;
	out << "meanAnomaly: " << meanAnomaly << endl;


	// Next lines patched by Guillermo on April 23 2010 to speed up calculations outside the for loop
	double argOfPeriapsisUpdated      = 0.0;
	double meanAnomalyUpdated         = 0.0;
	double raanUpdated                = 0.0;

	double perigee = sma * (1 - e);
	if (perigee < centralBody->meanRadius())
	{
	    propFeedback.raiseError(QObject::tr("The perigee distance is smaller than the main body radius."));
	    return false;
	}
	else
	{

	  for (double t = dt; t < timelineDuration + dt; t += dt)
	  {
	    JulianDate jd = startTime + sta::secsToDays(t);

	    stateVector = propagateTWObody(mu, sma, e, inclination, argOfPeriapsis, raan, meanAnomaly,
					   dt,
					   raanUpdated, argOfPeriapsisUpdated, meanAnomalyUpdated);

	    argOfPeriapsis = argOfPeriapsisUpdated;
	    meanAnomaly    = meanAnomalyUpdated;
	    raan           = raanUpdated;


	    out << "argOfPeriapsis: " << argOfPeriapsis << endl;
	    out << "meanAnomaly: " << meanAnomaly << endl;
	    out << "raan: " << raan << endl;


	    // Append a trajectory sample every outputRate integration steps (and
	    // always at the last step.)
	    if (steps % outputRate == 0 || t >= timelineDuration)
	    {
		sampleTimes << jd;
		samples << stateVector;
	    }
	    ++steps;
	  }
	}
    }
    else if (propagator == "COWELL")
    {
	for (double t = dt; t < timelineDuration + dt; t += dt)
	{
	    JulianDate jd = startTime + sta::secsToDays(t);
	    stateVector = propagateCOWELL(mu, stateVector, dt, perturbationsList, jd, integrator, propFeedback);
	    // Append a trajectory sample every outputRate integration steps (and
	    // always at the last step.)
	    if (steps % outputRate == 0 || t >= timelineDuration)
	    {
		sampleTimes << jd;
		samples << stateVector;
	    }
	    ++steps;
	}
    }
    else if (propagator == "ENCKE")
    {
	double sma            = initialStateKeplerian.SemimajorAxis;
	double e              = initialStateKeplerian.Eccentricity;
	double inclination    = initialStateKeplerian.Inclination;
	double raan           = initialStateKeplerian.AscendingNode;
	double argOfPeriapsis = initialStateKeplerian.ArgumentOfPeriapsis;
	double meanAnomaly    = initialStateKeplerian.MeanAnomaly;

	for (double t = dt; t < timelineDuration + dt; t += dt)
	{
	    JulianDate jd = startTime + sta::secsToDays(t);
	    deviation = propagateENCKE(mu, reference, dt, perturbationsList, jd, stateVector, deviation,  q, integrator, propFeedback);

	    // PropagateTWObody is used to propagate the reference trajectory
	    double argOfPeriapsisUpdated      = 0.0;
	    double meanAnomalyUpdated         = 0.0;
	    double raanUpdated                = 0.0;
	    reference = propagateTWObody(mu, sma, e, inclination, argOfPeriapsis, raan, meanAnomaly,
					 dt,
					 raanUpdated, argOfPeriapsisUpdated, meanAnomalyUpdated);

	    argOfPeriapsis = argOfPeriapsisUpdated;
	    meanAnomaly    = meanAnomalyUpdated;
	    raan           = raanUpdated;

	    // Calculating the perturbed trajectory
	    stateVector = reference + deviation;
	    q = deviation.position.dot(reference.position + 0.5 * deviation.position) / pow(reference.position.norm(), 2.0);

#if 0
	    // Rectification of the reference trajectory, when the deviation is too large.
	    if (q > 0.01)
	    {
	       sta::KeplerianElements keplerian = cartesianTOorbital(mu, stateVector);

	       sma = keplerian.SemimajorAxis;
	       e = keplerian.Eccentricity;
	       inclination = keplerian.Inclination;
	       argOfPeriapsis = keplerian.ArgumentOfPeriapsis;
	       raan = keplerian.AscendingNode;
	       meanAnomaly = keplerian.MeanAnomaly;

	       q = 0;
	       reference = stateVector;
	       deviation = sta::StateVector(null, null);
	    }
#endif
	    // Append a trajectory sample every outputRate integration steps (and
	    // always at the last step.)
	    if (steps % outputRate == 0 || t >= timelineDuration)
	    {
		sampleTimes << jd;
		samples << stateVector;
	    }
	    ++steps;
	}
    }
    else if (propagator == "GAUSS")
    {
	for (double t = dt; t < timelineDuration + dt; t += dt)
	{
	    JulianDate jd = startTime + sta::secsToDays(t);

	    // Append a trajectory sample every outputRate integration steps (and
	    // always at the last step.)
	    if (steps % outputRate == 0 || t >= timelineDuration)
	    {
		sampleTimes << jd;
		samples << stateVector;
	    }
	    ++steps;

	    stateVector = propagateGAUSS(mu, stateVector, dt, perturbationsList, jd, integrator);
	}
    }
    else
    {
	propFeedback.raiseError(QObject::tr("Unsupported propagator '%1'").arg(propagator));
	return false;
    }


    */

    return true;
}
