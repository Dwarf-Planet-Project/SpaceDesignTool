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

#include "aerodynamicmethods.h"
#include "aeroanalysis.h"
#include "Astro-Core/stamath.h"
#include "Scenario/staschema.h"
#include "vehiclegeometry.h"
#include "advancedselectionGUI.h"
#include <QDebug>
#include <QComboBox>
#include <QMessageBox>

AerodynamicMethodDialog::~AerodynamicMethodDialog()
{
    int i,j;
    delete [] m_Methods[0];
    delete [] m_Methods[1];
    delete [] m_Methods;
    delete [] defaultMethods[0];
    delete [] defaultMethods[1];
    delete [] defaultMethods[2];
    delete [] defaultMethods;

    for(i=1;i<aeroTreeWidget->topLevelItemCount();i++)
    {
        QTreeWidgetItem* treeItem=aeroTreeWidget->topLevelItem(i);
        for(j=0;j<treeItem->childCount();j++)
        {
            QTreeWidgetItem * methodItem=treeItem->child(j);
            QWidget * boxWidget;
            boxWidget=aeroTreeWidget->itemWidget(methodItem,1);
            QComboBox * box = dynamic_cast<QComboBox*>(boxWidget);
            delete box;
            delete methodItem;
        }
        delete treeItem;

    }

    delete m_vehicle;
}

AerodynamicMethodDialog::AerodynamicMethodDialog(ScenarioREVAeroThermodynamicsType* aerothermo, QString geomFileName, QWidget * parent, Qt::WindowFlags f) : QDialog(parent,f)
{
    setupUi(this);
    loadGeomDialog = new QFileDialog(this, "Select a file:", "data/vehiclewgs/");
    connect(geomFileButton,SIGNAL(clicked()),loadGeomDialog,SLOT(exec()));
    connect(loadGeomDialog,SIGNAL(fileSelected(QString)),this,SLOT(writeGeomFile(QString)));
    m_aerothermo=aerothermo;
    GeomFile="data/vehiclewgs/"+geomFileName;//m_aerothermo->geomFile();
    //geomFileLineEdit->setReadOnly(1);
    geomFileLineEdit->setText(GeomFile);
    autoSelected=0;
    attitudeGroupBox->setEnabled(1);
    forceCoordBox->setCurrentIndex(1);
    momentCoordBox->setCurrentIndex(0);
    Parameters=new PartAnalysis::SelectionStruct;
    if(aerothermo->CoefficientType()!=1)
    {
        attitudeGroupBox->setEnabled(0);
    }
    else
    {
        defaultAlphaLineEdit->setText("0");
        defaultBetaLineEdit->setText("0");
        momentCoordLabel->setEnabled(0);
        momentCoordBox->setEnabled(0);
    }

    m_vehicle=new VehicleGeometry(GeomFile);
    m_Methods=new int*[2];
    int i;
    for(i=0;i<2;i++)
    {
        m_Methods[i]=new int[2*m_vehicle->nParts];
        for(int j=0;j<2*m_vehicle->nParts;j++)
            m_Methods[i][j]=1;

    }
    defaultMethods=new int*[3];
    for(i=0; i<3;i++)
    {
        defaultMethods[i]=new int[4];
    }
    setDefaultMethods();
    aeroTreeWidget->setColumnWidth(0,200);
    aeroTreeWidget->setColumnWidth(1,210);
    aeroTreeWidget->setColumnWidth(2,60);
    aeroTreeWidget->setColumnWidth(3,60);

    setTreeParts();
    setAeroMethods();
    initializeAutoSettings();
}


