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
 ------------------ Author: Chris Laurel  -------------------------------------------------
 ------------------ E-mail: (claurel@gmail.com) ----------------------------
  Modified by Tiziana Sabatini May 2009
  Modified by Guillermo June 3rd 2009
  Modified by Valentino Zuccarelli June 14th 2009
  Extensvely modified by Guillermo on October 2009 to include TLEs
 */

#include <QSettings>
#include <QFile>
#include <QFileDialog>
#include <QtDebug>
#include <QtGui>

#include <QMessageBox>

#include "ACT_input.h"


//Costructor-----------------------------------------------------
ACT_input::ACT_input(QWidget* parent) :    QWidget(parent)
{
    setupUi(this);
    // Set up distribution combo----------------------------------
    distribution_combo->addItem(tr("Normal"), (int) act::DIST_NORMAL);
    distribution_combo->addItem(tr("Poisson"), (int) act::DIST_POISSON);
    distribution_combo->addItem(tr("Binomial"), (int) act::DIST_BINOMIAL);
    distribution_combo->addItem(tr("Negative Binomial"), (int) act::DIST_NEG_BINOMIAL);
    distribution_combo->addItem(tr("Chi Square"), (int) act::DIST_CHI_SQUARE);
    distribution_combo->addItem(tr("Discrete Uniform"), (int) act::DIST_DISCRETE_UNIFORM);
    distribution_combo->addItem(tr("Geometric"), (int) act::DIST_GEOMETRIC);
    distribution_combo->addItem(tr("Hyper Geometric"), (int) act::DIST_HYPER_GEOMETRIC);
    distribution_combo->setCurrentIndex(0);

    // Set up the input validators----------------------------------
    QDoubleValidator* doubleValidator = new QDoubleValidator(this);
//    m_doubleValidator1 = new QDoubleValidator(this);
    QIntValidator* intValidator = new QIntValidator(this);
//    m_intValidator1 = new QIntValidator(this);

    int n_row=mat_grid->rowCount();
    int n_col=mat_grid->columnCount();
    int n_vect_col;

    n_vect_col=vett_grid->columnCount();

    for(int j=1;j<n_row;j++)
    {
        for(int k=1;k<n_col;k++)
        {
            QWidget* grid_item = mat_grid->itemAtPosition(j,k)->widget();
            if (dynamic_cast<QLineEdit*>(grid_item))
            {
                doubleValidator = new QDoubleValidator(this);
                if(j==k)
                    doubleValidator->setBottom(0);
                (dynamic_cast<QLineEdit*>(mat_grid->itemAtPosition(j,k)->widget()))->setValidator(doubleValidator);
                QLineEdit* this_line_edit=dynamic_cast<QLineEdit*>(mat_grid->itemAtPosition(j,k)->widget());
                connect(this_line_edit,SIGNAL(textChanged(const QString)),this, SLOT(copy_value(const QString)));
            }

        }
    }
}

//Methods---------------------------------------------------------
void ACT_input::on_distribution_combo_currentIndexChanged(int ind)
{
    m_curr_distrib=(act::ActDistributionType) distribution_combo->itemData(ind).toInt();
    if (m_curr_distrib==3) m_curr_distrib=2;
    distribution_set_up(this, m_curr_distrib);
}


