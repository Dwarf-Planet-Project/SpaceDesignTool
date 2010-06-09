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
 ------------------ Author: Dominic Dirkx  ------------------------------------------
 ------------------ E-mail: (d.dirkxATstudent.tudelft.nl) ----------------------------
*/

#include "parametrization.h"
#include "Astro-Core/stamath.h"
#include "Scenario/staschema.h"
#include <QDebug>
#include <QMessageBox>

ParametrizationDialog::ParametrizationDialog( QWidget * parent, Qt::WindowFlags f) : QDialog(parent,f)
{
	setupUi(this);
        currentType=-1;
        QDoubleValidator* positiveDoubleValidator = new QDoubleValidator(this);
        positiveDoubleValidator->setBottom(0.0);
        QIntValidator* positiveIntValidator = new QIntValidator(this);
        positiveIntValidator->setBottom(2.0);
        blConeLineEdit1->setValidator(positiveDoubleValidator);
        blConeLineEdit2->setValidator(positiveDoubleValidator);
        blConeLineEdit3->setValidator(positiveDoubleValidator);
        biConeLineEdit1->setValidator(positiveDoubleValidator);
        biConeLineEdit2->setValidator(positiveDoubleValidator);
        biConeLineEdit3->setValidator(positiveDoubleValidator);
        biConeLineEdit4->setValidator(positiveDoubleValidator);
        biConeLineEdit5->setValidator(positiveDoubleValidator);
        probeLineEdit1->setValidator(positiveDoubleValidator);
        probeLineEdit2->setValidator(positiveDoubleValidator);
        probeLineEdit3->setValidator(positiveDoubleValidator);
        probeLineEdit4->setValidator(positiveDoubleValidator);
        probeLineEdit5->setValidator(positiveDoubleValidator);
        capsuleLineEdit1->setValidator(positiveDoubleValidator);
        capsuleLineEdit2->setValidator(positiveDoubleValidator);
        capsuleLineEdit3->setValidator(positiveDoubleValidator);
        capsuleLineEdit4->setValidator(positiveDoubleValidator);
        linesLineEdit->setValidator(positiveIntValidator);
        pointsLineEdit->setValidator(positiveIntValidator);;

        linesLineEdit->setText("21");
        pointsLineEdit->setText("21");
 }

ParametrizationDialog::~ParametrizationDialog()
{
}

