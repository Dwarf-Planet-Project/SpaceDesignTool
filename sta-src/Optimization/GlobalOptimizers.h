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

//------------------ Author: Francesco Castellini -------------------------------------------//
//------- Affiliation: Dipartimento di Ingegneria Aerospaziale, Politecnico di Milano -------//
//------------------------- E-mail: (castellfr@gmail.com) -----------------------------------//

//========================== STA library for global optimizers ==============================//*/


/* This header file defines the class that encapsulates STA global optimization algorithms.

 --> HOW TO USE THE CLASS GlobalOptimizers:
The user shall include in the code of the project the following files:

GlobalOptimizers.h

random.h
input.h
realinit.h
init.h
decode.h
ranking.h
rancon.h
dfit.h
select.h
crossover.h
uniformxr.h
realcross2.h
mut.h
realmut1.h
keepaliven.h

RunOptimizer.cpp
pso1D.cpp
dgmopso.cpp
nsga2.cpp

Moreover the user shall provide in other file(s) of the project the definition of the following two functions,
declared and briefly described in the class body below:

void GlobalOptimizers::InitializeOptimizer()
gsl_matrix* GlobalOptimizers::EvaluateModel(gsl_matrix*,int)


To run an optimization, the user shall then define an object of the class GlobalOptimizers (e.g. OPT) and call
the following two functions:

OPT.InitializeOptimizer();			//to initialize the optimizer
OPT.RunOptimizer(int);				//to run the optimizer.

//NOTE I: in RunOptimizer int determines the type of optimizer used for the run. The following optimizers are available:

0. PSO-1D (Particle Swarm Optimization - mono dimensional --> file pso1D.cpp):
                basic implementation of Particle Swarm Optimization method for single-objective problems. This can be used only
                if the parameter NOBJ has been defined as 1 in InitializeOptimizer(), otherwise the program cannot execute the
                optimization run (it's only for single objective optimization!) and returns an error.
                This algorithm is very fast and for single-objective optimization should definitely be preferred to the other two
                algorithms, that are specifically implemented for dealing with more than one objective and are therefore not
                efficient for a single objective run (even though they can be used for that as well).
                PSO 1D implements a simple constraint handling technique that is efficient in dealing with the constraints, but
                the user shall comply with the following requirements in the modeling of the constraints:
                - NCONS must be defined in InitializeOptimizer() as the number of constraints to be satisfied;
                - the function EvaluateModel() must return 0 if the constraint is satisfied and a negative number correspondant to
                        the constraint violation if the constraint is violated (see structure of output gsl matrix below)
                - in case NCONS>1 the different constraints should preferibly have comparable constraint violation values, meaning
                        that if two different constraints have the same "gravity" for the user they should also have comparable
                        values for the constraint violations returned by EvaluateModel(). In fact, since the optimizer aggregates the
                        different constraints violations in a penalty coefficient, if the values are of different orders of magnitude
                        the optimizer will tend to minimize the constraint violation of the constraint(s) of higher order of magnitude
                        and will hardly find solutions with low constraint violation on the other constraint(s).

1. NSGA-II (Non Dominated Sorting Genetic algorithm - II --> file nsga2.cpp):
                efficient elitist genetic algorithm developed by Prof.Deb of the Kanpur Genetic Algorithms Laboratory. This
                algorithm can deal both with single or multi-objective problems, but is more indicated for multi-objective problems.
                NSGA-II has a constant size archive of solutions and therefore always returns to the user the number of solutions
                that have been required though the parameter NsolPareto defined in InitializeOptimizer(); of course it is not
                guaranteed that all these solutions are non-dominated. Since the archive and the population coincide, this
                algorithm does not work well if NsolPareto is low (50<NsolPareto<200 is suggested).	Even though the constraint
                handling technique is different with respect to PSO-1D, the same requirements are imposed on the user.

2. DG-MOPSO (Double Grid - MultiObjective Particle Swarm Optimization --> file dgmopso.cpp):
                further development of the first proposed MOPSO algorithm. As NSGA-II, this algorithm can deal both with single
                or multi-objective problems, but is more indicated for multi-objective problems. DG-MOPSO has a variable size
                archive that collects only the non-dominated solutions found so far up to a maximum of NsolPareto solutions;
                it is not therefore guaranteed that all the requested NsolPareto solutions are returned to the user. Since the
                swarm size (number of particles (solutions) that are evaluated at each iteration) is NOT equal to the archive size,
                the algorithm does not need a high number of archive solutions to work well as for NSGA-II. However, even if the
                user does not need a high number of solutions it is preferable to keep NsolPareto>30 (for lower values the
                mechanisms that maintain the diversity of solutions usually do not work well). DG-MOPSO uses the same constraint
                handling technique of PSO-1D, therefore the same requirements are imposed on the user.

//NOTE II: NSGA-II or DG-MOPSO ?
While for single-objective problems the use of PSO-1D is strongly suggested, for multi-objective problems two alternatives
are given. In general, DG-MOPSO is a faster algorithm and is therefore suggested as first choice, but NSGA-II sometimes
gives better results in terms of uniformity of distribution of the solutions over the Pareto front. Moreover it is not
guaranteed that DG-MOPSO will converge faster than NSGA-II on all kinds of problems and for all the runs.
The stochastic aspect of the algorithms implies that each run has different results, unless the same random seed is used.
Therefore it is strongly raccommended, in order to have a good confidence in the optimality of the results found, to
perform at least 2 or 3 runs with different random seeds with DG-MOPSO as first choice, but also to verify the achieved
Pareto fronts with NSGA-II.

//NOTE III: ALGORITHMS PARAMETERS :
only few basic problem data and algorithms parameters shall be set up by the user in InitializeOptimizer(); most of the
algorithms parameters are instead fixed to the default values inside the related codes. The values of these parameters
can influence sensibly the behaviour of the algorithms, and if the user that wants to change them should manually modify
them in the following files:
PSO-1D --> pso1D.cpp, at the beginning of the pso1D() function
NSGA-II --> input.h, inside the body of the function input()
DG-MOPSO --> dgmopso.cpp, at the beginning of the dgmopso() function

//NOTE IV:
at the end of this file two test mathematical functions are reported, a single-objective function called Griewangk and a
bi-objective function called t1, so that the user can use them to test the behaviour of the optimization algorithms.
*/