void AerodynamicMethodDialog::setTreeParts()
{
    int i,j;
    for(i=1;i<aeroTreeWidget->topLevelItemCount();i++)//de-allocate comb
    {
        QTreeWidgetItem* treeItem=aeroTreeWidget->topLevelItem(i);
        for(j=0;j<treeItem->childCount();j++)
        {
            QTreeWidgetItem * methodItem=treeItem->child(j);
            QWidget * boxWidget;
            boxWidget=aeroTreeWidget->itemWidget(methodItem,1);
            QComboBox * box = dynamic_cast<QComboBox*>(boxWidget);
            delete box;
            delete methodItem;
        }
        delete treeItem;

    }
    aeroTreeWidget->clear();
    for(i=0;i<m_vehicle->nParts;i++)
    {
        QTreeWidgetItem*treeParticipant=new QTreeWidgetItem(aeroTreeWidget);
        treeParticipant->setText(0,m_vehicle->PartsOut[i].name);
        treeParticipant->setText(2,QString::number(m_vehicle->PartsOut[i].Nlines));
        treeParticipant->setText(3,QString::number(m_vehicle->PartsOut[i].Npoints));


        if(!lowHighCheckBox->isChecked())
        {
            QTreeWidgetItem*methodItem=new QTreeWidgetItem(treeParticipant);
            QString methodName="Compression";
            methodItem->setText(0,methodName);
            aeroTreeWidget->setItemWidget(methodItem,1,compressionComboBox());
            QTreeWidgetItem*methodItem2=new QTreeWidgetItem(treeParticipant);
            QString methodName2="Expansion";
            methodItem2->setText(0,methodName2);
            aeroTreeWidget->setItemWidget(methodItem2,1,expansionComboBox());
        }
        else
        {
            QTreeWidgetItem*methodItem=new QTreeWidgetItem(treeParticipant);
            QString methodName="High Hypersonic Compression";
            methodItem->setText(0,methodName);
            aeroTreeWidget->setItemWidget(methodItem,1,compressionComboBox());
            QTreeWidgetItem*methodItem2=new QTreeWidgetItem(treeParticipant);
            QString methodName2="High Hypersonic Expansion";
            methodItem2->setText(0,methodName2);
            aeroTreeWidget->setItemWidget(methodItem2,1,expansionComboBox());
            QTreeWidgetItem*methodItem3=new QTreeWidgetItem(treeParticipant);
            QString methodName3="Low Hypersonic Compression";
            methodItem3->setText(0,methodName3);
            aeroTreeWidget->setItemWidget(methodItem3,1,compressionComboBox());
            QTreeWidgetItem*methodItem4=new QTreeWidgetItem(treeParticipant);
            QString methodName4="Low Hypersonic Expansion";
            methodItem4->setText(0,methodName4);
            aeroTreeWidget->setItemWidget(methodItem4,1,expansionComboBox());
        }
    }
    aeroTreeWidget->expandAll();
}


void AerodynamicMethodDialog::on_selectPushButton_clicked()
{
    int Error;
    bool ConcavityIgnore=concavityCheckBox->isChecked();
    for(int i=0;i<2;i++)//Delete m_methods, will be allocated by m_vehicle->AutomaticMethodSelection(m_Methods)
    {
        delete [] m_Methods[i];
    }
    Error=m_vehicle->AutomaticMethodSelection(m_Methods,Parameters,ConcavityIgnore,defaultMethods);
    if(Error==0)
    {
        autoSelected=1;
        if(m_vehicle->wgsModified==1)
        {
            QMessageBox::information(this, tr(""), tr("Modified .wgs file created"));
            geomFileLineEdit->setText(m_vehicle->Filename.c_str());
            m_vehicle->wgsModified=0;
        }
        setTreeParts();
        setAeroMethods();
    }
    else
    {
        for(int i=0;i<2;i++)//m_Methods has not been set by seletor, must re-set to defaults
        {
            m_Methods[i]=new int[2*m_vehicle->nParts];
            for(int j=0;j<2*m_vehicle->nParts;j++)
                m_Methods[i][j]=1;
        }
        displayGeometryError(Error);
    }

}

