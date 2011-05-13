//- Copyright (C) 2010 STA Steering Board (space.trajectory.analysis AT gmail.com) -
//
// This program is free software; you can redistribute it and/or modify it under
// the terms of the European Union Public License version 1.1
//
// This program is distributed in the hope that it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
// FOR A PARTICULAR PURPOSE.  See the European Union Public License for more
// details.
//
//
// Further information about the European Union Public License can also be found on
// the world wide web at http://ec.europa.eu/idabc/eupl
//
//------------------ Authors:     Marco Di Perna                 -------------------
//------------------              Elisa Ceriani                  -------------------
//------------------ Affiliation: ENGENIO SRL                    -------------------
//------------------ E-mail:      marco.diperna AT engenio.eu    -------------------
//------------------              elisa.ceriani AT engenio.eu    -------------------
//----------------------------------------------------------------------------------


#include "AVTACT.h"
#include "ui_AVTACT.h"
#include "ACT_input.h"
#include <QMessageBox>
#include <QtGui>

using namespace act;


AVTACTDialog::AVTACTDialog(SpaceScenario* scenario, PropagatedScenario* prop_scenario, QList<QTreeWidgetItem *> selectedItem, QList<double> startTime, QList<double> stopTime) : QDialog()
{
    setupUi(this);
    QSize right_size =this->size();

    this->setFixedSize(right_size.rwidth(),right_size.rheight());

    if (!prop_scenario)
    return;

    /* hide TEST text boxes */
    test->setVisible(false);
    test_2->setVisible(false);

    /* Create private variable scenario */
    m_prop_scenario = prop_scenario;
    m_scenario = scenario;

    //____________UPA2_________________________________________________

    m_propagation_type=propagation->currentIndex();
    organize_sat_arcs(selectedItem);
    organize_time_intervals(startTime, stopTime);

    //Generate list of satellite and arcs to be propagated-------------
    QString ts;

    for (int i=0; i<m_satsArcs.count(); i++)
    {
        if (m_satsArcs.at(i).missionAvailable==1)
        {
            prop_sat_arcs temporary;
            temporary.sat=m_satsArcs.at(i).sat;
            for (int k=0; k<m_satsArcs.at(i).arcs.count(); k++)
            {
                if(m_satsArcs.at(i).availability.at(k)==1)
                {
                    temporary.arcs.append(m_satsArcs.at(i).arcs.at(k));
                    temporary.startProp.append(m_satsArcs.at(i).startProp.at(k));
                    temporary.stopProp.append(m_satsArcs.at(i).stopProp.at(k));
                }
            }
            this->m_prop_satsArcs.append(temporary);
        }
    }

    //Write sat arc table---------------------------------------------
    int count=0;

    QDateTimeEdit date;
    double mjd_start,mjd_stop;
    for (int i=0; i<m_prop_satsArcs.count(); i++)
    {
        for (int k=0; k<m_prop_satsArcs.at(i).arcs.count(); k++)
        {
            QTableWidgetItem* arc=new QTableWidgetItem(ts.setNum(m_prop_satsArcs.at(i).arcs.at(k)),0);
            QTableWidgetItem* sat_name=new QTableWidgetItem(m_prop_scenario->spaceObjects().at(m_prop_satsArcs.at(i).sat)->name(),0);

            mjd_start=m_prop_satsArcs.at(i).startProp.at(k);
            mjd_stop=m_prop_satsArcs.at(i).stopProp.at(k);

            tableWidgetSatArc->insertRow(count);

            tableWidgetSatArc->setItem(count,1,arc);
            tableWidgetSatArc->setItem(count,0,sat_name);

            date.setDateTime(sta::JdToCalendar(sta::MjdToJd(mjd_start)));
            QTableWidgetItem* begin=new QTableWidgetItem(date.text(),0);
            tableWidgetSatArc->setItem(count,2,begin);

            date.setDateTime(sta::JdToCalendar(sta::MjdToJd(mjd_stop)));
            QTableWidgetItem* end=new QTableWidgetItem(date.text(),0);
            tableWidgetSatArc->setItem(count,3,end);
            count++;
        }
     }

    //____________________________________________________________________

    //ACT_input box dock item--------------------------------------------
    ACT_inputTab = new QTabWidget(this->InputGroup);
    ACT_inputTab->move(5, 20);

    //ACT widget set up---------------------------------------------------
    m_ACT_input = new ACT_input(this->InputGroup);
    m_ACT_input->treat_grid->itemAtPosition(0,0)->widget()->setVisible(false);
    m_ACT_input->treat_grid->itemAtPosition(0,1)->widget()->setVisible(false);
    m_ACT_input->treat_grid->itemAtPosition(0,2)->widget()->setVisible(false);
    m_ACT_input->setVisible(false);

    //AVT generic input widget-------------------------------------------
    m_AVT_input = new ACT_input(this->InputGroup);
    int ncol=m_AVT_input->vett_grid->columnCount();
    int nrow=m_AVT_input->vett_grid->rowCount();

    connect(m_AVT_input->treat_num,SIGNAL(editingFinished()),this,SLOT(disable_run()));
    for (int j=1; j<ncol; j++)
    {
        for (int i=1; i<nrow; i++)
        {
            delete m_AVT_input->vett_grid->itemAtPosition(i,j)->widget();
            m_AVT_input->vett_grid->addWidget(new QCheckBox(),i,j,Qt::AlignHCenter);
            QCheckBox* this_check=dynamic_cast<QCheckBox*>(m_AVT_input->vett_grid->itemAtPosition(i,j)->widget());
            connect(this_check,SIGNAL(clicked()),this,SLOT(disable_run()));
        }
    }

    ncol=m_AVT_input->mat_grid->columnCount();
    nrow=m_AVT_input->mat_grid->rowCount();
    for (int j=1; j<ncol; j++)
    {
        for (int i=1; i<nrow; i++)
        {
            delete m_AVT_input->mat_grid->itemAtPosition(i,j)->widget();
            m_AVT_input->mat_grid->addWidget(new QCheckBox(),i,j,Qt::AlignHCenter);
            QCheckBox* this_check=dynamic_cast<QCheckBox*>(m_AVT_input->mat_grid->itemAtPosition(i,j)->widget());
            connect(this_check,SIGNAL(clicked()),this,SLOT(disable_run()));
        }
    }
    m_AVT_input->setVisible(false);

    //ACT AVT load file-------------------------------------------
    m_smpl_input = new ACT_input(this->InputGroup);
    ncol=m_smpl_input->vett_grid->columnCount();
    nrow=m_smpl_input->vett_grid->rowCount();
    for (int j=0; j<ncol; j++)
    {
        for (int i=0; i<nrow; i++)
        {
              delete m_smpl_input->vett_grid->itemAtPosition(i,j)->widget();
        }
    }

    ncol=m_smpl_input->mat_grid->columnCount();
    nrow=m_smpl_input->mat_grid->rowCount();
    for (int j=0; j<ncol; j++)
    {
        for (int i=0; i<nrow; i++)
        {
            delete m_smpl_input->mat_grid->itemAtPosition(i,j)->widget();
        }
    }

    m_smpl_input->distribution_combo->setVisible(false);
    m_smpl_input->label->setVisible(false);
    m_smpl_input->sample_num->setVisible(false);
    m_smpl_input->label_2->setVisible(false);
    m_smpl_input->setVisible(false);
    m_smpl_input->treat_grid->itemAtPosition(0,2)->widget()->setVisible(false);

    dynamic_cast<QLabel*>(m_smpl_input->treat_grid->itemAtPosition(0,0)->widget())->setText("Number of factors");

    m_smpl_input->file_loading_grid=new QGridLayout(this->m_smpl_input);

    m_smpl_input->file_loading_grid->addWidget(new QPushButton,0,0,Qt::Alignment());
    m_smpl_input->file_loading_grid->addWidget(new QLineEdit,0,1,Qt::Alignment());
    dynamic_cast<QPushButton*>(m_smpl_input->file_loading_grid->itemAtPosition(0,0)->widget())->setText("Load sample file");
    dynamic_cast<QLineEdit*>(m_smpl_input->file_loading_grid->itemAtPosition(0,1)->widget())->setReadOnly(true);
    connect(dynamic_cast<QPushButton*>(m_smpl_input->file_loading_grid->itemAtPosition(0,0)->widget()),SIGNAL(clicked()),this->m_smpl_input,SLOT(on_load_file_clicked()));
    m_smpl_input->setVisible(false);


    //Input type------------------------------------------------------------------
    QStringList type_list = (QStringList() << "RMS" << "Statistical parameters"
                             << "Covariance Matrix" << "Samples");

    input_combo->addItems(type_list);

    //Reference system-------------------------------------------------------------
    ref_system_combo->addItem(tr("Inertial (J2000)"), (int) sta::COORDSYS_EME_J2000);
    ref_system_combo->addItem(tr("Inertial (B1950)"), (int) sta::COORDSYS_EME_B1950);
    ref_system_combo->addItem(tr("Ecliptic (J2000)"), (int) sta::COORDSYS_ECLIPTIC_J2000);
    ref_system_combo->addItem(tr("Planet Fixed"), (int) sta::COORDSYS_BODYFIXED);
    ref_system_combo->addItem(tr("Rotating r.f."), (int) sta::COORDSYS_ROT);
    ref_system_combo->addItem(tr("Rotating normalized r.f."), (int) sta::COORDSYS_ROT_NORM);

    //Coord type system-------------------------------------------------------------
    coord_type_combo->addItem(tr("Cartesian"), (int) act::ELEM_CARTESIAN);
    coord_type_combo->addItem(tr("Keplerian"), (int) act::ELEM_KEPLERIAN);
    coord_type_combo->addItem(tr("Spherical"), (int) act::ELEM_SPHERICAL);

    m_check=true; //check the actions on ACT, AVT radio buttons
    m_check2=true; //check the actions on propagation combo box

    this->maximumSize();
  

    //-------------------------- Set up the combo boxes------------------------------//

    ref_system_combo->setCurrentIndex(0);
    coord_type_combo->setCurrentIndex(0);
    input_combo->setCurrentIndex(2);

    //-------------------------- Set up Output tree Widget----------------------------//

    this->treeWidgetOuputList->setColumnHidden(1,true);
    connect(quit,SIGNAL(clicked()),this,SLOT(close()));
}


