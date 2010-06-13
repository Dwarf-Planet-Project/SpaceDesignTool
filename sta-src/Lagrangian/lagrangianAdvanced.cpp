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
------------------ Author: Valentino Zuccarelli  -------------------------------------------------
 ------------------ E-mail: (Valentino.Zuccarelli@gmail.com) ----------------------------
 */

#include "lagrangianAdvanced.h"
#include "lagrangian.h"

#include "Main/scenariotree.h"
#include "Scenario/scenario.h"
#include "Astro-Core/date.h"
#include "Astro-Core/stacoordsys.h"
#include "Astro-Core/stamath.h"
#include "Scenario/scenario.h"
#include "Astro-Core/threebodyParametersComputation.h"
#include "Astro-Core/cartesianTOrotating.h"
#include "Astro-Core/rotatingTOcartesian.h"
#include "halorbitcomputation.h"
#include "EarthMoonTransfer.h"
#include "Astro-Core/stamath.h"
#include "Optimization/GlobalOptimizers.h"
#include <QDebug>
#include <QString>
#include <QFile>
#include <QTextStream>
#include <QMessageBox>

using namespace Eigen;

extern double Grav_Param, bodies_distance, period_days, velocity;
extern int Lagrmode, System, coordinates_transformation;

#ifdef OLDSCENARIO
extern ScenarioBody* firstBody;
extern ScenarioBody* secondBody;
#endif

LagrangianAdvancedDialog::LagrangianAdvancedDialog(ScenarioTree* parent) :
    QDialog(parent)
{
    setupUi(this);
    //qDebug()<<"starting Lagrangian Mode"<<Lagrmode;

    QDoubleValidator* doubleValidator = new QDoubleValidator(this);
    QDoubleValidator* angleValidator = new QDoubleValidator(this);
    angleValidator->setBottom(0.0);
    angleValidator->setTop(360.0);
    QDoubleValidator* positiveDoubleValidator = new QDoubleValidator(this);
    positiveDoubleValidator->setBottom(0.0);
    QDoubleValidator* zeroToOneValidator = new QDoubleValidator(this);
    zeroToOneValidator->setBottom(0.0);
    zeroToOneValidator->setTop(0.9999);

    connect(HaloDcomboBox, SIGNAL(currentIndexChanged(int)), this, SLOT (research_init_cond()));
    connect(initialConditionscheckBox, SIGNAL(clicked()), this, SLOT (research_init_cond()));
    connect(initialConditionComputationspushButton, SIGNAL(clicked()),this,SLOT(halo_initial_conditions()));
    connect(haloOrbitComputationpushButton, SIGNAL(clicked()), this, SLOT (halo_orbit_computation()));
    connect(ManifoldsComputationpushButton, SIGNAL(clicked()), this, SLOT (manifolds_computation()));
    //connect(ModeSelectionWidget, SIGNAL(currentChanged(int)), this, SLOT (mode_selection()));
    connect(tabWidget, SIGNAL(currentChanged(int)), this, SLOT (mode_selection()));
    connect(haloInitialConditioncheckBox, SIGNAL(clicked()), this, SLOT (halo_orbit_initial_conditions_mode3()));
    //connect(haloInitialConditioncheckBox_4, SIGNAL(clicked()), this, SLOT (halo_orbit_initial_conditions_mode3()));
    //connect(haloInitialConditioncheckBox_5, SIGNAL(clicked()), this, SLOT (halo_orbit_initial_conditions_mode3()));
    connect(RunButton, SIGNAL (pressed()), this, SLOT(run_transfer_optimization()));
    //connect(RunButton_leg1, SIGNAL (pressed()), this, SLOT(run_EarthMoonTransferLeg1()));
    //connect(RunButton_leg2, SIGNAL (pressed()), this, SLOT(run_EarthMoonTransferLeg2()));
    //connect(RunButton_4, SIGNAL (pressed()), this, SLOT(run_EarthMoonTransferPatch()));
    connect(checkBox_64, SIGNAL (clicked()), this, SLOT(optimization_settings()));
    connect(checkBox_63, SIGNAL (clicked()), this, SLOT(optimization_settings()));
}

LagrangianAdvancedDialog::~LagrangianAdvancedDialog()
{
}


 void LagrangianAdvancedDialog::mode_selection()
 {
     if (tabWidget->currentIndex()==1) //manifolds
         Lagrmode=2;
     else if (tabWidget->currentIndex()==0)
         Lagrmode=1;                   //halo orbit
     else if (tabWidget->currentIndex()==2) //transfer orbit
         Lagrmode=3;
     else if (tabWidget->currentIndex()==3) //Earth-Moon transfer orbit
         Lagrmode=4;
     else
         Lagrmode=0; //default mode: trajectory propagator
     //qDebug()<<"mode in use:"<<Lagrmode;
 }


//ScenarioHalo* halo;
HaloOrbit halo;

void LagrangianAdvancedDialog::research_init_cond()
{
    if (initialConditionscheckBox->isChecked())
    {
        label_271->setEnabled(true);
        label_304->setEnabled(true);
        label_305->setEnabled(true);
        label_312->setEnabled(true);
        label_303->setEnabled(true);
        initialConditionComputationspushButton->setEnabled(true);
        LpointSelectioncomboBox->setEnabled(true);
        HaloDcomboBox->setEnabled(true);
        AzAmplitudelineEdit->setEnabled(true);
        AxAmplitudelineEdit->setEnabled(true);

        if (HaloDcomboBox->currentIndex()==0)		//2D case
        {
            AzAmplitudelineEdit->setEnabled(false); ZpositionlineEdit->setEnabled(false);
            halo.trajectory.init_data.Az_amplitude=0;
            halo.trajectory.init_data.State.position.z()=0;
        }
        else
        {
            ZpositionlineEdit->setEnabled(true); AzAmplitudelineEdit->setEnabled(true);
        }
    }
    else
    {
        label_271->setEnabled(false);
        label_304->setEnabled(false);
        label_305->setEnabled(false);
        label_312->setEnabled(false);
        label_303->setEnabled(false);
        initialConditionComputationspushButton->setEnabled(false);
        LpointSelectioncomboBox->setEnabled(false);
        HaloDcomboBox->setEnabled(false);
        AzAmplitudelineEdit->setEnabled(false);
        AxAmplitudelineEdit->setEnabled(false);
     }

}


//function to compute the initial conditions for the Halo orbit (mode 2!)
void LagrangianAdvancedDialog::halo_initial_conditions()
{
#if OLDSCENARIO
    halo.lpointsComputation ();
    //qDebug()<<"Lpoints"<<halo.L1_xposition<<halo.L2_xposition;
    halo.trajectory.init_data.Ax_amplitude=AxAmplitudelineEdit->text().toDouble();

    HaloXpositionlineEdit->setText(" "); HaloZpositionlineEdit->setText(" "); HaloYvelocitylineEdit->setText(" "); HaloPeriodlineEdit->setText(" "); AxAmplitudelineRealEdit->setText(" "); AzAmplitudelineRealEdit->setText(" ");

    //the locations of the Lpoints are read
    halo.L1_xposition=halo.L1_xposition*bodies_distance; halo.L2_xposition=halo.L2_xposition*bodies_distance;

    if (HaloDcomboBox->currentIndex()==1)	//check halo orbit dimensions (2D-3D)
    {
        if (AzAmplitudelineEdit->text().toDouble()==0)
        {
            QMessageBox::warning(NULL,
                              QObject::tr("Halo orbit initial conditions not computed"),
                              QObject::tr("For a 3D halo orbit, the z position must not be 0."));
            XpositionlineEdit->setText(" "); ZpositionlineEdit->setText(" "); YvelocitylineEdit->setText(" ");
            return;
        }
        AzAmplitudelineEdit->setEnabled(true); ZpositionlineEdit->setEnabled(true); HaloZpositionlineEdit->setEnabled(true);

        halo.trajectory.init_data.Az_amplitude=0;
        halo.trajectory.init_data.State.position.z()=0;
    }

    if (LpointSelectioncomboBox->currentIndex()==0)	//Lpoint selection
    {
        halo.trajectory.Lpoint_selected=1; halo.trajectory.Lpoint_Xposition=halo.L1_xposition;}
    else if (LpointSelectioncomboBox->currentIndex()==1)
    {
        halo.trajectory.Lpoint_selected=2; halo.trajectory.Lpoint_Xposition=halo.L2_xposition;}

    //read required amplitudes
    halo.trajectory.init_data.Ax_amplitude=AxAmplitudelineEdit->text().toDouble(); halo.trajectory.init_data.Az_amplitude=AzAmplitudelineEdit->text().toDouble();

    if (halo.trajectory.init_data.Ax_amplitude<0)
    {
        halo.trajectory.init_data.Ax_amplitude=fabs(halo.trajectory.init_data.Ax_amplitude); 	//Ax must be positive
        QMessageBox::warning(NULL,
                              QObject::tr("Halo orbit initial conditions not computed"),
                              QObject::tr("The Ax amplitude has to be a positive number, therefore the sign has been changed."));
        AxAmplitudelineEdit->setText(QString::number(halo.trajectory.init_data.Ax_amplitude,'f',3));
    }
    //qDebug()<<halo.trajectory.init_data.Ax_amplitude<<halo.trajectory.init_data.Az_amplitude;
    halo.getAnalyticalApprox ();
    //qDebug()<<"amplitudes"<<halo.trajectory.init_data.Ax_amplitude<<halo.trajectory.init_data.Az_amplitude;
    //qDebug()<<"state"<<halo.trajectory.init_data.State.position.x()<<halo.trajectory.init_data.State.position.z()<<halo.trajectory.init_data.State.velocity.y();

    if (halo.trajectory.init_data.Ax_amplitude>AxAmplitudelineEdit->text().toDouble()) //check the minimum Ax value allowed for a Halo orbit
        QMessageBox::warning(NULL,
                              QObject::tr("Halo orbit initial conditions not computed"),
                              QObject::tr("The Ax amplitude was too small to compute a halo orbit, therefore it has been set to the minimum value."));
    int digits_number=10;
    XpositionlineEdit->setText(QString::number(halo.trajectory.init_data.State.position.x(),'f',digits_number));
    ZpositionlineEdit->setText(QString::number(halo.trajectory.init_data.State.position.z(),'f',digits_number));
    YvelocitylineEdit->setText(QString::number(halo.trajectory.init_data.State.velocity.y(),'f',digits_number));
    AxAmplitudelineEdit->setText(QString::number(halo.trajectory.init_data.Ax_amplitude,'f',3));
    AzAmplitudelineEdit->setText(QString::number(halo.trajectory.init_data.Az_amplitude,'f',3));
    haloOrbitComputationpushButton->setEnabled(true);
#endif
}


