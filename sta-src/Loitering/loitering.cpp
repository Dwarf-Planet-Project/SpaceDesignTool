/*
 File name: loitering.cpp
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
 ------------------ Author:       Guillermo Ortega               -------------------
 ------------------ Affiliation:  European Space Agency (ESA)    -------------------
 -----------------------------------------------------------------------------------
 Modified by Tiziana Sabatini on July 2009 to support the new perturbations layer
*/

#include "Loitering/loitering.h"
#include "Main/scenariotree.h"
#include "Scenario/scenario.h"
#include "Astro-Core/date.h"
#include "Astro-Core/stacoordsys.h"

#include <QTextStream>
#include <QDebug>

using namespace Eigen;


LoiteringDialog::LoiteringDialog(ScenarioTree* parent) :
    QDialog(parent),
    m_tesserals(0)
{
    setupUi(this);
    // Set up the combo boxes
    CoordSystemComboBox->addItem(tr("Planet Fixed"), (int) sta::COORDSYS_BODYFIXED);
    CoordSystemComboBox->addItem(tr("Inertial (J2000)"), (int) sta::COORDSYS_EME_J2000);
    CoordSystemComboBox->addItem(tr("Inertial (B1950)"), (int) sta::COORDSYS_EME_B1950);
    CoordSystemComboBox->addItem(tr("Ecliptic (J2000)"), (int) sta::COORDSYS_ECLIPTIC_J2000);

    PropagatorComboBox->addItem(tr("Two Body"), "TWO BODY");
    PropagatorComboBox->addItem(tr("Gauss"), "GAUSS");
    PropagatorComboBox->addItem(tr("Cowell"), "COWELL");
    PropagatorComboBox->addItem(tr("Encke"), "ENCKE");

    IntegratorComboBox->addItem(tr("Runge-Kutta 3-4"), "RK4");
    IntegratorComboBox->addItem(tr("Runge-Kutta-Fehlberg"), "RKF");

    CentralBodyComboBox->addItem(QIcon(":/icons/ComboEarth.png"), tr("Earth"), (int)STA_EARTH);
    CentralBodyComboBox->addItem(QIcon(":/icons/ComboMoon.png"), tr("Moon"),(int)STA_MOON);
    CentralBodyComboBox->addItem(QIcon(":/icons/ComboMercury.png"), tr("Mercury"),(int)STA_MERCURY);
    CentralBodyComboBox->addItem(QIcon(":/icons/ComboVenus.png"), tr("Venus"),(int)STA_VENUS);
    CentralBodyComboBox->addItem(QIcon(":/icons/ComboMars.png"), tr("Mars"),(int)STA_MARS);
    CentralBodyComboBox->addItem(QIcon(":/icons/ComboJupiter.png"), tr("Jupiter"),(int)STA_JUPITER);
    CentralBodyComboBox->addItem(QIcon(":/icons/ComboSaturn.png"), tr("Saturn"),(int)STA_SATURN);
    CentralBodyComboBox->addItem(QIcon(":/icons/ComboUranus.png"), tr("Uranus"),(int)STA_URANUS);
    CentralBodyComboBox->addItem(QIcon(":/icons/ComboNeptune.png"), tr("Neptune"),(int)STA_NEPTUNE);
    CentralBodyComboBox->addItem(QIcon(":/icons/ComboPluto.png"), tr("Pluto"),(int)STA_PLUTO);
    CentralBodyComboBox->addItem(QIcon(":/icons/ComboDeimos.png"), tr("Deimos"),(int)STA_DEIMOS);
    CentralBodyComboBox->addItem(QIcon(":/icons/ComboTitan.png"), tr("Titan"),(int)STA_TITAN);

    CentralBodyComboBox->addItem(QIcon(":/icons/ComboEuropa.png"), tr("Europa"),(int)STA_EUROPA);
    CentralBodyComboBox->addItem(QIcon(":/icons/ComboEnceladus.png"), tr("Enceladus"),(int)STA_ENCELADUS);
    CentralBodyComboBox->addItem(QIcon(":/icons/ComboTriton.png"), tr("Triton"),(int)STA_TRITON);

    CentralBodyComboBox->addItem(QIcon(":/icons/ComboSun.png"), tr("Sun"),(int)STA_SUN);
    // Moons of the solar system at http://www.windows.ucar.edu/tour/link=/our_solar_system/moons_table.html

    // Set up the input validators
    QDoubleValidator* doubleValidator = new QDoubleValidator(this);
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

    //positionXEdit_2->setValidator(doubleValidator);
    //positionYEdit_2->setValidator(doubleValidator);
    //positionZEdit_2->setValidator(doubleValidator);

    Euler123alphaEdit->setValidator(angleValidator);
    Euler123betaEdit->setValidator(angleValidator);
    Euler123gammaEdit->setValidator(angleValidator);

    AngularVelocityXEdit->setValidator(doubleValidator);
    AngularVelocityYEdit->setValidator(doubleValidator);
    AngularVelocityZEdit->setValidator(doubleValidator);

    quaternion1Edit->setValidator(minusOneToOneValidator);
    quaternion2Edit->setValidator(minusOneToOneValidator);
    quaternion3Edit->setValidator(minusOneToOneValidator);
    quaternion4Edit->setValidator(minusOneToOneValidator);

    AngularVelocityXEdit_2->setValidator(doubleValidator);
    AngularVelocityYEdit_2->setValidator(doubleValidator);
    AngularVelocityZEdit_2->setValidator(doubleValidator);

    //Added combo box to select atmosphere models
    /*
    AtmosphereComboBox->addItem(QIcon(":/icons/ComboEarth.png"), tr("gram99 [0-120km]"), "gram99");
    AtmosphereComboBox->addItem(QIcon(":/icons/ComboEarth.png"), tr("exponential [0-120km]"), "exponential");
    AtmosphereComboBox->addItem(QIcon(":/icons/ComboEarth.png"), tr("ussa1976 [0-1000km]"), "ussa1976");
    AtmosphereComboBox->addItem(QIcon(":/icons/ComboMars.png"), tr("mars_emcd [0-120km]"), "mars_emcd");
    */

    // Lines patched by Guillermo to take away the icons
    AtmosphereComboBox->addItem(tr("GRAM 99"), "gram99");
    AtmosphereComboBox->addItem(tr("Exponential"), "exponential");
    AtmosphereComboBox->addItem(tr("US 76"), "ussa1976");
    AtmosphereComboBox->addItem(tr("Mars EMCD"), "mars_emcd");

    /*
    GravityComboBox->addItem(QIcon(":/icons/ComboEarth.png"), tr("EGM2008"), "EGM2008");
    GravityComboBox->addItem(QIcon(":/icons/ComboMars.png"), tr("GTM090"), "GTM090");
    GravityComboBox->addItem(QIcon(":/icons/ComboVenus.png"), tr("SHGJ180"), "SHGJ180");
    GravityComboBox->addItem(QIcon(":/icons/ComboMoon.png"), tr("JGL165"), "JGL165");
    */

    // Lines patched by Guillermo to take away the icons
    GravityComboBox->addItem(tr("EGM 2008"), "EGM2008");
    GravityComboBox->addItem(tr("GTM 090"), "GTM090");
    GravityComboBox->addItem(tr("SHGJ 180"), "SHGJ180");
    GravityComboBox->addItem(tr("JGL 165"), "JGL165");

    AtmosphereComboBox->setEnabled(false);
    GravityComboBox->setEnabled(false);

    IntStepEdit->setValidator(positiveDoubleValidator);
    IntStepEdit_2->setValidator(positiveDoubleValidator);

    ZonalsSpinBox->setMaximum(20);
    //Add the tesserals SpinBox
    TesseralSpinBox = new TesseralBox(this);
    TesseralSpinBox->setObjectName(QString::fromUtf8("TesseralSpinBox"));
    TesseralSpinBox->setEnabled(false);
    TesseralSpinBox->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
    TesseralSpinBox->setMaximum(ZonalsSpinBox->value());
    TesseralSpinBox->setValue(m_tesserals);

    gridLayout_4->addWidget(TesseralSpinBox, 2, 1, 1, 1);

    connect(GravityFieldRadioButton, SIGNAL(toggled(bool)), GravityComboBox, SLOT(setEnabled(bool)));
    connect(GravityFieldRadioButton, SIGNAL(toggled(bool)), ZonalsSpinBox, SLOT(setEnabled(bool)));
    connect(GravityFieldRadioButton, SIGNAL(toggled(bool)), TesseralSpinBox, SLOT(setEnabled(bool)));
    connect(AtmDragRadioButton, SIGNAL(toggled(bool)), AtmosphereComboBox, SLOT(setEnabled(bool)));
    connect(SolarPressureRadioButton, SIGNAL(toggled(bool)), CrSpinBox, SLOT(setEnabled(bool)));
    connect(SolarPressureRadioButton, SIGNAL(toggled(bool)), AlbedoCheckBox, SLOT(setEnabled(bool)));
    connect(SolarPressureRadioButton, SIGNAL(toggled(bool)), IRCheckBox, SLOT(setEnabled(bool)));
    connect(PertBodyRadioButton, SIGNAL(toggled(bool)), BodyListWidget, SLOT(setEnabled(bool)));
    connect(PertBodyRadioButton, SIGNAL(toggled(bool)), PertBodyListWidget, SLOT(setEnabled(bool)));
    connect(PertBodyRadioButton, SIGNAL(toggled(bool)), AddPushButton, SLOT(setEnabled(bool)));
    connect(PertBodyRadioButton, SIGNAL(toggled(bool)), RemovePushButton, SLOT(setEnabled(bool)));
    connect(BodyListWidget, SIGNAL(itemDoubleClicked(QListWidgetItem*)), this, SLOT(addPerturbingPlanet(QListWidgetItem*)));
    connect(AddPushButton, SIGNAL(pressed()), this, SLOT(addPerturbingPlanet()));
    connect(RemovePushButton, SIGNAL(pressed()), this, SLOT(removePerturbingPlanet()));
    connect(TesseralSpinBox, SIGNAL(valueChanged(int)), this, SLOT(setTesserals(int)));
    connect(ZonalsSpinBox, SIGNAL(valueChanged(int)), TesseralSpinBox, SLOT(setVariableMaximum(int)));

    connect(PropagatorComboBox, SIGNAL(activated(int)), this, SLOT(disableIntegratorComboBox(int)));
}

