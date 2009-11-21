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
 ------ Copyright (C) 2008 European Space Agency (space.trajectory.analysis AT gmail.com) ----
 ------------------ Author: Annalisa Riccardi  -------------------------------------------------
 ------------------ E-mail: (nina1983@gmail.com) ----------------------------
 Modified by Tiziana Sabatini  May 2009
 Modified by Valentino Zuccarelli on June 11th 2009 lines 533-534
 Modified by Tiziana Sabatini on July 2009 to add the perturbation layer
 */

#include <iostream>
#include <iomanip>
#include <fstream>
#include "reentry.h"
#include "Main/scenariotree.h"
#include "Scenario/scenario.h"
#include "Astro-Core/stacoordsys.h"
#include "Astro-Core/date.h"
#include "reentrystructures.h"
#include <QtDebug>


using namespace Eigen;

// The commented parts are concerning the Window Analysis Mode/ Dispersion Analysis/ Target Analysis;
// this modes are not functioning in the first version of the software, but their code has been kept for
// further developments

ReEntryDialog::ReEntryDialog(ScenarioTree* parent) :
    QDialog(parent)
{
    setupUi(this);

//    XVariabComboBox->addItem(tr("CAPSULE BALLISTIC COEFFICIENT [Kg/m2]"), (int) CAPSULE_BALLISTIC_COEFFICIENT);
//    XVariabComboBox->addItem(tr("CAPSULE NOSE RADIUS [m]"), (int) CAPSULE_NOSE_RADIUS);
//    XVariabComboBox->addItem(tr("INERTIAL ENTRY VELOCITY [Km/s]"), (int) INERTIAL_ENTRY_VELOCITY);
//    XVariabComboBox->addItem(tr("INERTIAL FLIGHT PATH ANGLE [deg]"), (int) INERTIAL_FLIGHT_PATH_ANGLE);
//    XVariabComboBox->addItem(tr("INERTIAL HEADING [deg]"), (int) INERTIAL_HEADING);
//
//    YVariabComboBox->addItem(tr("CAPSULE BALLISTIC COEFFICIENT [Kg/m2]"), (int) CAPSULE_BALLISTIC_COEFFICIENT);
//    YVariabComboBox->addItem(tr("CAPSULE NOSE RADIUS [m]"), (int) CAPSULE_NOSE_RADIUS);
//    YVariabComboBox->addItem(tr("INERTIAL ENTRY VELOCITY [Km/s]"), (int) INERTIAL_ENTRY_VELOCITY);
//    YVariabComboBox->addItem(tr("INERTIAL FLIGHT PATH ANGLE [deg]"), (int) INERTIAL_FLIGHT_PATH_ANGLE);
//    YVariabComboBox->addItem(tr("INERTIAL HEADING [deg]"), (int) INERTIAL_HEADING);
//
//    ReentryModeComboBox->addItem(tr("Simulation Mode"), "SIMULATION MODE");
//    ReentryModeComboBox->addItem(tr("Window Analysis Mode"), "WINDOW ANALYSIS MODE");


    CentralBodyComboBox->setMinimumContentsLength(11);
    CentralBodyComboBox->addItem(QIcon(":/icons/ComboEarth.png"), tr("Earth"), (int)STA_EARTH);
    CentralBodyComboBox->addItem(QIcon(":/icons/ComboMars.png"), tr("Mars"),(int)STA_MARS);
    //CentralBodyComboBox->addItem(QIcon(":/icons/ComboVenus.png"), tr("Venus"),(int)STA_VENUS);
    //CentralBodyComboBox->addItem(QIcon(":/icons/ComboJupiter.png"), tr("Jupiter"),(int)STA_JUPITER);
    //CentralBodyComboBox->addItem(QIcon(":/icons/ComboSaturn.png"), tr("Saturn"),(int)STA_SATURN);
    //CentralBodyComboBox->addItem(QIcon(":/icons/ComboUranus.png"), tr("Uranus"),(int)STA_URANUS);
    //CentralBodyComboBox->addItem(QIcon(":/icons/ComboNeptune.png"), tr("Neptune"),(int)STA_NEPTUNE);
    //CentralBodyComboBox->addItem(QIcon(":/icons/ComboEarth.png"), tr("Titan"),(int)STA_TITAN);

    PropagatorComboBox->addItem(tr("Cowell"), "COWELL");
    //PropagatorComboBox->addItem(tr("Gauss"), "GAUSS");

    IntegratorComboBox->addItem(tr("Runge-Kutta 4"), "RK4");
    //IntegratorComboBox->addItem(tr("Runge-Kutta Fehlberg"), "RKF");

    InitialStateComboBox->addItem(tr("Spherical Coordinates"), "SPHERICAL COORDINATES");
    InitialStateComboBox->addItem(tr("Keplerian Elements"), "KEPLERIAN ELEMENTS");
    InitialStateComboBox->addItem(tr("Cartesian Coordinates"), "CARTESIAN COORDINATES");

    //Added combo box to select atmosphere and gravity models
    // Patched by Guillermo to make in consistant with Loitering GUI
    AtmosphereComboBox->addItem(tr("Earth GRAM-99"), "gram99");
    AtmosphereComboBox->addItem(tr("Earth Exponential"), "exponential");
    AtmosphereComboBox->addItem(tr("Earth US76"), "ussa1976");
    AtmosphereComboBox->addItem(tr("Mars EMCD"), "mars_emcd");


    // Patched by Guillermo to make in consistant with Loitering GUI
    //GravityComboBox->addItem(QIcon(":/icons/ComboEarth.png"), tr("EGM2008"), "EGM2008");
    //GravityComboBox->addItem(QIcon(":/icons/ComboMars.png"), tr("GTM090"), "GTM090");
    GravityComboBox->addItem(tr("Earth GM2"), "EGM2008");
    GravityComboBox->addItem(tr("Mars GTM"), "GTM090");
    GravityComboBox->setEnabled(false);

    ZonalsSpinBox->setMaximum(20);
    //Add the tesserals SpinBox
    TesseralSpinBox = new TesseralBox(this);
    TesseralSpinBox->setObjectName(QString::fromUtf8("TesseralSpinBox"));
    TesseralSpinBox->setEnabled(false);
    TesseralSpinBox->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
    TesseralSpinBox->setMaximum(ZonalsSpinBox->value());
    TesseralSpinBox->setValue(m_tesserals);

    gridLayout_8->addWidget(TesseralSpinBox, 2, 1, 1, 1);

    // Set up the input validators
    QDoubleValidator* doubleValidator = new QDoubleValidator(this);
    QIntValidator* intValidator = new QIntValidator(this);
    QDoubleValidator* angleValidator = new QDoubleValidator(this);
    angleValidator->setBottom(0.0);
    angleValidator->setTop(360.0);
    QDoubleValidator* angleLatitudeValidator = new QDoubleValidator(this);
    angleLatitudeValidator->setBottom(-90);
    angleLatitudeValidator->setTop(90);
    QDoubleValidator* angleCommandValidator = new QDoubleValidator(this);
    angleCommandValidator->setBottom(-180);
    angleCommandValidator->setTop(180);
    QDoubleValidator* positiveDoubleValidator = new QDoubleValidator(this);
    positiveDoubleValidator->setBottom(0.0);
    QDoubleValidator* zeroToOneValidator = new QDoubleValidator(this);
    zeroToOneValidator->setBottom(0.0);
    zeroToOneValidator->setTop(0.9999);
    QDoubleValidator* zeroToHundredValidator = new QDoubleValidator(this);
    zeroToHundredValidator->setBottom(0.0);
    zeroToHundredValidator->setTop(100);

    IntegStepLineEdit->setValidator(positiveDoubleValidator);
    EntryAltLineEdit->setValidator(positiveDoubleValidator);
    EntryLonLineEdit->setValidator(angleValidator);
    EntryLatLineEdit->setValidator(angleLatitudeValidator);
    InertialVLineEdit->setValidator(doubleValidator);
    InertiaFPALineEdit->setValidator(angleCommandValidator);
    InertialHeadLineEdit->setValidator(angleCommandValidator);

    positionXEdit->setValidator(doubleValidator);
    positionYEdit->setValidator(doubleValidator);
    positionZEdit->setValidator(doubleValidator);
    velocityXEdit->setValidator(doubleValidator);
    velocityYEdit->setValidator(doubleValidator);
    velocityZEdit->setValidator(doubleValidator);

    semimajorAxisEdit->setValidator(positiveDoubleValidator);
    eccentricityEdit->setValidator(zeroToOneValidator);
    inclinationEdit->setValidator(angleValidator);
    raanEdit->setValidator(angleValidator);
    argOfPeriapsisEdit->setValidator(angleValidator);
    trueAnomalyEdit->setValidator(angleValidator);

//    TargetLonEdit->setValidator(angleValidator);
//    TargetLatEdit->setValidator(angleLatitudeValidator);
//    NumSimEdit->setValidator(intValidator);
//
//    DeltaCBRLineEdit->setValidator(doubleValidator);
//    DeltaCMLineEdit->setValidator(doubleValidator);
//    DeltaCCDLineEdit->setValidator(zeroToHundredValidator);
//    DeltaPCDLineEdit->setValidator(zeroToHundredValidator);
//    DeltaAthmDLineEdit->setValidator(doubleValidator);
//    EntryLonLineEdit_2->setValidator(angleValidator);
//    EntryLatLineEdit_2->setValidator(angleLatitudeValidator);
//    VelocityLineEdit->setValidator(doubleValidator);
//    FPALineEdit->setValidator(angleCommandValidator);
//    IHLineEdit->setValidator(angleCommandValidator);
//
//    TimeStepLineEdit_2->setValidator(positiveDoubleValidator);
//    TargetLonLineEdit_2->setValidator(angleValidator);
//    TargetLatLineEdit_2->setValidator(angleLatitudeValidator);
//    NumSimLineEdit_2->setValidator(intValidator);
//    BCLineEdit->setValidator(positiveDoubleValidator);
//    EntryAltLineEdit_2->setValidator(positiveDoubleValidator);
//    InertialVLineEdit_2->setValidator(doubleValidator);
//    InertialFPALineEdit->setValidator(angleCommandValidator);
//    InertialHLineEdit->setValidator(angleCommandValidator);
//
//    MinXLineEdit->setValidator(doubleValidator);
//    MaxXLineEdit->setValidator(doubleValidator);
//    MinYLineEdit->setValidator(doubleValidator);
//    MaxYLineEdit->setValidator(doubleValidator);

    connect(GravityFieldRadioButton, SIGNAL(toggled(bool)), GravityComboBox, SLOT(setEnabled(bool)));
    connect(GravityFieldRadioButton, SIGNAL(toggled(bool)), ZonalsSpinBox, SLOT(setEnabled(bool)));
    connect(GravityFieldRadioButton, SIGNAL(toggled(bool)), TesseralSpinBox, SLOT(setEnabled(bool)));
    connect(ZonalsSpinBox, SIGNAL(valueChanged(int)), TesseralSpinBox, SLOT(setVariableMaximum(int)));

}

