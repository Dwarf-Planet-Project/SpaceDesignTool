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
#include "QDebug"

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

    //connect(cdButton,SIGNAL(clicked()),loadCDDialog,SLOT(exec()));
    connect(loadCDDialog,SIGNAL(fileSelected(QString)),this,SLOT(writeCdFile(QString)));

    loadCDpDialog = new QFileDialog(this, "Select a file:", "data/aerodynamics/");
    //connect(cdpButton,SIGNAL(clicked()),loadCDpDialog,SLOT(exec()));
    connect(loadCDpDialog,SIGNAL(fileSelected(QString)),this,SLOT(writeCdpFile(QString)));
}


bool AerodynamicPropertiesDialog::loadValues(ScenarioREVAeroThermodynamicsType* aerothermo)
{
    m_aerothermo=aerothermo;
    lineEditRefArea->setText(QString::number(aerothermo->referenceArea()));
    lineEditRefLength->setText(QString::number(aerothermo->referenceLength()));
    if(aerothermo->CoefficientType()==1)
    {
        xMomLabel->setEnabled(0);
        yMomLabel->setEnabled(0);
        zMomLabel->setEnabled(0);
        xMomLineEdit->setEnabled(0);
        xMomLineEdit->setEnabled(0);
        yMomLineEdit->setEnabled(0);
        zMomLineEdit->setEnabled(0);

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
    aerothermo->setReferenceLength(lineEditRefLength->text().toDouble());


    //ScenarioParachuteProperties *parachuteproperties = new ScenarioParachuteProperties();
    //parachuteproperties->setSurfaceArea(lineEditSurfaceArea->text().toDouble());
    //parachuteproperties->setCDCoefficients(lineEditCDCoefficients_2->text());
    //parachuteproperties->setDeploymentMach(lineEditDeploymentMachNumber->text().toDouble());

    //aerodynamicProperties->setParachuteProperties(parachuteproperties);

    return true;
}

/*
void AerodynamicPropertiesDialog::writeCdFile(QString filename)
{
    filename.remove(this->loadCDDialog->directory().path() + "/");
    lineEditCDCoefficients->setText(filename);
}


void AerodynamicPropertiesDialog::writeCdpFile(QString filename)
{
    filename.remove(this->loadCDpDialog->directory().path() + "/");
    lineEditCDCoefficients_2->setText(filename);
}
*/
void AerodynamicPropertiesDialog::on_fromGeomPushButton_clicked()
{
    AerodynamicMethodDialog dialog(m_aerothermo,this);
    dialog.exec();
}
