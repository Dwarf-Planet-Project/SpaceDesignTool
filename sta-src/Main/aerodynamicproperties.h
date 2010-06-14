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

#ifndef AERODYNAMICPROPERTIESDIALOG_H
#define AERODYNAMICPROPERTIESDIALOG_H

#include <QDialog>
#include "ui_aerodynamicproperties.h"
#include "RAM/aerodynamicmethods.h"
#include <QFileDialog>
#include "Scenario/staschema.h"


class ScenarioTree;
class ScenarioAerodynamicProperties;

class AerodynamicPropertiesDialog : public QDialog, private Ui_AerodynamicPropertiesDialog
{
    Q_OBJECT

public:
    AerodynamicPropertiesDialog(ScenarioTree* parent=NULL);
    ~AerodynamicPropertiesDialog(){}

    bool loadValues(ScenarioREVAeroThermodynamicsType* aero, ScenarioREVGeometryType* geom=NULL);
    bool saveValues(ScenarioREVAeroThermodynamicsType* aero, ScenarioREVGeometryType* geom=NULL);
    AerodynamicMethodDialog::globCharStruct globChars;
    QString geomFileName;



public slots:
    void writeCDFile(QString filename);
    void writeCSFile(QString filename);
    void writeCLFile(QString filename);
    void writeClFile(QString filename);
    void writeCmFile(QString filename);
    void writeCnFile(QString filename);
    void on_fromGeomPushButton_clicked();


private:
    QFileDialog* loadCDDialog;
    QFileDialog* loadCSDialog;
    QFileDialog* loadCLDialog;
    QFileDialog* loadClDialog;
    QFileDialog* loadCmDialog;
    QFileDialog* loadCnDialog;
    ScenarioREVAeroThermodynamicsType* m_aerothermo;

};

#endif // AERODYNAMICPROPERTIESDIALOG_H