ReEntryDialog::~ReEntryDialog()
{
    delete TesseralSpinBox;
}

bool ReEntryDialog::loadValues(ScenarioReEntryTrajectory* reentry)
{
    ScenarioEnvironment* environment = reentry->environment();
    ScenarioSimulationMode* simulationmode = reentry->simulationMode();
    //ScenarioWindowMode* windowmode = reentry->windowMode();
    ScenarioTrajectoryPropagation* propagation = reentry->trajectoryPropagation();

    //MaxAltLineEdit->setText(QString::number(mainbody->maxAltitude()));

    if(loadValues(environment) && loadValues(propagation))
    {
        if (simulationmode->isActive())
            return loadValues(simulationmode);

//            else if (windowmode->isActive())
//               return loadValues(windowmode);

        else
            return false;
    }
    else
    {
        return false;
    }
}


bool ReEntryDialog::loadValues(ScenarioEnvironment* environment)
{
    ScenarioBody* centralBody = environment->centralBody();

    if (centralBody)
    {
        // Set the central body combo box
        for (int i = 0; i < CentralBodyComboBox->count(); i++)
        {
            if (CentralBodyComboBox->itemData(i) == centralBody->body()->id())
            {
              CentralBodyComboBox->setCurrentIndex(i);
                break;
            }
        }
    }
    else return false;

    //Add the perturbations; only gravity field and atmospheric drag have been considered;
    //(the second one is added by default because obviously is always present in the re-entry)
    QList<ScenarioPerturbations*> perturbationsList = environment->perturbationsList();

    GravityFieldRadioButton->setChecked(false);

    if (!perturbationsList.isEmpty())
    {
        foreach (ScenarioPerturbations* perturbation, perturbationsList)
        {
            if (dynamic_cast<ScenarioGravityPerturbations*>(perturbation))
            {
                ScenarioGravityPerturbations* gravity = dynamic_cast<ScenarioGravityPerturbations*>(perturbation);
                GravityFieldRadioButton->setChecked(true);
                if(!gravity->modelName().isEmpty())
                {
                    QString gravitymodel = gravity->modelName();
                    gravitymodel.remove(".stad");

                    for (int i = 0; i < GravityComboBox->count(); i++)
                    {
                        if (GravityComboBox->itemData(i) == gravitymodel)
                        {
                            GravityComboBox->setCurrentIndex(i);
                            break;
                        }
                    }
                }
                else return false;

                ZonalsSpinBox->setValue(gravity->zonalCount());
                m_tesserals = gravity->tesseralCount();
                TesseralSpinBox->setValue(m_tesserals);
            }

            else if (dynamic_cast<ScenarioAtmosphericDragPerturbations*>(perturbation))
            {
                ScenarioAtmosphericDragPerturbations* drag = dynamic_cast<ScenarioAtmosphericDragPerturbations*>(perturbation);
                if(!drag->atmosphericModel().isEmpty())
                {
                    QString atmospheremodel = drag->atmosphericModel();
                    atmospheremodel.remove(".stad");

                    for (int i = 0; i < AtmosphereComboBox->count(); i++)
                    {
                        if (AtmosphereComboBox->itemData(i) == atmospheremodel)
                        {
                            AtmosphereComboBox->setCurrentIndex(i);
                            break;
                        }
                    }
                }
                else return false;
            }
        }
    }

    return true;
}


