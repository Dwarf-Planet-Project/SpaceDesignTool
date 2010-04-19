
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

 ------ Copyright (C) 2009 European Space Agency (space.trajectory.analysis AT gmail.com) ----
 ------------------ Author: Valentino Zuccarelli  -------------------------------------------------
 ------------------ E-mail: (Valentino.Zuccarelli@gmail.com) ----------------------------
 */

#include "lagrangian.h"
#include "lagrangianAdvanced.h"
#include "Main/scenariotree.h"
#include "Scenario/scenario.h"
#include "Astro-Core/date.h"
#include "Astro-Core/stacoordsys.h"
#include "Astro-Core/stamath.h"
#include "Scenario/scenario.h"
#include "Astro-Core/threebodyParametersComputation.h"
#include "halorbitcomputation.h"
#include "Astro-Core/stamath.h"
#include <QDebug>
#include <QString>
#include <QFile>
#include <QTextStream>
#include <QMessageBox>

using namespace Eigen;

double Grav_Param;
int System, coordinates_transformation;
extern int Lagrmode, numObj;
extern double raanReq, inclinationReq, n, twoBodyMu, inclinationInit, eInit, trueAnomalyInit, smaInit, argOfPeriapsisInit, raanInit, jdMission, timefinTransfer, trueAno;
extern double patchingPoint_rotating_x, patchingPoint_rotating_y, patchingPoint_rotating_z, patchingPoint_rotating_Vx, patchingPoint_rotating_Vy, patchingPoint_rotating_Vz;
extern double finalHaloRotX,finalHaloRotY,finalHaloRotZ,finalHaloRotVX,finalHaloRotVY,finalHaloRotVZ, eigen0, deviation, BestTotalDeltaV;
extern double trueAno_Leg1, inclinationInit_Leg1, eInit_Leg1, trueAnomalyInit_Leg1, smaInit_Leg1, argOfPeriapsisInit_Leg1, raanInit_Leg1, jdMission_Leg1;
extern double raanReq_Leg1, inclinationReq_Leg1, n_Leg1;
extern double patchingPoint_rotating_x_Leg1, patchingPoint_rotating_y_Leg1, patchingPoint_rotating_z_Leg1, patchingPoint_rotating_Vx_Leg1, patchingPoint_rotating_Vy_Leg1, patchingPoint_rotating_Vz_Leg1;
extern double trueAno_Leg2, inclinationInit_Leg2, eInit_Leg2, trueAnomalyInit_Leg2, smaInit_Leg2, argOfPeriapsisInit_Leg2, raanInit_Leg2, jdMission_Leg2;
extern double raanReq_Leg2, inclinationReq_Leg2, n_Leg2;
extern double patchingPoint_rotating_x_Leg2, patchingPoint_rotating_y_Leg2, patchingPoint_rotating_z_Leg2, patchingPoint_rotating_Vx_Leg2, patchingPoint_rotating_Vy_Leg2, patchingPoint_rotating_Vz_Leg2;
extern QList <sta::StateVector> patchingPointsLeg1, patchingPointsLeg2, initialConditionsLeg1, initialConditionsLeg2, finalConditionsLeg2, lastLegPositions;
extern QList <double> patchingPointsJDLeg1, patchingPointsJDLeg2, epsilon1, DVpatch, finalT, leg1PatchPos, epPatch, Distancepatch, lastLegEpoch;
extern double bestPat, timeinHalo1, timefinHalo2, bestDistance, timefinTransfer_Leg1, timefinTransfer_Leg2;
extern int flagHalo;
extern double EM_Az;