//function to compute the Halo orbit (mode 2!)
void LagrangianAdvancedDialog::halo_orbit_computation()
{
#if OLDSCENARIO
    halo.trajectory.error=halo.manifolds.error=0;
    if (XpositionlineEdit->text().toDouble()==0 || YvelocitylineEdit->text().toDouble()==0)
    {
        QMessageBox::warning(NULL,
                              QObject::tr("Halo orbit not computed"),
                              QObject::tr("The initial guess are wrong or zero."));
        haloOrbitComputationpushButton->setEnabled(false);
        return;
    }

    label_296->setEnabled(true);
    label_297->setEnabled(true);
    label_298->setEnabled(true);
    label_299->setEnabled(true);
    label_300->setEnabled(true);
    label_302->setEnabled(true);
    HaloXpositionlineEdit->setEnabled(true);
    HaloZpositionlineEdit->setEnabled(true);
    HaloYvelocitylineEdit->setEnabled(true);
    AxAmplitudelineRealEdit->setEnabled(true);
    AzAmplitudelineRealEdit->setEnabled(true);
    HaloPeriodlineEdit->setEnabled(true);
    halo.trajectory.Num_positions=haloNumPointsspinBox->text().toInt();		//read number of points for Halo orbit
    halo.trajectory.init_data.State.position.x()=XpositionlineEdit->text().toDouble(); //read initial conditions
    halo.trajectory.init_data.State.position.z()=ZpositionlineEdit->text().toDouble();

    if (halo.trajectory.init_data.State.position.z()==0 && AzAmplitudelineEdit->text().toDouble()!=0)
    {
        QMessageBox::warning(NULL,
                              QObject::tr("Halo orbit not computed"),
                              QObject::tr("The z position must be a number."));
        return;
    }
    else if (halo.trajectory.init_data.State.position.z()==0 && HaloDcomboBox->currentIndex()==1)
    {
        QMessageBox::warning(NULL,
                              QObject::tr("Halo orbit not computed"),
                              QObject::tr("For a 3D halo orbit, the z position cannot be 0."));
        return;
    }
    halo.trajectory.init_data.State.velocity.y()=YvelocitylineEdit->text().toDouble();

    int MAX_integration_time=iterationTimespinBox->text().toInt();

    halo.getNumericalApprox (System, MAX_integration_time);     //Halo orbit computation

    if (halo.trajectory.error==1)
    {
        QMessageBox::warning(NULL,
                              QObject::tr("Halo orbit not computed"),
                              QObject::tr("The computation of a halo orbit with these initial conditions is not possible within the selected maximum integration time, maybe the accuracy required is too high or simply such a Halo orbit doesn't exist."));
        ManifoldsComputationpushButton->setEnabled(false);
        return;
    }
    else if (halo.trajectory.error==3)
    {
        QMessageBox::warning(NULL,
                              QObject::tr("Halo orbit not computed"),
                              QObject::tr("It has not been possible to open the halo orbit output file in order to check the real amplitudes"));
        ManifoldsComputationpushButton->setEnabled(false);
        return;
    }
    else if (halo.trajectory.error==4)
    {
        QMessageBox::warning(NULL,
                              QObject::tr("Halo orbit error"),
                              QObject::tr("The orbit computed is too different from the requested one, then it's not possible to compute its manifolds."));
        ManifoldsComputationpushButton->setEnabled(false);
        return;
    }
    else
        halo.trajectory.error=0;

    ManifoldsComputationpushButton->setEnabled(true);

    int digits_number=10;
    //qDebug()<<halo.trajectory.final_data.Ax_amplitude<<halo.trajectory.final_data.Az_amplitude<<bodies_distance;
    HaloXpositionlineEdit->setText(QString::number(halo.trajectory.init_data.State.position.x(),'f',digits_number)); //show results
    HaloZpositionlineEdit->setText(QString::number(halo.trajectory.init_data.State.position.z(),'f',digits_number));
    HaloYvelocitylineEdit->setText(QString::number(halo.trajectory.init_data.State.velocity.y(),'f',digits_number));
    AxAmplitudelineRealEdit->setText(QString::number(halo.trajectory.final_data.Ax_amplitude*bodies_distance,'f',3));
    AzAmplitudelineRealEdit->setText(QString::number(halo.trajectory.final_data.Az_amplitude*bodies_distance,'f',3));
    HaloPeriodlineEdit->setText(QString::number(halo.trajectory.final_data.period*period_days/(2*sta::Pi()),'f',3));

    if (halo.trajectory.error==1 || halo.trajectory.error==2 || halo.trajectory.error==3) //if the halo orbit has not been computed, a manifolds system cannot be computed
    {
        ManifoldsComputationpushButton->setEnabled(false);
        QMessageBox::warning(NULL,
                              QObject::tr("Manifolds not computed"),
                              QObject::tr("Before computing a manifolds system, you must compute a halo orbit."));
    }
    else
    {
        if (RstablecheckBox->isChecked() || LstablecheckBox->isChecked() || RunstablecheckBox->isChecked() || LunstablecheckBox->isChecked())
            ManifoldsComputationpushButton->setEnabled(true); //if the halo orbit has been computed, now a manifolds system can be computed
    }
#endif
}


