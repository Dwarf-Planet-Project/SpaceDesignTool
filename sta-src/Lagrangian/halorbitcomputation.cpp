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

#include "math.h"
#include "Astro-Core/propagateTHREEbody.h"
#include "Astro-Core/threebodyParametersComputation.h"
#include "halorbitcomputation.h"
#include "trajectoryprinting.h"
#include "Astro-Core/EODE/eode.h"
#include <QDebug>
#include <QString>
#include <ctime>
#include <QFile>
#include <QTextStream>
#include <Eigen/LU>
#include <Eigen/QR>

using namespace Eigen;
extern double Grav_Param, bodies_distance;
int halo_num_positions;
#define Pi 3.14159265358979
QString halo_input_data;

void HaloOrbit::lpointsComputation ()
{
    double ni, csi_1, csi_2;

    ni=pow(Grav_Param/(3*(1-Grav_Param)), 1.0/3.0);
    csi_1=ni*(1-ni/3.0-(pow(ni,2.0))/9.0-23*(pow(ni,3.0))/81.0+151*(pow(ni,4.0))/243.0-(pow(ni,5.0))/9.0);
    L1_xposition=(1-Grav_Param-csi_1);		//L1 coordinate
    csi_2=ni*(1+ni/3.0-(pow(ni,2.0))/9.0-31*(pow(ni,3.0))/81.0-119*(pow(ni,4.0))/243.0-(pow(ni,5.0))/9.0);
    L2_xposition=(1+csi_2-Grav_Param);	//L2 coordinate
}


void HaloOrbit::getAnalyticalApprox ()