void AVTACTDialog::on_input_combo_currentIndexChanged(int ind)
{
    QSize Tabsize= QSize(InputGroup->geometry().width()-10,InputGroup->geometry().height()-25);

    //Remove old tabs
    int old_tab_number=ACT_inputTab->count();
    for (int i=old_tab_number; i>0; i=i-1)
    {
        ACT_inputTab->removeTab(i-1);
    }
    ACT_input* input_widget;
    //Create new tabs
    switch ( ind )
    {
        case 0:
        {
            if(ACT_rbut->isChecked())
            {
                input_widget=m_ACT_input;
                ACT_inputTab->addTab(m_ACT_input,tr("Root Mean Squares"));
                m_ACT_input->transfer = this;
            }
            else
            {
                input_widget=m_AVT_input;
                ACT_inputTab->addTab(m_AVT_input,tr("Factor and treatments"));
                m_AVT_input->transfer = this;
            }
            if(m_propagation_type==0)
            {
                input_widget->label->setVisible(false);
                input_widget->sample_num->setVisible(false);
            }
            else
            {
                input_widget->label->setVisible(true);
                input_widget->sample_num->setVisible(true);
            }
            input_widget->distribution_combo->setVisible(false);
            input_widget->label_2->setVisible(false);
            input_widget->distribution_set_up(input_widget,9);
        }
        break;
        case 1:
        {            
            if(ACT_rbut->isChecked())
            {
                input_widget=m_ACT_input;
                m_ACT_input->on_distribution_combo_currentIndexChanged(0);
                ACT_inputTab->addTab(m_ACT_input,tr("Statistical parameters"));
                m_ACT_input->transfer = this;
            }
            else
            {
                input_widget=m_AVT_input;
                ACT_inputTab->addTab(m_AVT_input,tr("Factor and treatments"));
                m_AVT_input->transfer = this;
            }
            input_widget->label->setVisible(true);
            input_widget->label_2->setVisible(true);
            input_widget->sample_num->setVisible(true);
            input_widget->distribution_combo->setVisible(true);
            input_widget->on_distribution_combo_currentIndexChanged(0);
        }
        break;
        case 2:
        {
            if(ACT_rbut->isChecked())
            {
                input_widget=m_ACT_input;
                ACT_inputTab->addTab(m_ACT_input,tr("Covariance Matrix"));
                m_ACT_input->transfer = this;
            }
            else
            {
                input_widget=m_AVT_input;
                ACT_inputTab->addTab(m_AVT_input,tr("Factor and treatments"));
                m_AVT_input->transfer = this;
            }
             if(m_propagation_type==0)
            {
                input_widget->label->setVisible(false);
                input_widget->sample_num->setVisible(false);
            }
            else
            {
                input_widget->label->setVisible(true);
                input_widget->sample_num->setVisible(true);
            }
            input_widget->label_2->setVisible(false);
            input_widget->distribution_combo->setVisible(false);
            input_widget->distribution_set_up(input_widget,8);
        }
        break;
        case 3:
        {
            if(ACT_rbut->isChecked())
            {
                m_smpl_input->treat_grid->itemAtPosition(0,0)->widget()->setVisible(false);
                m_smpl_input->treat_num->setVisible(false);
                m_smpl_input->treat_num->setText("0");
                dynamic_cast<QLineEdit*>(m_smpl_input->file_loading_grid->itemAtPosition(0,1)->widget())->clear();
            }
            else
            {
                m_smpl_input->treat_grid->itemAtPosition(0,0)->widget()->setVisible(true);
                m_smpl_input->treat_num->setVisible(true);
                m_smpl_input->treat_num->setText("1");
                dynamic_cast<QLineEdit*>(m_smpl_input->file_loading_grid->itemAtPosition(0,1)->widget())->clear();
            }
            ACT_inputTab->addTab(m_smpl_input,tr("Load file"));
            m_smpl_input->transfer=this;
        }
        break;
        default:
        {
            ACT_inputTab->setVisible(false);
            ACT_inputTab->setEnabled(false);
        }
    }
    ACT_inputTab->resize(Tabsize);
    ACT_inputTab->setVisible(true);
    ACT_inputTab->setEnabled(true);
}


