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
------------------ Author: Annalisa Riccardi  ---------------------------------------------
 ------------------ E-mail: (nina1983@gmail.com) -------------------------------------------
 Modified by Tiziana Sabatini on July 2009 to support the new perturbations layer
*/
 
#include <QListWidget>
#include <QHeaderView>
#include <QMessageBox> 
#include <QHash>
#include <QDir>
#include <QTextStream>
#include <iostream>
#include <cmath>
#include "RendezVous/rendezvous.h"
#include "Main/scenariotree.h"
#include "Scenario/scenario.h"
#include "Main/treeItemDelegate.h"
#include "Astro-Core/date.h"
#include "Astro-Core/stacoordsys.h"
#include "Astro-Core/stabody.h"

const char* RVD_INPUT_FILE_NAME = "rendezvouspipe.stap";

QTreeWidgetItem* invisibleRoot;
QList<ScenarioSC*> spacevehicles;
static bool flag, dragdropflag;

//RendezvousDialog::RendezvousDialog(ScenarioTree* parent) : QDialog(parent)
RendezvousDialog::RendezvousDialog ( QWidget * parent, Qt::WindowFlags f) : QDialog(parent, f)
{
	flag = false;
	dragdropflag = false;
	
    //m_scenariotree = parent;
	
    setupUi(this);
    
    // Set up the coordinate system combo box
    CoordSystemComboBox->addItem(tr("PLANET FIXED"), (int) sta::COORDSYS_BODYFIXED);
    CoordSystemComboBox->addItem(tr("INERTIAL (J2000)"), (int) sta::COORDSYS_EME_J2000);
    CoordSystemComboBox->addItem(tr("INERTIAL (B1950)"), (int) sta::COORDSYS_EME_B1950);
    CoordSystemComboBox->addItem(tr("ECLIPTIC (J2000)"), (int) sta::COORDSYS_ECLIPTIC_J2000);

    // Set up the central body combo box
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

    //Added combo box to select atmosphere models
    AtmosphereComboBox->addItem(QIcon(":/icons/ComboEarth.png"), tr("gram99"), "gram99");
    AtmosphereComboBox->addItem(QIcon(":/icons/ComboEarth.png"), tr("exponential"), "exponential");
    AtmosphereComboBox->addItem(QIcon(":/icons/ComboEarth.png"), tr("UASA2000"), "UASA2000");
    AtmosphereComboBox->addItem(QIcon(":/icons/ComboMars.png"), tr("mars_emcd"), "mars_emcd");

    GravityComboBox->addItem(QIcon(":/icons/ComboEarth.png"), tr("JGM3"), "JGM3");
    AtmosphereComboBox->setEnabled(false);
    GravityComboBox->setEnabled(false);

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
    
    //Add the tesserals SpinBox
    TesseralSpinBox = new TesseralBox(this);
    TesseralSpinBox->setObjectName(QString::fromUtf8("TesseralSpinBox"));
    TesseralSpinBox->setEnabled(false);
    TesseralSpinBox->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
    TesseralSpinBox->setMaximum(ZonalsSpinBox->value());
    TesseralSpinBox->setValue(m_tesserals);

    gridLayout_8->addWidget(TesseralSpinBox, 2, 1, 1, 1);

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
    connect(ZonalsSpinBox, SIGNAL(valueChanged(int)), TesseralSpinBox, SLOT(setVariableMaximum(int)));

    
    //set up Target list
    
    //QTreeWidgetItem* spacescenarioItem = parent->topLevelItem(0);
    //SpaceScenario* spacescenario = dynamic_cast<SpaceScenario*> (parent->objectForItem(spacescenarioItem));
    
#if OLDSCENARIO
	if(spacescenario){
		QList<ScenarioParticipant*> participants = spacescenario->participants();
		int num = participants.size();
		for(int i=0; i<num; i++){
			QString name = participants.at(i)->elementName();
			if(name=="Spacecraft"){
				targetcomboBox->addItem(participants.at(i)->name());
				spacevehicles.append(participants.at(i));
			} 
		}
	}
#endif

	//set up Manoeuvre list
	QListWidgetItem* FreeDrift = new QListWidgetItem(ManoeuvresListWidget,1000);
	FreeDrift->setText("Free Drift");
	QListWidgetItem* StationKeeping = new QListWidgetItem(ManoeuvresListWidget,1000);
	StationKeeping->setText("Station Keeping");
	QListWidgetItem* DeltaV = new QListWidgetItem(ManoeuvresListWidget,1001);
	DeltaV->setText("Delta V");
	QListWidgetItem* HomannTransfer = new QListWidgetItem(ManoeuvresListWidget,1003);
	HomannTransfer->setText("Homann Transfer");
	QListWidgetItem* Imp2PointTransfer = new QListWidgetItem(ManoeuvresListWidget,1005);
	Imp2PointTransfer->setText("Impulsive Two-point Transfer");
	QListWidgetItem* ImpHoppVbarRadial = new QListWidgetItem(ManoeuvresListWidget,1002);
	ImpHoppVbarRadial->setText("Impulsive Hopping on V-bar in z-direction by radial impulse");
	QListWidgetItem* ImpHoppVbarTg = new QListWidgetItem(ManoeuvresListWidget,1002);
	ImpHoppVbarTg->setText("Impulsive Hopping on V-bar in z-direction by tangential impulse");
	QListWidgetItem* ImpHoppNoVbar = new QListWidgetItem(ManoeuvresListWidget,1002);
	ImpHoppNoVbar->setText("Impulsive Hopping not on V-Bar");
	QListWidgetItem* XThrustTransfer = new QListWidgetItem(ManoeuvresListWidget,1003);
	XThrustTransfer->setText("Continuous tangential transfer (X-thrust transfer)");
	QListWidgetItem* ForcedVbar = new QListWidgetItem(ManoeuvresListWidget,1002);
	ForcedVbar->setText("Forced motion along V-bar");
	QListWidgetItem* ForcedRbar = new QListWidgetItem(ManoeuvresListWidget,1003);
	ForcedRbar->setText("Forced motion along R-bar");
	QListWidgetItem* ContHoppVbar = new QListWidgetItem(ManoeuvresListWidget,1002);
	ContHoppVbar->setText("Continuous Hopping on V-Bar");
	
	ManoeuvresListWidget->addItem(FreeDrift);
	ManoeuvresListWidget->addItem(StationKeeping);
	ManoeuvresListWidget->addItem(DeltaV);
	ManoeuvresListWidget->addItem(HomannTransfer);
	ManoeuvresListWidget->addItem(Imp2PointTransfer);
	ManoeuvresListWidget->addItem(ImpHoppVbarRadial);
	ManoeuvresListWidget->addItem(ImpHoppVbarTg);
	ManoeuvresListWidget->addItem(ImpHoppNoVbar);
	ManoeuvresListWidget->addItem(XThrustTransfer);
	ManoeuvresListWidget->addItem(ForcedVbar);
	ManoeuvresListWidget->addItem(ForcedRbar);
	ManoeuvresListWidget->addItem(ContHoppVbar);
	ManoeuvresListWidget->setSelectionMode(QAbstractItemView::ExtendedSelection);
	ManoeuvresListWidget->adjustSize(); 
	
	//set up Manoeuvre plan tree
	ManoeuvresPlanTreeWidget->setRootIsDecorated(false);
	ManoeuvresPlanTreeWidget->setSelectionMode(QAbstractItemView::ExtendedSelection);
	ManoeuvresPlanTreeWidget->setAcceptDrops(true);
	ManoeuvresPlanTreeWidget->setDragEnabled(true);
	ManoeuvresPlanTreeWidget->setDragDropMode(QAbstractItemView::InternalMove);
	ManoeuvresPlanTreeWidget->setItemDelegate(new TreeItemDelegate(ManoeuvresPlanTreeWidget));
	ManoeuvresPlanTreeWidget->header()->setResizeMode(QHeaderView::ResizeToContents);
	
    //m_manoeuvreplan = new ScenarioManoeuvrePlan();
	invisibleRoot = ManoeuvresPlanTreeWidget->invisibleRootItem();
	ManoeuvresPlanTreeWidget->addTopLevelItem(invisibleRoot);
	
	connect(ManoeuvresListWidget, SIGNAL(itemDoubleClicked(QListWidgetItem*)), this, SLOT(addManoeuvre(QListWidgetItem*)));
	connect(ManoeuvresPlanTreeWidget, SIGNAL(itemChanged(QTreeWidgetItem *, int)), this, SLOT(manoeuvrePlanChanged(QTreeWidgetItem *, int)));
}