//function to compute manifolds (stil mode 2!!!)
void LagrangianAdvancedDialog::manifolds_computation()
{
#if OLDSCENARIO
    double integration_time;
    int points_number;
    halo.manifolds.error=0;
    int first=0;

    //check what manifolds have to be developed
    if (RstablecheckBox->isChecked() || RunstablecheckBox->isChecked() || LstablecheckBox->isChecked() || LunstablecheckBox->isChecked())
    {
        halo.manifolds.epsilon=EpsilondoubleSpinBox->text().toDouble(); //read the halo.manifolds.epsilon value
        if (halo.trajectory.error==0)
        {
            halo.getEigenvalues();
        }
        if (RstablecheckBox->isChecked())  //Right Stable branch
        {
            first=first+1;
            if (MainRstablecheckBox->isChecked()) //stop condition: intersection with a main body
            {
                integration_time=0;
                if (halo.trajectory.Lpoint_selected==2)	//intersection with a main body is not allowed in this case
                {
                    QMessageBox::warning(NULL,
                              QObject::tr("Manifolds not computed"),
                              QObject::tr("There are no main bodies on the right side of L2."));
                    return;
                }
            }
            else if (TintRstablecheckBox->isChecked()) //stop condition: integration time
            {
                integration_time=TintRstabledoubleSpinBox->text().toDouble();
                points_number=nOrbRstablespinBox->text().toInt();
            }
            else
            {
                QMessageBox::warning(NULL,
                              QObject::tr("Manifolds not computed"),
                              QObject::tr("You must choose the manifolds limit condition for the right Stable-branch manifold."));
                return;
            }

        if (halo.trajectory.Lpoint_selected==2 || halo.trajectory.Lpoint_selected==1)   //external manifold, then halo.manifolds.epsilon has to be negative
            halo.manifolds.epsilon=-halo.manifolds.epsilon;

        halo.manifolds.kind=1;	//specification of the manifold required
        halo.getManifolds(halo.manifolds.eigen1, points_number, integration_time, first);
        halo.manifolds.epsilon=fabs(halo.manifolds.epsilon);

        if (halo.manifolds.error==1)
        {
            QMessageBox::warning(NULL,
                              QObject::tr("Manifolds not computed"),
                              QObject::tr("It has not been possible to open the halo orbit data file."));
            return;
        }
        if (halo.manifolds.error==2)
        {
            QMessageBox::warning(NULL,
                              QObject::tr("Manifolds not computed"),
                              QObject::tr("It has not been possible to open the file where the manifolds should be saved."));
            return;
        }
        if (halo.manifolds.error==3 || halo.manifolds.error==4 || halo.manifolds.error==5)
        {
            QMessageBox::warning(NULL,
                              QObject::tr("Manifolds not computed"),
                              QObject::tr("It has not been possible to open the file where the manifolds settings should be saved."));
            return;
        }
        if (halo.manifolds.error==6)
            {
                QMessageBox::warning(NULL,
                              QObject::tr("Manifolds not computed"),
                              QObject::tr("The deviation chosen is too small or the halo orbit is too stable to get a such intersection."));
                return;
            }
        }

        if (RunstablecheckBox->isChecked()) 	//Right Unstable branch
        {
            first=first+1;
            if (MainRunstablecheckBox->isChecked()) //stop condition: intersection with a main body
            {
                integration_time=0;
                if (halo.trajectory.Lpoint_selected==2) //intersection with a main body is not allowed in this case
                {
                    QMessageBox::warning(NULL,
                              QObject::tr("Manifolds not computed"),
                              QObject::tr("There are no main bodies on the right side of L2."));
                    return;
                }
            }
            else if (TintRunstablecheckBox->isChecked()) //stop condition: integration time
            {
                integration_time=TintRunstabledoubleSpinBox->text().toDouble();
                points_number=nOrbRunstablespinBox->text().toInt();
            }
            else
            {
                QMessageBox::warning(NULL,
                              QObject::tr("Manifolds not computed"),
                              QObject::tr("You must choose the manifolds limit condition for the right Unstable-branch manifold."));
                return;
            }
            if (halo.trajectory.Lpoint_selected==2 || halo.trajectory.Lpoint_selected==1) //external manifold, then halo.manifolds.epsilon has to be negative
                halo.manifolds.epsilon=-halo.manifolds.epsilon;
            halo.manifolds.kind=2;//specification of the manifold required
            halo.getManifolds(halo.manifolds.eigen2, points_number, integration_time, first);
            halo.manifolds.epsilon=fabs(halo.manifolds.epsilon);

            if (halo.manifolds.error==1)
            {
                QMessageBox::warning(NULL,
                              QObject::tr("Manifolds not computed"),
                              QObject::tr("It has not been possible to open the halo orbit data file."));
                return;
            }
            if (halo.manifolds.error==2)
            {
                QMessageBox::warning(NULL,
                              QObject::tr("Manifolds not computed"),
                              QObject::tr("It has not been possible to open the file where the manifolds should be saved."));
                return;
            }
            if (halo.manifolds.error==3 || halo.manifolds.error==4 || halo.manifolds.error==5)
            {
                QMessageBox::warning(NULL,
                              QObject::tr("Manifolds not computed"),
                              QObject::tr("It has not been possible to open the file where the manifolds settings should be saved."));
                return;
            }
            if (halo.manifolds.error==6)
            {
                QMessageBox::warning(NULL,
                              QObject::tr("Manifolds not computed"),
                              QObject::tr("The deviation chosen is too small or the halo orbit is too stable to get a such intersection."));
                return;
            }
        }

        if (LstablecheckBox->isChecked())  //Left Stable branch
        {
            first=first+1;
            if (MainLstablecheckBox->isChecked()) //stop condition: intersection with a main body
            {
                integration_time=0;
                if (halo.trajectory.Lpoint_selected==1 && (System!=9 && System!=10 && System!=11 && System!=12))	//intersection with the sun makes no sense, too far away
                {
                    QMessageBox::warning(NULL,
                              QObject::tr("Manifolds not computed"),
                              QObject::tr("An intersection with the Sun makes no sense."));

                    return;
                }
            }
            else if (TintLstablecheckBox->isChecked()) //stop condition: integration time
            {
                integration_time=TintLstabledoubleSpinBox->text().toDouble();
                points_number=nOrbLstablespinBox->text().toInt();
            }
            else
            {
                QMessageBox::warning(NULL,
                              QObject::tr("Manifolds not computed"),
                              QObject::tr("You must choose the manifolds limit condition for the left Stable-branch manifold."));
                return;
            }

            if (halo.trajectory.Lpoint_selected==3)  //external manifold, then halo.manifolds.epsilon has to be negative
                halo.manifolds.epsilon=-halo.manifolds.epsilon;
            halo.manifolds.kind=3;//specification of the manifold required
            halo.getManifolds(halo.manifolds.eigen1, points_number, integration_time, first);
            halo.manifolds.epsilon=fabs(halo.manifolds.epsilon);
            if (halo.manifolds.error==1)
            {
                QMessageBox::warning(NULL,
                              QObject::tr("Manifolds not computed"),
                              QObject::tr("It has not been possible to open the halo orbit data file."));
                return;
            }
            if (halo.manifolds.error==2)
            {
                QMessageBox::warning(NULL,
                              QObject::tr("Manifolds not computed"),
                              QObject::tr("It has not been possible to open the file where the manifolds should be saved."));
                return;
            }
            if (halo.manifolds.error==3 || halo.manifolds.error==4 || halo.manifolds.error==5)
            {
                QMessageBox::warning(NULL,
                              QObject::tr("Manifolds not computed"),
                              QObject::tr("It has not been possible to open the file where the manifolds settings should be saved."));
                return;
            }
            if (halo.manifolds.error==6)
            {
                QMessageBox::warning(NULL,
                              QObject::tr("Manifolds not computed"),
                              QObject::tr("The deviation chosen is too small or the halo orbit is too stable to get a such intersection."));
                return;
            }
        }

        if (LunstablecheckBox->isChecked()) 	//Left Unstable branch
        {
            first=first+1;
            if (MainLunstablecheckBox->isChecked()) //stop condition: intersection with a main body
            {
                integration_time=0;
                if (halo.trajectory.Lpoint_selected==1 && (System!=9 && System!=10 && System!=11 && System!=12))	//intersection with the sun makes no sense, too far away
                {
                    QMessageBox::warning(NULL,
                              QObject::tr("Manifolds not computed"),
                              QObject::tr("An intersection with the Sun makes no sense."));
                    return;
                }
            }
            else if (TintLunstablecheckBox->isChecked()) //stop condition: integration time
            {
                integration_time=TintLunstabledoubleSpinBox->text().toDouble();
                points_number=nOrbLunstablespinBox->text().toInt();
            }
            else
            {
                QMessageBox::warning(NULL,
                              QObject::tr("Manifolds not computed"),
                              QObject::tr("You must choose the manifolds limit condition for the left Unstable-branch manifold."));
                return;
            }

            if (halo.trajectory.Lpoint_selected==3)	//external manifold, then halo.manifolds.epsilon has to be negative
                halo.manifolds.epsilon=-halo.manifolds.epsilon;
            halo.manifolds.kind=4;  //specification of the manifold required
            halo.getManifolds(halo.manifolds.eigen2, points_number, integration_time, first);
            halo.manifolds.epsilon=fabs(halo.manifolds.epsilon);

            if (halo.manifolds.error==1)
            {
                QMessageBox::warning(NULL,
                              QObject::tr("Manifolds not computed"),
                              QObject::tr("It has not been possible to open the halo orbit data file."));
                return;
            }
            if (halo.manifolds.error==2)
            {
                QMessageBox::warning(NULL,
                              QObject::tr("Manifolds not computed"),
                              QObject::tr("It has not been possible to open the file where the manifolds should be saved."));
                return;
            }
            if (halo.manifolds.error==3 || halo.manifolds.error==4 || halo.manifolds.error==5)
            {
                QMessageBox::warning(NULL,
                              QObject::tr("Manifolds not computed"),
                              QObject::tr("It has not been possible to open the file where the manifolds settings should be saved."));
                return;
            }
            if (halo.manifolds.error==6)
            {
                QMessageBox::warning(NULL,
                              QObject::tr("Manifolds not computed"),
                              QObject::tr("The deviation chosen is too small or the halo orbit is too stable to get a such intersection."));
                return;
            }
        }

        QFile file_manifolds_settings2 ("3BMmanifolds_settings.stam");
        if (!file_manifolds_settings2.open(QIODevice::Append | QIODevice::Text))
        {
            QMessageBox::warning(NULL,
                              QObject::tr("Manifolds not computed"),
                              QObject::tr("It has not been possible to write the condition that states the end of the settings in the apposite file."));
            return;
        }
        QTextStream endsettings(&file_manifolds_settings2);
        endsettings<<"0";		//condition that states the end of the settings
        file_manifolds_settings2.close();
        //qDebug()<<"3BMmanifolds_settings.stam";
    }
    else
    {
        QMessageBox::warning(NULL,
                              QObject::tr("Manifolds not computed"),
                              QObject::tr("You must selected at least one branch to compute manifolds."));
        return;
    }
#endif
}

void LagrangianAdvancedDialog::halo_orbit_initial_conditions_mode3()
{
#if OLDSCENARIO
    if (haloInitialConditioncheckBox->isChecked())
    {
        HaloXpositionlineEdit_2->setText(HaloXpositionlineEdit->text());
        HaloZpositionlineEdit_2->setText(HaloZpositionlineEdit->text());
        HaloYvelocitylineEdit_2->setText(HaloYvelocitylineEdit->text());
    }
    /*
    else if (haloInitialConditioncheckBox_4->isChecked())
    {
        HaloXpositionlineEdit_4->setText(HaloXpositionlineEdit->text());
        HaloZpositionlineEdit_4->setText(HaloZpositionlineEdit->text());
        HaloYvelocitylineEdit_4->setText(HaloYvelocitylineEdit->text());
    }
    else if (haloInitialConditioncheckBox_5->isChecked())
    {
        HaloXpositionlineEdit_5->setText(HaloXpositionlineEdit->text());
        HaloZpositionlineEdit_5->setText(HaloZpositionlineEdit->text());
        HaloYvelocitylineEdit_5->setText(HaloYvelocitylineEdit->text());
    }
    */
#endif
}