bool AerodynamicMethodDialog::on_generatePushButton_clicked()
{
    int i;
    double lowHypMax=lowHypLineEdit->text().toDouble();
    double highHypMin=highHypLineEdit->text().toDouble();
    if(m_aerothermo->referenceArea()==0)
    {
        QMessageBox::warning(this,tr("Reference values"),tr("Reference area is zero, coefficients not generated"));
        return false   ;
    }
    else if(m_aerothermo->referenceLength()==0 && m_aerothermo->CoefficientType()!=1)
    {
        QMessageBox::warning(this,tr("Reference values"),tr("Reference length is zero, coefficients not generated"));
        return false;
    }
    else if(lowHypMax<highHypMin)
    {
        QMessageBox::warning(this,tr("mach range"),tr("Low and high hypersonic regime boundaries incompatible, coefficients not generated"));
        return false;
    }

    for(i=0;i<m_vehicle->nParts;i++)
    {
        if(m_vehicle->PartsOut[i].ErrorInt==1)
        {
            QMessageBox::warning(this,tr("Database not generated"),tr("Zero area panel(s) in geometry"));
            return false;
        }
    }
    bool LowHigh;
    LowHigh=lowHighCheckBox->isChecked();
    getAeroMethods();
    Eigen::Vector3d momRefEi;//Temporaray use
    for(i=0;i<3;i++)
    {
        momRefEi[i]=m_aerothermo->momentReferencePoint()[i];
    }
    bool writeAero=presWriteCheckBox->isChecked();
    bool writeGeom=geomWriteCheckBox->isChecked();
    int ForceCoord=forceCoordBox->currentIndex();
    int MomentCoord=momentCoordBox->currentIndex();
    QList<QListWidgetItem*> machNumberWidgetList=machList->selectedItems();
    QList<double> machNumberList;
    for(i=0;i<machNumberWidgetList.length();i++)
    {
        if(writeAero)
            machNumberList.append(machNumberWidgetList.at(i)->text().toDouble());
    }
    AeroAnalysis* aeroAn;
    AeroAnalysis* aeroAn1;
    AeroAnalysis* aeroAn2;
    if(LowHigh==0)
    {
        aeroAn =new AeroAnalysis(m_vehicle,m_aerothermo->referenceLength(),m_aerothermo->referenceArea(),momRefEi,m_Methods,machNumberList,writeGeom);
    }
    else
    {
        int** LowMethods;
        int** HighMethods;
        LowMethods=new int*[2];
        HighMethods=new int*[2];
        for(int i=0;i<2;i++)
        {
            LowMethods[i]=new int[m_vehicle->nParts];
            HighMethods[i]=new int[m_vehicle->nParts];
            for(int j=0;j<m_vehicle->nParts;j++)
            {
                LowMethods[i][j]=m_Methods[i][j+m_vehicle->nParts];
                HighMethods[i][j]=m_Methods[i][j];
            }


        }
        aeroAn1 =new AeroAnalysis(m_vehicle,m_aerothermo->referenceLength(),m_aerothermo->referenceArea(),momRefEi,LowMethods,machNumberList,writeGeom);
        aeroAn2 =new AeroAnalysis(m_vehicle,m_aerothermo->referenceLength(),m_aerothermo->referenceArea(),momRefEi,HighMethods,machNumberList,writeGeom);

    }

    if(m_aerothermo->CoefficientType()==1)
    {
        double attrefs[2];
        attrefs[0]=defaultAlphaLineEdit->text().toDouble()*sta::Pi()/180;
        attrefs[1]=defaultBetaLineEdit->text().toDouble()*sta::Pi()/180;
        if(LowHigh==0)
        {
            aeroAn->setIndepVariablesTranslation(attrefs,3.0,20.0,2);
        }
        else if(LowHigh==1)
        {
            aeroAn1->setIndepVariablesTranslation(attrefs,3.0,lowHypMax,1);
            aeroAn2->setIndepVariablesTranslation(attrefs,highHypMin,20.0,0);
        }
    }
    else
    {//defaults for checking functionality
        double attbounds[4];
        attbounds[0]=0;
        attbounds[1]=40*sta::Pi()/180;
        attbounds[2]=0;
        attbounds[3]=20*sta::Pi()/180;
        if(LowHigh==0)
        {
            aeroAn->setIndepVariablesFull(2,attbounds,3.0,20.0,2);
        }
        else if(LowHigh==1)
        {
            aeroAn1->setIndepVariablesFull(2,attbounds,3.0,lowHypMax,1);

            aeroAn2->setIndepVariablesFull(2,attbounds,highHypMin,20.0,0);

        }
    }
    if(LowHigh==0)
    {
        aeroAn->GenerateDatabase(ForceCoord,MomentCoord);
    }
    else
    {
        aeroAn1->GenerateDatabase(ForceCoord,MomentCoord);
        aeroAn2->GenerateDatabase(ForceCoord,MomentCoord);
        aeroAn=new AeroAnalysis(aeroAn1,aeroAn2,highHypMin,lowHypMax);
        delete aeroAn1;
        delete aeroAn2;
    }
    fileList=aeroAn->writeAeroDatabase(ForceCoord,MomentCoord);
    for(i=0;i<aeroAn->CoefsOut;i++)
    {
        QString CoefName;
        m_aerothermo->AeroCoefFile()[i]->setFileLocation(fileList[i]);
        switch(i)
        {
        case 0:
            if(ForceCoord==0)
                CoefName="CA";
            else
                CoefName="CD";
            break;
        case 1:
            if(ForceCoord==0)
                CoefName="CY";
            else
                CoefName="CS";
            break;
        case 2:
            if(ForceCoord==0)
                CoefName="CN";
            else
                CoefName="CL";
            break;
        case 3:
            if(MomentCoord==0)
                CoefName="Cl";
            else
                CoefName="Clpr";
            break;
        case 4:
            if(MomentCoord==0)
                CoefName="Cm";
            else
                CoefName="Cmpr";
            break;
        case 5: if(MomentCoord==0)
                CoefName="Cn";
            else
                CoefName="Cnpr";
            break;
        }
        m_aerothermo->AeroCoefFile()[i]->setCoefName(CoefName);


        if(aeroAn->CoefsOut==3)
        {
            m_aerothermo->AeroCoefFile()[i]->setNumberOfIndepVars(3);
        }
        else if(aeroAn->CoefsOut==6)
        {
            m_aerothermo->AeroCoefFile()[i]->setNumberOfIndepVars(6);
        }
        if(m_aerothermo->CoefficientType()==1)
        {
            m_aerothermo->AeroCoefFile()[i]->setNumberOfIndepVars(1);
            QList<int> nPoints;
            nPoints.append(aeroAn->nMach);
            m_aerothermo->AeroCoefFile()[i]->setIndepVarDiscretizationPoints(nPoints);
            QList<QString> nNames;
            nNames.append("Mach");
            m_aerothermo->AeroCoefFile()[i]->setIndepVarNames(nNames);
            QList<double> nMin;
            m_aerothermo->AeroCoefFile()[i]->setIndepVarMin(nMin);
            nMin.append(aeroAn->Machs[0]);
            QList<double> nMax;
            nMax.append(aeroAn->Machs[aeroAn->nMach-1]);
            m_aerothermo->AeroCoefFile()[i]->setIndepVarMax(nMax);
        }
        else if(m_aerothermo->CoefficientType()==2 || m_aerothermo->CoefficientType()==3)
        {
            m_aerothermo->AeroCoefFile()[i]->setNumberOfIndepVars(3);
            QList<int> nPoints;
            nPoints.append(aeroAn->nMach);
            nPoints.append(aeroAn->nAlpha);
            nPoints.append(aeroAn->nBeta);
            m_aerothermo->AeroCoefFile()[i]->setIndepVarDiscretizationPoints(nPoints);
            QList<QString> nNames;
            nNames.append("Mach");
            nNames.append("Alpha");
            nNames.append("Beta");
            m_aerothermo->AeroCoefFile()[i]->setIndepVarNames(nNames);
            QList<double> nMin;
            nMin.append(aeroAn->Machs[0]);
            nMin.append(aeroAn->alphas[0]);
            nMin.append(aeroAn->betas[0]);
            m_aerothermo->AeroCoefFile()[i]->setIndepVarMin(nMin);
            QList<double> nMax;
            nMax.append(aeroAn->Machs[aeroAn->nMach-1]);
            nMax.append(aeroAn->Machs[aeroAn->nAlpha-1]);
            nMax.append(aeroAn->Machs[aeroAn->nBeta-1]);
            m_aerothermo->AeroCoefFile()[i]->setIndepVarMax(nMax);
        }
    }
    delete aeroAn;
    QMessageBox::information(this, tr(""), tr("Aerodynamic coefficient files generated"));

    return true;
}