RendezvousDialog::~RendezvousDialog()
{
    delete TesseralSpinBox;
}


//SLOTS

void RendezvousDialog::addPerturbingPlanet()
{
	QList<QListWidgetItem *> planets = BodyListWidget->selectedItems();
	for (int i = 0; i < planets.size(); i++){
		addPerturbingPlanet(planets.at(i));
	}
}

void RendezvousDialog::addPerturbingPlanet(QListWidgetItem* item)
{
	QString text = item->text();
	if(PertBodyListWidget->findItems(text,Qt::MatchExactly).isEmpty())
		PertBodyListWidget->addItem(text);
}

void RendezvousDialog::removePerturbingPlanet()
{
	QList<QListWidgetItem *> perturbplanets = PertBodyListWidget->selectedItems();
	for (int i = 0; i < perturbplanets.size(); i++){
		delete perturbplanets.at(i);
	}
}


/*
 * Tanking the manoeuvre name form the QListWidget, this method constructs a new ScenarioManoeuvre 
 * element, adds it to the m_manoeuvreplan global variable, and create the respective QTreeWidgetItem
 * to attach to the Manoeuvre Plan QTreeWidgets
 */
void RendezvousDialog::addManoeuvre(QListWidgetItem* manoeuvreWidget)
{
#if OLDSCENARIO
	flag = true;
	ScenarioManoeuvre* manoeuvre = new ScenarioManoeuvre();
	QString title = manoeuvre->nameMap().key(manoeuvreWidget->text());
	manoeuvre->setName(title);
	manoeuvre->setActive(true);
	manoeuvre->setManoeuvreInputs();
	
	m_manoeuvreplan->addManoeuvre(manoeuvre);
	
	disconnect(ManoeuvresPlanTreeWidget, SIGNAL(itemChanged(QTreeWidgetItem *, int)), this, SLOT(manoeuvrePlanChanged(QTreeWidgetItem *, int)));
	
	QTreeWidgetItem* manoeuvreItem = manoeuvre->createItem(invisibleRoot);
	manoeuvreItem->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsDragEnabled | Qt::ItemIsUserCheckable);
	manoeuvreItem->setCheckState(0,Qt::Checked);
	manoeuvreItem->setExpanded(true);
	int num = manoeuvreItem->childCount();
	for(int i=0; i<num; i++)
		manoeuvreItem->child(i)->setFlags(Qt::ItemIsEnabled | Qt::ItemIsEditable);
	