bool ReEntryDialog::loadValues(ScenarioSimulationMode* simulationmode)
{
    ScenarioTargettingSimulationParameters* parameters = simulationmode->simulationParameters();
    //ScenarioDispersionAnalysis* dispersion = simulationmode->dispersionAnalysis();

    if(parameters)
    {
        ScenarioTimeline* timeline = parameters->timeline();
        ScenarioInitialStatePosition* initialStatePos = parameters->initialStatePosition();
        ScenarioFinalState* finalstate = parameters->finalState();

        QDateTime dateTime = sta::JdToCalendar(sta::MjdToJd(timeline->startTime()));
        if(timeline)
        {
            dateTimeEdit->setDateTime(dateTime);
            //TimeStepLineEdit->setText(QString::number(timeline->timeStep()));
        }
         else return false;

        if(initialStatePos)
        {
                                // Set the coordinate system combo box value
//                                for (int i = 0; i < CoordSystemComboBox->count(); i++)
//                                    {
//                                        if (CoordSystemComboBox->itemData(i) == initialStatePos->coordinateSystem().type())
//                                        {
//                                            CoordSystemComboBox->setCurrentIndex(i);
//                                            break;
//                                        }
//                                    }
            ScenarioAbstractInitialState* initialState = initialStatePos->initialState();
            ScenarioKeplerianElements* elements = dynamic_cast<ScenarioKeplerianElements*>(initialState);
            ScenarioStateVector* stateVector = dynamic_cast<ScenarioStateVector*>(initialState);
            ScenarioSphericalCoordinates* sphericalCoord = dynamic_cast<ScenarioSphericalCoordinates*>(initialState);
            Q_ASSERT(elements || stateVector || sphericalCoord);
            if (elements != NULL)
            {
                InitialStateComboBox->setCurrentIndex(1);
                InitialStateStackedWidget->setCurrentWidget(keplerianPage);

                semimajorAxisEdit->setText(QString::number(elements->m_semimajorAxis));
                eccentricityEdit->setText(QString::number(elements->m_eccentricity));
                inclinationEdit->setText(QString::number(elements->m_inclination));
                raanEdit->setText(QString::number(elements->m_raan));
                argOfPeriapsisEdit->setText(QString::number(elements->m_argumentOfPeriapsis));
                trueAnomalyEdit->setText(QString::number(elements->m_trueAnomaly));
            }
            else if (stateVector != NULL)
            {
                InitialStateComboBox->setCurrentIndex(2);
                InitialStateStackedWidget->setCurrentWidget(stateVectorPage);
                //TODO: check if it works
                positionXEdit->setText(QString::number(stateVector->position().x()));
                positionYEdit->setText(QString::number(stateVector->position().y()));
                positionZEdit->setText(QString::number(stateVector->position().z()));
                velocityXEdit->setText(QString::number(stateVector->velocity().x()));
                velocityYEdit->setText(QString::number(stateVector->velocity().y()));
                velocityZEdit->setText(QString::number(stateVector->velocity().z()));
            }
            else if(sphericalCoord != NULL)
            {
                InitialStateComboBox->setCurrentIndex(0);
                InitialStateStackedWidget->setCurrentWidget(sphericalPage);

                EntryAltLineEdit->setText(QString::number(sphericalCoord->altitude()));
                EntryLonLineEdit->setText(QString::number(sphericalCoord->longitude()));
                EntryLatLineEdit->setText(QString::number(sphericalCoord->latitude()));
                InertialVLineEdit->setText(QString::number(sphericalCoord->inertialVelocity()));
                InertiaFPALineEdit->setText(QString::number(sphericalCoord->inertialFlightPathAngle()));
                InertialHeadLineEdit->setText(QString::number(sphericalCoord->inertialHeading()));
            }
            else
            {
                // Unknown initial state type
                return false;
            }
        }
        else return false;

//                if(finalstate->isActive()){
//                        TargetRadioButton->setChecked(true);
//                        targetLon->setEnabled(true);
//                        TargetLonEdit->setEnabled(true);
//                        TragetLat->setEnabled(true);
//                        TargetLatEdit->setEnabled(true);
//                        TargetLatEdit->setText(QString::number(finalstate->targetLat()));
//                        TargetLonEdit->setText(QString::number(finalstate->targetLon()));
//                }
//                else TargetRadioButton->setChecked(false);

        }
        else return false;
//        if(dispersion->isActive()){
//                DispersionAnalysisRadioButton->setChecked(true);
//                NumSimulLabel->setEnabled(true);
//                NumSimEdit->setEnabled(true);
//                DeviationGroupBox->setEnabled(true);
//                NumSimEdit->setText(QString::number(dispersion->numSimulations()));
//                ScenarioDeviations* deviations = dispersion->deviations();
//                DeltaCBRLineEdit->setText(QString::number(deviations->capsuleBaseRadius()));
//                DeltaCMLineEdit->setText(QString::number(deviations->capsuleMass()));
//                DeltaCCDLineEdit->setText(QString::number(deviations->capsuleCD()));
//                DeltaPCDLineEdit->setText(QString::number(deviations->parachuteCD()));
//                DeltaAthmDLineEdit->setText(QString::number(deviations->atmosphereDensity()));
//                EntryLonLineEdit_2->setText(QString::number(deviations->entryLong()));
//                EntryLatLineEdit_2->setText(QString::number(deviations->entryLat()));
//                VelocityLineEdit->setText(QString::number(deviations->velocity()));
//                FPALineEdit->setText(QString::number(deviations->flightPathAngle()));
//                IHLineEdit->setText(QString::number(deviations->inertialHeading()));
//        }
//        else DispersionAnalysisRadioButton->setChecked(false);

        return true;

}

