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

//void perturbationForcesDialog::on_Dialog_accepted()
//{
//	qWarning("TODO: %s	%d",__FILE__,__LINE__);
//}

//void perturbationForcesDialog::on_Dialog_rejected()
//{
//	qWarning("TODO: %s	%d",__FILE__,__LINE__);
//}

//void perturbationForcesDialog::on_ZonalsSpinBox_valueChanged(int)
//{
//	qWarning("TODO: %s	%d",__FILE__,__LINE__);
//}

//void perturbationForcesDialog::on_GravityEffectsModelComboBox_currentIndexChanged(int)
//{
//	qWarning("TODO: %s	%d",__FILE__,__LINE__);
//}

//void perturbationForcesDialog::on_TesseralsEditBox_textChanged(const QString&)
//{
//	qWarning("TODO: %s	%d",__FILE__,__LINE__);
//}

//void perturbationForcesDialog::on_AtmosphereDragTypeComboBox_currentIndexChanged(int)
//{
//	qWarning("TODO: %s	%d",__FILE__,__LINE__);
//}

//void perturbationForcesDialog::on_dragDayF10Edit_textChanged(const QString&)
//{
//	qWarning("TODO: %s	%d",__FILE__,__LINE__);
//}

//void perturbationForcesDialog::on_dragAvF10Edit_textChanged(const QString&)
//{
//	qWarning("TODO: %s	%d",__FILE__,__LINE__);
//}

//void perturbationForcesDialog::on_dragGeomagEdit_textChanged(const QString&)
//{
//	qWarning("TODO: %s	%d",__FILE__,__LINE__);
//}

//void perturbationForcesDialog::on_GravityFieldRadioButton_clicked(bool)
//{
//	qWarning("TODO: %s	%d",__FILE__,__LINE__);
//}

//void perturbationForcesDialog::on_AtmDragRadioButton_clicked(bool)
//{
//	qWarning("TODO: %s	%d",__FILE__,__LINE__);
//}

//void perturbationForcesDialog::on_SolarPressureRadioButton_clicked(bool)
//{
//	qWarning("TODO: %s	%d",__FILE__,__LINE__);
//}

//void perturbationForcesDialog::on_DebrisRadioButton_clicked(bool)
//{
//	qWarning("TODO: %s	%d",__FILE__,__LINE__);
//}

//void perturbationForcesDialog::on_AlbedoCheckBox_clicked(bool)
//{
//	qWarning("TODO: %s	%d",__FILE__,__LINE__);
//}

//void perturbationForcesDialog::on_IRCheckBox_clicked(bool)
//{
//	qWarning("TODO: %s	%d",__FILE__,__LINE__);
//}

//void perturbationForcesDialog::on_ThirdBodyRadioButton_clicked(bool)
//{
//	qWarning("TODO: %s	%d",__FILE__,__LINE__);
//}

//void perturbationForcesDialog::on_AddPushButton_clicked(bool)
//{
//	qWarning("TODO: %s	%d",__FILE__,__LINE__);
//}

//void perturbationForcesDialog::on_RemovePushButton_clicked(bool)
//{
//	qWarning("TODO: %s	%d",__FILE__,__LINE__);
//}


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
    if(PertBodyListWidget->findItems(text,Qt::MatchExactly).isEmpty())
        PertBodyListWidget->addItem(text);
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

