#ifndef ACTTOOL_H
#define ACTTOOL_H

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

#include <QtGui/QDialog>
#include <QFileDialog>
#include <QFile>
#include <QTextStream>
#include "Main/propagatedscenario.h"
#include "Scenario/scenario.h"
#include "ui_acttool.h"
#include "Act/actanalysis.h"
#include <QTextStream>
#include <Eigen/Array>
//#include <Eigen/LU>
//#include <Eigen/Cholesky>
//#include <Eigen/QR>
//#include <Eigen/SVD>
//#include <Eigen/Geometry>
//#include <Eigen/LeastSquares>
//#include <Eigen/Sparse>

// import most common Eigen types
USING_PART_OF_NAMESPACE_EIGEN

//class SpaceObject;
//class MissionArc;

namespace Ui {
    class ActTool;
}

class ActTool : public QDialog , public Ui_ActTool {
    Q_OBJECT
public:
    ActTool(QWidget *parent = 0);
    ~ActTool();

     void setScenario(SpaceScenario* scenario, PropagatedScenario* propscenario);

protected:
    void changeEvent(QEvent *e);

private:
    Ui::ActTool *m_ui;
    SpaceScenario* m_scenario;
    PropagatedScenario* m_propscenario;
    act::ActAnalysis* analysis;

private slots:
    void on_SaveButton_clicked();
    void updateComboBox(int currentvehicleindex);
    void updateSpinBox(int currenttrajectoryindex);
};

#endif // ACTTOOL_H