{
        double gammaL, c[4], r1; int i;
        r1=trajectory.Lpoint_Xposition/bodies_distance;
        ////qdebug()<<bodies_distance;

        if (trajectory.Lpoint_selected==1)
                {
                double oi=1.0-Grav_Param-r1;
                gammaL=fabs(oi);
                for (i=2;i<5;i++)
                        c[i]=(pow(1.0,i)*Grav_Param+pow(-1.0,i)*(1-Grav_Param)*pow(gammaL,i+1)/pow(1.0-gammaL,i+1))/pow(gammaL,3);
                }
        else if (trajectory.Lpoint_selected==2)
                {
                gammaL=fabs(1-Grav_Param-r1);
                for (i=2;i<5;i++)
                        c[i]=(pow(-1.0,i)*Grav_Param+pow(-1.0,i)*(1-Grav_Param)*pow(gammaL,i+1)/pow(1.0+gammaL,i+1))/pow(gammaL,3);
                }
        else if (trajectory.Lpoint_selected==3)
                {
                gammaL=fabs(Grav_Param+r1);
                for (i=2;i<5;i++)
                        c[i]=(1-Grav_Param+(Grav_Param)*pow(gammaL,i+1)/pow(1.0+gammaL,i+1))/pow(gammaL,3);
                }

        double b, cc, lambda, k, delta, d1, d2;

        b=c[2]-2; cc=-(c[2]-1)*(c[2]*2+1);
        lambda=sqrt((-b+sqrt(pow(b,2.0)-4*cc))/2);
        k=(pow(lambda,2)+1+2*c[2])/(2*lambda);
        delta=pow(lambda,2)-c[2];
        d1=3*pow(lambda,2)*(k*(6*pow(lambda,2.0)-1)-2*lambda)/k;
        d2=8*pow(lambda,2)*(k*(11*pow(lambda,2.0)-1)-2*lambda)/k;

        double a21, a22, a23, a24, a31, a32, b21, b22, b31, b32, d21, d31, d32;

        a21=3*c[3]*(pow(k,2.0)-2)/(4*(1+2*c[2]));
        a22=3*c[3]/(4*(1+2*c[2]));
        a23=-3*c[3]*lambda*(3*lambda*pow(k,3.0)-6*k*(k-lambda)+4)/(4*k*d1);
        a24=-3*c[3]*lambda*(3*lambda*k+2)/(4*k*d1);

        b21=-3*c[3]*lambda*(3*k*lambda-4)/(2*d1);
        b22=3*c[3]*lambda/d1;
        a31=-9*lambda*(4*c[3]*(k*a23-b21)+k*c[4]*(4+pow(k,2)))/(4*d2)+((9*pow(lambda,2)+1-c[2])/(2*d2))*(3*c[3]*(2*a23-k*b21)+c[4]*(2+3*pow(k,2)));
        d21=-c[3]/(2*pow(lambda,2));
        a32=-(9*lambda*(4*c[3]*(k*a24-b22)+k*c[4])/4+3*(9*pow(lambda,2)+1-c[2])*(c[3]*(k*b22+d21-2*a24)-c[4])/2)/d2;
        b31=3*(8*lambda*(3*c[3]*(k*b21-2*a23)-c[4]*(2+3*pow(k,2)))+(9*pow(lambda,2)+1+2*c[2])*(4*c[3]*(k*a23-b21)+k*c[4]*(4+pow(k,2))))/(8*d2);
        b32=(9*lambda*(c[3]*(k*b22+d21-2*a24)-c[4])+3*(9*pow(lambda,2)+1+2*c[2])*(4*c[3]*(k*a24-b22)+k*c[4])/8)/d2;
        d31=3*(4*c[3]*a24+c[4])/(64*pow(lambda,2));
        d32=3*(4*c[3]*(a23-d21)+c[4]*(4+pow(k,2)))/(64*pow(lambda,2));

        double teta, s1, s2, l1, l2, a1, a2;

        a1=-3*c[3]*(2*a21+a23+5*d21)/2-3*c[4]*(12-pow(k,2))/8;
        a2=3*c[3]*(a24-2*a22)/2+9*c[4]/8;
        s1=(3*c[3]*(2*a21*(pow(k,2)-2)-a23*(pow(k,2)+2)-2*k*b21)/2-3*c[4]*(3*pow(k,4)-8*pow(k,2)+8)/8)/(2*lambda*(lambda*(1+pow(k,2))-2*k));
        s2=(3*c[3]*(2*a22*(pow(k,2)-2)+a24*(pow(k,2)+2)+2*k*b22+5*d21)/2+3*c[4]*(12-pow(k,2))/8)/(2*lambda*(lambda*(1+pow(k,2))-2*k));
        l1=a1+2*s1*pow(lambda,2);
        l2=a2+2*s2*pow(lambda,2);

        double Axmin, R1;

        R1=bodies_distance*gammaL;
        Axmin=(sqrt(fabs(delta/l1)))*R1;

        if (trajectory.init_data.Ax_amplitude<Axmin)
                trajectory.init_data.Ax_amplitude=Axmin;
        double Ax, Az;
        Az=trajectory.init_data.Az_amplitude/R1;
        Ax=trajectory.init_data.Ax_amplitude/R1;

        teta=0; i=0;

        double temporary_Zposition=((Az*cos(teta)+d21*Ax*Az*(cos(2*teta)-3)+cos(3*teta)*(d32*Az*pow(Ax,2)-d31*pow(Az,3)))*R1)/bodies_distance;
        if (temporary_Zposition<0)
                i=20;
        teta=Pi*0.05*i;
        double temporary_Xposition=((a21*pow(Ax,2)+a22*pow(Az,2)-Ax*cos(teta)+(a23*pow(Ax,2)-a24*pow(Az,2))*cos(2*teta)+(a31*pow(Ax,3)-a32*Ax*pow(Az,2))*cos(3*teta))*R1)/bodies_distance;

        trajectory.init_data.State.position.x()=r1+temporary_Xposition;
        trajectory.init_data.State.position.z()=((Az*cos(teta)+d21*Ax*Az*(cos(2*teta)-3)+cos(3*teta)*(d32*Az*pow(Ax,2)-d31*pow(Az,3)))*R1)/bodies_distance;
        trajectory.init_data.State.velocity.y()=((k*Ax*cos(teta)*lambda+(b21*pow(Ax,2)-b22*pow(Az,2))*cos(teta*2)*2*lambda+(b31*pow(Ax,3)-b32*Ax*pow(Az,2))*cos(3*teta)*3*lambda))*R1/bodies_distance;
}