void ParametrizationDialog::on_generateGeometryButton_clicked()
{
    if(verifyInput())
    {
        int lines=linesLineEdit->text().toInt()-1;
        int points=pointsLineEdit->text().toInt();
        switch(geomStackedWidget->currentIndex())
        {
        case 0:
            if(blConeLineEdit1->text()!=""&&blConeLineEdit2->text()!=""&&blConeLineEdit3->text()!="")
            {
                double* params=new double[3];
                params[0]=blConeLineEdit1->text().toDouble();
                params[1]=blConeLineEdit2->text().toDouble();
                params[2]=blConeLineEdit3->text().toDouble()*sta::Pi()/180;
                VehicleGeometry *vehicle=new VehicleGeometry(0,params,lines,points);
                geometryFileLineEdit->setText(QString((vehicle->Filename).c_str()));
                string folder="data/vehiclewgs/";
                string oldName=vehicle->Filename;
                vehicle->Filename=folder.append(oldName);
                vehicle->WriteWGSfile();
                vehicle->CalculateGlobalCharacteristics();
                NoseRadius=params[0];
                Volume=vehicle->Volume;
                Surface=vehicle->Surface;

                delete vehicle;
                delete [] params;
                currentType=geomStackedWidget->currentIndex();
            }
            else
                QMessageBox::warning(this,tr("Parameters"),tr("Please set values for all parameters"));
            break;
         case 1:
            if(capsuleLineEdit1->text()!=""&&capsuleLineEdit2->text()!=""&&capsuleLineEdit3->text()!=""&&capsuleLineEdit4->text()!="")
            {
                double* params=new double[4];
                params[0]=capsuleLineEdit1->text().toDouble();
                params[1]=capsuleLineEdit2->text().toDouble();
                params[2]=capsuleLineEdit3->text().toDouble();
                params[3]=capsuleLineEdit4->text().toDouble()*sta::Pi()/180;
                VehicleGeometry *vehicle=new VehicleGeometry(1,params,lines,points);
                geometryFileLineEdit->setText(QString((vehicle->Filename).c_str()));
                string folder="data/vehiclewgs/";
                string oldName=vehicle->Filename;
                vehicle->Filename=folder.append(oldName);
                vehicle->WriteWGSfile();
                vehicle->CalculateGlobalCharacteristics();
                NoseRadius=params[0];
                Volume=vehicle->Volume;
                Surface=vehicle->Surface;

                delete vehicle;
                delete [] params;

                currentType=geomStackedWidget->currentIndex();
            }
            else
                QMessageBox::warning(this,tr("Parameters"),tr("Please set values for all parameters"));
            break;
        case 2:
            if(probeLineEdit1->text()!=""&&probeLineEdit2->text()!=""&&probeLineEdit3->text()!=""&&probeLineEdit4->text()!=""&&probeLineEdit5->text()!="")
            {
                double* params=new double[5];
                params[0]=probeLineEdit1->text().toDouble();
                params[1]=probeLineEdit2->text().toDouble();
                params[2]=probeLineEdit3->text().toDouble();
                params[3]=probeLineEdit4->text().toDouble()*sta::Pi()/180;
                params[4]=-probeLineEdit5->text().toDouble()*sta::Pi()/180;
                VehicleGeometry *vehicle=new VehicleGeometry(2,params,lines,points);
                geometryFileLineEdit->setText(QString((vehicle->Filename).c_str()));
                string folder="data/vehiclewgs/";
                string oldName=vehicle->Filename;
                vehicle->Filename=folder.append(oldName);
                vehicle->WriteWGSfile();
                vehicle->CalculateGlobalCharacteristics();
                NoseRadius=params[0];
                Volume=vehicle->Volume;
                Surface=vehicle->Surface;

                delete vehicle;
                delete [] params;

                currentType=geomStackedWidget->currentIndex();
            }
            else
                QMessageBox::warning(this,tr("Parameters"),tr("Please set values for all parameters"));
            break;
        case 3:
            if(biConeLineEdit1->text()!=""&&biConeLineEdit2->text()!=""&&biConeLineEdit3->text()!=""&&biConeLineEdit4->text()!=""&&biConeLineEdit5->text()!="")
            {
                double* params=new double[5];
                params[0]=biConeLineEdit1->text().toDouble();
                params[1]=biConeLineEdit2->text().toDouble();
                params[2]=biConeLineEdit3->text().toDouble();
                params[3]=biConeLineEdit4->text().toDouble()*sta::Pi()/180;
                params[4]=biConeLineEdit5->text().toDouble()*sta::Pi()/180;
                VehicleGeometry *vehicle=new VehicleGeometry(2,params,lines,points);
                geometryFileLineEdit->setText(QString((vehicle->Filename).c_str()));
                string folder="data/vehiclewgs/";
                string oldName=vehicle->Filename;
                vehicle->Filename=folder.append(oldName);
                vehicle->WriteWGSfile();
                vehicle->CalculateGlobalCharacteristics();
                NoseRadius=params[0];
                Volume=vehicle->Volume;
                Surface=vehicle->Surface;

                delete vehicle;
                delete [] params;

                currentType=geomStackedWidget->currentIndex();
            }
            else
                QMessageBox::warning(this,tr("Parameters"),tr("Please set values for all parameters"));
            break;

        }
    }
}

bool ParametrizationDialog::loadValues(ScenarioREVGeometryType* geometry)
{
    if(geometry->shapeFamily()->value()=="spherecone")
    {
        blConeLineEdit1->setText(QString::number(geometry->sphereconeShape()->param1()->value()));
        blConeLineEdit2->setText(QString::number(geometry->sphereconeShape()->param2()->value()));
        blConeLineEdit3->setText(QString::number(geometry->sphereconeShape()->param3()->value()));
        geomStackedWidget->setCurrentIndex(0);
        geomComboBox->setCurrentIndex(0);
        geometryFileLineEdit->setText(geometry->geometryFile());

    }
    else if(geometry->shapeFamily()->value()=="biconic")
    {
        biConeLineEdit1->setText(QString::number(geometry->biconicShape()->param1()->value()));
        biConeLineEdit2->setText(QString::number(geometry->biconicShape()->param2()->value()));
        biConeLineEdit3->setText(QString::number(geometry->biconicShape()->param3()->value()));
        biConeLineEdit4->setText(QString::number(geometry->biconicShape()->param4()->value()));
        biConeLineEdit5->setText(QString::number(geometry->biconicShape()->param5()->value()));
        geomStackedWidget->setCurrentIndex(3);
        geomComboBox->setCurrentIndex(3);

        geometryFileLineEdit->setText(geometry->geometryFile());
    }
    else if(geometry->shapeFamily()->value()=="probe")
    {
        probeLineEdit1->setText(QString::number(geometry->probeShape()->param1()->value()));
        probeLineEdit2->setText(QString::number(geometry->probeShape()->param2()->value()));
        probeLineEdit3->setText(QString::number(geometry->probeShape()->param3()->value()));
        probeLineEdit4->setText(QString::number(geometry->probeShape()->param4()->value()));
        probeLineEdit5->setText(QString::number(geometry->probeShape()->param5()->value()));
        geomStackedWidget->setCurrentIndex(2);
        geomComboBox->setCurrentIndex(2);

        geometryFileLineEdit->setText(geometry->geometryFile());
    }
    else if(geometry->shapeFamily()->value()=="capsule")
    {
        capsuleLineEdit1->setText(QString::number(geometry->capsuleShape()->param1()->value()));
        capsuleLineEdit2->setText(QString::number(geometry->capsuleShape()->param2()->value()));
        capsuleLineEdit3->setText(QString::number(geometry->capsuleShape()->param3()->value()));
        capsuleLineEdit4->setText(QString::number(geometry->capsuleShape()->param4()->value()));
        geomStackedWidget->setCurrentIndex(1);
        geomComboBox->setCurrentIndex(1);

        geometryFileLineEdit->setText(geometry->geometryFile());
    }
    return true;
}