void AVTACTDialog::on_AVT_rbut_clicked(bool j)
{
    if (ACT_inputTab->isVisible() && m_check==true)
    {
        QMessageBox msgBox;
        msgBox.setWindowTitle("Uncertainty Propagation Analysis");
        msgBox.setText("If you change the type of the analysis all the input inserted will be lost.");
        msgBox.setInformativeText("Do you want to continue?");
        msgBox.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
        msgBox.setDefaultButton(QMessageBox::Cancel);
        int ret = msgBox.exec();
        switch (ret)
        {
            case QMessageBox::Ok:
            {
                run_pushb->setDisabled(true);
                this->reload_input->setDisabled(true);
                this->store_input->setDisabled(true);
                ACT_inputTab->removeTab(0);
                this->on_input_combo_currentIndexChanged(input_combo->currentIndex());
                m_check=true;
                break;
            }
            case QMessageBox::Cancel:
            {
                m_check=false;
                this->reload_input->setDisabled(false);
                this->store_input->setDisabled(false);
                ACT_rbut->click();
                m_check=true;
                break;
            }
            default:
            break;
        }
    }
    return;
}

void AVTACTDialog::on_ACT_rbut_clicked(bool)
{
    if (ACT_inputTab->isVisible()&& m_check==true)
    {
        QMessageBox msgBox;
        msgBox.setWindowTitle("Uncertainty Propagation Analysis");
        msgBox.setText("If you change the type of the analysis all the input inserted will be lost.");
        msgBox.setInformativeText("Do you want to continue?");
        msgBox.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
        msgBox.setDefaultButton(QMessageBox::Cancel);
        int ret = msgBox.exec();
        switch (ret)
        {
            case QMessageBox::Ok:
            {
                run_pushb->setDisabled(false);
                this->reload_input->setDisabled(false);
                this->store_input->setDisabled(false);
                ACT_inputTab->removeTab(0);
                this->on_input_combo_currentIndexChanged(input_combo->currentIndex());
                m_check=true;
                break;
            }
            case QMessageBox::Cancel:
            {
                m_check=false;
                this->reload_input->setDisabled(true);
                this->store_input->setDisabled(true);
                AVT_rbut->click();
                m_check=true;
                break;
            }
            default:
            break;
        }
        return;
    }
}


void AVTACTDialog::on_ref_system_combo_currentIndexChanged(int ind)
{
    current_input.ref_type_ind=(sta::CoordinateSystemType) ref_system_combo->itemData(ind).toInt();
}

void AVTACTDialog::on_coord_type_combo_currentIndexChanged(int ind)
{
    current_input.coord_type_ind=(act::ActElementType) coord_type_combo->itemData(ind).toInt();
}

void AVTACTDialog::disable_run()
{
    //If AVT is selected the run pushbutton is enabled only after the confirm pushbutton is pressed
    run_pushb->setDisabled(true);
}


void AVTACTDialog::on_initialise_input_boxes_clicked(bool)
{
    // Search for current widget ----------------------
    int widg_num=this->ACT_inputTab->count();

    if (widg_num>1)
    {
        for (int i=widg_num-1; i>0;i=i-1)
        {
            this->ACT_inputTab->removeTab(i);
        }
    }

    m_curr_widg = dynamic_cast<ACT_input *>(this->ACT_inputTab->widget(0));

    int sub_ind;
    int ind=this->input_combo->currentIndex();
    switch ( ind )
    {
        case 0:
        {
            m_curr_widg->distribution_set_up(m_curr_widg,9);
        }
        break;
        case 1:
        {
            sub_ind=m_curr_widg->distribution_combo->currentIndex();
            m_curr_widg->distribution_set_up(m_curr_widg,sub_ind);
        }
        break;
        case 2:
        {
            m_curr_widg->distribution_set_up(m_curr_widg,8);
        }
        break;
        case 3:
        {
            dynamic_cast<QLineEdit*>(m_curr_widg->file_loading_grid->itemAtPosition(0,1)->widget())->setText("");
        }
        break;
        default:
        {

        }
    }
}

void AVTACTDialog::on_store_input_clicked(bool)
{ 
    // Useful var definition ----------------------------
    bool replace=FALSE;
    int element_num=0;
    int n_input = m_input.count();

    read_current_input();

    // TEST VARIABLE DEFINE  ----------------------------------
    int elem;
    if (current_input.covariance.count()>1)
        elem = current_input.covariance.at(0).operator ()(0,1);
    else
        elem = 0;
    // TEST END-----------------------------------------------

    // Search for current combo configuration in the stored input class ---

    for (int f=0;f<n_input;f++)
    {
        element_num++;
        if (m_input.at(f).an_type==m_an_type_ind &&
            m_input.at(f).coord_type_ind==m_coord_type_ind &&
            m_input.at(f).distribution_ind==m_distribution &&
            m_input.at(f).input_type_ind==m_input_type_ind &&
            m_input.at(f).ref_type_ind==m_ref_type_ind &&
            m_input.at(f).space_object_ind==m_space_object_ind &&
            m_input.at(f).traj_arc_ind==m_traj_arc_ind)
        {
            replace=TRUE;   // if yes variable will be replaced
            break;
        }
    }

    if (!replace)  // replace input class list element if it exists
    {
        m_input.append(current_input);
    }
    else  // add new input class list element if it does not exists yet
    {
        m_input.replace(element_num-1,current_input);
    }
}

bool AVTACTDialog::on_reload_input_clicked(bool)
{
    // Search for current widget ----------------------
    m_curr_widg = dynamic_cast<ACT_input*>(this->ACT_inputTab->widget(0));
    QGridLayout *vett_grid=m_curr_widg->vett_grid;
    QGridLayout *mat_grid=m_curr_widg->mat_grid;

    // Define useful vars----------------------------------
    int element_num=0;
    int n_input = m_input.count();
    bool reload=FALSE;
    QString ts;
    read_current_input();

    // Proceed if at least an element exists in the input list------------------
    if (n_input>0)
    {
        // Search for current combo configuration in the stored input class ---
        for (int f=0;f<n_input;f++)
        {
            element_num++;
            if (m_input.at(f).an_type==m_an_type_ind &&
                m_input.at(f).coord_type_ind==m_coord_type_ind &&
                m_input.at(f).distribution_ind==m_distribution &&
                m_input.at(f).input_type_ind==m_input_type_ind &&
                m_input.at(f).ref_type_ind==m_ref_type_ind &&
                m_input.at(f).space_object_ind==m_space_object_ind &&
                m_input.at(f).traj_arc_ind==m_traj_arc_ind)
            {
                reload=TRUE;  // if yes variable will be reloaded
                break;
            }
        }


        // Write the found element in a input class variable ----------
        input_class stored_input = m_input.at(element_num-1);

        // Define FOR limits -------------------------------------------
        int n_row=mat_grid->rowCount();
        int n_col=mat_grid->columnCount();
        int n_vect_col=vett_grid->columnCount();

        // Proceed only if the element corresponding to the current config exists --
        if (reload)
        {
            for(int j=1;j<n_row;j++)
            {
                for(int k=1;k<n_col;k++)
                {
                    QWidget* grid_item = mat_grid->itemAtPosition(j,k)->widget();
                    if (dynamic_cast<QLineEdit*>(grid_item))
                    {
                        (dynamic_cast<QLineEdit*>(mat_grid->itemAtPosition(j,k)->widget()))->setText(ts.setNum(stored_input.covariance.at(0).operator ()(j-1,k-1)));
                    }

                }
                for(int z=1;z<n_vect_col;z++)
                {
                    QWidget* vect_grid_item = vett_grid->itemAtPosition(j,z)->widget();
                    if (dynamic_cast<QLineEdit*>(vect_grid_item))
                    {
                        (dynamic_cast<QLineEdit*>(vett_grid->itemAtPosition(j,z)->widget()))->setText(ts.setNum(stored_input.distribution_vect1.at(0).operator ()(j-1,z-1)));
                    }
                }
            }
        }
    }
    if(!reload)
    {
        QMessageBox msgBox;
        msgBox.setText("No input stored for the current configuration.");
        msgBox.setWindowTitle("Uncertainty Propagation Analysis");
        msgBox.setIcon(QMessageBox::Warning);
        msgBox.setStandardButtons(QMessageBox::Ok);
        msgBox.setDefaultButton(QMessageBox::Ok);
        msgBox.exec();
        reload=false;
    }
    return (reload);
}

