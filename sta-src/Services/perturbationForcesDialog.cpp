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
 ------ Copyright (C) 2011 STA Steering Board (space.trajectory.analysis AT gmail.com) ----
*/
/*
------------------ Author: Guillermo Ortega  -------------------------------------------
 */

#include "Services/perturbationForcesDialog.h"

#include "QDebug"

perturbationForcesDialog::perturbationForcesDialog( QWidget * parent, Qt::WindowFlags f) : QDialog(parent,f)
{
	setupUi(this);

    //Added combo box to select atmosphere models
    // Lines patched by Guillermo to take away the icons
    AtmosphereDragTypeComboBox->addItem(tr("GRAM 99"), "gram99");
    AtmosphereDragTypeComboBox->addItem(tr("Exponential"), "exponential");
    AtmosphereDragTypeComboBox->addItem(tr("US 76"), "ussa1976");
    AtmosphereDragTypeComboBox->addItem(tr("Mars EMCD"), "mars_emcd");

    // Lines patched by Guillermo to take away the icons
    GravityEffectsModelComboBox->addItem(tr("EGM 2008"), "EGM2008");
    GravityEffectsModelComboBox->addItem(tr("GTM 090"), "GTM090");
    GravityEffectsModelComboBox->addItem(tr("SHGJ 180"), "SHGJ180");
    GravityEffectsModelComboBox->addItem(tr("JGL 165"), "JGL165");

    AtmosphereDragTypeComboBox->setEnabled(false);
    GravityEffectsModelComboBox->setEnabled(false);

    ZonalsSpinBox->setMaximum(20);

    //Add the tesserals SpinBox
    TesseralSpinBox = new TesseralBox(this);
    TesseralSpinBox->setObjectName(QString::fromUtf8("TesseralSpinBox"));
    TesseralSpinBox->setEnabled(false);
    TesseralSpinBox->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
    TesseralSpinBox->setMaximum(ZonalsSpinBox->value());
    TesseralSpinBox->setValue(m_tesserals);

    gridLayout_4->addWidget(TesseralSpinBox, 2, 1, 1, 1);

    SolarPressureRadioButton->setCheckable(true);
    SolarPressureRadioButton->setChecked(false);
    AlbedoCheckBox->setCheckable(true);
    AlbedoCheckBox->setChecked(false);
    IRCheckBox->setCheckable(true);
    IRCheckBox->setChecked(false);

    connect(GravityFieldRadioButton, SIGNAL(toggled(bool)), GravityEffectsModelComboBox, SLOT(setEnabled(bool)));
    connect(GravityFieldRadioButton, SIGNAL(toggled(bool)), ZonalsSpinBox, SLOT(setEnabled(bool)));
    connect(GravityFieldRadioButton, SIGNAL(toggled(bool)), TesseralSpinBox, SLOT(setEnabled(bool)));
    connect(AtmDragRadioButton, SIGNAL(toggled(bool)), AtmosphereDragTypeComboBox, SLOT(setEnabled(bool)));
    connect(SolarPressureRadioButton, SIGNAL(toggled(bool)), CrSpinBox, SLOT(setEnabled(bool)));
    connect(SolarPressureRadioButton, SIGNAL(toggled(bool)), AlbedoCheckBox, SLOT(setEnabled(bool)));
    connect(SolarPressureRadioButton, SIGNAL(toggled(bool)), IRCheckBox, SLOT(setEnabled(bool)));
    connect(SolarPressureRadioButton, SIGNAL(clicked(bool)), AlbedoCheckBox, SLOT(setChecked(bool)));
    connect(SolarPressureRadioButton, SIGNAL(clicked(bool)), IRCheckBox, SLOT(setChecked(bool)));
    connect(ThirdBodyRadioButton, SIGNAL(toggled(bool)), BodyListWidget, SLOT(setEnabled(bool)));
    connect(ThirdBodyRadioButton, SIGNAL(toggled(bool)), PertBodyListWidget, SLOT(setEnabled(bool)));
    connect(ThirdBodyRadioButton, SIGNAL(toggled(bool)), AddPushButton, SLOT(setEnabled(bool)));
    connect(ThirdBodyRadioButton, SIGNAL(toggled(bool)), RemovePushButton, SLOT(setEnabled(bool)));
    connect(BodyListWidget, SIGNAL(itemDoubleClicked(QListWidgetItem*)), this, SLOT(addPerturbingPlanet(QListWidgetItem*)));
    connect(AddPushButton, SIGNAL(pressed()), this, SLOT(addPerturbingPlanet()));
    connect(RemovePushButton, SIGNAL(pressed()), this, SLOT(removePerturbingPlanet()));
    connect(TesseralSpinBox, SIGNAL(valueChanged(int)), this, SLOT(setTesserals(int)));
    connect(ZonalsSpinBox, SIGNAL(valueChanged(int)), TesseralSpinBox, SLOT(setVariableMaximum(int)));


}

