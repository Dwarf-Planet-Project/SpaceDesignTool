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