#include <Eigen/Core>
using namespace Eigen;
#define huge 1e10	//NOTE: increase this if the objective functions have values of this order of magnitude. Maintain approximately 10 orders of magnitude of difference.

class GlobalOptimizers
{
public:
        int NVAR,NOBJ,NCONS,MAXITER,InitMode,NsolPareto;
        FILE *fpSolutions, *fpParetoFront;
        Eigen::VectorXd LOWERBOUND,UPPERBOUND;
        Eigen::MatrixXd OptimalSolutions, ParetoFront, OldSwarm, OldPbest, OldVel, OldJpbest, OldJswarm;

        void InitializeOptimizer();
        //This function has to be provided by the user and should initialize the values of the main problem parameters
        //(NVAR,NOBJ,NCONS,MAXITER,InitMode,NsolPareto,LOWERBOUND,UPPERBOUND), define the seed for the random numbers
        //generator, allocate memory for the GSL vectors and matrixes AND open the output files.
        //NOTE that the matrixes *OptimalSolutions,*ParetoFront,*OldSwarm,*OldPbest,*OldVel,*OldJpbest and *OldJswarm DO NOT
        //have to be initialized by the user, unless he wants the optimization algorithms to start from a previous iteration
        //or run results, but memory must be allocated for them.
        //See the prototype below for an example.

        Eigen::VectorXd EvaluateModel(Eigen::VectorXd, int);
        //This function has to be provided by the user and is the function that provides the mathematical function to be
        //optimized AND the constraints to which the function is subjected. The function must take as input a gsl_vector of
        //NVAR elements containing the variables values and return another gsl_vector of NOBJ+NCONS elements that contains
        //first the optimization objectives and then the optimization constraints (if there are constraints). As additional
        //input a integer flag can be used by the user, for example it can be used to tell the function whether to write
        //results to file or not (or the int input can simply be a dummy variable).
        //See mathematical benchmark problems below for examples.

        void RunOptimizer(int);
        //This function must not be modified by the user and it calls the different global optimization algorithms. It is the
        //function that has to be called to run an optimization (the input 0, 1 or 2 determines the algorithm that will be used).

        //Optimizers functions, these functions contain the main bodies of the optimization algorithms and refer to other
        //subroutines with sequential C-style:
        void nsga2();
        void dgmopso();
        void pso1D();
};