LoiteringDialog::~LoiteringDialog()
{
    delete TesseralSpinBox;
}

void LoiteringDialog::addPerturbingPlanet()
{
    QList<QListWidgetItem *> planets = BodyListWidget->selectedItems();
    for (int i = 0; i < planets.size(); i++)
    {
        addPerturbingPlanet(planets.at(i));
    }
}

void LoiteringDialog::addPerturbingPlanet(QListWidgetItem* item)
{
        QString text = item->text();
        if(PertBodyListWidget->findItems(text,Qt::MatchExactly).isEmpty())
                PertBodyListWidget->addItem(text);
}

void LoiteringDialog::removePerturbingPlanet()
{
    QList<QListWidgetItem *> perturbplanets = PertBodyListWidget->selectedItems();
    for (int i = 0; i < perturbplanets.size(); i++)
    {
        delete perturbplanets.at(i);
    }
}

void LoiteringDialog::setTesserals(int i)
{
    m_tesserals = i;
}

void LoiteringDialog::disableIntegratorComboBox(int i)
{
    if (i == 0)
        IntegratorComboBox->setEnabled(false);
    else IntegratorComboBox->setEnabled(true);
}

// FUNCTIONS

bool LoiteringDialog::loadValues(ScenarioLoiteringTrajectory* loitering)
{
        ScenarioEnvironment* environment = loitering->environment();
        ScenarioSimulationParameters* parameters = loitering->simulationParameters();
        ScenarioTrajectoryPropagation* propagation = loitering->trajectoryPropagation();

        if(loadValues(environment) && loadValues(parameters) && loadValues(propagation))
    {
                return true;
        }

        return false;
}