extern ScenarioThreebodyenvironment* threebodyenvironment;// = lagrangianAdvanced->threebodyenvironment();
extern ScenarioSimulationParameters* parameters;// = lagrangianAdvanced->simulationParameters();
extern ScenarioHalo* Halo;// = lagrangianAdvanced->halo();
extern ScenarioThreebodyTransfer* threebodyTransfer;// = lagrangianAdvanced->threebodyTransfer();

bool LagrangianAdvancedDialog::loadValues(ScenarioLagrangianTrajectory* lagrangianAdvanced)
{
#if OLDSCENARIO
    Lagrmode=1;

        loadValues(Halo); loadValues (threebodyTransfer);
        if (Lagrmode==1)
            tabWidget->setCurrentIndex(0);
        else if (Lagrmode==2)
            tabWidget->setCurrentIndex(1);
        else if (Lagrmode==3)
            tabWidget->setCurrentIndex(2);
        else if (Lagrmode==4)
            tabWidget->setCurrentIndex(3);

        if(loadValues(Halo) && loadValues(threebodyTransfer))
        {
            return true;
        }
        else
#endif
            return false;
}


bool LagrangianAdvancedDialog::loadValues(ScenarioHalo* Halo)
{
#if OLDSCENARIO
        halo.trajectory.error=-1;
        ScenarioInitialStatePosition* initialposition=Halo->initialStatePosition();
        ScenarioAmplitudes* amplitudes=Halo->amplitudes();
        ScenarioManifolds* manifolds=Halo->manifolds();

        const StaBody* sun = STA_SOLAR_SYSTEM->sun();
        double JD=1;

        if (Halo->Lpoint()=="L1")
            LpointSelectioncomboBox->setCurrentIndex(0);
        else if (Halo->Lpoint()=="L2")
            LpointSelectioncomboBox->setCurrentIndex(1);

        if (Halo->amplitudes()->Ax()!=0 && Halo->amplitudes()->Az()!=0)
        {
            HaloDcomboBox->setCurrentIndex(1);
            initialConditionscheckBox->setChecked(true);
            label_271->setEnabled(true);
            label_304->setEnabled(true);
            label_305->setEnabled(true);
            label_312->setEnabled(true);
            label_303->setEnabled(true);
            initialConditionComputationspushButton->setEnabled(true);
            LpointSelectioncomboBox->setEnabled(true);
            HaloDcomboBox->setEnabled(true);
            AzAmplitudelineEdit->setEnabled(true);
            AxAmplitudelineEdit->setEnabled(true);
            AxAmplitudelineEdit->setText(QString::number(Halo->amplitudes()->Ax()));
            AzAmplitudelineEdit->setText(QString::number(Halo->amplitudes()->Az()));
        }
        else if (Halo->amplitudes()->Ax()!=0 && Halo->amplitudes()->Az()==0)
        {
            HaloDcomboBox->setCurrentIndex(0);
            initialConditionscheckBox->setChecked(true);
            label_271->setEnabled(true);
            label_304->setEnabled(true);
            label_305->setEnabled(true);
            label_312->setEnabled(true);
            label_303->setEnabled(true);
            initialConditionComputationspushButton->setEnabled(true);
            LpointSelectioncomboBox->setEnabled(true);
            HaloDcomboBox->setEnabled(true);
            AzAmplitudelineEdit->setEnabled(true);
            AxAmplitudelineEdit->setEnabled(true);
            AzAmplitudelineEdit->setEnabled(false); label_305->setEnabled(false);
            AxAmplitudelineEdit->setText(QString::number(Halo->amplitudes()->Ax()));
        }
        ScenarioAbstractInitialState* initialState = Halo->initialStatePosition()->initialState();
        ScenarioStateVector* stateVector_Halo = dynamic_cast<ScenarioStateVector*>(initialState);
        if (stateVector_Halo->position().x()>0.5)
        {
            XpositionlineEdit->setText(QString::number(stateVector_Halo->position().x()));
            ZpositionlineEdit->setText(QString::number(stateVector_Halo->position().z()));
            YvelocitylineEdit->setText(QString::number(stateVector_Halo->velocity().y()));
        }
        if (Halo->manifolds()->branch()->parameters()->deviation()<=0.001000 && Halo->manifolds()->branch()->parameters()->deviation()>=0.000001)
            EpsilondoubleSpinBox->setValue(Halo->manifolds()->branch()->parameters()->deviation());

        if (Halo->manifolds()->branch()->state()=="true") //Right-stable manifold
        {
            RstablecheckBox->setChecked(true);
            if (Halo->manifolds()->branch()->parameters()->endCondition()->intersection()==1)
            {
                MainRstablecheckBox->setChecked(true);
                MainRstablecheckBox->setEnabled(true);
            }
            else if (Halo->manifolds()->branch()->parameters()->endCondition()->integrationTime()>0)
            {
                TintRstablecheckBox->setChecked(true);
                TintRstabledoubleSpinBox->setValue(Halo->manifolds()->branch()->parameters()->endCondition()->integrationTime());
                TintRstablecheckBox->setEnabled(true);
                TintRstabledoubleSpinBox->setEnabled(true);
                nOrbRstablespinBox->setEnabled(true);
                nOrbRstablespinBox->setValue(Halo->manifolds()->branch()->parameters()->endCondition()->numPositions());
            }
        }

        if (Halo->manifolds()->branch2()->state()=="true")  //Left-stable manifold
        {
            LstablecheckBox->setChecked(true);
            if (Halo->manifolds()->branch2()->parameters()->endCondition()->intersection()==1)
            {
                MainLstablecheckBox->setChecked(true);
                MainLstablecheckBox->setEnabled(true);
            }
            else if (Halo->manifolds()->branch2()->parameters()->endCondition()->integrationTime()>0)
            {
                TintLstablecheckBox->setChecked(true);
                TintLstabledoubleSpinBox->setValue(Halo->manifolds()->branch2()->parameters()->endCondition()->integrationTime());
                TintLstablecheckBox->setEnabled(true);
                TintLstabledoubleSpinBox->setEnabled(true);
                nOrbLstablespinBox->setEnabled(true);
                nOrbLstablespinBox->setValue(Halo->manifolds()->branch2()->parameters()->endCondition()->numPositions());
            }
        }

        if (Halo->manifolds()->branch3()->state()=="true") //Right-unstable manifold
        {
            RunstablecheckBox->setChecked(true);
            if (Halo->manifolds()->branch3()->parameters()->endCondition()->intersection()==1)
            {
                MainRunstablecheckBox->setChecked(true);
                MainRunstablecheckBox->setEnabled(true);
            }
            else if (Halo->manifolds()->branch3()->parameters()->endCondition()->integrationTime()>0)
            {
                TintRunstablecheckBox->setChecked(true);
                TintRunstabledoubleSpinBox->setValue(Halo->manifolds()->branch3()->parameters()->endCondition()->integrationTime());
                TintRunstablecheckBox->setEnabled(true);
                TintRunstabledoubleSpinBox->setEnabled(true);
                nOrbRunstablespinBox->setEnabled(true);
                nOrbRunstablespinBox->setValue(Halo->manifolds()->branch3()->parameters()->endCondition()->numPositions());
            }
        }

        if (Halo->manifolds()->branch4()->state()=="true")  //Left-unstable manifold
        {
            LunstablecheckBox->setChecked(true);
            if (Halo->manifolds()->branch4()->parameters()->endCondition()->intersection()==1)
            {
                MainLunstablecheckBox->setChecked(true);
                MainLunstablecheckBox->setEnabled(true);
            }
            else if (Halo->manifolds()->branch4()->parameters()->endCondition()->integrationTime()>0)
            {
                TintLunstablecheckBox->setChecked(true);
                TintLunstabledoubleSpinBox->setValue(Halo->manifolds()->branch4()->parameters()->endCondition()->integrationTime());
                TintLunstablecheckBox->setEnabled(true);
                TintLunstabledoubleSpinBox->setEnabled(true);
                nOrbLunstablespinBox->setEnabled(true);
                nOrbLunstablespinBox->setValue(Halo->manifolds()->branch4()->parameters()->endCondition()->numPositions());
            }
        }
#endif
        return true;
}


bool LagrangianAdvancedDialog::loadValues(ScenarioThreebodyTransfer* threebodyTransfer)
{
#if OLDSCENARIO
    QDateTime startTime = sta::JdToCalendar(sta::MjdToJd(threebodyTransfer->startTime));
    //qDebug()<<"startTime"<<startTime;
    dateTimeEdit->setDateTime(startTime);

    ScenarioAbstractInitialState* parkingOrbit=threebodyTransfer->parkingOrbit()->pOrbit()->initialState();
    ScenarioAbstractInitialState* haloOrbit=threebodyTransfer->haloOrbit()->hOrbit()->initialState();
    ScenarioKeplerianElements* elements = dynamic_cast<ScenarioKeplerianElements*>(parkingOrbit);
    ScenarioStateVector* stateVector_Halo = dynamic_cast<ScenarioStateVector*>(haloOrbit);

    semimajorAxisEdit_2->setText(QString::number(elements->m_semimajorAxis));
    eccentricityEdit_2->setText(QString::number(elements->m_eccentricity));
    inclinationEdit_2->setText(QString::number(elements->m_inclination));
    raanEdit_2->setText(QString::number(elements->m_raan));
    argOfPeriapsisEdit_2->setText(QString::number(elements->m_argumentOfPeriapsis));
    trueAnomalyEdit_2->setText(QString::number(elements->m_trueAnomaly));

    HaloXpositionlineEdit_2->setText(QString::number(stateVector_Halo->position().x()));
    HaloZpositionlineEdit_2->setText(QString::number(stateVector_Halo->position().z()));
    HaloYvelocitylineEdit_2->setText(QString::number(stateVector_Halo->velocity().y()));

    CentralBodyComboBox_2->clear();
        if (firstBody->body()->name()!="Sun")
        {
            CentralBodyComboBox_2->setEnabled(true);
            CentralBodyComboBox_2->addItem(tr((firstBody->body()->name()).toLatin1()), firstBody->body()->id());
        }

    CentralBodyComboBox_2->addItem(tr((secondBody->body()->name()).toLatin1()),secondBody->body()->id());

    if (threebodyTransfer->parkingOrbit()->pOrbit()->centralsystemBody()->body()->name()==secondBody->body()->name() && CentralBodyComboBox_2->count()==2)
            CentralBodyComboBox_2->setCurrentIndex(1);
        else
            CentralBodyComboBox_2->setCurrentIndex(0);

    if (threebodyTransfer->optimizationParameters()->timeofFlight()==true)
    {
        checkBox_63->setChecked(true);
        spinBox->setValue(threebodyTransfer->optimizationParameters()->maximumToF());
    }
    else
        checkBox_63->setChecked(false);
    if (threebodyTransfer->optimizationParameters()->propellant()==true)
        checkBox_64->setChecked(true);
    else
        checkBox_64->setChecked(false);
#endif
    return true;
}