void AVTACTDialog::GUI_configuration()
{
    bool ok;
    // Store current combo configuration ----------------
    m_an_type_ind=this->AVT_rbut->isChecked();
    m_ref_type_ind=(sta::CoordinateSystemType) ref_system_combo->itemData(ref_system_combo->currentIndex()).toInt();
    m_coord_type_ind=(act::ActElementType) coord_type_combo->itemData(coord_type_combo->currentIndex()).toInt();
    m_distribution=(act::ActDistributionType)m_curr_widg->distribution_combo->itemData(m_curr_widg->distribution_combo->currentIndex()).toInt();
    m_input_type_ind=this->input_combo->currentIndex();

    // UPA2 Analyses only the one satellite at a time
    m_space_object_ind = m_prop_satsArcs.at(0).sat; //will be analysed only the first satellite

    // Temporarily (12/05/2011)UPA2 works only with one arc
    m_traj_arc_ind= m_prop_satsArcs.at(0).arcs.at(0);

    m_factors=m_curr_widg->treat_num->text().toInt(&ok,10);

    if (m_input_type_ind!=3)
        m_sample_num=m_curr_widg->sample_num->text().toInt(&ok,10);
    else
        m_sample_num=1;
}

void AVTACTDialog::read_current_input()
{
    bool ok;

    // Search for current widget ----------------------
    m_curr_widg = dynamic_cast<ACT_input*>(this->ACT_inputTab->widget(0));

    // Initialise tmp matrices and lists -------------
    Vect_List_Member.setZero(6,3);
    Mat_List_Member.setZero(6,6);
    current_input.covariance.clear();
    current_input.distribution_vect1.clear();
    current_input.steps_date.clear();
    current_input.steps_num.clear();

    // Define FOR limits ------------------------------
    int n_widg=ACT_inputTab->count();

    // Read generic input----------------------------------------
    GUI_configuration();

    current_input.prop_type=m_propagation_type;
    current_input.an_type=m_an_type_ind;
    current_input.coord_type_ind=m_coord_type_ind;

    // In case of Non-Linear propagation: to propagate Covariance and RMS
    // the core propagates a Normal Distribution

    int configureRMS_for_MC=0;
    if (current_input.prop_type==1 && (m_input_type_ind==2 || m_input_type_ind==0))
    {
        if (m_input_type_ind==0)
        {
            configureRMS_for_MC=1;
        }
        m_input_type_ind=1;
        m_distribution=act::DIST_NORMAL;
    }
    //----------------------------------------------------------------

    current_input.distribution_ind=m_distribution;
    current_input.input_type_ind=m_input_type_ind;
    current_input.ref_type_ind=m_ref_type_ind;
    current_input.treat_num=n_widg;
    current_input.fact_num=m_factors;
    current_input.sample_num=m_sample_num;

    // UPA2 Analyses only the one satellite at a time
    current_input.space_object_ind= m_prop_satsArcs.at(0).sat; //will be analysed only the first satellite

    // Temporarily (12/05/2011)UPA2 works only with one arc
    current_input.traj_arc_ind= m_prop_satsArcs.at(0).arcs.at(0);
    current_input.steps_date.append(m_prop_satsArcs.at(0).startProp.at(0));
    current_input.steps_date.append(m_prop_satsArcs.at(0).stopProp.at(0));

    //  Read input TabWidgets  -------------------------------------
    if (this->input_combo->currentIndex()!=3)
    {
        for(int i=0;i<n_widg;i++)
        {
            read_input_widget(i);

            // For Monte Carlo propagation RMS ==> Normal distribution with Diagonal matrix

            if(configureRMS_for_MC==1)
            {
                for(int j=0;j<Vect_List_Member.rows();j++)
                {
                    Mat_List_Member(j,j)=Vect_List_Member(j,0);
                    Vect_List_Member(j,0)=0;
                }

            }

            // Append treatment matrix and vector to the list --------------------
            current_input.covariance.append(Mat_List_Member);
            current_input.distribution_vect1.append(Vect_List_Member);
        }
    }
    else
    {
        Vect_List_Member.setZero(6,3);
        Mat_List_Member.setZero(6,6);
        current_input.covariance.append(Mat_List_Member);
        current_input.distribution_vect1.append(Vect_List_Member);
        current_input.sample_num=0;
    }
}


void AVTACTDialog::read_input_widget(int index)
{
    // Search for and define current widget and relatated limits--------------------
    m_curr_widg = dynamic_cast<ACT_input*>(this->ACT_inputTab->widget(index));
    QGridLayout* vett_grid=m_curr_widg->vett_grid;
    QGridLayout* mat_grid=m_curr_widg->mat_grid;

    int n_row=mat_grid->rowCount();
    int n_col=mat_grid->columnCount();
    int n_vect_col=vett_grid->columnCount();

    Mat_List_Member=read_input_wid_matrix(mat_grid,n_col,n_row);
    Vect_List_Member=read_input_wid_matrix(vett_grid,n_vect_col,n_row);
}

MatrixXd AVTACTDialog::read_input_wid_matrix(QGridLayout* curr_mat,int n_col, int n_row)
{
    bool go;
    double var;
// Cycle on matrices and vectors: read GUI and store in temporary e matrix-------------------------
    MatrixXd read_mat;
    read_mat.setZero(n_row-1,n_col-1);
    for(int j=1;j<n_row;j++)
    {
        for(int k=1;k<n_col;k++)
        {
            // Pick up grid element
            QWidget* grid_item = curr_mat->itemAtPosition(j,k)->widget();
            if (dynamic_cast<QLineEdit*>(grid_item)) // ------ > if LineEdit
            {
                var=(dynamic_cast<QLineEdit*>(curr_mat->itemAtPosition(j,k)->widget()))->text().toDouble(&go);
                // if conversion is not ok, keep the valeu from previous tab. If yes replace it.
                if (go) read_mat(j-1,k-1)=var;
            }
            if (dynamic_cast<QCheckBox*>(grid_item)) // ------ > if CheckBox
            {
                var=(dynamic_cast<QCheckBox*>(curr_mat->itemAtPosition(j,k)->widget()))->isChecked();
                read_mat(j-1,k-1)=1.0*var;
            }
        }
    }
    return (read_mat);
}