void AerodynamicMethodDialog::on_lowHighCheckBox_stateChanged()
{
    getAeroMethods();
    setTreeParts();
    setAeroMethods();
}

void AerodynamicMethodDialog::getAeroMethods()
{
    int i;
    int j;
    int addn;
    for(i=0;i<m_vehicle->nParts;i++)
    {

        QList<QTreeWidgetItem*> itemList=aeroTreeWidget->findItems(m_vehicle->PartsOut[i].name,0);
        QTreeWidgetItem* parent=itemList[0];
        for(j=0;j<parent->childCount();j++)
        {
            QTreeWidgetItem* methodItem=parent->child(j);
            QWidget * boxWidget;
            boxWidget=aeroTreeWidget->itemWidget(methodItem,1);
            if(j<2)
                addn=0;
            else
                addn=m_vehicle->nParts;
            QComboBox * box = dynamic_cast<QComboBox*>(boxWidget);
            m_Methods[j%2][i+addn]=box->currentIndex();
        }
    }
}

void AerodynamicMethodDialog::setAeroMethods()
{
    int i;
    int j;
    int addn;
    for(i=0;i<m_vehicle->nParts;i++)
    {
        QTreeWidgetItem* parent=aeroTreeWidget->topLevelItem(i);//->findItems(m_vehicle->PartsOut[i].name,0);
        for(j=0;j<parent->childCount();j++)
        {
            QTreeWidgetItem* methodItem=parent->child(j);
            QWidget * boxWidget;
            boxWidget=aeroTreeWidget->itemWidget(methodItem,1);

            if(j<2)
                addn=0;
            else
                addn=m_vehicle->nParts;

            QComboBox * box = dynamic_cast<QComboBox*>(boxWidget);
            box->setCurrentIndex(m_Methods[j%2][i+addn]);
        }
    }
}

