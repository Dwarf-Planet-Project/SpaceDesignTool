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

//=========== Function of GlobalOptimizers class implementing DG-MOPSO algorithm ============//

 Implementation of Double Grid MOPSO algorithm: the method is a modification of the original MOPSO algorithm developed by
	prof. Coello Coello and his assistants. See papers:

	- Coello Coello C. A., "A proposal for multiple objective Particle Swarm Optimization Method", 2002
	- Coello Coello C.A., Toscano Pulido G., Lechuga M.S., "Handling multiple objectives with Particle Swarm Optimization", 2004

	DG-MOPSO has an external archive of variable size, which contains non-dominated solutions only; the main difference with 
	respect to MOPSO is represented by the archive pruning and leaders selection, which are performed on the basis of a 
	diversity criterion obtained repeating two times PAES adaptive grid technique. See paper proposing PAES algorithm: 

	- Knowles J.D., Corne D.W., "Approximating the Non-dominated Front Using the Pareto Archived Evolution Strategy", 2000.

	The complete description and validation of DG-MOPSO can be found in the MSc thesis: 
	
	- F.Castellini, "Global Optimization Techniques for Space Missions Design", Politecnico di Milano, 2008.

	--> Characteristics of this implementation:
   Real coding, constraints treated with penalty method. Linear variation is possible for all algorithm's parameters, 
   Only gbest and pbest (no lbest). Stop conditions based on number of iterations only.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <search.h>
#include <Eigen/Core>
#include "GlobalOptimizers.h"
#include "arrays.h"

using namespace std;
using namespace Eigen;

// ************************************************************************************************************************
// ************************************************************************************************************************
// ************************************************************************************************************************
// ************************************************** DG-MOPSO FUNCTIONS: *************************************************
// ************************************************************************************************************************
// ************************************************************************************************************************
// ************************************************************************************************************************

int* integer2binary(int n, int nbits)
{
/*Function to convert an integer number (in base 10) to a binary number (base 2)
INPUT:	n: number to be converted
		nbits: number of bits of the output binary number
OUTPUT:	binary (nbits x 1): vector of bits (integers 0/1)	*/
  	int *binary, i;

	binary=(int*)malloc(nbits*sizeof(int));
 	for(i=0; i<nbits; i++)
		{
		binary[i]=n/pow(2.0,nbits-i-1);
		n=n%(int)pow(2.0,nbits-i-1);
		}

   	return binary;
}

int binary2integer(int *binary, int nbits)
{
  	/*Function to convert a binary number in base 10
INPUT: 	binary (nbits x 1): vector of bits (integers 0/1)
		nbits: number of bits
OUTPUT: res: converted integer number*/
int i, res;

	res=0;
 	for(i=0; i<nbits; i++)
		res = res + pow(2.0,i)*binary[nbits-i-1];		//computing the number in base 10: the first bit is the one on the right (the last of the vector)

   	return res;
}



void check_boundaries_real(double **position, double **velocity, int i, int nvar, double *LB, double *UB)
{
//INPUT: position and velocity matrixes, index of the particle to be checked
//OUTPUT: updated position and velocity matrixes (only particle i is updated)
	int j;

	for(j=0;j<nvar;j++)							//for each variable
		if (position[i][j]<LB[j])
			{
			position[i][j]=LB[j];				//set variable to lower limit
		 	velocity[i][j]=-velocity[i][j];		//invert velocity that leads out of the boudaries
			}
		else if (position[i][j]>UB[j])
			{
			position[i][j]=UB[j];				//set variable to upper limit
		 	velocity[i][j]=-velocity[i][j];		//invert velocity that leads out of the boudaries
			}
}



int checkgridbound(double **Jarchive,int *erase,int counterase,double **gridbound,double *newsol,int pos,int nobj,int *minB,int *maxB)
{
//function that finds the maximum and minimum boundaries of a given set in all dimensions
//INPUT: Jarchive (size x nobj): archive solutions before erasing the dominated ones
//		 erase (counterase x 1): indexes of the dominated solutions to be erased
//		 counterase: number of dominated solutions to be erased
//		 gridbound (nobj x 2): grid boundaries before erasing the dominated ones
//		 newsol (nobj x 1): new solution to be added (objective functions)
//		 pos: position in the current archive of the new solution to be added
//		 minB (nobj x 1): boundary solutions that define the minimum value in each dimension
//		 max B (nobj x 1): boundary solutions that define the maximum value in each dimension
//OUTPUT: check=0 if the solutions to be erased are all internal solutions --> in this case the gridbound is modified to take into account the new solution
//		  check=1 if at least one of the solutions to be erased is a boundary solution --> gridbound is NOT modified--> it will have to be modified outside
	int i,j,check=0;

//printf("\n UFF! Obj.1 LB:%lf; UB:%lf -- Obj.2 LB:%lf; UB:%lf",gridbound[0][0],gridbound[0][1],gridbound[1][0],gridbound[1][1]);			//PC
//printf("\n UFF2! %lf %lf",newsol[0],newsol[1]);																							//PC

	for(i=0;i<counterase;i++)										//cycle to check if at least one of the solutions to be erased is a boundary solution
		{
		if(check==1)
			break;
		for(j=0;j<nobj;j++)
			if(minB[j]==erase[i] || maxB[j]==erase[i])				//if one of the solutions to be erased is an extreme solution
				{
				check=1;											// --> and set to 1 the check variable to exit cycle on the solutions to be erased
				break;
				}
		}

	if(check==0)													//if no boundary solution has to be erased --> check the solution to be added
		for(j=0;j<nobj;j++)
			{
			if(newsol[j]<gridbound[j][0])							//check if the new solution is a lower boundary for the current grid
				{
				gridbound[j][0]=newsol[j];
				minB[j]=pos;
				}
			if(newsol[j]>gridbound[j][1])							//check if the new solution is an upper boundary for the current grid
				{
				gridbound[j][1]=newsol[j];
				maxB[j]=pos;
				}
			}

	return check;
}


void define_fitness_in(int Nocc_out, int *gridocc_out, int *Nocc_in, int **gridocc_in, int **gridNO_in, double **fitness_in)
{
// Function that defines the inner grid that has to be selected in each of the outer grids: one (randomly chosen) of the non-empty grids with the minimum
// number of solutions contained is always selected in deterministic selection; stochastic selection with roulette wheel selection involves the definition
// of the inner fitness as a matrix, but it's not used.
//INPUT: Nocc_out: number of non-empty outer grids (address call to the function)
//		 gridocc_out (ngrids_out x 1): list of all the outer grid zones occupied by at least one solution
//		 Nocc_in (ngrids_out x 1): number of non-empty inner grids for each outer grid
//		 gridocc_in (ngrids_out x ngrids_in): list of all the inner grid zones of each outer grid zone occupied by at least one solution
//		 gridNO_in (ngrids_out x ngrids_in): number of solutions in each inner grid zone of each outer grid zone. Rows: outer grid numbers; Columns: inner
//											 grid zones numbers
//OUTPUT:fitness_in (ngrids_out x ngrids_in): fitness of each non-empty inner grid of each non-empty outer grid
//		 fitness_in(i,j): fitness of the j-th non-empty inner grid of the i-th non-empty outer grid
	int i,j;

	for(i=0;i<Nocc_out;i++)															//cycle on non-empty outer grids
		for(j=0;j<Nocc_in[gridocc_out[i]];j++)										//cycle on non-empty inner grids of i-th non-empty outer grid
			fitness_in[i][j]=10.0/gridNO_in[gridocc_out[i]][gridocc_in[i][j]];		//define fitness
}


void define_fitness_out(int Nocc_out, int *gridNO_out, int *gridocc_out, double *fitness_out)
{
//Function that defines the fitness of the outer grids according to the number of solutions in each of them
//INPUT: Nocc_out: number of non-empty outer grids
//		 gridNO_out (ngrids x 1): number of solutions in each of the outer grids
//		 gridocc_out (ngrids x 1): list of indexes of the non-empty outer grids
//OUTPUT:fitness_out (Nocc_out x 1): list of fitness values for the non-empty outer grids
	int j;

	for(j=0;j<Nocc_out;j++)			//define fitness of the outer grids
		fitness_out[j]=10.0/gridNO_out[gridocc_out[j]];
}


int roulette_wheel_selection(double *fitness, int size)
{
//Function performing roulette wheel selection of an element in a vector basing on a fitness value
//INPUT: fitness and size of the vector
//OUTPUT: winner: index of the selected solution
	int winner, i, LB, UB, cut;
	double temprand,totfit=0,*cumfitness;
	cumfitness=(double*)malloc((size+1)*sizeof(double));

	for(i=0;i<size;i++)
		totfit+=fitness[i];

	cumfitness[0]=0;
	for(i=1;i<size;i++)
		cumfitness[i]=cumfitness[i-1]+fitness[i-1]/totfit;
	cumfitness[size]=1;

	temprand=(double)rand()/RAND_MAX;

	LB=0;
	UB=size;
	i=0;
	while (i==0)					//find the position of the random number in on the roulette wheel by bisection
		{
		cut=(LB+UB)/2;
		if((temprand>=cumfitness[cut])&&(temprand<=cumfitness[cut+1]))
			{
			winner=cut;
			i=1;
			}
		else if(temprand>cumfitness[cut])
			LB=cut+1;
		else
			UB=cut;
		}

	return winner;
}



int dgchoose_leader(int solgrid_out,double *fitness_out,double **fitness_in,int *archivegrid_out,int *archivegrid_in,int **gridID_out,int ***gridID_in,int *gridNO_out,int *gridocc_out,int *gridoccref_out,int **gridocc_in,int Nocc_out,int *Nocc_in)
{
//Function finding the leader of a particle according to the double grid strategy
//INPUT: solgrid_out: outer grid the particle belongs to
//		 fitness_out(ngrids_out): outer grids fitness (1/Number of solutions in the grid)
//		 fitness_in(ngrids_out x ngrids_in): inner grids fitness (1/Number of solutions in the grid) --> NOT USED!!! this is for stochastic selection
//		 fitness_in(ngrids_out x 1): inner grid to be selected in each outer grid: the first of the less crowded ones is always selected --> USED! this is for deterministic selection
//		 archivegrid_out (size x 1): list of outer grid zones each solution in the archive belongs to
//		 archivegrid_in (size x 1): list of inner grid zones each solution in the archive belongs to
//		 gridID_out (ngrids_out x size): solutions in each outer grid zone. Rows: grid zones numbers; Columns: IDs of the solutions in the selected outer
//										 grid zone
//		 gridID_in (ngrids_out x ngrids_in x size): solutions in each inner grid zone. Pages: outer grid numbers; Rows: inner grid zones numbers; Columns:
//													IDs of the solutions in selected outer&inner grid zone
//		 gridNO_out (ngrids_out x 1): number of solutions in each outer grid zone
//		 gridocc_out (ngrids_out x 1): list of all the outer grid zones occupied by at least one solution
//		 gridoccref_out (ngrids_outF x 1): reference of the memory location occupied in gridocc_out by each of the outer grids
//		 gridocc_in (ngrids_out x ngrids_in): list of all the inner grid zones of each outer grid zone occupied by at least one solution
//		 Nocc_out: number of non-empty outer grids (address call to the function)
//		 Nocc_in (ngrids_out x 1): number of non-empty inner grids for each outer grid
//OUTPUT: leader: selected leader for the given solution
	double temprand;
	int j,leadergrid_out,leadergrid_in,leader,max,row;

	if(gridNO_out[solgrid_out]==0)		//if there's no archive solution in the same outer grid of the particle --> perform single grid leader selection
		{
//		printf("\nSingle grid selection");																								//PC

		if(Nocc_out==1)					//if there's only one non-empty outer grid, no need to perform roulette wheel selection
			leadergrid_out=0;			//select the first (and only) outer grid
		else							//else perform roulette wheel selection of the non-empty outer grids:
			leadergrid_out=roulette_wheel_selection(fitness_out,Nocc_out);

		//Simplified version: always select the first solution in the selected outer grid (it's not important to randomize the choice):
		leader=gridID_out[gridocc_out[leadergrid_out]][0];

		}

	else								//if there are archive solutions in the same outer grid of the particle --> perform double grid leader selection
		{
//		printf("\nDouble grid selection %d %d",Nocc_in[solgrid_out],gridNO_out[solgrid_out]);											//PC
		if (Nocc_in[solgrid_out]==1)	//if there's only one non-empty inner grid in the selected outer grid, no need to perform roulette wheel selection
			{
			leadergrid_in=0;			//select the first (and only) inner grid
			row=gridoccref_out[solgrid_out];
			}
		else							//else perform roulette wheel selection of the non-empty inner grids of the selected outer grid:
			{
			row=gridoccref_out[solgrid_out];

			//Deterministic selection of the leadergrid: the less crowded one is always chosen (data taken from vector fitness_in):
			max=0;
			temprand=(double)rand()/RAND_MAX;
			for(j=0;j<Nocc_in[solgrid_out];j++)
				if(fitness_in[row][j]>max)
					{
					max=fitness_in[row][j];
					leadergrid_in=j;
					}
				else if (fitness_in[row][j]==max && temprand<0.5)		//if there are more inner grids with the same number of solutions --> choose randomly
					{
					leadergrid_in=j;
					temprand=(double)rand()/RAND_MAX;				//change random number to maintain randomity
					}
			}
		//Simplified version: always select the first solution in the selected inner grid (it's not important to randomize the choice):
		leader=gridID_in[solgrid_out][gridocc_in[row][leadergrid_in]][0];
		}
	return leader;
}

