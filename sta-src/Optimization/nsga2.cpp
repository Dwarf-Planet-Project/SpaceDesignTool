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

//=========== Function of GlobalOptimizers class implementing NSGA-II algorithm =============//

This is a Multi-Objective GA program.
**********************************************************************
*  This program is the implementation of the NSGA-2 proposed by      *
*                                                                    *
*  Prof. Kalyanmoy Deb and his students .                            *
*                                                                    *
*  copyright Kalyanmoy Deb
**********************************************************************

Modification of the input and output structure to interface with STA made by F.Castellini.
Modifications are limited to the input/output, the algorithm's code is the same given by Prof. Deb.

NSGA-II complete description can be found in paper:
- Deb K., Evolutionary Algorithms for Multi-criterion optimization in Engineering Design,
KanGAL report, Kanpur Genetic Algorithms Laboratories, 1999

*/

//#include "stdafx.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "GlobalOptimizers.h"
#include <Eigen/Core>

using namespace Eigen;

#define square(x) ((x)*(x))
#define maxpop   1000  	/*Max population */
#define maxchrom 1000  	/*Max chromosome length*/
#define maxvar   1000  	/*Max no. of variables*/
#define maxfun   10  	/*Max no. of functions */
#define maxcons  20  	/*Max no. of Constraints*/

int gener,       		/*No of generations*/
  nvar,nchrom,   		/*No of variables*/
  ncons,         		/*No of Constraints*/
  vlen[maxvar],  		/*Array to store no of bits for each variable*/
  nmut,          		/*No of Mutations */
  ncross,        		/*No of crossovers*/
  ans;
double seed2,      		/*Random Seed*/
  pcross,        		/*Cross-over Probability*/
  pmut_b, pmut_r,		/*Mutation Probability*/
  lim_b[maxvar][2], lim_r[maxvar][2];/*Limits of variable in array*/
double di,        		/*Distribution Index for the Cross-over*/
  dim,           		/*Distribution Index for the Mutation*/
  delta_fit,     		/* variables required forfitness for fitness sharing */
  min_fit,
  front_ratio;
int optype,      		/*Cross-over type*/
  nfunc,         		/*No of functions*/
  sharespace;    		/*Sharing space (either parameter or fitness)*/

double coef[maxvar]; 	/*Variable used for decoding*/

static int popsize,  /*Population Size*/
  chrom;             /*Chromosome size*/

typedef struct       /*individual properties*/
{
  int genes[maxchrom], /*bianry chromosome*/
    rank,              /*Rank of the individual*/
    flag;              /*Flag for ranking*/
  double xreal[maxvar], /*list of real variables*/
    xbin[maxvar];      /*list of decoded value of the chromosome */
  double fitness[maxfun],/*Fitness values */
    constr[maxcons],     /*Constraints values*/
    cub_len,             /*crowding distance of the individual*/
    error;              /* overall constraint violation for the individual*/
}individual;        /*Structure defining individual*/


typedef struct
{
  int maxrank;            /*Maximum rank present in the population*/
  double rankrat[maxpop];  /*Rank Ratio*/
  int rankno[maxpop];     /*Individual at different ranks*/
  individual ind[maxpop], /*Different Individuals*/
    *ind_ptr;				//why does he have a pointer in addition to the fixed size array???
}population ;             /*Popuation Structure*/

#include "random.h"       /*Random Number Generator*/

#include "input.h"        /*File Takes Input from user*/

#include "realinit.h"     /*Random Initialization of the populaiton*/

#include "init.h"         /*Random Initialization of the population*/

#include "decode.h"       /*File decoding the binary strings*/

#include "ranking.h"      /*File Creating the Pareto Fronts*/

#include "rancon.h"       /*File Creating the Pareto Fronts when
			    Constraints are specified*/

#include "dfit.h"         /*File Assigning Dummyfitness*/

#include "select.h"       /*File for Tournament Selection*/

#include "crossover.h"    /*Binary Cross-over*/

#include "uniformxr.h"    /*Uniform Cross-over*/