void ACT_input::distribution_set_up(QWidget* curr_widg,int ind)
{
    QLabel *lcol1=curr_widg->findChild<QLabel *>("lcol1");
    QLabel *lcol2=curr_widg->findChild<QLabel *>("lcol2");
    QLabel *lcol3=curr_widg->findChild<QLabel *>("lcol3");
    QGridLayout *vett_grid=curr_widg->findChild<QGridLayout *>("vett_grid");
    QGridLayout *mat_grid=curr_widg->findChild<QGridLayout *>("mat_grid");

    int value=0;
    QString val;
    int ncol;
    bool matrix;
    bool isdoublevalidator;
    QDoubleValidator* doubleValidator;
    QIntValidator* intValidator;
    switch ( ind )
    {
        case 0:
        {
            lcol1->setText("lambda");
            ncol=0;
            matrix=true;
            isdoublevalidator=true;
            intValidator=new QIntValidator(this);
            doubleValidator=new QDoubleValidator(this);
            doubleValidator->setBottom(0);
        }
        break;
        case 1:
        {
            // Vettors set-up
            lcol1->setText("lambda");
            ncol=2;
            matrix=false;
            isdoublevalidator=true;
            intValidator=new QIntValidator(this);
            doubleValidator=new QDoubleValidator(this);
            doubleValidator->setBottom(0);
        }
        break;
        case 2:
        {
            // Vettors set-up
            lcol1->setText("p");
            lcol2->setText("n");
            ncol=3;
            matrix=false;
            isdoublevalidator=true;
            doubleValidator=new QDoubleValidator(this);
            intValidator=new QIntValidator(this);
            doubleValidator->setRange(0.000000,1.000000,6);
            intValidator->setBottom(0);
        }
        break;
        case 4:
        {
            // Vettors set-up
            lcol1->setText("lambda");
            lcol2->setText("k");
            ncol=3;
            matrix=false;
            isdoublevalidator=true;
            doubleValidator=new QDoubleValidator(this);
            intValidator=new QIntValidator(this);
            intValidator->setBottom(1);
            value=1;
        }
        break;
        case 5:
        {
            // Vettors set-up
            lcol1->setText("min");
            lcol2->setText("Max");
            ncol=3;
            matrix=false;
            isdoublevalidator=false;
            doubleValidator=new QDoubleValidator(this);
            intValidator=new QIntValidator(this);
            intValidator->setBottom(0);
        }
        break;
        case 6:
        {
            // Vettors set-up
            lcol1->setText("p");
            ncol=2;
            matrix=false;
            isdoublevalidator=true;
            intValidator=new QIntValidator(this);
            doubleValidator=new QDoubleValidator(this);
            doubleValidator->setRange(0.000000,1.000000,6);
        }
        break;
        case 7:
        {
            // Vettors set-up
            lcol1->setText("n");
            lcol2->setText("m");
            lcol3->setText("N");
            ncol=4;
            matrix=false;
            isdoublevalidator=false;
            intValidator=new QIntValidator(this);
            doubleValidator=new QDoubleValidator(this);
            intValidator->setBottom(0);
        }
        break;
        case 8:
        {
            // Covariance matrix
            ncol=0;
            matrix=true;
            isdoublevalidator=true;
            intValidator=new QIntValidator(this);
            doubleValidator=new QDoubleValidator(this);
        }
        break;
        case 9:
        {
            // RMS
            lcol1->setText("RMS");
            ncol=2;
            matrix=false;
            isdoublevalidator=true;
            intValidator=new QIntValidator(this);
            doubleValidator=new QDoubleValidator(this);
            doubleValidator->setBottom(0);

        }
        break;
        default:
        {

        }
    }

    // Vettors set-up
    for(int j=0;j<vett_grid->columnCount();j++)
    {
        for(int i=0;i<vett_grid->rowCount();i++)
        {
            //checkbox initialization
            if (dynamic_cast<QCheckBox*>(vett_grid->itemAtPosition(i,j)->widget()) && j>0 && i>0)
            {
                dynamic_cast<QCheckBox*>(vett_grid->itemAtPosition(i,j)->widget())->setChecked(false);
            }
            if (j<ncol)
            {
                vett_grid->itemAtPosition(i,j)->widget()->setVisible(true);
                QWidget* vect_grid_item = vett_grid->itemAtPosition(i,j)->widget();
                if (j>0 && dynamic_cast<QLineEdit*>(vect_grid_item))
                {
                    if (isdoublevalidator && j==1)
                    {
                        (dynamic_cast<QLineEdit*>(vett_grid->itemAtPosition(i,j)->widget()))->setValidator(doubleValidator);
                        (dynamic_cast<QLineEdit*>(vett_grid->itemAtPosition(i,j)->widget()))->setText("0.00");
                    }
                    else
                    {
                        val.setNum(value);
                        (dynamic_cast<QLineEdit*>(vett_grid->itemAtPosition(i,j)->widget()))->setText(val);//"0");
                        (dynamic_cast<QLineEdit*>(vett_grid->itemAtPosition(i,j)->widget()))->setValidator(intValidator);
                    }
                }
            }
            else {vett_grid->itemAtPosition(i,j)->widget()->setVisible(false);}
        }
    }

    // Matrix set-up
    for(int j=0;j<mat_grid->columnCount();j++)
    {
        for(int i=0;i<mat_grid->rowCount();i++)
        {
            mat_grid->itemAtPosition(i,j)->widget()->setVisible(matrix);
            //checkbox initialization
            if (dynamic_cast<QCheckBox*>(mat_grid->itemAtPosition(i,j)->widget()))
            {
                dynamic_cast<QCheckBox*>(mat_grid->itemAtPosition(i,j)->widget())->setChecked(false);
            }
            if (dynamic_cast<QLineEdit*>(mat_grid->itemAtPosition(i,j)->widget()))
            {
                (dynamic_cast<QLineEdit*>(mat_grid->itemAtPosition(i,j)->widget()))->setText("0.00");
            }
            if (dynamic_cast<QCheckBox*>(mat_grid->itemAtPosition(i,j)->widget()) && i>j)
            {
                mat_grid->itemAtPosition(i,j)->widget()->setVisible(false);
            }
        }
    }
}


