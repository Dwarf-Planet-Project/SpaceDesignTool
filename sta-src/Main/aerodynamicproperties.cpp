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
 ------ Copyright (C) 2009 European Space Agency (space.trajectory.analysis AT gmail.com) ----
 ------------------ Author: Tiziana Sabatini  -------------------------------------------------
 ------------------ E-mail: (tiziana.sabatini@yahoo.it) ----------------------------
 */

#include "scenariotree.h"
#include "aerodynamicproperties.h"
#include "RAM/aerodynamicmethods.h"
#include "Scenario/scenario.h"
#include "QFileDialog"
#include <QMessageBox>
#include "QDebug"
#include "cmath"

AerodynamicPropertiesDialog::AerodynamicPropertiesDialog(ScenarioTree* parent) :
    QDialog(parent)
{
    setupUi(this);
    QDoubleValidator* positiveDoubleValidator = new QDoubleValidator(this);
    positiveDoubleValidator->setBottom(0.0);

    //lineEditGLoadLimit->setValidator(positiveDoubleValidator);
    //lineEditSPHeatRateLimit->setValidator(positiveDoubleValidator);
    lineEditRefArea->setValidator(positiveDoubleValidator);
    lineEditRefLength->setValidator(positiveDoubleValidator);
    loadCDDialog = new QFileDialog(this, "Select a file:", "data/aerodynamics/");
    loadCSDialog = new QFileDialog(this, "Select a file:", "data/aerodynamics/");
    loadCLDialog = new QFileDialog(this, "Select a file:", "data/aerodynamics/");
    loadClDialog = new QFileDialog(this, "Select a file:", "data/aerodynamics/");
    loadCmDialog = new QFileDialog(this, "Select a file:", "data/aerodynamics/");
    loadCnDialog = new QFileDialog(this, "Select a file:", "data/aerodynamics/");


    connect(CDPushButton,SIGNAL(clicked()),loadCDDialog,SLOT(exec()));
    connect(loadCDDialog,SIGNAL(fileSelected(QString)),this,SLOT(writeCDFile(QString)));
    connect(CSPushButton,SIGNAL(clicked()),loadCSDialog,SLOT(exec()));
    connect(loadCSDialog,SIGNAL(fileSelected(QString)),this,SLOT(writeCSFile(QString)));
    connect(CLPushButton,SIGNAL(clicked()),loadCLDialog,SLOT(exec()));
    connect(loadCLDialog,SIGNAL(fileSelected(QString)),this,SLOT(writeCLFile(QString)));
    connect(ClPushButton,SIGNAL(clicked()),loadClDialog,SLOT(exec()));
    connect(loadClDialog,SIGNAL(fileSelected(QString)),this,SLOT(writeClFile(QString)));
    connect(CmPushButton,SIGNAL(clicked()),loadCmDialog,SLOT(exec()));
    connect(loadCmDialog,SIGNAL(fileSelected(QString)),this,SLOT(writeCmFile(QString)));
    connect(CnPushButton,SIGNAL(clicked()),loadCnDialog,SLOT(exec()));
    connect(loadCnDialog,SIGNAL(fileSelected(QString)),this,SLOT(writeCnFile(QString)));

}


bool AerodynamicPropertiesDialog::loadValues(ScenarioREVAeroThermodynamicsType* aerothermo)
{
    m_aerothermo=aerothermo;
    lineEditRefArea->setText(QString::number(aerothermo->referenceArea()));
    
    QList<QSharedPointer<ScenarioAeroCoefFileType> > & AeroCoefList=aerothermo->AeroCoefFile();
    foreach (QSharedPointer<ScenarioAeroCoefFileType> CoefFile, AeroCoefList)
    {
        if(CoefFile->CoefName()=="CD")
            CDLineEdit->setText(CoefFile->FileLocation());
        else if(CoefFile->CoefName()=="CS")
            CSLineEdit->setText(CoefFile->FileLocation());
        else if(CoefFile->CoefName()=="CL")
            CLLineEdit->setText(CoefFile->FileLocation());
        if(aerothermo->CoefficientType()==3)
        {
            if(CoefFile->CoefName()=="Cl")
                ClLineEdit->setText(CoefFile->FileLocation());
            else if(CoefFile->CoefName()=="Cm")
                CmLineEdit->setText(CoefFile->FileLocation());
            else if(CoefFile->CoefName()=="Cn")
                CnLineEdit->setText(CoefFile->FileLocation());
        }
    }

    if(aerothermo->CoefficientType()!=3)
    {
        xMomLabel->setEnabled(0);
        yMomLabel->setEnabled(0);
        zMomLabel->setEnabled(0);
        xMomLineEdit->setEnabled(0);
        yMomLineEdit->setEnabled(0);
        zMomLineEdit->setEnabled(0);
        ClLabel->setEnabled(0);
        CmLabel->setEnabled(0);
        CnLabel->setEnabled(0);
        ClLineEdit->setEnabled(0);
        CmLineEdit->setEnabled(0);
        CnLineEdit->setEnabled(0);
        ClPushButton->setEnabled(0);
        CmPushButton->setEnabled(0);
        CnPushButton->setEnabled(0);
        lineEditRefLength->setEnabled(0);
        refLengthLabel->setEnabled(0);
    }
    else
    {
        lineEditRefLength->setText(QString::number(aerothermo->referenceLength()));
        xMomLineEdit->setText(QString::number(aerothermo->momentReferencePoint()[0]));
        yMomLineEdit->setText(QString::number(aerothermo->momentReferencePoint()[1]));
        zMomLineEdit->setText(QString::number(aerothermo->momentReferencePoint()[2]));
    }
    //ScenarioParachuteProperties *parachuteproperties = aerodynamicProperties->parachuteProperties();
    /*
    if(parachuteproperties)
    {
        lineEditSurfaceArea->setText(QString::number(parachuteproperties->surfaceArea()));
        lineEditCDCoefficients_2->setText(parachuteproperties->CDCoefficients());
        lineEditDeploymentMachNumber->setText(QString::number(parachuteproperties->deploymentMach()));
    }
    */
    return true;
}