void erase_sol(double **archive,double **Jarchive,double **Carchive,int j_erase,int l_erase,int sol_erase,int size,int *Nocc_out,int *Nocc_in,int *archivegrid_out,int *archivegrid_in,int **gridID_out,int ***gridID_in,int *gridNO_out,int **gridNO_in,int *gridocc_out,int **gridocc_in,int *gridoccref_out,int **gridoccref_in,int **gridIDref_out,int ***gridIDref_in,double *fitness_out,double **fitness_in,int nvar,int nobj,int ncons,int *miB, int *maB)
{
//Function updating archive, grid structure and fitness values after comparison with a non-dominated swarm solution.
//Vectors and matrixes lengths: F-->fixed,always the same; NF-->non-fixed, depending on the number of solutions in the grid
//INPUT: archive, Jarchive: archive...
//		 j_erase: element in gridocc_out of the outer grid containing the inner grid where one solution at random has to be erased
//		 l_erase: column in gridocc_in of the inner grid where one solution at random has to be erased
// --> (gridocc_out[j_erase], gridocc_in[j_erase][l_erase]) is the (outer,inner) grid to be erased
//		 size: current dimension of the archive
//		 Nocc_out: number of non-empty outer grids (address call to the function)
//		 Nocc_in (ngrids_outF x 1): number of non-empty inner grids for each outer grid
//		 archivegrid_out (sizeF x 1): outer grid position of each archive solution
//		 archivegrid_in (sizeF x 1): inner grid position of each archive solution
//		 gridID_out (ngrids_outF x sizeNF): solutions in each outer grid zone. Rows: grid zones numbers; Columns: IDs of the solutions in the selected outer
//										 grid zone
//		 gridID_in (ngrids_outF x ngrids_inF x sizeNF): solutions in each inner grid zone. Pages: outer grid numbers; Rows: inner grid zones numbers; Columns:
//													IDs of the solutions in selected outer&inner grid zone
//		 gridNO_out (ngrids_outF x 1): number of solutions in each outer grid zone
//		 gridNO_in (ngrids_outF x ngrids_inF): number of solutions in each inner grid zone of each outer grid zone. Rows: outer grid numbers; Columns: inner
//											 grid zones numbers
//		 gridocc_out (ngrids_outNF x 1): list of all the outer non-empty grids
//		 gridocc_in (ngrids_outNF x ngrids_inNF): list of all the inner grid zones of each outer grid zone occupied by at least one solution
//		 gridoccref_out (ngrids_outF x 1): reference of the memory location occupied in gridocc_out by each of the outer grids
//		 gridoccref_in (ngrids_outF x ngrids_inF): reference of the memory location occupied in gridocc_in by each of the inner grids
//		 gridIDref_out (ngrids_outF x sizeF): gridIDref_out(i,j) = position in gridID_out(i,:) of archive solution j (non-null only if the solution j belongs to outer grid i)
//		 gridIDref_in (ngrids_outF x ngrids_inF x sizeF): gridIDref_in(i,j,k) = position in gridID_in(i,j,:) of archive solution k (non-null only if the solution k belongs to inner grid j of outer grid i)
//		 fitness_out (ngrids_outNF x 1): fitness of the non-empty outer grids
//		 fitness_in (ngrids_outNF x ngrids_inNF): fitness of the non-empty inner grids of the non-empty outer grids
//		 nobj, nvar: number of objectives and variables
//		 miB (nobj x 1): boundary solutions that define the minimum value in each dimension
//		 maB (nobj x 1): boundary solutions that define the maximum value in each dimension
//OUTPUT:updates in all inputs, after erasing of solution (j,l)
	int i,j,k,l;

	//Update the outer grid structure:
	gridNO_out[archivegrid_out[sol_erase]]--;									//update number of sol. in outer grid of the archive solution to be erased
	if(gridNO_out[archivegrid_out[sol_erase]]==0)								//IF there are no more solutions in that grid --> erase it
		{
		(*Nocc_out)--;															//lower the number of non-empty outer grids
		for(l=j_erase;l<(*Nocc_out);l++)						//move one position backward the outer grids in gridocc_out after the one to be elimated
			{
			gridocc_out[l]=gridocc_out[l+1];									//move list of non-empty outer grids
			gridoccref_out[gridocc_out[l]]--;									//move the position in gridocc_out of the moved grid
			fitness_out[l]=fitness_out[l+1];									//move the outer fitness
			//all elements of gridocc_in and gridoccref_in in the rows after the one of the outer grid became empty must be shifted 1 row up:
			for(k=0;k<Nocc_in[gridocc_out[l]];k++)		//for each of the non-empty inner grids in outer grid l
				{
				gridocc_in[gridoccref_out[gridocc_out[l]]][k]=gridocc_in[gridoccref_out[gridocc_out[l]]+1][k];	//update list of non-empty inner grids
				gridoccref_in[gridocc_out[l]][gridocc_in[gridoccref_out[gridocc_out[l]]][k]]=k;				//update the position in gridocc_in of the moved grid
				fitness_in[gridoccref_out[gridocc_out[l]]][k]=fitness_in[gridoccref_out[gridocc_out[l]]+1][k];	//update the inner fitness
				}
			}
		}
	else																		//ELSE --> just modify fitness and grid solutions
		{
		for(l=gridIDref_out[archivegrid_out[sol_erase]][sol_erase];l<gridNO_out[archivegrid_out[sol_erase]];l++)	//move one position backward the solutions in gridID_out after the one to be elimated
			{
			gridID_out[archivegrid_out[sol_erase]][l]=gridID_out[archivegrid_out[sol_erase]][l+1];	//move solution
			gridIDref_out[archivegrid_out[sol_erase]][gridID_out[archivegrid_out[sol_erase]][l]]--;	//move reference to position in gridID_out of the solution
			}
		fitness_out[j_erase]=10.0/gridNO_out[archivegrid_out[sol_erase]];		//modify the fitness
		}

	//Update the inner grid variables:
	gridNO_in[archivegrid_out[sol_erase]][archivegrid_in[sol_erase]]--;			//update number of sol. in inner grid of the archive solution to be erased
	if(gridNO_in[archivegrid_out[sol_erase]][archivegrid_in[sol_erase]]==0)		//IF there are no more solutions in that grid --> erase it
		{
		Nocc_in[archivegrid_out[sol_erase]]--;									//lower the number of non-empty inner grids
		for(l=l_erase;l<Nocc_in[archivegrid_out[sol_erase]];l++)	//move one position backward the inner grids in gridocc_in after the one to be elimated
			{
			gridocc_in[j_erase][l]=gridocc_in[j_erase][l+1];					//move list of non-empty inner grids
			gridoccref_in[archivegrid_out[sol_erase]][gridocc_in[j_erase][l]]--;//move the position in gridocc_in of the moved grid
			fitness_in[j_erase][l]=fitness_in[j_erase][l+1];					//move the inner fitness
			}
		}
		else																	//ELSE --> just modify fitness
			{
			for(l=gridIDref_in[archivegrid_out[sol_erase]][archivegrid_in[sol_erase]][sol_erase];l<gridNO_in[archivegrid_out[sol_erase]][archivegrid_in[sol_erase]];l++)	//move one position backward the solutions in gridID_out after the one to be elimated
				{
				gridID_in[archivegrid_out[sol_erase]][archivegrid_in[sol_erase]][l]=gridID_in[archivegrid_out[sol_erase]][archivegrid_in[sol_erase]][l+1];	//move solution
				gridIDref_in[archivegrid_out[sol_erase]][archivegrid_in[sol_erase]][gridID_in[archivegrid_out[sol_erase]][archivegrid_in[sol_erase]][l]]--;	//move reference to position in gridID_out of the solution
				}
			fitness_in[j_erase][l_erase]=10.0/gridNO_in[archivegrid_out[sol_erase]][archivegrid_in[sol_erase]];						//modify the fitness
			}

	//Copy one position backward all solutions after sol_erase :
	for(i=sol_erase;i<size;i++)
		{
		for(j=0;j<nvar;j++)														//copy decision variables
			archive[i][j]=archive[i+1][j];
		for(j=0;j<nobj;j++)														//copy obiective functions
			Jarchive[i][j]=Jarchive[i+1][j];
		for(j=0;j<ncons;j++)													//copy constraints
			Carchive[i][j]=Carchive[i+1][j];
		archivegrid_out[i]=archivegrid_out[i+1];								//copy outer grid location
		archivegrid_in[i]=archivegrid_in[i+1];									//copy inner grid location
		gridID_out[archivegrid_out[i]][gridIDref_out[archivegrid_out[i]][i+1]]-=1;		//update solution number in gridID_out
		gridIDref_out[archivegrid_out[i]][i]=gridIDref_out[archivegrid_out[i]][i+1];	//update solution position in gridIDref_out
		gridID_in[archivegrid_out[i]][archivegrid_in[i]][gridIDref_in[archivegrid_out[i]][archivegrid_in[i]][i+1]]-=1;		//update solution number in gridID_in
		gridIDref_in[archivegrid_out[i]][archivegrid_in[i]][i]=gridIDref_in[archivegrid_out[i]][archivegrid_in[i]][i+1];	//update solution position in gridIDref_in
		}

	//Update the boundary solutions:
	for(j=0;j<nobj;j++)											//update indexes of the lower boundary solutions
		if(sol_erase<miB[j])									//if the l-th solution to be erased is after miB[j]
			miB[j]-=1;										//update miB[j], lowering it of 1

	for(j=0;j<nobj;j++)											//update indexes of the upper boundary solutions
		if(sol_erase<maB[j])									//if the l-th solution to be erased is after maB[j]
			maB[j]-=1;										//update maB[j], lowering it of 1
}