int transformationMode,  maxTOF;  double startTime, finalTime, twoBodyMu;
double raanReq, inclinationReq, n, BestTotalDeltaV, timefinTransfer;
HaloOrbit haloTransfer;
ScenarioKeplerianElementsType* elementTransfer;
double inclinationInit, eInit, trueAnomalyInit, smaInit, argOfPeriapsisInit, raanInit, jdMission;

sta::StateVector initialPositionRotating,initialPosition;
double patchingPoint_rotating_x, patchingPoint_rotating_y, patchingPoint_rotating_z, patchingPoint_rotating_Vx, patchingPoint_rotating_Vy, patchingPoint_rotating_Vz, trueAno;
ScenarioStateVectorType* stateVector_HaloTransfer;
double finalHaloRotX,finalHaloRotY,finalHaloRotZ,finalHaloRotVX,finalHaloRotVY,finalHaloRotVZ,deviation,eigen0;
int numObj, numIterations, initMODE, op=0;

void LagrangianAdvancedDialog::run_transfer_optimization()
{
#if OLDSCENARIO
    const StaBody* centralBody = STA_SOLAR_SYSTEM->lookup(CentralBodyComboBox_2->currentText());

    //qDebug()<<"check"<<Grav_Param<<bodies_distance<<firstBody->body()->name()<<secondBody->body()->name();
    startTime = sta::JdToMjd(sta::CalendarToJd(dateTimeEdit->dateTime()));
    finalTime = sta::JdToMjd(sta::CalendarToJd(dateTimeEdit_2->dateTime()));
    //qDebug()<<"interval"<<startTime<<finalTime;

    ScenarioInitialStatePosition*  parkingOrbitsave = threebodyTransfer->parkingOrbit()->pOrbit();
    ScenarioInitialStatePosition* haloOrbitsave = threebodyTransfer->haloOrbit()->hOrbit();

    stateVector_HaloTransfer = new ScenarioStateVector();
    stateVector_HaloTransfer->setPosition(Vector3d(HaloXpositionlineEdit_2->text().toDouble(),
                                           0,
                                           HaloZpositionlineEdit_2->text().toDouble()));
    stateVector_HaloTransfer->setVelocity(Vector3d(0,
                                           HaloYvelocitylineEdit_2->text().toDouble(),
                                           0));
    haloOrbitsave->setInitialState(stateVector_HaloTransfer);

    elementTransfer = new ScenarioKeplerianElements();
    smaInit = elementTransfer->m_semimajorAxis       = semimajorAxisEdit_2->text().toDouble();
    eInit = elementTransfer->m_eccentricity        = eccentricityEdit_2->text().toDouble();
    inclinationInit = elementTransfer->m_inclination         = inclinationEdit_2->text().toDouble();
    raanInit = elementTransfer->m_raan                = raanEdit_2->text().toDouble();
    argOfPeriapsisInit = elementTransfer->m_argumentOfPeriapsis = argOfPeriapsisEdit_2->text().toDouble();
    trueAnomalyInit = elementTransfer->m_trueAnomaly         = trueAnomalyEdit_2->text().toDouble();

    parkingOrbitsave->setInitialState(elementTransfer);

    threebodyTransfer->parkingOrbit()->setInitialStatePosition(parkingOrbitsave);
    threebodyTransfer->haloOrbit()->setInitialStatePosition(haloOrbitsave);

    initialPosition = threebodyTransfer->parkingOrbit()->pOrbit()->initialState()->computeStateVector(centralBody);
    //qDebug()<<"initialX"<<initialPosition.position.x()<<initialPosition.position.y()<<initialPosition.position.z()<<initialPosition.velocity.x()<<initialPosition.velocity.y()<<initialPosition.velocity.z();

    if (centralBody->id()==firstBody->body()->id())
    {
        twoBodyMu=firstBody->body()->mu();
        transformationMode=3;
    }
    else if (centralBody->id()==secondBody->body()->id())
    {
        twoBodyMu=secondBody->body()->mu();
        transformationMode=4;
    }
    else
        return;
    //qDebug()<<"coordinates transformation"<<transformationMode;
    initialPositionRotating=cartesianTOrotating(transformationMode, firstBody->body(), secondBody->body(), initialPosition, startTime);

        //qDebug()<<"initialX"<<initialPositionRotating.position.x()<<initialPositionRotating.position.y()<<initialPositionRotating.position.z()<<initialPositionRotating.velocity.x()<<initialPositionRotating.velocity.y()<<initialPositionRotating.velocity.z();

        haloTransfer.trajectory.init_data.State.velocity.x() = haloTransfer.trajectory.init_data.State.velocity.z() = haloTransfer.trajectory.init_data.State.position.y() = 0;
        haloTransfer.trajectory.init_data.State.position.x() = stateVector_HaloTransfer->position().x();
        haloTransfer.trajectory.init_data.State.position.z() = stateVector_HaloTransfer->position().z();
        haloTransfer.trajectory.init_data.State.velocity.y() = stateVector_HaloTransfer->velocity().y();

        //qDebug()<<haloTransfer.trajectory.init_data.State.velocity.x()<<haloTransfer.trajectory.init_data.State.velocity.y();
        //qDebug()<<2<<initialPositionRotating.position.x();
        haloTransfer.manifolds.epsilon=EpsilondoubleSpinBox_2->text().toDouble();
        //qDebug()<<haloTransfer.manifolds.epsilon;

        haloTransfer.lpointsComputation();
        if (haloTransfer.trajectory.init_data.State.position.x()>(1-Grav_Param)) //from 2nd body to L2
        {
            haloTransfer.trajectory.Lpoint_Xposition=haloTransfer.L2_xposition*bodies_distance;
            haloTransfer.trajectory.Lpoint_selected=2;
            haloTransfer.manifolds.kind=4;
        }
        else if (haloTransfer.trajectory.init_data.State.position.x()<(1-Grav_Param) && initialPositionRotating.position.x()<0) //from 1st body to L1
        {
            haloTransfer.trajectory.Lpoint_Xposition=haloTransfer.L1_xposition*bodies_distance;
            haloTransfer.trajectory.Lpoint_selected=1;
            haloTransfer.manifolds.kind=4;
        }
        else if (haloTransfer.trajectory.init_data.State.position.x()<(1-Grav_Param) && initialPositionRotating.position.x()>0) //from 2nd body to L1
        {
            haloTransfer.trajectory.Lpoint_Xposition=haloTransfer.L1_xposition*bodies_distance;
            haloTransfer.trajectory.Lpoint_selected=1;
            haloTransfer.manifolds.kind=2;
        }
        int MAX_integration_time=10;
        haloTransfer.trajectory.Num_positions=1000;
        haloTransfer.getNumericalApprox(System,MAX_integration_time);

        //qDebug()<<"period"<<haloTransfer.trajectory.final_data.period;
        haloTransfer.getEigenvalues();
        //qDebug()<<"eigen"<<haloTransfer.manifolds.eigen1<<haloTransfer.manifolds.eigen2;

    BestTotalDeltaV=100; op=0;
    GlobalOptimizers OPT;
    //qDebug()<<"fase 1";
    maxTOF=spinBox->text().toInt();
    numIterations=IterationspinBox->text().toInt();
    //qDebug()<<maxTOF<<numIterations<<haloTransfer.manifolds.epsilon;
    if(checkBox_64->isChecked()==true && checkBox_63->isChecked()==true && (checkBox->isChecked()==true || checkBox_2->isChecked()==true))
    {
        numObj=2;
    }
    else if (checkBox_64->isChecked()==true && checkBox_63->isChecked()==true && checkBox->isChecked()==false && checkBox_2->isChecked()==false)
    {
        QMessageBox::warning(NULL,
                          QObject::tr("Optimized process can't start"),
                          QObject::tr("An optimizer has to be selected."));
        return;
    }
    else
        numObj=1;
    OPT.InitializeOptimizer();
    //qDebug()<<"fase 2";
    if(checkBox_64->isChecked()==true && checkBox_63->isChecked()==true)
    {
        if(checkBox->isChecked()==true)
            OPT.RunOptimizer(1); //ngsa2
        if(checkBox_2->isChecked()==true)
            OPT.RunOptimizer(2); //dgmopso
    }
    else
    {
        initMODE=0;
        OPT.RunOptimizer(1);
        //initMODE=1;
        //OPT.RunOptimizer(1);
    }

    if(numObj==1)
    {
        qDebug()<<"best parameters"<<n<<n*smaInit<<raanReq<<inclinationReq<<jdMission;
        qDebug()<<"performance:"<<"DeltaV"<<BestTotalDeltaV<<"TOF"<<sta::secsToDays((timefinTransfer)/(2*sta::Pi())*(secondBody->body()->orbitalPeriod()*24*3600));
        qDebug()<<"final position"<<patchingPoint_rotating_x<<patchingPoint_rotating_y<<patchingPoint_rotating_z<<patchingPoint_rotating_Vx<<patchingPoint_rotating_Vy<<patchingPoint_rotating_Vz;
    }

    if (BestTotalDeltaV==100 && numObj==1)
    {
        QMessageBox::warning(NULL,
                          QObject::tr("Optimized transfer orbit not computed"),
                          QObject::tr("The optimizer has not been able to find an optimized solution. Maybe the max TOF required was to low or the launch window not well set."));
    }
    else if (numObj==2)
    {
        QMessageBox::warning(NULL,
                          QObject::tr("Pareto front computed"),
                          QObject::tr("The Pareto front has been computed, but for multi-objective optimization is not possible to plot the best trajectory, because different trajectories with the same fitness have been computed."));
    }
#endif
}