//bool ReEntryDialog::loadValues(ScenarioWindowMode* windowmode){
//        ScenarioTargettingSimulationParameters* parameters = windowmode->simulationParameters();
//        ScenarioFixedVariables* fixedvar = windowmode->fixedVariables();
//        ScenarioWindow* window = windowmode->window();
//
//        NumSimLineEdit_2->setText(QString::number(windowmode->numSimulations()));
//
//        if(parameters)
//        {
//            ScenarioTimeline* timeline = parameters->timeline();
//            ScenarioFinalState* finalstate = parameters->finalState();
//
//            QDateTime dateTime = sta::JdToCalendar(sta::MjdToJd(timeline->startTime()));
//            if(timeline){
//                 dateTimeEdit_2->setDateTime(dateTime);
//                 TimeStepLineEdit_2->setText(QString::number(timeline->timeStep()));
//            }
//            else return false;
//            if(finalstate->isActive()){
//                                TargetLatLineEdit_2->setText(QString::number(finalstate->targetLat()));
//                                TargetLonLineEdit_2->setText(QString::number(finalstate->targetLon()));
//            }
//
//        }
//        else return false;
//
//        if(fixedvar){
//                EntryAltLineEdit_2->setText(QString::number(fixedvar->entryAltitude()));
//                BCLineEdit->setText(QString::number(fixedvar->ballisticCoeff()));
//
//                InertialVLineEdit_2->setText(QString::number(fixedvar->inertialVelocity()));
//                InertialFPALineEdit->setText(QString::number(fixedvar->inertialFlightPathAngle()));
//                InertialHLineEdit->setText(QString::number(fixedvar->inertialHeading()));
//        }
//        else return false;
//
//        if(window){
//                ScenarioAbstractReentryTrajectoryParam* xparam = window->xAxisVariable();
//                ScenarioAbstractReentryTrajectoryParam* yparam = window->yAxisVariable();
//                if(xparam){
//                        for(int i=0; i<XVariabComboBox->count(); i++){
//                                if (XVariabComboBox->itemData(i) == xparam->type())
//                                {
//                                        XVariabComboBox->setCurrentIndex(i);
//                                        break;
//                                }
//                        }
//                        MinXLineEdit->setText(QString::number(xparam->minRange()));
//                        MaxXLineEdit->setText(QString::number(xparam->maxRange()));
//                }
//                else return false;
//                if(yparam){
//                        for(int i=0; i<YVariabComboBox->count(); i++){
//                                if (YVariabComboBox->itemData(i) == yparam->type())
//                                {
//                                        YVariabComboBox->setCurrentIndex(i);
//                                        break;
//                                }
//                        }
//                        MinYLineEdit->setText(QString::number(yparam->minRange()));
//                        MaxYLineEdit->setText(QString::number(yparam->maxRange()));
//                }
//                else return false;
//        }
//        else return false;
//
//	return true;
//}