/*
//PROTOTYPE FOR FUNCTION InitializeOptimizer() (Note that all the features defined MUST be present in the user's function):

#include <gsl/gsl_vector.h>
#include <gsl/gsl_matrix.h>
#include <time.h>	//to define the random seed with the CPU clock --> automatically varying the seed at each run

void GlobalOptimizers::InitializeOptimizer()
{
        int i;

        //Random numbers generator initialization:
        int seed=time(0);						//set initial seed for random number generator based on current clock
        srand(seed);							//initialize random numbers generator
        //NOTE: seed can be ANY integer number

        //Problem parameters definition:
        NVAR=10;								//define number of optimization variables
        NOBJ=2;									//define number of optimization objectives
        NCONS=1;								//define number of optimization constraints
        NsolPareto=100;							//define number of archive solutions
        //NOTE: the parameter NsolPareto is NOT used within PSO-1D (only one solution is found for mono-objective optimization),
        //for NSGA-II it defines the population size (and the archive coincides with the population) and for DG-MOPSO it defines
        //the archive size, while the swarm size is fixed to 100 (it can be changed within the code in file dgmopso.cpp).
        MAXITER=50;								//define number of iterations to be performed by the algorithm
        InitMode=0;								//define initial set of solutions initialization mode
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
        LOWERBOUND=gsl_vector_alloc(NVAR);
        UPPERBOUND=gsl_vector_alloc(NVAR);
        OptimalSolutions=gsl_matrix_alloc(NsolPareto,NVAR);
        ParetoFront=gsl_matrix_alloc(NsolPareto,NOBJ+NCONS);

        OldSwarm=gsl_matrix_alloc(100,NVAR);		//This matrix must be allocated only if PSO-1D or DG-MOPSO are selected
        OldPbest=gsl_matrix_alloc(100,NVAR);		//This matrix must be allocated only if PSO-1D or DG-MOPSO are selected
        OldVel=gsl_matrix_alloc(100,NVAR);			//This matrix must be allocated only if PSO-1D or DG-MOPSO are selected
        OldJpbest=gsl_matrix_alloc(100,NVAR);		//This matrix must be allocated only if DG-MOPSO is selected
        OldJswarm=gsl_matrix_alloc(100,NVAR);		//This matrix must be allocated only if DG-MOPSO is selected

        //Variables optimization boundaries definition:
        for(i=0;i<NVAR;i++)
                {
                gsl_vector_set(LOWERBOUND,i,0);
                gsl_vector_set(UPPERBOUND,i,1);
                }

        char *file1="OutputPareto.stam";		//File name for Pareto front objectives and constraints history writing
        char *file2="OutputSolutions.stam";		//File name for Pareto front variables writing at the end of run
        if ((fpParetoFront=fopen(file1,"w"))==NULL) { printf("\nError: could not open file %s for writing.\n",file1); exit(1); }
        if ((fpSolutions=fopen(file2,"w"))==NULL) { printf("\nError: could not open file %s for writing.\n",file2); exit(1); }
}

*/


/*
//PROTOTYPES FOR FUNCTION EvaluateModel()

#include <gsl/gsl_vector.h>
#include <gsl/gsl_matrix.h>
#include <math.h>

//1. Mono-objective Griewangk function with linear constraint
//In void Globaloptimizers::InitializeOptimizer() NVAR,NCONS,NOBJ,LOWERBOUND and UPPERBOUND must be defined as:
NVAR=30; NOBJ=1; NCONS=1;
for(i=0;i<NVAR;i++)
        {
        gsl_vector_set(LOWERBOUND,i,-512);
        gsl_vector_set(UPPERBOUND,i,511);
        }

gsl_vector*  GlobalOptimizers::EvaluateModel(gsl_vector* x,int)
{
        //results gsl_vector definition and allocation:
        gsl_vector *y;
        y=gsl_vector_alloc(NOBJ+NCONS);

        double prod;
        int i;

        //Objective:
        gsl_vector_set(y,0,2);
        prod=1;
        for(i=0;i<NVAR;i++)
                {
                prod=prod*cos(gsl_vector_get(x,i)/sqrt(i+1.0));
                gsl_vector_set(y,0,gsl_vector_get(y,0)+pow(gsl_vector_get(x,i),2)/4000);
                }
        gsl_vector_set(y,0,gsl_vector_get(y,0)-prod);

        //Constraint:
        gsl_vector_set(y,1,gsl_vector_get(x,0)+gsl_vector_get(x,1)-10);		//x1+x2-10>=0
        if(gsl_vector_set(y,1)>0)
                gsl_vector_set(y,1)=0;			//returning 0 if constraint is satisfied

        return y;
}


//2. Bi-objective test function with linear constraint
//In void Globaloptimizers::InitializeOptimizer() NVAR,NCONS,NOBJ,LOWERBOUND and UPPERBOUND must be defined as:
NVAR=30; NOBJ=2; NCONS=1;
for(i=0;i<NVAR;i++)
        {
        gsl_vector_set(LOWERBOUND,i,0);
        gsl_vector_set(UPPERBOUND,i,1);
        }

gsl_vector*  GlobalOptimizers::EvaluateModel(gsl_vector* x,int)
{
        //results gsl_vector definition and allocation:
        gsl_vector *y;
        y=gsl_vector_alloc(NOBJ+NCONS);

        double g, h, alfa;
        int i;

        g=1;
        alfa=0.25;

        //Objectives:
        gsl_vector_set(y,0,	gsl_vector_get(x,0));

        for(i=1;i<NVAR;i++)
                g=g+(i+1)*gsl_vector_get(x,i)/(n-1);

        if((gsl_vector_get(y,0)/(5*g))<1)
                h=1-pow((gsl_vector_get(y,0)/(5*g)),alfa);
        else
                h=0;

        gsl_vector_set(y,1,g*h);

        //Constraint:
        gsl_vector_set(y,1,gsl_vector_get(x,0)+gsl_vector_get(x,1)-1);		//x1+x2-1>=0
        if(gsl_vector_set(y,1)>0)
                gsl_vector_set(y,1)=0;			//returning 0 if constraint is satisfied

        return y;
}



*/
