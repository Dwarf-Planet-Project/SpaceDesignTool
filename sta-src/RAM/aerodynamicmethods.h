#ifndef AERODYNAMICMETHODS_H
#define AERODYNAMICMETHODS_H

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

#include "ui_aerodynamicmethods.h"
#include "vehiclegeometry.h"
#include "Scenario/staschema.h"
#include <QDialog>
#include <QString>
#include <QComboBox>
#include <QFileDialog>

#include "aeroanalysis.h"

class AerodynamicMethodDialog : public QDialog , private Ui_AerodynamicMethodDialog
{
Q_OBJECT
public:
        AerodynamicMethodDialog(ScenarioREVAeroThermodynamicsType* aero, QString geomFileName, QWidget * parent = 0, Qt::WindowFlags f = 0 );
	~AerodynamicMethodDialog();
        struct globCharStruct
        {
            double Rn;
            double S;
            double V;
        };
        void getGeomFileInfo(QString& filename, globCharStruct& geomOut);
        QString GeomFile;
        QList<QString> getAeroFiles();
        PartAnalysis::SelectionStruct * Parameters;        
        globCharStruct globChars;
        bool fileChanged;

private:
        QFileDialog* loadGeomDialog;
        void displayGeometryError(int Error);
        void setTreeParts();
        void setDefaultMethods();
        void newVehicleSelected();
        QComboBox* expansionComboBox();
        QComboBox* compressionComboBox();
        void getAeroMethods();
        void setAeroMethods();
        void initializeAutoSettings();
        VehicleGeometry* m_vehicle;
        ScenarioREVAeroThermodynamicsType* m_aerothermo;
        int** m_Methods;
        int** defaultMethods;
        bool autoSelected;
        QList<QString> fileList;

public slots:
        void writeGeomFile(QString filename);
protected slots:
        void on_advancedSelectionPushButton_clicked();
        bool on_generatePushButton_clicked();
        void on_selectPushButton_clicked();
        void on_lowHighCheckBox_stateChanged();
};
#endif //AERODYNAMICMETHODDIALOG_H
