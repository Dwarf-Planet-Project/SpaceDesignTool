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
#include "Astro-Core/stamath.h"
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
    fileLoaded=0;
    connect(geomFileButton,SIGNAL(clicked()),loadGeomDialog,SLOT(exec()));
    connect(loadGeomDialog,SIGNAL(fileSelected(QString)),this,SLOT(writeGeomFile(QString)));
    connect(loadGeomDialog,SIGNAL(fileSelected(QString)),this,SLOT(setGlobalCharacteristics(QString)));

    //ParametrizationDialog* editDialog = new ParametrizationDialog();
    //QObject::connect(geomParaButton,SIGNAL(clicked()),editDialog,SLOT(exec()));


    //lineEditMass->setValidator(positiveDoubleValidator);
    lineEditSurface->setValidator(positiveDoubleValidator);
    lineEditVolume->setValidator(positiveDoubleValidator);
    //lineEditRadiusBase->setValidator(positiveDoubleValidator);
    lineEditRadiusNose->setValidator(positiveDoubleValidator);
    //layout()->setSizeConstraint(QLayout::SetFixedSize);
}

bool PhysicalPropertiesDialog::loadValues(ScenarioREVGeometryType* REVgeometry)
{
    m_geometry=REVgeometry;
    lineEditRadiusNose->setText(QString::number(REVgeometry->noseRadius()));
    lineEditSurface->setText(QString::number(REVgeometry->REVsurface()));
    lineEditVolume->setText(QString::number(REVgeometry->REVvolume()));
    fileLineEdit->setText(REVgeometry->geometryFile());
    return true;
}

bool PhysicalPropertiesDialog::saveValues(ScenarioREVGeometryType* REVgeometry)
{
    REVgeometry->setNoseRadius(lineEditRadiusNose->text().toDouble());
    REVgeometry->setREVsurface(lineEditSurface->text().toDouble());
    REVgeometry->setREVvolume(lineEditVolume->text().toDouble());

    REVgeometry->setGeometryFile(fileLineEdit->text());
    if(fileLoaded)
        REVgeometry->shapeFamily()->setValue(QString(""));
    return true;
}

void PhysicalPropertiesDialog::writeGeomFile(QString filename)
{
    filename.remove(this->loadGeomDialog->directory().path() + "/");
    fileLineEdit->setText(filename);
    //fileLoaded=1;
}

void PhysicalPropertiesDialog::on_geomParaButton_clicked()
{
    double Rn, Vol, Surf;
    ParametrizationDialog editDialog(this);
    editDialog.loadValues(m_geometry);
    if (editDialog.exec() == QDialog::Accepted)
    {
        editDialog.saveValues(m_geometry);
        writeGeomFile(m_geometry->geometryFile());
        if (editDialog.currentType!=-1)
        {
            editDialog.getGlobalCharacteristics(Rn,Vol,Surf);
            lineEditSurface->setText(QString::number(Surf));
            lineEditVolume->setText(QString::number(Vol));
            lineEditRadiusNose->setText(QString::number(Rn));
        }
    }

}

void PhysicalPropertiesDialog::setGlobalCharacteristics(QString file)
{
    if(!file.isEmpty())
    {
        VehicleGeometry vehicle=VehicleGeometry(file);
        vehicle.CalculateGlobalCharacteristics();
        lineEditSurface->setText(QString::number(vehicle.Surface));
        lineEditVolume->setText(QString::number(vehicle.Volume));
        lineEditRadiusNose->setText(QString::number(vehicle.NoseRadius));
    }
}


double PhysicalPropertiesDialog::getRefArea()//temporary fix for communicating with aerodynamics GUI
{
    double refArea;
    if(m_geometry->shapeFamily()->value()=="spherecone")
        refArea=sta::Pi()*pow(m_geometry->sphereconeShape()->param2()->value(),2);
    else if(m_geometry->shapeFamily()->value()=="capsule")
         refArea=sta::Pi()*pow(m_geometry->capsuleShape()->param2()->value(),2);
    else if(m_geometry->shapeFamily()->value()=="probe")
        refArea=sta::Pi()*pow(m_geometry->probeShape()->param2()->value(),2);
    else if(m_geometry->shapeFamily()->value()=="biconic")
        refArea=sta::Pi()*pow(m_geometry->biconicShape()->param3()->value(),2);
    else
        refArea=0;
    return refArea;
}