MatrixXd STM(6,6);

void HaloOrbit::getNumericalApprox (int system, int &MAX_integration_time)//, const Eigen::MatrixXd STM)

{
    clock_t start, end;
    start= clock();
    time_t t0, t1, diff;
    double time_elapsed;

    double paramv[1]={0}; double deriv[42]; double state[42];
    trajectory.init_data.State.velocity.x()=trajectory.init_data.State.velocity.z()=trajectory.init_data.State.position.y()=trajectory.init_data.period=0;
    trajectory.final_data.State.velocity.x()=trajectory.final_data.State.velocity.z()=1;

    double timein, timefin, initialtimefin, relerr, abserr, accuracy, count, count2, T, step; int flag, k, q, p, l=0;
    relerr=abserr=accuracy=1E-9; timefin=0; initialtimefin=0.25; //period   // 0.25!!!!!!!!!!!!!!!!!!!!!!! per velocizzare 1.0

    double correction[6]; double z_correction, x_correction; double deriv_vel[3];

    if (trajectory.init_data.State.velocity.y()<0) //check the initial y-velocity sign
    {l=1;}

    while (fabs(trajectory.final_data.State.velocity.x())>10e-8 || fabs( trajectory.final_data.State.velocity.z())>10e-8) //stop condition
    {
        T=0; step=0.01;
        ////qdebug()<<"before change"<<timefin;
        //if (timefin>initialtimefin)
            //timefin=timefin-0.8;
        //else
//            timefin=initialtimefin;
        timefin=0.3;
        count=1;

       while (count>0)
       {
           flag=1; timein=0; timefin=timefin+step;
           for (p=0; p<42; p++)
           {
               state[p]=0;
               deriv[p]=0;
           }
               state[1]=trajectory.init_data.State.velocity.y(); state[5]=trajectory.init_data.State.position.z();
               state[0]=trajectory.init_data.State.velocity.x(); state[2]=trajectory.init_data.State.velocity.z();
               state[3]=trajectory.init_data.State.position.x(); state[4]=trajectory.init_data.State.position.y();

               state[41]=state[6]=state[13]=state[20]=state[27]=state[34]=1;
               //qDebug()<<"step0"<<state[0]<<state[1]<<state[2]<<state[3]<<state[4]<<state[5];

               while (flag!=2)
               {
                   flag=Runge_Kutta_Fehlberg(halorbit_EOM, 42, state, paramv, deriv, &timein, timefin, &relerr, abserr, flag);
                   if (flag==4)
                       flag=Runge_Kutta_Fehlberg(halorbit_EOM, 42, state, paramv, deriv, &timein, timefin, &relerr, abserr, flag);
               }
               //qDebug()<<"step1"<<state[0]<<state[1]<<state[2]<<state[3]<<state[4]<<state[5];
               count=state[4];
               if (l==1)
                   count=-state[4];
       }

       count2=1;
       end= clock(); time_elapsed=(double(end-start))/CLOCKS_PER_SEC;

       if (time_elapsed>MAX_integration_time)
       {
           ////qdebug()<<"Max time allowed:"<<MAX_integration_time<<"time elapsed"<<time_elapsed;
           trajectory.error=1;
           return;
       }

       while (fabs(count2)>10e-12)
       {
           timefin=timefin-step;
           step=step/10.0;
           count2=1;
           while (count2>0)
           {
               for (p=0; p<42; p++)
               {
                   state[p]=0;
                   deriv[p]=0;
               }
                   flag=1; timein=0; timefin=timefin+step;
                   state[1]=trajectory.init_data.State.velocity.y(); state[5]=trajectory.init_data.State.position.z();
                   state[0]=trajectory.init_data.State.velocity.x(); state[2]=trajectory.init_data.State.velocity.z();
                   state[3]=trajectory.init_data.State.position.x(); state[4]=trajectory.init_data.State.position.y();
                   state[41]=state[6]=state[13]=state[20]=state[27]=state[34]=1;
                   //qDebug()<<"step2"<<state[0]<<state[1]<<state[2]<<state[3]<<state[4]<<state[5];


                   while (flag!=2)
                   {
                       flag=Runge_Kutta_Fehlberg(halorbit_EOM, 42, state, paramv, deriv, &timein, timefin, &relerr, abserr, flag);
                       if (flag==4)
                           flag=Runge_Kutta_Fehlberg(halorbit_EOM, 42, state, paramv, deriv, &timein, timefin, &relerr, abserr, flag);
                   }
                   //qDebug()<<"step3"<<state[0]<<state[1]<<state[2]<<state[3]<<state[4]<<state[5];

                   count2=state[4];
                   if (l==1)
                       count2=-state[4];

                   end=clock(); time_elapsed=(double(end-start))/CLOCKS_PER_SEC;

                   if (time_elapsed>MAX_integration_time)
                   {
                       ////qdebug()<<"Max time allowed:"<<MAX_integration_time<<"time elapsed"<<time_elapsed;
                       trajectory.error=1;
                       return;
                   }
            }

           trajectory.final_data.State.velocity.x()=state[0]; trajectory.final_data.State.velocity.y()=state[1]; trajectory.final_data.State.velocity.z()=state[2];
           trajectory.final_data.State.position.x()=state[3]; trajectory.final_data.State.position.y()=state[4]; trajectory.final_data.State.position.z()=state[5];
           trajectory.final_data.period=T=2*timefin;
           correction[0]=state[12]; correction[1]=state[14]; correction[2]=state[24]; correction[3]=state[26]; correction[4]=state[36]; correction[5]=state[38];
       }

       ////qdebug()<<"initial state"<<trajectory.init_data.State.position.x()<<trajectory.init_data.State.position.z()<<trajectory.init_data.State.velocity.y();
       ////qdebug()<<"final state"<<trajectory.final_data.State.position.y()<<trajectory.final_data.State.velocity.x()<<trajectory.final_data.State.velocity.z()<<timefin;
       double C;
       Jacobi (Grav_Param, trajectory.final_data.State.position.x(), trajectory.final_data.State.position.y(), trajectory.final_data.State.position.z(), trajectory.final_data.State.velocity.x(), trajectory.final_data.State.velocity.y(), trajectory.final_data.State.velocity.z(),C);
       ////qdebug()<<"C"<<C;
       if (fabs(trajectory.final_data.State.velocity.x())<10e-8 && fabs(trajectory.final_data.State.velocity.z())<10e-8)
       {
           for (k=0;k<6;k++)
           {
               for (p=0;p<6;p++)
               {
                   STM(k,p)=state[6*(k+1)+p];
               }
           }
       }
       else
       {
           double r1, r2;
           r1=sqrt(pow(trajectory.final_data.State.position.x()+Grav_Param,2)+pow(trajectory.final_data.State.position.y(),2)+pow(trajectory.final_data.State.position.z(),2));
           r2=sqrt(pow(trajectory.final_data.State.position.x()-1+Grav_Param,2)+pow(trajectory.final_data.State.position.y(),2)+pow(trajectory.final_data.State.position.z(),2));
           deriv_vel[0]=2*trajectory.final_data.State.velocity.y()+trajectory.final_data.State.position.x()-(1-Grav_Param)*(trajectory.final_data.State.position.x()+Grav_Param)/(pow(r1,3))+ Grav_Param*(1-Grav_Param-trajectory.final_data.State.position.x())/(pow(r2,3));
           deriv_vel[2]=-(1-Grav_Param)*trajectory.final_data.State.position.z()/(pow(r1,3))-Grav_Param*trajectory.final_data.State.position.z()/(pow(r2,3));

           MatrixXd correction_matrix(2,2);
           for (k=0; k<4; k++)
           {
               q=(k+2)/2;
               int col = (k+1)/2-(q-1);
               correction_matrix(q-1,col)=correction[k+2]-deriv_vel[2*(q-1)]*correction[k-2*(q-1)]/trajectory.final_data.State.velocity.y();
               //correction_matrix[k]=correction[k+2]-deriv_vel[2*(q-1)]*correction[k-2*(q-1)]/trajectory.final_data.State.velocity.y();
           }
           //correction_matrix.inverse();
           ////qdebug()<<"inverse!"<<correction_matrix.inverse()(0,0)<<correction_matrix.inverse()(0,1)<<correction_matrix.inverse()(1,0)<<correction_matrix.inverse()(1,1);
           //determinant= correction_matrix[0]*correction_matrix[3]-correction_matrix[2]*correction_matrix[1];
           //inverse_correction_matrix[0]=correction_matrix[3]/determinant;
           //inverse_correction_matrix[1]=-correction_matrix[1]/determinant;
           //inverse_correction_matrix[2]=-correction_matrix[2]/determinant;
           //inverse_correction_matrix[3]=correction_matrix[0]/determinant;

               //x_correction=-inverse_correction_matrix[0]*trajectory.final_data.State.velocity.x()-inverse_correction_matrix[1]*trajectory.final_data.State.velocity.z();
               //z_correction=-inverse_correction_matrix[2]*trajectory.final_data.State.velocity.x()-inverse_correction_matrix[3]*trajectory.final_data.State.velocity.z();
           x_correction=-correction_matrix.inverse()(0,0)*trajectory.final_data.State.velocity.x()-correction_matrix.inverse()(0,1)*trajectory.final_data.State.velocity.z();
           z_correction=-correction_matrix.inverse()(1,0)*trajectory.final_data.State.velocity.x()-correction_matrix.inverse()(1,1)*trajectory.final_data.State.velocity.z();
               trajectory.init_data.State.position.x()=trajectory.init_data.State.position.x()+x_correction;
               trajectory.init_data.State.position.z()=trajectory.init_data.State.position.z()+z_correction;
       }

       }

//qdebug()<<"here|";
        int trajectory_error=0;
        state[6]=T; state[0]=trajectory.init_data.State.velocity.x(); state[1]=trajectory.init_data.State.velocity.y(); state[2]=trajectory.init_data.State.velocity.z();
        state[3]=trajectory.init_data.State.position.x(); state[4]=trajectory.init_data.State.position.y(); state[5]=trajectory.init_data.State.position.z();
        QString save_path = "3BMhalorbit";
        halo_input_data=save_path=save_path+QString::number(trajectory.init_data.Ax_amplitude,'f',0)+QString::number(trajectory.init_data.Az_amplitude,'f',0)+".stae";
        halo_num_positions=trajectory.Num_positions;
        trajectory_integration(state, trajectory.Num_positions, save_path, trajectory_error);
        ////qdebug() << "halo file: " << save_path;

        if (trajectory_error==2) //if the halo trajectory.txt has been not opened a message error must be given
        {
            trajectory.error=trajectory_error;
            return;
        }

        //file 3BMsystem.stam
        QString file = "3BMsystem.stam";
        QFile threeBMsystemFile(file);
        threeBMsystemFile.open(QIODevice::WriteOnly | QIODevice::Text);

        QTextStream outsys(&threeBMsystemFile);
        outsys.setRealNumberNotation(QTextStream::FixedNotation);
        outsys.setRealNumberPrecision(16);
        if (system==12)
            system=30; //user system, as ICD states

        outsys<<system<<"\n";
        outsys<<Grav_Param<<"\n";
        outsys<<bodies_distance<<"\n";
        outsys<<L1_xposition<<"\n";
        outsys<<L2_xposition<<"\n";
        threeBMsystemFile.close();
        ////qdebug()<<file;

        //computation of the real amplitudes obtained for the halo orbit
        QFile haloFile1(save_path);
        if (!haloFile1.open(QIODevice::ReadOnly | QIODevice::Text))
            {trajectory.error=3;
                return;
            }

        QTextStream in(&haloFile1);
        double t,Vx,Vy,Vz,x,y,z,X_max=0, X_min=10,Z_max=0, Z_min=10;
        for (p=0;p<trajectory.Num_positions;p++) //search for the maximum and minimum values of x and z coordinates
                {
                in>>t>>x>>y>>z>>Vx>>Vy>>Vz;
                //qDebug()<<x<<z;
                if (x<X_min)
                        X_min=x;
                if (x>X_max)
                        X_max=x;
                if (z<Z_min)
                        Z_min=z;
                if (z>Z_max)
                        Z_max=z;
                }
        haloFile1.close();

        trajectory.final_data.Ax_amplitude=(fabs(X_max)-fabs(X_min))/2;	trajectory.final_data.Az_amplitude=(fabs(Z_max)+fabs(Z_min))/2; //computation of the amplitudes obtained
        if (trajectory.init_data.Az_amplitude<0)
            trajectory.final_data.Az_amplitude=-trajectory.final_data.Az_amplitude;
        //qDebug()<<trajectory.final_data.Az_amplitude<<trajectory.final_data.Ax_amplitude<<bodies_distance;
        
        //check on the real amplitudes obtained, if too different the manifolds computation will be not allowed
        if (fabs((trajectory.final_data.Az_amplitude*bodies_distance-trajectory.init_data.Az_amplitude)/trajectory.init_data.Az_amplitude)>15 || fabs((trajectory.final_data.Ax_amplitude*bodies_distance-trajectory.init_data.Ax_amplitude)/trajectory.init_data.Ax_amplitude)>15)
            trajectory.error=4;
}