void AVTACTDialog::on_run_pushb_clicked(bool)
{
    read_current_input();

//  check the input consistency

    if (current_input.coord_type_ind==act::ELEM_SPHERICAL && current_input.ref_type_ind!=sta::COORDSYS_BODYFIXED)
    {
        QMessageBox msgBox;
        msgBox.setText("Spherical elements can be selected only for body fixed coordinate frames.");
        msgBox.setWindowTitle("Uncertainty Propagation Analysis");
        msgBox.setIcon(QMessageBox::Warning);
        msgBox.setStandardButtons(QMessageBox::Ok);
        msgBox.setDefaultButton(QMessageBox::Ok);
        msgBox.exec();
        return;
    }
    if (current_input.coord_type_ind==act::ELEM_KEPLERIAN && (current_input.ref_type_ind!=sta::COORDSYS_ECLIPTIC_J2000 && current_input.ref_type_ind!=sta::COORDSYS_EME_J2000 && current_input.ref_type_ind!=sta::COORDSYS_EME_B1950))
    {
        QMessageBox msgBox;
        msgBox.setText("Keplerian elements can be selected only for inertial reference frames.");
        msgBox.setWindowTitle("Uncertainty Propagation Analysis");
        msgBox.setIcon(QMessageBox::Warning);
        msgBox.setStandardButtons(QMessageBox::Ok);
        msgBox.setDefaultButton(QMessageBox::Ok);
        msgBox.exec();
        return;
    }

    // Initialize m_act_analysis--------------------------------------------
    m_act_analysis=new ActAnalysis(m_scenario, m_prop_scenario);

    set_AVTACT_general_input();

    if (ACT_rbut->isChecked())
    {
        int is_out=read_output_tree();
        if (is_out==0)
            return;

        run_ACT();
    }
    else
    {
        run_AVT();
    }
}

void AVTACTDialog::set_AVTACT_general_input()
{
    m_act_analysis->setCurrentSpaceObject(current_input.space_object_ind);
    m_act_analysis->setCurrentArc(current_input.traj_arc_ind);
    m_act_analysis->setTimeSteps(current_input.steps_date);
    m_coordType = current_input.ref_type_ind;

    //Define the propagation type: Linear or Monte Carlo
    if (current_input.prop_type==0)
    {
        m_act_analysis->setLinear();
    }
    else
    {
        m_act_analysis->setNonLinear();
    }

    m_elemType = current_input.coord_type_ind;
    m_distribution = current_input.distribution_ind;
    m_sample_num=current_input.sample_num;
}

void AVTACTDialog::run_ACT()
{
    int success;
    int index=0;
    bool act=true;
    MatrixXd Samples;
    if (current_input.input_type_ind==3)
    {
        int n_col=6;
        int n_row=m_smpl_input->samples.count()/n_col;
        Samples=vector2matrix(m_smpl_input->samples, n_row, n_col);
    }

    success=ACT_initialization(current_input.input_type_ind, current_input.distribution_vect1.at(index),current_input.covariance.at(index), index , act,Samples);

    int go_on = m_act_analysis->isInitialized();
    if (go_on == 0)
    {
        QMessageBox msgBox;
        msgBox.setWindowTitle("ERROR");
        msgBox.setIcon(QMessageBox::Warning);
        msgBox.setText("Input values are not congruent with the selected input type. Please verify.");
        msgBox.setStandardButtons(QMessageBox::Ok);
        msgBox.setDefaultButton(QMessageBox::Ok);
        msgBox.exec();
        return;
    }

    m_act_analysis->propagate();

    QString filenameout=save_output_file();
    int long_name=filenameout.count();
    QString filename=filenameout;
    filename.truncate(long_name-4);

    QString filenamesamples;
    QString filenamesamplesJ2K;
    QString stepss;
    QList<act::ActOutputType> list;

    list.append(act::OUT_COV);
    list.append(act::OUT_COVJ2K);
    list.append(act::OUT_MEAN);
    list.append(act::OUT_MEANJ2K);
    list.append(act::OUT_EIG);
    list.append(act::OUT_EIGV);
    list.append(act::OUT_ELLIPSOID);
    list.append(act::OUT_RMS);
    list.append(act::OUT_WEAK);
    list.append(act::OUT_MEDIAN);
    list.append(act::OUT_NORMAL);
    list.append(act::OUT_MAX);
    list.append(act::OUT_MIN);
    list.append(act::OUT_RANGE);

    // create output file
    m_act_analysis->saveOutputFile(list,filenameout);
}