QComboBox* AerodynamicMethodDialog::expansionComboBox()
{
    QComboBox* box=new QComboBox();
    box->addItem(tr("Vacuum"), 0);
    box->addItem(tr("Newtonian"), 1);
    box->addItem(tr("Modified Newtonian - Prandtl Meyer"), 2);
    box->addItem(tr("Freestream Prandtl Meyer"), 3);
    box->addItem(tr("High Mach Base Pressure"), 4);
    box->addItem(tr("Van Dyke Unified"), 5);
    box->addItem(tr("ACM Empirical"), 6);
    return box;
}

QComboBox* AerodynamicMethodDialog::compressionComboBox()
{
    QComboBox* box=new QComboBox();
    box->addItem(tr("Newtonian"), 0);
    box->addItem(tr("Modified Newtonian"), 1);
    box->addItem(tr("Modified Newtonian - Prandtl-Meyer"), 2);
    box->addItem(tr("Tangent Wedge"), 3);
    box->addItem(tr("Tangent Wedge Empirical"), 4);
    box->addItem(tr("Tangent Cone Empirical"), 5);
    box->addItem(tr("Modified Dahlem Buck"), 6);
    box->addItem(tr("Van Dyke Unified"), 7);
    box->addItem(tr("Smyth Delta Wing"), 8);
    box->addItem(tr("Hankey Flat Surface"), 9);
    return box;
}