bool ReEntryDialog::loadValues(ScenarioTrajectoryPropagation* propagation){

    QString currentPropagator = propagation->propagator();
    for (int i = 0; i < PropagatorComboBox->count(); i++)
    {
        if (PropagatorComboBox->itemData(i) == currentPropagator)
        {
                PropagatorComboBox->setCurrentIndex(i);
            break;
        }
    }
    QString currentIntegrator = propagation->integrator();
    for (int i = 0; i < IntegratorComboBox->count(); i++)
    {
        if (IntegratorComboBox->itemData(i) == currentIntegrator)
        {
                IntegratorComboBox->setCurrentIndex(i);
            break;
        }
    }


    // Lines atched by Guillermo to cope with the problem of the propagation step on the entry module
    // Step <= 5 seconds otherwise it does not integrate properly
    //IntegStepLineEdit->setText(QString::number(propagation->timeStep()));

    IntegStepLineEdit->setText("5");

    return true;
}

bool ReEntryDialog::saveValues(ScenarioReEntryTrajectory* reentry)
{
    ScenarioEnvironment* environment = reentry->environment();
    //ScenarioProperties* vehicleproperties = reentry->vehicleProperties();
    ScenarioSimulationMode* simulationmode = reentry->simulationMode();
    //ScenarioWindowMode* windowmode = reentry->windowMode();
    ScenarioTrajectoryPropagation* propagation = reentry->trajectoryPropagation();

    if(saveValues(environment) && saveValues(propagation) && saveValues(simulationmode))
    {
        //if(ReentryModeComboBox->currentIndex()==0)
        {

            //windowmode->setIsActiveFalse();

            return true;
        }
//        else if(ReentryModeComboBox->currentIndex()==1)
//        {
//            saveValues(windowmode);
//            simulationmode->setIsActiveFalse();
//            return true;
//        }

    }

}


