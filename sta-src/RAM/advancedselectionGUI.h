#ifndef ADVANCEDSELECTIONGUI_H
#define ADVANCEDSELECTIONGUI_H

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
 ------------------ E-mail: (d.dirkxAT_student.tudelft.nl) ----------------------------
*/
#include "partgeometry.h"
#include "ui_advancedselectionGUI.h"
#include "aerodynamicmethods.h"
#include <QObject>
#include <QDialog>
#include <QString>


class advancedSelectionGUI : public QDialog , private Ui_advancedSelectionGUI
{
Q_OBJECT
public:
    advancedSelectionGUI(QWidget * parent=0, Qt::WindowFlags f=0);

    void loadValues(PartAnalysis::SelectionStruct * Parameters, int** methods);
    void saveValues(PartAnalysis::SelectionStruct * Parameters, int** methods);
};

#endif // ADVANCEDSELECTIONGUI_H