LagrangianDialog::LagrangianDialog(ScenarioTree* parent) :
    QDialog(parent)
{
    setupUi(this);
    //qDebug()<<"starting Lagrangian Mode"<<Lagrmode;
    // Set up the combo boxes
    CoordSystemComboBox->addItem(tr("Co-Rotating normalized"), (int) sta::COORDSYS_ROT_NORM);
    CoordSystemComboBox->addItem(tr("Co-Rotating"), (int) sta::COORDSYS_ROT);
    CoordSystemComboBox->addItem(tr("Inertial J2000"), (int) sta::COORDSYS_BODYFIXED);
   // CoordSystemComboBox->addItem(tr("Ecliptic (J2000)"), (int) sta::COORDSYS_ECLIPTIC_J2000);

    QDoubleValidator* doubleValidator = new QDoubleValidator(this);
    QDoubleValidator* angleValidator = new QDoubleValidator(this);
    angleValidator->setBottom(0.0);
    angleValidator->setTop(360.0);
    QDoubleValidator* positiveDoubleValidator = new QDoubleValidator(this);
    positiveDoubleValidator->setBottom(0.0);
    QDoubleValidator* zeroToOneValidator = new QDoubleValidator(this);
    zeroToOneValidator->setBottom(0.0);
    zeroToOneValidator->setTop(0.9999);

    TimeStepLineEdit->setValidator(positiveDoubleValidator);
    positionXEdit->setValidator(doubleValidator);
    positionYEdit->setValidator(doubleValidator);
    positionZEdit->setValidator(doubleValidator);
    velocityXEdit->setValidator(doubleValidator);
    velocityYEdit->setValidator(doubleValidator);
    velocityZEdit->setValidator(doubleValidator);
    velocityXEdit_2->setValidator(doubleValidator);
    velocityYEdit_2->setValidator(doubleValidator);
    velocityZEdit_2->setValidator(doubleValidator);

    semimajorAxisEdit->setValidator(positiveDoubleValidator);
    eccentricityEdit->setValidator(zeroToOneValidator);
    inclinationEdit->setValidator(angleValidator);
    raanEdit->setValidator(angleValidator);
    argOfPeriapsisEdit->setValidator(angleValidator);
    trueAnomalyEdit->setValidator(angleValidator);

    connect(BodyListWidget, SIGNAL(itemDoubleClicked(QListWidgetItem*)), this, SLOT(addPerturbingPlanet(QListWidgetItem*)));
    connect(SystemComboBox, SIGNAL(activated(int)), this, SLOT(threebody_system()));
    connect(InitialStateComboBox, SIGNAL (currentIndexChanged(int)), this, SLOT(update_coordinate_systems()));
    connect(CoordSystemComboBox, SIGNAL (currentIndexChanged(int)), this, SLOT(update_units()));
    connect(AdvancedpushButton, SIGNAL (pressed()), this, SLOT(lagrangian_advanced()));
#if 0
    connect(HaloDcomboBox, SIGNAL(currentIndexChanged(int)), this, SLOT (research_init_cond()));
    connect(initialConditionscheckBox, SIGNAL(clicked()), this, SLOT (research_init_cond()));
    connect(initialConditionComputationspushButton, SIGNAL(clicked()),this,SLOT(halo_initial_conditions()));
    connect(haloOrbitComputationpushButton, SIGNAL(clicked()), this, SLOT (halo_orbit_computation()));
    connect(ManifoldsComputationpushButton, SIGNAL(clicked()), this, SLOT (manifolds_computation()));
    connect(ModeSelectionWidget, SIGNAL(currentChanged(int)), this, SLOT (mode_selection()));
    connect(tabWidget, SIGNAL(currentChanged(int)), this, SLOT (mode_selection()));
    connect(haloInitialConditioncheckBox, SIGNAL(clicked()), this, SLOT (halo_orbit_initial_conditions_mode3()));
    connect(RunButton, SIGNAL (pressed()), this, SLOT(run_transfer_optimization()));
#endif
}

LagrangianDialog::~LagrangianDialog()
{
}


//
void LagrangianDialog::lagrangian_advanced()
{
    ScenarioLagrangianTrajectory* lagrangianAdvanced;

    LagrangianAdvancedDialog editAdvancedDialog;


   editAdvancedDialog.loadValues(lagrangianAdvanced);
   //editAdvancedDialog.exec();
   if (editAdvancedDialog.exec() == QDialog::Accepted)
   {
       editAdvancedDialog.saveValues(lagrangianAdvanced);
   }

}


void LagrangianDialog::update_coordinate_systems()
{
     if (InitialStateComboBox->currentIndex()==0)
     {
         CoordSystemComboBox->clear();
         CoordSystemComboBox->addItem(tr("Inertial J2000"), (int) sta::COORDSYS_BODYFIXED);
     }
     else if (InitialStateComboBox->currentIndex()==1)
     {
         CoordSystemComboBox->clear();
         CoordSystemComboBox->addItem(tr("Co-Rotating normalized"), (int) sta::COORDSYS_ROT_NORM);
         CoordSystemComboBox->addItem(tr("Co-Rotating"), (int) sta::COORDSYS_ROT);
         CoordSystemComboBox->addItem(tr("Inertial J2000"), (int) sta::COORDSYS_EME_J2000);
       //  CoordSystemComboBox->addItem(tr("Ecliptic (J2000)"), (int) sta::COORDSYS_ECLIPTIC_J2000);
     }
     if (InitialStateComboBox->currentIndex()==0 && (CoordSystemComboBox->currentIndex()==1 || CentralBodyComboBox->currentIndex()==0))
        CentralBodyComboBox->setEnabled(false);
}


 void LagrangianDialog::update_units()
 {
     if (CoordSystemComboBox->currentIndex()==0 && InitialStateComboBox->currentIndex()==1)
     {
         //PositionGroupBox->setTitle("Position");
         //VelocityGroupBox->setTitle("Velocity");
         label_10->setText("X-DeltaV:");
         label_11->setText("Y-DeltaV:");
         label_12->setText("Z-DeltaV:");
         a->setText("X:");
         e->setText("Y:");
         i->setText("Z:");
         raan->setText("Vx:");
         omega->setText("Vy:");
         nu->setText("Vz:");
         CentralBodyComboBox->setEnabled(false);
         CentralBodyLabel->setEnabled(false);
         return;
     }
     else
     {
         //PositionGroupBox->setTitle("Position [km]");
         //VelocityGroupBox->setTitle("Velocity [Km/s]");
         label_10->setText("X-DeltaV (km/s):");
         label_11->setText("Y-DeltaV (km/s):");
         label_12->setText("Z-DeltaV (km/s):");
         a->setText("X [km]:");
         e->setText("Y [km]:");
         i->setText("Z [km]:");
         raan->setText("Vx [km/s]:");
         omega->setText("Vy [km/s]:");
         nu->setText("Vz [km/s]:");
         CentralBodyComboBox->setEnabled(true);
         CentralBodyLabel->setEnabled(true);
     }

     if (CoordSystemComboBox->currentIndex()==1 && InitialStateComboBox->currentIndex()==1)
     {
         CentralBodyComboBox->setEnabled(false);
         CentralBodyLabel->setEnabled(false);
         return;
     }
     else
     {
         CentralBodyComboBox->setEnabled(true);
         CentralBodyLabel->setEnabled(true);
         return;
     }
 }


