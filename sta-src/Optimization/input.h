#include <stdio.h>
#include <Eigen/Core>

using namespace Eigen;

/*This is a file to get the input for the GA program*/

void input(int ,int ,int ,int , int , Eigen::VectorXd, Eigen::VectorXd);

void input(int NVAR,int NOBJ,int NCONS,int MAXITER,int NsolPareto,Eigen::VectorXd UPPERBOUND,Eigen::VectorXd LOWERBOUND)
{

  int i;
  double cc,dcross,pmut,dmut;

// ************************************************************************************************************************
// ************************************************************************************************************************
	//CHANGE HERE NSGA-II PARAMETERS:
	pcross=0.8;				//crossover probability
	dcross=20;				//crossover distribution
        //pmut=1.0/NVAR;			//mutation probability
        pmut=0.05;			//mutation probability
        dmut=10;				//mutation distribution
	//DO NOT CHANGE ANYTHING BELOW
// ************************************************************************************************************************
// ************************************************************************************************************************

	nvar=NVAR;				//Number of real-coded variables
	nchrom=0;				//Number of binary variables
	nfunc=NOBJ;				//Number of objective functions
	ncons=NCONS;			//Number of constraint functions --> for unconstrained problems
	fflush(stdin);																				//WHAT IS THIS LINE???
	popsize=NsolPareto;		//Population size
	gener=MAXITER;			//Number of generations


	  if (nvar > 0)
		{
		pmut_r=pmut;		//Mutation probability
		di=dcross;			//Crossover distribution
		dim=dmut;			//Mutation distribution

		for(i=0;i<nvar;i++)
			{
                        lim_r[i][0]=LOWERBOUND(i);
                        lim_r[i][1]=UPPERBOUND(i);
			}
		}

  ans=1;			//Rigid boundaries --> if 0 sets no limits for the decision variables

  chrom = 0;
  if (nchrom > 0)
    {
	optype=1;			//Crossover type: 1 for Simple & 2 for Uniform X-over
      for (i = 0; i < nchrom; i++)
		{
		//Number of bits assigned to the i-th variable:
		vlen[i]=1;
		chrom += vlen[i];
		lim_b[i][0]=0;		//lower limit of the binary variable --> minimum decoded value??
		lim_b[i][1]=1;		//uper limit of the binary variable --> maximum decoded value??
		}
    cc = 3.0/chrom;
	pmut_b=cc;			//Mutation probability for binary problems

    }
  return;
}




