void LagrangianAdvancedDialog::optimization_settings()
{
    if(checkBox_64->isChecked()==true && checkBox_63->isChecked()==true)
    {
        label_9->setEnabled(true);
        checkBox->setEnabled(true);
        checkBox_2->setEnabled(true);
    }
    else
    {
        label_9->setEnabled(false);
        checkBox->setEnabled(false);
        checkBox_2->setEnabled(false);
    }
}



/*
int transformationMode,  maxTOF;  double startTime, finalTime, twoBodyMu;
double raanReq, inclinationReq, n, BestTotalDeltaV, timefinTransfer;
HaloOrbit haloTransfer;
ScenarioKeplerianElements* elementTransfer;
double inclinationInit, eInit, trueAnomalyInit, smaInit, argOfPeriapsisInit, raanInit, jdMission;

sta::StateVector initialPositionRotating,initialPosition;
double patchingPoint_rotating_x, patchingPoint_rotating_y, patchingPoint_rotating_z, patchingPoint_rotating_Vx, patchingPoint_rotating_Vy, patchingPoint_rotating_Vz, trueAno;
ScenarioStateVector* stateVector_HaloTransfer;
double finalHaloRotX,finalHaloRotY,finalHaloRotZ,finalHaloRotVX,finalHaloRotVY,finalHaloRotVZ,deviation,eigen0;
*/

double trueAno_Leg1, inclinationInit_Leg1, eInit_Leg1, trueAnomalyInit_Leg1, smaInit_Leg1, argOfPeriapsisInit_Leg1, raanInit_Leg1, jdMission_Leg1;
double raanReq_Leg1, inclinationReq_Leg1, n_Leg1; bool leg1=false;
double patchingPoint_rotating_x_Leg1, patchingPoint_rotating_y_Leg1, patchingPoint_rotating_z_Leg1, patchingPoint_rotating_Vx_Leg1, patchingPoint_rotating_Vy_Leg1, patchingPoint_rotating_Vz_Leg1;
HaloOrbit halo_Leg1;

void LagrangianAdvancedDialog::run_EarthMoonTransferLeg1()
{
#if OLDSCENARIO
    const StaBody* sun = STA_SOLAR_SYSTEM->sun();
    const StaBody* earth = STA_SOLAR_SYSTEM->earth();
    firstBody->setBody(sun); secondBody->setBody(earth);
    Grav_Param=getGravParam_user (firstBody->body()->mu(),secondBody->body()->mu());
    bodies_distance=secondBody->body()->distance();
#endif
    /*
    ScenarioInitialStatePosition*  parkingOrbitsave = threebodyTransfer->parkingOrbit()->pOrbit();
    ScenarioInitialStatePosition* haloOrbitsave = threebodyTransfer->haloOrbit()->hOrbit();

    stateVector_HaloTransfer = new ScenarioStateVector();
    stateVector_HaloTransfer->setPosition(Vector3d(HaloXpositionlineEdit_5->text().toDouble(),
                                           0,
                                           HaloZpositionlineEdit_5->text().toDouble()));
    stateVector_HaloTransfer->setVelocity(Vector3d(0,
                                           HaloYvelocitylineEdit_5->text().toDouble(),
                                           0));
    haloOrbitsave->setInitialState(stateVector_HaloTransfer);

    elementTransfer = new ScenarioKeplerianElements();
    smaInit = elementTransfer->m_semimajorAxis       = semimajorAxisEdit_5->text().toDouble();
    eInit = elementTransfer->m_eccentricity        = eccentricityEdit_5->text().toDouble();
    inclinationInit = elementTransfer->m_inclination         = inclinationEdit_5->text().toDouble();
    raanInit = elementTransfer->m_raan                = raanEdit_5->text().toDouble();
    argOfPeriapsisInit = elementTransfer->m_argumentOfPeriapsis = argOfPeriapsisEdit_5->text().toDouble();
    trueAnomalyInit = elementTransfer->m_trueAnomaly         = trueAnomalyEdit_5->text().toDouble();

    parkingOrbitsave->setInitialState(elementTransfer);

    threebodyTransfer->parkingOrbit()->setInitialStatePosition(parkingOrbitsave);
    threebodyTransfer->haloOrbit()->setInitialStatePosition(haloOrbitsave);

    initialPosition = threebodyTransfer->parkingOrbit()->pOrbit()->initialState()->computeStateVector(earth);
    //qDebug()<<"initialX"<<initialPosition.position.x()<<initialPosition.position.y()<<initialPosition.position.z()<<initialPosition.velocity.x()<<initialPosition.velocity.y()<<initialPosition.velocity.z();

    twoBodyMu=secondBody->body()->mu();
    transformationMode=4;

    double startTime=0; ///////////////////TO BE FIXED

    //qDebug()<<"coordinates transformation"<<transformationMode;
    initialPositionRotating=cartesianTOrotating(transformationMode, firstBody->body(), secondBody->body(), initialPosition, startTime);

        //qDebug()<<"initialX"<<initialPositionRotating.position.x()<<initialPositionRotating.position.y()<<initialPositionRotating.position.z()<<initialPositionRotating.velocity.x()<<initialPositionRotating.velocity.y()<<initialPositionRotating.velocity.z();

        haloTransfer.trajectory.init_data.State.velocity.x() = haloTransfer.trajectory.init_data.State.velocity.z() = haloTransfer.trajectory.init_data.State.position.y() = 0;
        haloTransfer.trajectory.init_data.State.position.x() = stateVector_HaloTransfer->position().x();
        haloTransfer.trajectory.init_data.State.position.z() = stateVector_HaloTransfer->position().z();
        haloTransfer.trajectory.init_data.State.velocity.y() = stateVector_HaloTransfer->velocity().y();

        //qDebug()<<haloTransfer.trajectory.init_data.State.velocity.x()<<haloTransfer.trajectory.init_data.State.velocity.y();
        //qDebug()<<2<<initialPositionRotating.position.x();
        haloTransfer.manifolds.epsilon=EpsilondoubleSpinBox_5->text().toDouble();
        //qDebug()<<haloTransfer.manifolds.epsilon;

        haloTransfer.lpointsComputation();
        if (haloTransfer.trajectory.init_data.State.position.x()>(1-Grav_Param)) //from 2nd body to L2
        {
            haloTransfer.trajectory.Lpoint_Xposition=haloTransfer.L2_xposition*bodies_distance;
            haloTransfer.trajectory.Lpoint_selected=2;
            haloTransfer.manifolds.kind=4;
        }
        else if (haloTransfer.trajectory.init_data.State.position.x()<(1-Grav_Param) && initialPositionRotating.position.x()<0) //from 1st body to L1
        {
            haloTransfer.trajectory.Lpoint_Xposition=haloTransfer.L1_xposition*bodies_distance;
            haloTransfer.trajectory.Lpoint_selected=1;
            haloTransfer.manifolds.kind=4;
        }
        else if (haloTransfer.trajectory.init_data.State.position.x()<(1-Grav_Param) && initialPositionRotating.position.x()>0) //from 2nd body to L1
        {
            haloTransfer.trajectory.Lpoint_Xposition=haloTransfer.L1_xposition*bodies_distance;
            haloTransfer.trajectory.Lpoint_selected=1;
            haloTransfer.manifolds.kind=2;
        }
        int MAX_integration_time=10;
        haloTransfer.trajectory.Num_positions=1000;
        haloTransfer.getNumericalApprox(System,MAX_integration_time);

        //qDebug()<<"period"<<haloTransfer.trajectory.final_data.period;
        haloTransfer.getEigenvalues();
        //qDebug()<<"eigen"<<haloTransfer.manifolds.eigen1<<haloTransfer.manifolds.eigen2;

    BestTotalDeltaV=100; op=0;
    GlobalOptimizers OPT_Transfer;
    //qDebug()<<"fase 1 transfer";
    maxTOF=TOFspinBox_leg1->value();;
    numIterations=100;
    //qDebug()<<maxTOF<<numIterations;

    numObj=1;
    OPT_Transfer.InitializeOptimizer();
    qDebug()<<"fase 2 transfer";

    initMODE=0;
    leg1=true;
    OPT_Transfer.RunOptimizer(1);

    qDebug()<<"best parameters"<<n<<n*smaInit<<raanReq<<inclinationReq<<jdMission;
    qDebug()<<"performance:"<<"DeltaV"<<BestTotalDeltaV<<"TOF"<<sta::secsToDays((timefinTransfer)/(2*sta::Pi())*(secondBody->body()->orbitalPeriod()*24*3600));
    qDebug()<<"final position"<<patchingPoint_rotating_x<<patchingPoint_rotating_y<<patchingPoint_rotating_z<<patchingPoint_rotating_Vx<<patchingPoint_rotating_Vy<<patchingPoint_rotating_Vz;

    trueAno_Leg1=trueAno; inclinationInit_Leg1=inclinationInit; eInit_Leg1=eInit; trueAnomalyInit_Leg1=trueAnomalyInit; smaInit_Leg1=smaInit; argOfPeriapsisInit_Leg1=argOfPeriapsisInit; raanInit_Leg1=raanInit; jdMission_Leg1=jdMission;
    raanReq_Leg1=raanReq; inclinationReq_Leg1=inclinationReq; n_Leg1=n;
    patchingPoint_rotating_x_Leg1=patchingPoint_rotating_x; patchingPoint_rotating_y_Leg1=patchingPoint_rotating_y; patchingPoint_rotating_z_Leg1=patchingPoint_rotating_z; patchingPoint_rotating_Vx_Leg1=patchingPoint_rotating_Vx; patchingPoint_rotating_Vy_Leg1=patchingPoint_rotating_Vy; patchingPoint_rotating_Vz_Leg1=patchingPoint_rotating_Vz;

    extern int System;
    firstBody=getBody1 (System); secondBody=getBody2 (System);
    Grav_Param=getGravParam_user (firstBody->body()->mu(),secondBody->body()->mu());
    bodies_distance=secondBody->body()->distance();
    */

}


