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

#include "Astro-Core/stamath.h"
#include "advancedselectionGUI.h"
#include "aeroanalysis.h"

advancedSelectionGUI::advancedSelectionGUI(QWidget * parent, Qt::WindowFlags f) : QDialog(parent,f)
{
    setupUi(this);
}

void advancedSelectionGUI::loadValues(PartAnalysis::SelectionStruct * Parameters, int** methods)
{
    C1LineEdit->setText(QString::number(Parameters->C[0]*180/sta::Pi()));
    C2LineEdit->setText(QString::number(Parameters->C[1]));
    C3LineEdit->setText(QString::number(Parameters->C[2]*180/sta::Pi()));
    C4LineEdit->setText(QString::number(Parameters->C[3]*180/sta::Pi()));
    C5LineEdit->setText(QString::number(Parameters->C[4]));
    C6LineEdit->setText(QString::number(Parameters->C[5]));

    k1LineEdit->setText(QString::number(Parameters->k[0]*180/sta::Pi()));
    k2LineEdit->setText(QString::number(Parameters->k[1]));
    k3LineEdit->setText(QString::number(Parameters->k[2]*180/sta::Pi()));
    comboBox11->setCurrentIndex(methods[0][0]);
    comboBox12->setCurrentIndex(methods[0][1]);
    comboBox13->setCurrentIndex(methods[0][2]);
    comboBox14->setCurrentIndex(methods[0][3]);
    comboBox21->setCurrentIndex(methods[1][0]);
    comboBox22->setCurrentIndex(methods[1][1]);
    comboBox23->setCurrentIndex(methods[1][2]);
    comboBox24->setCurrentIndex(methods[1][3]);
    comboBox31->setCurrentIndex(methods[2][0]);
    comboBox32->setCurrentIndex(methods[2][1]);
    comboBox33->setCurrentIndex(methods[2][2]);
    comboBox34->setCurrentIndex(methods[2][3]);


}

void advancedSelectionGUI::saveValues(PartAnalysis::SelectionStruct * Parameters, int ** methods)
{
    Parameters->C[0]=C1LineEdit->text().toDouble()*sta::Pi()/180;
    Parameters->C[1]=C2LineEdit->text().toDouble();
    Parameters->C[2]=C3LineEdit->text().toDouble()*sta::Pi()/180;
    Parameters->C[3]=C4LineEdit->text().toDouble()*sta::Pi()/180;
    Parameters->C[4]=C5LineEdit->text().toDouble();
    Parameters->C[5]=C6LineEdit->text().toDouble();
    Parameters->k[0]=k1LineEdit->text().toDouble()*sta::Pi()/180;
    Parameters->k[1]=k2LineEdit->text().toDouble();
    Parameters->k[2]=k3LineEdit->text().toDouble()*sta::Pi()/180;
    methods[0][0]=comboBox11->currentIndex();
    methods[0][1]=comboBox12->currentIndex();
    methods[0][2]=comboBox13->currentIndex();
    methods[0][3]=comboBox14->currentIndex();
    methods[1][0]=comboBox21->currentIndex();
    methods[1][1]=comboBox22->currentIndex();
    methods[1][2]=comboBox23->currentIndex();
    methods[1][3]=comboBox24->currentIndex();
    methods[2][0]=comboBox31->currentIndex();
    methods[2][1]=comboBox32->currentIndex();
    methods[2][2]=comboBox33->currentIndex();
    methods[2][3]=comboBox34->currentIndex();


}


