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
 ------ Copyright (C) 2010 European Space Agency (space.trajectory.analysis AT gmail.com) ----
 ------------------ Author: Marco Di Perna  -------------------------------------------------
 ------------------ E-mail: (marco.diperna@engenio.eu) --------------------------------------
 */

#ifndef _AVTACT_H
#define _AVTACT_H

#include <QMainWindow>
#include <QDialog>
#include <QTreeWidgetItem>
#include "ui_AVTACT.h"



#include "Main/exportdialog.h"
#include "Scenario/scenario.h"
#include "Scenario/staschema.h"
#include "Astro-Core/stacoordsys.h"
#include "Astro-Core/date.h"
#include "Main/propagatedscenario.h"
#include "Main/timelineview.h"
#include "ActAvt/Act/actanalysis.h"
#include "ActAvt/Act/actcovariance.h"
#include "avtanalysis.h"


class PropagatedScenario;
class ACT_input;
class TimelineWidget;
class TimelineView;
class act::ActAnalysis;
class AvtAnalysis;
class avt_input_class;
class SS_struct;

class sat_arcs
{
    public:
    int sat;
    int missionAvailable;
    QList<int> arcs;
    QList<int> availability;
    QList<double> beginning;
    QList<double> ending;
    QList<double> startProp;
    QList<double> stopProp;
};

class prop_sat_arcs
{
    public:
    int sat;
    QList<int> arcs;
    QList<double> startProp;
    QList<double> stopProp;
};

class input_class
{
 public:
    int prop_type; //Linear = 0; MonteCarlo = 1;
    int an_type;  // ACT = 0; AVT = 1;
    int input_type_ind;
    int space_object_ind;
    int traj_arc_ind;
    int treat_num;
    int fact_num;
    int sample_num;
    sta::CoordinateSystemType ref_type_ind;
    act::ActElementType coord_type_ind;
    act::ActDistributionType distribution_ind;

    QList<MatrixXd> distribution_vect1;
    QList<MatrixXd> covariance;
    QList<double> steps_date;
    QList<int> steps_num;
};


class AVTACTDialog : public QDialog, public Ui_AVTACT_DialogClass
{
    Q_OBJECT

public:
    AVTACTDialog(SpaceScenario* scenario, PropagatedScenario* prop_scenario, QList<QTreeWidgetItem *> selectedItem, QList<double> startTime, QList<double> stopTime);
    ~AVTACTDialog();
    QTabWidget* ACT_inputTab;

    QList<input_class> m_input;
    QList<int> m_outputList;
    input_class current_input;
    int organize_sat_arcs(QList<QTreeWidgetItem *> selectedItem);
    QList<int> organize_time_intervals(QList<double> startTime, QList<double> stopTime);
    int verify_arcsVStime();

private:
    SpaceScenario* m_scenario;
    PropagatedScenario* m_prop_scenario;
    ACT_input* m_ACT_input,*m_AVT_input,*m_smpl_input,*m_curr_widg;
    act::ActAnalysis *m_act_analysis;
    AvtAnalysis* m_avt_analysis;
    QList<avt_input_class> m_all_anova_input;
    QString m_fileName;

    QList<int> m_arc;
    QList<double> m_startTime;
    QList<double> m_stopTime;
    QList<QTreeWidgetItem *> m_selectedItem;
    bool m_check, m_check2;
    double m_t0, m_tf, m_t1, m_t2;
    
    MatrixXd Mat_List_Member;
    MatrixXd Vect_List_Member;

    int m_an_type_ind;
    int m_input_type_ind;
    int m_space_object_ind;
    int m_traj_arc_ind;
    int m_factors;
    int m_sample_num;
    int m_propagation_type;
    sta::CoordinateSystemType m_ref_type_ind;
    act::ActElementType m_coord_type_ind;
    act::ActDistributionType m_distribution_ind;

    sta::CoordinateSystemType m_coordType;
    act::ActElementType m_elemType;
    act::ActDistributionType m_distribution;

    void set_time_view(int object, int arc);
    MatrixXd vect2matrix(int n_col,int n_row);
    void GUI_configuration();
    void run_ACT();
    void run_AVT();
    void set_AVTACT_general_input();
    void read_current_input();
    void read_input_widget(int);
    int read_output_tree(); //UPA2
    int ACT_initialization(int input_type_ind, MatrixXd distribution_vect, MatrixXd covariance, int index,bool act, MatrixXd sample);
    FILE* write_output(FILE *woutFile,double curr_step,VectorXd SS_tot,QList<SS_struct> SS_k,VectorXd err,VectorXd err_MS,int err_dof,int n_fact);
    FILE* write_error(FILE *woutFile,double curr_step,VectorXd err,VectorXd err_MS,int err_dof,int n_fact);
    MatrixXd read_input_wid_matrix(QGridLayout* curr_mat,int n_col, int n_row);
    MatrixXd vector2matrix(QList<double> vect, int n_row, int n_col);
    QString save_output_file();

    //__________UPA2______________
    QList<sat_arcs> m_satsArcs;
    QList<prop_sat_arcs> m_prop_satsArcs;

private slots:
    void on_input_combo_currentIndexChanged(int);
    void on_ACT_rbut_clicked(bool);
    void on_AVT_rbut_clicked(bool);
    void on_run_pushb_clicked(bool);
    void on_ref_system_combo_currentIndexChanged(int ind);
    void on_coord_type_combo_currentIndexChanged(int);
    void on_initialise_input_boxes_clicked(bool);
    void on_store_input_clicked(bool);
    bool on_reload_input_clicked(bool);
    void disable_run();
    //__________UPA2______________
    void on_propagation_currentIndexChanged(int);
};


#endif // _AVTACT_H
