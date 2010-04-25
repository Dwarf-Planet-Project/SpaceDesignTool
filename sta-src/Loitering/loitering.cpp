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

#include <QTextStream>
#include <QDebug>
#include <QFile>

using namespace Eigen;

static const unsigned int MAX_OUTPUT_STEPS = 1000000;


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
    // Lines patched by Guillermo to take away the icons
    AtmosphereComboBox->addItem(tr("GRAM 99"), "gram99");
    AtmosphereComboBox->addItem(tr("Exponential"), "exponential");
    AtmosphereComboBox->addItem(tr("US 76"), "ussa1976");
    AtmosphereComboBox->addItem(tr("Mars EMCD"), "mars_emcd");

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

bool LoiteringDialog::loadValues(ScenarioLoiteringType* loitering)
{
    ScenarioSCEnvironmentType* environment	    = loitering->Environment().data();
    ScenarioTimeLine* parameters		    = loitering->TimeLine().data();
    ScenarioPropagationPositionType* propagation    = loitering->PropagationPosition().data();
    ScenarioInitialPositionType* initPosition	    = loitering->InitialPosition().data();  //Modified by Dominic to reflect chages in XML schema (initial position now in sharedelements)

    if (loadValues(environment) && loadValues(parameters) && loadValues(propagation) && loadValues(initPosition))
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool LoiteringDialog::loadValues(ScenarioSCEnvironmentType* environment)
{
    QSharedPointer<ScenarioCentralBodyType> centralBody = environment->CentralBody();
    if (!centralBody.isNull())
    {
        QString centralBodyName = centralBody->Name().trimmed();
        const StaBody* body = STA_SOLAR_SYSTEM->lookup(centralBodyName);
        if (!body)
        {
            qDebug() << "Bad central body '" << centralBodyName << "' in loitering trajectory.";
            return false;
        }

        // Set the central body combo box
        for (int i = 0; i < CentralBodyComboBox->count(); i++)
        {
            if (CentralBodyComboBox->itemData(i) == body->id())
            {
                CentralBodyComboBox->setCurrentIndex(i);
                break;
            }
        }
    }
    else
    {
        return false;
    }

    return true;
#if OLDSCENARIO
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
#endif
}


bool LoiteringDialog::loadValues(ScenarioTimeLine* timeLine)
{
    dateTimeEdit->setDateTime(timeLine->StartTime());
    dateTimeEdit_2->setDateTime(timeLine->EndTime());
    TimeStepLineEdit->setText(QString::number(timeLine->StepTime()));

    return true;
}


bool LoiteringDialog::loadValues(ScenarioPropagationPositionType* propagation)
{
    QString currentPropagator = propagation->propagator().trimmed();
    for (int i = 0; i < PropagatorComboBox->count(); i++)
    {
        if (PropagatorComboBox->itemData(i) == currentPropagator)
        {
            PropagatorComboBox->setCurrentIndex(i);
            break;
        }
    }
    QString currentIntegrator = propagation->integrator().trimmed();
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


bool LoiteringDialog::loadValues(ScenarioInitialPositionType* initPosition)
{
    QString coordSysName = initPosition->CoordinateSystem().trimmed();
    sta::CoordinateSystem coordSys(coordSysName);
    if (!coordSys.valid())
    {
        qDebug() << "Bad coordinate system '" << coordSysName << "' in scenario ";
        coordSys = sta::CoordinateSystem(sta::COORDSYS_EME_J2000);
    }

    // Set the coordinate system combo box value
    for (int i = 0; i < CoordSystemComboBox->count(); i++)
    {
        if (CoordSystemComboBox->itemData(i) == coordSys.type())
        {
            CoordSystemComboBox->setCurrentIndex(i);
            break;
        }
    }

    ScenarioAbstract6DOFPositionType* position = initPosition->Abstract6DOFPosition().data();

    ScenarioKeplerianElementsType* elements = dynamic_cast<ScenarioKeplerianElementsType*>(position);
    ScenarioStateVectorType* stateVector    = dynamic_cast<ScenarioStateVectorType*>(position);

    Q_ASSERT(elements || stateVector);

    if (elements)
    {
        InitialStateComboBox->setCurrentIndex(0);
        InitialStateStackedWidget->setCurrentWidget(keplerianPage);

        semimajorAxisEdit->setText(QString::number(elements->semiMajorAxis()));
        eccentricityEdit->setText(QString::number(elements->eccentricity()));
	inclinationEdit->setText(QString::number(elements->inclination()));
	raanEdit->setText(QString::number(elements->RAAN())); //*180/Pi()));
	argOfPeriapsisEdit->setText(QString::number(elements->argumentOfPeriapsis())); //*180/Pi()));
	trueAnomalyEdit->setText(QString::number(elements->trueAnomaly())); //*180/Pi()));
    }
    else if (stateVector)
    {
        InitialStateComboBox->setCurrentIndex(1);
        InitialStateStackedWidget->setCurrentWidget(stateVectorPage);

        positionXEdit->setText(QString::number(stateVector->x()));
        positionYEdit->setText(QString::number(stateVector->y()));
        positionZEdit->setText(QString::number(stateVector->z()));
        velocityXEdit->setText(QString::number(stateVector->vx()));
        velocityYEdit->setText(QString::number(stateVector->vy()));
        velocityZEdit->setText(QString::number(stateVector->vz()));
    }
    else
    {
        // Unknown initial state type
        return false;
    }

    return true;
#if OLDSCENARIO
    ScenarioInitialStatePosition* initialStatePos = parameters->initialStatePosition();
    ScenarioInitialStateAttitude* initialStateAtt = parameters->initialStateAttitude();

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
    else
    {
        return false;
    }

    if (initialStateAtt)
    {
        // TODO: the class has to be implemented
    }
#endif

}


bool LoiteringDialog::saveValues(ScenarioLoiteringType* loitering)
{
    ScenarioSCEnvironmentType* environment	 = loitering->Environment().data();
    ScenarioTimeLine* parameters		 = loitering->TimeLine().data();
    ScenarioPropagationPositionType* propagation = loitering->PropagationPosition().data();
    ScenarioInitialPositionType* initPosition    = loitering->InitialPosition().data();//Modified by Dominic to reflect chages in XML schema (initial position now in sharedelements)


    if (saveValues(environment) && saveValues(parameters) && saveValues(propagation) && saveValues(initPosition))
    {
        return true;
    }
    else
    {
        return false;
    }
}


bool LoiteringDialog::saveValues(ScenarioSCEnvironmentType* environment)
{
    StaBody* centralBody = STA_SOLAR_SYSTEM->lookup(CentralBodyComboBox->currentText());
    if (centralBody)
    {
        environment->CentralBody()->setName(centralBody->name());
    }
    else
    {
        qWarning("Unknown central body %s", CentralBodyComboBox->currentText().toAscii().data());
        return false;
    }

#if OLDSCENARIO
    if (GravityFieldRadioButton->isChecked())
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
#endif

    environment->setAtmosphericDrag(AtmDragRadioButton->isChecked());

    // TODO: Is the code below still necessary with the new schema? -cjl
#if OLDSCENARIO
    if (AtmDragRadioButton->isChecked())
    {
        ScenarioAtmosphericDragPerturbations* dragPerturbation = new ScenarioAtmosphericDragPerturbations();

        QString atmospheremodel = AtmosphereComboBox->itemData(AtmosphereComboBox->currentIndex()).toString();
        atmospheremodel.append(".stad");
        dragPerturbation->setAtmosphericModel(atmospheremodel);
        dragPerturbation->setCentralBody(environment->centralBody()->body());
        environment->addPerturbation(dragPerturbation);
    }
#endif

    environment->setSolarPressure(SolarPressureRadioButton->isChecked());

    // TODO: Is the code below still necessary with the new schema? -cjl
#if OLDSCENARIO
    if (SolarPressureRadioButton->isChecked())
    {
        ScenarioSolarPressurePerturbations* solarPerturbation = new ScenarioSolarPressurePerturbations();
        solarPerturbation->setCentralBody(environment->centralBody()->body());
        solarPerturbation->setReflectivity(CrSpinBox->value());
        solarPerturbation->setAlbedo(AlbedoCheckBox->isChecked());
        solarPerturbation->setIr(IRCheckBox->isChecked());
        environment->addPerturbation(solarPerturbation);
    }
#endif

    // Reset the perturbing bodies list
    environment->perturbingBody().clear();

    // Add selected perturbers
    if (PertBodyRadioButton->isChecked() && PertBodyListWidget->count() != 0)
    {
        for (int j = 0; j < PertBodyListWidget->count(); j++)
        {
            StaBody* body = STA_SOLAR_SYSTEM->lookup(PertBodyListWidget->item(j)->text());
            if (body && body != centralBody)
            {
                environment->perturbingBody().append(body->name());
            }
        }
    }


    // TODO: debris perturbations not available in schema -cjl
#if OLDSCENARIO
    if (DebrisRadioButton->isChecked())
    {
        ScenarioDebrisPerturbations* debrisPerturbation = new ScenarioDebrisPerturbations();
        debrisPerturbation->setCentralBody(environment->centralBody()->body());

        environment->addPerturbation(debrisPerturbation);
    }
#endif

    return true;
}


bool LoiteringDialog::saveValues(ScenarioTimeLine* timeline)
{
    timeline->setStartTime(dateTimeEdit->dateTime());
    timeline->setEndTime(dateTimeEdit_2->dateTime());
    timeline->setStepTime(TimeStepLineEdit->text().toDouble());

    return true;
}


bool LoiteringDialog::saveValues(ScenarioInitialPositionType* initPos)
{
    sta::CoordinateSystemType coordSysType = sta::CoordinateSystemType(CoordSystemComboBox->itemData(CoordSystemComboBox->currentIndex()).toInt());
    initPos->setCoordinateSystem(sta::CoordinateSystem(coordSysType).name());

    switch (InitialStateComboBox->currentIndex())
    {
    case 0:
        {
	    // Guillermo April 23 2010 patching the mess of Dominic
	    ScenarioKeplerianElementsType* elements_deg = new ScenarioKeplerianElementsType;
	    elements_deg->setSemiMajorAxis(semimajorAxisEdit->text().toDouble());
	    elements_deg->setEccentricity(eccentricityEdit->text().toDouble());
	    elements_deg->setInclination(inclinationEdit->text().toDouble());  //Modified by Dominic save values in scenario in radians
	    elements_deg->setRAAN(raanEdit->text().toDouble());
	    elements_deg->setArgumentOfPeriapsis(argOfPeriapsisEdit->text().toDouble());
	    elements_deg->setTrueAnomaly(trueAnomalyEdit->text().toDouble());

	    initPos->setAbstract6DOFPosition(QSharedPointer<ScenarioAbstract6DOFPositionType>(elements_deg));
        }
        return true;

    case 1:
        {
            ScenarioStateVectorType* stateVector = new ScenarioStateVectorType();
            stateVector->setX(positionXEdit->text().toDouble());
            stateVector->setY(positionYEdit->text().toDouble());
            stateVector->setZ(positionZEdit->text().toDouble());
            stateVector->setVx(velocityXEdit->text().toDouble());
            stateVector->setVy(velocityYEdit->text().toDouble());
            stateVector->setVz(velocityZEdit->text().toDouble());

            initPos->setAbstract6DOFPosition(QSharedPointer<ScenarioAbstract6DOFPositionType>(stateVector));
        }
        return true;

    default:
        return false;
    }
}


#if OLDSCENARIO
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
#endif


bool LoiteringDialog::saveValues(ScenarioPropagationPositionType* propagation)
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


/////////////////////////////////////// PropagateLoiteringTrajectory /////////////////////////////
bool
PropagateLoiteringTrajectory(ScenarioLoiteringType* loitering,
                             QList<double>& sampleTimes,
                             QList<sta::StateVector>& samples,
                             PropagationFeedback& propFeedback)
{
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

    double startTime = sta::JdToMjd(sta::CalendarToJd(timeline->StartTime()));
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
	/*
	QTextStream out (stdout);
	out << "sma: " << sma << endl;
	out << "e: " << e << endl;
	out << "inclination: " << inclination << endl;
	out << "raan: " << raan << endl;
	out << "argOfPeriapsis: " << argOfPeriapsis << endl;
	out << "trueAnomaly: " << trueAnomaly << endl;
	out << "meanAnomaly: " << meanAnomaly << endl;
	*/

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

	    /*
	    out << "argOfPeriapsis: " << argOfPeriapsis << endl;
	    out << "meanAnomaly: " << meanAnomaly << endl;
	    out << "raan: " << raan << endl;
	    */

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

#if OLDSCENARIO
    for (double t = dt; t < timelineDuration + dt; t += dt)
    {
        JulianDate jd = startTime + sta::secsToDays(t);

        // Choosing the propagator and propagating the trajectory
        if (trajectoryPropagation()->propagator() == "TWO BODY")
        {
            double perigee=sma*(1-e);
            if (perigee<centralBody()->meanRadius())
            {
                QMessageBox::warning(NULL,
                              QObject::tr("The trajectory has been not propagated"),
                              QObject::tr("The perigee distance is smaller than the main body radius."));
                return stateVector.zero();
            }

            double argOfPeriapsisUpdated      = 0.0;
            double meanAnomalyUpdated         = 0.0;
            double raanUpdated                = 0.0;
            stateVector = propagateTWObody(mu, sma, e, inclination, argOfPeriapsis, raan, meanAnomaly,
                                      trajectoryPropagation()->timeStep(), raanUpdated, argOfPeriapsisUpdated,
                                      meanAnomalyUpdated);

            argOfPeriapsis = argOfPeriapsisUpdated;
            meanAnomaly    = meanAnomalyUpdated;
            raan           = raanUpdated;
        }

        else if (trajectoryPropagation()->propagator() == "COWELL")
        {
            stateVector = propagateCOWELL(mu, stateVector, trajectoryPropagation()->timeStep(), perturbationsList, time, trajectoryPropagation()->integrator(), propFeedback);
        }

        else if (trajectoryPropagation()->propagator() == "ENCKE")
        {
            deviation = propagateENCKE(mu, reference, trajectoryPropagation()->timeStep(),perturbationsList, time, stateVector, deviation,  q, trajectoryPropagation()->integrator(), propFeedback);

            // PropagateTWObody is used to propagate the reference trajectory
            double argOfPeriapsisUpdated      = 0.0;
            double meanAnomalyUpdated         = 0.0;
            double raanUpdated                = 0.0;
            reference = propagateTWObody(mu, sma, e, inclination, argOfPeriapsis, raan, meanAnomaly,
                                      trajectoryPropagation()->timeStep(), raanUpdated, argOfPeriapsisUpdated,
                                      meanAnomalyUpdated);

            argOfPeriapsis = argOfPeriapsisUpdated;
            meanAnomaly    = meanAnomalyUpdated;
            raan           = raanUpdated;

            // Calculating the perturbed trajectory
            stateVector = reference + deviation;
            q = deviation.position.dot(reference.position + 0.5 * deviation.position) / pow(reference.position.norm(), 2.0);

//            // Rectification of the reference trajectory, when the deviation is too large.
//            if (q > 0.01)
//            {
//               sta::KeplerianElements keplerian = cartesianTOorbital(mu, stateVector);
//
//               sma = keplerian.SemimajorAxis;
//               e = keplerian.Eccentricity;
//               inclination = keplerian.Inclination;
//               argOfPeriapsis = keplerian.ArgumentOfPeriapsis;
//               raan = keplerian.AscendingNode;
//               meanAnomaly = keplerian.MeanAnomaly;
//
//               q = 0;
//               reference = stateVector;
//               deviation = sta::StateVector(null, null);
//            }
        }

        else if (trajectoryPropagation()->propagator() == "GAUSS")
        {
            stateVector = propagateGAUSS(mu, stateVector, trajectoryPropagation()->timeStep(), perturbationsList, time, trajectoryPropagation()->integrator());
        }

        KeplerianElements kep = cartesianTOorbital(mu, stateVector);

        cicciostream.setRealNumberPrecision(8);
        //cicciostream << kep.SemimajorAxis << "      ";
        //cicciostream << kep.Eccentricity << "      ";
        cicciostream << kep.Inclination << "      ";
        //cicciostream << kep.ArgumentOfPeriapsis << "      ";
        //cicciostream << kep.AscendingNode << "      ";
        //cicciostream << kep.TrueAnomaly << "      ";

//        //Treat the debris perturbation if selected by the user
//        foreach (Perturbations* perturbation, perturbationsList)
//        if (dynamic_cast<DebrisPerturbations*>(perturbation))
//        {
//            DebrisPerturbations* debris = dynamic_cast<DebrisPerturbations*>(perturbation);
//            double gravityAcceleration = (-pow(initialState.position.norm(),-3.0) * mu * initialState.position).norm();
//            double perturbedAcceleration = gravityAcceleration + debris->calculatePerturbingEffect(initialState, sta::daysToSecs(jd));
//        }

        // Append a trajectory sample every outputRate integration steps (and
        // always at the last step.)
        if (steps % outputRate == 0 || t >= timelineDuration)
        {
            sampleTimes << jd;
            samples << stateVector;
        }
        ++steps;

        time += sta::secsToDays(dt);
    };
#endif

    return true;
}