int findgrid(double *sol, double **gridbound, int gridbisect, int nobj)
{
//Function defining the grid locations of a given solutions (see Knowles00 for the explanation of the grid bisection strategy)
//INPUT: sol (1 x nobj): solution for which the grid has to be defined
//		 gridbound (nobj x 2): lower and upper boundaries for each objective of the entire occupied region in the objectives space
//		 gridbisect: number of successive bisections of the grid for each objective
//		 nobj: number of objectives
//OUTPUT: gridzone: grid number
	int j,k, bitcount, gridzone;
	const int nbits=gridbisect*nobj;
	int *bit;
	double LB, UB, cut;
	bit=(int*)malloc(nbits*sizeof(int));

	bitcount=0;									//initializing counter of already defined bits
	for(j=0;j<nobj;j++)							//cycle on objectives
		{
		LB=gridbound[j][0];						//setting first LB as the lower boundary of the entire occupied region
		UB=gridbound[j][1];						//setting first UB as the upper boundary of the entire occupied region
		for(k=0;k<gridbisect;k++)				//cycle on grid bisections
			{
			cut=(UB+LB)/2;
			if(sol[j]>=cut)
				{
				bit[bitcount++]=1;				//if solution is in the upper part set bit to 1
				LB=cut;							//set new lower boundary
				}
			else
				{
				bit[bitcount++]=0;				//if solution is in the lower part set bit to 0
				UB=cut;							//set new upper boundary
				}
			}
		}
	gridzone=binary2integer(bit, nbits);		//convert the string of bits in base 10 --> ID of the grid zone particle i belongs to

	return gridzone;
}


void findgridbound(double **J, double **gridbound, int size, int nobj, int *minB, int *maxB)
{
//function that finds the maximum and minimum boundaries of a given set in all dimensions
//INPUT: J: set of solutions, size: number of solutions, nobj: number of dimensions;
//		 minB (nobj x 1): boundary solutions that define the minimum value in each dimension
//		 max B (nobj x 1): boundary solutions that define the maximum value in each dimension
//OUTPUT: gridbound (nobj x 2): lower and upper boundaries for each of the dimensions
	int i,j;

	for(j=0;j<nobj;j++)
		{
		gridbound[j][0]=1e308;		//initializing high value for lower boundary
		gridbound[j][1]=-1e308;		//initializing low value for upper boundary
		}

	for(i=0;i<size;i++)				//cycle on the solutions in the set
		for(j=0;j<nobj;j++)			//cycle on the dimensions
			{
			if(J[i][j]<gridbound[j][0])
				{
				gridbound[j][0]=J[i][j];
				minB[j]=i;
				}
			if(J[i][j]>gridbound[j][1])
				{
				gridbound[j][1]=J[i][j];
				maxB[j]=i;
				}
			}

}


void findinnerbound(double **outer,double ***inner, int bisect, int nobj)
{
//function that finds the boundaries of the grids obtained by successive bisections of a n-dimensional space
//INPUT: out (nobj x 2): external lower and upper boundaries for each of the dimensions of the grid
//		 bisect: number of successive inner bisections in each dimension
//		 nobj: number of dimensions
//OUTPUT: in (ngrids x nobj x 2): lower and upper boundaries for each of the dimensions for each of the grid zones
	int N1,N2,ngrids,i,j;
        N1=nobj*bisect;							//dimensions of the binary number defining a grid zone
        N2=pow(2.0,bisect);						//number of intervals in each dimension
        ngrids=pow(2.0,N1);						//total number of grid zones for which the boundaries have to be computed
        double *deltabound, **bound;					//boundaries to be computed
	int temp;
	int *bin;
	bin=(int*)malloc(N1*sizeof(int));
	deltabound=(double*)malloc(nobj*sizeof(double));
	bound=dmatrix_allocation(nobj,N2+1);
        for(i=0;i<nobj;i++)						//cycle on the dimensions to find all the boundaries
		{
		deltabound[i]=(outer[i][1]-outer[i][0])/N2;
                for(j=0;j<N2+1;j++)					//cycle on the grids of each dimension
                        bound[i][j]=outer[i][0]+j*deltabound[i];	//defining the boundaries
		}

        for(i=0;i<ngrids;i++)						//cycle on the total number of grid zones
		{
                bin=integer2binary(i,N1);				//defining the binary number associated with each grid zone
                for(j=0;j<nobj;j++)					//cycle on the dimensions
			{
                        temp=binary2integer(&bin[bisect*j],bisect);	//defining the integer number associated with each dimension
			inner[i][j][0]=bound[j][temp];
			inner[i][j][1]=bound[j][temp+1];
			}
		}
}



int findnondom(double **archive, double **swarm, double **Jarchive, double **Jswarm, double **Carchive, double **Cswarm, int swarmsize, int nvars, int nobj, int ncons)
{
//function that finds the non-dominated solutions among the solutions in swarm-Jswarm and returns them in archive-Jarchive
//if in the non-dominated front there are identical solutions --> only one of them is kept, randomly chosen
//INPUT: archive, Jarchive: current archive's decision variables and objective functions (they can be free allocated space, to be updated with results)
//		 swarm, Jswarm: population's decision variables and objective functions to be investigated to find the non-dominated front
//		 swarmsize, nvars, nobj: as in main file
//OUTPUT: count: number of solutions in the resulting non-dominated front + modification of archive and Jarchive
	int i, j, k, flag, *flagequal, flagdom, count;
	flagequal=(int*)malloc(swarmsize*sizeof(int));

	count=0;										//counter for the number of non-dominated solutions
	for (i=0;i<swarmsize;i++)
		flagequal[i]=0;								//flag to check if the particle i is equal to any other

	for (i=0;i<swarmsize;i++)
		{
		if(flagequal[i]==1)							//if the particle is equal to one of the previous ones
			flagdom=1;								//consider it dominated --> don't copy it to the archive!
		else										//else check dominance with all other particles except itself
			{
			flagdom=0;									//until flagdom=0 --> particle i is not dominated
			for (j=0;j<swarmsize;j++)					//comparison cycle on all other particles
				{
				if (i!=j)								//except itself
					{
					flag=1;								//flag to check if the particle j dominates particle i
					for (k=0;k<nobj;k++)				//check if the particle j dominates particle i
						{
						if (Jswarm[i][k]<Jswarm[j][k])	//if particle i is at least in one objective < than particle j --> flag=0 --> i is not dominated by j
							flag=0;
						if (Jswarm[i][k]==Jswarm[j][k])	//if particle i is equal to j
							flagequal[j]++;				//increase flagequal counter for particle j
						}
					if(flagequal[j]==nobj)	//if particles i and j are equal in all objectives
						flagequal[j]=1;		//define j as equal --> particle j will not be checked and maybe added to the archive
					else					//and set particle i as non-dominated by particle j
						if (flag==1)		//if flag has remained=1 --> in no objective particle i is < than particle j -->particle i is dominated by j
						{
						flagdom=1;			// and remember that i is not a non-dominated solution
						break;				// --> break cycle on j, without copying to the arhive the particle i
						}
					}
				}
			}

//if flag has never been=1 at the end of the cycles on j(flagdom==0)-->particle i has never been dominated by the other particles-->copy in the archive:
		if (flagdom==0)
			{
			for (j=0;j<nvars;j++)
				archive[count][j]=swarm[i][j];	//adding the particle's decision variables to the archive
			for (j=0;j<nobj;j++)
				Jarchive[count][j]=Jswarm[i][j];	//adding the particle's objective functions to the archive
			for (j=0;j<ncons;j++)
				Carchive[count][j]=Cswarm[i][j];	//adding the particle's constraint values to the archive
			count++;								//counting the number of non-dominated solutions
			}
		}
		return count;
}


void init_real(double **swarm,double **pbest,double **vel, double *LB, double *UB, int swarmsize, int nvar)
{
// Random initialization of a swarm positions, velocity and pbest for real-coded variables
// Random seed must be initialized outside of the function
	int i,j;
	double temprand;

	for(i=0;i<swarmsize;i++)
		{
		for(j=0;j<nvar;j++)
			{
			temprand=rand();									//random number generation
			swarm[i][j]=LB[j]+(UB[j]-LB[j])*temprand/RAND_MAX;	//positions random initialization
			pbest[i][j]=swarm[i][j];							//initial pbest=initial swarm
			vel[i][j]=0;										//null initial velocity
			}
		}
}



int isequal(double* fun1, double* fun2, int nobj)
{
//Function determining if two int vectors are equal
//INPUT: fun1, fun2: vectors and number of elements
//OUTPUT: flag=1 if equal, flag=0 if different
	int i, flag=1;

	for (i=0;i<nobj;i++)
		if (fun1[i]!=fun2[i])		//if in at least one objective they are different
			{
			flag=0;				//fun2 cannot dominate fun1
			break;
			}
	return flag;
}


void move_particle_real(double **position, double **velocity, double **archive, double **pbest, int i, int k, int nvar, int archivesize, double swarmconf, double selfconf, double inertiainit, double inertiafinal,  int maxiter, int leader)
{
//Function determining the movement of a particle (computing velocity and position), without mutation and control on the boundaries, real-coded variables
//INPUT: position and velocity of the particles, archive, personal best positions, particle to be moved (i), current iteration (k), number of variables,
//		 size of the archive, swarm and self confidence and inertia parameters, maximum number of iterations, leader (index of the particle in the archive
//		 to be used as gbest)
//OUTPUT: updated position and velocity matrixes (only particle i is updated)
	int j;
	double temprand, inertia, tempselfconf, tempswarmconf;

	inertia=(inertiafinal-inertiainit)/maxiter*k+inertiainit;	//setting the value of inertia weight for the current iteration

	temprand=rand();
	tempselfconf=temprand/RAND_MAX*selfconf;				//randomizing the value of self confidence for the particle i at iteration k
	temprand=rand();
	tempswarmconf=temprand/RAND_MAX*swarmconf;				//randomizing the value of swarmnce for the particle i at iteration k

	for(j=0;j<nvar;j++)	//for each decision variable j of particle i at iteration k
		{
		velocity[i][j]=inertia*velocity[i][j]+tempselfconf*(pbest[i][j]-position[i][j])+tempswarmconf*(archive[leader][j]-position[i][j]);	//velocity update
		position[i][j]=position[i][j]+velocity[i][j];			//position update
		}

}


void mutate_nsga2(double **position, int i, int nvar, double *LB, double *UB, double mutdistr)
{
//Function mutating a particle's position with NSGA-II algorithm distribution --> see NSGA2_mutation.m for probability distribution plot:
// if mutdistr=0 --> linear distribution over the entire search space;
// if mutdistr increases, the allowable jump is lowered, with non-linear distribution (higher probability for smaller jumps)
//INPUT: position of the particles, particle to be mutated (i, starting from 0)
//OUTPUT: updated position (only particle i is updated)
	double delta, temprand;
	int j;

	for(j=0;j<nvar;j++)		//for each decision variable j of particle i at iteration k
		{
		temprand=rand();
		if ((temprand/RAND_MAX)<(1.0/nvar))			//decide if mutate or not the variable (only one on average for each mutated particle)
			{
			temprand=rand();
			temprand=temprand/RAND_MAX;

			if(temprand<0.5)
				delta=pow(2*temprand,1/(1+mutdistr))-1;
			else
				delta=1-pow(2*(1-temprand),1/(1+mutdistr));

			position[i][j]=position[i][j]+(UB[j]-LB[j])*delta;
			}
		}
}


int pareto2tournament(double* fun1, double* fun2, int nobj)
{
//Function evaluating the ranking of 2 solutions in terms of Pareto dominance
//INPUT: fun1, fun2: objective functions vectors; nobj: dimension of fun1 and fun2 (number of objectives)
//OUTPUT: flag=1 if fun1 dominates fun2, flag=2 if fun2 dominates fun1, flag=0 if neither fun1 nor fun2 dominates the other
	int i, flag, flag1, flag2;

	flag1=1;			//if flag1 remains = 1 -->  fun1 dominates fun2
	flag2=1;			//if flag2 remains = 1 -->  fun2 dominates fun1

	for (i=0;i<nobj;i++)
		if (fun1[i]<fun2[i])		//if in at least one objective fun2>fun1
			flag2=0;				//fun2 cannot dominate fun1
		else if (fun2[i]<fun1[i])	//if in at least one objective fun1>fun2
			flag1=0;				//fun1 cannot dominate fun2

	if (flag1&&flag2)	//same solution - no one dominates
		flag=0;
	else if (flag1)		//fun1 dominates
		flag=1;
	else if (flag2)		//fun2 dominates
		flag=2;
	else				//no one dominates
		flag=0;

	return flag;
}