#if OLDSCENARIO
ScenarioBody* firstBody;
ScenarioBody* secondBody;
#endif
double bodies_distance, period_days, velocity;

void LagrangianDialog::threebody_system()	//system selection
{
#if OLDSCENARIO
    if (SystemComboBox->currentIndex()==0 || SystemComboBox->currentIndex()==12)
        return;
    firstBody=getBody1 (SystemComboBox->currentIndex()); secondBody=getBody2 (SystemComboBox->currentIndex());
    firstMIlineEdit->setText(QString::number(firstBody->body()->mu(), 'f', 2));
    secondMIlineEdit->setText(QString::number(secondBody->body()->mu(), 'f', 2));
    CentralBodyComboBox->clear();
    //CentralBodyComboBox_2->clear();
    bodies_distance=secondBody->body()->distance();
    velocity=secondBody->body()->linearV();

    if (firstBody->body()->name()!="Sun")
    {
        CentralBodyComboBox->clear();
        CentralBodyComboBox->setEnabled(true);
        CentralBodyComboBox->addItem(tr((firstBody->body()->name()).toLatin1()), firstBody->body()->id());
        //CentralBodyComboBox_2->clear();
        //CentralBodyComboBox_2->setEnabled(true);
        //CentralBodyComboBox_2->addItem(tr((firstBody->body()->name()).toLatin1()), firstBody->body()->id());
    }
    CentralBodyComboBox->addItem(tr((secondBody->body()->name()).toLatin1()),secondBody->body()->id());
    //CentralBodyComboBox_2->addItem(tr((secondBody->body()->name()).toLatin1()),secondBody->body()->id());

    Grav_Param=getGravParam_user (firstBody->body()->mu(),secondBody->body()->mu());
    //qDebug()<<Grav_Param;
    bodies_distance=secondBody->body()->distance();
    period_days=secondBody->body()->orbitalPeriod();
    threeBodyMIlineEdit->setText(QString::number(Grav_Param, 'f',15));
    primariesDistancelineEdit->setText(QString::number(secondBody->body()->distance(), 'f', 0));
    //qDebug()<<SystemComboBox->currentIndex();
    System=SystemComboBox->currentIndex();
    SystemComboBox_2->setText(SystemComboBox->currentText());
    primariesDistancelineEdit_2->setText(primariesDistancelineEdit->text());
#endif
}


//extern HaloOrbit halo;
#if 0
void LagrangianDialog::research_init_cond()

{
    if (initialConditionscheckBox->isChecked())
    {
        initialConditionwidget->setEnabled(true);

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
        initialConditionwidget->setEnabled(false);
}


//function to compute the initial conditions for the Halo orbit (mode 2!)
void LagrangianDialog::halo_initial_conditions()
{
    //il pulsante "compute Halo orbit" deve essere disabilitato all'avvio!!! cambialo!
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
}


//function to compute the Halo orbit (mode 2!)
void LagrangianDialog::halo_orbit_computation()
{
    halo.trajectory.error=halo.manifolds.error=0;
    if (XpositionlineEdit->text().toDouble()==0 || YvelocitylineEdit->text().toDouble()==0)
    {
        QMessageBox::warning(NULL,
                              QObject::tr("Halo orbit not computed"),
                              QObject::tr("The initial guess are wrong or zero."));
        haloOrbitComputationpushButton->setEnabled(false);
        return;
    }

    HaloOrbitwidget->setEnabled(true);
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

    halo.getNumericalApprox (SystemComboBox->currentIndex(), MAX_integration_time);     //Halo orbit computation

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
}


//function to compute manifolds (stil mode 2!!!)
void LagrangianDialog::manifolds_computation()
{
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
                              QObject::tr("The deviation chosed is too small or the halo orbit is too stable to get a such intersection."));
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
                              QObject::tr("The deviation chosed is too small or the halo orbit is too stable to get a such intersection."));
                return;
            }
        }

        if (LstablecheckBox->isChecked())  //Left Stable branch
        {
            first=first+1;
            if (MainLstablecheckBox->isChecked()) //stop condition: intersection with a main body
            {
                integration_time=0;
                if (halo.trajectory.Lpoint_selected==1 && (SystemComboBox->currentIndex()!=9 && SystemComboBox->currentIndex()!=10 && SystemComboBox->currentIndex()!=11 && SystemComboBox->currentIndex()!=12))	//intersection with the sun makes no sense, too far away
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
                              QObject::tr("The deviation chosed is too small or the halo orbit is too stable to get a such intersection."));
                return;
            }
        }

        if (LunstablecheckBox->isChecked()) 	//Left Unstable branch
        {
            first=first+1;
            if (MainLunstablecheckBox->isChecked()) //stop condition: intersection with a main body
            {
                integration_time=0;
                if (halo.trajectory.Lpoint_selected==1 && (SystemComboBox->currentIndex()!=9 && SystemComboBox->currentIndex()!=10 && SystemComboBox->currentIndex()!=11 && SystemComboBox->currentIndex()!=12))	//intersection with the sun makes no sense, too far away
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
                              QObject::tr("The deviation chosed is too small or the halo orbit is too stable to get a such intersection."));
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
}