void AVTACTDialog::run_AVT()
{

    if (this->ACT_inputTab->count()<2 && current_input.input_type_ind!=3)
    {
        QMessageBox msgBox;
        msgBox.setWindowTitle("AVT");
        msgBox.setIcon(QMessageBox::Warning);
        msgBox.setText("Select at least 1 factor and press the Confirm button.  ");
        msgBox.setStandardButtons(QMessageBox::Ok);
        msgBox.setDefaultButton(QMessageBox::Ok);
        msgBox.exec();
        return;
    }
    m_avt_analysis=new AvtAnalysis(current_input);
    MatrixXd Samples;
    int success_mat=1;
    int success_sample=1;
    int success=1;

    if (current_input.input_type_ind==3) //sample
    {
        // from a double vector to a matrix n_sample x 6+n_fact
        int n_col=6+current_input.fact_num;
        int n_row=m_smpl_input->samples.count()/n_col;

        Samples=vector2matrix(m_smpl_input->samples, n_row, n_col);

        success_sample = m_avt_analysis->group_samples(Samples,current_input.fact_num);
    }
    else
    {
        success_mat = m_avt_analysis->combine_matrices();
    }

    if (success_mat==0)
    {
        QMessageBox msgBox;
        msgBox.setWindowTitle("AVT");
        msgBox.setIcon(QMessageBox::Warning);
        msgBox.setText("Select at least 1 factor and press the Confirm button.  ");
        msgBox.setStandardButtons(QMessageBox::Ok);
        msgBox.setDefaultButton(QMessageBox::Ok);
        msgBox.exec();
        return;
    }
    if (success_sample==0)
    {
        QMessageBox msgBox;
        msgBox.setWindowTitle("AVT");
        msgBox.setIcon(QMessageBox::Warning);
        msgBox.setText("The samples do not cover all the treatment-factor combination.  ");
        msgBox.setStandardButtons(QMessageBox::Ok);
        msgBox.setDefaultButton(QMessageBox::Ok);
        msgBox.exec();
        return;
    }
    if (success_sample==2)
    {
        QMessageBox msgBox;
        msgBox.setWindowTitle("AVT");
        msgBox.setIcon(QMessageBox::Warning);
        msgBox.setText("The factors have different number of treatments.  ");
        msgBox.setStandardButtons(QMessageBox::Ok);
        msgBox.setDefaultButton(QMessageBox::Ok);
        msgBox.exec();
        return;
    }

    MatrixXd vect,covariance,sample_ordered;

    vect.setZero(6,3);
    covariance.setZero(6,6);

    avt_input_class combined_matrices=m_avt_analysis->get_combined_matrices();

    int n_combination=combined_matrices.fct_cmbntn.count();

    m_act_analysis->setInitialCovarianceNumber(n_combination);   // Needed to fix an act bug. To verify.

    for (int i=0; i<n_combination; i++)
    {
        if(current_input.input_type_ind==3) //sample
        {
            sample_ordered=combined_matrices.samples.at(i);
        }
        else
        {
            int n_col=combined_matrices.treatment_mat.at(i).cols();
            int n_row=combined_matrices.treatment_mat.at(i).rows();
            for (int k=0; k<n_col; k++)
            {
                 for(int j=0; j<n_row; j++)
                 {
                    if (k<3)
                    {
                        vect(j,k)=combined_matrices.treatment_mat.at(i)(j,k);
                    }
                    else
                    {
                        covariance(j,k-3)=combined_matrices.treatment_mat.at(i)(j,k);
                    }
                }
             }
        }
        bool act=false;
        success=ACT_initialization(current_input.input_type_ind, vect,covariance,i,act,sample_ordered);

        if (success == 0)
        {
            return;
        }
    }

    int go_on =m_act_analysis->isInitialized();

    if (go_on == 0)
    {
        QMessageBox msgBox;
        msgBox.setWindowTitle("ERROR");
        msgBox.setIcon(QMessageBox::Warning);
        msgBox.setText("Input values are not congruent with the selected input type. Please verify.");
        msgBox.setStandardButtons(QMessageBox::Ok);
        msgBox.setDefaultButton(QMessageBox::Ok);
        msgBox.exec();
        return;
    }

    m_act_analysis->propagate();

    QString name_file_output= save_output_file();
    int long_name=name_file_output.count();
    QString file_4_avt=name_file_output;
    file_4_avt.truncate(long_name-4);
    m_avt_analysis->set_output_directory(file_4_avt);

    const char* name_file_char_out=(const char*) name_file_output.toLatin1();
    FILE *woutFile = fopen (name_file_char_out,"w");
    for(int j=0; j<current_input.steps_date.count(); j++)
    {
        avt_input_class single_class;
        QList<MatrixXd> all_cov_4_var;
        QList<MatrixXd> all_sample_4_var;
        for (int i=0; i<n_combination; i++)
        {
            all_cov_4_var.append(m_act_analysis->covariance(j,i));
            all_sample_4_var.append(m_act_analysis->samples(j,i));
        }
        single_class.treatment_mat.append(all_cov_4_var);
        single_class.samples.append(all_sample_4_var);
        this->m_all_anova_input.append(single_class);

        m_avt_analysis->save_propagation(all_cov_4_var,all_sample_4_var);
        int n_factor;
        if (j>0)
        {
            m_avt_analysis->anova(j);
            n_factor=m_avt_analysis->get_n_factor();
            // Get ANOVA output
            if (j<2)
            {
               fprintf(woutFile, "Time_step \t Interaction \t Dof \t");
               for (int z=1;z<=n_factor;z++)
               {
                   fprintf(woutFile, "Factor_%d \t",z);
               }
               for (int z=1;z<=6;z++)
               {
                   fprintf(woutFile, "SSwith_%d \t",z);
               }
               for (int z=1;z<=6;z++)
               {
                   fprintf(woutFile, "SSbetw_%d \t",z);
               }
               for (int z=1;z<=6;z++)
               {
                   fprintf(woutFile, "MS_%d \t",z);
               }
               for (int z=1;z<=6;z++)
               {
                   fprintf(woutFile, "F_%d \t",z);
               }
               fprintf(woutFile, "\n");
            }

            VectorXd SS_tot=m_avt_analysis->get_SStot();
            QList<SS_struct> SS_k=m_avt_analysis->get_SS();
            VectorXd err=m_avt_analysis->get_error();
            VectorXd err_MS = m_avt_analysis->get_errorMS();
            int err_dof=m_avt_analysis->get_error_dof();
            double curr_step=current_input.steps_date.at(j);

            woutFile=this->write_output(woutFile,curr_step,SS_tot,SS_k,err,err_MS,err_dof,n_factor);
            woutFile=this->write_error(woutFile,curr_step,err,err_MS,err_dof,n_factor);
        }
    }
    fclose (woutFile);

}

int AVTACTDialog::ACT_initialization(int input_type_ind, MatrixXd distribution_vect, MatrixXd covariance, int index,bool act,MatrixXd Samples)
{
    int success=1;
    switch (input_type_ind)
    {
        case 0: //RMS
        {
            MatrixXd RMSValues;
            RMSValues.setZero(6,1);
            RMSValues=distribution_vect.col(0);
            if (act)
            {
                success=m_act_analysis->InitializeRMS(RMSValues.transpose(), m_coordType, m_elemType);
            }
            else
            {
                success=m_act_analysis->InitializeRMS(RMSValues.transpose(), m_coordType, m_elemType,index);
            }

        }
        break;
        case 1: //Statistical parameters
        {
            MatrixXd Param1d;
            MatrixXi Param2i,Param3i,Param4i;
            switch (m_distribution)
            {
                case DIST_NORMAL:
                {
                    Param1d.setZero(6,6);
                    Param1d=covariance;
                }
                break;
                case DIST_POISSON:
                {
                    Param1d.setZero(6,1);
                    Param1d =distribution_vect.col(0);
                }
                break;
                case DIST_BINOMIAL:
                {
                    Param1d.setZero(6,1);
                    Param2i.setZero(6,1);
                    Param1d = distribution_vect.col(0);
                    for (int i=0; i<distribution_vect.rows();i++)
                        Param2i(i,0) = (int)distribution_vect(i,1);
                }
                break;
                case DIST_NEG_BINOMIAL:
                {
                    Param1d.setZero(6,1);
                    Param2i.setZero(6,1);
                    Param1d = distribution_vect.col(0);
                    for (int i=0; i<distribution_vect.rows();i++)
                        Param2i(i,0) = (int)distribution_vect(i,1);
                }
                break;
                case DIST_CHI_SQUARE:
                {
                    Param1d.setZero(6,1);
                    Param2i.setZero(6,1);
                    Param1d = distribution_vect.col(0);
                    for (int i=0; i<distribution_vect.rows();i++)
                        Param2i(i,0) = (int)distribution_vect(i,1);
                }
                break;
                case DIST_DISCRETE_UNIFORM:
                {
                    Param2i.setZero(6,1);
                    Param3i.setZero(6,1);
                    for (int i=0; i<distribution_vect.rows();i++)
                    {
                        Param2i(i,0) = (int)distribution_vect(i,0);
                        Param3i(i,0) = (int)distribution_vect(i,1);
                    }
                }
                break;
                case DIST_GEOMETRIC:
                {
                    Param1d.setZero(6,1);
                    Param1d =distribution_vect.col(0);
                }
                break;
                case DIST_HYPER_GEOMETRIC:
                {
                    Param2i.setZero(6,1);
                    Param3i.setZero(6,1);
                    Param4i.setZero(6,1);
                    for (int i=0; i<distribution_vect.rows();i++)
                    {
                        Param2i(i,0) = (int)distribution_vect(i,0);
                        Param3i(i,0) = (int)distribution_vect(i,1);
                        Param4i(i,0) = (int)distribution_vect(i,2);
                    }
                }
                break;
                default:
                {

                }
            }

            if (act)
                success=m_act_analysis->InitializeDist(m_distribution, Param1d.transpose(), Param2i.transpose(),Param3i.transpose(),Param4i.transpose(), m_sample_num,m_coordType, m_elemType);
            else
                success=m_act_analysis->InitializeDist(m_distribution, Param1d.transpose(), Param2i.transpose(),Param3i.transpose(),Param4i.transpose(), m_sample_num,m_coordType, m_elemType,index);
        }
        break;
        case 2: //Covariance Matrix
        {
            MatrixXd Cov;
            Cov.setZero(6,6);
            Cov = covariance;
            if (act)
                success=m_act_analysis->InitializeCov(Cov, m_coordType, m_elemType);
            else
                success=m_act_analysis->InitializeCov(Cov, m_coordType, m_elemType,index);
        }
        break;
        case 3: //Samples
        {
            if (act)
                success=m_act_analysis->InitializeSamples(Samples, m_coordType, m_elemType);
            else
                success=m_act_analysis->InitializeSamples(Samples, m_coordType, m_elemType,index);
        }
        break;
        default:
        {

        }
    }
    return(success);
}