bool LoiteringDialog::loadValues(ScenarioEnvironment* environment)
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

    QList<ScenarioPerturbations*> perturbationsList = environment->perturbationsList();

    GravityFieldRadioButton->setChecked(false);
    SolarPressureRadioButton->setChecked(false);
    AtmDragRadioButton->setChecked(false);
    PertBodyRadioButton->setChecked(false);
    DebrisRadioButton->setChecked(false);

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
                AtmDragRadioButton->setChecked(true);
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

            else if (dynamic_cast<ScenarioSolarPressurePerturbations*>(perturbation))
            {
                ScenarioSolarPressurePerturbations* pressure = dynamic_cast<ScenarioSolarPressurePerturbations*>(perturbation);
                SolarPressureRadioButton->setChecked(true);
                CrSpinBox->setValue(pressure->reflectivity());
                AlbedoCheckBox->setChecked(pressure->albedo());
                IRCheckBox->setChecked(pressure->ir());
            }
            else if (dynamic_cast<ScenarioExternalBodyPerturbations*>(perturbation))
            {
                ScenarioExternalBodyPerturbations* bodies = dynamic_cast<ScenarioExternalBodyPerturbations*>(perturbation);
                PertBodyRadioButton->setChecked(true);

                QString nameBody;

                for (int j=0; j<bodies->perturbingBodyList().size(); j++)
                {
                    nameBody = bodies->perturbingBodyList().at(j)->body()->name();
                    QList<QListWidgetItem*> items = BodyListWidget->findItems(nameBody ,Qt::MatchFixedString);
                    if(!items.isEmpty() && nameBody != environment->centralBody()->body()->name())
                    {
                        PertBodyListWidget->addItem(nameBody.toUpper());
                    }
                    else return false;
                }
            }
            else if (dynamic_cast<ScenarioDebrisPerturbations*>(perturbation))
            {
                DebrisRadioButton->setChecked(true);
            }
        }
    }
    return true;
}