void reset_grid(int *archivegrid_out,int *archivegrid_in, int ngrids_out, int ngrids_in, int size, int nobj, int **gridID_out, int ***gridID_in, int *gridNO_out, int **gridNO_in, int *gridocc_out, int **gridocc_in, int *Nocc_out, int *Nocc_in, int *gridoccref_out, int **gridoccref_in, int **gridIDref_out, int ***gridIDref_in)
{
//Function resetting the archive grid structure for dgmopso algorithm --> it defines how many and what solutions are in each of the outer and inner grid
//zones --> it has to be used at the initialization or after boundaries redefinition
//INPUT: archivegrid_out (size x 1): list of outer grid zones each solution in the archive belongs to
//		 archivegrid_in (size x 1): list of inner grid zones each solution in the archive belongs to
//		 ngrids_out: number of outer grid zones in which the objectives space is divided
//		 ngrids_in: number of inner grid zones in which each outer grid zone of the objectives space is divided
//		 size: number of solutions in the given archive set
//		 nobj: number of objectives
//OUTPUT:gridID_out (ngrids_out x size): solutions in each outer grid zone. Rows: grid zones numbers; Columns: IDs of the solutions in the selected outer
//										 grid zone
//		 gridID_in (ngrids_out x ngrids_in x size): solutions in each inner grid zone. Pages: outer grid numbers; Rows: inner grid zones numbers; Columns:
//													IDs of the solutions in selected outer&inner grid zone
//		 gridNO_out (ngrids_out x 1): number of solutions in each outer grid zone
//		 gridNO_in (ngrids_out x ngrids_in): number of solutions in each inner grid zone of each outer grid zone. Rows: outer grid numbers; Columns: inner
//											 grid zones numbers
//		 gridocc_out (ngrids_out x 1): list of all the outer non-empty grids
//		 gridocc_in (ngrids_out x ngrids_in): list of all the inner grid zones of each outer grid zone occupied by at least one solution
//		 Nocc_out: number of non-empty outer grids (address call to the function)
//		 Nocc_in (ngrids_out x 1): number of non-empty inner grids for each outer grid
//		 gridoccref_out (ngrids_out x 1): gridoccref_out(i) = position in gridocc_out of the non-empty grid i (if empty --> null)
//		 gridoccref_in (ngrids_out x ngrids_in): gridoccref_in(i,j) = position in gridocc_in(i,:) of the non-empty inner grid j of the outer grid i (if empty --> null)
//		 gridIDref_out (ngrids_out x size): gridIDref_out(i,j) = position in gridID_out(i,:) of archive solution j (non-null only if the solution j belongs to outer grid i)
//		 gridIDref_in (ngrids_out x ngrids_in x size): gridIDref_in(i,j,k) = position in gridID_in(i,j,:) of archive solution k (non-null only if the solution k belongs to inner grid j of outer grid i)
	int i,j,temp,temp1,temp2;

	//Initializations to zero:
	*Nocc_out=0;													//initializing the number of non-empty outer grids
	for(i=0;i<ngrids_out;i++)										//cycle on outer grids
		{
		Nocc_in[i]=0;												//initializing the number of non-empty inner grids in outer grid i
		gridNO_out[i]=0;											//initializing the number of solutions in outer grid i
		for(j=0;j<ngrids_in;j++)									//cycle on inner grids
			gridNO_in[i][j]=0;										//initializing the number of solutions in inner grid j of outer grid i
		}

	for(i=0;i<size;i++)												//cycle on the solutions in the given archive
		{
		temp=archivegrid_out[i];
		temp2=archivegrid_in[i];
		if(gridNO_out[temp]==0)										//if i is the first solution to be found in its outer grid (thus in its inner grid too)
			{
			gridoccref_out[temp]=*Nocc_out;							//set reference of the occupied memory location
			gridocc_out[(*Nocc_out)++]=temp;						//add the outer grid of i to the list of outer grid occupied by at least one solution

			gridoccref_in[temp][temp2]=Nocc_in[temp];				//set reference of the occupied memory location
			gridocc_in[(*Nocc_out)-1][(Nocc_in[temp])++]=temp2;		//add the inner grid of i to the list of inner grids of the outer grid of i occupied by at least one solution
			}
		else if(gridNO_in[temp][temp2]==0)							//if i is the first solution to be found in its inner grid (but not in its outer grid)
			{
			gridoccref_in[temp][temp2]=Nocc_in[temp];				//set reference of the occupied memory location
			gridocc_in[gridoccref_out[temp]][(Nocc_in[temp])++]=temp2;	//add the inner grid of i to the list of inner grids of the outer grid of i occupied by at least one solution
			}

		temp1=gridNO_out[temp]++;
		gridID_out[temp][temp1]=i;									//add solution i to the set of solutions and add 1 to the counter
		gridIDref_out[temp][i]=temp1;								//set reference of the occupied memory location

		temp1=gridNO_in[temp][temp2]++;
		gridID_in[temp][temp2][temp1]=i;							//add solution i to the set of solutions and add 1 to the counter
		gridIDref_in[temp][temp2][i]=temp1;							//set reference of the occupied memory location

		}
}