perturbationForcesDialog::~perturbationForcesDialog()
{
}



bool perturbationForcesDialog::loadValues(const ScenarioCentralBodyType* centralBody, const ScenarioPerturbationsForceType* perturbationsToCentralBody)
{
    GravityFieldRadioButton->setChecked(false);
    SolarPressureRadioButton->setChecked(false);
    AtmDragRadioButton->setChecked(false);
    ThirdBodyRadioButton->setChecked(false);

    if (perturbationsToCentralBody == NULL)
    {
        return true;
    }

    bool theAlbedo = perturbationsToCentralBody->albedo();
    bool theAtmosphericDrag = perturbationsToCentralBody->atmosphereDrag();
    bool theSolarPressure = perturbationsToCentralBody->solarPressure();
    bool theGravityEffects = perturbationsToCentralBody->gravityEffets();
    bool the3rdBody = perturbationsToCentralBody->thirdBody();
    bool theIR = perturbationsToCentralBody->IR();

    GravityFieldRadioButton->setChecked(theGravityEffects);
    SolarPressureRadioButton->setChecked(theSolarPressure);
    AtmDragRadioButton->setChecked(theAtmosphericDrag);
    AlbedoCheckBox->setChecked(theAlbedo);
    IRCheckBox->setChecked(theIR);
    ThirdBodyRadioButton->setChecked(the3rdBody);

    if (theGravityEffects)
    {
        GravityFieldRadioButton->setChecked(true);
        if(!centralBody->GravityModel()->modelName().isEmpty())
        {
            QString gravitymodel = centralBody->GravityModel()->modelName();
            gravitymodel.remove(".stad");

            for (int i = 0; i < GravityEffectsModelComboBox->count(); i++)
            {
                if (GravityEffectsModelComboBox->itemData(i) == gravitymodel)
                {
                    GravityEffectsModelComboBox->setCurrentIndex(i);
                    break;
                }
            }
        }

        ZonalsSpinBox->setValue(centralBody->GravityModel()->numberOfZonals());
        m_tesserals = centralBody->GravityModel()->numberOfTesserals();
        TesseralSpinBox->setValue(m_tesserals);
    } // ---- Gravity

    if (theAtmosphericDrag)
    {
        AtmDragRadioButton->setChecked(true);
        if(!centralBody->AtmosphereModel().isEmpty())
        {
            QString atmospheremodel = centralBody->AtmosphereModel();
            atmospheremodel.remove(".stad");

            for (int i = 0; i < AtmosphereDragTypeComboBox->count(); i++)
            {
                if (AtmosphereDragTypeComboBox->itemData(i) == atmospheremodel)
                {
                    AtmosphereDragTypeComboBox->setCurrentIndex(i);
                    break;
                }
            }
        }
    } // ---- Atmopsheric drag

    if (the3rdBody)
    {
        QList<QString> perturbingBodyList = perturbationsToCentralBody->perturbingBody();
        QString nameBody;
        for (int j = 0; j < perturbingBodyList.size(); j++)
        {
            nameBody = perturbingBodyList.at(j);
            QList<QListWidgetItem*> items = BodyListWidget->findItems(nameBody, Qt::MatchFixedString);
            if (nameBody != centralBody->Name())
            {
                PertBodyListWidget->addItem(nameBody);
            }
        }
    } // --- the 3rd Bodies

    return true;
}



