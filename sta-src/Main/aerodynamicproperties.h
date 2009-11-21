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
#include <QFileDialog>

class ScenarioTree;
class ScenarioAerodynamicProperties;

class AerodynamicPropertiesDialog : public QDialog, private Ui_AerodynamicPropertiesDialogClass
{
    Q_OBJECT

public:
    AerodynamicPropertiesDialog(ScenarioTree* parent=NULL);
    ~AerodynamicPropertiesDialog(){}

    bool loadValues(ScenarioAerodynamicProperties* aerodynamicProperties);
    bool saveValues(ScenarioAerodynamicProperties* aerodynamicProperties);


public slots:
    void writeCdFile(QString filename);
    void writeCdpFile(QString filename);

private:
    QFileDialog* loadCDDialog;
    QFileDialog* loadCDpDialog;

};

#endif // AERODYNAMICPROPERTIESDIALOG_H