void LagrangianDialog::halo_orbit_initial_conditions_mode3()
{
    if (haloInitialConditioncheckBox->isChecked())
    {
        HaloXpositionlineEdit_2->setText(HaloXpositionlineEdit->text());
        HaloZpositionlineEdit_2->setText(HaloZpositionlineEdit->text());
        HaloYvelocitylineEdit_2->setText(HaloYvelocitylineEdit->text());
    }
}


void LagrangianDialog::run_transfer_optimization()
{

}
#endif

ScenarioThreebodyenvironment* threebodyenvironment;// = lagrangian->threebodyenvironment();
ScenarioSimulationParameters* parameters;// = lagrangian->simulationParameters();
ScenarioHalo* Halo;
ScenarioThreebodyTransfer* threebodyTransfer;

bool LagrangianDialog::loadValues(ScenarioLagrangianTrajectory* lagrangian)

{
#if OLDSCENARIO
    Halo = lagrangian->halo();
    threebodyTransfer = lagrangian->threebodyTransfer();
    Lagrmode=0;

    threebodyenvironment = lagrangian->threebodyenvironment();
    parameters = lagrangian->simulationParameters();

    loadValues(threebodyenvironment); loadValues(parameters);

#if 0
        if (Lagrmode==0)
            ModeSelectionWidget->setCurrentIndex(0);
        else if (Lagrmode==1)
        {
            ModeSelectionWidget->setCurrentIndex(1);
            tabWidget->setCurrentIndex(0);
        }
        else if (Lagrmode==2)
        {
            ModeSelectionWidget->setCurrentIndex(1);
            tabWidget->setCurrentIndex(1);
        }
#endif

        if(loadValues(threebodyenvironment) &&  loadValues(parameters))// &&  loadValues(halo) && loadValues(threebodyTransfer))
        {
            return true;
        }
        else
#endif
            return false;
}


bool LagrangianDialog::loadValues(ScenarioThreebodyenvironment* threebodyenvironment)
{
#ifdef OLDSCENARIO
        firstBody = threebodyenvironment->firstBody();
        secondBody = threebodyenvironment->secondBody();
        bodies_distance=secondBody->body()->distance();
        velocity=secondBody->body()->linearV();
        Grav_Param=getGravParam_user (firstBody->body()->mu(),secondBody->body()->mu());
        period_days=secondBody->body()->orbitalPeriod();

#if 0
        ScenarioGravityModel* gravmodel = firstBody->gravityModel();
        if(gravmodel->zonalCount()==2)
            {
            J2firstradioButton->setChecked(true);
            }
        gravmodel = secondBody->gravityModel();
        if(gravmodel->zonalCount()==2)
            {
            J2secondradioButton->setChecked(true);
            }
#endif
        bool solarPressureEnabled = threebodyenvironment->solarPressureEnabled();
        if (solarPressureEnabled==true)
            SolarPressureRadioButton->setChecked(true);

        QList<ScenarioBody*> perturbingBodies = threebodyenvironment->perturbingBodyList();
        if (!perturbingBodies.isEmpty())
            {PertBodyRadioButton->setChecked(true);
            BodyListWidget->setEnabled(true);  PertBodyListWidget->setEnabled(true);
            AddPushButton->setEnabled(true);   RemovePushButton->setEnabled(true);
            }

        PertBodyListWidget->clear();
        while (!perturbingBodies.isEmpty())
            {
            QString bodytext = perturbingBodies.takeFirst()->body()->name();
            //qDebug()<<bodytext;
            PertBodyListWidget->addItem(bodytext);
            }

            System=getSystem (firstBody->body()->id(), secondBody->body()->id());
            SystemComboBox->setCurrentIndex(System);
            SystemComboBox_2->setText(SystemComboBox->currentText());
            firstMIlineEdit->setText(QString::number(firstBody->body()->mu(), 'f', 2));
            secondMIlineEdit->setText(QString::number(secondBody->body()->mu(), 'f', 2));
            primariesDistancelineEdit->setText(QString::number(secondBody->body()->distance(), 'f', 0));
            primariesDistancelineEdit_2->setText(primariesDistancelineEdit->text());
            bodies_distance=secondBody->body()->distance();
            Grav_Param=getGravParam_user (firstBody->body()->mu(),secondBody->body()->mu());
            period_days=secondBody->body()->orbitalPeriod();
            threeBodyMIlineEdit->setText(QString::number(Grav_Param, 'f',15));

        CentralBodyComboBox->clear();
        //CentralBodyComboBox_2->clear();
        if (firstBody->body()->name()!="Sun")
        {
            CentralBodyComboBox->setEnabled(true);
            CentralBodyComboBox->addItem(tr((firstBody->body()->name()).toLatin1()), firstBody->body()->id());
            //CentralBodyComboBox_2->setEnabled(true);
            //CentralBodyComboBox_2->addItem(tr((firstBody->body()->name()).toLatin1()), firstBody->body()->id());
        }

        CentralBodyComboBox->addItem(tr((secondBody->body()->name()).toLatin1()),secondBody->body()->id());
        //CentralBodyComboBox_2->addItem(tr((secondBody->body()->name()).toLatin1()),secondBody->body()->id());
#endif // OLDSCENARIO
return true;
}



