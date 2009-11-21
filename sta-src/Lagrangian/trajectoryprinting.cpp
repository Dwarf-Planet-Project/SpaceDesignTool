
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

#include "trajectoryprinting.h"
#include <QString>
#include <cmath>
#include "Astro-Core/EODE/eode.h"
#include <iomanip>
#include "halorbitcomputation.h"
#include "Astro-Core/propagateTHREEbody.h"
#include <QTextStream>
#include <QDebug>
#include <QFile>
#include <QTemporaryFile>

using namespace std;


extern double Lpoint_Xposition, Grav_Param, bodies_distance;

void trajectory_integration (double state[], int &points, QString &file, int &trajectory_error)

{
    int digits_number=16;
#if 0
    QTemporaryFile* haloTempFile = new QTemporaryFile(file);
    if (!haloTempFile->open())
    {
        delete haloTempFile;
        haloTempFile = NULL;
        trajectory_error=2;
        return;
    }
#endif


        QFile haloFile(file);
        if (!haloFile.open(QIODevice::WriteOnly | QIODevice::Text))
            {trajectory_error=2;
                return;
            }

        QTextStream out(&haloFile);
        out.setRealNumberNotation(QTextStream::FixedNotation);
        out.setRealNumberPrecision(digits_number);

        int j, k, flag; double timefin, timein, relerr, abserr, accuracy, deriv[6], initial_condition[6]; double paramv[1]={0};
        abserr=relerr=accuracy=1E-9;   timefin=0;
        k=0;

        out<<0<<" ";
        for (j=3;j<6;j++)
                out<<state[j]<<" ";
        for (j=0;j<3;j++)
                out<<state[j]<<" ";
                out<<"\n";

        while (k!=points-1)//(timefin<=state[6]*(1-2/points))
                {k=k+1;
                for (j=0;j<6;j++)
                        {initial_condition[j]=state[j];
                        deriv[j]=0;}
                timein=0; flag=1;
                timefin=timefin+state[6]/(points-1);
                while (flag!=2)
                        {flag=Runge_Kutta_Fehlberg(threebody_EOM, 6, initial_condition, paramv, deriv, &timein, timefin, &relerr, abserr, flag );
                if (flag ==4)
                        flag = Runge_Kutta_Fehlberg(threebody_EOM, 6, initial_condition, paramv, deriv, &timein, timefin, &relerr, abserr, flag );}

                out<<timefin<<" ";
                for (j=3;j<6;j++)
                        out<<initial_condition[j]<<" ";
                for (j=0;j<3;j++)
                        out<<initial_condition[j]<<" ";
                out<<"\n";
                }

     //   haloTempFile->close();
        haloFile.close();
}



void manifolds_integration (const double state[], int &points, const char * file, int &trajectory_error, int &i, double Lpoint_Xposition, int manifold_kind)