double trueAno_Leg2, inclinationInit_Leg2, eInit_Leg2, trueAnomalyInit_Leg2, smaInit_Leg2, argOfPeriapsisInit_Leg2, raanInit_Leg2, jdMission_Leg2;
double raanReq_Leg2, inclinationReq_Leg2, n_Leg2; bool leg2=false;
double patchingPoint_rotating_x_Leg2, patchingPoint_rotating_y_Leg2, patchingPoint_rotating_z_Leg2, patchingPoint_rotating_Vx_Leg2, patchingPoint_rotating_Vy_Leg2, patchingPoint_rotating_Vz_Leg2;
HaloOrbit halo_Leg2;

void LagrangianAdvancedDialog::run_EarthMoonTransferLeg2()
{
#if OLDSCENARIO
    const StaBody* moon = STA_SOLAR_SYSTEM->lookup(STA_MOON);
    const StaBody* earth = STA_SOLAR_SYSTEM->earth();
    firstBody->setBody(earth); secondBody->setBody(moon);
    Grav_Param=getGravParam_user (firstBody->body()->mu(),secondBody->body()->mu());
    bodies_distance=secondBody->body()->distance();
#endif
    /*
    ScenarioInitialStatePosition*  parkingOrbitsave = threebodyTransfer->parkingOrbit()->pOrbit();
    ScenarioInitialStatePosition* haloOrbitsave = threebodyTransfer->haloOrbit()->hOrbit();

    stateVector_HaloTransfer = new ScenarioStateVector();
    stateVector_HaloTransfer->setPosition(Vector3d(HaloXpositionlineEdit_4->text().toDouble(),
                                           0,
                                           HaloZpositionlineEdit_4->text().toDouble()));
    stateVector_HaloTransfer->setVelocity(Vector3d(0,
                                           HaloYvelocitylineEdit_4->text().toDouble(),
                                           0));
    haloOrbitsave->setInitialState(stateVector_HaloTransfer);

    elementTransfer = new ScenarioKeplerianElements();
    smaInit = elementTransfer->m_semimajorAxis       = semimajorAxisEdit_4->text().toDouble();
    eInit = elementTransfer->m_eccentricity        = eccentricityEdit_4->text().toDouble();
    inclinationInit = elementTransfer->m_inclination         = inclinationEdit_4->text().toDouble();
    raanInit = elementTransfer->m_raan                = raanEdit_4->text().toDouble();
    argOfPeriapsisInit = elementTransfer->m_argumentOfPeriapsis = argOfPeriapsisEdit_4->text().toDouble();
    trueAnomalyInit = elementTransfer->m_trueAnomaly         = trueAnomalyEdit_4->text().toDouble();

    parkingOrbitsave->setInitialState(elementTransfer);

    threebodyTransfer->parkingOrbit()->setInitialStatePosition(parkingOrbitsave);
    threebodyTransfer->haloOrbit()->setInitialStatePosition(haloOrbitsave);

    initialPosition = threebodyTransfer->parkingOrbit()->pOrbit()->initialState()->computeStateVector(moon);
    //qDebug()<<"initialX"<<initialPosition.position.x()<<initialPosition.position.y()<<initialPosition.position.z()<<initialPosition.velocity.x()<<initialPosition.velocity.y()<<initialPosition.velocity.z();

    twoBodyMu=secondBody->body()->mu();
    transformationMode=4;

    double startTime=0; ///////////////////TO BE FIXED

    //qDebug()<<"coordinates transformation"<<transformationMode;
    initialPositionRotating=cartesianTOrotating(transformationMode, firstBody->body(), secondBody->body(), initialPosition, startTime);

        //qDebug()<<"initialX"<<initialPositionRotating.position.x()<<initialPositionRotating.position.y()<<initialPositionRotating.position.z()<<initialPositionRotating.velocity.x()<<initialPositionRotating.velocity.y()<<initialPositionRotating.velocity.z();

        haloTransfer.trajectory.init_data.State.velocity.x() = haloTransfer.trajectory.init_data.State.velocity.z() = haloTransfer.trajectory.init_data.State.position.y() = 0;
        haloTransfer.trajectory.init_data.State.position.x() = stateVector_HaloTransfer->position().x();
        haloTransfer.trajectory.init_data.State.position.z() = stateVector_HaloTransfer->position().z();
        haloTransfer.trajectory.init_data.State.velocity.y() = stateVector_HaloTransfer->velocity().y();

        //qDebug()<<haloTransfer.trajectory.init_data.State.velocity.x()<<haloTransfer.trajectory.init_data.State.velocity.y();
        //qDebug()<<2<<initialPositionRotating.position.x();
        haloTransfer.manifolds.epsilon=EpsilondoubleSpinBox_4->text().toDouble();
        //qDebug()<<haloTransfer.manifolds.epsilon;

        haloTransfer.lpointsComputation();
        if (haloTransfer.trajectory.init_data.State.position.x()>(1-Grav_Param)) //from 2nd body to L2
        {
            haloTransfer.trajectory.Lpoint_Xposition=haloTransfer.L2_xposition*bodies_distance;
            haloTransfer.trajectory.Lpoint_selected=2;
            haloTransfer.manifolds.kind=4;
        }
        else if (haloTransfer.trajectory.init_data.State.position.x()<(1-Grav_Param) && initialPositionRotating.position.x()<0) //from 1st body to L1
        {
            haloTransfer.trajectory.Lpoint_Xposition=haloTransfer.L1_xposition*bodies_distance;
            haloTransfer.trajectory.Lpoint_selected=1;
            haloTransfer.manifolds.kind=4;
        }
        else if (haloTransfer.trajectory.init_data.State.position.x()<(1-Grav_Param) && initialPositionRotating.position.x()>0) //from 2nd body to L1
        {
            haloTransfer.trajectory.Lpoint_Xposition=haloTransfer.L1_xposition*bodies_distance;
            haloTransfer.trajectory.Lpoint_selected=1;
            haloTransfer.manifolds.kind=2;
        }
        int MAX_integration_time=10;
        haloTransfer.trajectory.Num_positions=1000;
        haloTransfer.getNumericalApprox(System,MAX_integration_time);

        //qDebug()<<"period"<<haloTransfer.trajectory.final_data.period;
        haloTransfer.getEigenvalues();
        //qDebug()<<"eigen"<<haloTransfer.manifolds.eigen1<<haloTransfer.manifolds.eigen2;

    BestTotalDeltaV=100; op=0;
    GlobalOptimizers OPT_Transfer;
    //qDebug()<<"fase 1 transfer";
    maxTOF=TOFspinBox_leg2->value();
    numIterations=50;
    //qDebug()<<maxTOF<<numIterations;

    numObj=1;
    OPT_Transfer.InitializeOptimizer();
    //qDebug()<<"fase 2 transfer";

    initMODE=0;
    leg2=true;
    OPT_Transfer.RunOptimizer(1);

    qDebug()<<"best parameters"<<n<<n*smaInit<<raanReq<<inclinationReq<<jdMission;
    qDebug()<<"performance:"<<"DeltaV"<<BestTotalDeltaV<<"TOF"<<sta::secsToDays((timefinTransfer)/(2*sta::Pi())*(secondBody->body()->orbitalPeriod()*24*3600));
    qDebug()<<"final position"<<patchingPoint_rotating_x<<patchingPoint_rotating_y<<patchingPoint_rotating_z<<patchingPoint_rotating_Vx<<patchingPoint_rotating_Vy<<patchingPoint_rotating_Vz;

    trueAno_Leg2=trueAno; inclinationInit_Leg2=inclinationInit; eInit_Leg2=eInit; trueAnomalyInit_Leg2=trueAnomalyInit; smaInit_Leg2=smaInit; argOfPeriapsisInit_Leg2=argOfPeriapsisInit; raanInit_Leg2=raanInit; jdMission_Leg2=jdMission;
    raanReq_Leg2=raanReq; inclinationReq_Leg2=inclinationReq; n_Leg2=n;
    patchingPoint_rotating_x_Leg2=patchingPoint_rotating_x; patchingPoint_rotating_y_Leg2=patchingPoint_rotating_y; patchingPoint_rotating_z_Leg2=patchingPoint_rotating_z; patchingPoint_rotating_Vx_Leg2=patchingPoint_rotating_Vx; patchingPoint_rotating_Vy_Leg2=patchingPoint_rotating_Vy; patchingPoint_rotating_Vz_Leg2=patchingPoint_rotating_Vz;

    if (leg1==true && leg2==true)
        RunButton_4->setEnabled(true);

    extern int System;
    firstBody=getBody1 (System); secondBody=getBody2 (System);
    Grav_Param=getGravParam_user (firstBody->body()->mu(),secondBody->body()->mu());
    bodies_distance=secondBody->body()->distance();
    */
}


