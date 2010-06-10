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
 ------------------ Author: Ozgun YILMAZ      ---------------------------------------------
 ------------------ email: ozgunus@yahoo.com  ---------------------------------------------

 */

#ifndef STRUCTUREGUI_H
#define STRUCTUREGUI_H

#include "ui_StructureGUI.h"
#include <QDialog>

#include "SemMain.h"

class StructureGUI : public QDialog , private Ui::StructureGUI
{
Q_OBJECT
public:
        StructureGUI(SemMain * SC, QWidget * parent = 0, Qt::WindowFlags f = 0 );
        ~StructureGUI();

protected slots:
	void on_StructureGUISCShapeComboBox_activated(const QString&);
        void on_StructureGUIMaterialComboBox_activated(const QString&);
        void on_StructureGUISavePushButton_clicked();

private:
        SemMain * SCVirtual;
//        void RefreshStructureGUI();


};
#endif //STRUCTUREGUI_H