void HaloOrbit::getManifolds (double eigen, int points_number, double period_manifolds, int &first)

{
        double initial_condition_perturbed[7], initial_condition_0[7]; const char * manifolds_save_path;
        initial_condition_perturbed[6]=period_manifolds;

        QFile file_manifolds_settings ("3BMmanifolds_settings.stam");
        if (first==1)
                {if (!file_manifolds_settings.open(QIODevice::WriteOnly | QIODevice::Text))
                        {manifolds.error=3;
                        return;}
                }
        else
                {if (!file_manifolds_settings.open(QIODevice::Append | QIODevice::Text))
                        {manifolds.error=3;
                        return;}
                }

        QTextStream settings(&file_manifolds_settings);

        if (manifolds.kind==1)	//information about the manifolds required
                manifolds_save_path="3BMmanifold_rightStable.stae";
        else if (manifolds.kind==2)
                manifolds_save_path="3BMmanifold_rightUnstable.stae";
        else if (manifolds.kind==3)
                manifolds_save_path="3BMmanifold_leftStable.stae";
        else if (manifolds.kind==4)
                manifolds_save_path="3BMmanifold_leftUnstable.stae";
        else
                {manifolds.error=5;
                return;
                }
        settings<<manifolds.kind<<"\n"<<trajectory.Num_positions<<"\n";	//the kind of manifold and the number of orbits are printed

        if (period_manifolds!=0)
                settings<<points_number<<"\n";	//the number of positions per orbit is printed in case a time of integration has been specified
        file_manifolds_settings.close();

        int i;
       /////// if(trajectory.Num_positions>0) //normal manifolds computation


             QString halo_data = "3BMhalorbit";
        halo_data=halo_data+QString::number(trajectory.init_data.Ax_amplitude,'f',0)+QString::number(trajectory.init_data.Az_amplitude,'f',0)+".stae";
        QFile file_halo2(halo_data);
        if (!file_halo2.open(QIODevice::ReadOnly | QIODevice::Text))
        {
            manifolds.error=1;
            return;
        }
        QTextStream in(&file_halo2);
        for (i=1;i<trajectory.Num_positions+1;i++)
                {
                in>>initial_condition_0[6]>>initial_condition_0[3]>>initial_condition_0[4]>>initial_condition_0[5]>>initial_condition_0[0]>>initial_condition_0[1]>>initial_condition_0[2];

                for (int j=0;j<6;j++)
                        {initial_condition_perturbed[j]=initial_condition_0[j]-manifolds.epsilon*eigen*initial_condition_0[j];}
                if (eigen<1) //condition in case of stable manifolds
                {
                    initial_condition_perturbed[0]=-initial_condition_perturbed[0];
                    initial_condition_perturbed[4]=-initial_condition_perturbed[4];
                    initial_condition_perturbed[5]=-initial_condition_perturbed[5];
                    i=-i;
                }
                ////qdebug()<<"Debug| Bon"<<initial_condition_perturbed[0]<<initial_condition_perturbed[1]<<initial_condition_perturbed[2]<<initial_condition_perturbed[3]<<initial_condition_perturbed[4]<<initial_condition_perturbed[5]<<initial_condition_perturbed[6]<<points_number<<manifolds_save_path<<manifolds.error<<i<<trajectory.Lpoint_Xposition<<manifolds.kind;
                manifolds_integration(initial_condition_perturbed, points_number, manifolds_save_path, manifolds.error, i, trajectory.Lpoint_Xposition, manifolds.kind);
                i=abs(i);
                if (manifolds.error!=0)
                    return;
                }

                file_halo2.close();
                if (i!=trajectory.Num_positions+1)
                manifolds.error=5;

        /* manifolds computation in case of transfer optimization
           in this case only on trajectory is randomly computed
        */

       /*
 else

        {
            double paramv[1]={0}; double deriv[5]; double timefin, timein=0;
            double relerr, abserr, accuracy; abserr=relerr=accuracy=1E-9;

            for (int p=0; p<6; p++)
                deriv[p]=0;
            int flag=1;

            extern double fractionOrbit;
            timefin=trajectory.final_data.period*fractionOrbit;
            //qdebug()<<"time"<<timefin<<trajectory.final_data.period<<fractionOrbit;
            initial_condition_0[3]=trajectory.final_data.State.position.x();
            initial_condition_0[4]=trajectory.final_data.State.position.y();
            initial_condition_0[5]=trajectory.final_data.State.position.z();
            initial_condition_0[0]=trajectory.final_data.State.velocity.x();
            initial_condition_0[1]=trajectory.final_data.State.velocity.y();
            initial_condition_0[2]=trajectory.final_data.State.velocity.z();
//qdebug()<<"Debug| 1"<<initial_condition_0[0]<<initial_condition_0[1]<<initial_condition_0[2]<<initial_condition_0[3]<<initial_condition_0[4]<<initial_condition_0[5];
//qdebug()<<timein;
double initial_condition_1[5];
        while (flag!=2)
        {
            flag=Runge_Kutta_Fehlberg(threebody_EOM, 6, initial_condition_0, paramv, deriv, &timein, timefin, &relerr, abserr, flag );
            if (flag==4)
                flag=Runge_Kutta_Fehlberg(threebody_EOM, 6, initial_condition_0, paramv, deriv, &timein, timefin, &relerr, abserr, flag );
        //qdebug()<<timein;
        }
        for (int u=0; u<6; u++)
            initial_condition_1[u]=initial_condition_0[u];


//qdebug()<<"Debug| 2"<<initial_condition_1[0]<<initial_condition_1[1]<<initial_condition_1[2]<<initial_condition_1[3]<<initial_condition_1[4]<<initial_condition_1[5];
        initial_condition_1[6]=0; //required intersection with a main body
                for (int j=0;j<6;j++)
                        {initial_condition_perturbed[j]=initial_condition_1[j]-manifolds.epsilon*eigen*initial_condition_1[j];}
                i=1;
                //qdebug()<<"Debug| 3"<<initial_condition_perturbed[0]<<initial_condition_perturbed[1]<<initial_condition_perturbed[2]<<initial_condition_perturbed[3]<<initial_condition_perturbed[4]<<initial_condition_perturbed[5];
                if (eigen<1) //condition in case of stable manifolds
                {
                    initial_condition_perturbed[0]=-initial_condition_perturbed[0];
                    initial_condition_perturbed[4]=-initial_condition_perturbed[4];
                    initial_condition_perturbed[5]=-initial_condition_perturbed[5];
                    i=-1;
                }
                //qdebug()<<"Debug| final"<<initial_condition_perturbed[0]<<initial_condition_perturbed[1]<<initial_condition_perturbed[2]<<initial_condition_perturbed[3]<<initial_condition_perturbed[4]<<initial_condition_perturbed[5]<<initial_condition_perturbed[6]<<points_number<<manifolds_save_path<<manifolds.error<<i<<trajectory.Lpoint_Xposition<<manifolds.kind;
                manifolds_integration(initial_condition_perturbed, points_number, manifolds_save_path, manifolds.error, i, trajectory.Lpoint_Xposition, manifolds.kind);
                if (manifolds.error!=0)
                    return;
        }
        */

        //qdebug()<<manifolds_save_path;
        //qdebug()<<"deviation:"<<manifolds.epsilon;

        ////qdebug()<<manifolds_save_path;
        ////qdebug()<<"deviation:"<<manifolds.epsilon;

}