bool LoiteringDialog::loadValues(ScenarioSimulationParameters* parameters)
{
        ScenarioExtendedTimeline* timeline = parameters->timeline();
        ScenarioInitialStatePosition* initialStatePos = parameters->initialStatePosition();
        ScenarioInitialStateAttitude* initialStateAtt = parameters->initialStateAttitude();

        QDateTime startTime = sta::JdToCalendar(sta::MjdToJd(timeline->startTime()));
        QDateTime endTime = sta::JdToCalendar(sta::MjdToJd(timeline->endTime()));

        if (timeline)
    {
                dateTimeEdit->setDateTime(startTime);
                dateTimeEdit_2->setDateTime(endTime);
                TimeStepLineEdit->setText(QString::number(timeline->timeStep()));
        }
        else
    {
        return false;
    }

        if (initialStatePos)
    {
                // Set the coordinate system combo box value
                for (int i = 0; i < CoordSystemComboBox->count(); i++)
        {
            if (CoordSystemComboBox->itemData(i) == initialStatePos->coordinateSystem().type())
            {
                CoordSystemComboBox->setCurrentIndex(i);
                break;
            }
        }


            ScenarioAbstractInitialState* initialState = initialStatePos->initialState();
            ScenarioKeplerianElements* elements = dynamic_cast<ScenarioKeplerianElements*>(initialState);
            ScenarioStateVector* stateVector = dynamic_cast<ScenarioStateVector*>(initialState);
            //ScenarioTLEs* TLEs = dynamic_cast<ScenarioTLEs*>(initialState);

            Q_ASSERT(elements || stateVector);

            if (elements != NULL)
            {
                InitialStateComboBox->setCurrentIndex(0);
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
                InitialStateComboBox->setCurrentIndex(1);
                InitialStateStackedWidget->setCurrentWidget(stateVectorPage);
                //TODO: check if it works
                positionXEdit->setText(QString::number(stateVector->position().x()));
                positionYEdit->setText(QString::number(stateVector->position().y()));
                positionZEdit->setText(QString::number(stateVector->position().z()));
                velocityXEdit->setText(QString::number(stateVector->velocity().x()));
                velocityYEdit->setText(QString::number(stateVector->velocity().y()));
                velocityZEdit->setText(QString::number(stateVector->velocity().z()));
            }
	    else
            {
                // Unknown initial state type
                return false;
            }
        }
        else return false;
        if(initialStateAtt)
        {
                //TODO: the class has to be implemented
        }
        return true;
}


bool LoiteringDialog::loadValues(ScenarioTrajectoryPropagation* propagation)
{
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

    IntStepEdit->setText(QString::number(propagation->timeStep()));
    return true;
}


bool LoiteringDialog::saveValues(ScenarioLoiteringTrajectory* loitering)
{
        ScenarioEnvironment* environment = loitering->environment();
        ScenarioSimulationParameters* parameters = loitering->simulationParameters();
        ScenarioTrajectoryPropagation* propagation = loitering->trajectoryPropagation();

        if(saveValues(environment) && saveValues(parameters) && saveValues(propagation))
    {
                return true;
        }

        return false;
}

bool LoiteringDialog::saveValues(ScenarioEnvironment* environment)
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
        gravityPerturbation->setTesseralCount(m_tesserals);

        environment->addPerturbation(gravityPerturbation);
    }

    if(AtmDragRadioButton->isChecked())
    {
        ScenarioAtmosphericDragPerturbations* dragPerturbation = new ScenarioAtmosphericDragPerturbations();

        QString atmospheremodel = AtmosphereComboBox->itemData(AtmosphereComboBox->currentIndex()).toString();
        atmospheremodel.append(".stad");
        dragPerturbation->setAtmosphericModel(atmospheremodel);
        dragPerturbation->setCentralBody(environment->centralBody()->body());
        environment->addPerturbation(dragPerturbation);
    }

    if(SolarPressureRadioButton->isChecked())
    {
        ScenarioSolarPressurePerturbations* solarPerturbation = new ScenarioSolarPressurePerturbations();
        solarPerturbation->setCentralBody(environment->centralBody()->body());
        solarPerturbation->setReflectivity(CrSpinBox->value());
        solarPerturbation->setAlbedo(AlbedoCheckBox->isChecked());
        solarPerturbation->setIr(IRCheckBox->isChecked());
        environment->addPerturbation(solarPerturbation);
    }

    if (PertBodyRadioButton->isChecked() && PertBodyListWidget->count()!=0)
    {
        ScenarioExternalBodyPerturbations* externalbodiesPerturbation = new ScenarioExternalBodyPerturbations();

        externalbodiesPerturbation->setCentralBody(environment->centralBody()->body());
        for(int j=0; j<PertBodyListWidget->count(); j++)
        {
            StaBody* body = STA_SOLAR_SYSTEM->lookup(PertBodyListWidget->item(j)->text());
            if(body && body->name() != environment->centralBody()->body()->name())
            {
                ScenarioBody* perturbingbody = new ScenarioBody(body);
                externalbodiesPerturbation->addPerturbingBody(perturbingbody);
            }
        }
        environment->addPerturbation(externalbodiesPerturbation);
    }

    if (DebrisRadioButton->isChecked())
    {
        ScenarioDebrisPerturbations* debrisPerturbation = new ScenarioDebrisPerturbations();
        debrisPerturbation->setCentralBody(environment->centralBody()->body());

        environment->addPerturbation(debrisPerturbation);
    }

    return true;
}