MatrixXd AVTACTDialog::vector2matrix(QList<double> vect, int n_row, int n_col)
{
    MatrixXd mat;
    mat.setZero(n_row,n_col);
    for (int i=0; i<n_row;i++)
    {
        for (int j=0; j<n_col;j++)
        {
               mat(i,j)=vect.at((n_col)*i+j);
        }
    }
    return(mat);
}

FILE* AVTACTDialog::write_output(FILE *woutFile,double curr_step,VectorXd SS_tot,QList<SS_struct> SS_k,VectorXd err,VectorXd err_MS,int err_dof, int n_fact)
{
    VectorXi fact_inter;
    int max_pos=SS_k.count();
    for (int g_num=0;g_num<max_pos;g_num++)
    {
        int n_comb_sempl= SS_k.at(g_num).SSwithinn.count();
        for (int s=0;s<n_comb_sempl;s++)
        {
            fact_inter.setZero(n_fact);
            VectorXi comb=SS_k.at(g_num).simple_combinations.at(s);
            for (int j=0; j<comb.size();j++)
            {
                fact_inter(comb(j))=1;
            }

            fprintf(woutFile,"%f \t",curr_step); //time step
            fprintf(woutFile,"%d \t",g_num+1);  //interaction order
            fprintf(woutFile,"%d \t",SS_k.at(g_num).dof);
            for (int i=0;i<n_fact;i++)
            {
                fprintf(woutFile,"%d \t",fact_inter(i)); //interactive factor
            }
            for (int i=0;i<6;i++)
            {
                fprintf(woutFile,"%f \t",SS_k.at(g_num).SSwithinn.at(s)(i));
            }
            for (int i=0;i<6;i++)
            {
                fprintf(woutFile,"%f \t",SS_k.at(g_num).SSbetween.at(s)(i));
            }
            for (int i=0;i<6;i++)
            {
                fprintf(woutFile,"%f \t",SS_k.at(g_num).MS.at(s)(i));
            }
            for (int i=0;i<6;i++)
            {
                fprintf(woutFile,"%f \t",SS_k.at(g_num).F_factor.at(s)(i));
            }
            fprintf(woutFile,"\n");
        }
    }
    return(woutFile);
}


FILE* AVTACTDialog::write_error(FILE *woutFile,double curr_step,VectorXd err,VectorXd err_MS,int err_dof,int n_factor)
{
    fprintf(woutFile,"%f \t",curr_step);
    fprintf(woutFile,"%d \t",0);
    fprintf(woutFile,"%d \t",err_dof);
    for (int z=0;z<n_factor;z++)
    {
       fprintf(woutFile, "%d \t",0);
    }
    for (int z=0;z<6;z++)
    {
        fprintf(woutFile,"%d \t",0);
    }
    for (int z=0;z<6;z++)
    {
        fprintf(woutFile,"%f \t",err(z));
    }
    for (int z=0;z<6;z++)
    {
        fprintf(woutFile,"%f \t",err_MS(z));
    }
    for (int z=0;z<6;z++)
    {
        fprintf(woutFile,"%f \t",0);
    }
    fprintf(woutFile, "\n");
    return(woutFile);
}

QString AVTACTDialog::save_output_file()
{
    QString string_avtact;

    if (this->ACT_rbut->isChecked())
        string_avtact="ACT results(*.act)";
    else
        string_avtact="AVT results(*.avt)";

    m_fileName = QFileDialog::getSaveFileName(this,
                                                    tr("Save AVT - ACT results"),
                                                    "",
                                                    string_avtact);

    return(m_fileName);
}

//UPA2____________________________________________________________________________

void AVTACTDialog::on_propagation_currentIndexChanged(int ind)
{
    if (m_check2==true)
    {
        QMessageBox msgBox;
        msgBox.setWindowTitle("Uncertainty Propagation Analysis");
        msgBox.setText("If you change the type of propagation all the input inserted will be lost.");
        msgBox.setInformativeText("Do you want to continue?");
        msgBox.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
        msgBox.setDefaultButton(QMessageBox::Cancel);
        int ret = msgBox.exec();

        switch (ret)
        {
            case QMessageBox::Ok:
            {
                //run_pushb->setDisabled(true);
                this->reload_input->setDisabled(true);
                this->store_input->setDisabled(true);
                ACT_inputTab->removeTab(0);
                m_propagation_type=propagation->currentIndex();
                this->on_input_combo_currentIndexChanged(input_combo->currentIndex());
                m_check2=true;
                break;
            }
            case QMessageBox::Cancel:
            {
                m_check2=false;
                this->reload_input->setDisabled(false);
                this->store_input->setDisabled(false);
                propagation->setCurrentIndex((ind-1)*(-1));
                m_propagation_type=propagation->currentIndex();
                m_check2=true;
                break;
            }
            default:
            break;
        }
    }
}

