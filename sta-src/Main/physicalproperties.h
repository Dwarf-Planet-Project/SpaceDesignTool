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
