/*This is a header file to assign the dummy fitness*/
void dumfitness(population *pop_ptr);

void share1(population *pop_ptr,int rnk);
/*This Functon do the sharing of the fitness*/

void sort1(int m1);
/*This is the routine to sort the fitness for each function 
  in the ascending order*/

int rec_arr[maxpop][maxpop],  /*This array holds the record of indiviuals 
				at different ranks*/
  r_arr[maxpop];              /*This array gives the number of individuals
				at different ranks*/

double fpara[maxpop][2];       /*Stores individual no and its fitness
				 in one dimension*/
void dumfitness(population *pop_ptr)
{
  int *ptr[maxpop];
  int i,j,k,m,rnk1;
  
//  FILE *fptr;
//  fptr = fopen("/tmp/Dft.dat","w");
//  fprintf(fptr," S no F1\tF2\tRank\n");
  
//  for(i = 0;i < popsize;i++)
//    fprintf(fptr,"%d  %f %f %d\n",i+1,pop_ptr->ind[i].fitness[0],pop_ptr->ind[i].fitness[1],pop_ptr->ind[i].rank);
  
  m = pop_ptr->maxrank;
  
  for(i = 0;i < m;i++)
    {
      r_arr[i] = pop_ptr->rankno[i];      /*Put the initial individuals at this rank to zero*/
//      fprintf(fptr,"Rank = %d No of Ind = %d\n",i+1,r_arr[i]);
    }
  
  for(i = 0;i < popsize;i++)
    pop_ptr->ind[i].cub_len = 0; /*Initialise the dummy fitness to zero*/
      
  for(i = 0;i < m;i++)
    {
      ptr[i] = &(rec_arr[i][0]);
    }
  
  for(i = 0;i < popsize;i++)
    {
      rnk1 = pop_ptr->ind[i].rank;
      *ptr[rnk1-1]++ = i;
    }
  
//  fprintf(fptr,"The rankarray\n");
  for(i = 0 ;i < m;i++)
    {
//      fprintf(fptr,"Rank = %d\t",i+1);
      k = r_arr[i];
//      for(j = 0;j < k ;j++)
//	fprintf(fptr,"%d ",rec_arr[i][j]);
//      fprintf(fptr,"\n");
    }
  
  for(i = 0;i < m;i++)
    {
      share1(pop_ptr,i+1);
    }
  
//  fclose(fptr);
  return;
}


/*Assigns the dummyfitness to the individuals at different ranks*/
void share1(population *pop_ptr,int rnk)
{
  double length[maxpop][2],max;
  int i,j,m1,a ;
  
  m1 = r_arr[rnk-1];
  
  for(j = 0;j < nfunc;j++)
    {
      for(i = 0;i < m1;i++)
	{
	  fpara[i][0] = 0;
	  fpara[i][1] = 1;
	}
      
      for(i = 0;i < m1;i++)
	{
	  a = rec_arr[rnk-1][i];
	  fpara[i][0] = (double)a;
	  fpara[i][1] = pop_ptr->ind[a].fitness[j];
	}
      
      sort1(m1);  /*Sort the array of fitnesses(in ascending order)*/
      
      max = fpara[m1-1][1];   /*Find the maximum length*/
      
      for(i = 0;i < m1;i++)
	{
	  if(i == 0 || i == (m1-1))
	    { 
	      /*Put the ends fitness more to preserve them*/
	      length[i][0] = fpara[i][0];
	      length[i][1] = 100*max;
	    }
	  else
	    {
	      /*Find the dimension of the cuboid*/
	      length[i][0] = fpara[i][0];
	      length[i][1] = fabs(fpara[i+1][1]- fpara[i-1][1]);
	    }
	}
      
      for(i = 0;i < m1;i++)
	{
	  a = (int)length[i][0];
	  pop_ptr->ind[a].cub_len += length[i][1];
	}
    }
  return;
}
 


/*Sorting the length in ascending order*/
void sort1(int m1)
{
  double temp,temp1; 
  int i1,k1;
  for(k1 = 0;k1 < m1-1;k1++)
    {
      for(i1 = k1+1;i1 < m1;i1++)
	{
	  if(fpara[k1][1] > fpara[i1][1])
	    {
	      temp = fpara[k1][1];
	      temp1 = fpara[k1][0];
	      fpara[k1][1] = fpara[i1][1];
	      fpara[k1][0] = fpara[i1][0];
	      fpara[i1][1] = temp;
	      fpara[i1][0] = temp1;
	    }
	}
    }
  return;
}








