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

bool PhysicalPropertiesDialog::saveValues(ScenarioREVGeometryType* REVgeometry,ScenarioREVAeroThermodynamicsType* REVaerothermo)
{
    REVgeometry->setNoseRadius(lineEditRadiusNose->text().toDouble());
    REVgeometry->setREVsurface(lineEditSurface->text().toDouble());
    REVgeometry->setREVvolume(lineEditVolume->text().toDouble());

    REVgeometry->setGeometryFile(fileLineEdit->text());
    //REVaerothermo->setGeomFile(fileLineEdit->text());
    if(fileLoaded)
        REVgeometry->shapeFamily()->setValue(QString(""));
    if (getRefArea()>0)
        REVaerothermo->setReferenceArea(getRefArea());
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
        int i;
        VehicleGeometry* vehicle=new VehicleGeometry(file);
        qDebug()<<"vehicle created";
        vehicle->CalculateGlobalCharacteristics();
        qDebug()<<"globs calced";
        lineEditSurface->setText(QString::number(vehicle->Surface));
        lineEditVolume->setText(QString::number(vehicle->Volume));
        lineEditRadiusNose->setText(QString::number(vehicle->NoseRadius));
        delete vehicle;
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
        refArea=-1;
    return refArea;
}
