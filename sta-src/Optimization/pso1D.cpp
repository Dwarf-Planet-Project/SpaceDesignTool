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

------ Copyright (C) 2008 European Space Agency (space.trajectory.analysis AT gmail.com) ------
//------------------ Author: Francesco Castellini -------------------------------------------//
//------- Affiliation: Dipartimento di Ingegneria Aerospaziale, Politecnico di Milano -------//
//------------------------- E-mail: (castellfr@gmail.com) -----------------------------------//

//============ Function of GlobalOptimizers class implementing PSO 1D algorithm =============//

Implementation of the basic PSO single objective optimizer: constraints, linear inertia weight, constant parameters,
mutation on all variables of the mutated particle, only gbest and pbest (no lbest), no variables discretization, no mailboxes

Reference paper for original single-objective PSO:
- Eberhart J., Kennedy J., Particle Swarm Optimization, 1995
*/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "arrays.h"
#include "GlobalOptimizers.h"
#include <QDebug>

using namespace Eigen;

void GlobalOptimizers::pso1D()
{

/* ---------------------------------------------------- PRE-PROCESS -----------------------------------------------------*/

// ************************************************************************************************************************
// ************************************************************************************************************************
        //CHANGE HERE PSO PARAMETERS:
        int swarmsize=100;				//number of particles in the swarm
        double inertiainit=1.5;			//initial inertia parameter (linear)
        double inertiafinal=0.5;		//final inertia parameter (linear)
        double selfconf=1.5;				//self confidence parameter (constant)
        double swarmconf=1.5;			//swarm confidence parameter (constant)
        //double mutprob=0.01;			//mutation probability (constant)
        double mutprob=0.05;			//mutation probability (constant)
        //DO NOT CHANGE ANYTHING BELOW
// ************************************************************************************************************************
// ************************************************************************************************************************

        int nvar=NVAR;					//number of variables
        int ncons=NCONS;				//number of constraints
        int maxiter=MAXITER;			//number of iterations

        //Local variables:
        double **vel,**swarm,**pbest,*gbest,*Jswarm,*Jpbest,Jgbest,**Cswarm,**Cpbest,*Cgbest,
                                tempselfconf,tempswarmconf,tempinertia,temprand,*LB,*UB,penalty,tmpJgbest;
        int i,j,k;

        //memory allocations:
        LB=(double*)malloc(nvar*sizeof(double));			//lower boundaries for optimization variables
        UB=(double*)malloc(nvar*sizeof(double));			//upper boundaries for optimization variables
        swarm=dmatrix_allocation(swarmsize,nvar);			//particles positions (position=optimization variables)
        pbest=dmatrix_allocation(swarmsize,nvar);			//personal best positions
        gbest=(double*)malloc(nvar*sizeof(double));			//global best position
        Jswarm=(double*)malloc(swarmsize*sizeof(double));	//function values for the swarm
        Jpbest=(double*)malloc(swarmsize*sizeof(double));	//personal best function value
        Cswarm=dmatrix_allocation(swarmsize,ncons);			//constraint values for the swarm
        Cpbest=dmatrix_allocation(swarmsize,ncons);			//constraint values for the pbest
        Cgbest=(double*)malloc(ncons*sizeof(double));		//gbest constraints values
        vel=dmatrix_allocation(swarmsize,nvar);				//particles velocities

        //Boundaries definition:
        for(i=0;i<nvar;i++)
                {
                LB[i]=LOWERBOUND(i);
                UB[i]=UPPERBOUND(i);
                }

        /* ---------------------------------------------------------------- MAIN ------------------------------------------------------------------------*/
        //random inizialization of the swarm position (uniform distribution over the search space), first evaluation and inizialization of pbest and  gbest
        Jgbest=huge*huge;	//initial high number for Jgbest
        if(InitMode==0)		//random initialization
        //I.a) RANDOMLY INIZIALIZE SWARM POSITION, (uniform distribution over the search space), VELOCITY (null) and PBEST (equal to the swarm)
                {
                for(i=0;i<swarmsize;i++)
                        {
                        for(j=0;j<nvar;j++)
                                {
                                temprand=((double)rand()/RAND_MAX);			//define random number in [0;1]
                                swarm[i][j]=LB[j]+(UB[j]-LB[j])*temprand;	//positions random initialization
                                pbest[i][j]=swarm[i][j];					//initial pbest=initial swarm
                                vel[i][j]=0;								//null initial velocity
                                }
                        }
                }
        else if(InitMode==1)		//initialization from previous iteration
                {
                //I.b) LOAD GBEST AND SWARM FROM PAST RESULTS:
                //COPY GBEST FROM PREVIOUS ITERATION:
                for(j=0;j<nvar;j++)
                        gbest[j]=OptimalSolutions(0,j);			//copy variables
                Jgbest=ParetoFront(0,0);						//copy objective
                for(j=0;j<ncons;j++)
                        Cgbest[j]=ParetoFront(0,j+1);			//copy constraints
                //COPY SWARM FROM PREVIOUS ITERATION:
                for(i=0;i<swarmsize;i++)
                        {
                        for(j=0;j<nvar;j++)
                                {
                                swarm[i][j]=OldSwarm(i,j);			//copy variables
                                pbest[i][j]=OldPbest(i,j);			//copy pbests
                                vel[i][j]=OldVel(i,j);				//copy velocities
                                }
                        }
                }
        else						//initialization from previous run
                {
                //I.C) LOAD GBEST OF PREVIOUS RUN (ONLY VARIABLES) AND USE IT AS FIRST PARTICLE:
                //TAKE GBEST SOLUTION AS FIRST SWARM PARTICLE:
                for(j=0;j<nvar;j++)									//TAKE SOLUTION FROM FILE AS FIRST SWARM PARTICLE
                        {
                        swarm[0][j]=OptimalSolutions(0,j);		//copy variables
                        pbest[0][j]=swarm[0][j];								//pbest is the same
                        vel[0][j]=0;											//velocity is 0
                        }
                //RANDOMLY INITIALIZE THE OTHER SWARM PARTICLES:
                for(i=1;i<swarmsize;i++)							//AND TAKE THE OTHERS AT RANDOM
                        {
                        for(j=0;j<nvar;j++)
                                {
                                temprand=((double)rand()/RAND_MAX);			//define random number in [0;1]
                                swarm[i][j]=LB[j]+(UB[j]-LB[j])*temprand;	//positions random initialization
                                pbest[i][j]=swarm[i][j];					//initial pbest=initial swarm
                                vel[i][j]=0;								//null initial velocity
                                }
                        }
                }
        //IF THE OPTIMIZATION DOES NOT CONTINUE FROM ITERATIONS --> Perform first evaluation for the swarm particles
        // AND check gbest:
        Eigen::VectorXd var (NVAR);
        Eigen::VectorXd objconst (NOBJ+NCONS);//allocate memory for temp. gsl vector

        if(InitMode==0 || InitMode==2)
                {
                //Perform first evaluation for each particle AND check gbest:
                for(i=0;i<swarmsize;i++)
                        {
                        for(j=0;j<nvar;j++)
                                var(j)=swarm[i][j];			//copy current swarm to temp. gsl vector
                        printf("\nInitialization, particle %d.",i);
                        objconst=EvaluateModel(var,0);					//evaluate model
                        penalty=0;
                        for (j=0;j<ncons;j++)								//copy constraints from temp. gsl vector
                                {
                                Cswarm[i][j]=objconst(j+1);		//NOTE: constraints MUST be <0 for violations and =0 for satisfaction
                                Cpbest[i][j]=Cswarm[i][j];		//initial Cpbest value for each solution
                                penalty+=Cswarm[i][j];			//adding current constraint violation to penalty
                                }
                        Jswarm[i]=objconst(0)-penalty*huge;	//adding a huge value if the constraint is violated
                        Jpbest[i]=Jswarm[i];				//initial Jpbest value for each solution
                        if (Jswarm[i]<Jgbest)				//Updating gbest if the function value is lower than previous gbest
                                {
                                Jgbest=Jswarm[i];
                                for (j=0;j<ncons;j++)
                                        Cgbest[j]=Cswarm[i][j];		//initial Cgbest values
                                for(j=0;j<nvar;j++)
                                        gbest[j]=swarm[i][j];		//initial gbest values
                                }
                        }
                //Write initial objectives and constraints values to file:
                penalty=0;
                for(j=0;j<ncons;j++)
                        penalty+=Cgbest[j];
                tmpJgbest=Jgbest+penalty*huge;

                fprintf(fpParetoFront,"%lf ",tmpJgbest);
                for(j=0;j<ncons;j++)
                        fprintf(fpParetoFront,"%lf ",Cgbest[j]);
                fprintf(fpParetoFront,"\n");
                }

        printf("\n\n SWARM INITIALIZATION COMPLETED. \n\n");

        //MAIN CYCLE ON k: maxiter iterations of the PSO algorithm (no other stop criterion)
        for(k=1;k<maxiter+1;k++)
                {
                tempinertia=(inertiafinal-inertiainit)/maxiter*k+inertiainit;
                for(i=0;i<swarmsize;i++)	//for each particle i at iteration k
                        {
                        temprand=((double)rand()/RAND_MAX);
                        tempselfconf=temprand*selfconf;		//randomizing the value of self confidence for the particle i at iteration k
                        temprand=((double)rand()/RAND_MAX);
                        tempswarmconf=temprand*swarmconf;		//randomizing the value of swarm confidence for the particle i at iteration k
                        for(j=0;j<nvar;j++)	//for each decision variable j of particle i at iteration k
                                {
                                 vel[i][j]=tempinertia*vel[i][j]+tempselfconf*(pbest[i][j]-swarm[i][j])+tempswarmconf*(gbest[j]-swarm[i][j]);	//velocity update
                                 swarm[i][j]=swarm[i][j]+vel[i][j];			//position update
                                 //Restraint the position within the boundaries:
                                 if (swarm[i][j]<LB[j])
                                        {
                                        swarm[i][j]=LB[j];		//set variable to lower limit
                                        vel[i][j]=-vel[i][j];	//invert velocity that leads out of the boudaries
                                        }
                                 else if (swarm[i][j]>UB[j])
                                        {
                                        swarm[i][j]=UB[j];		//set variable to upper limit
                                        vel[i][j]=-vel[i][j];	//invert velocity that leads out of the boudaries
                                        }
                                }

                        //Mutation:
                        temprand=((double)rand()/RAND_MAX);
                        if (temprand<mutprob)		//decide if mutate or not the particle
                                for(j=0;j<nvar;j++)	//for each decision variable j of particle i at iteration k
                                        {
                                        temprand=((double)rand()/RAND_MAX);
                                        swarm[i][j]=LB[j]+(UB[j]-LB[j])*temprand;	//position random reset is used as mutation
                                        }

                        //Function and constraints evaluation:
                        for(j=0;j<nvar;j++)
                                var(j)=swarm[i][j];			//copy current swarm to temp. gsl vector
                        printf("\nIteration %d, particle %d.",k,i);
                        objconst=EvaluateModel(var,0);
                        penalty=0;
                        for (j=0;j<ncons;j++)								//copy constraints from temp. gsl vector
                                {
                                Cswarm[i][j]=objconst(j+1);		//NOTE: constraints MUST be <0 for violations and =0 for satisfaction
                                penalty+=Cswarm[i][j];			//adding current constraint violation to penalty
                                }
                        Jswarm[i]=objconst(0)-penalty*huge;	//adding a huge value if the constraint is violated

                        //Personal best update:
                        if (Jswarm[i]<Jpbest[i])			//if the current J value is lower the previous Jpbest
                                {
                                Jpbest[i]=Jswarm[i];			//Jpbest value update
                                for(j=0;j<nvar;j++)
                                        pbest[i][j]=swarm[i][j];	//pbest update
                                for(j=0;j<ncons;j++)
                                        Cpbest[i][j]=Cswarm[i][j];	//Cpbest update
                                }

                        //Global best update:
                        if (Jswarm[i]<Jgbest)				//if the J value is lower than previous Jgbest
                                {
                                Jgbest=Jswarm[i];				//Jgbest update
                                for(j=0;j<nvar;j++)
                                        gbest[j]=swarm[i][j];		//gbest update
                                for (j=0;j<ncons;j++)
                                        Cgbest[j]=Cswarm[i][j];		//Cgbest update
                                }
                        }	//close for cycle on the particles

                //Write the objectives and constraints values of current gbest to file:
                penalty=0;
                for(j=0;j<ncons;j++)
                        penalty+=Cgbest[j];
                tmpJgbest=Jgbest+penalty*huge;

                fprintf(fpParetoFront,"%lf ",tmpJgbest);
                for(j=0;j<ncons;j++)
                        fprintf(fpParetoFront,"%lf ",Cgbest[j]);
                fprintf(fpParetoFront,"\n");

                printf("\n\n ITERATION %d COMPLETED. \n",k);

                }	//close for cycle on the iterations


        /* ------------------------------------------------------------- POST PROCESS ----------------------------------------------------------------------*/
        //COPY FINAL RESULTS FOR THE NEXT OPTIMIZATION RUN OR FOR THE FINALIZATION:
        for(j=0;j<nvar;j++)
                OptimalSolutions(0,j)=gbest[j];		//copy gbest variables
                ParetoFront(0,0)=Jgbest;					//copy gbest objective
        for(j=0;j<ncons;j++)
                ParetoFront(0,j+1)=Cgbest[j];		//copy gbest constraints
        for(i=0;i<swarmsize;i++)
                for(j=0;j<nvar;j++)
                        {
                        OldSwarm(i,j)=swarm[i][j];		//copy swarm variables
                        OldPbest(i,j)=pbest[i][j];		//copy swarm pbests
                        OldVel(i,j)=vel[i][j];			//copy swarm velocities
                        }

        //Write variables values of final gbest to file:
        for(j=0;j<nvar;j++)
                fprintf(fpSolutions,"%lf ",gbest[j]);

}