//subroutine to compute Eigenvalues
void HaloOrbit::getEigenvalues ()

{
        MatrixXd mat1(6,6), mat2(6,6), mat3(6,6), fi(6,6);//, STM_inv(6,6);
        mat2.setZero();
        mat3.setZero();
        mat1.setIdentity();
        mat1=-mat1;

        mat2(0,3)=mat2(1,4)=mat2(2,5)=-1;
        mat2(3,0)=mat2(4,1)=mat2(5,2)=1;
        mat2(3,4)=-2; mat2(4,3)=2;

        mat3(0,1)=-2; mat3(1,0)=2;
        mat3(0,3)=mat3(1,4)=mat3(2,5)=1;
        mat3(3,0)=mat3(4,1)=mat3(5,2)=-1;
        fi=mat1*STM.inverse()*mat2*mat3;


        //Eigen libray doesn't support the eigenvalues computation of any matrix, but only of selfAdjoint matrix.
        //because the STM is not such a matrix, the eigenvalues are computed in an alternative way.
        //When the eigenvalues computation will be available also for this kind of matrix, it's stronly recommended
        //to change this subroutine, by using the Eigen library.
        //the following hidden part is the code able to compute eigenvalues for selfAdjoint matrix. By replacing that
        //specific line, it will be possible to use the new subroutine of the Eigen library, when available.
#if 0
        //
        VectorXd eigenvalues_vector(6);
        eigenvalues_vector=fi.marked<SelfAdjoint>().eigenvalues();
        //VectorXd eigenvalues(6);
//cout<<fi;

        //fi.eigenvalues();
        //qdebug()<<"fi";
        //qdebug()<<fi(0,0)<<fi(0,1)<<fi(0,2)<<fi(0,3)<<fi(0,4)<<fi(0,5);
        //qdebug()<<fi(1,0)<<fi(1,1)<<fi(1,2)<<fi(1,3)<<fi(1,4)<<fi(1,5);
        //qdebug()<<fi(2,0)<<fi(2,1)<<fi(2,2)<<fi(2,3)<<fi(2,4)<<fi(2,5);
        //qdebug()<<fi(3,0)<<fi(3,1)<<fi(3,2)<<fi(3,3)<<fi(3,4)<<fi(3,5);
        //qdebug()<<fi(4,0)<<fi(4,1)<<fi(4,2)<<fi(4,3)<<fi(4,4)<<fi(4,5);
        //qdebug()<<fi(5,0)<<fi(5,1)<<fi(5,2)<<fi(5,3)<<fi(5,4)<<fi(5,5);
        //qdebug()<<"eigen";
        //qdebug()<<evve(0)<<evve(1)<<evve(2)<<evve(3)<<evve(4)<<evve(5);
        ////qdebug()<<eigenvalues(0)<<eigenvalues(1)<<eigenvalues(2)<<eigenvalues(3)<<eigenvalues(4)<<eigenvalues(5);

#endif

         MatrixXd mm1(6,6);
         mm1=fi;
         double step=0.001;
         double l=0; double h=1; int i;

         while (abs(h)>0.00001)
         {
             if (l>1)
             {
                 l=0;
                 step=step/10;}
                 l=step+l;
                 for (i=0;i<6;i++)
                 {
                     mm1(i,i)=fi(i,i)-l;}
                     h=mm1.determinant();
                     if (step<0.0000000001)
                     {
                         manifolds.error=4;
                         return;}
                     }

         manifolds.eigen1=l;
         manifolds.eigen2=1/l;
         //double index=0.5*(manifolds.eigen1+1/manifolds.eigen1); //stability index
}