bool ReEntryDialog::saveValues(ScenarioEnvironment* environment)
{
    StaBody* body = STA_SOLAR_SYSTEM->lookup(CentralBodyComboBox->currentText());
    ScenarioBody* centralbody = new ScenarioBody(body);
    if (body)
    {
        centralbody->setBody(body);
        environment->setCentralBody(centralbody);
    }
    else return false;

    if(!environment->perturbationsList().isEmpty())
    {
        foreach (ScenarioPerturbations* perturbation, environment->perturbationsList())
        environment->removePerturbation(perturbation);
    }

    if(GravityFieldRadioButton->isChecked())
    {
        ScenarioGravityPerturbations* gravityPerturbation = new ScenarioGravityPerturbations();

        QString gravitymodel = GravityComboBox->itemData(GravityComboBox->currentIndex()).toString();
        gravitymodel.append(".stad");
        gravityPerturbation->setCentralBody(environment->centralBody()->body());
        gravityPerturbation->setModelName(gravitymodel);
        gravityPerturbation->setZonalCount(ZonalsSpinBox->value());
        m_tesserals = TesseralSpinBox->value();
        gravityPerturbation->setTesseralCount(m_tesserals);

        environment->addPerturbation(gravityPerturbation);
    }

    //Atmospheric drag perturbation is added by default
    ScenarioAtmosphericDragPerturbations* dragPerturbation = new ScenarioAtmosphericDragPerturbations();

    QString atmospheremodel = AtmosphereComboBox->itemData(AtmosphereComboBox->currentIndex()).toString();
    atmospheremodel.append(".stad");
    dragPerturbation->setAtmosphericModel(atmospheremodel);
    dragPerturbation->setCentralBody(environment->centralBody()->body());

    environment->addPerturbation(dragPerturbation);

    return true;
}