bool ParametrizationDialog::saveValues(ScenarioREVGeometryType* geometry)
{
    ScenarioOptVarDouble * par1=new ScenarioOptVarDouble;
    ScenarioOptVarDouble * par2=new ScenarioOptVarDouble;
    ScenarioOptVarDouble * par3=new ScenarioOptVarDouble;
    ScenarioOptVarDouble * par4=new ScenarioOptVarDouble;
    ScenarioOptVarDouble * par5=new ScenarioOptVarDouble;

    if(currentType==0)
    {
        ScenariosphereconeShape * bicone=new ScenariosphereconeShape;
        par1->setValue(blConeLineEdit1->text().toDouble());
        par2->setValue(blConeLineEdit2->text().toDouble());
        par3->setValue(blConeLineEdit3->text().toDouble());
        bicone->setParam1(QSharedPointer<ScenarioOptVarDouble>(par1));
        bicone->setParam2(QSharedPointer<ScenarioOptVarDouble>(par2));
        bicone->setParam3(QSharedPointer<ScenarioOptVarDouble>(par3));
        geometry->setSphereconeShape(QSharedPointer<ScenariosphereconeShape>(bicone));
        geometry->shapeFamily()->setValue(QString("spherecone"));
    }
    else if(currentType==1)
    {
        ScenariocapsuleShape * capsule=new ScenariocapsuleShape;
        par1->setValue(capsuleLineEdit1->text().toDouble());
        par2->setValue(capsuleLineEdit2->text().toDouble());
        par3->setValue(capsuleLineEdit3->text().toDouble());
        par4->setValue(capsuleLineEdit4->text().toDouble());
        capsule->setParam1(QSharedPointer<ScenarioOptVarDouble>(par1));
        capsule->setParam2(QSharedPointer<ScenarioOptVarDouble>(par2));
        capsule->setParam3(QSharedPointer<ScenarioOptVarDouble>(par3));
        capsule->setParam4(QSharedPointer<ScenarioOptVarDouble>(par4));
        geometry->setCapsuleShape(QSharedPointer<ScenariocapsuleShape>(capsule));
        geometry->shapeFamily()->setValue(QString("capsule"));

    }
    else if(currentType==2)
    {
        ScenarioprobeShape * probe=new ScenarioprobeShape;
        par1->setValue(probeLineEdit1->text().toDouble());
        par2->setValue(probeLineEdit2->text().toDouble());
        par3->setValue(probeLineEdit3->text().toDouble());
        par4->setValue(probeLineEdit4->text().toDouble());
        par5->setValue(probeLineEdit5->text().toDouble());
        probe->setParam1(QSharedPointer<ScenarioOptVarDouble>(par1));
        probe->setParam2(QSharedPointer<ScenarioOptVarDouble>(par2));
        probe->setParam3(QSharedPointer<ScenarioOptVarDouble>(par3));
        probe->setParam4(QSharedPointer<ScenarioOptVarDouble>(par4));
        probe->setParam5(QSharedPointer<ScenarioOptVarDouble>(par5));
        geometry->setProbeShape(QSharedPointer<ScenarioprobeShape>(probe));
        geometry->shapeFamily()->setValue(QString("probe"));
    }
    else if(currentType==3)
    {

        ScenariobiconicShape * biconic=new ScenariobiconicShape;
        par1->setValue(biConeLineEdit1->text().toDouble());
        par2->setValue(biConeLineEdit2->text().toDouble());
        par3->setValue(biConeLineEdit3->text().toDouble());
        par4->setValue(biConeLineEdit4->text().toDouble());
        par5->setValue(biConeLineEdit5->text().toDouble());
        biconic->setParam1(QSharedPointer<ScenarioOptVarDouble>(par1));
        biconic->setParam2(QSharedPointer<ScenarioOptVarDouble>(par2));
        biconic->setParam3(QSharedPointer<ScenarioOptVarDouble>(par3));
        biconic->setParam4(QSharedPointer<ScenarioOptVarDouble>(par4));
        biconic->setParam5(QSharedPointer<ScenarioOptVarDouble>(par5));
        geometry->setBiconicShape(QSharedPointer<ScenariobiconicShape>(biconic));
        geometry->shapeFamily()->setValue(QString("biconic"));
    }
    geometry->setGeometryFile(geometryFileLineEdit->text());
    return true;
}