#endif

	connect(ManoeuvresPlanTreeWidget, SIGNAL(itemChanged(QTreeWidgetItem *, int)), this, SLOT(manoeuvrePlanChanged(QTreeWidgetItem *, int)));
}

/*
 * slot called from AddButton. It adds the selected manoeuvres in the QListWidget to the 
 * QTreeWidget representing the Manoeuvre Plan
 */
void RendezvousDialog::on_AddButton_clicked()
{
	QList<QListWidgetItem*> manoeuvreList = ManoeuvresListWidget->selectedItems(); 
	for (int i = 0; i < manoeuvreList.size(); i++){
		addManoeuvre(manoeuvreList.at(i));
	}
}

/*
 * slot called from DeleteButton. It remove the selected manoeuvre from the QTreeWidget 
 * representing the ManoeuvrePlan and from the list of all the manoeuvres.
 */
void RendezvousDialog::on_DeleteButton_clicked()
{
#if OLDSCENARIO
	QList<QTreeWidgetItem *>  listOfItem = ManoeuvresPlanTreeWidget->selectedItems();
	if(!listOfItem.isEmpty()){
		int saveBox = QMessageBox::warning(this,tr("RendezVous"),tr("Are you sure you want to delete the selected manoeuvres?"),QMessageBox::Yes | QMessageBox::No ,QMessageBox::Yes);
		if (saveBox == QMessageBox::Yes) {
	    	for (int i = 0; i < listOfItem.size(); i++){
	    		ScenarioManoeuvre* manoeuvre = manoeuvreForItem(listOfItem.at(i));
	    		delete listOfItem.at(i);
	    		m_manoeuvreplan->manoeuvresList().removeOne(manoeuvre);
	    	}
	    	return;
	    }
	    if (saveBox == QMessageBox::No){
	    	return;
	    }
	}
#endif
}

/*
 * slot called from ClearAllButton. It removes all the elements from the QTreeWidget 
 * representing the ManoeuvrePlan and from the list of all the manoeuvres.
 */
void RendezvousDialog::on_ClearAllButton_clicked()
{
#if OLDSCENARIO
	int saveBox = QMessageBox::warning(this,tr("RendezVous"),tr("Are you sure you want to delete all the manoeuvres?"),QMessageBox::Yes | QMessageBox::No ,QMessageBox::Yes);
    if (saveBox == QMessageBox::Yes) {
    	ManoeuvresPlanTreeWidget->clear();
    	invisibleRoot = ManoeuvresPlanTreeWidget->invisibleRootItem();
    	m_manoeuvreplan->manoeuvresList().clear();
    	return;
    }
    if (saveBox == QMessageBox::No){
    	return;
    }
#endif
}

void RendezvousDialog::on_OKButton_clicked()
{
	if(isDragDropHappened()) {
		reorderManoeuvrePlanTree();
	}
	accept();
}

void RendezvousDialog::on_CancelButton_clicked()
{
	save_changes();
	reject();
}

/*
 * slot called from QTreeWidget::itemChanged(). If some items in the tree changes (it means active/inactive or 
 * changes in the values ) the respective manoeuvre object is update 
 */
