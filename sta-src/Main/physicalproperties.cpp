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
 ------------------ Author: Tiziana Sabatini  -------------------------------------------------
 ------------------ E-mail: (tiziana.sabatini@yahoo.it) ----------------------------
 */

#include "scenariotree.h"
#include "physicalproperties.h"
#include "Scenario/scenario.h"
#include "RAM/parametrization.h"
#include <QDebug>

PhysicalPropertiesDialog::PhysicalPropertiesDialog(ScenarioTree* parent):
        QDialog(parent)
{
    setupUi(this);

    QDoubleValidator* positiveDoubleValidator = new QDoubleValidator(this);
    positiveDoubleValidator->setBottom(0.0);
    loadGeomDialog = new QFileDialog(this, "Select a file:", "data/vehiclewgs/");

    connect(geomFileButton,SIGNAL(clicked()),loadGeomDialog,SLOT(exec()));
    connect(loadGeomDialog,SIGNAL(fileSelected(QString)),this,SLOT(writeGeomFile(QString)));
    //ParametrizationDialog* editDialog = new ParametrizationDialog();
    //QObject::connect(geomParaButton,SIGNAL(clicked()),editDialog,SLOT(exec()));
    /*
    connect(FileRadioButton, SIGNAL(toggled(bool)), lineEditGeomFile, SLOT(setEnabled(bool)));
    connect(FileRadioButton, SIGNAL(toggled(bool)), geomFileButton, SLOT(setEnabled(bool)));
    connect(ParaRadioButton, SIGNAL(toggled(bool)), lineEditGeomPara, SLOT(setEnabled(bool)));
    connect(ParaRadioButton, SIGNAL(toggled(bool)), geomParaButton, SLOT(setEnabled(bool)));
*/

    lineEditMass->setValidator(positiveDoubleValidator);
    lineEditSurface->setValidator(positiveDoubleValidator);
    lineEditVolume->setValidator(positiveDoubleValidator);
    //lineEditRadiusBase->setValidator(positiveDoubleValidator);
    lineEditRadiusNose->setValidator(positiveDoubleValidator);
    //layout()->setSizeConstraint(QLayout::SetFixedSize);
}

bool PhysicalPropertiesDialog::loadValues(ScenarioREVGeometryType* REVgeometry)
{

   /*
    if(physicalcharacteristics)
    {
        lineEditMass->setText(QString::number(physicalcharacteristics->mass()));
        lineEditSurface->setText(QString::number(physicalcharacteristics->surfaceArea()));
        lineEditVolume->setText(QString::number(physicalcharacteristics->volume()));
    }
    else return false;
    */
    //lineEditVolume->setText(QString::number(REVgeometry->totalVolume()));
    //lineEditSurface->setText(QString::number(REVgeometry->surfaceArea()));
    lineEditRadiusNose->setText(QString::number(REVgeometry->noseRadius()));
    if(REVgeometry->shapeFamily()->value()=="" && REVgeometry->geometryFile()!="")
        lineEditGeomFile->setText(REVgeometry->geometryFile());
    else if(REVgeometry->shapeFamily()->value()!="")
        lineEditGeomPara->setText(REVgeometry->geometryFile());



    return true;
}

bool PhysicalPropertiesDialog::saveValues(ScenarioREVGeometryType* REVgeometry){


        //physicalcharacteristics->setMass(lineEditMass->text().toDouble());
        //physicalcharacteristics->setSurfaceArea(lineEditSurface->text().toDouble());
        //physicalcharacteristics->setVolume(lineEditVolume->text().toDouble());

        //geometricalcharacteristics->setRadiusBase(lineEditRadiusBase->text().toDouble());
        REVgeometry->setNoseRadius(lineEditRadiusNose->text().toDouble());
        //REVgeometry->setSurfaceArea(lineEditSurface->text().toDouble());
        //REVgeometry->setTotalVolume(lineEditVolume->text().toDouble());

//        propulsion->setEngineCount(lineEditNEngine->text().toUInt());
//        propulsion->setPropellantMass(lineEditPropellant->text().toDouble());
//        propulsion->setSpecificImpulse(lineEditIsp->text().toDouble());
//        propulsion->setThrustPerEngine(lineEditThrust->text().toDouble());
//        propulsion->setType(comboBoxTypePropulsion->currentText());

        //physicalProperties->setPhysicalCharacteristics(physicalcharacteristics);
        //physicalProperties->setGeometricalCharacteristics(geometricalcharacteristics);
        //tc->setPropulsion(propulsion);

        return true;
}

//void PropertiesDialog::openSEM(){
//        if(m_semDialog == NULL)
//                m_semDialog = new SEM(this);
//        m_semDialog->show();
//        m_semDialog->raise();
//        m_semDialog->activateWindow();
//}

//void PropertiesDialog::reshape(){
//        SubsystemDesignButton->setVisible(!SubsystemDesignButton->isVisible());
//        AddButton->setVisible(!AddButton->isVisible());
//        OverviewGroupBox->setVisible(!OverviewGroupBox->isVisible());
//        line->setVisible(!line->isVisible());
//}

void PhysicalPropertiesDialog::writeGeomFile(QString filename)
{
    filename.remove(this->loadGeomDialog->directory().path() + "/");
    lineEditGeomFile->setText(filename);
}

void PhysicalPropertiesDialog::on_geomParaButton_clicked()
{
    ParametrizationDialog dialog(this);
    dialog.exec();
}