int update_gbestandgrid(double **archive,double **Jarchive,double **Carchive,double *sol,double *Jsol,double *Csol,int size,int *Nocc_out,int *Nocc_in,int *agr_out,int *agr_in,int **gridID_out,int ***gridID_in,int *gridNO_out,int **gridNO_in,int *gridocc_out,int **gridocc_in,int *gridoccref_out,int **gridoccref_in,int **gridIDref_out,int ***gridIDref_in,int solgrid_out,int solgrid_in,int nvar,int nobj,int ncons,double *fitness_out,double **fitness_in,double **ngrb,int *miB,int *maB)
{
//Function updating archive, grid structure and fitness values after comparison with a non-dominated swarm solution.
//Vectors and matrixes lengths: F-->fixed,always the same; NF-->non-fixed, depending on the number of solutions in the grid
//INPUT: archive, Jarchive: archive...
//		 sol, Jsol: solution to be compared with the archive for update
//		 size: current dimension of the archive
//		 Nocc_out: number of non-empty outer grids (address call to the function)
//		 Nocc_in (ngrids_outF x 1): number of non-empty inner grids for each outer grid
//		 agr_out (sizeF x 1): outer grid position of each archive solution
//		 agr_in (sizeF x 1): inner grid position of each archive solution
//		 gridID_out (ngrids_outF x sizeNF): solutions in each outer grid zone. Rows: grid zones numbers; Columns: IDs of the solutions in the selected outer
//										 grid zone
//		 gridID_in (ngrids_outF x ngrids_inF x sizeNF): solutions in each inner grid zone. Pages: outer grid numbers; Rows: inner grid zones numbers; Columns:
//													IDs of the solutions in selected outer&inner grid zone
//		 gridNO_out (ngrids_outF x 1): number of solutions in each outer grid zone
//		 gridNO_in (ngrids_outF x ngrids_inF): number of solutions in each inner grid zone of each outer grid zone. Rows: outer grid numbers; Columns: inner
//											 grid zones numbers
//		 gridocc_out (ngrids_outNF x 1): list of all the outer non-empty grids
//		 gridocc_in (ngrids_outNF x ngrids_inNF): list of all the inner grid zones of each outer grid zone occupied by at least one solution
//		 gridoccref_out (ngrids_outF x 1): reference of the memory location occupied in gridocc_out by each of the outer grids
//		 gridoccref_in (ngrids_outF x ngrids_inF): reference of the memory location occupied in gridocc_in by each of the inner grids
//		 gridIDref_out (ngrids_outF x sizeF): gridIDref_out(i,j) = position in gridID_out(i,:) of archive solution j (non-null only if the solution j belongs to outer grid i)
//		 gridIDref_in (ngrids_outF x ngrids_inF x sizeF): gridIDref_in(i,j,k) = position in gridID_in(i,j,:) of archive solution k (non-null only if the solution k belongs to inner grid j of outer grid i)
//		 solgrid_out: outer grid of the new solution
//		 solgrid_in: inner grid of the new solution
//		 fitness_out (ngrids_outNF x 1): fitness of the non-empty outer grids
//		 fitness_in (ngrids_outNF x ngrids_inNF): fitness of the non-empty inner grids of the non-empty outer grids
//		 ngrb (nobj x 2): matrix with grid boundaries updates for checking of the error
//		 minB (nobj x 1): boundary solutions that define the minimum value in each dimension
//		 max B (nobj x 1): boundary solutions that define the maximum value in each dimension
//OUTPUT:size: updated size of the archive + potential updates in all inputs
	int j,k,l, counterase, flagadd, flagtournament, flagequal,temp,temp1,check,*erase;
	erase=(int*)malloc(size*sizeof(int));

	for(j=0;j<size;j++)
		erase[j]=0;	//initializing container of indexes of the dominated solutions to be erased
	counterase=0;	//initializing counter for the number of solutions in the archive to be erased beacause the new solution dominates them
	flagadd=1;		//initializing flag for addition of the new solution to the archive: if remains =1 --> it'n not dominated by no solutions in the archive
	flagequal=0;	//initializing flag for the equality check

	//a. Comparing the new solution with the old archive solutions:
	for(j=0;j<size;j++)	//for each archive solution j
		{
		if(flagequal==1)											//if the solution is already equal to some archive solution, do not add it
			break;

		flagtournament=pareto2tournament(Jsol,Jarchive[j],nobj);	//check domination
		flagequal=isequal(Jsol,Jarchive[j],nobj);					//check equality, if the solution is not already equal to some archive solution

		if (flagtournament==1)										//if one of the archive solution is dominated --> it must be erased from archive
			erase[counterase++]=j;									// --> update the erase vector, which counts the solutions in the archive to be erased
		else if (flagtournament==2)									//if the new solution is dominated by any of the archive solutions --> can't be added
				{
				flagadd=0;											// --> prevent from adding to the archive
				break;												// no reason to continue comparison with other solutions in the archive
				}
		//If flagtournament==0 --> both archive solutions and new solution are non-dominated --> add the new one and leave the others!
		}

	//b. Checking if the new solution is an extreme solution:
	if((flagadd && !flagequal)==1)							//if the new solution has to be added to the front --> check grid boundaries
		check=checkgridbound(Jarchive,erase,counterase,ngrb,Jsol,size,nobj,miB,maB);	//check grid boundaries after the addition of the new solution
	else check=0;											//else don't check them!

	//c. Erase dominated solutions of the archive
	if (counterase!=0)			//if at least one solution must be erased --> rewrite the entire archive with solutions in different positions
		{

		counterase=0;									//counting the number of solutions already erased from the archive
		for(j=0;j<size;j++)								//for each archive solution j, at iteration k and particle i

			if(erase[counterase]==j)					//ELSE IF it MUST be erased --> update the grid only
				{

				//I.) Update the outer grid variables:
				gridNO_out[agr_out[j]]--;				//update number of sol. in outer grid of the archive solution to be erased
				if(gridNO_out[agr_out[j]]==0)			//IF j was the last solution in its outer grid-->erase it from gridocc and fitness structures
					{
					temp=gridoccref_out[agr_out[j]];	//define memory location of the selected grid in the list of non-empty outer grids
					(*Nocc_out)--;								//lowering the number of non-empty outer grids
					for(l=temp;l<(*Nocc_out);l++)				//cycling on the occupied outer grids after the one to be elimated
						{
						gridocc_out[l]=gridocc_out[l+1];		//update list of non-empty outer grids
						gridoccref_out[gridocc_out[l+1]]--;		//update the position in gridocc_out of the moved grid
						fitness_out[l]=fitness_out[l+1];		//update the outer fitness
						//all elements of gridocc_in and gridoccref_in in the rows after the one of the outer grid became empty must be shifted 1 row up:
						for(k=0;k<Nocc_in[gridocc_out[l]];k++)		//for each of the non-empty inner grids in outer grid l
							{
							gridocc_in[gridoccref_out[gridocc_out[l]]][k]=gridocc_in[gridoccref_out[gridocc_out[l]]+1][k];	//update list of non-empty inner grids
							gridoccref_in[gridocc_out[l]][gridocc_in[gridoccref_out[gridocc_out[l]]][k]]=k;				//update the position in gridocc_in of the moved grid
							fitness_in[gridoccref_out[gridocc_out[l]]][k]=fitness_in[gridoccref_out[gridocc_out[l]]+1][k];	//update the inner fitness
							}
						}
					}
				else											//ELSE it's enough to update gridID_out and gridIDref_out
					{
					for(l=gridIDref_out[agr_out[j]][j];l<gridNO_out[agr_out[j]];l++)
						{
						gridID_out[agr_out[j]][l]=gridID_out[agr_out[j]][l+1];		//update list of solutions in the selected outer grid
						gridIDref_out[agr_out[j]][gridID_out[agr_out[j]][l]]--;		//update also reference to the position in gridID_out (it scales of 1 position due to the elimination of the previous solution)
						}
					fitness_out[gridoccref_out[agr_out[j]]]=10.0/gridNO_out[agr_out[j]];
					}

				//II.) Update the inner grid variables:
				gridNO_in[agr_out[j]][agr_in[j]]--;		//update number of sol. in inner grid of the archive solution to be erased
				if(gridNO_in[agr_out[j]][agr_in[j]]==0)	//IF j was the last solution in its inner grid-->erase it from gridocc and fitness structures
					{
					temp=gridoccref_out[agr_out[j]];		//define memory location of the selected grid in the list of non-empty outer grids
					temp1=gridoccref_in[agr_out[j]][agr_in[j]];	//define memory location of the selected grid in the list of non-empty inner grids
					Nocc_in[agr_out[j]]--;					//lowering the number of non-empty inner grids
					for(l=temp1;l<Nocc_in[agr_out[j]];l++)	//cycling on the occupied inner grids after the one to be elimated
						{
						gridocc_in[temp][l]=gridocc_in[temp][l+1];		//update list of non-empty inner grids
						gridoccref_in[agr_out[j]][gridocc_in[temp][l+1]]--;	//update the position in gridocc_in of the moved grid
						fitness_in[temp][l]=fitness_in[temp][l+1];			//update the outer fitness
						}
					}
				else															//ELSE it's enough to cancel the solution reference in gridID
					{
					for(l=gridIDref_in[agr_out[j]][agr_in[j]][j];l<gridNO_in[agr_out[j]][agr_in[j]];l++)
						{
						gridID_in[agr_out[j]][agr_in[j]][l]=gridID_in[agr_out[j]][agr_in[j]][l+1];	//update list of solutions in the selected inner grid
						gridIDref_in[agr_out[j]][agr_in[j]][gridID_in[agr_out[j]][agr_in[j]][l]]--;	//update also reference to the position in gridID_in (it scales of 1 position due to the elimination of the previous solution)
						}
					fitness_in[gridoccref_out[agr_out[j]]][gridoccref_in[agr_out[j]][agr_in[j]]]=10.0/gridNO_in[agr_out[j]][agr_in[j]];
					}

				counterase++;									//IF the archive solution j MUST be erased --> increase counter
				}										//CLOSE IF on the possibility of erasing the archive solution

		//d.) Lower solution indexes depending on the number of erased solutions before each solution:
		counterase=0;									//counting the number of solutions already erased from the archive
		for(j=0;j<size;j++)								//for each archive solution j, at iteration k and particle i
			if (erase[counterase]!=j && counterase!=0)	//IF the archive solution j MUST NOT be erased --> copy it counterase positions backwards in the new archive
				{				//copying from [j-counterase] to [j]: if no solution has already been erased (counterase=0) --> copy on the same position
				for(l=0;l<nvar;l++)												//update decision variables
					archive[j-counterase][l]=archive[j][l];
				for(l=0;l<nobj;l++)												//update objective functions
					Jarchive[j-counterase][l]=Jarchive[j][l];
				for(l=0;l<ncons;l++)											//update constraints
					Carchive[j-counterase][l]=Carchive[j][l];
				agr_out[j-counterase]=agr_out[j];				//update outer grid location
				agr_in[j-counterase]=agr_in[j];					//update inner grid location
				gridID_out[agr_out[j]][gridIDref_out[agr_out[j]][j]]-=counterase;		//update solution number in gridID_out
				gridIDref_out[agr_out[j]][j-counterase]=gridIDref_out[agr_out[j]][j];	//update solution position in gridIDref_out
				gridID_in[agr_out[j]][agr_in[j]][gridIDref_in[agr_out[j]][agr_in[j]][j]]-=counterase;		//update solution number in gridID_in
				gridIDref_in[agr_out[j]][agr_in[j]][j-counterase]=gridIDref_in[agr_out[j]][agr_in[j]][j];	//update solution position in gridIDref_in
				}
			else if(erase[counterase]==j)
				counterase++;

		//Update the boundary solutions:
		for(j=0;j<nobj;j++)											//update indexes of the lower boundary solutions
			{
			temp=0;
			for(l=0;l<counterase;l++)								//cycle on the solutions to be erased
				if(erase[l]>miB[j])									//if the l-th solution to be erased is after miB[j]
					{
					miB[j]-=l;										//update miB[j], lowering it of l (there are l solutions to be erased before it)
					temp=1;											//variable to check if miB has been updated
					break;
					}
			if(temp==0)												//if miB[j] is after the last solution to be erased it has not been updated
				miB[j]-=l;											// --> update miB[j], lowering it of l (there are l solutions to be erased before it)
			}


		for(j=0;j<nobj;j++)											//update indexes of the upper boundary solutions
			{
			temp=0;
			for(l=0;l<counterase;l++)								//cycle on the solutions to be erased
				if(erase[l]>maB[j])									//if the l-th solution to be erased is after miB[j]
					{
					maB[j]-=l;										//update miB[j], lowering it of l (there are l solutions to be erased before it)
					temp=1;											//variable to check if miB has been updated
					break;
					}
			if(temp==0)												//if maB[j] is after the last solution to be erased it has not been updated
				maB[j]-=l;											// --> update maB[j], lowering it of l (there are l solutions to be erased before it)
			}

		}								//CLOSE IF cycle for the erasing of solutions


		//e. Counting the number of solutions in the new archive:
		size=size+(flagadd && !flagequal)-counterase;		//Add one solution if the new one is not dominated and different and erase the dominated ones

		//f. Adding the new solution to the archive:
		if (flagadd && !flagequal)									//if the new solution is non dominated and non equal to any of the previous ones
			{														//--> copy it in the last archive position
			//Adding variables and objectives:
			for(l=0;l<nvar;l++)										//update decision variables
				archive[size-1][l]=sol[l];
			for(l=0;l<nobj;l++)										//update objective functions
				Jarchive[size-1][l]=Jsol[l];
			for(l=0;l<ncons;l++)									//update constraints
				Carchive[size-1][l]=Csol[l];

			//Adding grid positions:
			agr_out[size-1]=solgrid_out;			//define outer grid zone for the solution to be added
			agr_in[size-1]=solgrid_in;				//define inner grid zone for the solution to be added

			//Adding outer grid structure:
			if(gridNO_out[agr_out[size-1]]==0)			//if solution to be added is the first found in its outer grid (thus in its inner grid too)
				{
				gridoccref_out[agr_out[size-1]]=*Nocc_out;		//set reference of the occupied memory location
				gridocc_out[(*Nocc_out)++]=agr_out[size-1];		//add the outer grid of i to the list of non-empty outer grid
				gridoccref_in[agr_out[size-1]][agr_in[size-1]]=Nocc_in[agr_out[size-1]];	//set reference of the occupied memory location
				gridocc_in[(*Nocc_out)-1][Nocc_in[agr_out[size-1]]++]=agr_in[size-1];	//add the inner grid of i to the list of inner grids of the outer grid of i occupied by at least one solution
				}
			else if(gridNO_in[agr_out[size-1]][agr_in[size-1]]==0)							//if i is the first solution to be found in its inner grid (but not in its outer grid)
				{
				gridoccref_in[agr_out[size-1]][agr_in[size-1]]=Nocc_in[agr_out[size-1]];	//set reference of the occupied memory location
				fitness_in[gridoccref_out[agr_out[size-1]]][Nocc_in[agr_out[size-1]]]=10.0;			//define inner fitness for the new non-empty grid --> only 1 solution in it
				gridocc_in[gridoccref_out[agr_out[size-1]]][Nocc_in[agr_out[size-1]]++]=agr_in[size-1];	//add the inner grid of i to the list of inner grids of the outer grid of i occupied by at least one solution
				}

			gridIDref_out[agr_out[size-1]][size-1]=gridNO_out[agr_out[size-1]];								//set reference of the occupied memory location
			gridID_out[agr_out[size-1]][gridNO_out[agr_out[size-1]]++]=size-1;									//add solution i to the set of solutions and add 1 to the counter
			fitness_out[gridoccref_out[agr_out[size-1]]]=10.0/gridNO_out[agr_out[size-1]];	//define outer fitness for the grid of the solution to be added

			gridIDref_in[agr_out[size-1]][agr_in[size-1]][size-1]=gridNO_in[agr_out[size-1]][agr_in[size-1]];							//set reference of the occupied memory location
			gridID_in[agr_out[size-1]][agr_in[size-1]][gridNO_in[agr_out[size-1]][agr_in[size-1]]++]=size-1;							//add solution i to the set of solutions and add 1 to the counter
			fitness_in[gridoccref_out[agr_out[size-1]]][gridoccref_in[agr_out[size-1]][agr_in[size-1]]]=10.0/gridNO_in[agr_out[size-1]][agr_in[size-1]];		//define inner fitness for the new non-empty grid --> only 1 solution in it

			}																		//CLOSE IF cycle for the adding of the new solution

	//g. Checking again the boundaries if extreme solutions have been erased:
	if(check==1)												//if at least one of the erased solutions is a boundary solution
		findgridbound(Jarchive,ngrb,size,nobj,miB,maB);			//	--> recompute grid boundaries completely
	return size;
}