bool RendezvousDialog::manoeuvrePlanChanged(QTreeWidgetItem* item, int column)
{
#if OLDSCENARIO
	ScenarioManoeuvre* manoeuvre = manoeuvreForItem(item);
	if(manoeuvre){
		flag = true;
		if(column==0){
			if(item->checkState(0)==Qt::Unchecked) {
				manoeuvre->setActive(false);
			}
			else {
				manoeuvre->setActive(true);
			}
		}
	}
	else if(item->childCount()==0){
		if(column==1){
			flag = true;
			ScenarioManoeuvre* manoeuvre = manoeuvreForItem(item->parent());
			foreach(ManoeuvreInput* input, manoeuvre->manoeuvreInputs()){
				if (input->getNameMap().value(input->name()) == item->text(0)){
					input->setValue(item->text(1).toDouble());
				}
			}
		}
	}
#endif
	return flag;
}


// FUNCTION

#if OLDSCENARIO
ScenarioManoeuvre* RendezvousDialog::manoeuvreForItem(QTreeWidgetItem* item) const
{
    ScenarioObject* scenarioObject = m_scenariotree->objectForItem(item);
    return dynamic_cast<ScenarioManoeuvre*>(scenarioObject);
}
#endif

/*
 * This function is called if a drag and drop action take place. It reorders the manoeuvre list 
 * contained in the variable m_manoeuvreplan.
 */
void RendezvousDialog::reorderManoeuvrePlanTree()
{
#if OLDSCENARIO
	int numManoeuvre = invisibleRoot->childCount();
	if(numManoeuvre!=0){
		for(int i=0; i<numManoeuvre; i++){
			ScenarioManoeuvre* manoeuvre = manoeuvreForItem(invisibleRoot->child(i));
			if(m_manoeuvreplan->manoeuvresList().at(i) != manoeuvre){
				int index = m_manoeuvreplan->manoeuvresList().indexOf(manoeuvre,0);
				m_manoeuvreplan->manoeuvresList().swap(i,index);				
			}
		}
	}
#endif
}


/*
 * This functions rises the warning message if some changes in the Manoeuvre Plan are occurred
 */
void RendezvousDialog::save_changes()
{
#if OLDSCENARIO
	if(isDragDropHappened()) {
		dragdropflag = true;
	}
	if(flag || dragdropflag){
		int saveBox = QMessageBox::warning(this,tr("RendezVous"),tr("The order of the Manoeuvre Plan has been modified.\n Do you want to save your changes?"),QMessageBox::Save | QMessageBox::Discard | QMessageBox::Discard, QMessageBox::Save);
        if (saveBox == QMessageBox::Save) {
        	if(dragdropflag){
        		reorderManoeuvrePlanTree();
        		displayManoeuvrePlan();
        	}
        	flag = false;
        	dragdropflag = false;
        	return;
        }
        if (saveBox == QMessageBox::Discard){
        	if(dragdropflag)
        		displayManoeuvrePlan();
        	flag = false;
        	dragdropflag = false;
        	return;
        }
		return;
	}
#endif
}

void RendezvousDialog::displayManoeuvrePlan()
{
#if OLDSCENARIO
	QList<ScenarioManoeuvre*> manoeuvreList = m_manoeuvreplan->manoeuvresList();
	
	disconnect(ManoeuvresPlanTreeWidget, SIGNAL(itemChanged(QTreeWidgetItem *, int)), this, SLOT(manoeuvrePlanChanged(QTreeWidgetItem *, int)));
	
	ManoeuvresPlanTreeWidget->clear();
	invisibleRoot = ManoeuvresPlanTreeWidget->invisibleRootItem();
	for(int i=0; i<manoeuvreList.size(); i++){
		QTreeWidgetItem* manoeuvreItem = manoeuvreList.at(i)->createItem(invisibleRoot);
		manoeuvreItem->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsDragEnabled | Qt::ItemIsUserCheckable);
		if(manoeuvreList.at(i)->isActive()){
			manoeuvreItem->setCheckState(0,Qt::Checked);
		}
		else{
			manoeuvreItem->setCheckState(0,Qt::Unchecked);
		}
		manoeuvreItem->setExpanded(true);
		int num = manoeuvreItem->childCount();
		for(int i=0; i<num; i++){
			manoeuvreItem->child(i)->setFlags(Qt::ItemIsEnabled | Qt::ItemIsEditable);
		}
	}
	
	connect(ManoeuvresPlanTreeWidget, SIGNAL(itemChanged(QTreeWidgetItem *, int)), this, SLOT(manoeuvrePlanChanged(QTreeWidgetItem *, int)));
#endif
}

bool RendezvousDialog::isDragDropHappened()
{
#if OLDSCENARIO
	QList<ScenarioManoeuvre* > currentManoeuvreList;
	QList<ScenarioManoeuvre* > storedManoeuvreList = m_manoeuvreplan->manoeuvresList();
	int num = invisibleRoot->childCount();
	
	for(int i=0; i<num; i++) {
		currentManoeuvreList.append(manoeuvreForItem(invisibleRoot->child(i)));
	}
	
	if (storedManoeuvreList == currentManoeuvreList) {
			return false;
	}
#endif

	return true;
}

