
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

#include "EarthMoonTransfer.h"

#include "Optimization/GlobalOptimizers.h"
#include "Lagrangian/trajectoryprinting.h"
#include "Lagrangian/halorbitcomputation.h"
#include "Lagrangian/lagrangianAdvanced.h"
#include "Scenario/scenario.h"
#include "Eigen/Core"
#include "Astro-Core/stabody.h"
#include "Astro-Core/EODE/eode.h"
#include "Astro-Core/propagateTHREEbody.h"
#include "Astro-Core/rotatingTOcartesian.h"
#include "Astro-Core/stamath.h"
#include "Astro-Core/threebodyParametersComputation.h"
#include <QDebug>
#include <QMessageBox>

using namespace Eigen;
using namespace std;


#if OLDSCENARIO
void EarthMoonPatch()
{
    //Earth-Sun system
        extern ScenarioThreebodyTransfer* threebodyTransfer;
        extern sta::StateVector initialPositionRotating;
        extern ScenarioStateVector* stateVector_HaloTransfer;

        double Grav_Param, bodies_distance_Leg1, velocity_Leg1, bodies_distance_Leg2, velocity_Leg2;
        Grav_Param=getGravParam_user(STA_SOLAR_SYSTEM->sun()->mu(),STA_SOLAR_SYSTEM->earth()->mu());
        bodies_distance_Leg1=STA_SOLAR_SYSTEM->earth()->distance(); velocity_Leg1=STA_SOLAR_SYSTEM->earth()->linearV();
        bodies_distance_Leg2=STA_SOLAR_SYSTEM->lookup(STA_MOON)->distance(); velocity_Leg2=STA_SOLAR_SYSTEM->lookup(STA_MOON)->linearV();

        double timefinTransfer0;
        extern int System, Lagrmode;
        extern HaloOrbit halo_Leg1;
        extern HaloOrbit halo_Leg2;

        halo_Leg1.manifolds.error=halo_Leg2.manifolds.error=1;

        double paramv[1]={0}; double deriv[5]; double timein=0;
        double relerr, abserr, accuracy; abserr=relerr=accuracy=1E-10;
        double initial_condition_perturbed[6],initial_condition_perturbed_0[6], initial_condition_0[5];

        int totalPosition=50;
        for (int i=1;i<totalPosition+1;i++)
        {
        for (int p=0; p<6; p++)
            deriv[p]=0;
        int flag=1;
        timefinTransfer0=halo_Leg1.trajectory.final_data.period*(1.0/4.0+i/(2*totalPosition));
        initial_condition_0[3]=halo_Leg1.trajectory.init_data.State.position.x();
        initial_condition_0[4]=halo_Leg1.trajectory.init_data.State.position.y();
        initial_condition_0[5]=halo_Leg1.trajectory.init_data.State.position.z();
        initial_condition_0[0]=halo_Leg1.trajectory.init_data.State.velocity.x();
        initial_condition_0[1]=halo_Leg1.trajectory.init_data.State.velocity.y();
        initial_condition_0[2]=halo_Leg1.trajectory.init_data.State.velocity.z();
        //qDebug()<<"Debug| 1"<<initial_condition_0[0]<<initial_condition_0[1]<<initial_condition_0[2]<<initial_condition_0[3]<<initial_condition_0[4]<<initial_condition_0[5];
        //qDebug()<<timein;

        while (flag!=2)
        {
            flag=Runge_Kutta_Fehlberg(threebody_EOM, 6, initial_condition_0, paramv, deriv, &timein, timefinTransfer0, &relerr, abserr, flag );
            if (flag==4)
                flag=Runge_Kutta_Fehlberg(threebody_EOM, 6, initial_condition_0, paramv, deriv, &timein, timefinTransfer0, &relerr, abserr, flag );
  //      qDebug()<<timein;
        }

        //qDebug()<<"Debug| 2"<<initial_condition_0[0]<<initial_condition_0[1]<<initial_condition_0[2]<<initial_condition_0[3]<<initial_condition_0[4]<<initial_condition_0[5];
        double finalHaloRotX_actual,finalHaloRotY_actual,finalHaloRotZ_actual,finalHaloRotVX_actual,finalHaloRotVY_actual,finalHaloRotVZ_actual;
        finalHaloRotX_actual=initial_condition_0[3];
        finalHaloRotY_actual=initial_condition_0[4];
        finalHaloRotZ_actual=initial_condition_0[5];
        finalHaloRotVX_actual=initial_condition_0[0];
        finalHaloRotVY_actual=initial_condition_0[1];
        finalHaloRotVZ_actual=initial_condition_0[2];

        halo_Leg1.manifolds.epsilon=halo_Leg1.manifolds.epsilon;
        qDebug()<<"epsilon Leg1"<<halo_Leg1.manifolds.epsilon;
        double eigen=halo_Leg1.manifolds.eigen2; //eigenvalue in use (if 1 stable manifolds, if 2 unstable)

  //      qDebug()<<"eigens"<<haloTransfer.manifolds.eigen1<<haloTransfer.manifolds.eigen2;
        for (int j=0;j<6;j++)
            {initial_condition_perturbed[j]=initial_condition_perturbed_0[j]=initial_condition_0[j]-halo_Leg1.manifolds.epsilon*eigen*initial_condition_0[j];}
   //     qDebug()<<"Debug| 3"<<initial_condition_perturbed[0]<<initial_condition_perturbed[1]<<initial_condition_perturbed[2]<<initial_condition_perturbed[3]<<initial_condition_perturbed[4]<<initial_condition_perturbed[5];
  //      qDebug()<<"eigen"<<eigen;

        if (eigen<1) //condition in case of stable manifolds
        {
            initial_condition_perturbed[0]=-initial_condition_perturbed[0];
            initial_condition_perturbed[4]=-initial_condition_perturbed[4];
            initial_condition_perturbed[5]=-initial_condition_perturbed[5];
        }

        }
        timein=0;
        double step=0.2;
        double initial_condition_memory[6], initial_interval[6],initial[6];
        for (int p=0; p<6; p++)
        initial[p]=initial_condition_perturbed[p];

    //    qDebug()<<"Debug| final"<<initial_condition_perturbed[0]<<initial_condition_perturbed[1]<<initial_condition_perturbed[2]<<initial_condition_perturbed[3]<<initial_condition_perturbed[4]<<initial_condition_perturbed[5]<<haloTransfer.manifolds.error<<haloTransfer.trajectory.Lpoint_Xposition<<haloTransfer.manifolds.kind;

        /*


    double limitTOF; //to speed up the process
    extern int maxTOF;
    limitTOF=maxTOF*2*Pi()/(secondBody->body()->orbitalPeriod());
    double targetX, radiusLimit, stopcondition;


    double  LastRapproaching=0;
    timefinTransfer0=0;
    while (step>0.00000001 && fabs(initial_condition_perturbed[3]-(1-Grav_Param)>100/bodies_distance_Leg1))
    {
        for (int p=0; p<6; p++)
            initial_condition_perturbed[p]=initial[p];
        timein=0;
        flag=1;
        for (int p=0; p<6; p++)
            deriv[p]=0;
        timefinTransfer0=timefinTransfer0+step;
        while (flag!=2)
        {
            flag=Runge_Kutta_Fehlberg(threebody_EOM, 6, initial_condition_perturbed, paramv, deriv, &timein, timefinTransfer0, &relerr, abserr, flag );
            if (flag==4)
                flag=Runge_Kutta_Fehlberg(threebody_EOM, 6, initial_condition_perturbed, paramv, deriv, &timein, timefinTransfer0, &relerr, abserr, flag );
        }

//        qDebug()<<timein;
        if (initial_condition_perturbed[3]<(1-Grav_Param))
        {
            timefinTransfer0=timefinTransfer0-step;
            step=step/2;

        }
        else if ((initial_condition_perturbed[3]<targetX && haloTransfer.trajectory.Lpoint_selected==2) || (initial_condition_perturbed[3]>targetX && haloTransfer.trajectory.Lpoint_selected==1 && firstBody->body()->id()==10))
        {
            timefinTransfer0=timefinTransfer0-step;
            step=step/2;

            if (fabs(initial_condition_perturbed[3]-targetX)<50000/bodies_distance && LastRapproaching>300000)
            {
                //qDebug()<<"exit!"<<step<<0.00000001<<fabs(initial_condition_perturbed[3]-targetX)*bodies_distance<<LastRapproaching;
                step=0.0000000001;
                firstBorderTime=0;
            }
        }
    }

 */
        //Earth-Moon system
        Grav_Param=getGravParam_user(STA_SOLAR_SYSTEM->earth()->mu(),STA_SOLAR_SYSTEM->lookup(STA_MOON)->mu());
}
#endif // OLDSCENARIO