int update_pbest_real_fitness(double **Jswarm,double **Jpbest,double **swarm,double **pbest,int i,int nvar,int nobj,double **gridbound_out,double ***gridbound_in,int gridbisect_out,int gridbisect_in,int *gridNO_out,int **gridNO_in,int *solgrid_out,int *solgrid_in,double *pbest_fitness_out,double *pbest_fitness_in)
{
//Function updating pbest for real-coded variables; simple update based on dominance, 50% probability for each solution in case of mutual non-dominance
//INPUT: swarm and pbest position and function matrixes, index of the particle to be checked;
//		 pbest_fitness_out and pbest_fitness_in: outer and inner fitness of the grid containing each of the current pbest particles (address passage)
//		 see other functions for other variables definitions
//OUTPUT: flag=0 if neither solution is dominated, flag=1 if new solution dominates old pbest, flag=2 if old pbest dominates the new solution
//	 	  + updated pbest position (only pbest for particle i is updated) both in search and objective spaces
//Pbest update strategy:
//	if newsol dominates old pbest --> UPDATE PBEST
//	if newsol is dominated by old pbest --> DON'T UPDATE PBEST
//	if newsol and old pbest are non-dominated & fitness_out of newsol > fitness_out of pbest --> UPDATE PBEST
//	if newsol and old pbest are non-dominated & fitness_out of newsol < fitness_out of pbest --> DON'T UPDATE PBEST
//	if newsol and old pbest are non-dominated & fitness_out of newsol = fitness_out of pbest & fitness_IN of newsol > fitness_IN of pbest --> UPDATE PBEST
//	if newsol and old pbest are non-dominated & fitness_out of newsol = fitness_out of pbest & fitness_IN of newsol < fitness_IN of pbest --> DON'T UPDATE PBEST
//	if newsol and old pbest are non-dominated & fitness_out of newsol = fitness_out of pbest & fitness_IN of newsol = fitness_IN of pbest --> RANDOMLY CHOOSE
	int flag, j;
	double temprand, tempfit_out, tempfit_in;

	flag=pareto2tournament(Jswarm[i],Jpbest[i],nobj);				//check domination

	if(flag!=2)														//if the new solution is not dominated by the previous pbest
		{
		(*solgrid_out)=findgrid(Jswarm[i],gridbound_out,gridbisect_out,nobj);					//find outer grid zone of the new solution
		(*solgrid_in)=findgrid(Jswarm[i],gridbound_in[(*solgrid_out)],gridbisect_in,nobj);		//find inner grid zone of the new solution
		tempfit_out=10.0/gridNO_out[(*solgrid_out)];											//define fitness of the outer grid of the new solution
		tempfit_in=10.0/gridNO_in[(*solgrid_out)][(*solgrid_in)];								//define fitness of the inner grid of the new solution
		temprand=(double)rand()/RAND_MAX;							//define random number
		}															//ELSE DO NOTHING: pbest does not have to be updated

	if (flag==1)													//if the new solution dominates the pbest --> update
		{
		for(j=0;j<nvar;j++)											//update decision variables
			pbest[i][j]=swarm[i][j];
		for(j=0;j<nobj;j++)											//update objective functions
			Jpbest[i][j]=Jswarm[i][j];
		pbest_fitness_out[i]=tempfit_out;							//update pbest outer fitness
		pbest_fitness_in[i]=tempfit_in;								//update pbest inner fitness
//		printf("\n1");																															//PC
		}
	else if ((flag==0)&&(tempfit_out>pbest_fitness_out[i]))			//if newsol and pbest are non-dominated, but newsol has higher outer fitness --> update
		{
		for(j=0;j<nvar;j++)											//update decision variables
			pbest[i][j]=swarm[i][j];
		for(j=0;j<nobj;j++)											//update objective functions
			Jpbest[i][j]=Jswarm[i][j];
		pbest_fitness_out[i]=tempfit_out;							//update pbest outer fitness
		pbest_fitness_in[i]=tempfit_in;								//update pbest inner fitness
//		printf("\n2");																															//PC
		}
	else if ((flag==0)&&(tempfit_out==pbest_fitness_out[i])&&(tempfit_in>pbest_fitness_in[i]))	//if newsol and pbest are non-dominated, but newsol has higher inner fitness --> update
		{
		for(j=0;j<nvar;j++)											//update decision variables
			pbest[i][j]=swarm[i][j];
		for(j=0;j<nobj;j++)											//update objective functions
			Jpbest[i][j]=Jswarm[i][j];
		pbest_fitness_out[i]=tempfit_out;							//update pbest outer fitness
		pbest_fitness_in[i]=tempfit_in;								//update pbest inner fitness
//		printf("\n3");																															//PC
		}
	else
		{
		if ((flag==0)&&(tempfit_out==pbest_fitness_out[i])&&(tempfit_in==pbest_fitness_in[i])&&(temprand<0.5))	//if newsol and pbest are non-dominated, and have the same outer and inner fitness --> decide randomly if updating or not
			{
			for(j=0;j<nvar;j++)										//update decision variables
				pbest[i][j]=swarm[i][j];
			for(j=0;j<nobj;j++)										//update objective functions
				Jpbest[i][j]=Jswarm[i][j];
			pbest_fitness_out[i]=tempfit_out;						//update pbest outer fitness
			pbest_fitness_in[i]=tempfit_in;							//update pbest inner fitness
//			printf("\n4");																														//PC
			}
		}

	return flag;
}





double *values;								//double vector for the sorting of the archive with vectorsort
int compare_index(const void *pp1,const void *pp2)
{
	double val1 = values[*(int*)pp1];
	double val2 = values[*(int*)pp2];
	if (val1 > val2)
		return 1;
	else if (val1 < val2)
		return -1;
	else
		return 0;
}


// ************************************************************************************************************************
// ************************************************************************************************************************
// ************************************************************************************************************************
// **************************************************** DG-MOPSO MAIN: ****************************************************
// ************************************************************************************************************************
// ************************************************************************************************************************
// ************************************************************************************************************************