void AerodynamicMethodDialog::displayGeometryError(int Error)
{
    switch(Error)
    {
    case 1:
        QMessageBox::warning(this,tr("Incorrect Geometry"),tr("Zero area panel(s) in geometry"));
        break;
    case 2:
        QMessageBox::warning(this,tr("Incorrect Geometry"),tr("Planar geometry incompatible with automatic selection (nBottom=0)"));
        break;
    case 3:
        QMessageBox::warning(this,tr("Incorrect Geometry"),tr("Planar geometry incompatible with automatic selection (nTop=0)"));
        break;


    }
}

void AerodynamicMethodDialog::initializeAutoSettings()
{
    Parameters->C[0]=45*sta::Pi()/180;
    Parameters->C[1]=30;
    Parameters->C[2]=35*sta::Pi()/180;
    Parameters->C[3]=30*sta::Pi()/180;
    Parameters->C[4]=1.75;
    Parameters->C[5]=0.8;
    Parameters->k[0]=40*sta::Pi()/180;
    Parameters->k[1]=50;
    Parameters->k[2]=sta::Pi();

}

void AerodynamicMethodDialog::on_advancedSelectionPushButton_clicked()
{
    advancedSelectionGUI dialog(this);

    dialog.loadValues(Parameters,defaultMethods);
    if (dialog.exec() == QDialog::Accepted)
    {
        dialog.saveValues(Parameters,defaultMethods);
    }
}

void AerodynamicMethodDialog::setDefaultMethods()
{
    defaultMethods[0][0]=2;
    defaultMethods[0][1]=4;
    defaultMethods[0][2]=2;
    defaultMethods[0][3]=6;
    defaultMethods[1][0]=5;
    defaultMethods[1][1]=3;
    defaultMethods[1][2]=5;
    defaultMethods[1][3]=3;
    defaultMethods[2][0]=4;
    defaultMethods[2][1]=3;
    defaultMethods[2][2]=4;
    defaultMethods[2][3]=3;

}

void AerodynamicMethodDialog::writeGeomFile(QString filename)
{
    filename.remove(this->loadGeomDialog->directory().path() + "/");
    geomFileLineEdit->setText("data/vehiclewgs/"+filename);
    GeomFile=geomFileLineEdit->text();
    newVehicleSelected();
    //fileLoaded=1;
}

void AerodynamicMethodDialog::newVehicleSelected()
{
    int i,j;
    for(i=1;i<aeroTreeWidget->topLevelItemCount();i++)
    {
        QTreeWidgetItem* treeItem=aeroTreeWidget->topLevelItem(i);
        for(j=0;j<treeItem->childCount();j++)
        {
            QTreeWidgetItem * methodItem=treeItem->child(j);
            QWidget * boxWidget;
            boxWidget=aeroTreeWidget->itemWidget(methodItem,1);
            QComboBox * box = dynamic_cast<QComboBox*>(boxWidget);
            delete box;
            delete methodItem;
        }
        delete treeItem;

    }
    delete m_vehicle;
    delete [] m_Methods[0];
    delete [] m_Methods[1];
    m_vehicle=new VehicleGeometry(GeomFile);
    m_vehicle->CalculateGlobalCharacteristics();
    globChars.Rn=m_vehicle->NoseRadius;
    globChars.S=m_vehicle->Surface;
    globChars.V=m_vehicle->Volume;
    for(i=0;i<2;i++)
    {
        m_Methods[i]=new int[2*m_vehicle->nParts];
        for(int j=0;j<2*m_vehicle->nParts;j++)
            m_Methods[i][j]=1;

    }
    setTreeParts();
    setAeroMethods();
}

void AerodynamicMethodDialog::getGeomFileInfo(QString& filename, globCharStruct & geomOut)
{
    filename=GeomFile;
    filename.remove(0,16);
    geomOut=globChars;
}