bool LoiteringDialog::saveValues(ScenarioSimulationParameters* parameters)
{
    ScenarioExtendedTimeline* timeline = parameters->timeline();;
    ScenarioInitialStatePosition*  initialStatePos = parameters->initialStatePosition();
    timeline->setStartTime(sta::JdToMjd(sta::CalendarToJd(dateTimeEdit->dateTime())));
    timeline->setEndTime(sta::JdToMjd(sta::CalendarToJd(dateTimeEdit_2->dateTime())));
    timeline->setTimeStep(TimeStepLineEdit->text().toDouble());

    int coordSysIndex = CoordSystemComboBox->currentIndex();
    initialStatePos->setCoordinateSystem((sta::CoordinateSystemType) CoordSystemComboBox->itemData(coordSysIndex).toInt());

    if (InitialStateComboBox->currentIndex() == 0)
    {

        ScenarioKeplerianElements* elements = new ScenarioKeplerianElements();
        elements->m_semimajorAxis       = semimajorAxisEdit->text().toDouble();
        elements->m_eccentricity        = eccentricityEdit->text().toDouble();
        elements->m_inclination         = inclinationEdit->text().toDouble();
        elements->m_raan                = raanEdit->text().toDouble();
        elements->m_argumentOfPeriapsis = argOfPeriapsisEdit->text().toDouble();
        elements->m_trueAnomaly         = trueAnomalyEdit->text().toDouble();

        initialStatePos->setInitialState(elements);

        //QTextStream out (stdout);
        //out << "a: " << elements->semimajorAxis() << endl;


        return true;

    }

    else  if (InitialStateComboBox->currentIndex() == 1)
    {

        ScenarioStateVector* stateVector = new ScenarioStateVector();
        stateVector->setPosition(Vector3d(positionXEdit->text().toDouble(),
                                          positionYEdit->text().toDouble(),
                                          positionZEdit->text().toDouble()));
        stateVector->setVelocity(Vector3d(velocityXEdit->text().toDouble(),
                                          velocityYEdit->text().toDouble(),
                                          velocityZEdit->text().toDouble()));

        initialStatePos->setInitialState(stateVector);
        return true;
    }

    /* else (InitialStateComboBox->currentIndex() == 2)
    {
        ScenarioTLEs* elements          = new ScenarioTLEs();
        elements->m_semimajorAxis       = semimajorAxisEdit->text().toDouble();
        elements->m_eccentricity        = eccentricityEdit->text().toDouble();
        elements->m_inclination         = inclinationEdit->text().toDouble();
        elements->m_raan                = raanEdit->text().toDouble();
        elements->m_argumentOfPeriapsis = argOfPeriapsisEdit->text().toDouble();
        elements->m_trueAnomaly         = trueAnomalyEdit->text().toDouble();

        initialStatePos->setInitialState(elements);
    }*/

    else return false;

}


bool LoiteringDialog::saveValues(ScenarioTrajectoryPropagation* propagation)
{
        propagation->setIntegrator(IntegratorComboBox->itemData(IntegratorComboBox->currentIndex()).toString());
        propagation->setPropagator(PropagatorComboBox->itemData(PropagatorComboBox->currentIndex()).toString());
        propagation->setTimeStep(IntStepEdit->text().toDouble());

    return true;
}

TesseralBox::TesseralBox(QDialog* parent) : QSpinBox(parent)
{
}

void TesseralBox::setVariableMaximum(int i)
{
    this->setMaximum(i);
}