bool RendezvousDialog::loadValues(ScenarioRendezvousTrajectory* rendezvous)
{
#if OLDSCENARIO
	ScenarioEnvironment* environment = rendezvous->environment();
	ScenarioTargettingSimulationParameters* parameters = rendezvous->simulationParameters();
	QString targetName = rendezvous->target();
	ScenarioManoeuvrePlan* manoeuvreplan = rendezvous->manoeuvrePlan();
	
	if(loadValues(environment) && loadValues(parameters) && loadValues(manoeuvreplan)){
	    // Set the target combo box name
		//TODO: check if it works
	    for (int i = 0; i < targetcomboBox->count(); i++)
	    {
	        if (targetcomboBox->currentText() == targetName)
	        {
	        	targetcomboBox->setCurrentIndex(i);
	            break;
	        }
	    }
		return true;
	}
#endif
	return false;
}

bool RendezvousDialog::loadValues(ScenarioEnvironment* environment)
{
#if OLDSCENARIO
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
                    if(!items.isEmpty())
                    {
                        PertBodyListWidget->addItem(nameBody.toUpper());
                    }
                    else return false;
                }
            }
        }
    }
#endif
    return true;
}


bool RendezvousDialog::loadValues(ScenarioTargettingSimulationParameters* parameters)
{
#if OLDSCENARIO
	ScenarioTimeline* timeline = parameters->timeline();
	ScenarioInitialStatePosition* initialStatePos = parameters->initialStatePosition();

	QDateTime dateTime = sta::JdToCalendar(sta::MjdToJd(timeline->startTime()));
	
	if(timeline){
		dateTimeEdit->setDateTime(dateTime);
		TimeStepLineEdit->setText(QString::number(timeline->timeStep()));
	}
	else return false;
	
	if(initialStatePos){
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
	    Q_ASSERT(elements || stateVector);
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
	    	InitialStateComboBox->setCurrentIndex(0);
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
#endif

	return true;
}

//TODO: check if it works
bool RendezvousDialog::loadValues(ScenarioManoeuvrePlan* manoeuvreplan)
{
	if(manoeuvreplan){
		m_manoeuvreplan = manoeuvreplan;
		displayManoeuvrePlan();
		return true;
	}
	return false;
}

bool RendezvousDialog::saveValues(ScenarioRendezvousTrajectory* rendezvous)
{
#if OLDSCENARIO
	ScenarioEnvironment* environment = rendezvous->environment();
	ScenarioTargettingSimulationParameters* parameters = rendezvous->simulationParameters();
	ScenarioManoeuvrePlan* manoeuvreplan = rendezvous->manoeuvrePlan();
	
	if(saveValues(environment) && saveValues(parameters) && saveValues(manoeuvreplan)){
		rendezvous->setTarget(targetcomboBox->currentText());
		return true;
	}
#endif

	return false;
}

bool RendezvousDialog::saveValues(ScenarioEnvironment* environment)
{
#if OLDSCENARIO
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

        QString gravitymodel = GravityComboBox->currentText();
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

        QString atmospheremodel = AtmosphereComboBox->currentText();
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
            if(body)
            {
                ScenarioBody* perturbingbody = new ScenarioBody(body);
                externalbodiesPerturbation->addPerturbingBody(perturbingbody);
            }
        }
        environment->addPerturbation(externalbodiesPerturbation);
    }
#endif
    return true;
}


bool RendezvousDialog::saveValues(ScenarioTargettingSimulationParameters* parameters)
{
#if OLDSCENARIO
	ScenarioTimeline* timeline = new ScenarioTimeline();
	ScenarioInitialStatePosition*  initialStatePos = new ScenarioInitialStatePosition();
	timeline->setStartTime(sta::JdToMjd(sta::CalendarToJd(dateTimeEdit->dateTime())));
	timeline->setTimeStep(TimeStepLineEdit->text().toDouble());
	
	parameters->setTimeline(timeline);
	
    int coordSysIndex = CoordSystemComboBox->currentIndex();
    initialStatePos->setCoordinateSystem((sta::CoordinateSystemType) CoordSystemComboBox->itemData(coordSysIndex).toInt());
    
    if (InitialStateComboBox->currentIndex() == 0)
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
    else
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
#endif

	return true;
}

bool RendezvousDialog::saveValues(ScenarioManoeuvrePlan* manoeuvreplan)
{
	return true;
}