void ACT_input::on_confirm_AVT_clicked()
{
    bool ok;
    transfer->run_pushb->setDisabled(false);

    //Remove old tabs
    int old_tab_number=transfer->ACT_inputTab->count();
    for (int i=old_tab_number; i>1; i=i-1)
    {
        transfer->ACT_inputTab->removeTab(i-1);
    }
    //Read tab type
    int input_type=transfer->input_combo->currentIndex();
    int distribution_type=m_curr_distrib;
    
    
    //Generate new tabs
    int n_treat=treat_num->text().toInt(&ok,10);
    for(int k=1;k<=n_treat;k++)
    {
        QString curr_k;
        curr_k.setNum(k, 10);
        folder=new ACT_input;
        folder->treat_grid->itemAtPosition(0,0)->widget()->setVisible(false);
        folder->treat_grid->itemAtPosition(0,1)->widget()->setVisible(false);
        folder->treat_grid->itemAtPosition(0,2)->widget()->setVisible(false);
        folder->label_2->setVisible(false);
        folder->label->setVisible(false);
        folder->sample_num->setVisible(false);
        folder->distribution_combo->setVisible(false);
    
        if (input_type==0) this->distribution_set_up(folder,9);
        if (input_type==1) this->distribution_set_up(folder,distribution_type);
        if (input_type==2) this->distribution_set_up(folder,8);
        transfer->ACT_inputTab->addTab(folder,"Treat "+curr_k);
        if (k>1)
        {
            //Vectors element
            for(int j=1;j<vett_grid->columnCount();j++)
            {
                for(int i=1;i<vett_grid->rowCount();i++)
                {
                    QWidget* grid_item = vett_grid->itemAtPosition(i,j)->widget();
                    if (dynamic_cast<QCheckBox*>(grid_item) && grid_item->isVisible())
                    {
                        QCheckBox* temp_check=dynamic_cast<QCheckBox*>(grid_item);
                        if (temp_check->isChecked()==false)
                        {
                            dynamic_cast<QLineEdit*>(folder->vett_grid->itemAtPosition(i,j)->widget())->setEnabled(false);
                            dynamic_cast<QLineEdit*>(folder->vett_grid->itemAtPosition(i,j)->widget())->setStyleSheet("background-color: grey");
                        }
                    }
                }
            }
            //Matrix element
            for(int i=1;i<mat_grid->rowCount();i++)
            {
                for(int j=i;j<mat_grid->columnCount();j++)
                    {
                    QWidget* grid_item = mat_grid->itemAtPosition(i,j)->widget();
                    if (dynamic_cast<QCheckBox*>(grid_item) && grid_item->isVisible())
                    {
                        QCheckBox* temp_check=dynamic_cast<QCheckBox*>(grid_item);
                        if (temp_check->isChecked()==false)
                        {
                            dynamic_cast<QLineEdit*>(folder->mat_grid->itemAtPosition(i,j)->widget())->setEnabled(false);
                            dynamic_cast<QLineEdit*>(folder->mat_grid->itemAtPosition(i,j)->widget())->setStyleSheet("background-color: grey");
                            dynamic_cast<QLineEdit*>(folder->mat_grid->itemAtPosition(j,i)->widget())->setEnabled(false);
                            dynamic_cast<QLineEdit*>(folder->mat_grid->itemAtPosition(j,i)->widget())->setStyleSheet("background-color: grey");
                        }
                    }
                }
            }
        }
    }
}

void ACT_input::on_load_file_clicked()
{
    // Start the open file dialog in the same directory as last time
    QString dir,ts;
    QSettings settings;
    bool ok;
    settings.beginGroup("Preferences");
    if (settings.contains("OpenScenarioDir"))
    {
        dir = settings.value("OpenScenarioDir").toString();
    }

    fileName = QFileDialog::getOpenFileName(this,tr("Load Samples"),dir,"Sample File (*.txt)");

    const char* fname=fileName.toLatin1().data();
    samples.clear();
    samples=read_data_file(fname);

    float data_num=samples.count();
    float factor=this->treat_num->text().toFloat(&ok);
    float sample_num=data_num /(factor+6);
    float control_sample_num=floor(data_num/(factor+6));

    if (sample_num!=control_sample_num)
    {
        QMessageBox msgBox;
        msgBox.setText("The samples data are not complete. Check the loaded file.");
        msgBox.setStandardButtons(QMessageBox::Ok);
        msgBox.setDefaultButton(QMessageBox::Ok);
        int ret = msgBox.exec();
        return;
    }

    dynamic_cast<QLineEdit*>(this->file_loading_grid->itemAtPosition(0,1)->widget())->setText(fileName);
    transfer->run_pushb->setEnabled(true);
}

QList<double> ACT_input::read_data_file(const char* fname)
{
    int i;
    QList<double> sample;
    FILE *pFile, *wFile;
    if (fname!=NULL)
    {
        float f;
        pFile = fopen (fname,"r");
        if (pFile!=NULL)
        {
            i=0;
        while (!feof(pFile))
                 {
                  fscanf(pFile, "%f", &f);
                  sample.append(f);
                  i++;
                 }

        fclose (pFile);
        }
    }
}

void ACT_input::copy_value(const QString)
{
    int n_row=mat_grid->rowCount();
    int n_col=mat_grid->columnCount();

    for(int j=1;j<n_row;j++)
    {
        for(int k=j+1;k<n_col;k++)
        {
        dynamic_cast<QLineEdit*>(mat_grid->itemAtPosition(k,j)->widget())->setText(dynamic_cast<QLineEdit*>(mat_grid->itemAtPosition(j,k)->widget())->text());
        }
    }
}

//Destructor-----------------------------------------------------

ACT_input::~ACT_input()
{

}