bool LagrangianDialog::loadValues(ScenarioSimulationParameters* parameters)
{
#ifdef OLDSCENARIO
    ScenarioExtendedTimeline* timeline = parameters->timeline();
    ScenarioInitialStatePosition* initialStatePos = parameters->initialStatePosition();

    QDateTime startTime = sta::JdToCalendar(sta::MjdToJd(timeline->startTime()));
    QDateTime endTime = sta::JdToCalendar(sta::MjdToJd(timeline->endTime()));
    QDateTime fire = sta::JdToCalendar(sta::MjdToJd(parameters->deltav()->Time()));

    if (timeline)
    {
                dateTimeEdit->setDateTime(startTime);
                dateTimeEdit_2->setDateTime(endTime);
                dateTimeEdit_3->setDateTime(fire);
                TimeStepLineEdit->setText(QString::number(timeline->timeStep()));
        }
        else
    {
        return false;
    }


    ScenarioDeltav* deltav=parameters->deltav();
    double epoch_day = deltav->Time();
    QTime epoch;
    int days = floor(epoch_day);
    double hoursdecimals = floor((epoch_day-days)*24.0);
    double minutes=((epoch_day-days-hoursdecimals/24.0)*24.0*60.0);
    if (ceil(minutes)-minutes<0.0001)
        minutes=ceil(minutes);
    double seconds=((epoch_day-floor(epoch_day)-hoursdecimals/24.0-minutes/24.0/60.0)*24.0*3600.0);
    double leap_seconds= (ceil(seconds)-seconds)*100;

    if (initialStatePos)
    {
                // Set the coordinate system combo box value
        for (int i = 0; i < CoordSystemComboBox->count(); i++)
        {
            if (CoordSystemComboBox->itemData(i) == initialStatePos->coordinateSystem().type())
            {
                CoordSystemComboBox->setCurrentIndex(i);
                break;
            }
        }

        ScenarioAbstractInitialState* initialState = initialStatePos->initialState();
        ScenarioKeplerianElements* elements = dynamic_cast<ScenarioKeplerianElements*>(initialState);
        ScenarioStateVector* stateVector = dynamic_cast<ScenarioStateVector*>(initialState);

        Q_ASSERT(elements || stateVector);
        if (elements != NULL)
        {
            CoordSystemComboBox->clear();
            CoordSystemComboBox->addItem(tr("Inertial J2000"), (int) sta::COORDSYS_BODYFIXED);
            InitialStateComboBox->setCurrentIndex(0);
            CentralBodyComboBox->setDisabled(true);
            InitialStateStackedWidget->setCurrentWidget(keplerianPage);
            semimajorAxisEdit->setText(QString::number(elements->m_semimajorAxis));
            eccentricityEdit->setText(QString::number(elements->m_eccentricity));
            inclinationEdit->setText(QString::number(elements->m_inclination));
            raanEdit->setText(QString::number(elements->m_raan));
            argOfPeriapsisEdit->setText(QString::number(elements->m_argumentOfPeriapsis));
            trueAnomalyEdit->setText(QString::number(elements->m_trueAnomaly));
        }
        else if (stateVector != NULL)
        {
            CoordSystemComboBox->setCurrentIndex(1);
            if (parameters->initialStatePosition()->coordinateSystem().name()=="INERTIAL J2000")
                CoordSystemComboBox->setCurrentIndex(2);
            else if (parameters->initialStatePosition()->coordinateSystem().name()=="ROTATING")
                CoordSystemComboBox->setCurrentIndex(1);
            else if (parameters->initialStatePosition()->coordinateSystem().name()=="ROTATING NORMALIZED")
                CoordSystemComboBox->setCurrentIndex(0);

            InitialStateComboBox->setCurrentIndex(1);
            InitialStateStackedWidget->setCurrentWidget(stateVectorPage);
//TODO: check if it works
            positionXEdit->setText(QString::number(stateVector->position().x()));
            positionYEdit->setText(QString::number(stateVector->position().y()));
            positionZEdit->setText(QString::number(stateVector->position().z()));
            velocityXEdit->setText(QString::number(stateVector->velocity().x()));
            velocityYEdit->setText(QString::number(stateVector->velocity().y()));
            velocityZEdit->setText(QString::number(stateVector->velocity().z()));
        }
        else
        {
                // Unknown initial state type
            return false;
        }
#if 0
        if (CoordSystemComboBox->currentIndex()==0 || CoordSystemComboBox->currentIndex()==1)
        {
            CentralBodyComboBox->setEnabled(false);
            CentralBodyLabel->setEnabled(false);
        }
#endif
    }
        else
           return false;

        if (parameters->initialStatePosition()->centralsystemBody()->body()->name()==secondBody->body()->name() && CentralBodyComboBox->count()==2)
            CentralBodyComboBox->setCurrentIndex(1);
        else
            CentralBodyComboBox->setCurrentIndex(0);
        epoch.setHMS(hoursdecimals,minutes,seconds);
    if (deltav)
    {
        //if (InitialStateComboBox->currentIndex()==1 && CoordSystemComboBox->currentIndex()==0)
            velocity=1;
        //dateTimeEdit_3->dateTimeEdit_3(epoch);
        velocityXEdit_2->setText(QString::number(deltav->Xvelocity()*velocity));
        velocityYEdit_2->setText(QString::number(deltav->Yvelocity()*velocity));
        velocityZEdit_2->setText(QString::number(deltav->Zvelocity()*velocity));
        velocity=secondBody->body()->linearV();
    }
    else
    {
        return false;
    }
#endif
    return true;
}

