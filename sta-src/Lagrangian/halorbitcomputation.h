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


#include "Astro-Core/statevector.h"
#include <Eigen/LU>

using namespace Eigen;

class HaloOrbit

{
public:
    double L1_xposition;
    double L2_xposition;

            
    struct halo_data
    {
        double Ax_amplitude;
        double Az_amplitude;
        double period;
        sta::StateVector State;
    };

    struct trajectory
    {
        halo_data init_data, final_data;
        int Lpoint_selected;
        double Lpoint_Xposition;
        int Num_positions;
        int error;
    };
    struct manifolds
        {
        double epsilon;        //done!
        double eigen1;       //done!
        double eigen2;       //done!
        int error;           //done!
        int kind;            //done, but useless?
        //manifold RS, RU, LS, LU;
        };


void lpointsComputation ();
void getAnalyticalApprox();
void getNumericalApprox (int systems, int &MAX_integration_time);
void getManifolds (double eigen, int points_number, double period_manifolds, int &first);
void getEigenvalues ();

    trajectory trajectory;
    manifolds  manifolds;

public:
    //MatrixXd STM;
    //Eigen::MatrixXd STM();

};