void RendezvousDialog::writeInputFile()
{
#if OLDSCENARIO
	QString RVDInputFilePath = QDir::temp().filePath(RVD_INPUT_FILE_NAME);
	QFile RVDInputFile(RVDInputFilePath);
	QTextStream RVDInputStream(&RVDInputFile);
	if (!RVDInputFile.open(QIODevice::WriteOnly | QIODevice::Text))
	{
	    QMessageBox::warning(this, tr("File Error"), tr("Unable to open RVD input file '%1'").arg(RVDInputFilePath));
	    return;
	}
	RVDInputStream.setRealNumberPrecision(16);
	
	double a,e,i,w0,o0,t0;
	double a1,e1,i1,w1,o1,t1;
	
	//write on the input file
	if(m_chaser && m_target){
		
		// TIMELINE
		/*ScenarioTimeline* timeline = m_chaser->trajectoryPlan()->trajectories().at(m_indexTrajectoryTarget)->simulationParameters()->timeline();
		double startJTime = timeline->startTime();
		RVDInputStream << startJTime <<"\n";
		RVDInputStream << timeline->timeStep()<<"\n";
		
		// CENTRAL BODY
		ScenarioEnvironment* environmentChaser = m_chaser->trajectoryPlan()->trajectories().at(m_indexTrajectoryTarget)->environment();
		const StaBody* centralbodyChaser = environmentChaser->centralBody();
		QList<StaBodyId> perturbingBodyListID;
		for(int j=0; j<environmentChaser->perturbingBodyList().size();j++){
			perturbingBodyListID.append(environmentChaser->perturbingBodyList().at(j)->id());
		}
		int idconvert = (centralbodyChaser->id()-99)/100;
		if(idconvert > 9 ||  idconvert < 1) {
			QMessageBox::warning(this, tr("Warning"), tr("The central body of the manoeuvre can be only one planet of the solar system."));
			return;
		}
		RVDInputStream << idconvert <<"\n";
		
		// TARGET		
		ScenarioEnvironment* environmentTarget = m_chaser->trajectoryPlan()->trajectories().at(m_indexTrajectoryTarget)->environment();
		const StaBody* centralbodyTarget = environmentTarget->centralBody();
		if(centralbodyTarget->id()!=centralbodyChaser->id()){
			QMessageBox::warning(this, tr("Warning"), tr("Target and Chaser must have the same central body"));
			return;
		}
		ScenarioInitialStatePosition* initialstateTarget = m_target->trajectoryPlan()->trajectories().at(m_indexTrajectoryTarget)->simulationParameters()->initialStatePosition();
		ScenarioCoordinateSystem coordinatesystemTarget = initialstateTarget->coordinateSystem();
		
		if(initialstateTarget->initialState()->displayName()=="State Vector"){
			ScenarioStateVector* statevector = (ScenarioStateVector*)(initialstateTarget->initialState());
			cartesianTOorbital(centralbodyTarget->mu(),statevector->position().x, statevector->position().y, statevector->position().z, statevector->velocity().x, statevector->velocity().y, statevector->velocity().z ,a,e,i,w0,o0,t0);
		}
		else{
			ScenarioKeplerianElements* keplelements = (ScenarioKeplerianElements*)(initialstateTarget->initialState());
			a = keplelements->semimajorAxis();
			e = keplelements->eccentricity();
			i = keplelements->inclination();
			w0 = keplelements->argumentOfPeriapsis();
			o0 = keplelements->raan();
			t0 = keplelements->trueAnomaly();
		}
		if(e!=0){
			QMessageBox::warning(this, tr("Warning"), tr("Only circular orbits."));
			return;
		}
		// TARGET ORBITAL PARAMETERS
		RVDInputStream << a-centralbodyChaser->meanRadius()<<"\n";
		RVDInputStream << i<<"\n";
		RVDInputStream << o0<<"\n";
		RVDInputStream << t0<<"\n";
		RVDInputStream << w0<<"\n";
		RVDInputStream << e<<"\n";
		
		// TARGET PROPERTIES
		ScenarioPhysicalCharacteristics* targetphysicalproperties = m_target->properties()->physical();
		ScenarioTechnicalCharacteristics* targettechnicalproperties = m_target->properties()->technical();
		RVDInputStream << targetphysicalproperties->mass()<<"\n";
		RVDInputStream << targettechnicalproperties->dragCoefficient()<<"\n";
		RVDInputStream << targetphysicalproperties->surfaceArea()<<"\n";
		RVDInputStream << targettechnicalproperties->reflectionCoefficient()<<"\n";
		
		// CHASER X-Z POSITION
		ScenarioInitialStatePosition* initialstateChaser = m_chaser->trajectoryPlan()->trajectories().at(m_indexTrajectoryChaser)->simulationParameters()->initialStatePosition();
		ScenarioCoordinateSystem coordinatesystemChaser = initialstateChaser->coordinateSystem();
		
		if(coordinatesystemTarget!=SCENARIO_EME_J2000 || coordinatesystemChaser!=SCENARIO_EME_J2000){
				QMessageBox::warning(this, tr("Warning"), tr("For the time being only inertial coordinate system are accepted."));
				return;
		}
		double x,y,z,xd,yd,zd;
		double xl,yl,zl;
		if(initialstateChaser->initialState()->displayName()=="State Vector"){
			ScenarioStateVector* statevectorChaser = (ScenarioStateVector*)(initialstateTarget->initialState());
			x = statevectorChaser->position().x;
			y = statevectorChaser->position().y;
			z = statevectorChaser->position().z;
			cartesianTOorbital(centralbodyChaser->mu(), x, y, z, statevectorChaser->velocity().x, statevectorChaser->velocity().y, statevectorChaser->velocity().z ,a1,e1,i1,w1,o1,t1);
		}
		else{
			ScenarioKeplerianElements* keplelements = (ScenarioKeplerianElements*)(initialstateChaser->initialState());
			a1 = keplelements->semimajorAxis();
			e1 = keplelements->eccentricity();
			i1 = keplelements->inclination();
			w1 = keplelements->argumentOfPeriapsis();
			o1 = keplelements->raan();
			t1 = keplelements->trueAnomaly();
			double E = asin(sin(t1)*sqrt(1-e1*e1)/(1+e1*cos(t1)));
			double m0 = E-e1*sin(E); 
			orbitalTOcartesian(centralbodyChaser->mu(),a1,e1,i1,w1,o1,m0,x,y,z,xd,yd,zd);
		}
		inertialTOlocal(x,y,z,i1,o1,t1,w1,xl,yl,zl);
		RVDInputStream << x <<"\n";
		RVDInputStream << z <<"\n";
		
		// CHASER PROPERTIES
		ScenarioPhysicalCharacteristics* chaserphysicalproperties = m_target->properties()->physical();
		ScenarioTechnicalCharacteristics* chasertechnicalproperties = m_target->properties()->technical();
		RVDInputStream << chaserphysicalproperties->mass()<<"\n";\
		// TODO: Propulsion element missed
		RVDInputStream << "ISPPPPPPPPPPPPPP"<<"\n";
		RVDInputStream << chasertechnicalproperties->dragCoefficient()<<"\n";
		RVDInputStream << chaserphysicalproperties->surfaceArea()<<"\n";
		RVDInputStream << chasertechnicalproperties->reflectionCoefficient()<<"\n";
		
		// PERTURBATION
		
		RVDInputStream << environmentChaser->solarPressureEnabled()<<"\n";
		RVDInputStream << environmentChaser->atmosphericDragEnabled()<<"\n";
		
		ScenarioTrajectoryPropagation* chaserpropagator = m_chaser->trajectoryPlan()->trajectories().at(m_indexTrajectoryTarget)->trajectoryPropagation();	
		// TODO: propagator j2, j4, ...
		RVDInputStream << chaserpropagator->propagator()<<"\n";
		
		// THIRD BODIES
		for(int i=0; i<16; i++){
			if(perturbingBodyListID.contains(STA_MERCURY)) RVDInputStream << 1 <<"\n";
			else RVDInputStream << 0 <<"\n";
			if(perturbingBodyListID.contains(STA_VENUS)) RVDInputStream << 1 <<"\n";
			else RVDInputStream << 0 <<"\n";
			if(perturbingBodyListID.contains(STA_EARTH)) RVDInputStream << 1 <<"\n";
			else RVDInputStream << 0 <<"\n";
			if(perturbingBodyListID.contains(STA_MARS)) RVDInputStream << 1 <<"\n";
			else RVDInputStream << 0 <<"\n";
			if(perturbingBodyListID.contains(STA_JUPITER)) RVDInputStream << 1 <<"\n";
			else RVDInputStream << 0 <<"\n";
			if(perturbingBodyListID.contains(STA_SATURN)) RVDInputStream << 1 <<"\n";
			else RVDInputStream << 0 <<"\n";
			if(perturbingBodyListID.contains(STA_URANUS)) RVDInputStream << 1 <<"\n";
			else RVDInputStream << 0 <<"\n";
			if(perturbingBodyListID.contains(STA_NEPTUNE)) RVDInputStream << 1 <<"\n";
			else RVDInputStream << 0 <<"\n";
			if(perturbingBodyListID.contains(STA_PLUTO)) RVDInputStream << 1 <<"\n";
			else RVDInputStream << 0 <<"\n";
			if(perturbingBodyListID.contains(STA_SUN)) RVDInputStream << 1 <<"\n";
			else RVDInputStream << 0 <<"\n";
			if(perturbingBodyListID.contains(STA_MOON)) RVDInputStream << 1 <<"\n";
			else RVDInputStream << 0 <<"\n";
			if(perturbingBodyListID.contains(STA_DEIMOS)) RVDInputStream << 1 <<"\n";
			else RVDInputStream << 0 <<"\n";
			if(perturbingBodyListID.contains(STA_EUROPA)) RVDInputStream << 1 <<"\n";
			else RVDInputStream << 0 <<"\n";
			if(perturbingBodyListID.contains(STA_TITAN)) RVDInputStream << 1 <<"\n";
			else RVDInputStream << 0 <<"\n";
			if(perturbingBodyListID.contains(STA_ENCELADUS)) RVDInputStream << 1 <<"\n";
			else RVDInputStream << 0 <<"\n";
			if(perturbingBodyListID.contains(STA_TRITON)) RVDInputStream << 1 <<"\n";
			else RVDInputStream << 0 <<"\n";
		}
	}
	
	// MANOEUVRES
	QList<ScenarioManoeuvre*> listManoeuvres = m_manoeuvreplan->manoeuvresList();
	if(listManoeuvres.isEmpty()) {
		QMessageBox::warning(this, tr("Warning"), tr("Please, input at least one manoeuvre."));
		return;
	}
	int manoeuvreCount = 0;
	for(int i=0; i<listManoeuvres.size(); i++){
		if(listManoeuvres.at(i)->isActive()) manoeuvreCount++;
	}
	RVDInputStream << manoeuvreCount << "\n";
	for(int i=0; i<listManoeuvres.size(); i++){
		ScenarioManoeuvre* manoeuvre = listManoeuvres.at(i);
		if(manoeuvre->isActive()){
			RVDInputStream << m_manoeuvreplan->identificatorMap().value(manoeuvre->name())<<"\n";
			switch(manoeuvre->type()){
			case STA_MANOEUVRE_DURATION:
				if(manoeuvre->manoeuvreInputs().at(0)->value()>0){
					RVDInputStream << manoeuvre->manoeuvreInputs().at(0)->value()<<"\n";
					for(int j=0; j<6; j++) RVDInputStream << 0<<"\n";
					break;
				}
				else{
					QMessageBox::warning(this, tr("Warning"), tr("The duration time must be positive."));
					return;
				}
			case STA_MANOEUVRE_DELTAV:
				RVDInputStream << manoeuvre->manoeuvreInputs().at(3)->value()<<"\n";
				RVDInputStream << 0<<"\n";
				RVDInputStream << 0<<"\n";
				RVDInputStream << 0<<"\n";
				RVDInputStream << manoeuvre->manoeuvreInputs().at(0)->value()<<"\n";
				RVDInputStream << manoeuvre->manoeuvreInputs().at(1)->value()<<"\n";
				RVDInputStream << manoeuvre->manoeuvreInputs().at(2)->value()<<"\n";
				break;
			case STA_MANOEUVRE_V_POSITION:
				RVDInputStream << 0<<"\n";
				RVDInputStream << manoeuvre->manoeuvreInputs().at(0)->value()<<"\n";
				RVDInputStream << 0<<"\n";
				RVDInputStream << 0<<"\n";
				RVDInputStream << 0<<"\n";
				RVDInputStream << 0<<"\n";
				RVDInputStream << 0<<"\n";
				break;
			case STA_MANOEUVRE_R_POSITION:
				RVDInputStream << 0<<"\n";
				RVDInputStream << 0<<"\n";
				RVDInputStream << 0<<"\n";
				RVDInputStream << manoeuvre->manoeuvreInputs().at(0)->value()<<"\n";
				RVDInputStream << 0<<"\n";
				RVDInputStream << 0<<"\n";
				RVDInputStream << 0<<"\n";
				break;
			case STA_MANOEUVRE_V_R_POSITION:
				RVDInputStream << 0<<"\n";
				RVDInputStream << manoeuvre->manoeuvreInputs().at(0)->value()<<"\n";
				RVDInputStream << 0<<"\n";
				RVDInputStream << manoeuvre->manoeuvreInputs().at(1)->value()<<"\n";
				RVDInputStream << 0<<"\n";
				RVDInputStream << 0<<"\n";
				RVDInputStream << 0<<"\n";
				break;
			case STA_MANOEUVRE_V_R_DURATION:
				RVDInputStream << manoeuvre->manoeuvreInputs().at(0)->value()<<"\n";
				RVDInputStream << manoeuvre->manoeuvreInputs().at(1)->value()<<"\n";
				RVDInputStream << 0<<"\n";
				RVDInputStream << manoeuvre->manoeuvreInputs().at(2)->value()<<"\n";
				RVDInputStream << 0<<"\n";
				RVDInputStream << 0<<"\n";
				RVDInputStream << 0<<"\n";
				break;
			}
		}*/
	}
	RVDInputFile.close();
	accept();
#endif
}