bool ParametrizationDialog::verifyInput()
{

    if(linesLineEdit->text().isEmpty()||pointsLineEdit->text().isEmpty())
    {
        QMessageBox::warning(this,tr("Parameters"),tr("Please set number of points on geometry"));
        return false;
    }
    
    
    switch(geomStackedWidget->currentIndex())
    {
    case 0: 
        if(blConeLineEdit1->text().toDouble()>blConeLineEdit2->text().toDouble())
        {
            QMessageBox::warning(this,tr("Parameters"),tr("Incompatible vehicle parameters, nose radius must be smaller than base radius"));
            return false;
        }
        else if(blConeLineEdit3->text().toDouble()<0 || blConeLineEdit3->text().toDouble()>90)
        {
            QMessageBox::warning(this,tr("Parameters"),tr("Incompatible vehicle parameters, cone angle mut be between 0 and 90 degrees"));
            return false;
        }
        else
            return true;
    case 1:
        if(capsuleLineEdit2->text().toDouble()<capsuleLineEdit3->text().toDouble())
        {
            QMessageBox::warning(this,tr("Parameters"),tr("Middle radius must be larger than rear radius"));
            return false;
        }
        else if(capsuleLineEdit1->text().toDouble()<capsuleLineEdit2->text().toDouble())
        {
            QMessageBox::warning(this,tr("Parameters"),tr("Incompatible vehicle parameters, nose radius must be larger than middle radius"));
            return false;
        }
        else if(capsuleLineEdit4->text().toDouble()<0 || capsuleLineEdit4->text().toDouble()>90)
        {
            QMessageBox::warning(this,tr("Parameters"),tr("Incompatible vehicle parameters, base cone angle mut be between 0 and 90 degrees"));
            return false;
        }
        else
            return true;
    case 2:
        if(probeLineEdit1->text().toDouble()>probeLineEdit2->text().toDouble())
        {
            QMessageBox::warning(this,tr("Parameters"),tr("Incompatible vehicle parameters, nose radius must be smaller than base radius"));
            return false;
        }
        else if(probeLineEdit2->text().toDouble()<probeLineEdit3->text().toDouble())
        {
            QMessageBox::warning(this,tr("Parameters"),tr("Middle radius must be larger than rear radius"));
            return false;
        }
        else if(probeLineEdit4->text().toDouble()<0 || probeLineEdit4->text().toDouble()>90 || probeLineEdit5->text().toDouble()<0 || probeLineEdit5->text().toDouble()>90)
        {
            QMessageBox::warning(this,tr("Parameters"),tr("Incompatible vehicle parameters, cone angles mut be between 0 and 90 degrees"));
            return false;
        }
        else
            return true;
    case 3:
        if(biConeLineEdit1->text().toDouble()>biConeLineEdit2->text().toDouble())
        {
            QMessageBox::warning(this,tr("Parameters"),tr("Incompatible vehicle parameters, nose radius must be smaller than base radius"));
            return false;
        }
        else if(biConeLineEdit2->text().toDouble()>biConeLineEdit3->text().toDouble())
        {
            QMessageBox::warning(this,tr("Parameters"),tr("Middle radius must be smaller than rear radius"));
            return false;
        }
        else if(biConeLineEdit4->text().toDouble()<0 || biConeLineEdit4->text().toDouble()>90 || biConeLineEdit5->text().toDouble()<0 || biConeLineEdit5->text().toDouble()>90)
        {
            QMessageBox::warning(this,tr("Parameters"),tr("Incompatible vehicle parameters, cone angles mut be between 0 and 90 degrees"));
            return false;
        }
        else
            return true;
    }
return false;
}

void ParametrizationDialog::getGlobalCharacteristics(double&Rn, double&Vol, double& Surf)
{
    Rn=NoseRadius;
    Vol=Volume;
    Surf=Surface;
}

/*
QString ParametrizationDialog::getFileName()
{
    QString name=geometryFileLineEdit->text();
    return name;
}
*/


