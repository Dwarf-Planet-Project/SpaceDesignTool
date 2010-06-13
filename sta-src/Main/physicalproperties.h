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

#ifndef PHYSICALPROPERTIES_H
#define PHYSICALPROPERTIES_H

#include <QDialog>
#include <QFileDialog>
#include "ui_physicalproperties.h"
#include "Scenario/staschema.h"

class ScenarioTree;
class ScenarioPhysicalProperties;

class PhysicalPropertiesDialog : public QDialog, private Ui_PhysicalPropertiesDialog
{
    Q_OBJECT

public:
    PhysicalPropertiesDialog(ScenarioTree* parent=NULL);
    ~PhysicalPropertiesDialog(){}
    ScenarioREVGeometryType* m_geometry;
    bool loadValues(ScenarioREVGeometryType* REVgeometry);
    bool saveValues(ScenarioREVGeometryType* REVgeometry, ScenarioREVAeroThermodynamicsType * REVaerothermo);
    double getRefArea();

public slots:
    void writeGeomFile(QString filename);
    void on_geomParaButton_clicked();
    void setGlobalCharacteristics(QString file);

private:
    QFileDialog* loadGeomDialog;
    bool fileLoaded;

};

#endif // PHYSICALPROPERTIES_H