bool ReEntryDialog::saveValues(ScenarioSimulationMode* simulationmode){

        ScenarioTargettingSimulationParameters* parameters = new ScenarioTargettingSimulationParameters();

        ScenarioTimeline* timeline = new ScenarioTimeline();
        ScenarioInitialStatePosition*  initialStatePos = new ScenarioInitialStatePosition();
        ScenarioBody* centralbody=new ScenarioBody(STA_SOLAR_SYSTEM->lookup(CentralBodyComboBox->currentText()));
        initialStatePos->setCentralsystemBody(centralbody);
        timeline->setStartTime(sta::JdToMjd(sta::CalendarToJd(dateTimeEdit->dateTime())));
        //timeline->setTimeStep(TimeStepLineEdit->text().toDouble());

        parameters->setTimeline(timeline);

//	int coordSysIndex = CoordSystemComboBox->currentIndex();
//	initialStatePos->setCoordinateSystem((sta::CoordinateSystemType) CoordSystemComboBox->itemData(coordSysIndex).toInt());

        if (InitialStateComboBox->currentIndex() == 0){
                ScenarioSphericalCoordinates* spherical = new ScenarioSphericalCoordinates();
                spherical->m_altitude = EntryAltLineEdit->text().toDouble();
                spherical->m_latitude = EntryLatLineEdit->text().toDouble();
                spherical->m_longitude = EntryLonLineEdit->text().toDouble();
                spherical->m_inertialV = InertialVLineEdit->text().toDouble();
                spherical->m_inertialFPA = InertiaFPALineEdit->text().toDouble();
                spherical->m_inertialH = InertialHeadLineEdit->text().toDouble();

                initialStatePos->setInitialState(spherical);
        }

        else if (InitialStateComboBox->currentIndex() == 1)
        {
           ScenarioKeplerianElements* elements = new ScenarioKeplerianElements();
           elements->m_semimajorAxis       = semimajorAxisEdit->text().toDouble();
           elements->m_eccentricity        = eccentricityEdit->text().toDouble();
           elements->m_inclination         = inclinationEdit->text().toDouble();
           elements->m_raan                = raanEdit->text().toDouble();
           elements->m_argumentOfPeriapsis = argOfPeriapsisEdit->text().toDouble();
           elements->m_trueAnomaly         = trueAnomalyEdit->text().toDouble();

           initialStatePos->setInitialState(elements);
        }

        else
        {
        ScenarioStateVector* stateVector = new ScenarioStateVector();
        Eigen::Vector3d position(positionXEdit->text().toDouble(),
                                 positionYEdit->text().toDouble(),
                                 positionZEdit->text().toDouble());
        Eigen::Vector3d velocity(velocityXEdit->text().toDouble(),
                                 velocityYEdit->text().toDouble(),
                                 velocityZEdit->text().toDouble());

        stateVector->setPosition(position);
        stateVector->setVelocity(velocity);

        initialStatePos->setInitialState(stateVector);
        }

        parameters->setInitialStatePosition(initialStatePos);

//	if(TargetRadioButton->isChecked()){
//		ScenarioFinalState* finalstate = new ScenarioFinalState();
//                finalstate->setIsActiveTrue();
//		finalstate->setTargetLat(TargetLatEdit->text().toDouble());
//                finalstate->setTargetLon(TargetLonEdit->text().toDouble());
//                parameters->setFinalState(finalstate);
//	}
//        else
//        {
//            ScenarioFinalState* finalstate = new ScenarioFinalState();
//            finalstate->setIsActiveFalse();
//            parameters->setFinalState(finalstate);
//        }

//        if(DispersionAnalysisRadioButton->isChecked())
//        {
//            ScenarioDispersionAnalysis* dispersion = new ScenarioDispersionAnalysis();
//            ScenarioDeviations* deviations = new ScenarioDeviations();
//
//            dispersion->setIsActiveTrue();
//            dispersion->setnumSimulations(NumSimEdit->text().toInt());
//
//            deviations->setAtmosphereDensity(DeltaAthmDLineEdit->text().toDouble());
//            deviations->setCapsuleBaseRadius(DeltaCBRLineEdit->text().toDouble());
//            deviations->setCapsuleCD(DeltaCCDLineEdit->text().toDouble());
//            deviations->setCapsuleMass(DeltaCMLineEdit->text().toDouble());
//            deviations->setEntryLat(EntryLatLineEdit_2->text().toDouble());
//            deviations->setEntryLong(EntryLonLineEdit_2->text().toDouble());
//            deviations->setFlightPathAngle(FPALineEdit->text().toDouble());
//            deviations->setInertialHeading(IHLineEdit->text().toDouble());
//            deviations->setParachuteCD(DeltaPCDLineEdit->text().toDouble());
//            deviations->setVelocity(VelocityLineEdit->text().toDouble());
//
//            dispersion->setDeviations(deviations);
//            simulationmode->setDispersionAnalysis(dispersion);
//	}
//        else
//        {
//            ScenarioDispersionAnalysis* dispersion = new ScenarioDispersionAnalysis();
//            dispersion->setIsActiveFalse();
//            simulationmode->setDispersionAnalysis(dispersion);
//        }

        simulationmode->setSimulationParameters(parameters);
        simulationmode->setIsActiveTrue();

        return true;
}