#if 0

bool LagrangianDialog::loadValues(ScenarioHalo* Halo)
{
        halo.trajectory.error=-1;
        ScenarioInitialStatePosition* initialposition=Halo->initialStatePosition();
        ScenarioAmplitudes* amplitudes=Halo->amplitudes();
        ScenarioManifolds* manifolds=Halo->manifolds();

        const StaBody* sun = STA_SOLAR_SYSTEM->sun();
        double JD=1;
        const StaBody* firstbody = STA_SOLAR_SYSTEM->earth();

        if (Halo->Lpoint()=="L1")
            LpointSelectioncomboBox->setCurrentIndex(0);
        else if (Halo->Lpoint()=="L2")
            LpointSelectioncomboBox->setCurrentIndex(1);

        if (Halo->amplitudes()->Ax()!=0 && Halo->amplitudes()->Az()!=0)
        {
            HaloDcomboBox->setCurrentIndex(1);
            initialConditionscheckBox->setChecked(true);
            initialConditionwidget->setEnabled(true);
            AxAmplitudelineEdit->setText(QString::number(Halo->amplitudes()->Ax()));
            AzAmplitudelineEdit->setText(QString::number(Halo->amplitudes()->Az()));
        }
        else if (Halo->amplitudes()->Ax()!=0 && Halo->amplitudes()->Az()==0)
        {
            HaloDcomboBox->setCurrentIndex(0);
            initialConditionscheckBox->setChecked(true);
            initialConditionwidget->setEnabled(true);
            AzAmplitudelineEdit->setEnabled(false); label_273->setEnabled(false);
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
        return true;
}


bool LagrangianDialog::loadValues(ScenarioThreebodyTransfer* threebodyTransfer)
{
    QDateTime startTime = sta::JdToCalendar(sta::MjdToJd(threebodyTransfer->startTime));
    dateTimeEdit_4->setDateTime(startTime);

    //qDebug()<<1;
    ScenarioAbstractInitialState* parkingOrbit=threebodyTransfer->parkingOrbit()->pOrbit()->initialState();
    ScenarioAbstractInitialState* haloOrbit=threebodyTransfer->haloOrbit()->hOrbit()->initialState();
    ScenarioKeplerianElements* elements = dynamic_cast<ScenarioKeplerianElements*>(parkingOrbit);
    ScenarioStateVector* stateVector_Halo = dynamic_cast<ScenarioStateVector*>(haloOrbit);
//qDebug()<<2;
    semimajorAxisEdit_2->setText(QString::number(elements->m_semimajorAxis));
// qDebug()<<3;
 eccentricityEdit_2->setText(QString::number(elements->m_eccentricity));
    inclinationEdit_2->setText(QString::number(elements->m_inclination));
    raanEdit_2->setText(QString::number(elements->m_raan));
    argOfPeriapsisEdit_2->setText(QString::number(elements->m_argumentOfPeriapsis));
    trueAnomalyEdit_2->setText(QString::number(elements->m_trueAnomaly));

    HaloXpositionlineEdit_2->setText(QString::number(stateVector_Halo->position().x()));
    HaloZpositionlineEdit_2->setText(QString::number(stateVector_Halo->position().z()));
    HaloYvelocitylineEdit_2->setText(QString::number(stateVector_Halo->velocity().y()));

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
    //qDebug()<<"caricato";
    return true;
}

#endif

bool LagrangianDialog::saveValues(ScenarioLagrangianTrajectory* lagrangian)
{
#ifdef OLDSCENARIO
    ScenarioThreebodyenvironment* threebodyenvironment = lagrangian->threebodyenvironment();
    ScenarioSimulationParameters* parameters = lagrangian->simulationParameters();
    Halo = lagrangian->halo();
    threebodyTransfer = lagrangian->threebodyTransfer();
    //lagrangian->setHalo(lagrangianAdvanced->halo());
    //lagrangian->setThreebodyTransfer(lagrangian->threebodyTransfer());
    //qDebug()<<Lagrmode;
    if (saveValues(threebodyenvironment) && saveValues(parameters))// && saveValues(halo) && saveValues(threebodyTransfer))
    {
        return true;
    }
    else
#endif
    {
        return false;
    }
}


bool LagrangianDialog::saveValues(ScenarioThreebodyenvironment* threebodyenvironment)
{
#ifdef OLDSCENARIO
    threebodyenvironment->setFirstBody(firstBody);
    threebodyenvironment->setSecondBody(secondBody);
    ScenarioGravityModel* gravmodel = new ScenarioGravityModel();
#if 0
    if (J2firstradioButton->isChecked())
        {gravmodel->setZonalCount(2);
        }
    else
        {gravmodel->setZonalCount(0);
        }
    firstBody->setGravityModel(gravmodel);

    ScenarioGravityModel* gravmodel2 = new ScenarioGravityModel();

    if (J2secondradioButton->isChecked())
        {gravmodel2->setZonalCount(2);
        }
    else
        {gravmodel2->setZonalCount(0);
        }
    secondBody->setGravityModel(gravmodel2);
#endif
#endif // OLDSCENARIO

    return true;
}


bool LagrangianDialog::saveValues(ScenarioSimulationParameters* parameters)
{
#ifdef OLDSCENARIO
    ScenarioExtendedTimeline* timeline = parameters->timeline();
    ScenarioInitialStatePosition*  initialState_threebody_Pos = parameters->initialStatePosition();

    timeline->setStartTime(sta::JdToMjd(sta::CalendarToJd(dateTimeEdit->dateTime())));
    timeline->setEndTime(sta::JdToMjd(sta::CalendarToJd(dateTimeEdit_2->dateTime())));
    timeline->setTimeStep(TimeStepLineEdit->text().toDouble());
    parameters->deltav()->setTime(sta::JdToMjd(sta::CalendarToJd(dateTimeEdit_3->dateTime())));
    ScenarioDeltav* deltav=parameters->deltav();

    double epoch_day;
    //QTime epoch(timeEdit->time().hour(), timeEdit->time().minute(), timeEdit->time().second());

    //epoch_day=daysSpinBox->value()+(epoch.hour()-floor(epoch.hour()/24.0))/24.0+epoch.minute()/24.0/60.0+epoch.second()/24.0/3600.0;


    if (Lagrmode>0 && firstBody->body()->id()!=10)
    {
        //qDebug()<<"no propagator!";
        coordinates_transformation=5;
        return true;
    }
        deltav->setXvelocity(velocityXEdit_2->text().toDouble());
        deltav->setYvelocity(velocityYEdit_2->text().toDouble());
        deltav->setZvelocity(velocityZEdit_2->text().toDouble());
//        deltav->setTime(epoch_day);

#if 0
        double Accu;
    if (accuracycomboBox_4->currentIndex()==0)
        Accu=1;
    else if (accuracycomboBox_4->currentIndex()==1)
        Accu=10;
    else if (accuracycomboBox_4->currentIndex()==2)
        Accu=100;
    else if (accuracycomboBox_4->currentIndex()==3)
        Accu=1000;
#endif

    int coordSysIndex = CoordSystemComboBox->currentIndex();
    initialState_threebody_Pos->setCoordinateSystem((sta::CoordinateSystemType) CoordSystemComboBox->itemData(coordSysIndex).toInt());

    ScenarioStateVector* stateVector = new ScenarioStateVector();
    //the Sun is the first body,
    //then the orbit is considered around the 2nd one

        if (InitialStateComboBox->currentIndex() == 1) //we are using StateVector as initial conditions
        {
            coordinates_transformation=0;

         //if (SystemComboBox->currentIndex()>8) //the 1st body is not the Sun, then the coordinates have to be expressed w.r.t. the 1st body in case of rotating r.f.
         //{

            if (CoordSystemComboBox->currentIndex()==0) //normalized rotating r.f.
            {
            stateVector->setPosition(Vector3d(positionXEdit->text().toDouble(),
                                              positionYEdit->text().toDouble(),
                                              positionZEdit->text().toDouble()));
            stateVector->setVelocity(Vector3d(velocityXEdit->text().toDouble(),
                                              velocityYEdit->text().toDouble(),
                                              velocityZEdit->text().toDouble()));

            deltav->setXvelocity(deltav->Xvelocity());
            deltav->setYvelocity(deltav->Yvelocity());
            deltav->setZvelocity(deltav->Zvelocity());

            initialState_threebody_Pos->setCoordinateSystem(sta::COORDSYS_ROT_NORM);
            initialState_threebody_Pos->setInitialState(stateVector);
            return true;
            }
            else if (CoordSystemComboBox->currentIndex()==1) //un-normalized rotating r.f.
            {
            stateVector->setPosition(Vector3d(positionXEdit->text().toDouble()/bodies_distance,
                                              positionYEdit->text().toDouble()/bodies_distance,
                                              positionZEdit->text().toDouble()/bodies_distance));
            stateVector->setVelocity(Vector3d(velocityXEdit->text().toDouble()/velocity,
                                              velocityYEdit->text().toDouble()/velocity,
                                              velocityZEdit->text().toDouble()/velocity));

            deltav->setXvelocity(deltav->Xvelocity());
            deltav->setYvelocity(deltav->Yvelocity());
            deltav->setZvelocity(deltav->Zvelocity());

            initialState_threebody_Pos->setCoordinateSystem(sta::COORDSYS_ROT);
            initialState_threebody_Pos->setInitialState(stateVector);
            return true;
            }
            else if (CoordSystemComboBox->currentIndex()==2) //body-fixed r.f.
            {
            stateVector->setPosition(Vector3d(positionXEdit->text().toDouble(),
                                              positionYEdit->text().toDouble(),
                                              positionZEdit->text().toDouble()));
            stateVector->setVelocity(Vector3d(velocityXEdit->text().toDouble(),
                                              velocityYEdit->text().toDouble(),
                                              velocityZEdit->text().toDouble()));
            deltav->setXvelocity(deltav->Xvelocity());
            deltav->setYvelocity(deltav->Yvelocity());
            deltav->setZvelocity(deltav->Zvelocity());

            initialState_threebody_Pos->setCoordinateSystem(sta::COORDSYS_EME_J2000);
            initialState_threebody_Pos->setInitialState(stateVector);
            if (SystemComboBox->currentIndex()<=8) //the 1st body is not the Sun, then the coordinates have to be expressed w.r.t. the 1st body
                coordinates_transformation=2;
            else if (SystemComboBox->currentIndex()>8 && CentralBodyComboBox->currentIndex()==0)
                coordinates_transformation=1;
            else if (SystemComboBox->currentIndex()>8 && CentralBodyComboBox->currentIndex()==1)
                coordinates_transformation=2;
            return true;
            }
        }
        else if (InitialStateComboBox->currentIndex() == 0)//we are using keplerian elements
        {
            ScenarioKeplerianElements* elements = new ScenarioKeplerianElements();
            elements->m_semimajorAxis       = semimajorAxisEdit->text().toDouble();
            elements->m_eccentricity        = eccentricityEdit->text().toDouble();
            elements->m_inclination         = inclinationEdit->text().toDouble();
            elements->m_raan                = raanEdit->text().toDouble();
            elements->m_argumentOfPeriapsis = argOfPeriapsisEdit->text().toDouble();
            elements->m_trueAnomaly         = trueAnomalyEdit->text().toDouble();

            deltav->setXvelocity(deltav->Xvelocity());
            deltav->setYvelocity(deltav->Yvelocity());
            deltav->setZvelocity(deltav->Zvelocity());
            initialState_threebody_Pos->setInitialState(elements);

            if (SystemComboBox->currentIndex()<=8) //the 1st body is not the Sun, then the coordinates have to be expressed w.r.t. the 2nd body
                coordinates_transformation=4;
            else if (SystemComboBox->currentIndex()>8 && CentralBodyComboBox->currentIndex()==0)
                coordinates_transformation=3;
            else if (SystemComboBox->currentIndex()>8 && CentralBodyComboBox->currentIndex()==1)
                coordinates_transformation=4;

            /*
            coordinates transformation explanation:
            0 orbit around the baricenter, rotating coordinates
            1 orbit around the 1st body, state-vector
            2 orbit around the 2nd body, state-vector
            3 orbit around the 1st body, keplerian
            4 orbit around the 2nd body, keplerian
            5 manifolds for no-Sun system
            for the last 4cases, the reference frame is Inertial J2000
            */
           // if (coordinates_transformation==4 || coordinates_transformation==2)
             //   parameters->initialStatePosition()->setCentralsystemBody(secondBody);

            return true;
        }


#if 0

    if (CentralBodyComboBox->count()==2) //the Sun is not one of the primaries
    {
        if (CentralBodyComboBox->currentIndex()==0) //the inital orbit is around the 1st body
        {
            //parameters->initialStatePosition()->setCentralsystemBody(firstBody);
            coordinates_transformation=2;
            //unless you are very close to the 2nd body (2% of the parameter: distance!)
            if ((positionXEdit->text().toDouble()>0.98 && CoordSystemComboBox->currentIndex()==0) || (positionXEdit->text().toDouble()>0.98*bodies_distance && CoordSystemComboBox->currentIndex()==1))
                coordinates_transformation=1;
        }
        else if (CentralBodyComboBox->currentIndex()==1) //the inital orbit is around the 2nd body
        {
            coordinates_transformation=1;
            //parameters->initialStatePosition()->setCentralsystemBody(secondBody);
        }
    }
#endif

    parameters->setTimeline(timeline);
    parameters->setDeltav(deltav);
    parameters->setInitialStatePosition(initialState_threebody_Pos);
#endif
    return true;
}


#if 0

bool LagrangianDialog::saveValues(ScenarioHalo* halo)
{
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
       halo->manifolds()->branch4()->setState("false");
       return true;
}


bool LagrangianDialog::saveValues(ScenarioThreebodyTransfer* threebodyTransfer)
{
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

    return true;
}

#endif

