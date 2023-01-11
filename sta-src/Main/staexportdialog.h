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
------------------ Author: Chris Laurel  -------------------------------------------------
 ------------------ E-mail: (claurel@gmail.com) ----------------------------
 */

#ifndef _STA_EXPORT_DIALOG_H_
#define _STA_EXPORT_DIALOG_H_

#include <QWidget>
#include <QIODevice>
#include "ui_staexportdialog.h"



class PropagatedScenario;
	
class staExportDialog : public QDialog, private Ui_staExportDialog
{
    Q_OBJECT
    
public:
    staExportDialog(PropagatedScenario* m_scenario, QWidget* parent = NULL);
    ~staExportDialog();

    bool exportTrajectory(QIODevice* out);

private:
    PropagatedScenario* m_scenario;
};
#endif // _STA_EXPORT_DIALOG_H_
