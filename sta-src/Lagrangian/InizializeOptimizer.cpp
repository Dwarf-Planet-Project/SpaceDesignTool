
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

#include <Eigen/Core>
#include "Optimization/GlobalOptimizers.h"
#include <QDebug>
using namespace Eigen;

extern int numObj, numIterations, initMODE;
void GlobalOptimizers::InitializeOptimizer()
{
        int i;

        //Random numbers generator initialization:
        // Next line patched by Guillermo to avoid complaints of MingW 4.4
        //int seed=time(0);						//set initial seed for random number generator based on current clock
        int seed=0;
        srand(seed);							//initialize random numbers generator
        //NOTE: seed can be ANY integer number


        //Problem parameters definition:
        NVAR=4;								//define number of optimization variables
        NOBJ=numObj;									//define number of optimization objectives
        qDebug()<<"numObj"<<NOBJ;
        NCONS=1;								//define number of optimization constraints
        NsolPareto=150;							//define number of archive solutions
        //NOTE: the parameter NsolPareto is NOT used within PSO-1D (only one solution is found for mono-objective optimization),
        //for NSGA-II it defines the population size (and the archive coincides with the population) and for DG-MOPSO it defines
        //the archive size, while the swarm size is fixed to 100 (it can be changed within the code in file dgmopso.cpp).
        MAXITER=numIterations;							//define number of iterations to be performed by the algorithm
        InitMode=initMODE;								//define initial set of solutions initialization mode
        // InitMode=0: standard random initialization, to use the first time an optimization is run;
        // InitMode=1: initialize from previous iteration results: optimization variables for N ARCHIVE solutions must have previously
        //				been saved in gsl_matrix *OptimalSolutions (of dimensions NxNVAR: 1 solution for each of the N rows,
        //				1 variable for each of the NVAR variables) and optimization objectives and constraints must have
        //				previously been saved in gsl_matrix *ParetoFront (of dimensions Nx(NOBJ+NCONS): 1 solution for each of
        //				the N rows, with the objectives and then the constraints on the columns).
        //		In case of NSGA-II archive and population coincide, while for PSO-1D and DG-MOPSO it is also necessary to provide
        //		the results in terms of previous swarm positions, velocities and personal bests --> also the matrixes *OldSwarm,
        //		*OldPbest,*OldVel (which have 100 rows for the 100 particles of the swarm and NVAR columns for each variable
        //		value) must be initialized by the user.
        // InitMode=2: initialize from previous run results: only optimization variables of a the optimal solutions found in
        //				a previous run must have been saved in *OptimalSolutions (same structure as above).


        //GSL vectors and matrixes memory allocation
        LOWERBOUND=VectorXd(NVAR); //=gsl_vector_alloc(NVAR);
        UPPERBOUND=VectorXd(NVAR); //=gsl_vector_alloc(NVAR);
        OptimalSolutions=MatrixXd(NsolPareto,NVAR); //=gsl_matrix_alloc(NsolPareto,NVAR);
        ParetoFront=MatrixXd(NsolPareto,NOBJ+NCONS); //=gsl_matrix_alloc(NsolPareto,NOBJ+NCONS);

        //the 1st number in brackets should be the same of swarmsize in pso1D or DG-MOPSO
        OldSwarm=MatrixXd(500,NVAR); //=gsl_matrix_alloc(100,NVAR);		//This matrix must be allocated only if PSO-1D or DG-MOPSO are selected
        OldPbest=MatrixXd(500,NVAR); //=gsl_matrix_alloc(100,NVAR);		//This matrix must be allocated only if PSO-1D or DG-MOPSO are selected
        OldVel=MatrixXd(500,NVAR); //=gsl_matrix_alloc(100,NVAR);			//This matrix must be allocated only if PSO-1D or DG-MOPSO are selected
        OldJpbest=MatrixXd(500,NVAR); //=gsl_matrix_alloc(100,NVAR);		//This matrix must be allocated only if DG-MOPSO is selected
        OldJswarm=MatrixXd(500,NVAR); //=gsl_matrix_alloc(100,NVAR);		//This matrix must be allocated only if DG-MOPSO is selected

        //Variables optimization boundaries definition:
        for(i=0;i<NVAR;i++)
                {
                    LOWERBOUND(i)=0;
                    UPPERBOUND(i)=1;
                }
        LOWERBOUND(0)=-0.001; UPPERBOUND(0)=0.001;//for deviation of Halo orbit

        char *file1="OutputPareto.stam";		//File name for Pareto front objectives and constraints history writing
        char *file2="OutputSolutions.stam";		//File name for Pareto front variables writing at the end of run
        if ((fpParetoFront=fopen(file1,"w"))==NULL) { printf("\nError: could not open file %s for writing.\n",file1); exit(1); }
        if ((fpSolutions=fopen(file2,"w"))==NULL) { printf("\nError: could not open file %s for writing.\n",file2); exit(1); }
}