//bool ReEntryDialog::saveValues(ScenarioWindowMode* windowmode){
//	ScenarioTargettingSimulationParameters* parameters = new ScenarioTargettingSimulationParameters();
//	ScenarioFixedVariables* fixedvar = new ScenarioFixedVariables();
//	ScenarioWindow* window = new ScenarioWindow();
//
//        ScenarioTimeline* timeline = new ScenarioTimeline();
//        timeline->setStartTime(sta::JdToMjd(sta::CalendarToJd(dateTimeEdit_2->dateTime())));
//        timeline->setTimeStep(TimeStepLineEdit_2->text().toDouble());
//
//        parameters->setTimeline(timeline);
//
//        ScenarioFinalState* finalstate = new ScenarioFinalState();
//        finalstate->setTargetLat(TargetLatLineEdit_2->text().toDouble());
//        finalstate->setTargetLon(TargetLonLineEdit_2->text().toDouble());
//        finalstate->setIsActiveTrue();
//        parameters->setFinalState(finalstate);
//
//        fixedvar->setBallisticCoeff(BCLineEdit->text().toDouble());
//        fixedvar->setEntryAltitude(EntryAltLineEdit_2->text().toDouble());
//        fixedvar->setInertialHeading(InertialHLineEdit->text().toDouble());
//        fixedvar->setInertialFlightPathAngle(InertialFPALineEdit->text().toDouble());
//        fixedvar->setInertialVelocity(InertialVLineEdit_2->text().toDouble());
//
//
//        ScenarioAbstractReentryTrajectoryParam* xvar = new ScenarioAbstractReentryTrajectoryParam((ReentryTrajectoryParamType)(XVariabComboBox->currentIndex()));
//        ScenarioAbstractReentryTrajectoryParam* yvar = new ScenarioAbstractReentryTrajectoryParam((ReentryTrajectoryParamType)(YVariabComboBox->currentIndex()));
//        xvar->setMax(MaxXLineEdit->text().toDouble());
//        xvar->setMin(MinXLineEdit->text().toDouble());
//        yvar->setMax(MaxYLineEdit->text().toDouble());
//        yvar->setMin(MinYLineEdit->text().toDouble());
//
//
//        window->setXAxisVariable(xvar);
//        window->setYAxisVariable(yvar);
//
//        windowmode->setnumSimulations(NumSimLineEdit_2->text().toInt());
//        windowmode->setSimulationParameters(parameters);
//        windowmode->setFixedVariables(fixedvar);
//        windowmode->setWindow(window);
//
//        windowmode->setIsActiveTrue();
//
//	return true;
//}

bool ReEntryDialog::saveValues(ScenarioTrajectoryPropagation* propagation)
{
    propagation->setIntegrator(IntegratorComboBox->itemData(IntegratorComboBox->currentIndex()).toString());
    propagation->setPropagator(PropagatorComboBox->itemData(PropagatorComboBox->currentIndex()).toString());
    propagation->setTimeStep(IntegStepLineEdit->text().toDouble());
    return true;
}