int AVTACTDialog::organize_sat_arcs(QList<QTreeWidgetItem *> selectedItem)
{
    m_selectedItem=selectedItem;

    QList<int> arc_index;
    QList<int> sat_index;

    QString ts;

    for (int i=0; i<m_selectedItem.count(); i++)
    {
        int childnum=m_selectedItem.at(i)->childCount();
        if (m_selectedItem.at(i)->child(childnum-1)->text(0)=="MArc")
        {
            arc_index.append(m_selectedItem.at(i)->child(childnum-3)->text(0).toInt());
            sat_index.append(m_selectedItem.at(i)->child(1)->text(0).toInt());
        }
     }

    //Group the arcs for each satellite---------------------------------

    int check;
    this->m_satsArcs.clear();
    sat_arcs one_sat;
    one_sat.sat=sat_index.at(0);
    one_sat.arcs.append(arc_index.at(0));
    one_sat.beginning.append(m_prop_scenario->spaceObjects().at(sat_index.at(0))->mission().at(arc_index.at(0))->beginning());
    one_sat.ending.append(m_prop_scenario->spaceObjects().at(sat_index.at(0))->mission().at(arc_index.at(0))->ending());
    m_satsArcs.append(one_sat);

    if (sat_index.count()>1)
    {

    for (int i=1; i<sat_index.count(); i++)
        {
        check=0;
            for (int j=0; j<m_satsArcs.count(); j++)
            {
                one_sat.arcs.clear();
                if(m_satsArcs.at(j).sat==sat_index.at(i))
                {
                    one_sat=m_satsArcs.at(j);
                    one_sat.arcs.append(arc_index.at(i));
                    one_sat.beginning.append(m_prop_scenario->spaceObjects().at(sat_index.at(i))->mission().at(arc_index.at(i))->beginning());
                    one_sat.ending.append(m_prop_scenario->spaceObjects().at(sat_index.at(i))->mission().at(arc_index.at(i))->ending());
                    m_satsArcs.replace(j,one_sat);
                    check=1;
                }
            }
            if(check==0)
            {
                one_sat.sat=sat_index.at(i);
                one_sat.beginning.append(m_prop_scenario->spaceObjects().at(sat_index.at(i))->mission().at(arc_index.at(i))->beginning());
                one_sat.ending.append(m_prop_scenario->spaceObjects().at(sat_index.at(i))->mission().at(arc_index.at(i))->ending());
                one_sat.arcs.append(arc_index.at(i));
                m_satsArcs.append(one_sat);
            }
        }
    }

    //Order the arcs of the same satellite-------------------------------------

    QList<int> ref_list,final_list,ref_index;
    sat_arcs ref_sat;
    int position;

    for (int i=0; i<m_satsArcs.count(); i++)
    {
        if (m_satsArcs.at(i).arcs.count()>1)
        {
            ref_sat=m_satsArcs.at(i);
            ref_list=m_satsArcs.at(i).arcs;
            final_list=ref_list;
            for (int j=0; j<ref_list.count(); j++)
            {
                position=0;
                for (int k=0; k<m_satsArcs.at(i).arcs.count(); k++)
                {
                    if (ref_list.at(j)>m_satsArcs.at(i).arcs.at(k))
                    {
                        position++;
                    }
                }
                ref_index.append(position);
            }
            for (int k=0; k<m_satsArcs.at(i).arcs.count(); k++)
            {
                final_list.replace(ref_index.at(k),ref_list.at(k));
            }
            ref_sat.arcs.clear();
            ref_sat.arcs=final_list;
            m_satsArcs.replace(i,ref_sat);
        }
    }

    //Verify the arcs are consecutive-----------------------------------------
    int ok=1;
    for (int i=0; i<m_satsArcs.count(); i++)
    {
        if (m_satsArcs.at(i).arcs.count()>1)
        {
            for (int j=1; j<m_satsArcs.at(i).arcs.count(); j++)
            {
                if (m_satsArcs.at(i).arcs.at(j)>(m_satsArcs.at(i).arcs.at(j-1)+1))
                    ok=0;
            }
        }
    }
    return(ok);
}


QList<int> AVTACTDialog::organize_time_intervals(QList<double> startTime, QList<double> stopTime)
{
    m_startTime=startTime;
    m_stopTime=stopTime;
    QList<int> output;
    output.append(0);
    output.append(0);
    QList<double> ref_startTime,ref_stopTime;
    QList<int> ref_index;
    ref_startTime=m_startTime;
    ref_stopTime=stopTime;
    int position;

    for (int i=0; i<m_startTime.count(); i++)
    {
        if (m_startTime.at(i)>m_stopTime.at(i))
        {
            output.replace(0,2);
            return(output); //Invalid interval definition
        }

        position=0;
        for (int j=0; j<m_startTime.count(); j++)
        {
            if (ref_startTime.at(i)>m_startTime.at(j))
            {
                position++;
            }
        }
        ref_index.append(position);
    }

    //Order the intervals in function of the start time
    for (int i=0; i<m_startTime.count(); i++)
    {
        m_startTime.replace(ref_index.at(i),ref_startTime.at(i));
        m_stopTime.replace(ref_index.at(i),ref_stopTime.at(i));
    }

    //Verify the intervals are consecutive -----------------------------------------
    for (int i=0; i<m_startTime.count()-1; i++)
    {
        if(m_stopTime.at(i)<m_startTime.at(i+1))
        {
            output.replace(0,0);
            return(output); //Not consecutive arcs
        }
    }

    int input_congruent=verify_arcsVStime();
    output.replace(0,1);
    output.replace(1,input_congruent);
    return(output);
 }


int AVTACTDialog::verify_arcsVStime()
{
    int ok=0;
    sat_arcs temporary_sat;
    int sat,arc,arcSteps;
    int check_mission;
    int find_start,find_stop,k,y;
    double curr_step;

    //verify the arc exists in the selected intervals
    for (int i=0; i<m_satsArcs.count();i++)
    {
        check_mission=0;
        temporary_sat=m_satsArcs.at(i);
        for (int j=0; j<m_satsArcs.at(i).arcs.count();j++)
        {
            if (m_startTime.at(0)>= m_satsArcs.at(i).ending.at(j) || m_stopTime.last()<=m_satsArcs.at(i).beginning.at(j))
            {
                temporary_sat.availability.append(0);
            }
            else
            {
                temporary_sat.availability.append(1);
            }
        }

        for (int j=0; j<m_satsArcs.at(i).arcs.count();j++)
        {
            check_mission=check_mission+temporary_sat.availability.at(j);
        }

        if(check_mission>0)
        {
            temporary_sat.missionAvailable=1;
            ok=ok+1;
        }
        else
        {
            temporary_sat.missionAvailable=0;
        }
        m_satsArcs.replace(i,temporary_sat);
    }



    //find the propagation interval-----------------------------------------------------

    for (int i=0; i<m_satsArcs.count();i++)
    {
        if(m_satsArcs.at(i).missionAvailable==1)
        {
            temporary_sat=m_satsArcs.at(i);
            sat=m_satsArcs.at(i).sat;
            for (int j=0; j<m_satsArcs.at(i).arcs.count();j++)
            {
                if(m_satsArcs.at(i).availability.at(j)==1)
                {
                    arc=m_satsArcs.at(i).arcs.at(j);
                    arcSteps=m_prop_scenario->spaceObjects().at(sat)->mission().at(arc)->trajectorySampleCount();
                    k=0;
                    find_start=0;
                    while (k<arcSteps && find_start==0)
                    {
                        curr_step=m_prop_scenario->spaceObjects().at(sat)->mission().at(arc)->trajectorySampleTime(k);
                        if(m_startTime.at(0)<=curr_step)
                        {
                            find_start=1;
                            temporary_sat.startProp.append(curr_step);
                        }
                        k++;
                    }

                    find_stop=0;
                    y=arcSteps-1;
                    while (y>=0 && find_stop==0)
                    {
                        curr_step=m_prop_scenario->spaceObjects().at(sat)->mission().at(arc)->trajectorySampleTime(y);
                        if(m_stopTime.last()>=curr_step)
                        {
                            find_stop=1;
                            temporary_sat.stopProp.append(curr_step);
                        }
                        y=y-1;
                    }
                }
            }
            m_satsArcs.replace(i,temporary_sat);
        }
    }
    return (ok);
}

int AVTACTDialog::read_output_tree()
{
    bool ok;
    m_outputList.clear();
    int n_output=this->treeWidgetOuputList->selectedItems().count();

    for (int i=0; i<n_output; i++)
    {
        if(this->treeWidgetOuputList->selectedItems().at(i)->childCount()==0)
        {
            m_outputList.append(treeWidgetOuputList->selectedItems().at(i)->text(1).toInt(&ok,10));
        }
    }

    if (m_outputList.count()==0)
    {
        QMessageBox msgBox;
        msgBox.setText("Please select at least one output.");
        msgBox.setWindowTitle("Uncertainty Propagation Analysis");
        msgBox.setIcon(QMessageBox::Warning);
        msgBox.setStandardButtons(QMessageBox::Ok);
        msgBox.setDefaultButton(QMessageBox::Ok);
        msgBox.exec();
        return (0);
    }
    return(1);
}

//_________________________________________________________________________________________________________

AVTACTDialog::~AVTACTDialog()
{

}