void GlobalOptimizers::dgmopso()
{

/* ---------------------------------------------------- PRE-PROCESS -----------------------------------------------------*/

// ************************************************************************************************************************
// ************************************************************************************************************************
	//CHANGE HERE DG-MOPSO PARAMETERS:
        const int swarmsize=150;			//number of particles in the swarm
	const int archivesize=NsolPareto;	//non-dominated archive size
	const int gridbisect_out=4;			//number of successive bisections for each dimension to create the outer grid
	const int gridbisect_in=1;			//number of further successive bisections for each dimension to create the inner grid
	const double eps=0.03;				//percentage error on the external boundaries over which the grid is resetted
	const double inertiainit=0.5;		//initial inertia parameter (linear)
	const double inertiafinal=0.2;		//final inertia parameter (linear)
	const double selfconfinit=1;		//initial self confidence parameter (linear)
	const double selfconffinal=1;		//final self confidence parameter (linear)
	const double swarmconfinit=1.5;		//initial swarmconfidence parameter (linear)
	const double swarmconffinal=1.5;	//final swarmconfidence parameter (linear)
	const double mutprobinit=0.01;		//initial mutation probability (linear)
	const double mutprobfinal=0.01;		//final mutation probability (linear)
	const double mutdistrinit=0;		//initial NSGA-II mutation distribution (linear)
	const double mutdistrfinal=5;		//final NSGA-II mutation distribution (linear)
	//DO NOT CHANGE ANYTHING BELOW
// ************************************************************************************************************************
// ************************************************************************************************************************

	const int nobj=NOBJ;			//number of functions
	const int nvar=NVAR;			//number of variables
	const int ncons=NCONS;			//number of constraints
	const int maxiter=MAXITER;		//number of iterations
	
	//variables definitions:
	double **swarm, **pbest, **vel, **archive;														//position variables
	double **Jswarm, **Jpbest, **Jarchive, **Cswarm, **Carchive;									//function values variables
	double **gridbound_out,**newgridbound_out, ***gridbound_in;										//grid bounds variables
	double *fitness_out, **fitness_in,*pbest_fitness_out, *pbest_fitness_in;						//fitness variables
	double temprand, mutdistr, mutprob, selfconf, swarmconf;										//temporary variables
	int *archivegrid_out, *archivegrid_in, solgrid_out, solgrid_in, *minB, *maxB;					//archive grid positions variables
	int ngrids_out,**gridID_out,**gridIDref_out,*gridocc_out,*gridoccref_out,*gridNO_out,Nocc_out;	//outer grid structure variables
	int ngrids_in,***gridID_in,***gridIDref_in,**gridocc_in,**gridoccref_in,**gridNO_in ,*Nocc_in;	//inner grid structure variables
	int i,j,k,l,counter1,counter2,leader,flagpbest,*archivesizehist;								//counters and other variables
	int *j_indexes,*l_indexes,j_erase,l_erase,sol_erase,check_erase,count_jerase,count_lerase;		//variables for the archive pruning
	double penalty,tmpJarchive;

        //memory allocations, Eigen standard for external interface with STA:
        VectorXd var(NVAR);
        VectorXd objconst(NOBJ+NCONS);

        //memory allocations, DG-MOPSO internal standard:
        swarm=dmatrix_allocation(swarmsize,nvar);			//particles positions at the current iteration
        pbest=dmatrix_allocation(swarmsize,nvar);			//personal best positions at the current iteration
        vel=dmatrix_allocation(swarmsize,nvar);				//particles velocities at the current iteration
        archive=dmatrix_allocation(archivesize+1,nvar);			//archive solutions

        Jswarm=dmatrix_allocation(swarmsize,nobj);			//function values for the swarm at the current iteration
        Jpbest=dmatrix_allocation(swarmsize,nobj);			//personal best function values at the current iteration
        Jarchive=dmatrix_allocation(archivesize+1,nobj);		//archive function values
        Cswarm=dmatrix_allocation(swarmsize,ncons);			//constraints values for the swarm at the current iteration
        Carchive=dmatrix_allocation(archivesize+1,ncons);		//archive constraints values

	values=(double*)malloc((archivesize+1)*sizeof(double));
	j_indexes=(int*)malloc((archivesize+1)*sizeof(int));
	l_indexes=(int*)malloc((archivesize+1)*sizeof(int));

        archivegrid_in=(int*)malloc((archivesize)*sizeof(int));		//outer grid identifiers for the archive solutions
        archivegrid_out=(int*)malloc((archivesize)*sizeof(int));	//inner grid identifiers for the archive solutions

        ngrids_out=pow(2.0,nobj*gridbisect_out);			//total number of outer grid zones
        ngrids_in=pow(2.0,nobj*gridbisect_in);				//total number of inner grid zones for each outer grid zone
        gridbound_out=dmatrix_allocation(nobj,2);			//matrix for outer grid boundaries: dimensions in rows; column 1-->low.bound.; 2-->up.bound.
        minB=(int*)malloc(nobj*sizeof(int));				//vector with the lower boundary solutions of the archive
        maxB=(int*)malloc(nobj*sizeof(int));				//vector with the lower boundary solutions of the archive
        newgridbound_out=dmatrix_allocation(nobj,2);			//matrix for outer grid boundaries update for comparison
        gridbound_in=d3darray_allocation(ngrids_out,nobj,2);		//matrix for inner grid boundaries: dimensions in rows; column 1-->low.bound.; 2-->up.bound.
        gridID_out=imatrix_allocation(ngrids_out,archivesize+1);	//in rows: grid numbers; in columns: IDs of the solutions in each grid zone
        gridIDref_out=imatrix_allocation(ngrids_out,archivesize+1);	//reference to the positions in gridID_out
        gridNO_out=(int*)malloc(ngrids_out*sizeof(int));		//number of solutions in each grid zone
        gridocc_out=(int*)malloc(ngrids_out*sizeof(int));		//list of occupied grid zones
        gridoccref_out=(int*)malloc(ngrids_out*sizeof(int));		//reference to the positions in gridocc_out
        gridID_in=i3darray_allocation(ngrids_out,ngrids_in,archivesize+1);	//in rows: grid numbers; in columns: IDs of the solutions in each grid zone
	gridIDref_in=i3darray_allocation(ngrids_out,ngrids_in,archivesize+1);	//reference to the positions in gridID_out
        gridNO_in=imatrix_allocation(ngrids_out,ngrids_in);		//number of solutions in each grid zone
        gridocc_in=imatrix_allocation(ngrids_out,ngrids_in);		//list of occupied grid zones
        gridoccref_in=imatrix_allocation(ngrids_out,ngrids_in);		//reference to the positions in gridocc_out
        Nocc_in=(int*)malloc(ngrids_out*sizeof(int));			//number of non-empty inner grids for each outer grid
        fitness_out=(double*)malloc(ngrids_out*sizeof(double));		//fitness of the outer grids based on the number of solutions contained
        fitness_in=dmatrix_allocation(ngrids_out,ngrids_in);		//fitness of the inner grids based on the number of solutions contained
        pbest_fitness_out=(double*)malloc(swarmsize*sizeof(double));	//pbest outer fitness
        pbest_fitness_in=(double*)malloc(swarmsize*sizeof(double));	//pbest inner fitness

        archivesizehist=(int*)malloc((maxiter+1)*sizeof(int));		//vector for storage of archive dimensions history

        double *LB, *UB;                                                //lower and upper boundaries for the decision variables. N.B: LB and UB MUST HAVE THE SAME SIZE
	LB=(double*)malloc(nvar*sizeof(double));
	UB=(double*)malloc(nvar*sizeof(double));
	
	for(i=0;i<NVAR;i++)
		{
                LB[i]=LOWERBOUND(i);
                UB[i]=UPPERBOUND(i);
		}

/* ---------------------------------------------------------------- MAIN ------------------------------------------------------------------------*/

	archivesizehist[0]=0;		//initially set archive size to 0
	int flaghuge=0;
        if(InitMode==0)			//random initialization
		//I.a) RANDOMLY INIZIALIZE SWARM POSITION and set VELOCITY to 0 and PBEST equal to the swarm:
		{
		init_real(swarm, pbest, vel, &LB[0], &UB[0], swarmsize, nvar);
		}
	else if(InitMode==1)		//initialization from previous iteration
		{
		//I.b) LOAD ARCHIVE AND SWARM FROM PAST ITERATION RESULTS
		//COPY ARCHIVE FROM PREVIOUS ITERATION:
		for(i=0;i<archivesize;i++)
			{
			//check if solution i of the archive of previous iteration is actually a solution or is "huge"
			if(flaghuge==0)		//if a huge solution (--> end of archive) has not been found yet
				for(j=0;j<nvar;j++)									
                                        if(OptimalSolutions(i,j)==huge)
						{
						flaghuge=1;							//set flag to 1
						break;								//and exit check cycle
						}
			if(flaghuge==0)			//if solution i is a solution (archive is full at least till i-th position)
				{
				for(j=0;j<nvar;j++)									
                                        archive[i][j]=OptimalSolutions(i,j);		//copy variables
				for(j=0;j<nobj;j++)									
                                        Jarchive[i][j]=ParetoFront(i,j);			//copy objective
				for(j=0;j<ncons;j++)
                                        Carchive[i][j]=ParetoFront(i,j+nobj);	//copy constraints
				}
			else					//else (solution i is "huge")
				{
				archivesizehist[0]=i;						//the end of the archive of previous iteration has been reached
				break;										//exit from cycle for copying archive solutions
				}
			}

		if(i==archivesize)									//if the archive of previous iteration was full
			archivesizehist[0]=archivesize;					//define current archive size as the maximum size

		//COPY SWARM FROM PREVIOUS ITERATION:
		for(i=0;i<swarmsize;i++)
			{
			for(j=0;j<nvar;j++)
				{
                                swarm[i][j]=OldSwarm(i,j);			//copy particles variables
                                pbest[i][j]=OldPbest(i,j);			//copy pbests variables
                                vel[i][j]=OldVel(i,j);				//copy particles velocities
				}
			for(j=0;j<nobj;j++)
				{
                                Jswarm[i][j]=OldJswarm(i,j);			//copy particles objectives
                                Jpbest[i][j]=OldJpbest(i,j);			//copy pbests objectives
				}
			}

		}
	else						//initialization from previous run
		{
		//I.c) LOAD ARCHIVE OF PREVIOUS RUN (ONLY VARIABLES) AND USE IT AS FIRST PART OF THE SWARM:
		//TAKE AVAILABLE SOLUTION FROM ARCHIVE OF PREVIOUS ITERATION AS FIRST SWARM PARTICLES:
		for(i=0;i<archivesize;i++)
			{
			//check if solution i of the archive of previous iteration is actually a solution or is "huge"
			if(flaghuge==0)		//if a huge solution (--> end of archive) has not been found yet
				for(j=0;j<nvar;j++)									
                                        if(OptimalSolutions(i,j)==huge)
						{
						flaghuge=1;							//set flag to 1
						break;								//and exit check cycle
						}
			if(flaghuge==0)			//if solution i is a solution (archive is full at least till i-th position)
				{
				for(j=0;j<nvar;j++)							//copy variables
					{
					if(i<swarmsize)							//check for the case that the archive is larger then the swarm
						{
                                                swarm[i][j]=OptimalSolutions(i,j);	//load variables of previous archive solution
						pbest[i][j]=swarm[i][j];			//previous archive solution is also current pbest solution
						vel[i][j]=0;						//null initial velocity
						}
					}
				}
			else					//else (solution i is "huge" --> it is not a solution)
				{
				archivesizehist[0]=i;						//the end of the archive of previous iteration has been reached
				break;										//exit from cycle for copying archive solutions
				}
			}

		if(i==archivesize)									//if the archive is full until the last position
			archivesizehist[0]=archivesize;					//define current archive size as the maximum size

		//RANDOMLY INITIALIZE THE OTHER SWARM SOLUTIONS (if archivesizehist[0]<swarmsize):
		for(i=archivesizehist[0];i<swarmsize;i++)
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
	if(InitMode==0 || InitMode==2)			//if it's not the continuation of a run
		{
                //I. Perform first evaluation for each particle AND check gbest:
		for(i=0;i<swarmsize;i++)
			{
			for(j=0;j<nvar;j++)
                                var(j)=swarm[i][j];	//copy current swarm variables to Eigen temp. vector
			printf("\nInitialization, particle %d.",i);
                        objconst=EvaluateModel(var,0);
			penalty=0;
			for (j=0;j<ncons;j++)
				{
                                Cswarm[i][j]=objconst(j+nobj);	//NOTE: constraints MUST be <0 for violations and =0 for satisfaction
				penalty+=Cswarm[i][j];
				}
			for (j=0;j<nobj;j++)
				{
                                Jswarm[i][j]=objconst(j)-penalty*huge;	//adding a huge value if the constraint is violated
				Jpbest[i][j]=Jswarm[i][j];
				}
			}
		//II.) FIND NON-DOMINATED SOLUTIONS IN THE RANDOM INITIAL SWARM:
		archivesizehist[0]=findnondom(archive, swarm, Jarchive, Jswarm, Carchive, Cswarm, swarmsize, nvar, nobj, ncons);

		if (archivesizehist[0]>archivesize)
			archivesizehist[0]=archivesize;		//for the unlikely case the first non-dominated front has too many solutions
		}

        //III.) DEFINE INITIAL GRID BOUNDARIES:
	findgridbound(Jarchive,gridbound_out,archivesizehist[0], nobj,minB,maxB);		//find external boundaries of the initial archive
	findinnerbound(gridbound_out,gridbound_in,gridbisect_out,nobj);					//find boundaries for each of the outer grid zones
	for(j=0;j<nobj;j++)																//copy in another matrix to check the grid boundaries modifications
		{
		newgridbound_out[j][0]=gridbound_out[j][0];
		newgridbound_out[j][1]=gridbound_out[j][1];
		}

        //IV.) DEFINE INITIAL GRID STRUCTURE:
	for(i=0;i<archivesizehist[0];i++)
		{
		archivegrid_out[i]=findgrid(Jarchive[i],gridbound_out,gridbisect_out,nobj);						//find outer grid zone for archive solution i
		archivegrid_in[i]=findgrid(Jarchive[i],gridbound_in[archivegrid_out[i]],gridbisect_in,nobj);	//find inner grid zone for archive solution i
		}
	reset_grid(archivegrid_out,archivegrid_in,ngrids_out,ngrids_in,archivesizehist[0],nobj,gridID_out,gridID_in,gridNO_out,gridNO_in,gridocc_out,gridocc_in,&Nocc_out,Nocc_in,gridoccref_out,gridoccref_in,gridIDref_out,gridIDref_in);

        //V.) DEFINE INITIAL FITNESS VALUES:
	define_fitness_out(Nocc_out, gridNO_out, gridocc_out, fitness_out);									//define outer grid fitness
	define_fitness_in(Nocc_out, gridocc_out, Nocc_in, gridocc_in, gridNO_in, fitness_in);				//define inner grid fitness
	for(i=0;i<swarmsize;i++)																			//define initial pbest fitness
		{
		solgrid_out=findgrid(Jpbest[i],gridbound_out,gridbisect_out,nobj);								//find outer grid zone of pbest
		solgrid_in=findgrid(Jpbest[i],gridbound_in[solgrid_out],gridbisect_in,nobj);					//find inner grid zone of pbest
		pbest_fitness_out[i]=10.0/gridNO_out[solgrid_out];												//define fitness of the outer grid of pbest
		pbest_fitness_in[i]=10.0/gridNO_in[solgrid_out][solgrid_in];									//define fitness of the inner grid of pbest
		}

	if(InitMode==0 || InitMode==2)			//if it's not the continuation of a run
		{
		//Write initial objectives and constraints values to file:
		for(counter1=0;counter1<archivesizehist[0];counter1++)
			{
			penalty=0;													//recomputing penalty
			for(counter2=0;counter2<ncons;counter2++)
				penalty+=Carchive[counter1][counter2];			

			for(counter2=0;counter2<nobj;counter2++)
				{
				tmpJarchive=Jarchive[counter1][counter2]+penalty*huge;
				fprintf(fpParetoFront,"%lf ",tmpJarchive);
				}
			for(counter2=0;counter2<ncons;counter2++)
				fprintf(fpParetoFront,"%lf ",Carchive[counter1][counter2]);
			fprintf(fpParetoFront,"\n");
			}
		//If initial archive size is not as large as the number of solutions required  by the user (most likely) add zeros:
		for(counter1=archivesizehist[0];counter1<archivesize;counter1++)
			{
			for(counter2=0;counter2<nobj;counter2++)
                fprintf(fpParetoFront,"%lf ", 0.0);
			for(counter2=0;counter2<ncons;counter2++)
                fprintf(fpParetoFront,"%lf ", 0.0);
			fprintf(fpParetoFront,"\n");
			}
		}

	printf("\n\n SWARM INITIALIZATION COMPLETED, archive size: %d.\n\n",archivesizehist[0]);

        //VI.) MAIN CYCLE on k: maxiter iterations of the PSO algorithm (no other stop criterion)
	k=1;		//initializing counter
	while(k<=maxiter)		//ONLY ITERATIONS NUMBER STOP CONDITION
		{
		//a.) set the archivesize at first evaluation of each iteration equal to the last evaluation of the previous iteration:
		archivesizehist[k]=archivesizehist[k-1];		//initially setting the archive size at the current iteration equal to the previous one
		mutdistr=(mutdistrfinal-mutdistrinit)/maxiter*k+mutdistrinit;		//setting the value of mutation distribution for the current iteration
		mutprob=(mutprobfinal-mutprobinit)/maxiter*k+mutprobinit;			//setting the value of mutation probability for the current iteration
		selfconf=(selfconffinal-selfconfinit)/maxiter*k+selfconfinit;		//setting the value of self confidence for the current iteration
		swarmconf=(swarmconffinal-swarmconfinit)/maxiter*k+swarmconfinit;	//setting the value of swarm confidence for the current iteration

		//b.) CYCLE ON THE SOLUTIONS OF THE SWARM (i):
		for(i=0;i<swarmsize;i++)	//for each particle i at iteration k
			{

			//1. Choose leader:
			solgrid_out=findgrid(Jswarm[i],gridbound_out,gridbisect_out,nobj);				//find outer grid zone for particle i
//			if(k==1 && i==4) solgrid_out=1;
			leader=dgchoose_leader(solgrid_out,fitness_out,fitness_in,archivegrid_out,archivegrid_in,gridID_out,gridID_in,gridNO_out,gridocc_out,gridoccref_out,gridocc_in,Nocc_out,Nocc_in);

			//2. Move the particle:
			move_particle_real(swarm, vel, archive, pbest, i, k, nvar, archivesize, swarmconf, selfconf, inertiainit, inertiafinal, maxiter, leader);

			//3. Mutate the particle (NSGA-II mutation) with probability mutprob
			temprand=(double)rand()/RAND_MAX;
			if ((temprand)<mutprob)		//decide if mutate or not the particle
				mutate_nsga2(swarm, i, nvar, LB, UB, mutdistr);					//NSGA-II mutation definition

			//4. Restraint the position within the boundaries:
			check_boundaries_real(swarm, vel, i, nvar, LB, UB);

			//5. Evaluate the function:
			for(j=0;j<nvar;j++)
                                var(j)=swarm[i][j];	//copy current swarm into Eigen temp. vector
			printf("\nIteration %d, particle %d.",k,i);
                        objconst=EvaluateModel(var,0);			//run model
			penalty=0;
			for (j=0;j<ncons;j++)
				{
                                Cswarm[i][j]=objconst(j+nobj);	//NOTE: constraints MUST be <0 for violations and =0 for satisfaction
				penalty+=Cswarm[i][j];
				}
			for (j=0;j<nobj;j++)
                                Jswarm[i][j]=objconst(j)-penalty*huge;	//adding a huge value if the constraint is violated
			
			//6. Update pbest (update pbest strategy is described in the function):
			flagpbest=update_pbest_real_fitness(Jswarm,Jpbest,swarm,pbest,i,nvar,nobj,gridbound_out,gridbound_in,gridbisect_out,gridbisect_in,gridNO_out,gridNO_in,&solgrid_out,&solgrid_in,pbest_fitness_out,pbest_fitness_in);

			//7. Update gbest: compare the new solution with the archive, IF it's not dominated by the former pbest (flagpbest=2)
			if(flagpbest!=2)
				{
				archivesizehist[k]=update_gbestandgrid(archive,Jarchive,Carchive,swarm[i],Jswarm[i],Cswarm[i],archivesizehist[k],&Nocc_out,Nocc_in,archivegrid_out,archivegrid_in,gridID_out,gridID_in,gridNO_out,gridNO_in,gridocc_out,gridocc_in,gridoccref_out,gridoccref_in,gridIDref_out,gridIDref_in,solgrid_out,solgrid_in,nvar,nobj,ncons,fitness_out, fitness_in,newgridbound_out,minB,maxB);
				}

			//8. Prune archive if necessary:
			if (archivesizehist[k]>archivesize)						//IF archivesize has been exceeded
				{
				archivesizehist[k]=archivesize;						//redefine the archive size as the maximum admitted
				for(j=0;j<Nocc_out;j++)								//cycle on the non-empty outer grids j
					{
					j_indexes[j]=j;									//define indexes and values for the indexed qsort
					values[j]=fitness_out[j];						//define fitness_in as the function to be sorted
					}
				qsort(j_indexes,Nocc_out,sizeof(int),compare_index);//sort the non-emty outer grids from the less to the most crowded one

				count_jerase=0;			//initialize counter of solutions with worst outer fitness but at the extremes --> not to be erased
				count_lerase=0;			//initialize counter of solutions with worst inner fitness but at the extremes --> not to be erased
				check_erase=0;			//initialize variable for checking the solution to be erased: if it becomes = 1 --> the solution has been found
				do													//control cycle to avoid the elimination of an extreme solution
					{
					if (count_lerase==0)							//if it's the first inner grid to be checked in the count_jerase-th most crowded outer grid
						{
						j_erase=j_indexes[count_jerase];			//j_erase = position in gridocc_out of the count_jerase-th most crowded outer grid
						for(l=0;l<Nocc_in[gridocc_out[j_erase]];l++)//cycle on the non-empty inner grids l of outer grid j_erase to sort them
							{
							l_indexes[l]=l;							//define indexes and values for the indexed qsort
							values[l]=fitness_in[j_erase][l];		//define fitness_in as the function to be sorted
							}
						//sort the non-emty inner grids of outer grid j_erase from the less to the most crowded one:
						qsort(l_indexes,Nocc_in[gridocc_out[j_erase]],sizeof(int),compare_index);
						}

					l_erase=l_indexes[count_lerase];//l_erase = position in gridocc_in(j_erase,:) of the count_lerase-th most crowded inner grid of j_erase

					//Cycle on the solutions in inner grid l_erase of outer grid j_erase to find a non-extreme solution to be erased:
					for(l=0;l<gridNO_in[gridocc_out[j_erase]][gridocc_in[j_erase][l_erase]];l++)
						{
						sol_erase=gridID_in[gridocc_out[j_erase]][gridocc_in[j_erase][l_erase]][l];		//define the solution to be erased, starting from the first
						//Check if the selected solution to be erased is one of the extremes solutions:
						for(j=0;j<nobj;j++)
							if(minB[j]!=sol_erase && maxB[j]!=sol_erase)		//if it is NOT one of the extremes --> it can be erased
								{
								check_erase=1;									// --> the solution to be erased has been found
								break;											// --> exit control cycle on extremes solutions
								}
						if (check_erase==1)										//IF the solution to be erased has been found
							break;												// --> exit the cycle to find the solution to be erased too
						}
					//IF the solution to be erased has NOT been found in inner grid l_erase of outer grid j_erase && there still are non-empty inner
					//grids to be checked in outer grid j_erase:
					if (check_erase==0 && count_lerase<Nocc_in[gridocc_out[j_erase]]-1)
						count_lerase++;			// --> increase counter of inner grids checked --> check the next most crowded inner grid
					//ELSE IF the solution to be erased has NOT been found in inner grid l_erase of outer grid j_erase && there are no more non-empty
					//inner grids to be checked in outer grid j_erase:
					else if (check_erase==0)
						{
						count_jerase++;			// --> increase counter of outer grids checke --> check the next most crowded outer grid
						count_lerase=0;			// --> reset counter of inner grids --> check the first most crowded inner grid of the next most crowded outer grid
						}

					if (gridNO_in[gridocc_out[j_erase]][gridocc_in[j_erase][l_erase]]==0) 
						{ 
                                                printf("\nError in DG-MOPSO execution, at iteration %d, particle %d ",k,i);
						exit(0);
						}
//					if (check_erase==0) 
//						printf("\n%d,%d: Pruning solution (%lf %lf) substituted",k,i,Jarchive[sol_erase][0],Jarchive[sol_erase][1]);					//PC

					} while(check_erase==0);
				//Erase from the archive and from all the grid structures the solution (j_erase,l_erase):
				if(Jarchive[sol_erase][0]==0)
						{ 
                                                printf("\nError in DG-MOPSO execution, at iteration %d, particle %d ",k,i);
						exit(0);
						}

				erase_sol(archive,Jarchive,Carchive,j_erase,l_erase,sol_erase,archivesize,&Nocc_out,Nocc_in,archivegrid_out,archivegrid_in,gridID_out,gridID_in,gridNO_out,gridNO_in,gridocc_out,gridocc_in,gridoccref_out,gridoccref_in,gridIDref_out,gridIDref_in,fitness_out,fitness_in,nvar,nobj,ncons,minB,maxB);
				}												//CLOSE IF cycle for archive pruning

			}													//CLOSE CYCLE on the particles (i)

		//c.) Check if error on the boundaries has reached a predetermined tolerance --> if in any of the objectives the error is too high --> RESET GRID!
		for(l=0;l<nobj;l++)				//check the difference between the current and the previous boundaries for each dimension
			if(fabs((gridbound_out[l][0]-newgridbound_out[l][0])/(gridbound_out[l][1]-gridbound_out[l][0]))>eps || fabs((gridbound_out[l][1]-newgridbound_out[l][1])/(gridbound_out[l][1]-gridbound_out[l][0]))>eps)
				{						//if the difference is excessive in any of the dimensions --> RESET GRID! (only 1 time --> break)
//				printf("\nReset grid at iteration %d %d: %lf ; %lf;  %lf ; %lf",k,i,gridbound_out[l][0],newgridbound_out[l][0],gridbound_out[l][1],newgridbound_out[l][1]);
				for(j=0;j<nobj;j++)		//update the values of external boundaries
					{
					gridbound_out[j][0]=newgridbound_out[j][0];						//update lower external boundaries
					gridbound_out[j][1]=newgridbound_out[j][1];						//update upper external boundaries
					}
				findinnerbound(gridbound_out,gridbound_in,gridbisect_out,nobj);		//update inner boundaries for each of the outer grid zones

				for(j=0;j<archivesizehist[k];j++)									//update grid positions of archive solutions
					{
					archivegrid_out[j]=findgrid(Jarchive[j],gridbound_out,gridbisect_out,nobj);							//update outer grid
					archivegrid_in[j]=findgrid(Jarchive[j],gridbound_in[archivegrid_out[j]],gridbisect_in,nobj);		//update inner grid
					}
				reset_grid(archivegrid_out,archivegrid_in,ngrids_out,ngrids_in,archivesizehist[k],nobj,gridID_out,gridID_in,gridNO_out,gridNO_in,gridocc_out,gridocc_in,&Nocc_out,Nocc_in,gridoccref_out,gridoccref_in,gridIDref_out,gridIDref_in);

				define_fitness_out(Nocc_out, gridNO_out, gridocc_out, fitness_out);										//update outer fitness
				define_fitness_in(Nocc_out, gridocc_out, Nocc_in, gridocc_in, gridNO_in, fitness_in);					//update inner fitness

				break;																//exit comparison cycle
				}

		//Write the objectives and constraints values of current archive to file:
		for(counter1=0;counter1<archivesizehist[k];counter1++)
			{
			penalty=0;													//recomputing penalty
			for(counter2=0;counter2<ncons;counter2++)
				penalty+=Carchive[counter1][counter2];			

			for(counter2=0;counter2<nobj;counter2++)
				{
				tmpJarchive=Jarchive[counter1][counter2]+penalty*huge;
				fprintf(fpParetoFront,"%lf ",tmpJarchive);
				}
			for(counter2=0;counter2<ncons;counter2++)
				fprintf(fpParetoFront,"%lf ",Carchive[counter1][counter2]);
			fprintf(fpParetoFront,"\n");
			}
		//If current archive size is not as large as the number of solutions required  by the user add zeros:
		for(counter1=archivesizehist[k];counter1<archivesize;counter1++)
			{
			for(counter2=0;counter2<nobj;counter2++)
                fprintf(fpParetoFront,"%lf ", 0.0);
			for(counter2=0;counter2<ncons;counter2++)
                fprintf(fpParetoFront,"%lf ",0.0);
			fprintf(fpParetoFront,"\n");
			}

		k++;		//increment i for the cycle
		printf("\n\n ITERATION %d COMPLETED, archive size: %d \n",k-1,archivesizehist[k-1]);
		}	//close cycle on the iterations (k)

// ----------------------------------------------------------------- POST PROCESS ----------------------------------------------------------------------


	//COPY FINAL RESULTS TO VARIABLE OPTIMAL SOLUTION FOR THE REST OF THE CODE:
	for(counter1=0;counter1<archivesizehist[k-1];counter1++)			//for the non dominated solutions found so far
		{
		for(counter2=0;counter2<nvar;counter2++)
                        OptimalSolutions(counter1,counter2)=archive[counter1][counter2];		//copy archive variables
		for(counter2=0;counter2<nobj;counter2++)
                        ParetoFront(counter1,counter2)=Jarchive[counter1][counter2];			//copy archive objectives
		for(counter2=0;counter2<ncons;counter2++)
                        ParetoFront(counter1,counter2+nobj)=Carchive[counter1][counter2];	//copy archive constraints
		}
	for(counter1=archivesizehist[k-1];counter1<archivesize;counter1++)	//for the remaining positions in the final archive
		{																//set variables, objectives and constraints to huge
		for(counter2=0;counter2<nvar;counter2++)
                        OptimalSolutions(counter1,counter2)=huge;
		for(counter2=0;counter2<nobj+ncons;counter2++)
                        ParetoFront(counter1,counter2)=huge;
		}
	for(counter1=0;counter1<swarmsize;counter1++)
		{
		for(counter2=0;counter2<nvar;counter2++)
			{
                        OldSwarm(counter1,counter2)=swarm[counter1][counter2];		//copy swarm
                        OldPbest(counter1,counter2)=pbest[counter1][counter2];		//copy pbests
                        OldVel(counter1,counter2)=vel[counter1][counter2];			//copy swarm velocities
			}
		for(counter2=0;counter2<nobj;counter2++)
			{
                        OldJswarm(counter1,counter2)=Jswarm[counter1][counter2];		//copy swarm objectives
                        OldJpbest(counter1,counter2)=Jpbest[counter1][counter2];		//copy pbests objectives
			}
		}
	//Write variables values of final archive to file:
	for(counter1=0;counter1<archivesizehist[k-1];counter1++)			//for the non dominated solutions found so far
		{
		for(counter2=0;counter2<nvar;counter2++)
			fprintf(fpSolutions,"%lf ",archive[counter1][counter2]);
		fprintf(fpSolutions,"\n");
		}
	for(counter1=archivesizehist[k-1];counter1<archivesize;counter1++)	//for the remaining positions in the final archive
		{
		for(counter2=0;counter2<nvar;counter2++)
            fprintf(fpSolutions,"%lf ", 0.0);
		fprintf(fpSolutions,"\n");
		}
}