bool AerodynamicPropertiesDialog::saveValues(ScenarioREVAeroThermodynamicsType* aerothermo)
{
    aerothermo->setReferenceArea(lineEditRefArea->text().toDouble());
    if (aerothermo->CoefficientType()==3)
    {
        QList<double> momRefTemp;
        momRefTemp.append(xMomLineEdit->text().toDouble());
        momRefTemp.append(yMomLineEdit->text().toDouble());
        momRefTemp.append(zMomLineEdit->text().toDouble());

        aerothermo->setReferenceLength(lineEditRefLength->text().toDouble());
        aerothermo->setMomentReferencePoint(momRefTemp);
    }
    foreach (QSharedPointer<ScenarioAeroCoefFileType> CoefFile, aerothermo->AeroCoefFile())
    {
        if(CoefFile->CoefName()=="CD")
            CoefFile->setFileLocation(CDLineEdit->text());
        else if(CoefFile->CoefName()=="CS")
            CoefFile->setFileLocation(CSLineEdit->text());
        else if(CoefFile->CoefName()=="CL")
            CoefFile->setFileLocation(CLLineEdit->text());
        if(aerothermo->CoefficientType()==3)
        {
            if(CoefFile->CoefName()=="Cl")
                CoefFile->setFileLocation(ClLineEdit->text());
            else if(CoefFile->CoefName()=="Cm")
                CoefFile->setFileLocation(CmLineEdit->text());
            else if(CoefFile->CoefName()=="Cn")
                CoefFile->setFileLocation(CnLineEdit->text());
        }
    }

    return true;
}

void AerodynamicPropertiesDialog::writeCDFile(QString filename)
{
    filename.remove(this->loadCDDialog->directory().path() + "/");
    CDLineEdit->setText(filename);
}

void AerodynamicPropertiesDialog::writeCSFile(QString filename)
{
    filename.remove(this->loadCSDialog->directory().path() + "/");
    CSLineEdit->setText(filename);
}

void AerodynamicPropertiesDialog::writeCLFile(QString filename)
{
    filename.remove(this->loadCLDialog->directory().path() + "/");
    CLLineEdit->setText(filename);
}

void AerodynamicPropertiesDialog::writeClFile(QString filename)
{
    filename.remove(this->loadClDialog->directory().path() + "/");
    ClLineEdit->setText(filename);
}

void AerodynamicPropertiesDialog::writeCmFile(QString filename)
{
    filename.remove(this->loadCmDialog->directory().path() + "/");
    CmLineEdit->setText(filename);
}

void AerodynamicPropertiesDialog::writeCnFile(QString filename)
{
    filename.remove(this->loadCnDialog->directory().path() + "/");
    CnLineEdit->setText(filename);
}

void AerodynamicPropertiesDialog::on_fromGeomPushButton_clicked()
{
    saveValues(m_aerothermo);
    if(m_aerothermo->geomFile().isEmpty())
    {
        QMessageBox::information(this, tr(""), tr("Geometry file not set, please set file in REVGeometry GUI"));
    }
    else
    {
        AerodynamicMethodDialog dialog(m_aerothermo,this);
        QList<QString> aeroList;

        if (dialog.exec() == QDialog::Accepted)
        {
        }

        if(m_aerothermo->AeroCoefFile().length()>2)
        {
            CDLineEdit->setText(m_aerothermo->AeroCoefFile()[0]->FileLocation());
            CSLineEdit->setText(m_aerothermo->AeroCoefFile()[1]->FileLocation());
            CLLineEdit->setText(m_aerothermo->AeroCoefFile()[2]->FileLocation());
        }
        if(m_aerothermo->CoefficientType()==3 && m_aerothermo->AeroCoefFile().length()>5)
        {
            ClLineEdit->setText(m_aerothermo->AeroCoefFile()[3]->FileLocation());
            CmLineEdit->setText(m_aerothermo->AeroCoefFile()[4]->FileLocation());
            CnLineEdit->setText(m_aerothermo->AeroCoefFile()[5]->FileLocation());
        }
    }
}