bool perturbationForcesDialog::saveValues(ScenarioCentralBodyType* centralBody, ScenarioPerturbationsForceType* perturbationsToCentralBody)
{
    if (GravityFieldRadioButton->isChecked())
    {
        perturbationsToCentralBody->setGravityEffets(true);
        QString gravitymodel = GravityEffectsModelComboBox->itemData(GravityEffectsModelComboBox->currentIndex()).toString();
        gravitymodel.append(".stad");
        centralBody->GravityModel()->setModelName(gravitymodel);
        centralBody->GravityModel()->setNumberOfZonals(ZonalsSpinBox->value());
        centralBody->GravityModel()->setNumberOfTesserals(m_tesserals);
    }
    else
    {
        perturbationsToCentralBody->setGravityEffets(false);
    }

    if (AtmDragRadioButton->isChecked())
    {
        perturbationsToCentralBody->setAtmosphereDrag(true);
        QString atmospheremodel = AtmosphereDragTypeComboBox->itemData(AtmosphereDragTypeComboBox->currentIndex()).toString();
        atmospheremodel.append(".stad");
        centralBody->setAtmosphereModel(atmospheremodel);
    }
    else
    {
        perturbationsToCentralBody->setAtmosphereDrag(false);
    }

    if (SolarPressureRadioButton->isChecked())
    {
        perturbationsToCentralBody->setSolarPressure(true);
        if (AlbedoCheckBox->isChecked())
            perturbationsToCentralBody->setAlbedo(true);
        else
        {
            perturbationsToCentralBody->setAlbedo(false);
            AlbedoCheckBox->setChecked(false);
        }
        if (IRCheckBox->isChecked())
            perturbationsToCentralBody->setIR(true);
        else
        {
            perturbationsToCentralBody->setIR(false);
            IRCheckBox->setChecked(false);
        }
    }
    else
    {
        AlbedoCheckBox->setChecked(false);
        IRCheckBox->setChecked(false);
        perturbationsToCentralBody->setSolarPressure(false);
        perturbationsToCentralBody->setIR(false);
        perturbationsToCentralBody->setAlbedo(false);
    }

    if (ThirdBodyRadioButton->isChecked())
    {
        perturbationsToCentralBody->setThirdBody(true);
        QList<QString> myPerturbingBodyList = perturbationsToCentralBody->perturbingBody();

        myPerturbingBodyList.clear(); // Reset the perturbing bodies list

        for (int j = 0; j < PertBodyListWidget->count(); j++)
        {
            QString myBody = PertBodyListWidget->item(j)->text();
            if (myBody != centralBody->Name()) myPerturbingBodyList.append(myBody);
        }

        perturbationsToCentralBody->setPerturbingBody(myPerturbingBodyList);

    }
    else
    {
        perturbationsToCentralBody->setThirdBody(false);
    }

    return true;
}




void perturbationForcesDialog::addPerturbingPlanet()
{
    QList<QListWidgetItem *> planets = BodyListWidget->selectedItems();
    for (int i = 0; i < planets.size(); i++)
    {
        addPerturbingPlanet(planets.at(i));
    }
}

void perturbationForcesDialog::addPerturbingPlanet(QListWidgetItem* item)
{
    QString text = item->text();
    QIcon theIcon = item->icon();
    int theRow;
    if(PertBodyListWidget->findItems(text,Qt::MatchExactly).isEmpty())
    {
        PertBodyListWidget->addItem(text);
        theRow = PertBodyListWidget->count();
        PertBodyListWidget->item(theRow-1)->setIcon(theIcon);
    }
}

void perturbationForcesDialog::removePerturbingPlanet()
{
    QList<QListWidgetItem *> perturbplanets = PertBodyListWidget->selectedItems();
    for (int i = 0; i < perturbplanets.size(); i++)
    {
        delete perturbplanets.at(i);
    }
}

void perturbationForcesDialog::setTesserals(int i)
{
    m_tesserals = i;
}


TesseralBox::TesseralBox(QDialog* parent) : QSpinBox(parent)
{
}

void TesseralBox::setVariableMaximum(int i)
{
    this->setMaximum(i);
}