void LagrangianAdvancedDialog::run_EarthMoonTransferPatch()
{
    const StaBody* sun = STA_SOLAR_SYSTEM->sun();
    const StaBody* moon = STA_SOLAR_SYSTEM->lookup(STA_MOON);
    const StaBody* earth = STA_SOLAR_SYSTEM->earth();
}


bool LagrangianAdvancedDialog::saveValues(ScenarioLagrangianTrajectory* lagrangianAdvanced)
{
        //ScenarioThreebodyenvironment* threebodyenvironment = lagrangianAdvanced->threebodyenvironment();
        //ScenarioSimulationParameters* parameters = lagrangianAdvanced->simulationParameters();
        //ScenarioHalo*
        //halo = lagrangianAdvanced->halo();
        //ScenarioThreebodyTransfer* threebodyTransfer = lagrangianAdvanced->threebodyTransfer();
        //qDebug()<<Lagrmode;
        if (saveValues(Halo) && saveValues(threebodyTransfer))
        {
            return true;
        }
        else
            return false;

}


bool LagrangianAdvancedDialog::saveValues(ScenarioHalo* halo)
{
#if OLDSCENARIO
    if (Lagrmode==1 || Lagrmode==2)
        coordinates_transformation=2;
    ScenarioInitialStatePosition* initialposition=halo->initialStatePosition();
    ScenarioAmplitudes* amplitudes=halo->amplitudes();
    ScenarioManifolds* manifolds=halo->manifolds();

    if (LpointSelectioncomboBox->currentIndex()==0)
        halo->setLpoint("L1");
    else
        halo->setLpoint("L2");

    halo->amplitudes()->setAx(AxAmplitudelineEdit->text().toDouble());
    if (HaloDcomboBox->currentIndex()==1)
    {
        halo->amplitudes()->setAz(AzAmplitudelineEdit->text().toDouble());
    }
    ScenarioStateVector* stateVector_halo = new ScenarioStateVector;
    stateVector_halo->setPosition(Vector3d(XpositionlineEdit->text().toDouble(), 0.0, ZpositionlineEdit->text().toDouble()));
    stateVector_halo->setVelocity(Vector3d(0.0, YvelocitylineEdit->text().toDouble(), 0.0));
    initialposition->setInitialState(stateVector_halo);
    halo->setInitialStatePosition(initialposition);
    halo->manifolds()->branch()->parameters()->setDeviation(EpsilondoubleSpinBox->text().toDouble());

   if (RstablecheckBox->isChecked()) //Right-stable manifold
    {
       halo->manifolds()->branch()->setState("true");
       if (MainRstablecheckBox->isChecked())
           halo->manifolds()->branch()->parameters()->endCondition()->setIntersection(true);
       else
           halo->manifolds()->branch()->parameters()->endCondition()->setIntersection(false);
       if (TintRstablecheckBox->isChecked())
       {
           double integrationtime=TintRstabledoubleSpinBox->value();
           halo->manifolds()->branch()->parameters()->endCondition()->setIntegrationTime(integrationtime);
           halo->manifolds()->branch()->parameters()->endCondition()->setNumPositions(nOrbRstablespinBox->value());
       }
   }
   else
       halo->manifolds()->branch()->setState("false");

   if (LstablecheckBox->isChecked()) //Left-stable manifold
    {
       halo->manifolds()->branch2()->setState("true");
       if (MainLstablecheckBox->isChecked())
           halo->manifolds()->branch2()->parameters()->endCondition()->setIntersection(true);
       else
           halo->manifolds()->branch2()->parameters()->endCondition()->setIntersection(false);
       if (TintLstablecheckBox->isChecked())
       {
           double integrationtime=TintLstabledoubleSpinBox->value();
           halo->manifolds()->branch2()->parameters()->endCondition()->setIntegrationTime(integrationtime);
           halo->manifolds()->branch2()->parameters()->endCondition()->setNumPositions(nOrbLstablespinBox->value());
       }
   }
   else
       halo->manifolds()->branch2()->setState("false");

   if (RunstablecheckBox->isChecked()) //Right-unstable manifold
   {
       halo->manifolds()->branch3()->setState("true");
       if (MainRunstablecheckBox->isChecked())
           halo->manifolds()->branch3()->parameters()->endCondition()->setIntersection(true);
       else
           halo->manifolds()->branch3()->parameters()->endCondition()->setIntersection(false);
       if (TintRunstablecheckBox->isChecked())
       {
           double integrationtime=TintRunstabledoubleSpinBox->value();
           halo->manifolds()->branch3()->parameters()->endCondition()->setIntegrationTime(integrationtime);
           halo->manifolds()->branch3()->parameters()->endCondition()->setNumPositions(nOrbRunstablespinBox->value());
       }
   }
   else
       halo->manifolds()->branch3()->setState("false");

   if (LunstablecheckBox->isChecked()) //Left-unstable manifold
    {
       halo->manifolds()->branch4()->setState("true");
       if (MainLunstablecheckBox->isChecked())
           halo->manifolds()->branch4()->parameters()->endCondition()->setIntersection(true);
       else
           halo->manifolds()->branch4()->parameters()->endCondition()->setIntersection(false);
       if (TintLunstablecheckBox->isChecked())
       {
           double integrationtime=TintLunstabledoubleSpinBox->value();
           halo->manifolds()->branch4()->parameters()->endCondition()->setIntegrationTime(integrationtime);
           halo->manifolds()->branch4()->parameters()->endCondition()->setNumPositions(nOrbLunstablespinBox->value());
       }
    }
   else
   {
       halo->manifolds()->branch4()->setState("false");
   }
#endif
   return true;
}


bool LagrangianAdvancedDialog::saveValues(ScenarioThreebodyTransfer* threebodyTransfer)
{
#if OLDSCENARIO
    threebodyTransfer->setStartTime(sta::JdToMjd(sta::CalendarToJd(dateTimeEdit->dateTime())));

    //ScenarioAbstractInitialState* parkingOrbit=threebodyTransfer->parkingOrbit()->pOrbit()->initialState();
    //ScenarioAbstractInitialState* haloOrbit=threebodyTransfer->haloOrbit()->hOrbit()->initialState();
    ScenarioInitialStatePosition*  parkingOrbitsave = threebodyTransfer->parkingOrbit()->pOrbit();
    ScenarioInitialStatePosition* haloOrbitsave = threebodyTransfer->haloOrbit()->hOrbit();

    ScenarioStateVector* stateVector_Halosave = new ScenarioStateVector();
    stateVector_Halosave->setPosition(Vector3d(HaloXpositionlineEdit_2->text().toDouble(),
                                           0,
                                           HaloZpositionlineEdit_2->text().toDouble()));
    stateVector_Halosave->setVelocity(Vector3d(0,
                                           HaloYvelocitylineEdit_2->text().toDouble(),
                                           0));
    haloOrbitsave->setInitialState(stateVector_Halosave);

#if 0
    if (firstBody->body()->id()==10) //the Sun is the 1st body
        threebodyTransfer->parkingOrbit()->pOrbit()->setCentralsystemBody(secondBody);
    else if (CentralBodyComboBox_2->currentIndex()==0)
        threebodyTransfer->parkingOrbit()->pOrbit()->setCentralsystemBody(firstBody);
    else
        threebodyTransfer->parkingOrbit()->pOrbit()->setCentralsystemBody(secondBody);
#endif

    ScenarioKeplerianElements* elementsave = new ScenarioKeplerianElements();
    elementsave->m_semimajorAxis       = semimajorAxisEdit_2->text().toDouble();
    elementsave->m_eccentricity        = eccentricityEdit_2->text().toDouble();
    elementsave->m_inclination         = inclinationEdit_2->text().toDouble();
    elementsave->m_raan                = raanEdit_2->text().toDouble();
    elementsave->m_argumentOfPeriapsis = argOfPeriapsisEdit_2->text().toDouble();
    elementsave->m_trueAnomaly         = trueAnomalyEdit_2->text().toDouble();
    parkingOrbitsave->setInitialState(elementsave);

    threebodyTransfer->parkingOrbit()->setInitialStatePosition(parkingOrbitsave);
    threebodyTransfer->haloOrbit()->setInitialStatePosition(haloOrbitsave);
#endif
    return true;
}