#include "realcross2.h"   /*Real Cross-over*/

#include "mut.h"          /*Binary Mutation*/

#include "realmut1.h"     /*Real Mutation*/

#include "keepaliven.h"   /*File For Elitism and Sharing Scheme*/

population oldpop,
  newpop,
  matepop,
  *old_pop_ptr,
  *new_pop_ptr,
  *mate_pop_ptr;
/*Defining the population Structures*/

void GlobalOptimizers::nsga2()
{

	int counter1,counter2;

  /*Some Local variables to this Problem: Counters And some other pointers*/
  int i,j,l,maxrank1;
  double tot;

  old_pop_ptr = &(oldpop);

  nmut = 0;
  ncross = 0;

  /*Get the input from the file input.h*/
  input(NVAR,NOBJ,NCONS,MAXITER,NsolPareto,UPPERBOUND,LOWERBOUND);

  /*Initialize the random no generator*/
  seed2=(double)rand()/RAND_MAX;
  warmup_random(seed2);

   /*Binary Initializaton*/
  if (nchrom > 0)
    init(old_pop_ptr);
  if (nvar > 0)
    {
			
	if(InitMode==0)				//random initialization
	//I.a) RANDOMLY INIZIALIZE POPULATION 
		{
		realinit(old_pop_ptr);
		}
	else						//initialization from previous iteration or previous run
		{
		//I.b) LOAD	POPULATION VARIABLES FROM PAST ITERATION OR RUN RESULTS:
		for (i = 0 ; i < popsize ; i++)
			{
			for (j = 0; j < nvar; j++)
                                old_pop_ptr->ind[i].xreal[j]=OptimalSolutions(i,j);
			}
		}
	}

  old_pop_ptr = &(oldpop);

  // decode binary strings
  decode(old_pop_ptr);

  old_pop_ptr = &(oldpop);
  new_pop_ptr = &(newpop);

  for(j = 0;j < popsize;j++)
    {
      /*Initializing the Rank array having different individuals
	at a particular  rank to zero*/
       old_pop_ptr->rankno[j] = 0;
       new_pop_ptr->rankno[j] = 0;
    }

  old_pop_ptr = &(oldpop);
  Eigen::VectorXd var (NVAR);
  Eigen::VectorXd objconst (NOBJ+NCONS);

  if(InitMode==0 || InitMode==2)		//for random initialization of solutions loading from file (from previous run)
{
  /*File ptr to the file to store the value of the g for last iteration
    g is the parameter required for a particular problem
    Every problem is not required*/

double *realx_ptr, /*Pointer to the array of x values*/
    *binx_ptr,      /* Pointer to the binary variables */
    *fitn_ptr,      /*Pointer to the array of fitness function*/
    x[2*maxvar],     /* problem variables */
    f[maxfun],     /*array of fitness values*/
    *err_ptr,      /*Pointer to the error */
    cstr[maxcons];

  int i,j,k;
  double error, cc;

  old_pop_ptr->ind_ptr= &(old_pop_ptr->ind[0]);

  /*Initializing the max rank to zero*/
  old_pop_ptr->maxrank = 0;

  for(i = 0;i < popsize;i++)
    {
      old_pop_ptr->ind_ptr = &(old_pop_ptr->ind[i]);
      realx_ptr = &(old_pop_ptr->ind_ptr->xreal[0]);
      binx_ptr = &(old_pop_ptr->ind_ptr->xbin[0]);

      for(j = 0; j < nvar; j++)
	{ // Real-coded variables
	  x[j] = *realx_ptr++;
	}

      for(j = 0; j < nchrom; j++)
	{ // Binary-codced variables
	  x[nvar+j] = *binx_ptr++;
	}

      fitn_ptr = &(old_pop_ptr->ind_ptr->fitness[0]);
      err_ptr = &(old_pop_ptr->ind_ptr->error);



      /*   DO NOT CHANGE ANYTHING ABOVE   */
      /*----------------------CODE YOUR OBJECTIVE FUNCTIONS HERE------------*/
      /*All functions must be of minimization type, negate maximization
            functions */
      /*============Start Coding Your Function From This Point=============*/
      // Fitness functions:

                for(j=0;j<nvar;j++)
                        var(j)=x[j];
		printf("\nInitialization, individual %d.",i,j);
                objconst=EvaluateModel(var,0);					//evaluate model
		for (j=0;j<nfunc;j++)
                        f[j]=objconst(j);				//copy current objectives from gsl temp. vector
		for (j=0;j<ncons;j++)
                        cstr[j]=objconst(j+nfunc);		//copy current constraints from gsl temp. vector

	/*=========End Your Coding Upto This Point===============*/
      	/******************************************************************/
      	/*              Put The Constraints Here                          */
      	/******************************************************************/
      	// g(x) >= 0 type (normalize g(x) as in the cstr[1] below)
      	/*===========Start Coding Here=============*/


      /*===========Constraints Are Coded Upto Here=============*/
      /*   DO NOT CHANGE ANYTHING BELOW  */



      for(k = 0 ; k < nfunc ;k++)
	{
	  *fitn_ptr++  = f[k];
	}

      for (k = 0;k < ncons; k++)
	{
	  old_pop_ptr->ind_ptr->constr[k] = cstr[k];
	}
      error = 0.0;
      for (k = 0;k < ncons;k++)
	{
	  cc = cstr[k];
	  if(cc < 0.0)
	    error = error - cc;
	}
      *err_ptr = error;
    }

  /*---------------------------* RANKING *------------------------------*/

  if(ncons == 0)
    ranking(old_pop_ptr);
  else
    rankcon(old_pop_ptr);

}

  else									//for initialization from previous iteration --> results are already available
{
  /*File ptr to the file to store the value of the g for last iteration
    g is the parameter required for a particular problem
    Every problem is not required*/

  double *realx_ptr, /*Pointer to the array of x values*/
    *binx_ptr,      /* Pointer to the binary variables */
    *fitn_ptr,      /*Pointer to the array of fitness function*/
    x[2*maxvar],     /* problem variables */
    f[maxfun],     /*array of fitness values*/
    *err_ptr,      /*Pointer to the error */
    cstr[maxcons];

  int i,j,k;
  double error, cc;

  old_pop_ptr->ind_ptr= &(old_pop_ptr->ind[0]);

  /*Initializing the max rank to zero*/
  old_pop_ptr->maxrank = 0;

  for(i = 0;i < popsize;i++)
    {
      old_pop_ptr->ind_ptr = &(old_pop_ptr->ind[i]);
      realx_ptr = &(old_pop_ptr->ind_ptr->xreal[0]);
      binx_ptr = &(old_pop_ptr->ind_ptr->xbin[0]);

      for(j = 0; j < nvar; j++)
	{ // Real-coded variables
	  x[j] = *realx_ptr++;
	}

      for(j = 0; j < nchrom; j++)
	{ // Binary-codced variables
	  x[nvar+j] = *binx_ptr++;
	}

      fitn_ptr = &(old_pop_ptr->ind_ptr->fitness[0]);
      err_ptr = &(old_pop_ptr->ind_ptr->error);



      /*   DO NOT CHANGE ANYTHING ABOVE   */
      /*----------------------CODE YOUR OBJECTIVE FUNCTIONS HERE------------*/
      /*All functions must be of minimization type, negate maximization
            functions */
      /*============Start Coding Your Function From This Point=============*/
      // Fitness functions:

		for (j=0;j<nfunc;j++)
                        f[j]=ParetoFront(i,j);
		for (j=0;j<ncons;j++)
                        cstr[j]=ParetoFront(i,j+nfunc);

	/*=========End Your Coding Upto This Point===============*/
      	/******************************************************************/
      	/*              Put The Constraints Here                          */
      	/******************************************************************/
      	// g(x) >= 0 type (normalize g(x) as in the cstr[1] below)
      	/*===========Start Coding Here=============*/


      /*===========Constraints Are Coded Upto Here=============*/
      /*   DO NOT CHANGE ANYTHING BELOW  */



      for(k = 0 ; k < nfunc ;k++)
	{
	  *fitn_ptr++  = f[k];
	}

      for (k = 0;k < ncons; k++)
	{
	  old_pop_ptr->ind_ptr->constr[k] = cstr[k];
	}
      error = 0.0;
      for (k = 0;k < ncons;k++)
	{
	  cc = cstr[k];
	  if(cc < 0.0)
	    error = error - cc;
	}
      *err_ptr = error;
    }

  /*---------------------------* RANKING *------------------------------*/

  if(ncons == 0)
    ranking(old_pop_ptr);
  else
    rankcon(old_pop_ptr);

}

	if(InitMode==0 || InitMode==2)			//if it's not the continuation of a run
		{
		//Write initial objectives and constraints values to file:
		for(counter1=0;counter1<popsize;counter1++)
			{	
			for(counter2=0;counter2<nfunc;counter2++)
				fprintf(fpParetoFront,"%lf ",old_pop_ptr->ind[counter1].fitness[counter2]);
			for(counter2=0;counter2<ncons;counter2++)
				fprintf(fpParetoFront,"%lf ",old_pop_ptr->ind[counter1].constr[counter2]);
			fprintf(fpParetoFront,"\n");
			}
		}

	printf("\n\n POPULATION INITIALIZATION COMPLETED. \n\n");
  /********************************************************************/
  /*----------------------GENERATION STARTS HERE----------------------*/
	i=0;
	while(i<gener)													//stop only with number of iterations
    {
      old_pop_ptr = &(oldpop);
      mate_pop_ptr = &(matepop);

      /*--------SELECT----------------*/
      nselect(old_pop_ptr ,mate_pop_ptr );

      new_pop_ptr = &(newpop);
      mate_pop_ptr = &(matepop);

      /*CROSSOVER----------------------------*/
      if (nchrom > 0)
	{

	  if(optype == 1)
	    {
	      crossover(new_pop_ptr ,mate_pop_ptr );
	      /*Binary Cross-over*/
	    }

	  if(optype == 2)
	    {
	      unicross(new_pop_ptr ,mate_pop_ptr );
	      /*Binary Uniform Cross-over*/
	    }
	}
      if (nvar > 0)
	realcross(new_pop_ptr ,mate_pop_ptr );
      /*Real Cross-over*/


      /*------MUTATION-------------------*/
      new_pop_ptr = &(newpop);

      if (nchrom > 0)
	mutate(new_pop_ptr );
      /*Binary Mutation */

      if (nvar > 0)
	real_mutate(new_pop_ptr );
      /*Real Mutation*/

      new_pop_ptr = &(newpop);

      /*-------DECODING----------*/
      if(nchrom > 0)
	decode(new_pop_ptr );
      /*Decoding for binary strings*/

      /*----------FUNCTION EVALUATION-----------*/
      new_pop_ptr = &(newpop);
	  if(0==0)
	  {
  /*File ptr to the file to store the value of the g for last iteration
    g is the parameter required for a particular problem
    Every problem is not required*/

  double *realx_ptr, /*Pointer to the array of x values*/
    *binx_ptr,      /* Pointer to the binary variables */
    *fitn_ptr,      /*Pointer to the array of fitness function*/
    x[2*maxvar],     /* problem variables */
    f[maxfun],     /*array of fitness values*/
    *err_ptr,      /*Pointer to the error */
    cstr[maxcons];

  int j,k;
  double error, cc;

  new_pop_ptr->ind_ptr= &(new_pop_ptr->ind[0]);

  /*Initializing the max rank to zero*/
  new_pop_ptr->maxrank = 0;

  for(j=0;j<popsize;j++)
    {
      new_pop_ptr->ind_ptr = &(new_pop_ptr->ind[j]);
      realx_ptr = &(new_pop_ptr->ind_ptr->xreal[0]);
      binx_ptr = &(new_pop_ptr->ind_ptr->xbin[0]);

      for(k = 0; k < nvar; k++)
	{ // Real-coded variables
	  x[k] = *realx_ptr++;
	}

      for(k = 0; k < nchrom; k++)
	{ // Binary-codced variables
	  x[nvar+k] = *binx_ptr++;
	}

      fitn_ptr = &(new_pop_ptr->ind_ptr->fitness[0]);
      err_ptr = &(new_pop_ptr->ind_ptr->error);



      /*   DO NOT CHANGE ANYTHING ABOVE   */
      /*----------------------CODE YOUR OBJECTIVE FUNCTIONS HERE------------*/
      /*All functions must be of minimization type, negate maximization
            functions */
      /*============Start Coding Your Function From This Point=============*/
      // Fitness functions:

///
///

      //allocate memory for temp. vector
		for(k=0;k<nvar;k++)
                        var(k)=x[k];				//copy current variables to gsl temp. vector
		printf("\nGeneration %d, individual %d.",i+1,j);
                objconst=EvaluateModel(var,0);					//evaluate model
		for (k=0;k<nfunc;k++)
                        f[k]=objconst(k);				//copy current objectives from gsl temp. vector
		for (k=0;k<ncons;k++)
                        cstr[k]=objconst(k+nfunc);		//copy current constraints from gsl temp. vector

	/*=========End Your Coding Upto This Point===============*/
      	/******************************************************************/
      	/*              Put The Constraints Here                          */
      	/******************************************************************/
      	// g(x) >= 0 type (normalize g(x) as in the cstr[1] below)
      	/*===========Start Coding Here=============*/


      /*===========Constraints Are Coded Upto Here=============*/
      /*   DO NOT CHANGE ANYTHING BELOW  */



      for(k = 0 ; k < nfunc ;k++)
	{
	  *fitn_ptr++  = f[k];
	}

      for (k = 0;k < ncons; k++)
	{
	  new_pop_ptr->ind_ptr->constr[k] = cstr[k];
	}
      error = 0.0;
      for (k = 0;k < ncons;k++)
	{
	  cc = cstr[k];
	  if(cc < 0.0)
	    error = error - cc;
	}
      *err_ptr = error;
    }

  /*---------------------------* RANKING *------------------------------*/

  if(ncons == 0)
    ranking(new_pop_ptr);
  else
    rankcon(new_pop_ptr);

}

      /*-------------------SELECTION KEEPING FRONTS ALIVE--------------*/
      old_pop_ptr = &(oldpop);
      new_pop_ptr = &(newpop);
      mate_pop_ptr = &(matepop);

      /*Elitism And Sharing Implemented*/
      keepalive(old_pop_ptr ,new_pop_ptr ,mate_pop_ptr,i+1);

      mate_pop_ptr = &(matepop);
      if(nchrom > 0)
	decode(mate_pop_ptr );

      mate_pop_ptr = &(matepop);

      /*==================================================================*/

      /*----------------Rank Ratio Calculation------------------------*/
      new_pop_ptr = &(matepop);
      old_pop_ptr = &(oldpop);

      /*Finding the greater maxrank among the two populations*/

      if(old_pop_ptr->maxrank > new_pop_ptr->maxrank)
	maxrank1 = old_pop_ptr->maxrank;
      else
	maxrank1 = new_pop_ptr->maxrank;

      for(j = 0;j < maxrank1 ; j++)
	{
	  /*Sum of the no of individuals at any rank in old population
	    and the new populaion*/

	  tot = (old_pop_ptr->rankno[j])+ (new_pop_ptr->rankno[j]);

	  /*Finding the rank ratio for new population at this rank*/

	  new_pop_ptr->rankrat[j] = (new_pop_ptr->rankno[j])/tot;

	}

      /*=======Copying the new population to old population======*/

      old_pop_ptr = &(oldpop);
      new_pop_ptr = &(matepop);

      for(j = 0;j < popsize;j++)
	{
	  old_pop_ptr->ind_ptr = &(old_pop_ptr->ind[j]);
	  new_pop_ptr->ind_ptr = &(new_pop_ptr->ind[j]);
	  if(nchrom > 0)
	    {
	      /*For Binary GA copying of the chromosome*/

	      for(l = 0;l < chrom;l++)
		old_pop_ptr->ind_ptr->genes[l]=new_pop_ptr->ind_ptr->genes[l];

	      for(l = 0;l < nchrom;l++)
		old_pop_ptr->ind_ptr->xbin[l] = new_pop_ptr->ind_ptr->xbin[l];
	    }
	  if(nvar > 0)
	    {
	      /*For Real Coded GA copying of the chromosomes*/
	      for(l = 0;l < nvar;l++)
		old_pop_ptr->ind_ptr->xreal[l] = new_pop_ptr->ind_ptr->xreal[l];
	    }

	  /*Copying the fitness vector */
	  for(l = 0 ; l < nfunc ;l++)
	    old_pop_ptr->ind_ptr->fitness[l] = new_pop_ptr->ind_ptr->fitness[l];

	  /*Copying the dummy fitness*/
	  old_pop_ptr->ind_ptr->cub_len = new_pop_ptr->ind_ptr->cub_len;

	  /*Copying the rank of the individuals*/
	  old_pop_ptr->ind_ptr->rank = new_pop_ptr->ind_ptr->rank;

	  /*Copying the error and constraints of the individual*/

	  old_pop_ptr->ind_ptr->error = new_pop_ptr->ind_ptr->error;
	  for(l = 0;l < ncons;l++)
	    {
	      old_pop_ptr->ind_ptr->constr[l] = new_pop_ptr->ind_ptr->constr[l];
	    }

	  /*Copying the flag of the individuals*/
	  old_pop_ptr->ind_ptr->flag = new_pop_ptr->ind_ptr->flag;
	}   // end of j

      maxrank1 = new_pop_ptr->maxrank ;

      /*Copying the array having the record of the individual
	at different ranks */
      for(l = 0;l < popsize;l++)
	{
	  old_pop_ptr->rankno[l] = new_pop_ptr->rankno[l];
	}

      /*Copying the maxrank */
      old_pop_ptr->maxrank = new_pop_ptr->maxrank;


		//Write the variables values of current population to file
		for(counter1=0;counter1<popsize;counter1++)
			{	
			for(counter2=0;counter2<nfunc;counter2++)
				fprintf(fpParetoFront,"%lf ",old_pop_ptr->ind[counter1].fitness[counter2]);
			for(counter2=0;counter2<ncons;counter2++)
				fprintf(fpParetoFront,"%lf ",old_pop_ptr->ind[counter1].constr[counter2]);
			fprintf(fpParetoFront,"\n");
			}

		i++;		//increment i for the cycle
		printf("\n\n GENERATION %d COMPLETED. \n\n",i);
		}			//close cycle on the iterations (i)

  /*                   Generation Loop Ends                                */
  /************************************************************************/


	//COPY FINAL RESULTS TO VARIABLE OPTIMAL SOLUTION FOR THE REST OF THE CODE
	for(counter1=0;counter1<popsize;counter1++)
		{
		for(counter2=0;counter2<nvar;counter2++)
                        OptimalSolutions(counter1,counter2)=old_pop_ptr->ind[counter1].xreal[counter2];
		for(counter2=0;counter2<nfunc;counter2++)
                        ParetoFront(counter1,counter2)=old_pop_ptr->ind[counter1].fitness[counter2];
		for(counter2=0;counter2<ncons;counter2++)
                        ParetoFront(counter1,counter2+nfunc)=old_pop_ptr->ind[counter1].constr[counter2];
		}

	//Write variables values of final population to file:
	for(counter1=0;counter1<popsize;counter1++)
		{
		for(counter2=0;counter2<nvar;counter2++)
			fprintf(fpSolutions,"%lf ",old_pop_ptr->ind[counter1].xreal[counter2]);
		fprintf(fpSolutions,"\n");
		}
	}