{
        int j, k=1, flag;
        double timefin, timein, relerr, abserr, accuracy, deriv[6];
        double initial_condition[6];
        double paramv[1]={0};
        abserr=relerr=accuracy=1E-9;
        timefin=0; int digits_number=16;
        //qDebug()<<"i"<<i;
        QFile file_halo(file);
        if (abs(i)==1)		//the file has to be created
                {if (!file_halo.open(QIODevice::WriteOnly | QIODevice::Text))
                        {trajectory_error=2;
                        return;
                        }
                }
        if (abs(i)!=1)		//the file has to be updated
                {if (!file_halo.open(QIODevice::Append | QIODevice::Text))
                        {trajectory_error=2;
                        return;
                        }
                }
        QTextStream out(&file_halo);
        out.setRealNumberNotation(QTextStream::FixedNotation);
        out.setRealNumberPrecision(digits_number);

        if (state[6]!=0)			//a time of integration has been specified
            {
                double step=state[6]/(points-1);
                out<<"0 ";
                if (i>0)
                {
                     //the initial conditions are printed
                     for (j=3;j<6;j++)
                         out<<state[j]<<" ";
                     for (j=0;j<3;j++)
                         out<<state[j]<<" ";
                     out<<"\n";
                 }
                else  //initial conditions in case of unstable manifolds, because of backwards integration
                    out<<state[3]<<" "<<-state[4]<<" "<<-state[5]<<" "<<-state[0]<<" "<<state[1]<<" "<<state[2]<<" ";
                out<<"\n";

                for (k=1;k<points;k++)//(timefin<=state[6]*(1-2/points))
                {
                    for (j=0;j<6;j++)
                    {
                        initial_condition[j]=state[j];
                        deriv[j]=0;
                    }
                    timein=0; flag=1;
                    timefin=timefin+step;
                    while (flag!=2)
                    {
                        flag=Runge_Kutta_Fehlberg(threebody_EOM, 6, initial_condition, paramv, deriv, &timein, timefin, &relerr, abserr, flag );
                        if (flag ==4)
                            flag = Runge_Kutta_Fehlberg(threebody_EOM, 6, initial_condition, paramv, deriv, &timein, timefin, &relerr, abserr, flag );}

                        out<<timefin<<" ";
                        if (i>0)
                        {
                            for (j=3;j<6;j++)
                               out<<initial_condition[j]<<" ";
                            for (j=0;j<3;j++)
                               out<<initial_condition[j]<<" ";
                            out<<"\n"; //the final conditions are printed
                        }
                            else //final conditions in case of unstable manifolds, because of backwards integration
                                out<<initial_condition[3]<<" "<<-initial_condition[4]<<" "<<-initial_condition[5]<<" "<<-initial_condition[0]<<" "<<initial_condition[1]<<" "<<initial_condition[2]<<" ";
                            out<<"\n";
                        }
                file_halo.close();
                return;
                }
            else    //the manifolds have to be stopped at the intersection with a main body
                {
                    double memory=2;
                    QFile file_manifolds_settings ("3BMmanifolds_settings.stam");
                    if (!file_manifolds_settings.open(QIODevice::Append | QIODevice::Text))
                        {trajectory_error=4;
                        return;}
                    QTextStream settings(&file_manifolds_settings);
                    out<<"0 ";
                    if (i>0)
                {
                     //the initial conditions are printed
                     for (j=3;j<6;j++)
                         out<<state[j]<<" ";
                     for (j=0;j<3;j++)
                         out<<state[j]<<" ";
                     out<<"\n";
                 }
                    else   //initial conditions in case of unstable manifolds, because of backwards integration
                        out<<state[3]<<" "<<-state[4]<<" "<<-state[5]<<" "<<-state[0]<<" "<<state[1]<<" "<<state[2]<<" ";
                    out<<"\n";

                    double L_x=Lpoint_Xposition/bodies_distance; double step=0.05;
                    while (step>0.0001) //till the intersection with M1
                    {
                        k=k+1;
                        for (j=0;j<6;j++)
                        {
                            initial_condition[j]=state[j];
                            deriv[j]=0;
                        }
                        timein=0; flag=1;
                        timefin=timefin+step;
                        while (flag!=2)
                        {
                            flag=Runge_Kutta_Fehlberg(threebody_EOM, 6, initial_condition, paramv, deriv, &timein, timefin, &relerr, abserr, flag );
                            if (flag ==4)
                                flag = Runge_Kutta_Fehlberg(threebody_EOM, 6, initial_condition, paramv, deriv, &timein, timefin, &relerr, abserr, flag );
                        }
                        if (timefin>10) //the manifold is "too stable", then it will never reach an intersection with the main body selected
                        {
                            trajectory_error=6;
                            file_halo.close();
                            file_manifolds_settings.close();
                            return;
                        }

                        if (L_x>1) //we are using L2
                        {
                            if ((fabs(initial_condition[3])>fabs(memory)) && fabs(initial_condition[3])<(1-Grav_Param-0.1*(state[3]-(1-Grav_Param))))
                            {
                                k=k-1;
                                settings<<k<<"\n";
                                file_manifolds_settings.close();
                                file_halo.close();
                                return;
                            }
                            else if(fabs(initial_condition[3])>1-Grav_Param)
                            {
                                out<<timefin<<" ";
                                if (i>0)
                                {
                                    for (j=3;j<6;j++)
                                        out<<initial_condition[j]<<" ";
                                    for (j=0;j<3;j++)
                                        out<<initial_condition[j]<<" ";
                                    out<<"\n"; //the final conditions are printed
                                }
                                else //final conditions in case of unstable manifolds, because of backwards integration
                                    out<<initial_condition[3]<<" "<<-initial_condition[4]<<" "<<-initial_condition[5]<<" "<<-initial_condition[0]<<" "<<initial_condition[1]<<" "<<initial_condition[2]<<" ";
                                out<<"\n";

                                memory=initial_condition[3];
                            }
                            else
                            {
                                k=k-1;
                                timefin=timefin-step;
                                step=step/2;
                            }
                        }
                        else if (manifold_kind==1 || manifold_kind==2)	//we are using L1 till the intersection with the right-main body
                        {
                            if ((fabs(initial_condition[3])<fabs(memory)) && fabs(initial_condition[3])>(1-Grav_Param+0.1*((1-Grav_Param)-state[3])))
                            {
                                k=k-1;
                                settings<<k<<"\n";
                                file_manifolds_settings.close();
                                file_halo.close();
                                return;
                            }
                            else if(fabs(initial_condition[3])<1-Grav_Param)
                            {
                                out<<timefin<<" ";
                                if (i>0)
                                {
                                    for (j=3;j<6;j++)
                                        out<<initial_condition[j]<<" ";
                                    for (j=0;j<3;j++)
                                        out<<initial_condition[j]<<" ";
                                    out<<"\n"; //the final conditions are printed
                                }
                                else //final conditions in case of unstable manifolds, because of backwards integration
                                    out<<initial_condition[3]<<" "<<-initial_condition[4]<<" "<<-initial_condition[5]<<" "<<-initial_condition[0]<<" "<<initial_condition[1]<<" "<<initial_condition[2]<<" ";
                                out<<"\n";
                                memory=initial_condition[3];
                            }
                            else
                            {
                                k=k-1;
                                timefin=timefin-step;
                                step=step/2;
                            }
                        }
                        else if (manifold_kind==3 || manifold_kind==4)	//we are using L1 till the intersection with the left-main body
                        {
                            if ((initial_condition[3]>memory) && initial_condition[3]<(-Grav_Param+0.5*(state[3]+Grav_Param)))
                            {
                                k=k-1;
                                settings<<k<<"\n";
                                file_manifolds_settings.close();
                                file_halo.close();
                                return;
                            }
                            else if(initial_condition[3]>-Grav_Param)
                            {
                                out<<timefin<<" ";
                                if (i>0)
                                {
                                    for (j=3;j<6;j++)
                                       out<<initial_condition[j]<<" ";
                                    for (j=0;j<3;j++)
                                       out<<initial_condition[j]<<" ";
                                    out<<"\n"; //the final conditions are printed
                                }
                                else //final conditions in case of unstable manifolds, because of backwards integration
                                    out<<initial_condition[3]<<" "<<-initial_condition[4]<<" "<<-initial_condition[5]<<" "<<-initial_condition[0]<<" "<<initial_condition[1]<<" "<<initial_condition[2]<<" ";
                                out<<"\n";
                            }
                            else
                            {
                                k=k-1;
                                timefin=timefin-step;
                                step=step/2;
                            }
                        }
                    }
                settings<<k<<"\n";
                file_manifolds_settings.close();
                }
        file_halo.close();

}
